/*
 ****************************************************************
 *								*
 *			sd.1542.c				*
 *								*
 *	Driver SCSI Adaptec 1542				*
 *								*
 *	Versão	3.0.0, de 13.01.95				*
 *		4.5.0, de 17.06.03				*
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

#include "../h/map.h"
#include "../h/frame.h"
#include "../h/intr.h"
#include "../h/disktb.h"
#include "../h/scsi.h"
#include "../h/kfile.h"
#include "../h/inode.h"
#include "../h/devhead.h"
#include "../h/bhead.h"
#include "../h/ioctl.h"
#include "../h/signal.h"
#include "../h/uproc.h"
#include "../h/uerror.h"

#include "../h/cdio.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
#define NSD		8	/* 8 unidades */
#define PL		3	/* Prioridade de interrupção */
#define splsd		spl3	/* Função de prioridade de interrupção */

#define M64KBSZ		(64 * KBSZ)
#define M64KBMASK	(M64KBSZ - 1)
#define M16MBLIMIT	(16 * MBSZ)

#define b_malloc_size	b_cyloff

#define	NUM_TRY		6	/* Número de tentativas */

#define	offsetof(type,member)	((int)&(((type *)0)->member))

#define	SD_MAGIC	0xAA55BB66	/* Para conferir end. de "mail_box" */

/*
 ****** Registros do controlador ********************************
 */
const int	sd_ports[] =	/* Portas possíveis */
{
	0x334, 0x330, 0x230, 0x234, 0x130, 0x134, 0	/* Ordem boa para MIDI */
};

entry int	SDPORT;		/* A porta em uso */
entry int	SDIRQ;		/* O IRQ */

#define SD_CONTROL	(SDPORT + 0)	/* De escrita */
#define SD_DATA_OUT	(SDPORT + 1)

#define SD_STATUS	(SDPORT + 0)	/* De leitura */
#define SD_DATA_IN	(SDPORT + 1)
#define SD_INTR		(SDPORT + 2)

/*
 ****** Indicadores de estado ***********************************
 */
/* control port bits */
#define SD_CONTROL_HRST		0x80 /* hard reset */
#define SD_CONTROL_SRST		0x40 /* soft reset */
#define SD_CONTROL_IRST		0x20 /* interrupt reset */
#define SD_CONTROL_SCRST	0x10 /* scsi bus reset */

/* status port bits */
#define SD_STATUS_STST		0x80 /* self test in progress */
#define SD_STATUS_DIAGF		0x40 /* internal diagnostic failure */
#define SD_STATUS_INIT		0x20 /* mailbox initialization required */
#define SD_STATUS_IDLE		0x10 /* scsi host adapter idle */
#define SD_STATUS_CDF		0x08 /* command/data out port full */
#define SD_STATUS_DF		0x04 /* data in port full */
/* 0x02 reserved */
#define SD_STATUS_INVDCMD	0x01 /* invalid host adapter command */

/* interrupt port bits */
#define SD_INTR_ANY		0x80
#define SD_INTR_SCRD		0x08 /* scsi reset detected */
#define SD_INTR_HACC		0x04 /* host adapter command complete */
#define SD_INTR_MBOA		0x02 /* mailbox out available */
#define SD_INTR_MBIF		0x01 /* mailbox in available */

/*
 ****** Comandos ************************************************
 */
#define SD_CMD_NOP				0x00
#define SD_CMD_MAILBOX_INIT			0x01
#define SD_CMD_START_SCSI_COMMAND		0x02
#define SD_CMD_START_BIOS_COMMAND		0x03
#define SD_CMD_ADAPTER_INQUIRY			0x04
#define SD_CMD_ENABLE_MBOA_INTR			0x05
#define SD_CMD_SET_SELECTION_TIMEOUT		0x06
#define SD_CMD_SET_BUS_ON_TIME			0x07
#define SD_CMD_SET_BUS_OFF_TIME			0x08
#define SD_CMD_SET_TRANSFER_SPEED		0x09
#define SD_CMD_RETURN_INSTALLED_DEVICES		0x0A
#define SD_CMD_RETURN_CONFIGURATION_DATA	0x0B
#define SD_CMD_ENABLE_TARGET_MODE		0x0D
#define SD_CMD_RETURN_SETUP_DATA		0x0D
#define SD_CMD_WRITE_ADAPTER_CHANNEL_2_BUFFER	0x1A
#define SD_CMD_READ_ADAPTER_CHANNEL_2_BUFGFER	0x1B
#define SD_CMD_WRITE_ADAPTER_FIFO_BUFFER	0x1C
#define SD_CMD_READ_ADAPTER_FIFO_BUFFER		0x1D
#define SD_CMD_ECHO				0x1F
#define SD_CMD_ADAPTER_DIAG			0x20
#define SD_CMD_SET_ADAPTER_OPTIONS		0x21

/*
 ****** Bloco de "caixa de correio" *****************************
 */
typedef struct mailbox
{
	char	mail_cmd;
	char	mail_ccb[3];

}	MAILBOX;

entry  MAILBOX	sdmailbox[2*NSD];

/*
 ****** Bloco de comando para o controlador *********************
 */
#define MAXCDB 50
#define MAXSENSE 100

typedef struct ccb
{
	char	ccb_opcode;		/* Código da operação */
	char	ccb_addr_and_control;	/* Unit + code */
	char	ccb_scsi_command_len;	/* Tamanho do comando SCSI */
	char	ccb_sense_len;		/* Tamanho da área de sense */
	char	ccb_data_len[3];	/* Tamanho da "scatter" list */ 
	char	ccb_data_ptr[3];	/* Endereço da "scatter" list */ 
	char	ccb_link[3];		/* Ponteiro para o comando seguinte */
	char	ccb_link_id;		/* ... */
	char	ccb_host_status;	/* ... */
	char	ccb_unit_status;	/* ... */
	char	ccb_zero1;		/* ... */
	char	ccb_zero2;		/* ... */
	char	ccb_cdb[MAXCDB+MAXSENSE]; /* área de comando + sense */

}	CCB;

#define	ccb_sense(ccb) ((ccb)->ccb_cdb + (ccb)->ccb_scsi_command_len)

/*
 ****** Estrutura de informações sobre cada unidade *************
 */
typedef struct sdinfo
{
	SCSI	info_scsi;	/* A estrutura comum SCSI */

	int	info_magic;	/* Para conferir o end. de "mail_box" */

	LOCK	info_busy;	/* Unidade ocupado */
	DEVHEAD	info_utab;	/* Cabeças das listas de pedidos da unidade */

	BHEAD	*info_bp;
	int	info_numtry;

	MAILBOX	*info_mailbox;

	CCB	info_ccb;

}	SDINFO;

entry SDINFO	sdinfo[NSD];

/*
 ****** Variáveis globais ***************************************
 */
entry DEVHEAD	sdtab;		/* Cabeca da lista de dps e do major */

entry BHEAD	rsdbuf;		/* Para as operações "raw" */

entry int	sd_busy;	/* Para controlar o uso */

/*
 ****** Protótipos de funções ***********************************
 */
void		sdstart (SDINFO *);
int		sdcmd (BHEAD *);
int		sdstrategy (BHEAD *);
void		sdint (struct intr_frame);
void		sdint_unit (SDINFO *, int);
void		sd_ltol3 (unsigned long, char *);
int		sd_put_byte (int);
int		sd_get_byte (void);
void		sd_wait_command_complete (void);

/*
 ****************************************************************
 *   Verifica se a unidade existe e prepara a interrupção	*
 ****************************************************************
 */
int
sdattach (int major)
{
	SDINFO		*sp;
	MAILBOX		*mp;
	int		n, i, pl;
	const int	*ip;
	unsigned long	mailbox_phaddr;
	int		irq, dma, id;
	int		board_id, spec_opts, rev_1, rev_2;
	const char	*str;
	extern void	sd_vector ();

	/*
	 *	Procura a unidade em alguma das possíveis portas
	 */
	for (ip = sd_ports; /* abaixo */; ip++)
	{
		if ((SDPORT = *ip) == 0)
			return (-1);

		write_port (SD_CONTROL_SRST, SD_CONTROL);
	
		for (n = 20; n > 0; n --)
		{
			DELAY (1000);
	
			i = read_port (SD_STATUS);
	
			if (i == (SD_STATUS_INIT|SD_STATUS_IDLE))
				goto found;
		}
	}

	/*
	 *	Obtém o modelo do controlador
	 */
    found:
	pl = splsd ();

	if
	(	sd_put_byte (SD_CMD_ADAPTER_INQUIRY) < 0 ||
		(board_id = sd_get_byte ()) < 0 ||
		(spec_opts = sd_get_byte ()) < 0 ||
		(rev_1 = sd_get_byte ()) < 0 ||
		(rev_2 = sd_get_byte ()) < 0
	)
	{
		printf ("sdattach: Não consegui obter o modelo do controlador\n");
		splx (pl);
		return (-1);
	}

	sd_wait_command_complete ();

	switch (board_id)
	{
	    case 0x31: str = "AHA-1540"; break;
	    case 0x41: str = "AHA-1540A/1542A/1542B"; break;
	    case 0x42: str = "AHA-1640A"; break;
	    case 0x43: str = "AHA-1542C"; break;
	    case 0x44: str = "AHA-1542CF"; break;
	    case 0x45: str = "AHA-1542CF, BIOS V2.02"; break;
	    case 0x46: str = "AHA-1542CP, BIOS V1.02"; break;
	    default:   str = "AHA-????"; break;
	}

	/*
	 *	Obtém os valores de IRQ e DMA
	 */
	if
	(	sd_put_byte (SD_CMD_RETURN_CONFIGURATION_DATA) < 0 ||
		(dma = sd_get_byte ()) < 0 ||
		(irq = sd_get_byte ()) < 0 ||
		(id = sd_get_byte ()) < 0
	)
	{
		printf ("sdattach: Não consegui obter os valores de IRQ e DMA\n");
		splx (pl);
		return (-1);
	}

	sd_wait_command_complete ();

	switch (dma)
	{
	    case 0x01: dma =  0; break;
	    case 0x20: dma =  5; break;
	    case 0x40: dma =  6; break;
	    case 0x80: dma =  7; break;
	    default:
		printf ("sdattach: Valor de DMA inválido: %X\n", dma);
		splx (pl);
		return (-1);

	}	/* end switch */

	switch (irq)
	{
	    case 0x01: irq =  9; break;
	    case 0x02: irq = 10; break;
	    case 0x04: irq = 11; break;
	    case 0x08: irq = 12; break;
	    case 0x20: irq = 14; break;
	    case 0x40: irq = 15; break;
	    default: 
		printf ("sdattach: Valor de IRQ inválido: %X\n", irq);
		splx (pl);
		return (-1);

	}	/* end switch */

	/*
	 *	Prepara a interrupção e DMA
	 */
	if (set_dev_irq (irq, PL, 0, sdint) < 0)
		return (-1);

	for (mp = sdmailbox, sp = &sdinfo[0]; sp < &sdinfo[NSD]; mp++, sp++)
	{
		sp->info_magic   = SD_MAGIC;
		sp->info_mailbox = mp;
	}

	dmaexternal (dma);

	sdtab.v_flags |= V_DMA_24;

	/*
	 *	Ajusta o tempo de ligar/desligar o BUS
	 */
	sd_put_byte (SD_CMD_SET_BUS_ON_TIME);  sd_put_byte (7);
	sd_wait_command_complete ();

	sd_put_byte (SD_CMD_SET_BUS_OFF_TIME); sd_put_byte (4);
	sd_wait_command_complete ();

	/*
	 *	Inicializa a "mailbox"
	 */
	mailbox_phaddr = VIRT_TO_PHYS_ADDR (sdmailbox);

	if
	(	sd_put_byte (SD_CMD_MAILBOX_INIT) < 0 ||
		sd_put_byte (NSD) < 0 ||
		sd_put_byte (mailbox_phaddr >> 16) < 0 ||
		sd_put_byte (mailbox_phaddr >> 8) < 0 ||
		sd_put_byte (mailbox_phaddr) < 0
	)
	{
		printf ("sdattach: Não consegui dar o endereço da MAILBOX\n");
		splx (pl);
		return (-1);
	}

	sd_wait_command_complete ();

	splx (pl);

	DELAY (300);

	i = read_port (SD_STATUS);

	if (i & SD_STATUS_INIT)
	{
		printf ("sdattach: Erro de inicialização da MAILBOX\n");
		return (-1);
	}

	printf
	(	"SCSI [%s]: port 0x%X, irq = %d, dma = %d, id = %d\n",
		str, SDPORT, irq, dma, id
	);

	SDIRQ = irq;

	return (0);

}	/* end sdattach */

/*
 ****************************************************************
 *	Função de "open"					*
 ****************************************************************
 */
int
sdopen (dev_t dev, int oflag)
{
	int		target;
	DISKTB		*up;
	SDINFO		*sp;

	/*
	 *	Prólogo
	 */
	if ((up = disktb_get_entry (dev)) == NODISKTB)
		return (-1);

	if ((unsigned)(target = up->p_target) >= NSD)
		{ u.u_error = ENXIO; return (-1); }

	/*
	 *	Verifica o "O_LOCK"
	 */
	if (up->p_lock || (oflag & O_LOCK) && up->p_nopen)
		{ u.u_error = EBUSY; return (-1); }

	sp = &sdinfo[target];

	sp->info_scsi.scsi_cmd = sdcmd;

	if (scsi_open (&sp->info_scsi, dev, oflag) < 0)
		return (-1);

	/*
	 *	Se for o primeiro "open" em um dispositivo de meio
	 *	removível, trava o meio.
	 */
	if (sp->info_scsi.scsi_nopen == 0 && sp->info_scsi.scsi_removable)
		scsi_ctl (&sp->info_scsi, dev, CD_LOCK_UNLOCK, 1);

	/*
	 *	Sucesso
	 */
	sp->info_scsi.scsi_nopen++;
	up->p_nopen++;

	if (oflag & O_LOCK)
		up->p_lock = 1;

	return (0);

}	/* end sdopen */

/*
 ****************************************************************
 *	Função de close						*
 ****************************************************************
 */
int
sdclose (dev_t dev, int flag)
{
	DISKTB		*up;
	int		target;
	SDINFO		*sp;

	/*
	 *	Prólogo
	 */
	up = &disktb[MINOR (dev)]; target = up->p_target;
	sp = &sdinfo[target];

	/*
	 *	Atualiza os contadores
	 */
	if (--up->p_nopen <= 0)
		up->p_lock = 0;

	if (--sp->info_scsi.scsi_nopen <= 0)
	{
		if (sp->info_scsi.scsi_removable)
			scsi_ctl (&sp->info_scsi, dev, CD_LOCK_UNLOCK, 0);

		scsi_close (&sp->info_scsi, dev);
	}

	return (0);

}	/* end sdclose */

/*
 ****************************************************************
 *	Prepara uma operação interna				*
 ****************************************************************
 */
int
sdcmd (BHEAD *bp)
{
	SDINFO		*sp;

	/*
	 *	recebe e devolve o bhead "locked"
	 */
	sp = &sdinfo[disktb[MINOR (bp->b_phdev)].p_target];

	memmove (sp->info_ccb.ccb_cdb, bp->b_cmd_txt, bp->b_cmd_len);
   	sp->info_ccb.ccb_scsi_command_len = bp->b_cmd_len;

#if (0)	/*******************************************************/
	memmove (sp->info_ccb.ccb_cdb, cmd_text, cmd_len);
   	sp->info_ccb.ccb_scsi_command_len = cmd_len;
#endif	/*******************************************************/

	bp->b_flags |= B_STAT;

	sdstrategy (bp);

	EVENTWAIT (&bp->b_done, PBLKIO);

	if (geterror (bp) < 0)
		return (-1);

	return (0);

}	/* end sdcmd */

/*
 ****************************************************************
 *	Executa uma operação de entrada/saida			*
 ****************************************************************
 */
int
sdstrategy (BHEAD *bp)
{
	SDINFO		*sp;
	const DISKTB	*up;
	int		target;
	daddr_t		bn;
	ulong		ph_area;
	pg_t		malloc_area = 0, malloc_size;

	/*
	 *	verifica a validade do pedido
	 */
	up = &disktb[MINOR (bp->b_phdev)]; target = up->p_target;

	if ((unsigned)target >= NSD)
		{ bp->b_error = ENXIO; goto bad; }

	sp = &sdinfo[target];

	if ((bn = bp->b_phblkno) < 0 || bn + BYTOBL (bp->b_base_count) > up->p_size)
	{
		if ((bp->b_flags & B_STAT) == 0)
			{ bp->b_error = ENXIO; goto bad; }
	}

	/*
	 *	Verifica se precisa de uma área auxiliar
	 */
	ph_area = VIRT_TO_PHYS_ADDR (bp->b_base_addr);

	if
	(	bp->b_base_count &&
		(ph_area + bp->b_base_count > M16MBLIMIT || (ph_area & M64KBMASK) + bp->b_base_count > M64KBSZ)
	)
	{
		if ((malloc_area = malloc_dma_24 (&malloc_size)) == NOPG)
		{
			printf ("%g: Não consegui obter memória abaixo de 16 MB\n");

			bp->b_error = EIO; goto bad;
		}

		bp->b_ptr = (void *)PGTOBY (malloc_area);
		bp->b_malloc_size = malloc_size;

		if ((bp->b_flags & (B_READ|B_WRITE)) == B_WRITE)
			memmove (bp->b_ptr, bp->b_base_addr, bp->b_base_count);
	}
	else
	{
		bp->b_ptr = NOVOID;
	}

	/*
	 *	coloca o pedido na fila e inicia a operação
	 */
	bp->b_cylin = (unsigned)(bn + up->p_offset) >> 10;

#if (0)	/*******************************************************/
	splsd (); 
#endif	/*******************************************************/
	splx (irq_pl_vec[SDIRQ]);

	insdsort (&sp->info_utab, bp); sd_busy++;

	if (TAS (&sp->info_busy) >= 0)
		sdstart (sp);

	spl0 ();

	return (0);

	/*
	 *	Houve algum erro
	 */
    bad:
	bp->b_flags |= B_ERROR;
	EVENTDONE (&bp->b_done);
	return (-1);

}	/* end sdstrategy */

/*
 ****************************************************************
 *	Inicia uma operação em uma unidade			*
 ****************************************************************
 */
void
sdstart (SDINFO *sp)
{
	const DISKTB	*up;
	BHEAD		*bp;
	char		*cp;
	int		target;

	/*
	 *	Verifica se há pedidos para esta unidade
	 */
	SPINLOCK (&sp->info_utab.v_lock);

	bp = sp->info_utab.v_first;

	SPINFREE (&sp->info_utab.v_lock);

	if (bp == NOBHEAD)
		{ CLEAR (&sp->info_busy); return; }

	/*
	 *	Inicia a operação
	 */
	sp->info_bp = bp;

	if (sp->info_numtry == 0)
		sp->info_numtry = NUM_TRY;

	up = &disktb[MINOR (bp->b_phdev)]; target = up->p_target;

	/*
	 *	Prepara o CCB
	 */
	sp->info_ccb.ccb_opcode = 3;	/* Sem scatter list */

	sp->info_ccb.ccb_addr_and_control = target << 5;

	if   (bp->b_base_count == 0)
		sp->info_ccb.ccb_addr_and_control |= 0x18;
	elif ((bp->b_flags & (B_READ|B_WRITE)) == B_READ)
		sp->info_ccb.ccb_addr_and_control |= 0x08;
	else
		sp->info_ccb.ccb_addr_and_control |= 0x10;

	sd_ltol3 (bp->b_base_count, sp->info_ccb.ccb_data_len);

	sp->info_ccb.ccb_sense_len = MAXSENSE;

	if   (bp->b_base_count == 0)
		sd_ltol3 (0, sp->info_ccb.ccb_data_ptr);
	elif (bp->b_ptr)
		sd_ltol3 (VIRT_TO_PHYS_ADDR (bp->b_ptr), sp->info_ccb.ccb_data_ptr);
	else
		sd_ltol3 (VIRT_TO_PHYS_ADDR (bp->b_base_addr), sp->info_ccb.ccb_data_ptr);

	sd_ltol3 (0, sp->info_ccb.ccb_link);

	sp->info_ccb.ccb_link_id = 0;
	sp->info_ccb.ccb_host_status = 0;
	sp->info_ccb.ccb_unit_status = 0;
	sp->info_ccb.ccb_zero1 = 0;
	sp->info_ccb.ccb_zero2 = 0;

	/*
	 *	Prepara o comando SCSI de leitura/escrita
	 */
	if ((bp->b_flags & B_STAT) == 0)
	{
	   	sp->info_ccb.ccb_scsi_command_len = 10;
	   	cp = sp->info_ccb.ccb_cdb;

		cp[0] = ((bp->b_flags & (B_READ|B_WRITE)) == B_WRITE)
				? SCSI_CMD_WRITE_BIG
				: SCSI_CMD_READ_BIG;
		cp[1] = 0;
#if (0)	/*******************************************************/
		*(long *)&cp[2] = long_endian_cv (bp->b_phblkno + up->p_offset);
#endif	/*******************************************************/
		*(long *)&cp[2] = long_endian_cv
				((bp->b_phblkno + up->p_offset) >> (sp->info_scsi.scsi_blshift - BLSHIFT));
		cp[6] = 0;
#if (0)	/*******************************************************/
		*(short *)&cp[7] = short_endian_cv (bp->b_base_count / sp->info_scsi.scsi_blsz);
#endif	/*******************************************************/
		*(short *)&cp[7] = short_endian_cv (bp->b_base_count >> sp->info_scsi.scsi_blshift);
		cp[9] = 0;
	}

	/*
	 *	Epílogo
	 */
	sd_ltol3 (VIRT_TO_PHYS_ADDR (&sp->info_ccb), sp->info_mailbox->mail_ccb);
	sp->info_mailbox->mail_cmd = 1;

	sd_put_byte (SD_CMD_START_SCSI_COMMAND);

#ifdef	MSG
	if (CSWT (10))
	{
		printf
		(	"%s: %c %d\n",
			sp->info_scsi.scsi_dev_nm,
			bp->b_flags & B_READ ? 'R' : 'W',
			bp->b_phblkno + up->p_offset
		);
	}
#endif	MSG

}	/* end sdstart */

/*
 ****************************************************************
 *	Interrupção						*
 ****************************************************************
 */
void
sdint (struct intr_frame frame)
{
	SDINFO		*sp;
	MAILBOX		*mp;
	int		status, phaddr, value;

	/*
	 *	Agradece a interrupção
	 */
	status = read_port (SD_INTR);

	write_port (SD_CONTROL_IRST, SD_CONTROL);

	if ((status & SD_INTR_MBIF) == 0)
		return;

	/*
	 *	Faz o "polling" pelas unidades
	 */
	for (mp = &sdmailbox[NSD]; mp < &sdmailbox[2*NSD]; mp++)
	{
		if ((value = mp->mail_cmd) == 0)
			continue;

		phaddr = (mp->mail_ccb[0] << 16) | (mp->mail_ccb[1] << 8) | mp->mail_ccb[2];

		if (phaddr == 0)
			continue;

		sp = (SDINFO *)(PHYS_TO_VIRT_ADDR (phaddr) - offsetof (SDINFO, info_ccb));

		if (sp->info_magic != SD_MAGIC)
		{
			printf ("sdint: Erro no MAGIC da MAILBOX\n");
			mp->mail_cmd = 0;
			continue;
		}

		/*
		 *	Achou uma interrupção com CCB correto
		 */
		sdint_unit (sp, value);

		mp->mail_cmd = 0;

	}	/* malha pelas MAILBOXEs */

}	/* end sdint */

/*
 ****************************************************************
 *	Processa a interrupção de uma unidade			*
 ****************************************************************
 */
void
sdint_unit (SDINFO *sp, int value)
{
	BHEAD		*bp;
	char		*cp;
	int		sense;

	if (value != 1 && value != 4)
	{
		printf ("%s: Mensagem inválida (%d) da MAILBOX\n", sp->info_scsi.scsi_dev_nm, value);
		goto bad;
	}

	if ((value = sp->info_ccb.ccb_host_status) != 0)
	{
		if (value == 17)	/* Unidade inexistente */
		{
			printf ("%s: Unidade inexistente\n", sp->info_scsi.scsi_dev_nm);
			sp->info_scsi.scsi_is_present = -1;
			goto very_bad;
		}

		printf ("%s: Erro do controlador: %d\n", sp->info_scsi.scsi_dev_nm, value);
		goto bad;
	}

	switch (value = sp->info_ccb.ccb_unit_status)
	{
	    case 0:	/* Sucesso */
		goto done;

	    case 2: 	/* Erro tradicional */
		break;

	    case 8:	/* Está ocupado */
		printf ("%s: Unidade ocupada\n", sp->info_scsi.scsi_dev_nm);
		goto bad;

	    default:	/* Erro da unidade */
		printf ("%s: Erro da unidade: %d\n", sp->info_scsi.scsi_dev_nm, value);
		goto bad;

	}	/* end switch */

	/*
	 *	Erro tradicional
	 */
	cp = ccb_sense (&sp->info_ccb);

	if ((cp[0] & 0x7F) != 0x70)	/* Não tem informação adicional */
	{
		printf ("%s: Erro SCSI: 0x%X\n", sp->info_scsi.scsi_dev_nm, cp[0] & 0x7F);
		goto bad;
	}

	if (sp->info_scsi.scsi_is_tape && (cp[2] & 0x0F) == 0)
	{
		if (cp[2] & 0xE0)	/* Tape Mark ? */
			goto done;
	}

	if   ((sense = scsi_sense (&sp->info_scsi, cp[2] & 0x0F)) > 0)
		goto done;
	elif (sense < 0)
		goto very_bad;

	/*
	 *	Teve erro: verifica se tenta novamente
	 */
    bad:
	if (--sp->info_numtry > 0)
		{ sdstart (sp); return; }
    very_bad:
	bp = sp->info_bp;

	bp->b_error = EIO;
	bp->b_flags |= B_ERROR;

	/*
	 *	Terminou a operação
	 */
    done:
	SPINLOCK (&sp->info_utab.v_lock);

	bp = remdsort (&sp->info_utab); sd_busy--;

	SPINFREE (&sp->info_utab.v_lock);

	bp->b_residual = 0;

	/*
	 *	Se for o caso, copia de volta a área
	 */
	if (bp->b_ptr)
	{
		if ((bp->b_flags & (B_READ|B_WRITE)) == B_READ)
			memmove (bp->b_base_addr, bp->b_ptr, bp->b_base_count);

		mrelease (M_CORE, bp->b_malloc_size, BYUPPG (bp->b_ptr));

		bp->b_ptr = NOVOID;
	}

	EVENTDONE (&bp->b_done);

	/*
	 *	Chama "sdstart" para começar outra operação na mesma unidade
	 */
	sp->info_numtry = 0;

	sdstart (sp);

}	/* end sdint_unit */ 

/*
 ****************************************************************
 *	Leitura de modo "raw"					*
 ****************************************************************
 */
int
sdread (IOREQ *iop)
{
	if (iop->io_offset_low & BLMASK || iop->io_count & BLMASK)
		u.u_error = EINVAL;
	else
		physio (iop, sdstrategy, &rsdbuf, B_READ, 1 /* dma */);

	return (UNDEF);

}	/* end sdread */

/*
 ****************************************************************
 *	Escrita de modo "raw"					*
 ****************************************************************
 */
int
sdwrite (IOREQ *iop)
{
	if (iop->io_offset_low & BLMASK || iop->io_count & BLMASK)
		u.u_error = EINVAL;
	else
		physio (iop, sdstrategy, &rsdbuf, B_WRITE, 1 /* dma */);

	return (UNDEF);

}	/* end sdwrite */

/*
 ****************************************************************
 *	Rotina de IOCTL						*
 ****************************************************************
 */
int
sdctl (dev_t dev, int cmd, int arg, int flag)
{
	SDINFO		*sp;

	sp = &sdinfo[disktb[MINOR (dev)].p_target];

	return (scsi_ctl (&sp->info_scsi, dev, cmd, arg));

}	/* end sdctl */

/*
 ****************************************************************
 *	Escreve um byte no controlador				*
 ****************************************************************
 */
int
sd_put_byte (int value)
{
	int		i;

	for (i = 100; i > 0; i--)
	{
		if ((read_port (SD_STATUS) & SD_STATUS_CDF) == 0)
			break;

		DELAY (50);
	}

	if (i == 0)
	{
		printf ("sd_put_byte: Não consegui escrever um byte\n");
		return (-1);
	}

	write_port (value, SD_DATA_OUT);

	return (0);

}	/* sd_put_byte */

/*
 ****************************************************************
 *	Lê um byte do controlador				*
 ****************************************************************
 */
int
sd_get_byte (void)
{
	int		i;

	for (i = 100; i > 0; i--)
	{
		if ((read_port (SD_STATUS) & SD_STATUS_DF) != 0)
			break;

		DELAY (50);
	}

	if (i == 0)
	{
		printf ("sd_get_byte: Não consegui ler um byte\n");
		return (-1);
	}

	return (read_port (SD_DATA_OUT));

}	/* sd_get_byte */

/*
 ****************************************************************
 *	Espera o controlador terminar a operação		*
 ****************************************************************
 */
void
sd_wait_command_complete (void)
{
	int		i;

	for (i = 20000; i > 0; i--)
	{
		if (read_port (SD_INTR) & SD_INTR_HACC)
			break;

		DELAY (50);
	}

	if (i <= 0)
		printf ("sd_wait_command_complete: Não terminou o comando\n");

	write_port (SD_CONTROL_IRST, SD_CONTROL);

}	/* sd_wait_command_complete */

/*
 ****************************************************************
 *	Converte um valor de 4 bytes para 3			*
 ****************************************************************
 */
void
sd_ltol3 (unsigned long value, char *cp)
{
	if (value >> 24)
		printf ("sd_ltol3: Valor acima de 2^24: %P\n", value);

	*cp++ = value >> 16;
	*cp++ = value >> 8;
	*cp++ = value;

}	/* end sd_ltol3 */
