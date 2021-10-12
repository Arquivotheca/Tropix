/*
 * SBUS bus-specific code.
 *
 * Copyright (C) 2000 Jakub Jelinek (jakub@redhat.com)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * JAKUB JELINEK BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
/* $XFree86: xc/programs/Xserver/hw/xfree86/common/xf86sbusBus.c,v 3.14 2007/03/15 14:38:36 tsi Exp $ */

#include <ctype.h>
#include <stdio.h>
#include <unistd.h>
#include <X11/X.h>
#include "os.h"
#include "xf86.h"
#include "xf86Priv.h"
#include "xf86_OSlib.h"
#include "xf86Resources.h"
#include "xf86cmap.h"

#include "xf86Bus.h"

#include "xf86sbusBus.h"
#include "xf86Sbus.h"

#ifdef sun
/*
 * VIS_GETIDENTIFIER ioctl was added in Solaris 2.3, but don't actually
 * #include the header so this can be built on older releases.
 */
/* #include <sys/visual_io.h> */
# define VIS_GETIDENTIFIER	(('V' << 8) | 0)
# define VIS_MAXNAMELEN		128
struct vis_identifier {
    char name[VIS_MAXNAMELEN];
};
#endif

Bool sbusSlotClaimed = FALSE;

static int xf86nSbusInfo;

static void
#ifdef __OpenBSD__
CheckSbusDevice(const char *device)
#else
CheckSbusDevice(const char *device, int fbNum)
#endif
{
    int fd, i;
    sbusDevicePtr psdp;
#ifdef sun
    struct vis_identifier vid;
#endif
#ifdef __OpenBSD__
    struct wsdisplay_fbinfo fbinfo;
#else
    struct fbgattr fbattr;
#endif

    fd = open(device, O_RDONLY, 0);
    if (fd < 0)
	return;

#ifdef __OpenBSD__
    memset(&fbinfo, 0, sizeof(fbinfo));
#else
    memset(&fbattr, 0, sizeof(fbattr));
#endif

#ifdef sun
    if (ioctl(fd, VIS_GETIDENTIFIER, &vid) >= 0)
    {
	char *promName;

	for (promName = vid.name; *promName; promName++)
	    if (((*promName >= 'a') && (*promName <= 'z')) ||
		((*promName >= '0') && (*promName <= '9')))
		break;

	for (i = 0; sbusDeviceTable[i].devId; i++)
	    if (!strcmp(sbusDeviceTable[i].promName, promName))
		break;
    }
    else
#endif
#ifdef __OpenBSD__
    {
	int fbtype;

	if (ioctl(fd, WSDISPLAYIO_GTYPE, &fbtype) < 0) {
	    close(fd);
	    return;
	}

	if (ioctl(fd, WSDISPLAYIO_GINFO, &fbinfo) < 0) {
	    close(fd);
	    return;
	}

	for (i = 0; sbusDeviceTable[i].devId; i++)
	    if (sbusDeviceTable[i].fbType == fbtype)
		break;
    }
#else
    {
	if (ioctl(fd, FBIOGATTR, &fbattr) < 0) {
	    if (ioctl(fd, FBIOGTYPE, &fbattr.fbtype) < 0) {
		close(fd);
		return;
	    }
	}

	for (i = 0; sbusDeviceTable[i].devId; i++)
	    if (sbusDeviceTable[i].fbType == fbattr.fbtype.fb_type)
		break;
    }
#endif

    close(fd);

    if (!sbusDeviceTable[i].devId)
	return;

    xf86nSbusInfo++;
    xf86SbusInfo = xnfrealloc(xf86SbusInfo, sizeof(psdp) * (xf86nSbusInfo + 1));
    xf86SbusInfo[xf86nSbusInfo] = NULL;
    xf86SbusInfo[xf86nSbusInfo - 1] = psdp = xnfcalloc(sizeof(sbusDevice), 1);
    psdp->devId = sbusDeviceTable[i].devId;
    psdp->device = xnfstrdup(device);
    psdp->descr = sbusDeviceTable[i].descr;
#ifdef __OpenBSD__
    psdp->fbNum = xf86nSbusInfo - 1;
    psdp->width = fbinfo.width;
    psdp->height = fbinfo.height;
#else
    psdp->fbNum = fbNum;
    psdp->width = fbattr.fbtype.fb_width;
    psdp->height = fbattr.fbtype.fb_height;
#endif
    psdp->fd = -1;
}

void
xf86SbusProbe(void)
{
    int useProm = 0;
    char fbDevName[32];
    sbusDevicePtr psdp, *psdpp;

    xf86SbusInfo = xalloc(sizeof(psdp));
    *xf86SbusInfo = NULL;

#ifdef __OpenBSD__
    {
	const char *c1, *c2;

	for (c1 = "CDEFGHIJ"; *c1; c1++) {
	    for (c2 = "0123456789ab"; *c2; c2++) {
		sprintf(fbDevName, "/dev/tty%c%c", *c1, *c2);
		CheckSbusDevice(fbDevName);
	    }
	}
    }
#else
    {
	int i;

	for (i = 0; i < 32; i++) {
	    sprintf(fbDevName, "/dev/fb%d", i);
	    CheckSbusDevice(fbDevName, i);
	}
    }
#endif

    if (sparcPromInit() >= 0) {
	useProm = 1;
	sparcPromAssignNodes();
    }

    for (psdpp = xf86SbusInfo; (psdp = *psdpp); psdpp++) {
	/*
	 * If we can use PROM information and found the PROM node for this
	 * device, we can tell more about the card.
	 */
	if (useProm && psdp->node.node) {
	    char *prop, *promPath;
	    int len, chiprev, vmsize;

	    if (psdp->height <= 0) {
		prop = sparcPromGetProperty(&psdp->node, "height", &len);
		if (prop && (len == 4))
		    psdp->height = *(int *)prop;
	    }

	    if (psdp->width <= 0) {
		prop = sparcPromGetProperty(&psdp->node, "width", &len);
		if (prop && (len == 4))
		    psdp->width = *(int *)prop;
	    }

	    switch (psdp->devId) {
	    case SBUS_DEVICE_MGX:
		prop = sparcPromGetProperty(&psdp->node, "fb_size", &len);
		if (prop && (len == 4) && (*(int *)prop == 0x400000))
		    psdp->descr = "Quantum 3D MGXplus with 4M VRAM";
		break;

	    case SBUS_DEVICE_CG6:
		chiprev = 0;
		vmsize = 0;

		prop = sparcPromGetProperty(&psdp->node, "chiprev", &len);
		if (prop && (len == 4))
		    chiprev = *(int *)prop;

		prop = sparcPromGetProperty(&psdp->node, "vmsize", &len);
		if (prop && (len == 4))
		    vmsize = *(int *)prop;

		switch (chiprev) {
		case 1:
		case 2:
		case 3:
		case 4:
		    psdp->descr = "Sun Double width GX";
		    break;

		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		    psdp->descr = "Sun Single width GX";
		    break;

		case 11:
		    switch (vmsize) {
		    case 2:
			psdp->descr = "Sun Turbo GX with 1M VSIMM";
			break;

		    case 4:
			psdp->descr = "Sun Turbo GX Plus";
			break;

		    default:
			psdp->descr = "Sun Turbo GX";
			break;
		    }
		    break;
		}
		break;

	    case SBUS_DEVICE_CG14:
		vmsize = 0;

		prop = sparcPromGetProperty(&psdp->node, "reg", &len);
		if (prop && (len > 0) && !(len % 12))
		    vmsize = *(int *)(prop + len - 4);

		switch (vmsize) {
		case 0x400000:
		    psdp->descr = "Sun SX with 4M VSIMM";
		    break;

		case 0x800000:
		    psdp->descr = "Sun SX with 8M VSIMM";
		    break;
		}
		break;

	    case SBUS_DEVICE_LEO:
		prop = sparcPromGetProperty(&psdp->node, "model", &len);
		if (prop && (len > 0) && !strstr(prop, "501-2503"))
		    psdp->descr = "Sun Turbo ZX";
		break;

	    case SBUS_DEVICE_TCX:
		if (sparcPromGetBool(&psdp->node, "tcx-8-bit"))
		    psdp->descr = "Sun TCX (8bit)";
		else
		    psdp->descr = "Sun TCX (S24)";
		break;

	    case SBUS_DEVICE_FFB:
		chiprev = 0;

		prop = sparcPromGetProperty(&psdp->node, "board_type", &len);
		if (prop && (len == 4))
		    chiprev = *(int *)prop;

		prop = sparcPromGetProperty(&psdp->node, "name", &len);
		if (!prop || (len < 3))
		    break;

		if (strstr(prop, "afb")) {
		    if (chiprev == 3)
			psdp->descr = "Sun|Elite3D-M6 Horizontal";
		} else {
		    switch (chiprev) {
		    case 0x08:
			psdp->descr = "Sun FFB 67MHz Creator";
			break;

		    case 0x0b:
			psdp->descr = "Sun FFB 67MHz Creator 3D";
			break;

		    case 0x1b:
			psdp->descr = "Sun FFB 75MHz Creator 3D";
			break;

		    case 0x20:
		    case 0x28:
			psdp->descr = "Sun FFB2 Vertical Creator";
			break;

		    case 0x23:
		    case 0x2b:
			psdp->descr = "Sun FFB2 Vertical Creator 3D";
			break;

		    case 0x30:
			psdp->descr = "Sun FFB2+ Vertical Creator";
			break;

		    case 0x33:
			psdp->descr = "Sun FFB2+ Vertical Creator 3D";
			break;

		    case 0x40:
		    case 0x48:
			psdp->descr = "Sun FFB2 Horizontal Creator";
			break;

		    case 0x43:
		    case 0x4b:
			psdp->descr = "Sun FFB2 Horizontal Creator 3D";
			break;
		    }
		}
		break;
	    }

	    promPath = sparcPromNode2Pathname(&psdp->node);
	    if (promPath) {
		xf86Msg(X_PROBED, "SBUS: (0x%08x) %s at %s\n",
			psdp->node.node, psdp->descr, promPath);
		xfree(promPath);
	    } else {
		xf86Msg(X_PROBED, "SBUS: (0x%08x) %s\n",
			psdp->node.node, psdp->descr);
	    }
	} else {
	    xf86Msg(X_PROBED, "SBUS: %s\n", psdp->descr);
	}
    }

    if (useProm)
	sparcPromClose();
}

/*
 * Parse a BUS ID string, and return the SBUS bus parameters if it was
 * in the correct format for a SBUS bus id.
 */

Bool
xf86ParseSbusBusString(const char *busID, int *fbNum)
{
    /*
     * The format is assumed to be one of:
     * the name of the device (with or without the "/dev/" part)
     * "nameN", e.g. "cgsix0", which means Nth instance of card NAME
     * "/prompath", e.g. "/sbus@0,10001000/cgsix@3,0" which is PROM pathname
     * to the device.
     */

    sbusDevicePtr *psdpp;
    const char *id;
    int i, len, devId;

    if (StringToBusType(busID, &id) != BUS_SBUS)
	return FALSE;

    for (psdpp = xf86SbusInfo;  *psdpp; ++psdpp) {
	if (!strcmp((*psdpp)->device, id)) {
	    *fbNum = (*psdpp)->fbNum;
	    return TRUE;
	}
    }

    if (*id != '/') {
	for (psdpp = xf86SbusInfo;  *psdpp; ++psdpp) {
	    if (!strcmp((*psdpp)->device + 5, id)) {
		*fbNum = (*psdpp)->fbNum;
		return TRUE;
	    }
	}

	len = 0;
	for (i = 0; sbusDeviceTable[i].devId; i++) {
	    len = strlen(sbusDeviceTable[i].promName);
	    if (!strncmp(sbusDeviceTable[i].promName, id, len) &&
		isdigit(id[len]))
		break;
	}

	devId = sbusDeviceTable[i].devId;
	if (!devId)
	    return FALSE;

	i = atoi(id + len);
	for (psdpp = xf86SbusInfo; *psdpp; ++psdpp) {
	    if ((*psdpp)->devId != devId)
		continue;

	    if (!i) {
		*fbNum = (*psdpp)->fbNum;
		return TRUE;
	    }

	    i--;
	}

	return FALSE;
    }

    if (sparcPromInit() >= 0) {
	i = sparcPromPathname2Node(id);
	sparcPromClose();
	if (i) {
	    sbusDevicePtr *psdpp;

	    for (psdpp = xf86SbusInfo; *psdpp; ++psdpp) {
		if ((*psdpp)->node.node == i) {
		    *fbNum = (*psdpp)->fbNum;
		    return TRUE;
		}
	    }
	}
    }

    return FALSE;
}

/*
 * Compare a BUS ID string with a SBUS bus id.  Return TRUE if they match.
 */

Bool
xf86CompareSbusBusString(const char *busID, int fbNum)
{
    int iFbNum;

    if (xf86ParseSbusBusString(busID, &iFbNum) && (fbNum == iFbNum))
	return TRUE;
    return FALSE;
}

/*
 * Check if the slot requested is free.  If it is already in use, return FALSE.
 */

Bool
xf86CheckSbusSlot(int fbNum)
{
    int i;
    EntityPtr p;

    for (i = 0; i < xf86NumEntities; i++) {
	p = xf86Entities[i];

	/* Check if this SBUS slot is taken */
	if ((p->busType == BUS_SBUS) && (p->sbusBusId.fbNum == fbNum))
	    return FALSE;
    }

    return TRUE;
}

/*
 * If the slot requested is already in use, return -1.
 * Otherwise, claim the slot for the screen requesting it.
 */

int
xf86ClaimSbusSlot(sbusDevicePtr psdp, DriverPtr drvp,
		  GDevPtr dev, Bool active)
{
    EntityPtr p = NULL;
    int num;

    if (!xf86CheckSbusSlot(psdp->fbNum))
	return -1;

    num = xf86AllocateEntity();
    p = xf86Entities[num];
    p->driver = drvp;
    p->chipset = -1;
    p->busType = BUS_SBUS;
    xf86AddDevToEntity(num, dev);
    p->sbusBusId.fbNum = psdp->fbNum;
    p->active = active;
    p->inUse = FALSE;
    /* Here we initialize the access structure */
    p->access = xnfcalloc(1, sizeof(EntityAccessRec));
    p->access->fallback = &AccessNULL;
    p->access->pAccess = &AccessNULL;
    sbusSlotClaimed = TRUE;
    return num;
}

int
xf86MatchSbusInstances(const char *driverName, int sbusDevId,
		       GDevPtr *devList, int numDevs, DriverPtr drvp,
		       int **foundEntities)
{
    int i, j;
    sbusDevicePtr psdp, *psdpp;
    int numClaimedInstances = 0;
    int allocatedInstances = 0;
    int numFound = 0;
    GDevPtr devBus = NULL;
    GDevPtr dev = NULL;
    int *retEntities = NULL;
    int useProm = 0;

    struct Inst {
	sbusDevicePtr	sbus;
	GDevPtr		dev;
	Bool		claimed;  /* BusID matches with a device section */
    } *instances = NULL;

    *foundEntities = NULL;
    for (psdpp = xf86SbusInfo; (psdp = *psdpp); psdpp++) {
	if (psdp->devId != sbusDevId)
	    continue;
	if (psdp->fd == -2)
	    continue;
	++allocatedInstances;
	instances = xnfrealloc(instances,
			       allocatedInstances * sizeof(struct Inst));
	instances[allocatedInstances - 1].sbus = psdp;
	instances[allocatedInstances - 1].dev = NULL;
	instances[allocatedInstances - 1].claimed = FALSE;
	numFound++;
    }

    /*
     * This may be debatable, but if no SBUS devices with a matching vendor
     * type is found, return zero now.  It is probably not desirable to
     * allow the config file to override this.
     */
    if (allocatedInstances <= 0) {
	xfree(instances);
	return 0;
    }

    if (xf86DoProbe) {
	xfree(instances);
	return numFound;
    }

    if (sparcPromInit() >= 0)
	useProm = 1;

    if (xf86DoConfigure && xf86DoConfigurePass1) {
	GDevPtr pGDev;
	int actualcards = 0;
	for (i = 0; i < allocatedInstances; i++) {
	    actualcards++;
	    pGDev = xf86AddBusDeviceToConfigure(drvp->driverName, BUS_SBUS,
						instances[i].sbus, -1);
	    if (pGDev) {
		/*
		 * XF86Match???Instances() treat chipID and chipRev as
		 * overrides, so clobber them here.
		 */
		pGDev->chipID = pGDev->chipRev = -1;
	    }
	}
	xfree(instances);
	if (useProm)
	    sparcPromClose();
	return actualcards;
    }

#ifdef DEBUG
    ErrorF("%s instances found: %d\n", driverName, allocatedInstances);
#endif

    for (i = 0; i < allocatedInstances; i++) {
	char *promPath = NULL;

	psdp = instances[i].sbus;
	devBus = NULL;
	dev = NULL;
	if (useProm && psdp->node.node)
	    promPath = sparcPromNode2Pathname(&psdp->node);

	for (j = 0; j < numDevs; j++) {
	    if (devList[j]->busID && *devList[j]->busID) {
		if (xf86CompareSbusBusString(devList[j]->busID, psdp->fbNum)) {
		    if (devBus)
			xf86MsgVerb(X_WARNING, 0,
			    "%s: More than one matching Device section for "
			    "instance (BusID: %s) found: %s\n",
			    driverName, devList[j]->identifier,
			    devList[j]->busID);
		    else
			devBus = devList[j];
		}
	    } else {
		if (!dev && !devBus) {
		    if (promPath)
			xf86Msg(X_PROBED, "Assigning device section with no"
				" busID to SBUS:%s\n", promPath);
		    else
			xf86Msg(X_PROBED, "Assigning device section with no"
				" busID to SBUS:fb%d\n", psdp->fbNum);
		    dev = devList[j];
		} else {
		    xf86MsgVerb(X_WARNING, 0,
			    "%s: More than one matching Device section "
			    "found: %s\n", driverName, devList[j]->identifier);
		}
	    }
	}

	if (devBus)
	    dev = devBus;	/* busID preferred */
	if (!dev && (psdp->fd != -2)) {
	    if (promPath) {
		xf86MsgVerb(X_WARNING, 0, "%s: No matching Device section "
			    "for instance (BusID SBUS:%s) found\n",
			    driverName, promPath);
	    } else
		xf86MsgVerb(X_WARNING, 0, "%s: No matching Device section "
			    "for instance (BusID SBUS:fb%d) found\n",
			    driverName, psdp->fbNum);
	} else if (dev) {
	    numClaimedInstances++;
	    instances[i].claimed = TRUE;
	    instances[i].dev = dev;
	}

	if (promPath)
	    xfree(promPath);
    }

#ifdef DEBUG
    ErrorF("%s instances found: %d\n", driverName, numClaimedInstances);
#endif

    /*
     * Of the claimed instances, check that another driver hasn't already
     * claimed its slot.
     */
    numFound = 0;
    if (numClaimedInstances > 0) {
	for (i = 0; i < allocatedInstances; i++) {
	    if (!instances[i].claimed)
		continue;

	    psdp = instances[i].sbus;
	    if (!xf86CheckSbusSlot(psdp->fbNum))
		continue;

#ifdef DEBUG
	    ErrorF("%s: card at fb%d %08x is claimed by a Device section\n",
		   driverName, psdp->fbNum, psdp->node.node);
#endif

	    /* Allocate an entry in the lists to be returned */
	    numFound++;
	    retEntities = xnfrealloc(retEntities, numFound * sizeof(int));
	    retEntities[numFound - 1] =
		xf86ClaimSbusSlot(psdp, drvp, instances[i].dev,
				  instances[i].dev->active ?  TRUE : FALSE);
	}
    }

    xfree(instances);
    if (numFound > 0)
	*foundEntities = retEntities;

    if (useProm)
	sparcPromClose();

    return numFound;
}

/*
 * xf86GetSbusInfoForEntity() -- Get the sbusDevicePtr of entity.
 */
sbusDevicePtr
xf86GetSbusInfoForEntity(int entityIndex)
{
    sbusDevicePtr *psdpp;
    EntityPtr p;

    if ((entityIndex < 0) || (entityIndex >= xf86NumEntities) ||
	(xf86SbusInfo == NULL))
	return NULL;

    p = xf86Entities[entityIndex];
    if (p->busType != BUS_SBUS)
	return NULL;

    for (psdpp = xf86SbusInfo; *psdpp; psdpp++) {
	if (p->sbusBusId.fbNum == (*psdpp)->fbNum)
	    return *psdpp;
    }

    return NULL;
}

int
xf86GetEntityForSbusInfo(sbusDevicePtr psdp)
{
    int i;

    for (i = 0; i < xf86NumEntities; i++) {
	EntityPtr p = xf86Entities[i];

	if (p->busType != BUS_SBUS)
	    continue;

	if (p->sbusBusId.fbNum == psdp->fbNum)
	    return i;
    }
    return -1;
}

void
xf86SbusUseBuiltinMode(ScrnInfoPtr pScrn, sbusDevicePtr psdp)
{
    DisplayModePtr mode;

    mode = xnfcalloc(sizeof(DisplayModeRec), 1);
    mode->name = "current";
    mode->next = mode;
    mode->prev = mode;
    mode->type = M_T_BUILTIN;
    mode->Clock = 100000000;
    mode->HDisplay = psdp->width;
    mode->HSyncStart = psdp->width;
    mode->HSyncEnd = psdp->width;
    mode->HTotal = psdp->width;
    mode->VDisplay = psdp->height;
    mode->VSyncStart = psdp->height;
    mode->VSyncEnd = psdp->height;
    mode->VTotal = psdp->height;
    mode->SynthClock = mode->Clock;
    mode->CrtcHDisplay = mode->HDisplay;
    mode->CrtcHSyncStart = mode->HSyncStart;
    mode->CrtcHSyncEnd = mode->HSyncEnd;
    mode->CrtcHTotal = mode->HTotal;
    mode->CrtcVDisplay = mode->VDisplay;
    mode->CrtcVSyncStart = mode->VSyncStart;
    mode->CrtcVSyncEnd = mode->VSyncEnd;
    mode->CrtcVTotal = mode->VTotal;
    mode->CrtcHAdjusted = FALSE;
    mode->CrtcVAdjusted = FALSE;
    pScrn->modes = mode;
    pScrn->virtualX = psdp->width;
    pScrn->virtualY = psdp->height;
}

/*
 * Colourmap control.
 */

static int sbusPaletteIndex = -1;
static unsigned long sbusPaletteGeneration = 0;
typedef struct _sbusCmap {
    sbusDevicePtr psdp;
    CloseScreenProcPtr CloseScreen;
    Bool origCmapValid;
    unsigned char origRed[16];
    unsigned char origGreen[16];
    unsigned char origBlue[16];
} sbusCmapRec, *sbusCmapPtr;

#define SBUSCMAPPTR(pScreen) (pScreen)->devPrivates[sbusPaletteIndex].ptr

static void
xf86SbusCmapLoadPalette(ScrnInfoPtr pScrn, int numColors, int *indices,
			LOCO *colors, VisualPtr pVisual)
{
    int i, index;
    sbusCmapPtr cmap;
    struct fbcmap fbcmap;
    unsigned char *data = ALLOCATE_LOCAL(numColors * 3);

    cmap = SBUSCMAPPTR(pScrn->pScreen);
    if (!cmap)
	return;

    fbcmap.count = 0;
    fbcmap.index = indices[0];
    fbcmap.red = data;
    fbcmap.green = data + numColors;
    fbcmap.blue = fbcmap.green + numColors;

    for (i = 0; i < numColors; i++) {
	index = indices[i];

	if (fbcmap.count && (index != fbcmap.index + fbcmap.count)) {
	    ioctl(cmap->psdp->fd, FBIOPUTCMAP, &fbcmap);
	    fbcmap.count = 0;
	    fbcmap.index = index;
	}

	fbcmap.red[fbcmap.count] = colors[index].red;
	fbcmap.green[fbcmap.count] = colors[index].green;
	fbcmap.blue[fbcmap.count++] = colors[index].blue;
    }

    ioctl(cmap->psdp->fd, FBIOPUTCMAP, &fbcmap);
    DEALLOCATE_LOCAL(data);
}

static Bool
xf86SbusCmapCloseScreen(int i, ScreenPtr pScreen)
{
    sbusCmapPtr cmap;
    struct fbcmap fbcmap;

    cmap = SBUSCMAPPTR(pScreen);
    if (cmap->origCmapValid) {
	fbcmap.index = 0;
	fbcmap.count = 16;
	fbcmap.red = cmap->origRed;
	fbcmap.green = cmap->origGreen;
	fbcmap.blue = cmap->origBlue;
	ioctl(cmap->psdp->fd, FBIOPUTCMAP, &fbcmap);
    }

    pScreen->CloseScreen = cmap->CloseScreen;
    xfree(cmap);
    return (*pScreen->CloseScreen)(i, pScreen);
}

Bool
xf86SbusHandleColormaps(ScreenPtr pScreen, sbusDevicePtr psdp)
{
    sbusCmapPtr cmap;
    struct fbcmap fbcmap;
    unsigned char data[2];

    if(sbusPaletteGeneration != serverGeneration) {
	if((sbusPaletteIndex = AllocateScreenPrivateIndex()) < 0)
	    return FALSE;

	sbusPaletteGeneration = serverGeneration;
    }

    cmap = xnfcalloc(1, sizeof(sbusCmapRec));
    SBUSCMAPPTR(pScreen) = cmap;
    cmap->psdp = psdp;
    fbcmap.index = 0;
    fbcmap.count = 16;
    fbcmap.red = cmap->origRed;
    fbcmap.green = cmap->origGreen;
    fbcmap.blue = cmap->origBlue;

    if (ioctl(psdp->fd, FBIOGETCMAP, &fbcmap) >= 0)
	cmap->origCmapValid = TRUE;

    fbcmap.index = 0;
    fbcmap.count = 2;
    fbcmap.red = data;
    fbcmap.green = data;
    fbcmap.blue = data;

    if (pScreen->whitePixel == 0) {
	data[0] = 255;
	data[1] = 0;
    } else {
	data[0] = 0;
	data[1] = 255;
    }

    ioctl(psdp->fd, FBIOPUTCMAP, &fbcmap);
    cmap->CloseScreen = pScreen->CloseScreen;
    pScreen->CloseScreen = xf86SbusCmapCloseScreen;
    return xf86HandleColormaps(pScreen, 256, 8,
			       xf86SbusCmapLoadPalette, NULL, 0);
}

/*
 * Cursor control.
 */

/* Tell OS that we are driving the HW cursor ourselves */
void
xf86SbusHideOsHwCursor(sbusDevicePtr psdp)
{
    struct fbcursor fbcursor;
    unsigned char zeros[8];

    memset(&fbcursor, 0, sizeof(fbcursor));
    memset(&zeros, 0, sizeof(zeros));
    fbcursor.cmap.count = 2;
    fbcursor.cmap.red = zeros;
    fbcursor.cmap.green = zeros;
    fbcursor.cmap.blue = zeros;
    fbcursor.image = (char *)zeros;
    fbcursor.mask = (char *)zeros;
    fbcursor.size.x = 32;
    fbcursor.size.y = 1;
    fbcursor.set = FB_CUR_SETALL;
    ioctl(psdp->fd, FBIOSCURSOR, &fbcursor);
}

/* Set HW cursor colormap */
void
xf86SbusSetOsHwCursorCmap(sbusDevicePtr psdp, int bg, int fg)
{
    struct fbcursor fbcursor;
    unsigned char red[2], green[2], blue[2];

    memset(&fbcursor, 0, sizeof(fbcursor));
    red[0] = bg >> 16;
    green[0] = bg >> 8;
    blue[0] = bg;
    red[1] = fg >> 16;
    green[1] = fg >> 8;
    blue[1] = fg;
    fbcursor.cmap.count = 2;
    fbcursor.cmap.red = red;
    fbcursor.cmap.green = green;
    fbcursor.cmap.blue = blue;
    fbcursor.set = FB_CUR_SETCMAP;
    ioctl(psdp->fd, FBIOSCURSOR, &fbcursor);
}

/* Set HW cursor image & mask */
void
xf86SbusSetOsHwCursorImage(sbusDevicePtr psdp, pointer image, pointer mask)
{
    struct fbcursor fbcursor;

    memset(&fbcursor, 0, sizeof(fbcursor));
    fbcursor.image = image;
    fbcursor.mask = mask;
    fbcursor.set = FB_CUR_SETSHAPE;
    ioctl(psdp->fd, FBIOSCURSOR, &fbcursor);
}

/* Set hide/un-hide HW cursor */
void
xf86SbusSetOsHwCursor(sbusDevicePtr psdp, Bool onoff)
{
    struct fbcursor fbcursor;

    memset(&fbcursor, 0, sizeof(fbcursor));
    if (onoff)
	fbcursor.enable = 1;
    fbcursor.set = FB_CUR_SETCUR;
    ioctl(psdp->fd, FBIOSCURSOR, &fbcursor);
}

/* Set HW cursor position */
void
xf86SbusSetOsHwCursorPosition(sbusDevicePtr psdp, int x, int y)
{
    struct fbcursor fbcursor;

    memset(&fbcursor, 0, sizeof(fbcursor));
    fbcursor.pos.x = x;
    fbcursor.pos.y = y;
    fbcursor.set = FB_CUR_SETPOS;
    ioctl(psdp->fd, FBIOSCURSOR, &fbcursor);
}

/* Set HW cursor hot spot */
void
xf86SbusSetOsHwCursorHotSpot(sbusDevicePtr psdp, int hotx, int hoty)
{
    struct fbcursor fbcursor;

    memset(&fbcursor, 0, sizeof(fbcursor));
    fbcursor.hot.x = hotx;
    fbcursor.hot.y = hoty;
    fbcursor.set = FB_CUR_SETHOT;
    ioctl(psdp->fd, FBIOSCURSOR, &fbcursor);
}

/*
 * Screen on/off
 */

Bool
xf86SbusSaveScreen(sbusDevicePtr psdp, int mode)
{
    int state;

    switch (mode) {
    case SCREEN_SAVER_ON:
    case SCREEN_SAVER_CYCLE:
	state = 0;
	break;

    case SCREEN_SAVER_OFF:
    case SCREEN_SAVER_FORCER:
	state = 1;
	break;

    default:
	return FALSE;
    }

    return (ioctl(psdp->fd, FBIOSVIDEO, &state) >= 0);
}
