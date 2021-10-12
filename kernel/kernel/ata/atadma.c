/*
 ****************************************************************
 *								*
 *			atadma.c				*
 *								*
 *	Tratamento do DMA					*
 *								*
 *	Versão	4.9.0, de 26.04.07				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2007 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include "../h/common.h"
#include "../h/sync.h"
#include "../h/map.h"

#include "../h/pci.h"
#include "../h/scsi.h"
#include "../h/devhead.h"
#include "../h/bhead.h"
#include "../h/ata.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ******	Definições relativas ao DMA *****************************
 */
#define DMATB_MAX_SZ			256
#define	NODMATB				(DMATB *)0

#define	DMA_EOT_BIT			0x80000000

typedef	struct				/* A tabela de endereços */
{
	ulong	dma_base;
	ulong	dma_count;

}	DMATB;

/*
 ****************************************************************
 *	Inicializa o DMA para os Controladores SATA		*
 ****************************************************************
 */
void
ata_sata_setmode (PCIDATA *pci, ATA_DEVICE *adp, int mode)
{
	if (adp->ad_param.satacapabilities != 0x0000 && adp->ad_param.satacapabilities != 0xFFFF)
	{
		int port = adp->ad_channel->ch_ports[ATA_SSTATUS];

		/* SATA verdadeiro */

		mode = ata_limit_mode (adp, mode, ATA_UDMA6),

		ata_command (adp, ATA_C_SETFEATURES, 0, mode, ATA_C_F_SETXFER, 0);

#if (0)	/*******************************************************/
		if (ch->r_io[ATA_SSTATUS].res && 
			   ((ATA_IDX_INL(ch, ATA_SSTATUS) & ATA_SS_CONWELL_MASK) == ATA_SS_CONWELL_GEN2))
#endif	/*******************************************************/

		if (port > 0 && (read_port_long (port) & ATA_SS_CONWELL_MASK) == ATA_SS_CONWELL_GEN2)
			adp->ad_transfer_mode = ATA_SA300;
		else
			adp->ad_transfer_mode = ATA_SA150;
	}
	else
	{
		/* Não é SATA verdadeiro: limita a UDMA5 = ATA100 */

		mode = ata_limit_mode (adp, mode, ATA_UDMA5);

		ata_command (adp, ATA_C_SETFEATURES, 0, mode, ATA_C_F_SETXFER, 0);

		adp->ad_transfer_mode = mode;
	}

}	/* end ata_sata_setmode */

/*
 ****************************************************************
 *	Inicializa o DMA					*
 ****************************************************************
 */
void
ata_dma_reset (ATA_CHANNEL *chp)
{
	write_port
	(
		read_port (chp->ch_ports[ATA_BMCMD_PORT]) & ~ATA_BMCMD_START_STOP,
		chp->ch_ports[ATA_BMCMD_PORT]
	);

	write_port (ATA_BMSTAT_INTERRUPT | ATA_BMSTAT_ERROR, chp->ch_ports[ATA_BMSTAT_PORT]);

}	/* end ata_dma_reset */

/*
 ****************************************************************
 *	Prepara a tabela relativa a uma transferência		*
 ****************************************************************
 */
int
ata_dma_load (ATA_DEVICE *adp, ATA_SCB *scbp)
{
	ATA_CHANNEL		*chp;
	DMATB			*dma_tb_ptr;
	ulong			vaddr;
	int			count, ncount;

	/*
	 *	Consistência inicial
	 */
	vaddr  = (ulong)scbp->scb_area;
	count  = scbp->scb_bytes_requested;

	if (count <= 0 || (vaddr & 0x03) != 0 || (count & 0x03) != 0)
	{
		printf
		(	"ata[%d,%d]: impossível usar DMA: vaddr = %P, count = %d\n",
			ATA_DEVNO, vaddr, count
		);

		return (-1);
	}

	/*
	 *	Verifica se é preciso alocar a tabela de pedidos
	 */
	chp = adp->ad_channel;

	if ((dma_tb_ptr = chp->ch_dma_area) == NODMATB)
	{
		dma_tb_ptr = (DMATB *)PGTOBY (malloce (M_CORE, 1 /* página */));

		if (dma_tb_ptr == NODMATB)
		{
			printf ("ata[%d,%d]: não foi possível alocar a tabela DMATB\n", ATA_DEVNO);
			return (-1);
		}

		chp->ch_dma_area = dma_tb_ptr;

	}	/* end if (não há DMATB alocada) */

	/*
	 *	Gera a primeira entrada da tabela
	 */
	dma_tb_ptr->dma_base  = VIRT_TO_PHYS_ADDR (vaddr);
	dma_tb_ptr->dma_count = ncount = MIN (count, PGSZ - (vaddr & PGMASK));

	vaddr += ncount;
	count -= ncount;

	/*
	 *	Gera as demais entradas
	 */
	while (count > 0)
	{
		if (++dma_tb_ptr >= &dma_tb_ptr[DMATB_MAX_SZ])
		{
			printf ("ata[%d,%d]: estouro na tabela DMATB\n", ATA_DEVNO);
			return (-1);
		}

		dma_tb_ptr->dma_base  = VIRT_TO_PHYS_ADDR (vaddr);
		dma_tb_ptr->dma_count = ncount = MIN (count, PGSZ);

		vaddr += ncount;
		count -= ncount;
	}

	dma_tb_ptr->dma_count |= DMA_EOT_BIT;	/* Última entrada */

	return (0);

}	/* end ata_dma_load */

/*
 ****************************************************************
 *	Inicia a operação por DMA				*
 ****************************************************************
 */
void
ata_dma_start (ATA_CHANNEL *chp, int rw)
{
	write_port
	(
		read_port (chp->ch_ports[ATA_BMSTAT_PORT]) | ATA_BMSTAT_INTERRUPT | ATA_BMSTAT_ERROR,
		chp->ch_ports[ATA_BMSTAT_PORT]
	);

	write_port_long (VIRT_TO_PHYS_ADDR (chp->ch_dma_area), chp->ch_ports[ATA_BMDTP_PORT]);

	write_port
	(
		(read_port (chp->ch_ports[ATA_BMCMD_PORT]) & ~ATA_BMCMD_WRITE_READ) |
		(rw == B_READ ? ATA_BMCMD_WRITE_READ : 0) | ATA_BMCMD_START_STOP,
		chp->ch_ports[ATA_BMCMD_PORT]
	);

}	/* end ata_dma_start */

/*
 ****************************************************************
 *	Encerra uma operação por DMA				*
 ****************************************************************
 */
int
ata_dma_stop (ATA_DEVICE *adp)
{
	ATA_CHANNEL		*chp = adp->ad_channel;
	int			error;

	write_port
	(
		read_port (chp->ch_ports[ATA_BMCMD_PORT]) & ~ATA_BMCMD_START_STOP,
		chp->ch_ports[ATA_BMCMD_PORT]
	);

	error = read_port (chp->ch_ports[ATA_BMSTAT_PORT]) & ATA_BMSTAT_MASK;

	write_port (ATA_BMSTAT_INTERRUPT | ATA_BMSTAT_ERROR, chp->ch_ports[ATA_BMSTAT_PORT]);

	return (error);

}	/* end ata_dma_stop */

/*
 ****************************************************************
 *	Obtém o modo máximo de transferência			*
 ****************************************************************
 */
int
ata_limit_mode (ATA_DEVICE *adp, int mode, int maxmode)
{
	if (maxmode && mode > maxmode)
		mode = maxmode;

	if (mode >= ATA_UDMA0 && adp->ad_udma_mode > 0)
		return (MIN (mode, adp->ad_udma_mode));

	if (mode >= ATA_WDMA0 && adp->ad_wdma_mode > 0)
		return (MIN (mode, adp->ad_wdma_mode));

	if (mode > adp->ad_pio_mode)
		return (MIN (mode, adp->ad_pio_mode));

	return (mode);

}	/* end ata_limit_mode */

/*
 ****************************************************************
 *	Converte um modo de DMA para uma cadeia			*
 ****************************************************************
 */
char *
ata_dmamode_to_str (int mode)
{
	switch (mode)
	{
	    case ATA_PIO0:
		return ("PIO0");

	    case ATA_PIO1:
		return ("PIO1");

	    case ATA_PIO2:
		return ("PIO2");

	    case ATA_PIO3:
		return ("PIO3");

	    case ATA_PIO4:
		return ("PIO4");

	    case ATA_WDMA0:
		return ("WDMA0");

	    case ATA_WDMA1:
		return ("WDMA1");

	    case ATA_WDMA2:
		return ("WDMA2");

	    case ATA_UDMA0:
		return ("UDMA16");

	    case ATA_UDMA1:
		return ("UDMA25");

	    case ATA_UDMA2:
		return ("UDMA33");

	    case ATA_UDMA3:
		return ("UDMA40");

	    case ATA_UDMA4:
		return ("UDMA66");

	    case ATA_UDMA5:
		return ("UDMA100");

	    case ATA_UDMA6:
		return ("UDMA133");

	    case ATA_SA150:
		return ("SATA150");

	    case ATA_SA300:
		return ("SATA300");

	    default:
		return ("modo desconhecido");
	}

}	/* end ata_dmamode_to_str */

int
ata_mode2idx (int mode)
{
	if ((mode & ATA_DMA_MASK) == ATA_UDMA0)
		return (mode & ATA_MODE_MASK) + 8;

	if ((mode & ATA_DMA_MASK) == ATA_WDMA0)
		return (mode & ATA_MODE_MASK) + 5;

	return (mode & ATA_MODE_MASK) - ATA_PIO0;

}	/* end ata_mode2idx */
