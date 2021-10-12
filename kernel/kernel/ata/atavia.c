/*
 ****************************************************************
 *								*
 *			atavia.c				*
 *								*
 *	Rotinas auxiliares para controladores Via		*
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

/*
 ****************************************************************
 *	Informações sobre os Controladores			*
 ****************************************************************
 */
const ATA_CHIP_ID via_ids[] =
{
	{ ATA_VIA82C586,	0x02,		VIA33,  0x00,    ATA_UDMA2, "82C586B"	},
	{ ATA_VIA82C586,	0x00,		VIA33,  0x00,    ATA_WDMA2, "82C586"	},
	{ ATA_VIA82C596,	0x12,		VIA66,  VIACLK,  ATA_UDMA4, "82C596B"	},
	{ ATA_VIA82C596,	0x00,		VIA33,  0x00,    ATA_UDMA2, "82C596"	},
	{ ATA_VIA82C686,	0x40,		VIA100, VIABUG,  ATA_UDMA5, "82C686B"	},
	{ ATA_VIA82C686,	0x10,		VIA66,  VIACLK,  ATA_UDMA4, "82C686A"	},
	{ ATA_VIA82C686,	0x00,		VIA33,  0x00,    ATA_UDMA2, "82C686"	},
	{ ATA_VIA8231,		0x00,		VIA100, VIABUG,  ATA_UDMA5, "8231"	},
	{ ATA_VIA8233,		0x00,		VIA100, 0x00,    ATA_UDMA5, "8233"	},
	{ ATA_VIA8233C,		0x00,		VIA100, 0x00,    ATA_UDMA5, "8233C"	},
	{ ATA_VIA8233A,		0x00,		VIA133, 0x00,    ATA_UDMA6, "8233A"	},
	{ ATA_VIA8235,		0x00,		VIA133, 0x00,    ATA_UDMA6, "8235"	},
	{ ATA_VIA8237,		0x00,		VIA133, 0x00,    ATA_UDMA6, "8237"	},
	{ ATA_VIA8237A,		0x00,		VIA133, 0x00,    ATA_UDMA6, "8237A"	},
	{ ATA_VIA8251,		0x00,		VIA133, 0x00,    ATA_UDMA6, "8251"	},
	{ 0 }
};

const ATA_CHIP_ID new_via_ids[] =
{
	{ ATA_VIA6410,		0x00,		0,      0x00,    ATA_UDMA6, "6410"	},
	{ ATA_VIA6420,		0x00,		7,      0x00,    ATA_SA150, "6420"	},
	{ ATA_VIA6421,		0x00,		6,      VIABAR,  ATA_SA150, "6421"	},
	{ ATA_VIA8237A,		0x00,		0,      0x00,    ATA_SA150, "8237A"	},
	{ ATA_VIA8251,		0x00,		0,      VIAAHCI, ATA_SA300, "8251"	},
	{ 0 }
};

void	ata_via_family_setmode (PCIDATA *, ATA_DEVICE *, int);

/*
 ****************************************************************
 *	Reconhecimento						*
 ****************************************************************
 */
const ATA_CHIP_ID *
ata_via_ident (PCIDATA *pci, ulong chipset_id)
{
	const ATA_CHIP_ID	*idp;
	const PCIDATA		*parent = pci->pci_parent;
	ATA_PCI_CTLR		*ctlrp = pci->pci_dev_info_ptr;

	ctlrp->ac_chip_nm  = "VIA";
	ctlrp->ac_chipinit = ata_via_chipinit;

	if (chipset_id == ATA_VIA82C571)
	{
		ulong	parent_id = (parent->pci_device << 16) | parent->pci_vendor;
		uchar	rev  = parent->pci_revid;

		for (idp = via_ids; idp->ch_id != 0; idp++)
		{
			if (parent_id == idp->ch_id && rev >= idp->ch_rev)
				return (ctlrp->ac_idp = idp);
		}
	}
	else
	{
		uchar	rev  = pci->pci_revid;

		for (idp = new_via_ids; idp->ch_id != 0; idp++)
		{
			if (chipset_id == idp->ch_id && rev >= idp->ch_rev)
				return (ctlrp->ac_idp = idp);
		}
	}

	return (ctlrp->ac_idp = NULL);

}	/* end ata_via_ident */

/*
 ****************************************************************
 *	Inicialização						*
 ****************************************************************
 */
int
ata_via_chipinit (PCIDATA *pci)
{
 	ATA_PCI_CTLR		*ctlrp = pci->pci_dev_info_ptr;
	const ATA_CHIP_ID	*idp = ctlrp->ac_idp;

	if (idp->ch_max_dma >= ATA_SA150)
	{
		return (-1);
#if (0)	/*******************************************************/
		if (ctlr->chip->cfg2 == VIAAHCI)
		{
			ctlr->r_type2 = SYS_RES_MEMORY;
			ctlr->r_rid2 = PCIR_BAR(5);
			if ((ctlr->r_res2 = bus_alloc_resource_any(pci, ctlr->r_type2,
							   &ctlr->r_rid2,
							   RF_ACTIVE))) {
				return ata_ahci_chipinit(pci);
			} 
		}

		ctlr->r_type2 = SYS_RES_IOPORT;
		ctlr->r_rid2 = PCIR_BAR(5);
		if ((ctlr->r_res2 = bus_alloc_resource_any(pci, ctlr->r_type2, &ctlr->r_rid2, RF_ACTIVE)))
		{
			ctlr->allocate = ata_via_allocate;
			ctlr->reset = ata_via_reset;

			/* enable PCI interrupt */
			pci_write (pci, PCIR_COMMAND,
				 pci_read (pci, PCIR_COMMAND, 2) & ~0x0400,2);
		}

		ctlrp->ac_setmode = ata_sata_setmode;

		return (0);
#endif	/*******************************************************/
	}

	/* ATA-66 para 82C686a e 82C596b */

	if (idp->ch_cfg2 & VIACLK)
		pci_write (pci, 0x50, 0x030b030b, 4);	   

	/* Corrige o erro da ponte sul */

	if (idp->ch_cfg2 & VIABUG)
	{
		return (-1);
#if (0)	/*******************************************************/
		ata_via_southbridge_fixup (pci);
#endif	/*******************************************************/
	}

	/* Configura o FIFO */

	pci_write (pci, 0x43, (pci_read (pci, 0x43, 1) & 0x90) | 0x2a, 1);

	/* set status register read retry */

	pci_write (pci, 0x44, pci_read (pci, 0x44, 1) | 0x08, 1);

	/* Configura o DMA */

	pci_write (pci, 0x46, (pci_read (pci, 0x46, 1) & 0x0c) | 0xf0, 1);

	/* Configura o tamanho do setor */

	pci_write (pci, 0x60, BLSZ, 2);
	pci_write (pci, 0x68, BLSZ, 2);

#ifndef	BOOT
	ctlrp->ac_setmode = ata_via_family_setmode;
#endif

	return (0);

}	/* end ata_via_chipinit */

#ifndef BOOT
/*
 ****************************************************************
 *	Inicializa o DMA para os Controladores PATA		*
 ****************************************************************
 */
void
ata_via_family_setmode (PCIDATA *pci, ATA_DEVICE *adp, int mode)
{
 	ATA_PCI_CTLR		*ctlrp = pci->pci_dev_info_ptr;
	const ATA_CHIP_ID	*idp = ctlrp->ac_idp;

	uchar	timings[] = 
	{
		0xa8, 0x65, 0x42, 0x22, 0x20, 0x42, 0x22,
		0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20
	};

	int modes[][7] =
	{
		{ 0xc2, 0xc1, 0xc0, 0x00, 0x00, 0x00, 0x00 },   /* VIA ATA33 */
		{ 0xee, 0xec, 0xea, 0xe9, 0xe8, 0x00, 0x00 },   /* VIA ATA66 */
		{ 0xf7, 0xf6, 0xf4, 0xf2, 0xf1, 0xf0, 0x00 },   /* VIA ATA100 */
		{ 0xf7, 0xf7, 0xf6, 0xf4, 0xf2, 0xf1, 0xf0 },   /* VIA ATA133 */
		{ 0xc2, 0xc1, 0xc0, 0xc4, 0xc5, 0xc6, 0xc7 }    /* AMD/nVIDIA */
	};

	int devno = (adp->ad_channel->ch_unit << 1) + adp->ad_devno;
	int reg   = 0x53 - devno;

	mode = ata_limit_mode (adp, mode, idp->ch_max_dma);

	if (idp->ch_cfg2 & AMDCABLE)
	{
		if (mode > ATA_UDMA2 && !(pci_read (pci, 0x42, 1) & (1 << devno)))
		{
			mode = ATA_UDMA2;
			printf ("ata[%d,%d]: DMA limitado a %s\n", ATA_DEVNO, ata_dmamode_to_str (mode));
		}
	}
	else 
	{
		mode = ata_check_80pin (adp, mode);
	}

	if (idp->ch_cfg2 & NVIDIA)
		reg += 0x10;

	if (idp->ch_cfg1 != VIA133)
		pci_write (pci, reg - 0x08, timings[ata_mode2idx (mode)], 1);

	if (ata_command (adp, ATA_C_SETFEATURES, 0, mode, ATA_C_F_SETXFER, 0))
		return;

	if (mode >= ATA_UDMA0)
		pci_write (pci, reg, modes[idp->ch_cfg1][mode & ATA_MODE_MASK], 1);
	else
		pci_write (pci, reg, 0x8B, 1);

	adp->ad_transfer_mode = mode;

}	/* end ata_via_family_setmode */
#endif
