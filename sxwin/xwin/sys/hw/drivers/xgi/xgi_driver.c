/* $XFree86: xc/programs/Xserver/hw/xfree86/drivers/xgi/xgi_driver.c,v 1.12 2007/01/23 18:03:11 tsi Exp $ */
/*
 * XGI driver main code
 *
 * Copyright (C) 2001-2004 by Thomas Winischhofer, Vienna, Austria.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1) Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2) Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3) The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESSED OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Author: Thomas Winischhofer <thomas@winischhofer.net>
 *	- driver entirely rewritten since 2001, only basic structure taken from
 *	  old code (except xgi_dri.c, xgi_shadow.c, xgi_accel.c and parts of
 *	  xgi_dga.c; these were mostly taken over; xgi_dri.c was changed for
 *	  new versions of the DRI layer)
 *
 * This notice covers the entire driver code unless otherwise indicated.
 *
 * Formerly based on code which is
 * 	     Copyright (C) 1998, 1999 by Alan Hourihane, Wigan, England.
 * Written by:
 *           Alan Hourihane <alanh@fairlite.demon.co.uk>,
 *           Mike Chapman <mike@paranoia.com>,
 *           Juanjo Santamarta <santamarta@ctv.es>,
 *           Mitani Hiroshi <hmitani@drl.mei.co.jp>,
 *           David Thomas <davtom@dream.org.uk>.
 */

#include "fb.h"
#include "mibank.h"
#include "micmap.h"
#include "xf86.h"
#include "xf86Priv.h"
#include "xf86_OSproc.h"
#include "xf86Resources.h"
#include "xf86_ansic.h"
#include "dixstruct.h"
#include "xf86Version.h"
#include "xf86PciInfo.h"
#include "xf86Pci.h"
#include "xf86cmap.h"
#include "vgaHW.h"
#include "xf86RAC.h"
#include "shadowfb.h"
#include "vbe.h"
#ifndef xf86LoadVBEModule
#   define xf86LoadVBEModule(_pScrn) \
	   xf86LoadSubModule(_pScrn, "vbe")
#endif


#include "mipointer.h"
#include "mibstore.h"

#include "xgi.h"
#include "regs.h"
#include "xgi_vb.h"
#include "xgi_dac.h"

#include "xgi_driver.h"
#include "valid_mode.h" 

#define _XF86DGA_SERVER_
#include <X11/extensions/xf86dgastr.h>

#include "globals.h"

#define DPMS_SERVER
#include <X11/extensions/dpms.h>

#include "xf86xv.h"
#include <X11/extensions/Xv.h>

#ifdef XF86DRI
#include "dri.h"
#endif

void Volari_EnableAccelerator(ScrnInfoPtr pScrn) ;
/* Globals (yes, these ARE really required to be global) */

#ifdef XGIDUALHEAD
static int      	XGIEntityIndex = -1;
#endif


/*
 * This is intentionally screen-independent.  It indicates the binding
 * choice made in the first PreInit.
 */
static int pix24bpp = 0;
int    XGI_FbDevExist;

#define FBIOGET_FSCREENINFO	0x4602
#define FB_ACCEL_XGI_GLAMOUR	41

struct fb_fix_screeninfo {
        char id[16];                    /* identification string eg "TT Builtin" */
        unsigned long smem_start;       /* Start of frame buffer mem */
                                        /* (physical address) */
        unsigned long smem_len;         /* Length of frame buffer mem */
        unsigned long type;             /* see FB_TYPE_*                */
        unsigned long type_aux;         /* Interleave for interleaved Planes */
        unsigned long visual;           /* see FB_VISUAL_*              */
        unsigned short xpanstep;        /* zero if no hardware panning  */
        unsigned short ypanstep;        /* zero if no hardware panning  */
        unsigned short ywrapstep;       /* zero if no hardware ywrap    */
        unsigned long line_length;      /* length of a line in bytes    */
        unsigned long mmio_start;       /* Start of Memory Mapped I/O   */
                                        /* (physical address) */
        unsigned long mmio_len;         /* Length of Memory Mapped I/O  */
        unsigned long accel;            /* Type of acceleration available */
        unsigned short reserved[3];     /* Reserved for future compatibility */
};

/*
 * This contains the functions needed by the server after loading the driver
 * module.  It must be supplied, and gets passed back by the SetupProc
 * function in the dynamic case.  In the static case, a reference to this
 * is compiled in, and this requires that the name of this DriverRec be
 * an upper-case version of the driver name.
 */

DriverRec XGI = {
    XGI_CURRENT_VERSION,
    XGI_DRIVER_NAME,
    XGIIdentify,
    XGIProbe,
    XGIAvailableOptions,
    NULL,
    0
};

static SymTabRec XGIChipsets[] = {
    { PCI_CHIP_XGIXG40, "Volari V8_V5_V3XT" },
    { PCI_CHIP_XGIXG20, "Volari Z7"         },
    { -1,                   NULL }
};

static PciChipsets XGIPciChipsets[] = {
    { PCI_CHIP_XGIXG40,     PCI_CHIP_XGIXG40,   RES_SHARED_VGA },
    { PCI_CHIP_XGIXG20,     PCI_CHIP_XGIXG20,   RES_SHARED_VGA },
    { -1,                   -1,                 RES_UNDEFINED }
};

static const char *xaaSymbols[] = {
    "XAACopyROP",
    "XAACreateInfoRec",
    "XAADestroyInfoRec",
#if XF86_VERSION_CURRENT < XF86_VERSION_NUMERIC(4,3,99,0,0)
    "XAAFillSolidRects",
#endif
    "XAAFillMono8x8PatternRects",
    "XAAPatternROP",
    "XAAHelpPatternROP",
    "XAAInit",
    NULL
};

static const char *vgahwSymbols[] = {
    "vgaHWFreeHWRec",
    "vgaHWGetHWRec",
    "vgaHWGetIOBase",
    "vgaHWGetIndex",
    "vgaHWInit",
    "vgaHWLock",
    "vgaHWMapMem",
    "vgaHWUnmapMem",
    "vgaHWProtect",
    "vgaHWRestore",
    "vgaHWSave",
    "vgaHWSaveScreen",
    "vgaHWUnlock",
    NULL
};

static const char *fbSymbols[] = {
    "fbPictureInit",
    "fbScreenInit",
    NULL
};

static const char *shadowSymbols[] = {
    "ShadowFBInit",
    NULL
};

static const char *ramdacSymbols[] = {
    "xf86CreateCursorInfoRec",
    "xf86DestroyCursorInfoRec",
    "xf86InitCursor",
    NULL
};


static const char *ddcSymbols[] = {
    "xf86PrintEDID",
    "xf86SetDDCproperties",
    "xf86InterpretEDID",
    NULL
};


/* static const char *i2cSymbols[] = {
    "xf86I2CBusInit",
    "xf86CreateI2CBusRec",
    NULL
}; */

static const char *int10Symbols[] = {
    "xf86FreeInt10",
    "xf86InitInt10",
    NULL
};

static const char *vbeSymbols[] = {
#if XF86_VERSION_CURRENT < XF86_VERSION_NUMERIC(4,2,99,0,0)
    "VBEInit",
#else
    "VBEExtendedInit",
#endif
    "vbeDoEDID",
    "vbeFree",
    "VBEGetVBEInfo",
    "VBEFreeVBEInfo",
    "VBEGetModeInfo",
    "VBEFreeModeInfo",
    "VBESaveRestore",
    "VBESetVBEMode",
    "VBEGetVBEMode",
    "VBESetDisplayStart",
    "VBESetGetLogicalScanlineLength",
    NULL
};

#ifdef XF86DRI
static const char *drmSymbols[] = {
    "drmAddMap",
    "drmAgpAcquire",
    "drmAgpAlloc",
    "drmAgpBase",
    "drmAgpBind",
    "drmAgpEnable",
    "drmAgpFree",
    "drmAgpGetMode",
    "drmAgpRelease",
    "drmCtlInstHandler",
    "drmGetInterruptFromBusID",
    "drmXGIAgpInit",
    NULL
};

static const char *driSymbols[] = {
    "DRICloseScreen",
    "DRICreateInfoRec",
    "DRIDestroyInfoRec",
    "DRIFinishScreenInit",
    "DRIGetSAREAPrivate",
    "DRILock",
    "DRIQueryVersion",
    "DRIScreenInit",
    "DRIUnlock",
    "GlxSetVisualConfigs",
#ifdef XGINEWDRI2
    "DRICreatePCIBusID",
#endif
    NULL
};
#endif

#ifdef XFree86LOADER

static MODULESETUPPROTO(xgiSetup);

static XF86ModuleVersionInfo xgiVersRec =
{
    XGI_DRIVER_NAME,
    MODULEVENDORSTRING,
    MODINFOSTRING1,
    MODINFOSTRING2,
    XF86_VERSION_CURRENT,
    XGI_MAJOR_VERSION, XGI_MINOR_VERSION, XGI_PATCHLEVEL,
    ABI_CLASS_VIDEODRV,         /* This is a video driver */
    6,
    MOD_CLASS_VIDEODRV,
    {0,0,0,0}
};

XF86ModuleData xgiModuleData = { &xgiVersRec, xgiSetup, NULL };

pointer
xgiSetup(ModuleDescPtr module, pointer opts, int *errmaj, int *errmin)
{
    static Bool setupDone = FALSE;

    if(!setupDone) {
       setupDone = TRUE;
       xf86AddDriver(&XGI, module, 0);
       LoaderModRefSymLists(module, vgahwSymbols, fbSymbols, xaaSymbols,
			    shadowSymbols, ramdacSymbols, ddcSymbols,
			    vbeSymbols, int10Symbols,
#ifdef XF86DRI
			    drmSymbols, driSymbols,
#endif
			    NULL);
       return (pointer)TRUE;
    }

    if(errmaj) *errmaj = LDR_ONCEONLY;
    return NULL;
}

#endif /* XFree86LOADER */


static Bool
XGIGetRec(ScrnInfoPtr pScrn)
{
    /*
     * Allocate an XGIRec, and hook it into pScrn->driverPrivate.
     * pScrn->driverPrivate is initialised to NULL, so we can check if
     * the allocation has already been done.
     */
    if(pScrn->driverPrivate != NULL) return TRUE;

    pScrn->driverPrivate = xnfcalloc(sizeof(XGIRec), 1);

    /* Initialise it to 0 */
    memset(pScrn->driverPrivate, 0, sizeof(XGIRec));

    return TRUE;
}

static void
XGIFreeRec(ScrnInfoPtr pScrn)
{
    XGIPtr pXGI = XGIPTR(pScrn);
#ifdef XGIDUALHEAD
    XGIEntPtr pXGIEnt = NULL;
#endif

    /* Just to make sure... */
    if(!pXGI) return;

#ifdef XGIDUALHEAD
    pXGIEnt = pXGI->entityPrivate;
#endif

    if(pXGI->pstate) xfree(pXGI->pstate);
    pXGI->pstate = NULL;
    if(pXGI->fonts) xfree(pXGI->fonts);
    pXGI->fonts = NULL;

#ifdef XGIDUALHEAD
    if(pXGIEnt) {
       if(!pXGI->SecondHead) {
          /* Free memory only if we are first head; in case of an error
	   * during init of the second head, the server will continue -
	   * and we need the BIOS image and XGI_Private for the first
	   * head.
	   */
	  if(pXGIEnt->BIOS) xfree(pXGIEnt->BIOS);
          pXGIEnt->BIOS = pXGI->BIOS = NULL;
	  if(pXGIEnt->XGI_Pr) xfree(pXGIEnt->XGI_Pr);
          pXGIEnt->XGI_Pr = pXGI->XGI_Pr = NULL;
	  if(pXGIEnt->RenderAccelArray) xfree(pXGIEnt->RenderAccelArray);
	  pXGIEnt->RenderAccelArray = pXGI->RenderAccelArray = NULL;
       } else {
      	  pXGI->BIOS = NULL;
	  pXGI->XGI_Pr = NULL;
	  pXGI->RenderAccelArray = NULL;
       }
    } else {
#endif
       if(pXGI->BIOS) xfree(pXGI->BIOS);
       pXGI->BIOS = NULL;
       if(pXGI->XGI_Pr) xfree(pXGI->XGI_Pr);
       pXGI->XGI_Pr = NULL;
       if(pXGI->RenderAccelArray) xfree(pXGI->RenderAccelArray);
       pXGI->RenderAccelArray = NULL;
#ifdef XGIDUALHEAD
    }
#endif
#ifdef XGIMERGED

    if(pXGI->MetaModes) xfree(pXGI->MetaModes);
    pXGI->MetaModes = NULL;

    if(pXGI->CRT1Modes) {
       if(pXGI->CRT1Modes != pScrn->modes) {
          if(pScrn->modes) {
             pScrn->currentMode = pScrn->modes;
             do {
                DisplayModePtr p = pScrn->currentMode->next;
                if(pScrn->currentMode->Private)
                   xfree(pScrn->currentMode->Private);
                xfree(pScrn->currentMode);
                pScrn->currentMode = p;
             } while(pScrn->currentMode != pScrn->modes);
          }
          pScrn->currentMode = pXGI->CRT1CurrentMode;
          pScrn->modes = pXGI->CRT1Modes;
          pXGI->CRT1CurrentMode = NULL;
          pXGI->CRT1Modes = NULL;
       }
    }
#endif
    if(pXGI->pVbe) vbeFree(pXGI->pVbe);
    pXGI->pVbe = NULL;
    if(pScrn->driverPrivate == NULL)
        return;
    xfree(pScrn->driverPrivate);
    pScrn->driverPrivate = NULL;
}

static void
XGIDisplayPowerManagementSet(ScrnInfoPtr pScrn, int PowerManagementMode, int flags)
{
    XGIPtr pXGI = XGIPTR(pScrn);
    BOOLEAN docrt1 = TRUE;
    unsigned char sr1=0, cr17=0, pmreg=0;
    unsigned char oldpmreg=0;

    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, 3,
          "XGIDisplayPowerManagementSet(%d)\n",PowerManagementMode);

#ifdef XGIDUALHEAD
    if(pXGI->DualHeadMode) {
       if(!pXGI->SecondHead) docrt1 = FALSE;
    }
#endif

#ifdef UNLOCK_ALWAYS
    xgiSaveUnlockExtRegisterLock(pXGI, NULL, NULL);
#endif

    switch (PowerManagementMode) {

       case DPMSModeOn:      /* HSync: On, VSync: On */
            if(docrt1)  pXGI->Blank = FALSE;
#ifdef XGIDUALHEAD
	    else	pXGI->BlankCRT2 = FALSE;
#endif
            sr1   = 0x00;
            cr17  = 0x80;
	    pmreg = 0x00;
            break;

       case DPMSModeSuspend: /* HSync: On, VSync: Off */
            if(docrt1)  pXGI->Blank = TRUE;
#ifdef XGIDUALHEAD
	    else        pXGI->BlankCRT2 = TRUE;
#endif
            sr1   = 0x20;
	    cr17  = 0x80;
	    pmreg = 0x80;
            break;

       case DPMSModeStandby: /* HSync: Off, VSync: On */
            if(docrt1)  pXGI->Blank = TRUE;
#ifdef XGIDUALHEAD
	    else        pXGI->BlankCRT2 = TRUE;
#endif
            sr1   = 0x20;
	    cr17  = 0x80;
	    pmreg = 0x40;
            break;

       case DPMSModeOff:     /* HSync: Off, VSync: Off */
            if(docrt1)  pXGI->Blank = TRUE;
#ifdef XGIDUALHEAD
	    else        pXGI->BlankCRT2 = TRUE;
#endif
            sr1   = 0x20;
	    cr17  = 0x00;
	    pmreg = 0xc0;
	    break;

       default:
	    return;
    }

    if(docrt1) {
       setXGIIDXREG(XGISR, 0x01, ~0x20, sr1);    /* Set/Clear "Display On" bit */
       switch(pXGI->VGAEngine) {
       case XGI_OLD_VGA:
            inXGIIDXREG(XGISR, 0x11, oldpmreg);
            setXGIIDXREG(XGICR, 0x17, 0x7f, cr17);
	    setXGIIDXREG(XGISR, 0x11, 0x3f, pmreg);
	    break;
	    /* fall through */
       default:
            if((!(pXGI->VBFlags & CRT1_LCDA)) || (pXGI->VBFlags & VB_301C)) {
               inXGIIDXREG(XGISR, 0x1f, oldpmreg);
               if(!pXGI->CRT1off) {
	          setXGIIDXREG(XGISR, 0x1f, 0x3f, pmreg);
	       }
	    }
	    /* TODO: Check if Chrontel TV is active and in slave mode,
	     * don't go into power-saving mode this in this case!
	     */
       }
       oldpmreg &= 0xc0;
    }

    if((docrt1) && (pmreg != oldpmreg) && ((!(pXGI->VBFlags & CRT1_LCDA)) || (pXGI->VBFlags & VB_301C))) {
       outXGIIDXREG(XGISR, 0x00, 0x01);    /* Synchronous Reset */
       usleep(10000);
       outXGIIDXREG(XGISR, 0x00, 0x03);    /* End Reset */
    }
}

/* Mandatory */
static void
XGIIdentify(int flags)
{
    xf86PrintChipsets(XGI_NAME, "driver for XGI chipsets", XGIChipsets);
}

static void
XGIErrorLog(ScrnInfoPtr pScrn, const char *format, ...)
{
    va_list ap;
    static const char *str = "**************************************************\n";

    va_start(ap, format);
    xf86DrvMsg(pScrn->scrnIndex, X_ERROR, str);
    xf86DrvMsg(pScrn->scrnIndex, X_ERROR,
    	"                      ERROR:\n");
    xf86VDrvMsgVerb(pScrn->scrnIndex, X_ERROR, 1, format, ap);
    va_end(ap);
    xf86DrvMsg(pScrn->scrnIndex, X_ERROR,
    	"                  END OF MESSAGE\n");
    xf86DrvMsg(pScrn->scrnIndex, X_ERROR, str);
}

/* Mandatory */
static Bool
XGIProbe(DriverPtr drv, int flags)
{
    int     i;
    GDevPtr *devSections;
    int     *usedChips;
    int     numDevSections;
    int     numUsed;
    Bool    foundScreen = FALSE;

    /*
     * The aim here is to find all cards that this driver can handle,
     * and for the ones not already claimed by another driver, claim the
     * slot, and allocate a ScrnInfoRec.
     *
     * This should be a minimal probe, and it should under no circumstances
     * change the state of the hardware.  Because a device is found, don't
     * assume that it will be used.  Don't do any initialisations other than
     * the required ScrnInfoRec initialisations.  Don't allocate any new
     * data structures.
     *
     */

    /*
     * Next we check, if there has been a chipset override in the config file.
     * For this we must find out if there is an active device section which
     * is relevant, i.e., which has no driver specified or has THIS driver
     * specified.
     */

    if((numDevSections = xf86MatchDevice(XGI_DRIVER_NAME, &devSections)) <= 0) {
       /*
        * There's no matching device section in the config file, so quit
        * now.
        */
       return FALSE;
    }

    /*
     * We need to probe the hardware first.  We then need to see how this
     * fits in with what is given in the config file, and allow the config
     * file info to override any contradictions.
     */

    /*
     * All of the cards this driver supports are PCI, so the "probing" just
     * amounts to checking the PCI data that the server has already collected.
     */
    if(xf86GetPciVideoInfo() == NULL) {
       /*
        * We won't let anything in the config file override finding no
        * PCI video cards at all.  This seems reasonable now, but we'll see.
        */
       return FALSE;
    }

    numUsed = xf86MatchPciInstances(XGI_NAME, PCI_VENDOR_XGI,
               		XGIChipsets, XGIPciChipsets, devSections,
               		numDevSections, drv, &usedChips);

    /* Free it since we don't need that list after this */
    xfree(devSections);
    if(numUsed <= 0) return FALSE;

    if(flags & PROBE_DETECT) {
        foundScreen = TRUE;
    } else for(i = 0; i < numUsed; i++) {
        ScrnInfoPtr pScrn;

        /* Allocate a ScrnInfoRec and claim the slot */
        pScrn = NULL;

        if((pScrn = xf86ConfigPciEntity(pScrn, 0, usedChips[i],
                                         XGIPciChipsets, NULL, NULL,
                                         NULL, NULL, NULL))) {
            /* Fill in what we can of the ScrnInfoRec */
            pScrn->driverVersion    = XGI_CURRENT_VERSION;
            pScrn->driverName       = XGI_DRIVER_NAME;
            pScrn->name             = XGI_NAME;
            pScrn->Probe            = XGIProbe;
            pScrn->PreInit          = XGIPreInit;
            pScrn->ScreenInit       = XGIScreenInit;
            pScrn->SwitchMode       = XGISwitchMode;
            pScrn->AdjustFrame      = XGIAdjustFrame;
            pScrn->EnterVT          = XGIEnterVT;
            pScrn->LeaveVT          = XGILeaveVT;
            pScrn->FreeScreen       = XGIFreeScreen;
            pScrn->ValidMode        = XGIValidMode;
            foundScreen = TRUE;
        }
    }
    xfree(usedChips);

    return foundScreen;
}


/* If monitor section has no HSync/VRefresh data,
 * derive it from DDC data. Done by common layer
 * since 4.3.99.14.
 */
#if XF86_VERSION_CURRENT < XF86_VERSION_NUMERIC(4,3,99,14,0)
static void
XGISetSyncRangeFromEdid(ScrnInfoPtr pScrn, int flag)
{
   MonPtr      mon = pScrn->monitor;
   xf86MonPtr  ddc = mon->DDC;
   int         i,j;
   float       myhhigh, myhlow;
   int         myvhigh, myvlow;
   unsigned char temp;
   const myhddctiming myhtiming[11] = {
       { 1, 0x20, 31.6 }, /* rounded up by .1 */
       { 1, 0x02, 35.3 },
       { 1, 0x04, 37.6 },
       { 1, 0x08, 38.0 },
       { 1, 0x01, 38.0 },
       { 2, 0x40, 47.0 },
       { 2, 0x80, 48.2 },
       { 2, 0x08, 48.5 },
       { 2, 0x04, 56.6 },
       { 2, 0x02, 60.1 },
       { 2, 0x01, 80.1 }
   };
   const myvddctiming myvtiming[10] = {
       { 1, 0x02, 56 },
       { 1, 0x01, 60 },
       { 2, 0x08, 60 },
       { 2, 0x04, 70 },
       { 1, 0x08, 72 },
       { 2, 0x80, 72 },
       { 1, 0x04, 75 },
       { 2, 0x40, 75 },
       { 2, 0x02, 75 },
       { 2, 0x01, 75 }
   };
   /* "Future modes"; we only check the really high ones */
   const myddcstdmodes mystdmodes[8] = {
       { 1280, 1024, 85, 91.1  },
       { 1600, 1200, 60, 75.0  },
       { 1600, 1200, 65, 81.3  },
       { 1600, 1200, 70, 87.5  },
       { 1600, 1200, 75, 93.8  },
       { 1600, 1200, 85, 106.3 },
       { 1920, 1440, 60, 90.0  },
       { 1920, 1440, 75, 112.5 }
   };

   if(flag) { /* HSync */
      for(i = 0; i < 4; i++) {
    	 if(ddc->det_mon[i].type == DS_RANGES) {
            mon->nHsync = 1;
            mon->hsync[0].lo = ddc->det_mon[i].section.ranges.min_h;
            mon->hsync[0].hi = ddc->det_mon[i].section.ranges.max_h;
            return;
         }
      }
      /* If no sync ranges detected in detailed timing table, we
       * derive them from supported VESA modes. */
      myhlow = myhhigh = 0.0;
      for(i=0; i<11; i++) {
         if(myhtiming[i].whichone == 1) temp = ddc->timings1.t1;
	 else                           temp = ddc->timings1.t2;
	 if(temp & myhtiming[i].mask) {
	    if((i==0) || (myhlow > myhtiming[i].rate))
	            myhlow = myhtiming[i].rate;
	 }
	 if(myhtiming[10-i].whichone == 1) temp = ddc->timings1.t1;
	 else                              temp = ddc->timings1.t2;
	 if(temp & myhtiming[10-i].mask) {
	    if((i==0) || (myhhigh < myhtiming[10-i].rate))
	            myhhigh = myhtiming[10-i].rate;
	 }
      }
      for(i=0;i<STD_TIMINGS;i++) {
	 if(ddc->timings2[i].hsize > 256) {
            for(j=0; j<8; j++) {
	       if((ddc->timings2[i].hsize == mystdmodes[j].hsize) &&
	          (ddc->timings2[i].vsize == mystdmodes[j].vsize) &&
		  (ddc->timings2[i].refresh == mystdmodes[j].refresh)) {
		  if(mystdmodes[j].hsync > myhhigh)
		     myhhigh = mystdmodes[j].hsync;
	       }
	    }
	 }
      }
      if((myhhigh) && (myhlow)) {
         mon->nHsync = 1;
	 mon->hsync[0].lo = myhlow - 0.1;
	 mon->hsync[0].hi = myhhigh;
      }


   } else {  /* Vrefresh */

      for(i = 0; i < 4; i++) {
         if(ddc->det_mon[i].type == DS_RANGES) {
            mon->nVrefresh = 1;
            mon->vrefresh[0].lo = ddc->det_mon[i].section.ranges.min_v;
            mon->vrefresh[0].hi = ddc->det_mon[i].section.ranges.max_v;
            return;
         }
      }

      myvlow = myvhigh = 0;
      for(i=0; i<10; i++) {
         if(myvtiming[i].whichone == 1) temp = ddc->timings1.t1;
	 else                           temp = ddc->timings1.t2;
	 if(temp & myvtiming[i].mask) {
	    if((i==0) || (myvlow > myvtiming[i].rate))
	           myvlow = myvtiming[i].rate;
	 }
	 if(myvtiming[9-i].whichone == 1) temp = ddc->timings1.t1;
	 else                             temp = ddc->timings1.t2;
	 if(temp & myvtiming[9-i].mask) {
	    if((i==0) || (myvhigh < myvtiming[9-i].rate))
	           myvhigh = myvtiming[9-i].rate;
	 }
      }
      for(i=0;i<STD_TIMINGS;i++) {
	 if(ddc->timings2[i].hsize > 256) {
            for(j=0; j<8; j++) {
	       if((ddc->timings2[i].hsize == mystdmodes[j].hsize) &&
	          (ddc->timings2[i].vsize == mystdmodes[j].vsize) &&
		  (ddc->timings2[i].refresh == mystdmodes[j].refresh)) {
		  if(mystdmodes[j].refresh > myvhigh)
		     myvhigh = mystdmodes[j].refresh;
	       }
	    }
	 }
      }
      if((myvhigh) && (myvlow)) {
         mon->nVrefresh = 1;
	 mon->vrefresh[0].lo = myvlow;
	 mon->vrefresh[0].hi = myvhigh;
      }

    }
}
#endif

/* Some helper functions for MergedFB mode */

#ifdef XGIMERGED

/* Copy and link two modes form mergedfb mode
 * (Code base taken from mga driver)
 * Copys mode i, links the result to dest, and returns it.
 * Links i and j in Private record.
 * If dest is NULL, return value is copy of i linked to itself.
 * For mergedfb auto-config, we only check the dimension
 * against virtualX/Y, if they were user-provided.
 */
static DisplayModePtr
XGICopyModeNLink(ScrnInfoPtr pScrn, DisplayModePtr dest,
                 DisplayModePtr i, DisplayModePtr j,
		 XGIScrn2Rel srel)
{
    XGIPtr pXGI = XGIPTR(pScrn);
    DisplayModePtr mode;
    int dx = 0,dy = 0;

    if(!((mode = xalloc(sizeof(DisplayModeRec))))) return dest;
    memcpy(mode, i, sizeof(DisplayModeRec));
    if(!((mode->Private = xalloc(sizeof(XGIMergedDisplayModeRec))))) {
       xfree(mode);
       return dest;
    }
    ((XGIMergedDisplayModePtr)mode->Private)->CRT1 = i;
    ((XGIMergedDisplayModePtr)mode->Private)->CRT2 = j;
    ((XGIMergedDisplayModePtr)mode->Private)->CRT2Position = srel;
    mode->PrivSize = 0;

    switch(srel) {
    case xgiLeftOf:
    case xgiRightOf:
       if(!(pScrn->display->virtualX)) {
          dx = i->HDisplay + j->HDisplay;
       } else {
          dx = min(pScrn->virtualX, i->HDisplay + j->HDisplay);
       }
       dx -= mode->HDisplay;
       if(!(pScrn->display->virtualY)) {
          dy = max(i->VDisplay, j->VDisplay);
       } else {
          dy = min(pScrn->virtualY, max(i->VDisplay, j->VDisplay));
       }
       dy -= mode->VDisplay;
       break;
    case xgiAbove:
    case xgiBelow:
       if(!(pScrn->display->virtualY)) {
          dy = i->VDisplay + j->VDisplay;
       } else {
          dy = min(pScrn->virtualY, i->VDisplay + j->VDisplay);
       }
       dy -= mode->VDisplay;
       if(!(pScrn->display->virtualX)) {
          dx = max(i->HDisplay, j->HDisplay);
       } else {
          dx = min(pScrn->virtualX, max(i->HDisplay, j->HDisplay));
       }
       dx -= mode->HDisplay;
       break;
    case xgiClone:
       if(!(pScrn->display->virtualX)) {
          dx = max(i->HDisplay, j->HDisplay);
       } else {
          dx = min(pScrn->virtualX, max(i->HDisplay, j->HDisplay));
       }
       dx -= mode->HDisplay;
       if(!(pScrn->display->virtualY)) {
          dy = max(i->VDisplay, j->VDisplay);
       } else {
	  dy = min(pScrn->virtualY, max(i->VDisplay, j->VDisplay));
       }
       dy -= mode->VDisplay;
       break;
    }
    mode->HDisplay += dx;
    mode->HSyncStart += dx;
    mode->HSyncEnd += dx;
    mode->HTotal += dx;
    mode->VDisplay += dy;
    mode->VSyncStart += dy;
    mode->VSyncEnd += dy;
    mode->VTotal += dy;
    mode->Clock = 0;

    if( ((mode->HDisplay * ((pScrn->bitsPerPixel + 7) / 8) * mode->VDisplay) > pXGI->maxxfbmem) ||
        (mode->HDisplay > 4088) ||
	(mode->VDisplay > 4096) ) {

       xf86DrvMsg(pScrn->scrnIndex, X_ERROR,
       		"Skipped %dx%d, not enough video RAM or beyond hardware specs\n",
		mode->HDisplay, mode->VDisplay);
       xfree(mode->Private);
       xfree(mode);

       return dest;
    }

#ifdef XGIXINERAMA
    if(srel != xgiClone) {
       pXGI->AtLeastOneNonClone = TRUE;
    }
#endif

    xf86DrvMsg(pScrn->scrnIndex, X_INFO,
    	"Merged %dx%d and %dx%d to %dx%d%s\n",
	i->HDisplay, i->VDisplay, j->HDisplay, j->VDisplay,
	mode->HDisplay, mode->VDisplay, (srel == xgiClone) ? " (Clone)" : "");

    mode->next = mode;
    mode->prev = mode;

    if(dest) {
       mode->next = dest->next; 	/* Insert node after "dest" */
       dest->next->prev = mode;
       mode->prev = dest;
       dest->next = mode;
    }

    return mode;
}

/* Helper function to find a mode from a given name
 * (Code base taken from mga driver)
 */
static DisplayModePtr
XGIGetModeFromName(char* str, DisplayModePtr i)
{
    DisplayModePtr c = i;
    if(!i) return NULL;
    do {
       if(strcmp(str, c->name) == 0) return c;
       c = c->next;
    } while(c != i);
    return NULL;
}

static DisplayModePtr
XGIFindWidestTallestMode(DisplayModePtr i, Bool tallest)
{
    DisplayModePtr c = i, d = NULL;
    int max = 0;
    if(!i) return NULL;
    do {
       if(tallest) {
          if(c->VDisplay > max) {
             max = c->VDisplay;
	     d = c;
          }
       } else {
          if(c->HDisplay > max) {
             max = c->HDisplay;
	     d = c;
          }
       }
       c = c->next;
    } while(c != i);
    return d;
}

static DisplayModePtr
XGIGenerateModeListFromLargestModes(ScrnInfoPtr pScrn,
		    DisplayModePtr i, DisplayModePtr j,
		    XGIScrn2Rel srel)
{
#ifdef XGIXINERAMA
    XGIPtr pXGI = XGIPTR(pScrn);
#endif
    DisplayModePtr mode1 = NULL;
    DisplayModePtr mode2 = NULL;
    DisplayModePtr result = NULL;

#ifdef XGIXINERAMA
    pXGI->AtLeastOneNonClone = FALSE;
#endif

    switch(srel) {
    case xgiLeftOf:
    case xgiRightOf:
       mode1 = XGIFindWidestTallestMode(i, FALSE);
       mode2 = XGIFindWidestTallestMode(j, FALSE);
       break;
    case xgiAbove:
    case xgiBelow:
       mode1 = XGIFindWidestTallestMode(i, TRUE);
       mode2 = XGIFindWidestTallestMode(j, TRUE);
       break;
    case xgiClone:
       mode1 = i;
       mode2 = j;
    }

    if(mode1 && mode2) {
       return(XGICopyModeNLink(pScrn, result, mode1, mode2, srel));
    } else {
       return NULL;
    }
}

/* Generate the merged-fb mode modelist from metamodes
 * (Code base taken from mga driver)
 */
static DisplayModePtr
XGIGenerateModeListFromMetaModes(ScrnInfoPtr pScrn, char* str,
		    DisplayModePtr i, DisplayModePtr j,
		    XGIScrn2Rel srel)
{
#ifdef XGIXINERAMA
    XGIPtr pXGI = XGIPTR(pScrn);
#endif
    char* strmode = str;
    char modename[256];
    Bool gotdash = FALSE;
    XGIScrn2Rel sr;
    DisplayModePtr mode1 = NULL;
    DisplayModePtr mode2 = NULL;
    DisplayModePtr result = NULL;

#ifdef XGIXINERAMA
    pXGI->AtLeastOneNonClone = FALSE;
#endif

    do {
        switch(*str) {
        case 0:
        case '-':
        case ' ':
           if((strmode != str)) {

              strncpy(modename, strmode, str - strmode);
              modename[str - strmode] = 0;

              if(gotdash) {
                 if(mode1 == NULL) return NULL;
                 mode2 = XGIGetModeFromName(modename, j);
                 if(!mode2) {
                    xf86DrvMsg(pScrn->scrnIndex, X_WARNING,
                        "Mode \"%s\" is not a supported mode for CRT2\n", modename);
                    xf86DrvMsg(pScrn->scrnIndex, X_WARNING,
                        "Skipping metamode \"%s-%s\".\n", mode1->name, modename);
                    mode1 = NULL;
                 }
              } else {
                 mode1 = XGIGetModeFromName(modename, i);
                 if(!mode1) {
                    char* tmps = str;
                    xf86DrvMsg(pScrn->scrnIndex, X_WARNING,
                        "Mode \"%s\" is not a supported mode for CRT1\n", modename);
                    gotdash = FALSE;
                    while(*tmps == ' ') tmps++;
                    if(*tmps == '-') { 							/* skip the next mode */
                       tmps++;
                       while((*tmps == ' ') && (*tmps != 0)) tmps++; 			/* skip spaces */
                       while((*tmps != ' ') && (*tmps != '-') && (*tmps != 0)) tmps++; 	/* skip modename */
                       strncpy(modename,strmode,tmps - strmode);
                       modename[tmps - strmode] = 0;
                       str = tmps-1;
                    }
                    xf86DrvMsg(pScrn->scrnIndex, X_WARNING,
                        "Skipping metamode \"%s\".\n", modename);
                    mode1 = NULL;
                 }
              }
              gotdash = FALSE;
           }
           strmode = str + 1;
           gotdash |= (*str == '-');

           if(*str != 0) break;
	   /* Fall through otherwise */

        default:
           if(!gotdash && mode1) {
              sr = srel;
              if(!mode2) {
                 mode2 = XGIGetModeFromName(mode1->name, j);
                 sr = xgiClone;
              }
              if(!mode2) {
                 xf86DrvMsg(pScrn->scrnIndex, X_WARNING,
                     "Mode: \"%s\" is not a supported mode for CRT2\n", mode1->name);
                 xf86DrvMsg(pScrn->scrnIndex, X_WARNING,
                     "Skipping metamode \"%s\".\n", modename);
                 mode1 = NULL;
              } else {
                 result = XGICopyModeNLink(pScrn, result, mode1, mode2, sr);
                 mode1 = NULL;
                 mode2 = NULL;
              }
           }
           break;

        }

    } while(*(str++) != 0);

    return result;
}

static DisplayModePtr
XGIGenerateModeList(ScrnInfoPtr pScrn, char* str,
		    DisplayModePtr i, DisplayModePtr j,
		    XGIScrn2Rel srel)
{
   if(str != NULL) {
      return(XGIGenerateModeListFromMetaModes(pScrn, str, i, j, srel));
   } else {
      xf86DrvMsg(pScrn->scrnIndex, X_INFO,
      	"No MetaModes given, linking %s modes by default\n",
	(srel == xgiClone) ? "first" :
	   (((srel == xgiLeftOf) || (srel == xgiRightOf)) ? "widest" :  "tallest"));
      return(XGIGenerateModeListFromLargestModes(pScrn, i, j, srel));
   }
}

static void
XGIRecalcDefaultVirtualSize(ScrnInfoPtr pScrn)
{
    DisplayModePtr mode, bmode;
    int max;
    static const char *str = "MergedFB: Virtual %s %d\n";

    if(!(pScrn->display->virtualX)) {
       mode = bmode = pScrn->modes;
       max = 0;
       do {
          if(mode->HDisplay > max) max = mode->HDisplay;
          mode = mode->next;
       } while(mode != bmode);
       pScrn->virtualX = max;
       pScrn->displayWidth = max;
       xf86DrvMsg(pScrn->scrnIndex, X_PROBED, str, "width", max);
    }
    if(!(pScrn->display->virtualY)) {
       mode = bmode = pScrn->modes;
       max = 0;
       do {
          if(mode->VDisplay > max) max = mode->VDisplay;
          mode = mode->next;
       } while(mode != bmode);
       pScrn->virtualY = max;
       xf86DrvMsg(pScrn->scrnIndex, X_PROBED, str, "height", max);
    }
}

static void
XGIMergedFBSetDpi(ScrnInfoPtr pScrn1, ScrnInfoPtr pScrn2, XGIScrn2Rel srel)
{
   XGIPtr pXGI = XGIPTR(pScrn1);
   MessageType from = X_DEFAULT;
   xf86MonPtr DDC1 = (xf86MonPtr)(pScrn1->monitor->DDC);
   xf86MonPtr DDC2 = (xf86MonPtr)(pScrn2->monitor->DDC);
   int ddcWidthmm = 0, ddcHeightmm = 0;
   const char *dsstr = "MergedFB: Display dimensions: (%d, %d) mm\n";

   /* This sets the DPI for MergedFB mode. The problem is that
    * this can never be exact, because the output devices may
    * have different dimensions. This function tries to compromise
    * through a few assumptions, and it just calculates an average DPI
    * value for both monitors.
    */

   /* Given DisplaySize should regard BOTH monitors */
   pScrn1->widthmm = pScrn1->monitor->widthmm;
   pScrn1->heightmm = pScrn1->monitor->heightmm;

   /* Get DDC display size; if only either CRT1 or CRT2 provided these,
    * assume equal dimensions for both, otherwise add dimensions
    */
   if( (DDC1 && (DDC1->features.hsize > 0 && DDC1->features.vsize > 0)) &&
       (DDC2 && (DDC2->features.hsize > 0 && DDC2->features.vsize > 0)) ) {
      ddcWidthmm = max(DDC1->features.hsize, DDC2->features.hsize) * 10;
      ddcHeightmm = max(DDC1->features.vsize, DDC2->features.vsize) * 10;
      switch(srel) {
      case xgiLeftOf:
      case xgiRightOf:
         ddcWidthmm = (DDC1->features.hsize + DDC2->features.hsize) * 10;
	 break;
      case xgiAbove:
      case xgiBelow:
         ddcHeightmm = (DDC1->features.vsize + DDC2->features.vsize) * 10;
      default:
	 break;
      }
   } else if(DDC1 && (DDC1->features.hsize > 0 && DDC1->features.vsize > 0)) {
      ddcWidthmm = DDC1->features.hsize * 10;
      ddcHeightmm = DDC1->features.vsize * 10;
      switch(srel) {
      case xgiLeftOf:
      case xgiRightOf:
         ddcWidthmm *= 2;
	 break;
      case xgiAbove:
      case xgiBelow:
         ddcHeightmm *= 2;
      default:
	 break;
      }
   } else if(DDC2 && (DDC2->features.hsize > 0 && DDC2->features.vsize > 0) ) {
      ddcWidthmm = DDC2->features.hsize * 10;
      ddcHeightmm = DDC2->features.vsize * 10;
      switch(srel) {
      case xgiLeftOf:
      case xgiRightOf:
         ddcWidthmm *= 2;
	 break;
      case xgiAbove:
      case xgiBelow:
         ddcHeightmm *= 2;
      default:
	 break;
      }
   }

   if(monitorResolution > 0) {

      /* Set command line given values (overrules given options) */
      pScrn1->xDpi = monitorResolution;
      pScrn1->yDpi = monitorResolution;
      from = X_CMDLINE;

   } else if(pXGI->MergedFBXDPI) {

      /* Set option-wise given values (overrule DisplaySize) */
      pScrn1->xDpi = pXGI->MergedFBXDPI;
      pScrn1->yDpi = pXGI->MergedFBYDPI;
      from = X_CONFIG;

   } else if(pScrn1->widthmm > 0 || pScrn1->heightmm > 0) {

      /* Set values calculated from given DisplaySize */
      from = X_CONFIG;
      if(pScrn1->widthmm > 0) {
	 pScrn1->xDpi = (int)((double)pScrn1->virtualX * 25.4 / pScrn1->widthmm);
      }
      if(pScrn1->heightmm > 0) {
	 pScrn1->yDpi = (int)((double)pScrn1->virtualY * 25.4 / pScrn1->heightmm);
      }
      xf86DrvMsg(pScrn1->scrnIndex, from, dsstr, pScrn1->widthmm, pScrn1->heightmm);

    } else if(ddcWidthmm && ddcHeightmm) {

      /* Set values from DDC-provided display size */
      from = X_PROBED;
      xf86DrvMsg(pScrn1->scrnIndex, from, dsstr, ddcWidthmm, ddcHeightmm );
      pScrn1->widthmm = ddcWidthmm;
      pScrn1->heightmm = ddcHeightmm;
      if(pScrn1->widthmm > 0) {
	 pScrn1->xDpi = (int)((double)pScrn1->virtualX * 25.4 / pScrn1->widthmm);
      }
      if(pScrn1->heightmm > 0) {
	 pScrn1->yDpi = (int)((double)pScrn1->virtualY * 25.4 / pScrn1->heightmm);
      }

    } else {

      pScrn1->xDpi = pScrn1->yDpi = DEFAULT_DPI;

    }

    /* Sanity check */
    if(pScrn1->xDpi > 0 && pScrn1->yDpi <= 0)
       pScrn1->yDpi = pScrn1->xDpi;
    if(pScrn1->yDpi > 0 && pScrn1->xDpi <= 0)
       pScrn1->xDpi = pScrn1->yDpi;

    pScrn2->xDpi = pScrn1->xDpi;
    pScrn2->yDpi = pScrn1->yDpi;

    xf86DrvMsg(pScrn1->scrnIndex, from, "MergedFB: DPI set to (%d, %d)\n",
	       pScrn1->xDpi, pScrn1->yDpi);
}

static void
XGIFreeCRT2Structs(XGIPtr pXGI)
{
    if(pXGI->CRT2pScrn) {
       if(pXGI->CRT2pScrn->modes) {
          while(pXGI->CRT2pScrn->modes)
  	     xf86DeleteMode(&pXGI->CRT2pScrn->modes, pXGI->CRT2pScrn->modes);
       }
       if(pXGI->CRT2pScrn->monitor) {
          if(pXGI->CRT2pScrn->monitor->Modes) {
             while(pXGI->CRT2pScrn->monitor->Modes)
  	        xf86DeleteMode(&pXGI->CRT2pScrn->monitor->Modes, pXGI->CRT2pScrn->monitor->Modes);
	  }
	  if(pXGI->CRT2pScrn->monitor->DDC) xfree(pXGI->CRT2pScrn->monitor->DDC);
	  xfree(pXGI->CRT2pScrn->monitor);
       }
       xfree(pXGI->CRT2pScrn);
       pXGI->CRT2pScrn = NULL;
   }
}

#endif	/* End of MergedFB helpers */

static xf86MonPtr
XGIInternalDDC(ScrnInfoPtr pScrn, int crtno)
{
   XGIPtr        pXGI = XGIPTR(pScrn);
   USHORT        temp = 0xffff;
   unsigned char buffer[256];
   xf86MonPtr    pMonitor = NULL;

   /* If CRT1 is off, skip DDC */
   if((pXGI->CRT1off) && (!crtno)) return NULL;

   if(crtno) {
      if(!(pXGI->VBFlags & (CRT2_LCD|CRT2_VGA)))
         return NULL;
   } else {
      /* If CRT1 is LCDA, skip DDC (except 301C: DDC allowed, but uses CRT2 port!) */
      if(pXGI->VBFlags & CRT1_LCDA) {
         if(!(pXGI->VBFlags & VB_301C))
            return NULL;
      }
   }

   if(temp != 0xffff) {
      xf86DrvMsg(pScrn->scrnIndex, X_PROBED, "CRT%d DDC supported\n", crtno + 1);
      xf86DrvMsg(pScrn->scrnIndex, X_PROBED, "CRT%d DDC level: %s%s%s%s\n",
	     crtno + 1,
	     (temp & 0x1a) ? "" : "[none of the supported]",
	     (temp & 0x02) ? "2 " : "",
	     (temp & 0x08) ? "D&P" : "",
             (temp & 0x10) ? "FPDI-2" : "");
      if(temp & 0x02) {
         if(!temp) {
	    if((pMonitor = xf86InterpretEDID(pScrn->scrnIndex, &buffer[0]))) {
	       return(pMonitor);
	    } else {
	       xf86DrvMsg(pScrn->scrnIndex, X_WARNING,
	           "CRT%d DDC EDID corrupt\n", crtno + 1);
	       return(NULL);
	    }
	 } else {
            xf86DrvMsg(pScrn->scrnIndex, X_WARNING,
	    	"CRT%d DDC reading failed\n", crtno + 1);
	    return(NULL);
	 }
      } else if(!crtno) {
	 xf86DrvMsg(pScrn->scrnIndex, X_INFO,
	      "DDC for VESA D&P and FPDI-2 not supported for CRT1.\n");
         return(NULL);
      } else if(temp & 0x18) {
         xf86DrvMsg(pScrn->scrnIndex, X_INFO,
	      "DDC for VESA D&P and FPDI-2 not supported for CRT2 yet.\n");
         return(NULL);
      }
      return(NULL);
   } else {
      xf86DrvMsg(pScrn->scrnIndex, X_PROBED,
                "CRT%d DDC probing failed\n", crtno + 1);
      return(NULL);
   }
}

/* static xf86MonPtr
XGIDoPrivateDDC(ScrnInfoPtr pScrn, int *crtnum)
{
    XGIPtr pXGI = XGIPTR(pScrn);

#ifdef XGIDUALHEAD
    if(pXGI->DualHeadMode) {
       if(pXGI->SecondHead) {
          *crtnum = 1;
	  return(XGIInternalDDC(pScrn, 0));
       } else {
          *crtnum = 2;
	  return(XGIInternalDDC(pScrn, 1));
       }
    } else
#endif
    if(pXGI->CRT1off) {
       *crtnum = 2;
       return(XGIInternalDDC(pScrn, 1));
    } else {
       *crtnum = 1;
       return(XGIInternalDDC(pScrn, 0));
    }
} */

/* Mandatory */
static Bool
XGIPreInit(ScrnInfoPtr pScrn, int flags)
{
    XGIPtr pXGI;
    MessageType from;
/*    unsigned char usScratchCR17, CR5F;
    unsigned char usScratchCR32, usScratchCR63;
    unsigned char usScratchSR1F;  */
    unsigned long int i;
    int temp;
    ClockRangePtr clockRanges;
    int pix24flags;
    int fd; 
    struct fb_fix_screeninfo fix;

#ifdef XGIDUALHEAD
    XGIEntPtr pXGIEnt = NULL;
#endif
#if defined(XGIMERGED) || defined(XGIDUALHEAD)
    DisplayModePtr first, p, n;
#endif
    unsigned char srlockReg,crlockReg;
/*    unsigned char tempreg;  */
    xf86MonPtr pMonitor = NULL;
    Bool didddc2;

    vbeInfoPtr pVbe;
    VbeInfoBlock *vbe;
    ModuleDescPtr pVBEModule = NULL, pMod, pDDCModule;

    static const char *ddcsstr = "CRT%d DDC monitor info: ************************************\n";
    static const char *ddcestr = "End of CRT%d DDC monitor info ******************************\n";
#if XF86_VERSION_CURRENT < XF86_VERSION_NUMERIC(4,3,99,14,0)
    static const char *subshstr = "Substituting missing CRT%d monitor HSync data by DDC data\n";
    static const char *subsvstr = "Substituting missing CRT%d monitor VRefresh data by DDC data\n";
#endif
#ifdef XGIMERGED
    static const char *mergednocrt1 = "CRT1 not detected or forced off. %s.\n";
    static const char *mergednocrt2 = "No CRT2 output selected or no bridge detected. %s.\n";
    static const char *mergeddisstr = "MergedFB mode disabled";
    static const char *modesforstr = "Modes for CRT%d: *********************************************\n";
    static const char *crtsetupstr = "------------------------ CRT%d setup -------------------------\n";
#endif
/* #if defined(XGIDUALHEAD) || defined(XGIMERGED)
    static const char *notsuitablestr = "Not using mode \"%s\" (not suitable for %s mode)\n";
#endif */

    if(flags & PROBE_DETECT) {
       if((pVBEModule = xf86LoadVBEModule(pScrn))) {
          int index = xf86GetEntityInfo(pScrn->entityList[0])->index;

          xf86LoaderModReqSymLists(pVBEModule, vbeSymbols, NULL);
#if XF86_VERSION_CURRENT < XF86_VERSION_NUMERIC(4,2,99,0,0)
	  if((pVbe = VBEInit(NULL,index))) {
#else
          if((pVbe = VBEExtendedInit(NULL,index,0))) {
#endif
             ConfiguredMonitor = vbeDoEDID(pVbe, NULL);
             vbeFree(pVbe);
          }
       }
       return TRUE;
    }

    /*
     * Note: This function is only called once at server startup, and
     * not at the start of each server generation.  This means that
     * only things that are persistent across server generations can
     * be initialised here.  xf86Screens[] is the array of all screens,
     * (pScrn is a pointer to one of these).  Privates allocated using
     * xf86AllocateScrnInfoPrivateIndex() are too, and should be used
     * for data that must persist across server generations.
     *
     * Per-generation data should be allocated with
     * AllocateScreenPrivateIndex() from the ScreenInit() function.
     */

    /* Check the number of entities, and fail if it isn't one. */
    if(pScrn->numEntities != 1) {
       XGIErrorLog(pScrn, "Number of entities is not 1\n");
       return FALSE;
    }

    /* The vgahw module should be loaded here when needed */
    if(!(pMod = xf86LoadSubModule(pScrn, "vgahw"))) {
       XGIErrorLog(pScrn, "Could not load vgahw module\n");
       return FALSE;
    }

    xf86LoaderModReqSymLists(pMod, vgahwSymbols, NULL);

    /* Due to the liberal license terms this is needed for
     * keeping the copyright notice readable and intact in
     * binary distributions. Removing this is a copyright
     * infringement. Please read the license terms above.
     */

    xf86DrvMsg(pScrn->scrnIndex, X_INFO,
        "XGI driver (%d/%02d/%02d-%d)\n",
	XGIDRIVERVERSIONYEAR + 2000, XGIDRIVERVERSIONMONTH,
	XGIDRIVERVERSIONDAY, XGIDRIVERREVISION);
    xf86DrvMsg(pScrn->scrnIndex, X_INFO,
	"Copyright (C) 2001-2004 Thomas Winischhofer <thomas@winischhofer.net> and others\n");
    xf86DrvMsg(pScrn->scrnIndex, X_INFO,
        "Compiled for XFree86 %d.%d.%d.%d\n",
	XF86_VERSION_MAJOR, XF86_VERSION_MINOR,
	XF86_VERSION_PATCH, XF86_VERSION_SNAP);
#if XF86_VERSION_CURRENT >= XF86_VERSION_NUMERIC(4,2,99,0,0)
    if(xf86GetVersion() != XF86_VERSION_CURRENT) {
       xf86DrvMsg(pScrn->scrnIndex, X_WARNING,
         "This version of the driver is not compiled for this version of XFree86!\n");
    }
#endif

    /* Allocate a vgaHWRec */
    if(!vgaHWGetHWRec(pScrn)) {
       XGIErrorLog(pScrn, "Could not allocate VGA private\n");
       return FALSE;
    }

    /* Allocate the XGIRec driverPrivate */
    if(!XGIGetRec(pScrn)) {
       XGIErrorLog(pScrn, "Could not allocate memory for pXGI private\n");
       return FALSE;
    }
    pXGI = XGIPTR(pScrn);
    pXGI->pScrn = pScrn;

#if XF86_VERSION_CURRENT < XF86_VERSION_NUMERIC(4,2,99,0,0)
    pXGI->IODBase = 0;
#else
    pXGI->IODBase = pScrn->domainIOBase;
#endif

    /* Get the entity, and make sure it is PCI. */
    pXGI->pEnt = xf86GetEntityInfo(pScrn->entityList[0]);
    if(pXGI->pEnt->location.type != BUS_PCI)  {
       XGIErrorLog(pScrn, "Entity's bus type is not PCI\n");
       XGIFreeRec(pScrn);
       return FALSE;
    }

#ifdef XGIDUALHEAD
    /* Allocate an entity private if necessary */
    if(xf86IsEntityShared(pScrn->entityList[0])) {
       pXGIEnt = xf86GetEntityPrivate(pScrn->entityList[0],
					XGIEntityIndex)->ptr;
       pXGI->entityPrivate = pXGIEnt;

       /* If something went wrong, quit here */
       if((pXGIEnt->DisableDual) || (pXGIEnt->ErrorAfterFirst)) {
	  XGIErrorLog(pScrn, "First head encountered fatal error, can't continue\n");
	  XGIFreeRec(pScrn);
	  return FALSE;
       }
    }
#endif

    /* Find the PCI info for this screen */
    pXGI->PciInfo = xf86GetPciInfoForEntity(pXGI->pEnt->index);
    pXGI->PciTag = pXGI->xgi_HwDevExt.PciTag = pciTag(pXGI->PciInfo->bus,
                           pXGI->PciInfo->device, pXGI->PciInfo->func);

    pXGI->Primary = xf86IsPrimaryPci(pXGI->PciInfo);
    xf86DrvMsg(pScrn->scrnIndex, X_PROBED,
    	"This adapter is %s display adapter\n",
	(pXGI->Primary ? "primary" : "secondary"));

    if(pXGI->Primary) {
       VGAHWPTR(pScrn)->MapSize = 0x10000;     /* Standard 64k VGA window */
       if(!vgaHWMapMem(pScrn)) {
          XGIErrorLog(pScrn, "Could not map VGA memory\n");
          XGIFreeRec(pScrn);
          return FALSE;
       }
    }
    vgaHWGetIOBase(VGAHWPTR(pScrn));

    /* We "patch" the PIOOffset inside vgaHW in order to force
     * the vgaHW module to use our relocated i/o ports.
     */
    VGAHWPTR(pScrn)->PIOOffset = pXGI->IODBase + (pXGI->PciInfo->ioBase[2] & 0xFFFC) - 0x380;

    pXGI->pInt = NULL;
    if(!pXGI->Primary) {
#if !defined(__alpha__)
       xf86DrvMsg(pScrn->scrnIndex, X_INFO,
       		"Initializing display adapter through int10\n");
#endif
       if((pMod = xf86LoadSubModule(pScrn, "int10"))) {
          xf86LoaderModReqSymLists(pMod, int10Symbols, NULL);
#if !defined(__alpha__)
          pXGI->pInt = xf86InitInt10(pXGI->pEnt->index);
#endif
       } else {
          XGIErrorLog(pScrn, "Could not load int10 module\n");
       }
    }

#if XF86_VERSION_CURRENT < XF86_VERSION_NUMERIC(4,2,99,0,0)
    {
        resRange vgamem[] = {   {ResShrMemBlock,0xA0000,0xAFFFF},
                                {ResShrMemBlock,0xB0000,0xB7FFF},
                                {ResShrMemBlock,0xB8000,0xBFFFF},
                            _END };
        xf86SetOperatingState(vgamem, pXGI->pEnt->index, ResUnusedOpr);
    }
#else
    xf86SetOperatingState(resVgaMem, pXGI->pEnt->index, ResUnusedOpr);
#endif

    /* Operations for which memory access is required */
    pScrn->racMemFlags = RAC_FB | RAC_COLORMAP | RAC_CURSOR | RAC_VIEWPORT;
    /* Operations for which I/O access is required */
    pScrn->racIoFlags = RAC_COLORMAP | RAC_CURSOR | RAC_VIEWPORT;

    /* The ramdac module should be loaded here when needed */
    if(!(pMod = xf86LoadSubModule(pScrn, "ramdac"))) {
       XGIErrorLog(pScrn, "Could not load ramdac module\n");
#ifdef XGIDUALHEAD
       if(pXGIEnt) pXGIEnt->ErrorAfterFirst = TRUE;
#endif
       if(pXGI->pInt) xf86FreeInt10(pXGI->pInt);
       XGIFreeRec(pScrn);
       return FALSE;
    }

    xf86LoaderModReqSymLists(pMod, ramdacSymbols, NULL);

    /* Set pScrn->monitor */
    pScrn->monitor = pScrn->confScreen->monitor;

    /*
     * Set the Chipset and ChipRev, allowing config file entries to
     * override. DANGEROUS!
     */
    if(pXGI->pEnt->device->chipset && *pXGI->pEnt->device->chipset)  {
PDEBUG(ErrorF(" --- Chipset 1 \n"));
       pScrn->chipset = pXGI->pEnt->device->chipset;
       pXGI->Chipset = xf86StringToToken(XGIChipsets, pScrn->chipset);
       from = X_CONFIG;
    } else if(pXGI->pEnt->device->chipID >= 0) {
PDEBUG(ErrorF(" --- Chipset 2 \n"));
       pXGI->Chipset = pXGI->pEnt->device->chipID;
       pScrn->chipset = (char *)xf86TokenToString(XGIChipsets, pXGI->Chipset);

       from = X_CONFIG;
       xf86DrvMsg(pScrn->scrnIndex, X_CONFIG, "ChipID override: 0x%04X\n",
                                pXGI->Chipset);
    } else {
PDEBUG(ErrorF(" --- Chipset 3 \n"));
       from = X_PROBED;
       pXGI->Chipset = pXGI->PciInfo->chipType;
       pScrn->chipset = (char *)xf86TokenToString(XGIChipsets, pXGI->Chipset);
    }
    if(pXGI->pEnt->device->chipRev >= 0) {
       pXGI->ChipRev = pXGI->pEnt->device->chipRev;
       xf86DrvMsg(pScrn->scrnIndex, X_CONFIG, "ChipRev override: %d\n",
                        pXGI->ChipRev);
    } else {
       pXGI->ChipRev = pXGI->PciInfo->chipRev;
    }
    pXGI->xgi_HwDevExt.jChipRevision = pXGI->ChipRev;

PDEBUG(ErrorF(" --- Chipset : %s \n", pScrn->chipset));


    pXGI->XGI6326Flags = 0;

    /*
     * This shouldn't happen because such problems should be caught in
     * XGIProbe(), but check it just in case.
     */
    if(pScrn->chipset == NULL) {
       XGIErrorLog(pScrn, "ChipID 0x%04X is not recognised\n", pXGI->Chipset);
#ifdef XGIDUALHEAD
       if(pXGIEnt) pXGIEnt->ErrorAfterFirst = TRUE;
#endif
       if(pXGI->pInt) xf86FreeInt10(pXGI->pInt);
       XGIFreeRec(pScrn);
       return FALSE;
    }
    if(pXGI->Chipset < 0) {
       XGIErrorLog(pScrn, "Chipset \"%s\" is not recognised\n", pScrn->chipset);
#ifdef XGIDUALHEAD
       if(pXGIEnt) pXGIEnt->ErrorAfterFirst = TRUE;
#endif
       if(pXGI->pInt) xf86FreeInt10(pXGI->pInt);
       XGIFreeRec(pScrn);
       return FALSE;
    }

    /* Determine chipset and VGA engine type */
    pXGI->ChipFlags = 0;
    pXGI->XGI_SD_Flags = 0;
    pXGI->HWCursorMBufNum = pXGI->HWCursorCBufNum = 0;

    switch(pXGI->Chipset) {

	case PCI_CHIP_XGIXG40:
	case PCI_CHIP_XGIXG20:
		pXGI->xgi_HwDevExt.jChipType = XG40;
		pXGI->VGAEngine = XGI_XGX_VGA;
		pXGI->myCR63 = 0x63;
		pXGI->mmioSize = 64;
                break;
	default:
		pXGI->xgi_HwDevExt.jChipType = XGI_OLD;
		pXGI->VGAEngine = XGI_OLD_VGA;
		pXGI->mmioSize = 64;
		break;
    }

/* load frame_buffer */

    XGI_FbDevExist = FALSE;
    if( pXGI->Chipset != PCI_CHIP_XGIXG20 )
    {
        if( (fd = open("/dev/fb", 'r')) != -1)     
        {
            PDEBUG(ErrorF("--- open /dev/fb....   \n"));
            ioctl(fd,FBIOGET_FSCREENINFO, &fix);
   	    if (fix.accel == FB_ACCEL_XGI_GLAMOUR)
            {
		PDEBUG(ErrorF("--- fix.accel....   \n"));
                XGI_FbDevExist = TRUE;
            }
            else
		PDEBUG(ErrorF("--- no fix.accel.... 0x%08lx  \n", fix.accel));
  	    close(fd);
        }
    }

    /* Now check if xgifb is loaded. Since xgifb only supports
     * the 300 and 315 series, we only do this for these chips.
     * We use this for checking where xgifb starts its memory
     * heap in order to automatically detect the correct MaxXFBMem
     * setting (which normally is given by the option of the same name).
     * Under kernel 2.4.y, that only works if xgifb is completely
     * running, ie with a video mode because the fbdev will not be
     * installed otherwise. Under 2.5 and later, xgifb will install
     * the framebuffer device in any way and running it with mode=none
     * is no longer supported (or necessary).
     */

    pXGI->donttrustpdc = FALSE;
    pXGI->xgifbpdc = 0xff;
    pXGI->xgifbpdca = 0xff;
    pXGI->xgifblcda = 0xff;
    pXGI->xgifbscalelcd = -1;
    pXGI->xgifbspecialtiming = CUT_NONE;
    pXGI->xgifb_haveemi = FALSE;
    pXGI->OldMode = 0;
    pXGI->xgifbfound = FALSE;

    /*
     * The first thing we should figure out is the depth, bpp, etc.
     * Additionally, determine the size of the HWCursor memory area.
     */
    switch(pXGI->VGAEngine) {
      case XGI_XGX_VGA:
        pXGI->CursorSize = 4096;
    	pix24flags = Support32bppFb;
	break;
      default:
        pXGI->CursorSize = 2048;
        pix24flags = Support24bppFb;
	break;
    }

#ifdef XGIDUALHEAD
    /* In case of Dual Head, we need to determine if we are the "master" head or
     * the "slave" head. In order to do that, we set PrimInit to DONE in the
     * shared entity at the end of the first initialization. The second
     * initialization then knows that some things have already been done. THIS
     * ALWAYS ASSUMES THAT THE FIRST DEVICE INITIALIZED IS THE MASTER!
     */

    if(xf86IsEntityShared(pScrn->entityList[0])) {
       if(pXGIEnt->lastInstance > 0) {
     	  if(!xf86IsPrimInitDone(pScrn->entityList[0])) {
	     /* First Head (always CRT2) */
	     pXGI->SecondHead = FALSE;
	     pXGIEnt->pScrn_1 = pScrn;
	     pXGIEnt->CRT1ModeNo = pXGIEnt->CRT2ModeNo = -1;
	     pXGIEnt->CRT2ModeSet = FALSE;
	     pXGI->DualHeadMode = TRUE;
	     pXGIEnt->DisableDual = FALSE;
	     pXGIEnt->BIOS = NULL;
	     pXGIEnt->ROM661New = FALSE;
	     pXGIEnt->XGI_Pr = NULL;
	     pXGIEnt->RenderAccelArray = NULL;
	  } else {
	     /* Second Head (always CRT1) */
	     pXGI->SecondHead = TRUE;
	     pXGIEnt->pScrn_2 = pScrn;
	     pXGI->DualHeadMode = TRUE;
	  }
       } else {
          /* Only one screen in config file - disable dual head mode */
          pXGI->SecondHead = FALSE;
	  pXGI->DualHeadMode = FALSE;
	  pXGIEnt->DisableDual = TRUE;
       }
    } else {
       /* Entity is not shared - disable dual head mode */
       pXGI->SecondHead = FALSE;
       pXGI->DualHeadMode = FALSE;
    }
#endif

    pXGI->ForceCursorOff = FALSE;

    /* Allocate XGI_Private (for mode switching code) and initialize it */
    pXGI->XGI_Pr = NULL;
#ifdef XGIDUALHEAD
    if(pXGIEnt) {
       if(pXGIEnt->XGI_Pr) pXGI->XGI_Pr = pXGIEnt->XGI_Pr;
    }
#endif
    if(!pXGI->XGI_Pr) {
       if(!(pXGI->XGI_Pr = xnfcalloc(sizeof(XGI_Private), 1))) {
          XGIErrorLog(pScrn, "Could not allocate memory for XGI_Pr private\n");
#ifdef XGIDUALHEAD
	  if(pXGIEnt) pXGIEnt->ErrorAfterFirst = TRUE;
#endif
	  if(pXGI->pInt) xf86FreeInt10(pXGI->pInt);
	  XGIFreeRec(pScrn);
          return FALSE;
       }
#ifdef XGIDUALHEAD
       if(pXGIEnt) pXGIEnt->XGI_Pr = pXGI->XGI_Pr;
#endif
       memset(pXGI->XGI_Pr, 0, sizeof(XGI_Private));
       pXGI->XGI_Pr->XGI_Backup70xx = 0xff;
       pXGI->XGI_Pr->XGI_CHOverScan = -1;
       pXGI->XGI_Pr->XGI_ChSW = FALSE;
       pXGI->XGI_Pr->XGI_CustomT = CUT_NONE;
       pXGI->XGI_Pr->PanelSelfDetected = FALSE;
       pXGI->XGI_Pr->UsePanelScaler = -1;
       pXGI->XGI_Pr->CenterScreen = -1;
       pXGI->XGI_Pr->CRT1UsesCustomMode = FALSE;
       pXGI->XGI_Pr->PDC = pXGI->XGI_Pr->PDCA = -1;
       pXGI->XGI_Pr->LVDSHL = -1;
       pXGI->XGI_Pr->HaveEMI = FALSE;
       pXGI->XGI_Pr->HaveEMILCD = FALSE;
       pXGI->XGI_Pr->OverruleEMI = FALSE;
       pXGI->XGI_Pr->XGI_SensibleSR11 = FALSE;
       if(pXGI->xgi_HwDevExt.jChipType >= XGI_661) {
          pXGI->XGI_Pr->XGI_SensibleSR11 = TRUE;
       }
       pXGI->XGI_Pr->XGI_MyCR63 = pXGI->myCR63;
    }

    /* Get our relocated IO registers */
    pXGI->RelIO = (XGIIOADDRESS)((pXGI->PciInfo->ioBase[2] & 0xFFFC) + pXGI->IODBase);
    pXGI->xgi_HwDevExt.pjIOAddress = (XGIIOADDRESS)(pXGI->RelIO + 0x30);
    xf86DrvMsg(pScrn->scrnIndex, from, "Relocated IO registers at 0x%lX\n",
           (unsigned long)pXGI->RelIO);

    /* Initialize XGI Port Reg definitions for externally used
     * init.c/init301.c functions.
     */
    XGIRegInit(pXGI->XGI_Pr, pXGI->RelIO + 0x30);

    if(!xf86SetDepthBpp(pScrn, 0, 0, 0, pix24flags)) {
       XGIErrorLog(pScrn, "xf86SetDepthBpp() error\n");
#ifdef XGIDUALHEAD
       if(pXGIEnt) pXGIEnt->ErrorAfterFirst = TRUE;
#endif
       if(pXGI->pInt) xf86FreeInt10(pXGI->pInt);
       XGIFreeRec(pScrn);
       return FALSE;
    }

    /* Check that the returned depth is one we support */
    temp = 0;
    switch(pScrn->depth) {
       case 8:
       case 16:
       case 24:
       case 15:
          break;
       default:
	  temp = 1;
    }

    if(temp) {
       XGIErrorLog(pScrn,
               "Given color depth (%d) is not supported by this driver/chipset\n",
               pScrn->depth);
       if(pXGI->pInt) xf86FreeInt10(pXGI->pInt);
       XGIFreeRec(pScrn);
       return FALSE;
    }

    xf86PrintDepthBpp(pScrn);

    /* Get the depth24 pixmap format */
    if(pScrn->depth == 24 && pix24bpp == 0) {
       pix24bpp = xf86GetBppFromDepth(pScrn, 24);
    }

    /*
     * This must happen after pScrn->display has been set because
     * xf86SetWeight references it.
     */
    if(pScrn->depth > 8) {
        /* The defaults are OK for us */
        rgb zeros = {0, 0, 0};

        if(!xf86SetWeight(pScrn, zeros, zeros)) {
	    XGIErrorLog(pScrn, "xf86SetWeight() error\n");
#ifdef XGIDUALHEAD
	    if(pXGIEnt) pXGIEnt->ErrorAfterFirst = TRUE;
#endif
	    if(pXGI->pInt) xf86FreeInt10(pXGI->pInt);
	    XGIFreeRec(pScrn);
            return FALSE;
        } else {
           Bool ret = FALSE;
           switch(pScrn->depth) {
	   case 15:
	      if((pScrn->weight.red != 5) ||
	         (pScrn->weight.green != 5) ||
		 (pScrn->weight.blue != 5)) ret = TRUE;
	      break;
	   case 16:
	      if((pScrn->weight.red != 5) ||
	         (pScrn->weight.green != 6) ||
		 (pScrn->weight.blue != 5)) ret = TRUE;
	      break;
	   case 24:
	      if((pScrn->weight.red != 8) ||
	         (pScrn->weight.green != 8) ||
		 (pScrn->weight.blue != 8)) ret = TRUE;
	      break;
           }
	   if(ret) {
	      XGIErrorLog(pScrn,
	      	"RGB weight %d%d%d at depth %d not supported by hardware\n",
		(int)pScrn->weight.red, (int)pScrn->weight.green,
		(int)pScrn->weight.blue, pScrn->depth);
#ifdef XGIDUALHEAD
	      if(pXGIEnt) pXGIEnt->ErrorAfterFirst = TRUE;
#endif
	      if(pXGI->pInt) xf86FreeInt10(pXGI->pInt);
	      XGIFreeRec(pScrn);
              return FALSE;
	   }
        }
    }

    /* Set the current layout parameters */
    pXGI->CurrentLayout.bitsPerPixel = pScrn->bitsPerPixel;
    pXGI->CurrentLayout.depth        = pScrn->depth;
    /* (Inside this function, we can use pScrn's contents anyway) */

    if(!xf86SetDefaultVisual(pScrn, -1)) {
        XGIErrorLog(pScrn, "xf86SetDefaultVisual() error\n");
#ifdef XGIDUALHEAD
	if(pXGIEnt) pXGIEnt->ErrorAfterFirst = TRUE;
#endif
	if(pXGI->pInt) xf86FreeInt10(pXGI->pInt);
        XGIFreeRec(pScrn);
        return FALSE;
    } else {
        /* We don't support DirectColor at > 8bpp */
        if(pScrn->depth > 8 && pScrn->defaultVisual != TrueColor) {
            XGIErrorLog(pScrn,
	       	"Given default visual (%s) is not supported at depth %d\n",
                xf86GetVisualName(pScrn->defaultVisual), pScrn->depth);
#ifdef XGIDUALHEAD
	    if(pXGIEnt) pXGIEnt->ErrorAfterFirst = TRUE;
#endif
	    if(pXGI->pInt) xf86FreeInt10(pXGI->pInt);
	    XGIFreeRec(pScrn);
            return FALSE;
        }
    }

#ifdef XGIDUALHEAD
    /* Due to palette & timing problems we don't support 8bpp in DHM */
    if((pXGI->DualHeadMode) && (pScrn->bitsPerPixel == 8)) {
       XGIErrorLog(pScrn, "Color depth 8 not supported in Dual Head mode.\n");
       if(pXGIEnt) pXGIEnt->ErrorAfterFirst = TRUE;
       if(pXGI->pInt) xf86FreeInt10(pXGI->pInt);
       XGIFreeRec(pScrn);
       return FALSE;
    }
#endif

    /*
     * The cmap layer needs this to be initialised.
     */
    {
        Gamma zeros = {0.0, 0.0, 0.0};

        if(!xf86SetGamma(pScrn, zeros)) {
	    XGIErrorLog(pScrn, "xf86SetGamma() error\n");
#ifdef XGIDUALHEAD
	    if(pXGIEnt) pXGIEnt->ErrorAfterFirst = TRUE;
#endif
	    if(pXGI->pInt) xf86FreeInt10(pXGI->pInt);
	    XGIFreeRec(pScrn);
            return FALSE;
        }
    }

    /* We use a programamble clock */
    pScrn->progClock = TRUE;

    /* Set the bits per RGB for 8bpp mode */
    if(pScrn->depth == 8) {
       pScrn->rgbBits = 6;
    }

    from = X_DEFAULT;

    /* Unlock registers */
    xgiSaveUnlockExtRegisterLock(pXGI, &srlockReg, &crlockReg);

    /* Read BIOS for 300 and 315/330 series customization */
    pXGI->xgi_HwDevExt.pjVirtualRomBase = NULL;
    pXGI->BIOS = NULL;
    pXGI->xgi_HwDevExt.UseROM = FALSE;
    pXGI->ROM661New = FALSE;

    /* Evaluate options */
    xgiOptions(pScrn);

#ifdef XGIMERGED
    /* Due to palette & timing problems we don't support 8bpp in MFBM */
    if((pXGI->MergedFB) && (pScrn->bitsPerPixel == 8)) {
       XGIErrorLog(pScrn, "MergedFB: Color depth 8 not supported, %s\n", mergeddisstr);
       pXGI->MergedFB = pXGI->MergedFBAuto = FALSE;
    }
#endif

    /* Do basic configuration */
    XGISetup(pScrn);

    from = X_PROBED;
    if(pXGI->pEnt->device->MemBase != 0) {
       /*
        * XXX Should check that the config file value matches one of the
        * PCI base address values.
        */
       pXGI->FbAddress = pXGI->pEnt->device->MemBase;
       from = X_CONFIG;
    } else {
       pXGI->FbAddress = pXGI->PciInfo->memBase[0] & 0xFFFFFFF0;
    }

    pXGI->realFbAddress = pXGI->FbAddress;

#ifdef XGIDUALHEAD
    if(pXGI->DualHeadMode)
       xf86DrvMsg(pScrn->scrnIndex, from, "Global linear framebuffer at 0x%lX\n",
           (unsigned long)pXGI->FbAddress);
    else
#endif
       xf86DrvMsg(pScrn->scrnIndex, from, "Linear framebuffer at 0x%lX\n",
           (unsigned long)pXGI->FbAddress);

    if(pXGI->pEnt->device->IOBase != 0) {
        /*
         * XXX Should check that the config file value matches one of the
         * PCI base address values.
         */
       pXGI->IOAddress = pXGI->pEnt->device->IOBase;
       from = X_CONFIG;
    } else {
       pXGI->IOAddress = pXGI->PciInfo->memBase[1] & 0xFFFFFFF0;
    }

    xf86DrvMsg(pScrn->scrnIndex, from, "MMIO registers at 0x%lX (size %ldK)\n",
           (unsigned long)pXGI->IOAddress, pXGI->mmioSize);
    pXGI->xgi_HwDevExt.bIntegratedMMEnabled = TRUE;

    /* Register the PCI-assigned resources. */
    if(xf86RegisterResources(pXGI->pEnt->index, NULL, ResExclusive)) {
       XGIErrorLog(pScrn, "xf86RegisterResources() found resource conflicts\n");
#ifdef XGIDUALHEAD
       if(pXGIEnt) pXGIEnt->ErrorAfterFirst = TRUE;
#endif
       if(pXGI->pInt) xf86FreeInt10(pXGI->pInt);
       xgiRestoreExtRegisterLock(pXGI,srlockReg,crlockReg);
       XGIFreeRec(pScrn);
       return FALSE;
    }

    from = X_PROBED;
    if(pXGI->pEnt->device->videoRam != 0) {

          xf86DrvMsg(pScrn->scrnIndex, X_WARNING,
	  	"Option \"VideoRAM\" ignored\n");
    }

    pXGI->RealVideoRam = pScrn->videoRam;

        xf86DrvMsg(pScrn->scrnIndex, from, "VideoRAM: %d KB\n",
               pScrn->videoRam);

    pXGI->FbMapSize = pXGI->availMem = pScrn->videoRam * 1024;
    pXGI->xgi_HwDevExt.ulVideoMemorySize = pScrn->videoRam * 1024;
    pXGI->xgi_HwDevExt.bSkipDramSizing = TRUE;

    /* Calculate real availMem according to Accel/TurboQueue and
     * HWCursur setting. Also, initialize some variables used
     * in other modules.
     */

    pXGI->cursorOffset = 0;
    pXGI->CurARGBDest = NULL;
    pXGI->CurMonoSrc = NULL;
    pXGI->CurFGCol = pXGI->CurBGCol = 0;

    switch(pXGI->VGAEngine) {
      default:
        /* cursorOffset not used in cursor functions for 530 and
	 * older chips, because the cursor is *above* the TQ.
	 * On 5597 and older revisions of the 6326, the TQ is
	 * max 32K, on newer 6326 revisions and the 530 either 30
	 * (or 32?) or 62K (or 64?). However, to make sure, we
	 * use only 30K (or 32?), but reduce the available memory
	 * by 64, and locate the TQ at the beginning of this last
	 * 64K block. (We do this that way even when using the
	 * HWCursor, because the cursor only takes 2K and the
	 * queue does not seem to last that far anyway.)
	 * The TQ must be located at 32KB boundaries.
	 */
	if(pXGI->RealVideoRam < 3072) {
	   if(pXGI->TurboQueue) {
	      xf86DrvMsg(pScrn->scrnIndex, X_INFO,
		    "Not enough video RAM for TurboQueue. TurboQueue disabled\n");
	      pXGI->TurboQueue = FALSE;
	   }
	}
	pXGI->CmdQueMaxLen = 32;
     	if(pXGI->TurboQueue) {
	              	      pXGI->availMem -= (64*1024);
			      pXGI->CmdQueMaxLen = 900;   /* To make sure; should be 992 */
	} else if(pXGI->HWCursor) {
	                      pXGI->availMem -= pXGI->CursorSize;
	}

        pXGI->CmdQueLenMask = (pXGI->TurboQueue) ? 0x7FFF : 0x003F;

	/* This is to be subtracted from MMIO queue length register contents
	 * for getting the real Queue length.
	 */
	pXGI->CmdQueLenFix  = (pXGI->TurboQueue) ? 32 : 0;
    }

#ifdef XGIDUALHEAD
    /* In dual head mode, we share availMem equally - so align it
     * to 8KB; this way, the address of the FB of the second
     * head is aligned to 4KB for mapping.
     */
   if(pXGI->DualHeadMode)
      pXGI->availMem &= 0xFFFFE000;
#endif

    /* Check MaxXFBMem setting */
#ifdef XGIDUALHEAD
    /* Since DRI is not supported in dual head mode, we
       don't need the MaxXFBMem setting. */
    if(pXGI->DualHeadMode) {
       if(pXGI->maxxfbmem) {
	  xf86DrvMsg(pScrn->scrnIndex, X_INFO,
		"MaxXFBMem not used in Dual Head mode. Using all VideoRAM.\n");
       }
       pXGI->maxxfbmem = pXGI->availMem;
    } else
#endif
       if(pXGI->maxxfbmem) {
    	  if(pXGI->maxxfbmem > pXGI->availMem) {
	     if(pXGI->xgifbMem) {
	        pXGI->maxxfbmem = pXGI->xgifbMem * 1024;
	        xf86DrvMsg(pScrn->scrnIndex, X_WARNING,
             	   	"Invalid MaxXFBMem setting. Using xgifb heap start information\n");
	     } else {
	        xf86DrvMsg(pScrn->scrnIndex, X_WARNING,
                	"Invalid MaxXFBMem setting. Using all VideoRAM for framebuffer\n");
	        pXGI->maxxfbmem = pXGI->availMem;
	     }
	  } else if(pXGI->xgifbMem) {
	     if(pXGI->maxxfbmem > pXGI->xgifbMem * 1024) {
	        xf86DrvMsg(pScrn->scrnIndex, X_WARNING,
	       		"MaxXFBMem beyond xgifb heap start. Using xgifb heap start\n");
                pXGI->maxxfbmem = pXGI->xgifbMem * 1024;
	     }
	  }
    } else if(pXGI->xgifbMem) {
       pXGI->maxxfbmem = pXGI->xgifbMem * 1024;
    }
    else pXGI->maxxfbmem = pXGI->availMem;

    xf86DrvMsg(pScrn->scrnIndex, X_INFO, "Using %ldK of framebuffer memory\n",
    				pXGI->maxxfbmem / 1024);

    /* There are some machines out there which require a special
     * setup of the GPIO registers in order to make the Chrontel
     * work. Try to find out if we're running on such a machine.
     * Furthermore, there is some highly customized hardware,
     * which requires some non-standard LVDS timing. Since the
     * vendors don't seem to care about PCI subsystem ID's we
     * need to find out using the BIOS version and date strings.
     */
    pXGI->XGI_Pr->XGI_ChSW = FALSE;

    if(pXGI->XGI_Pr->XGI_CustomT == CUT_NONE) {
       int i = 0, j;
       unsigned short bversptr = 0;
       BOOLEAN footprint;
       unsigned long chksum = 0;

       if(pXGI->xgi_HwDevExt.UseROM) {
          bversptr = pXGI->BIOS[0x16] | (pXGI->BIOS[0x17] << 8);
          for(i=0; i<32768; i++) chksum += pXGI->BIOS[i];
       }

       i = 0;
       do {
	  if( (mycustomttable[i].chipID == pXGI->xgi_HwDevExt.jChipType)                 &&
	      ((!strlen(mycustomttable[i].biosversion)) ||
	       (pXGI->xgi_HwDevExt.UseROM &&
	       (!strncmp(mycustomttable[i].biosversion, (char *)&pXGI->BIOS[bversptr],
	                strlen(mycustomttable[i].biosversion)))))                     &&
	      ((!strlen(mycustomttable[i].biosdate)) ||
	       (pXGI->xgi_HwDevExt.UseROM &&
	       (!strncmp(mycustomttable[i].biosdate, (char *)&pXGI->BIOS[0x2c],
	                strlen(mycustomttable[i].biosdate)))))			      &&
	      ((!mycustomttable[i].bioschksum) ||
	       (pXGI->xgi_HwDevExt.UseROM &&
	       (mycustomttable[i].bioschksum == chksum)))			      &&
	      (mycustomttable[i].pcisubsysvendor == pXGI->PciInfo->subsysVendor)      &&
	      (mycustomttable[i].pcisubsyscard == pXGI->PciInfo->subsysCard) ) {
	     footprint = TRUE;
	     for(j=0; j<5; j++) {
	        if(mycustomttable[i].biosFootprintAddr[j]) {
		   if(pXGI->xgi_HwDevExt.UseROM) {
	              if(pXGI->BIOS[mycustomttable[i].biosFootprintAddr[j]] !=
		      				mycustomttable[i].biosFootprintData[j])
		         footprint = FALSE;
		   } else footprint = FALSE;
	        }
	     }
	     if(footprint) {
	        xf86DrvMsg(pScrn->scrnIndex, X_INFO,
	           "Identified %s %s, special timing applies\n",
		   mycustomttable[i].vendorName, mycustomttable[i].cardName);
	        pXGI->XGI_Pr->XGI_CustomT = mycustomttable[i].SpecialID;
	        break;
	     }
          }
          i++;
       } while(mycustomttable[i].chipID);
    }

    /* Handle ForceCRT1 option */
    if(pXGI->forceCRT1 != -1) {
       if(pXGI->forceCRT1) pXGI->CRT1off = 0;
       else                pXGI->CRT1off = 1;
    } else                 pXGI->CRT1off = -1;

    /* Detect video bridge and sense TV/VGA2 */
    XGIVGAPreInit(pScrn);

    /* Detect CRT1 (via DDC1 and DDC2, hence via VGA port; regardless of LCDA) */
    XGICRT1PreInit(pScrn);

    /* Detect LCD (connected via CRT2, regardless of LCDA) and LCD resolution */
    XGILCDPreInit(pScrn);

    /* LCDA only supported under these conditions: */
    if(pXGI->ForceCRT1Type == CRT1_LCDA) {
       if( ((pXGI->xgi_HwDevExt.jChipType != XGI_650) &&
            (pXGI->xgi_HwDevExt.jChipType < XGI_661))     ||
	   (!(pXGI->VBFlags & (VB_301C | VB_302B | VB_301LV | VB_302LV))) ) {
          xf86DrvMsg(pScrn->scrnIndex, X_WARNING,
	  	"Chipset/Video bridge does not support LCD-via-CRT1\n");
	  pXGI->ForceCRT1Type = CRT1_VGA;
       } else if(!(pXGI->VBFlags & CRT2_LCD)) {
          xf86DrvMsg(pScrn->scrnIndex, X_WARNING,
	  	"No digitally connected LCD panel found, LCD-via-CRT1 disabled\n");
	  pXGI->ForceCRT1Type = CRT1_VGA;
       }
    }

    /* Setup SD flags */
    pXGI->XGI_SD_Flags |= XGI_SD_ADDLSUPFLAG;

    if(pXGI->VBFlags & (VB_XGITVBRIDGE | VB_CHRONTEL)) {
       pXGI->XGI_SD_Flags |= XGI_SD_SUPPORTTV;
    }

#ifdef ENABLE_YPBPR
    if(pXGI->VBFlags & (VB_301|VB_301B|VB_302B)) {
       pXGI->XGI_SD_Flags |= XGI_SD_SUPPORTHIVISION;
    }
#endif

#ifdef TWDEBUG	/* @@@ TEST @@@ */
    pXGI->XGI_SD_Flags |= XGI_SD_SUPPORTYPBPRAR;
    xf86DrvMsg(0, X_INFO, "TEST: Support Aspect Ratio\n");
#endif

    /* Detect CRT2-TV and PAL/NTSC mode */
    XGITVPreInit(pScrn);

    /* Detect CRT2-VGA */
    /* XGICRT2PreInit(pScrn); */

    /* Backup detected CRT2 devices */
    pXGI->detectedCRT2Devices = pXGI->VBFlags & (CRT2_LCD|CRT2_TV|CRT2_VGA|TV_AVIDEO|TV_SVIDEO|TV_SCART|TV_HIVISION|TV_YPBPR);

    if(!(pXGI->XGI_SD_Flags & XGI_SD_SUPPORTYPBPR)) {
       if((pXGI->ForceTVType != -1) && (pXGI->ForceTVType & TV_YPBPR)) {
          pXGI->ForceTVType = -1;
	  xf86DrvMsg(pScrn->scrnIndex, X_WARNING, "YPbPr TV output not supported\n");
       }
    }

    if(!(pXGI->XGI_SD_Flags & XGI_SD_SUPPORTHIVISION)) {
       if((pXGI->ForceTVType != -1) && (pXGI->ForceTVType & TV_HIVISION)) {
          pXGI->ForceTVType = -1;
	  xf86DrvMsg(pScrn->scrnIndex, X_WARNING, "HiVision TV output not supported\n");
       }
    }

    if((pXGI->VBFlags & VB_XGITVBRIDGE) ||
       ((pXGI->VBFlags & VB_CHRONTEL) && (pXGI->ChrontelType == CHRONTEL_701x))) {
       pXGI->XGI_SD_Flags |= (XGI_SD_SUPPORTPALMN | XGI_SD_SUPPORTNTSCJ);
    }
    if((pXGI->VBFlags & VB_XGITVBRIDGE) ||
       ((pXGI->VBFlags & VB_CHRONTEL) && (pXGI->ChrontelType == CHRONTEL_700x))) {
       pXGI->XGI_SD_Flags |= XGI_SD_SUPPORTTVPOS;
    }
    if(pXGI->VBFlags & (VB_301|VB_301B|VB_301C|VB_302B)) {
       pXGI->XGI_SD_Flags |= (XGI_SD_SUPPORTSCART | XGI_SD_SUPPORTVGA2);
    }
    if(pXGI->VBFlags & VB_CHRONTEL) {
       pXGI->XGI_SD_Flags |= XGI_SD_SUPPORTOVERSCAN;
       if(pXGI->ChrontelType == CHRONTEL_700x) {
          pXGI->XGI_SD_Flags |= XGI_SD_SUPPORTSOVER;
       }
    }

    if( ((pXGI->xgi_HwDevExt.jChipType == XGI_650) ||
         (pXGI->xgi_HwDevExt.jChipType >= XGI_661))                    &&
        (pXGI->VBFlags & (VB_301C | VB_302B | VB_301LV | VB_302LV)) &&
        (pXGI->VBFlags & CRT2_LCD) 			            &&
	(pXGI->VESA != 1) ) {
       pXGI->XGI_SD_Flags |= XGI_SD_SUPPORTLCDA;
    } else {
       /* Paranoia */
       pXGI->ForceCRT1Type = CRT1_VGA;
    }

    pXGI->VBFlags |= pXGI->ForceCRT1Type;

#ifdef TWDEBUG
    xf86DrvMsg(0, X_INFO, "SDFlags %lx\n", pXGI->XGI_SD_Flags);
#endif


#ifdef XGIDUALHEAD
    if((!pXGI->DualHeadMode) || (pXGI->SecondHead)) {
#endif
       xf86DrvMsg(pScrn->scrnIndex, pXGI->CRT1gammaGiven ? X_CONFIG : X_INFO,
       	     "CRT1 gamma correction is %s\n",
             pXGI->CRT1gamma ? "enabled" : "disabled");

#ifdef XGIDUALHEAD
    }
#endif

    /* Eventually overrule TV Type (SVIDEO, COMPOSITE, SCART, HIVISION, YPBPR) */
    if(pXGI->VBFlags & VB_XGITVBRIDGE) {
       if(pXGI->ForceTVType != -1) {
    	  pXGI->VBFlags &= ~(TV_INTERFACE);
	  if(!(pXGI->VBFlags & VB_CHRONTEL)) {
	     pXGI->VBFlags &= ~(TV_CHSCART | TV_CHYPBPR525I);
	  }
	  pXGI->VBFlags |= pXGI->ForceTVType;
	  if(pXGI->VBFlags & TV_YPBPR) {
	     pXGI->VBFlags &= ~(TV_STANDARD);
	     pXGI->VBFlags &= ~(TV_YPBPRAR);
	     pXGI->VBFlags |= pXGI->ForceYPbPrType;
	     pXGI->VBFlags |= pXGI->ForceYPbPrAR;
	  }
       }
    }

    /* Handle ForceCRT1 option (part 2) */
    pXGI->CRT1changed = FALSE;

    /* Check if CRT1 used (or needed; this eg. if no CRT2 detected) */
    if(pXGI->VBFlags & VB_VIDEOBRIDGE) {

        /* No CRT2 output? Then we NEED CRT1!
	 * We also need CRT1 if depth = 8 and bridge=LVDS|301B-DH
	 */
        if( (!(pXGI->VBFlags & (CRT2_VGA | CRT2_LCD | CRT2_TV))) ||
	    ( (pScrn->bitsPerPixel == 8) &&
	      ( (pXGI->VBFlags & (VB_LVDS | VB_CHRONTEL)) ||
	        ((pXGI->VBFlags & VB_30xBDH) && (pXGI->VBFlags & CRT2_LCD)) ) ) ) {
	    pXGI->CRT1off = 0;
	}
	/* No CRT2 output? Then we can't use Xv on CRT2 */
	if(!(pXGI->VBFlags & (CRT2_VGA | CRT2_LCD | CRT2_TV)))
	    pXGI->XvOnCRT2 = FALSE;

    } else { /* no video bridge? */

        /* Then we NEED CRT1... */
        pXGI->CRT1off = 0;
	/* ... and can't use CRT2 for Xv output */
	pXGI->XvOnCRT2 = FALSE;
    }

    /* LCDA? Then we don't switch off CRT1 */
    if(pXGI->VBFlags & CRT1_LCDA) pXGI->CRT1off = 0;

    /* Handle TVStandard option */
    if((pXGI->NonDefaultPAL != -1) || (pXGI->NonDefaultNTSC != -1)) {
       if( (!(pXGI->VBFlags & VB_XGITVBRIDGE)) &&
	   (!((pXGI->VBFlags & VB_CHRONTEL)) && (pXGI->ChrontelType == CHRONTEL_701x)) ) {
	  xf86DrvMsg(pScrn->scrnIndex, X_INFO,
	   	"PALM, PALN and NTSCJ not supported on this hardware\n");
 	  pXGI->NonDefaultPAL = pXGI->NonDefaultNTSC = -1;
	  pXGI->VBFlags &= ~(TV_PALN | TV_PALM | TV_NTSCJ);
	  pXGI->XGI_SD_Flags &= ~(XGI_SD_SUPPORTPALMN | XGI_SD_SUPPORTNTSCJ);
       }
    }

#ifdef XGI_CP
    XGI_CP_DRIVER_RECONFIGOPT
#endif

    /* Do some checks */
    if(pXGI->OptTVOver != -1) {
       if(pXGI->VBFlags & VB_CHRONTEL) {
	  pXGI->UseCHOverScan = pXGI->OptTVOver;
       } else {
	  xf86DrvMsg(pScrn->scrnIndex, X_INFO,
	    	"CHTVOverscan only supported on CHRONTEL 70xx\n");
          pXGI->UseCHOverScan = -1;
       }
    } else pXGI->UseCHOverScan = -1;

    if(pXGI->xgitvedgeenhance != -1) {
       if(!(pXGI->VBFlags & VB_301)) {
	  xf86DrvMsg(pScrn->scrnIndex, X_INFO,
	       "XGITVEdgeEnhance only supported on XGI301\n");
	  pXGI->xgitvedgeenhance = -1;
       }
    }
    if(pXGI->xgitvsaturation != -1) {
       if(pXGI->VBFlags & VB_301) {
	  xf86DrvMsg(pScrn->scrnIndex, X_INFO,
	       "XGITVSaturation not supported on XGI301\n");
	  pXGI->xgitvsaturation = -1;
       }
    }

    /* Do some MergedFB mode initialisation */
#ifdef XGIMERGED
    if(pXGI->MergedFB) {
       pXGI->CRT2pScrn = xalloc(sizeof(ScrnInfoRec));
       if(!pXGI->CRT2pScrn) {
          XGIErrorLog(pScrn, "Failed to allocate memory for 2nd pScrn, %s\n", mergeddisstr);
	  pXGI->MergedFB = FALSE;
       } else {
          memcpy(pXGI->CRT2pScrn, pScrn, sizeof(ScrnInfoRec));
       }
    }
#endif


    /* Determine CRT1<>CRT2 mode
     *     Note: When using VESA or if the bridge is in slavemode, display
     *           is ALWAYS in MIRROR_MODE!
     *           This requires extra checks in functions using this flag!
     *           (see xgi_video.c for example)
     */
    if(pXGI->VBFlags & DISPTYPE_DISP2) {
        if(pXGI->CRT1off) {	/* CRT2 only ------------------------------- */
#ifdef XGIDUALHEAD
	     if(pXGI->DualHeadMode) {
	     	XGIErrorLog(pScrn,
		    "CRT1 not detected or forced off. Dual Head mode can't initialize.\n");
	     	if(pXGIEnt) pXGIEnt->DisableDual = TRUE;
	        if(pXGIEnt) pXGIEnt->ErrorAfterFirst = TRUE;
		if(pXGI->pInt) xf86FreeInt10(pXGI->pInt);
		pXGI->pInt = NULL;
		xgiRestoreExtRegisterLock(pXGI,srlockReg,crlockReg);
		XGIFreeRec(pScrn);
		return FALSE;
	     }
#endif
#ifdef XGIMERGED
	     if(pXGI->MergedFB) {
	        if(pXGI->MergedFBAuto) {
		   xf86DrvMsg(pScrn->scrnIndex, X_INFO, mergednocrt1, mergeddisstr);
		} else {
	     	   XGIErrorLog(pScrn, mergednocrt1, mergeddisstr);
		}
		if(pXGI->CRT2pScrn) xfree(pXGI->CRT2pScrn);
		pXGI->CRT2pScrn = NULL;
		pXGI->MergedFB = FALSE;
	     }
#endif
	     pXGI->VBFlags |= VB_DISPMODE_SINGLE;
	     /* No CRT1? Then we use the video overlay on CRT2 */
	     pXGI->XvOnCRT2 = TRUE;
	} else			/* CRT1 and CRT2 - mirror or dual head ----- */
#ifdef XGIDUALHEAD
	     if(pXGI->DualHeadMode) {
		pXGI->VBFlags |= (VB_DISPMODE_DUAL | DISPTYPE_CRT1);
	        if(pXGI->VESA != -1) {
		    xf86DrvMsg(pScrn->scrnIndex, X_INFO,
			"VESA option not used in Dual Head mode. VESA disabled.\n");
		}
		if(pXGIEnt) pXGIEnt->DisableDual = FALSE;
		pXGI->VESA = 0;
	     } else
#endif
#ifdef XGIMERGED
	            if(pXGI->MergedFB) {
		 pXGI->VBFlags |= (VB_DISPMODE_MIRROR | DISPTYPE_CRT1);
		 if(pXGI->VESA != -1) {
		    xf86DrvMsg(pScrn->scrnIndex, X_INFO,
			"VESA option not used in MergedFB mode. VESA disabled.\n");
		 }
		 pXGI->VESA = 0;
	     } else
#endif
		 pXGI->VBFlags |= (VB_DISPMODE_MIRROR | DISPTYPE_CRT1);
    } else {			/* CRT1 only ------------------------------- */
#ifdef XGIDUALHEAD
	     if(pXGI->DualHeadMode) {
	     	XGIErrorLog(pScrn,
		   "No CRT2 output selected or no bridge detected. "
		   "Dual Head mode can't initialize.\n");
	        if(pXGIEnt) pXGIEnt->ErrorAfterFirst = TRUE;
		if(pXGI->pInt) xf86FreeInt10(pXGI->pInt);
		pXGI->pInt = NULL;
		xgiRestoreExtRegisterLock(pXGI,srlockReg,crlockReg);
		XGIFreeRec(pScrn);
		return FALSE;
	     }
#endif
#ifdef XGIMERGED
	     if(pXGI->MergedFB) {
	        if(pXGI->MergedFBAuto) {
		   xf86DrvMsg(pScrn->scrnIndex, X_INFO, mergednocrt2, mergeddisstr);
		} else {
	     	   XGIErrorLog(pScrn, mergednocrt2, mergeddisstr);
		}
		if(pXGI->CRT2pScrn) xfree(pXGI->CRT2pScrn);
		pXGI->CRT2pScrn = NULL;
		pXGI->MergedFB = FALSE;
	     }
#endif
             pXGI->VBFlags |= (VB_DISPMODE_SINGLE | DISPTYPE_CRT1);
    }

    /* Init Ptrs for Save/Restore functions and calc MaxClock */
    XGIDACPreInit(pScrn);

    /* ********** end of VBFlags setup ********** */

    /* VBFlags are initialized now. Back them up for SlaveMode modes. */
    pXGI->VBFlags_backup = pXGI->VBFlags;

    /* Find out about paneldelaycompensation and evaluate option */
#ifdef XGIDUALHEAD
    if((!pXGI->DualHeadMode) || (!pXGI->SecondHead)) {
#endif

       
#ifdef XGIDUALHEAD
    }
#endif

#ifdef XGIDUALHEAD
    /* In dual head mode, both heads (currently) share the maxxfbmem equally.
     * If memory sharing is done differently, the following has to be changed;
     * the other modules (eg. accel and Xv) use dhmOffset for hardware
     * pointer settings relative to VideoRAM start and won't need to be changed.
     */
    if(pXGI->DualHeadMode) {
        if(pXGI->SecondHead == FALSE) {
	    /* ===== First head (always CRT2) ===== */
	    /* We use only half of the memory available */
	    pXGI->maxxfbmem /= 2;
	    /* Initialize dhmOffset */
	    pXGI->dhmOffset = 0;
	    /* Copy framebuffer addresses & sizes to entity */
	    pXGIEnt->masterFbAddress = pXGI->FbAddress;
	    pXGIEnt->masterFbSize    = pXGI->maxxfbmem;
	    pXGIEnt->slaveFbAddress  = pXGI->FbAddress + pXGI->maxxfbmem;
	    pXGIEnt->slaveFbSize     = pXGI->maxxfbmem;
	    xf86DrvMsg(pScrn->scrnIndex, X_PROBED,
	    		"%ldKB video RAM at 0x%lx available for master head (CRT2)\n",
	    		pXGI->maxxfbmem/1024, pXGI->FbAddress);
	} else {
	    /* ===== Second head (always CRT1) ===== */
	    /* We use only half of the memory available */
	    pXGI->maxxfbmem /= 2;
	    /* Adapt FBAddress */
	    pXGI->FbAddress += pXGI->maxxfbmem;
	    /* Initialize dhmOffset */
	    pXGI->dhmOffset = pXGI->availMem - pXGI->maxxfbmem;
	    xf86DrvMsg(pScrn->scrnIndex, X_PROBED,
	    		"%ldKB video RAM at 0x%lx available for slave head (CRT1)\n",
	    		pXGI->maxxfbmem/1024,  pXGI->FbAddress);
	}
    } else
        pXGI->dhmOffset = 0;
#endif

    /* Note: Do not use availMem for anything from now. Use
     * maxxfbmem instead. (availMem does not take dual head
     * mode into account.)
     */

    pXGI->DRIheapstart = pXGI->maxxfbmem;
    pXGI->DRIheapend = pXGI->availMem;
#ifdef XGIDUALHEAD
    if(pXGI->DualHeadMode) {
       pXGI->DRIheapstart = pXGI->DRIheapend = 0;
    } else
#endif
    if(pXGI->DRIheapstart == pXGI->DRIheapend) {

       pXGI->DRIheapstart = pXGI->DRIheapend = 0;
    }

    /* Now for something completely different: DDC.
     * For 300 and 315/330 series, we provide our
     * own functions (in order to probe CRT2 as well)
     * If these fail, use the VBE.
     * All other chipsets will use VBE. No need to re-invent
     * the wheel there.
     */

    pXGI->pVbe = NULL;
    didddc2 = FALSE;

#ifdef XGIDUALHEAD
    /* In dual head mode, probe DDC using VBE only for CRT1 (second head) */
    if((pXGI->DualHeadMode) && (!didddc2) && (!pXGI->SecondHead))
         didddc2 = TRUE;
#endif

    if(!didddc2) {
       /* If CRT1 is off or LCDA, skip DDC via VBE */
       if((pXGI->CRT1off) || (pXGI->VBFlags & CRT1_LCDA))
          didddc2 = TRUE;
    }

    /* Now (re-)load and initialize the DDC module */
    if(!didddc2) {

       if((pDDCModule = xf86LoadSubModule(pScrn, "ddc"))) {

          xf86LoaderModReqSymLists(pDDCModule, ddcSymbols, NULL);

          /* Now load and initialize VBE module. */
          if((pVBEModule = xf86LoadVBEModule(pScrn))) {
	      xf86LoaderModReqSymLists(pVBEModule, vbeSymbols, NULL);
#if XF86_VERSION_CURRENT < XF86_VERSION_NUMERIC(4,2,99,0,0)
	      pXGI->pVbe = VBEInit(pXGI->pInt,pXGI->pEnt->index);
#else
              pXGI->pVbe = VBEExtendedInit(pXGI->pInt, pXGI->pEnt->index,
	                SET_BIOS_SCRATCH | RESTORE_BIOS_SCRATCH);
#endif
              if(!pXGI->pVbe) {
	         xf86DrvMsg(pScrn->scrnIndex, X_WARNING,
	            "Could not initialize VBE module for DDC\n");
              }
          } else {
              xf86DrvMsg(pScrn->scrnIndex, X_WARNING,
	          "Could not load VBE module for DDC\n");
          }

  	  if(pXGI->pVbe) {
	      if((pMonitor = vbeDoEDID(pXGI->pVbe,pDDCModule))) {
	         xf86DrvMsg(pScrn->scrnIndex, X_PROBED,
	   	      "VBE CRT1 DDC monitor info:\n");
                 xf86SetDDCproperties(pScrn, xf86PrintEDID(pMonitor));
		 xf86DrvMsg(pScrn->scrnIndex, X_PROBED,
	   	      "End of VBE CRT1 DDC monitor info:\n");
		 pScrn->monitor->DDC = pMonitor;
              }
          } else {
	      xf86DrvMsg(pScrn->scrnIndex, X_INFO,
			"Could not retrieve DDC data\n");
	  }
       }
    }

#ifdef XGIMERGED
    if(pXGI->MergedFB) {
       pXGI->CRT2pScrn->monitor = xalloc(sizeof(MonRec));
       if(pXGI->CRT2pScrn->monitor) {
          DisplayModePtr tempm = NULL, currentm = NULL, newm = NULL;
          memcpy(pXGI->CRT2pScrn->monitor, pScrn->monitor, sizeof(MonRec));
          pXGI->CRT2pScrn->monitor->DDC = NULL;
	  pXGI->CRT2pScrn->monitor->Modes = NULL;
	  tempm = pScrn->monitor->Modes;
	  while(tempm) {
	     if(!(newm = xalloc(sizeof(DisplayModeRec)))) break;
	     memcpy(newm, tempm, sizeof(DisplayModeRec));
	     if(!(newm->name = xalloc(strlen(tempm->name) + 1))) {
	        xfree(newm);
		break;
	     }
	     strcpy(newm->name, tempm->name);
	     if(!pXGI->CRT2pScrn->monitor->Modes) pXGI->CRT2pScrn->monitor->Modes = newm;
	     if(currentm) {
	        currentm->next = newm;
		newm->prev = currentm;
	     }
	     currentm = newm;
	     tempm = tempm->next;
	  }
	  if((pMonitor = XGIInternalDDC(pXGI->CRT2pScrn, 1))) {
	     xf86DrvMsg(pScrn->scrnIndex, X_PROBED, ddcsstr, 2);
	     xf86PrintEDID(pMonitor);
	     xf86DrvMsg(pScrn->scrnIndex, X_PROBED, ddcestr, 2);
	     xf86SetDDCproperties(pXGI->CRT2pScrn, pMonitor);
	     pXGI->CRT2pScrn->monitor->DDC = pMonitor;
	     /* use DDC data if no ranges in config file */
	     if(!pXGI->CRT2HSync) {
	        pXGI->CRT2pScrn->monitor->nHsync = 0;
	     }
	     if(!pXGI->CRT2VRefresh) {
	        pXGI->CRT2pScrn->monitor->nVrefresh = 0;
	     }
          } else {
	     xf86DrvMsg(pScrn->scrnIndex, X_PROBED,
	     	"Failed to read DDC data for CRT2\n");
	  }
       } else {
          XGIErrorLog(pScrn, "Failed to allocate memory for CRT2 monitor, %s.\n",
	  		mergeddisstr);
	  if(pXGI->CRT2pScrn) xfree(pXGI->CRT2pScrn);
    	  pXGI->CRT2pScrn = NULL;
	  pXGI->MergedFB = FALSE;
       }
    }
#endif

    /* If there is no HSync or VRefresh data for the monitor,
     * derive it from DDC data. Done by common layer since
     * 4.3.99.14.
     */
#if XF86_VERSION_CURRENT < XF86_VERSION_NUMERIC(4,3,99,14,0)
    if(pScrn->monitor->DDC) {
       if(pScrn->monitor->nHsync <= 0) {
         xf86DrvMsg(pScrn->scrnIndex, X_INFO, subshstr,
#ifdef XGIDUALHEAD
			pXGI->DualHeadMode ? (pXGI->SecondHead ? 1 : 2) :
#endif
		 		pXGI->CRT1off ? 2 : 1);
         XGISetSyncRangeFromEdid(pScrn, 1);
       }
       if(pScrn->monitor->nVrefresh <= 0) {
         xf86DrvMsg(pScrn->scrnIndex, X_INFO, subsvstr,
#ifdef XGIDUALHEAD
			pXGI->DualHeadMode ? (pXGI->SecondHead ? 1 : 2) :
#endif
		  		pXGI->CRT1off ? 2 : 1);
         XGISetSyncRangeFromEdid(pScrn, 0);
       }
    }
#endif

#ifdef XGIMERGED
    if(pXGI->MergedFB) {
#if XF86_VERSION_CURRENT < XF86_VERSION_NUMERIC(4,3,99,14,0)
       if(pXGI->CRT2pScrn->monitor->DDC) {
          if(pXGI->CRT2pScrn->monitor->nHsync <= 0) {
             xf86DrvMsg(pScrn->scrnIndex, X_INFO, subshstr, 2);
             XGISetSyncRangeFromEdid(pXGI->CRT2pScrn, 1);
          }
          if(pXGI->CRT2pScrn->monitor->nVrefresh <= 0) {
             xf86DrvMsg(pScrn->scrnIndex, X_INFO, subsvstr, 2);
             XGISetSyncRangeFromEdid(pXGI->CRT2pScrn, 0);
          }
       }
#endif

       xf86DrvMsg(pScrn->scrnIndex, X_INFO, crtsetupstr, 1);
    }
#endif
    /* end of DDC */

    /* From here, we mainly deal with clocks and modes */

    /* Set the min pixel clock */
    pXGI->MinClock = 5000;

    xf86DrvMsg(pScrn->scrnIndex, X_DEFAULT, "Min pixel clock is %d MHz\n",
                pXGI->MinClock / 1000);

    from = X_PROBED;
    /*
     * If the user has specified ramdac speed in the XF86Config
     * file, we respect that setting.
     */
    if(pXGI->pEnt->device->dacSpeeds[0]) {
        int speed = 0;
        switch(pScrn->bitsPerPixel) {
        case 8:
           speed = pXGI->pEnt->device->dacSpeeds[DAC_BPP8];
           break;
        case 16:
           speed = pXGI->pEnt->device->dacSpeeds[DAC_BPP16];
           break;
        case 24:
           speed = pXGI->pEnt->device->dacSpeeds[DAC_BPP24];
           break;
        case 32:
           speed = pXGI->pEnt->device->dacSpeeds[DAC_BPP32];
           break;
        }
        if(speed == 0)
            pXGI->MaxClock = pXGI->pEnt->device->dacSpeeds[0];
        else
            pXGI->MaxClock = speed;
        from = X_CONFIG;
    }
    xf86DrvMsg(pScrn->scrnIndex, from, "Max pixel clock is %d MHz\n",
                pXGI->MaxClock / 1000);

    /*
     * Setup the ClockRanges, which describe what clock ranges are available,
     * and what sort of modes they can be used for.
     */
    clockRanges = xnfcalloc(sizeof(ClockRange), 1);
    clockRanges->next = NULL;
    clockRanges->minClock = pXGI->MinClock;
    clockRanges->maxClock = pXGI->MaxClock;
    clockRanges->clockIndex = -1;               /* programmable */
    clockRanges->interlaceAllowed = TRUE;
    clockRanges->doubleScanAllowed = TRUE;

    /*
     * xf86ValidateModes will check that the mode HTotal and VTotal values
     * don't exceed the chipset's limit if pScrn->maxHValue and
     * pScrn->maxVValue are set.  Since our XGIValidMode() already takes
     * care of this, we don't worry about setting them here.
     */

    /* Select valid modes from those available */
    /*
     * Assuming min pitch 256, min height 128
     */
    {
       int minpitch, maxpitch, minheight, maxheight;
       minpitch = 256;
       minheight = 128;
       switch(pXGI->VGAEngine) {
       case XGI_OLD_VGA:
          maxpitch = 2040;
          maxheight = 2048;
          break;
       default:
          maxpitch = 2048;
          maxheight = 2048;
          break;
       }
#ifdef XGIMERGED
       pXGI->CheckForCRT2 = FALSE;
#endif
       i = xf86ValidateModes(pScrn, pScrn->monitor->Modes,
                      pScrn->display->modes, clockRanges, NULL,
                      minpitch, maxpitch,
                      pScrn->bitsPerPixel * 8,
		      minheight, maxheight,
                      pScrn->display->virtualX,
                      pScrn->display->virtualY,
                      pXGI->maxxfbmem,
                      LOOKUP_BEST_REFRESH);
    }

    if(i == -1) {
        XGIErrorLog(pScrn, "xf86ValidateModes() error\n");
#ifdef XGIDUALHEAD
	if(pXGIEnt) pXGIEnt->ErrorAfterFirst = TRUE;
#endif
	if(pXGI->pInt) xf86FreeInt10(pXGI->pInt);
	xgiRestoreExtRegisterLock(pXGI,srlockReg,crlockReg);
        XGIFreeRec(pScrn);
        return FALSE;
    }

    /* Check the virtual screen against the available memory */
    {
       unsigned long memreq = (pScrn->virtualX * ((pScrn->bitsPerPixel + 7) / 8)) * pScrn->virtualY;

       if(memreq > pXGI->maxxfbmem) {
          XGIErrorLog(pScrn,
       		"Virtual screen too big for memory; %ldK needed, %ldK available\n",
		memreq/1024, pXGI->maxxfbmem/1024);
#ifdef XGIDUALHEAD
          if(pXGIEnt) pXGIEnt->ErrorAfterFirst = TRUE;
#endif
          if(pXGI->pInt) xf86FreeInt10(pXGI->pInt);
          pXGI->pInt = NULL;
          xgiRestoreExtRegisterLock(pXGI,srlockReg,crlockReg);
          XGIFreeRec(pScrn);
          return FALSE;
       }
    }

    /* Dual Head:
     * -) Go through mode list and mark all those modes as bad,
     *    which are unsuitable for dual head mode.
     * -) Find the highest used pixelclock on the master head.
     */
#ifdef XGIDUALHEAD
    if(pXGI->DualHeadMode) {

       if(!pXGI->SecondHead) {

          pXGIEnt->maxUsedClock = 0;

          if((p = first = pScrn->modes)) {
             do {
	        n = p->next;

	        /* Modes that require the bridge to operate in SlaveMode
                 * are not suitable for Dual Head mode.
                 */

		/* Search for the highest clock on first head in order to calculate
	         * max clock for second head (CRT1)
	         */
		if((p->status == MODE_OK) && (p->Clock > pXGIEnt->maxUsedClock)) {
		   pXGIEnt->maxUsedClock = p->Clock;
		}

	        p = n;

             } while (p != NULL && p != first);
	  }
       }
    }
#endif

    /* Prune the modes marked as invalid */
    xf86PruneDriverModes(pScrn);

    if(i == 0 || pScrn->modes == NULL) {
        XGIErrorLog(pScrn, "No valid modes found\n");
#ifdef XGIDUALHEAD
	if(pXGIEnt) pXGIEnt->ErrorAfterFirst = TRUE;
#endif
	if(pXGI->pInt) xf86FreeInt10(pXGI->pInt);
	xgiRestoreExtRegisterLock(pXGI,srlockReg,crlockReg);
        XGIFreeRec(pScrn);
        return FALSE;
    }

    xf86SetCrtcForModes(pScrn, INTERLACE_HALVE_V);

    /* Set the current mode to the first in the list */
    pScrn->currentMode = pScrn->modes;

    /* Copy to CurrentLayout */
    pXGI->CurrentLayout.mode = pScrn->currentMode;
    pXGI->CurrentLayout.displayWidth = pScrn->displayWidth;

#ifdef XGIMERGED
    if(pXGI->MergedFB) {
       xf86DrvMsg(pScrn->scrnIndex, X_INFO, modesforstr, 1);
    }
#endif

    /* Print the list of modes being used */
    xf86PrintModes(pScrn);

#ifdef XGIMERGED
    if(pXGI->MergedFB) {
       xf86DrvMsg(pScrn->scrnIndex, X_INFO, crtsetupstr, 2);

       clockRanges->next = NULL;
       clockRanges->minClock = pXGI->MinClock;
       clockRanges->clockIndex = -1;
       clockRanges->interlaceAllowed = FALSE;
       clockRanges->doubleScanAllowed = FALSE;

       xf86DrvMsg(pScrn->scrnIndex, X_DEFAULT, "Min pixel clock for CRT2 is %d MHz\n",
                clockRanges->minClock / 1000);
       xf86DrvMsg(pScrn->scrnIndex, X_DEFAULT, "Max pixel clock for CRT2 is %d MHz\n",
                clockRanges->maxClock / 1000);

       pXGI->HaveCustomModes2 = FALSE;
    }

    if(pXGI->MergedFB) {

       pXGI->CheckForCRT2 = TRUE;
       i = xf86ValidateModes(pXGI->CRT2pScrn, pXGI->CRT2pScrn->monitor->Modes,
                      pXGI->CRT2pScrn->display->modes, clockRanges,
                      NULL, 256, 4088,
                      pXGI->CRT2pScrn->bitsPerPixel * 8, 128, 4096,
                      pScrn->display->virtualX ? pScrn->virtualX : 0,
                      pScrn->display->virtualY ? pScrn->virtualY : 0,
                      pXGI->maxxfbmem,
                      LOOKUP_BEST_REFRESH);
       pXGI->CheckForCRT2 = FALSE;

       if(i == -1) {
          XGIErrorLog(pScrn, "xf86ValidateModes() error, %s.\n", mergeddisstr);
	  XGIFreeCRT2Structs(pXGI);
          pXGI->MergedFB = FALSE;
       }

    }

    if(pXGI->MergedFB) {

       if((p = first = pXGI->CRT2pScrn->modes)) {
          do {
	     n = p->next;
	     p = n;
	  } while (p != NULL && p != first);
       }

       xf86PruneDriverModes(pXGI->CRT2pScrn);

       if(i == 0 || pXGI->CRT2pScrn->modes == NULL) {
          XGIErrorLog(pScrn, "No valid modes found for CRT2; %s\n", mergeddisstr);
	  XGIFreeCRT2Structs(pXGI);
	  pXGI->MergedFB = FALSE;
       }

    }

    if(pXGI->MergedFB) {

       xf86SetCrtcForModes(pXGI->CRT2pScrn, INTERLACE_HALVE_V);

       xf86DrvMsg(pScrn->scrnIndex, X_INFO, modesforstr, 2);

       xf86PrintModes(pXGI->CRT2pScrn);

       pXGI->CRT1Modes = pScrn->modes;
       pXGI->CRT1CurrentMode = pScrn->currentMode;

       xf86DrvMsg(pScrn->scrnIndex, X_INFO, "Generating MergedFB mode list\n");

       pScrn->modes = XGIGenerateModeList(pScrn, pXGI->MetaModes,
	            	                  pXGI->CRT1Modes, pXGI->CRT2pScrn->modes,
					  pXGI->CRT2Position);

       if(!pScrn->modes) {

	  XGIErrorLog(pScrn, "Failed to parse MetaModes or no modes found. %s.\n",
	  		mergeddisstr);
	  XGIFreeCRT2Structs(pXGI);
	  pScrn->modes = pXGI->CRT1Modes;
	  pXGI->CRT1Modes = NULL;
	  pXGI->MergedFB = FALSE;

       }

    }

    if(pXGI->MergedFB) {

       /* If no virtual dimension was given by the user,
        * calculate a sane one now. Adapts pScrn->virtualX,
	* pScrn->virtualY and pScrn->displayWidth.
	*/
       XGIRecalcDefaultVirtualSize(pScrn);

       pScrn->modes = pScrn->modes->next;  /* We get the last from GenerateModeList(), skip to first */
       pScrn->currentMode = pScrn->modes;

       /* Update CurrentLayout */
       pXGI->CurrentLayout.mode = pScrn->currentMode;
       pXGI->CurrentLayout.displayWidth = pScrn->displayWidth;

    }
#endif

    /* Set display resolution */
#ifdef XGIMERGED
    if(pXGI->MergedFB) {
       XGIMergedFBSetDpi(pScrn, pXGI->CRT2pScrn, pXGI->CRT2Position);
    } else
#endif
       xf86SetDpi(pScrn, 0, 0);

    /* Load fb module */
    switch(pScrn->bitsPerPixel) {
      case 8:
      case 16:
      case 24:
      case 32:
	if(!(pMod = xf86LoadSubModule(pScrn, "fb"))) {
           XGIErrorLog(pScrn, "Failed to load fb module");
#ifdef XGIDUALHEAD
	   if(pXGIEnt) pXGIEnt->ErrorAfterFirst = TRUE;
#endif
	   if(pXGI->pInt) xf86FreeInt10(pXGI->pInt);
	   xgiRestoreExtRegisterLock(pXGI,srlockReg,crlockReg);
           XGIFreeRec(pScrn);
           return FALSE;
        }
	break;
      default:
        XGIErrorLog(pScrn, "Unsupported framebuffer bpp (%d)\n", pScrn->bitsPerPixel);
#ifdef XGIDUALHEAD
	if(pXGIEnt) pXGIEnt->ErrorAfterFirst = TRUE;
#endif
	if(pXGI->pInt) xf86FreeInt10(pXGI->pInt);
	xgiRestoreExtRegisterLock(pXGI,srlockReg,crlockReg);
        XGIFreeRec(pScrn);
        return FALSE;
    }
    xf86LoaderModReqSymLists(pMod, fbSymbols, NULL);

    /* Load XAA if needed */
    if(!pXGI->NoAccel) {
        xf86DrvMsg(pScrn->scrnIndex, X_INFO, "Accel enabled\n");
        if(!(pMod = xf86LoadSubModule(pScrn, "xaa"))) {
	    XGIErrorLog(pScrn, "Could not load xaa module\n");
#ifdef XGIDUALHEAD
	    if(pXGIEnt) pXGIEnt->ErrorAfterFirst = TRUE;
#endif
	    if(pXGI->pInt) xf86FreeInt10(pXGI->pInt);
	    xgiRestoreExtRegisterLock(pXGI,srlockReg,crlockReg);
            XGIFreeRec(pScrn);
            return FALSE;
        }
        xf86LoaderModReqSymLists(pMod, xaaSymbols, NULL);
    }

    /* Load shadowfb if needed */
    if(pXGI->ShadowFB) {
        if(!(pMod = xf86LoadSubModule(pScrn, "shadowfb"))) {
	    XGIErrorLog(pScrn, "Could not load shadowfb module\n");
#ifdef XGIDUALHEAD
	    if(pXGIEnt) pXGIEnt->ErrorAfterFirst = TRUE;
#endif
	    if(pXGI->pInt) xf86FreeInt10(pXGI->pInt);
	    xgiRestoreExtRegisterLock(pXGI,srlockReg,crlockReg);
	    XGIFreeRec(pScrn);
            return FALSE;
        }
        xf86LoaderModReqSymLists(pMod, shadowSymbols, NULL);
    }

    /* Load the dri module if requested. */
#ifdef XF86DRI
/* if(pXGI->loadDRI) { */
       if((pMod = xf86LoadSubModule(pScrn, "dri"))) {
          xf86LoaderModReqSymLists(pMod, driSymbols, drmSymbols, NULL);
       } else {
#ifdef XGIDUALHEAD
          if(!pXGI->DualHeadMode)
#endif
             xf86DrvMsg(pScrn->scrnIndex, X_ERROR,
	  	 "Remove >Load \"dri\"< from the Module section of your XF86Config file\n");
       }
/* } */
#endif

    /* Now load and initialize VBE module for VESA and mode restoring. */
    pXGI->UseVESA = 0;
    if(pXGI->VESA == 1) {
       if(!pXGI->pVbe) {
          if(pVBEModule || (pVBEModule = xf86LoadVBEModule(pScrn))) {
	     xf86LoaderModReqSymLists(pVBEModule, vbeSymbols, NULL);
#if XF86_VERSION_CURRENT < XF86_VERSION_NUMERIC(4,2,99,0,0)
	     pXGI->pVbe = VBEInit(pXGI->pInt,pXGI->pEnt->index);
#else
             pXGI->pVbe = VBEExtendedInit(pXGI->pInt, pXGI->pEnt->index,
	    			SET_BIOS_SCRATCH | RESTORE_BIOS_SCRATCH);
#endif
          }
       }
       if(pXGI->pVbe) {
          vbe = VBEGetVBEInfo(pXGI->pVbe);
          pXGI->vesamajor = (unsigned)(vbe->VESAVersion >> 8);
          pXGI->vesaminor = vbe->VESAVersion & 0xff;
          if(pXGI->VESA == 1) {
             XGIBuildVesaModeList(pScrn, pXGI->pVbe, vbe);
             pXGI->UseVESA = 1;
          }
          VBEFreeVBEInfo(vbe);
       } else {
          xf86DrvMsg(pScrn->scrnIndex, X_WARNING,
	    	"Could not load and initialize VBE module.%s\n",
		(pXGI->VESA == 1) ? " VESA disabled." : "");
       }
    }

    if(pXGI->pVbe) {
       vbeFree(pXGI->pVbe);
       pXGI->pVbe = NULL;
    }

#ifdef XGIDUALHEAD
    xf86SetPrimInitDone(pScrn->entityList[0]);
#endif

    xgiRestoreExtRegisterLock(pXGI,srlockReg,crlockReg);

    if(pXGI->pInt) xf86FreeInt10(pXGI->pInt);
    pXGI->pInt = NULL;

#ifdef XGIDUALHEAD
    if(pXGI->DualHeadMode) {
    	pXGI->XGI_SD_Flags |= XGI_SD_ISDUALHEAD;
	if(pXGI->SecondHead)      pXGI->XGI_SD_Flags |= XGI_SD_ISDHSECONDHEAD;
	else			  pXGI->XGI_SD_Flags &= ~(XGI_SD_SUPPORTXVGAMMA1);
#if XF86_VERSION_CURRENT < XF86_VERSION_NUMERIC(4,5,99,22,0)
#ifdef PANORAMIX
	if(!noPanoramiXExtension) {
	   pXGI->XGI_SD_Flags |= XGI_SD_ISDHXINERAMA;
	   pXGI->XGI_SD_Flags &= ~(XGI_SD_SUPPORTXVGAMMA1);
	}
#endif
#else
	if(IsXineramaActive()) {
	   pXGI->XGI_SD_Flags |= XGI_SD_ISDHXINERAMA;
	   pXGI->XGI_SD_Flags &= ~(XGI_SD_SUPPORTXVGAMMA1);
	}
#endif
    }
#endif

#ifdef XGIMERGED
    if(pXGI->MergedFB)      pXGI->XGI_SD_Flags |= XGI_SD_ISMERGEDFB;
#endif

    if(pXGI->enablexgictrl) pXGI->XGI_SD_Flags |= XGI_SD_ENABLED;

    return TRUE;
}


/*
 * Map the framebuffer and MMIO memory.
 */

static Bool
XGIMapMem(ScrnInfoPtr pScrn)
{
    XGIPtr pXGI;
    int mmioFlags;

    pXGI = XGIPTR(pScrn);

    /*
     * Map IO registers to virtual address space
     */
#if !defined(__alpha__)
    mmioFlags = VIDMEM_MMIO;
#else
    /*
     * For Alpha, we need to map SPARSE memory, since we need
     * byte/short access.
     */
    mmioFlags = VIDMEM_MMIO | VIDMEM_SPARSE;
#endif
    pXGI->IOBase = xf86MapPciMem(pScrn->scrnIndex, mmioFlags,
                        pXGI->PciTag, pXGI->IOAddress, 0x10000);
    if (pXGI->IOBase == NULL)
        return FALSE;

#ifdef __alpha__
    /*
     * for Alpha, we need to map DENSE memory as well, for
     * setting CPUToScreenColorExpandBase.
     */
    pXGI->IOBaseDense = xf86MapPciMem(pScrn->scrnIndex, VIDMEM_MMIO,
                pXGI->PciTag, pXGI->IOAddress, 0x10000);

    if (pXGI->IOBaseDense == NULL)
        return FALSE;
#endif /* __alpha__ */

    pXGI->FbBase = xf86MapPciMem(pScrn->scrnIndex, VIDMEM_FRAMEBUFFER,
                                 pXGI->PciTag,
                                 (unsigned long)pXGI->FbAddress,
                                 pXGI->FbMapSize);

    PDEBUG(ErrorF("pXGI->FbBase = 0x%08lx\n",(ULONG)(pXGI->FbBase))) ;

    if (pXGI->FbBase == NULL)
        return FALSE;

    return TRUE;
}


/*
 * Unmap the framebuffer and MMIO memory.
 */

static Bool
XGIUnmapMem(ScrnInfoPtr pScrn)
{
    XGIPtr pXGI;
#ifdef XGIDUALHEAD
    XGIEntPtr pXGIEnt = NULL;
#endif

    pXGI = XGIPTR(pScrn);

#ifdef XGIDUALHEAD
    pXGIEnt = pXGI->entityPrivate;
#endif

/* In dual head mode, we must not unmap if the other head still
 * assumes memory as mapped
 */
#ifdef XGIDUALHEAD
    if(pXGI->DualHeadMode) {
        if(pXGIEnt->MapCountIOBase) {
	    pXGIEnt->MapCountIOBase--;
	    if((pXGIEnt->MapCountIOBase == 0) || (pXGIEnt->forceUnmapIOBase)) {
	    	xf86UnMapVidMem(pScrn->scrnIndex, (pointer)pXGIEnt->IOBase, (pXGI->mmioSize * 1024));
	    	pXGIEnt->IOBase = NULL;
		pXGIEnt->MapCountIOBase = 0;
		pXGIEnt->forceUnmapIOBase = FALSE;
	    }
	    pXGI->IOBase = NULL;
	}
#ifdef __alpha__
	if(pXGIEnt->MapCountIOBaseDense) {
	    pXGIEnt->MapCountIOBaseDense--;
	    if((pXGIEnt->MapCountIOBaseDense == 0) || (pXGIEnt->forceUnmapIOBaseDense)) {
	    	xf86UnMapVidMem(pScrn->scrnIndex, (pointer)pXGIEnt->IOBaseDense, (pXGI->mmioSize * 1024));
	    	pXGIEnt->IOBaseDense = NULL;
		pXGIEnt->MapCountIOBaseDense = 0;
		pXGIEnt->forceUnmapIOBaseDense = FALSE;
	    }
	    pXGI->IOBaseDense = NULL;
	}
#endif /* __alpha__ */
	if(pXGIEnt->MapCountFbBase) {
	    pXGIEnt->MapCountFbBase--;
	    if((pXGIEnt->MapCountFbBase == 0) || (pXGIEnt->forceUnmapFbBase)) {
	    	xf86UnMapVidMem(pScrn->scrnIndex, (pointer)pXGIEnt->FbBase, pXGI->FbMapSize);
	    	pXGIEnt->FbBase = NULL;
		pXGIEnt->MapCountFbBase = 0;
		pXGIEnt->forceUnmapFbBase = FALSE;

	    }
	    pXGI->FbBase = NULL;
	}
    } else {
#endif
    	xf86UnMapVidMem(pScrn->scrnIndex, (pointer)pXGI->IOBase, (pXGI->mmioSize * 1024));
    	pXGI->IOBase = NULL;
#ifdef __alpha__
    	xf86UnMapVidMem(pScrn->scrnIndex, (pointer)pXGI->IOBaseDense, (pXGI->mmioSize * 1024));
    	pXGI->IOBaseDense = NULL;
#endif
    	xf86UnMapVidMem(pScrn->scrnIndex, (pointer)pXGI->FbBase, pXGI->FbMapSize);
    	pXGI->FbBase = NULL;
#ifdef XGIDUALHEAD
    }
#endif
    return TRUE;
}

/*
 * This function saves the video state.
 */
static void
XGISave(ScrnInfoPtr pScrn)
{
    XGIPtr pXGI;
    vgaRegPtr vgaReg;
    XGIRegPtr xgiReg;

    pXGI = XGIPTR(pScrn);

#ifdef XGIDUALHEAD
    /* We always save master & slave */
    if(pXGI->DualHeadMode && pXGI->SecondHead) return;
#endif

    vgaReg = &VGAHWPTR(pScrn)->SavedReg;
    xgiReg = &pXGI->SavedReg;

       vgaHWSave(pScrn, vgaReg, VGA_SR_ALL);

    xgiSaveUnlockExtRegisterLock(pXGI,&xgiReg->xgiRegs3C4[0x05],&xgiReg->xgiRegs3D4[0x80]);

    (*pXGI->XGISave)(pScrn, xgiReg);

    if(pXGI->UseVESA) XGIVESASaveRestore(pScrn, MODE_SAVE);

    /* "Save" these again as they may have been changed prior to XGISave() call */
}

static void
XGI_WriteAttr(XGIPtr pXGI, int index, int value)
{
    (void) inb(pXGI->IODBase + VGA_IOBASE_COLOR + VGA_IN_STAT_1_OFFSET);
    index |= 0x20;
    outb(pXGI->IODBase + VGA_ATTR_INDEX, index);
    outb(pXGI->IODBase + VGA_ATTR_DATA_W, value);
}

static int
XGI_ReadAttr(XGIPtr pXGI, int index)
{
    (void) inb(pXGI->IODBase + VGA_IOBASE_COLOR + VGA_IN_STAT_1_OFFSET);
    index |= 0x20;
    outb(pXGI->IODBase + VGA_ATTR_INDEX, index);
    return(inb(pXGI->IODBase + VGA_ATTR_DATA_R));
}

#define XGI_FONTS_SIZE (8 * 8192)

static void
XGI_SaveFonts(ScrnInfoPtr pScrn)
{
    XGIPtr pXGI = XGIPTR(pScrn);
    unsigned char miscOut, attr10, gr4, gr5, gr6, seq2, seq4, scrn;
#if XF86_VERSION_CURRENT < XF86_VERSION_NUMERIC(4,2,99,0,0)
    CARD8 *vgaIOBase = (CARD8 *)VGAHWPTR(pScrn)->IOBase;
#else
    pointer vgaIOBase = VGAHWPTR(pScrn)->Base;
#endif

    if(pXGI->fonts) return;

    /* If in graphics mode, don't save anything */
    attr10 = XGI_ReadAttr(pXGI, 0x10);
    if(attr10 & 0x01) return;

    if(!(pXGI->fonts = xalloc(XGI_FONTS_SIZE * 2))) {
       xf86DrvMsg(pScrn->scrnIndex, X_ERROR,
       		"Could not save console fonts, mem allocation failed\n");
       return;
    }

    /* save the registers that are needed here */
    miscOut = inXGIREG(XGIMISCR);
    inXGIIDXREG(XGIGR, 0x04, gr4);
    inXGIIDXREG(XGIGR, 0x05, gr5);
    inXGIIDXREG(XGIGR, 0x06, gr6);
    inXGIIDXREG(XGISR, 0x02, seq2);
    inXGIIDXREG(XGISR, 0x04, seq4);

    /* Force into color mode */
    outXGIREG(XGIMISCW, miscOut | 0x01);

    inXGIIDXREG(XGISR, 0x01, scrn);
    outXGIIDXREG(XGISR, 0x00, 0x01);
    outXGIIDXREG(XGISR, 0x01, scrn | 0x20);
    outXGIIDXREG(XGISR, 0x00, 0x03);

    XGI_WriteAttr(pXGI, 0x10, 0x01);  /* graphics mode */

    /*font1 */
    outXGIIDXREG(XGISR, 0x02, 0x04);  /* write to plane 2 */
    outXGIIDXREG(XGISR, 0x04, 0x06);  /* enable plane graphics */
    outXGIIDXREG(XGIGR, 0x04, 0x02);  /* read plane 2 */
    outXGIIDXREG(XGIGR, 0x05, 0x00);  /* write mode 0, read mode 0 */
    outXGIIDXREG(XGIGR, 0x06, 0x05);  /* set graphics */
    slowbcopy_frombus(vgaIOBase, pXGI->fonts, XGI_FONTS_SIZE);

    /* font2 */
    outXGIIDXREG(XGISR, 0x02, 0x08);  /* write to plane 3 */
    outXGIIDXREG(XGISR, 0x04, 0x06);  /* enable plane graphics */
    outXGIIDXREG(XGIGR, 0x04, 0x03);  /* read plane 3 */
    outXGIIDXREG(XGIGR, 0x05, 0x00);  /* write mode 0, read mode 0 */
    outXGIIDXREG(XGIGR, 0x06, 0x05);  /* set graphics */
    slowbcopy_frombus(vgaIOBase, pXGI->fonts + XGI_FONTS_SIZE, XGI_FONTS_SIZE);

    inXGIIDXREG(XGISR, 0x01, scrn);
    outXGIIDXREG(XGISR, 0x00, 0x01);
    outXGIIDXREG(XGISR, 0x01, scrn & ~0x20);
    outXGIIDXREG(XGISR, 0x00, 0x03);

    /* Restore clobbered registers */
    XGI_WriteAttr(pXGI, 0x10, attr10);
    outXGIIDXREG(XGISR, 0x02, seq2);
    outXGIIDXREG(XGISR, 0x04, seq4);
    outXGIIDXREG(XGIGR, 0x04, gr4);
    outXGIIDXREG(XGIGR, 0x05, gr5);
    outXGIIDXREG(XGIGR, 0x06, gr6);
    outXGIREG(XGIMISCW, miscOut);
}

static void
XGI_RestoreFonts(ScrnInfoPtr pScrn)
{
    XGIPtr pXGI = XGIPTR(pScrn);
    unsigned char miscOut, attr10, gr1, gr3, gr4, gr5, gr6, gr8, seq2, seq4, scrn;
#if XF86_VERSION_CURRENT < XF86_VERSION_NUMERIC(4,2,99,0,0)
    CARD8 *vgaIOBase = (CARD8 *)VGAHWPTR(pScrn)->IOBase;
#else
    pointer vgaIOBase = VGAHWPTR(pScrn)->Base;
#endif

    if(!pXGI->fonts) return;

    /* save the registers that are needed here */
    miscOut = inXGIREG(XGIMISCR);
    attr10 = XGI_ReadAttr(pXGI, 0x10);
    inXGIIDXREG(XGIGR, 0x01, gr1);
    inXGIIDXREG(XGIGR, 0x03, gr3);
    inXGIIDXREG(XGIGR, 0x04, gr4);
    inXGIIDXREG(XGIGR, 0x05, gr5);
    inXGIIDXREG(XGIGR, 0x06, gr6);
    inXGIIDXREG(XGIGR, 0x08, gr8);
    inXGIIDXREG(XGISR, 0x02, seq2);
    inXGIIDXREG(XGISR, 0x04, seq4);

    /* Force into color mode */
    outXGIREG(XGIMISCW, miscOut | 0x01);
    inXGIIDXREG(XGISR, 0x01, scrn);
    outXGIIDXREG(XGISR, 0x00, 0x01);
    outXGIIDXREG(XGISR, 0x01, scrn | 0x20);
    outXGIIDXREG(XGISR, 0x00, 0x03);

    XGI_WriteAttr(pXGI, 0x10, 0x01);	  /* graphics mode */
    if(pScrn->depth == 4) {
       outXGIIDXREG(XGIGR, 0x03, 0x00);  /* don't rotate, write unmodified */
       outXGIIDXREG(XGIGR, 0x08, 0xFF);  /* write all bits in a byte */
       outXGIIDXREG(XGIGR, 0x01, 0x00);  /* all planes come from CPU */
    }

    outXGIIDXREG(XGISR, 0x02, 0x04); /* write to plane 2 */
    outXGIIDXREG(XGISR, 0x04, 0x06); /* enable plane graphics */
    outXGIIDXREG(XGIGR, 0x04, 0x02); /* read plane 2 */
    outXGIIDXREG(XGIGR, 0x05, 0x00); /* write mode 0, read mode 0 */
    outXGIIDXREG(XGIGR, 0x06, 0x05); /* set graphics */
    slowbcopy_tobus(pXGI->fonts, vgaIOBase, XGI_FONTS_SIZE);

    outXGIIDXREG(XGISR, 0x02, 0x08); /* write to plane 3 */
    outXGIIDXREG(XGISR, 0x04, 0x06); /* enable plane graphics */
    outXGIIDXREG(XGIGR, 0x04, 0x03); /* read plane 3 */
    outXGIIDXREG(XGIGR, 0x05, 0x00); /* write mode 0, read mode 0 */
    outXGIIDXREG(XGIGR, 0x06, 0x05); /* set graphics */
    slowbcopy_tobus(pXGI->fonts + XGI_FONTS_SIZE, vgaIOBase, XGI_FONTS_SIZE);

    inXGIIDXREG(XGISR, 0x01, scrn);
    outXGIIDXREG(XGISR, 0x00, 0x01);
    outXGIIDXREG(XGISR, 0x01, scrn & ~0x20);
    outXGIIDXREG(XGISR, 0x00, 0x03);

    /* restore the registers that were changed */
    outXGIREG(XGIMISCW, miscOut);
    XGI_WriteAttr(pXGI, 0x10, attr10);
    outXGIIDXREG(XGIGR, 0x01, gr1);
    outXGIIDXREG(XGIGR, 0x03, gr3);
    outXGIIDXREG(XGIGR, 0x04, gr4);
    outXGIIDXREG(XGIGR, 0x05, gr5);
    outXGIIDXREG(XGIGR, 0x06, gr6);
    outXGIIDXREG(XGIGR, 0x08, gr8);
    outXGIIDXREG(XGISR, 0x02, seq2);
    outXGIIDXREG(XGISR, 0x04, seq4);
}

#undef XGI_FONTS_SIZE

/* VESASaveRestore taken from vesa driver */
static void
XGIVESASaveRestore(ScrnInfoPtr pScrn, vbeSaveRestoreFunction function)
{
    XGIPtr pXGI = XGIPTR(pScrn);

    /* Query amount of memory to save state */
    if((function == MODE_QUERY) ||
       (function == MODE_SAVE && pXGI->state == NULL)) {

       /* Make sure we save at least this information in case of failure */
       (void)VBEGetVBEMode(pXGI->pVbe, &pXGI->stateMode);
       XGI_SaveFonts(pScrn);

       if(pXGI->vesamajor > 1) {
	  if(!VBESaveRestore(pXGI->pVbe, function, (pointer)&pXGI->state,
				&pXGI->stateSize, &pXGI->statePage)) {
	     return;
	  }
       }
    }

    /* Save/Restore Super VGA state */
    if(function != MODE_QUERY) {

       if(pXGI->vesamajor > 1) {
	  if(function == MODE_RESTORE) {
	     memcpy(pXGI->state, pXGI->pstate, pXGI->stateSize);
	  }

	  if(VBESaveRestore(pXGI->pVbe,function,(pointer)&pXGI->state,
			    &pXGI->stateSize,&pXGI->statePage) &&
	     (function == MODE_SAVE)) {
	     /* don't rely on the memory not being touched */
	     if(!pXGI->pstate) {
		pXGI->pstate = xalloc(pXGI->stateSize);
	     }
	     memcpy(pXGI->pstate, pXGI->state, pXGI->stateSize);
	  }
       }

       if(function == MODE_RESTORE) {
	  VBESetVBEMode(pXGI->pVbe, pXGI->stateMode, NULL);
	  XGI_RestoreFonts(pScrn);
       }

    }
}

/*
 * Initialise a new mode.  This is currently done using the
 * "initialise struct, restore/write struct to HW" model for
 * the old chipsets (5597/530/6326). For newer chipsets,
 * we use our own mode switching code (or VESA).
 */

static Bool
XGIModeInit(ScrnInfoPtr pScrn, DisplayModePtr mode)
{
    vgaHWPtr hwp = VGAHWPTR(pScrn);
    vgaRegPtr vgaReg;
    XGIPtr pXGI = XGIPTR(pScrn);
    XGIRegPtr xgiReg;
#ifdef XGIDUALHEAD
    XGIEntPtr pXGIEnt = NULL;
#endif
PDEBUG(ErrorF("XGIModeInit(). \n"));
    andXGIIDXREG(XGICR,0x11,0x7f);   	/* Unlock CRTC registers */

    XGIModifyModeInfo(mode);		/* Quick check of the mode parameters */

    if(pXGI->UseVESA) {  /* With VESA: */

#ifdef XGIDUALHEAD
       /* No dual head mode when using VESA */
       if(pXGI->SecondHead) return TRUE;
#endif

PDEBUG(ErrorF("XGIModeInit() VESA. \n"));
       pScrn->vtSema = TRUE;

       /*
	* This order is required:
	* The video bridge needs to be adjusted before the
	* BIOS is run as the BIOS sets up CRT2 according to
	* these register settings.
	* After the BIOS is run, the bridges and turboqueue
	* registers need to be readjusted as the BIOS may
	* very probably have messed them up.
	*/

       if(!XGISetVESAMode(pScrn, mode)) {
	  XGIErrorLog(pScrn, "XGISetVESAMode() failed\n");
	  return FALSE;
       }
       xgiSaveUnlockExtRegisterLock(pXGI,NULL,NULL);

#ifdef TWDEBUG
       xf86DrvMsg(pScrn->scrnIndex, X_INFO,
		   "REAL REGISTER CONTENTS AFTER SETMODE:\n");
#endif
       if(!(*pXGI->ModeInit)(pScrn, mode)) {
	  XGIErrorLog(pScrn, "ModeInit() failed\n");
	  return FALSE;
       }

       vgaHWProtect(pScrn, TRUE);
       (*pXGI->XGIRestore)(pScrn, &pXGI->ModeReg);
       vgaHWProtect(pScrn, FALSE);

    } else { /* Without VESA: */

PDEBUG(ErrorF("XGIModeInit().  none VESA\n"));
#ifdef XGIDUALHEAD
       if(pXGI->DualHeadMode) {

          if(!(*pXGI->ModeInit)(pScrn, mode)) {
	     XGIErrorLog(pScrn, "ModeInit() failed\n");
	     return FALSE;
	  }

	  pScrn->vtSema = TRUE;

	  pXGIEnt = pXGI->entityPrivate;
  
	     /* Head 2 (slave) is always CRT1 */
	     XGIPreSetMode(pScrn, mode, XGI_MODE_CRT1);
	     if(!XGIBIOSSetModeCRT1(pXGI->XGI_Pr, &pXGI->xgi_HwDevExt, pScrn, mode, pXGI->IsCustom)) {
		XGIErrorLog(pScrn, "XGIBIOSSetModeCRT1() failed\n");
		return FALSE;
	     }
	     XGIPostSetMode(pScrn, &pXGI->ModeReg);
	     XGIAdjustFrame(pXGIEnt->pScrn_1->scrnIndex,
		            pXGIEnt->pScrn_1->frameX0,
		            pXGIEnt->pScrn_1->frameY0, 0);	  

       } else {
#endif
	     /* For other chipsets, use the old method */

	     /* Initialise the ModeReg values */
    	     if(!vgaHWInit(pScrn, mode)) {
	        XGIErrorLog(pScrn, "vgaHWInit() failed\n");
	        return FALSE;
	     }

	     /* Reset our PIOOffset as vgaHWInit might have reset it */
      	     VGAHWPTR(pScrn)->PIOOffset = pXGI->IODBase + (pXGI->PciInfo->ioBase[2] & 0xFFFC) - 0x380;

	     /* Prepare the register contents */
	     if(!(*pXGI->ModeInit)(pScrn, mode)) {
	        XGIErrorLog(pScrn, "ModeInit() failed\n");
	        return FALSE;
	     }

	     pScrn->vtSema = TRUE;

	     /* Program the registers */
	     vgaHWProtect(pScrn, TRUE);
	     vgaReg = &hwp->ModeReg;
	     xgiReg = &pXGI->ModeReg;

	     vgaReg->Attribute[0x10] = 0x01;
    	     if(pScrn->bitsPerPixel > 8) {
	    	vgaReg->Graphics[0x05] = 0x00;
	     }

    	     vgaHWRestore(pScrn, vgaReg, VGA_SR_MODE);

	     (*pXGI->XGIRestore)(pScrn, xgiReg);

#ifdef TWDEBUG
	     xf86DrvMsg(pScrn->scrnIndex, X_INFO,
			"REAL REGISTER CONTENTS AFTER SETMODE:\n");
             (*pXGI->ModeInit)(pScrn, mode);
#endif

  	     vgaHWProtect(pScrn, FALSE);
	  
#ifdef XGIDUALHEAD
       }
#endif
    }

if(pXGI->Chipset == PCI_CHIP_XGIXG40 || pXGI->Chipset == PCI_CHIP_XGIXG20) {
        /* PDEBUG(XGIDumpRegs(pScrn)) ; */
                   PDEBUG(ErrorF(" *** PreSetMode(). \n"));
		   XGIPreSetMode(pScrn, mode, XGI_MODE_SIMU);
        /* PDEBUG(XGIDumpRegs(pScrn)) ; */
                   PDEBUG(ErrorF(" *** Start SetMode() \n"));

	           if(!XGIBIOSSetMode(pXGI->XGI_Pr, &pXGI->xgi_HwDevExt, pScrn,
		                   mode, pXGI->IsCustom, TRUE)) {
		      XGIErrorLog(pScrn, "XGIBIOSSetModeCRT() failed\n");
		      return FALSE;
		   }
                   Volari_EnableAccelerator(pScrn);
	       /* XGIPostSetMode(pScrn, &pXGI->ModeReg); */
      /* outXGIIDXREG(XGISR, 0x20, 0xA1) ; */
      /* outXGIIDXREG(XGISR, 0x1E, 0xDA) ; */
          /* PDEBUG(XGIDumpRegs(pScrn)) ; */
}

    /* Update Currentlayout */
    pXGI->CurrentLayout.mode = mode;

    return TRUE;
}

static Bool
XGISetVESAMode(ScrnInfoPtr pScrn, DisplayModePtr pMode)
{
    XGIPtr pXGI;
    int mode;

    pXGI = XGIPTR(pScrn);

    if(!(mode = XGICalcVESAModeIndex(pScrn, pMode))) return FALSE;

    mode |= (1 << 15);	/* Don't clear framebuffer */
    mode |= (1 << 14); 	/* Use linear adressing */

    if(VBESetVBEMode(pXGI->pVbe, mode, NULL) == FALSE) {
       XGIErrorLog(pScrn, "Setting VESA mode 0x%x failed\n",
	             	mode & 0x0fff);
       return (FALSE);
    }

    if(pMode->HDisplay != pScrn->virtualX) {
       VBESetLogicalScanline(pXGI->pVbe, pScrn->virtualX);
    }

    xf86DrvMsg(pScrn->scrnIndex, X_PROBED,
    	"Setting VESA mode 0x%x succeeded\n",
	mode & 0x0fff);

    return (TRUE);
}

/* static void
XGISpecialRestore(ScrnInfoPtr pScrn)
{
    XGIPtr    pXGI = XGIPTR(pScrn);
    XGIRegPtr xgiReg = &pXGI->SavedReg;
    unsigned char temp;
    int i;















    if(!(pXGI->ChipFlags & XGICF_Is65x)) return;
    inXGIIDXREG(XGICR, 0x34, temp);
    temp &= 0x7f;
    if(temp > 0x13) return;

#ifdef UNLOCK_ALWAYS
    xgiSaveUnlockExtRegisterLock(pXGI, NULL,NULL);
#endif

    outXGIIDXREG(XGICAP, 0x3f, xgiReg->xgiCapt[0x3f]);
    outXGIIDXREG(XGICAP, 0x00, xgiReg->xgiCapt[0x00]);
    for(i = 0; i < 0x4f; i++) {
       outXGIIDXREG(XGICAP, i, xgiReg->xgiCapt[i]);
    }
    outXGIIDXREG(XGIVID, 0x32, (xgiReg->xgiVid[0x32] & ~0x05));
    outXGIIDXREG(XGIVID, 0x30, xgiReg->xgiVid[0x30]);
    outXGIIDXREG(XGIVID, 0x32, ((xgiReg->xgiVid[0x32] & ~0x04) | 0x01));
    outXGIIDXREG(XGIVID, 0x30, xgiReg->xgiVid[0x30]);

    if(!(pXGI->ChipFlags & XGICF_Is651)) return;
    if(!(pXGI->VBFlags & VB_XGIBRIDGE)) return;

    inXGIIDXREG(XGICR, 0x30, temp);
    if(temp & 0x40) {
       unsigned char myregs[] = {
       			0x2f, 0x08, 0x09, 0x03, 0x0a, 0x0c,
			0x0b, 0x0d, 0x0e, 0x12, 0x0f, 0x10,
			0x11, 0x04, 0x05, 0x06, 0x07, 0x00,
			0x2e
       };
       for(i = 0; i <= 18; i++) {
          outXGIIDXREG(XGIPART1, myregs[i], xgiReg->VBPart1[myregs[i]]);
       }
    } else if((temp & 0x20) || (temp & 0x9c)) {
       unsigned char myregs[] = {
       			0x04, 0x05, 0x06, 0x07, 0x00, 0x2e
       };
       for(i = 0; i <= 5; i++) {
          outXGIIDXREG(XGIPART1, myregs[i], xgiReg->VBPart1[myregs[i]]);
       }
    }
}  */

/*
 * Restore the initial mode. To be used internally only!
 */
static void
XGIRestore(ScrnInfoPtr pScrn)
{
    XGIPtr    pXGI = XGIPTR(pScrn);
    XGIRegPtr xgiReg = &pXGI->SavedReg;
    vgaHWPtr  hwp = VGAHWPTR(pScrn);
    vgaRegPtr vgaReg = &hwp->SavedReg;
/*    Bool      doit = FALSE, doitlater = FALSE;
    Bool      vesasuccess = FALSE;  */

    /* WARNING: Don't ever touch this. It now seems to work on
     * all chipset/bridge combinations - but finding out the
     * correct combination was pure hell.
     */

    /* Wait for the accelerators */
    if(pXGI->AccelInfoPtr) {
       (*pXGI->AccelInfoPtr->Sync)(pScrn);
    }

        vgaHWProtect(pScrn, TRUE);

#ifdef UNLOCK_ALWAYS
        xgiSaveUnlockExtRegisterLock(pXGI, NULL,NULL);
#endif

        (*pXGI->XGIRestore)(pScrn, xgiReg);

        vgaHWProtect(pScrn, TRUE);
	if(pXGI->Primary) {
           vgaHWRestore(pScrn, vgaReg, VGA_SR_ALL);
	}

	/* Restore TV. This is rather complicated, but if we don't do it,
	 * TV output will flicker terribly
	 */


        xgiRestoreExtRegisterLock(pXGI,xgiReg->xgiRegs3C4[5],xgiReg->xgiRegs3D4[0x80]);

        vgaHWProtect(pScrn, FALSE);
}

static void
XGIVESARestore(ScrnInfoPtr pScrn)
{
   XGIPtr pXGI = XGIPTR(pScrn);

   if(pXGI->UseVESA) {
      XGIVESASaveRestore(pScrn, MODE_RESTORE);
#ifdef XGIVRAMQ
      /* Restore queue mode registers on 315/330 series */
      /* (This became necessary due to the switch to VRAM queue) */
#endif
   }
}

/* Restore bridge config registers - to be called BEFORE VESARestore */
static void
XGIBridgeRestore(ScrnInfoPtr pScrn)
{
    XGIPtr pXGI = XGIPTR(pScrn);

#ifdef XGIDUALHEAD
    /* We only restore for master head */
    if(pXGI->DualHeadMode && pXGI->SecondHead) return;
#endif

}

/* Our generic BlockHandler for Xv */
static void
XGIBlockHandler(int i, pointer blockData, pointer pTimeout, pointer pReadmask)
{
    ScreenPtr pScreen = screenInfo.screens[i];
    ScrnInfoPtr pScrn   = xf86Screens[i];
    XGIPtr pXGI = XGIPTR(pScrn);

    pScreen->BlockHandler = pXGI->BlockHandler;
    (*pScreen->BlockHandler) (i, blockData, pTimeout, pReadmask);
    pScreen->BlockHandler = XGIBlockHandler;

    if(pXGI->VideoTimerCallback) {
       (*pXGI->VideoTimerCallback)(pScrn, currentTime.milliseconds);
    }

    if(pXGI->RenderCallback) {
       (*pXGI->RenderCallback)(pScrn);
    }
}

/* Mandatory
 * This gets called at the start of each server generation
 *
 * We use pScrn and not CurrentLayout here, because the
 * properties we use have not changed (displayWidth,
 * depth, bitsPerPixel)
 */
static Bool
XGIScreenInit(int scrnIndex, ScreenPtr pScreen, const int argc, const char **argv)
{
    ScrnInfoPtr pScrn;
    vgaHWPtr hwp;
    XGIPtr pXGI;
    int ret;
    VisualPtr visual;
    unsigned long OnScreenSize;
    int height, width, displayWidth;
    unsigned char *FBStart;
    ModuleDescPtr pVBEModule;
#ifdef XGIDUALHEAD
    XGIEntPtr pXGIEnt = NULL;
#endif
PDEBUG(ErrorF("XGIScreenInit(). \n"));
    pScrn = xf86Screens[pScreen->myNum];

    hwp = VGAHWPTR(pScrn);

    pXGI = XGIPTR(pScrn);

#ifdef XGIDUALHEAD
    if((!pXGI->DualHeadMode) || (!pXGI->SecondHead)) {
#endif
       if((pVBEModule = xf86LoadVBEModule(pScrn))) {
	  xf86LoaderModReqSymLists(pVBEModule, vbeSymbols, NULL);
#if XF86_VERSION_CURRENT < XF86_VERSION_NUMERIC(4,2,99,0,0)
          pXGI->pVbe = VBEInit(NULL, pXGI->pEnt->index);
#else
          pXGI->pVbe = VBEExtendedInit(NULL, pXGI->pEnt->index,
	                   SET_BIOS_SCRATCH | RESTORE_BIOS_SCRATCH);
#endif
       } else {
          XGIErrorLog(pScrn, "Failed to load VBE submodule\n");
       }
#ifdef XGIDUALHEAD
    }
#endif

#ifdef XGIDUALHEAD
    if(pXGI->DualHeadMode) {
       pXGIEnt = pXGI->entityPrivate;
       pXGIEnt->refCount++;
    }
#endif

    /* Map the VGA memory and get the VGA IO base */
    if(pXGI->Primary) {
       hwp->MapSize = 0x10000;  /* Standard 64k VGA window */
       if(!vgaHWMapMem(pScrn)) {
          XGIErrorLog(pScrn, "Could not map VGA memory window\n");
          return FALSE;
       }
    }
    vgaHWGetIOBase(hwp);

    /* Patch the PIOOffset inside vgaHW to use
     * our relocated IO ports.
     */
    VGAHWPTR(pScrn)->PIOOffset = pXGI->IODBase + (pXGI->PciInfo->ioBase[2] & 0xFFFC) - 0x380;

    /* Map the XGI memory and MMIO areas */
    if(!XGIMapMem(pScrn)) {
       XGIErrorLog(pScrn, "XGIMapMem() failed\n");
       return FALSE;
    }

#ifdef UNLOCK_ALWAYS
    xgiSaveUnlockExtRegisterLock(pXGI, NULL, NULL);
#endif

    /* Enable TurboQueue so that XGISave() saves it in enabled
     * state. If we don't do this, X will hang after a restart!
     * (Happens for some unknown reason only when using VESA
     * for mode switching; assumingly a BIOS issue.)
     * This is done on 300 and 315 series only.
     */
    if(pXGI->UseVESA) {
#ifdef XGIVRAMQ
       if(pXGI->VGAEngine != XGI_315_VGA)
#endif
          XGIEnableTurboQueue(pScrn);

    }

    /* Save the current state */
    XGISave(pScrn);

    XGI_InitHwDevInfo(pScrn);

    PDEBUG(ErrorF("--- ScreenInit ---  \n")) ;
    PDEBUG(XGIDumpRegs(pScrn)) ;

    /* Initialise the first mode */
    if(!XGIModeInit(pScrn, pScrn->currentMode)) {
       XGIErrorLog(pScrn, "XGIModeInit() failed\n");
       return FALSE;
    }

    PDEBUG(ErrorF("--- XGIModeInit ---  \n")) ;
    PDEBUG(XGIDumpRegs(pScrn)) ;

    /* Darken the screen for aesthetic reasons */
    /* Not using Dual Head variant on purpose; we darken
     * the screen for both displays, and un-darken
     * it when the second head is finished
     */
    XGISaveScreen(pScreen, SCREEN_SAVER_ON);

    /* Set the viewport */
    XGIAdjustFrame(scrnIndex, pScrn->frameX0, pScrn->frameY0, 0);

    /*
     * The next step is to setup the screen's visuals, and initialise the
     * framebuffer code.  In cases where the framebuffer's default
     * choices for things like visual layouts and bits per RGB are OK,
     * this may be as simple as calling the framebuffer's ScreenInit()
     * function.  If not, the visuals will need to be setup before calling
     * a fb ScreenInit() function and fixed up after.
     *
     * For most PC hardware at depths >= 8, the defaults that fb uses
     * are not appropriate.  In this driver, we fixup the visuals after.
     */

    /*
     * Reset visual list.
     */
    miClearVisualTypes();

    /* Setup the visuals we support. */

    /*
     * For bpp > 8, the default visuals are not acceptable because we only
     * support TrueColor and not DirectColor.
     */
    if(!miSetVisualTypes(pScrn->depth,
    			 (pScrn->bitsPerPixel > 8) ?
			 	TrueColorMask : miGetDefaultVisualMask(pScrn->depth),
			 pScrn->rgbBits, pScrn->defaultVisual)) {
       XGISaveScreen(pScreen, SCREEN_SAVER_OFF);
       XGIErrorLog(pScrn, "miSetVisualTypes() failed (bpp %d)\n",
	  		pScrn->bitsPerPixel);
       return FALSE;
    }

    width = pScrn->virtualX;
    height = pScrn->virtualY;
    displayWidth = pScrn->displayWidth;

    if(pXGI->Rotate) {
       height = pScrn->virtualX;
       width = pScrn->virtualY;
    }

    if(pXGI->ShadowFB) {
       pXGI->ShadowPitch = BitmapBytePad(pScrn->bitsPerPixel * width);
       pXGI->ShadowPtr = xalloc(pXGI->ShadowPitch * height);
       displayWidth = pXGI->ShadowPitch / (pScrn->bitsPerPixel >> 3);
       FBStart = pXGI->ShadowPtr;
    } else {
       pXGI->ShadowPtr = NULL;
       FBStart = pXGI->FbBase;
    }

    if(!miSetPixmapDepths()) {
       XGISaveScreen(pScreen, SCREEN_SAVER_OFF);
       XGIErrorLog(pScrn, "miSetPixmapDepths() failed\n");
       return FALSE;
    }

    /* Point cmdQueuePtr to pXGIEnt for shared usage
     * (same technique is then eventually used in DRIScreeninit)
     * For 315/330 series, this is done in EnableTurboQueue
     * which has already been called during ModeInit().
     */
#ifdef XGIDUALHEAD
    if(pXGI->SecondHead)
       pXGI->cmdQueueLenPtr = &(XGIPTR(pXGIEnt->pScrn_1)->cmdQueueLen);
    else
#endif
       pXGI->cmdQueueLenPtr = &(pXGI->cmdQueueLen);

    pXGI->cmdQueueLen = 0; /* Force an EngineIdle() at start */

#ifdef XF86DRI
/* if(pXGI->loadDRI) { */
#ifdef XGIDUALHEAD
       /* No DRI in dual head mode */
       if(pXGI->DualHeadMode) {
          pXGI->directRenderingEnabled = FALSE;
          xf86DrvMsg(pScrn->scrnIndex, X_INFO,
		"DRI not supported in Dual Head mode\n");
       } else
#endif
       /* Force the initialization of the context */
/* if(pXGI->VGAEngine != XGI_315_VGA) { */
       if( (XGI_FbDevExist) && (pXGI->Chipset != PCI_CHIP_XGIXG20) ) {
          pXGI->directRenderingEnabled = XGIDRIScreenInit(pScreen);
          PDEBUG(ErrorF("--- DRI supported   \n"));
       } else {
          PDEBUG(ErrorF("--- DRI not supported   \n"));
          xf86DrvMsg(pScrn->scrnIndex, X_NOT_IMPLEMENTED,
	        "DRI not supported on this chipset\n");
          pXGI->directRenderingEnabled = FALSE;
       }
/* } */
#endif

    /*
     * Call the framebuffer layer's ScreenInit function, and fill in other
     * pScreen fields.
     */
    switch(pScrn->bitsPerPixel) {
      case 24:
      case 8:
      case 16:
      case 32:
        ret = fbScreenInit(pScreen, FBStart, width,
                        height, pScrn->xDpi, pScrn->yDpi,
                        displayWidth, pScrn->bitsPerPixel);
        break;
      default:
        ret = FALSE;
        break;
    }
    if(!ret) {
       XGIErrorLog(pScrn, "Unsupported bpp (%d) or fbScreenInit() failed\n",
               pScrn->bitsPerPixel);
       XGISaveScreen(pScreen, SCREEN_SAVER_OFF);
       return FALSE;
    }

    if(pScrn->bitsPerPixel > 8) {
       /* Fixup RGB ordering */
       visual = pScreen->visuals + pScreen->numVisuals;
       while (--visual >= pScreen->visuals) {
          if((visual->class | DynamicClass) == DirectColor) {
             visual->offsetRed = pScrn->offset.red;
             visual->offsetGreen = pScrn->offset.green;
             visual->offsetBlue = pScrn->offset.blue;
             visual->redMask = pScrn->mask.red;
             visual->greenMask = pScrn->mask.green;
             visual->blueMask = pScrn->mask.blue;
          }
       }
    }

    /* Initialize RENDER ext; must be after RGB ordering fixed */
    fbPictureInit(pScreen, 0, 0);

    /* hardware cursor needs to wrap this layer    <-- TW: what does that mean? */
    if(!pXGI->ShadowFB) XGIDGAInit(pScreen);

    xf86SetBlackWhitePixels(pScreen);

    if(!pXGI->NoAccel) {
       switch(pXGI->VGAEngine) {
	  case XGI_XGX_VGA:
/* Volari_EnableAccelerator(pScrn); */
            PDEBUG(ErrorF("---Volari Accel..  \n"));
          default:
            Volari_AccelInit(pScreen);
            break;
       }
    }

    PDEBUG(ErrorF("--- AccelInit ---  \n")) ;
    PDEBUG(XGIDumpRegs(pScrn)) ;

    miInitializeBackingStore(pScreen);
    xf86SetBackingStore(pScreen);
    xf86SetSilkenMouse(pScreen);

    /* Initialise cursor functions */
    miDCInitialize(pScreen, xf86GetPointerScreenFuncs());

    if(pXGI->HWCursor) {
       XGIHWCursorInit(pScreen);
    }

    /* Initialise default colourmap */
    if(!miCreateDefColormap(pScreen)) {
       XGISaveScreen(pScreen, SCREEN_SAVER_OFF);
       XGIErrorLog(pScrn, "miCreateDefColormap() failed\n");
       return FALSE;
    }

    if(!xf86HandleColormaps(pScreen, 256, (pScrn->depth == 8) ? 8 : pScrn->rgbBits,
                    XGILoadPalette, NULL,
                    CMAP_PALETTED_TRUECOLOR | CMAP_RELOAD_ON_MODE_SWITCH)) {
PDEBUG(ErrorF("XGILoadPalette() check-return.  \n"));
       XGISaveScreen(pScreen, SCREEN_SAVER_OFF);
       XGIErrorLog(pScrn, "xf86HandleColormaps() failed\n");
       return FALSE;
    }

/*
    if (!xf86HandleColormaps(pScreen, 256, 8, XGILoadPalette, NULL,
                             CMAP_RELOAD_ON_MODE_SWITCH))
    {
        return FALSE;
    }
*/
    xf86DPMSInit(pScreen, (DPMSSetProcPtr)XGIDisplayPowerManagementSet, 0);

    /* Init memPhysBase and fbOffset in pScrn */
    pScrn->memPhysBase = pXGI->FbAddress;
    pScrn->fbOffset = 0;

    pXGI->ResetXv = pXGI->ResetXvGamma = NULL;

    if(!pXGI->NoXvideo) {
        if(pXGI->VGAEngine == XGI_XGX_VGA) {
           XGIInitVideo(pScreen);
        }
    }

#ifdef XF86DRI
/* if(pXGI->loadDRI) { */
       if(pXGI->directRenderingEnabled) {
          /* Now that mi, drm and others have done their thing,
           * complete the DRI setup.
           */
          pXGI->directRenderingEnabled = XGIDRIFinishScreenInit(pScreen);
       }
       if(pXGI->directRenderingEnabled) {
          xf86DrvMsg(pScrn->scrnIndex, X_INFO, "Direct rendering enabled\n");
          /* TODO */
          /* XGISetLFBConfig(pXGI); */
       } else {
          xf86DrvMsg(pScrn->scrnIndex, X_INFO, "Direct rendering disabled\n");
       }
/* } */
#endif

    /* Wrap some funcs and setup remaining SD flags */

    pXGI->XGI_SD_Flags &= ~(XGI_SD_PSEUDOXINERAMA);

    pXGI->CloseScreen = pScreen->CloseScreen;
    pScreen->CloseScreen = XGICloseScreen;
#ifdef XGIDUALHEAD
    if(pXGI->DualHeadMode)
       pScreen->SaveScreen = XGISaveScreenDH;
    else
#endif
       pScreen->SaveScreen = XGISaveScreen;

    /* Install BlockHandler */
    pXGI->BlockHandler = pScreen->BlockHandler;
    pScreen->BlockHandler = XGIBlockHandler;

    /* Report any unused options (only for the first generation) */
    if(serverGeneration == 1) {
       xf86ShowUnusedOptions(pScrn->scrnIndex, pScrn->options);
    }

    /* Clear frame buffer */
    /* For CRT2, we don't do that at this point in dual head
     * mode since the mode isn't switched at this time (it will
     * be reset when setting the CRT1 mode). Hence, we just
     * save the necessary data and clear the screen when
     * going through this for CRT1.
     */

    OnScreenSize = pScrn->displayWidth * pScrn->currentMode->VDisplay
                               * (pScrn->bitsPerPixel >> 3);

    /* Turn on the screen now */
    /* We do this in dual head mode after second head is finished */
#ifdef XGIDUALHEAD
    if(pXGI->DualHeadMode) {
       if(pXGI->SecondHead) {
          bzero(pXGI->FbBase, OnScreenSize);
	  bzero(pXGIEnt->FbBase1, pXGIEnt->OnScreenSize1);
    	  XGISaveScreen(pScreen, SCREEN_SAVER_OFF);
       } else {
          pXGIEnt->FbBase1 = pXGI->FbBase;
	  pXGIEnt->OnScreenSize1 = OnScreenSize;
       }
    } else {
#endif
       XGISaveScreen(pScreen, SCREEN_SAVER_OFF);
       bzero(pXGI->FbBase, OnScreenSize);
#ifdef XGIDUALHEAD
    }
#endif

    pXGI->XGI_SD_Flags &= ~XGI_SD_ISDEPTH8;
    if(pXGI->CurrentLayout.bitsPerPixel == 8) {
    	pXGI->XGI_SD_Flags |= XGI_SD_ISDEPTH8;
	pXGI->XGI_SD_Flags &= ~XGI_SD_SUPPORTXVGAMMA1;
    }
PDEBUG(ErrorF("XGIScreenInit() End.  \n"));
    return TRUE;
}

/* Usually mandatory */
Bool
XGISwitchMode(int scrnIndex, DisplayModePtr mode, int flags)
{
    ScrnInfoPtr pScrn = xf86Screens[scrnIndex];
    XGIPtr pXGI = XGIPTR(pScrn);

    if(!pXGI->NoAccel) {
       if(pXGI->AccelInfoPtr) {
          (*pXGI->AccelInfoPtr->Sync)(pScrn);
PDEBUG(ErrorF("XGISwitchMode Accel Enabled. \n"));
       }
    }
PDEBUG(ErrorF("XGISwitchMode (%d, %d) \n", mode->HDisplay, mode->VDisplay));

    if(!(XGIModeInit(xf86Screens[scrnIndex], mode))) return FALSE;

    /* Since RandR (indirectly) uses SwitchMode(), we need to
     * update our Xinerama info here, too, in case of resizing
     */
    return TRUE;
}

Bool
XGISwitchCRT1Status(ScrnInfoPtr pScrn, int onoff)
{
    XGIPtr pXGI = XGIPTR(pScrn);
    DisplayModePtr mode = pScrn->currentMode;
    unsigned long vbflags = pXGI->VBFlags;
    int crt1off;

    /* onoff: 0=OFF, 1=ON(VGA), 2=ON(LCDA) */
    /* Switching to LCDA will disable CRT2 if previously LCD */

    /* Do NOT use this to switch from CRT1_LCDA to CRT2_LCD */

    return FALSE;

    /* Off only if at least one CRT2 device is active */
    if((!onoff) && (!(vbflags & CRT2_ENABLE))) return FALSE;

#ifdef XGIDUALHEAD
    if(pXGI->DualHeadMode) return FALSE;
#endif

    /* Can't switch to LCDA of not supported (duh!) */
    if(!(pXGI->XGI_SD_Flags & XGI_SD_SUPPORTLCDA)) {
       if(onoff == 2) {
          xf86DrvMsg(pScrn->scrnIndex, X_ERROR,
	   	"LCD-via-CRT1 not supported on this hardware\n");
          return FALSE;
       }
    }

#ifdef XGIMERGED
    if(pXGI->MergedFB) {
       if(!onoff) {
          xf86DrvMsg(pScrn->scrnIndex, X_ERROR,
	   	"CRT1 can't be switched off in MergedFB mode\n");
          return FALSE;
       } else if(onoff == 2) {
          if(vbflags & CRT2_LCD) {
	     xf86DrvMsg(pScrn->scrnIndex, X_ERROR,
	   	"CRT2 type can't be LCD while CRT1 is LCD-via-CRT1\n");
             return FALSE;
	  }
       }
       if(mode->Private) {
	  mode = ((XGIMergedDisplayModePtr)mode->Private)->CRT1;
       }
    }
#endif

    vbflags &= ~(DISPTYPE_CRT1 | SINGLE_MODE | MIRROR_MODE | CRT1_LCDA);
    crt1off = 1;
    if(onoff > 0) {
       vbflags |= DISPTYPE_CRT1;
       crt1off = 0;
       if(onoff == 2) {
       	  vbflags |= CRT1_LCDA;
	  vbflags &= ~CRT2_LCD;
       }
       /* Remember: Dualhead not supported */
       if(vbflags & CRT2_ENABLE) vbflags |= MIRROR_MODE;
       else vbflags |= SINGLE_MODE;
    } else {
       vbflags |= SINGLE_MODE;
    }

    if(vbflags & CRT1_LCDA) {
       if(!XGI_CalcModeIndex(pScrn, mode, vbflags, pXGI->HaveCustomModes)) {
          xf86DrvMsg(pScrn->scrnIndex, X_ERROR,
		"Current mode not suitable for LCD-via-CRT1\n");
          return FALSE;
       }
    }

    pXGI->CRT1off = crt1off;
    pXGI->VBFlags = pXGI->VBFlags_backup = vbflags;

    /* Sync the accelerators */
    if(!pXGI->NoAccel) {
       if(pXGI->AccelInfoPtr) {
          (*pXGI->AccelInfoPtr->Sync)(pScrn);
       }
    }

    if(!(pScrn->SwitchMode(pScrn->scrnIndex, pScrn->currentMode, 0))) return FALSE;
    XGIAdjustFrame(pScrn->scrnIndex, pScrn->frameX0, pScrn->frameY0, 0);
    return TRUE;
}

/* static void
XGISetStartAddressCRT1(XGIPtr pXGI, unsigned long base)
{
    unsigned char cr11backup;

    inXGIIDXREG(XGICR,  0x11, cr11backup);  
    andXGIIDXREG(XGICR, 0x11, 0x7F);
    outXGIIDXREG(XGICR, 0x0D, base & 0xFF);
    outXGIIDXREG(XGICR, 0x0C, (base >> 8) & 0xFF);
    outXGIIDXREG(XGISR, 0x0D, (base >> 16) & 0xFF);

    
    setXGIIDXREG(XGICR, 0x11, 0x7F,(cr11backup & 0x80));
} */

#ifdef XGIMERGED
/* static Bool
InRegion(int x, int y, region r)
{
    return (r.x0 <= x) && (x <= r.x1) && (r.y0 <= y) && (y <= r.y1);
} */

/* static void
XGIAdjustFrameHW_CRT1(ScrnInfoPtr pScrn, int x, int y)
{
    XGIPtr pXGI = XGIPTR(pScrn);
    unsigned long base;

    base = y * pXGI->CurrentLayout.displayWidth + x;
    switch(pXGI->CurrentLayout.bitsPerPixel) {
       case 16:  base >>= 1; 	break;
       case 32:  		break;
       default:  base >>= 2;
    }
    XGISetStartAddressCRT1(pXGI, base);
} */

/* static void
XGIMergePointerMoved(int scrnIndex, int x, int y)
{
  ScrnInfoPtr   pScrn1 = xf86Screens[scrnIndex];
  XGIPtr        pXGI = XGIPTR(pScrn1);
  ScrnInfoPtr   pScrn2 = pXGI->CRT2pScrn;
  region 	out, in1, in2, f2, f1;
  int 		deltax, deltay;

  f1.x0 = pXGI->CRT1frameX0;
  f1.x1 = pXGI->CRT1frameX1;
  f1.y0 = pXGI->CRT1frameY0;
  f1.y1 = pXGI->CRT1frameY1;
  f2.x0 = pScrn2->frameX0;
  f2.x1 = pScrn2->frameX1;
  f2.y0 = pScrn2->frameY0;
  f2.y1 = pScrn2->frameY1;

  out.x0 = pScrn1->frameX0;
  out.x1 = pScrn1->frameX1;
  out.y0 = pScrn1->frameY0;
  out.y1 = pScrn1->frameY1;

  in1 = out;
  in2 = out;
  switch(((XGIMergedDisplayModePtr)pXGI->CurrentLayout.mode->Private)->CRT2Position) {
     case xgiLeftOf:
        in1.x0 = f1.x0;
        in2.x1 = f2.x1;
        break;
     case xgiRightOf:
        in1.x1 = f1.x1;
        in2.x0 = f2.x0;
        break;
     case xgiBelow:
        in1.y1 = f1.y1;
        in2.y0 = f2.y0;
        break;
     case xgiAbove:
        in1.y0 = f1.y0;
        in2.y1 = f2.y1;
        break;
     case xgiClone:
        break;
  }

  deltay = 0;
  deltax = 0;

  if(InRegion(x, y, out)) {	

     if(InRegion(x, y, in1) && !InRegion(x, y, f1)) {
        REBOUND(f1.x0, f1.x1, x);
        REBOUND(f1.y0, f1.y1, y);
        deltax = 1;
     }
     if(InRegion(x, y, in2) && !InRegion(x, y, f2)) {
        REBOUND(f2.x0, f2.x1, x);
        REBOUND(f2.y0, f2.y1, y);
        deltax = 1;
     }

  } else {			

     if(out.x0 > x) {
        deltax = x - out.x0;
     }
     if(out.x1 < x) {
        deltax = x - out.x1;
     }
     if(deltax) {
        pScrn1->frameX0 += deltax;
        pScrn1->frameX1 += deltax;
	f1.x0 += deltax;
        f1.x1 += deltax;
        f2.x0 += deltax;
        f2.x1 += deltax;
     }

     if(out.y0 > y) {
        deltay = y - out.y0;
     }
     if(out.y1 < y) {
        deltay = y - out.y1;
     }
     if(deltay) {
        pScrn1->frameY0 += deltay;
        pScrn1->frameY1 += deltay;
	f1.y0 += deltay;
        f1.y1 += deltay;
        f2.y0 += deltay;
        f2.y1 += deltay;
     }

     switch(((XGIMergedDisplayModePtr)pXGI->CurrentLayout.mode->Private)->CRT2Position) {
        case xgiLeftOf:
	   if(x >= f1.x0) { REBOUND(f1.y0, f1.y1, y); }
	   if(x <= f2.x1) { REBOUND(f2.y0, f2.y1, y); }
           break;
        case xgiRightOf:
	   if(x <= f1.x1) { REBOUND(f1.y0, f1.y1, y); }
	   if(x >= f2.x0) { REBOUND(f2.y0, f2.y1, y); }
           break;
        case xgiBelow:
	   if(y <= f1.y1) { REBOUND(f1.x0, f1.x1, x); }
	   if(y >= f2.y0) { REBOUND(f2.x0, f2.x1, x); }
           break;
        case xgiAbove:
	   if(y >= f1.y0) { REBOUND(f1.x0, f1.x1, x); }
	   if(y <= f2.y1) { REBOUND(f2.x0, f2.x1, x); }
           break;
        case xgiClone:
           break;
     }

  }

  if(deltax || deltay) {
     pXGI->CRT1frameX0 = f1.x0;
     pXGI->CRT1frameY0 = f1.y0;
     pScrn2->frameX0 = f2.x0;
     pScrn2->frameY0 = f2.y0;

     pXGI->CRT1frameX1 = pXGI->CRT1frameX0 + CDMPTR->CRT1->HDisplay - 1;
     pXGI->CRT1frameY1 = pXGI->CRT1frameY0 + CDMPTR->CRT1->VDisplay - 1;
     pScrn2->frameX1   = pScrn2->frameX0   + CDMPTR->CRT2->HDisplay - 1;
     pScrn2->frameY1   = pScrn2->frameY0   + CDMPTR->CRT2->VDisplay - 1;
     pScrn1->frameX1   = pScrn1->frameX0   + pXGI->CurrentLayout.mode->HDisplay  - 1;
     pScrn1->frameY1   = pScrn1->frameY0   + pXGI->CurrentLayout.mode->VDisplay  - 1;

     XGIAdjustFrameHW_CRT1(pScrn1, pXGI->CRT1frameX0, pXGI->CRT1frameY0);
  }
}  */


/* static void
XGIAdjustFrameMerged(int scrnIndex, int x, int y, int flags)
{
    ScrnInfoPtr pScrn1 = xf86Screens[scrnIndex];
    XGIPtr pXGI = XGIPTR(pScrn1);
    ScrnInfoPtr pScrn2 = pXGI->CRT2pScrn;
    int VTotal = pXGI->CurrentLayout.mode->VDisplay;
    int HTotal = pXGI->CurrentLayout.mode->HDisplay;
    int VMax = VTotal;
    int HMax = HTotal;

    BOUND(x, 0, pScrn1->virtualX - HTotal);
    BOUND(y, 0, pScrn1->virtualY - VTotal);

    switch(SDMPTR(pScrn1)->CRT2Position) {
        case xgiLeftOf:
            pScrn2->frameX0 = x;
            BOUND(pScrn2->frameY0,   y, y + VMax - CDMPTR->CRT2->VDisplay);
            pXGI->CRT1frameX0 = x + CDMPTR->CRT2->HDisplay;
            BOUND(pXGI->CRT1frameY0, y, y + VMax - CDMPTR->CRT1->VDisplay);
            break;
        case xgiRightOf:
            pXGI->CRT1frameX0 = x;
            BOUND(pXGI->CRT1frameY0, y, y + VMax - CDMPTR->CRT1->VDisplay);
            pScrn2->frameX0 = x + CDMPTR->CRT1->HDisplay;
            BOUND(pScrn2->frameY0,   y, y + VMax - CDMPTR->CRT2->VDisplay);
            break;
        case xgiAbove:
            BOUND(pScrn2->frameX0,   x, x + HMax - CDMPTR->CRT2->HDisplay);
            pScrn2->frameY0 = y;
            BOUND(pXGI->CRT1frameX0, x, x + HMax - CDMPTR->CRT1->HDisplay);
            pXGI->CRT1frameY0 = y + CDMPTR->CRT2->VDisplay;
            break;
        case xgiBelow:
            BOUND(pXGI->CRT1frameX0, x, x + HMax - CDMPTR->CRT1->HDisplay);
            pXGI->CRT1frameY0 = y;
            BOUND(pScrn2->frameX0,   x, x + HMax - CDMPTR->CRT2->HDisplay);
            pScrn2->frameY0 = y + CDMPTR->CRT1->VDisplay;
            break;
        case xgiClone:
            BOUND(pXGI->CRT1frameX0, x, x + HMax - CDMPTR->CRT1->HDisplay);
            BOUND(pXGI->CRT1frameY0, y, y + VMax - CDMPTR->CRT1->VDisplay);
            BOUND(pScrn2->frameX0,   x, x + HMax - CDMPTR->CRT2->HDisplay);
            BOUND(pScrn2->frameY0,   y, y + VMax - CDMPTR->CRT2->VDisplay);
            break;
    }

    BOUND(pXGI->CRT1frameX0, 0, pScrn1->virtualX - CDMPTR->CRT1->HDisplay);
    BOUND(pXGI->CRT1frameY0, 0, pScrn1->virtualY - CDMPTR->CRT1->VDisplay);
    BOUND(pScrn2->frameX0,   0, pScrn1->virtualX - CDMPTR->CRT2->HDisplay);
    BOUND(pScrn2->frameY0,   0, pScrn1->virtualY - CDMPTR->CRT2->VDisplay);

    pScrn1->frameX0 = x;
    pScrn1->frameY0 = y;

    pXGI->CRT1frameX1 = pXGI->CRT1frameX0 + CDMPTR->CRT1->HDisplay - 1;
    pXGI->CRT1frameY1 = pXGI->CRT1frameY0 + CDMPTR->CRT1->VDisplay - 1;
    pScrn2->frameX1   = pScrn2->frameX0   + CDMPTR->CRT2->HDisplay - 1;
    pScrn2->frameY1   = pScrn2->frameY0   + CDMPTR->CRT2->VDisplay - 1;
    pScrn1->frameX1   = pScrn1->frameX0   + pXGI->CurrentLayout.mode->HDisplay  - 1;
    pScrn1->frameY1   = pScrn1->frameY0   + pXGI->CurrentLayout.mode->VDisplay  - 1;

    XGIAdjustFrameHW_CRT1(pScrn1, pXGI->CRT1frameX0, pXGI->CRT1frameY0);
} */
#endif

/*
 * This function is used to initialize the Start Address - the first
 * displayed location in the video memory.
 *
 * Usually mandatory
 */
void
XGIAdjustFrame(int scrnIndex, int x, int y, int flags)
{
    ScrnInfoPtr pScrn = xf86Screens[scrnIndex];
    XGIPtr pXGI = XGIPTR(pScrn) ;
    unsigned long base ;
    unsigned char ucSR5Stat, ucTemp ;

    inXGIIDXREG(XGISR, 0x05, ucSR5Stat ) ;
    if( ucSR5Stat == 0xA1 ) ucSR5Stat = 0x86 ;
    outXGIIDXREG(XGISR, 0x05, 0x86) ;

    base = (pScrn->bitsPerPixel + 7 )/8 ;
    base *= x ;
    base += pXGI->scrnOffset * y ;
    base >>= 2 ;

    switch( pXGI->Chipset )
    {
    case PCI_CHIP_XGIXG40:
	default:

        ucTemp = base & 0xFF       ; outXGIIDXREG( XGICR, 0x0D, ucTemp ) ;
        ucTemp = (base>>8) & 0xFF  ; outXGIIDXREG( XGICR, 0x0C, ucTemp ) ;
        ucTemp = (base>>16) & 0xFF ; outXGIIDXREG( XGISR, 0x0D, ucTemp ) ;
        ucTemp = (base>>24) & 0x01 ; setXGIIDXREG( XGISR, 0x37, 0xFE, ucTemp ) ;

/*        if (pXGI->VBFlags)  {
            XGI_UnLockCRT2(&(pXGI->xgi_HwDevExt),pXGI->pVBInfo);
            ucTemp = base & 0xFF       ; outXGIIDXREG( XGIPART1, 6 , ucTemp ) ;
            ucTemp = (base>>8) & 0xFF  ; outXGIIDXREG( XGIPART1, 5 , ucTemp ) ;
            ucTemp = (base>>16) & 0xFF ; outXGIIDXREG( XGIPART1, 4 , ucTemp ) ;
            ucTemp = (base>>24) & 0x01 ; ucTemp <<= 7 ;
            setXGIIDXREG( XGIPART1, 0x2, 0x7F, ucTemp ) ;

            XGI_LockCRT2(&(pXGI->xgi_HwDevExt),pXGI->pVBInfo);
        }  */
        break ;

    }

    outXGIIDXREG(XGISR, 0x05, ucSR5Stat ) ;

}

/*
 * This is called when VT switching back to the X server.  Its job is
 * to reinitialise the video mode.
 * Mandatory!
 */
static Bool
XGIEnterVT(int scrnIndex, int flags)
{
    ScrnInfoPtr pScrn = xf86Screens[scrnIndex];
    XGIPtr pXGI = XGIPTR(pScrn);

    xgiSaveUnlockExtRegisterLock(pXGI, NULL, NULL);

    if(!XGIModeInit(pScrn, pScrn->currentMode)) {
       XGIErrorLog(pScrn, "XGIEnterVT: XGIModeInit() failed\n");
       return FALSE;
    }

    XGIAdjustFrame(scrnIndex, pScrn->frameX0, pScrn->frameY0, 0);

#ifdef XF86DRI
/*    ScreenPtr pScreen; */
    if(pXGI->directRenderingEnabled) {
       DRIUnlock(screenInfo.screens[scrnIndex]);
    }
#endif

#ifdef XGIDUALHEAD
    if((!pXGI->DualHeadMode) || (!pXGI->SecondHead))
#endif
       if(pXGI->ResetXv) {
          (pXGI->ResetXv)(pScrn);
       }

    return TRUE;
}

/*
 * This is called when VT switching away from the X server.  Its job is
 * to restore the previous (text) mode.
 * Mandatory!
 */
static void
XGILeaveVT(int scrnIndex, int flags)
{
    ScrnInfoPtr pScrn = xf86Screens[scrnIndex];
    vgaHWPtr hwp = VGAHWPTR(pScrn);
    XGIPtr pXGI = XGIPTR(pScrn);
#ifdef XF86DRI
    ScreenPtr pScreen;

    if(pXGI->directRenderingEnabled) {
       pScreen = screenInfo.screens[scrnIndex];
       DRILock(pScreen, 0);
    }
#endif

#ifdef XGIDUALHEAD
    if(pXGI->DualHeadMode && pXGI->SecondHead) return;
#endif

    if(pXGI->CursorInfoPtr) {
#ifdef XGIDUALHEAD
       if(pXGI->DualHeadMode) {
          if(!pXGI->SecondHead) {
	     pXGI->ForceCursorOff = TRUE;
	     pXGI->CursorInfoPtr->HideCursor(pScrn);
	     XGIWaitVBRetrace(pScrn);
	     pXGI->ForceCursorOff = FALSE;
	  }
       } else {
#endif
          pXGI->CursorInfoPtr->HideCursor(pScrn);
          XGIWaitVBRetrace(pScrn);
#ifdef XGIDUALHEAD
       }
#endif
    }

    XGIBridgeRestore(pScrn);

    if(pXGI->UseVESA) {

       /* This is a q&d work-around for a BIOS bug. In case we disabled CRT2,
    	* VBESaveRestore() does not restore CRT1. So we set any mode now,
	* because VBESetVBEMode correctly restores CRT1. Afterwards, we
	* can call VBESaveRestore to restore original mode.
	*/
       if((pXGI->VBFlags & VB_VIDEOBRIDGE) && (!(pXGI->VBFlags & DISPTYPE_DISP2)))
	  VBESetVBEMode(pXGI->pVbe, (pXGI->XGIVESAModeList->n) | 0xc000, NULL);

       XGIVESARestore(pScrn);

    } else {

       XGIRestore(pScrn);

    }

    /* We use (otherwise unused) bit 7 to indicate that we are running
     * to keep xgifb to change the displaymode (this would result in
     * lethal display corruption upon quitting X or changing to a VT
     * until a reboot)
     */

    vgaHWLock(hwp);
}


/*
 * This is called at the end of each server generation.  It restores the
 * original (text) mode.  It should really also unmap the video memory too.
 * Mandatory!
 */
static Bool
XGICloseScreen(int scrnIndex, ScreenPtr pScreen)
{
    ScrnInfoPtr pScrn = xf86Screens[scrnIndex];
    vgaHWPtr hwp = VGAHWPTR(pScrn);
    XGIPtr pXGI = XGIPTR(pScrn);
#ifdef XGIDUALHEAD
    XGIEntPtr pXGIEnt = pXGI->entityPrivate;
#endif

#ifdef XF86DRI
    if(pXGI->directRenderingEnabled) {
       XGIDRICloseScreen(pScreen);
       pXGI->directRenderingEnabled = FALSE;
    }
#endif

    if(pScrn->vtSema) {

        if(pXGI->CursorInfoPtr) {
#ifdef XGIDUALHEAD
           if(pXGI->DualHeadMode) {
              if(!pXGI->SecondHead) {
	         pXGI->ForceCursorOff = TRUE;
	         pXGI->CursorInfoPtr->HideCursor(pScrn);
	         XGIWaitVBRetrace(pScrn);
	         pXGI->ForceCursorOff = FALSE;
	      }
           } else {
#endif
             pXGI->CursorInfoPtr->HideCursor(pScrn);
             XGIWaitVBRetrace(pScrn);
#ifdef XGIDUALHEAD
           }
#endif
	}

        XGIBridgeRestore(pScrn);

	if(pXGI->UseVESA) {

	  /* This is a q&d work-around for a BIOS bug. In case we disabled CRT2,
    	   * VBESaveRestore() does not restore CRT1. So we set any mode now,
	   * because VBESetVBEMode correctly restores CRT1. Afterwards, we
	   * can call VBESaveRestore to restore original mode.
	   */
           if((pXGI->VBFlags & VB_VIDEOBRIDGE) && (!(pXGI->VBFlags & DISPTYPE_DISP2)))
	      VBESetVBEMode(pXGI->pVbe, (pXGI->XGIVESAModeList->n) | 0xc000, NULL);

	   XGIVESARestore(pScrn);

	} else {

	   XGIRestore(pScrn);

	}

        vgaHWLock(hwp);

    }

    /* We should restore the mode number in case vtsema = false as well,
     * but since we haven't register access then we can't do it. I think
     * I need to rework the save/restore stuff, like saving the video
     * status when returning to the X server and by that save me the
     * trouble if xgifb was started from a textmode VT while X was on.
     */

    XGIUnmapMem(pScrn);
    vgaHWUnmapMem(pScrn);

#ifdef XGIDUALHEAD
    if(pXGI->DualHeadMode) {
       pXGIEnt = pXGI->entityPrivate;
       pXGIEnt->refCount--;
    }
#endif

    if(pXGI->pInt) {
       xf86FreeInt10(pXGI->pInt);
       pXGI->pInt = NULL;
    }

    if(pXGI->AccelLinearScratch) {
       xf86FreeOffscreenLinear(pXGI->AccelLinearScratch);
       pXGI->AccelLinearScratch = NULL;
    }

    if(pXGI->AccelInfoPtr) {
       XAADestroyInfoRec(pXGI->AccelInfoPtr);
       pXGI->AccelInfoPtr = NULL;
    }

    if(pXGI->CursorInfoPtr) {
       xf86DestroyCursorInfoRec(pXGI->CursorInfoPtr);
       pXGI->CursorInfoPtr = NULL;
    }

    if(pXGI->ShadowPtr) {
       xfree(pXGI->ShadowPtr);
       pXGI->ShadowPtr = NULL;
    }

    if(pXGI->DGAModes) {
       xfree(pXGI->DGAModes);
       pXGI->DGAModes = NULL;
    }

    if(pXGI->adaptor) {
       xfree(pXGI->adaptor);
       pXGI->adaptor = NULL;
       pXGI->ResetXv = pXGI->ResetXvGamma = NULL;
    }

    pScrn->vtSema = FALSE;

    /* Restore Blockhandler */
    pScreen->BlockHandler = pXGI->BlockHandler;

    pScreen->CloseScreen = pXGI->CloseScreen;

    return(*pScreen->CloseScreen)(scrnIndex, pScreen);
}


/* Free up any per-generation data structures */

/* Optional */
static void
XGIFreeScreen(int scrnIndex, int flags)
{
    if(xf86LoaderCheckSymbol("vgaHWFreeHWRec")) {
       vgaHWFreeHWRec(xf86Screens[scrnIndex]);
    }

    XGIFreeRec(xf86Screens[scrnIndex]);
}


/* Checks if a mode is suitable for the selected chipset. */

static ModeStatus
XGIValidMode(int scrnIndex, DisplayModePtr mode, Bool verbose, int flags)
{
    ScrnInfoPtr pScrn = xf86Screens[scrnIndex];
    XGIPtr pXGI = XGIPTR(pScrn);
    int HDisplay = mode->HDisplay ;
    int VDisplay = mode->VDisplay ;
    int Clock = mode->Clock;
    int i = 0;
PDEBUG(ErrorF(" XGIValidMode(). HD: %d, VD:%d, Clock: %d \n", HDisplay, VDisplay, Clock));
    if( (pXGI->VBFlags & CRT2_LCD)||(pXGI->VBFlags & CRT2_TV))
    {
        if( HDisplay > 1024 && VDisplay > 768 ) return(MODE_NOMODE) ;
        if( HDisplay < 640 && VDisplay < 480 ) return(MODE_NOMODE) ;
    }
    else if( pXGI->VBFlags & CRT2_VGA )
    {
        if( HDisplay > 1280 && VDisplay > 1280 ) return(MODE_NOMODE) ;
        if( HDisplay < 640 && VDisplay < 480 ) return(MODE_NOMODE) ;

        if( HDisplay == 1280 && VDisplay == 1280 && mode->VRefresh > 75 )
        {
            return(MODE_NOMODE) ;
        }

    }
    if( pXGI->Chipset == PCI_CHIP_XGIXG20 )
    {
        XgiMode = XG20_Mode ;  
        PDEBUG(ErrorF(" ---XG20_Mode \n"));
    }
    else
    {
        XgiMode = XGI_Mode ; 
        PDEBUG(ErrorF(" ---XGI_Mode \n"));
    }
    while ( (XgiMode[i].Clock != Clock) || (XgiMode[i].HDisplay != HDisplay) || (XgiMode[i].VDisplay != VDisplay) )
    {
        if (XgiMode[i].Clock == 0) {
            PDEBUG(ErrorF("--- NO_Mode support \n"));
            return(MODE_NOMODE) ;
        }
        else
            i++;
    }

    return(MODE_OK);
}

/* Do screen blanking
 *
 * Mandatory
 */
static Bool
XGISaveScreen(ScreenPtr pScreen, int mode)
{
    ScrnInfoPtr pScrn = xf86Screens[pScreen->myNum];

    if((pScrn != NULL) && pScrn->vtSema) {

    	XGIPtr pXGI = XGIPTR(pScrn);

#ifdef UNLOCK_ALWAYS
        xgiSaveUnlockExtRegisterLock(pXGI, NULL, NULL);
#endif
    }

    return vgaHWSaveScreen(pScreen, mode);
}

#ifdef XGIDUALHEAD
/* SaveScreen for dual head mode */
static Bool
XGISaveScreenDH(ScreenPtr pScreen, int mode)
{
    ScrnInfoPtr pScrn = xf86Screens[pScreen->myNum];
    Bool checkit = FALSE;

    if((pScrn != NULL) && pScrn->vtSema) {

       XGIPtr pXGI = XGIPTR(pScrn);

       if((pXGI->SecondHead) && ((!(pXGI->VBFlags & CRT1_LCDA)) || (pXGI->VBFlags & VB_301C))) {

	  /* Slave head is always CRT1 */
	  if(pXGI->VBFlags & CRT1_LCDA) pXGI->Blank = xf86IsUnblank(mode) ? FALSE : TRUE;

	  return vgaHWSaveScreen(pScreen, mode);

       } else {

	  /* Master head is always CRT2 */
	  /* But we land here if CRT1 is LCDA, too */

	  /* We can only blank LCD, not other CRT2 devices */
	  if(!(pXGI->VBFlags & (CRT2_LCD|CRT1_LCDA))) return TRUE;

	  /* enable access to extended sequencer registers */
#ifdef UNLOCK_ALWAYS
          xgiSaveUnlockExtRegisterLock(pXGI, NULL, NULL);
#endif
	  if(checkit) {
	     if(!pXGI->SecondHead) pXGI->BlankCRT2 = xf86IsUnblank(mode) ? FALSE : TRUE;
	     else if(pXGI->VBFlags & CRT1_LCDA) pXGI->Blank = xf86IsUnblank(mode) ? FALSE : TRUE;
	  }

       }
    }
    return TRUE;
}
#endif

#ifdef DEBUG
void
XGIDumpModeInfo(ScrnInfoPtr pScrn, DisplayModePtr mode)
{
    xf86DrvMsg(pScrn->scrnIndex,X_INFO, "Clock : %x\n", mode->Clock);
    xf86DrvMsg(pScrn->scrnIndex,X_INFO, "Hz Display : %x\n", mode->CrtcHDisplay);
    xf86DrvMsg(pScrn->scrnIndex,X_INFO, "Hz Blank Start : %x\n", mode->CrtcHBlankStart);
    xf86DrvMsg(pScrn->scrnIndex,X_INFO, "Hz Sync Start : %x\n", mode->CrtcHSyncStart);
    xf86DrvMsg(pScrn->scrnIndex,X_INFO, "Hz Sync End : %x\n", mode->CrtcHSyncEnd);
    xf86DrvMsg(pScrn->scrnIndex,X_INFO, "Hz Blank End : %x\n", mode->CrtcHBlankEnd);
    xf86DrvMsg(pScrn->scrnIndex,X_INFO, "Hz Total : %x\n", mode->CrtcHTotal);
    xf86DrvMsg(pScrn->scrnIndex,X_INFO, "Hz Skew : %x\n", mode->CrtcHSkew);
    xf86DrvMsg(pScrn->scrnIndex,X_INFO, "Hz HAdjusted : %x\n", mode->CrtcHAdjusted);
    xf86DrvMsg(pScrn->scrnIndex,X_INFO, "Vt Display : %x\n", mode->CrtcVDisplay);
    xf86DrvMsg(pScrn->scrnIndex,X_INFO, "Vt Blank Start : %x\n", mode->CrtcVBlankStart);
    xf86DrvMsg(pScrn->scrnIndex,X_INFO, "Vt Sync Start : %x\n", mode->CrtcVSyncStart);
    xf86DrvMsg(pScrn->scrnIndex,X_INFO, "Vt Sync End : %x\n", mode->CrtcVSyncEnd);
    xf86DrvMsg(pScrn->scrnIndex,X_INFO, "Vt Blank End : %x\n", mode->CrtcVBlankEnd);
    xf86DrvMsg(pScrn->scrnIndex,X_INFO, "Vt Total : %x\n", mode->CrtcVTotal);
    xf86DrvMsg(pScrn->scrnIndex,X_INFO, "Vt VAdjusted : %x\n", mode->CrtcVAdjusted);
}
#endif

static void
XGIModifyModeInfo(DisplayModePtr mode)
{
    if(mode->CrtcHBlankStart == mode->CrtcHDisplay)
        mode->CrtcHBlankStart++;
    if(mode->CrtcHBlankEnd == mode->CrtcHTotal)
        mode->CrtcHBlankEnd--;
    if(mode->CrtcVBlankStart == mode->CrtcVDisplay)
        mode->CrtcVBlankStart++;
    if(mode->CrtcVBlankEnd == mode->CrtcVTotal)
        mode->CrtcVBlankEnd--;
}

/* Enable the Turboqueue/Commandqueue (For 300 and 315/330 series only) */
void
XGIEnableTurboQueue(ScrnInfoPtr pScrn)
{
/*    XGIPtr pXGI = XGIPTR(pScrn);
    unsigned short SR26, SR27;
    unsigned long  temp;  */
}

/* Things to do before a ModeSwitch. We set up the
 * video bridge configuration and the TurboQueue.
 */
void XGIPreSetMode(ScrnInfoPtr pScrn, DisplayModePtr mode, int viewmode)
{
    XGIPtr         pXGI = XGIPTR(pScrn);
    unsigned char  CR30, CR31, CR32, CR33;
    unsigned char  CR3B = 0;
    unsigned char  CR17, CR38 = 0;
    unsigned char  CR35 = 0, CR79 = 0;
    unsigned long  vbflag;
    int            temp = 0, i;
    int 	   crt1rateindex = 0;
    DisplayModePtr mymode;
#ifdef XGIMERGED
    DisplayModePtr mymode2 = NULL;
#endif

#ifdef XGIMERGED
    if(pXGI->MergedFB) {
       mymode = ((XGIMergedDisplayModePtr)mode->Private)->CRT1;
       mymode2 = ((XGIMergedDisplayModePtr)mode->Private)->CRT2;
    } else
#endif
    mymode = mode;

    vbflag = pXGI->VBFlags;
    pXGI->IsCustom = FALSE;
#ifdef XGIMERGED
    pXGI->IsCustomCRT2 = FALSE;

    if(pXGI->MergedFB) {
       /* CRT2 */
       if(vbflag & CRT2_LCD) {
          if(pXGI->XGI_Pr->CP_HaveCustomData) {
	     for(i=0; i<7; i++) {
	        if(pXGI->XGI_Pr->CP_DataValid[i]) {
	           if((mymode2->HDisplay == pXGI->XGI_Pr->CP_HDisplay[i]) &&
	              (mymode2->VDisplay == pXGI->XGI_Pr->CP_VDisplay[i])) {
	              if(mymode2->type & M_T_BUILTIN) {
	                 pXGI->IsCustomCRT2 = TRUE;
		      }
	           }
		}
	     }
	  }
       }
       if(vbflag & (CRT2_VGA|CRT2_LCD)) {
          if(pXGI->AddedPlasmaModes) {
	     if(mymode2->type & M_T_BUILTIN) {
	        pXGI->IsCustomCRT2 = TRUE;
	     }
	  }
	  if(pXGI->HaveCustomModes2) {
             if(!(mymode2->type & M_T_DEFAULT)) {
	        pXGI->IsCustomCRT2 = TRUE;
             }
          }
       }
       /* CRT1 */
       if(pXGI->HaveCustomModes) {
          if(!(mymode->type & M_T_DEFAULT)) {
	     pXGI->IsCustom = TRUE;
          }
       }
    } else
#endif
#ifdef XGIDUALHEAD
    if(pXGI->DualHeadMode) {
       if(!pXGI->SecondHead) {
          /* CRT2 */
          if(vbflag & CRT2_LCD) {
	     if(pXGI->XGI_Pr->CP_HaveCustomData) {
	        for(i=0; i<7; i++) {
                   if(pXGI->XGI_Pr->CP_DataValid[i]) {
	              if((mymode->HDisplay == pXGI->XGI_Pr->CP_HDisplay[i]) &&
	                 (mymode->VDisplay == pXGI->XGI_Pr->CP_VDisplay[i])) {
	                 if(mymode->type & M_T_BUILTIN) {
	                    pXGI->IsCustom = TRUE;
		         }
		      }
		   }
	        }
	     }
          }
	  if(vbflag & (CRT2_VGA|CRT2_LCD)) {
             if(pXGI->AddedPlasmaModes) {
	        if(mymode->type & M_T_BUILTIN) {
	           pXGI->IsCustom = TRUE;
	        }
	     }
	     if(pXGI->HaveCustomModes) {
                if(!(mymode->type & M_T_DEFAULT)) {
	           pXGI->IsCustom = TRUE;
                }
             }
          }
       } else {
          /* CRT1 */
          if(pXGI->HaveCustomModes) {
             if(!(mymode->type & M_T_DEFAULT)) {
	        pXGI->IsCustom = TRUE;
             }
          }
       }
    } else
#endif
    {
       if(vbflag & CRT2_LCD) {
          if(pXGI->XGI_Pr->CP_HaveCustomData) {
	     for(i=0; i<7; i++) {
	        if(pXGI->XGI_Pr->CP_DataValid[i]) {
                   if((mymode->HDisplay == pXGI->XGI_Pr->CP_HDisplay[i]) &&
	              (mymode->VDisplay == pXGI->XGI_Pr->CP_VDisplay[i])) {
	              if(mymode->type & M_T_BUILTIN) {
	                 pXGI->IsCustom = TRUE;
	              }
		   }
	        }
	     }
          }
       }
       if(vbflag & (CRT2_LCD|CRT2_VGA)) {
          if(pXGI->AddedPlasmaModes) {
             if(mymode->type & M_T_BUILTIN) {
	        pXGI->IsCustom = TRUE;
             }
          }
       }
       if((pXGI->HaveCustomModes) && (!(vbflag & CRT2_TV))) {
          if(!(mymode->type & M_T_DEFAULT)) {
	     pXGI->IsCustom = TRUE;
          }
       }
    }

#ifdef UNLOCK_ALWAYS
    xgiSaveUnlockExtRegisterLock(pXGI, NULL, NULL);    /* Unlock Registers */
#endif

    inXGIIDXREG(XGICR, 0x30, CR30);
    inXGIIDXREG(XGICR, 0x31, CR31);
    CR32 = pXGI->newCR32;
    inXGIIDXREG(XGICR, 0x33, CR33);

       inXGIIDXREG(XGICR, 0x3b, CR3B);
       xf86DrvMsgVerb(pScrn->scrnIndex, X_PROBED, 4,
	   "Before: CR30=0x%02x, CR31=0x%02x, CR32=0x%02x, CR33=0x%02x, CR%02x=0x%02x\n",
              CR30, CR31, CR32, CR33, temp, CR38);

    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, 4, "VBFlags=0x%lx\n", pXGI->VBFlags);

    CR30 = 0x00;
    CR31 &= ~0x60;  /* Clear VB_Drivermode & VB_OutputDisable */
    CR31 |= 0x04;   /* Set VB_NotSimuMode (not for 30xB/1400x1050?) */
    CR35 = 0x00;


    if(!pXGI->AllowHotkey) {
       CR31 |= 0x80;   /* Disable hotkey-switch */
    }
       CR79 &= ~0x10;     /* Enable Backlight control on 315 series */

    XGI_SetEnableDstn(pXGI->XGI_Pr, FALSE);
    XGI_SetEnableFstn(pXGI->XGI_Pr, FALSE);

    if((vbflag & CRT1_LCDA) && (viewmode == XGI_MODE_CRT1)) {

       CR38 |= 0x02;

    } else {

       switch(vbflag & (CRT2_TV|CRT2_LCD|CRT2_VGA)) {

       case CRT2_TV:

          CR38 &= ~0xC0; 	/* Clear Pal M/N bits */

          if((vbflag & VB_CHRONTEL) && (vbflag & TV_CHSCART)) {			/* Chrontel */
	     CR30 |= 0x10;
	     CR38 |= 0x04;
	     CR38 &= ~0x08;
	     CR31 |= 0x01;
	  } else if((vbflag & VB_CHRONTEL) && (vbflag & TV_CHYPBPR525I)) {	/* Chrontel */
	     CR38 |= 0x08;
	     CR38 &= ~0x04;
	     CR31 &= ~0x01;
          } else if(vbflag & TV_HIVISION) {	/* Video bridge */
	     CR30 |= 0x80;
	     CR31 |= 0x01;
	     CR35 |= 0x01;
	  } else if(vbflag & TV_YPBPR) {					/* Video bridge */
	     if(pXGI->XGI_SD_Flags & XGI_SD_SUPPORTYPBPR) {
	        CR30 |= 0x80;
		CR38 |= 0x08;
	        if(vbflag & TV_YPBPR525P)       CR38 |= 0x10;
		else if(vbflag & TV_YPBPR750P)  CR38 |= 0x20;
		else if(vbflag & TV_YPBPR1080I) CR38 |= 0x30;
		CR31 &= ~0x01;
		if(pXGI->XGI_SD_Flags & XGI_SD_SUPPORTYPBPRAR) {
		   CR3B &= ~0x03;
		   if((vbflag & TV_YPBPRAR) == TV_YPBPR43LB)     CR3B |= 0x00;
		   else if((vbflag & TV_YPBPRAR) == TV_YPBPR43)  CR3B |= 0x03;
		   else if((vbflag & TV_YPBPRAR) == TV_YPBPR169) CR3B |= 0x01;
		   else					         CR3B |= 0x03;
		}
	     }
          } else {								/* All */
	     if(vbflag & TV_SCART)  CR30 |= 0x10;
	     if(vbflag & TV_SVIDEO) CR30 |= 0x08;
	     if(vbflag & TV_AVIDEO) CR30 |= 0x04;
	     if(!(CR30 & 0x1C))	    CR30 |= 0x08;    /* default: SVIDEO */

	     if(vbflag & TV_PAL) {
		CR31 |= 0x01;
		CR35 |= 0x01;
		if( (vbflag & VB_XGIBRIDGE) ||
		    ((vbflag & VB_CHRONTEL) && (pXGI->ChrontelType == CHRONTEL_701x)) )  {
		   if(vbflag & TV_PALM) {
		      CR38 |= 0x40;
		      CR35 |= 0x04;
		   } else if(vbflag & TV_PALN) {
		      CR38 |= 0x80;
		      CR35 |= 0x08;
	  	   }
	        }
	     } else {
		CR31 &= ~0x01;
		CR35 &= ~0x01;
		if(vbflag & TV_NTSCJ) {
		   CR38 |= 0x40;  /* TW, not BIOS */
		   CR35 |= 0x02;
	 	}
	     }
	     if(vbflag & TV_SCART) {
	        CR31 |= 0x01;
		CR35 |= 0x01;
	     }
	  }

	  CR31 &= ~0x04;   /* Clear NotSimuMode */
	  pXGI->XGI_Pr->XGI_CHOverScan = pXGI->UseCHOverScan;
	  if((pXGI->OptTVSOver == 1) && (pXGI->ChrontelType == CHRONTEL_700x)) {
	     pXGI->XGI_Pr->XGI_CHSOverScan = TRUE;
	  } else {
	     pXGI->XGI_Pr->XGI_CHSOverScan = FALSE;
	  }
#ifdef XGI_CP
	  XGI_CP_DRIVER_CONFIG
#endif
          break;

       case CRT2_LCD:
          CR30 |= 0x20;
	  XGI_SetEnableDstn(pXGI->XGI_Pr, pXGI->DSTN);
	  XGI_SetEnableFstn(pXGI->XGI_Pr, pXGI->FSTN);
          break;

       case CRT2_VGA:
          CR30 |= 0x40;
          break;

       default:
          CR30 |= 0x00;
          CR31 |= 0x20;    /* VB_OUTPUT_DISABLE */
	  if(pXGI->UseVESA) {
	     crt1rateindex = XGISearchCRT1Rate(pScrn, mymode);
	  }
       }

    }

    if(vbflag & CRT1_LCDA) {
       switch(viewmode) {
       case XGI_MODE_CRT1:
          CR38 |= 0x01;
          break;
       case XGI_MODE_CRT2:
          if(vbflag & (CRT2_TV|CRT2_VGA)) {
             CR30 |= 0x02;
	     CR38 |= 0x01;
	  } else {
	     CR38 |= 0x03;
	  }
          break;
       case XGI_MODE_SIMU:
       default:
          if(vbflag & (CRT2_TV|CRT2_LCD|CRT2_VGA)) {
             CR30 |= 0x01;
	  }
          break;
       }
    } else {
       if(vbflag & (CRT2_TV|CRT2_LCD|CRT2_VGA)) {
          CR30 |= 0x01;
       }
    }

    /* for VESA: no DRIVERMODE, otherwise
     * -) CRT2 will not be initialized correctly when using mode
     *    where LCD has to scale, and
     * -) CRT1 will have too low rate
     */
     if(pXGI->UseVESA) {
        CR31 &= ~0x40;   /* Clear Drivermode */
	CR31 |= 0x06;    /* Set SlaveMode, Enable SimuMode in Slavemode */
#ifdef TWDEBUG
        CR31 |= 0x40;    /* DEBUG (for non-slave mode VESA) */
	crt1rateindex = XGISearchCRT1Rate(pScrn, mymode);
#endif
     } else {
        CR31 |=  0x40;  /* Set Drivermode */
	CR31 &=  ~0x06; /* Disable SlaveMode, disable SimuMode in SlaveMode */
	if(!pXGI->IsCustom) {
           crt1rateindex = XGISearchCRT1Rate(pScrn, mymode);
	} else {
	   crt1rateindex = CR33;
	}
     }

#ifdef XGIDUALHEAD
     if(pXGI->DualHeadMode) {
        if(pXGI->SecondHead) {
	    /* CRT1 */
	    CR33 &= 0xf0;
	    if(!(vbflag & CRT1_LCDA)) {
	       CR33 |= (crt1rateindex & 0x0f);
	    }
	} else {
	    /* CRT2 */
	    CR33 &= 0x0f;
	    if(vbflag & CRT2_VGA) {
	       CR33 |= ((crt1rateindex << 4) & 0xf0);
	    }
	}
     } else
#endif
#ifdef XGIMERGED
     if(pXGI->MergedFB) {
        CR33 = 0;
	if(!(vbflag & CRT1_LCDA)) {
	   CR33 |= (crt1rateindex & 0x0f);
	}
        if(vbflag & CRT2_VGA) {
	   if(!pXGI->IsCustomCRT2) {
	      CR33 |= (XGISearchCRT1Rate(pScrn, mymode2) << 4);
	   }
	}
     } else
#endif
     {
        CR33 = 0;
	if(!(vbflag & CRT1_LCDA)) {
	   CR33 |= (crt1rateindex & 0x0f);
	}
        if(vbflag & CRT2_VGA) {
           CR33 |= ((crt1rateindex & 0x0f) << 4);
	}
	if((!(pXGI->UseVESA)) && (vbflag & CRT2_ENABLE)) {
	   if(pXGI->CRT1off) CR33 &= 0xf0;
	}
     }
        outXGIIDXREG(XGICR, 0x30, CR30);
        outXGIIDXREG(XGICR, 0x31, CR31);
        outXGIIDXREG(XGICR, 0x33, CR33);
        if(temp) {
           outXGIIDXREG(XGICR, temp, CR38);
        }
	xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, 4,
		"After:  CR30=0x%02x,CR31=0x%02x,CR33=0x%02x,CR%02x=%02x\n",
		    CR30, CR31, CR33, temp, CR38);

     pXGI->XGI_Pr->XGI_UseOEM = pXGI->OptUseOEM;

     /* Enable TurboQueue */
#ifdef XGIVRAMQ
     if(pXGI->VGAEngine != XGI_315_VGA)
#endif
        XGIEnableTurboQueue(pScrn);

     if((!pXGI->UseVESA) && (pXGI->VBFlags & CRT2_ENABLE)) {
        /* Switch on CRT1 for modes that require the bridge in SlaveMode */
	andXGIIDXREG(XGISR,0x1f,0x3f);
	inXGIIDXREG(XGICR, 0x17, CR17);
	if(!(CR17 & 0x80)) {
           orXGIIDXREG(XGICR, 0x17, 0x80);
	   outXGIIDXREG(XGISR, 0x00, 0x01);
	   usleep(10000);
           outXGIIDXREG(XGISR, 0x00, 0x03);
	}
     }
}

/* PostSetMode:
 * -) Disable CRT1 for saving bandwidth. This doesn't work with VESA;
 *    VESA uses the bridge in SlaveMode and switching CRT1 off while
 *    the bridge is in SlaveMode not that clever...
 * -) Check if overlay can be used (depending on dotclock)
 * -) Check if Panel Scaler is active on LVDS for overlay re-scaling
 * -) Save TV registers for further processing
 * -) Apply TV settings
 */
static void
XGIPostSetMode(ScrnInfoPtr pScrn, XGIRegPtr xgiReg)
{
    XGIPtr pXGI = XGIPTR(pScrn);
/* #ifdef XGIDUALHEAD
    XGIEntPtr pXGIEnt = pXGI->entityPrivate;
#endif */
/*    unsigned char usScratchCR17;
    Bool flag = FALSE;
    Bool doit = TRUE; */
    int myclock;
    unsigned char  sr2b, sr2c, tmpreg;
    float          num, denum, postscalar, divider;
PDEBUG(ErrorF(" XGIPostSetMode(). \n"));
#ifdef TWDEBUG
    xf86DrvMsg(pScrn->scrnIndex, X_INFO,
    	"CRT1off is %d\n", pXGI->CRT1off);
#endif
    pXGI->CRT1isoff = pXGI->CRT1off;

#ifdef UNLOCK_ALWAYS
    xgiSaveUnlockExtRegisterLock(pXGI, NULL, NULL);
#endif

    /* Determine if the video overlay can be used */
    if(!pXGI->NoXvideo) {
       inXGIIDXREG(XGISR,0x2b,sr2b);
       inXGIIDXREG(XGISR,0x2c,sr2c);
       divider = (sr2b & 0x80) ? 2.0 : 1.0;
       postscalar = (sr2c & 0x80) ?
              ( (((sr2c >> 5) & 0x03) == 0x02) ? 6.0 : 8.0 ) :
	      ( ((sr2c >> 5) & 0x03) + 1.0 );
       num = (sr2b & 0x7f) + 1.0;
       denum = (sr2c & 0x1f) + 1.0;
       myclock = (int)((14318 * (divider / postscalar) * (num / denum)) / 1000);

       pXGI->MiscFlags &= ~(MISC_CRT1OVERLAY | MISC_CRT1OVERLAYGAMMA);
/*       switch(pXGI->xgi_HwDevExt.jChipType) {
            break;
       } */
       if(!(pXGI->MiscFlags & MISC_CRT1OVERLAY)) {
#ifdef XGIDUALHEAD
          if((!pXGI->DualHeadMode) || (pXGI->SecondHead))
#endif
             xf86DrvMsgVerb(pScrn->scrnIndex, X_WARNING, 3,
	     	"Current dotclock (%dMhz) too high for video overlay on CRT1\n",
		myclock);
       }
    }

    /* Determine if the Panel Link scaler is active */
    pXGI->MiscFlags &= ~MISC_PANELLINKSCALER;
    if(pXGI->VBFlags & (CRT2_LCD | CRT1_LCDA)) {
          if(pXGI->VBFlags & (VB_LVDS | VB_30xBDH | CRT1_LCDA)) {
	     inXGIIDXREG(XGIPART1,0x35,tmpreg);
	     tmpreg &= 0x04;
	     if(!tmpreg) pXGI->MiscFlags |= MISC_PANELLINKSCALER;
	  }
    }

    /* Determine if our very special TV mode is active */
    pXGI->MiscFlags &= ~MISC_TVNTSC1024;
    if((pXGI->VBFlags & VB_XGIBRIDGE) && (pXGI->VBFlags & CRT2_TV) && (!(pXGI->VBFlags & TV_HIVISION))) {
       if( ((pXGI->VBFlags & TV_YPBPR) && (pXGI->VBFlags & TV_YPBPR525I)) ||
           ((!(pXGI->VBFlags & TV_YPBPR)) && (pXGI->VBFlags & (TV_NTSC | TV_PALM))) ) {
          inXGIIDXREG(XGICR,0x34,tmpreg);
	  tmpreg &= 0x7f;
	  if((tmpreg == 0x64) || (tmpreg == 0x4a) || (tmpreg == 0x38)) {
	     pXGI->MiscFlags |= MISC_TVNTSC1024;
	  }
       }
    }

    /* Reset XV gamma correction */
    if(pXGI->ResetXvGamma) {
       (pXGI->ResetXvGamma)(pScrn);
    }

    /*  Apply TV settings given by options
           Do this even in DualHeadMode:
	   - if this is called by SetModeCRT1, CRT2 mode has been reset by SetModeCRT1
	   - if this is called by SetModeCRT2, CRT2 mode has changed (duh!)
	   -> Hence, in both cases, the settings must be re-applied.
     */
}

/* Build a list of the VESA modes the BIOS reports as valid */
static void
XGIBuildVesaModeList(ScrnInfoPtr pScrn, vbeInfoPtr pVbe, VbeInfoBlock *vbe)
{
    XGIPtr pXGI = XGIPTR(pScrn);
    int i = 0;

    while(vbe->VideoModePtr[i] != 0xffff) {
	xgiModeInfoPtr m;
	VbeModeInfoBlock *mode;
	int id = vbe->VideoModePtr[i++];
	int bpp;

	if((mode = VBEGetModeInfo(pVbe, id)) == NULL)
	    continue;

	bpp = mode->BitsPerPixel;

	m = xnfcalloc(sizeof(xgiModeInfoRec),1);
	m->width = mode->XResolution;
	m->height = mode->YResolution;
	m->bpp = bpp;
	m->n = id;
	m->next = pXGI->XGIVESAModeList;

	xf86DrvMsg(pScrn->scrnIndex, X_PROBED,
	      "BIOS supports VESA mode 0x%x: x:%i y:%i bpp:%i\n",
	       m->n, m->width, m->height, m->bpp);

	pXGI->XGIVESAModeList = m;

	VBEFreeModeInfo(mode);
    }
}

/* Calc VESA mode from given resolution/depth */
static UShort
XGICalcVESAModeIndex(ScrnInfoPtr pScrn, DisplayModePtr mode)
{
    XGIPtr pXGI = XGIPTR(pScrn);
    xgiModeInfoPtr m = pXGI->XGIVESAModeList;
    UShort i = (pScrn->bitsPerPixel+7)/8 - 1;
    UShort ModeIndex = 0;

    while(m) {
	if(pScrn->bitsPerPixel == m->bpp &&
	   mode->HDisplay == m->width &&
	   mode->VDisplay == m->height)
	    return m->n;
	m = m->next;
    }

    xf86DrvMsg(pScrn->scrnIndex, X_PROBED,
             "No valid BIOS VESA mode found for %dx%dx%d; searching built-in table.\n",
             mode->HDisplay, mode->VDisplay, pScrn->bitsPerPixel);

    switch(mode->HDisplay) {
      case 320:
          if(mode->VDisplay == 200)
             ModeIndex = VESAModeIndex_320x200[i];
	  else if(mode->VDisplay == 240)
             ModeIndex = VESAModeIndex_320x240[i];
          break;
      case 400:
          if(mode->VDisplay == 300)
             ModeIndex = VESAModeIndex_400x300[i];
          break;
      case 512:
          if(mode->VDisplay == 384)
             ModeIndex = VESAModeIndex_512x384[i];
          break;
      case 640:
          if(mode->VDisplay == 480)
             ModeIndex = VESAModeIndex_640x480[i];
	  else if(mode->VDisplay == 400)
             ModeIndex = VESAModeIndex_640x400[i];
          break;
      case 800:
          if(mode->VDisplay == 600)
             ModeIndex = VESAModeIndex_800x600[i];
          break;
      case 1024:
          if(mode->VDisplay == 768)
             ModeIndex = VESAModeIndex_1024x768[i];
          break;
      case 1280:
          if(mode->VDisplay == 1024)
             ModeIndex = VESAModeIndex_1280x1024[i];
          break;
      case 1600:
          if(mode->VDisplay == 1200)
             ModeIndex = VESAModeIndex_1600x1200[i];
          break;
      case 1920:
          if(mode->VDisplay == 1440)
             ModeIndex = VESAModeIndex_1920x1440[i];
          break;
   }

   if(!ModeIndex) xf86DrvMsg(pScrn->scrnIndex, X_PROBED,
        "No valid mode found for %dx%dx%d in built-in table either.\n",
	mode->HDisplay, mode->VDisplay, pScrn->bitsPerPixel);

   return(ModeIndex);
}

USHORT
XGI_CalcModeIndex(ScrnInfoPtr pScrn, DisplayModePtr mode, unsigned long VBFlags, BOOLEAN havecustommodes)
{
   XGIPtr pXGI = XGIPTR(pScrn);
   UShort i = (pXGI->CurrentLayout.bitsPerPixel+7)/8 - 1;

   if(!(VBFlags & CRT1_LCDA)) {
      if((havecustommodes) && (!(mode->type & M_T_DEFAULT))) {
         return 0xfe;
      }
   } else {
      if((mode->HDisplay > pXGI->LCDwidth) ||
         (mode->VDisplay > pXGI->LCDheight)) {
	 return 0;
      }
   }

   return(XGI_GetModeID(pXGI->VGAEngine, VBFlags, mode->HDisplay, mode->VDisplay,
   			i, pXGI->FSTN, pXGI->LCDwidth, pXGI->LCDheight));
}

USHORT
XGI_CheckCalcModeIndex(ScrnInfoPtr pScrn, DisplayModePtr mode, unsigned long VBFlags, BOOLEAN havecustommodes)
{
   XGIPtr pXGI = XGIPTR(pScrn);
/*   UShort i = (pXGI->CurrentLayout.bitsPerPixel+7)/8 - 1; */
   UShort ModeIndex = 0;
   int    j;

#ifdef TWDEBUG
   xf86DrvMsg(0, X_INFO, "Inside CheckCalcModeIndex (VBFlags %x, mode %dx%d)\n",
   	VBFlags,mode->HDisplay, mode->VDisplay);
#endif

   if(VBFlags & CRT2_LCD) {			/* CRT2 is LCD */

      if(pXGI->XGI_Pr->CP_HaveCustomData) {
         for(j=0; j<7; j++) {
            if((pXGI->XGI_Pr->CP_DataValid[j]) &&
               (mode->HDisplay == pXGI->XGI_Pr->CP_HDisplay[j]) &&
               (mode->VDisplay == pXGI->XGI_Pr->CP_VDisplay[j]) &&
               (mode->type & M_T_BUILTIN))
               return 0xfe;
	 }
      }

      if((pXGI->AddedPlasmaModes) && (mode->type & M_T_BUILTIN))
         return 0xfe;

      if((havecustommodes) &&
         (pXGI->LCDwidth) &&		/* = test if LCD present */
         (!(mode->type & M_T_DEFAULT)) &&
	 (!(mode->Flags & V_INTERLACE)))
         return 0xfe;

      if( ((mode->HDisplay <= pXGI->LCDwidth) &&
           (mode->VDisplay <= pXGI->LCDheight)) ||
	  ((pXGI->XGI_Pr->XGI_CustomT == CUT_PANEL848) &&
	   (((mode->HDisplay == 1360) && (mode->HDisplay == 768)) ||
	    ((mode->HDisplay == 1024) && (mode->HDisplay == 768)) ||
	    ((mode->HDisplay ==  800) && (mode->HDisplay == 600)))) ) {

      }

   } else if(VBFlags & CRT2_TV) {		/* CRT2 is TV */

   } else if(VBFlags & CRT2_VGA) {		/* CRT2 is VGA2 */

      if((pXGI->AddedPlasmaModes) && (mode->type & M_T_BUILTIN))
	 return 0xfe;

      if((havecustommodes) &&
	 (!(mode->type & M_T_DEFAULT)) &&
	 (!(mode->Flags & V_INTERLACE)))
         return 0xfe;

   } else {					/* CRT1 only, no CRT2 */

      ModeIndex = XGI_CalcModeIndex(pScrn, mode, VBFlags, havecustommodes);

   }

   return(ModeIndex);
}

/* Calculate the vertical refresh rate from a mode */
int
XGICalcVRate(DisplayModePtr mode)
{
   float hsync, refresh = 0;

   if(mode->HSync > 0.0)
       	hsync = mode->HSync;
   else if(mode->HTotal > 0)
       	hsync = (float)mode->Clock / (float)mode->HTotal;
   else
       	hsync = 0.0;

   if(mode->VTotal > 0)
       	refresh = hsync * 1000.0 / mode->VTotal;

   if(mode->Flags & V_INTERLACE)
       	refresh *= 2.0;

   if(mode->Flags & V_DBLSCAN)
       	refresh /= 2.0;

   if(mode->VScan > 1)
        refresh /= mode->VScan;

   if(mode->VRefresh > 0.0)
    	refresh = mode->VRefresh;

   if(hsync == 0 || refresh == 0) return(0);

   return((int)(refresh));
}

/* Calculate CR33 (rate index) for CRT1.
 * Calculation is done using currentmode, therefore it is
 * recommended to set VertRefresh and HorizSync to correct
 * values in config file.
 */
unsigned char
XGISearchCRT1Rate(ScrnInfoPtr pScrn, DisplayModePtr mode)
{
/*   XGIPtr         pXGI = XGIPTR(pScrn);  */
   int            i = 0;
   int            irefresh;
   unsigned short xres = mode->HDisplay;
   unsigned short yres = mode->VDisplay;
   unsigned char  index;
   BOOLEAN	  checkxgi730 = FALSE;

   irefresh = XGICalcVRate(mode);
   if(!irefresh) {
      if(xres == 800 || xres == 1024 || xres == 1280) return 0x02;
      else return 0x01;
   }

#ifdef TWDEBUG
   xf86DrvMsg(0, X_INFO, "Debug: CalcVRate returned %d\n", irefresh);
#endif

   /* We need the REAL refresh rate here */
   if(mode->Flags & V_INTERLACE)
       	irefresh /= 2;

   /* Do not multiply by 2 when DBLSCAN! */

#ifdef TWDEBUG
   xf86DrvMsg(0, X_INFO, "Debug: Rate after correction = %d\n", irefresh);
#endif

   index = 0;
   while((xgix_vrate[i].idx != 0) && (xgix_vrate[i].xres <= xres)) {
	if((xgix_vrate[i].xres == xres) && (xgix_vrate[i].yres == yres)) {
	    if((checkxgi730 == FALSE) || (xgix_vrate[i].XGI730valid32bpp == TRUE)) {
	       if(xgix_vrate[i].refresh == irefresh) {
		   index = xgix_vrate[i].idx;
		   break;
	       } else if(xgix_vrate[i].refresh > irefresh) {
		   if((xgix_vrate[i].refresh - irefresh) <= 3) {
		      index = xgix_vrate[i].idx;
		   } else if( ((checkxgi730 == FALSE) || (xgix_vrate[i - 1].XGI730valid32bpp == TRUE)) &&
		              ((irefresh - xgix_vrate[i - 1].refresh) <=  2) &&
			      (xgix_vrate[i].idx != 1) ) {
		      index = xgix_vrate[i - 1].idx;
		   }
		   break;
	       } else if((irefresh - xgix_vrate[i].refresh) <= 2) {
	           index = xgix_vrate[i].idx;
		   break;
	       }
	    }
	}
	i++;
   }
   if(index > 0)
	return index;
   else {
        /* Default Rate index */
        if(xres == 800 || xres == 1024 || xres == 1280) return 0x02;
   	else return 0x01;
   }
}

void
XGIWaitRetraceCRT1(ScrnInfoPtr pScrn)
{
   XGIPtr        pXGI = XGIPTR(pScrn);
   int           watchdog;
   unsigned char temp;

   inXGIIDXREG(XGICR,0x17,temp);
   if(!(temp & 0x80)) return;

   inXGIIDXREG(XGISR,0x1f,temp);
   if(temp & 0xc0) return;

   watchdog = 65536;
   while((inXGIREG(XGIINPSTAT) & 0x08) && --watchdog);
   watchdog = 65536;
   while((!(inXGIREG(XGIINPSTAT) & 0x08)) && --watchdog);
}

static void
XGIWaitVBRetrace(ScrnInfoPtr pScrn)
{
/*   XGIPtr  pXGI = XGIPTR(pScrn);  */

      XGIWaitRetraceCRT1(pScrn);
}

#define MODEID_OFF 0x449

unsigned char
XGI_GetSetModeID(ScrnInfoPtr pScrn, unsigned char id)
{
    return(XGI_GetSetBIOSScratch(pScrn, MODEID_OFF, id));
}

unsigned char
XGI_GetSetBIOSScratch(ScrnInfoPtr pScrn, USHORT offset, unsigned char value)
{
    unsigned char ret = 0;
#if defined(i386) || defined(__i386) || defined(__i386__) || defined(__amd64__) || defined(__x86_64__)
    unsigned char *base;

    base = xf86MapVidMem(pScrn->scrnIndex, VIDMEM_MMIO, 0, 0x2000);
    if(!base) {
       XGIErrorLog(pScrn, "(Could not map BIOS scratch area)\n");
       return 0;
    }

    ret = *(base + offset);

    /* value != 0xff means: set register */
    if(value != 0xff)
       *(base + offset) = value;

    xf86UnMapVidMem(pScrn->scrnIndex, base, 0x2000);
#endif
    return ret;
}

void
xgiSaveUnlockExtRegisterLock(XGIPtr pXGI, unsigned char *reg1, unsigned char *reg2)
{
    register unsigned char val;
    unsigned long mylockcalls;

    pXGI->lockcalls++;
    mylockcalls = pXGI->lockcalls;

    /* check if already unlocked */
    inXGIIDXREG(XGISR, 0x05, val);
    if(val != 0xa1) {
       /* save State */
       if(reg1) *reg1 = val;
       /* unlock */
/*
       outb (0x3c4, 0x20);
       val4 = inb (0x3c5);
       val4 |= 0x20;
       outb (0x3c5, val4);
*/
       outXGIIDXREG(XGISR, 0x05, 0x86);
       inXGIIDXREG(XGISR, 0x05, val);
       if(val != 0xA1) {
#ifdef TWDEBUG
	  unsigned char val1, val2;
	  int i;
#endif
          XGIErrorLog(pXGI->pScrn,
               "Failed to unlock sr registers (%p, %lx, 0x%02x; %ld)\n",
	       (void *)pXGI, (unsigned long)pXGI->RelIO, val, mylockcalls);
#ifdef TWDEBUG
          for(i = 0; i <= 0x3f; i++) {
	  	inXGIIDXREG(XGISR, i, val1);
		inXGIIDXREG(0x3c4, i, val2);
		xf86DrvMsg(pXGI->pScrn->scrnIndex, X_INFO,
			"SR%02d: RelIO=0x%02x 0x3c4=0x%02x (%d)\n",
			i, val1, val2, mylockcalls);
	  }
#endif
          if((pXGI->VGAEngine == XGI_OLD_VGA) || (pXGI->VGAEngine == XGI_530_VGA)) {
	     /* Emergency measure: unlock at 0x3c4, and try to enable Relocated IO ports */
	     outXGIIDXREG(0x3c4,0x05,0x86);
	     andXGIIDXREG(0x3c4,0x33,~0x20);
	     outXGIIDXREG(XGISR, 0x05, 0x86);
          }
       }
    }
/*
    if((pXGI->VGAEngine == XGI_OLD_VGA) || (pXGI->VGAEngine == XGI_530_VGA)) {
       inXGIIDXREG(XGICR, 0x80, val);
       if(val != 0xa1) {
          / * save State * /
          if(reg2) *reg2 = val;
          outXGIIDXREG(XGICR, 0x80, 0x86);
	  inXGIIDXREG(XGICR, 0x80, val);
	  if(val != 0xA1) {
	     XGIErrorLog(pXGI->pScrn,
	        "Failed to unlock cr registers (%p, %lx, 0x%02x)\n",
	       (void *)pXGI, (unsigned long)pXGI->RelIO, val);
	  }
       }
    }
*/
}

void
xgiRestoreExtRegisterLock(XGIPtr pXGI, unsigned char reg1, unsigned char reg2)
{
    /* restore lock */
#ifndef UNLOCK_ALWAYS
    outXGIIDXREG(XGISR, 0x05, reg1 == 0xA1 ? 0x86 : 0x00);
    if((pXGI->VGAEngine == XGI_OLD_VGA) || (pXGI->VGAEngine == XGI_530_VGA)) {
       outXGIIDXREG(XGICR, 0x80, reg2 == 0xA1 ? 0x86 : 0x00);
    }
#endif
}

void
XGIDelayUS(int us)
{
    int i , j ;

    for( i = 0 ; i < us ; i++ )
    {
        for( j = 0 ; j < 100 ; j++ )
        {
            inb(VGA_SEQ_INDEX) ;
        }
    }
}

#ifdef DEBUG
void
XGIDumpRegs(ScrnInfoPtr pScrn)
{

    XGIPtr pXGI = XGIPTR(pScrn);

    int i ;
    unsigned long temp ;


    ErrorF("----------------------------------------------------------------------\n") ;
    ErrorF("SR xx\n") ;
    ErrorF("----------------------------------------------------------------------\n") ;
    for( i = 0 ; i < 0x40 ; i++ ){
        inXGIIDXREG(XGISR, i, temp ) ;
        ErrorF("SR%02X = 0x%02lX   ", i, temp ) ;
        if( ((i+1) % 4) == 0 )
        {
            ErrorF("\n") ;
        }
        if( ((i+1) % 16) == 0 )
        {
            ErrorF("\n") ;
        }
    }
    ErrorF( "\n" ) ;
    ErrorF("----------------------------------------------------------------------\n") ;
    ErrorF("CR xx\n") ;
    ErrorF("----------------------------------------------------------------------\n") ;
    for( i = 0 ; i < 0x80 ; i++ ){
        inXGIIDXREG(XGICR, i, temp ) ;
        ErrorF("CR%02X = 0x%02lX   ", i, temp ) ;
        if( ((i+1) % 4) == 0 )
        {
            ErrorF("\n") ;
        }
        if( ((i+1) % 16) == 0 )
        {
            ErrorF("\n") ;
        }
    }
    ErrorF( "\n" ) ;
    ErrorF("----------------------------------------------------------------------\n") ;
    ErrorF("GR xx\n") ;
    ErrorF("----------------------------------------------------------------------\n") ;
    for( i = 0 ; i < 0x9 ; i++ ){
        inXGIIDXREG(XGIGR, i, temp ) ;
        ErrorF("GR%02X = 0x%02lX   ", i, temp ) ;
        if( ((i+1) % 4) == 0 )
        {
            ErrorF("\n") ;
        }
        if( ((i+1) % 16) == 0 )
        {
            ErrorF("\n") ;
        }
    }
    ErrorF( "\n" ) ;

    ErrorF("----------------------------------------------------------------------\n") ;
    ErrorF("PART1 xx\n") ;
    ErrorF("----------------------------------------------------------------------\n") ;
    for( i = 0 ; i < 0x100 ; i++ ){
        inXGIIDXREG(XGIPART1, i, temp ) ;
        ErrorF("PART1_%02X = 0x%02lX   ", i, temp ) ;
        if( ((i+1) % 4) == 0 )
        {
            ErrorF("\n") ;
        }
        if( ((i+1) % 16) == 0 )
        {
            ErrorF("\n") ;
        }
    }
    ErrorF( "\n" ) ;

    ErrorF("----------------------------------------------------------------------\n") ;
    ErrorF("PART2 xx\n") ;
    ErrorF("----------------------------------------------------------------------\n") ;
    for( i = 0 ; i < 0x100 ; i++ ){
        inXGIIDXREG(XGIPART2, i, temp ) ;
        ErrorF("PART2_%02X = 0x%02lX   ", i, temp ) ;
        if( ((i+1) % 4) == 0 )
        {
            ErrorF("\n") ;
        }
        if( ((i+1) % 16) == 0 )
        {
            ErrorF("\n") ;
        }
    }
    ErrorF( "\n" ) ;

    ErrorF("----------------------------------------------------------------------\n") ;
    ErrorF("PART3 xx\n") ;
    ErrorF("----------------------------------------------------------------------\n") ;
    for( i = 0 ; i < 0x100 ; i++ ){
        inXGIIDXREG(XGIPART3, i, temp ) ;
        ErrorF("PART3_%02X = 0x%02lX   ", i, temp ) ;
        if( ((i+1) % 4) == 0 )
        {
            ErrorF("\n") ;
        }
        if( ((i+1) % 16) == 0 )
        {
            ErrorF("\n") ;
        }
    }
    ErrorF( "\n" ) ;

    ErrorF("----------------------------------------------------------------------\n") ;
    ErrorF("PART4 xx\n") ;
    ErrorF("----------------------------------------------------------------------\n") ;
    for( i = 0 ; i < 0x100 ; i++ ){
        inXGIIDXREG(XGIPART4, i, temp ) ;
        ErrorF("PART4_%02X = 0x%02lX   ", i, temp ) ;
        if( ((i+1) % 4) == 0 )
        {
            ErrorF("\n") ;
        }
        if( ((i+1) % 16) == 0 )
        {
            ErrorF("\n") ;
        }
    }
    ErrorF( "\n" ) ;

}
#endif
