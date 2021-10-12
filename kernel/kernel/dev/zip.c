/*
 ****************************************************************
 *								*
 *			zip.c					*
 *								*
 *	Driver para o IOMEGA ZIP de 100 MB			*
 *								*
 *	Versão	3.0.0, de 12.12.96				*
 *		4.5.0, de.17.06.03				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2003 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include "../h/common.h"
#include "../h/sync.h"
#include "../h/scb.h"
#include "../h/region.h"

#include "../h/cpu.h"
#include "../h/frame.h"
#include "../h/intr.h"
#include "../h/disktb.h"
#include "../h/kfile.h"
#include "../h/inode.h"
#include "../h/devhead.h"
#include "../h/bhead.h"
#include "../h/scsi.h"
#include "../h/ioctl.h"
#include "../h/timeout.h"
#include "../h/signal.h"
#include "../h/uproc.h"
#include "../h/uerror.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
#define NZIP		1	/* 1 unidades */
#define IRQ		7	/* No. do IRQ */
#define PL		3	/* Prioridade de interrupção */
#define splzip		spl2	/* Função de prioridade de interrupção */

#define	NUM_TRY		10	/* Número de tentativas */

/*
 ****** Leituras e escritas nas portas **************************
 */
#define zip_outb(zp, port, byte)				\
		write_port (byte, (zp)->zip_port + port);

#define zip_inb(zp, port) (read_port ((zp)->zip_port + port))

/*
 ****** Valores para Sintonizar *********************************
 */
#define ZIP_INTR_TMO	4	/* Duração do timeout em clicks */

#define ZIP_SPEED_LOW	3
#define ZIP_SPEED_HIGH	1
#define ZIP_EPP_SPEED	4096

/*
 ****** Valores que "não devem ser alterados" *******************
 */
#define ZIP_INITIATOR	7
#define ZIP_TARGET	6

#define ZIP_SECTOR_SIZE	512

/*
 ******	Registros da porta paralela *****************************
 */
#define PP_DATA		0	/* Data to/from printer (R/W) */
#define PP_STATUS	1	/* Status of printer (R) */
#define PP_CONTROL	2	/* Control printer (R/W) */

#define EPP_DATA	4	/* EPP data register (8, 16 or 32 bit) */
#define ECP_FIFO	0x400	/* ECP fifo register */
#define ECP_ECR		0x402	/* ECP extended control register */

/*
 ****** Bloco de comando para o controlador *********************
 */
#define MAXCCB	12

typedef struct ccb
{
	char	ccb_cmd[MAXCCB]; 	/* Área do comando SCSI */
	char	ccb_cmd_len;		/* Tamanho do comando SCSI */

}	CCB;

/*
 ****** Estrutura de informações sobre cada unidade *************
 */
typedef struct zipinfo
{
	SCSI	zip_scsi;	/* A estrutura comum SCSI */

	int	zip_port;	/* No. da porta */

	char	zip_mode;	/* Modo */
	char	zip_ecp;	/* ECP */
	char	zip_status;	/* Estado após a operação */
	char	zip_motor_timeout; /* Não zero se o motor estava desligado */

	int	zip_speed_high;	/* DELAY para alta velocidade */
	int	zip_speed_low;	/* DELAY para baixa velocidade */
	int	zip_port_delay;	/* DELAY em uso no momento */

	unsigned zip_count;	/* No. de bytes efetivamente lidos/escritos */
	int	zip_epp_speed;	/* Tempo após escrever o reg. de controle */

	LOCK	zip_busy;	/* Unidade ocupada */
	DEVHEAD	zip_utab;	/* Cabeças das listas de pedidos da unidade */

	BHEAD	*zip_bp;	/* Operação corrente */
	int	zip_numtry;

	CCB	zip_ccb;	/* O comando SCSI */

}	ZIPINFO;

entry ZIPINFO	zipinfo[NZIP];

/*
 ******	Códigos de erros ****************************************
 */
enum
{
	ZIP_SELECT_TIMEOUT = 1,
	ZIP_SCSI_CMD_TIMEOUT,
	ZIP_REPLY_TIMEOUT,
	ZIP_STATUS_TIMEOUT,
	ZIP_DATA_OVERFLOW,
	ZIP_DATA_TIMEOUT,
	ZIP_STATUS_ERROR,
	ZIP_LOW_STATUS_ERROR,
	ZIP_STATUS_WAIT_ERROR,
	ZIP_HIGH_STATUS_ERROR
};

const char zip_error_names[][18] =
{
	"NO_TIMEOUT",
	"SELECT_TIMEOUT",
	"SCSI_CMD_TIMEOUT",
	"REPLY_TIMEOUT",
	"STATUS_TIMEOUT",
	"DATA_OVERFLOW",
	"DATA_TIMEOUT",
	"STATUS_ERROR",
	"LOW_STATUS_ERROR",
	"STATUS_WAIT_ERROR",
	"HIGH_STATUS_ERROR"
};

/*
 ******	Os modos de porta paralela ******************************
 */
enum
{
	PPA_NIBBLE = 1,		/* standard 4 bit mode */
	PPA_PS2,		/* PS/2 byte mode */
	PPA_EPP			/* EPP mode */
};

enum	{ CONNECT_NORMAL, CONNECT_MAYBE_EPP };

/*
 ****** Variáveis globais ***************************************
 */
entry DEVHEAD	ziptab;		/* Cabeca da lista de dps e do major */

entry BHEAD	rzipbuf;	/* Para as operações "raw" */

/*
 ****** Protótipos de funções ***********************************
 */
int		zip_identify_paralel_port (ZIPINFO *);
void		zipstart (ZIPINFO *);
int		zipcmd (BHEAD *);
int		zipstrategy (BHEAD *);
int		zip_init (ZIPINFO *);
void		zip_clock_int (ZIPINFO *);
int 		zip_do_scsi (ZIPINFO *, char *, int, char *, int);
int		zip_request_sense (ZIPINFO *);
void		zip_connect (const ZIPINFO *, int);
int		zip_wait (const ZIPINFO *);
int		zip_select (const ZIPINFO *, int, int);
void		zip_c_pulse (const ZIPINFO *, int);
void		zip_disconnect (const ZIPINFO *);
void		zip_d_pulse (const ZIPINFO *, int);
void		zip_delay_outb (const ZIPINFO *, int, int);

extern void	DELAY (int);

/*
 ****************************************************************
 *	Verifica se a unidade existe				*
 ****************************************************************
 */
int
zipattach (int major)
{
	register ZIPINFO	*zp;
	DISKTB			*up;
	int			unit, port;
	char			*cp;

	/*
	 *	Por enquanto usando "lp0"
	 */
	unit = 0;

	if ((port = scb.y_lp_port[unit]) == 0)
		return (-1);

	zp = &zipinfo[unit];

	zp->zip_scsi.scsi_dev_nm = "zip";		/* Nome provisório */
	zp->zip_scsi.scsi_cmd = zipcmd;

	zp->zip_port = port;
	zp->zip_speed_low  = ZIP_SPEED_LOW;
	zp->zip_speed_high = ZIP_SPEED_HIGH;
	zp->zip_port_delay = ZIP_SPEED_LOW;
	zp->zip_epp_speed  = ZIP_EPP_SPEED;

	if (zip_identify_paralel_port (zp) < 0)
	{
#ifdef	MSG
		if (CSWT (32))
			printf ("zipattach: zip_identify () error\n");
#endif	MSG
		zp->zip_scsi.scsi_is_present = -1;
		return (-1);
	}

	if (zip_init (zp) < 0)
	{
#ifdef	MSG
		if (CSWT (32))
			printf ("zipattach: zip_init () error\n");
#endif	MSG
		zp->zip_scsi.scsi_is_present = -1;
		return (-1);
	}

	/*
	 *	Prepara as entradas da disktb
	 *	Duas entradas: Para o disco todo e uma partição
	 */
	if ((up = disktb_attach_entries ('4', major, unit, 0)) == NODISKTB)
		return (-1);

	zp->zip_scsi.scsi_dev_nm = up->p_name;

	/*
	 *	Indica o dispositivo encontrado
	 */
	switch (zp->zip_mode)
	{
	    case PPA_NIBBLE:
		cp = "NIBBLE";
		break;

	    case PPA_PS2:
		cp = "PS2";
		break;

	    case PPA_EPP:
		cp = "EPP";
		break;

	    default:
		cp = "???";
		break;
	}

	printf
	(	"zip: [%d: 0x%X, %s%s, \"%s\"]\n",
		unit, port, cp, zp->zip_ecp ? "+ECP" : "", zp->zip_scsi.scsi_dev_nm
	);

	return (0);

}	/* end zipattach */

/*
 ****************************************************************
 *	Identifica o tipo de porta paralela			*
 ****************************************************************
 */
int
zip_identify_paralel_port (ZIPINFO *zp)
{
	/*
	 *	Generic parallel port detection
	 *	This will try to discover if the port is
	 *	EPP, ECP, PS/2 or NIBBLE (In that order, approx....)
	 */
	char		save_control, status, data;

	/*
	 *	Teste preliminar para ver se tem uma porta paralela
	 */
	zip_delay_outb (zp, PP_CONTROL, 0x0C);	/* To avoid missing PS2 ports */
	zip_outb (zp, PP_DATA, 0xAA);

	if (zip_inb (zp, PP_DATA) != 0xAA)
		return (-1);

	/*
	 *	Procura EPP
	 */
	status = zip_inb (zp, PP_STATUS);
	zip_outb (zp, PP_STATUS, status);
	zip_outb (zp, PP_STATUS, status & 0xFE);

	if ((zip_inb (zp, PP_STATUS) & 0x01) == 0)
		{ zp->zip_mode = PPA_EPP; return (0); }

	/*
	 *	Now check for ECP
	 */
	zip_outb (zp, ECP_ECR, 0x20);

	if ((zip_inb (zp, ECP_ECR) & 0xE0) == 0x20)
	{
		/* Search for SMC style EPP+ECP mode */

		zip_outb (zp, ECP_ECR, 0x80);

		/* First reset the EPP timeout bit */

		status = zip_inb (zp, PP_STATUS);
		zip_outb (zp, PP_STATUS, status);
		zip_outb (zp, PP_STATUS, status & 0xFE);

		status = zip_inb (zp, PP_STATUS);

		/* If that EPP timeout bit is not reset, DON'T use EPP */

		if ((status & 0x01) == 0)
			{ zp->zip_mode = PPA_EPP; zp->zip_ecp++; return (0); }

		zip_outb (zp, ECP_ECR, 0x20);
		zp->zip_mode = PPA_PS2;
		return (0);
	}

	/*
	 *	Agora, só resta NIBBLE ou PS2
	 */
	save_control = zip_inb (zp, PP_CONTROL);
	zp->zip_mode = PPA_PS2;

	zip_delay_outb (zp, PP_CONTROL, 0xEC);
	zip_outb (zp, PP_DATA, 0x55);

	if ((data = zip_inb (zp, PP_DATA)) != 0xFF)
	{
		zp->zip_mode = PPA_NIBBLE;

		if (data != 0x55)
			return (-1);

		zip_outb (zp, PP_DATA, 0xAA);

		if (zip_inb (zp, PP_DATA) != 0xAA)
			return (-1);
	}

	zip_delay_outb (zp, PP_CONTROL, save_control);
	return (0);

}	/* end zip_identify_paralel_port */

/*
 ****************************************************************
 *	Prepara as portas do dispositivo			*
 ****************************************************************
 */
int
zip_init (register ZIPINFO *zp)
{
	zip_disconnect (zp);
	zip_connect (zp, CONNECT_NORMAL);

	zip_delay_outb (zp, PP_CONTROL, 0x06);

	if ((zip_inb (zp, PP_STATUS) & 0xF0) != 0xF0)
		return (-1);

	zip_delay_outb (zp, PP_CONTROL, 0x04);

	if ((zip_inb (zp, PP_STATUS) & 0xF0) != 0x80)
		return (-1);

	/*
	 *	This is SCSI reset signal
	 */
	zip_outb (zp, PP_DATA, 0x40);
	zip_delay_outb (zp, PP_CONTROL, 0x08);
	DELAY (50);
	zip_delay_outb (zp, PP_CONTROL, 0x0C);
	zip_disconnect (zp);

	return (0);

}	/* end zip_init */

/*
 ****************************************************************
 *	Função de "open"					*
 ****************************************************************
 */
int
zipopen (dev_t dev, int oflag)
{
	register int		unit;
	register DISKTB		*up;
	register ZIPINFO	*zp;

	/*
	 *	Prólogo
	 */
	if ((up = disktb_get_entry (dev)) == NODISKTB)
		return (-1);

	if ((unsigned)(unit = up->p_unit) >= NZIP)
		{ u.u_error = ENXIO; return (-1); }

	/*
	 *	Verifica o "O_LOCK"
	 */
	if (up->p_lock || (oflag & O_LOCK) && up->p_nopen)
		{ u.u_error = EBUSY; return (-1); }

	zp = &zipinfo[unit];

	if (scsi_open (&zp->zip_scsi, dev, oflag) < 0)
		return (-1);

	/*
	 *	Sucesso na abertura
	 */
	zp->zip_scsi.scsi_nopen++;
	up->p_nopen++;

	if (oflag & O_LOCK)
		up->p_lock = 1;

#undef	OPEN_CLOSE
#ifdef	OPEN_CLOSE
	printf
	(	"ZIP open: zp->zip_scsi.scsi_nopen = %d, up[%s]->p_nopen = %d\n",
		zp->zip_scsi.scsi_nopen, up->p_name,  up->p_nopen
	);
#endif	OPEN_CLOSE

	return (0);

}	/* end zipopen */

/*
 ****************************************************************
 *	Função de close						*
 ****************************************************************
 */
int
zipclose (dev_t dev, int flag)
{
	register int		unit;
	register DISKTB		*up;
	register ZIPINFO	*zp;

	/*
	 *	Prólogo
	 */
	up = &disktb[MINOR (dev)]; unit = up->p_unit;
	zp = &zipinfo[unit];

	/*
	 *	Atualiza os contadores
	 */
	if (--up->p_nopen <= 0)
		up->p_lock = 0;

	if (--zp->zip_scsi.scsi_nopen <= 0)
	{
		zp->zip_speed_low  = ZIP_SPEED_LOW;
		zp->zip_speed_high = ZIP_SPEED_HIGH;
		zp->zip_port_delay = ZIP_SPEED_LOW;

		scsi_close (&zp->zip_scsi, dev);
	}

#ifdef	OPEN_CLOSE
	printf
	(	"ZIP close: zp->zip_scsi.scsi_nopen = %d, up[%s]->p_nopen = %d\n",
		zp->zip_scsi.scsi_nopen, up->p_name,  up->p_nopen
	);
#endif	OPEN_CLOSE

	return (0);

}	/* end zipclose */

/*
 ****************************************************************
 *	Executa uma operação interna				*
 ****************************************************************
 */
int
zipcmd (register BHEAD *bp)
{
	register ZIPINFO	*zp;

	/*
	 *	recebe e devolve o bhead "locked"
	 */
	zp = &zipinfo[disktb[MINOR (bp->b_phdev)].p_unit];

	memmove (zp->zip_ccb.ccb_cmd, bp->b_cmd_txt, bp->b_cmd_len);
	zp->zip_ccb.ccb_cmd_len = bp->b_cmd_len;

#if (0)	/*******************************************************/
	memmove (zp->zip_ccb.ccb_cmd, cmd_text, cmd_len);
	zp->zip_ccb.ccb_cmd_len = cmd_len;
#endif	/*******************************************************/

	bp->b_flags |= B_STAT;

	zipstrategy (bp);

	EVENTWAIT (&bp->b_done, PBLKIO);

	if (geterror (bp) < 0)
		return (-1);

	return (0);

}	/* end zipcmd */

/*
 ****************************************************************
 *	Executa uma operação de entrada/saida			*
 ****************************************************************
 */
int
zipstrategy (register BHEAD *bp)
{
	register ZIPINFO	*zp;
	register const DISKTB	*up;
	register int		unit;
	register daddr_t	bn;

	/*
	 *	verifica a validade do pedido
	 */
	up = &disktb[MINOR (bp->b_phdev)]; unit = up->p_unit;

	if ((unsigned)unit >= NZIP || up->p_dev != bp->b_phdev)
		{ bp->b_error = ENXIO; goto bad; }

	zp = &zipinfo[unit];

	if ((bn = bp->b_phblkno) < 0 || bn + BYTOBL (bp->b_base_count) > up->p_size)
		{ bp->b_error = ENXIO; goto bad; }

	/*
	 *	coloca o pedido na fila e inicia a operação
	 */
	bp->b_cylin = (unsigned)(bn + up->p_offset) >> 10;

#if (0)	/*******************************************************/
	splzip (); 
#endif	/*******************************************************/
	splx (irq_pl_vec[IRQ]);

	insdsort (&zp->zip_utab, bp);

	if (TAS (&zp->zip_busy) >= 0)
		zipstart (zp);

	spl0 ();

	return (0);

	/*
	 *	Houve algum erro
	 */
    bad:
	bp->b_flags |= B_ERROR;
	EVENTDONE (&bp->b_done);
	return (-1);

}	/* end zipstrategy */

/*
 ****************************************************************
 *	Inicia uma operação em uma unidade			*
 ****************************************************************
 */
void
zipstart (register ZIPINFO *zp)
{
	register BHEAD		*bp;

	/*
	 *	Verifica se há pedidos para esta unidade
	 */
	SPINLOCK (&zp->zip_utab.v_lock);

	bp = zp->zip_utab.v_first;

	SPINFREE (&zp->zip_utab.v_lock);

	if (bp == NOBHEAD)
		{ CLEAR (&zp->zip_busy); return; }

	/*
	 *	Prepara o "timeout" para iniciar a operação
	 */
	zp->zip_bp = bp;

	if (zp->zip_numtry == 0)
	{
		zp->zip_numtry = NUM_TRY;
		zp->zip_motor_timeout = 0;
	}

	toutset (ZIP_INTR_TMO, zip_clock_int, (int)zp);

}	/* end zipstart */

/*
 ****************************************************************
 *	Pseudo interrupção					*
 ****************************************************************
 */
void
zip_clock_int (register ZIPINFO *zp)
{
	register const DISKTB	*up;
	register BHEAD		*bp;
	register char		*cp;
	int			unit, timeout, sense;
	unsigned		zip_count;

	/*
	 *	Inicia a operação
	 */
	bp = zp->zip_bp;

	up = &disktb[MINOR (bp->b_phdev)]; unit = up->p_unit;

	/*
	 *	Prepara o comando SCSI de leitura/escrita
	 */
	if ((bp->b_flags & B_STAT) == 0)
	{
		zp->zip_ccb.ccb_cmd_len = 10;
		cp = zp->zip_ccb.ccb_cmd;

		cp[0] = ((bp->b_flags & (B_READ|B_WRITE)) == B_WRITE)
				? SCSI_CMD_WRITE_BIG
				: SCSI_CMD_READ_BIG;
		cp[1] = 0;
#if (0)	/*******************************************************/
		*(long *)&cp[2] = long_endian_cv (bp->b_phblkno + up->p_offset);
#endif	/*******************************************************/
		*(long *)&cp[2] = long_endian_cv
				((bp->b_phblkno + up->p_offset) >> (zp->zip_scsi.scsi_blshift - BLSHIFT));
		cp[6] = 0;
#if (0)	/*******************************************************/
		*(short *)&cp[7] = short_endian_cv (bp->b_base_count / zp->zip_scsi.scsi_blsz);
#endif	/*******************************************************/
		*(short *)&cp[7] = short_endian_cv (bp->b_base_count >> zp->zip_scsi.scsi_blshift);
		cp[9] = 0;
	}

	/*
	 *	Vou realizar a operação
	 */
#ifdef	MSG
	if (CSWT (32))
	{
		printf
		(	"%s: %c %d\n",
			zp->zip_scsi.scsi_dev_nm,
			bp->b_flags & B_READ ? 'R' : 'W',
			bp->b_phblkno + up->p_offset
		);
	}
#endif	MSG

	timeout = zip_do_scsi
	(
		zp,
		zp->zip_ccb.ccb_cmd,
		zp->zip_ccb.ccb_cmd_len,
		bp->b_base_addr,	/* Virtual! */
		bp->b_base_count
	);

	zip_count = zp->zip_count;

	/*
	 *	Imprime o resultado
	 */
#ifdef	MSG
	if (CSWT (32))
	{
		printf
		(	"%s: op = %02X, timeout = %s, status = %d, count = %d\n",
			zp->zip_scsi.scsi_dev_nm, zp->zip_ccb.ccb_cmd[0],
			zip_error_names[timeout], zp->zip_status, zip_count
		);
	}
#endif	MSG

	/*
	 *	Primeiro teste: TIMEOUT
	 */
	if (timeout != 0)
	{
		if (timeout == ZIP_SCSI_CMD_TIMEOUT) /* erro ao enviar o comando SCSI */
		{
			if (zp->zip_motor_timeout == 0)
			{
				zp->zip_numtry = 5 * NUM_TRY;
				zp->zip_motor_timeout++;
			}
		}
		else
		{
			printf
			(	"%s: ERRO de I/O (TIMEOUT = %s)\n",
				zp->zip_scsi.scsi_dev_nm, zip_error_names[timeout]
			);
		}
		goto bad;
	}

	/*
	 *	Segundo teste: STATUS
	 */
	if (zp->zip_status == 2)
	{
		sense = zip_request_sense (zp);

		/* NÃO conserva "zp->zip_count" nem "zp->zip_status" */

		if   ((sense = scsi_sense (&zp->zip_scsi, sense)) < 0)
			goto very_bad;
		elif (sense == 0)
			goto bad;
	}
	elif (zp->zip_status != 0)
	{
		printf
		(	"%s: ERRO de I/O (STATUS = %d)\n",
			zp->zip_scsi.scsi_dev_nm, zp->zip_status
		);
		goto bad;
	}

	/*
	 *	Terceiro teste: COUNT
	 */
	if ((bp->b_flags & B_STAT) != 0 || zip_count == bp->b_base_count)
		goto done;

	printf
	(	"%s: COUNT inválido: (%d < %d)\n",
		zp->zip_scsi.scsi_dev_nm, zip_count, bp->b_base_count
	);

	/*
	 *	Teve erro
	 */
    bad:
	if (--zp->zip_numtry > 0)
		goto start_op;

    very_bad:
	bp = zp->zip_bp;

	bp->b_error = EIO;
	bp->b_flags |= B_ERROR;

	/*
	 *	Terminou a operação
	 */
    done:
	SPINLOCK (&zp->zip_utab.v_lock);

	bp = remdsort (&zp->zip_utab);

	SPINFREE (&zp->zip_utab.v_lock);

	bp->b_residual = 0;

	EVENTDONE (&bp->b_done);

	/*
	 *	Chama "zipstart" para começar outra operação na mesma unidade
	 */
	zp->zip_numtry = 0;

    start_op:
#if (0)	/*******************************************************/
	splzip (); 
#endif	/*******************************************************/
	splx (irq_pl_vec[IRQ]);

	zipstart (zp);

	spl0 ();

}	/* end zip_clock_int */

/*
 ****************************************************************
 *	Obtém o SENSE						*
 ****************************************************************
 */
int
zip_request_sense (register ZIPINFO *zp)
{
	int			timeout;
	char			sense[32];
	char			cmd[6];

	cmd[0] = 0x03;	/* Request Sense */
	cmd[1] = 0;
	cmd[2] = 0;
	cmd[3] = 0;
	cmd[4] = sizeof (sense);	/* Comprimento da área */
	cmd[5] = 0;

	timeout = zip_do_scsi
	(
		zp,
		cmd,
		sizeof (cmd),
		sense,
		sizeof (sense)
	);

	if (timeout)
		return (-1);

#ifdef	MSG
	if (CSWT (32))
	{
		printf
		(	"%s: SENSE = %02X, %02X, %02X, %02X\n",
			zp->zip_scsi.scsi_dev_nm, sense[0], sense[1], sense[2], sense[3]
		);
	}
#endif	MSG

	return (sense[2] & 0x0F);

}	/* end zip_request_sense */

/*
 ****************************************************************
 *	Leitura de modo "raw"					*
 ****************************************************************
 */
int
zipread (IOREQ *iop)
{
	if (iop->io_offset_low & BLMASK || iop->io_count & BLMASK)
		u.u_error = EINVAL;
	else
		physio (iop, zipstrategy, &rzipbuf, B_READ, 0 /* sem dma */);

	return (UNDEF);

}	/* end zipread */

/*
 ****************************************************************
 *	Escrita de modo "raw"					*
 ****************************************************************
 */
int
zipwrite (IOREQ *iop)
{
	if (iop->io_offset_low & BLMASK || iop->io_count & BLMASK)
		u.u_error = EINVAL;
	else
		physio (iop, zipstrategy, &rzipbuf, B_WRITE, 0 /* sem dma */);

	return (UNDEF);

}	/* end zipwrite */

/*
 ****************************************************************
 *	Rotina de IOCTL						*
 ****************************************************************
 */
int
zipctl (dev_t dev, int cmd, int arg, int flag)
{
	register ZIPINFO	*zp;

	zp = &zipinfo[disktb[MINOR (dev)].p_unit];

	return (scsi_ctl (&zp->zip_scsi, dev, cmd, arg));

}	/* end zipctl */

/*
 ****************************************************************
 *	Connecta o dispositivo					*
 ****************************************************************
 */
void
zip_connect (register const ZIPINFO *zp, int how)
{
	zip_c_pulse (zp, 0x00);
	zip_c_pulse (zp, 0x3C);
	zip_c_pulse (zp, 0x20);

	if (how == CONNECT_MAYBE_EPP && zp->zip_mode == PPA_EPP)
		zip_c_pulse (zp, 0xCF);
	else
		zip_c_pulse (zp, 0x8F);

}	/* end zip_connect */

/*
 ****************************************************************
 *	Envia o pulso estilo "C"				*
 ****************************************************************
 */
void
zip_c_pulse (register const ZIPINFO *zp, int data)
{
	zip_outb (zp, PP_DATA, data);
	zip_delay_outb (zp, PP_CONTROL, 0x04);
	zip_delay_outb (zp, PP_CONTROL, 0x06);
	zip_delay_outb (zp, PP_CONTROL, 0x04);
	zip_delay_outb (zp, PP_CONTROL, 0x0C);

}	/* end zip_c_pulse */

/*
 ****************************************************************
 *	Desconnecta o dispositivo				*
 ****************************************************************
 */
void
zip_disconnect (register const ZIPINFO *zp)
{
	zip_d_pulse (zp, 0x00);
	zip_d_pulse (zp, 0x3C);
	zip_d_pulse (zp, 0x20);
	zip_d_pulse (zp, 0x0F);

}	/* end zip_disconnect */

/*
 ****************************************************************
 *	Envia o pulso estilo "D"				*
 ****************************************************************
 */
void
zip_d_pulse (register const ZIPINFO *zp, int data)
{
	zip_outb (zp, PP_DATA, data);
	zip_delay_outb (zp, PP_CONTROL, 0x0C);
	zip_delay_outb (zp, PP_CONTROL, 0x0E);
	zip_delay_outb (zp, PP_CONTROL, 0x0C);
	zip_delay_outb (zp, PP_CONTROL, 0x04);
	zip_delay_outb (zp, PP_CONTROL, 0x0C);

}	/* end zip_d_pulse */

/*
 ****************************************************************
 *	Seleciona o TARGET					*
 ****************************************************************
 */
int
zip_select (register const ZIPINFO *zp, int initiator, int target)
{
	char		status;
	int		k;

	status = zip_inb (zp, PP_STATUS);

	zip_outb (zp, PP_DATA, (1 << target));
	zip_delay_outb (zp, PP_CONTROL, 0x0E);
	zip_delay_outb (zp, PP_CONTROL, 0x0C);
	zip_outb (zp, PP_DATA, (1 << initiator));
	zip_delay_outb (zp, PP_CONTROL, 0x08);

	/*
	 *	Espera até 5 segundos
	 */
	for (k = 5000000; k > 0; k--)
	{
		if (((status = zip_inb (zp, PP_STATUS)) & 0xF0) != 0)
			return (status);

		DELAY (1);
	}

	return (0);

}	/* end zip_select */

/*
 ****************************************************************
 *	Espera o ACK do dispositivo				*
 ****************************************************************
 */
int
zip_wait (register const ZIPINFO *zp)
{
	int		k;
	char		status;

	/*
	 *	Return some status information.
	 *
	 *	Semantics:	0xC0 = ZIP wants more data
	 *			0xD0 = ZIP wants to send more data
	 *			0xE0 = ???
	 *			0xF0 = end of transfer, ZIP is sending status
	 */

	/*
	 *	Espera até 5 segundos
	 */
	for (k = 5000000; k > 0; k--)
	{
		if (((status = zip_inb (zp, PP_STATUS)) & 0x80) != 0)
			return (status & 0xF0);

		DELAY (1);
	}

	return (0);

}	/* end zip_wait */

/*
 ****************************************************************
 *	Sincroniza o final de ECP				*
 ****************************************************************
 */
void
zip_ecp_sync (register const ZIPINFO *zp)
{
	int		i;

	if ((zip_inb (zp, ECP_ECR) & 0xE0) != 0x80)
		return;

	for (i = 0; i < 100; i++)
	{
		if (zip_inb (zp, ECP_ECR) & 0x01)
			return;

		DELAY (100);
	}

	printf ("zip: ECP sync failed as data still present in FIFO\n");

} 	/* end zip_ecp_sync */

/*
 ****************************************************************
 *	Verifica se o byte foi transferido corretamente		*
 ****************************************************************
 */
int
zip_check_epp_status (register ZIPINFO *zp)
{
	char		status;

	/*
	 *	EPP timeout, according to the PC87332 manual
	 *	Semantics of clearing EPP timeout bit.
	 *	PC87332	- reading SPP_STR does it...
	 *	SMC		- write 1 to EPP timeout bit
	 *	Others	- (???) write 0 to EPP timeout bit
	 */
	if ((status = zip_inb (zp, PP_STATUS)) & 0x01)
	{
#ifdef	MSG
		if (CSWT (32))
			printf ("zip: EPP timeout\n");
#endif	MSG
		zip_outb (zp, PP_STATUS, status);
		zip_outb (zp, PP_STATUS, status & 0xFE);

		return (ZIP_DATA_TIMEOUT);
	}

	if ((status & 0x80) == 0)
		return (ZIP_STATUS_ERROR);

	return (0);

}	/* end zip_check_epp_status */

/*
 ****************************************************************
 *	Escreve um byte EPP					*
 ****************************************************************
 */
int
zip_force_epp_byte (register ZIPINFO *zp, int byte)
{
	char		status;

	zip_outb (zp, EPP_DATA, byte);

	if (((status = zip_inb (zp, PP_STATUS)) & 0x01) == 0)
		return (0);

	/*
	 *	EPP timeout, according to the PC87332 manual
	 *	Semantics of clearing EPP timeout bit.
	 *	PC87332	- reading SPP_STR does it...
	 *	SMC		- write 1 to EPP timeout bit
	 *	Others	- (???) write 0 to EPP timeout bit
	 */
	if (zp->zip_epp_speed)
	{
		zip_outb (zp, PP_STATUS, status);
		zip_outb (zp, PP_STATUS, status & 0xFE);
		DELAY (zp->zip_epp_speed);

		zip_outb (zp, EPP_DATA, byte);
		status = zip_inb (zp, PP_STATUS);
	}	

	if (status & 0x01)
	{
		zip_outb (zp, PP_STATUS, status);
		zip_outb (zp, PP_STATUS, status & 0xFE);
#ifdef	MSG
		if (CSWT (32))
			printf ("zip: EPP timeout while forcing\n");
#endif	MSG
		return (1);
	}

	return (0);

}	/* end zip_force_epp_byte */

/*
 ****************************************************************
 *	Escrita de uma área					*
 ****************************************************************
 */
int
zip_outstr (register ZIPINFO *zp, register const char *area)
{
	register int		k;
	register char		status;

	switch (zp->zip_mode)
	{
	    case PPA_NIBBLE:
	    case PPA_PS2:
		for (k = ZIP_SECTOR_SIZE; k > 0; k--)
		{
			zip_outb (zp, PP_DATA, *area++);
			zip_delay_outb (zp, PP_CONTROL, 0x0E);
			zip_delay_outb (zp, PP_CONTROL, 0x0C);
		}

		return (0);

	    case PPA_EPP:
		zip_delay_outb  (zp, PP_CONTROL, 0x04);

		for (k = ZIP_SECTOR_SIZE; k > 0; k--)
		{
			zip_outb (zp, EPP_DATA, *area++);

			if (status = zip_check_epp_status (zp))
				return (status);
		}

		zip_delay_outb (zp, PP_CONTROL, 0x0C);
		zip_ecp_sync (zp);
		return (0);

	    default:
		printf
		(	"zip_outstr: Unknown transfer mode (%d)!\n",
			zp->zip_mode
		);
		return (1);

	}	/* end switch */

}	/* end zip_outstr */

/*
 ****************************************************************
 *	Leitura de uma área					*
 ****************************************************************
 */
int
zip_instr (register ZIPINFO *zp, register char *area)
{
	register int		k;
	register char		h, l;
	int			status;
	
	switch (zp->zip_mode)
	{
	    case PPA_NIBBLE:
		for (k = ZIP_SECTOR_SIZE; k > 0; k--)
		{
			zip_delay_outb (zp, PP_CONTROL, 0x04);
			h = zip_inb (zp, PP_STATUS);
			zip_delay_outb (zp, PP_CONTROL, 0x06);
			l = zip_inb (zp, PP_STATUS);
			*area++ = ((l >> 4) & 0x0F) + (h & 0xF0);
		}

		zip_delay_outb (zp, PP_CONTROL, 0x0C);
		return (0);

	    case PPA_PS2:
		for (k = ZIP_SECTOR_SIZE; k > 0; k--)
		{
			zip_delay_outb (zp, PP_CONTROL, 0x25);
			*area++ = zip_inb (zp, PP_DATA);
			zip_delay_outb (zp, PP_CONTROL, 0x27);
		}

		zip_delay_outb (zp, PP_CONTROL, 0x05);
		zip_delay_outb (zp, PP_CONTROL, 0x04);
		zip_delay_outb (zp, PP_CONTROL ,0x0C);
		return (0);

	    case PPA_EPP:
		zip_delay_outb (zp, PP_CONTROL, 0x24);

		for (k = ZIP_SECTOR_SIZE; k > 0; k--)
		{
			*area++ = zip_inb (zp, EPP_DATA);

			if (status = zip_check_epp_status (zp))
				return (status);
		}

		zip_delay_outb (zp, PP_CONTROL, 0x2C);
		zip_ecp_sync (zp);
		return (0);

	    default:
		printf
		(	"zip_instr: Unknown transfer mode (%d)!\n",
			zp->zip_mode
		);
		return (1);

	}	/* end switch */

}	/* end zip_instr */

/*
 ****************************************************************
 *	Escrita de um byte					*
 ****************************************************************
 */
int
zip_outbyte (ZIPINFO *zp, int byte)
{
	register int		status;

	switch (zp->zip_mode)
	{
	    case PPA_NIBBLE:
	    case PPA_PS2:
		zip_outb (zp, PP_DATA, byte);
		zip_delay_outb (zp, PP_CONTROL, 0x0E);
		zip_delay_outb (zp, PP_CONTROL, 0x0C);
		return (0);

	    case PPA_EPP:
		zip_delay_outb (zp, PP_CONTROL, 0x04);
		zip_outb (zp, EPP_DATA, byte);
		status = zip_check_epp_status (zp);
		zip_delay_outb (zp, PP_CONTROL, 0x0C);
		zip_ecp_sync (zp);
		return (status);

	    default:
		printf
		(	"zip_outbyte: Unknown transfer mode (%d)!\n",
			zp->zip_mode
		);
		return (1);

	}	/* end switch */

}	/* end zip_outbyte */

/*
 ****************************************************************
 *	Leitura de um byte					*
 ****************************************************************
 */
int
zip_inbyte (ZIPINFO *zp, char *area)
{
	register int		status;
	register char		h, l;
	
	switch (zp->zip_mode)
	{
	    case PPA_NIBBLE:
		zip_delay_outb (zp, PP_CONTROL, 0x04);
		h = zip_inb (zp, PP_STATUS);
		zip_delay_outb (zp, PP_CONTROL, 0x06);
		l = zip_inb (zp, PP_STATUS);
		*area = ((l >> 4) & 0x0F) + (h & 0xF0);
		zip_delay_outb (zp, PP_CONTROL, 0x0C);
		return (0);

	    case PPA_PS2:
		zip_delay_outb (zp, PP_CONTROL, 0x25);
		*area = zip_inb (zp, PP_DATA);
		zip_delay_outb (zp, PP_CONTROL, 0x27);
		zip_delay_outb (zp, PP_CONTROL, 0x05);
		zip_delay_outb (zp, PP_CONTROL, 0x04);
		zip_delay_outb (zp, PP_CONTROL, 0x0C);
		return (0);

	    case PPA_EPP:
		zip_delay_outb (zp, PP_CONTROL, 0x04);
		*area = zip_inb (zp, EPP_DATA);
		status = zip_check_epp_status (zp);
		zip_delay_outb (zp, PP_CONTROL, 0x0C);
		zip_ecp_sync (zp);
		return (status);

	    default:
		printf
		(	"zip_inbyte: Unknown transfer mode (%d)!\n",
			zp->zip_mode
		);
		return (1);

	}	/* end switch */

}	/* end zip_inbyte */

/*
 ****************************************************************
 *	Executa um comando SCSI					*
 ****************************************************************
 */
int 
zip_do_scsi (ZIPINFO *zp, char *cmd_area, int cmd_count, char *area, int count)
{

	register char		r;
	register int		k, cnt;
	char			l, h;
	int			write_op, bulk, fast;
	int			error = 0;

	zp->zip_count = cnt = 0;

	zp->zip_port_delay = zp->zip_speed_low;

	zip_connect (zp, CONNECT_MAYBE_EPP);

	if ((r = zip_select (zp, ZIP_INITIATOR, ZIP_TARGET)) == 0)
		{ error = ZIP_SELECT_TIMEOUT; goto error; }

	/*
	 *	Envia o comando SCSI
	 */
	if (zp->zip_mode == PPA_EPP)
	{
		zip_delay_outb (zp, PP_CONTROL, 0x04);

		for (k = 0; k < cmd_count; k++)
		{
			if (zip_force_epp_byte (zp, cmd_area[k]))
				{ error = ZIP_SCSI_CMD_TIMEOUT; goto error; }
		}

		zip_delay_outb (zp, PP_CONTROL, 0x0C);
		zip_ecp_sync (zp);
	}
	else
	{
		zip_delay_outb (zp, PP_CONTROL, 0x0C);

		for (k = 0; k < cmd_count; k++)
		{
			if (zip_wait (zp) == 0)
				{ error = ZIP_SCSI_CMD_TIMEOUT; goto error; }

			zip_outb (zp, PP_DATA, cmd_area[k]);
			zip_delay_outb (zp, PP_CONTROL, 0x0E);
			zip_delay_outb (zp, PP_CONTROL, 0x0C);
		}
	}

	/* 
	 *	Transfere os dados
	 */
	bulk = (cmd_area[0] == SCSI_CMD_READ_BIG || cmd_area[0] == SCSI_CMD_WRITE_BIG);

	if ((r = zip_wait (zp)) == 0)
		{ error = ZIP_REPLY_TIMEOUT; goto error; }

	write_op = (r == 0xC0);

	if (zp->zip_mode != PPA_EPP)
		zp->zip_port_delay = zp->zip_speed_high;

	while (r != 0xF0)
	{
		if (cnt >= count)
			{ error = ZIP_DATA_OVERFLOW; goto error; }

		fast = (bulk && ((count - cnt) >= ZIP_SECTOR_SIZE) &&
			(((int)(area + cnt)) & 0x03) == 0);

		if (fast)
		{
			if (write_op)
				error = zip_outstr (zp, &area[cnt]);
			else
				error = zip_instr (zp, &area[cnt]);

			cnt += ZIP_SECTOR_SIZE;
		}
		else
		{
			if (write_op)
				error = zip_outbyte (zp, area[cnt]);
			else
				error = zip_inbyte (zp, &area[cnt]);
			cnt++;
		}

		if (error)
			goto error;

		if ((r = zip_wait (zp)) == 0)
			{ error = ZIP_STATUS_TIMEOUT; goto error; }
	}

	zp->zip_port_delay = zp->zip_speed_low;
	zp->zip_count = cnt;

	if (zip_inbyte (zp, &l))
		{ error = ZIP_LOW_STATUS_ERROR; goto error; }

	if (zip_wait (zp) == 0)
		{ error = ZIP_STATUS_WAIT_ERROR; goto error; }

	if (zip_inbyte (zp, &h))
		{ error = ZIP_HIGH_STATUS_ERROR; goto error; }

	zip_disconnect (zp);

	zp->zip_status = (h << 8) | (l & 0xFF);

	return (0);

	/*
	 *	Em caso de erro, ...
	 */
    error:
	zp->zip_port_delay = zp->zip_speed_low;
	zip_disconnect (zp);
	return (error);

}	/* end zip_do_scsi */

/*
 ****************************************************************
 *	Escrita com atraso					*
 ****************************************************************
 */
void
zip_delay_outb (const ZIPINFO *zp, int port, int byte)
{
	write_port (byte, zp->zip_port + port);
	DELAY (zp->zip_port_delay);

}	/* end zip_delay_outb */
