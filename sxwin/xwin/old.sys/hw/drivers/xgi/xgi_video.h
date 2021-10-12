/* $XFree86: xc/programs/Xserver/hw/xfree86/drivers/xgi/xgi_video.h,v 1.1 2005/05/02 13:28:02 dawes Exp $ */

#ifndef _XGI_VIDEO_H_
#define _XGI_VIDEO_H_

#define PIXEL_FMT_YV12 FOURCC_YV12  /* 0x32315659 */
#define PIXEL_FMT_UYVY FOURCC_UYVY  /* 0x59565955 */
#define PIXEL_FMT_YUY2 FOURCC_YUY2  /* 0x32595559 */
#define PIXEL_FMT_RGB5 0x35315652
#define PIXEL_FMT_RGB6 0x36315652
#define PIXEL_FMT_YVYU 0x55595659   
#define PIXEL_FMT_NV12 0x3231564e   
#define PIXEL_FMT_NV21 0x3132564e   

#define IMAGE_MIN_WIDTH         32
#define IMAGE_MIN_HEIGHT        24
#define IMAGE_MAX_WIDTH         1920
#define IMAGE_MAX_HEIGHT        1080

#define DISPMODE_SINGLE1 0x1
#define DISPMODE_SINGLE2 0x2
#define DISPMODE_MIRROR  0x4

#define OFF_DELAY       200  /* milliseconds */
#define FREE_DELAY      60000

#define OFF_TIMER              0x01
#define FREE_TIMER             0x02
#define CLIENT_VIDEO_ON        0x04
#define CLIENT_CAPTURE_ON      0x08
#define TIMER_MASK      (OFF_TIMER | FREE_TIMER)

typedef struct {
    FBLinearPtr     linear;
    FBAreaPtr       fbAreaPtr;
    int             fbSize;
    CARD32          bufAddr[2];

    unsigned char   currentBuf;

    short                   drw_x, drw_y, drw_w, drw_h;
    short                   src_x, src_y, src_w, src_h;
    int                             id;
    short                   srcPitch, height;

    INT32   brightness;
    INT32   contrast;
    INT32   saturation;
    INT32   hue;

    RegionRec       clip;
    CARD32          colorKey;

    CARD32          videoStatus;
    Time            offTime;
    Time            freeTime;
	
	short	 linebufMergeLimit;

    CARD32                  displayMode;

	Bool            grabbedByV4L;  /*V4L stuff*/
	int             pitch;
	int             offset;

} XGIPortPrivRec, *XGIPortPrivPtr;

#define GET_PORT_PRIVATE(pScrn) \
   (XGIPortPrivPtr)((XGIPTR(pScrn))->adaptor->pPortPrivates[0].ptr)

extern void XGI_SetSRRegMask(XGIPtr, CARD8, CARD8, CARD8);
extern void XGIUpdateXvGamma(XGIPtr, XGIPortPrivPtr);


#endif /* _XGI_VIDEO_H_ */

