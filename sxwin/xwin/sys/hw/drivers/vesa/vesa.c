#define DEBUG_VERB 2
/*
 * Copyright (c) 2000 by Conectiva S.A. (http://www.conectiva.com)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * CONECTIVA LINUX BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
 * OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Except as contained in this notice, the name of Conectiva Linux shall
 * not be used in advertising or otherwise to promote the sale, use or other
 * dealings in this Software without prior written authorization from
 * Conectiva Linux.
 *
 * Authors: Paulo César Pereira de Andrade <pcpa@conectiva.com.br>
 *          David Dawes <dawes@xfree86.org>
 *
 * $XFree86: xc/programs/Xserver/hw/xfree86/drivers/vesa/vesa.c,v 1.57 2007/01/23 18:03:10 tsi Exp $
 */
/*
 * Copyright (c) 2000-2005 by The XFree86 Project, Inc.
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject
 * to the following conditions:
 *
 *   1.  Redistributions of source code must retain the above copyright
 *       notice, this list of conditions, and the following disclaimer.
 *
 *   2.  Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer
 *       in the documentation and/or other materials provided with the
 *       distribution, and in the same place and form as other copyright,
 *       license and disclaimer information.
 *
 *   3.  The end-user documentation included with the redistribution,
 *       if any, must include the following acknowledgment: "This product
 *       includes software developed by The XFree86 Project, Inc
 *       (http://www.xfree86.org/) and its contributors", in the same
 *       place and form as other third-party acknowledgments.  Alternately,
 *       this acknowledgment may appear in the software itself, in the
 *       same form and location as other such third-party acknowledgments.
 *
 *   4.  Except as contained in this notice, the name of The XFree86
 *       Project, Inc shall not be used in advertising or otherwise to
 *       promote the sale, use or other dealings in this Software without
 *       prior written authorization from The XFree86 Project, Inc.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE XFREE86 PROJECT, INC OR ITS CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 * OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "vesa.h"

/* All drivers initialising the SW cursor need this */
#include "mipointer.h"

/* All drivers implementing backing store need this */
#include "mibstore.h"

/* Colormap handling */
#include "micmap.h"
#include "xf86cmap.h"

/* DPMS */
#define DPMS_SERVER
#include <X11/extensions/dpms.h>

/* Mandatory functions */
static const OptionInfoRec * VESAAvailableOptions(int chipid, int busid);
static void VESAIdentify(int flags);
static Bool VESAProbe(DriverPtr drv, int flags);
static Bool VESAPreInit(ScrnInfoPtr pScrn, int flags);
static Bool VESAScreenInit(int Index, ScreenPtr pScreen,
			   const int argc, const char **argv);
static Bool VESAEnterVT(int scrnIndex, int flags);
static void VESALeaveVT(int scrnIndex, int flags);
static Bool VESACloseScreen(int scrnIndex, ScreenPtr pScreen);
static Bool VESASaveScreen(ScreenPtr pScreen, int mode);

static Bool VESASwitchMode(int scrnIndex, DisplayModePtr pMode, int flags);
static Bool VESASetMode(ScrnInfoPtr pScrn, DisplayModePtr pMode, Bool blank);
static void VESAAdjustFrame(int scrnIndex, int x, int y, int flags);
static void VESAFreeScreen(int scrnIndex, int flags);
static void VESAFreeRec(ScrnInfoPtr pScrn);

static void
VESADisplayPowerManagementSet(ScrnInfoPtr pScrn, int mode,
		int flags);

/* locally used functions */
static int VESAFindIsaDevice(GDevPtr dev);
static Bool VESAMapVidMem(ScrnInfoPtr pScrn);
static void VESAUnmapVidMem(ScrnInfoPtr pScrn);
static int VESABankSwitch(ScreenPtr pScreen, unsigned int iBank);
static void VESALoadPalette(ScrnInfoPtr pScrn, int numColors, int *indices,
			    LOCO *colors, VisualPtr pVisual);
static void VESASavePlanes(ScrnInfoPtr pScrn);
static void VESARestorePlanes(ScrnInfoPtr pScrn);
static Bool VESASaveRestore(ScrnInfoPtr pScrn,
			    vbeSaveRestoreFunction function);

static void *VESAWindowPlanar(ScreenPtr pScrn, CARD32 row, CARD32 offset,
			      int mode, CARD32 *size, void *closure);
static void *VESAWindowLinear(ScreenPtr pScrn, CARD32 row, CARD32 offset,
			      int mode, CARD32 *size, void *closure);
static void *VESAWindowWindowed(ScreenPtr pScrn, CARD32 row, CARD32 offset,
				int mode, CARD32 *size, void *closure);

static Bool VESADGAInit(ScrnInfoPtr pScrn, ScreenPtr pScreen);

/*
 * This contains the functions needed by the server after loading the
 * driver module.  It must be supplied, and gets added the driver list by
 * the Module Setup funtion in the dynamic case.  In the static case a
 * reference to this is compiled in, and this requires that the name of
 * this DriverRec be an upper-case version of the driver name.
 */
DriverRec VESA = {
    VESA_VERSION,
    VESA_DRIVER_NAME,
    VESAIdentify,
    VESAProbe,
    VESAAvailableOptions,
    NULL,
    0
};

enum GenericTypes
{
    CHIP_VESA_GENERIC
};

/* Supported chipsets */
static SymTabRec VESAChipsets[] =
{
    {CHIP_VESA_GENERIC, "vesa"},
    {-1,		NULL}
};

static PciChipsets VESAPCIchipsets[] = {
  { CHIP_VESA_GENERIC, PCI_CHIP_VGA, RES_SHARED_VGA },
  { -1,		       -1,	     RES_UNDEFINED },
};

static IsaChipsets VESAISAchipsets[] = {
  {CHIP_VESA_GENERIC, RES_EXCLUSIVE_VGA},
  {-1,		      0 }
};

typedef enum {
    OPTION_SHADOW_FB,
    OPTION_DFLT_REFRESH,
    OPTION_VBE_BIG_ENDIAN
} VESAOpts;

static const OptionInfoRec VESAOptions[] = {
    { OPTION_SHADOW_FB,    "ShadowFB",		OPTV_BOOLEAN,	{0},	FALSE },
    { OPTION_DFLT_REFRESH, "DefaultRefresh",	OPTV_BOOLEAN,	{0},	FALSE },
    { OPTION_VBE_BIG_ENDIAN, "VBEBigEndian",	OPTV_BOOLEAN,	{0},	FALSE },
    { -1,		   NULL,		OPTV_NONE,	{0},	FALSE }
};

/*
 * List of symbols from other modules that this module references.  This
 * list is used to tell the loader that it is OK for symbols here to be
 * unresolved providing that it hasn't been told that they haven't been
 * told that they are essential via a call to xf86LoaderModReqSymbols() or
 * xf86LoaderModReqSymLists().  The purpose is this is to avoid warnings about
 * unresolved symbols that are not required.
 */
#ifdef XFree86LOADER
static const char *miscfbSymbols[] = {
    "xf1bppScreenInit",
    "xf4bppScreenInit",
    "afbScreenInit",
    "mfbScreenInit",
    NULL
};
#endif

static const char *fbSymbols[] = {
    "fbPictureInit",
    "fbScreenInit",
    NULL
};

static const char *shadowSymbols[] = {
    "shadowAlloc",
    "shadowInit",
    "shadowUpdatePacked",
    "shadowUpdatePlanar4",
    "shadowUpdatePlanar4x8",
    NULL
};

static const char *vbeSymbols[] = {
    "VBEBankSwitch",
    "VBEExtendedInit",
    "VBEFindSupportedDepths",
    "VBEFreeVBEInfo",
    "VBEGetModeInfo",
    "VBEGetModePool",
    "VBEGetVBEInfo",
    "VBEGetVBEMode",
    "VBEPrintModes",
    "VBESaveRestore",
    "VBESetDisplayStart",
    "VBESetGetDACPaletteFormat",
    "VBESetGetLogicalScanlineLength",
    "VBESetGetPaletteData",
    "VBESetModeNames",
    "VBESetModeParameters",
    "VBESetVBEMode",
    "VBEValidateModes",
    "vbeDoEDID",
    "vbeFree",
    NULL
};

static const char *ddcSymbols[] = {
    "xf86PrintEDID",
    "xf86SetDDCproperties",
    NULL
};

#ifdef XFree86LOADER
/* Module loader interface */
static MODULESETUPPROTO(vesaSetup);

static XF86ModuleVersionInfo vesaVersionRec =
{
    VESA_DRIVER_NAME,
    MODULEVENDORSTRING,
    MODINFOSTRING1,
    MODINFOSTRING2,
    XF86_VERSION_CURRENT,
    VESA_MAJOR_VERSION, VESA_MINOR_VERSION, VESA_PATCHLEVEL,
    ABI_CLASS_VIDEODRV,			/* This is a video driver */
    ABI_VIDEODRV_VERSION,
    MOD_CLASS_VIDEODRV,
    {0, 0, 0, 0}
};

/*
 * This data is accessed by the loader.  The name must be the module name
 * followed by "ModuleData".
 */
XF86ModuleData vesaModuleData = { &vesaVersionRec, vesaSetup, NULL };

static pointer
vesaSetup(ModuleDescPtr Module, pointer Options, int *ErrorMajor, int *ErrorMinor)
{
    static Bool Initialised = FALSE;

    if (!Initialised)
    {
	Initialised = TRUE;
	xf86AddDriver(&VESA, Module, 0);
	LoaderModRefSymLists(Module,
			     miscfbSymbols,
			     fbSymbols,
			     shadowSymbols,
			     vbeSymbols,
			     ddcSymbols,
			     NULL);
	return (pointer)TRUE;
    }

    if (ErrorMajor)
	*ErrorMajor = LDR_ONCEONLY;
    return (NULL);
}

#endif

static const OptionInfoRec *
VESAAvailableOptions(int chipid, int busid)
{
    return (VESAOptions);
}

static void
VESAIdentify(int flags)
{
    xf86PrintChipsets(VESA_NAME, "driver for VESA chipsets", VESAChipsets);
}

/*
 * This function is called once, at the start of the first server generation to
 * do a minimal probe for supported hardware.
 */

static Bool
VESAProbe(DriverPtr drv, int flags)
{
    Bool foundScreen = FALSE;
    int numDevSections, numUsed;
    GDevPtr *devSections;
    int *usedChips;
    int i;

    /*
     * Find the config file Device sections that match this
     * driver, and return if there are none.
     */
    if ((numDevSections = xf86MatchDevice(VESA_NAME,
					  &devSections)) <= 0)
	return (FALSE);

    /* PCI BUS */
    if (xf86GetPciVideoInfo()) {
	numUsed = xf86MatchPciInstances(VESA_NAME, PCI_VENDOR_GENERIC,
					VESAChipsets, VESAPCIchipsets,
					devSections, numDevSections,
					drv, &usedChips);
	if (numUsed > 0) {
	    if (flags & PROBE_DETECT)
		foundScreen = TRUE;
	    else {
		for (i = 0; i < numUsed; i++) {
		    ScrnInfoPtr pScrn = NULL;
		    /* Allocate a ScrnInfoRec  */
		    if ((pScrn = xf86ConfigPciEntity(pScrn,0,usedChips[i],
						     VESAPCIchipsets,NULL,
						     NULL,NULL,NULL,NULL))) {
			pScrn->driverVersion = VESA_VERSION;
			pScrn->driverName    = VESA_DRIVER_NAME;
			pScrn->name	     = VESA_NAME;
			pScrn->Probe	     = VESAProbe;
			pScrn->PreInit       = VESAPreInit;
			pScrn->ScreenInit    = VESAScreenInit;
			pScrn->SwitchMode    = VESASwitchMode;
			pScrn->AdjustFrame   = VESAAdjustFrame;
			pScrn->EnterVT       = VESAEnterVT;
			pScrn->LeaveVT       = VESALeaveVT;
			pScrn->FreeScreen    = VESAFreeScreen;
			foundScreen = TRUE;
		    }
		}
	    }
	    xfree(usedChips);
	}
    }

    /* Isa Bus */
    numUsed = xf86MatchIsaInstances(VESA_NAME,VESAChipsets,
				    VESAISAchipsets, drv,
				    VESAFindIsaDevice, devSections,
				    numDevSections, &usedChips);
    if(numUsed > 0) {
	if (flags & PROBE_DETECT)
	    foundScreen = TRUE;
	else for (i = 0; i < numUsed; i++) {
	    ScrnInfoPtr pScrn = NULL;
	    if ((pScrn = xf86ConfigIsaEntity(pScrn, 0,usedChips[i],
					     VESAISAchipsets, NULL,
					     NULL, NULL, NULL, NULL))) {

		pScrn->driverVersion = VESA_VERSION;
		pScrn->driverName    = VESA_DRIVER_NAME;
		pScrn->name	     = VESA_NAME;
		pScrn->Probe	     = VESAProbe;
		pScrn->PreInit       = VESAPreInit;
		pScrn->ScreenInit    = VESAScreenInit;
		pScrn->SwitchMode    = VESASwitchMode;
		pScrn->AdjustFrame   = VESAAdjustFrame;
		pScrn->EnterVT       = VESAEnterVT;
		pScrn->LeaveVT       = VESALeaveVT;
		pScrn->FreeScreen    = VESAFreeScreen;
		foundScreen = TRUE;
	    }
	}
	xfree(usedChips);
    }

    xfree(devSections);

    return (foundScreen);
}

static int
VESAFindIsaDevice(GDevPtr dev)
{
#ifndef PC98_EGC
    CARD16 GenericIOBase = VGAHW_GET_IOBASE();
    CARD8 CurrentValue, TestValue;

    /* There's no need to unlock VGA CRTC registers here */

    /* VGA has one more read/write attribute register than EGA */
    (void) inb(GenericIOBase + VGA_IN_STAT_1_OFFSET);  /* Reset flip-flop */
    outb(VGA_ATTR_INDEX, 0x14 | 0x20);
    CurrentValue = inb(VGA_ATTR_DATA_R);
    outb(VGA_ATTR_DATA_W, CurrentValue ^ 0x0F);
    outb(VGA_ATTR_INDEX, 0x14 | 0x20);
    TestValue = inb(VGA_ATTR_DATA_R);
    outb(VGA_ATTR_DATA_W, CurrentValue);

    /* Quit now if no VGA is present */
    if ((CurrentValue ^ 0x0F) != TestValue)
      return -1;
#endif
    return (int)CHIP_VESA_GENERIC;
}

static VESAPtr
VESAGetRec(ScrnInfoPtr pScrn)
{
    if (!pScrn->driverPrivate)
	pScrn->driverPrivate = xnfcalloc(sizeof(VESARec), 1);

    return ((VESAPtr)pScrn->driverPrivate);
}

static void
VESAFreeRec(ScrnInfoPtr pScrn)
{
    VESAPtr pVesa = VESAGetRec(pScrn);
#if 0
    DisplayModePtr mode = pScrn->modes;
    /* I am not sure if the modes will ever get freed.
     * Anyway, the data unknown to other modules is being freed here.
     */
    if (mode) {
	do {
	    if (mode->Private) {
		VbeModeInfoData *data = (VbeModeInfoData*)mode->Private;

		if (data->block)
		    xfree(data->block);

		xfree(data);

		mode->Private = NULL;
	    }
	    mode = mode->next;
	} while (mode && mode != pScrn->modes);
    }
#endif
    xfree(pVesa->monitor);
    if (pVesa->vbeInfo)
	VBEFreeVBEInfo(pVesa->vbeInfo);
    xfree(pVesa->pal);
    xfree(pVesa->savedPal);
    xfree(pVesa->planes);
    xfree(pScrn->driverPrivate);
    pScrn->driverPrivate = NULL;
}

/*
 * This function is called once for each screen at the start of the first
 * server generation to initialise the screen for all server generations.
 */
static Bool
VESAPreInit(ScrnInfoPtr pScrn, int flags)
{
    VESAPtr pVesa;
    VbeInfoBlock *vbe;
    DisplayModePtr pMode;
    VbeModeInfoBlock *mode;
    char *mod = NULL;
    const char *reqSym = NULL;
    static const Gamma defaultGamma = {0.0, 0.0, 0.0};
    static const rgb defaultWeight = {0, 0, 0};
    rgb defaultMask = {0, 0, 0};
    ModuleDescPtr pDDCModule = NULL, pVBEModule, pMod = NULL;
    int i;
    int flags24 = 0;
    int defaultDepth = 0;
    int depths = 0;

    if (flags & PROBE_DETECT)
	return (FALSE);

    pVesa = VESAGetRec(pScrn);
    pVesa->pEnt = xf86GetEntityInfo(pScrn->entityList[0]);
    pVesa->device = xf86GetDevFromEntity(pScrn->entityList[0],
					 pScrn->entityInstanceList[0]);

    /* Load vbe module */
    if (!(pVBEModule = xf86LoadVBEModule(pScrn)))
	return (FALSE);

    xf86LoaderModReqSymLists(pVBEModule, vbeSymbols, NULL);

    if ((pVesa->pVbe = VBEExtendedInit(NULL, pVesa->pEnt->index,
					SET_BIOS_SCRATCH
					| RESTORE_BIOS_SCRATCH)) == NULL)
	return (FALSE);

    if (pVesa->pEnt->location.type == BUS_PCI) {
	pVesa->pciInfo = xf86GetPciInfoForEntity(pVesa->pEnt->index);
	pVesa->pciTag = pciTag(pVesa->pciInfo->bus, pVesa->pciInfo->device,
			       pVesa->pciInfo->func);
	pVesa->primary = xf86IsPrimaryPci(pVesa->pciInfo);
    }
    else
	pVesa->primary = TRUE;

    pScrn->chipset = "vesa";
    pScrn->monitor = pScrn->confScreen->monitor;
    pScrn->progClock = TRUE;
    pScrn->rgbBits = 8;

    if ((vbe = VBEGetVBEInfo(pVesa->pVbe)) == NULL)
	return (FALSE);
    pVesa->major = (unsigned)(vbe->VESAVersion >> 8);
    pVesa->minor = vbe->VESAVersion & 0xff;
    pVesa->vbeInfo = vbe;
    pScrn->videoRam = vbe->TotalMemory * 64;

    /*
     * Find what depths are available.
     */
    depths = VBEFindSupportedDepths(pVesa->pVbe, pVesa->vbeInfo, &flags24,
				    V_MODETYPE_VBE);

    /* Preferred order for default depth selection. */
#if X_BYTE_ORDER == X_LITTLE_ENDIAN
    /* These defaults aren't appropriate for big-endian systems */
    if (depths & V_DEPTH_16)
	defaultDepth = 16;
    else
    if (depths & V_DEPTH_15)
	defaultDepth = 15;
    else
#endif
    if (depths & V_DEPTH_8)
	defaultDepth = 8;
    else
    if (depths & V_DEPTH_24)
	defaultDepth = 24;
    else
    if (depths & V_DEPTH_4)
	defaultDepth = 4;
    else
    if (depths & V_DEPTH_1)
	defaultDepth = 1;

    /*
     * Setting this avoids a "Driver can't support depth 24" message,
     * which could be misleading.
     */
    if (!flags24)
	flags24 = Support24bppFb;

    /* Prefer 24bpp for fb since it potentially allows larger modes. */
    if (flags24 & Support24bppFb)
	flags24 |= SupportConvert32to24 | PreferConvert32to24;

    if (!xf86SetDepthBpp(pScrn, defaultDepth, 0, 0, flags24)) {
	vbeFree(pVesa->pVbe);
	pVesa->pVbe = NULL;
	return (FALSE);
    }

    xf86PrintDepthBpp(pScrn);

    /* options */
    xf86CollectOptions(pScrn, NULL);
    if (!(pVesa->Options = xnfalloc(sizeof(VESAOptions)))) {
	vbeFree(pVesa->pVbe);
	pVesa->pVbe = NULL;
	return FALSE;
    }

    memcpy(pVesa->Options, VESAOptions, sizeof(VESAOptions));
    xf86ProcessOptions(pScrn->scrnIndex, pScrn->options, pVesa->Options);

#if X_BYTE_ORDER == X_BIG_ENDIAN

    /* The default is to assume all apertures are little-endian */
    if (!xf86ReturnOptValBool(pVesa->Options, OPTION_VBE_BIG_ENDIAN, FALSE)) {
	if ((pScrn->depth == 24) && (pScrn->bitsPerPixel == 24)) {
	    /*
	     * For bpp 24, the common layer's xf86SetWeight() sets a default of
	     * 'RGB'.  Byte-swap this to 'BGR'.
	     */
	    defaultMask.red = 0x000000FF;
	    defaultMask.green = 0x0000FF00;
	    defaultMask.blue = 0x00FF0000;
	} else if ((pScrn->depth == 24) && (pScrn->bitsPerPixel == 32)) {
	    /*
	     * For bpp 32, the common layer's xf86SetWeight() sets a default of
	     * 'aRGB'.  Byte-swap this to 'BGRa'.
	     */
	    defaultMask.red = 0x0000FF00;
	    defaultMask.green = 0x00FF0000;
	    defaultMask.blue = 0xFF000000;
	} else if (pScrn->bitsPerPixel > 8) {
	    xf86DrvMsg(pScrn->scrnIndex, X_ERROR, "Support for depth %d fbbpp"
		       " %d requires a big-endian aperture\n",
		       pScrn->depth, pScrn->bitsPerPixel);
	    vbeFree(pVesa->pVbe);
	    pVesa->pVbe = NULL;
	    return FALSE;
	}
    }

#endif

    /* color weight */
    if (pScrn->depth > 8 && !xf86SetWeight(pScrn, defaultWeight, defaultMask)) {
	vbeFree(pVesa->pVbe);
	pVesa->pVbe = NULL;
	return (FALSE);
    }

    /* visual init */
    if (!xf86SetDefaultVisual(pScrn, -1)) {
	vbeFree(pVesa->pVbe);
	pVesa->pVbe = NULL;
	return (FALSE);
    }

    xf86SetGamma(pScrn, defaultGamma);

    if (pVesa->major >= 2) {
	/* Load ddc module */
	if (!(pDDCModule = xf86LoadSubModule(pScrn, "ddc"))) {
	    vbeFree(pVesa->pVbe);
	    pVesa->pVbe = NULL;
	    return (FALSE);
	}

	xf86LoaderModReqSymLists(pDDCModule, ddcSymbols, NULL);
	if ((pVesa->monitor = vbeDoEDID(pVesa->pVbe, pDDCModule)) != NULL) {
	    xf86PrintEDID(pVesa->monitor);
	}

    }

    if ((pScrn->monitor->DDC = pVesa->monitor) != NULL) {
	xf86SetDDCproperties(pScrn, pVesa->monitor);

	/* This test is probably not entirely accurate */
	if (pVesa->monitor->features.input_type) {
	    /*
	     * For digital panels, ignore any sync tolerance specifications in
	     * XF86Config monitor sections.  This assumes the BIOS is smart
	     * enough to deal with the panel's tolerances by stretching and/or
	     * centering modes, and disallowing modes larger than the panel.
	     */
	    if (pScrn->monitor->nHsync > 0) {
		xf86DrvMsg(pScrn->scrnIndex, X_NOTICE,
			   "Ignoring XF86Config HorizSync specification(s) for"
			   " digital panel\n");
		pScrn->monitor->nHsync = 0;
	    }

	    if (pScrn->monitor->nVrefresh > 0) {
		xf86DrvMsg(pScrn->scrnIndex, X_NOTICE,
			   "Ignoring XF86Config VertRefresh specification(s)"
			   " for digital panel\n");
		pScrn->monitor->nVrefresh = 0;
	    }

	    pScrn->monitor->flags |= MON_TOLERANCES_OPTIONAL;
	    pVesa->defaultRefresh = TRUE;
	}
    }
    if (pDDCModule)
	xf86UnloadSubModule(pDDCModule);

    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DEBUG_VERB,
		   "Searching for matching VESA mode(s):\n");

    /*
     * Check the available BIOS modes, and extract those that match the
     * requirements into the modePool.  Note: modePool is a NULL-terminated
     * list.
     */
    pScrn->modePool = VBEGetModePool(pScrn, pVesa->pVbe, pVesa->vbeInfo,
				     V_MODETYPE_VBE);

    xf86ErrorFVerb(DEBUG_VERB, "\n");
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DEBUG_VERB,
		   "Total Memory: %d 64KB banks (%dkB)\n", vbe->TotalMemory,
		   (vbe->TotalMemory * 65536) / 1024);

    pVesa->mapSize = vbe->TotalMemory * 65536;
    if (pScrn->modePool == NULL) {
	xf86DrvMsg(pScrn->scrnIndex, X_ERROR, "No matching modes\n");
	vbeFree(pVesa->pVbe);
	pVesa->pVbe = NULL;
	return (FALSE);
    }

    VBESetModeNames(pScrn->modePool);

    i = VBEValidateModes(pScrn, NULL, pScrn->display->modes,
			  NULL, NULL, 0, 2048, 1, 0, 2048,
			  pScrn->display->virtualX,
			  pScrn->display->virtualY,
			  pVesa->mapSize, LOOKUP_BEST_REFRESH);

    if (i <= 0) {
	xf86DrvMsg(pScrn->scrnIndex, X_ERROR, "No valid modes\n");
	vbeFree(pVesa->pVbe);
	pVesa->pVbe = NULL;
	return (FALSE);
    }

    xf86PruneDriverModes(pScrn);

    pMode = pScrn->modes;
    do {
	mode = ((VbeModeInfoData*)pMode->Private)->data;
	if (mode->BytesPerScanline > pVesa->maxBytesPerScanline) {
	    pVesa->maxBytesPerScanline = mode->BytesPerScanline;
	}
	pMode = pMode->next;
    } while (pMode != pScrn->modes);

    pScrn->currentMode = pScrn->modes;
    pScrn->displayWidth = pScrn->virtualX;

    VBEPrintModes(pScrn);

    /* Set display resolution */
    xf86SetDpi(pScrn, 0, 0);

    if (pScrn->modes == NULL) {
	xf86DrvMsg(pScrn->scrnIndex, X_ERROR, "No modes\n");
	vbeFree(pVesa->pVbe);
	pVesa->pVbe = NULL;
	return (FALSE);
    }

    /* Use shadow by default */
    if (xf86ReturnOptValBool(pVesa->Options, OPTION_SHADOW_FB, TRUE))
	pVesa->shadowFB = TRUE;

    if (xf86ReturnOptValBool(pVesa->Options, OPTION_DFLT_REFRESH, FALSE))
	pVesa->defaultRefresh = TRUE;

    if (!pVesa->defaultRefresh)
	VBESetModeParameters(pScrn, pVesa->pVbe);

    mode = ((VbeModeInfoData*)pScrn->modes->Private)->data;
    switch (mode->MemoryModel) {
	case 0x0:	/* Text mode */
	case 0x1:	/* CGA graphics */
	case 0x2:	/* Hercules graphics */
	case 0x5:	/* Non-chain 4, 256 color */
	case 0x7:	/* YUV */
	    xf86DrvMsg(pScrn->scrnIndex, X_ERROR,
		       "Unsupported Memory Model: %d", mode->MemoryModel);
	    break;
	case 0x3:	/* Planar */
	    if (pVesa->shadowFB) {
		mod = "fb";
		pScrn->bitmapBitOrder = BITMAP_BIT_ORDER;
	    }
	    else {
		switch (pScrn->bitsPerPixel) {
		    case 1:
			mod = "xf1bpp";
			reqSym = "xf1bppScreenInit";
			break;
		    case 4:
			mod = "xf4bpp";
			reqSym = "xf4bppScreenInit";
			break;
		    default:
			mod = "afb";
			reqSym = "afbScreenInit";
			break;
		}
	    }
	    break;
	case 0x4:	/* Packed pixel */
	case 0x6:	/*  Direct Color */
	    mod = "fb";
	    pScrn->bitmapBitOrder = BITMAP_BIT_ORDER;

	    switch (pScrn->bitsPerPixel) {
		case 8:
		case 16:
		case 24:
		case 32:
		    break;
		default:
		    xf86DrvMsg(pScrn->scrnIndex, X_ERROR,
			       "Unsupported bpp: %d", pScrn->bitsPerPixel);
		    vbeFree(pVesa->pVbe);
		    pVesa->pVbe = NULL;
		    return FALSE;
	    }
	    break;
    }

    if (pVesa->shadowFB) {
	xf86DrvMsg(pScrn->scrnIndex, X_CONFIG, "Using \"Shadow Framebuffer\"\n");
	if (pScrn->depth == 1) {
	    mod = "mfb";
	    reqSym = "mfbScreenInit";
	}
	if (!(pMod = xf86LoadSubModule(pScrn, "shadow"))) {
	    vbeFree(pVesa->pVbe);
	    pVesa->pVbe = NULL;
	    return (FALSE);
	}
	xf86LoaderModReqSymLists(pMod, shadowSymbols, NULL);
    }

    if (mod && !(pMod = xf86LoadSubModule(pScrn, mod))) {
	VESAFreeRec(pScrn);
	vbeFree(pVesa->pVbe);
	pVesa->pVbe = NULL;
	return (FALSE);
    }

    if (mod) {
	if (reqSym) {
	    xf86LoaderModReqSymbols(pMod, reqSym, NULL);
	} else {
	    xf86LoaderModReqSymLists(pMod, fbSymbols, NULL);
	}
    }

    vbeFree(pVesa->pVbe);
    pVesa->pVbe = NULL;

    return (TRUE);
}

static Bool
VESAScreenInit(int scrnIndex, ScreenPtr pScreen,
	       const int argc, const char **argv)
{
    ScrnInfoPtr pScrn = xf86Screens[scrnIndex];
    VESAPtr pVesa = VESAGetRec(pScrn);
    VisualPtr visual;
    VbeModeInfoBlock *mode;
    int flags;
    int init_picture = 0;
    Bool blank = FALSE;

    if ((pVesa->pVbe = VBEExtendedInit(NULL, pVesa->pEnt->index,
				       SET_BIOS_SCRATCH
				       | RESTORE_BIOS_SCRATCH)) == NULL)
	return (FALSE);

    mode = ((VbeModeInfoData*)(pScrn->currentMode->Private))->data;
    if (pVesa->mapPhys == 0) {
	pScrn->videoRam = pVesa->mapSize;
	pVesa->mapPhys = mode->PhysBasePtr;
	pVesa->mapOff = 0;
    }

    if (pVesa->mapPhys == 0) {
	pVesa->mapPhys = 0xa0000;
	pVesa->mapSize = 0x10000;
    }

    if (!VESAMapVidMem(pScrn)) {
	if (pVesa->mapPhys != 0xa0000) {
	    pVesa->mapPhys = 0xa0000;
	    pVesa->mapSize = 0x10000;
	    if (!VESAMapVidMem(pScrn))
		return (FALSE);
	}
	else
	    return (FALSE);
    }

    /* Set bpp to 8 for depth 4 when using shadowfb. */
    if (pVesa->shadowFB && pScrn->bitsPerPixel == 4)
	pScrn->bitsPerPixel = 8;

    if (pVesa->shadowFB && (pVesa->shadowPtr =
	shadowAlloc(pScrn->virtualX, pScrn->virtualY,
		    pScrn->bitsPerPixel)) == NULL)
	return (FALSE);

    /* save current video state */
    VESASaveRestore(pScrn, MODE_SAVE);
    pVesa->savedPal = VBESetGetPaletteData(pVesa->pVbe, FALSE, 0, 256,
					    NULL, FALSE, FALSE);

    if (mode->ModeAttributes & (1 << 5))
	blank = TRUE;

    /* set first video mode */
    if (!VESASetMode(pScrn, pScrn->currentMode, blank))
	return (FALSE);

    /* set the viewport */
    VESAAdjustFrame(scrnIndex, pScrn->frameX0, pScrn->frameY0, 0);

    /* Blank the screen for aesthetic reasons. */
    VESASaveScreen(pScreen, SCREEN_SAVER_ON);

    /* mi layer */
    miClearVisualTypes();
    if (!xf86SetDefaultVisual(pScrn, -1))
	return (FALSE);
    if (pScrn->bitsPerPixel > 8) {
	if (!miSetVisualTypes(pScrn->depth, TrueColorMask,
			      pScrn->rgbBits, TrueColor))
	    return (FALSE);
    }
    else {
	if (!miSetVisualTypes(pScrn->depth,
			      miGetDefaultVisualMask(pScrn->depth),
			      pScrn->rgbBits, pScrn->defaultVisual))
	    return (FALSE);
    }
    if (!miSetPixmapDepths())
	return (FALSE);

    mode = ((VbeModeInfoData*)pScrn->modes->Private)->data;
    switch (mode->MemoryModel) {
	case 0x0:	/* Text mode */
	case 0x1:	/* CGA graphics */
	case 0x2:	/* Hercules graphics */
	case 0x5:	/* Non-chain 4, 256 color */
	case 0x7:	/* YUV */
	    xf86DrvMsg(pScrn->scrnIndex, X_ERROR,
		       "Unsupported Memory Model: %d", mode->MemoryModel);
	    return (FALSE);
	case 0x3:	/* Planar */
	    if (pVesa->shadowFB) {
		if (pScrn->depth == 1) {
		    if (!mfbScreenInit(pScreen,
				      pVesa->shadowPtr,
				      pScrn->virtualX, pScrn->virtualY,
				      pScrn->xDpi, pScrn->yDpi,
				      pScrn->displayWidth))
			return FALSE;
		} else {
		    if (!fbScreenInit(pScreen,
				      pVesa->shadowPtr,
				      pScrn->virtualX, pScrn->virtualY,
				      pScrn->xDpi, pScrn->yDpi,
				      pScrn->displayWidth, pScrn->bitsPerPixel))
		    return (FALSE);
		    init_picture = 1;
		}
	    } else {
		switch (pScrn->bitsPerPixel) {
		    case 1:
			if (!xf1bppScreenInit(pScreen, pVesa->base,
					      pScrn->virtualX, pScrn->virtualY,
					      pScrn->xDpi, pScrn->yDpi,
					      pScrn->displayWidth))
			    return (FALSE);
			break;
		    case 4:
			if (!xf4bppScreenInit(pScreen, pVesa->base,
					      pScrn->virtualX, pScrn->virtualY,
					      pScrn->xDpi, pScrn->yDpi,
					      pScrn->displayWidth))
			    return (FALSE);
			break;
		    default:
			if (!afbScreenInit(pScreen, pVesa->base,
					   pScrn->virtualX, pScrn->virtualY,
					   pScrn->xDpi, pScrn->yDpi, pScrn->displayWidth))
			    return (FALSE);
			break;
		}
	    }
	    break;
	case 0x4:	/* Packed pixel */
	case 0x6:	/*  Direct Color */
	    switch (pScrn->bitsPerPixel) {
		case 8:
		case 16:
		case 24:
		case 32:
		    if (!fbScreenInit(pScreen,
			    pVesa->shadowFB ? pVesa->shadowPtr : pVesa->base,
				       pScrn->virtualX, pScrn->virtualY,
				       pScrn->xDpi, pScrn->yDpi,
				       pScrn->displayWidth, pScrn->bitsPerPixel))
			return (FALSE);
		    init_picture = 1;
		    break;
		default:
		    xf86DrvMsg(pScrn->scrnIndex, X_ERROR,
			       "Unsupported bpp: %d", pScrn->bitsPerPixel);
		    return (FALSE);
	    }
	    break;
    }


    if (pScrn->bitsPerPixel > 8) {
	/* Fixup RGB ordering */
	visual = pScreen->visuals + pScreen->numVisuals;
	while (--visual >= pScreen->visuals) {
	    if ((visual->class | DynamicClass) == DirectColor) {
		visual->offsetRed   = pScrn->offset.red;
		visual->offsetGreen = pScrn->offset.green;
		visual->offsetBlue  = pScrn->offset.blue;
		visual->redMask     = pScrn->mask.red;
		visual->greenMask   = pScrn->mask.green;
		visual->blueMask    = pScrn->mask.blue;
	    }
	}
    }

    /* must be after RGB ordering fixed */
    if (init_picture)
	fbPictureInit(pScreen, 0, 0);

    if (pVesa->shadowFB) {
	ShadowUpdateProc update;
	ShadowWindowProc window;

	if (mode->MemoryModel == 3) {	/* Planar */
	  if (pScrn->bitsPerPixel == 8)
		update = shadowUpdatePlanar4x8;
	    else
		update = shadowUpdatePlanar4;
	    window = VESAWindowPlanar;
	}
	else if (pVesa->mapPhys == 0xa0000) {	/* Windowed */
	    update = shadowUpdatePacked;
	    window = VESAWindowWindowed;
	}
	else {	/* Linear */
	    update = shadowUpdatePacked;
	    window = VESAWindowLinear;
	}

	if (!shadowInit(pScreen, update, window))
	    return (FALSE);
    }
    else if (pVesa->mapPhys == 0xa0000 && mode->MemoryModel != 0x3) {
	unsigned int bankShift = 0;
	while ((unsigned)(64 >> bankShift) != mode->WinGranularity)
	    bankShift++;
	pVesa->curBank = -1;
	pVesa->bank.SetSourceBank =
	pVesa->bank.SetDestinationBank =
	pVesa->bank.SetSourceAndDestinationBanks = VESABankSwitch;
	pVesa->bank.pBankA = pVesa->bank.pBankB = pVesa->base;
	pVesa->bank.BankSize = (mode->WinSize * 1024) >> bankShift;
	pVesa->bank.nBankDepth = pScrn->depth;
	if (!miInitializeBanking(pScreen, pScrn->virtualX, pScrn->virtualY,
				 pScrn->virtualX, &pVesa->bank)) {
	    xf86DrvMsg(pScrn->scrnIndex, X_ERROR,
		       "Bank switch initialization failed!\n");
	    return (FALSE);
	}
    }

    VESADGAInit(pScrn, pScreen);

    xf86SetBlackWhitePixels(pScreen);
    miInitializeBackingStore(pScreen);
    xf86SetBackingStore(pScreen);

    /* software cursor */
    miDCInitialize(pScreen, xf86GetPointerScreenFuncs());

    /* colormap */
    if (!miCreateDefColormap(pScreen))
	return (FALSE);

    flags = CMAP_RELOAD_ON_MODE_SWITCH;

    if(!xf86HandleColormaps(pScreen, 256,
	pVesa->vbeInfo->Capabilities[0] & 0x01 ? 8 : 6,
	VESALoadPalette, NULL, flags))
	return (FALSE);

    pVesa->CloseScreen = pScreen->CloseScreen;
    pScreen->CloseScreen = VESACloseScreen;
    pScreen->SaveScreen = VESASaveScreen;

    xf86DPMSInit(pScreen, VESADisplayPowerManagementSet, 0);

    /* Report any unused options (only for the first generation) */
    if (serverGeneration == 1)
	xf86ShowUnusedOptions(pScrn->scrnIndex, pScrn->options);

    return (TRUE);
}

static Bool
VESAEnterVT(int scrnIndex, int flags)
{
    ScrnInfoPtr pScrn = xf86Screens[scrnIndex];

    if (!VESASetMode(pScrn, pScrn->currentMode, FALSE))
	return FALSE;
    VESAAdjustFrame(scrnIndex, pScrn->frameX0, pScrn->frameY0, 0);
    return TRUE;
}

static void
VESALeaveVT(int scrnIndex, int flags)
{
    VESASaveRestore(xf86Screens[scrnIndex], MODE_RESTORE);
}

static Bool
VESACloseScreen(int scrnIndex, ScreenPtr pScreen)
{
    ScrnInfoPtr pScrn = xf86Screens[scrnIndex];
    VESAPtr pVesa = VESAGetRec(pScrn);

    if (pScrn->vtSema) {
	VESASaveRestore(xf86Screens[scrnIndex], MODE_RESTORE);
	VBESetGetPaletteData(pVesa->pVbe, TRUE, 0, 256,
			     pVesa->savedPal, FALSE, TRUE);
	VESAUnmapVidMem(pScrn);
    }
    if (pVesa->shadowPtr) {
	xfree(pVesa->shadowPtr);
	pVesa->shadowPtr = NULL;
    }
    if (pVesa->pDGAMode) {
	xfree(pVesa->pDGAMode);
	pVesa->pDGAMode = NULL;
	pVesa->nDGAMode = 0;
    }
    if (pVesa->pVbe) {
	vbeFree(pVesa->pVbe);
	pVesa->pVbe = NULL;
    }
    pScrn->vtSema = FALSE;

    pScreen->CloseScreen = pVesa->CloseScreen;
    return pScreen->CloseScreen(scrnIndex, pScreen);
}

static Bool
VESASwitchMode(int scrnIndex, DisplayModePtr pMode, int flags)
{
    return VESASetMode(xf86Screens[scrnIndex], pMode, FALSE);
}

/* Set a graphics mode */
static Bool
VESASetMode(ScrnInfoPtr pScrn, DisplayModePtr pMode, Bool blank)
{
    VESAPtr pVesa;
    VbeModeInfoData *data;
    int mode;

    pVesa = VESAGetRec(pScrn);

    data = (VbeModeInfoData*)pMode->Private;

    mode = data->mode;
    if (!blank)
	mode |= (1 << 15);

    /* enable linear addressing */
    if (pVesa->mapPhys != 0xa0000)
	mode |= 1 << 14;

    if (VBESetVBEMode(pVesa->pVbe, mode, data->block) == FALSE) {
	xf86DrvMsg(pScrn->scrnIndex, X_INFO, "VBESetVBEMode failed");
	if ((data->block || (data->mode & (1 << 11))) &&
	    VBESetVBEMode(pVesa->pVbe, (mode & ~(1 << 11)), NULL) == TRUE) {
	    /* Some cards do not like setting the clock.
	     * Free it as it will not be any longer useful
	     */
	    xf86ErrorF("...Tried again without customized values.\n");
	    xfree(data->block);
	    data->block = NULL;
	    data->mode &= ~(1 << 11);
	}
	else {
	    ErrorF("\n");
	    xf86DrvMsg(pScrn->scrnIndex, X_ERROR, "Set VBE Mode failed!\n");
	    return (FALSE);
	}
    }

    pVesa->bankSwitchWindowB =
	!((data->data->WinBSegment == 0) && (data->data->WinBAttributes == 0));

    if (data->data->XResolution != pScrn->displayWidth)
	VBESetLogicalScanline(pVesa->pVbe, pScrn->displayWidth);

    if (pScrn->bitsPerPixel == 8 && pVesa->vbeInfo->Capabilities[0] & 0x01)
	VBESetGetDACPaletteFormat(pVesa->pVbe, 8);

    pScrn->vtSema = TRUE;

    return (TRUE);
}

static void
VESAAdjustFrame(int scrnIndex, int x, int y, int flags)
{
    VESAPtr pVesa = VESAGetRec(xf86Screens[scrnIndex]);

    VBESetDisplayStart(pVesa->pVbe, x, y, TRUE);
}

static void
VESAFreeScreen(int scrnIndex, int flags)
{
    VESAFreeRec(xf86Screens[scrnIndex]);
}

static Bool
VESAMapVidMem(ScrnInfoPtr pScrn)
{
    VESAPtr pVesa = VESAGetRec(pScrn);

    if (pVesa->base != NULL)
	return (TRUE);

    pScrn->memPhysBase = pVesa->mapPhys;
    pScrn->fbOffset = pVesa->mapOff;

    pVesa->ioBase = pScrn->domainIOBase;

    if (pVesa->pEnt->location.type == BUS_PCI)
	pVesa->VGAbase = xf86MapPciMem(pScrn->scrnIndex, VIDMEM_MMIO,
				       pVesa->pciTag, 0x0a0000, 0x010000);
    else
	pVesa->VGAbase = xf86MapVidMem(pScrn->scrnIndex, VIDMEM_MMIO,
				       0x0a0000, 0x010000);

    if (pVesa->VGAbase != NULL) {
	if (pVesa->mapPhys == 0x0a0000)
	    pVesa->base = pVesa->VGAbase;
	else if (pVesa->pEnt->location.type == BUS_PCI)
	    pVesa->base = xf86MapPciMem(pScrn->scrnIndex, VIDMEM_FRAMEBUFFER,
					pVesa->pciTag, pScrn->memPhysBase,
					pVesa->mapSize);
	else
	    pVesa->base = xf86MapVidMem(pScrn->scrnIndex, VIDMEM_FRAMEBUFFER,
					pScrn->memPhysBase, pVesa->mapSize);
    }

    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DEBUG_VERB,
		   "virtual address = %p,\n"
		   "\tphysical address = 0x%lx, size = %ld\n",
		   pVesa->base, pScrn->memPhysBase, pVesa->mapSize);

    return pVesa->base != NULL;
}

static void
VESAUnmapVidMem(ScrnInfoPtr pScrn)
{
    VESAPtr pVesa = VESAGetRec(pScrn);

    if (pVesa->base == NULL)
	return;

    xf86UnMapVidMem(pScrn->scrnIndex, pVesa->base, pVesa->mapSize);
    if (pVesa->mapPhys != 0xa0000)
	xf86UnMapVidMem(pScrn->scrnIndex, pVesa->VGAbase, 0x10000);
    pVesa->base = NULL;
}

void *
VESAWindowPlanar(ScreenPtr pScreen, CARD32 row, CARD32 offset, int mode,
		 CARD32 *size, void *closure)
{
    ScrnInfoPtr pScrn = xf86Screens[pScreen->myNum];
    VESAPtr pVesa = VESAGetRec(pScrn);
    VbeModeInfoBlock *data = ((VbeModeInfoData*)(pScrn->currentMode->Private))->data;
    int window;
    int mask = 1 << (offset & 3);

    outb(pVesa->ioBase + VGA_SEQ_INDEX, 2);
    outb(pVesa->ioBase + VGA_SEQ_DATA, mask);
    offset = (offset >> 2) + pVesa->maxBytesPerScanline * row;
    window = offset / (data->WinGranularity * 1024);
    pVesa->windowAoffset = window * data->WinGranularity * 1024;
    VESABankSwitch(pScreen, window);
    *size = data->WinSize * 1024 - (offset - pVesa->windowAoffset);

    return (void *)((unsigned long)pVesa->base +
		   (offset - pVesa->windowAoffset));
}

static void *
VESAWindowLinear(ScreenPtr pScreen, CARD32 row, CARD32 offset, int mode,
		 CARD32 *size, void *closure)
{
    ScrnInfoPtr pScrn = xf86Screens[pScreen->myNum];
    VESAPtr pVesa = VESAGetRec(pScrn);

    *size = pVesa->maxBytesPerScanline;
    return ((CARD8 *)pVesa->base + row * pVesa->maxBytesPerScanline + offset);
}

static void *
VESAWindowWindowed(ScreenPtr pScreen, CARD32 row, CARD32 offset, int mode,
		   CARD32 *size, void *closure)
{
    ScrnInfoPtr pScrn = xf86Screens[pScreen->myNum];
    VESAPtr pVesa = VESAGetRec(pScrn);
    VbeModeInfoBlock *data = ((VbeModeInfoData*)(pScrn->currentMode->Private))->data;
    int window;

    offset += pVesa->maxBytesPerScanline * row;
    window = offset / (data->WinGranularity * 1024);
    pVesa->windowAoffset = window * data->WinGranularity * 1024;
    VESABankSwitch(pScreen, window);
    *size = data->WinSize * 1024 - (offset - pVesa->windowAoffset);

    return (void *)((unsigned long)pVesa->base +
		    (offset - pVesa->windowAoffset));
}

static void
VESALoadPalette(ScrnInfoPtr pScrn, int numColors, int *indices,
		LOCO *colors, VisualPtr pVisual)
{
    VESAPtr pVesa = VESAGetRec(pScrn);
    int i, idx;

#if 0

    /* This code works, but is very slow for programs that use it intensively */
    int base;

    if (pVesa->pal == NULL)
	pVesa->pal = xnfcalloc(1, sizeof(CARD32) * 256);

    for (i = 0, base = idx = indices[i]; i < numColors; i++, idx++) {
	int j = indices[i];

	if (j < 0 || j >= 256)
	    continue;
	pVesa->pal[j] = colors[j].blue |
			(colors[j].green << 8) |
			(colors[j].red << 16);
	if (j != idx) {
	    VBESetGetPaletteData(pVesa->pVbe, TRUE, base, idx - base,
				  pVesa->pal + base, FALSE, TRUE);
	    idx = base = j;
	}
    }

    if (idx - 1 == indices[i - 1])
	VBESetGetPaletteData(pVesa->pVbe, TRUE, base, idx - base,
			      pVesa->pal + base, FALSE, TRUE);

#else

#define VESADACDelay()							    \
    do {								    \
	(void)inb(pVesa->ioBase + VGA_IOBASE_COLOR + VGA_IN_STAT_1_OFFSET); \
	(void)inb(pVesa->ioBase + VGA_IOBASE_COLOR + VGA_IN_STAT_1_OFFSET); \
    } while (0)

    for (i = 0; i < numColors; i++) {
	idx = indices[i];
	outb(pVesa->ioBase + VGA_DAC_WRITE_ADDR, idx);
	VESADACDelay();
	outb(pVesa->ioBase + VGA_DAC_DATA, colors[idx].red);
	VESADACDelay();
	outb(pVesa->ioBase + VGA_DAC_DATA, colors[idx].green);
	VESADACDelay();
	outb(pVesa->ioBase + VGA_DAC_DATA, colors[idx].blue);
	VESADACDelay();
    }

#endif
}

/*
 * Just adapted from the std* functions in vgaHW.c
 */
static void
WriteAttr(VESAPtr pVesa, int index, int value)
{
    (void) inb(pVesa->ioBase + VGA_IOBASE_COLOR + VGA_IN_STAT_1_OFFSET);

    index |= 0x20;
    outb(pVesa->ioBase + VGA_ATTR_INDEX, index);
    outb(pVesa->ioBase + VGA_ATTR_DATA_W, value);
}

static int
ReadAttr(VESAPtr pVesa, int index)
{
    (void) inb(pVesa->ioBase + VGA_IOBASE_COLOR + VGA_IN_STAT_1_OFFSET);

    index |= 0x20;
    outb(pVesa->ioBase + VGA_ATTR_INDEX, index);
    return (inb(pVesa->ioBase + VGA_ATTR_DATA_R));
}

#define WriteMiscOut(value)	outb(pVesa->ioBase + VGA_MISC_OUT_W, value)
#define ReadMiscOut()		inb(pVesa->ioBase + VGA_MISC_OUT_R)
#define WriteSeq(index, value)	outb(pVesa->ioBase + VGA_SEQ_INDEX, index);\
				outb(pVesa->ioBase + VGA_SEQ_DATA, value)

static int
ReadSeq(VESAPtr pVesa, int index)
{
    outb(pVesa->ioBase + VGA_SEQ_INDEX, index);

    return (inb(pVesa->ioBase + VGA_SEQ_DATA));
}

#define WriteGr(index, value)				\
    outb(pVesa->ioBase + VGA_GRAPH_INDEX, index);	\
    outb(pVesa->ioBase + VGA_GRAPH_DATA, value)

static int
ReadGr(VESAPtr pVesa, int index)
{
    outb(pVesa->ioBase + VGA_GRAPH_INDEX, index);

    return (inb(pVesa->ioBase + VGA_GRAPH_DATA));
}

#define WriteCrtc(index, value)						     \
    outb(pVesa->ioBase + (VGA_IOBASE_COLOR + VGA_CRTC_INDEX_OFFSET), index); \
    outb(pVesa->ioBase + (VGA_IOBASE_COLOR + VGA_CRTC_DATA_OFFSET), value)

static int
ReadCrtc(VESAPtr pVesa, int index)
{
    outb(pVesa->ioBase + (VGA_IOBASE_COLOR + VGA_CRTC_INDEX_OFFSET), index);
    return inb(pVesa->ioBase + (VGA_IOBASE_COLOR + VGA_CRTC_DATA_OFFSET));
}

static void
SeqReset(VESAPtr pVesa, Bool start)
{
    if (start) {
	WriteSeq(0x00, 0x01);		/* Synchronous Reset */
    }
    else {
	WriteSeq(0x00, 0x03);		/* End Reset */
    }
}

static void
VESASavePlanes(ScrnInfoPtr pScrn)
{
    VESAPtr pVesa = VESAGetRec(pScrn);
    CARD8 miscOut, attr10, gr1, gr3, gr4, gr5, gr6, gr8, seq2, seq4, scrn;

    if (pVesa->planes != NULL)
	return;

    /* If in graphics mode, don't save anything */
    attr10 = ReadAttr(pVesa, 0x10);
    if (attr10 & 0x01)
	return;

    /* Ensure bank 0 (ignoring error returns) */
    (void) VBEBankSwitch(pVesa->pVbe, 0, 0);
    (void) VBEBankSwitch(pVesa->pVbe, 0, 1);
    pVesa->curBank = -1;

    /* Save the first 64K of all four planes */
    pVesa->planes = xnfalloc(4 * 0x010000);

    /* save the registers that are needed here */
    miscOut = ReadMiscOut();
    gr1 = ReadGr(pVesa, 0x01);
    gr3 = ReadGr(pVesa, 0x03);
    gr4 = ReadGr(pVesa, 0x04);
    gr5 = ReadGr(pVesa, 0x05);
    gr6 = ReadGr(pVesa, 0x06);
    gr8 = ReadGr(pVesa, 0x08);
    seq2 = ReadSeq(pVesa, 0x02);
    seq4 = ReadSeq(pVesa, 0x04);

    /* Force into colour mode */
    WriteMiscOut(miscOut | 0x01);

    scrn = ReadSeq(pVesa, 0x01) | 0x20;
    SeqReset(pVesa, TRUE);
    WriteSeq(0x01, scrn);
    SeqReset(pVesa, FALSE);

    WriteAttr(pVesa, 0x10, 0x01);	/* graphics mode */
    WriteSeq(0x04, 0x06);		/* enable plane graphics */
    WriteGr(0x01, 0x00);		/* all planes come from CPU */
    WriteGr(0x03, 0x00);		/* don't rotate, write unmodified */
    WriteGr(0x05, 0x00);		/* write mode 0, read mode 0 */
    WriteGr(0x06, 0x05);		/* set graphics */
    WriteGr(0x08, 0xFF);		/* write all bits in a byte */

    /* plane0 */
    WriteSeq(0x02, 0x01);		/* write to plane 0 */
    WriteGr(0x04, 0x00);		/* read plane 0 */
    slowbcopy_frombus(pVesa->VGAbase, pVesa->planes + 0 * 0x010000, 0x010000);

    /* plane1 */
    WriteSeq(0x02, 0x02);		/* write to plane 1 */
    WriteGr(0x04, 0x01);		/* read plane 1 */
    slowbcopy_frombus(pVesa->VGAbase, pVesa->planes + 1 * 0x010000, 0x010000);

    /* plane2 */
    WriteSeq(0x02, 0x04);		/* write to plane 2 */
    WriteGr(0x04, 0x02);		/* read plane 2 */
    slowbcopy_frombus(pVesa->VGAbase, pVesa->planes + 2 * 0x010000, 0x010000);

    /* plane3 */
    WriteSeq(0x02, 0x08);		/* write to plane 3 */
    WriteGr(0x04, 0x03);		/* read plane 3 */
    slowbcopy_frombus(pVesa->VGAbase, pVesa->planes + 3 * 0x010000, 0x010000);

    scrn = ReadSeq(pVesa, 0x01) & ~0x20;
    SeqReset(pVesa, TRUE);
    WriteSeq(0x01, scrn);
    SeqReset(pVesa, FALSE);

    /* Restore clobbered registers */
    WriteAttr(pVesa, 0x10, attr10);
    WriteSeq(0x02, seq2);
    WriteSeq(0x04, seq4);
    WriteGr(0x01, gr1);
    WriteGr(0x03, gr3);
    WriteGr(0x04, gr4);
    WriteGr(0x05, gr5);
    WriteGr(0x06, gr6);
    WriteGr(0x08, gr8);
    WriteMiscOut(miscOut);
}

static void
VESARestorePlanes(ScrnInfoPtr pScrn)
{
    VESAPtr pVesa = VESAGetRec(pScrn);
    CARD8 miscOut, attr10, gr1, gr3, gr4, gr5, gr6, gr8, seq2, seq4, scrn;

    if (pVesa->planes == NULL)
	return;

    /* Ensure bank 0 (ignoring error returns) */
    (void) VBEBankSwitch(pVesa->pVbe, 0, 0);
    (void) VBEBankSwitch(pVesa->pVbe, 0, 1);
    pVesa->curBank = -1;

    /* save the registers that are needed here */
    miscOut = ReadMiscOut();
    attr10 = ReadAttr(pVesa, 0x10);
    gr1 = ReadGr(pVesa, 0x01);
    gr3 = ReadGr(pVesa, 0x03);
    gr4 = ReadGr(pVesa, 0x04);
    gr5 = ReadGr(pVesa, 0x05);
    gr6 = ReadGr(pVesa, 0x06);
    gr8 = ReadGr(pVesa, 0x08);
    seq2 = ReadSeq(pVesa, 0x02);
    seq4 = ReadSeq(pVesa, 0x04);

    /* Force into colour mode */
    WriteMiscOut(miscOut | 0x01);

    scrn = ReadSeq(pVesa, 0x01) | 0x20;
    SeqReset(pVesa, TRUE);
    WriteSeq(0x01, scrn);
    SeqReset(pVesa, FALSE);

    WriteAttr(pVesa, 0x10, 0x01);	/* graphics mode */
    WriteSeq(0x04, 0x06);		/* enable plane graphics */
    WriteGr(0x01, 0x00);		/* all planes come from CPU */
    WriteGr(0x03, 0x00);		/* don't rotate, write unmodified */
    WriteGr(0x05, 0x00);		/* write mode 0, read mode 0 */
    WriteGr(0x06, 0x05);		/* set graphics */
    WriteGr(0x08, 0xFF);		/* write all bits in a byte */

    WriteSeq(0x02, 0x01);		/* write to plane 0 */
    WriteGr(0x04, 0x00);		/* read plane 0 */
    slowbcopy_tobus(pVesa->planes + 0 * 0x010000, pVesa->VGAbase, 0x010000);

    WriteSeq(0x02, 0x02);		/* write to plane 1 */
    WriteGr(0x04, 0x01);		/* read plane 1 */
    slowbcopy_tobus(pVesa->planes + 1 * 0x010000, pVesa->VGAbase, 0x010000);

    WriteSeq(0x02, 0x04);		/* write to plane 2 */
    WriteGr(0x04, 0x02);		/* read plane 2 */
    slowbcopy_tobus(pVesa->planes + 2 * 0x010000, pVesa->VGAbase, 0x010000);

    WriteSeq(0x02, 0x08);		/* write to plane 3 */
    WriteGr(0x04, 0x03);		/* read plane 3 */
    slowbcopy_tobus(pVesa->planes + 3 * 0x010000, pVesa->VGAbase, 0x010000);

    scrn = ReadSeq(pVesa, 0x01) & ~0x20;
    SeqReset(pVesa, TRUE);
    WriteSeq(0x01, scrn);
    SeqReset(pVesa, FALSE);

    /* restore the registers that were changed */
    WriteMiscOut(miscOut);
    WriteAttr(pVesa, 0x10, attr10);
    WriteGr(0x01, gr1);
    WriteGr(0x03, gr3);
    WriteGr(0x04, gr4);
    WriteGr(0x05, gr5);
    WriteGr(0x06, gr6);
    WriteGr(0x08, gr8);
    WriteSeq(0x02, seq2);
    WriteSeq(0x04, seq4);
}

static Bool
VESASaveScreen(ScreenPtr pScreen, int mode)
{
    ScrnInfoPtr pScrn = xf86Screens[pScreen->myNum];
    VESAPtr pVesa = VESAGetRec(pScrn);
    Bool on = xf86IsUnblank(mode);
    VbeModeInfoBlock *vMode;

    if (pScrn->currentMode) {
	vMode = ((VbeModeInfoData*)pScrn->currentMode->Private)->data;
	/*
	 * Don't try to use VGA blanking for modes that are not
	 * VGA-compatible.
	 */
	if (vMode->ModeAttributes & (1 << 5)) {
	    return FALSE;
	}
    }

    if (on)
	SetTimeSinceLastInputEvent();

    if (pScrn->vtSema) {
	unsigned char scrn = ReadSeq(pVesa, 0x01);

	if (on)
	    scrn &= ~0x20;
	else
	    scrn |= 0x20;
	SeqReset(pVesa, TRUE);
	WriteSeq(0x01, scrn);
	SeqReset(pVesa, FALSE);
    }

    return (TRUE);
}

static int
VESABankSwitch(ScreenPtr pScreen, unsigned int iBank)
{
    ScrnInfoPtr pScrn = xf86Screens[pScreen->myNum];
    VESAPtr pVesa = VESAGetRec(pScrn);

    if (pVesa->curBank == iBank)
	return (0);
    if (!VBEBankSwitch(pVesa->pVbe, iBank, 0))
	return (1);
    if (pVesa->bankSwitchWindowB) {
	if (!VBEBankSwitch(pVesa->pVbe, iBank, 1))
	   return (1);
    }
    pVesa->curBank = iBank;

    return (0);
}

Bool
VESASaveRestore(ScrnInfoPtr pScrn, vbeSaveRestoreFunction function)
{
    VESAPtr pVesa;

    if ((function < MODE_QUERY) || (function > MODE_RESTORE))
	return (FALSE);

    pVesa = VESAGetRec(pScrn);


    /* Query amount of memory to save state */
    if (function == MODE_QUERY ||
	(function == MODE_SAVE && pVesa->state == NULL)) {

	/* Make sure we save at least this information in case of failure */
	(void)VBEGetVBEMode(pVesa->pVbe, &pVesa->stateMode);
	VESASavePlanes(pScrn);

	/* This only for VESA modes */
	if ((pVesa->stateMode & 0x0100) && (pVesa->major > 1)) {
	    if (!VBESaveRestore(pVesa->pVbe,function,(pointer)&pVesa->state,
				&pVesa->stateSize,&pVesa->statePage))
		return FALSE;

	}
    }

    /* Save/Restore Super VGA state */
    if (function != MODE_QUERY) {
	Bool retval = TRUE;

	if (function == MODE_RESTORE)
	    VBESetVBEMode(pVesa->pVbe, pVesa->stateMode, NULL);

	/* This only for VESA modes */
	if ((pVesa->stateMode & 0x0100) && (pVesa->major > 1)) {
	    if (function == MODE_RESTORE)
		memcpy(pVesa->state, pVesa->pstate, pVesa->stateSize);

	    if ((retval = VBESaveRestore(pVesa->pVbe,function,
					 (pointer)&pVesa->state,
					 &pVesa->stateSize,&pVesa->statePage))
		&& function == MODE_SAVE) {
		/* don't rely on the memory not being touched */
		if (pVesa->pstate == NULL)
		    pVesa->pstate = xnfalloc(pVesa->stateSize);
		memcpy(pVesa->pstate, pVesa->state, pVesa->stateSize);
	    }
	}

	if (function == MODE_RESTORE)
	    VESARestorePlanes(pScrn);

	if (!retval)
	    return (FALSE);

    }

    return (TRUE);
}

static void
VESADisplayPowerManagementSet(ScrnInfoPtr pScrn, int mode,
		int flags)
{
    VESAPtr pVesa = VESAGetRec(pScrn);
    unsigned char seq1 = 0, crtc17 = 0;

    if (!pScrn->vtSema)
	return;

    switch (mode) {
	case DPMSModeOn:
	    /* Screen: On; HSync: On, VSync: On */
	    seq1 = 0x00;
	    crtc17 = 0x80;
	    break;
	case DPMSModeStandby:
	    /* Screen: Off; HSync: Off, VSync: On -- Not Supported */
	    seq1 = 0x20;
	    crtc17 = 0x80;
	    break;
	case DPMSModeSuspend:
	    /* Screen: Off; HSync: On, VSync: Off -- Not Supported */
	    seq1 = 0x20;
	    crtc17 = 0x80;
	    break;
	case DPMSModeOff:
	    /* Screen: Off; HSync: Off, VSync: Off */
	    seq1 = 0x20;
	    crtc17 = 0x00;
	    break;
    }
    WriteSeq(0x00, 0x01);		  /* Synchronous Reset */
    seq1 |= ReadSeq(pVesa, 0x01) & ~0x20;
    WriteSeq(0x01, seq1);
    crtc17 |= ReadCrtc(pVesa, 0x17) & ~0x80;
    usleep(10000);
    WriteCrtc(0x17, crtc17);
    WriteSeq(0x00, 0x03);		  /* End Reset */
}




/***********************************************************************
 * DGA stuff
 ***********************************************************************/
static Bool VESADGAOpenFramebuffer(ScrnInfoPtr pScrn, char **DeviceName,
				   unsigned int *ApertureBase,
				   unsigned int *ApertureSize,
				   unsigned int *ApertureOffset,
				   unsigned int *flags);
static Bool VESADGASetMode(ScrnInfoPtr pScrn, DGAModePtr pDGAMode);
static void VESADGASetViewport(ScrnInfoPtr pScrn, int x, int y, int flags);

static Bool
VESADGAOpenFramebuffer(ScrnInfoPtr pScrn, char **DeviceName,
		       unsigned int *ApertureBase, unsigned int *ApertureSize,
		       unsigned int *ApertureOffset, unsigned int *flags)
{
    VESAPtr pVesa = VESAGetRec(pScrn);

    *DeviceName = NULL;		/* No special device */
    *ApertureBase = pVesa->mapPhys;
    *ApertureSize = pVesa->mapSize;
    *ApertureOffset = pVesa->mapOff;
    *flags = 0;

    return (TRUE);
}

static Bool
VESADGASetMode(ScrnInfoPtr pScrn, DGAModePtr pDGAMode)
{
    DisplayModePtr pMode;
    int scrnIdx = pScrn->pScreen->myNum;
    int frameX0, frameY0;

    if (pDGAMode) {
	pMode = pDGAMode->mode;
	frameX0 = frameY0 = 0;
    }
    else {
	if (!(pMode = pScrn->currentMode))
	    return (TRUE);

	frameX0 = pScrn->frameX0;
	frameY0 = pScrn->frameY0;
    }

    if (!(*pScrn->SwitchMode)(scrnIdx, pMode, 0))
	return (FALSE);
    (*pScrn->AdjustFrame)(scrnIdx, frameX0, frameY0, 0);

    return (TRUE);
}

static void
VESADGASetViewport(ScrnInfoPtr pScrn, int x, int y, int flags)
{
    (*pScrn->AdjustFrame)(pScrn->pScreen->myNum, x, y, flags);
}

static int
VESADGAGetViewport(ScrnInfoPtr pScrn)
{
    return (0);
}

static DGAFunctionRec VESADGAFunctions =
{
    VESADGAOpenFramebuffer,
    NULL,       /* CloseFramebuffer */
    VESADGASetMode,
    VESADGASetViewport,
    VESADGAGetViewport,
    NULL,       /* Sync */
    NULL,       /* FillRect */
    NULL,       /* BlitRect */
    NULL,       /* BlitTransRect */
};

static void
VESADGAAddModes(ScrnInfoPtr pScrn)
{
    VESAPtr pVesa = VESAGetRec(pScrn);
    DisplayModePtr pMode = pScrn->modes;
    DGAModePtr pDGAMode;

    do {
	pDGAMode = xrealloc(pVesa->pDGAMode,
			    (pVesa->nDGAMode + 1) * sizeof(DGAModeRec));
	if (!pDGAMode)
	    break;

	pVesa->pDGAMode = pDGAMode;
	pDGAMode += pVesa->nDGAMode;
	(void)memset(pDGAMode, 0, sizeof(DGAModeRec));

	++pVesa->nDGAMode;
	pDGAMode->mode = pMode;
	pDGAMode->flags = DGA_CONCURRENT_ACCESS | DGA_PIXMAP_AVAILABLE;
	pDGAMode->byteOrder = pScrn->imageByteOrder;
	pDGAMode->depth = pScrn->depth;
	pDGAMode->bitsPerPixel = pScrn->bitsPerPixel;
	pDGAMode->red_mask = pScrn->mask.red;
	pDGAMode->green_mask = pScrn->mask.green;
	pDGAMode->blue_mask = pScrn->mask.blue;
	pDGAMode->visualClass = pScrn->bitsPerPixel > 8 ?
	    TrueColor : PseudoColor;
	pDGAMode->xViewportStep = 1;
	pDGAMode->yViewportStep = 1;
	pDGAMode->viewportWidth = pMode->HDisplay;
	pDGAMode->viewportHeight = pMode->VDisplay;

	pDGAMode->bytesPerScanline = pVesa->maxBytesPerScanline;
	pDGAMode->imageWidth = pMode->HDisplay;
	pDGAMode->imageHeight =  pMode->VDisplay;
	pDGAMode->pixmapWidth = pDGAMode->imageWidth;
	pDGAMode->pixmapHeight = pDGAMode->imageHeight;
	pDGAMode->maxViewportX = pScrn->virtualX -
				    pDGAMode->viewportWidth;
	pDGAMode->maxViewportY = pScrn->virtualY -
				    pDGAMode->viewportHeight;

	pDGAMode->address = pVesa->base;

	pMode = pMode->next;
    } while (pMode != pScrn->modes);
}

static Bool
VESADGAInit(ScrnInfoPtr pScrn, ScreenPtr pScreen)
{
    VESAPtr pVesa = VESAGetRec(pScrn);

    if (pScrn->depth < 8 || pVesa->mapPhys == 0xa0000L)
	return (FALSE);

    if (!pVesa->nDGAMode)
	VESADGAAddModes(pScrn);

    return (DGAInit(pScreen, &VESADGAFunctions,
	    pVesa->pDGAMode, pVesa->nDGAMode));
}
