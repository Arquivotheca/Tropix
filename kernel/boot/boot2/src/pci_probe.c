/*
 ****************************************************************
 *								*
 *			pci_probe.c				*
 *								*
 *	Interface "de mentira" para ETHERNET e MODEM PCI	*
 *								*
 *	Versão	3.1.0, de 10.08.98				*
 *		4.9.0, de 24.04.07				*
 *								*
 *	Módulo: Boot2						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2007 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include <types.h>
#include <pci.h>

/*
 ******	Definições globais **************************************
 */
#define NOSTR	(char *)0

/*
 ****************************************************************
 *	Identifica EDs PCI					*
 ****************************************************************
 */
char *
pci_ed_probe (PCIDATA *tag, ulong type)
{
	switch (type)
	{
	    case 0x802910EC:
		return ("NE2000 PCI Ethernet (RealTek 8029)");

	    case 0x50004A14:
		return ("NE2000 PCI Ethernet (NetVin 5000)");

	    case 0x09401050:
		return ("NE2000 PCI Ethernet (ProLAN)");

	    case 0x140111F6:
		return ("NE2000 PCI Ethernet (Compex)");

	    case 0x30008E2E:
		return ("NE2000 PCI Ethernet (KTI)");

	    default:
		break;

	}	/* end switch (type) */

	return (NOSTR);

}	/* end pci_ed_probe */

/*
 ****************************************************************
 *	Anexa EDs PCI						*
 ****************************************************************
 */
void
pci_ed_attach (PCIDATA * config_id, int unit)
{

}	/* end pci_ed_attach */

/*
 ****************************************************************
 *	Identifica RTLs PCI					*
 ****************************************************************
 */
char *
pci_rtl_probe (PCIDATA *tag, ulong type)
{
	switch (type)
	{
	    case 0x812910EC:
		return ("RealTek RTL8129 Fast ETHERNET");

	    case 0x813910EC:
		return ("RealTek RTL8139 Fast ETHERNET");

	    default:
		break;

	}	/* end switch (type) */

	return (NOSTR);

}	/* end pci_rtl_probe */

/*
 ****************************************************************
 *	Anexa RTLs PCI						*
 ****************************************************************
 */
void
pci_rtl_attach (PCIDATA * config_id, int unit)
{

}	/* end pci_rtl_attach */

/*
 ****************************************************************
 *	Identifica SIOs PCI					*
 ****************************************************************
 */
char *
pci_sio_probe (PCIDATA *tag, ulong type)
{
	switch (type)
	{
	    case 0x100812B9:
		return ("U.S. Robotics 56K PCI Fax Modem");

	    default:
		break;

	}	/* end switch (type) */

	return (NOSTR);

}	/* end pci_sio_probe */

/*
 ****************************************************************
 *	Função de "attach" para unidades PCI			*
 ****************************************************************
 */
void
pci_sio_attach (PCIDATA * config_id, int unit)
{

}	/* end pci_sio_attach */

/*
 ****************************************************************
 *	Identifica Placas de Som Sound Blaster			*
 ****************************************************************
 */
char *
pci_sb_probe (PCIDATA *tag, ulong type)
{
	return (NOSTR);

}	/* end pci_sb_probe */

/*
 ****************************************************************
 *	Anexa Placas de Som Sound Blaster			*
 ****************************************************************
 */
void
pci_sb_attach (PCIDATA *tag, int unit)
{

}	/* end pci_sb_attach */

/*
 ****************************************************************
 *	Função de "probe" dos controladores USB UHCI		*
 ****************************************************************
 */
#define	PCI_INTERFACE_UHCI	0x00

char *
uhci_probe (PCIDATA *pci, ulong dev_vendor)
{
	if
	(	pci->pci_baseclass != PCIC_SERIALBUS     ||
		pci->pci_subclass  != PCIS_SERIALBUS_USB ||
		pci->pci_progif    != PCI_INTERFACE_UHCI
	)
		return (NOSTR);

	switch (dev_vendor)
	{
		/* Intel -- vendor 0x8086 */

	    case 0x70208086:
		return ("Intel 82371SB (PIIX3) USB controller");

	    case 0x71128086:
		return ("Intel 82371AB/EB (PIIX4) USB controller");

	    case 0x24128086:
		return ("Intel 82801AA (ICH) USB controller");

	    case 0x24228086:
		return ("Intel 82801AB (ICH0) USB controller");

	    case 0x24428086:
		return ("Intel 82801BA/BAM (ICH2) USB controller USB-A");

	    case 0x24448086:
		return ("Intel 82801BA/BAM (ICH2) USB controller USB-B");

	    case 0x24828086:
		return ("Intel 82801CA/CAM (ICH3) USB controller USB-A");

	    case 0x24848086:
		return ("Intel 82801CA/CAM (ICH3) USB controller USB-B");

	    case 0x24878086:
		return ("Intel 82801CA/CAM (ICH3) USB controller USB-C");

	    case 0x24c28086:
		return ("Intel 82801DB (ICH4) USB controller USB-A");

	    case 0x24c48086:
		return ("Intel 82801DB (ICH4) USB controller USB-B");

	    case 0x24c78086:
		return ("Intel 82801DB (ICH4) USB controller USB-C");

	    case 0x719a8086:
		return ("Intel 82443MX USB controller");

	    case 0x76028086:
		return ("Intel 82372FB/82468GX USB controller");

		/* VIA Technologies -- vendor 0x1106 (0x1107 on the Apollo Master) */

	    case 0x30381106:
		return ("VIA 83C572 USB controller");

		/* AcerLabs -- vendor 0x10b9 */

	    case 0x523710b9:
		return ("AcerLabs M5237 (Aladdin-V) USB controller");

		/* OPTi -- vendor 0x1045 */

	    case 0xc8611045:
		return ("OPTi 82C861 (FireLink) USB controller");

		/* NEC -- vendor 0x1033 */

	    case 0x00351033:
		return ("NEC uPD 9210 USB controller");

		/* CMD Tech -- vendor 0x1095 */

	    case 0x06701095:
		return ("CMD Tech 670 (USB0670) USB controller");

	    case 0x06731095:
		return ("CMD Tech 673 (USB0673) USB controller");

	    default:
		return ("UHCI (generic) USB controller");
	}

}	/* uhci_probe */

/*
 ****************************************************************
 *	Função de "probe" dos controladores USB OHCI		*
 ****************************************************************
 */
#define PCI_INTERFACE_OHCI	0x10

char *
ohci_probe (PCIDATA *pci, ulong dev_vendor)
{
	if
	(	pci->pci_baseclass != PCIC_SERIALBUS     ||
		pci->pci_subclass  != PCIS_SERIALBUS_USB ||
		pci->pci_progif    != PCI_INTERFACE_OHCI
	)
		return (NOSTR);

	switch (dev_vendor)
	{
	    case 0x523710B9:
		return ("AcerLabs M5237 (Aladdin-V) USB controller");

	    case 0x740C1022:
		return ("AMD-756 USB Controller");

	    case 0x74141022:
		return ("AMD-766 USB Controller");

	    case 0xc8611045:
		return ("OPTi 82C861 (FireLink) USB controller");

	    case 0x00351033:
		return ("NEC uPD 9210 USB controller");

	    case 0x06701095:
		return ("CMD Tech 670 (USB0670) USB controller");

	    case 0x06731095:
		return ("CMD Tech 673 (USB0673) USB controller");

	    case 0x70011039:
		return ("SiS 5571 USB controller");

	    case 0x0019106B:
		return ("Apple KeyLargo USB controller");

	    default:
		return ("OHCI (generic) USB controller");
	}

}	/* end ohci_probe */

/*
 ****************************************************************
 *	Função de "probe" dos controladores USB EHCI		*
 ****************************************************************
 */
#define PCI_INTERFACE_EHCI      0x20

char *
ehci_probe (PCIDATA *pci, ulong dev_vendor)
{
	if
	(	pci->pci_baseclass != PCIC_SERIALBUS     ||
		pci->pci_subclass  != PCIS_SERIALBUS_USB ||
		pci->pci_progif    != PCI_INTERFACE_EHCI
	)
		return (NOSTR);

	switch (dev_vendor)
	{
	    case 0x00E01033:
		return ("NEC uPD 720100 USB 2.0 controller");

	    default:
		return ("EHCI (generic) USB controller");
	}

}	/* ehci_probe */

/*
 ****************************************************************
 *	Anexa controladores USB do tipo UHCI			*
 ****************************************************************
 */
void
uhci_attach (PCIDATA *tag, int unit)
{
}	/* end uhci_attach */

/*
 ****************************************************************
 *	Anexa controladores USB do tipo OHCI			*
 ****************************************************************
 */
void
ohci_attach (PCIDATA *tag, int unit)
{
}	/* end ohci_attach */

/*
 ****************************************************************
 *	Anexa controladores USB do tipo EHCI			*
 ****************************************************************
 */
void
ehci_attach (PCIDATA *tag, int unit)
{
}	/* end ehci_attach */

/*
 ****************************************************************
 *	Identifica Placas de Som Sound Blaster LIVE!		*
 ****************************************************************
 */
char *
pci_live_probe (PCIDATA *tag, ulong type)
{
	switch (type)
	{
	    case 0x00021102:
		return ("Creative SB Live");

	}	/* end switch (type) */

	return (NOSTR);

}	/* end pci_live_probe */

/*
 ****************************************************************
 *	Anexa Placas de Som Sound Blaster LIVE!			*
 ****************************************************************
 */
void
pci_live_attach (PCIDATA *tag, int unit)
{

}	/* end pci_live_attach */

