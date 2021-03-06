
/**************************************************************************

Copyright 1998-1999 Precision Insight, Inc., Cedar Park, Texas.
Copyright ? 2002 David Dawes

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
IN NO EVENT SHALL PRECISION INSIGHT AND/OR ITS SUPPLIERS BE LIABLE FOR
ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

**************************************************************************/
/* $XFree86: xc/programs/Xserver/hw/xfree86/drivers/i810/i830.h,v 1.22 2006/03/10 12:44:20 alanh Exp $ */

/*
 * Authors:
 *   Keith Whitwell <keith@tungstengraphics.com>
 *   David Dawes <dawes@xfree86.org>
 *
 */

#if 0
#define I830DEBUG
#endif

#ifndef REMAP_RESERVED
#define REMAP_RESERVED 0
#endif

#ifndef _I830_H_
#define _I830_H_

#include "xf86_ansic.h"
#include "compiler.h"
#include "xf86PciInfo.h"
#include "xf86Pci.h"
#include "i810_reg.h"
#include "xaa.h"
#include "xf86Cursor.h"
#include "xf86xv.h"
#include "xf86int10.h"
#include "vbe.h"
#include "vgaHW.h"

#ifdef XF86DRI
#include "xf86drm.h"
#include "sarea.h"
#define _XF86DRI_SERVER_
#include "xf86dri.h"
#include "dri.h"
#include "GL/glxint.h"
#include "i830_dri.h"
#endif

#include "common.h"

/* I830 Video BIOS support */

/*
 * The mode handling is based upon the VESA driver written by
 * Paulo C?sar Pereira de Andrade <pcpa@conectiva.com.br>.
 */

#define PIPE_NONE	0<<0
#define PIPE_CRT	1<<0
#define PIPE_TV		1<<1
#define PIPE_DFP	1<<2
#define PIPE_LFP	1<<3
#define PIPE_CRT2	1<<4
#define PIPE_TV2	1<<5
#define PIPE_DFP2	1<<6
#define PIPE_LFP2	1<<7

typedef struct _VESARec {
   /* SVGA state */
   pointer state, pstate;
   int statePage, stateSize, stateMode, stateRefresh;
   CARD32 *savedPal;
   int savedScanlinePitch;
   xf86MonPtr monitor;
   /* Don't try to set the refresh rate for any modes. */
   Bool useDefaultRefresh;
   /* display start */
   int x, y;
} VESARec, *VESAPtr;


typedef struct _I830Rec *I830Ptr;

typedef void (*I830WriteIndexedByteFunc)(I830Ptr pI830, IOADDRESS addr,
                                         CARD8 index, CARD8 value);
typedef CARD8(*I830ReadIndexedByteFunc)(I830Ptr pI830, IOADDRESS addr,
                                        CARD8 index);
typedef void (*I830WriteByteFunc)(I830Ptr pI830, IOADDRESS addr, CARD8 value);
typedef CARD8(*I830ReadByteFunc)(I830Ptr pI830, IOADDRESS addr);

/* Linear region allocated in framebuffer. */
typedef struct _I830MemPool *I830MemPoolPtr;
typedef struct _I830MemRange *I830MemRangePtr;
typedef struct _I830MemRange {
   long Start;
   long End;
   long Size;
   unsigned long Physical;
   unsigned long Offset;		/* Offset of AGP-allocated portion */
   unsigned long Alignment;
   int Key;
   I830MemPoolPtr Pool;
} I830MemRange;

typedef struct _I830MemPool {
   I830MemRange Total;
   I830MemRange Free;
   I830MemRange Fixed;
   I830MemRange Allocated;
} I830MemPool;

typedef struct {
   int tail_mask;
   I830MemRange mem;
   unsigned char *virtual_start;
   int head;
   int tail;
   int space;
} I830RingBuffer;

typedef struct {
   unsigned int Fence[8];
} I830RegRec, *I830RegPtr;

typedef struct {
   int            lastInstance;
   int            refCount;
   ScrnInfoPtr    pScrn_1;
   ScrnInfoPtr    pScrn_2;
   int            RingRunning;
   int            XvInUse;
} I830EntRec, *I830EntPtr;

typedef struct _I830Rec {
   unsigned char *MMIOBase;
   unsigned char *FbBase;
   int cpp;

   unsigned int bios_version;

   Bool newPipeSwitch;

   Bool fakeSwitch;
   
   int fixedPipe;

   Bool Clone;
   int CloneRefresh;
   int CloneHDisplay;
   int CloneVDisplay;

   I830EntPtr entityPrivate;	
   int pipe, origPipe;
   int init;

   unsigned int bufferOffset;		/* for I830SelectBuffer */
   BoxRec FbMemBox;
   BoxRec FbMemBox2;
   int CacheLines;

   /* These are set in PreInit and never changed. */
   unsigned long FbMapSize;
   unsigned long TotalVideoRam;
   I830MemRange StolenMemory;		/* pre-allocated memory */
   unsigned long BIOSMemorySize;	/* min stolen pool size */
   int BIOSMemSizeLoc;

   /* These change according to what has been allocated. */
   long FreeMemory;
   I830MemRange MemoryAperture;
   I830MemPool StolenPool;
   unsigned long allocatedMemory;

   /* Regions allocated either from the above pools, or from agpgart. */
   /* for single and dual head configurations */
   I830MemRange FrontBuffer;
   I830MemRange FrontBuffer2;
   I830MemRange Scratch;
   I830MemRange Scratch2;

   /* Regions allocated either from the above pools, or from agpgart. */
   I830MemRange	*CursorMem;
   I830MemRange	*CursorMemARGB;
   I830RingBuffer *LpRing;

#if REMAP_RESERVED
   I830MemRange Dummy;
#endif

   /* For Xvideo */
   I830MemRange *OverlayMem;
   I830MemRange LinearMem;
   unsigned int LinearAlloc;

#ifdef XF86DRI
   I830MemRange BackBuffer;
   I830MemRange DepthBuffer;
   I830MemRange TexMem;
   I830MemRange ContextMem;
   int TexGranularity;
   int drmMinor;
   Bool have3DWindows;
#endif

   Bool NeedRingBufferLow;
   Bool allowPageFlip;
   Bool disableTiling;

   int backPitch;

   Bool CursorNeedsPhysical;
   Bool CursorIsARGB;

   int MonType1;
   int MonType2;
   Bool specifiedMonitor;

   DGAModePtr DGAModes;
   int numDGAModes;
   Bool DGAactive;
   int DGAViewportStatus;

   int Chipset;
   unsigned long LinearAddr;
   unsigned long MMIOAddr;
   IOADDRESS ioBase;
   EntityInfoPtr pEnt;
   pciVideoPtr PciInfo;
   PCITAG PciTag;
   CARD8 variant;

   unsigned int BR[20];

   int GttBound;

   unsigned char **ScanlineColorExpandBuffers;
   int NumScanlineColorExpandBuffers;
   int nextColorExpandBuf;

   I830RegRec SavedReg;
   I830RegRec ModeReg;

   Bool noAccel;
   Bool SWCursor;
   Bool cursorOn;
   XAAInfoRecPtr AccelInfoRec;
   xf86CursorInfoPtr CursorInfoRec;
   CloseScreenProcPtr CloseScreen;

   I830WriteIndexedByteFunc writeControl;
   I830ReadIndexedByteFunc readControl;
   I830WriteByteFunc writeStandard;
   I830ReadByteFunc readStandard;

   Bool XvDisabled;			/* Xv disabled in PreInit. */
   Bool XvEnabled;			/* Xv enabled for this generation. */

   int colorKey;
   XF86VideoAdaptorPtr adaptor;
   ScreenBlockHandlerProcPtr BlockHandler;
   Bool *overlayOn;

   Bool directRenderingDisabled;	/* DRI disabled in PreInit. */
   Bool directRenderingEnabled;		/* DRI enabled this generation. */

#ifdef XF86DRI
   Bool directRenderingOpen;
   int LockHeld;
   DRIInfoPtr pDRIInfo;
   int drmSubFD;
   int numVisualConfigs;
   __GLXvisualConfig *pVisualConfigs;
   I830ConfigPrivPtr pVisualConfigsPriv;
   drm_handle_t buffer_map;
   drm_handle_t ring_map;
#endif

   /* Broken-out options. */
   OptionInfoPtr Options;
   int rotate;
   Bool shadowFB;

   /* Support for shadowFB and rotation. */
   unsigned char *shadowPtr;
   int shadowPitch;
   void (*PointerMoved)(int, int, int);

   /* Stolen memory support */
   Bool StolenOnly;

   /* Video BIOS support. */
   vbeInfoPtr pVbe;
   VbeInfoBlock *vbeInfo;
   VESAPtr vesa;

   Bool overrideBIOSMemSize;
   int saveBIOSMemSize;
   int newBIOSMemSize;
   Bool useSWF1;
   int saveSWF1;

   Bool swfSaved;
   CARD32 saveSWF0;
   CARD32 saveSWF4;

   /* Use BIOS call 0x5f05 to set the refresh rate. */
   Bool useExtendedRefresh;

   Bool checkDevices;
   int monitorSwitch;
   int operatingDevices;
   int savedDevices;
   int toggleDevices;
   int lastDevice0, lastDevice1, lastDevice2;

   /* These are indexed by the display types */
   Bool displayAttached[NumDisplayTypes];
   Bool displayPresent[NumDisplayTypes];
   BoxRec displaySize[NumDisplayTypes];

   /* [0] is Pipe A, [1] is Pipe B. */
   int availablePipes;
   int pipeDevices[MAX_DISPLAY_PIPES];
   /* [0] is display plane A, [1] is display plane B. */
   Bool pipeEnabled[MAX_DISPLAY_PIPES];
   BoxRec pipeDisplaySize[MAX_DISPLAY_PIPES];
   int planeEnabled[MAX_DISPLAY_PIPES];

   /* Driver phase/state information */
   Bool preinit;
   Bool starting;
   Bool closing;
   Bool suspended;
   Bool leaving;

   /* fbOffset converted to (x, y). */
   int xoffset;
   int yoffset;

   int SaveGeneration;
   Bool vbeRestoreWorkaround;
   Bool displayInfo;
   Bool devicePresence;

   OsTimerPtr devicesTimer;
} I830Rec;

#define I830PTR(p) ((I830Ptr)((p)->driverPrivate))
#define I830REGPTR(p) (&(I830PTR(p)->ModeReg))

#define I830_SELECT_FRONT	0
#define I830_SELECT_BACK	1
#define I830_SELECT_DEPTH	2

/* I830 specific functions */
extern int I830WaitLpRing(ScrnInfoPtr pScrn, int n, int timeout_millis);
extern void I830SetPIOAccess(I830Ptr pI830);
extern void I830SetMMIOAccess(I830Ptr pI830);
extern void I830PrintErrorState(ScrnInfoPtr pScrn);
extern void I830Sync(ScrnInfoPtr pScrn);
extern void I830InitHWCursor(ScrnInfoPtr pScrn);
extern Bool I830CursorInit(ScreenPtr pScreen);
extern void I830EmitInvarientState(ScrnInfoPtr pScrn);
extern void I830SelectBuffer(ScrnInfoPtr pScrn, int buffer);

extern void I830RefreshRing(ScrnInfoPtr pScrn);
extern void I830EmitFlush(ScrnInfoPtr pScrn);

extern Bool I830DGAInit(ScreenPtr pScreen);

extern void I830InitVideo(ScreenPtr pScreen);
extern void I830VideoSwitchModeBefore(ScrnInfoPtr pScrn, DisplayModePtr mode);
extern void I830VideoSwitchModeAfter(ScrnInfoPtr pScrn, DisplayModePtr mode);

#ifdef XF86DRI
extern Bool I830Allocate3DMemory(ScrnInfoPtr pScrn, const int flags);
extern void I830SetupMemoryTiling(ScrnInfoPtr pScrn);
extern Bool I830DRIScreenInit(ScreenPtr pScreen);
extern Bool I830DRIDoMappings(ScreenPtr pScreen);
extern Bool I830DRIResume(ScreenPtr pScreen);
extern void I830DRICloseScreen(ScreenPtr pScreen);
extern Bool I830DRIFinishScreenInit(ScreenPtr pScreen);
#endif
extern Bool I830AccelInit(ScreenPtr pScreen);
extern void I830SetupForScreenToScreenCopy(ScrnInfoPtr pScrn, int xdir,
					   int ydir, int rop,
					   unsigned int planemask,
					   int trans_color);
extern void I830SubsequentScreenToScreenCopy(ScrnInfoPtr pScrn, int srcX,
					     int srcY, int dstX, int dstY,
					     int w, int h);
extern void I830SetupForSolidFill(ScrnInfoPtr pScrn, int color, int rop,
				  unsigned int planemask);
extern void I830SubsequentSolidFillRect(ScrnInfoPtr pScrn, int x, int y,
					int w, int h);

extern void I830ResetAllocations(ScrnInfoPtr pScrn, const int flags);
extern long I830CheckAvailableMemory(ScrnInfoPtr pScrn);
extern long I830GetExcessMemoryAllocations(ScrnInfoPtr pScrn);
extern Bool I830Allocate2DMemory(ScrnInfoPtr pScrn, const int flags);
extern Bool I830DoPoolAllocation(ScrnInfoPtr pScrn, I830MemPool *pool);
extern Bool I830FixupOffsets(ScrnInfoPtr pScrn);
extern Bool I830BindGARTMemory(ScrnInfoPtr pScrn);
extern Bool I830UnbindGARTMemory(ScrnInfoPtr pScrn);
extern unsigned long I830AllocVidMem(ScrnInfoPtr pScrn, I830MemRange *result,
				     I830MemPool *pool, unsigned long size,
				     unsigned long alignment, int flags);

extern void I830PrintAllRegisters(I830RegPtr i830Reg);
extern void I830ReadAllRegisters(I830Ptr pI830, I830RegPtr i830Reg);

extern void I830ChangeFrontbuffer(ScrnInfoPtr pScrn,int buffer);

extern DisplayModePtr I830GetModePool(ScrnInfoPtr pScrn, vbeInfoPtr pVbe,
					VbeInfoBlock *vbe);
extern void I830SetModeParameters(ScrnInfoPtr pScrn, vbeInfoPtr pVbe);
extern void I830UnsetModeParameters(ScrnInfoPtr pScrn, vbeInfoPtr pVbe);
extern void I830PrintModes(ScrnInfoPtr pScrn);
extern int I830GetBestRefresh(ScrnInfoPtr pScrn, int refresh);
extern Bool I830CheckModeSupport(ScrnInfoPtr pScrn, int x, int y, int mode);
extern void I830PointerMoved(int index, int x, int y);
extern void I830RefreshArea(ScrnInfoPtr pScrn, int num, BoxPtr pbox);
extern void I830RefreshArea8(ScrnInfoPtr pScrn, int num, BoxPtr pbox);
extern void I830RefreshArea16(ScrnInfoPtr pScrn, int num, BoxPtr pbox);
extern void I830RefreshArea24(ScrnInfoPtr pScrn, int num, BoxPtr pbox);
extern void I830RefreshArea32(ScrnInfoPtr pScrn, int num, BoxPtr pbox);

/*
 * 12288 is set as the maximum, chosen because it is enough for
 * 1920x1440@32bpp with a 2048 pixel line pitch with some to spare.
 */
#define I830_MAXIMUM_VBIOS_MEM		12288
#define I830_DEFAULT_VIDEOMEM_2D	(MB(32) / 1024)
#define I830_DEFAULT_VIDEOMEM_3D	(MB(64) / 1024)

/* Flags for memory allocation function */
#define FROM_ANYWHERE			0x00000000
#define FROM_POOL_ONLY			0x00000001
#define FROM_NEW_ONLY			0x00000002
#define FROM_MASK			0x0000000f

#define ALLOCATE_AT_TOP			0x00000010
#define ALLOCATE_AT_BOTTOM		0x00000020
#define FORCE_GAPS			0x00000040

#define NEED_PHYSICAL_ADDR		0x00000100
#define ALIGN_BOTH_ENDS			0x00000200
#define FORCE_LOW			0x00000400

#define ALLOC_NO_TILING			0x00001000
#define ALLOC_INITIAL			0x00002000

#define ALLOCATE_DRY_RUN		0x80000000

/* Chipset registers for VIDEO BIOS memory RW access */
#define _855_DRAM_RW_CONTROL 0x58
#define _845_DRAM_RW_CONTROL 0x90
#define DRAM_WRITE    0x33330000

#endif /* _I830_H_ */
