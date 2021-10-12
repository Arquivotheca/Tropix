/* $XFree86: xc/programs/Xserver/hw/xfree86/scanpci/xf86ScanPci.c,v 1.12 2002/07/15 20:46:04 dawes Exp $ */
/*
 * Display the Subsystem Vendor Id and Subsystem Id in order to identify
 * the cards installed in this computer
 *
 * Copyright 1995-2002 by The XFree86 Project, Inc.
 *
 * A lot of this comes from Robin Cutshaw's scanpci
 *
 */


/*
 * This file is used to build both the scanpci and pcidata modules.
 * The interfaces have changed compared with XFree86 4.2.0 and earlier.
 * The data is no longer exported directly.  Lookup functions are provided.
 * This means that the data format can change in the future without affecting
 * the exported interfaces.
 *
 * The namespaces for pcidata and scanpci clash, so both modules can't be
 * loaded at the same time.  The X server should only load the scanpci module
 * when run with the '-scanpci' flag.  The main difference between the
 * two modules is size.  pcidata only holds the subset of data that is
 * "interesting" to the X server.  "Interesting" is determined by the
 * PCI_VENDOR_* defines in ../common/xf86PciInfo.h.
 */


/* XXX This is including a lot of stuff that modules should not include! */

#include "X.h"
#include "os.h"
#include "xf86.h"
#include "xf86Priv.h"
#include "xf86Pci.h"
#include "xf86_OSproc.h"

#ifndef IN_MODULE
#include <ctype.h>
#include <stdlib.h>
#else
#include <xf86_ansic.h>
#endif

#ifndef PCIDATA
#define VENDOR_INCLUDE_NONVIDEO
#endif
#define INIT_SUBSYS_INFO
#define INIT_VENDOR_SUBSYS_INFO

#ifdef	TROPIX
#define	static
#endif	TROPIX

#include "xf86PciStr.h"
#include "xf86PciIds.h"
#include "xf86ScanPci.h"

#ifdef	TROPIX
void
ScanPciDisplayPCICardInfo(int verbosity)
{
    pciConfigPtr pcrp, *pcrpp;
    int i;

    xf86EnableIO();
    pcrpp = xf86scanpci(0);

    if (pcrpp == NULL) {
        xf86MsgVerb(X_NONE,0,"No PCI info available\n");
	return;
    }
    xf86MsgVerb(X_NONE,0,"Probing for PCI devices (Bus:Device:Function)\n\n");
    for (i = 0; (pcrp = pcrpp[i]); i++) {
	const char *svendorname = NULL, *subsysname = NULL;
	const char *vendorname = NULL, *devicename = NULL;
	Bool noCard = FALSE;
	const char *prefix1 = "", *prefix2 = "";

	xf86MsgVerb(X_NONE, -verbosity, "(%d:%d:%d) ",
		    pcrp->busnum, pcrp->devnum, pcrp->funcnum);

	/*
	 * Lookup as much as we can about the device.
	 */
	if (pcrp->pci_subsys_vendor || pcrp->pci_subsys_card) {
	    ScanPciFindPciNamesByDevice(pcrp->pci_vendor, pcrp->pci_device,
				     NOVENDOR, NOSUBSYS,
				     &vendorname, &devicename, NULL, NULL);
	} else {
	    ScanPciFindPciNamesByDevice(pcrp->pci_vendor, pcrp->pci_device,
				     pcrp->pci_subsys_vendor,
				     pcrp->pci_subsys_card,
				     &vendorname, &devicename,
				     &svendorname, &subsysname);
	}

	if (svendorname)
	    xf86MsgVerb(X_NONE, -verbosity, "%s ", svendorname);
	if (subsysname)
	    xf86MsgVerb(X_NONE, -verbosity, "%s ", subsysname);
	if (svendorname && !subsysname) {
	    if (pcrp->pci_subsys_card && pcrp->pci_subsys_card != NOSUBSYS) {
		xf86MsgVerb(X_NONE, -verbosity, "unknown card (0x%04x) ",
			    pcrp->pci_subsys_card);
	    } else {
		xf86MsgVerb(X_NONE, -verbosity, "card ");
	    }
	}
	if (!svendorname && !subsysname) {
	    /*
	     * We didn't find a text representation of the information 
	     * about the card.
	     */
	    if (pcrp->pci_subsys_vendor || pcrp->pci_subsys_card) {
		/*
		 * If there was information and we just couldn't interpret
		 * it, print it out as unknown, anyway.
		 */
		xf86MsgVerb(X_NONE, -verbosity,
			    "unknown card (0x%04x/0x%04x) ",
			    pcrp->pci_subsys_vendor, pcrp->pci_subsys_card);
	    } else
		noCard = TRUE;
	}
	if (!noCard) {
	    prefix1 = "using a ";
	    prefix2 = "using an ";
	}
	if (vendorname && devicename) {
	    xf86MsgVerb(X_NONE, -verbosity,"%s%s %s\n", prefix1, vendorname,
			devicename);
	} else if (vendorname) {
	    xf86MsgVerb(X_NONE, -verbosity,
			"%sunknown chip (DeviceId 0x%04x) from %s\n",
			prefix2, pcrp->pci_device, vendorname);
	} else {
	    xf86MsgVerb(X_NONE, -verbosity,
			"%sunknown chipset(0x%04x/0x%04x)\n",
			prefix2, pcrp->pci_vendor, pcrp->pci_device);
	}
    }
}
#endif	TROPIX
