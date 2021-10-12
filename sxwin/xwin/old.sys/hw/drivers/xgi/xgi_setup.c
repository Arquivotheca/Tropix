/* $XFree86: xc/programs/Xserver/hw/xfree86/drivers/xgi/xgi_setup.c,v 1.4 2005/10/14 15:16:49 tsi Exp $ */
/*
 * Basic hardware and memory detection
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
 * Author:  	Thomas Winischhofer <thomas@winischhofer.net>
 *
 * Ideas and methods for old series based on code by Can-Ru Yeou, XGI Inc.
 *
 */
#include "xf86PciInfo.h"
#include "xf86Pci.h"
#include "xf86.h"
#include "fb.h"
#include "xf86_OSproc.h"
#include "xf86Resources.h"
#include "xf86_ansic.h"
#include "xf86Version.h"

#include "xf86cmap.h"

#include "xgi.h"
#include "regs.h"
#include "xgi_dac.h"
/* #include "valid_mode.h" */

#define _XF86DGA_SERVER_
#include <X11/extensions/xf86dgastr.h>

#include "globals.h"
#define DPMS_SERVER
#include <X11/extensions/dpms.h>

extern  int  XGI_FbDevExist;
/*
static const char *dramTypeStr[] = {
        "Fast Page DRAM",
        "2 cycle EDO RAM",
        "1 cycle EDO RAM",
        "SDRAM/SGRAM",
        "SDR SDRAM",
        "SGRAM",
        "ESDRAM",
	"DDR SDRAM",  
	"DDR SDRAM",  
	"VCM"	      
        "" };
*/

static BOOLEAN
bAccessNBridgePCIInfo(PXGI_HW_DEVICE_INFO pHwDevInfo, ULONG ulOffset, ULONG ulSet, ULONG *pulValue);
static BOOLEAN
bAccessVGAPCIInfo(PXGI_HW_DEVICE_INFO pHwDevInfo, ULONG ulOffset, ULONG ulSet, ULONG *pulValue);

static void
xgiXG40_Setup(ScrnInfoPtr pScrn)
{

/*********************************************************************
 * Setup
 * Decide the following item of execution data:
 *
 * pXGI->BusWidth
 * pXGI->videoRam (with KB unit)
 * pXGI->CursorOffset (with Byte Unit)
 * pXGI->cmdQueueSize (with Byte Unit)
 * pXGI->cmdQueueSizeMask (with Byte Unit)
 * pXGI->cmdQueueOffset (with Byte Unit)
 * pXGI->cmdQueueLen = 0 ; // init value
 * pXGI->cmdQueueLenMin = 0x200 ; // init value
 * pXGI->cmdQueueLenMax = pXGI->cmdQueueSize -  pXGI->cmdQueueLenMin ;
 *********************************************************************/

    XGIPtr        pXGI = XGIPTR(pScrn);
    unsigned int  ulMemConfig = 0;
    unsigned long ulMemSize   = 0;
    unsigned long ulDramType  = 0;
    char *dramTypeStr ;

    PDEBUG4(ErrorF("xgiXG40_Setup()\n")) ;

    dramTypeStr = "" ;

    pXGI->MemClock = XG40Mclk(pXGI);

    /*********************************************************************************************************
     * SR14 DRAM Size Register
     *     Default value: XXh
     *     D[7:4]    Memory size per channel {BChMemSize}
     *         0011: 8 MB
     *         0100: 16 MB
     *         0101: 32 MB
     *         0110: 64 MB
     *         0111: 128 MB
     *         1000: 256MB
     *        others: reserved
     *     D[3:2]    Number of  dram channels [1:0] {BChNum}
     *         00: uni-channel
     *         01: reserved
     *         10: dual-channel.
     *         11: quad-channel
     *     D1  Data width per channel selection {BDataWidth}
     *         0: 32-bits
     *         1: 64-bits
     *     D0  Dram channel mapping {BReverseChMapping}
     *         0: Normal mapping
     *         1: Reversal mapping
     *             Dual-channel: Logical channel A/B to physical channel B/A
     *             Quad-channel: Logical  channel A/B/C/D to physical channel C/D/A/B
     *
     *********************************************************************************************************/

    outXGIIDXREG(XGISR, 0x5, 0x86) ;
    inXGIIDXREG(XGISR, 0x14, ulMemConfig) ;
    inXGIIDXREG(XGISR, 0x3A, ulDramType) ;

    PDEBUG(ErrorF("xg40_Setup(): ulMemConfig = %02X\n",ulMemConfig)) ;
    PDEBUG(ErrorF("xg40_Setup(): ulDramType = %02X\n",ulDramType)) ;
    (void)ulDramType;

    pXGI->BusWidth = (ulMemConfig & (1<<1) )?64:32 ;

    switch(ulMemConfig>>4)
    {
    case 8:
        ulMemSize = 256*1024 ;
        break ;
    case 7:
        ulMemSize = 128*1024 ;
        break ;
    case 6:
        ulMemSize = 64*1024 ;
        break ;
    case 5:
        ulMemSize = 32*1024 ;
        break ;
    case 4:
        ulMemSize = 16*1024 ;
        break ;
    case 3:
        ulMemSize = 8*1024 ;
        break ;
    default:
        ulMemSize = 8*1024 ;
    }

    if( pXGI->Chipset == PCI_CHIP_XGIXG40)
    {
        if ( (pciReadLong(pXGI->PciTag, 0x08) & 0xFF ) == 2 )
        {
            switch((ulMemConfig>>2)&0x1)
            {
            case 0:
                /* Uni channel */
                ulMemSize *= 1 ;
       	        break ;
            case 1:
                /* Dual channel */
                ulMemSize *= 2 ;
    	        break ;
            }
        }
        else
        {
            switch((ulMemConfig>>2)&0x3)
            {
            case 2:
                /* Dual channel */
                ulMemSize *= 2 ;
        	break ;
            case 3:
                /* Quad channel */
                ulMemSize *= 4 ;
    	        break ;
           }
        }
    }

    pScrn->videoRam = ulMemSize ;

    /*********************************************************************************************************
     * SR15 DRAM Address Mapping Register
     * Default value: XXh
     *     D7  Channel  interleaving configuration { BChConfig }
     *         0: Divide the whole memory  into 2/4 equal-sized regions , each mapped to one channel
     *         1: Divide the whole memory into 2 regions according to BTilingSize[1:0] . The low-address region
     *         will be channel-interleaved as per BFineGranSize; the high-address region will be channel-
     *         interleaved  as per BCoarseGranSize[1:0]
     *     D[6:5]    Memory size of tile-mapped region {BTilingSize}
     *         00: 4 MB
     *         01: 8 MB
     *         10: 16 MB
     *         11: 32 MB
     *         The following bits are effective only when D7=1
     *     D4  Channel-interleaving granularity for tile-mapped region {BFineGranSize}
     *         0:  64 B
     *         1:  128 B
     *     D[3:2] Channel-interleaving granularity for linearly mapped region {BCoarseGranSize}
     *         00: 1KB
     *         01: 2KB
     *         10: 4KB
     *         11: 1MB
     *     D[1:0] reserved
     *********************************************************************************************************/

    /* Accelerator parameter Initialization */
    if( pXGI->Chipset == PCI_CHIP_XGIXG20 )
    {
        pXGI->cmdQueueSize = VOLARI_CQSIZEXG20;
  /*      XgiMode = XG20_Mode ;   */
        PDEBUG(ErrorF(" ---XG20_Mode \n"));
    }
    else
    {
        pXGI->cmdQueueSize = VOLARI_CQSIZE;
  /*      XgiMode = XGI_Mode ;  */
        PDEBUG(ErrorF(" ---XGI_Mode \n"));
    }

    pXGI->cmdQueueSizeMask = pXGI->cmdQueueSize - 1 ;
    pXGI->pCQ_shareWritePort = &(pXGI->cmdQueue_shareWP_only2D);


    /*
     If XGI_FbDevExist, XFree86 driver use the 8MB only. The rest
     frame buffer is used by other AP.
     */

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

    pXGI->CursorOffset = pXGI->cmdQueueOffset - 64*1024 ;
    PDEBUG4(ErrorF("pScrn->videoRam = %08lX pXGI->cmdQueueSize = %08lX\n",
			pScrn->videoRam, pXGI->cmdQueueSize)) ;
    PDEBUG4(ErrorF("pXGI->cmdQueueOffset = %08lX pXGI->CursorOffset = %08lX\n",
		pXGI->cmdQueueOffset, pXGI->CursorOffset)) ;

    pXGI->cmdQueueLen = 0 ;
    pXGI->cmdQueueLenMin = 0x200 ;
    pXGI->cmdQueueLenMax = pXGI->cmdQueueSize -  pXGI->cmdQueueLenMin ;

    /*****************************************************************
     * Dual Chip support put here                                    *
     *****************************************************************/
    xf86DrvMsg(pScrn->scrnIndex, X_PROBED,
            "Detected DRAM type : %s\n", dramTypeStr);
    xf86DrvMsg(pScrn->scrnIndex, X_PROBED,
            "Detected memory clock : %3.3fMHz\n",
            pXGI->MemClock/1000.0);
    xf86DrvMsg(pScrn->scrnIndex, X_PROBED,
            "Detected VRAM bus width is %d\n", pXGI->BusWidth);
    xf86DrvMsg(pScrn->scrnIndex, X_PROBED,
            "Detected Cmd Queue size is %d KB\n", pXGI->cmdQueueSize / 1024);
    xf86DrvMsg(pScrn->scrnIndex, X_PROBED,
            "Detected Cmd Queue Offset is %d\n", pXGI->cmdQueueOffset ) ;
}

void
XGISetup(ScrnInfoPtr pScrn)
{
    XGIPtr pXGI = XGIPTR(pScrn);

    pXGI->Flags = 0;
    pXGI->VBFlags = 0;

    switch  (XGIPTR(pScrn)->Chipset)  {

    case    PCI_CHIP_XGIXG40:
    case    PCI_CHIP_XGIXG20:
      default:
        xgiXG40_Setup(pScrn) ;
        break ;
    }
}

Bool
XGI_InitHwDevInfo(ScrnInfoPtr pScrn)
{
    XGIPtr pXGI  ;
    PXGI_HW_DEVICE_INFO pHwDevInfo ;
    ULONG ulTemp ;
    int i ;

    pXGI = XGIPTR(pScrn ) ;
    pHwDevInfo = &pXGI->xgi_HwDevExt ;
    pXGI->pVBInfo = &(pXGI->VBInfo) ;

    pHwDevInfo->pDevice = pXGI ;
    pHwDevInfo->pjVirtualRomBase = pXGI->BIOS ;
    pHwDevInfo->pjCustomizedROMImage = NULL ;
    pHwDevInfo->pjVideoMemoryAddress = (UCHAR*)(pXGI->FbBase) ;
    PDEBUG(ErrorF("pXGI->FbBase = 0x%08lx\n",(ULONG)(pXGI->FbBase))) ;
    PDEBUG(ErrorF("pHwDevInfo->pjVideoMemoryAddress = 0x%08lx\n",(ULONG)(pHwDevInfo->pjVideoMemoryAddress))) ;
    pHwDevInfo->ulVideoMemorySize = pXGI->FbMapSize ;
/*    pHwDevInfo->pjIOAddress = (PUCHAR)((ULONG)(pXGI->RelIO) + 0x30) ; */
    pHwDevInfo->pjIOAddress = pXGI->RelIO + 0x30 ;
    pHwDevInfo->jChipType = XGI_VGALegacy ;


    switch( pXGI->Chipset ){
    case PCI_CHIP_XGIXG40:
        pHwDevInfo->jChipType = XG40 ;
        break ;
    case PCI_CHIP_XGIXG20:
        pHwDevInfo->jChipType = XG20 ;
        break ;
    default:
        pHwDevInfo->jChipType = XG40 ;
        break ;
    }

    ulTemp = pciReadLong(pXGI->PciTag, 0x08) ;
    pHwDevInfo->jChipRevision = (UCHAR)(ulTemp & 0xff) ;
    pHwDevInfo->ujVBChipID = VB_CHIP_UNKNOWN ;
    pHwDevInfo->ulExternalChip = 0 ;

    if( pXGI->VBFlags & VB_301 )
    {
        pHwDevInfo->ujVBChipID = VB_CHIP_301 ;
    }
    else if( pXGI->VBFlags & VB_302 )
    {
        pHwDevInfo->ujVBChipID = VB_CHIP_302 ;
    }
    else
    {
        if( pXGI->VBFlags & VB_LVDS )
        {
            pHwDevInfo->ulExternalChip |= 0x01 ;
        }

        if( pXGI->VBFlags & VB_CHRONTEL )
        {
            pHwDevInfo->ulExternalChip |= 0x02 ;
        }

    }

    pHwDevInfo->ulCRT2LCDType = LCD_1024x768 ;
    pHwDevInfo->bIntegratedMMEnabled = FALSE ;
    pHwDevInfo->bSkipDramSizing = TRUE ;

    pHwDevInfo->pSR = pXGI->SRList ;
    pHwDevInfo->pCR = pXGI->CRList ;
    pHwDevInfo->pQueryVGAConfigSpace = bAccessVGAPCIInfo ;
    pHwDevInfo->pQueryNorthBridgeSpace = bAccessNBridgePCIInfo ;

    for( i = 0 ; i < ExtRegSize ; i++ ){
        pHwDevInfo->pSR[i].jIdx = 0xFF ;
        pHwDevInfo->pSR[i].jVal = 0xFF ;
        pHwDevInfo->pCR[i].jIdx = 0xFF ;
        pHwDevInfo->pCR[i].jVal = 0xFF ;
    }


    for( i = 0 ; i < VBIOS_VER_MAX_LENGTH ; i++ ){
        pHwDevInfo -> szVBIOSVer[i] = '\0' ;
    }

    PDEBUG(ErrorF("pHwDevInfo->jChipType = %08lX done\n",pHwDevInfo->jChipType)) ;
    XGINew_InitVBIOSData(pHwDevInfo,pXGI->pVBInfo) ;
    PDEBUG(ErrorF("XGINew_InitVBIOSData(pHwDevInfo) done\n")) ;
    return TRUE ;
}

static BOOLEAN
bAccessVGAPCIInfo(PXGI_HW_DEVICE_INFO pHwDevInfo, ULONG ulOffset, ULONG ulSet, ULONG *pulValue)
{
    XGIPtr pXGI ;
    PCITAG pciDev ;

    if( (!pHwDevInfo) || (!pulValue) )
    {
        return FALSE ;
    }

    pXGI = (XGIPtr)pHwDevInfo->pDevice ;
    pciDev = pXGI->PciTag ;

    if( ulSet )
    {
        pciWriteLong(pciDev, ulOffset&0xFFFFFFFc, *pulValue ) ;
    }
    else
    {
        *pulValue = pciReadLong(pciDev, ulOffset&0xFFFFFFFc ) ;
    }

    return TRUE ;
}


static BOOLEAN
bAccessNBridgePCIInfo(PXGI_HW_DEVICE_INFO pHwDevInfo, ULONG ulOffset, ULONG ulSet, ULONG *pulValue)
{
    PCITAG pciDev = pciTag(0,0,0);

    if( (!pulValue) )
    {
        return FALSE ;
    }

    if( ulSet )
    {
        pciWriteLong(pciDev, ulOffset&0xFFFFFFFc, *pulValue ) ;
    }
    else
    {
        *pulValue = pciReadLong(pciDev, ulOffset&0xFFFFFFFc ) ;
    }

    return TRUE ;
}
