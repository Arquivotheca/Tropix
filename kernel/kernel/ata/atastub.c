/*
 ****************************************************************
 *								*
 *			atastub.c				*
 *								*
 *	Modelo
 *								*
 *	Versão	4.9.0, de 09.04.07				*
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
const ATA_CHIP_ID stub_ids[] =
{
	{ 0 }
};

int		ata_stub_chipinit (PCIDATA *);
int		ata_stub_allocate (PCIDATA *, ATA_CHANNEL *);
int		ata_stub_reset (PCIDATA *, ATA_CHANNEL *);
void		ata_stub_pata_setmode (PCIDATA *, ATA_DEVICE *, int);
void		ata_stub_sata_setmode (PCIDATA *, ATA_DEVICE *, int);

/*
 ****************************************************************
 *	Reconhecimento						*
 ****************************************************************
 */
const ATA_CHIP_ID *
ata_stub_ident (PCIDATA *pci, ulong chipset_id)
{
	const ATA_CHIP_ID	*idp;
	ATA_PCI_CTLR		*ctlrp = pci->pci_dev_info_ptr;

	ctlrp->ac_chip_nm  = "Stub";
	ctlrp->ac_chipinit = ata_stub_chipinit;

	for (idp = stub_ids; idp->ch_id != 0; idp++)
	{
		if (chipset_id == idp->ch_id /* && rev >= idp->ch_rev */)
			return (ctlrp->ac_idp = idp);
	}

	return (ctlrp->ac_idp = NULL);

}	/* ata_stub_ident */

/*
 ****************************************************************
 *	Inicialização						*
 ****************************************************************
 */
int
ata_stub_chipinit (PCIDATA *pci)
{
 	ATA_PCI_CTLR		*ctlrp = pci->pci_dev_info_ptr;
#if (0)	/*******************************************************/
	const ATA_CHIP_ID	*idp = ctlrp->ac_idp;
#endif	/*******************************************************/

	/* Define as funções de inicialização */

	ctlrp->ac_allocate	= ata_stub_allocate;
	ctlrp->ac_reset		= NULL;
	ctlrp->ac_setmode		= NULL;

	return (0);

}	/* end ata_stub_chipinit */

/*
 ****************************************************************
 *	Aloca as portas						*
 ****************************************************************
 */
int
ata_stub_allocate (PCIDATA *pci, ATA_CHANNEL *chp)
{
	ata_pci_allocate (pci, chp);

#if (0)	/*******************************************************/
	chp->ch_flags |= ATA_ALWAYS_DMASTAT;
#endif	/*******************************************************/

	return (0);

}	/* end ata_stub_allocate */

/*
 ****************************************************************
 *	"Reset"							*
 ****************************************************************
 */
int
ata_stub_reset (PCIDATA *pci, ATA_CHANNEL *chp)
{
#if (0)	/*******************************************************/
 	ATA_PCI_CTLR		*ctlrp = pci->pci_dev_info_ptr;
	const ATA_CHIP_ID	*idp = ctlrp->ac_idp;
#endif	/*******************************************************/

	return (0);

}	/* end ata_stub_reset */

#ifndef	BOOT
/*
 ****************************************************************
 *	Inicializa o DMA para os Controladores PATA		*
 ****************************************************************
 */
void
ata_stub_pata_setmode (PCIDATA *pci, ATA_DEVICE *adp, int mode)
{
#if (0)	/*******************************************************/
 	ATA_PCI_CTLR		*ctlrp = pci->pci_dev_info_ptr;
	const ATA_CHIP_ID	*idp = ctlrp->ac_idp;
#endif	/*******************************************************/

	if (ata_command (adp, ATA_C_SETFEATURES, 0, mode, ATA_C_F_SETXFER, 0))
		return;

	adp->ad_transfer_mode = mode;

}	/* end ata_stub_new_setmode */

/*
 ****************************************************************
 *	Inicializa o DMA para os Controladores SATA		*
 ****************************************************************
 */
void
ata_stub_sata_setmode (PCIDATA *pci, ATA_DEVICE *adp, int mode)
{
	ata_sata_setmode (pci, adp, mode);

}	/* end ata_stub_sata_setmode */
#endif
