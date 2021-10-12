/* $XFree86: xc/programs/Xserver/hw/xfree86/drivers/xgi/xgi_dri.c,v 1.2 2005/08/28 17:48:03 tsi Exp $ */

/* modified from tdfx_dri.c, mga_dri.c */

#include "xf86.h"
#include "xf86_OSproc.h"
#include "xf86_ansic.h"
#include "xf86Priv.h"

#include "xf86PciInfo.h"
#include "xf86Pci.h"
#include "fb.h"

#include "miline.h"

#include "GL/glxtokens.h"

#include "xgi.h"
#include "xgi_dri.h"


#include "xgi_accel.h"
#include "xgi_common.h"
/*
#include "xgi_drm.h"
#include "drm.h"
*/

extern void GlxSetVisualConfigs(
    int nconfigs,
    __GLXvisualConfig *configs,
    void **configprivs
);


#define XGIIdle Volari_Idle

#define AGP_PAGE_SIZE 4096     /* Texture memory 8M  */
/*#define AGP_PAGE_SIZE 5120*/ /* Texture memory 10M */
#define AGP_PAGES 2048
#define AGP_SIZE (AGP_PAGE_SIZE * AGP_PAGES)
#define AGP_VTXBUF_PAGES 512
#define AGP_VTXBUF_SIZE (AGP_PAGE_SIZE * AGP_VTXBUF_PAGES)

#ifdef linux
static char XGIKernelDriverName[] = "xgi";
static char XGIClientDriverName[] = "xgi";

static Bool XGICreateContext(ScreenPtr pScreen, VisualPtr visual,
			      drm_context_t hwContext, void *pVisualConfigPriv,
			      DRIContextType contextStore);
static void XGIDestroyContext(ScreenPtr pScreen, drm_context_t hwContext,
			       DRIContextType contextStore);
static void XGIDRISwapContext(ScreenPtr pScreen, DRISyncType syncType,
			       DRIContextType readContextType,
			       void *readContextStore,
			       DRIContextType writeContextType,
			       void *writeContextStore);
static void XGIDRIInitBuffers(WindowPtr pWin, RegionPtr prgn, CARD32 index);
static void XGIDRIMoveBuffers(WindowPtr pParent, DDXPointRec ptOldOrg,
			       RegionPtr prgnSrc, CARD32 index);

void xgiLostContext(ScreenPtr pScreen);

static Bool
XGIInitVisualConfigs(ScreenPtr pScreen)
{
  ScrnInfoPtr pScrn = xf86Screens[pScreen->myNum];
  XGIPtr pXGI = XGIPTR(pScrn);
  int numConfigs = 0;
  __GLXvisualConfig *pConfigs = 0;
  XGIConfigPrivPtr pXGIConfigs = 0;
  XGIConfigPrivPtr *pXGIConfigPtrs = 0;
  int i, db, z_stencil, accum;
  Bool useZ16 = FALSE;

  if(getenv("XGI_FORCE_Z16")){
    useZ16 = TRUE;
  }

  switch (pScrn->bitsPerPixel) {
  case 8:
  case 24:
    break;
  case 16:
  case 32:
    numConfigs = (useZ16)?8:16;

    if (!(pConfigs = (__GLXvisualConfig*)xnfcalloc(sizeof(__GLXvisualConfig),
						   numConfigs))) {
      return FALSE;
    }
    if (!(pXGIConfigs = (XGIConfigPrivPtr)xnfcalloc(sizeof(XGIConfigPrivRec),
						    numConfigs))) {
      xfree(pConfigs);
      return FALSE;
    }
    if (!(pXGIConfigPtrs = (XGIConfigPrivPtr*)xnfcalloc(sizeof(XGIConfigPrivPtr),
							  numConfigs))) {
      xfree(pConfigs);
      xfree(pXGIConfigs);
      return FALSE;
    }
    for (i=0; i<numConfigs; i++)
      pXGIConfigPtrs[i] = &pXGIConfigs[i];

    i = 0;
    for (accum = 0; accum <= 1; accum++) {
      for (z_stencil=0; z_stencil<(useZ16?2:4); z_stencil++) {
        for (db = 0; db <= 1; db++) {
          pConfigs[i].vid = -1;
          pConfigs[i].class = -1;
          pConfigs[i].rgba = TRUE;
          pConfigs[i].redSize = -1;
          pConfigs[i].greenSize = -1;
          pConfigs[i].blueSize = -1;
          pConfigs[i].redMask = -1;
          pConfigs[i].greenMask = -1;
          pConfigs[i].blueMask = -1;
          pConfigs[i].alphaMask = 0;
          if (accum) {
            pConfigs[i].accumRedSize = 16;
            pConfigs[i].accumGreenSize = 16;
            pConfigs[i].accumBlueSize = 16;
            pConfigs[i].accumAlphaSize = 16;
          } else {
            pConfigs[i].accumRedSize = 0;
            pConfigs[i].accumGreenSize = 0;
            pConfigs[i].accumBlueSize = 0;
            pConfigs[i].accumAlphaSize = 0;
          }
          if (db)
            pConfigs[i].doubleBuffer = TRUE;
          else
            pConfigs[i].doubleBuffer = FALSE;
          pConfigs[i].stereo = FALSE;
          pConfigs[i].bufferSize = -1;
          switch (z_stencil){
            case 0:
              pConfigs[i].depthSize = 0;
              pConfigs[i].stencilSize = 0;
              break;
            case 1:
              pConfigs[i].depthSize = 16;
              pConfigs[i].stencilSize = 0;
              break;
            case 2:
              pConfigs[i].depthSize = 32;
              pConfigs[i].stencilSize = 0;
              break;
            case 3:
              pConfigs[i].depthSize = 24;
              pConfigs[i].stencilSize = 8;
              break;
          }
          pConfigs[i].auxBuffers = 0;
          pConfigs[i].level = 0;
          pConfigs[i].visualRating = GLX_NONE_EXT;
          pConfigs[i].transparentPixel = 0;
          pConfigs[i].transparentRed = 0;
          pConfigs[i].transparentGreen = 0;
          pConfigs[i].transparentBlue = 0;
          pConfigs[i].transparentAlpha = 0;
          pConfigs[i].transparentIndex = 0;
          i++;
        }
      }
    }
    if (i != numConfigs) {
      xf86DrvMsg(pScrn->scrnIndex, X_ERROR,
                 "[drm] Incorrect initialization of visuals\n");
      return FALSE;
    }
    break;
  }

  pXGI->numVisualConfigs = numConfigs;
  pXGI->pVisualConfigs = pConfigs;
  pXGI->pVisualConfigsPriv = pXGIConfigs;
  GlxSetVisualConfigs(numConfigs, pConfigs, (void**)pXGIConfigPtrs);

  return TRUE;
}
#endif

Bool XGIDRIScreenInit(ScreenPtr pScreen)
{
#ifndef linux
  return FALSE;
#else /* linux */

  ScrnInfoPtr pScrn = xf86Screens[pScreen->myNum];
  XGIPtr pXGI = XGIPTR(pScrn);
  DRIInfoPtr pDRIInfo;
  XGIDRIPtr pXGIDRI;

   /* Check that the GLX, DRI, and DRM modules have been loaded by testing
    * for canonical symbols in each module. */
   if (!xf86LoaderCheckSymbol("GlxSetVisualConfigs")) return FALSE;
   if (!xf86LoaderCheckSymbol("DRIScreenInit"))       return FALSE;
   if (!xf86LoaderCheckSymbol("drmAvailable"))        return FALSE;
   if (!xf86LoaderCheckSymbol("DRIQueryVersion")) {
      xf86DrvMsg(pScreen->myNum, X_ERROR,
                 "XGIDRIScreenInit failed (libdri.a too old)\n");
      return FALSE;
   }

   /* Check the DRI version */
   /* { */
   /* int major, minor, patch; */
   /* DRIQueryVersion(&major, &minor, &patch); */
   /* if (major != 3 || minor != 0 || patch < 0) { */
   /* xf86DrvMsg(pScreen->myNum, X_ERROR, */
   /* "[drm] XGIDRIScreenInit failed (DRI version = %d.%d.%d, expected 3.0.x). Disabling DRI.\n", */
   /* major, minor, patch); */
   /* return FALSE; */
   /* } */
   /* } */

  pDRIInfo = DRICreateInfoRec();
  if (!pDRIInfo) return FALSE;
  pXGI->pDRIInfo = pDRIInfo;

  pDRIInfo->drmDriverName = XGIKernelDriverName;
  pDRIInfo->clientDriverName = XGIClientDriverName;
  pDRIInfo->busIdString = xalloc(64);
  sprintf(pDRIInfo->busIdString, "PCI:%d:%d:%d",
	  ((pciConfigPtr)pXGI->PciInfo->thisCard)->busnum,
	  ((pciConfigPtr)pXGI->PciInfo->thisCard)->devnum,
	  ((pciConfigPtr)pXGI->PciInfo->thisCard)->funcnum);
  pDRIInfo->ddxDriverMajorVersion = 0;
  pDRIInfo->ddxDriverMinorVersion = 1;
  pDRIInfo->ddxDriverPatchVersion = 0;
  pDRIInfo->frameBufferPhysicalAddress = pXGI->FbAddress;
  pDRIInfo->frameBufferSize = pXGI->FbMapSize;

  /* ?? */
  pDRIInfo->frameBufferStride = pXGI->scrnOffset;
  pDRIInfo->ddxDrawableTableEntry = XGI_MAX_DRAWABLES;

  if (SAREA_MAX_DRAWABLES < XGI_MAX_DRAWABLES)
    pDRIInfo->maxDrawableTableEntry = SAREA_MAX_DRAWABLES;
  else
    pDRIInfo->maxDrawableTableEntry = XGI_MAX_DRAWABLES;

#ifdef NOT_DONE
  /* FIXME need to extend DRI protocol to pass this size back to client
   * for SAREA mapping that includes a device private record
   */
  pDRIInfo->SAREASize =
    ((sizeof(XF86DRISAREARec) + 0xfff) & 0x1000); /* round to page */
  /* + shared memory device private rec */
#else
  /* For now the mapping works by using a fixed size defined
   * in the SAREA header
   */
  if (sizeof(XF86DRISAREARec)+sizeof(XGISAREAPriv)>SAREA_MAX) {
/* ErrorF("Data does not fit in SAREA\n"); */
    return FALSE;
  }
  pDRIInfo->SAREASize = SAREA_MAX;
#endif

  if (!(pXGIDRI = (XGIDRIPtr)xnfcalloc(sizeof(XGIDRIRec),1))) {
    DRIDestroyInfoRec(pXGI->pDRIInfo);
    pXGI->pDRIInfo=0;
    return FALSE;
  }
  pDRIInfo->devPrivate = pXGIDRI;
  pDRIInfo->devPrivateSize = sizeof(XGIDRIRec);
  pDRIInfo->contextSize = sizeof(XGIDRIContextRec);

  pDRIInfo->CreateContext = XGICreateContext;
  pDRIInfo->DestroyContext = XGIDestroyContext;
  pDRIInfo->SwapContext = XGIDRISwapContext;
  pDRIInfo->InitBuffers = XGIDRIInitBuffers;
  pDRIInfo->MoveBuffers = XGIDRIMoveBuffers;
  pDRIInfo->bufferRequests = DRI_ALL_WINDOWS;

  if (!DRIScreenInit(pScreen, pDRIInfo, &pXGI->drmSubFD)) {
    xfree(pDRIInfo->devPrivate);
    pDRIInfo->devPrivate=0;
    DRIDestroyInfoRec(pXGI->pDRIInfo);
    pXGI->pDRIInfo=0;
    pXGI->drmSubFD = -1;
    return FALSE;
  }

  pXGIDRI->regs.size = XGIIOMAPSIZE;
  pXGIDRI->regs.map = 0;
  if (drmAddMap(pXGI->drmSubFD, (drm_handle_t)pXGI->IOAddress,
		pXGIDRI->regs.size, DRM_REGISTERS, 0,
		&pXGIDRI->regs.handle)<0)
  {
    XGIDRICloseScreen(pScreen);
    return FALSE;
  }

  xf86DrvMsg(pScreen->myNum, X_INFO, "[drm] Registers = 0x%08lx\n",
	       pXGIDRI->regs.handle);

  /* AGP */
  do{
    pXGI->agpSize = 0;
    pXGI->agpVtxBufSize = 0;
    pXGIDRI->AGPVtxBufSize = 0;

    if (drmAgpAcquire(pXGI->drmSubFD) < 0) {
      xf86DrvMsg(pScreen->myNum, X_ERROR, "[drm] drmAgpAcquire failed\n");
      break;
    }

    /* TODO: default value is 2x? */
/* if (drmAgpEnable(pXGI->drmSubFD, drmAgpGetMode(pXGI->drmSubFD)&~0x0) < 0) {
*/
    /* Default to 1X agp mode */
    if (drmAgpEnable(pXGI->drmSubFD, drmAgpGetMode(pXGI->drmSubFD)&~0x00000002) < 0) {
      xf86DrvMsg(pScreen->myNum, X_ERROR, "[drm] drmAgpEnable failed\n");
      break;
    }
/* ErrorF("[drm] drmAgpEnabled succeeded\n"); */

    if (drmAgpAlloc(pXGI->drmSubFD, AGP_SIZE, 0, NULL, &pXGI->agpHandle) < 0) {
      xf86DrvMsg(pScreen->myNum, X_ERROR,
                 "[drm] drmAgpAlloc failed\n");
      drmAgpRelease(pXGI->drmSubFD);
      break;
    }

    /* Bind agp-gart table                       */
    /* fill the phys addr. into gart table       */
    /*********************************************/
    if (drmAgpBind(pXGI->drmSubFD, pXGI->agpHandle, 0) < 0) {
      xf86DrvMsg(pScreen->myNum, X_ERROR,
                 "[drm] drmAgpBind failed\n");
      drmAgpFree(pXGI->drmSubFD, pXGI->agpHandle);
      drmAgpRelease(pXGI->drmSubFD);

      break;
    }

    pXGI->agpSize = AGP_SIZE;
    pXGI->agpAddr = drmAgpBase(pXGI->drmSubFD);
    /* pXGI->agpBase = */ /* Xserver connot access VtxBuf, bc. not mem-map */

    /* any client can access this VtxBuf AGP area */
    /* by mem-map pXGIDRI->agp.handle             */
    /**********************************************/
    pXGIDRI->agp.size = pXGI->agpSize;
    if (drmAddMap(pXGI->drmSubFD, (drm_handle_t)0,
                 pXGIDRI->agp.size, DRM_AGP, 0,
                 &pXGIDRI->agp.handle) < 0) {
      xf86DrvMsg(pScreen->myNum, X_ERROR,
                 "[drm] Failed to map public agp area\n");
      pXGIDRI->agp.size = 0;
      break;
    }

    pXGI->agpVtxBufSize = AGP_VTXBUF_SIZE; /* 2MB */
    pXGI->agpVtxBufAddr = pXGI->agpAddr;
    pXGI->agpVtxBufBase = pXGI->agpVtxBufAddr - pXGI->agpAddr +
                          pXGI->agpBase;
    pXGI->agpVtxBufFree = 0;

    pXGIDRI->AGPVtxBufOffset = pXGI->agpVtxBufAddr - pXGI->agpAddr;
    pXGIDRI->AGPVtxBufSize = pXGI->agpVtxBufSize;

    /* this AGP area is used for texture */
    /*               is managed by drm   */
    /*************************************/
    {
      drm_xgi_agp_t agp;

      agp.offset = AGP_VTXBUF_SIZE;
      agp.size = AGP_SIZE - AGP_VTXBUF_SIZE;
#ifdef DRM_IOCTL_XGI_AGP_INIT
      xf86ioctl(pXGI->drmSubFD, DRM_IOCTL_XGI_AGP_INIT, &agp);
/*
#else
      xf86ioctl(pXGI->drmSubFD, XGI_IOCTL_AGP_INIT, &agp);
*/
#endif
    }
  }
  while(0);

  /* enable IRQ */
  pXGI->irq = drmGetInterruptFromBusID(pXGI->drmSubFD,
	       ((pciConfigPtr)pXGI->PciInfo->thisCard)->busnum,
	       ((pciConfigPtr)pXGI->PciInfo->thisCard)->devnum,
	       ((pciConfigPtr)pXGI->PciInfo->thisCard)->funcnum);

  if((drmCtlInstHandler(pXGI->drmSubFD, pXGI->irq)) != 0)
    {
      xf86DrvMsg(pScrn->scrnIndex, X_INFO,
		 "[drm] failure adding irq %d handler, stereo disabled\n",
		 pXGI->irq);
      pXGI->irqEnabled = FALSE;
    }
  else
    {
      pXGI->irqEnabled = TRUE;
    }

  pXGIDRI->irqEnabled = pXGI->irqEnabled;

  if (!(XGIInitVisualConfigs(pScreen))) {
    XGIDRICloseScreen(pScreen);
    return FALSE;
  }
  xf86DrvMsg(pScrn->scrnIndex, X_INFO, "visual configs initialized\n" );

  return TRUE;
#endif /* linux */
}

void
XGIDRICloseScreen(ScreenPtr pScreen)
{
  ScrnInfoPtr pScrn = xf86Screens[pScreen->myNum];
  XGIPtr pXGI = XGIPTR(pScrn);

  DRICloseScreen(pScreen);

  if (pXGI->pDRIInfo) {
    if (pXGI->pDRIInfo->devPrivate) {
      xfree(pXGI->pDRIInfo->devPrivate);
      pXGI->pDRIInfo->devPrivate=0;
    }
    DRIDestroyInfoRec(pXGI->pDRIInfo);
    pXGI->pDRIInfo=0;
  }
  if (pXGI->pVisualConfigs) xfree(pXGI->pVisualConfigs);
  if (pXGI->pVisualConfigsPriv) xfree(pXGI->pVisualConfigsPriv);

  if(pXGI->agpSize){
/* ErrorF("Freeing agp memory\n"); */
     drmAgpFree(pXGI->drmSubFD, pXGI->agpHandle);
/* ErrorF("releasing agp module\n"); */
     drmAgpRelease(pXGI->drmSubFD);
  }
}

#ifdef linux
/* TODO: xserver receives driver's swapping event and do something
 *       according the data initialized in this function
 */
static Bool
XGICreateContext(ScreenPtr pScreen, VisualPtr visual,
		  drm_context_t hwContext, void *pVisualConfigPriv,
		  DRIContextType contextStore)
{
  return TRUE;
}

static void
XGIDestroyContext(ScreenPtr pScreen, drm_context_t hwContext,
		   DRIContextType contextStore)
{
}
#endif

Bool
XGIDRIFinishScreenInit(ScreenPtr pScreen)
{
  ScrnInfoPtr pScrn = xf86Screens[pScreen->myNum];
  XGIPtr pXGI = XGIPTR(pScrn);
/*  XGIPtr pXGI = XGIPTR(pScrn); */
  XGIDRIPtr pXGIDRI;

  /*pXGI->pDRIInfo->driverSwapMethod = DRI_HIDE_X_CONTEXT;*/
  pXGI->pDRIInfo->driverSwapMethod = DRI_SERVER_SWAP;

  pXGIDRI=(XGIDRIPtr)pXGI->pDRIInfo->devPrivate;
  pXGIDRI->deviceID=pXGI->Chipset;
  pXGIDRI->revisionID=pXGI->ChipRev; 
  pXGIDRI->width=pScrn->virtualX;
  pXGIDRI->height=pScrn->virtualY;
  pXGIDRI->mem=pScrn->videoRam*1024;
  pXGIDRI->bytesPerPixel= (pScrn->bitsPerPixel+7) / 8;
  /* TODO */
  pXGIDRI->scrnX=pXGIDRI->width;
  pXGIDRI->scrnY=pXGIDRI->height;

/*
  pXGIDRI->textureOffset=pXGI->texOffset;
  pXGIDRI->textureSize=pXGI->texSize;
  pXGIDRI->fbOffset=pXGI->fbOffset;
  pXGIDRI->backOffset=pXGI->backOffset;
  pXGIDRI->depthOffset=pXGI->depthOffset;
*/

  /* set SAREA value */
  {
    XGISAREAPriv *saPriv;

    saPriv=(XGISAREAPriv*)DRIGetSAREAPrivate(pScreen);
    assert(saPriv);

    saPriv->CtxOwner = -1;
    saPriv->QueueLength = 0;
    pXGI->cmdQueueLenPtr = &(saPriv->QueueLength);
    saPriv->AGPVtxBufNext = 0;

  
    saPriv->shareWPoffset = pXGI->cmdQueue_shareWP_only2D;
    pXGI->pCQ_shareWritePort = &(saPriv->shareWPoffset);



    XGIIdle
  }

  return DRIFinishScreenInit(pScreen);
}

#ifdef linux
static void
XGIDRISwapContext(ScreenPtr pScreen, DRISyncType syncType,
		   DRIContextType oldContextType, void *oldContext,
		   DRIContextType newContextType, void *newContext)
{
  ScrnInfoPtr pScrn = xf86Screens[pScreen->myNum];
  XGIPtr pXGI = XGIPTR(pScrn);

  /* mEndPrimitive */
  /*
   * TODO: do this only if X-Server get lock. If kernel supports delayed
   * signal, needless to do this
   */
  /* 
  *(pXGI->IOBase + 0X8B50) = 0xff;
  *(unsigned int *)(pXGI->IOBase + 0x8B60) = -1;
  */

  XGIIdle
}

static void
XGIDRIInitBuffers(WindowPtr pWin, RegionPtr prgn, CARD32 index)
{
  ScreenPtr pScreen = pWin->drawable.pScreen;
  ScrnInfoPtr pScrn = xf86Screens[pScreen->myNum];
  XGIPtr pXGI = XGIPTR(pScrn);

  XGIIdle
}

static void
XGIDRIMoveBuffers(WindowPtr pParent, DDXPointRec ptOldOrg,
		   RegionPtr prgnSrc, CARD32 index)
{
  ScreenPtr pScreen = pParent->drawable.pScreen;
  ScrnInfoPtr pScrn = xf86Screens[pScreen->myNum];
  XGIPtr pXGI = XGIPTR(pScrn);

  XGIIdle
}

#endif
