/*
 ****************************************************************
 *								*
 *			ataintel.c				*
 *								*
 *	Rotinas auxiliares para controladores INTEL		*
 *								*
 *	Versão	4.9.0, de 21.04.07				*
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
const ATA_CHIP_ID intel_ids[] =
{
	{ ATA_I82371FB,		0,	0,	0,	ATA_WDMA2,	"PIIX"		},
	{ ATA_I82371SB,		0,	0,	0,	ATA_WDMA2,	"PIIX3"		},
	{ ATA_I82371AB,		0,	0,	0,	ATA_UDMA2,	"PIIX4"		},
	{ ATA_I82443MX,		0,	0,	0,	ATA_UDMA2,	"PIIX4"		},
	{ ATA_I82451NX,		0,	0,	0,	ATA_UDMA2,	"PIIX4"		},
	{ ATA_I82801AB,		0,	0,	0,	ATA_UDMA2,	"ICH0"		},
	{ ATA_I82801AA,		0,	0,	0,	ATA_UDMA4,	"ICH"		},
	{ ATA_I82372FB,		0,	0,	0,	ATA_UDMA4,	"ICH"		},
	{ ATA_I82801BA,		0,	0,	0,	ATA_UDMA5,	"ICH2"		},
	{ ATA_I82801BA_1,	0,	0,	0,	ATA_UDMA5,	"ICH2"		},
	{ ATA_I82801CA,		0,	0,	0,	ATA_UDMA5,	"ICH3"		},
	{ ATA_I82801CA_1,	0,	0,	0,	ATA_UDMA5,	"ICH3"		},
	{ ATA_I82801DB,		0,	0,	0,	ATA_UDMA5,	"ICH4"		},
	{ ATA_I82801DB_1,	0,	0,	0,	ATA_UDMA5,	"ICH4"		},
	{ ATA_I82801EB,		0,	0,	0,	ATA_UDMA5,	"ICH5"		},
	{ ATA_I82801EB_S1,	0,	0,	0,	ATA_SA150,	"ICH5"		},
	{ ATA_I82801EB_R1,	0,	0,	0,	ATA_SA150,	"ICH5"		},
	{ ATA_I6300ESB,		0,	0,	0,	ATA_UDMA5,	"6300ESB"	},
	{ ATA_I6300ESB_S1,	0,	0,	0,	ATA_SA150,	"6300ESB"	},
	{ ATA_I6300ESB_R1,	0,	0,	0,	ATA_SA150,	"6300ESB"	},
	{ ATA_I82801FB,		0,	0,	0,	ATA_UDMA5,	"ICH6"		},
	{ ATA_I82801FB_S1,	0,	AHCI,	0,	ATA_SA150,	"ICH6"		},
	{ ATA_I82801FB_R1,	0,	AHCI,	0,	ATA_SA150,	"ICH6"		},
	{ ATA_I82801FBM,	0,	AHCI,	0,	ATA_SA150,	"ICH6M"		},
	{ ATA_I82801GB,		0,	0,	0,	ATA_UDMA5,	"ICH7"		},
	{ ATA_I82801GB_S1,	0,	AHCI,	0,	ATA_SA300,	"ICH7"		},
	{ ATA_I82801GB_R1,	0,	AHCI,	0,	ATA_SA300,	"ICH7"		},
	{ ATA_I82801GB_AH,	0,	AHCI,	0,	ATA_SA300,	"ICH7"		},
	{ ATA_I82801GBM_S1,	0,	AHCI,	0,	ATA_SA300,	"ICH7M"		},
	{ ATA_I82801GBM_R1,	0,	AHCI,	0,	ATA_SA300,	"ICH7M"		},
	{ ATA_I82801GBM_AH,	0,	AHCI,	0,	ATA_SA300,	"ICH7M"		},
	{ ATA_I63XXESB2,	0,	0,	0,	ATA_UDMA5,	"63XXESB2"	},
	{ ATA_I63XXESB2_S1,	0,	AHCI,	0,	ATA_SA300,	"63XXESB2"	},
	{ ATA_I63XXESB2_S2,	0,	AHCI,	0,	ATA_SA300,	"63XXESB2"	},
	{ ATA_I63XXESB2_R1,	0,	AHCI,	0,	ATA_SA300,	"63XXESB2"	},
	{ ATA_I63XXESB2_R2,	0,	AHCI,	0,	ATA_SA300,	"63XXESB2"	},
	{ ATA_I82801HB_S1,	0,	AHCI,	0,	ATA_SA300,	"ICH8"		},
	{ ATA_I82801HB_S2,	0,	AHCI,	0,	ATA_SA300,	"ICH8"		},
	{ ATA_I82801HB_R1,	0,	AHCI,	0,	ATA_SA300,	"ICH8"		},
	{ ATA_I82801HB_AH4,	0,	AHCI,	0,	ATA_SA300,	"ICH8"		},
	{ ATA_I82801HB_AH6,	0,	AHCI,	0,	ATA_SA300,	"ICH8"		},
	{ ATA_I82801HBM_S1,	0,	AHCI,	0,	ATA_SA300,	"ICH8M"		},
	{ ATA_I82801HBM_S2,	0,	AHCI,	0,	ATA_SA300,	"ICH8M"		},
	{ ATA_I82801IB_S1,	0,	AHCI,	0x00,	ATA_SA300,	"ICH9"		},
	{ ATA_I82801IB_S2,	0,	AHCI,	0x00,	ATA_SA300,	"ICH9"		},
	{ ATA_I82801IB_AH2,	0,	AHCI,	0x00,	ATA_SA300,	"ICH9"		},
	{ ATA_I82801IB_AH4,	0,	AHCI,	0x00,	ATA_SA300,	"ICH9"		},
	{ ATA_I82801IB_AH6,	0,	AHCI,	0x00,	ATA_SA300,	"ICH9"		},
	{ ATA_I31244,		0,	0,	0,	ATA_SA150,	"31244"		},
	{ 0 }
};

/*
 ******	Protótipos de funções ***********************************
 */
int		ata_intel_allocate (PCIDATA *, ATA_CHANNEL *);
int		ata_intel_reset (PCIDATA *, ATA_CHANNEL *);
void		ata_intel_old_setmode (PCIDATA *, ATA_DEVICE *, int);
void		ata_intel_pata_setmode (PCIDATA *, ATA_DEVICE *, int);

/*
 ****************************************************************
 *	Reconhecimento						*
 ****************************************************************
 */
const ATA_CHIP_ID *
ata_intel_ident (PCIDATA *pci, ulong chipset_id)
{
	const ATA_CHIP_ID	*idp;
	ATA_PCI_CTLR		*ctlrp = pci->pci_dev_info_ptr;

	ctlrp->ac_chip_nm  = "INTEL";
	ctlrp->ac_chipinit = ata_intel_chipinit;

	for (idp = intel_ids; idp->ch_id != 0; idp++)
	{
		if (chipset_id == idp->ch_id /* && rev >= idp->ch_rev */)
			return (ctlrp->ac_idp = idp);
	}

	return (ctlrp->ac_idp = NULL);

}	/* ata_intel_ident */

/*
 ****************************************************************
 *	Inicialização						*
 ****************************************************************
 */
int
ata_intel_chipinit (PCIDATA *pci)
{
 	ATA_PCI_CTLR		*ctlrp = pci->pci_dev_info_ptr;
	const ATA_CHIP_ID	*idp = ctlrp->ac_idp;

	/* Define a função de alocação de portas */

	if (idp->ch_id == ATA_I82371FB)
	{
		/* PIIX tem tratamento especial */
#ifndef	BOOT
		ctlrp->ac_setmode = ata_intel_old_setmode;
#endif
	}
	elif (idp->ch_id == ATA_I31244)
	{
		/* O Intel 31244 precisa de cuidados no modo DPA */

		return (-1);
#if (0)	/*******************************************************/
		if (pci_get_subclass (pci) != PCIS_STORAGE_IDE)
		{
			ctlr->r_type2 = SYS_RES_MEMORY;
			ctlr->r_rid2 = PCIR_BAR(0);
			if (!(ctlr->r_res2 = bus_alloc_resource_any(dev, ctlr->r_type2,
							&ctlr->r_rid2,
							RF_ACTIVE)))
				return ENXIO;
			ctlr->channels = 4;
			ctlr->allocate = ata_intel_31244_allocate;
			ctlr->reset = ata_intel_31244_reset;
		}

		setmode = ata_sata_setmode;
#endif	/*******************************************************/
	}
	elif (idp->ch_max_dma < ATA_SA150)
	{
		/* PATA */

		ctlrp->ac_allocate = ata_pci_allocate;
#ifndef	BOOT
		ctlrp->ac_setmode  = ata_intel_pata_setmode;
#endif
	}
	else
	{
		/* SATA - Força o modo "legagy" */

		pci_write (pci, 0x92, pci_read (pci, 0x92, 2) | 0x0F, 2);

		ctlrp->ac_allocate = ata_intel_allocate;
		ctlrp->ac_reset    = ata_intel_reset;

#ifndef	BOOT
#if (0)	/*******************************************************/
		/* Verifica se tem AHCI */

		if
		(	idp->ch_cfg1 == AHCI && (pci_read (pci, 0x90, 1) & 0xC0) &&
			pci->pci_addr[5] != 0 && (pci->pci_map_mask & (1 << 5)) == 0
		)
			return (ata_ahci_chipinit (pci));
#endif	/*******************************************************/

		ctlrp->ac_setmode = ata_sata_setmode;
#endif
		/* Habilita a interrupção do PCI */

		pci_write (pci, PCIR_COMMAND, pci_read (pci, PCIR_COMMAND, 2) & ~0x0400, 2);
	}

	return (0);

}	/* end ata_intel_chipinit */

/*
 ****************************************************************
 *	Preenche os números das portas				*
 ****************************************************************
 */
int
ata_intel_allocate (PCIDATA *pci, ATA_CHANNEL *chp)
{
	ata_pci_allocate (pci, chp);

	chp->ch_flags |= ATA_ALWAYS_DMASTAT;

	return (0);

}	/* end ata_intel_allocate */

/*
 ****************************************************************
 *	"Reset"							*
 ****************************************************************
 */
int
ata_intel_reset (PCIDATA *pci, ATA_CHANNEL *chp)
{
 	ATA_PCI_CTLR		*ctlrp = pci->pci_dev_info_ptr;
	const ATA_CHIP_ID	*idp = ctlrp->ac_idp;
	int			mask, timeout;

	/* ICH6 & ICH7 in compat mode has 4 SATA ports as master/slave on 2 ch's */

	if (idp->ch_cfg1)
	{
		mask = 0x0005 << chp->ch_unit;
	}
	else
	{
		/* ICH5 in compat mode has SATA ports as master/slave on 1 channel */

		if (pci_read (pci, 0x90, 1) & 0x04)
		{
			mask = 0x0003;
		}
		else
		{
			mask = 0x0001 << chp->ch_unit;
			chp->ch_flags |= ATA_NO_SLAVE;
		}
	}

	pci_write (pci, 0x92, pci_read (pci, 0x92, 2) & ~mask, 2);

	DELAY (10);

	pci_write (pci, 0x92, pci_read (pci, 0x92, 2) | mask, 2);

	/* wait up to 1 sec for "connect well" */

	for (timeout = 0; timeout < 100; timeout++)
	{
		if (((pci_read (pci, 0x92, 2) & (mask << 4)) == (mask << 4)))
		{
			if (read_port (pci->pci_addr[0] + ATA_STATUS) != 0xFF)
				break;
		}

		DELAY (10000);
	}

	return (ata_new_find_devices (chp));

}	/* end ata_intel_reset */

#ifndef	BOOT
/*
 ****************************************************************
 *	Inicializa o DMA para os Controladores PATA		*
 ****************************************************************
 */
void
ata_intel_pata_setmode (PCIDATA *pci, ATA_DEVICE *adp, int mode)
{
 	ATA_PCI_CTLR		*ctlrp = pci->pci_dev_info_ptr;
	const ATA_CHIP_ID	*idp = ctlrp->ac_idp;
	int			devno, index;
	ulong			reg40;
	uchar			reg44, reg48;
	ushort			reg4a, reg54;
	ulong			mask40 = 0, new40 = 0;
	uchar			mask44 = 0, new44 = 0;

	static uchar		timings[] =
	{
		0x00, 0x00, 0x10, 0x21,	0x23, 0x10, 0x21,
		0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23
	};

	devno = (adp->ad_channel->ch_unit << 1) + adp->ad_devno;

	reg40 = pci_read (pci, 0x40, 4);
	reg44 = pci_read (pci, 0x44, 1);
	reg48 = pci_read (pci, 0x48, 1);
	reg4a = pci_read (pci, 0x4a, 2);
	reg54 = pci_read (pci, 0x54, 2);

	mode = ata_limit_mode (adp, mode, idp->ch_max_dma);

	if (mode > ATA_UDMA2 && !(reg54 & (0x10 << devno)))
	{
		mode = ATA_UDMA2;
		printf ("ata[%d,%d]: DMA limitado a %s\n", ATA_DEVNO, ata_dmamode_to_str (mode));
	}

	if (ata_command (adp, ATA_C_SETFEATURES, 0, mode, ATA_C_F_SETXFER, 0))
		return;

	if (mode >= ATA_UDMA0)
	{
		pci_write (pci, 0x48, reg48 | (0x0001 << devno), 2);
		pci_write (pci, 0x4A, (reg4a & ~(0x3 << (devno << 2))) | (0x01 + !(mode & 0x01)), 2);

		index = mode - ATA_UDMA0 + 8;
	}
	else
	{
		pci_write (pci, 0x48, reg48 & ~(0x0001 << devno), 2);
		pci_write (pci, 0x4A, (reg4a & ~(0x3 << (devno << 2))), 2);

		index = mode - ATA_WDMA0 + 5;
	}

	if (mode >= ATA_UDMA2)
		pci_write (pci, 0x54, reg54 |  (0x00001 << devno), 2);
	else
		pci_write (pci, 0x54, reg54 & ~(0x00001 << devno), 2);

	if (mode >= ATA_UDMA5)
		pci_write (pci, 0x54, reg54 |  (0x10000 << devno), 2);
	else 
		pci_write (pci, 0x54, reg54 & ~(0x10000 << devno), 2);

	reg40 &= ~0x00FF00FF; 	reg40 |= 0x40774077;

	if (adp->ad_devno == 0)
	{
		mask40 = 0x3300;
		new40  = timings[index] << 8;
	}
	else
	{
		mask44 = 0x0F;
		new44  = ((timings[index] & 0x30) >> 2) | (timings[index] & 0x03);
	}

	if (adp->ad_channel->ch_unit == 1)
		{ mask40 <<= 16; new40 <<= 16; mask44 <<= 4; new44 <<= 4; }

	pci_write (pci, 0x40, (reg40 & ~mask40) | new40, 4);
	pci_write (pci, 0x44, (reg44 & ~mask44) | new44, 1);

	adp->ad_transfer_mode = mode;

}	/* end ata_intel_pata_setmode */

/*
 ****************************************************************
 *	Inicializa o DMA para o PIIX				*
 ****************************************************************
 */
void
ata_intel_old_setmode (PCIDATA *pci, ATA_DEVICE *adp, int mode)
{
	adp->ad_transfer_mode = ATA_PIO4;

}	/* end ata_intel_old_setmode */
#endif	BOOT
