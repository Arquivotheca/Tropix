/*
 ****************************************************************
 *								*
 *			fd.c					*
 *								*
 *	Driver bem simples para o disquette			*
 *								*
 *	Versão	3.0.0, de 26.06.94				*
 *		4.2.0, de 12.10.01				*
 *								*
 *	Módulo: Boot2						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2001 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include <common.h>

#include <disktb.h>
#include <devmajor.h>
#include <bhead.h>

#include "../h/common.h"
#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
#define NFD		2	/* Por enquanto 2 unidades */

/*
 ******	Definições referentes ao controlador 8259 ***************
 */
#define	IO_ICU1		0x20	/* Endereço base para 8259 mestre */
#define	IO_IRQ6		0x40	/* Máscara correspondente à IRQ6 */
#define	IO_READ_IRR	0x0A	/* Habilita leitura do Reg. Intr. Pendentes */
#define	IO_EOI		0x20	/* Fim de Interrupção */

/*
 ****** Portas do controlador do disquette **********************
 */
#define FD_STATUS 	0x3F4
#define FD_DATA		0x3F5
#define FD_DOR 		0x3F2	/* Digital Output Register */
#define FD_DIR 		0x3F7	/* Digital Input Register (read) */
#define FD_DCR 		0x3F7	/* Diskette Control Register (write)*/

/*
 ****** Indicadores do registro de estado ***********************
 */
#define STATUS_BUSYMASK	0x0F	/* drive busy mask */
#define STATUS_BUSY	0x10	/* FDC busy */
#define STATUS_DMA	0x20	/* 0- DMA mode */
#define STATUS_DIR	0x40	/* 0- cpu->fdc */
#define STATUS_READY	0x80	/* Data reg ready */

/*
 ****** Comandos ************************************************
 */
#define FD_RECALIBRATE	0x07	/* move to cylin 0 */
#define FD_SEEK		0x0F	/* seek cylin */
#define FD_READ		0xE6	/* read with MT, MFM, SKip deleted */
#define FD_WRITE	0xC5	/* write with MT, MFM */
#define FD_SENSEI	0x08	/* Sense Interrupt Status */
#define FD_SPECIFY	0x03	/* specify HUT etc */
#define FD_FORMAT	0x4D	/* format one track */
#define FD_VERSION	0x10	/* get version code */
#define FD_CONFIGURE	0x13	/* configure FIFO operation */
#define FD_PERPENDICULAR 0x12	/* perpendicular r/w mode */

/*
 ****** Bits de DOR *********************************************
 */
#define DOR_SEL		0x03	/* Unidade selecionada */
#define DOR_RESET	0x04	/* Reset */
#define DOR_DMA_ENABLE	0x08	/* Habilita o DMA */
#define DOR_MOTOR	0x10	/* Liga o motor (10 a 80) */

/*
 ****** Bits of FD_ST0 ******************************************
 */
#define ST0_DS		0x03	/* drive select mask */
#define ST0_HA		0x04	/* Head (Address) */
#define ST0_NR		0x08	/* Not Ready */
#define ST0_ECE		0x10	/* Equipment chech error */
#define ST0_SE		0x20	/* Seek end */
#define ST0_INTR	0xC0	/* Interrupt code mask */

/*
 ****** Bits of FD_ST1 ******************************************
 */
#define ST1_MAM		0x01	/* Missing Address Mark */
#define ST1_WP		0x02	/* Write Protect */
#define ST1_ND		0x04	/* No Data - unreadable */
#define ST1_OR		0x10	/* OverRun */
#define ST1_CRC		0x20	/* CRC error in data or addr */
#define ST1_EOC		0x80	/* End Of Cylinder */

/*
 ****** Bits of FD_ST2 ******************************************
 */
#define ST2_MAM		0x01	/* Missing Addess Mark (again) */
#define ST2_BC		0x02	/* Bad Cylinder */
#define ST2_SNS		0x04	/* Scan Not Satisfied */
#define ST2_SEH		0x08	/* Scan Equal Hit */
#define ST2_WC		0x10	/* Wrong Cylinder */
#define ST2_CRC		0x20	/* CRC error in data field */
#define ST2_CM		0x40	/* Control Mark = deleted */

/*
 ****** Bits of FD_ST3 ******************************************
 */
#define ST3_HA		0x04	/* Head (Address) */
#define ST3_TZ		0x10	/* Track Zero signal (1=track 0) */
#define ST3_WP		0x40	/* Write Protect */

/*
 ******	Tabela de tipos de disquetes ****************************
 */
typedef struct
{
	char	*nm;
	int	sectrac;	/* sectors per track         */
	int	secsize;	/* size code for sectors     */
	int	datalen;	/* data len when secsize = 0 */
	int	gap;		/* gap len between sectors   */
	int	tracks;		/* total num of tracks       */
	int	size;		/* size of disk in sectors   */
	int	steptrac;	/* steps per cylinder        */
	int	trans;		/* transfer speed code       */
	int	spec;		/* stepping rate, head unload time */

}	FD_TYPE;

#define	NO_FD_TYPE	(FD_TYPE *)NULL

/*
 ******	Disquetes de 3½ *****************************************
 */
const FD_TYPE fd_types_3[] =
{
/*   sectrac  secsize datalen   gap   tracks   size  steptrac  trans    spec */

	 "1.72 MB HD em dispositivo de 3.5",

	21,	2,	0xFF,	0x04,	82,	3444,	1,	0,	0xCF,

	 "1.44 MB HD em dispositivo de 3.5",

	18,	2,	0xFF,	0x1B,	80,	2880,	1,	0,	0xCF,

	 "720 Kb em dispositivo de 3.5",

	9,	2,	0xFF,	0x2A,	80,	1440,	1,	2,	0xDF,

	""			/* Final da tabela */
};

/*
 ******	Disquetes de 5¼ *****************************************
 */
const FD_TYPE fd_types_5[] =
{
/*   sectrac  secsize datalen   gap   tracks   size  steptrac  trans    spec */

/* As entradas de 3.5" são para o caso de obter informação errada da CMOS */

	 "1.72 MB HD em dispositivo de 3.5",

	21,	2,	0xFF,	0x04,	82,	3444,	1,	0,	0xCF,

	 "1.44 MB HD em dispositivo de 3.5",

	18,	2,	0xFF,	0x1B,	80,	2880,	1,	0,	0xCF,


	 "1.2 MB HD em dispositivo de 5.25",

	15,	2,	0xFF,	0x1B,	80,	2400,	1,	0,	0xDF,

	 "360 Kb em dispositivo de 5.25",

	9,	2,	0xFF,	0x23,	40,	720,	2,	1,	0xDF,

#if (0)	/*************************************/
	 "360 Kb floppy in 720 Kb DD drive",

	9,	2,	0xFF,	0x2A,	40,	720,	1,	1,	0xDF,
#endif	/*************************************/

	""			/* Final da tabela */
};

#define	NHEAD 2		/* Todos têm 2 cabeças */

/*
 ****** Estado das várias unidades ******************************
 */
typedef struct
{
	const FD_TYPE	*s_fd_table;	/* Tabela a usar para a busca de tipo */
	const FD_TYPE	*s_fd_type;	/* Tipo do disquete montado */
	int		s_geo_cyl;	/* Cilindro geométrico */
	int		s_need_recalibrate; /* Precisa de ser recalibrado */

}	UNIT_STATUS;

entry UNIT_STATUS	unit_status[NFD];

/*
 *	A variável "DOR_value" contém a informação corrente
 *	se o motor está ligado e a unidade selecionada
 */
entry int		value_DOR = DOR_RESET|DOR_DMA_ENABLE;

/*
 ****** Variáveis globais ***************************************
 */
#undef	DEBUG

entry int	fd_probe;

/*
 ****** Protótipos de funções ***********************************
 */
int		fd_do_seek (int unit, int cylin, int head);
void		fd_wait_not_busy (char *cmd);
void		fd_write_fdc_byte (int byte);
int		fd_get_status (void);
void		fd_setup_dma (int rw, void *area, long count);
int		fd_read_fdc_byte (void);

/*
 ****************************************************************
 *	Obtem os tipos das unidades				*
 ****************************************************************
 */
void
fdattach (void)
{
	int		unit, cab;
	const FD_TYPE	*fd_table;
	char		byte;

	/*
	 *	Obtém os tipos dos disquetes
	 */
	if ((byte = read_cmos (0x10)) == 0)
		return;

	for (cab = 0, unit = 0; unit < NFD; unit++)
	{
		if ((byte & 0xF0) == 0)		/* 0 == unidade inexistente */
			continue;

		if (!cab)
			printf ("Disquetes:");
		else
			printf (",");

		printf (" [fd%d: ", unit);	cab++;

		switch (byte & 0xf0)
		{
		    case 0x10:			/* 360 Kb */
			printf ("5¼, 360 Kb]");
			fd_table = fd_types_5;
			break;

		    case 0x20:			/* 1.2 MB */
			printf ("5¼, 1.2 MB]");
			fd_table = fd_types_5;
			break;

		    case 0x40:			/* 1.44 MB */
			printf ("3½, 1.44 MB]");
			fd_table = fd_types_3;
			break;

		    default:			/* ???? */
			printf ("tipo desconhecido]");
		   	fd_table = NO_FD_TYPE;
			break;
		}

		unit_status[unit].s_fd_table = fd_table;

		byte <<= 4;

	}	/* end for (unidades de disquete) */

	if (cab)
		printf ("\n");

}	/* end fdattach */

/*
 ****************************************************************
 *	Inicializa a operação					*
 ****************************************************************
 */
int
fd_open (int unit)
{
	UNIT_STATUS	*up;
	const FD_TYPE	*ft;
	int		i;
	int		numretry = 3;
	char		buf[BLSZ];
	static char	got_int;
	BHEAD		bhead;

	/*
	 *	Inicialização.
	 */
	if (!got_int)
	{
		/*
		 *	Habilita IRQ6
		 */
		write_port (read_port (IO_ICU1+1) & ~IO_IRQ6, IO_ICU1+1);
		write_port (IO_EOI, IO_ICU1);

		/*
		 *	Reseta o FDC
		 */
		write_port (0, FD_DOR);

		for (i = 0; i < 100000; i++)	/* Reset */
			/* vazio */;

		got_int++;
	}

	/*
	 *	Verifica se a unidade dada é válida
	 */
	up = &unit_status[unit];

	if ((unsigned)unit >= NFD || up->s_fd_table == NO_FD_TYPE)
	{
		printf ("fd_open: Unidade %d inexistente\n", unit);
		return (-1);
	}

	/*
	 *	Verifica se já está aberto
	 */
	if ((ft = up->s_fd_type) != NO_FD_TYPE)
		return (0);

	/*
	 *	Tenta decobrir o tipo de disquete
	 */
#ifndef	DEBUG
#endif	DEBUG
	fd_probe = 1;

    retry:
 	for (up->s_fd_type = up->s_fd_table; /* abaixo */; up->s_fd_type++)
	{
	 	ft = up->s_fd_type;

		if (ft->nm[0] == '\0')
			break;
#ifdef	DEBUG
		printf ("Tentando %s\n", ft->nm);
#endif	DEBUG
		/*
		 *	Liga o motor e seleciona
		 */
		value_DOR = (value_DOR & ~DOR_SEL) | (DOR_MOTOR << unit) | unit;

		write_port (value_DOR & ~(DOR_RESET|DOR_DMA_ENABLE), FD_DOR);

		for (i = 0; i < 100000; i++)
			/* vazio */;

		up->s_need_recalibrate++;

		/*
		 *	Tenta ler o setor
		 */
		bhead.b_disktb	   = &proto_disktb[unit];
		bhead.b_flags	   = B_READ;
		bhead.b_blkno	   = ft->sectrac-1;
		bhead.b_addr	   = buf;
		bhead.b_base_count = BLSZ;

		if (fd_strategy (&bhead) >= 0)
		{
			proto_disktb[unit].p_size = ft->size;

			fd_probe = 0; 	return (0);
		}

	}	/* Tentando vários tipos */

	/*
	 *	Não conseguiu!
	 */
	if (numretry-- > 0)
		goto retry;

	printf ("fdopen: Não consegui descobrir o tipo de disquete\n");

	up->s_fd_type = NO_FD_TYPE;

	fd_probe = 0; 	return (-1);

}	/* end fd_open */

/*
 ****************************************************************
 *	Encerra a operação					*
 ****************************************************************
 */
void
fd_close_all (void)
{
	write_port (0, FD_DOR);

}	/* end fd_close_all */

/*
 ****************************************************************
 *	Realiza uma leitura 					*
 ****************************************************************
 */
int
fd_strategy (BHEAD *bp)
{
	int		count = bp->b_base_count;
	const DISKTB	*pp = bp->b_disktb;
	int		unit = pp->p_target;
	int		rw = (bp->b_flags & (B_READ|B_WRITE));
	daddr_t		bno;
	long		block;
	int		i, cyl, geo_cyl, sector, head;
	int		must_copy = 0;
#define	NUM_RETRY	10
	int		numretry = fd_probe ? 1 : NUM_RETRY;
	const FD_TYPE	*ft;
	UNIT_STATUS	*up;
	int		status[7];
	char		buf[BLSZ];	/* Buffer não atravessando 64 Kb */

	/*
	 *	Verifica a validade dos argumentos
	 */
	if ((unsigned)unit >= NFD)
		{ printf ("fd%d: Unidade inválida\n", unit); return (-1); }

	if (pp->p_size != 0 && (unsigned)bp->b_blkno >= pp->p_size)
		{ printf ("fd%d: Bloco inválido: %d\n", unit, bp->b_blkno); return (-1); }

	bno = bp->b_blkno + pp->p_offset;

	/*
	 *	Verifica se a unidade está aberta
	 */
	if ((value_DOR & (DOR_MOTOR << unit)) == 0)
	{
		printf ("fd: Unidade %d não foi aberta (motor)\n", unit);
		return (-1);
	}

	up = &unit_status[unit];

	if ((ft = up->s_fd_type) == NO_FD_TYPE)
	{
		printf ("fd: Unidade %d não foi aberta (ft)\n", unit);
		return (-1);
	}

	/*
	 *	Calcula as coordenadas
	 */
    next_block:
	sector	= bno % ft->sectrac + 1;
	block	= bno / ft->sectrac;
	head	= block % NHEAD;
	cyl	= block / NHEAD;
	geo_cyl = cyl * ft->steptrac;

#ifdef	DEBUG
	printf
	(	"fd%d: bno = %d, sector = %d, head = %d, cyl = %d, geo_cyl = %d\n", 
		unit, bno, sector, head, cyl, geo_cyl
	);
	printf
	(	"fd%d: addr = %P, count = %d\n", 
		unit, bp->b_addr, count
	);
#endif	DEBUG

	/*
	 *	Verifica se precisa ser selecionada
	 */
	if ((value_DOR & DOR_SEL) != unit || up->s_need_recalibrate)
	{
	    recal:
#ifdef	DEBUG
		printf ("Selecionando unidade %d\n", unit);
#endif	DEBUG
		value_DOR = (value_DOR & ~DOR_SEL) | unit;

		write_port (value_DOR, FD_DOR);

		for (i = 0; i < 500000; i++)
			/* vazio */;

		fd_write_fdc_byte (FD_RECALIBRATE);
		fd_write_fdc_byte (unit);

		fd_wait_not_busy ("recalibrate");

		fd_write_fdc_byte (FD_SPECIFY);
		fd_write_fdc_byte (ft->spec);
		fd_write_fdc_byte (2);

		write_port (ft->trans, FD_DCR);

		up->s_need_recalibrate = 0;
		up->s_geo_cyl = -1;
	}

	/*
	 *	Verifica se precisa de um SEEK
	 */
    retry:
	if (up->s_geo_cyl != geo_cyl)
	{
#ifdef	DEBUG
		printf
		(	"fd%d: SEEK: cyl = %d, geo_cyl = %d\n",
			unit, cyl, geo_cyl
		);
#endif	DEBUG

		fd_write_fdc_byte (FD_SEEK);
		fd_write_fdc_byte (unit);
		fd_write_fdc_byte (geo_cyl);

		fd_wait_not_busy ("seek");

		fd_write_fdc_byte (FD_SENSEI);

		status[0] = fd_read_fdc_byte ();
		status[1] = fd_read_fdc_byte ();

		if ((status[0] & 0xF8) != 0x20 || status[1] != geo_cyl)
		{
			up->s_geo_cyl = status[1];

			if (!fd_probe && numretry != NUM_RETRY)
			{
				printf
				(	"fd_strategy: Erro no SEEK (%2X), "
					"cyl req = %d, at cyl = %d\n",
					status[0], geo_cyl, status[1]
				);
			}

			if (numretry-- > 0)
				goto retry;

			return (-1);
		}

		up->s_geo_cyl = geo_cyl;

	}	/* if (precisa de SEEK) */

	/*
	 *	Verifica se a "area" dada atravessa uma fronteira de 64 Kb
	 *	e prepara os parâmetros da leitura
	 */
	if (((int)bp->b_addr & (64 * KBSZ - 1)) > (64 * KBSZ - BLSZ))
	{
		must_copy = 1;

		if (rw == B_WRITE)
			memmove (buf, bp->b_addr, BLSZ);

		fd_setup_dma (rw, buf,  BLSZ);
	}
	else
	{
		must_copy = 0;

		fd_setup_dma (rw, bp->b_addr,  BLSZ);
	}

#ifdef	DEBUG
	printf
	(	"fd%d: LER: sector = %d, head = %d, cyl = %d\n",
		unit, sector, head, cyl
	);
#endif	DEBUG

	fd_write_fdc_byte (rw == B_WRITE ? FD_WRITE : FD_READ);
	fd_write_fdc_byte (head << 2 | unit);
	fd_write_fdc_byte (cyl);
	fd_write_fdc_byte (head);
	fd_write_fdc_byte (sector);
	fd_write_fdc_byte (ft->secsize);
	fd_write_fdc_byte (ft->sectrac);
	fd_write_fdc_byte (ft->gap);
	fd_write_fdc_byte (ft->datalen);

	fd_wait_not_busy (rw == B_WRITE ? "write" : "read");

	/*
	 *	Examina se houve erros
	 */
	for (i = 0; i < 7; i++)
		status[i] = fd_read_fdc_byte ();

	switch ((status[0] & ST0_INTR) >> 6)
	{
		/*
		 *	error occured during command execution
		 */
	    case 1:
		if (fd_probe)
		{
			if (numretry-- > 0)
				goto retry;

			return (-1);
		}

		if (status[1] & ST1_WP)
		{
			printf ("fd_strategy: Proteção de escrita\n");
		}
		elif (status[1] & ST1_OR)
		{
			printf ("fd_strategy: Over/Underrun - retrying\n");
		}
		elif (status[0] & ST0_ECE)
		{
			printf ("fd_strategy: Recalibrate failed!\n");
		}
		elif (status[2] & ST2_CRC)
		{
			printf ("fd_strategy: data CRC error\n");
		}
		elif (status[1] & ST1_CRC)
		{
			printf ("fd_strategy: CRC error\n");
		}
		elif ((status[1] & (ST1_MAM|ST1_ND)) || (status[2] & ST2_MAM))
		{
			if (numretry != NUM_RETRY)
				printf ("fd_strategy: sector not found\n");

			if (numretry-- > 0)
				goto recal;

			return (-1);
		}
		elif (status[2] & ST2_WC) 	/* seek error */
		{
			printf ("fd_strategy: wrong cylinder\n");
		}
		elif (status[2] & ST2_BC) 	/* cylinder marked as bad */
		{
			printf ("fd_strategy: bad cylinder\n");
		}
		else
		{
			printf
			(	"fd_strategy: unknown error. status[0..3] are: "
				"0x%2X 0x%2X 0x%2X 0x%2X\n",
				status[0], status[1], status[2], status[3]
			);
		}

		if (numretry-- > 0)
			goto retry;

		return (-1);

		/*
		 *	invalid command given
		 */
	    case 2:
		printf ("fd_strategy: Invalid FDC command given!\n");
		return (-1);

		/*
		 *	Abnormal termination caused by polling
		 */
	    case 3:
		printf ("fd_strategy: Abnormal termination caused by polling\n");
		return (-1);

		/*
		 *	Sucesso!
		 */
       /*** case 0: ***/
	    default:
		if (must_copy && rw == B_READ)
			memmove (bp->b_addr, buf, BLSZ);

		bp->b_addr += BLSZ;
		bno	   += 1;
		count	   -= BLSZ;

		if (count > 0)
			goto next_block;

		return (0);

	}	/* end switch */

}	/* end fd_strategy */

/*
 ****************************************************************
 *	Espera terminar a operação				*
 ****************************************************************
 */
void
fd_wait_not_busy (char *cmd)
{
	int		irq6_pend;
	int		count = 1000000;

	do
	{
		if (count-- <= 0)
		{
			printf ("fd_wait: TIMEOUT (%s)\n", cmd);
			break;
		}

		/*
		 *	Habilita a leitura do registro de interrupções
		 *	pendentes do controlador 8259.
		 *	Lê o registro e verifica se a IRQ6 está pendente.
		 */
		write_port (IO_READ_IRR, IO_ICU1);
		irq6_pend = read_port (IO_ICU1) & IO_IRQ6;
	}
	while (!irq6_pend);

	/*
	 *	Fim de interrupção.
	 */
	write_port (IO_EOI, IO_ICU1);

}	/* end fd_wait_not_busy */

/*
 ****************************************************************
 *	Escreve um byte de dados no controlador			*
 ****************************************************************
 */
void
fd_write_fdc_byte (int byte)
{
	int		i;
	char		status;

	/*
	 *	Tenta várias vezes
	 */
	for (i = 10000; i > 0 ; i--)
	{
		status = read_port (FD_STATUS) & (STATUS_READY|STATUS_DIR);

		if (status == STATUS_READY)
		{
			write_port (byte, FD_DATA);
			return;
		}
	}

	printf ("fd_write_fdc_byte: Não consegui enviar um byte\n");

}	/* end fd_write_fdc_byte */

/*
 ****************************************************************
 *	Lê um byte de dados do controlador			*
 ****************************************************************
 */
int
fd_read_fdc_byte (void)
{
	int		i;

	while ((i = read_port (FD_STATUS) & (STATUS_DIR|STATUS_READY)) != (STATUS_DIR|STATUS_READY))
	{
		if (i == STATUS_READY)
			return (-1);
	}

	return (read_port (FD_DATA));

}	/* end fd_read_fdc_byte */

/*
 ****************************************************************
 *	Prepara o DMA						*
 ****************************************************************
 */
void
fd_setup_dma (int rw, void *addr, long nbytes)
{
	/*
	 *	Set read/write bytes
	 */
	if (rw == B_WRITE)
		{ write_port (0x4A, 0x0C); write_port (0x4A, 0x0B); }
	else
		{ write_port (0x46, 0x0C); write_port (0x46, 0x0B); }

	/*
	 *	Send start address
	 */
	write_port ((ulong)addr,       0x04);
	write_port ((ulong)addr >> 8,  0x04);
	write_port ((ulong)addr >> 16, 0x81);

	/*
	 *	Send count
	 */
	nbytes--;

	write_port (nbytes,	   0x05);
	write_port ((nbytes >> 8), 0x05);

	/*
	 *	set channel 2
	 */
	write_port (2, 0x0A);

}	/* end fd_setup_dma */
