/*
 ****************************************************************
 *								*
 *			sb.c					*
 *								*
 *	"Driver" para placa de som estilo SOUND-BLASTER		*
 *								*
 *	Versão	3.2.0, de 03.05.99				*
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

#include "../h/pci.h"
#include "../h/inode.h"
#include "../h/frame.h"
#include "../h/mmu.h"
#include "../h/intr.h"
#include "../h/ioctl.h"
#include "../h/uerror.h"
#include "../h/signal.h"
#include "../h/uproc.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ******	Definições globais **************************************
 */
#define	NSB	1		/* Por enquanto só um */

#undef	DEBUG

#define PL	4		/* Prioridade de interrupção */
#define splsb	spl4		/* Função de prioridade de interrupção */

#define	B_WRITE		0x0000	/* Escrita (não-leitura) */
#define	B_READ		0x0001	/* Leitura */
#define	B_AUTO		0x0002	/* DMA auto iniciado */

#define M16MBLIMIT	(16 * MBSZ)
#define M64KBSZ		(64 * KBSZ)
#define M64KBMASK	(M64KBSZ - 1)

/*
 ******	Lista de possíveis endereços ****************************
 */
const int	sb_ports[] = { 0x220, 0x240, 0x260, 0x280, 0};

/*
 ******	Registros do SB *****************************************
 */
#define	SB_MIXER_ADDR_PORT	(port + 0x04)
#define	SB_MIXER_DATA_PORT	(port + 0x05)
#define	SB_RESET_PORT		(port + 0x06)
#define	SB_READ_DATA_PORT	(port + 0x0A)
#define	SB_WRITE_DATA_PORT	(port + 0x0C)
#define	SB_WRITE_CMD_PORT	(port + 0x0C)
#define	SB_WRITE_STATUS_PORT	(port + 0x0C)
#define	SB_DATA_AVAIL_PORT	(port + 0x0E)
#define	SB_INTR_8_ACK_PORT	(port + 0x0E)
#define	SB_INTR_16_ACK_PORT	(port + 0x0F)

/*
 ****** Lista de Comandos do SB *********************************
 */
#define	SB_SET_SAMPLING_RATE_CMD 0x41

#define	SB_SET_8_BIT_OUTPUT_CMD	 0xC6
#define	SB_SET_16_BIT_OUTPUT_CMD 0xB6

#define	SB_SET_8_BIT_MONO_CMD	 0x00
#define	SB_SET_16_BIT_MONO_CMD	 0x10
#define	SB_SET_8_BIT_STEREO_CMD	 0x20
#define	SB_SET_16_BIT_STEREO_CMD 0x30

#define	SB_EXIT_8_BIT_DMA_CMD	 0xDA
#define	SB_EXIT_16_BIT_DMA_CMD	 0xD9

#define	SB_GET_DSP_VERSION_CMD	 0xE1

/*
 ****** Lista de registros do "mixer" ***************************
 */
#define	SB_MASTER_VOL_L_INDEX	0x30
#define	SB_MASTER_VOL_R_INDEX	0x31
#define	SB_VOICE_VOL_L_INDEX	0x32
#define	SB_VOICE_VOL_R_INDEX	0x33
#define	SB_CD_VOL_L_INDEX	0x36
#define	SB_CD_VOL_R_INDEX	0x37
#define	SB_OUTPUT_GAIN_L_INDEX	0x41
#define	SB_OUTPUT_GAIN_R_INDEX	0x42

#define	SB_IRQ_INDEX		0x80
#define	SB_DMA_INDEX		0x81

/*
 ******	Estruturas de controle das unidades *********************
 */
typedef struct
{
	int		s_port;		/* Porta do DSP da SB */
	int		s_major;	/* Primeira parte da Versão */
	int		s_minor;	/* Segunda parte da Versão */
	int		s_irq;		/* No. do IRQ */
	int		s_dma_16;	/* No. do DMA de 16 bits */
	int		s_dma_8;	/* No. do DMA de 8 bits */
	const char	*s_nm;		/* Nome da Placa de som */

	int		s_bits;		/* Tamanho de cada amostra */
	int		s_rate;		/* Freqüência de amostragem */
	int		s_stereo;	/* Stereo */

	char		s_open;		/* Já está aberto */
	LOCK		s_busy;		/* Unidade fazendo I/O */
	EVENT		s_done;		/* Evento de final de operação */
	char		s_intr_cnt;	/* Contador de interrupções */

	char		s_low_high;	/* Qual das áreas em DMA */

	void		*s_area;	/* Endereço inicial das 2 áreas */
	int		s_count;	/* Tamanho de uma das áreas */

}	SBDATA;

/*
 ******	Variáveis globais ***************************************
 */
entry SBDATA	sbdata[NSB];	/* Dados das unidades */

/*
 ******	Protótipos de funções ***********************************
 */
int		sb_do_attach (int, int, int, int);
int		sb_wait_for_intr (SBDATA *);
void		sbint (struct intr_frame);
void		sb_start_write_operation (IOREQ *, SBDATA *);
int		sb_read_data_port (const SBDATA *);
int		sb_write_data_port (int, const SBDATA *);

/*
 ****************************************************************
 *	Identifica SBs PCI					*
 ****************************************************************
 */
char *
pci_sb_probe (PCIDATA * tag, ulong type)
{
	return (NOSTR);

}	/* end pci_sb_probe */

/*
 ****************************************************************
 *	Anexa SBs PCI						*
 ****************************************************************
 */
void
pci_sb_attach (PCIDATA * config_id, int unit)
{
}	/* end pci_sb_attach */

/*
 ****************************************************************
 *	Identifica SBs ISA PnP					*
 ****************************************************************
 */
int
pnp_sb_probe (ulong id)
{
	return ((id & 0xFFFF) == 0x8C0E);	/* Todas SB têm este sufixo */

	/** case 0xC5008C0E:	/* SB AWE 64 */
	/** case 0xE4008C0E:	/* SB AWE 64 */
	/** case 0xC3008C0E:	/* SB AWE 64 */
	/** case 0x48008C0E:
	/** case 0x28008C0E:	/* SB SB 16 */
	/** case 0x22105407:	/* AZT 1022 PnP */

}	/* end pnp_sb_probe */

/*
 ****************************************************************
 *	Anexa SBs ISA PnP					*
 ****************************************************************
 */
void
pnp_sb_attach (ushort *pport, uchar *irq, uchar *dma)
{
	int	port, count, value;

	port = pport[0];

	write_port (1, SB_RESET_PORT);
	DELAY (10);
	write_port (0, SB_RESET_PORT);

	for (count = 1000; /* abaixo */; count--)
	{
		if (count <= 0)
			return;

		DELAY (1);

		if (read_port (SB_DATA_AVAIL_PORT) & 0x80)
			break;
	}

	if ((value = read_port (SB_READ_DATA_PORT)) != 0xAA)
		return;

	sb_do_attach (port, irq[0], dma[0], dma[1]);

}	/* end pnp_sb_attach */

/*
 ****************************************************************
 *	Função de "attach"					*
 ****************************************************************
 */
int
sbattach (int major)
{
	const int	*ip;
	int		port, count, value, mask, irq, dma_8, dma_16;

	if (sbdata[0].s_port != 0)
		return (0);

	/*
	 *	Procura um SB em uma das portas
	 */
	for (ip = sb_ports; /* abaixo */; ip++)
	{
		if ((port = *ip) == 0)
			return (-1);

		write_port (1, SB_RESET_PORT);
		DELAY (10);
		write_port (0, SB_RESET_PORT);

		for (count = 1000; /* abaixo */; count--)
		{
			if (count <= 0)
				goto next_port;

			DELAY (1);

			if (read_port (SB_DATA_AVAIL_PORT) & 0x80)
				break;
		}

		if ((value = read_port (SB_READ_DATA_PORT)) == 0xAA)
			break;

	    next_port:
		/* vazio */;

	}	/* Percorrendo as possíveis portas */

	/*
	 *	Lê o registro do IRQ
	 */
	write_port (SB_IRQ_INDEX, SB_MIXER_ADDR_PORT);
	mask = read_port (SB_MIXER_DATA_PORT);

	switch (mask & 0x0F)
	{
	    case 0x08:
		irq = 10;
		break;

	    case 0x04:
		irq = 7;
		break;

	    case 0x02:
		irq = 5;
		break;

	    case 0x01:
		irq = 9;	/* O 2 é na verdade o 9 */
		break;

	    default:
		irq = -1;
		break;

	}	/* Examinando o IRQ */

	/*
	 *	Lê o registro do DMA
	 */
	write_port (SB_DMA_INDEX, SB_MIXER_ADDR_PORT);
	mask = read_port (SB_MIXER_DATA_PORT);

	switch (mask & 0xE0)
	{
	    case 0x80:
		dma_16 = 7;
		break;

	    case 0x40:
		dma_16 = 6;
		break;

	    case 0x20:
		dma_16 = 5;
		break;

	    default:
		dma_16 = -1;
		break;

	}	/* Examinando o DMA 16 */

	switch (mask & 0x0B)
	{
	    case 0x08:
		dma_8 = 3;
		break;

	    case 0x02:
		dma_8 = 1;
		break;

	    case 0x01:
		dma_8 = 0;
		break;

	    default:
		dma_8 = -1;
		break;

	}	/* Examinando o DMA 8 */

	return (sb_do_attach (port, irq, dma_8, dma_16));

}	/* end sbattach */

/*
 ****************************************************************
 *	Anexa SBs de qualquer natureza				*
 ****************************************************************
 */
int
sb_do_attach (int port, int irq, int dma_8, int dma_16)
{
	SBDATA		*sp = &sbdata[0];
	int		unit = 0;

	sp->s_port	= port;
	sp->s_irq	= irq;
	sp->s_dma_8	= dma_8;
	sp->s_dma_16	= dma_16;

	/*
	 *	Determina a versão do DSP do SB
	 */
	sb_write_data_port (SB_GET_DSP_VERSION_CMD, sp);

	sp->s_major = sb_read_data_port (sp);
	sp->s_minor = sb_read_data_port (sp);

	/*
	 *	Tenta encontrar o nome da placa de som
	 */
	if (sp->s_major == 4) switch (sp->s_minor)
	{
	    case 13:
		sp->s_nm = "SB16";
		break;

	    case 16:
		sp->s_nm = "SB AWE64";
		break;

	}	/* end switch */

	/*
	 *	Imprime a mensagem
	 */
	printf
	(	"sb:  [%d: 0x%X, V.%d.%d",
		unit, port, sp->s_major, sp->s_minor
	);

	if (sp->s_nm != NOSTR)
		printf (" (%s)", sp->s_nm);

	printf
	(	", irq %d, dma %d/%d]",
		sp->s_irq, sp->s_dma_8, sp->s_dma_16
	);

	/*
	 *	Verifica se nós suportamos esta placa de som
	 */
	if (sp->s_major != 4 || sp->s_irq < 0 || sp->s_dma_8 < 0 || sp->s_dma_16 < 0)
	{
		printf (" (ainda NÃO suportada)\n");
		sp->s_port = 0; return (-1);
	}
	else
	{
		printf ("\n");
	}

	/*
	 *	Põe os valores "default"
	 */
	sp->s_bits	= 8;
	sp->s_rate	= 22050;
	sp->s_stereo	= 0;

	/*
	 *	Prepara o IRQ
	 */
	if (set_dev_irq (sp->s_irq, PL, unit, sbint) < 0)
		return (-1);

	/*
	 *	Coloca os volumes e "ganho" em valores razoáveis
	 */
	write_port (SB_MASTER_VOL_L_INDEX, SB_MIXER_ADDR_PORT);
	write_port (0xF8, SB_MIXER_DATA_PORT);

	write_port (SB_MASTER_VOL_R_INDEX, SB_MIXER_ADDR_PORT);
	write_port (0xF8, SB_MIXER_DATA_PORT);

	write_port (SB_VOICE_VOL_L_INDEX, SB_MIXER_ADDR_PORT);
	write_port (0xF8, SB_MIXER_DATA_PORT);

	write_port (SB_VOICE_VOL_R_INDEX, SB_MIXER_ADDR_PORT);
	write_port (0xF8, SB_MIXER_DATA_PORT);

	write_port (SB_CD_VOL_L_INDEX, SB_MIXER_ADDR_PORT);
	write_port (0xF8, SB_MIXER_DATA_PORT);

	write_port (SB_CD_VOL_R_INDEX, SB_MIXER_ADDR_PORT);
	write_port (0xF8, SB_MIXER_DATA_PORT);

	write_port (SB_OUTPUT_GAIN_L_INDEX, SB_MIXER_ADDR_PORT);
	write_port (0x40, SB_MIXER_DATA_PORT);

	write_port (SB_OUTPUT_GAIN_R_INDEX, SB_MIXER_ADDR_PORT);
	write_port (0x40, SB_MIXER_DATA_PORT);

	return (0);

}	/* sb_do_attach */

/*
 ****************************************************************
 *	Função de "open"					*
 ****************************************************************
 */
void
sbopen (dev_t dev, int oflag)
{
	int		unit;
	SBDATA		*sp;

	/*
	 *	Vê se a unidade é válida e está aberta
	 */
	if ((unsigned)(unit = MINOR (dev)) >= NSB)
		{ u.u_error = ENXIO; return; }

	sp = &sbdata[unit];

	if (sp->s_port == 0)
		{ u.u_error = ENXIO; return; }

	if (sp->s_open == 0)
		sp->s_open++;
	else
		u.u_error = EBUSY;

}	/* end sbopen */

/*
 ****************************************************************
 *	Função de "close"					*
 ****************************************************************
 */
void
sbclose (dev_t dev, int flag)
{
	int		unit;
	SBDATA		*sp;

	/*
	 *	Vê se a unidade é válida e está aberta
	 */
	if ((unsigned)(unit = MINOR (dev)) >= NSB)
		{ u.u_error = ENXIO; return; }

	sp = &sbdata[unit];

	if (sp->s_port == 0)
		{ u.u_error = ENXIO; return; }

	if (sp->s_open > 0)
		sp->s_open--;

	/*
	 *	Se não há operação em andamento, retorna
	 */
#if (0)	/*******************************************************/
	splsb ();
#endif	/*******************************************************/
	splx (irq_pl_vec[sp->s_irq]);

	if (TAS (&sp->s_busy) >= 0)
		{ CLEAR (&sp->s_busy); spl0 (); return; }

	/*
	 *	Desliga a saída
	 */
	if (sp->s_bits == 8)
		sb_write_data_port (SB_EXIT_8_BIT_DMA_CMD, sp);
	else
		sb_write_data_port (SB_EXIT_16_BIT_DMA_CMD, sp);

	sb_wait_for_intr (sp);

	CLEAR (&sp->s_busy);

	spl0 ();

}	/* end sbclose */

/*
 ****************************************************************
 *	Função  de "write"					*
 ****************************************************************
 */
void
sbwrite (IOREQ *iop)
{
	SBDATA		*sp;
	char		time_msg_written = 0;

	/*
	 *	Vê se a unidade é válida e está aberta
	 */
	sp = &sbdata[MINOR (iop->io_dev)];

	if (!sp->s_open)
		{ u.u_error = ENXIO; return; }

	/*
	 *	Se ainda não há operação em andamente, começa uma
	 */
	if (TAS (&sp->s_busy) == 0)
	{
#if (0)	/*******************************************************/
		splsb ();
#endif	/*******************************************************/
		splx (irq_pl_vec[sp->s_irq]);

		sb_start_write_operation (iop, sp);

		iop->io_count = 0;

		spl0 ();

		return;
	}

	/*
	 *	Verifica se é EOF
	 */
	if (iop->io_area == NOVOID)
	{
#if (0)	/*******************************************************/
		splsb ();
#endif	/*******************************************************/
		splx (irq_pl_vec[sp->s_irq]);

		if (sp->s_bits == 8)
			sb_write_data_port (SB_EXIT_8_BIT_DMA_CMD, sp);
		else
			sb_write_data_port (SB_EXIT_16_BIT_DMA_CMD, sp);

		sb_wait_for_intr (sp);

		CLEAR (&sp->s_busy);

		iop->io_count = 0;

		spl0 ();

		return;
	}

	/*
	 *	Já há operação em andamento
	 *	Verifica se a interrupção já veio
	 */
	splx (irq_pl_vec[sp->s_irq]);

	if (sb_wait_for_intr (sp) > 1)
	{
		printf ("%g: A área de saída não foi preenchida em tempo hábil\n");
		time_msg_written++;

		if ((sp->s_intr_cnt & 1) == 0)	/* Para entrar em fase */
			sb_wait_for_intr (sp);
	}

	/*
	 *	Verifica se está em fase
	 */
	if (sp->s_count != iop->io_count)
		printf ("%g: O tamanho da área de saída não confere: %d :: %d\n", iop->io_count, sp->s_count);

	if (sp->s_low_high != 0)	/* Agora é high */
	{
		if (iop->io_area != sp->s_area + sp->s_count)
		{
			if (!time_msg_written)
			{
				printf
				(	"%g: O endereço da área de saída não confere: %P :: %P\n",
					iop->io_area, sp->s_area + sp->s_count
				);
			}
			sp->s_intr_cnt = 0; sb_wait_for_intr (sp);
		}
	}
	else				/* Agora é low */
	{
		if (iop->io_area != sp->s_area)
		{
			if (!time_msg_written)
			{
				printf
				(	"%g: O endereço da área de saída não confere: %P :: %P\n",
					iop->io_area, sp->s_area
				);
			}
			sp->s_intr_cnt = 0; sb_wait_for_intr (sp);
		}
	}

	/*
	 *	Atualiza os contadores
	 */
	sp->s_intr_cnt = 0;

	iop->io_count = 0;

	spl0 ();

}	/* end sbwrite */

/*
 ****************************************************************
 *	Espera por uma interrupção				*
 ****************************************************************
 */
int
sb_wait_for_intr (SBDATA *sp)
{
	/*
	 *	É chamada com nível alto, e assim retorna
	 */
	for (EVER)
	{
		if (sp->s_intr_cnt > 0)
			return (sp->s_intr_cnt);

		EVENTCLEAR (&sp->s_done);

		spl0 ();

		EVENTWAIT (&sp->s_done, PBLKIO);

#if (0)	/*******************************************************/
		splsb ();
#endif	/*******************************************************/
		splx (irq_pl_vec[sp->s_irq]);

	}	/* end for (EVER) */

}	/* end sb_wait_for_intr */

/*
 ****************************************************************
 *	Função  de "read"					*
 ****************************************************************
 */
void
sbread (IOREQ *iop)
{

}	/* end sbread */

/*
 ****************************************************************
 *	Função de IOCTL						*
 ****************************************************************
 */
int
sbctl (dev_t dev, int cmd, void *arg, ...)
{
	SBDATA		*sp;
	int		unit, port;
	int		data[2];
	int		mode[3];

	/*
	 *	Prólogo
	 */
	unit = MINOR (dev); sp = &sbdata[unit]; port = sp->s_port;

	switch (cmd)
	{
		/*
		 *	Lê do MIXER
		 */
	    case SB_READ_MIXER:
		write_port ((int)arg, SB_MIXER_ADDR_PORT);
		return (read_port (SB_MIXER_DATA_PORT));

		/*
		 *	Escreve no MIXER
		 */
	    case SB_WRITE_MIXER:
		if (unimove (data, arg, sizeof (data), US) < 0)
			{ u.u_error = EFAULT; return (-1); }

		write_port (data[1], SB_MIXER_ADDR_PORT);
		write_port (data[0], SB_MIXER_DATA_PORT);
		return (0);

		/*
		 *	Obtém o modo de saída
		 */
	    case SB_GET_MODE:
		mode[0] = sp->s_rate;
		mode[1] = sp->s_bits;
		mode[2] = sp->s_stereo ? 2 : 1;

		if (unimove (arg, mode, sizeof (mode), SU) < 0)
			{ u.u_error = EFAULT; return (-1); }

		return (0);

		/*
		 *	Atribui o modo de saída
		 */
	    case SB_SET_MODE:
		if (unimove (mode, arg, sizeof (mode), US) < 0)
			{ u.u_error = EFAULT; return (-1); }

		if ((unsigned)mode[0] > 44100)
			{ u.u_error = EINVAL; return (-1); }

		if (mode[1] != 8 && mode[1] != 16)
			{ u.u_error = EINVAL; return (-1); }

		if ((unsigned)mode[2] > 2)
			{ u.u_error = EINVAL; return (-1); }

		sp->s_rate   = mode[0];
		sp->s_bits   = mode[1];
		sp->s_stereo = mode[2] == 2 ? 1 : 0;

		return (0);
	}

	u.u_error = EINVAL;
	return (-1);

}	/* end sbctl */

/*
 ****************************************************************
 *	Função de interrupção					*
 ****************************************************************
 */
void
sbint (struct intr_frame frame)
{
	SBDATA		*sp;
	int		port;

	/*
	 *	Examina a interrupção
	 */
	sp = &sbdata[frame.if_unit]; port = sp->s_port;

	/*
	 *	Dá "ack" na interrupção
	 */
	if (sp->s_bits == 8)
		read_port (SB_INTR_8_ACK_PORT);
	else
		read_port (SB_INTR_16_ACK_PORT);

	/*
	 *	Simplesmente incrementa o contador
	 */
	sp->s_intr_cnt++;
	sp->s_low_high ^= 1;

	EVENTDONE (&sp->s_done);

}	/* end sbint */

/*
 ****************************************************************
 *	Inicia a operação de saída				*
 ****************************************************************
 */
void
sb_start_write_operation (IOREQ *iop, SBDATA *sp)
{
	int		count, dma_count;
	void		*area;
	int		cmd, mode;

	/*
	 *	Em primeiro lugar, obtém o endereço físico
	 *
	 *	Repare que o contador de DMA já
	 *	deve levar em conta as duas áreas!
	 */
	count = iop->io_count; dma_count = count << 1;

	area = user_area_to_phys_area (iop->io_area, dma_count);

	if (area == NOVOID)
		return;

	/*
	 *	Verifica se é válido
	 */
	if
	(	(int)area & 1 ||
		(unsigned)area + dma_count > M16MBLIMIT ||
		((unsigned)area & M64KBMASK) + dma_count > M64KBSZ
	)
	{
		printf ("%g: Endereço da área física %P inválido/desalinhado, count = %d\n", area, dma_count);
		u.u_error = EINVAL;
		return;
	}

	/*
	 *	Prepara o DMA
	 */
	uni_dma_setup
	(
		sp->s_major == 4 ? (B_WRITE|B_AUTO) : B_WRITE,
		area,
		dma_count,
		sp->s_bits == 8 ? sp->s_dma_8 : sp->s_dma_16
	);

	/*
	 *	Programa a freqüência de amostragem
	 */
	sb_write_data_port (SB_SET_SAMPLING_RATE_CMD, sp);
	sb_write_data_port (sp->s_rate >> 8, sp);
	sb_write_data_port (sp->s_rate, sp);

	/*
	 *	Inicia a operação
	 */
	if (sp->s_bits == 8)
	{
		cmd = SB_SET_8_BIT_OUTPUT_CMD;
		mode = (sp->s_stereo ? SB_SET_8_BIT_STEREO_CMD : SB_SET_8_BIT_MONO_CMD);
	}
	else	/* 16 bits */
	{
		cmd = SB_SET_16_BIT_OUTPUT_CMD;
		mode = (sp->s_stereo ? SB_SET_16_BIT_STEREO_CMD : SB_SET_16_BIT_MONO_CMD);
		count >>= 1;
	}

	sb_write_data_port (cmd, sp);
	sb_write_data_port (mode, sp);
	sb_write_data_port (--count, sp);
	sb_write_data_port (count >> 8, sp);

	/*
	 *	Guarda o histórico para conferir a cada "write" seguinte
	 */
	sp->s_intr_cnt  = 0;
	sp->s_area 	= iop->io_area;
	sp->s_count	= iop->io_count;
	sp->s_low_high	= 0;	/* Low */

}	/* end sb_start_write_operation */

/*
 ****************************************************************
 *	Lê da porta de dados					*
 ****************************************************************
 */
int
sb_read_data_port (const SBDATA	*sp)
{
	int		count, port = sp->s_port;

	for (count = 1000; count > 0; count--)
	{
		if (read_port (SB_DATA_AVAIL_PORT) & 0x80)
			return (read_port (SB_READ_DATA_PORT));

		DELAY (1);
	}

	printf ("%g: Timeout na leitura da porta de dados\n");

	return (-1);

}	/* end sb_read_data_port */

/*
 ****************************************************************
 *	Lê da porta de dados					*
 ****************************************************************
 */
int
sb_write_data_port (int data, const SBDATA *sp)
{
	int		count, port = sp->s_port;

	for (count = 1000; count > 0; count--)
	{
		if ((read_port (SB_WRITE_STATUS_PORT) & 0x80) == 0)
		{
			write_port (data, SB_WRITE_DATA_PORT);
			return (0);
		}

		DELAY (1);
	}

	printf ("%g: Timeout na escrita da porta de dados\n");

	return (-1);

}	/* end sb_write_data_port */
