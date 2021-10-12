/*
 ****************************************************************
 *								*
 *			ataacer.c				*
 *								*
 *	Rotinas auxiliares para controladores Acer		*
 *								*
 *	Versão	4.9.0, de 09.04.07				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2006 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */
#undef	DEBUG

#ifdef	BOOT

#include <common.h>
#include <bhead.h>

#include <scsi.h>
#include <ata.h>

#include "../../boot/boot2/h/proto.h"

#else

#include "../h/common.h"
#include "../h/sync.h"
#include "../h/devhead.h"
#include "../h/bhead.h"

#include "../h/pci.h"

#include "../h/scsi.h"
#include "../h/ata.h"

#include "../h/extern.h"
#include "../h/proto.h"

#endif

#if (0)	/********************************************************/
/*
 ****************************************************************
 *	Inicialização para o Chipset "Acer Alladin"		*
 ****************************************************************
 */
int
acer_dma_init (ATA *ap, PCIDATA *pci)
{
	ulong		word54;
	uchar		reg53;

	/*
	 *	Verifica se o cabo é apropriado para Ultra-DMA-66 ou superior
	 */
	if (adp->ad_udma_mode > ATA_UDMA2 && (adp->ad_flags & HD_CBLID) == 0)
	{
		printf ("ata[%d,%d]: DMA limitado a UDMA33\n", ATA_DEVNO);
		adp->ad_udma_mode = ATA_UDMA2;
	}

	/*
	 *	Tenta primeiro Ultra-DMA-33
	 */
	if (adp->ad_udma_mode >= ATA_UDMA2)
	{
		if (ata_command (ap, ATA_C_SETFEATURES, 0, ATA_UDMA2, ATA_C_F_SETXFER, 0) == 0)
		{
			word54  = pci_read (pci, 0x54, 4);
			word54 |= 0x5555;
			word54 |= (0x0A << (16 + (adp->ad_unit << 3) + (adp->ad_devno << 2)));

			pci_write (pci, 0x54, word54, 4);
			reg53 = pci_read (pci, 0x53, 1);
			pci_write (pci, 0x53, reg53 | 0x03, 1);

			return (ATA_UDMA2);
		}
	}

	/*
	 *	Tenta o DMA regular
	 */
	if (adp->ad_wdma_mode >= ATA_WDMA2 && adp->ad_pio_mode >= ATA_PIO4)
	{
		if (ata_command (ap, ATA_C_SETFEATURES, 0, ATA_WDMA2, ATA_C_F_SETXFER, 0) == 0)
		{
			reg53 = pci_read (pci, 0x53, 1);
			pci_write (pci, 0x53, reg53 | 0x03, 1);

			return (ATA_WDMA2);
		}
	}

	/*
	 *	Vai mesmo usar PIO
	 */
	reg53 = pci_read (pci, 0x53, 1);
	pci_write (pci, 0x53, (reg53 & ~0x01) | 0x02, 1);

	return (0);

}	/* end acer_dma_init */
#endif	/********************************************************/
