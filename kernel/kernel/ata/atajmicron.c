/*
 ****************************************************************
 *								*
 *			atajmicron.c				*
 *								*
 *	Rotinas auxiliares para controladores JMICRON		*
 *								*
 *	Versão	4.9.0, de 26.04.08				*
 *		4.9.0, de 07.05.08				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2008 NCE/UFRJ - tecle "man licença"	*
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
const ATA_CHIP_ID jmicron_ids[] =
{

	{ ATA_JMB360,		0,	1,	0,	ATA_SA300,	"JMB360"	},
	{ ATA_JMB361,		0,	1,	1,	ATA_SA300,	"JMB361"	},
    	{ ATA_JMB363,		0,	2,	1,	ATA_SA300,	"JMB363"	},
    	{ ATA_JMB365,		0,	1,	2,	ATA_SA300,	"JMB365"	},
    	{ ATA_JMB366,		0,	2,	2,	ATA_SA300,	"JMB366"	},
    	{ ATA_JMB368,		0,	0,	1,	ATA_UDMA6,	"JMB368"	},
	{ 0 }
};

/*
 ******	Protótipos de funções ***********************************
 */
int		ata_jmicron_chipinit (PCIDATA *pci);

int		ata_jmicron_allocate (PCIDATA *, ATA_CHANNEL *);
int		ata_jmicron_reset (PCIDATA *, ATA_CHANNEL *);
void		ata_jmicron_setmode (PCIDATA *, ATA_DEVICE *, int);

/*
 ****************************************************************
 *	Reconhecimento						*
 ****************************************************************
 */
const ATA_CHIP_ID *
ata_jmicron_ident (PCIDATA *pci, ulong chipset_id)
{
	const ATA_CHIP_ID	*idp;
	ATA_PCI_CTLR		*ctlrp = pci->pci_dev_info_ptr;

	/*
	 *	Prólogo
	 */
	ctlrp->ac_chip_nm  = "JMICRON";
	ctlrp->ac_chipinit = ata_jmicron_chipinit;

	/*
	 *	Procura na tabela
	 */
	for (idp = jmicron_ids; /* abaixo */; idp++)
	{
		if (idp->ch_id == 0)
			{ return (ctlrp->ac_idp = NULL); }

		if (chipset_id == idp->ch_id /* && rev >= idp->ch_rev */)
			break;
	}

	return (ctlrp->ac_idp = idp);

}	/* ata_jmicron_ident */

/*
 ****************************************************************
 *	Inicialização						*
 ****************************************************************
 */
int
ata_jmicron_chipinit (PCIDATA *pci)
{
 	ATA_PCI_CTLR		*ctlrp = pci->pci_dev_info_ptr;
	const ATA_CHIP_ID	*idp = ctlrp->ac_idp;

	/*
	 *	Supondo que NÃO é AHCI !
	 *
	 *	do we have multiple PCI functions ?
	 */
	if (pci_read (pci, 0xDF, 1) & 0x40)
	{
		ctlrp->ac_allocate	= ata_pci_allocate;
		ctlrp->ac_reset		= ata_generic_reset;
#ifndef	BOOT
		ctlrp->ac_setmode	= ata_jmicron_setmode;
#endif	BOOT
		ctlrp->ac_nchannels	= idp->ch_cfg2;
	}
	else
	{
		/* set controller configuration to a combined setup we support */

		pci_write (pci, 0x40, 0x80C0A131, 4);
		pci_write (pci, 0x80, 0x01200000, 4);

		ctlrp->ac_allocate	= ata_pci_allocate;
		ctlrp->ac_reset		= ata_generic_reset;
#ifndef	BOOT
		ctlrp->ac_setmode 	= ata_jmicron_setmode;
#endif	BOOT

		/* set the number of HW channels */ 

		ctlrp->ac_nchannels = idp->ch_cfg1 + idp->ch_cfg2;
	}

	return (0);

}	/* end ata_jmicron_chipinit */

#ifndef	BOOT
/*
 ****************************************************************
 *	Inicializa o DMA para os Controladores PATA		*
 ****************************************************************
 */
void
ata_jmicron_setmode (PCIDATA *pci, ATA_DEVICE *adp, int mode)
{
 	ATA_PCI_CTLR		*ctlrp = pci->pci_dev_info_ptr;
	const ATA_CHIP_ID	*idp = ctlrp->ac_idp;

	if (idp->ch_max_dma <= ATA_UDMA6)
	{
		if (pci_read (pci, 0x40, 1) & 0x08)
			mode = ata_limit_mode (adp, mode, ATA_UDMA2);
		else
			mode = ata_limit_mode (adp, mode, ATA_UDMA6);

		adp->ad_udma_mode = mode;
	}
	else
	{
		ata_sata_setmode (pci, adp, mode);
	}

}	/* end ata_jmicron_setmode */
#endif	BOOT
