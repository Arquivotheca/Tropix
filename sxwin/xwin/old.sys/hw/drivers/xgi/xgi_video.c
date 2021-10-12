/* $XFree86: xc/programs/Xserver/hw/xfree86/drivers/xgi/xgi_video.c,v 1.4 2005/10/14 15:16:49 tsi Exp $ */
/***************************************************************************

Copyright 2004 eXtreme Graphics Innovation Corp, Inc., HsinChu, Taiwan.
All Rights Reserved.

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sub license, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice (including the
next paragraph) shall be included in all copies or substantial portions
of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.
IN NO EVENT SHALL INTEL, AND/OR ITS SUPPLIERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
THE USE OR OTHER DEALINGS IN THE SOFTWARE.

**************************************************************************/

/*
 * Formerly based on Xv driver for SiS 300, 315 and 330 series by
 * Thomas Winischhofer <thomas@winischhofer.net>
 *
 * Basic structure based on the mga Xv driver by Mark Vojkovich
 *              and i810 Xv driver by Jonathan Bian <jonathan.bian@intel.com>.
 *
 * Authors:
 *      Nan-Hsing Chang <nan_chang@xgitech.com>
 *
 * This supports the following chipsets:
 * XGI340, XGI342: Full register range, one overlay (used for both CRT1 and CRT2 alt.)
 *
 */

#include "xf86.h"
#include "xf86_OSproc.h"
#include "xf86Resources.h"
#include "xf86_ansic.h"
#include "compiler.h"
#include "xf86PciInfo.h"
#include "xf86Pci.h"
#include "xf86fbman.h"
#include "regionstr.h"

#include "xgi.h"
#include "xf86xv.h"
#include <X11/extensions/Xv.h>
#include "xaa.h"
#include "xaalocal.h"
#include "dixstruct.h"
#include "fourcc.h"

/* TODO: move to xgi_regs.h */
#include "xgi_videohw.h"
#include "xgi_video.h"

static XF86VideoAdaptorPtr XGISetupImageVideo(ScreenPtr);
static int XGISetPortAttribute(ScrnInfoPtr, Atom, INT32, pointer);
static int XGIGetPortAttribute(ScrnInfoPtr, Atom ,INT32 *, pointer);
static void XGIQueryBestSize(ScrnInfoPtr, Bool,
        short, short, short, short, unsigned int *, unsigned int *, pointer);
static int XGIPutImage( ScrnInfoPtr,
        short, short, short, short, short, short, short, short,
        int, unsigned char*, short, short, Bool, RegionPtr, pointer);
static int XGIQueryImageAttributes(ScrnInfoPtr,
        int, unsigned short *, unsigned short *,  int *, int *);
static void XGIStopVideo(ScrnInfoPtr, pointer, Bool);
/* static void XGIFreeOverlayMemory(ScrnInfoPtr pScrn); */

#define MAKE_ATOM(a) MakeAtom(a, sizeof(a) - 1, TRUE)

void XGIInitVideo(ScreenPtr pScreen)
{
    ScrnInfoPtr pScrn = xf86Screens[pScreen->myNum];
    XF86VideoAdaptorPtr *adaptors, *newAdaptors = NULL;
    XF86VideoAdaptorPtr newAdaptor = NULL;
    int num_adaptors;

    newAdaptor = XGISetupImageVideo(pScreen);

    num_adaptors = xf86XVListGenericAdaptors(pScrn, &adaptors);

    if(newAdaptor) {
        if(!num_adaptors) {
            num_adaptors = 1;
            adaptors = &newAdaptor;
        } else {
            newAdaptors =  /* need to free this someplace */
                xalloc((num_adaptors + 1) * sizeof(XF86VideoAdaptorPtr*));
            if(newAdaptors) {
                memcpy(newAdaptors, adaptors, num_adaptors *
                                        sizeof(XF86VideoAdaptorPtr));
                newAdaptors[num_adaptors] = newAdaptor;
                adaptors = newAdaptors;
                num_adaptors++;
            }
        }
    }

    if(num_adaptors)
        xf86XVScreenInit(pScreen, adaptors, num_adaptors);

    if(newAdaptors)
        xfree(newAdaptors);

}


/* client libraries expect an encoding */
static XF86VideoEncodingRec DummyEncoding =
{
   0,
   "XV_IMAGE",
   0, 0,                /* Will be filled in */
   {1, 1}
};

#define NUM_FORMATS 3

static XF86VideoFormatRec XGIFormats[NUM_FORMATS] =
{
   { 8, PseudoColor},
   {16, TrueColor},
   {24, TrueColor}
};

static char xgixvcolorkey[] 				= "XV_COLORKEY";
static char xgixvbrightness[] 				= "XV_BRIGHTNESS";
static char xgixvcontrast[] 				= "XV_CONTRAST";
static char xgixvsaturation[] 				= "XV_SATURATION";
static char xgixvhue[] 				        = "XV_HUE";
static char xgixvgammared[] 				= "XV_GAMMA_RED";
static char xgixvgammagreen[] 				= "XV_GAMMA_GREEN";
static char xgixvgammablue[] 				= "XV_GAMMA_BLUE";

#define NUM_ATTRIBUTES 8

static XF86AttributeRec XGIAttributes[NUM_ATTRIBUTES] =
{
   {XvSettable | XvGettable, 0, (1 << 24) - 1, xgixvcolorkey},
   {XvSettable | XvGettable, -128, 127, xgixvbrightness},
   {XvSettable | XvGettable, 0, 255, xgixvcontrast},
   {XvSettable | XvGettable, -180, 180, xgixvsaturation},
   {XvSettable | XvGettable, -180, 180, xgixvhue},
   {XvSettable | XvGettable, 100, 10000, xgixvgammared},
   {XvSettable | XvGettable, 100, 10000, xgixvgammagreen},
   {XvSettable | XvGettable, 100, 10000, xgixvgammablue},
};

#define NUM_IMAGES 		8

static XF86ImageRec XGIImages[NUM_IMAGES] =
   {
    XVIMAGE_YUY2, /* If order is changed, XGIOffscreenImages must be adapted */
    XVIMAGE_YV12,
    XVIMAGE_UYVY,
    { /* RGB 555 */
      PIXEL_FMT_RGB5,
      XvRGB,
      LSBFirst,
      {'R','V','1','5',
       0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
      16,
      XvPacked,
      1,
      15, 0x7C00, 0x03E0, 0x001F,
      0, 0, 0,
      0, 0, 0,
      0, 0, 0,
      {'R', 'V', 'B',0,
       0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
      XvTopToBottom
    },
    { /* RGB 565 */
      PIXEL_FMT_RGB6,
      XvRGB,
        LSBFirst,
      {'R','V','1','6',
       0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
      16,
      XvPacked,
      1,
      16, 0xF800, 0x07E0, 0x001F,
      0, 0, 0,
      0, 0, 0,
      0, 0, 0,
      {'R', 'V', 'B',0,
       0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
      XvTopToBottom
    },
    {  /* YVYU */
      PIXEL_FMT_YVYU, \
      XvYUV, \
      LSBFirst, \
      {'Y','V','Y','U',
          0x00,0x00,0x00,0x10,0x80,0x00,0x00,0xAA,0x00,0x38,0x9B,0x71},
        16,
        XvPacked,
        1,
        0, 0, 0, 0 ,
        8, 8, 8,
        1, 2, 2,
        1, 1, 1,
      {'Y','V','Y','U',
          0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        XvTopToBottom
   },
   {   /* NV12 */
      PIXEL_FMT_NV12,
        XvYUV,
        LSBFirst,
      {'N','V','1','2',
          0x00,0x00,0x00,0x10,0x80,0x00,0x00,0xAA,0x00,0x38,0x9B,0x71},
        12,
        XvPlanar,
      2,
        0, 0, 0, 0 ,
        8, 8, 8,
        1, 2, 2,
        1, 2, 2,
      {'Y','U','V',0,
       0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        XvTopToBottom
   },
   {   /* NV21 */
      PIXEL_FMT_NV21,
      XvYUV,
      LSBFirst,
      {'N','V','2','1',
       0x00,0x00,0x00,0x10,0x80,0x00,0x00,0xAA,0x00,0x38,0x9B,0x71},
      12,
      XvPlanar,
      2,
      0, 0, 0, 0,
      8, 8, 8,
      1, 2, 2,
      1, 2, 2,
      {'Y','V','U',0,
       0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
      XvTopToBottom
   },
};

static void
set_maxencoding(XGIPtr pXGI, XGIPortPrivPtr pPriv)
{
    DummyEncoding.width  = IMAGE_MAX_WIDTH;
    DummyEncoding.height = IMAGE_MAX_HEIGHT;
}

static void
XGIResetXvGamma(ScrnInfoPtr pScrn)
{
    XGIPtr pXGI = XGIPTR(pScrn);
    XGIPortPrivPtr pPriv = GET_PORT_PRIVATE(pScrn);

    XGIUpdateXvGamma(pXGI, pPriv);
}

static void
XGISetPortDefaults(ScrnInfoPtr pScrn, XGIPortPrivPtr pPriv)
{
    pPriv->colorKey = 0x000101fe;
    pPriv->brightness = 0;
    pPriv->contrast = 128;
    pPriv->saturation = 0;
    pPriv->hue = 0;
}

static XF86VideoAdaptorPtr
XGISetupImageVideo(ScreenPtr pScreen)
{
    ScrnInfoPtr pScrn = xf86Screens[pScreen->myNum];
    XGIPtr pXGI = XGIPTR(pScrn);
    XF86VideoAdaptorPtr adapt;
    XGIPortPrivPtr pPriv;

    if(!(adapt = xcalloc(1, sizeof(XF86VideoAdaptorRec) +
                            sizeof(XGIPortPrivRec) +
                            sizeof(DevUnion))))
        return NULL;

    adapt->type = XvWindowMask | XvInputMask | XvImageMask;
    adapt->flags = VIDEO_OVERLAID_IMAGES | VIDEO_CLIP_TO_VIEWPORT;
    /*adapt->name = "XGI Video Overlay"; */
    adapt->name = "XGI Video";
    adapt->nEncodings = 1;
    adapt->pEncodings = &DummyEncoding;
    adapt->nFormats = NUM_FORMATS;
    adapt->pFormats = XGIFormats;
    adapt->nPorts = 1;
    adapt->pPortPrivates = (DevUnion*)(&adapt[1]);

    pPriv = (XGIPortPrivPtr)(&adapt->pPortPrivates[1]);

    adapt->pPortPrivates[0].ptr = (pointer)(pPriv);
    adapt->pAttributes = XGIAttributes;
    adapt->nAttributes = NUM_ATTRIBUTES;
	adapt->nImages = NUM_IMAGES;
    adapt->pImages = XGIImages;
    adapt->PutVideo = NULL;
    adapt->PutStill = NULL;
    adapt->GetVideo = NULL;
    adapt->GetStill = NULL;
    adapt->StopVideo = XGIStopVideo;
    adapt->SetPortAttribute = XGISetPortAttribute;
    adapt->GetPortAttribute = XGIGetPortAttribute;
    adapt->QueryBestSize = XGIQueryBestSize;
    adapt->PutImage = XGIPutImage;
    adapt->QueryImageAttributes = XGIQueryImageAttributes;

    pPriv->currentBuf = 0;
    pPriv->linear     = NULL;
    pPriv->fbAreaPtr = NULL;
    pPriv->fbSize = 0;
	pPriv->videoStatus = 0;
	pPriv->linebufMergeLimit = 1280;

    /* gotta uninit this someplace */
#if defined(REGION_NULL)
  REGION_NULL(pScreen, &pPriv->clip);
#else
    REGION_INIT(pScreen, &pPriv->clip, NullBox, 0);
#endif

    /* Reset the properties to their defaults */
    XGISetPortDefaults(pScrn, pPriv);

	pXGI->adaptor = adapt;
	pXGI->xvBrightness = MAKE_ATOM(xgixvbrightness);
	pXGI->xvContrast   = MAKE_ATOM(xgixvcontrast);
	pXGI->xvColorKey   = MAKE_ATOM(xgixvcolorkey);
	pXGI->xvSaturation = MAKE_ATOM(xgixvsaturation);
	pXGI->xvHue 	   = MAKE_ATOM(xgixvhue);
	pXGI->xvGammaRed   = MAKE_ATOM(xgixvgammared);
    pXGI->xvGammaGreen = MAKE_ATOM(xgixvgammagreen);
    pXGI->xvGammaBlue  = MAKE_ATOM(xgixvgammablue);

	/* set display register */
	if(pXGI->VBFlags) {
	  pPriv->displayMode = DISPMODE_MIRROR;
	  XGI_SetSRRegMask(pXGI, Index_SR_Graphic_Mode, 0x00, 0xc0); /* Only ovelray in CRT1*/
	  XGI_SetSRRegMask(pXGI, Index_SR_Ext_Clock_Sel, 0x00, 0xc0);
	}
	else {
	  pPriv->displayMode = DISPMODE_SINGLE1;
	  XGI_SetSRRegMask(pXGI, Index_SR_Graphic_Mode, 0x00, 0xc0);
	  XGI_SetSRRegMask(pXGI, Index_SR_Ext_Clock_Sel, 0x00, 0xc0);
	}
	
    set_maxencoding(pXGI, pPriv);

    XGIResetVideo(pScrn);
	
    pXGI->ResetXv = XGIResetVideo;
    pXGI->ResetXvGamma = XGIResetXvGamma;

    return adapt;
}

#if XF86_VERSION_CURRENT < XF86_VERSION_NUMERIC(4,3,99,0,0)
static Bool
RegionsEqual(RegionPtr A, RegionPtr B)
{
    int *dataA, *dataB;
    int num;

    num = REGION_NUM_RECTS(A);
    if(num != REGION_NUM_RECTS(B))
        return FALSE;

    if((A->extents.x1 != B->extents.x1) ||
       (A->extents.x2 != B->extents.x2) ||
       (A->extents.y1 != B->extents.y1) ||
       (A->extents.y2 != B->extents.y2))
        return FALSE;

    dataA = (int*)REGION_RECTS(A);
    dataB = (int*)REGION_RECTS(B);

    while(num--) {
        if((dataA[0] != dataB[0]) || (dataA[1] != dataB[1]))
           return FALSE;
        dataA += 2;
        dataB += 2;
    }

    return TRUE;
}
#endif 


static int
XGISetPortAttribute(
  ScrnInfoPtr pScrn,
  Atom attribute,
  INT32 value,
  pointer data
){
  XGIPortPrivPtr pPriv = (XGIPortPrivPtr)data;
  XGIPtr pXGI = XGIPTR(pScrn);

  if (attribute == pXGI->xvBrightness) {
          if((value < -128) || (value > 127))
             return BadValue;

          pPriv->brightness = value;
          XGI_SetVideoBrightnessReg(pXGI, value);
  }
  else if (attribute == pXGI->xvContrast) {
          if ((value < 0) || (value > 255))
             return BadValue;

          pPriv->contrast = value;
          XGI_SetVideoContrastReg(pXGI, value);
  }
  else if (attribute == pXGI->xvSaturation){
          if ((value < -180) || (value > 180))
             return BadValue;

          pPriv->saturation = value;
          XGI_SetVideoSaturationReg(pXGI, value);
  }
  else if (attribute == pXGI->xvHue){
          if ((value < -180) || (value > 180))
             return BadValue;

          pPriv->hue = value;
          XGI_SetVideoHueReg(pXGI, value);
  }
  else if (attribute == pXGI->xvColorKey) {
          pPriv->colorKey = value;
          REGION_EMPTY(pScrn->pScreen, &pPriv->clip);
  } else if(attribute == pXGI->xvGammaRed) {
       if((value < 100) || (value > 10000))
          return BadValue;
	   
       pXGI->XvGammaRed = value;
       XGIUpdateXvGamma(pXGI, pPriv);
  } else if(attribute == pXGI->xvGammaGreen) {
       if((value < 100) || (value > 10000))
          return BadValue;
	   
       pXGI->XvGammaGreen = value;
       XGIUpdateXvGamma(pXGI, pPriv);
  } else if(attribute == pXGI->xvGammaBlue) {
       if((value < 100) || (value > 10000))
          return BadValue;
	   
       pXGI->XvGammaBlue = value;
       XGIUpdateXvGamma(pXGI, pPriv);
  } else
     return BadMatch;

  return Success;
}

static int
XGIGetPortAttribute(
  ScrnInfoPtr pScrn,
  Atom attribute,
  INT32 *value,
  pointer data
){
  XGIPortPrivPtr pPriv = (XGIPortPrivPtr)data;
  XGIPtr pXGI = XGIPTR(pScrn);
  
  if (attribute == pXGI->xvBrightness) {
          *value = pPriv->brightness;
  }
  else if (attribute == pXGI->xvContrast) {
          *value = pPriv->contrast;
  }
  else if (attribute == pXGI->xvSaturation) {
          *value = pPriv->saturation;
  }
  else if (attribute == pXGI->xvHue) {
          *value = pPriv->hue;
		  
  } else if(attribute == pXGI->xvGammaRed) {
          *value = pXGI->XvGammaRed;
		  
  } else if(attribute == pXGI->xvGammaGreen) {
        *value = pXGI->XvGammaGreen;
		
  } else if(attribute == pXGI->xvGammaBlue) {
        *value = pXGI->XvGammaBlue;
		
  }
  else if (attribute == pXGI->xvColorKey) {
        *value = pPriv->colorKey;
		  
  }
  else
     return BadMatch;

  return Success;
}

static void
XGIQueryBestSize(
  ScrnInfoPtr pScrn,
  Bool motion,
  short vid_w, short vid_h,
  short drw_w, short drw_h,
  unsigned int *p_w, unsigned int *p_h,
  pointer data
){
  *p_w = drw_w;
  *p_h = drw_h;

  /* TODO: report the HW limitation */
}


static void
set_scale_factor(XGIOverlayPtr pOverlay)
{
  float  f_temp;
  int   NewPitch, srcPitch;

  CARD32 I=0;

  int dstW = pOverlay->dstBox.x2 - pOverlay->dstBox.x1;
  int dstH = pOverlay->dstBox.y2 - pOverlay->dstBox.y1;
  int srcW = pOverlay->srcW;
  int srcH = pOverlay->srcH;

  NewPitch = srcPitch = pOverlay->pitch;

  /*Set 1 as default, because we don't need change 4-tap DDA scale value in the upscale case*/
  pOverlay->f_scale = 1.0;

	if (dstW == srcW) {
	     pOverlay->HUSF   = 0x00;
	     pOverlay->IntBit = 0x05;
	}
	else if (dstW > srcW) {

	     /*pOverlay->HUSF   = (srcW << 16) / dstW; */

	     if ((dstW > 2) && (srcW > 2)) {
	         pOverlay->HUSF = (((srcW - 2) << 16) + dstW - 3) / (dstW - 2);
	     }
	     else {
	          pOverlay->HUSF = ((srcW << 16) + dstW - 1) / dstW;
	     }
	     pOverlay->IntBit = 0x04;
	}
	/* downscale in horizontal */
	else {

	     int tmpW = dstW;

	     I = 0x00;

	     pOverlay->IntBit = 0x01;

	     /*  */
	     while (srcW >= tmpW)
	     {
	           tmpW <<= 1;
	           I++;
	     }

	     pOverlay->wHPre = (CARD8)(I - 1);
	     dstW <<= (I - 1);

	     f_temp = srcW/dstW;

	     /* we don't need to change 4-tap DDA scale if upscale */
	     if (f_temp < 1.0)
	           f_temp = 1.0;

	     pOverlay->f_scale = f_temp;

	     if ((srcW % dstW))
	           pOverlay->HUSF = ((srcW - dstW) << 16) / dstW;
	     else
	           pOverlay->HUSF = 0x00;
	}

	if (dstH == srcH) {
	        pOverlay->VUSF   = 0x00;
	        pOverlay->IntBit |= 0x0A;
	}
	else if (dstH > srcH) {
	        dstH += 0x02;
	        /*pOverlay->VUSF = (srcH << 16) / dstH;*/
	        if ((dstH > 2) && (srcH > 2)) {
	           pOverlay->VUSF = (((srcH - 2) << 16) - 32768 + dstH - 3) / (dstH - 2);
	        }
	        else {
	           pOverlay->VUSF = ((srcH << 16) + dstH - 1) / dstH;
	        }

	        pOverlay->IntBit |= 0x08;
	}
	/* downscale in vertical */
	else {
	        I = srcH / dstH;
	        pOverlay->IntBit |= 0x02;

	        if (I < 2)
	        {
	              pOverlay->VUSF = ((srcH - dstH)<<16)/dstH;
	        }
	        else
	        {
	              if (((srcPitch * I)>>2) > 0xFFF)
	              {
	                       I = (0xFFF*2/srcPitch);
	                       pOverlay->VUSF = 0xFFFF;
	              }
	              else
	              {
	                       dstH = I * dstH;

	                       if (srcH % dstH)
	                                pOverlay->VUSF = ((srcH - dstH) << 16) / dstH;
	                       else
	                                pOverlay->VUSF = 0x00;
	              }

	              /* set video frame buffer offset */
	              NewPitch = (srcPitch*I);
	        }
	}

	pOverlay->pitch = (CARD16)(NewPitch);
 }

static void
set_contrast_factor(XGIPtr pXGI, XGIOverlayPtr pOverlay)
{
   ScrnInfoPtr    pScrn = pXGI->pScrn;
   CARD16         screenX = pScrn->currentMode->HDisplay;
   CARD16         screenY = pScrn->currentMode->VDisplay;

   CARD32         value, SamplePixel, dwTotalPixel;

   CARD16         top, left;
   CARD16         bottom, right;

    top    = pOverlay->dstBox.y1;
    bottom = pOverlay->dstBox.y2;

    if (bottom > screenY)
        bottom = screenY;

    left  = pOverlay->dstBox.x1;
    right = pOverlay->dstBox.x2;

    if (right > screenX)
        right = screenX;

    dwTotalPixel = (bottom - top) * (right - left);

    value = (dwTotalPixel - 10000) / 20000;

    if (value > 3 )
      value = 3;

    pOverlay->dwContrastFactor = value;

    switch (value) {
      case 1: SamplePixel = 4096; break;
      case 2: SamplePixel = 8192; break;
      case 3: SamplePixel = 8192; break;

      default:
         SamplePixel = 2048;
         break;
    }

    pOverlay->SamplePixel = (SamplePixel << 10) / dwTotalPixel;

}

static void
set_line_buf_size(XGIOverlayPtr pOverlay)
{
    CARD8   preHIDF;
    CARD32 dwI;
    CARD32 dwSrcWidth = pOverlay->srcW;
    int	   pixelFormat = pOverlay->pixelFormat;

    if ((pixelFormat == PIXEL_FMT_YV12) ||
    (pixelFormat == PIXEL_FMT_NV12) ||
    (pixelFormat == PIXEL_FMT_NV21)) 
    {
        preHIDF = pOverlay->wHPre & 0x07;

		if (preHIDF < 2)
                {
			preHIDF = 2;
		}

		if (dwSrcWidth & ~(0xffffff80 << (preHIDF - 2)))
		{
			dwI = (dwSrcWidth >> (preHIDF + 5)) + 1;
		}
                else
		{
			dwI = dwSrcWidth >> (preHIDF + 5);
		}

		pOverlay->lineBufSize = dwI * (0x01 << (preHIDF + 2)) - 1;

    }
    /* not plane format */
	else
	{
		if (dwSrcWidth & 0x07)
			dwI = (dwSrcWidth >> 3) + 1;
		        else
		    dwI = (dwSrcWidth >> 3);

		pOverlay->lineBufSize = dwI;
	}
}

static void
XGIDisplayVideo(ScrnInfoPtr pScrn, XGIPortPrivPtr pPriv)
{
   XGIPtr pXGI = XGIPTR(pScrn);

   short srcPitch = pPriv->srcPitch;
   short height = pPriv->height;
   XGIOverlayRec overlay;
   int srcOffsetX=0, srcOffsetY=0;
   int sx=0, sy=0;

   memset(&overlay, 0, sizeof(overlay));
   overlay.pixelFormat = pPriv->id;
   overlay.pitch = srcPitch;
   overlay.keyOP = 0x03;        /* destination colorkey */
   overlay.bobEnable = 0x00;

   overlay.dstBox.x1 = pPriv->drw_x - pScrn->frameX0;
   overlay.dstBox.x2 = pPriv->drw_x + pPriv->drw_w - pScrn->frameX0;
   overlay.dstBox.y1 = pPriv->drw_y - pScrn->frameY0;
   overlay.dstBox.y2 = pPriv->drw_y + pPriv->drw_h - pScrn->frameY0;

   if((overlay.dstBox.x2 < 0) || (overlay.dstBox.y2 < 0))
     return;

   if(overlay.dstBox.x1 < 0) {
     srcOffsetX = pPriv->src_w * (-overlay.dstBox.x1) / pPriv->drw_w;
     overlay.dstBox.x1 = 0;
   }
   if(overlay.dstBox.y1 < 0) {
     srcOffsetY = pPriv->src_h * (-overlay.dstBox.y1) / pPriv->drw_h;
     overlay.dstBox.y1 = 0;
   }

   if (pPriv->videoStatus & CLIENT_CAPTURE_ON)
   {
       /* We use AutoFlip for capture, not need buffer address */
       overlay.PSY = 0x0000;
   }
   else
   {
      switch(pPriv->id){
        case PIXEL_FMT_YV12:
          sx = (pPriv->src_x + srcOffsetX) & ~7;
          sy = (pPriv->src_y + srcOffsetY) & ~1;

          overlay.PSY = pPriv->bufAddr[pPriv->currentBuf] + sx + sy*srcPitch;
          overlay.PSV = pPriv->bufAddr[pPriv->currentBuf] + height*srcPitch + ((sx + sy*srcPitch/2) >> 1);
          overlay.PSU = pPriv->bufAddr[pPriv->currentBuf] + height*srcPitch*5/4 + ((sx + sy*srcPitch/2) >> 1);
          break;
		
		case PIXEL_FMT_NV12:
		case PIXEL_FMT_NV21:
			sx = (pPriv->src_x + srcOffsetX) & ~7;
			sy = (pPriv->src_y + srcOffsetY) & ~1;
			overlay.PSY = pPriv->bufAddr[pPriv->currentBuf] + sx + sy*srcPitch;
			overlay.PSV = pPriv->bufAddr[pPriv->currentBuf] + height*srcPitch + ((sx + sy*srcPitch/2) >> 1);
			overlay.PSU = overlay.PSV; 
			break;
			
        case PIXEL_FMT_YUY2:
		case PIXEL_FMT_UYVY:
		case PIXEL_FMT_YVYU:
		case PIXEL_FMT_RGB6:
		case PIXEL_FMT_RGB5:
          sx = (pPriv->src_x + srcOffsetX) & ~1;
          sy = (pPriv->src_y + srcOffsetY);

          overlay.PSY = pPriv->bufAddr[pPriv->currentBuf] + sx*2 + sy*srcPitch;
              break;

        default:
		/* ErrorF("UnSurpported Format"); */
          break;
      }

   }

   overlay.srcW = pPriv->src_w - (sx - pPriv->src_x);
   overlay.srcH = pPriv->src_h - (sy - pPriv->src_y);

   /* set line buffer length */
   set_line_buf_size (&overlay);

   /* set scale factor */
   set_scale_factor (&overlay);

   /* contrast factor */
   set_contrast_factor(pXGI, &overlay);

   XGI_SetSelectOverlayReg(pXGI, 0x00);

   XGI_SetColorkeyReg(pXGI, pPriv->colorKey);

   /* set overlay */
   XGI_SetOverlayReg(pXGI, &overlay);

   /* enable overlay */
   XGI_SetEnableOverlayReg(pXGI, TRUE);
}


static void
XGIStopVideo(ScrnInfoPtr pScrn, pointer data, Bool exit)
{
  XGIPortPrivPtr pPriv = (XGIPortPrivPtr)data;
  XGIPtr pXGI = XGIPTR(pScrn);

  REGION_EMPTY(pScrn->pScreen, &pPriv->clip);

  if(exit) {

     if(pPriv->videoStatus & CLIENT_VIDEO_ON) {
       XGI_SetEnableOverlayReg(pXGI, FALSE);
     }

     if (pPriv->videoStatus & CLIENT_CAPTURE_ON) {
       XGI_EnableCaptureAutoFlip(pXGI, FALSE);
     }
	
     if(pPriv->fbAreaPtr) {
       xf86FreeOffscreenArea(pPriv->fbAreaPtr);
       pPriv->fbAreaPtr = NULL;
       pPriv->fbSize = 0;
     }
	
	
     /* clear all flag */
     pPriv->videoStatus = 0;

  } else {
     if(pPriv->videoStatus & CLIENT_VIDEO_ON) {
        pPriv->videoStatus |= OFF_TIMER;
        pPriv->offTime = currentTime.milliseconds + OFF_DELAY;
     }
  }
}


static int
XGIPutImage(
  ScrnInfoPtr pScrn,
  short src_x, short src_y,
  short drw_x, short drw_y,
  short src_w, short src_h,
  short drw_w, short drw_h,
  int id, unsigned char* buf,
  short width, short height,
  Bool sync,
  RegionPtr clipBoxes, pointer data
){	
   XGIPtr pXGI = XGIPTR(pScrn);
   XGIPortPrivPtr pPriv = (XGIPortPrivPtr)data;

   int i;

   int totalSize=0;
/*   int depth = pXGI->CurrentLayout.bitsPerPixel >> 3; */
 
   pPriv->drw_x = drw_x;
   pPriv->drw_y = drw_y;
   pPriv->drw_w = drw_w;
   pPriv->drw_h = drw_h;
   pPriv->src_x = src_x;
   pPriv->src_y = src_y;
   pPriv->src_w = src_w;
   pPriv->src_h = src_h;
   pPriv->id = id;
   pPriv->height = height;

   /* Pixel formats:
      1. YU12:  3 planes:       H    V
               Y sample period  1    1   (8 bit per pixel)
	       V sample period  2    2	 (8 bit per pixel, subsampled)
	       U sample period  2    2   (8 bit per pixel, subsampled)

 	 Y plane is fully sampled (width*height), U and V planes
	 are sampled in 2x2 blocks, hence a group of 4 pixels requires
	 4 + 1 + 1 = 6 bytes. The data is planar, ie in single planes
	 for Y, U and V.
      2. UYVY: 3 planes:        H    V
               Y sample period  1    1   (8 bit per pixel)
	       V sample period  2    1	 (8 bit per pixel, subsampled)
	       U sample period  2    1   (8 bit per pixel, subsampled)
	 Y plane is fully sampled (width*height), U and V planes
	 are sampled in 2x1 blocks, hence a group of 4 pixels requires
	 4 + 2 + 2 = 8 bytes. The data is bit packed, there are no separate
	 Y, U or V planes.
	 Bit order:  U0 Y0 V0 Y1  U2 Y2 V2 Y3 ...
      3. I420: Like YU12, but planes U and V are in reverse order.
      4. YUY2: Like UYVY, but order is
                     Y0 U0 Y1 V0  Y2 U2 Y3 V2 ...
      5. YVYU: Like YUY2, but order is
      		     Y0 V0 Y1 U0  Y2 V2 Y3 U2 ...
   */
   switch(id){
     case PIXEL_FMT_YV12:
     case PIXEL_FMT_NV12:
     case PIXEL_FMT_NV21:
       pPriv->srcPitch = (width + 7) & ~7;
       totalSize = (pPriv->srcPitch * height * 3) >> 1; /* Verified */
       break;
     case PIXEL_FMT_YUY2:
     case PIXEL_FMT_UYVY:
     case PIXEL_FMT_YVYU:
     case PIXEL_FMT_RGB6:
     case PIXEL_FMT_RGB5:
     default:
       pPriv->srcPitch = ((width << 1) + 3) & ~3;	/* Verified */
       totalSize = pPriv->srcPitch * height;
   }

   /* make it a multiple of 16 to simplify to copy loop */
   totalSize += 15;
   totalSize &= ~15;
   
    /* allocate memory */
    do {
      int lines, pitch, depth;
      BoxPtr pBox;
      
      if(totalSize == pPriv->fbSize)
        break;
      
      pPriv->fbSize = totalSize;

      if(pPriv->fbAreaPtr) {
                xf86FreeOffscreenArea(pPriv->fbAreaPtr);
        }
      
      depth = (pScrn->bitsPerPixel + 7 ) / 8;
      pitch = pScrn->displayWidth * depth;
      lines = ((totalSize * 2) / pitch) + 1;
              
      pPriv->fbAreaPtr = xf86AllocateOffscreenArea(pScrn->pScreen, 
                                 pScrn->displayWidth,
                                lines, 0, NULL, NULL, NULL);

      if(!pPriv->fbAreaPtr) {
        xf86DrvMsg(pScrn->scrnIndex, X_ERROR, 
               "Allocate video memory fails\n");
        return BadAlloc;
          }
      
      pBox = &(pPriv->fbAreaPtr->box);     
      pPriv->bufAddr[0] = (pBox->x1 * depth) + (pBox->y1 * pitch); 
      pPriv->bufAddr[1] = pPriv->bufAddr[0] + totalSize;
      
   } while(0);
	
   /* copy data */
   if((pXGI->XvUseMemcpy) || (totalSize < 16)) {
#ifndef NewPath
	   		memcpy(pXGI->FbBase + pPriv->bufAddr[pPriv->currentBuf], buf, totalSize);
#else
		   switch(id){
		   case PIXEL_FMT_YV12:
	       {
		      BYTE *Y, *V, *U, *srcY, *srcV, *srcU;

		      short srcPitch2 = pPriv->srcPitch >> 1;
		      short height2 = height >> 1;
		      short width2 = width >> 1;

		      Y = (BYTE *)(pXGI->FbBase + pPriv->bufAddr[pPriv->currentBuf]);
		      V = Y + pPriv->srcPitch * height;
		      U = V + pPriv->srcPitch * height / 4 ;
			  
		      srcY = buf;
		      srcV = srcY + width * height;
		      srcU = srcV + width  * height / 4;
			  
		      for(i=0; i<height; i++)
		         memcpy(Y + i * pPriv->srcPitch, srcY + i*width, width);
			  
		      for(i=0; i<height2; i++)
		      {
		         memcpy(V + i * srcPitch2, srcV + i*width2, width2);
		         memcpy(U + i * srcPitch2, srcU + i*width2, width2);
		      }
		      break;
		   }
		   case PIXEL_FMT_NV12:
		   case PIXEL_FMT_NV21:
	       {
		      BYTE *Y, *VU, *srcY, *srcVU;
			  
		      short height2 = height >> 1;

		      Y = (BYTE *)(pXGI->FbBase + pPriv->bufAddr[pPriv->currentBuf]);
		      VU = Y + pPriv->srcPitch * height;

		      srcY = buf;
		      srcVU = srcY + width * height;

		      for(i=0; i<height; i++)
		         memcpy(Y + i * pPriv->srcPitch, srcY + i*width, width);
			  
		      for(i=0; i<height2; i++)
		         memcpy(VU + i * pPriv->srcPitch, srcVU + i*width, width);
		      break;
		   }
		   case PIXEL_FMT_YUY2:
		   case PIXEL_FMT_UYVY:
		   case PIXEL_FMT_YVYU:
		   case PIXEL_FMT_RGB6:
		   case PIXEL_FMT_RGB5:
		   {
		      BYTE *Base = (BYTE *)(pXGI->FbBase + pPriv->bufAddr[pPriv->currentBuf]);
		      for(i=0; i<height; i++)
		         memcpy( Base + i * pPriv->srcPitch, buf + i*width*2, width*2);
		      break;
		   }
		   default:
		      memcpy(pXGI->FbBase + pPriv->bufAddr[pPriv->currentBuf], buf, totalSize);
		      break;
		   	}
#endif
	   
   } else {

#ifndef NewPath
	   		BYTE *Base = (BYTE *)(pXGI->FbBase + pPriv->bufAddr[pPriv->currentBuf]);

	      	for(i=0; i<totalSize; i++)
	         	*Base = *(buf + i);
#else
	 	   switch(id){
		   case PIXEL_FMT_YV12:
	           {
		      BYTE *Y, *V, *U, *srcY, *srcV, *srcU;
		      short srcPitch2 = pPriv->srcPitch >> 1;
		      short height2 = height >> 1;
		      short width2 = width >> 1;

		      Y = (BYTE *)(pXGI->FbBase + pPriv->bufAddr[pPriv->currentBuf]);
		      V = Y + pPriv->srcPitch * height;
		      U = V + pPriv->srcPitch * height / 4 ;
		      srcY = buf;
		      srcV = srcY + width * height;
		      srcU = srcV + width * height / 4;

		      for(i=0; i<height; i++)
		      	for(j=0; j<width; j++)
		         *Y = *(srcY + pPriv->srcPitch*i + j);

		      for(i=0; i<height2; i++)
		      	for(j=0; j<width2; j++)
		      	{
		         *V = *(srcV + srcPitch2 * i + j);
		         *U = *(srcU + srcPitch2 * i + j);
		      	} 

		      break;
			  	 }
		   case PIXEL_FMT_NV12:
		   case PIXEL_FMT_NV21:
	       {
		      BYTE *Y, *VU, *srcY, *srcVU;
			  
		      short height2 = height >> 1;

		      Y = (BYTE *)(pXGI->FbBase + pPriv->bufAddr[pPriv->currentBuf]);
		      VU = Y + pPriv->srcPitch * height;

		      srcY = buf;
		      srcVU = srcY + width * height;

		      for(i=0; i<height; i++)
		      	for(j=0; j<width; j++)
		         *Y = *(srcY + pPriv->srcPitch*i + j);

		      for(i=0; i<height2; i++)
		      	for(j=0; j<width; j++)
		         *VU = *(srcVU + pPriv->srcPitch*i + j);

		      break;
		   }
		   case PIXEL_FMT_YUY2:
		   case PIXEL_FMT_UYVY:
	   	   case PIXEL_FMT_YVYU:
	       case PIXEL_FMT_RGB6:
	       case PIXEL_FMT_RGB5:
		   {
		      BYTE *Base = (BYTE *)(pXGI->FbBase + pPriv->bufAddr[pPriv->currentBuf]);

		      for(i=0; i<height; i++)
		      	for(j=0; j<width*2; j++)
		         *Base = *(buf + pPriv->srcPitch * i + j);
		      break;
		   }
		   default:
		   {
		      BYTE *Base = (BYTE *)(pXGI->FbBase + pPriv->bufAddr[pPriv->currentBuf]);

		      for(i=0; i<height; i++)
		      	for(j=0; j<width; j++)
		         *Base = *(buf + width * i + j);

		      break;
		   }
		  } /* end of switch */
#endif
		
   }

   XGIDisplayVideo(pScrn, pPriv);

   /* update cliplist */
#if XF86_VERSION_CURRENT < XF86_VERSION_NUMERIC(4,3,99,0,0)
     if(!RegionsEqual(&pPriv->clip, clipBoxes)) 
#else
     if(!REGION_EQUAL(pScrn->pScreen, &pPriv->clip, clipBoxes)) 
#endif
     {      
     	REGION_COPY(pScrn->pScreen, &pPriv->clip, clipBoxes);
	
        /* draw these */
     } else {
#if XF86_VERSION_CURRENT < XF86_VERSION_NUMERIC(4,2,99,0,0)
           XAAFillSolidRects(pScrn, pPriv->colorKey, GXcopy, ~0,
                           REGION_NUM_RECTS(clipBoxes),
                           REGION_RECTS(clipBoxes));
#else
	   xf86XVFillKeyHelper(pScrn->pScreen, pPriv->colorKey, clipBoxes);
#endif
   } /* update cliplist */
   
   pPriv->currentBuf ^= 1;
   pPriv->videoStatus = CLIENT_VIDEO_ON;

    return Success;
}


static int
XGIQueryImageAttributes(
  ScrnInfoPtr pScrn,
  int id,
  unsigned short *w, unsigned short *h,
  int *pitches, int *offsets
){
    int pitchY, pitchUV;
    int size, sizeY, sizeUV;

    if(*w < IMAGE_MIN_WIDTH) *w = IMAGE_MIN_WIDTH;
    if(*h < IMAGE_MIN_HEIGHT) *h = IMAGE_MIN_HEIGHT;

    if(*w > DummyEncoding.width) *w = DummyEncoding.width;
    if(*h > DummyEncoding.height) *h = DummyEncoding.height;

    switch(id) {
    case PIXEL_FMT_YV12:
        *w = (*w + 7) & ~7;
        *h = (*h + 1) & ~1;
        pitchY = *w;
        pitchUV = *w >> 1;
        if(pitches) {
          pitches[0] = pitchY;
          pitches[1] = pitches[2] = pitchUV;
        }
        sizeY = pitchY * (*h);
        sizeUV = pitchUV * ((*h) >> 1);
        if(offsets) {
          offsets[0] = 0;
          offsets[1] = sizeY;
          offsets[2] = sizeY + sizeUV;
        }
        size = sizeY + (sizeUV << 1);
        break;
    case PIXEL_FMT_NV12:
    case PIXEL_FMT_NV21:
        *w = (*w + 7) & ~7;
        *h = (*h + 1) & ~1;
		pitchY = *w;
    	pitchUV = *w;
    	if(pitches) {
      	    pitches[0] = pitchY;
            pitches[1] = pitchUV;
        }
    	sizeY = pitchY * (*h);
    	sizeUV = pitchUV * ((*h) >> 1);
    	if(offsets) {
          offsets[0] = 0;
          offsets[1] = sizeY;
        }
        size = sizeY + (sizeUV << 1);
        break;
    case PIXEL_FMT_YUY2:
    case PIXEL_FMT_UYVY:
    case PIXEL_FMT_YVYU:
    case PIXEL_FMT_RGB6:
    case PIXEL_FMT_RGB5:
    default:
        *w = (*w + 1) & ~1;
        pitchY = *w << 1;
        if(pitches) pitches[0] = pitchY;
        if(offsets) offsets[0] = 0;
        size = pitchY * (*h);
        break;
    }

    return size;
}
