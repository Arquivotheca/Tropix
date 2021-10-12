/*
 ****************************************************************
 *								*
 *			sd.1542.c				*
 *								*
 *	Driver SCSI Adaptec 1542 (por estado)			*
 *								*
 *	Versão	3.0.0, de 13.01.95				*
 *		4.6.0, de 14.04.04				*
 *								*
 *	Módulo: boot2						*
 *		NÚCLEO do sistema para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2004 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include <common.h>
#include <bcb.h>
#include <disktb.h>
#include <devmajor.h>
#include <bhead.h>
#include <scsi.h>

#include "../h/common.h"
#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
#define NSD		8	/* 8 unidades  no 1542 */

#define	NUM_TRY		5	/* Número de tentativas */

#define	PHYSADDR(n)	((unsigned)n)

/*
 ****** Registros do controlador ********************************
 */
const int	sd_ports[] =	/* Portas possíveis */
{
	0x334, 0x330, 0x230, 0x234, 0x130, 0x134, 0	/* Ordem para MIDI */
};

entry int	SDPORT;		/* A porta em uso */

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
#define SD_CMD_READ_ADAPTER_CHANNEL_2_BUFFER	0x1B
#define SD_CMD_WRITE_ADAPTER_FIFO_BUFFER	0x1C
#define SD_CMD_READ_ADAPTER_FIFO_BUFFER		0x1D
#define SD_CMD_ECHO				0x1F
#define SD_CMD_ADAPTER_DIAG			0x20
#define SD_CMD_SET_ADAPTER_OPTIONS		0x21
#define SD_CMD_GET_EXT_BIOS_INFO		0x28
#define SD_CMD_ENABLE_MAIL_BOX			0x29

/*
 ****** Bloco de "caixa de correio" *****************************
 */
typedef struct mailbox
{
	char	mail_cmd;
	char	mail_ccb[3];

}	MAILBOX;

entry  MAILBOX	sdmailbox[2];	/* Uma de saída, uma de entrada */

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

entry	CCB	sdccb;

/*
 ****** Estrutura de informações sobre cada unidade *************
 */
entry SCSI		sd_scsi[NSD];

/*
 ****** Protótipos de funções ***********************************
 */
void		sdstart (SCSI *);
int		sdinquiry (SCSI *);
int		sdcmd (BHEAD *);
void		sdint_unit (SCSI *, int);
void		sd_ltol3 (unsigned long, char *);
char		*sd_str_conv (char *, const char *, int);
int		sd_put_byte (int);
int		sd_get_byte (void);
void		sd_wait_command_complete (void);

/*
 ****************************************************************
 *   Verifica se a unidade existe e prepara a interrupção	*
 ****************************************************************
 */
int
sdattach (void)
{
	SCSI		*sp;
	int		i;
	const int	*ip;
	unsigned long	mailbox_phaddr;
	int		irq, dma, id, target;
	int		board_id, spec_opts, rev_1, rev_2;
	const char	*str;
	static DISKTB	d = { "sd?", 0, 0, 0, 0, 0, BLSHIFT, MAKEDEV (SD_MAJOR, 0), 0, 0 };

	/*
	 *	Procura a unidade em alguma das possíveis portas
	 */
	for (ip = sd_ports; /* abaixo */; ip++)
	{
		if ((SDPORT = ip[0]) == 0)
			return (-1);

		write_port (SD_CONTROL_SRST, SD_CONTROL);
	
		DELAY (1000);
	
		i = read_port (SD_STATUS);
	
		if (i == (SD_STATUS_INIT|SD_STATUS_IDLE))
			break;
	}

	/*
	 *	Obtém o modelo do controlador
	 */
	if
	(	sd_put_byte (SD_CMD_ADAPTER_INQUIRY) < 0 ||
		(board_id = sd_get_byte ()) < 0 ||
		(spec_opts = sd_get_byte ()) < 0 ||
		(rev_1 = sd_get_byte ()) < 0 ||
		(rev_2 = sd_get_byte ()) < 0
	)
	{
		printf ("sdattach: Não consegui obter o modelo do controlador\n");
		return (-1);
	}

	sd_wait_command_complete ();

	switch (board_id)
	{
	    case 0x31:	str = "AHA-1540"; break;
	    case 0x41:	str = "AHA-1540A/1542A/1542B"; break;
	    case 0x42:	str = "AHA-1640A"; break;
	    case 0x43:	str = "AHA-1542C"; break;
	    case 0x44:	str = "AHA-1542CF"; break;
	    case 0x45:	str = "AHA-1542CF, BIOS V2.01"; break;
	    case 0x46:	str = "AHA-1542CP, BIOS V1.02"; break;
	    default:	printf ("sdattach: Board ID = %X\n", board_id);
			str = "AHA-????"; break;
	}

	/*
	 *	Destranca a "mailbox"
	 */
	switch (board_id)
	{
		int	flags, lock;

	    case 0x41:
		if (rev_1 != 0x31 || rev_2 != 0x34)
			break;
	    case 0x43:
	    case 0x44:
	    case 0x45:
	    case 0x46:
	    case 0x47:	/* Tentando prever o futuro */
	    case 0x48:
	    case 0x49:
#if (0)	/*******************************************************/
		printf ("sd[0x%X]: Liberando a caixa de correio\n", SDPORT);
#endif	/*******************************************************/

		if
		(	sd_put_byte (SD_CMD_GET_EXT_BIOS_INFO) < 0 ||
			(flags = sd_get_byte ()) < 0 ||
			(lock = sd_get_byte ()) < 0
		)
		{
			printf
			(	"sdattach: Não consegui obter a informação "
				"da BIOS\n"
			);
			return (-1);
		}

		sd_wait_command_complete ();

		if
		(	sd_put_byte (SD_CMD_ENABLE_MAIL_BOX) < 0 ||
			sd_put_byte (0) < 0 ||
			sd_put_byte (lock) < 0
		)
		{
			printf
			(	"sdattach: Não consegui liberar a caixa de "
				"correio\n"
			);
			return (-1);
		}

		sd_wait_command_complete ();

	}	/* end switch */

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
		return (-1);

	}	/* end switch */

	/*
	 *	Prepara o DMA
	 */
	dmaexternal (dma);

	mailbox_phaddr = PHYSADDR (sdmailbox);

	/*
	 *	Inicializa a "mailbox" (uma de saída, uma de entrada)
	 */
	if
	(	sd_put_byte (SD_CMD_MAILBOX_INIT) < 0 ||
		sd_put_byte (1) < 0 ||
		sd_put_byte (mailbox_phaddr >> 16) < 0 ||
		sd_put_byte (mailbox_phaddr >> 8) < 0 ||
		sd_put_byte (mailbox_phaddr) < 0
	)
	{
		printf ("sdattach: Não consegui dar o endereço da MAILBOX\n");
		return (-1);
	}

	sd_wait_command_complete ();

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

	/*
	 *	Verifica qual das unidades está presentes
	 */
   /***	d.p_unit = 0; ***/

	for (target = 0, sp = sd_scsi; target < NSD; target++, sp++)
	{
		if (target == id)
			continue;

		sp->scsi_target = target;

		d.p_name[2] =  'a' + scsi_index;
	   /***	d.p_dev	    =  MAKEDEV (SD_MAJOR, 0); ***/
		d.p_target  =  target;

		sp->scsi_disktb = &d;

		sp->scsi_dev_nm = d.p_name;
		sp->scsi_cmd	= sdcmd;

		scsi_attach (sp);

		d.p_blshift = sp->scsi_blshift;

		if (sp->scsi_is_present)
		{
			if (scsi_index >= MAX_TARGETs)
				{ printf ("Dispositivos SCSI excessivos\n"); break; }

			if ((sp->scsi_disktb = malloc_byte (sizeof (DISKTB))) == NODISKTB)
				{ printf ("Memória esgotada\n"); break; }

			memmove (sp->scsi_disktb, &d, sizeof (DISKTB));

			sp->scsi_dev_nm = ((DISKTB *)sp->scsi_disktb)->p_name;

			scsi_ptr[scsi_index++] = sp;
		}

	}	/* end for (procurando alvos) */


	return (0);

}	/* end sdattach */

/*
 ****************************************************************
 *	Prepara para leitura/escrita				*
 ****************************************************************
 */
int
sdopen (int target)
{
	/*
	 *	Verifica a validade dos argumentos
	 */
	if ((unsigned)target >= NSD)
	{
		printf ("sd%d: Unidade inválida\n", target);
		return (-1);
	}

	return (scsi_open (&sd_scsi[target]));

}	/* end sdopen */

/*
 ****************************************************************
 *	Realiza uma leitura/escrita 				*
 ****************************************************************
 */
int
sdstrategy (BHEAD *bp)
{
	const DISKTB	*pp = bp->b_disktb;
	int		target = pp->p_target;
	SCSI		*sp = &sd_scsi[target];

	/*
	 *	Verifica a validade dos argumentos
	 */
	if ((unsigned)target >= NSD || !sp->scsi_is_present)
	{
		printf ("sd%d: Unidade inválida\n", target);
		return (-1);
	}

	if (pp->p_size != 0 && (unsigned)bp->b_blkno >= pp->p_size)
	{
		printf ("%s: Bloco inválido: %d\n", sp->scsi_dev_nm, bp->b_blkno);
		return (-1);
	}

	bp->b_ptr	= sp;
	bp->b_blkno    += pp->p_offset;

	return (sdcmd (bp));

}	/* end sdstrategy */

/*
 ****************************************************************
 *	Executa uma operação por estado				*
 ****************************************************************
 */
int
sdcmd (BHEAD *bp)
{
	SCSI		*sp = bp->b_ptr;
	CCB		*ccp = &sdccb;
	char		*cp;
	const DISKTB	*up = bp->b_disktb;
	int		value, num_try = NUM_TRY, sense;

	/*
	 *	Verifica a caixa postal
	 */
    again:
	if (sdmailbox[0].mail_cmd != 0)
		printf ("%s: MAILBOX não disponível?\n", sp->scsi_dev_nm);

	/*
	 *	Prepara o CCB
	 */
	ccp->ccb_opcode = 3;	/* Sem scatter list */

	ccp->ccb_addr_and_control = sp->scsi_target << 5;

	if   (bp->b_base_count == 0)
		ccp->ccb_addr_and_control |= 0x18;
	elif (bp->b_flags == B_WRITE)
		ccp->ccb_addr_and_control |= 0x10;
	else
		ccp->ccb_addr_and_control |= 0x08;

	sd_ltol3 (bp->b_base_count, ccp->ccb_data_len);

	ccp->ccb_sense_len = MAXSENSE;

	sd_ltol3 (PHYSADDR (bp->b_addr), ccp->ccb_data_ptr);
	sd_ltol3 (0, ccp->ccb_link);

	ccp->ccb_link_id = 0;
	ccp->ccb_host_status = 0;
	ccp->ccb_unit_status = 0;
	ccp->ccb_zero1 = 0;
	ccp->ccb_zero2 = 0;

	/*
	 *	Prepara os comandos SCSI
	 */
	if (bp->b_flags & B_STAT)
	{
	   	memmove (ccp->ccb_cdb, bp->b_cmd_txt, bp->b_cmd_len);
	   	ccp->ccb_scsi_command_len = bp->b_cmd_len;
	}
	else	/* (bp->b_flags == B_READ || bp->b_flags == B_WRITE) */
	{
	   	ccp->ccb_scsi_command_len = 10;
	   	cp = ccp->ccb_cdb;

		cp[0] = (bp->b_flags == B_WRITE) ? 0x2A : 0x28;
		cp[1] = 0;
#if (0)	/*******************************************************/
		*(long *)&cp[2] = long_endian_cv (bp->b_blkno);
#endif	/*******************************************************/
		*(long *)&cp[2] = long_endian_cv
				((bp->b_blkno + up->p_offset) >> (sp->scsi_blshift - BLSHIFT));
		cp[6] = 0;
#if (0)	/*******************************************************/
		*(short *)&cp[7] = short_endian_cv (1); 	/* Um bloco */
#endif	/*******************************************************/
		*(short *)&cp[7] = short_endian_cv (bp->b_base_count >> sp->scsi_blshift);
		cp[9] = 0;
	}

	/*
	 *	Inicia a operação
	 */
	sd_ltol3 (PHYSADDR (&sdccb), sdmailbox[0].mail_ccb);
	sdmailbox[0].mail_cmd = 1;

	sd_put_byte (SD_CMD_START_SCSI_COMMAND);

	/*
	 *	Espera o término da operação
	 */
	while (sdmailbox[1].mail_cmd == 0)
		/* vazio */;

	sdmailbox[1].mail_cmd = 0;

	/*
	 *	Verifica se houve erro
	 */
	if ((value = ccp->ccb_host_status) != 0)
	{
		if (value == 17)	/* Unidade inexistente */
			return (-1);

		printf ("%s: Erro do controlador: %d\n", sp->scsi_dev_nm, value);
		goto bad;
	}

	switch (value = ccp->ccb_unit_status)
	{
	    case 0:	/* Sucesso */
		return (0);

	    case 2: 	/* Erro tradicional */
		break;

	    case 8:	/* Está ocupado */
		printf ("%s: Unidade ocupada\n", sp->scsi_dev_nm);
		goto bad;

	    default:	/* Erro da unidade */
		printf ("%s: Erro da unidade: %d\n", sp->scsi_dev_nm, value);
		goto bad;

	}	/* end switch */

	/*
	 *	Erro tradicional
	 */
	cp = ccb_sense (ccp);

	if ((cp[0] & 0x7F) != 0x70)	/* Não tem informação adicional */
	{
		printf ("%s: Erro SCSI: 0x%X\n", sp->scsi_dev_nm, cp[0] & 0x7F);
		goto bad;
	}

	if (sp->scsi_is_tape && (cp[2] & 0x0F) == 0)
	{
		if (cp[2] & 0xE0)	/* Tape Mark ? */
			return (0);
	}

	if   ((sense = scsi_sense (sp, cp[2] & 0x0F)) > 0)
		return (0);
	elif (sense < 0)
		return (-1);

	/*
	 *	Em caso de erro, ....
	 */
    bad:
	if (--num_try > 0)
		goto again;

	return (-1);

}	/* end sdcmd */

/*
 ****************************************************************
 *	Rotina de IOCTL						*
 ****************************************************************
 */
int
sd_ctl (const DISKTB *up, int cmd, int arg)
{
	return (scsi_ctl (&sd_scsi[up->p_target], up, cmd, arg));

}	/* end sd_ctl */

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
