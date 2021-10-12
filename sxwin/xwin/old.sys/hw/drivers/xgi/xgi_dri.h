/* $XFree86: xc/programs/Xserver/hw/xfree86/drivers/xgi/xgi_dri.h,v 1.1 2005/05/02 13:28:02 dawes Exp $ */

/* modified from tdfx_dri.h */

#ifndef _XGI_DRI_
#define _XGI_DRI_

#include <xf86drm.h>

#define XGI_MAX_DRAWABLES 256
#define XGIIOMAPSIZE (64*1024)

typedef struct {
  int CtxOwner;
  int QueueLength;
  unsigned long AGPVtxBufNext;
  unsigned int FrameCount;
  
  unsigned long shareWPoffset;
  /*CARD16*/
  /*unsigned short RelIO;*/

  /* 2001/12/16 added by jjtseng for some bala reasons .... */
  unsigned char *AGPCmdBufBase;
  unsigned long AGPCmdBufAddr;
  unsigned long AGPCmdBufOffset;
  unsigned int  AGPCmdBufSize;
  unsigned long AGPCmdBufNext;
  /*~ 2001/12/16 added by jjtseng for some bala reasons .... */

} XGISAREAPriv;

#define XGI_FRONT 0
#define XGI_BACK 1
#define XGI_DEPTH 2

typedef struct {
  drm_handle_t handle;
  drmSize size;
  drmAddress map;
} xgiRegion, *xgiRegionPtr;

typedef struct {
  xgiRegion regs, agp;
  int deviceID;
  int revisionID;
  int width;
  int height;
  int mem;
  int bytesPerPixel;
  int priv1;
  int priv2;
  int fbOffset;
  int backOffset;
  int depthOffset;
  int textureOffset;
  int textureSize;
  unsigned int AGPVtxBufOffset;
  unsigned int AGPVtxBufSize;
  /* 2001/12/16 added by jjtseng for some bala reasons .... */
  unsigned char *AGPCmdBufBase;
  unsigned long AGPCmdBufAddr;
  unsigned long AGPCmdBufOffset;
  unsigned int AGPCmdBufSize;
  unsigned long *pAGPCmdBufNext;
  /*~ 2001/12/16 added by jjtseng for some bala reasons .... */
  int irqEnabled;
  unsigned int scrnX, scrnY;
} XGIDRIRec, *XGIDRIPtr;

typedef struct {
  /* Nothing here yet */
  int dummy;
} XGIConfigPrivRec, *XGIConfigPrivPtr;

typedef struct {
  /* Nothing here yet */
  int dummy;
} XGIDRIContextRec, *XGIDRIContextPtr;

Bool XGIDRIScreenInit(ScreenPtr pScreen);
void XGIDRICloseScreen(ScreenPtr pScreen);
Bool XGIDRIFinishScreenInit(ScreenPtr pScreen);

#endif
