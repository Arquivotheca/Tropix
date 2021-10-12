/* $XFree86: xc/programs/Xserver/hw/xfree86/drivers/xgi/xgi_accel.c,v 1.1 2005/05/02 13:28:02 dawes Exp $ */
/*

 *
 * Copyright (C) 1998, 1999 by Alan Hourihane, Wigan, England.
 * Parts Copyright (C) 2001-2004 Thomas Winischhofer, Vienna, Austria.
 *
 * Licensed under the following terms:
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appears in all copies and that both that copyright
 * notice and this permission notice appear in supporting documentation, and
 * and that the name of the copyright holder not be used in advertising
 * or publicity pertaining to distribution of the software without specific,
 * written prior permission. The copyright holder makes no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without expressed or implied warranty.
 *
 * THE COPYRIGHT HOLDER DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO
 * EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Authors:  Alan Hourihane <alanh@fairlite.demon.co.uk>,
 *           Mike Chapman <mike@paranoia.com>,
 *           Juanjo Santamarta <santamarta@ctv.es>,
 *           Mitani Hiroshi <hmitani@drl.mei.co.jp>,
 *           David Thomas <davtom@dream.org.uk>,
 *	     Thomas Winischhofer <thomas@winischhofer.net>.
 */


#include "xf86.h"
#include "xf86_OSproc.h"
#include "xf86_ansic.h"
#include <xf86_libc.h>

#include "xf86PciInfo.h"
#include "xf86Pci.h"

#include <compiler.h>
#include <miline.h>

#include "xgi_accel.h"
#include "regs.h"
#include "xgi.h"

#include "xaarop.h"
#include <xaa.h>
#include <xaalocal.h>
#include <xf86fbman.h>

/*************************************************************************/

void Volari_Sync(ScrnInfoPtr pScrn);

static void Volari_SetupForScreenToScreenCopy(ScrnInfoPtr pScrn,
                                int xdir, int ydir, int rop,
                                unsigned int planemask, int trans_color);
static void Volari_SubsequentScreenToScreenCopy(ScrnInfoPtr pScrn,
                                int x1, int y1, int x2, int y2,
                                int width, int height);
static void Volari_SetupForSolidFill(ScrnInfoPtr pScrn, int color,
                                int rop, unsigned int planemask);
static void Volari_SubsequentSolidFillRect(ScrnInfoPtr pScrn,
                                int x, int y, int w, int h);
static void Volari_SetupForMonoPatternFill(ScrnInfoPtr pScrn,
                                int patx, int paty, int fg, int bg,
                                int rop, unsigned int planemask);
static void Volari_SubsequentMonoPatternFill(ScrnInfoPtr pScrn,
                                int patx, int paty,
                                int x, int y, int w, int h);

void Volari_EnableAccelerator(ScrnInfoPtr pScrn) ;
static void Volari_InitCmdQueue(ScrnInfoPtr pScrn) ;
static void Volari_DisableDualPipe(ScrnInfoPtr pScrn) ;
static void Volari_DisableCmdQueue(ScrnInfoPtr pScrn) ;

static int Alignment;

extern int XGI_FbDevExist;

void
Volari_EnableAccelerator(ScrnInfoPtr pScrn)
{
    XGIPtr  pXGI = XGIPTR(pScrn);

    PDEBUG(ErrorF("Volari_EnableAccelerator()\n")) ;

    switch( pXGI->Chipset )
    {
	case PCI_CHIP_XGIXG40:
	default:
    	orXGIIDXREG(XGISR, 0x1E, 0xDA) ;
    }


    if( pXGI->TurboQueue )
    {
        Volari_InitCmdQueue(pScrn) ;
    }
}

static void
Volari_InitCmdQueue(ScrnInfoPtr pScrn)
{
    XGIPtr pXGI = XGIPTR(pScrn);
    unsigned long ulXGITempRP ;
    unsigned long ulCR55 ;
    unsigned long ulSR26 ;
    unsigned long temp ;
 /*   unsigned long ulFlag = 0 ; */

    PDEBUG(ErrorF("Volari_InitCmdQueue()\n"));
    PDEBUG(ErrorF( "pXGI->IOBase = 0x%08lX, [%04X] = 0x%08lX\n",(unsigned long)(pXGI->IOBase),0x85c0, XGIMMIOLONG(0x85c0))) ;
    PDEBUG(ErrorF( "pXGI->IOBase = 0x%08lX, [%04X] = 0x%08lX\n",(unsigned long)(pXGI->IOBase),0x85c4, XGIMMIOLONG(0x85c4))) ;
    PDEBUG(ErrorF( "pXGI->IOBase = 0x%08lX, [%04X] = 0x%08lX\n",(unsigned long)(pXGI->IOBase),0x85c8, XGIMMIOLONG(0x85c8))) ;
    PDEBUG(ErrorF( "pXGI->IOBase = 0x%08lX, [%04X] = 0x%08lX\n",(unsigned long)(pXGI->IOBase),0x85cc, XGIMMIOLONG(0x85cc))) ;

    inXGIIDXREG(XGICR, 0x55, ulCR55) ;
    andXGIIDXREG(XGICR, 0x55, 0x33) ;
    orXGIIDXREG(XGISR, 0x26, 1) ;    /* reset cmd queue */

    XGI_w_port = Volari_GetSwWP() ;      /* GuardBand()  Init   */
    XGI_r_port = Volari_GetHwRP() ;

    if( pXGI->Chipset == PCI_CHIP_XGIXG20 )
    {
        Alignment = 1 ;		/* 64 bits   */

        switch(pXGI->cmdQueueSize)
        {
        case 64*1024:
            ulSR26 = 0x40 + 0x00 ;
            break ;
        case 128*1024:
            ulSR26 = 0x40 + 0x04 ;
            break ;
        default:
            /* reset the command queue information */

            pXGI->cmdQueueSize = 128*1024 ; /* reset the command queue */
            pXGI->cmdQueueSizeMask = pXGI->cmdQueueSize - 1 ;
            if( XGI_FbDevExist )
            {
                if( pScrn->videoRam < 8*1024 )
                {
                    pXGI->cmdQueueOffset = 4*1024*1024 - pXGI->cmdQueueSize ;
                }
                else
                {
                    pXGI->cmdQueueOffset = 8*1024*1024 - pXGI->cmdQueueSize ;
                }
            }
            else
            {
                pXGI->cmdQueueOffset = (pScrn->videoRam)*1024 - pXGI->cmdQueueSize ;
            }

            pXGI->cmdQueueLen  = 0 ;
            pXGI->cmdQueueLenMin = 0x200 ;
            pXGI->cmdQueueLenMax = pXGI->cmdQueueSize - pXGI->cmdQueueLenMin ;

            ulSR26 = 0x40 ;
            break ;
        }
    }
    else
    {
        Alignment = 2 ;		/* 128 bits   */

        switch(pXGI->cmdQueueSize)
        {
        case 512*1024:
            ulSR26 = 0x40 + 0x00 ;
            break ;
        case 1024*1024:
            ulSR26 = 0x40 + 0x04 ;
            break ;
        case 2*1024*1024:
            ulSR26 = 0x40 + 0x08 ;
            break ;
        case 4*1024*1024:
            ulSR26 = 0x40 + 0x0C ;
            break ;
        default:
            /* reset the command queue information */

            pXGI->cmdQueueSize = 512*1024 ; /* reset the command queue */
            pXGI->cmdQueueSizeMask = pXGI->cmdQueueSize - 1 ;
            if( XGI_FbDevExist )
            {
                if( pScrn->videoRam < 8*1024 )
                {
                    pXGI->cmdQueueOffset = 4*1024*1024 - pXGI->cmdQueueSize ;
                }
                else
                {
                    pXGI->cmdQueueOffset = 8*1024*1024 - pXGI->cmdQueueSize ;
                }
            }
            else
            {
                pXGI->cmdQueueOffset = (pScrn->videoRam)*1024 - pXGI->cmdQueueSize ;
            }

            pXGI->cmdQueueLen  = 0 ;
            pXGI->cmdQueueLenMin = 0x200 ;
            pXGI->cmdQueueLenMax = pXGI->cmdQueueSize - pXGI->cmdQueueLenMin ;

            ulSR26 = 0x40 ;
            break ;
        }
    }

    pXGI->CursorOffset = pXGI->cmdQueueOffset - VOLARI_CURSOR_SHAPE_SIZE ;

    temp = (unsigned long)pXGI->FbBase ;
    temp += pXGI->cmdQueueOffset ;
    pXGI->cmdQueueBase = (unsigned char *)temp ;

    PDEBUG(ErrorF( "pXGI->FbBase = 0x%lX\n", pXGI->FbBase )) ;
    PDEBUG(ErrorF( "pXGI->cmdQueueOffset = 0x%lX\n", pXGI->cmdQueueOffset )) ;
    PDEBUG(ErrorF( "pXGI->cmdQueueBase = 0x%lX\n", pXGI->cmdQueueBase )) ;

    outXGIIDXREG(XGISR, 0x26, ulSR26) ;

    ulXGITempRP=Volari_GetHwRP() ;

    PDEBUG(ErrorF( "pXGI->IOBase = 0x%08lX, [%04X] = 0x%08lX\n",(unsigned long)(pXGI->IOBase),0x85c0, XGIMMIOLONG(0x85c0))) ;
    PDEBUG(ErrorF( "pXGI->IOBase = 0x%08lX, [%04X] = 0x%08lX\n",(unsigned long)(pXGI->IOBase),0x85c4, XGIMMIOLONG(0x85c4))) ;
    PDEBUG(ErrorF( "pXGI->IOBase = 0x%08lX, [%04X] = 0x%08lX\n",(unsigned long)(pXGI->IOBase),0x85c8, XGIMMIOLONG(0x85c8))) ;
    PDEBUG(ErrorF( "pXGI->IOBase = 0x%08lX, [%04X] = 0x%08lX\n",(unsigned long)(pXGI->IOBase),0x85cc, XGIMMIOLONG(0x85cc))) ;

    /* XGI315 */
    pXGI->cmdQueue_shareWP_only2D = ulXGITempRP;
    /* pXGI->pCQ_shareWritePort = &(pXGI->cmdQueue_shareWP_only2D); */

    Volari_UpdateHwWP(ulXGITempRP) ;
    
    

    MMIO_OUT32(pXGI->IOBase, 0x85C0, pXGI->cmdQueueOffset) ;

    outXGIIDXREG(XGICR, 0x55, ulCR55) ;

    if(pXGI->Chipset == PCI_CHIP_XGIXG40)
    {
        Volari_Idle ;
        Volari_DisableDualPipe(pScrn) ;
        Volari_Idle ;

    }
    PDEBUG(ErrorF("Volari_InitCmdQueue() done.\n")) ;
}

static void
Volari_DisableDualPipe(ScrnInfoPtr pScrn)
{
    XGIPtr pXGI = XGIPTR(pScrn) ;
    unsigned long ulTemp ;
	unsigned long ulValue = MMIO_IN32(pXGI->IOBase, 0x8240) ;
	ulValue |= 1 << 10 ; /* D[10] = 1, Disable Dual Pipe. */

    ulTemp = Volari_GetSwWP() ;

    *(CARD32 *)(pXGI->cmdQueueBase+ulTemp) = (CARD32)(GR_SKPC_HEADER + 0x8240) ;
    *(CARD32 *)(pXGI->cmdQueueBase+ulTemp+4) = (CARD32)(ulValue) ;

    if( pXGI->Chipset == PCI_CHIP_XGIXG40 )
    {
        *(CARD32 *)(pXGI->cmdQueueBase+ulTemp+8) = (CARD32)(GR_NIL_CMD) ;
        *(CARD32 *)(pXGI->cmdQueueBase+ulTemp+12) = (CARD32)(GR_NIL_CMD) ;

        ulTemp += 0x10 ;
    }
    else if( pXGI->Chipset == PCI_CHIP_XGIXG20 )
        ulTemp += 0x08 ;

    ulTemp &= pXGI->cmdQueueSizeMask ;
    Volari_UpdateHwWP(ulTemp) ;
}

void
Volari_DisableAccelerator(ScrnInfoPtr pScrn)
{
    XGIPtr pXGI = XGIPTR(pScrn) ;

    PDEBUG(ErrorF("Volari_DisableAccelerator(pScrn)\n")) ;

    Volari_Idle ;

    if( pXGI->TurboQueue )
    {
        Volari_DisableCmdQueue(pScrn) ;
    }

    andXGIIDXREG(XGISR, 0x1E, ~0xDA) ;
    PDEBUG(ErrorF("Volari_DisableAccelerator(pScrn) Done\n")) ;
}

static void
Volari_DisableCmdQueue(ScrnInfoPtr pScrn)
{
    XGIPtr pXGI = XGIPTR(pScrn) ;

    andXGIIDXREG(XGISR, 0x26, 0x0F) ;
}

static void
Volari_InitializeAccelerator(ScrnInfoPtr pScrn)
{
    XGIPtr pXGI = XGIPTR(pScrn);

    pXGI->DoColorExpand = FALSE;
}

Bool
Volari_AccelInit(ScreenPtr pScreen)
{
    XAAInfoRecPtr     infoPtr;
    ScrnInfoPtr       pScrn = xf86Screens[pScreen->myNum];
    XGIPtr            pXGI = XGIPTR(pScrn);
    int               reservedFbSize;
    long               UsableFbSize;
    unsigned char     *AvailBufBase;
    BoxRec            Avail;
    int               i; /*, divider;
    unsigned long     ulFbMgrSize;
    Bool              bRetValue ; */


    Avail.x1 = 0; Avail.y1 = 0; Avail.x2 = 0; Avail.y2 = 0;

    PDEBUG1(ErrorF("Volari_AccelInit()\n" )) ;

    pXGI->AccelInfoPtr = infoPtr = XAACreateInfoRec();
    if (!infoPtr)  return FALSE;

    Volari_InitializeAccelerator(pScrn);

    infoPtr->Flags = LINEAR_FRAMEBUFFER |
                     OFFSCREEN_PIXMAPS |
                     PIXMAP_CACHE;

    /* sync */
    infoPtr->Sync = Volari_Sync;

    if ((pScrn->bitsPerPixel != 8) &&
        (pScrn->bitsPerPixel != 16) &&
        (pScrn->bitsPerPixel != 32))
    {
        return FALSE;
    }

#ifdef XGIG2_SCR2SCRCOPY
    /* BitBlt */
    infoPtr->SetupForScreenToScreenCopy = Volari_SetupForScreenToScreenCopy;
    infoPtr->SubsequentScreenToScreenCopy = Volari_SubsequentScreenToScreenCopy;
    infoPtr->ScreenToScreenCopyFlags = NO_PLANEMASK | NO_TRANSPARENCY;
#endif

#ifdef XGIG2_SOLIDFILL
    /* solid fills */
    infoPtr->SetupForSolidFill = Volari_SetupForSolidFill;
    infoPtr->SubsequentSolidFillRect = Volari_SubsequentSolidFillRect;
    infoPtr->SolidFillFlags = NO_PLANEMASK;
#endif

#ifdef XGIG2_8X8MONOPATFILL
    /* 8x8 mono pattern fill */
    infoPtr->SetupForMono8x8PatternFill = Volari_SetupForMonoPatternFill;
    infoPtr->SubsequentMono8x8PatternFillRect = Volari_SubsequentMonoPatternFill;
    infoPtr->Mono8x8PatternFillFlags =
                                 NO_PLANEMASK |
                                 HARDWARE_PATTERN_SCREEN_ORIGIN |
                                 HARDWARE_PATTERN_PROGRAMMED_BITS |
                                 NO_TRANSPARENCY |
                                 BIT_ORDER_IN_BYTE_MSBFIRST ;
#endif /* XGIG2_8X8MONOPATFILL */

    /* init Frame Buffer Manager */
    reservedFbSize = 0;
    if (pXGI->TurboQueue)
    {
        reservedFbSize += pXGI->cmdQueueSize ;
    }

    if (pXGI->HWCursor)
    {
        reservedFbSize += XGI315_HCSIZE ;
    }

#ifdef XGIG2_COLOREXPSCANLN
    reservedFbSize += (pXGI->ColorExpandBufferNumber * pXGI->PerColorExpandBufferSize);
#endif

    UsableFbSize = pXGI->FbMapSize - reservedFbSize;
    AvailBufBase = pXGI->FbBase + UsableFbSize;

    for (i=0; i<pXGI->ColorExpandBufferNumber; i++)
    {
        pXGI->ColorExpandBufferAddr[i] = AvailBufBase + i*pXGI->PerColorExpandBufferSize;
        pXGI->ColorExpandBufferScreenOffset[i] = UsableFbSize + i*pXGI->PerColorExpandBufferSize;
    }

#ifdef XGIG2_IMAGEWRITE
    reservedFbSize += pXGI->ImageWriteBufferSize;
    UsableFbSize = pXGI->FbMapSize - reservedFbSize;
    pXGI->ImageWriteBufferAddr = AvailBufBase = pXGI->FbBase + UsableFbSize;
    infoPtr->ImageWriteRange = pXGI->ImageWriteBufferAddr;
#endif /* XGIG2_IMAGEWRITE */

    Avail.x1 = 0;
    Avail.y1 = 0;

/*
    Avail.x2 = pScrn->displayWidth;

    ErrorF("FbDevExist=%s\n",XGI_FbDevExist?"TRUE":"FALSE");

    if (XGI_FbDevExist)
    {
        if( UsableFbSize >= 8*1024*1024 )
        {
            UsableFbSize = 8*1024*1024 ;
        }
        else
        {
            UsableFbSize = 4*1024*1024 ;
        }
    }

    PDEBUG1(ErrorF( "UsabelFbSize = %08lx\n", UsableFbSize )) ;
    Avail.y2 = UsableFbSize / pXGI->scrnOffset ;

    if ((unsigned long)Avail.y2 > 8192)
    {
        Avail.y2 = 8192 ;
    }
*/

    UsableFbSize = pXGI->CursorOffset ;
    Avail.x1 = 0 ;
    Avail.y1 = 0 ;
    Avail.x2 = pScrn->displayWidth;
    Avail.y2 = UsableFbSize / pXGI->scrnOffset ;


    if ((unsigned long)Avail.y2 > 8192)
    {
        Avail.y2 = 8192 ;
    }


    xf86DrvMsg(pScrn->scrnIndex, X_INFO,
               "Usable FBSize = %08lx\n", UsableFbSize ) ;


    xf86DrvMsg(pScrn->scrnIndex, X_INFO,
               "Frame Buffer From (%d,%d) To (%d,%d)\n",
               Avail.x1, Avail.y1, Avail.x2, Avail.y2);

    xf86InitFBManager(pScreen, &Avail);

    return(XAAInit(pScreen, infoPtr));

}

void
Volari_Sync(ScrnInfoPtr pScrn)
{
        XGIPtr pXGI = XGIPTR(pScrn);

        PDEBUG1(ErrorF("Volari_Sync()\n"));
        pXGI->DoColorExpand = FALSE;
        Volari_Idle ;
}

static int xgiG2_ALUConv[] =
{
    0x00,       /* dest = 0;            0,      GXclear,        0 */
    0x88,       /* dest &= src;         DSa,    GXand,          0x1 */
    0x44,       /* dest = src & ~dest;  SDna,   GXandReverse,   0x2 */
    0xCC,       /* dest = src;          S,      GXcopy,         0x3 */
    0x22,       /* dest &= ~src;        DSna,   GXandInverted,  0x4 */
    0xAA,       /* dest = dest;         D,      GXnoop,         0x5 */
    0x66,       /* dest = ^src;         DSx,    GXxor,          0x6 */
    0xEE,       /* dest |= src;         DSo,    GXor,           0x7 */
    0x11,       /* dest = ~src & ~dest; DSon,   GXnor,          0x8 */
    0x99,       /* dest ^= ~src ;       DSxn,   GXequiv,        0x9 */
    0x55,       /* dest = ~dest;        Dn,     GXInvert,       0xA */
    0xDD,       /* dest = src|~dest ;   SDno,   GXorReverse,    0xB */
    0x33,       /* dest = ~src;         Sn,     GXcopyInverted, 0xC */
    0xBB,       /* dest |= ~src;        DSno,   GXorInverted,   0xD */
    0x77,       /* dest = ~src|~dest;   DSan,   GXnand,         0xE */
    0xFF,       /* dest = 0xFF;         1,      GXset,          0xF */
};
/* same ROP but with Pattern as Source */
static int xgiG2_PatALUConv[] =
{
    0x00,       /* dest = 0;            0,      GXclear,        0 */
    0xA0,       /* dest &= src;         DPa,    GXand,          0x1 */
    0x50,       /* dest = src & ~dest;  PDna,   GXandReverse,   0x2 */
    0xF0,       /* dest = src;          P,      GXcopy,         0x3 */
    0x0A,       /* dest &= ~src;        DPna,   GXandInverted,  0x4 */
    0xAA,       /* dest = dest;         D,      GXnoop,         0x5 */
    0x5A,       /* dest = ^src;         DPx,    GXxor,          0x6 */
    0xFA,       /* dest |= src;         DPo,    GXor,           0x7 */
    0x05,       /* dest = ~src & ~dest; DPon,   GXnor,          0x8 */
    0xA5,       /* dest ^= ~src ;       DPxn,   GXequiv,        0x9 */
    0x55,       /* dest = ~dest;        Dn,     GXInvert,       0xA */
    0xF5,       /* dest = src|~dest ;   PDno,   GXorReverse,    0xB */
    0x0F,       /* dest = ~src;         Pn,     GXcopyInverted, 0xC */
    0xAF,       /* dest |= ~src;        DPno,   GXorInverted,   0xD */
    0x5F,       /* dest = ~src|~dest;   DPan,   GXnand,         0xE */
    0xFF,       /* dest = 0xFF;         1,      GXset,          0xF */
};

static void
Volari_SetupForScreenToScreenCopy(
    ScrnInfoPtr pScrn,
    int xdir, int ydir, int rop,
    unsigned int planemask, int trans_color)
{
    XGIPtr  pXGI = XGIPTR(pScrn);
#ifdef SHOW_XAAINFO
    XAAInfoRecPtr   pXAA = XAAPTR(pScrn);
/*
    ErrorF("XAAInfoPtr->UsingPixmapCache = %s\n"
           "XAAInfoPtr->CanDoMono8x8 = %s\n"
           "XAAInfoPtr->CanDoColor8x8 = %s\n"
           "XAAInfoPtr->CachePixelGranularity = %d\n"
           "XAAInfoPtr->MaxCacheableTileWidth = %d\n"
           "XAAInfoPtr->MaxCacheableTileHeight = %d\n"
           "XAAInfoPtr->MaxCacheableStippleWidth = %d\n"
           "XAAInfoPtr->MaxCacheableStippleHeight = %d\n"
           "XAAInfoPtr->MonoPatternPitch = %d\n"
           "XAAInfoPtr->CacheWidthMono8x8Pattern = %d\n"
           "XAAInfoPtr->CacheHeightMono8x8Pattern = %d\n"
           "XAAInfoPtr->ColorPatternPitch = %d\n"
           "XAAInfoPtr->CacheWidthColor8x8Pattern = %d\n"
           "XAAInfoPtr->CacheHeightColor8x8Pattern = %d\n"
           "XAAInfoPtr->CacheColorExpandDensity = %d\n"
           "XAAInfoPtr->maxOffPixWidth = %d\n"
           "XAAInfoPtr->maxOffPixHeight= %d\n"
           "XAAInfoPtr->NeedToSync = %s\n"
           "\n",
           pXAA->UsingPixmapCache ? "True" : "False",
           pXAA->CanDoMono8x8 ? "True" : "False",
           pXAA->CanDoColor8x8 ? "True" : "False",
           pXAA->CachePixelGranularity,
           pXAA->MaxCacheableTileWidth,
           pXAA->MaxCacheableTileHeight,
           pXAA->MaxCacheableStippleWidth,
           pXAA->MaxCacheableStippleHeight,
           pXAA->MonoPatternPitch,
           pXAA->CacheWidthMono8x8Pattern,
           pXAA->CacheHeightMono8x8Pattern,
           pXAA->ColorPatternPitch,
           pXAA->CacheWidthColor8x8Pattern,
           pXAA->CacheHeightColor8x8Pattern,
           pXAA->CacheColorExpandDensity,
           pXAA->maxOffPixWidth,
           pXAA->maxOffPixHeight,
           pXAA->NeedToSync ? "True" : "False");
*/
#endif

    PDEBUG1(ErrorF("Setup ScreenCopy(%d, %d, 0x%x, 0x%x, 0x%x)\n",
        xdir, ydir, rop, planemask, trans_color));

    Volari_ResetCmd ;
    GuardBand(0x20 * Alignment);
    Volari_SetupDSTColorDepth(pXGI->DstColor);
    Volari_SetupSRCPitch(pXGI->scrnOffset) ;
    Volari_SetupDSTRect(pXGI->scrnOffset, Dst_Hight) ;
    Volari_SetupROP(xgiG2_ALUConv[rop]) ;
}

static void
Volari_SubsequentScreenToScreenCopy(
    ScrnInfoPtr pScrn,
    int src_x, int src_y,
    int dst_x, int dst_y,
    int width, int height)
{
    XGIPtr  pXGI = XGIPTR(pScrn);
    long srcbase, dstbase;

    PDEBUG1(ErrorF("Subsequent ScreenCopy(%d,%d, %d,%d, %d,%d)\n",
                    src_x, src_y,
                    dst_x, dst_y,
                    width, height));

    srcbase=dstbase=0;
    if (src_y >= 2048)
    {
        srcbase=pXGI->scrnOffset*src_y;
        src_y=0;
    }
    if (dst_y >= pScrn->virtualY)
    {
        dstbase=pXGI->scrnOffset*dst_y;
        dst_y=0;
    }
    PDEBUG1(ErrorF("SrcBase = %08lX DstBase = %08lX\n",srcbase,dstbase)) ;
    PDEBUG1(ErrorF("SrcX = %08lX SrcY = %08lX\n",src_x,src_y)) ;
    PDEBUG1(ErrorF("DstX = %08lX DstY = %08lX\n",dst_x,dst_y)) ;

    GuardBand(0x30 * Alignment);
    Volari_SetupSRCBase(srcbase);
    Volari_SetupDSTBase(dstbase);
    Volari_SetupSRCXY(src_x,src_y) ;
    Volari_SetupDSTXY(dst_x,dst_y) ;
    Volari_SetupRect(width, height) ;
    Volari_DoCMD ;
}

static void
Volari_SetupForSolidFill(ScrnInfoPtr pScrn,
                        int color, int rop, unsigned int planemask)
{
    XGIPtr  pXGI = XGIPTR(pScrn);

    PDEBUG1(ErrorF("Volari_SetupForSolidFill()\n")) ;
    PDEBUG1(ErrorF("Color = #%08lX ",color)) ;
    PDEBUG1(ErrorF("DstPitch = #%04lX ",(pXGI->scrnOffset))) ;
    PDEBUG1(ErrorF("\n")) ;

    Volari_ResetCmd ;
    GuardBand(0x28 * Alignment);
    Volari_SetupPATFG(color) ;
    Volari_SetupDSTRect(pXGI->scrnOffset, Dst_Hight) ;
    Volari_SetupDSTColorDepth(XGIPTR(pScrn)->DstColor) ;
    Volari_SetupROP(xgiG2_PatALUConv[rop]) ;
    Volari_SetupCMDFlag(PATFG | BITBLT) ;
}

static void
Volari_SubsequentSolidFillRect(
    ScrnInfoPtr pScrn,
    int x, int y,
    int width, int height)
{
    XGIPtr  pXGI = XGIPTR(pScrn);
    unsigned long dstbase = 0 ;

    PDEBUG1(ErrorF("Subsequent SolidFillRect(%d, %d, %d, %d)\n",
                    x, y, width, height));

    dstbase=0;
    if (y>=2048)
    {
        dstbase=pXGI->scrnOffset*y;
        y=0;
    }

    GuardBand(0x20 * Alignment);
    Volari_SetupDSTBase(dstbase) ;
    Volari_SetupDSTXY(x,y) ;
    Volari_SetupRect(width,height) ;
    Volari_DoCMD ;

}

static void
Volari_SetupForMonoPatternFill(ScrnInfoPtr pScrn,
                              int pat0, int pat1,
                              int fg, int bg,
                              int rop, unsigned int planemask)
{
    XGIPtr  pXGI = XGIPTR(pScrn);

    PDEBUG1(ErrorF("Setup MonoPatFill(0x%x,0x%x, 0x%x,0x%x, 0x%x, 0x%x)\n",
                    pat0, pat1, fg, bg, rop, planemask));

    Volari_ResetCmd ;
    GuardBand(0x40 * Alignment);
    Volari_SetupDSTRect(pXGI->scrnOffset, Dst_Hight) ;
    Volari_SetupMONOPAT0(pat0) ;
    Volari_SetupMONOPAT1(pat1) ;
    Volari_SetupPATFG(fg) ;
    Volari_SetupPATBG(bg) ;
    Volari_SetupROP(xgiG2_PatALUConv[rop]) ;
    Volari_SetupDSTColorDepth(pXGI->DstColor) ;
    Volari_SetupCMDFlag(PATMONO | BITBLT) ;
}

static void
Volari_SubsequentMonoPatternFill(ScrnInfoPtr pScrn,
                                int patx, int paty,
                                int x, int y, int w, int h)
{
    XGIPtr  pXGI = XGIPTR(pScrn);
    long dstbase;

    PDEBUG1(ErrorF("Subsequent MonoPatFill(0x%x,0x%x, %d,%d, %d,%d)\n",
                               patx, paty, x, y, w, h));
    dstbase=0;
    if (y>=2048)
    {
        dstbase=pXGI->scrnOffset*y;
        y=0;
    }

    GuardBand(0x20 * Alignment);
    Volari_SetupDSTBase(dstbase) ;
    Volari_SetupDSTXY(x,y) ;
    Volari_SetupRect(w,h) ;
    Volari_DoCMD ;
  Volari_Idle;
}

/************************************************************************/

