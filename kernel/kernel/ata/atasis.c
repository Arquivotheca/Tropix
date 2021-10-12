/*
 ****************************************************************
 *								*
 *			atasis.c				*
 *								*
 *	Rotinas auxiliares para controladores SiS		*
 *								*
 *	Versão	4.9.0, de 16.04.07				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2007 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */
#undef	DEBUG

#ifdef	BOOT

#include <common.h>
#include <bhead.h>

#include <pci.h>
#include <scsi.h>
#include <ata.h>

#include "../../boot/boot2/h/proto.h"

#else

#include "../h/common.h"
#include "../h/sync.h"

#include "../h/pci.h"
#include "../h/scsi.h"
#include "../h/devhead.h"
#include "../h/ata.h"

#include "../h/extern.h"
#include "../h/proto.h"

#endif

ATA_CHIP_ID sis_ids[] =
{
	{ ATA_SIS182,  0x00, SISSATA,   0, ATA_SA150, "182" }, /* south */
	{ ATA_SIS181,  0x00, SISSATA,   0, ATA_SA150, "181" }, /* south */
	{ ATA_SIS180,  0x00, SISSATA,   0, ATA_SA150, "180" }, /* south */

	{ ATA_SIS965,  0x00, SIS133NEW, 0, ATA_UDMA6, "965" }, /* south */
	{ ATA_SIS964,  0x00, SIS133NEW, 0, ATA_UDMA6, "964" }, /* south */
	{ ATA_SIS963,  0x00, SIS133NEW, 0, ATA_UDMA6, "963" }, /* south */
	{ ATA_SIS962,  0x00, SIS133NEW, 0, ATA_UDMA6, "962" }, /* south */

	{ ATA_SIS745,  0x00, SIS100NEW, 0, ATA_UDMA5, "745" }, /* 1chip */
	{ ATA_SIS735,  0x00, SIS100NEW, 0, ATA_UDMA5, "735" }, /* 1chip */
	{ ATA_SIS733,  0x00, SIS100NEW, 0, ATA_UDMA5, "733" }, /* 1chip */
	{ ATA_SIS730,  0x00, SIS100OLD, 0, ATA_UDMA5, "730" }, /* 1chip */

	{ ATA_SIS635,  0x00, SIS100NEW, 0, ATA_UDMA5, "635" }, /* 1chip */
	{ ATA_SIS633,  0x00, SIS100NEW, 0, ATA_UDMA5, "633" }, /* unknown */
	{ ATA_SIS630,  0x30, SIS100OLD, 0, ATA_UDMA5, "630S"}, /* 1chip */
	{ ATA_SIS630,  0x00, SIS66,     0, ATA_UDMA4, "630" }, /* 1chip */
	{ ATA_SIS620,  0x00, SIS66,     0, ATA_UDMA4, "620" }, /* 1chip */

	{ ATA_SIS550,  0x00, SIS66,     0, ATA_UDMA5, "550" },
	{ ATA_SIS540,  0x00, SIS66,     0, ATA_UDMA4, "540" },
	{ ATA_SIS530,  0x00, SIS66,     0, ATA_UDMA4, "530" },

	{ ATA_SIS5513, 0xC2, SIS33,     1, ATA_UDMA2, "5513" },
	{ ATA_SIS5513, 0x00, SIS33,     1, ATA_WDMA2, "5513" },
	{ 0 }
};

static ATA_CHIP_ID	sis_quirk1 = { ATA_SIS963, 0x00, SIS133NEW, 0, ATA_UDMA6, "962/963" };
static ATA_CHIP_ID	sis_quirk2;

/*
 ******	Protótipos de funções ***********************************
 */
int		ata_sis_reset (PCIDATA *, ATA_CHANNEL *);
int		ata_sis_intr_status (ATA_CHANNEL *);
void		ata_sis_pata_setmode (PCIDATA *, ATA_DEVICE *, int);
void		ata_sis_sata_setmode (PCIDATA *, ATA_DEVICE *, int);

/*
 ****************************************************************
 *	Reconhecimento						*
 ****************************************************************
 */
const ATA_CHIP_ID *
ata_sis_ident (PCIDATA *pci, ulong chipset_id)
{
	ATA_CHIP_ID		*idp;
	int			found = 0;
	uchar			rev  = pci->pci_revid;
	ATA_PCI_CTLR		*ctlrp = pci->pci_dev_info_ptr;

	ctlrp->ac_chip_nm  = "SiS";
	ctlrp->ac_chipinit = ata_sis_chipinit;

#if (0)	/*******************************************************/
	PCIDATA			*parent = pci->pci_parent;
	chipset_id = (parent->pci_device << 16) | parent->pci_vendor;
#endif	/*******************************************************/

	for (idp = sis_ids; /* abaixo */; idp++)
	{
		if (idp->ch_id == 0)
			return (ctlrp->ac_idp = NULL);

		if (chipset_id == idp->ch_id && rev >= idp->ch_rev)
			break;
	}

	if (idp->ch_cfg2 /*** && !found ***/)
	{
		uchar		reg57 = pci_read (pci, 0x57, 1);

		pci_write (pci, 0x57, (reg57 & 0x7F), 1);

		if (pci_read (pci, PCIR_DEVVENDOR, 4) == ATA_SIS5518)
			{ found = 1; idp = &sis_quirk1; }

		pci_write (pci, 0x57, reg57, 1);
	}

	if (idp->ch_cfg2 && !found)
	{
		uchar	reg4a = pci_read (pci, 0x4A, 1);

		pci_write (pci, 0x4A, (reg4a | 0x10), 1);

		if (pci_read (pci, PCIR_DEVVENDOR, 4) == ATA_SIS5517)
		{
			memmove (&sis_quirk2, idp, sizeof (ATA_CHIP_ID));

			found = 1;

			idp = &sis_quirk2;	idp->ch_nm = "961";

			if (chipset_id == ATA_SISSOUTH)
			{
				idp->ch_cfg1	= SIS133OLD;
				idp->ch_max_dma = ATA_UDMA6;
			}
			else
			{
				idp->ch_cfg1	= SIS100NEW;
				idp->ch_max_dma = ATA_UDMA5;
			}
		}

		pci_write (pci, 0x4A, reg4a, 1);
	}

	return (ctlrp->ac_idp = idp);

}	/* end ata_sis_ident */

/*
 ****************************************************************
 *	Inicialização						*
 ****************************************************************
 */
int
ata_sis_chipinit (PCIDATA *pci)
{
 	ATA_PCI_CTLR		*ctlrp = pci->pci_dev_info_ptr;
	const ATA_CHIP_ID	*idp = ctlrp->ac_idp;

	switch (idp->ch_cfg1)
	{
	    case SIS33:
		break;

	    case SIS66:
	    case SIS100OLD:
		pci_write (pci, 0x52, pci_read (pci, 0x52, 1) & ~0x04, 1);
		break;

	    case SIS100NEW:
	    case SIS133OLD:
		pci_write (pci, 0x49, pci_read (pci, 0x49, 1) & ~0x01, 1);
		break;

	    case SIS133NEW:
		pci_write (pci, 0x50, pci_read (pci, 0x50, 2) | 0x0008, 2);
		pci_write (pci, 0x52, pci_read (pci, 0x52, 2) | 0x0008, 2);
		break;

	    case SISSATA:
#if (0)	/*******************************************************/
		ctlr->r_type2 = SYS_RES_IOPORT;
		ctlr->r_rid2 = PCIR_BAR(5);
		if ((ctlr->r_res2 = bus_alloc_resource_any(dev, ctlr->r_type2, &ctlr->r_rid2, RF_ACTIVE)))
		{
			ctlrp->ac_allocate = ata_sis_allocate;
			ctlrp->ac_reset = ata_sis_reset;

			/* enable PCI interrupt */
			pci_write (pci, PCIR_COMMAND, pci_read (pci, PCIR_COMMAND, 2) & ~0x0400,2);
		}

		ctlrp->ac_setmode = ata_sata_setmode;

		return (0);
#endif	/*******************************************************/

	    default:
		return (-1);
	}

#ifndef	BOOT
	ctlrp->ac_setmode = ata_sis_pata_setmode;
#endif

	return (0);

}	/* end ata_sis_chipinit */

#ifndef	BOOT
/*
 ****************************************************************
 *	Inicializa o DMA para os Controladores PATA		*
 ****************************************************************
 */
void
ata_sis_pata_setmode (PCIDATA *pci, ATA_DEVICE *adp, int mode)
{
 	ATA_PCI_CTLR		*ctlrp = pci->pci_dev_info_ptr;
	const ATA_CHIP_ID	*idp = ctlrp->ac_idp;
	int			devno;

	devno = (adp->ad_channel->ch_unit << 1) + adp->ad_devno;

	mode  = ata_limit_mode (adp, mode, idp->ch_max_dma);

	if (idp->ch_cfg1 == SIS133NEW)
	{
		if (mode > ATA_UDMA2 && pci_read (pci, adp->ad_channel->ch_unit ? 0x52 : 0x50, 2) & 0x8000)
		{
			mode = ATA_UDMA2;
			printf ("ata[%d,%d]: DMA limitado a %s\n", ATA_DEVNO, ata_dmamode_to_str (mode));
		}
	}
	else
	{
		if (mode > ATA_UDMA2 &&	pci_read (pci, 0x48, 1) & (adp->ad_channel->ch_unit ? 0x20 : 0x10))
		{
			mode = ATA_UDMA2;
			printf ("ata[%d,%d]: DMA limitado a %s\n", ATA_DEVNO, ata_dmamode_to_str (mode));
		}
	}

	if (ata_command (adp, ATA_C_SETFEATURES, 0, mode, ATA_C_F_SETXFER, 0))
		return;

	switch (idp->ch_cfg1)
	{
	    case SIS133NEW:
	    {
		ulong timings[] = 
		{
			0x28269008, 0x0c266008, 0x04263008, 0x0c0a3008, 0x05093008,
			0x22196008, 0x0c0a3008, 0x05093008, 0x050939fc, 0x050936ac,
			0x0509347c, 0x0509325c, 0x0509323c, 0x0509322c, 0x0509321c
		};
		ulong reg;

		reg = (pci_read (pci, 0x57, 1) & 0x40?0x70 : 0x40) + (devno << 2);

		pci_write (pci, reg, timings[ata_mode2idx (mode)], 4);
		break;
	    }

	    case SIS133OLD:
	    {
		ushort timings[] =
		{
			0x00cb, 0x0067, 0x0044, 0x0033, 0x0031, 0x0044, 0x0033, 0x0031,
		 	0x8f31, 0x8a31, 0x8731, 0x8531, 0x8331, 0x8231, 0x8131
		};
		ushort reg = 0x40 + (devno << 1);

		pci_write (pci, reg, timings[ata_mode2idx (mode)], 2);
		break;
	    }

	    case SIS100NEW:
	    {
		ushort timings[] =
		{
			0x00cb, 0x0067, 0x0044, 0x0033, 0x0031, 0x0044, 0x0033,
			0x0031, 0x8b31, 0x8731, 0x8531, 0x8431, 0x8231, 0x8131
		};
		ushort reg = 0x40 + (devno << 1);

		pci_write (pci, reg, timings[ata_mode2idx (mode)], 2);
		break;
	    }

	    case SIS100OLD:
	    case SIS66:
	    case SIS33:
	    {
		ushort timings[] =
		{
			0x0c0b, 0x0607, 0x0404, 0x0303, 0x0301, 0x0404, 0x0303,
			0x0301, 0xf301, 0xd301, 0xb301, 0xa301, 0x9301, 0x8301
		};
		ushort reg = 0x40 + (devno << 1);

		pci_write (pci, reg, timings[ata_mode2idx (mode)], 2);
		break;
	    }
	}

	adp->ad_transfer_mode = mode;

}	/* end ata_sis_pata_setmode */
#endif
