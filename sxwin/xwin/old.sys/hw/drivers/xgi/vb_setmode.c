/* $XFree86: xc/programs/Xserver/hw/xfree86/drivers/xgi/vb_setmode.c,v 1.2 2005/06/07 01:33:39 tsi Exp $ */

#include "osdef.h"

#ifdef TC
#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <dos.h>
#endif


#ifdef LINUX_XF86
#include "xf86.h"
#include "xf86PciInfo.h"
#include "xgi.h"
#include "xgi_regs.h"
#endif

#ifdef LINUX_KERNEL
#include <asm/io.h>
#include <linux/types.h>
#include <linux/version.h>
#include "XGIfb.h"
/*#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,5,0)
#include <video/XGIfb.h>
#else
#include <linux/XGIfb.h>
#endif*/
#endif

#ifdef WIN2000
#include <dderror.h>
#include <devioctl.h>
#include <miniport.h>
#include <ntddvdeo.h>
#include <video.h>

#include "xgiv.h"
#include "dd_i2c.h"
#include "tools.h"
#endif

#include "vb_def.h"
#include "vgatypes.h"
#include "vb_struct.h"
#include "vb_util.h"
#include "vb_table.h"
#include "vb_setmode.h"



#define  IndexMask 0xff
#ifndef XGI_MASK_DUAL_CHIP
#define XGI_MASK_DUAL_CHIP	  0x04  /* SR3A */
#endif


BOOLEAN  XGI_IsLCDDualLink(PVB_DEVICE_INFO pVBInfo);
BOOLEAN  XGI_BacklightByDrv(PVB_DEVICE_INFO pVBInfo);

BOOLEAN  XGI_IsLCDON(PVB_DEVICE_INFO pVBInfo);
BOOLEAN  XGI_DisableChISLCD(PVB_DEVICE_INFO pVBInfo);
BOOLEAN  XGI_EnableChISLCD(PVB_DEVICE_INFO pVBInfo);
BOOLEAN  XGI_AjustCRT2Rate(USHORT ModeNo,USHORT ModeIdIndex,USHORT RefreshRateTableIndex,USHORT *i, PVB_DEVICE_INFO pVBInfo);
UCHAR    XGI_GetModePtr( USHORT ModeNo,USHORT ModeIdIndex,PVB_DEVICE_INFO pVBInfo);
USHORT   XGI_GetOffset(USHORT ModeNo,USHORT ModeIdIndex,USHORT RefreshRateTableIndex,PXGI_HW_DEVICE_INFO HwDeviceExtension,PVB_DEVICE_INFO pVBInfo);
USHORT   XGI_GetResInfo(USHORT ModeNo,USHORT ModeIdIndex, PVB_DEVICE_INFO pVBInfo);
USHORT   XGI_GetColorDepth(USHORT ModeNo,USHORT ModeIdIndex,PVB_DEVICE_INFO pVBInfo);
USHORT   XGI_GetVGAHT2(PVB_DEVICE_INFO pVBInfo);
USHORT   XGI_GetVCLK2Ptr(USHORT ModeNo,USHORT ModeIdIndex,USHORT RefreshRateTableIndex,PXGI_HW_DEVICE_INFO HwDeviceExtension,PVB_DEVICE_INFO pVBInfo);
void     XGI_VBLongWait(PVB_DEVICE_INFO pVBInfo);
void     XGI_SaveCRT2Info(USHORT ModeNo, PVB_DEVICE_INFO pVBInfo);
void     XGI_GetCRT2Data(USHORT ModeNo,USHORT ModeIdIndex,USHORT RefreshRateTableIndex, PVB_DEVICE_INFO pVBInfo);
void     XGI_GetCRT2ResInfo(USHORT ModeNo,USHORT ModeIdIndex, PVB_DEVICE_INFO pVBInfo);
void     XGI_PreSetGroup1(USHORT ModeNo,USHORT ModeIdIndex, PXGI_HW_DEVICE_INFO HwDeviceExtension,USHORT RefreshRateTableIndex, PVB_DEVICE_INFO pVBInfo);
void     XGI_SetGroup1(USHORT ModeNo,USHORT ModeIdIndex, PXGI_HW_DEVICE_INFO HwDeviceExtension,USHORT RefreshRateTableIndex, PVB_DEVICE_INFO pVBInfo);
void     XGI_SetLockRegs(USHORT ModeNo,USHORT ModeIdIndex, PXGI_HW_DEVICE_INFO HwDeviceExtension,USHORT RefreshRateTableIndex, PVB_DEVICE_INFO pVBInfo);
void     XGI_SetLCDRegs(USHORT ModeNo,USHORT ModeIdIndex, PXGI_HW_DEVICE_INFO HwDeviceExtension,USHORT RefreshRateTableIndex, PVB_DEVICE_INFO pVBInfo);
void     XGI_SetGroup2(USHORT ModeNo,USHORT ModeIdIndex,USHORT RefreshRateTableIndex,PXGI_HW_DEVICE_INFO HwDeviceExtension, PVB_DEVICE_INFO pVBInfo);
void     XGI_SetGroup3(USHORT ModeNo,USHORT ModeIdIndex, PVB_DEVICE_INFO pVBInfo);
void     XGI_SetGroup4(USHORT ModeNo,USHORT ModeIdIndex,USHORT RefreshRateTableIndex,PXGI_HW_DEVICE_INFO HwDeviceExtension, PVB_DEVICE_INFO pVBInfo);
void     XGI_SetGroup5(USHORT ModeNo,USHORT ModeIdIndex, PVB_DEVICE_INFO pVBInfo);
void*    XGI_GetLcdPtr(USHORT BX,  USHORT ModeNo, USHORT ModeIdIndex, USHORT RefreshRateTableIndex, PVB_DEVICE_INFO pVBInfo);
void*    XGI_GetTVPtr(USHORT BX, USHORT ModeNo,USHORT ModeIdIndex,USHORT RefreshRateTableIndex, PVB_DEVICE_INFO pVBInfo);
void 	 XGI_FirePWDEnable(PVB_DEVICE_INFO pVBInfo);
void 	 XGI_EnableGatingCRT(PXGI_HW_DEVICE_INFO HwDeviceExtension, PVB_DEVICE_INFO pVBInfo);
void 	 XGI_DisableGatingCRT(PXGI_HW_DEVICE_INFO HwDeviceExtension, PVB_DEVICE_INFO pVBInfo);
void 	 XGI_SetPanelDelay(USHORT tempbl, PVB_DEVICE_INFO pVBInfo);
void 	 XGI_SetPanelPower(USHORT tempah,USHORT tempbl, PVB_DEVICE_INFO pVBInfo);
void 	 XGI_EnablePWD( PVB_DEVICE_INFO pVBInfo);
void 	 XGI_DisablePWD( PVB_DEVICE_INFO pVBInfo);
void     XGI_AutoThreshold( PVB_DEVICE_INFO pVBInfo);
void     XGI_SetTap4Regs( PVB_DEVICE_INFO pVBInfo);
static void 	 SetDualChipRegs(PXGI_HW_DEVICE_INFO, PVB_DEVICE_INFO pVBInfo);
void     XGI_SetCRT1Group(PXGI_HW_DEVICE_INFO HwDeviceExtension,USHORT ModeNo,USHORT ModeIdIndex,PVB_DEVICE_INFO pVBInfo);
void     XGI_SetSeqRegs(USHORT ModeNo,USHORT StandTableIndex,USHORT ModeIdIndex,PVB_DEVICE_INFO pVBInfo);
void     XGI_SetMiscRegs(USHORT StandTableIndex, PVB_DEVICE_INFO pVBInfo);
void     XGI_SetCRTCRegs(PXGI_HW_DEVICE_INFO HwDeviceExtension,USHORT StandTableIndex, PVB_DEVICE_INFO pVBInfo);
void     XGI_SetATTRegs(USHORT ModeNo,USHORT StandTableIndex,USHORT ModeIdIndex,PVB_DEVICE_INFO pVBInfo );
void     XGI_SetGRCRegs(USHORT StandTableIndex, PVB_DEVICE_INFO pVBInfo);
void     XGI_ClearExt1Regs(PVB_DEVICE_INFO pVBInfo);

void     XGI_SetSync(USHORT RefreshRateTableIndex,PVB_DEVICE_INFO pVBInfo);
void     XGI_SetCRT1CRTC(USHORT ModeNo,USHORT ModeIdIndex,USHORT RefreshRateTableIndex,PVB_DEVICE_INFO pVBInfo,PXGI_HW_DEVICE_INFO HwDeviceExtension);
void     XGI_SetCRT1Timing_H(PVB_DEVICE_INFO pVBInfo,PXGI_HW_DEVICE_INFO HwDeviceExtension);
void     XGI_SetCRT1Timing_V(USHORT ModeIdIndex,USHORT ModeNo,PVB_DEVICE_INFO pVBInfo);
void     XGI_SetCRT1DE(PXGI_HW_DEVICE_INFO HwDeviceExtension,USHORT ModeNo,USHORT ModeIdIndex,USHORT RefreshRateTableIndex,PVB_DEVICE_INFO pVBInfo);
void     XGI_SetCRT1VCLK(USHORT ModeNo,USHORT ModeIdIndex,PXGI_HW_DEVICE_INFO HwDeviceExtension,USHORT RefreshRateTableIndex, PVB_DEVICE_INFO pVBInfo);
void     XGI_SetCRT1FIFO(USHORT ModeNo,PXGI_HW_DEVICE_INFO HwDeviceExtension, PVB_DEVICE_INFO pVBInfo);
void     XGI_SetCRT1ModeRegs(PXGI_HW_DEVICE_INFO HwDeviceExtension,USHORT ModeNo,USHORT ModeIdIndex,USHORT RefreshRateTableIndex,PVB_DEVICE_INFO pVBInfo);
void     XGI_SetVCLKState(PXGI_HW_DEVICE_INFO HwDeviceExtension,USHORT ModeNo,USHORT RefreshRateTableIndex,PVB_DEVICE_INFO pVBInfo);

void     XGI_LoadDAC(USHORT ModeNo,USHORT ModeIdIndex,PVB_DEVICE_INFO pVBInfo);
void     XGI_WriteDAC(USHORT dl, USHORT ah, USHORT al, USHORT dh, PVB_DEVICE_INFO pVBInfo);
void     XGI_ClearBuffer(PXGI_HW_DEVICE_INFO HwDeviceExtension,USHORT ModeNo,PVB_DEVICE_INFO pVBInfo);
void     XGI_GetLVDSResInfo( USHORT ModeNo,USHORT ModeIdIndex,PVB_DEVICE_INFO  pVBInfo);
void     XGI_GetLVDSData(USHORT ModeNo,USHORT ModeIdIndex,USHORT RefreshRateTableIndex,PVB_DEVICE_INFO  pVBInfo);
void     XGI_ModCRT1Regs(USHORT ModeNo,USHORT ModeIdIndex,USHORT RefreshRateTableIndex,PXGI_HW_DEVICE_INFO HwDeviceExtension,PVB_DEVICE_INFO  pVBInfo);
void     XGI_SetLVDSRegs(USHORT ModeNo,USHORT ModeIdIndex,USHORT RefreshRateTableIndex,PVB_DEVICE_INFO  pVBInfo);
void     XGI_UpdateModeInfo(PXGI_HW_DEVICE_INFO HwDeviceExtension,PVB_DEVICE_INFO  pVBInfo);
void     XGI_SetCRT2ECLK( USHORT ModeNo,USHORT ModeIdIndex,USHORT RefreshRateTableIndex,PVB_DEVICE_INFO  pVBInfo);
void     XGI_GetLCDSync(USHORT* HSyncWidth, USHORT* VSyncWidth, PVB_DEVICE_INFO pVBInfo);
void     XGI_SetCRT2VCLK(USHORT ModeNo,USHORT ModeIdIndex,USHORT RefreshRateTableIndex, PVB_DEVICE_INFO pVBInfo);
void     XGI_OEM310Setting(USHORT ModeNo,USHORT ModeIdIndex, PVB_DEVICE_INFO pVBInfo);
void     XGI_SetDelayComp(PVB_DEVICE_INFO pVBInfo);
void     XGI_SetLCDCap(PVB_DEVICE_INFO pVBInfo);
void     XGI_SetLCDCap_A(USHORT tempcx,PVB_DEVICE_INFO pVBInfo);
void     XGI_SetLCDCap_B(USHORT tempcx,PVB_DEVICE_INFO pVBInfo);
static void     SetSpectrum(PVB_DEVICE_INFO pVBInfo);
void     XGI_SetAntiFlicker(USHORT ModeNo,USHORT ModeIdIndex, PVB_DEVICE_INFO pVBInfo);
void     XGI_SetEdgeEnhance(USHORT ModeNo,USHORT ModeIdIndex, PVB_DEVICE_INFO pVBInfo);
void     XGI_SetPhaseIncr(PVB_DEVICE_INFO pVBInfo);
void     XGI_SetYFilter(USHORT ModeNo,USHORT ModeIdIndex, PVB_DEVICE_INFO pVBInfo);
void     XGI_GetTVPtrIndex2(USHORT* tempbx,UCHAR* tempcl,UCHAR* tempch, PVB_DEVICE_INFO pVBInfo);
USHORT   XGI_GetTVPtrIndex(  PVB_DEVICE_INFO pVBInfo );
void     XGI_CloseCRTC(PXGI_HW_DEVICE_INFO, PVB_DEVICE_INFO pVBInfo);
void     XGI_OpenCRTC(PXGI_HW_DEVICE_INFO, PVB_DEVICE_INFO pVBInfo);
void     XGI_GetRAMDAC2DATA(USHORT ModeNo,USHORT ModeIdIndex,USHORT RefreshRateTableIndex, PVB_DEVICE_INFO pVBInfo);
void     XGINew_EnableCRT2(PVB_DEVICE_INFO pVBInfo);
void     XGINew_LCD_Wait_Time(UCHAR DelayTime, PVB_DEVICE_INFO pVBInfo);
void     XGI_GetLCDVCLKPtr(UCHAR* di_0,UCHAR *di_1, PVB_DEVICE_INFO pVBInfo);
UCHAR    XGI_GetVCLKPtr(USHORT RefreshRateTableIndex,USHORT ModeNo,USHORT ModeIdIndex, PVB_DEVICE_INFO pVBInfo);
void     XGI_GetVCLKLen(UCHAR tempal,UCHAR* di_0,UCHAR* di_1, PVB_DEVICE_INFO pVBInfo);
USHORT   XGI_GetLCDCapPtr(PVB_DEVICE_INFO pVBInfo);
USHORT   XGI_GetLCDCapPtr1(PVB_DEVICE_INFO pVBInfo);
XGI301C_Tap4TimingStruct* XGI_GetTap4Ptr(USHORT tempcx, PVB_DEVICE_INFO pVBInfo);




/* USHORT XGINew_flag_clearbuffer; 0: no clear frame buffer 1:clear frame buffer */





static USHORT XGINew_MDA_DAC[]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
               0x15,0x15,0x15,0x15,0x15,0x15,0x15,0x15,
               0x15,0x15,0x15,0x15,0x15,0x15,0x15,0x15,
               0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,
               0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
               0x15,0x15,0x15,0x15,0x15,0x15,0x15,0x15,
               0x15,0x15,0x15,0x15,0x15,0x15,0x15,0x15,
               0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F};

static USHORT XGINew_CGA_DAC[]={0x00,0x10,0x04,0x14,0x01,0x11,0x09,0x15,
               0x00,0x10,0x04,0x14,0x01,0x11,0x09,0x15,
               0x2A,0x3A,0x2E,0x3E,0x2B,0x3B,0x2F,0x3F,
               0x2A,0x3A,0x2E,0x3E,0x2B,0x3B,0x2F,0x3F,
               0x00,0x10,0x04,0x14,0x01,0x11,0x09,0x15,
               0x00,0x10,0x04,0x14,0x01,0x11,0x09,0x15,
               0x2A,0x3A,0x2E,0x3E,0x2B,0x3B,0x2F,0x3F,
               0x2A,0x3A,0x2E,0x3E,0x2B,0x3B,0x2F,0x3F};

static USHORT XGINew_EGA_DAC[]={0x00,0x10,0x04,0x14,0x01,0x11,0x05,0x15,
               0x20,0x30,0x24,0x34,0x21,0x31,0x25,0x35,
               0x08,0x18,0x0C,0x1C,0x09,0x19,0x0D,0x1D,
               0x28,0x38,0x2C,0x3C,0x29,0x39,0x2D,0x3D,
               0x02,0x12,0x06,0x16,0x03,0x13,0x07,0x17,
               0x22,0x32,0x26,0x36,0x23,0x33,0x27,0x37,
               0x0A,0x1A,0x0E,0x1E,0x0B,0x1B,0x0F,0x1F,
               0x2A,0x3A,0x2E,0x3E,0x2B,0x3B,0x2F,0x3F};

static USHORT XGINew_VGA_DAC[]={0x00,0x10,0x04,0x14,0x01,0x11,0x09,0x15,
               0x2A,0x3A,0x2E,0x3E,0x2B,0x3B,0x2F,0x3F,
               0x00,0x05,0x08,0x0B,0x0E,0x11,0x14,0x18,
               0x1C,0x20,0x24,0x28,0x2D,0x32,0x38,0x3F,

               0x00,0x10,0x1F,0x2F,0x3F,0x1F,0x27,0x2F,
               0x37,0x3F,0x2D,0x31,0x36,0x3A,0x3F,0x00,
               0x07,0x0E,0x15,0x1C,0x0E,0x11,0x15,0x18,
               0x1C,0x14,0x16,0x18,0x1A,0x1C,0x00,0x04,
               0x08,0x0C,0x10,0x08,0x0A,0x0C,0x0E,0x10,
               0x0B,0x0C,0x0D,0x0F,0x10};


/* --------------------------------------------------------------------- */
/* Function : XGI_InitTo330Pointer */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGI_InitTo330Pointer( UCHAR ChipType ,PVB_DEVICE_INFO pVBInfo)
{
    pVBInfo->SModeIDTable = (XGI_StStruct *) XGI330_SModeIDTable ;
    pVBInfo->StandTable = (XGI_StandTableStruct *) XGI330_StandTable ;
    pVBInfo->EModeIDTable = (XGI_ExtStruct *) XGI330_EModeIDTable ;
    pVBInfo->RefIndex = (XGI_Ext2Struct *) XGI330_RefIndex ;
    pVBInfo->XGINEWUB_CRT1Table = (XGI_CRT1TableStruct *) XGI_CRT1Table ;

    /* add for new UNIVGABIOS */
    /* XGINew_UBLCDDataTable = (XGI_LCDDataTablStruct *) XGI_LCDDataTable ; */
    /* XGINew_UBTVDataTable = (XGI_TVDataTablStruct *) XGI_TVDataTable ; */


    if ( ChipType >= XG40 )
    {
        pVBInfo->MCLKData = (XGI_MCLKDataStruct *) XGI340New_MCLKData ;
        pVBInfo->ECLKData = (XGI_ECLKDataStruct *) XGI340_ECLKData ;
    }
    else
    {
        pVBInfo->MCLKData = (XGI_MCLKDataStruct *) XGI330New_MCLKData ;
        pVBInfo->ECLKData = (XGI_ECLKDataStruct *) XGI330_ECLKData ;
    }

    pVBInfo->VCLKData = (XGI_VCLKDataStruct *) XGI_VCLKData ;
    pVBInfo->VBVCLKData = (XGI_VBVCLKDataStruct *) XGI_VBVCLKData ;
    pVBInfo->ScreenOffset = XGI330_ScreenOffset ;
    pVBInfo->StResInfo = (XGI_StResInfoStruct *) XGI330_StResInfo ;
    pVBInfo->ModeResInfo = (XGI_ModeResInfoStruct *) XGI330_ModeResInfo ;

    pVBInfo->pOutputSelect = &XGI330_OutputSelect ;
    pVBInfo->pSoftSetting = &XGI330_SoftSetting ;
    pVBInfo->pSR07 = &XGI330_SR07 ;
    pVBInfo->LCDResInfo = 0 ;
    pVBInfo->LCDTypeInfo = 0 ;
    pVBInfo->LCDInfo = 0 ;
    pVBInfo->VBInfo = 0 ;
    pVBInfo->TVInfo = 0;

 
    pVBInfo->SR15 = XGI340_SR13 ;
    pVBInfo->CR40 = XGI340_cr41 ;
    pVBInfo->SR25 = XGI330_sr25 ;
    pVBInfo->pSR31 = &XGI330_sr31 ;
    pVBInfo->pSR32 = &XGI330_sr32 ;
    pVBInfo->CR6B = XGI340_CR6B ;
    pVBInfo->CR6E = XGI340_CR6E ;
    pVBInfo->CR6F = XGI340_CR6F ;
    pVBInfo->CR89 = XGI340_CR89 ;
    pVBInfo->AGPReg = XGI340_AGPReg ;
    pVBInfo->SR16 = XGI340_SR16 ;
    pVBInfo->pCRCF = &XG40_CRCF ;
    pVBInfo->pXGINew_DRAMTypeDefinition = &XG40_DRAMTypeDefinition ;
 

    pVBInfo->CR49 = XGI330_CR49 ;
    pVBInfo->pSR1F = &XGI330_SR1F ;
    pVBInfo->pSR21 = &XGI330_SR21 ;
    pVBInfo->pSR22 = &XGI330_SR22 ;
    pVBInfo->pSR23 = &XGI330_SR23 ;
    pVBInfo->pSR24 = &XGI330_SR24 ;
    pVBInfo->pSR33 = &XGI330_SR33 ;



    pVBInfo->pCRT2Data_1_2 = &XGI330_CRT2Data_1_2 ;
    pVBInfo->pCRT2Data_4_D = &XGI330_CRT2Data_4_D ;
    pVBInfo->pCRT2Data_4_E = &XGI330_CRT2Data_4_E ;
    pVBInfo->pCRT2Data_4_10 = &XGI330_CRT2Data_4_10 ;
    pVBInfo->pRGBSenseData = &XGI330_RGBSenseData ;
    pVBInfo->pVideoSenseData = &XGI330_VideoSenseData ;
    pVBInfo->pYCSenseData = &XGI330_YCSenseData ;
    pVBInfo->pRGBSenseData2 = &XGI330_RGBSenseData2 ;
    pVBInfo->pVideoSenseData2 = &XGI330_VideoSenseData2 ;
    pVBInfo->pYCSenseData2 = &XGI330_YCSenseData2 ;

    pVBInfo->NTSCTiming = XGI330_NTSCTiming ;
    pVBInfo->PALTiming = XGI330_PALTiming ;
    pVBInfo->HiTVExtTiming = XGI330_HiTVExtTiming ;
    pVBInfo->HiTVSt1Timing = XGI330_HiTVSt1Timing ;
    pVBInfo->HiTVSt2Timing = XGI330_HiTVSt2Timing ;
    pVBInfo->HiTVTextTiming = XGI330_HiTVTextTiming ;
    pVBInfo->YPbPr750pTiming = XGI330_YPbPr750pTiming ;
    pVBInfo->YPbPr525pTiming = XGI330_YPbPr525pTiming ;
    pVBInfo->YPbPr525iTiming = XGI330_YPbPr525iTiming ;
    pVBInfo->HiTVGroup3Data = XGI330_HiTVGroup3Data ;
    pVBInfo->HiTVGroup3Simu = XGI330_HiTVGroup3Simu ;
    pVBInfo->HiTVGroup3Text = XGI330_HiTVGroup3Text ;
    pVBInfo->Ren525pGroup3 = XGI330_Ren525pGroup3 ;
    pVBInfo->Ren750pGroup3 = XGI330_Ren750pGroup3 ;


    pVBInfo->TimingH = (XGI_TimingHStruct *) XGI_TimingH ;
    pVBInfo->TimingV = (XGI_TimingVStruct *) XGI_TimingV ;

    pVBInfo->CHTVVCLKUNTSC = XGI330_CHTVVCLKUNTSC ;
    pVBInfo->CHTVVCLKONTSC = XGI330_CHTVVCLKONTSC ;
    pVBInfo->CHTVVCLKUPAL = XGI330_CHTVVCLKUPAL ;
    pVBInfo->CHTVVCLKOPAL = XGI330_CHTVVCLKOPAL ;

    /* 310 customization related */
    if ( ( pVBInfo->VBType & VB_XGI301LV ) || ( pVBInfo->VBType & VB_XGI302LV ) )
        pVBInfo->LCDCapList = XGI_LCDDLCapList ;
    else
        pVBInfo->LCDCapList = XGI_LCDCapList ;

    pVBInfo->XGI_TVDelayList = XGI301TVDelayList ;
    pVBInfo->XGI_TVDelayList2 = XGI301TVDelayList2 ;


    pVBInfo->pXGINew_I2CDefinition = &XG40_I2CDefinition ;

    if ( ChipType == XG20 )
    	pVBInfo->pXGINew_CR97 = &XG20_CR97 ;
}






/* --------------------------------------------------------------------- */
/* Function : XGISetModeNew */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
BOOLEAN XGISetModeNew( PXGI_HW_DEVICE_INFO HwDeviceExtension , USHORT ModeNo )
{
/*    ULONG  temp ;  */
    USHORT ModeIdIndex ;
        /* PUCHAR pVBInfo->FBAddr = HwDeviceExtension->pjVideoMemoryAddress ; */
    VB_DEVICE_INFO VBINF;
    PVB_DEVICE_INFO pVBInfo = &VBINF;
    pVBInfo->ROMAddr = HwDeviceExtension->pjVirtualRomBase ;
    pVBInfo->BaseAddr = ( USHORT )HwDeviceExtension->pjIOAddress ;
    pVBInfo->IF_DEF_LVDS = 0 ;
    pVBInfo->IF_DEF_CH7005 = 0 ;
    pVBInfo->IF_DEF_LCDA = 1 ;
    pVBInfo->IF_DEF_CH7017 = 0 ;
    pVBInfo->IF_DEF_VideoCapture = 0 ;
    pVBInfo->IF_DEF_ScaleLCD = 0 ;
    pVBInfo->IF_DEF_OEMUtil = 0 ;
    pVBInfo->IF_DEF_PWD = 0 ;


    if ( HwDeviceExtension->jChipType == XG20 )			/* kuku 2004/06/25 */
    {
    	pVBInfo->IF_DEF_YPbPr = 0 ;
        pVBInfo->IF_DEF_HiVision = 0 ;
        pVBInfo->IF_DEF_CRT2Monitor = 0 ;
    }
    else if ( HwDeviceExtension->jChipType >= XG40 )
    {
        pVBInfo->IF_DEF_YPbPr = 1 ;
        pVBInfo->IF_DEF_HiVision = 1 ;
        pVBInfo->IF_DEF_CRT2Monitor = 1 ;
    }
    else
    {
        pVBInfo->IF_DEF_YPbPr = 1 ;
        pVBInfo->IF_DEF_HiVision = 1 ;
        pVBInfo->IF_DEF_CRT2Monitor = 0 ;
    }

    pVBInfo->P3c4 = pVBInfo->BaseAddr + 0x14 ;
    pVBInfo->P3d4 = pVBInfo->BaseAddr + 0x24 ;
    pVBInfo->P3c0 = pVBInfo->BaseAddr + 0x10 ;
    pVBInfo->P3ce = pVBInfo->BaseAddr + 0x1e ;
    pVBInfo->P3c2 = pVBInfo->BaseAddr + 0x12 ;
    pVBInfo->P3cc = pVBInfo->BaseAddr + 0x1C ;
    pVBInfo->P3ca = pVBInfo->BaseAddr + 0x1a ;
    pVBInfo->P3c6 = pVBInfo->BaseAddr + 0x16 ;
    pVBInfo->P3c7 = pVBInfo->BaseAddr + 0x17 ;
    pVBInfo->P3c8 = pVBInfo->BaseAddr + 0x18 ;
    pVBInfo->P3c9 = pVBInfo->BaseAddr + 0x19 ;
    pVBInfo->P3da = pVBInfo->BaseAddr + 0x2A ;
    pVBInfo->Part0Port = pVBInfo->BaseAddr + XGI_CRT2_PORT_00 ;
    pVBInfo->Part1Port = pVBInfo->BaseAddr + XGI_CRT2_PORT_04 ;
    pVBInfo->Part2Port = pVBInfo->BaseAddr + XGI_CRT2_PORT_10 ;
    pVBInfo->Part3Port = pVBInfo->BaseAddr + XGI_CRT2_PORT_12 ;
    pVBInfo->Part4Port = pVBInfo->BaseAddr + XGI_CRT2_PORT_14 ;
    pVBInfo->Part5Port = pVBInfo->BaseAddr + XGI_CRT2_PORT_14 + 2 ;

    if ( HwDeviceExtension->jChipType != XG20 )			/* kuku 2004/06/25 */
    XGI_GetVBType( pVBInfo ) ;

    XGI_InitTo330Pointer( HwDeviceExtension->jChipType, pVBInfo ) ;

    if ( ModeNo & 0x80 )
    {
        ModeNo = ModeNo & 0x7F ;
/* XGINew_flag_clearbuffer = 0 ; */
    }
/*    else
    {
        XGINew_flag_clearbuffer = 1 ;
    }
*/
    XGINew_SetReg1( pVBInfo->P3c4 , 0x05 , 0x86 ) ;

    if ( HwDeviceExtension->jChipType != XG20 )			/* kuku 2004/06/25 1.Openkey */
    XGI_UnLockCRT2( HwDeviceExtension , pVBInfo ) ;

    XGI_SearchModeID( ModeNo , &ModeIdIndex, pVBInfo ) ;

    XGI_GetVGAType(HwDeviceExtension,  pVBInfo) ;
    if ( HwDeviceExtension->jChipType != XG20 )			/* kuku 2004/06/25 */
    {
    XGI_GetVBInfo(ModeNo , ModeIdIndex , HwDeviceExtension, pVBInfo ) ;
    XGI_GetTVInfo(ModeNo , ModeIdIndex, pVBInfo ) ;
    XGI_GetLCDInfo(ModeNo , ModeIdIndex, pVBInfo ) ;
    XGI_DisableBridge( HwDeviceExtension,pVBInfo ) ;
/*    XGI_OpenCRTC( HwDeviceExtension, pVBInfo ) ; */

    if ( pVBInfo->VBInfo & ( SetSimuScanMode | SetCRT2ToLCDA ) )
    {
        XGI_SetCRT1Group(HwDeviceExtension , ModeNo , ModeIdIndex, pVBInfo ) ;

        if ( pVBInfo->VBInfo & SetCRT2ToLCDA )
        {
            XGI_SetLCDAGroup(ModeNo , ModeIdIndex , HwDeviceExtension, pVBInfo ) ;
        }
    }
    else
    {
        if ( !( pVBInfo->VBInfo & SwitchToCRT2) )
        {
            XGI_SetCRT1Group( HwDeviceExtension , ModeNo , ModeIdIndex, pVBInfo ) ;
            if ( pVBInfo->VBInfo & SetCRT2ToLCDA )
            {
                XGI_SetLCDAGroup( ModeNo , ModeIdIndex , HwDeviceExtension, pVBInfo ) ;
            }
        }
    }

    if ( pVBInfo->VBInfo & ( SetSimuScanMode | SwitchToCRT2 ) )
    {
        switch( HwDeviceExtension->ujVBChipID )
        {
            case VB_CHIP_301:
                XGI_SetCRT2Group301( ModeNo , HwDeviceExtension, pVBInfo ) ;		/*add for CRT2 */
                break ;

            case VB_CHIP_302:
                XGI_SetCRT2Group301(ModeNo , HwDeviceExtension, pVBInfo ) ;		/*add for CRT2 */
                break ;

            default:
                break ;
        }
    }

    XGI_SetCRT2ModeRegs( ModeNo, HwDeviceExtension,pVBInfo ) ;
    XGI_OEM310Setting( ModeNo, ModeIdIndex,pVBInfo ) ; /*0212*/
    XGI_CloseCRTC( HwDeviceExtension, pVBInfo ) ;
    XGI_EnableBridge( HwDeviceExtension ,pVBInfo) ;
    }	/* !XG20 */
    else
    {
        if ( ModeNo <= 0x13 )
        {
            pVBInfo->ModeType = pVBInfo->SModeIDTable[ ModeIdIndex ].St_ModeFlag & ModeInfoFlag;
        }
        else
        {
            pVBInfo->ModeType = pVBInfo->EModeIDTable[ ModeIdIndex ].Ext_ModeFlag & ModeInfoFlag;
        }
    	pVBInfo->SetFlag = 0 ;
    	pVBInfo->VBInfo = DisableCRT2Display ;
    	XGI_DisplayOff(pVBInfo) ;
    	XGI_SetCRT1Group(HwDeviceExtension , ModeNo , ModeIdIndex, pVBInfo ) ;
    	XGI_DisplayOn(pVBInfo) ;
    }

/*
    if ( ModeNo <= 0x13 )
    {
        modeflag = pVBInfo->SModeIDTable[ ModeIdIndex ].St_ModeFlag ;
    }
    else
    {
        modeflag = pVBInfo->EModeIDTable[ ModeIdIndex ].Ext_ModeFlag ;
    }
    pVBInfo->ModeType = modeflag&ModeInfoFlag ;
    pVBInfo->SetFlag = 0x00 ;
    pVBInfo->VBInfo = DisableCRT2Display ;
    temp = XGINew_CheckMemorySize(  HwDeviceExtension , ModeNo , ModeIdIndex, pVBInfo ) ;

    if ( temp == 0 )
        return( 0 ) ;

    XGI_DisplayOff( pVBInfo) ;
    XGI_SetCRT1Group( HwDeviceExtension , ModeNo , ModeIdIndex, pVBInfo ) ;
    XGI_DisplayOn( pVBInfo) ;
*/

    XGI_UpdateModeInfo( HwDeviceExtension, pVBInfo ) ;

    if ( HwDeviceExtension->jChipType != XG20 )			/* kuku 2004/06/25 */
    XGI_LockCRT2( HwDeviceExtension, pVBInfo ) ;

    return( TRUE ) ;
}


/* --------------------------------------------------------------------- */
/* Function : XGI_SetCRT1Group */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGI_SetCRT1Group( PXGI_HW_DEVICE_INFO HwDeviceExtension , USHORT ModeNo , USHORT ModeIdIndex, PVB_DEVICE_INFO pVBInfo )
{
    USHORT StandTableIndex ,
           RefreshRateTableIndex ,
           b3CC ,
           temp ;

    USHORT XGINew_P3cc =  pVBInfo->P3cc;
    	 /*  XGINew_P3c2 =  pVBInfo->P3c2; */

    /* XGINew_CRT1Mode = ModeNo ; // SaveModeID */
    StandTableIndex = XGI_GetModePtr( ModeNo , ModeIdIndex, pVBInfo ) ;
    /* XGI_SetBIOSData(ModeNo , ModeIdIndex ) ; */
    /* XGI_ClearBankRegs( ModeNo , ModeIdIndex ) ; */
    XGI_SetSeqRegs( ModeNo , StandTableIndex , ModeIdIndex, pVBInfo ) ;
    XGI_SetMiscRegs( StandTableIndex,  pVBInfo ) ;
    XGI_SetCRTCRegs( HwDeviceExtension , StandTableIndex,  pVBInfo) ;
    XGI_SetATTRegs( ModeNo , StandTableIndex , ModeIdIndex, pVBInfo ) ;
    XGI_SetGRCRegs(  StandTableIndex, pVBInfo ) ;
    XGI_ClearExt1Regs(pVBInfo) ;

/* if ( pVBInfo->IF_DEF_ExpLink ) */
/* XGI_SetDefaultVCLK() ; */

    temp = ~ProgrammingCRT2 ;
    pVBInfo->SetFlag &= temp ;
    pVBInfo->SelectCRT2Rate = 0 ;

    if ( pVBInfo->VBType & ( VB_XGI301B | VB_XGI302B | VB_XGI301LV | VB_XGI302LV | VB_XGI301C ) )
    {
        if ( pVBInfo->VBInfo & ( SetSimuScanMode | SetCRT2ToLCDA | SetInSlaveMode ) )
        {
            pVBInfo->SetFlag |= ProgrammingCRT2 ;
         }
    }

    RefreshRateTableIndex = XGI_GetRatePtrCRT2( ModeNo , ModeIdIndex, pVBInfo ) ;

    if ( RefreshRateTableIndex != 0xFFFF )
    {
        XGI_SetSync( RefreshRateTableIndex, pVBInfo ) ;
        XGI_SetCRT1CRTC( ModeNo , ModeIdIndex , RefreshRateTableIndex, pVBInfo, HwDeviceExtension ) ;
        XGI_SetCRT1DE( HwDeviceExtension , ModeNo , ModeIdIndex , RefreshRateTableIndex, pVBInfo ) ;
        XGI_SetCRT1Offset( ModeNo , ModeIdIndex , RefreshRateTableIndex , HwDeviceExtension, pVBInfo ) ;
        XGI_SetCRT1VCLK( ModeNo , ModeIdIndex , HwDeviceExtension , RefreshRateTableIndex, pVBInfo ) ;
    }

    if ( HwDeviceExtension->jChipType == XG20 )
    {
    	if ( ( ModeNo == 0x00 ) | (ModeNo == 0x01) )
    	{
    	    XGINew_SetReg1( pVBInfo->P3c4 , 0x2B , 0x4E) ;
    	    XGINew_SetReg1( pVBInfo->P3c4 , 0x2C , 0xE9) ;
    	    b3CC =(UCHAR) XGINew_GetReg2(XGINew_P3cc) ;
    	    XGINew_SetReg3(XGINew_P3cc ,  (b3CC |= 0x0C) ) ;
    	}
    	else if ( ( ModeNo == 0x04) | ( ModeNo == 0x05) | ( ModeNo == 0x0D) )
    	{
    	    XGINew_SetReg1( pVBInfo->P3c4 , 0x2B , 0x1B) ;
    	    XGINew_SetReg1( pVBInfo->P3c4 , 0x2C , 0xE3) ;
    	    b3CC = (UCHAR)XGINew_GetReg2(XGINew_P3cc) ;
    	    XGINew_SetReg3(XGINew_P3cc ,  (b3CC |= 0x0C) ) ;
    	}
    }

    pVBInfo->SetFlag &= ( ~ProgrammingCRT2 ) ;
    XGI_SetCRT1FIFO(  ModeNo , HwDeviceExtension,  pVBInfo ) ;
    XGI_SetCRT1ModeRegs(  HwDeviceExtension , ModeNo , ModeIdIndex , RefreshRateTableIndex , pVBInfo) ;

    if ( HwDeviceExtension->jChipType == XG40 )	/* Copy reg settings to 2nd chip */
    {
        if ( XGI_CheckDualChip( pVBInfo) )
            SetDualChipRegs( HwDeviceExtension, pVBInfo ) ;
    }

    /* XGI_LoadCharacter(); //dif ifdef TVFont */

    XGI_LoadDAC( ModeNo , ModeIdIndex, pVBInfo ) ;
    XGI_ClearBuffer( HwDeviceExtension , ModeNo, pVBInfo ) ;
}


/* --------------------------------------------------------------------- */
/* Function : XGI_GetModePtr */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
UCHAR XGI_GetModePtr( USHORT ModeNo , USHORT ModeIdIndex, PVB_DEVICE_INFO pVBInfo )
{
    UCHAR index ;

    if ( ModeNo <= 0x13 )
        index = pVBInfo->SModeIDTable[ ModeIdIndex ].St_StTableIndex ;
    else
    {
        if ( pVBInfo->ModeType <= 0x02 )
            index = 0x1B ;	/* 02 -> ModeEGA */
        else
            index = 0x0F ;
    }
    return( index ) ;		/* Get pVBInfo->StandTable index */
}


/* --------------------------------------------------------------------- */
/* Function : XGI_SetBIOSData */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
/*UCHAR XGI_SetBIOSData( USHORT ModeNo , USHORT ModeIdIndex )
{
    return( 0 ) ;
}
*/

/* --------------------------------------------------------------------- */
/* Function : XGI_ClearBankRegs */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
/*UCHAR XGI_ClearBankRegs( USHORT ModeNo , USHORT ModeIdIndex )
{
    return( 0 ) ;
}
*/

/* --------------------------------------------------------------------- */
/* Function : XGI_SetSeqRegs */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGI_SetSeqRegs(  USHORT ModeNo , USHORT StandTableIndex , USHORT ModeIdIndex, PVB_DEVICE_INFO pVBInfo )
{
    UCHAR tempah ,
          SRdata ;

    USHORT i ;

    XGINew_SetReg1( pVBInfo->P3c4 , 0x00 , 0x03 ) ;		/* Set SR0 */
    tempah=pVBInfo->StandTable[ StandTableIndex ].SR[ 0 ] ;

    i = SetCRT2ToLCDA ;
    if ( pVBInfo->VBInfo & SetCRT2ToLCDA )
    {
        tempah |= 0x01 ;
    }
    else
    {
        if ( pVBInfo->VBInfo & ( SetCRT2ToTV | SetCRT2ToLCD ) )
        {
            if ( pVBInfo->VBInfo & SetInSlaveMode )
                tempah |= 0x01 ;
        }
    }

    tempah |= 0x20 ;		/* screen off */
    XGINew_SetReg1( pVBInfo->P3c4 , 0x01 , tempah ) ;		/* Set SR1 */

    for( i = 02 ; i <= 04 ; i++ )
    {
        SRdata = pVBInfo->StandTable[ StandTableIndex ].SR[ i - 1 ] ;	/* Get SR2,3,4 from file */
        XGINew_SetReg1( pVBInfo->P3c4 , i , SRdata ) ;				/* Set SR2 3 4 */
    }
}


/* --------------------------------------------------------------------- */
/* Function : XGI_SetMiscRegs */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGI_SetMiscRegs( USHORT StandTableIndex, PVB_DEVICE_INFO pVBInfo )
{
    UCHAR Miscdata ;

    Miscdata = pVBInfo->StandTable[ StandTableIndex ].MISC ;	/* Get Misc from file */
/*
    if ( pVBInfo->VBType & ( VB_XGI301B | VB_XGI302B | VB_XGI301LV | VB_XGI302LV | VB_XGI301C ) )
    {
        if ( pVBInfo->VBInfo & SetCRT2ToLCDA )
        {
            Miscdata |= 0x0C ;
        }
    }
*/

    XGINew_SetReg3( pVBInfo->P3c2 , Miscdata ) ;		/* Set Misc(3c2) */
}


/* --------------------------------------------------------------------- */
/* Function : XGI_SetCRTCRegs */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGI_SetCRTCRegs( PXGI_HW_DEVICE_INFO HwDeviceExtension , USHORT StandTableIndex, PVB_DEVICE_INFO pVBInfo )
{
    UCHAR CRTCdata ;
    USHORT i ;

    CRTCdata = ( UCHAR )XGINew_GetReg1( pVBInfo->P3d4 , 0x11 ) ;
    CRTCdata &= 0x7f ;
    XGINew_SetReg1( pVBInfo->P3d4 , 0x11 , CRTCdata ) ;		/* Unlock CRTC */

    for( i = 0 ; i <= 0x18 ; i++ )
    {
        CRTCdata = pVBInfo->StandTable[ StandTableIndex ].CRTC[ i ] ;	/* Get CRTC from file */
        XGINew_SetReg1( pVBInfo->P3d4 , i , CRTCdata ) ;				/* Set CRTC( 3d4 ) */
    }
/*
    if ( ( HwDeviceExtension->jChipType == XGI_630 )&& ( HwDeviceExtension->jChipRevision == 0x30 ) )
    {
        if ( pVBInfo->VBInfo & SetInSlaveMode )
        {
            if ( pVBInfo->VBInfo & ( SetCRT2ToLCD | SetCRT2ToTV ) )
            {
                XGINew_SetReg1( pVBInfo->P3d4 , 0x18 , 0xFE ) ;
            }
        }
    }
*/
}


/* --------------------------------------------------------------------- */
/* Function : */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGI_SetATTRegs( USHORT ModeNo , USHORT StandTableIndex , USHORT ModeIdIndex, PVB_DEVICE_INFO pVBInfo )
{
    UCHAR ARdata ;
    USHORT i ,
           modeflag ;

    if ( ModeNo <= 0x13 )
        modeflag = pVBInfo->SModeIDTable[ ModeIdIndex ].St_ModeFlag ;
    else
        modeflag = pVBInfo->EModeIDTable[ ModeIdIndex ].Ext_ModeFlag ;

    for( i = 0 ; i <= 0x13 ; i++ )
    {
        ARdata = pVBInfo->StandTable[ StandTableIndex ].ATTR[ i ] ;
        if ( modeflag & Charx8Dot )	/* ifndef Dot9 */
        {
            if ( i == 0x13 )
            {
                if ( pVBInfo->VBInfo & SetCRT2ToLCDA )
                    ARdata = 0 ;
                else
                {
                    if ( pVBInfo->VBInfo & ( SetCRT2ToTV | SetCRT2ToLCD ) )
                    {
                        if ( pVBInfo->VBInfo & SetInSlaveMode )
                            ARdata = 0 ;
                    }
                }
            }
        }

        XGINew_GetReg2( pVBInfo->P3da ) ;			/* reset 3da */
        XGINew_SetReg3( pVBInfo->P3c0 , i ) ;		/* set index */
        XGINew_SetReg3( pVBInfo->P3c0 , ARdata ) ;	/* set data */
    }

    XGINew_GetReg2( pVBInfo->P3da ) ;			/* reset 3da */
    XGINew_SetReg3( pVBInfo->P3c0 , 0x14 ) ;		/* set index */
    XGINew_SetReg3( pVBInfo->P3c0 , 0x00 ) ;		/* set data */
    XGINew_GetReg2( pVBInfo->P3da ) ;			/* Enable Attribute */
    XGINew_SetReg3( pVBInfo->P3c0 , 0x20 ) ;
}


/* --------------------------------------------------------------------- */
/* Function : XGI_SetGRCRegs */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGI_SetGRCRegs( USHORT StandTableIndex, PVB_DEVICE_INFO pVBInfo )
{
    UCHAR GRdata ;
    USHORT i ;

    for( i = 0 ; i <= 0x08 ; i++ )
    {
        GRdata = pVBInfo->StandTable[ StandTableIndex ].GRC[ i ] ;	/* Get GR from file */
        XGINew_SetReg1( pVBInfo->P3ce , i , GRdata ) ;			/* Set GR(3ce) */
    }

    if ( pVBInfo->ModeType > ModeVGA )
    {
        GRdata = ( UCHAR )XGINew_GetReg1( pVBInfo->P3ce , 0x05 ) ;
        GRdata &= 0xBF ;						/* 256 color disable */
        XGINew_SetReg1( pVBInfo->P3ce , 0x05 , GRdata ) ;
    }
}


/* --------------------------------------------------------------------- */
/* Function : XGI_ClearExt1Regs */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGI_ClearExt1Regs(PVB_DEVICE_INFO pVBInfo)
{
    USHORT i ;

    for( i = 0x0A ; i <= 0x0E ; i++ )
        XGINew_SetReg1( pVBInfo->P3c4 , i , 0x00 ) ;	/* Clear SR0A-SR0E */
}


/* --------------------------------------------------------------------- */
/* Function : XGI_SetDefaultVCLK */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
/*UCHAR XGI_SetDefaultVCLK( void )
{
    return( 0 ) ;
}*/


/* --------------------------------------------------------------------- */
/* Function : XGI_GetRatePtrCRT2 */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
USHORT XGI_GetRatePtrCRT2( USHORT ModeNo , USHORT ModeIdIndex, PVB_DEVICE_INFO pVBInfo )
{
    SHORT  LCDRefreshIndex[] = { 0x00 , 0x00 , 0x03 , 0x01 } ,
           LCDARefreshIndex[] = { 0x00 , 0x00 , 0x03 , 0x01 , 0x01 , 0x01 , 0x01 } ;

    USHORT RefreshRateTableIndex , i ,
         modeflag , index , temp ;

    if ( ModeNo <= 0x13 )
    {
        modeflag = pVBInfo->SModeIDTable[ ModeIdIndex ].St_ModeFlag ;
    }
    else
    {
        modeflag = pVBInfo->EModeIDTable[ ModeIdIndex ].Ext_ModeFlag ;
    }

    if ( pVBInfo->IF_DEF_CH7005 == 1 )
    {
        if ( pVBInfo->VBInfo & SetCRT2ToTV )
        {
            if ( modeflag & HalfDCLK )
                return( 0 ) ;
        }
    }

    if ( ModeNo < 0x14 )
        return( 0xFFFF ) ;

    index = XGINew_GetReg1( pVBInfo->P3d4 , 0x33 ) ;
    index = index >> pVBInfo->SelectCRT2Rate ;
    index &= 0x0F ;

    if ( pVBInfo->LCDInfo & LCDNonExpanding )
        index = 0 ;

    if ( index > 0 )
        index-- ;

    if ( pVBInfo->SetFlag & ProgrammingCRT2 )
    {
        if ( pVBInfo->IF_DEF_CH7005 == 1 )
        {
            if ( pVBInfo->VBInfo & SetCRT2ToTV )
            {
                index = 0 ;
            }
        }

        if ( pVBInfo->VBInfo & ( SetCRT2ToLCD | SetCRT2ToLCDA ) )
        {
            if( pVBInfo->IF_DEF_LVDS == 0 )
            {
                if ( pVBInfo->VBType & ( VB_XGI301B | VB_XGI302B | VB_XGI301LV | VB_XGI302LV | VB_XGI301C ) )
                    temp = LCDARefreshIndex[ pVBInfo->LCDResInfo & 0x0F ] ;     /* 301b */
                else
                    temp = LCDRefreshIndex[ pVBInfo->LCDResInfo & 0x0F ] ;

                if ( index > temp )
                {
                    index = temp ;
                }
            }
            else
            {
                index = 0 ;
            }
        }
    }

    RefreshRateTableIndex = pVBInfo->EModeIDTable[ ModeIdIndex ].REFindex ;
    ModeNo = pVBInfo->RefIndex[ RefreshRateTableIndex ].ModeID ;
    i = 0 ;

    do
    {
        if ( pVBInfo->RefIndex[ RefreshRateTableIndex + i ].ModeID != ModeNo )
            break ;
        temp = pVBInfo->RefIndex[ RefreshRateTableIndex + i ].Ext_InfoFlag ;
        temp &= ModeInfoFlag ;
        if ( temp < pVBInfo->ModeType )
            break ;

        i++ ;
        index-- ;

    } while( index != 0xFFFF ) ;

    if ( !( pVBInfo->VBInfo & SetCRT2ToRAMDAC ) )
    {
        if ( pVBInfo->VBInfo & SetInSlaveMode )
        {
            temp = pVBInfo->RefIndex[ RefreshRateTableIndex + i - 1 ].Ext_InfoFlag ;
            if ( temp & InterlaceMode )
            {
                i++ ;
            }
        }
    }

    i-- ;
    if ( ( pVBInfo->SetFlag & ProgrammingCRT2 ) )
    {
        temp = XGI_AjustCRT2Rate( ModeNo , ModeIdIndex , RefreshRateTableIndex , &i, pVBInfo) ;
    }
    return( RefreshRateTableIndex + i ) ;                /*return(0x01|(temp1<<1));   */
}


/* --------------------------------------------------------------------- */
/* Function : XGI_AjustCRT2Rate */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
BOOLEAN XGI_AjustCRT2Rate( USHORT ModeNo , USHORT ModeIdIndex , USHORT RefreshRateTableIndex , USHORT *i, PVB_DEVICE_INFO pVBInfo )
{
    USHORT tempax ,
           tempbx ,
           resinfo ,
           modeflag ,
           infoflag ;

    if ( ModeNo <= 0x13 )
    {
        modeflag = pVBInfo->SModeIDTable[ ModeIdIndex ].St_ModeFlag ;	/* si+St_ModeFlag */
    }
    else
    {
        modeflag = pVBInfo->EModeIDTable[ ModeIdIndex ].Ext_ModeFlag ;
    }

    resinfo = pVBInfo->EModeIDTable[ ModeIdIndex ].Ext_RESINFO ;
    tempbx = pVBInfo->RefIndex[ RefreshRateTableIndex + ( *i ) ].ModeID ;
    tempax = 0 ;

    if ( pVBInfo->IF_DEF_LVDS == 0 )
    {
        if ( pVBInfo->VBInfo & SetCRT2ToRAMDAC )
        {
            tempax |= SupportRAMDAC2 ;

            if ( pVBInfo->VBType & VB_XGI301C )
                tempax |= SupportCRT2in301C ;
        }

        if ( pVBInfo->VBInfo & ( SetCRT2ToLCD | SetCRT2ToLCDA ) )	/* 301b */
        {
            tempax |= SupportLCD ;

            if ( pVBInfo->LCDResInfo != Panel1280x1024 )
            {
                if ( pVBInfo->LCDResInfo != Panel1280x960 )
                {
                    if ( pVBInfo->LCDInfo & LCDNonExpanding )
                    {
                        if ( resinfo >= 9 )
                        {
                            tempax = 0 ;
                            return( 0 ) ;
                        }
                    }
                }
            }
        }

        if ( pVBInfo->VBInfo & SetCRT2ToHiVisionTV )	/* for HiTV */
        {
            if ( ( pVBInfo->VBType & VB_XGI301LV ) && ( pVBInfo->VBExtInfo == VB_YPbPr1080i ) )
            {
                tempax |= SupportYPbPr ;
                if ( pVBInfo->VBInfo & SetInSlaveMode )
                {
                    if ( resinfo == 4 )
                        return( 0 ) ;

                    if ( resinfo == 3 )
                        return( 0 ) ;

                    if ( resinfo > 7 )
                        return( 0 ) ;
                }
            }
            else
            {
                tempax |= SupportHiVisionTV ;
                if ( pVBInfo->VBInfo & SetInSlaveMode )
                {
                    if ( resinfo == 4 )
                        return( 0 ) ;

                    if ( resinfo == 3 )
                    {
                        if ( pVBInfo->SetFlag & TVSimuMode )
                            return( 0 ) ;
                    }

                    if ( resinfo > 7 )
                        return( 0 ) ;
                }
            }
        }
        else
        {
            if ( pVBInfo->VBInfo & ( SetCRT2ToAVIDEO | SetCRT2ToSVIDEO | SetCRT2ToSCART | SetCRT2ToYPbPr | SetCRT2ToHiVisionTV ) )
            {
                tempax |= SupportTV ;

                if ( pVBInfo->VBType & ( VB_XGI301B | VB_XGI302B | VB_XGI301LV | VB_XGI302LV | VB_XGI301C ) )
                {
                    tempax |= SupportTV1024 ;
                }

                if ( !( pVBInfo->VBInfo & SetPALTV ) )
                {
                    if ( modeflag & NoSupportSimuTV )
                    {
                        if ( pVBInfo->VBInfo & SetInSlaveMode )
                        {
                            if ( !( pVBInfo->VBInfo & SetNotSimuMode ) )
                            {
                               return( 0 ) ;
                            }
                        }
                    }
                }
            }
        }
    }
    else		/* for LVDS */
    {
        if ( pVBInfo->IF_DEF_CH7005 == 1 )
        {
            if ( pVBInfo->VBInfo & SetCRT2ToTV )
            {
                tempax |= SupportCHTV ;
            }
        }

        if ( pVBInfo->VBInfo & SetCRT2ToLCD )
        {
            tempax |= SupportLCD ;

            if ( resinfo > 0x08 )
                return( 0 ) ;		/* 1024x768 */

            if ( pVBInfo->LCDResInfo < Panel1024x768 )
            {
                if ( resinfo > 0x07 )
                    return( 0 ) ;	/* 800x600 */

                if ( resinfo == 0x04 )
                    return( 0 ) ;	/* 512x384 */
            }
        }
    }

    for( ; pVBInfo->RefIndex[ RefreshRateTableIndex + ( *i ) ].ModeID == tempbx ; ( *i )-- )
    {
        infoflag = pVBInfo->RefIndex[ RefreshRateTableIndex + ( *i ) ].Ext_InfoFlag ;
        if ( infoflag & tempax )
        {
            return( 1 ) ;
        }
        if ( ( *i ) == 0 )
            break ;
    }

    for( ( *i ) = 0 ; ; ( *i )++ )
    {
        infoflag = pVBInfo->RefIndex[ RefreshRateTableIndex + ( *i ) ].Ext_InfoFlag ;
        if ( pVBInfo->RefIndex[ RefreshRateTableIndex + ( *i ) ].ModeID != tempbx )
        {
            return( 0 ) ;
        }

        if ( infoflag & tempax )
        {
            return( 1 ) ;
        }
    }
    return( 1 ) ;
}


/* --------------------------------------------------------------------- */
/* Function : XGI_SetSync */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGI_SetSync(USHORT RefreshRateTableIndex, PVB_DEVICE_INFO pVBInfo )
{
    USHORT sync ,
           temp ;

    sync = pVBInfo->RefIndex[ RefreshRateTableIndex ].Ext_InfoFlag >> 8 ;	/* di+0x00 */
    sync &= 0xC0 ;
    temp = 0x2F ;
    temp |= sync ;
    XGINew_SetReg3( pVBInfo->P3c2 , temp ) ;				/* Set Misc(3c2) */
}


/* --------------------------------------------------------------------- */
/* Function : XGI_SetCRT1CRTC */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGI_SetCRT1CRTC( USHORT ModeNo , USHORT ModeIdIndex , USHORT RefreshRateTableIndex,PVB_DEVICE_INFO pVBInfo, PXGI_HW_DEVICE_INFO HwDeviceExtension )
{
    UCHAR  index ,
           data ;

    USHORT i ;






    index = pVBInfo->RefIndex[ RefreshRateTableIndex ].Ext_CRT1CRTC ;	/* Get index */
    index = index&IndexMask ;

    data =( UCHAR )XGINew_GetReg1( pVBInfo->P3d4 , 0x11 ) ;
    data &= 0x7F ;
    XGINew_SetReg1(pVBInfo->P3d4,0x11,data);				/* Unlock CRTC */

    for( i = 0 ; i < 8 ; i++ )
        pVBInfo->TimingH[ 0 ].data[ i ] = pVBInfo->XGINEWUB_CRT1Table[ index ].CR[ i ] ;

    for( i = 0 ; i < 7 ; i++ )
        pVBInfo->TimingV[ 0 ].data[ i ] = pVBInfo->XGINEWUB_CRT1Table[ index ].CR[ i + 8 ] ;

    XGI_SetCRT1Timing_H( pVBInfo, HwDeviceExtension ) ;



    XGI_SetCRT1Timing_V( ModeIdIndex , ModeNo, pVBInfo ) ;


    if( pVBInfo->ModeType > 0x03 )
        XGINew_SetReg1( pVBInfo->P3d4 , 0x14 , 0x4F ) ;
}


/* --------------------------------------------------------------------- */
/* Function : XGI_SetCRT1Timing_H */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGI_SetCRT1Timing_H( PVB_DEVICE_INFO pVBInfo, PXGI_HW_DEVICE_INFO HwDeviceExtension )
{
    UCHAR data , data1, pushax;
    USHORT i , j ;

    /* XGINew_SetReg1( pVBInfo->P3d4 , 0x51 , 0 ) ; */
    /* XGINew_SetReg1( pVBInfo->P3d4 , 0x56 , 0 ) ; */
    /* XGINew_SetRegANDOR( pVBInfo->P3d4 ,0x11 , 0x7f , 0x00 ) ; */

    data = ( UCHAR )XGINew_GetReg1( pVBInfo->P3d4 , 0x11 ) ;		/* unlock cr0-7 */
    data &= 0x7F ;
    XGINew_SetReg1( pVBInfo->P3d4 , 0x11 , data ) ;

    data = pVBInfo->TimingH[ 0 ].data[ 0 ] ;
    XGINew_SetReg1( pVBInfo->P3d4 , 0 , data ) ;

    for( i = 0x01 ; i <= 0x04 ; i++ )
    {
        data = pVBInfo->TimingH[ 0 ].data[ i ] ;
        XGINew_SetReg1( pVBInfo->P3d4 , ( USHORT )( i + 1 ) , data ) ;
    }

    for( i = 0x05 ; i <= 0x06 ; i++ )
    {
        data = pVBInfo->TimingH[ 0 ].data[ i ];
        XGINew_SetReg1( pVBInfo->P3c4 ,( USHORT )( i + 6 ) , data ) ;
    }

    j = ( UCHAR )XGINew_GetReg1( pVBInfo->P3c4 , 0x0e ) ;
    j &= 0x1F ;
    data = pVBInfo->TimingH[ 0 ].data[ 7 ] ;
    data &= 0xE0 ;
    data |= j ;
    XGINew_SetReg1( pVBInfo->P3c4 , 0x0e , data ) ;

    if ( HwDeviceExtension->jChipType == XG20 )
    {
    	data = ( UCHAR )XGINew_GetReg1( pVBInfo->P3d4 , 0x04 ) ;
    	data = data - 1 ;
    	XGINew_SetReg1( pVBInfo->P3d4 , 0x04 , data ) ;
    	data = ( UCHAR )XGINew_GetReg1( pVBInfo->P3d4 , 0x05 ) ;
    	data1 = data ;
    	data1 &= 0xE0 ;
    	data &= 0x1F ;
    	if ( data == 0 )
    	{
    	    pushax = data ;
    	    data = ( UCHAR )XGINew_GetReg1( pVBInfo->P3c4 , 0x0c ) ;
    	    data &= 0xFB ;
    	    XGINew_SetReg1( pVBInfo->P3c4 , 0x0c , data ) ;
    	    data = pushax ;
    	}
    	data = data - 1 ;
    	data |= data1 ;
    	XGINew_SetReg1( pVBInfo->P3d4 , 0x05 , data ) ;
    	data = ( UCHAR )XGINew_GetReg1( pVBInfo->P3c4 , 0x0e ) ;
    	data = data >> 5 ;
    	data = data + 3 ;
    	if ( data > 7 )
    	    data = data - 7 ;
    	data = data << 5 ;
    	XGINew_SetRegANDOR( pVBInfo->P3c4 , 0x0e , ~0xE0 , data ) ;
    }
}


/* --------------------------------------------------------------------- */
/* Function : XGI_SetCRT1Timing_V */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGI_SetCRT1Timing_V( USHORT ModeIdIndex , USHORT ModeNo,PVB_DEVICE_INFO pVBInfo )
{
    UCHAR data ;
    USHORT i , j ;

    /* XGINew_SetReg1( pVBInfo->P3d4 , 0x51 , 0 ) ; */
    /* XGINew_SetReg1( pVBInfo->P3d4 , 0x56 , 0 ) ; */
    /* XGINew_SetRegANDOR( pVBInfo->P3d4 , 0x11 , 0x7f , 0x00 ) ; */

    for( i = 0x00 ; i <= 0x01 ; i++ )
    {
        data = pVBInfo->TimingV[ 0 ].data[ i ] ;
        XGINew_SetReg1( pVBInfo->P3d4 , ( USHORT )( i + 6 ) , data ) ;
    }

    for( i = 0x02 ; i <= 0x03 ; i++ )
    {
        data = pVBInfo->TimingV[ 0 ].data[ i ] ;
        XGINew_SetReg1( pVBInfo->P3d4 , ( USHORT )( i + 0x0e ) , data ) ;
    }

    for( i = 0x04 ; i <= 0x05 ; i++ )
    {
        data = pVBInfo->TimingV[ 0 ].data[ i ] ;
        XGINew_SetReg1( pVBInfo->P3d4 , ( USHORT )( i + 0x11 ) , data ) ;
    }

    j = ( UCHAR )XGINew_GetReg1( pVBInfo->P3c4 , 0x0a ) ;
    j &= 0xC0 ;
    data = pVBInfo->TimingV[ 0 ].data[ 6 ] ;
    data &= 0x3F ;
    data |= j ;
    XGINew_SetReg1( pVBInfo->P3c4 , 0x0a , data ) ;

    data = pVBInfo->TimingV[ 0 ].data[ 6 ] ;
    data &= 0x80 ;
    data = data >> 2 ;

    if ( ModeNo <= 0x13 )
        i = pVBInfo->SModeIDTable[ ModeIdIndex ].St_ModeFlag ;
    else
        i = pVBInfo->EModeIDTable[ ModeIdIndex ].Ext_ModeFlag ;

    i &= DoubleScanMode ;
    if ( i )
        data |= 0x80 ;

    j = ( UCHAR )XGINew_GetReg1( pVBInfo->P3d4 , 0x09 ) ;
    j &= 0x5F ;
    data |= j ;
    XGINew_SetReg1( pVBInfo->P3d4 , 0x09 , data ) ;
}


/* --------------------------------------------------------------------- */
/* Function : XGI_SetCRT1DE */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGI_SetCRT1DE( PXGI_HW_DEVICE_INFO HwDeviceExtension , USHORT ModeNo,USHORT ModeIdIndex , USHORT RefreshRateTableIndex, PVB_DEVICE_INFO pVBInfo )
{
    USHORT resindex ,
           tempax ,
           tempbx ,
           tempcx ,
           temp ,
           modeflag ;

    UCHAR data ;

    resindex = XGI_GetResInfo( ModeNo , ModeIdIndex, pVBInfo ) ;

    if ( ModeNo <= 0x13 )
    {
        modeflag = pVBInfo->SModeIDTable[ ModeIdIndex ].St_ModeFlag ;
        tempax = pVBInfo->StResInfo[ resindex ].HTotal ;
        tempbx = pVBInfo->StResInfo[ resindex ].VTotal ;
    }
    else
    {
        modeflag = pVBInfo->EModeIDTable[ ModeIdIndex ].Ext_ModeFlag ;
        tempax = pVBInfo->ModeResInfo[ resindex ].HTotal ;
        tempbx = pVBInfo->ModeResInfo[ resindex ].VTotal ;
    }

    if ( modeflag & HalfDCLK )
        tempax = tempax >> 1 ;

    if ( ModeNo > 0x13 )
    {
        if ( modeflag & HalfDCLK )
            tempax = tempax << 1 ;

        temp = pVBInfo->RefIndex[ RefreshRateTableIndex ].Ext_InfoFlag ;

        if ( temp & InterlaceMode )
            tempbx = tempbx >> 1 ;

        if ( modeflag & DoubleScanMode )
            tempbx = tempbx << 1 ;
    }

    tempcx = 8 ;

    /* if ( !( modeflag & Charx8Dot ) ) */
    /* tempcx = 9 ; */

    tempax /= tempcx ;
    tempax -= 1 ;
    tempbx -= 1 ;
    tempcx = tempax ;
    temp = ( UCHAR )XGINew_GetReg1( pVBInfo->P3d4 , 0x11 ) ;
    data = ( UCHAR )XGINew_GetReg1( pVBInfo->P3d4 , 0x11 ) ;
    data &= 0x7F ;
    XGINew_SetReg1( pVBInfo->P3d4 , 0x11 , data ) ;		/* Unlock CRTC */
    XGINew_SetReg1( pVBInfo->P3d4 , 0x01 , ( USHORT )( tempcx & 0xff ) ) ;
    XGINew_SetRegANDOR( pVBInfo->P3d4 , 0x0b , ~0x0c , ( USHORT )( ( tempcx & 0x0ff00 ) >> 10 ) ) ;
    XGINew_SetReg1( pVBInfo->P3d4 , 0x12 , ( USHORT )( tempbx & 0xff ) ) ;
    tempax = 0 ;
    tempbx = tempbx >> 8 ;

    if ( tempbx & 0x01 )
        tempax |= 0x02 ;

    if ( tempbx & 0x02 )
        tempax |= 0x40 ;

    XGINew_SetRegANDOR( pVBInfo->P3d4 , 0x07 , ~0x42 , tempax ) ;
    data =( UCHAR )XGINew_GetReg1( pVBInfo->P3d4 , 0x07 ) ;
    data &= 0xFF ;
    tempax = 0 ;

    if ( tempbx & 0x04 )
        tempax |= 0x02 ;

    XGINew_SetRegANDOR( pVBInfo->P3d4 ,0x0a , ~0x02 , tempax ) ;
    XGINew_SetReg1( pVBInfo->P3d4 , 0x11 , temp ) ;
}


/* --------------------------------------------------------------------- */
/* Function : XGI_GetResInfo */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
USHORT 	XGI_GetResInfo(USHORT ModeNo , USHORT ModeIdIndex, PVB_DEVICE_INFO pVBInfo )
{
    USHORT resindex ;

    if ( ModeNo <= 0x13 )
    {
        resindex = pVBInfo->SModeIDTable[ ModeIdIndex ].St_ResInfo ; 	/* si+St_ResInfo */
    }
    else
    {
        resindex = pVBInfo->EModeIDTable[ ModeIdIndex ].Ext_RESINFO ;	/* si+Ext_ResInfo */
    }
    return( resindex ) ;
}


/* --------------------------------------------------------------------- */
/* Function : XGI_SetCRT1Offset */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGI_SetCRT1Offset(  USHORT ModeNo , USHORT ModeIdIndex , USHORT RefreshRateTableIndex , PXGI_HW_DEVICE_INFO HwDeviceExtension, PVB_DEVICE_INFO pVBInfo )
{
    USHORT temp ,
           ah ,
           al ,
           temp2 ,
           i ,
           DisplayUnit ;

    /* GetOffset */
    temp = pVBInfo->EModeIDTable[ ModeIdIndex ].Ext_ModeInfo ;
    temp = temp >> 8 ;
    temp = pVBInfo->ScreenOffset[ temp ] ;

    temp2 = pVBInfo->RefIndex[ RefreshRateTableIndex ].Ext_InfoFlag ;
    temp2 &= InterlaceMode ;

    if ( temp2 )
        temp = temp << 1;

    temp2 = pVBInfo->ModeType - ModeEGA ;

    switch( temp2 )
    {
        case 0:
            temp2 = 1 ;
            break ;
        case 1:
            temp2 = 2 ;
            break ;
        case 2:
            temp2 = 4 ;
            break ;
        case 3:
            temp2 = 4 ;
            break ;
        case 4:
            temp2 = 6 ;
            break;
        case 5:
            temp2 = 8 ;
            break ;
        default:
            break ;
    }

    if ( ( ModeNo >= 0x26 ) && ( ModeNo <= 0x28 ) )
        temp = temp * temp2 + temp2 / 2 ;
    else
        temp *= temp2 ;

    /* SetOffset */
    DisplayUnit = temp ;
    temp2 = temp ;
    temp = temp >> 8 ;		/* ah */
    temp &= 0x0F ;
    i = XGINew_GetReg1( pVBInfo->P3c4 , 0x0E ) ;
    i &= 0xF0 ;
    i |= temp ;
    XGINew_SetReg1( pVBInfo->P3c4 , 0x0E , i ) ;

    temp =( UCHAR )temp2 ;
    temp &= 0xFF ;		/* al */
    XGINew_SetReg1( pVBInfo->P3d4 , 0x13 , temp ) ;

    /* SetDisplayUnit */
    temp2 = pVBInfo->RefIndex[ RefreshRateTableIndex ].Ext_InfoFlag ;
    temp2 &= InterlaceMode ;
    if ( temp2 )
        DisplayUnit >>= 1 ;

    DisplayUnit = DisplayUnit << 5 ;
    ah = ( DisplayUnit & 0xff00 ) >> 8 ;
    al = DisplayUnit & 0x00ff ;
    if ( al == 0 )
        ah += 1 ;
    else
        ah += 2 ;

    if ( HwDeviceExtension->jChipType == XG20 )
        if ( ( ModeNo == 0x4A ) | (ModeNo == 0x49 ) )
            ah -= 1 ;

    XGINew_SetReg1( pVBInfo->P3c4 , 0x10 , ah ) ;
}


/* --------------------------------------------------------------------- */
/* Function : XGI_SetCRT1VCLK */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGI_SetCRT1VCLK( USHORT ModeNo , USHORT ModeIdIndex ,
                        PXGI_HW_DEVICE_INFO HwDeviceExtension , USHORT RefreshRateTableIndex, PVB_DEVICE_INFO pVBInfo )
{
    UCHAR index , data ;
    USHORT vclkindex ;

    if ( pVBInfo->IF_DEF_LVDS == 1 )
    {
        index = pVBInfo->RefIndex[ RefreshRateTableIndex ].Ext_CRTVCLK ;
        data = XGINew_GetReg1( pVBInfo->P3c4 , 0x31 ) & 0xCF ;
        XGINew_SetReg1( pVBInfo->P3c4 , 0x31 , data ) ;
        XGINew_SetReg1( pVBInfo->P3c4 , 0x2B , pVBInfo->VCLKData[ index ].SR2B ) ;
        XGINew_SetReg1( pVBInfo->P3c4 , 0x2C , pVBInfo->VCLKData[ index ].SR2C ) ;
        XGINew_SetReg1( pVBInfo->P3c4 , 0x2D , 0x01 ) ;
    }
    else if ( ( pVBInfo->VBType & ( VB_XGI301B | VB_XGI302B | VB_XGI301LV | VB_XGI302LV | VB_XGI301C ) ) && ( pVBInfo->VBInfo & SetCRT2ToLCDA ) )
    {
        vclkindex = XGI_GetVCLK2Ptr( ModeNo , ModeIdIndex , RefreshRateTableIndex , HwDeviceExtension, pVBInfo ) ;
        data = XGINew_GetReg1( pVBInfo->P3c4 , 0x31 ) & 0xCF ;
        XGINew_SetReg1( pVBInfo->P3c4 , 0x31 , data ) ;
        data = pVBInfo->VBVCLKData[ vclkindex ].Part4_A ;
        XGINew_SetReg1( pVBInfo->P3c4 , 0x2B , data ) ;
        data = pVBInfo->VBVCLKData[ vclkindex ].Part4_B ;
        XGINew_SetReg1( pVBInfo->P3c4 , 0x2C , data ) ;
        XGINew_SetReg1( pVBInfo->P3c4 , 0x2D , 0x01 ) ;
    }
    else
    {
        index = pVBInfo->RefIndex[ RefreshRateTableIndex ].Ext_CRTVCLK ;
        data = XGINew_GetReg1( pVBInfo->P3c4 , 0x31 ) & 0xCF ;
        XGINew_SetReg1( pVBInfo->P3c4 , 0x31 , data ) ;
        XGINew_SetReg1( pVBInfo->P3c4 , 0x2B , pVBInfo->VCLKData[ index ].SR2B ) ;
        XGINew_SetReg1( pVBInfo->P3c4 , 0x2C , pVBInfo->VCLKData[ index ].SR2C ) ;
        XGINew_SetReg1( pVBInfo->P3c4 , 0x2D , 0x01 ) ;
    }

    if ( HwDeviceExtension->jChipType == XG20 )
    {
    	if ( pVBInfo->EModeIDTable[ ModeIdIndex ].Ext_ModeFlag & HalfDCLK )
    	{
    	data = XGINew_GetReg1( pVBInfo->P3c4 , 0x2B ) ;
    	XGINew_SetReg1( pVBInfo->P3c4 , 0x2B , data ) ;
    	data = XGINew_GetReg1( pVBInfo->P3c4 , 0x2C ) ;
    	index = data ;
    	index &= 0xE0 ;
    	data &= 0x1F ;
    	data = data << 1 ;
    	data += 1 ;
    	data |= index ;
    	XGINew_SetReg1( pVBInfo->P3c4 , 0x2C , data ) ;
    	}
    }
}


/* --------------------------------------------------------------------- */
/* Function : XGI_SetCRT1FIFO */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGI_SetCRT1FIFO( USHORT ModeNo , PXGI_HW_DEVICE_INFO HwDeviceExtension,PVB_DEVICE_INFO pVBInfo )
{
    USHORT data ;

    data = XGINew_GetReg1( pVBInfo->P3c4 , 0x3D ) ;
    data &= 0xfe ;
    XGINew_SetReg1( pVBInfo->P3c4 , 0x3D , data ) ;	/* diable auto-threshold */

    if ( ModeNo > 0x13 )
    {
        XGINew_SetReg1( pVBInfo->P3c4 , 0x08 , 0x34 ) ;
        data = XGINew_GetReg1( pVBInfo->P3c4 , 0x09 ) ;
        data &= 0xF0 ;
        XGINew_SetReg1( pVBInfo->P3c4 , 0x09 , data ) ;
        data = XGINew_GetReg1( pVBInfo->P3c4 , 0x3D ) ;
        data |= 0x01 ;
        XGINew_SetReg1( pVBInfo->P3c4 , 0x3D , data ) ;
    }
    else
    {
        XGINew_SetReg1( pVBInfo->P3c4 , 0x08 , 0xAE ) ;
        data = XGINew_GetReg1( pVBInfo->P3c4 , 0x09 ) ;
        data &= 0xF0 ;
        XGINew_SetReg1( pVBInfo->P3c4 , 0x09 , data ) ;
    }
}


/* --------------------------------------------------------------------- */
/* Function : XGI_SetCRT1ModeRegs */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGI_SetCRT1ModeRegs( PXGI_HW_DEVICE_INFO HwDeviceExtension ,
                            USHORT ModeNo , USHORT ModeIdIndex , USHORT RefreshRateTableIndex,PVB_DEVICE_INFO pVBInfo )
{
    USHORT data ,
           data2 ,
           data3 ,
           infoflag = 0 ,
           modeflag ,
           resindex ,
           xres ;

    if ( ModeNo > 0x13 )
    {
        modeflag = pVBInfo->EModeIDTable[ ModeIdIndex ].Ext_ModeFlag ;
        infoflag = pVBInfo->RefIndex[ RefreshRateTableIndex ].Ext_InfoFlag ;
    }
    else
        modeflag = pVBInfo->SModeIDTable[ ModeIdIndex ].St_ModeFlag ;    /* si+St_ModeFlag */

    if ( XGINew_GetReg1( pVBInfo->P3d4 , 0x31 ) & 0x01 )
      XGINew_SetRegANDOR( pVBInfo->P3c4 , 0x1F , 0x3F , 0x00 ) ;

    if ( ModeNo > 0x13 )
        data = infoflag ;
    else
        data = 0 ;

    data2 = 0 ;

    if ( ModeNo > 0x13 )
    {
        if ( pVBInfo->ModeType > 0x02 )
        {
            data2 |= 0x02 ;
            data3 = pVBInfo->ModeType - ModeVGA ;
            data3 = data3 << 2 ;
            data2 |= data3 ;
        }
    }

    data &= InterlaceMode ;

    if ( data )
        data2 |= 0x20 ;

    XGINew_SetRegANDOR( pVBInfo->P3c4 , 0x06 , ~0x3F , data2 ) ;
    /* XGINew_SetReg1(pVBInfo->P3c4,0x06,data2); */
    resindex = XGI_GetResInfo( ModeNo , ModeIdIndex, pVBInfo ) ;
    if ( ModeNo <= 0x13 )
        xres = pVBInfo->StResInfo[ resindex ].HTotal ;
    else
        xres = pVBInfo->ModeResInfo[ resindex ].HTotal ;                         /* xres->ax */

    data = 0x0000 ;
    if ( infoflag & InterlaceMode )
    {
        if ( xres == 1024 )
            data = 0x0035 ;
        else if ( xres == 1280 )
            data = 0x0048 ;
    }

    data2 = data & 0x00FF ;
    XGINew_SetRegANDOR( pVBInfo->P3d4 , 0x19 , 0xFF , data2 ) ;
    data2 = ( data & 0xFF00 ) >> 8 ;
    XGINew_SetRegANDOR( pVBInfo->P3d4 , 0x19 , 0xFC , data2 ) ;

    if( modeflag & HalfDCLK )
        XGINew_SetRegANDOR( pVBInfo->P3c4 , 0x01 , 0xF7 , 0x08 ) ;

    data2 = 0 ;

    if ( modeflag & LineCompareOff )
        data2 |= 0x08 ;

    if ( ModeNo > 0x13 )
    {
        if ( pVBInfo->ModeType == ModeEGA )
            data2 |= 0x40 ;
    }

    XGINew_SetRegANDOR( pVBInfo->P3c4 , 0x0F , ~0x48 , data2 ) ;
    data = 0x60 ;
    if ( pVBInfo->ModeType != ModeText )
    {
        data = data ^ 0x60 ;
        if ( pVBInfo->ModeType != ModeEGA )
        {
            data = data ^ 0xA0 ;
        }
    }
    XGINew_SetRegANDOR( pVBInfo->P3c4 , 0x21 , 0x1F , data ) ;

    XGI_SetVCLKState( HwDeviceExtension , ModeNo , RefreshRateTableIndex, pVBInfo) ;

    /* if(modeflag&HalfDCLK)//030305 fix lowresolution bug */
    /* if(XGINew_IF_DEF_NEW_LOWRES) */
    /* XGI_VesaLowResolution(ModeNo,ModeIdIndex);//030305 fix lowresolution bug */

    data=XGINew_GetReg1( pVBInfo->P3d4 , 0x31 ) ;

    if (HwDeviceExtension->jChipType == XG20 )
    {
    	if ( data & 0x40 )
    	    data = 0x33 ;
    	else
    	    data = 0x73 ;
    	XGINew_SetReg1( pVBInfo->P3d4 , 0x52 , data ) ;
    	XGINew_SetReg1( pVBInfo->P3d4 , 0x51 , 0x02 ) ;
    }
    else
    {
    if ( data & 0x40 )
        data = 0x2c ;
    else
        data = 0x6c ;
    XGINew_SetReg1( pVBInfo->P3d4 , 0x52 , data ) ;
    }

}


/* --------------------------------------------------------------------- */
/* Function : XGI_SetVCLKState */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGI_SetVCLKState(  PXGI_HW_DEVICE_INFO HwDeviceExtension , USHORT ModeNo , USHORT RefreshRateTableIndex,PVB_DEVICE_INFO pVBInfo )
{
    unsigned int  data ,
                  data2 = 0 ;
    int VCLK ;

    UCHAR  index ;

    if ( ModeNo <= 0x13 )
        VCLK = 0 ;
    else
    {
        index = pVBInfo->RefIndex[ RefreshRateTableIndex ].Ext_CRTVCLK ;
        index &= IndexMask ;
        VCLK = pVBInfo->VCLKData[ index ].CLOCK ;
    }

    data = XGINew_GetReg1( pVBInfo->P3c4 , 0x32 ) ;
    data &= 0xf3 ;
    if ( VCLK >= 200 )
        data |= 0x0c ;	/* VCLK > 200 */

    if ( HwDeviceExtension->jChipType == XG20 )
        data &= ~0x04 ; /* 2 pixel mode */

    XGINew_SetReg1( pVBInfo->P3c4 , 0x32 , data ) ;

    if ( HwDeviceExtension->jChipType != XG20 )
    {
    data = XGINew_GetReg1( pVBInfo->P3c4 , 0x1F ) ;
    data &= 0xE7 ;
    if ( VCLK < 200 )
        data |= 0x10 ;
    XGINew_SetReg1( pVBInfo->P3c4 , 0x1F , data ) ;
    }

    if ( ( VCLK >= 0 ) && ( VCLK < 135 ) )
        data2 = 0x03 ;
    else if ( ( VCLK >= 135 ) && ( VCLK < 160 ) )
        data2 = 0x02 ;
    else if ( ( VCLK >= 160 ) && ( VCLK < 260 ) )
        data2 = 0x01 ;
    else if ( VCLK > 260 )
        data2 = 0x00 ;

    XGINew_SetRegANDOR( pVBInfo->P3c4 , 0x07 , 0xFC , data2 ) ;
}


/* --------------------------------------------------------------------- */
/* Function : XGI_VesaLowResolution */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
/*void XGI_VesaLowResolution( USHORT ModeNo , USHORT ModeIdIndex ,PVB_DEVICE_INFO pVBInfo)
{
    USHORT modeflag;

    if ( ModeNo > 0x13 )
        modeflag = pVBInfo->EModeIDTable[ ModeIdIndex ].Ext_ModeFlag ;
    else
        modeflag = pVBInfo->SModeIDTable[ ModeIdIndex ].St_ModeFlag ;

    if ( ModeNo > 0x13 )
    {
        if ( modeflag & DoubleScanMode )
        {
            if ( modeflag & HalfDCLK )
            {
                if ( pVBInfo->VBType & ( VB_XGI301B | VB_XGI302B | VB_XGI301LV | VB_XGI302LV | VB_XGI301C ) )
                {
                    if ( !( pVBInfo->VBInfo & SetCRT2ToRAMDAC ) )
                    {
                        if ( pVBInfo->VBInfo & SetInSlaveMode )
                        {
                            XGINew_SetRegANDOR( pVBInfo->P3c4 , 0x01 , 0xf7 , 0x00 ) ;
                            XGINew_SetRegANDOR( pVBInfo->P3c4 , 0x0f , 0x7f , 0x00 ) ;
                            return ;
                        }
                    }
                }
                XGINew_SetRegANDOR( pVBInfo->P3c4 , 0x0f , 0xff , 0x80 ) ;
                XGINew_SetRegANDOR( pVBInfo->P3c4 , 0x01 , 0xf7 , 0x00 ) ;
                return ;
            }
        }
    }
    XGINew_SetRegANDOR( pVBInfo->P3c4 , 0x0f , 0x7f , 0x00 ) ;
}
*/

/* --------------------------------------------------------------------- */
/* Function : XGI_LoadDAC */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGI_LoadDAC( USHORT ModeNo , USHORT ModeIdIndex,PVB_DEVICE_INFO pVBInfo )
{
    USHORT data , data2 , time ,
           i  , j , k , m , n , o ,
           si , di , bx , dl , al , ah , dh ,
           *table = NULL ;

    if ( ModeNo <= 0x13 )
        data = pVBInfo->SModeIDTable[ ModeIdIndex ].St_ModeFlag ;
    else
        data = pVBInfo->EModeIDTable[ ModeIdIndex ].Ext_ModeFlag ;

    data &= DACInfoFlag ;
    time = 64 ;

    if ( data == 0x00 )
        table = XGINew_MDA_DAC ;
    else if ( data == 0x08 )
        table = XGINew_CGA_DAC ;
    else if ( data == 0x10 )
        table = XGINew_EGA_DAC ;
    else if ( data == 0x18 )
    {
        time = 256 ;
        table = XGINew_VGA_DAC ;
    }

    if ( time == 256 )
        j = 16 ;
    else
        j = time ;

    XGINew_SetReg3( pVBInfo->P3c6 , 0xFF ) ;
    XGINew_SetReg3( pVBInfo->P3c8 , 0x00 ) ;

    for( i = 0 ; i < j ; i++ )
    {
        data = table[ i ] ;

        for( k = 0 ; k < 3 ; k++ )
        {
            data2 = 0 ;

            if ( data & 0x01 )
                data2 = 0x2A ;

            if ( data & 0x02 )
                data2 += 0x15 ;

            XGINew_SetReg3( pVBInfo->P3c9 , data2 ) ;
            data = data >> 2 ;
        }
    }

    if ( time == 256 )
    {
        for( i = 16 ; i < 32 ; i++ )
        {
            data = table[ i ] ;

            for( k = 0 ; k < 3 ; k++ )
                XGINew_SetReg3( pVBInfo->P3c9 , data ) ;
        }

        si = 32 ;

        for( m = 0 ; m < 9 ; m++ )
        {
            di = si ;
            bx = si + 0x04 ;
            dl = 0 ;

            for( n = 0 ; n < 3 ; n++ )
            {
                for( o = 0 ; o < 5 ; o++ )
                {
                    dh = table[ si ] ;
                    ah = table[ di ] ;
                    al = table[ bx ] ;
                    si++ ;
                    XGI_WriteDAC( dl , ah , al , dh, pVBInfo ) ;
                }

                si -= 2 ;

                for( o = 0 ; o < 3 ; o++ )
                {
                    dh = table[ bx ] ;
                    ah = table[ di ] ;
                    al = table[ si ] ;
                    si-- ;
                    XGI_WriteDAC( dl , ah , al , dh, pVBInfo ) ;
                }

                dl++ ;
            }

            si += 5 ;
        }
    }
}


/* --------------------------------------------------------------------- */
/* Function : XGI_WriteDAC */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGI_WriteDAC( USHORT dl , USHORT ah , USHORT al , USHORT dh,PVB_DEVICE_INFO pVBInfo )
{
    USHORT temp , bh , bl ;

    bh = ah ;
    bl = al ;

    if ( dl != 0 )
    {
        temp = bh ;
        bh = dh ;
        dh = temp ;
        if ( dl == 1 )
        {
            temp = bl ;
            bl = dh ;
            dh = temp ;
        }
        else
        {
            temp = bl ;
            bl = bh ;
            bh = temp ;
        }
    }
    XGINew_SetReg3( pVBInfo->P3c9 , ( USHORT )dh ) ;
    XGINew_SetReg3( pVBInfo->P3c9 , ( USHORT )bh ) ;
    XGINew_SetReg3( pVBInfo->P3c9 , ( USHORT )bl ) ;
}


/* --------------------------------------------------------------------- */
/* Function : XGI_ClearBuffer */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGI_ClearBuffer( PXGI_HW_DEVICE_INFO HwDeviceExtension , USHORT ModeNo, PVB_DEVICE_INFO  pVBInfo)
{
    PVOID VideoMemoryAddress = ( PVOID )HwDeviceExtension->pjVideoMemoryAddress ;
    ULONG AdapterMemorySize  = ( ULONG )HwDeviceExtension->ulVideoMemorySize ;
 /* PUSHORT pBuffer ; */
 /*   int i ; */

    if ( pVBInfo->ModeType >= ModeEGA )
    {
        if ( ModeNo > 0x13 )
        {
            AdapterMemorySize = 0x40000 ;	/* clear 256k */
            /* GetDRAMSize( HwDeviceExtension ) ; */
            XGI_SetMemory( VideoMemoryAddress , AdapterMemorySize , 0 ) ;
        }
        else
        {
/*
            pBuffer = VideoMemoryAddress ;
            for( i = 0 ; i < 0x4000 ; i++ )
                pBuffer[ i ] = 0x0000 ;
*/
        }
    }
    else
    {
        if ( pVBInfo->ModeType < ModeCGA )
        {
/*
            pBuffer = VideoMemoryAddress ;
            for ( i = 0 ; i < 0x4000 ; i++ )
                pBuffer[ i ] = 0x0720 ;
*/
        }
        else
            XGI_SetMemory( VideoMemoryAddress , 0x8000 , 0 ) ;
    }
}


/* --------------------------------------------------------------------- */
/* Function : XGI_SetLCDAGroup */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGI_SetLCDAGroup( USHORT ModeNo , USHORT ModeIdIndex , PXGI_HW_DEVICE_INFO HwDeviceExtension, PVB_DEVICE_INFO  pVBInfo )
{
    USHORT RefreshRateTableIndex ;
    /* USHORT temp ; */

    /* pVBInfo->SelectCRT2Rate = 0 ; */

    pVBInfo->SetFlag |= ProgrammingCRT2 ;
    RefreshRateTableIndex = XGI_GetRatePtrCRT2( ModeNo , ModeIdIndex, pVBInfo ) ;
    XGI_GetLVDSResInfo(  ModeNo , ModeIdIndex,  pVBInfo ) ;
    XGI_GetLVDSData( ModeNo , ModeIdIndex , RefreshRateTableIndex,  pVBInfo);
    XGI_ModCRT1Regs( ModeNo , ModeIdIndex , RefreshRateTableIndex , HwDeviceExtension, pVBInfo ) ;
    XGI_SetLVDSRegs( ModeNo , ModeIdIndex , RefreshRateTableIndex, pVBInfo ) ;
    XGI_SetCRT2ECLK( ModeNo , ModeIdIndex , RefreshRateTableIndex, pVBInfo ) ;
}


/* --------------------------------------------------------------------- */
/* Function : XGI_GetLVDSResInfo */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGI_GetLVDSResInfo( USHORT ModeNo , USHORT ModeIdIndex,PVB_DEVICE_INFO  pVBInfo )
{
    USHORT resindex , xres , yres , modeflag ;

    if ( ModeNo <= 0x13 )
    {
        modeflag = pVBInfo->SModeIDTable[ ModeIdIndex ].St_ResInfo ;	/* si+St_ResInfo */
    }
    else
    {
        modeflag = pVBInfo->EModeIDTable[ ModeIdIndex ].Ext_RESINFO ;	/* si+Ext_ResInfo */
    }


    /* if ( ModeNo > 0x13 ) */
    /* modeflag = pVBInfo->EModeIDTable[ ModeIdIndex ].Ext_ModeFlag ; */
    /* else */
    /* modeflag = pVBInfo->SModeIDTable[ ModeIdIndex ].St_ModeFlag ; */

    if ( ModeNo <= 0x13 )
    {
        resindex = pVBInfo->SModeIDTable[ ModeIdIndex ].St_ResInfo ;	/* si+St_ResInfo */
    }
    else
    {
        resindex = pVBInfo->EModeIDTable[ ModeIdIndex ].Ext_RESINFO ;	/* si+Ext_ResInfo */
    }

    /* resindex = XGI_GetResInfo( ModeNo , ModeIdIndex, pVBInfo ) ; */

    if ( ModeNo <= 0x13 )
    {
        xres = pVBInfo->StResInfo[ resindex ].HTotal ;
        yres = pVBInfo->StResInfo[ resindex ].VTotal ;
    }
    else
    {
        xres = pVBInfo->ModeResInfo[ resindex ].HTotal ;
        yres = pVBInfo->ModeResInfo[ resindex ].VTotal ;
    }
    if ( ModeNo > 0x13 )
    {
        if ( modeflag & HalfDCLK )
            xres = xres << 1 ;

        if ( modeflag & DoubleScanMode )
            yres = yres << 1 ;
    }
    /* if ( modeflag & Charx8Dot ) */
    /* { */

    if ( xres == 720 )
        xres = 640 ;

    /* } */
    pVBInfo->VGAHDE = xres ;
    pVBInfo->HDE = xres ;
    pVBInfo->VGAVDE = yres ;
    pVBInfo->VDE = yres ;
}


/* --------------------------------------------------------------------- */
/* Function : XGI_GetLVDSData */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGI_GetLVDSData(  USHORT ModeNo , USHORT ModeIdIndex , USHORT RefreshRateTableIndex, PVB_DEVICE_INFO  pVBInfo )
{
    USHORT tempbx ;
    XGI330_LVDSDataStruct *LCDPtr = NULL ;
    XGI330_CHTVDataStruct  *TVPtr = NULL ;

    tempbx = 2 ;

    if ( pVBInfo->VBInfo & ( SetCRT2ToLCD | SetCRT2ToLCDA ) )
    {
        LCDPtr = ( XGI330_LVDSDataStruct * )XGI_GetLcdPtr( tempbx, ModeNo , ModeIdIndex , RefreshRateTableIndex, pVBInfo) ;
        pVBInfo->VGAHT = LCDPtr->VGAHT ;
        pVBInfo->VGAVT = LCDPtr->VGAVT ;
        pVBInfo->HT = LCDPtr->LCDHT ;
        pVBInfo->VT = LCDPtr->LCDVT ;
    }
    if ( pVBInfo->IF_DEF_CH7017 == 1 )
    {
        if ( pVBInfo->VBInfo & SetCRT2ToTV )
        {
            TVPtr = ( XGI330_CHTVDataStruct * )XGI_GetTVPtr( tempbx , ModeNo , ModeIdIndex , RefreshRateTableIndex, pVBInfo ) ;
            pVBInfo->VGAHT = TVPtr->VGAHT ;
            pVBInfo->VGAVT = TVPtr->VGAVT ;
            pVBInfo->HT = TVPtr->LCDHT ;
            pVBInfo->VT = TVPtr->LCDVT ;
        }
    }

    if ( pVBInfo->VBInfo & ( SetCRT2ToLCD | SetCRT2ToLCDA ) )
    {
        if ( !( pVBInfo->LCDInfo & ( SetLCDtoNonExpanding | EnableScalingLCD ) ) )
        {
            if ( ( pVBInfo->LCDResInfo == Panel1024x768 ) || ( pVBInfo->LCDResInfo == Panel1024x768x75 ) )
            {
                pVBInfo->HDE = 1024 ;
                pVBInfo->VDE = 768 ;
            }
            else if ( ( pVBInfo->LCDResInfo == Panel1280x1024 ) || ( pVBInfo->LCDResInfo == Panel1280x1024x75 ) )
            {
                pVBInfo->HDE = 1280 ;
                pVBInfo->VDE = 1024 ;
            }
            else if ( pVBInfo->LCDResInfo == Panel1400x1050 )
            {
                pVBInfo->HDE = 1400 ;
                pVBInfo->VDE = 1050 ;
            }
            else
            {
                pVBInfo->HDE = 1600 ;
                pVBInfo->VDE = 1200 ;
            }
        }
    }
}


/* --------------------------------------------------------------------- */
/* Function : XGI_ModCRT1Regs */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGI_ModCRT1Regs( USHORT ModeNo , USHORT ModeIdIndex ,
                        USHORT RefreshRateTableIndex , PXGI_HW_DEVICE_INFO HwDeviceExtension,PVB_DEVICE_INFO  pVBInfo )
{
    UCHAR index ;
    USHORT tempbx , i ;
    XGI_LVDSCRT1HDataStruct  *LCDPtr = NULL ;
    XGI_LVDSCRT1VDataStruct  *LCDPtr1 =NULL ;
    /* XGI330_CHTVDataStruct *TVPtr = NULL ; */

    if( ModeNo <= 0x13 )
        index = pVBInfo->SModeIDTable[ ModeIdIndex ].St_CRT2CRTC ;
    else
        index = pVBInfo->RefIndex[RefreshRateTableIndex].Ext_CRT2CRTC;

    index= index & IndexMask ;

    if ( ( pVBInfo->IF_DEF_ScaleLCD == 0 ) || ( ( pVBInfo->IF_DEF_ScaleLCD == 1 ) && ( !( pVBInfo->LCDInfo & EnableScalingLCD ) ) ) )
    {
        tempbx = 0 ;

        if ( pVBInfo->VBInfo & ( SetCRT2ToLCD | SetCRT2ToLCDA ) )
        {
            LCDPtr = ( XGI_LVDSCRT1HDataStruct * )XGI_GetLcdPtr( tempbx , ModeNo , ModeIdIndex , RefreshRateTableIndex, pVBInfo ) ;

            for( i = 0 ; i < 8 ; i++ )
                pVBInfo->TimingH[ 0 ].data[ i ] = LCDPtr[ 0 ].Reg[ i ] ;
        }

        /* if ( pVBInfo->IF_DEF_CH7017 == 1 )
        {
            if ( pVBInfo->VBInfo & SetCRT2ToTV )
                TVPtr = ( XGI330_CHTVDataStruct *)XGI_GetTVPtr( tempbx , ModeNo , ModeIdIndex , RefreshRateTableIndex, pVBInfo ) ;
        } */

        XGI_SetCRT1Timing_H(pVBInfo,HwDeviceExtension) ;

        tempbx = 1 ;

        if ( pVBInfo->VBInfo & ( SetCRT2ToLCD | SetCRT2ToLCDA ) )
        {
            LCDPtr1 = ( XGI_LVDSCRT1VDataStruct * )XGI_GetLcdPtr( tempbx , ModeNo , ModeIdIndex , RefreshRateTableIndex, pVBInfo ) ;
            for( i = 0 ; i < 7 ; i++ )
                pVBInfo->TimingV[ 0 ].data[ i ] = LCDPtr1[ 0 ].Reg[ i ] ;
        }

        /* if ( pVBInfo->IF_DEF_CH7017 == 1 )
        {
            if ( pVBInfo->VBInfo & SetCRT2ToTV )
                TVPtr = ( XGI330_CHTVDataStruct *)XGI_GetTVPtr( tempbx , ModeNo , ModeIdIndex , RefreshRateTableIndex, pVBInfo ) ;
        } */

        XGI_SetCRT1Timing_V( ModeIdIndex , ModeNo , pVBInfo) ;
    }
}



/* --------------------------------------------------------------------- */
/* Function : XGI_SetLVDSRegs */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGI_SetLVDSRegs( USHORT ModeNo , USHORT ModeIdIndex , USHORT RefreshRateTableIndex, PVB_DEVICE_INFO  pVBInfo )
{
    USHORT tempbx , tempax , tempcx , tempdx , push1 , push2 , modeflag ;
    unsigned long temp , temp1 , temp2 , temp3 , push3 ;
    XGI330_LCDDataDesStruct  *LCDPtr = NULL ;
    XGI330_LCDDataDesStruct2  *LCDPtr1 = NULL ;

    if ( ModeNo > 0x13 )
        modeflag = pVBInfo->EModeIDTable[ ModeIdIndex ].Ext_ModeFlag ;
    else
        modeflag = pVBInfo->SModeIDTable[ ModeIdIndex ].St_ModeFlag ;

    if ( !( pVBInfo->SetFlag & Win9xDOSMode ) )
    {
        if ( ( pVBInfo->IF_DEF_CH7017 == 0 ) || ( pVBInfo->VBInfo & ( SetCRT2ToLCD | SetCRT2ToLCDA ) ) )
        {
            if ( pVBInfo->IF_DEF_OEMUtil == 1 )
            {
    	        tempbx = 8 ;
    	        LCDPtr = ( XGI330_LCDDataDesStruct * )XGI_GetLcdPtr( tempbx , ModeNo , ModeIdIndex , RefreshRateTableIndex, pVBInfo ) ;
            }

            if ( ( pVBInfo->IF_DEF_OEMUtil == 0 ) || ( LCDPtr == 0 ) )
            {
                tempbx = 3 ;
                if ( pVBInfo->LCDInfo & EnableScalingLCD )
                    LCDPtr1 = ( XGI330_LCDDataDesStruct2 * )XGI_GetLcdPtr( tempbx , ModeNo , ModeIdIndex , RefreshRateTableIndex, pVBInfo ) ;
                else
                    LCDPtr = ( XGI330_LCDDataDesStruct * )XGI_GetLcdPtr( tempbx , ModeNo , ModeIdIndex , RefreshRateTableIndex, pVBInfo ) ;
            }

            XGI_GetLCDSync( &tempax , &tempbx ,pVBInfo) ;
            push1 = tempbx ;
            push2 = tempax ;

            /* GetLCDResInfo */
            if ( ( pVBInfo->LCDResInfo == Panel1024x768 ) || ( pVBInfo->LCDResInfo == Panel1024x768x75 ) )
            {
                tempax = 1024 ;
                tempbx = 768 ;
            }
            else if ( ( pVBInfo->LCDResInfo == Panel1280x1024 ) || ( pVBInfo->LCDResInfo == Panel1280x1024x75 ) )
            {
                tempax = 1280 ;
                tempbx = 1024 ;
            }
            else if ( pVBInfo->LCDResInfo == Panel1400x1050 )
            {
                tempax = 1400 ;
                tempbx = 1050 ;
            }
            else
            {
                tempax = 1600 ;
                tempbx = 1200 ;
            }

            if ( pVBInfo->LCDInfo & SetLCDtoNonExpanding )
            {
                pVBInfo->HDE=tempax;
                pVBInfo->VDE=tempbx;
                pVBInfo->VGAHDE=tempax;
                pVBInfo->VGAVDE=tempbx;
            }

            if ( ( pVBInfo->IF_DEF_ScaleLCD == 1 ) && ( pVBInfo->LCDInfo & EnableScalingLCD ) )
            {
                tempax=pVBInfo->HDE;
                tempbx=pVBInfo->VDE;
            }

            tempax = pVBInfo->HT ;

            if ( pVBInfo->LCDInfo & EnableScalingLCD )
                tempbx = LCDPtr1->LCDHDES ;
            else
                tempbx = LCDPtr->LCDHDES ;

            tempcx = pVBInfo->HDE ;
            tempbx = tempbx & 0x0fff ;
            tempcx += tempbx ;

            if ( tempcx >= tempax )
                tempcx -= tempax ;

            XGINew_SetReg1( pVBInfo->Part1Port , 0x1A , tempbx & 0x07 ) ;

            tempcx = tempcx >> 3 ;
            tempbx = tempbx >> 3 ;

            XGINew_SetReg1( pVBInfo->Part1Port , 0x16 , ( USHORT )( tempbx & 0xff ) ) ;
            XGINew_SetReg1( pVBInfo->Part1Port , 0x17 , ( USHORT )( tempcx & 0xff ) ) ;

            tempax = pVBInfo->HT ;

            if ( pVBInfo->LCDInfo & EnableScalingLCD )
                tempbx = LCDPtr1->LCDHRS ;
            else
                tempbx = LCDPtr->LCDHRS ;

            tempcx = push2 ;

            if ( pVBInfo->LCDInfo & EnableScalingLCD )
                tempcx = LCDPtr1->LCDHSync ;

            tempcx += tempbx ;

            if ( tempcx >= tempax )
                tempcx -= tempax ;

            tempax = tempbx & 0x07 ;
            tempax = tempax >> 5 ;
            tempcx = tempcx >> 3 ;
            tempbx = tempbx >> 3 ;

            tempcx &= 0x1f ;
            tempax |= tempcx ;

            XGINew_SetReg1( pVBInfo->Part1Port , 0x15 , tempax ) ;
            XGINew_SetReg1( pVBInfo->Part1Port , 0x14 , ( USHORT )( tempbx & 0xff ) ) ;

            tempax = pVBInfo->VT ;
            if ( pVBInfo->LCDInfo & EnableScalingLCD )
                tempbx = LCDPtr1->LCDVDES ;
            else
                tempbx = LCDPtr->LCDVDES ;
            tempcx = pVBInfo->VDE ;

            tempbx = tempbx & 0x0fff ;
            tempcx += tempbx ;
            if ( tempcx >= tempax )
                tempcx -= tempax ;

            XGINew_SetReg1( pVBInfo->Part1Port , 0x1b , ( USHORT )( tempbx & 0xff ) ) ;
            XGINew_SetReg1( pVBInfo->Part1Port , 0x1c , ( USHORT )( tempcx & 0xff ) ) ;

            tempbx = ( tempbx >> 8 ) & 0x07 ;
            tempcx = ( tempcx >> 8 ) & 0x07 ;

            XGINew_SetReg1( pVBInfo->Part1Port , 0x1d , ( USHORT )( ( tempcx << 3 ) | tempbx ) ) ;

            tempax = pVBInfo->VT ;
            if ( pVBInfo->LCDInfo & EnableScalingLCD )
                tempbx = LCDPtr1->LCDVRS ;
            else
                tempbx = LCDPtr->LCDVRS ;

            /* tempbx = tempbx >> 4 ; */
              tempcx = push1 ;

            if ( pVBInfo->LCDInfo & EnableScalingLCD )
                tempcx = LCDPtr1->LCDVSync ;

            tempcx += tempbx ;
            if ( tempcx >= tempax )
                tempcx -= tempax ;

            XGINew_SetReg1( pVBInfo->Part1Port , 0x18 , ( USHORT )( tempbx & 0xff ) ) ;
            XGINew_SetRegANDOR( pVBInfo->Part1Port , 0x19 , ~0x0f , ( USHORT )( tempcx & 0x0f ) ) ;

            tempax = ( ( tempbx >> 8 ) & 0x07 ) << 3 ;

            tempbx = pVBInfo->VGAVDE ;
            if ( tempbx != pVBInfo->VDE )
                tempax |= 0x40 ;

            if ( pVBInfo->LCDInfo & EnableLVDSDDA )
                tempax |= 0x40 ;

            XGINew_SetRegANDOR( pVBInfo->Part1Port , 0x1a , 0x07 , tempax ) ;

            tempcx = pVBInfo->VGAVT ;
            tempbx = pVBInfo->VDE ;
            tempax = pVBInfo->VGAVDE ;
            tempcx -= tempax ;

            temp = tempax ;            /* 0430 ylshieh */
            temp1 = ( temp << 18 ) / tempbx ;

            tempdx = ( USHORT )( ( temp << 18 ) % tempbx ) ;

            if ( tempdx != 0 )
            temp1 += 1 ;

            temp2 = temp1 ;
            push3 = temp2 ;

            XGINew_SetReg1( pVBInfo->Part1Port , 0x37 , ( USHORT )( temp2 & 0xff ) ) ;
            XGINew_SetReg1( pVBInfo->Part1Port , 0x36 , ( USHORT )( ( temp2 >> 8 ) & 0xff ) ) ;

            tempbx = ( USHORT )( temp2 >> 16 ) ;
            tempax = tempbx & 0x03 ;

            tempbx = pVBInfo->VGAVDE ;
            if ( tempbx == pVBInfo->VDE )
                tempax |= 0x04 ;

            XGINew_SetReg1( pVBInfo->Part1Port , 0x35 , tempax ) ;

            if ( pVBInfo->VBType & VB_XGI301C )
            {
                temp2 = push3 ;
      	        XGINew_SetReg1( pVBInfo->Part4Port , 0x3c , ( USHORT )( temp2 & 0xff ) ) ;
      	        XGINew_SetReg1( pVBInfo->Part4Port , 0x3b , ( USHORT )( ( temp2 >> 8 ) & 0xff ) ) ;
      	        tempbx = ( USHORT )( temp2 >> 16 ) ;
      	        XGINew_SetRegANDOR( pVBInfo->Part4Port , 0x3a , ~0xc0 , ( USHORT )( ( tempbx & 0xff ) << 6 ) ) ;

                tempcx = pVBInfo->VGAVDE ;
                if ( tempcx == pVBInfo->VDE )
                    XGINew_SetRegANDOR( pVBInfo->Part4Port , 0x30 , ~0x0c , 0x00 ) ;
                else
                    XGINew_SetRegANDOR( pVBInfo->Part4Port , 0x30 , ~0x0c , 0x08 ) ;
            }

            tempcx = pVBInfo->VGAHDE ;
            tempbx = pVBInfo->HDE ;

            temp1 = tempcx << 16 ;

            tempax = ( USHORT )( temp1 / tempbx ) ;

            if ( ( tempbx & 0xffff ) == ( tempcx & 0xffff ) )
                tempax = 65535 ;

            temp3 = tempax ;
            temp1 = pVBInfo->VGAHDE << 16 ;

            temp1 /= temp3 ;
            temp3 = temp3 << 16 ;
            temp1 -= 1 ;

            temp3 = ( temp3 & 0xffff0000 ) + ( temp1 & 0xffff ) ;

            tempax = ( USHORT )( temp3 & 0xff ) ;
            XGINew_SetReg1( pVBInfo->Part1Port , 0x1f , tempax ) ;

            temp1 = pVBInfo->VGAVDE << 18 ;
            temp1 = temp1 / push3 ;
            tempbx = ( USHORT )( temp1 & 0xffff ) ;

            if ( pVBInfo->LCDResInfo == Panel1024x768 )
                tempbx -= 1 ;

            tempax = ( ( tempbx >> 8 ) & 0xff ) << 3 ;
            tempax |= ( USHORT )( ( temp3 >> 8 ) & 0x07 ) ;
            XGINew_SetReg1( pVBInfo->Part1Port , 0x20 , ( USHORT )( tempax & 0xff ) ) ;
            XGINew_SetReg1( pVBInfo->Part1Port , 0x21 , ( USHORT )( tempbx & 0xff ) ) ;

            temp3 = temp3 >> 16 ;

            if ( modeflag & HalfDCLK )
                temp3 = temp3 >> 1 ;

            XGINew_SetReg1(pVBInfo->Part1Port , 0x22 , ( USHORT )( ( temp3 >> 8 ) & 0xff ) ) ;
            XGINew_SetReg1(pVBInfo->Part1Port , 0x23 , ( USHORT )( temp3 & 0xff ) ) ;
        }
    }
}


/* --------------------------------------------------------------------- */
/* Function : XGI_SetCRT2ECLK */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGI_SetCRT2ECLK( USHORT ModeNo , USHORT ModeIdIndex , USHORT RefreshRateTableIndex, PVB_DEVICE_INFO  pVBInfo )
{
    UCHAR di_0 , di_1 , tempal ;
    int i ;

    tempal = XGI_GetVCLKPtr( RefreshRateTableIndex , ModeNo , ModeIdIndex, pVBInfo ) ;
    XGI_GetVCLKLen( tempal , &di_0 , &di_1, pVBInfo ) ;
    XGI_GetLCDVCLKPtr( &di_0 , &di_1, pVBInfo ) ;

    for( i = 0 ; i < 4 ; i++ )
    {
        XGINew_SetRegANDOR( pVBInfo->P3d4 , 0x31 , ~0x30 , ( USHORT )( 0x10 * i ) ) ;
        if ( ( !( pVBInfo->VBInfo & SetCRT2ToLCDA ) ) && ( !( pVBInfo->VBInfo & SetInSlaveMode ) ) )
        {
            XGINew_SetReg1( pVBInfo->P3c4 , 0x2e , di_0 ) ;
            XGINew_SetReg1( pVBInfo->P3c4 , 0x2f , di_1 ) ;
        }
        else
        {
            XGINew_SetReg1( pVBInfo->P3c4 , 0x2b , di_0 ) ;
            XGINew_SetReg1( pVBInfo->P3c4 , 0x2c , di_1 ) ;
        }
    }
}


/* --------------------------------------------------------------------- */
/* Function : XGI_UpdateModeInfo */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGI_UpdateModeInfo( PXGI_HW_DEVICE_INFO HwDeviceExtension,PVB_DEVICE_INFO  pVBInfo )
{
    USHORT tempcl ,
           tempch ,
           temp ,
           tempbl ,
           tempax ;

    if ( pVBInfo->VBType & ( VB_XGI301B | VB_XGI302B | VB_XGI301LV | VB_XGI302LV | VB_XGI301C ) )
    {
        tempcl = 0 ;
        tempch = 0 ;
        temp = XGINew_GetReg1( pVBInfo->P3c4 , 0x01 ) ;

        if ( !( temp & 0x20 ) )
        {
            temp = XGINew_GetReg1( pVBInfo->P3d4 , 0x17 ) ;
            if ( temp & 0x80 )
            {
                if ( ( HwDeviceExtension->jChipType == XG20 ) || ( HwDeviceExtension->jChipType >= XG40 ) )
                    temp = XGINew_GetReg1( pVBInfo->P3d4 , 0x53 ) ;
                else
                    temp = XGINew_GetReg1( pVBInfo->P3d4 , 0x63 ) ;

                if ( !( temp & 0x40 ) )
                    tempcl |= ActiveCRT1 ;
            }
        }

        temp = XGINew_GetReg1( pVBInfo->Part1Port , 0x2e ) ;
        temp &= 0x0f ;

        if ( !( temp == 0x08 ) )
        {
            tempax = XGINew_GetReg1( pVBInfo->Part1Port , 0x13 ) ;	/* Check ChannelA by Part1_13 [2003/10/03] */
            if ( tempax & 0x04 )
               	tempcl = tempcl | ActiveLCD ;

            temp &= 0x05 ;

            if ( !( tempcl & ActiveLCD ) )
            	if ( temp == 0x01 )
                    tempcl |= ActiveCRT2 ;

            if ( temp == 0x04 )
                tempcl |= ActiveLCD ;

            if ( temp == 0x05 )
            {
                temp = XGINew_GetReg1( pVBInfo->Part2Port , 0x00 ) ;

                if( !( temp & 0x08 ) )
                    tempch |= ActiveAVideo ;

                if ( !( temp & 0x04 ) )
                    tempch |= ActiveSVideo ;

                if ( temp & 0x02 )
                    tempch |= ActiveSCART ;

                if ( pVBInfo->VBInfo & SetCRT2ToHiVisionTV )
                {
                    if ( temp & 0x01 )
                        tempch |= ActiveHiTV ;
                }

                if ( pVBInfo->VBInfo & SetCRT2ToYPbPr )
                {
                    temp = XGINew_GetReg1( pVBInfo->Part2Port , 0x4d ) ;

                    if ( temp & 0x10 )
                        tempch |= ActiveYPbPr ;
                }

                if ( tempch != 0 )
                    tempcl |= ActiveTV ;
            }
        }

        temp = XGINew_GetReg1( pVBInfo->P3d4 , 0x3d ) ;
        if ( tempcl & ActiveLCD )
        {
            if ( ( pVBInfo->SetFlag & ReserveTVOption ) )
            {
                if ( temp & ActiveTV )
                    tempcl |= ActiveTV ;
            }
        }
        temp = tempcl ;
        tempbl = ~ModeSwitchStatus ;
        XGINew_SetRegANDOR( pVBInfo->P3d4 , 0x3d , tempbl , temp ) ;

        if ( !( pVBInfo->SetFlag & ReserveTVOption ) )
            XGINew_SetReg1( pVBInfo->P3d4 , 0x3e , tempch ) ;
    }
    else
    {
        return ;
    }
}


/* --------------------------------------------------------------------- */
/* Function : XGI_GetVGAType */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGI_GetVGAType( PXGI_HW_DEVICE_INFO HwDeviceExtension, PVB_DEVICE_INFO  pVBInfo)
{
/*    USHORT tempbx ,
           tempah ; */

    if ( HwDeviceExtension->jChipType == XG20 )
    {
        pVBInfo->Set_VGAType = XG20;
    }
    else if ( HwDeviceExtension->jChipType >= XG40 )
    {
        pVBInfo->Set_VGAType = VGA_XGI340 ;
    }


}


/* --------------------------------------------------------------------- */
/* Function : XGI_GetVBType */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGI_GetVBType(PVB_DEVICE_INFO  pVBInfo)
{
    USHORT flag , tempbx , tempah ;

    if ( pVBInfo->IF_DEF_LVDS == 0 )	
    {
        tempbx = VB_XGI302B ;
        flag = XGINew_GetReg1( pVBInfo->Part4Port , 0x00 ) ;
        if ( flag != 0x02 )
        {
            tempbx = VB_XGI301 ;
            flag = XGINew_GetReg1( pVBInfo->Part4Port , 0x01 ) ;
            if ( flag >= 0xB0 )
            {
                tempbx = VB_XGI301B ;
                if ( flag >= 0xC0 )
                {
                    tempbx = VB_XGI301C ;
                    if ( flag >= 0xD0 )
                    {
                        tempbx = VB_XGI301LV ;
                        if ( flag >= 0xE0 )
                        {
                            tempbx = VB_XGI302LV ;
                            tempah = XGINew_GetReg1( pVBInfo->Part4Port , 0x39 ) ;
                            if ( tempah != 0xFF )
                                tempbx = VB_XGI301C ;
                        }
                    }
                }

                if ( tempbx & ( VB_XGI301B | VB_XGI302B ) )
                {
                    flag = XGINew_GetReg1( pVBInfo->Part4Port , 0x23 ) ;

                    if ( !( flag & 0x02 ) )
                        tempbx = tempbx | VB_NoLCD ;
                }
            }
        }
        pVBInfo->VBType = tempbx ;
    }
/*
    else if ( pVBInfo->IF_DEF_CH7017 == 1 )
        pVBInfo->VBType = VB_CH7017 ;
    else  //LVDS
        pVBInfo->VBType = VB_LVDS_NS ;
*/
}


/* --------------------------------------------------------------------- */
/* Function : XGI_GetVBInfo */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGI_GetVBInfo( USHORT ModeNo , USHORT ModeIdIndex , PXGI_HW_DEVICE_INFO HwDeviceExtension, PVB_DEVICE_INFO  pVBInfo )
{
    USHORT tempax ,
           push ,
           tempbx ,
           temp ,
           modeflag ;

/*    UCHAR OutputSelect = *pVBInfo->pOutputSelect ;  */

    if ( ModeNo <= 0x13 )
    {
        modeflag = pVBInfo->SModeIDTable[ ModeIdIndex ].St_ModeFlag ;
    }
    else
    {
        modeflag = pVBInfo->EModeIDTable[ ModeIdIndex ].Ext_ModeFlag ;
    }

    pVBInfo->SetFlag = 0 ;
    pVBInfo->ModeType = modeflag & ModeInfoFlag ;
    tempbx = 0 ;

    if ( pVBInfo->VBType & 0xFFFF )
    {
        temp = XGINew_GetReg1( pVBInfo->P3d4 , 0x30 ) ;           /* Check Display Device */
        tempbx = tempbx | temp ;
        temp = XGINew_GetReg1( pVBInfo->P3d4 , 0x31 ) ;
        push = temp ;
        push = push << 8 ;
        tempax = temp << 8 ;
        tempbx = tempbx | tempax ;
        temp = ( SetCRT2ToDualEdge | SetCRT2ToYPbPr | SetCRT2ToLCDA | SetInSlaveMode | DisableCRT2Display ) ;
        temp = 0xFFFF ^ temp ;
        tempbx &= temp ;

        temp = XGINew_GetReg1( pVBInfo->P3d4 , 0x38 ) ;

        if ( pVBInfo->IF_DEF_LCDA == 1 )
        {

            if ( ( pVBInfo->Set_VGAType == XG20 ) || ( pVBInfo->Set_VGAType >= XG40 ))
            {
                if ( pVBInfo->IF_DEF_LVDS == 0 )
                {
                    /* if ( ( pVBInfo->VBType & VB_XGI302B ) || ( pVBInfo->VBType & VB_XGI301LV ) || ( pVBInfo->VBType & VB_XGI302LV ) || ( pVBInfo->VBType & VB_XGI301C ) ) */
                    if ( pVBInfo->VBType & ( VB_XGI302B | VB_XGI301LV | VB_XGI302LV | VB_XGI301C ) )
                    {
                        if ( temp & EnableDualEdge )
                        {
                            tempbx |= SetCRT2ToDualEdge ;

                            if ( temp & SetToLCDA )
                                tempbx |= SetCRT2ToLCDA ;
                        }
                    }
                }
                else if ( pVBInfo->IF_DEF_CH7017 == 1 )
                {
                    if ( pVBInfo->VBType & VB_CH7017 )
                    {
                        if ( temp & EnableDualEdge )
                        {
                            tempbx |= SetCRT2ToDualEdge ;

                            if ( temp & SetToLCDA )
                                tempbx |= SetCRT2ToLCDA ;
                        }
                    }
                }
            }
        }

        if ( pVBInfo->IF_DEF_YPbPr == 1 )
        {
            if ( ( ( pVBInfo->IF_DEF_LVDS == 0 ) && ( ( pVBInfo->VBType & VB_XGI301LV ) || ( pVBInfo->VBType & VB_XGI302LV ) || ( pVBInfo->VBType & VB_XGI301C ) ) )
            || ( ( pVBInfo->IF_DEF_CH7017 == 1 ) && ( pVBInfo->VBType&VB_CH7017 ) ) )
            {
                if ( temp & SetYPbPr )	/* temp = CR38 */
                {
                    if ( pVBInfo->IF_DEF_HiVision == 1 )
                    {
                        temp = XGINew_GetReg1( pVBInfo->P3d4 , 0x35 ) ;  /* shampoo add for new scratch */
                        temp &= YPbPrMode ;
                        tempbx |= SetCRT2ToHiVisionTV ;

                        if ( temp != YPbPrMode1080i ) {
                            tempbx &= ( ~SetCRT2ToHiVisionTV ) ;
                            tempbx |= SetCRT2ToYPbPr ; }
                    }

                    /* tempbx |= SetCRT2ToYPbPr ; */
                }
            }
        }

        tempax = push ;  /* restore CR31 */

        if ( pVBInfo->IF_DEF_LVDS == 0 )	
        {
            if ( pVBInfo->IF_DEF_YPbPr == 1 )
            {
                if ( pVBInfo->IF_DEF_HiVision == 1 )
                    temp = 0x09FC ;
                else
                    temp = 0x097C ;
            }
            else
            {
                if ( pVBInfo->IF_DEF_HiVision == 1 )
                    temp = 0x01FC ;
                else
                    temp = 0x017C ;
            }
        }
        else	/* 3nd party chip */
        {
            if ( pVBInfo->IF_DEF_CH7017 == 1 )
                temp = ( SetCRT2ToTV | SetCRT2ToLCD | SetCRT2ToLCDA ) ;
            else
                temp = SetCRT2ToLCD ;
        }

        if ( !( tempbx & temp ) )
        {
            tempax |= DisableCRT2Display ;
            tempbx = 0 ;
        }

        if ( pVBInfo->IF_DEF_LCDA == 1 )	/* Select Display Device */
        {
            if ( !( pVBInfo->VBType & VB_NoLCD ) )
            {
                if ( tempbx & SetCRT2ToLCDA )
                {
                    if ( tempbx & SetSimuScanMode )
                        tempbx &= ( ~( SetCRT2ToLCD | SetCRT2ToRAMDAC | SwitchToCRT2 ) ) ;
                    else
                        tempbx &= ( ~( SetCRT2ToLCD | SetCRT2ToRAMDAC | SetCRT2ToTV | SwitchToCRT2 ) ) ;
                }
            }
        }

        /* shampoo add */
        if ( !( tempbx & ( SwitchToCRT2 | SetSimuScanMode ) ) )	/* for driver abnormal */
        {
            if ( pVBInfo->IF_DEF_CRT2Monitor == 1 )
            {
                if ( tempbx & SetCRT2ToRAMDAC )
                {
                    tempbx &= ( 0xFF00 | SetCRT2ToRAMDAC | SwitchToCRT2 | SetSimuScanMode ) ;
                    tempbx &= ( 0x00FF | ( ~SetCRT2ToYPbPr ) ) ;
                }
            }
            else
                tempbx &= ( ~( SetCRT2ToRAMDAC | SetCRT2ToLCD | SetCRT2ToTV ) ) ;
        }

        if ( !( pVBInfo->VBType & VB_NoLCD ) )
        {
            if ( tempbx & SetCRT2ToLCD )
            {
                tempbx &= ( 0xFF00 | SetCRT2ToLCD | SwitchToCRT2 | SetSimuScanMode ) ;
                tempbx &= ( 0x00FF | ( ~SetCRT2ToYPbPr ) ) ;
            }
        }

        if ( tempbx & SetCRT2ToSCART )
        {
            tempbx &= ( 0xFF00 | SetCRT2ToSCART | SwitchToCRT2 | SetSimuScanMode ) ;
            tempbx &= ( 0x00FF | ( ~SetCRT2ToYPbPr ) ) ;
        }

        if ( pVBInfo->IF_DEF_YPbPr == 1 )
        {
            if ( tempbx & SetCRT2ToYPbPr )
                tempbx &= ( 0xFF00 | SwitchToCRT2 | SetSimuScanMode ) ;
        }

        if ( pVBInfo->IF_DEF_HiVision == 1 )
        {
            if ( tempbx & SetCRT2ToHiVisionTV )
                tempbx &= ( 0xFF00 | SetCRT2ToHiVisionTV | SwitchToCRT2 | SetSimuScanMode ) ;
        }

        if ( tempax & DisableCRT2Display )	/* Set Display Device Info */
        {
            if ( !( tempbx & ( SwitchToCRT2 | SetSimuScanMode ) ) )
                tempbx = DisableCRT2Display ;
        }

        if ( !( tempbx & DisableCRT2Display ) )
        {
            if ( ( !( tempbx & DriverMode ) ) || ( !( modeflag & CRT2Mode ) ) )
            {
                if ( pVBInfo->IF_DEF_LCDA == 1 )
      	        {
                    if ( !( tempbx & SetCRT2ToLCDA ) )
                        tempbx |= ( SetInSlaveMode | SetSimuScanMode ) ;
                }

                if ( pVBInfo->IF_DEF_VideoCapture == 1 )
                {
                    if ( ( ( HwDeviceExtension->jChipType == XG40 ) && ( pVBInfo->Set_VGAType == XG40 ) )
                    || ( ( HwDeviceExtension->jChipType == XG41 ) && ( pVBInfo->Set_VGAType == XG41 ) )
                    || ( ( HwDeviceExtension->jChipType == XG42 ) && ( pVBInfo->Set_VGAType == XG42 ) )
                    || ( ( HwDeviceExtension->jChipType == XG45 ) && ( pVBInfo->Set_VGAType == XG45 ) ) )
                    {
                        if ( ModeNo <= 13 )
                        {
                            if ( !( tempbx & SetCRT2ToRAMDAC ) )	/*CRT2 not need to support*/
                            {
                                tempbx &= ( 0x00FF | ( ~SetInSlaveMode ) ) ;
                                pVBInfo->SetFlag |= EnableVCMode ;
                            }
                        }
                    }
                }
            }

            /*LCD+TV can't support in slave mode (Force LCDA+TV->LCDB)*/
            if ( ( tempbx & SetInSlaveMode ) && ( tempbx & SetCRT2ToLCDA ) )
            {
                tempbx ^= ( SetCRT2ToLCD | SetCRT2ToLCDA | SetCRT2ToDualEdge ) ;
                pVBInfo->SetFlag |= ReserveTVOption ;
            }
        }
    }

    pVBInfo->VBInfo = tempbx ;
}


/* --------------------------------------------------------------------- */
/* Function : XGI_GetTVInfo */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGI_GetTVInfo( USHORT ModeNo , USHORT ModeIdIndex ,PVB_DEVICE_INFO  pVBInfo )
{
    USHORT temp ,
           tempbx = 0 ,
           resinfo = 0 ,
           modeflag ,
           index1 ;
/*           temp2 ,
           OutputSelect = *pVBInfo->pOutputSelect ; */

    tempbx = 0 ;
    resinfo = 0 ;

    if ( pVBInfo->VBInfo & SetCRT2ToTV )
    {
        if ( ModeNo <= 0x13 )
        {
            modeflag = pVBInfo->SModeIDTable[ ModeIdIndex ].St_ModeFlag ;	/* si+St_ModeFlag */
            resinfo = pVBInfo->SModeIDTable[ ModeIdIndex ].St_ResInfo ;	/* si+St_ResInfo */
        }
        else
        {
            modeflag = pVBInfo->EModeIDTable[ ModeIdIndex ].Ext_ModeFlag ;
            resinfo = pVBInfo->EModeIDTable[ ModeIdIndex ].Ext_RESINFO ; /* si+Ext_ResInfo */
        }

        if ( pVBInfo->VBInfo & SetCRT2ToTV )
        {
            temp = XGINew_GetReg1( pVBInfo->P3d4 , 0x35 ) ;
            tempbx = temp;
            if ( tempbx & SetPALTV )
            {
                tempbx &= ( SetCHTVOverScan | SetPALMTV | SetPALNTV | SetPALTV ) ;
                if ( tempbx & SetPALMTV )
                    tempbx &= ~SetPALTV ; /* set to NTSC if PAL-M */
            }
            else
                tempbx &= ( SetCHTVOverScan | SetNTSCJ | SetPALTV ) ;
/*
            if ( pVBInfo->IF_DEF_LVDS == 0 )
            {
                index1 = XGINew_GetReg1( pVBInfo->P3d4 , 0x38 ) ; //PAL-M/PAL-N Info
                temp2 = ( index1 & 0xC0 ) >> 5 ; 	        //00:PAL, 01:PAL-M, 10:PAL-N
                tempbx |= temp2 ;
                if ( temp2 & 0x02 )          //PAL-M
                    tempbx &= ( ~SetPALTV ) ;
            }
*/
        }

        if ( pVBInfo->IF_DEF_CH7017 == 1 )
        {
            tempbx = XGINew_GetReg1( pVBInfo->P3d4 , 0x35 ) ;

            if ( tempbx & TVOverScan )
                tempbx |= SetCHTVOverScan ;
        }

        if ( pVBInfo->IF_DEF_LVDS == 0 )
        {
            if ( pVBInfo->VBInfo & SetCRT2ToSCART )
                tempbx |= SetPALTV ;
        }

        if ( pVBInfo->IF_DEF_YPbPr == 1 )
        {
            if ( pVBInfo->VBInfo & SetCRT2ToYPbPr )
            {
                index1 = XGINew_GetReg1( pVBInfo->P3d4 , 0x35 ) ;
                index1 &= YPbPrMode ;

                if ( index1 == YPbPrMode525i )
                    tempbx |= SetYPbPrMode525i ;

                if ( index1 == YPbPrMode525p )
                    tempbx = tempbx | SetYPbPrMode525p;
                if ( index1 == YPbPrMode750p)
                    tempbx = tempbx | SetYPbPrMode750p;
            }
        }

        if ( pVBInfo->IF_DEF_HiVision == 1 )
        {
            if ( pVBInfo->VBInfo & SetCRT2ToHiVisionTV )
            {
                tempbx = tempbx | SetYPbPrMode1080i | SetPALTV ;
            }
        }

        if ( pVBInfo->IF_DEF_LVDS == 0 )
        {      /* shampoo */
            if ( ( pVBInfo->VBInfo & SetInSlaveMode ) && ( !( pVBInfo->VBInfo & SetNotSimuMode ) ) )
                tempbx |= TVSimuMode ;

            if ( !( tempbx & SetPALTV ) && ( modeflag > 13 ) && ( resinfo == 8 ) ) /* NTSC 1024x768, */
                tempbx |= NTSC1024x768 ;

            tempbx |= RPLLDIV2XO ;

            if ( pVBInfo->VBInfo & SetCRT2ToHiVisionTV )
            {
                if ( pVBInfo->VBInfo & SetInSlaveMode )
                    tempbx &=( ~RPLLDIV2XO ) ;
            }
            else
            {
                if ( tempbx & ( SetYPbPrMode525p | SetYPbPrMode750p ) )
                    tempbx &= ( ~RPLLDIV2XO ) ;
                else if ( !( pVBInfo->VBType & ( VB_XGI301B | VB_XGI302B | VB_XGI301LV | VB_XGI302LV | VB_XGI301C ) ) )
                {
                    if ( tempbx & TVSimuMode )
                        tempbx &= ( ~RPLLDIV2XO ) ;
                }
            }
        }
    }
    pVBInfo->TVInfo = tempbx ;
}


/* --------------------------------------------------------------------- */
/* Function : XGI_GetLCDInfo */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
BOOLEAN XGI_GetLCDInfo( USHORT ModeNo , USHORT ModeIdIndex, PVB_DEVICE_INFO pVBInfo)
{
    USHORT temp ,
           tempax ,
           tempbx ,
           modeflag ,
           resinfo = 0 ,
           LCDIdIndex ;

    pVBInfo->LCDResInfo = 0 ;
    pVBInfo->LCDTypeInfo = 0 ;
    pVBInfo->LCDInfo = 0 ;

    if ( ModeNo <= 0x13 )
    {
        modeflag = pVBInfo->SModeIDTable[ ModeIdIndex ].St_ModeFlag ;  /* si+St_ModeFlag // */
    }
    else
    {
        modeflag = pVBInfo->EModeIDTable[ ModeIdIndex ].Ext_ModeFlag ;
        resinfo = pVBInfo->EModeIDTable[ ModeIdIndex ].Ext_RESINFO ; /* si+Ext_ResInfo// */
    }

    temp = XGINew_GetReg1( pVBInfo->P3d4 , 0x36 ) ; /* Get LCD Res.Info */
    tempbx = temp & 0x0F ;

    if ( tempbx == 0 )
        tempbx = Panel1024x768 ; /* default */

    /* LCD75 [2003/8/22] Vicent */
    if ( ( tempbx == Panel1024x768 ) || ( tempbx == Panel1280x1024 ) )
    {
        if ( pVBInfo->VBInfo & DriverMode )
        {
            tempax = XGINew_GetReg1( pVBInfo->P3d4 , 0x33 ) ;
            if ( pVBInfo->VBInfo & SetCRT2ToLCDA )
                tempax &= 0x0F ;
            else
                tempax = tempax >> 4 ;

            if ( ( resinfo == 6 ) || ( resinfo == 9 ) )
            {
                if ( tempax >= 3 )
      	            tempbx |= PanelRef75Hz ;
            }
            else if ( ( resinfo == 7 ) || ( resinfo == 8 ) )
            {
      	        if ( tempax >= 4 )
      	            tempbx |= PanelRef75Hz ;
            }
        }
    }

    pVBInfo->LCDResInfo = tempbx ;

    /* End of LCD75 */

    if( pVBInfo->IF_DEF_OEMUtil == 1 )
    {
        pVBInfo->LCDTypeInfo = ( temp & 0xf0 ) >> 4 ;
    }

    if ( !( pVBInfo->VBInfo & ( SetCRT2ToLCD | SetCRT2ToLCDA ) ) )
    {
        return 0;
    }

    tempbx = 0 ;

    temp = XGINew_GetReg1( pVBInfo->P3d4 , 0x37 ) ;

    temp &= ( ScalingLCD | LCDNonExpanding | LCDSyncBit | SetPWDEnable ) ;

    if ( ( pVBInfo->IF_DEF_ScaleLCD == 1 ) && ( temp & LCDNonExpanding ) )
        temp &= ~EnableScalingLCD ;

    tempbx |= temp ;

    LCDIdIndex = XGI_GetLCDCapPtr1(pVBInfo) ;

    tempax = pVBInfo->LCDCapList[ LCDIdIndex ].LCD_Capability ;

    if ( pVBInfo->IF_DEF_LVDS == 0 )	/* shampoo */
    {
        if ( ( ( pVBInfo->VBType & VB_XGI302LV ) || ( pVBInfo->VBType & VB_XGI301C ) ) && ( tempax & LCDDualLink ) )
        {
            tempbx |= SetLCDDualLink ;
        }
    }

    if ( pVBInfo->IF_DEF_CH7017 == 1 )
    {
        if ( tempax & LCDDualLink )
        {
            tempbx |= SetLCDDualLink ;
        }
    }

    if ( pVBInfo->IF_DEF_LVDS == 0 )
    {
        if ( ( pVBInfo->LCDResInfo == Panel1400x1050 ) && ( pVBInfo->VBInfo & SetCRT2ToLCD ) && ( ModeNo > 0x13 ) && ( resinfo == 9 ) && ( !( tempbx & EnableScalingLCD ) ) )
            tempbx |= SetLCDtoNonExpanding ;	/* set to center in 1280x1024 LCDB for Panel1400x1050 */
    }

/*
    if ( tempax & LCDBToA )
    {
        tempbx |= SetLCDBToA ;
    }
*/

    if ( pVBInfo->IF_DEF_ExpLink == 1 )
    {
        if ( modeflag & HalfDCLK )
        {
            /* if ( !( pVBInfo->LCDInfo&LCDNonExpanding ) ) */
            if ( !( tempbx & SetLCDtoNonExpanding ) )
            {
                tempbx |= EnableLVDSDDA ;
            }
            else
            {
                if ( ModeNo > 0x13 )
                {
                    if ( pVBInfo->LCDResInfo == Panel1024x768 )
                    {
                        if ( resinfo == 4 )
                        {                                /* 512x384  */
                            tempbx |= EnableLVDSDDA ;
                        }
                    }
                }
            }
        }
    }

    if ( pVBInfo->VBInfo & SetInSlaveMode )
    {
        if ( pVBInfo->VBInfo & SetNotSimuMode )
        {
            tempbx |= LCDVESATiming ;
        }
    }
    else
    {
        tempbx |= LCDVESATiming ;
    }

    pVBInfo->LCDInfo = tempbx ;

    if ( pVBInfo->IF_DEF_PWD == 1 )
    {
        if ( pVBInfo->LCDInfo & SetPWDEnable )
        {
            if ( ( pVBInfo->VBType & VB_XGI302LV ) || ( pVBInfo->VBType & VB_XGI301C ) )
            {
                if ( !( tempax & PWDEnable ) )
                {
                    pVBInfo->LCDInfo &= ~SetPWDEnable ;
                }
            }
        }
    }

    if ( pVBInfo->IF_DEF_LVDS == 0 )
    {
        if ( tempax & ( LockLCDBToA | StLCDBToA ) )
        {
            if ( pVBInfo->VBInfo & SetInSlaveMode )
            {
                if ( !( tempax & LockLCDBToA ) )
                {
                    if ( ModeNo <= 0x13 )
                    {
                        pVBInfo->VBInfo &= ~( SetSimuScanMode | SetInSlaveMode | SetCRT2ToLCD ) ;
                        pVBInfo->VBInfo |= SetCRT2ToLCDA | SetCRT2ToDualEdge ;
                    }
                }
            }
        }
    }

/*
    if ( pVBInfo->IF_DEF_LVDS == 0 )
    {
        if ( tempax & ( LockLCDBToA | StLCDBToA ) )
        {
            if ( pVBInfo->VBInfo & SetInSlaveMode )
            {
                if ( !( ( !( tempax & LockLCDBToA ) ) && ( ModeNo > 0x13 ) ) )
                {
                    pVBInfo->VBInfo&=~(SetSimuScanMode|SetInSlaveMode|SetCRT2ToLCD);
                    pVBInfo->VBInfo|=SetCRT2ToLCDA|SetCRT2ToDualEdge;
                }
            }
        }
    }
*/

    return( 1 ) ;
}


/* --------------------------------------------------------------------- */
/* Function : XGI_SearchModeID */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
BOOLEAN XGI_SearchModeID( USHORT ModeNo , USHORT *ModeIdIndex, PVB_DEVICE_INFO pVBInfo )
{
 /*   PUCHAR VGA_INFO = "\0x11" ;  */

#ifdef TC

    if ( ModeNo <= 5 )
        ModeNo |= 1 ;

    if ( ModeNo <= 0x13 )
    {
        /* for (*ModeIdIndex=0;*ModeIdIndex<sizeof(pVBInfo->SModeIDTable)/sizeof(XGI_StStruct);(*ModeIdIndex)++) */
        for( *ModeIdIndex = 0 ; ; ( *ModeIdIndex )++ )
        {
            if ( pVBInfo->SModeIDTable[ *ModeIdIndex ].St_ModeID == ModeNo )
                break ;
            if ( pVBInfo->SModeIDTable[ *ModeIdIndex ].St_ModeID == 0xFF )
                return( FALSE ) ;
        }

        VGA_INFO = ( PUCHAR )MK_FP( 0 , 0x489 ) ;

        if ( ModeNo == 0x07 )
        {
            if ( ( *VGA_INFO & 0x10 ) != 0 )
                ( *ModeIdIndex )++ ; /* 400 lines */
            /* else 350 lines */
        }

        if ( ModeNo <= 3 )
        {
            if ( ( *VGA_INFO & 0x80 ) == 0 )
            {
                ( *ModeIdIndex )++ ;
                if ( ( *VGA_INFO & 0x10 ) != 0 )
                    ( *ModeIdIndex )++ ; /* 400 lines */
                /* else 350 lines */
            }
            /* else 200 lines */
        }
    }
    else
    {
        /* for (*ModeIdIndex=0;*ModeIdIndex<sizeof(pVBInfo->EModeIDTable)/sizeof(XGI_ExtStruct);(*ModeIdIndex)++) */
        for( *ModeIdIndex = 0 ; ; ( *ModeIdIndex )++ )
        {
            if ( pVBInfo->EModeIDTable[ *ModeIdIndex ].Ext_ModeID == ModeNo )
                break ;
            if ( pVBInfo->EModeIDTable[ *ModeIdIndex ].Ext_ModeID == 0xFF )
                return( FALSE ) ;
        }
    }


#endif

#ifdef WIN2000

    if ( ModeNo <= 5 )
        ModeNo |= 1 ;
    if ( ModeNo <= 0x13 )
    {
        /* for (*ModeIdIndex=0;*ModeIdIndex<sizeof(pVBInfo->SModeIDTable)/sizeof(XGI_StStruct);(*ModeIdIndex)++) */
        for( *ModeIdIndex = 0 ; ; ( *ModeIdIndex )++ )
        {
            if ( pVBInfo->SModeIDTable[ *ModeIdIndex ].St_ModeID == ModeNo )
                break ;
            if ( pVBInfo->SModeIDTable[ *ModeIdIndex ].St_ModeID == 0xFF )
                return( FALSE ) ;
        }

        if ( ModeNo == 0x07 )
            ( *ModeIdIndex )++ ; /* 400 lines */

        if ( ModeNo <=3 )
            ( *ModeIdIndex ) += 2 ; /* 400 lines */
        /* else 350 lines */
    }
    else
    {
        /* for (*ModeIdIndex=0;*ModeIdIndex<sizeof(pVBInfo->EModeIDTable)/sizeof(XGI_ExtStruct);(*ModeIdIndex)++) */
        for( *ModeIdIndex = 0 ; ; ( *ModeIdIndex )++ )
        {
            if ( pVBInfo->EModeIDTable[ *ModeIdIndex ].Ext_ModeID == ModeNo )
                break ;
            if ( pVBInfo->EModeIDTable[ *ModeIdIndex ].Ext_ModeID == 0xFF )
                return( FALSE ) ;
        }
    }

#endif

#ifdef LINUX /* chiawen for linux solution */

    if ( ModeNo <= 5 )
        ModeNo |= 1 ;
    if ( ModeNo <= 0x13 )
    {
        /* for (*ModeIdIndex=0;*ModeIdIndex<sizeof(pVBInfo->SModeIDTable)/sizeof(XGI_StStruct);(*ModeIdIndex)++) */
        for( *ModeIdIndex = 0 ; ; ( *ModeIdIndex )++ )
        {
            if ( pVBInfo->SModeIDTable[ *ModeIdIndex ].St_ModeID == ModeNo )
                break ;
            if ( pVBInfo->SModeIDTable[ *ModeIdIndex ].St_ModeID == 0xFF )
                return( FALSE ) ;
        }

        if ( ModeNo == 0x07 )
            ( *ModeIdIndex )++ ; /* 400 lines */

        if ( ModeNo <= 3 )
            ( *ModeIdIndex ) += 2 ; /* 400 lines */
        /* else 350 lines */
    }
    else
    {
        /* for (*ModeIdIndex=0;*ModeIdIndex<sizeof(pVBInfo->EModeIDTable)/sizeof(XGI_ExtStruct);(*ModeIdIndex)++) */
        for( *ModeIdIndex = 0 ; ; ( *ModeIdIndex )++ )
        {
            if ( pVBInfo->EModeIDTable[ *ModeIdIndex ].Ext_ModeID == ModeNo )
                break ;
            if ( pVBInfo->EModeIDTable[ *ModeIdIndex ].Ext_ModeID == 0xFF )
                return( FALSE ) ;
        }
    }

#endif

    return( TRUE ) ;
}




/* win2000 MM adapter not support standard mode! */

#if 0
/* --------------------------------------------------------------------- */
/* Function : */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
static BOOLEAN XGINew_CheckMemorySize(PXGI_HW_DEVICE_INFO HwDeviceExtension,USHORT ModeNo,USHORT ModeIdIndex,PVB_DEVICE_INFO pVBInfo)
{
    USHORT memorysize ,
           modeflag ,
           temp ,
           temp1 ,
           tmp ;

/*  if ( ( HwDeviceExtension->jChipType == XGI_650 ) ||
         ( HwDeviceExtension->jChipType == XGI_650M ) )
    {
        return( TRUE ) ;
    } */

    if ( ModeNo <= 0x13 )
    {
        modeflag = pVBInfo->SModeIDTable[ ModeIdIndex ].St_ModeFlag ;
    }
    else  {
        modeflag = pVBInfo->EModeIDTable[ ModeIdIndex ].Ext_ModeFlag ;
    }

    /* ModeType = modeflag&ModeInfoFlag ; // Get mode type */

    memorysize = modeflag & MemoryInfoFlag ;
    memorysize = memorysize > MemorySizeShift ;
    memorysize++ ;					/* Get memory size */

    temp = XGINew_GetReg1( pVBInfo->P3c4 , 0x14 ) ;	/* Get DRAM Size */
    tmp = temp ;

    if ( HwDeviceExtension->jChipType == XG40 )
    {
        temp = 1 << ( ( temp & 0x0F0 ) >> 4 ) ;		/* memory size per channel SR14[7:4] */
        if ( ( tmp & 0x0c ) == 0x0C )			/* Qual channels */
        {
            temp <<= 2 ;
        }
        else if ( ( tmp & 0x0c ) == 0x08 )		/* Dual channels */
        {
            temp <<= 1 ;
        }
    }
    else if ( HwDeviceExtension->jChipType == XG42 )
    {
    	temp = 1 << ( ( temp & 0x0F0 ) >> 4 ) ;		/* memory size per channel SR14[7:4] */
        if ( ( tmp & 0x04 ) == 0x04 )			/* Dual channels */
        {
            temp <<= 1 ;
        }
    }
    else if ( HwDeviceExtension->jChipType == XG45 )    	
    {
    	temp = 1 << ( ( temp & 0x0F0 ) >> 4 ) ;		/* memory size per channel SR14[7:4] */
    	if ( ( tmp & 0x0c ) == 0x0C )			/* Qual channels */
        {
            temp <<= 2 ;
        }
        else if ( ( tmp & 0x0c ) == 0x08 )		/* triple channels */
        {
            temp1 = temp ;
            temp <<= 1 ;
            temp += temp1 ;
        }
        else if ( ( tmp & 0x0c ) == 0x04 )		/* Dual channels */
        {
            temp <<= 1 ;
        }
    }
    if ( temp < memorysize )
        return( FALSE ) ;
    else
        return( TRUE ) ;
}
#endif


/* --------------------------------------------------------------------- */
/* Function : XGINew_IsLowResolution */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
/*void XGINew_IsLowResolution( USHORT ModeNo , USHORT ModeIdIndex, BOOLEAN XGINew_CheckMemorySize(PXGI_HW_DEVICE_INFO HwDeviceExtension,USHORT ModeNo,USHORT ModeIdIndex,PVB_DEVICE_INFO pVBInfo)
{
    USHORT data ;
    USHORT ModeFlag ;

    data = XGINew_GetReg1( pVBInfo->P3c4 , 0x0F ) ;
    data &= 0x7F ;
    XGINew_SetReg1( pVBInfo->P3c4 , 0x0F , data ) ;

    if ( ModeNo > 0x13 )
    {
        ModeFlag = pVBInfo->EModeIDTable[ ModeIdIndex ].Ext_ModeFlag ;
        if ( ( ModeFlag & HalfDCLK ) && ( ModeFlag & DoubleScanMode ) )
        {
            data = XGINew_GetReg1( pVBInfo->P3c4 , 0x0F ) ;
            data |= 0x80 ;
            XGINew_SetReg1( pVBInfo->P3c4 , 0x0F , data ) ;
            data = XGINew_GetReg1( pVBInfo->P3c4 , 0x01 ) ;
            data &= 0xF7 ;
            XGINew_SetReg1( pVBInfo->P3c4 , 0x01 , data ) ;
        }
    }
}

*/

/* --------------------------------------------------------------------- */
/* Function : XGI_DisplayOn */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGI_DisplayOn( PVB_DEVICE_INFO pVBInfo )
{
    XGINew_SetRegANDOR(pVBInfo->P3c4,0x01,0xDF,0x00);
}


/* --------------------------------------------------------------------- */
/* Function : XGI_DisplayOff */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGI_DisplayOff( PVB_DEVICE_INFO pVBInfo )
{
    XGINew_SetRegANDOR( pVBInfo->P3c4 , 0x01 , 0xDF , 0x20 ) ;
}


/* --------------------------------------------------------------------- */
/* Function : XGI_WaitDisply */
/* Input : */
/* Output : */
/* Description : chiawen for sensecrt1 */
/* --------------------------------------------------------------------- */
void XGI_WaitDisply( PVB_DEVICE_INFO pVBInfo )
{
    while( ( XGINew_GetReg2( pVBInfo->P3da ) & 0x01 ) )
        break ;

    while( !( XGINew_GetReg2( pVBInfo->P3da ) & 0x01 ) )
        break ;
}

/* --------------------------------------------------------------------- */
/* Function : XGI_SenseCRT1 */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */

void XGI_SenseCRT1( PVB_DEVICE_INFO pVBInfo )
{
    UCHAR CRTCData[ 17 ] = { 0x5F , 0x4F , 0x50 , 0x82 , 0x55 , 0x81 ,
                             0x0B , 0x3E , 0xE9 , 0x0B , 0xDF , 0xE7 ,
                             0x04 , 0x00 , 0x00 , 0x05 , 0x00 } ;

    UCHAR SR01 = 0 , SR1F = 0 , SR07 = 0 , SR06 = 0 ;

    UCHAR CR17 , CR63 , SR31 ;
    USHORT temp ;
    UCHAR DAC_TEST_PARMS[ 3 ] = { 0x0F , 0x0F , 0x0F } ;

    int i ;

    XGINew_SetReg1( pVBInfo->P3c4 , 0x05 , 0x86 ) ;

    /* [2004/05/06] Vicent to fix XG42 single LCD sense to CRT+LCD */
    XGINew_SetReg1( pVBInfo->P3d4 , 0x57 , 0x4A ) ;
    XGINew_SetReg1( pVBInfo->P3d4 , 0x53 , ( UCHAR )( XGINew_GetReg1( pVBInfo->P3d4 , 0x53 ) | 0x02 ) ) ;

    SR31 = ( UCHAR )XGINew_GetReg1( pVBInfo->P3c4 , 0x31 ) ;
    CR63 = ( UCHAR )XGINew_GetReg1( pVBInfo->P3d4 , 0x63 ) ;
    SR01 = ( UCHAR )XGINew_GetReg1( pVBInfo->P3c4 , 0x01 ) ;

    XGINew_SetReg1( pVBInfo->P3c4 , 0x01 , ( UCHAR )( SR01 & 0xDF ) ) ;
    XGINew_SetReg1( pVBInfo->P3d4 , 0x63 , ( UCHAR )( CR63 & 0xBF ) ) ;

    CR17 = ( UCHAR )XGINew_GetReg1( pVBInfo->P3d4 , 0x17 ) ;
    XGINew_SetReg1( pVBInfo->P3d4 , 0x17 , ( UCHAR )( CR17 | 0x80 ) ) ;

    SR1F = ( UCHAR )XGINew_GetReg1( pVBInfo->P3c4 , 0x1F ) ;
    XGINew_SetReg1( pVBInfo->P3c4 , 0x1F , ( UCHAR )( SR1F | 0x04 ) ) ;

    SR07 = ( UCHAR )XGINew_GetReg1( pVBInfo->P3c4 , 0x07 ) ;
    XGINew_SetReg1( pVBInfo->P3c4 , 0x07 , ( UCHAR )( SR07 & 0xFB ) ) ;
    SR06 = ( UCHAR )XGINew_GetReg1( pVBInfo->P3c4 , 0x06 ) ;
    XGINew_SetReg1( pVBInfo->P3c4 , 0x06 , ( UCHAR )( SR06 & 0xC3 ) ) ;

    XGINew_SetReg1( pVBInfo->P3d4 , 0x11 , 0x00 ) ;

    for( i = 0 ; i < 8 ; i++ )
        XGINew_SetReg1( pVBInfo->P3d4 , ( USHORT )i , CRTCData[ i ] ) ;

    for( i = 8 ; i < 11 ; i++ )
        XGINew_SetReg1( pVBInfo->P3d4 , ( USHORT )( i + 8 ) , CRTCData[ i ] ) ;

    for( i = 11 ; i < 13 ; i++ )
        XGINew_SetReg1( pVBInfo->P3d4 , ( USHORT )( i + 4 ) , CRTCData[ i ] ) ;

    for( i = 13 ; i < 16 ; i++ )
        XGINew_SetReg1( pVBInfo->P3c4 , ( USHORT )( i - 3 ) , CRTCData[ i ] ) ;

    XGINew_SetReg1( pVBInfo->P3c4 , 0x0E , ( UCHAR )( CRTCData[ 16 ] & 0xE0 ) ) ;

    XGINew_SetReg1( pVBInfo->P3c4 , 0x31 , 0x00 ) ;
    XGINew_SetReg1( pVBInfo->P3c4 , 0x2B , 0x1B ) ;
    XGINew_SetReg1( pVBInfo->P3c4 , 0x2C , 0xE1 ) ;

    XGINew_SetReg3( pVBInfo->P3c8 , 0x00 ) ;
    for( i = 0 ; i < 256 ; i++ )
    {
        XGINew_SetReg3( ( USHORT )( pVBInfo->P3c8 + 1 ) , ( UCHAR )DAC_TEST_PARMS[ 0 ] ) ;
        XGINew_SetReg3( ( USHORT )( pVBInfo->P3c8 + 1 ) , ( UCHAR )DAC_TEST_PARMS[ 1 ] ) ;
        XGINew_SetReg3( ( USHORT )( pVBInfo->P3c8 + 1 ) , ( UCHAR )DAC_TEST_PARMS[ 2 ] ) ;
    }

    XGI_VBLongWait( pVBInfo ) ;
    XGI_VBLongWait( pVBInfo ) ;
    XGI_VBLongWait( pVBInfo ) ;

    XGINew_LCD_Wait_Time( 0x01 , pVBInfo ) ;
    XGI_WaitDisply( pVBInfo ) ;

    temp = XGINew_GetReg2( pVBInfo->P3c2 ) ;
    if ( temp & 0x10 )
    {
        XGINew_SetRegANDOR( pVBInfo->P3d4 , 0x32 , 0xDF , 0x20 ) ;
    }
    else
    {
        XGINew_SetRegANDOR( pVBInfo->P3d4 , 0x32 , 0xDF , 0x00 ) ;
    }

    /* alan, avoid display something, set BLACK DAC if not restore DAC */
    XGINew_SetReg3( pVBInfo->P3c8 , 0x00 ) ;

    for( i = 0 ; i < 256 ; i++ )
    {
      XGINew_SetReg3( ( USHORT )( pVBInfo->P3c8 + 1 ) , 0 ) ;
      XGINew_SetReg3( ( USHORT )( pVBInfo->P3c8 + 1 ) , 0 ) ;
      XGINew_SetReg3( ( USHORT )( pVBInfo->P3c8 + 1 ) , 0 ) ;
    }

    XGINew_SetReg1( pVBInfo->P3c4 , 0x01 , SR01 ) ;
    XGINew_SetReg1( pVBInfo->P3d4 , 0x63 , CR63 ) ;
    XGINew_SetReg1( pVBInfo->P3c4 , 0x31 , SR31 ) ;

    /* [2004/05/11] Vicent */
    XGINew_SetReg1( pVBInfo->P3d4 , 0x53 , ( UCHAR )( XGINew_GetReg1( pVBInfo->P3d4 , 0x53 ) & 0xFD ) ) ;
}





#ifdef TC
/* --------------------------------------------------------------------- */
/* Function : INT1AReturnCode */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
int INT1AReturnCode( union REGS regs )
{
    if ( regs.x.cflag )
    {
        /* printf( "Error to find pci device!\n" ) ; */
        return( 1 ) ;
    }

    switch(regs.h.ah)
    {
        case 0: return 0;
            break ;
        case 0x81:
            printf( "Function not support\n" ) ;
            break ;
        case 0x83:
            printf( "bad vendor id\n" ) ;
            break ;
        case 0x86:
            printf( "device not found\n" ) ;
            break ;
        case 0x87:
            printf( "bad register number\n" ) ;
            break ;
        case 0x88:
            printf( "set failed\n" ) ;
            break ;
        case 0x89:
            printf( "buffer too small" ) ;
            break ;
        default:
            break ;
    }
    return( 1 ) ;
}


/* --------------------------------------------------------------------- */
/* Function : FindPCIIOBase */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
unsigned FindPCIIOBase( unsigned index , unsigned deviceid )
{
    union REGS regs ;

    regs.h.ah = 0xb1 ;	/* PCI_FUNCTION_ID */
    regs.h.al = 0x02 ;	/* FIND_PCI_DEVICE */
    regs.x.cx = deviceid ;
    regs.x.dx = 0x1039 ;
    regs.x.si = index ;	/* find n-th device */

    int86( 0x1A , &regs , &regs ) ;

    if ( INT1AReturnCode( regs ) != 0 )
        return( 0 ) ;

    /* regs.h.bh bus number */
    /* regs.h.bl device number */
    regs.h.ah = 0xb1 ;  /* PCI_FUNCTION_ID */
    regs.h.al = 0x09 ;  /* READ_CONFIG_WORD */
    regs.x.cx = deviceid ;
    regs.x.dx = 0x1039 ;
    regs.x.di = 0x18 ;  /* register number */
    int86( 0x1A , &regs , &regs ) ;

    if ( INT1AReturnCode( regs ) != 0 )
        return( 0 ) ;

    return( regs.x.cx ) ;
}

#endif



#ifdef TC
/* --------------------------------------------------------------------- */
/* Function : main */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void main(int argc, char *argv[])
{
    XGI_HW_DEVICE_INFO HwDeviceExtension ;
    USHORT temp ;
    USHORT ModeNo ;

    /* HwDeviceExtension.pjVirtualRomBase =(PUCHAR) MK_FP(0xC000,0); */
    /* HwDeviceExtension.pjVideoMemoryAddress = (PUCHAR)MK_FP(0xA000,0); */


    HwDeviceExtension.pjIOAddress = ( FindPCIIOBase( 0 ,0x6300 ) & 0xFF80 ) + 0x30 ;
    HwDeviceExtension.jChipType = XGI_340 ;



    /* HwDeviceExtension.pjIOAddress = ( FindPCIIOBase( 0 , 0x5315 ) & 0xFF80 ) + 0x30 ; */
    
    HwDeviceExtension.pjIOAddress = ( FindPCIIOBase( 0 , 0x330 ) & 0xFF80 ) + 0x30 ;
    HwDeviceExtension.jChipType = XGI_340 ;


    HwDeviceExtension.ujVBChipID = VB_CHIP_301 ;
    StrCpy(HwDeviceExtension.szVBIOSVer , "0.84" ) ;
    HwDeviceExtension.bSkipDramSizing = FALSE ;
    HwDeviceExtension.ulVideoMemorySize = 0 ;

    if ( argc == 2 )
    {
        ModeNo = atoi( argv[ 1 ] ) ;
    }
    else
    {
        ModeNo = 0x2e ;
        /* ModeNo = 0x37 ; 1024x768x 4bpp */
        /* ModeNo = 0x38 ; 1024x768x 8bpp */
        /* ModeNo = 0x4A ; 1024x768x 16bpp */
        /* ModeNo = 0x47 ; 800x600x 16bpp */
    }

    /* XGIInitNew( &HwDeviceExtension ) ; */
    XGISetModeNew( &HwDeviceExtension , ModeNo ) ;
}
#endif


#if 0
/* --------------------------------------------------------------------- */
/* Function : XGI_WaitDisplay */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
static void XGI_WaitDisplay( PVB_DEVICE_INFO pVBInfo )
{
    while( !( XGINew_GetReg2( pVBInfo->P3da ) & 0x01 ) ) ;

    while( XGINew_GetReg2( pVBInfo->P3da ) & 0x01 ) ;
}
#endif





/* --------------------------------------------------------------------- */
/* Function : */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
BOOLEAN XGI_CheckDualChip(  PVB_DEVICE_INFO pVBInfo )
{
    /* Check H/W trap that 2nd chip is present or not. */
    return( ( BOOLEAN )(XGINew_GetReg1( pVBInfo->P3c4 , 0x3A ) & XGI_MASK_DUAL_CHIP ) ) ;
}



/* --------------------------------------------------------------------- */
/* Function : SetDualChipRegs */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
static void SetDualChipRegs( PXGI_HW_DEVICE_INFO HwDeviceExtension, PVB_DEVICE_INFO pVBInfo )
{

    USHORT  BaseAddr2nd = (USHORT)HwDeviceExtension->pj2ndIOAddress ;
    USHORT  XGINew_P3CC = pVBInfo->BaseAddr + MISC_OUTPUT_REG_READ_PORT ;
    USHORT  XGINew_2ndP3CE = BaseAddr2nd + GRAPH_ADDRESS_PORT ;
    USHORT  XGINew_2ndP3C4 = BaseAddr2nd + SEQ_ADDRESS_PORT ;
    USHORT  XGINew_2ndP3C2 = BaseAddr2nd + MISC_OUTPUT_REG_WRITE_PORT ;
    UCHAR   tempal,i;
    pVBInfo->BaseAddr    = (USHORT)HwDeviceExtension->pjIOAddress ;
    for( i = 0x00 ; i <= 0x04 ; i++ )
    {			/* SR0 - SR4 */
      tempal = ( UCHAR )XGINew_GetReg1( pVBInfo->P3c4 , i ) ;
      XGINew_SetReg1( XGINew_2ndP3C4 , i , tempal ) ;
    }
    for( i = 0x00 ; i <= 0x08 ; i++ )
    {			/* GR0 - GR8 */
      tempal = ( UCHAR )XGINew_GetReg1( pVBInfo->P3ce , i ) ;
      XGINew_SetReg1( XGINew_2ndP3CE , i , tempal ) ;
    }
    /* OpenKey in 2nd chip */
    XGINew_SetReg1( XGINew_2ndP3C4 , 0x05 , 0x86 ) ;

    /* Copy SR06 to 2nd chip */
    tempal = ( UCHAR )XGINew_GetReg1( pVBInfo->P3c4 , 0x06 ) ;
    XGINew_SetReg1( XGINew_2ndP3C4 , 0x06 , tempal ) ;

    /* Copy SR21 to 2nd chip */
    tempal = ( UCHAR )XGINew_GetReg1( pVBInfo->P3c4 , 0x21 ) ;
    XGINew_SetReg1( XGINew_2ndP3C4 , 0x21 , tempal ) ;

    /* Miscellaneous reg(input port 3cch,output port 3c2h) */
    tempal = ( UCHAR )XGINew_GetReg2( XGINew_P3CC ) ;  /* 3cc */
    XGINew_SetReg3( XGINew_2ndP3C2 , tempal ) ;

    /* Close key in 2nd chip */
    XGINew_SetReg1( XGINew_2ndP3C4 , 0x05 , 0x00 ) ;
}


/* --------------------------------------------------------------------- */
/* Function : XGI_SetCRT2Group301 */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
BOOLEAN XGI_SetCRT2Group301( USHORT ModeNo , PXGI_HW_DEVICE_INFO HwDeviceExtension, PVB_DEVICE_INFO pVBInfo )
{
    USHORT ModeIdIndex ,
           RefreshRateTableIndex ;

    pVBInfo->SetFlag |= ProgrammingCRT2 ;
    XGI_SearchModeID( ModeNo , &ModeIdIndex,  pVBInfo ) ;
    pVBInfo->SelectCRT2Rate = 4 ;
    RefreshRateTableIndex = XGI_GetRatePtrCRT2( ModeNo , ModeIdIndex, pVBInfo ) ;
    XGI_SaveCRT2Info( ModeNo, pVBInfo ) ;
    XGI_GetCRT2ResInfo( ModeNo , ModeIdIndex, pVBInfo) ;
    XGI_GetCRT2Data( ModeNo , ModeIdIndex , RefreshRateTableIndex, pVBInfo ) ;
    XGI_PreSetGroup1( ModeNo , ModeIdIndex , HwDeviceExtension , RefreshRateTableIndex, pVBInfo ) ;
    XGI_SetGroup1( ModeNo , ModeIdIndex , HwDeviceExtension , RefreshRateTableIndex, pVBInfo ) ;
    XGI_SetLockRegs( ModeNo , ModeIdIndex , HwDeviceExtension , RefreshRateTableIndex, pVBInfo ) ;
    XGI_SetGroup2(  ModeNo , ModeIdIndex , RefreshRateTableIndex , HwDeviceExtension, pVBInfo ) ;
    XGI_SetLCDRegs(ModeNo , ModeIdIndex , HwDeviceExtension , RefreshRateTableIndex, pVBInfo ) ;
    XGI_SetTap4Regs(pVBInfo) ;
    XGI_SetGroup3(ModeNo, ModeIdIndex, pVBInfo);
    XGI_SetGroup4( ModeNo , ModeIdIndex , RefreshRateTableIndex , HwDeviceExtension, pVBInfo ) ;
    XGI_SetCRT2VCLK( ModeNo , ModeIdIndex , RefreshRateTableIndex, pVBInfo ) ;
    XGI_SetGroup5( ModeNo , ModeIdIndex, pVBInfo) ;
    XGI_AutoThreshold( pVBInfo) ;
    return 1 ;
}


/* --------------------------------------------------------------------- */
/* Function : XGI_AutoThreshold */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGI_AutoThreshold(  PVB_DEVICE_INFO pVBInfo )
{
    if ( !( pVBInfo->SetFlag & Win9xDOSMode ) )
      XGINew_SetRegOR( pVBInfo->Part1Port , 0x01 , 0x40 ) ;
}


/* --------------------------------------------------------------------- */
/* Function : XGI_SaveCRT2Info */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGI_SaveCRT2Info( USHORT ModeNo , PVB_DEVICE_INFO pVBInfo)
{
    USHORT temp1 ,
           temp2 ;

    XGINew_SetReg1( pVBInfo->P3d4 , 0x34 , ModeNo ) ;  /* reserve CR34 for CRT1 Mode No */
    temp1 = ( pVBInfo->VBInfo&SetInSlaveMode ) >> 8 ;
    temp2 = ~( SetInSlaveMode >> 8 ) ;
    XGINew_SetRegANDOR( pVBInfo->P3d4 , 0x31 , temp2 , temp1 ) ;
}


/* --------------------------------------------------------------------- */
/* Function : XGI_GetCRT2ResInfo */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGI_GetCRT2ResInfo( USHORT ModeNo , USHORT ModeIdIndex, PVB_DEVICE_INFO pVBInfo )
{
    USHORT xres ,
           yres ,
           modeflag ,
           resindex ;

    resindex = XGI_GetResInfo( ModeNo , ModeIdIndex, pVBInfo) ;
    if ( ModeNo <= 0x13 )
    {
        xres = pVBInfo->StResInfo[ resindex ].HTotal ;
        yres = pVBInfo->StResInfo[ resindex ].VTotal ;
     /* modeflag = pVBInfo->SModeIDTable[ModeIdIndex].St_ModeFlag; si+St_ResInfo */
    }
    else
    {
        xres = pVBInfo->ModeResInfo[ resindex ].HTotal ;			/* xres->ax */
        yres = pVBInfo->ModeResInfo[ resindex ].VTotal ;			/* yres->bx */
        modeflag = pVBInfo->EModeIDTable[ ModeIdIndex].Ext_ModeFlag ;		/* si+St_ModeFlag */

/*        if ( pVBInfo->IF_DEF_FSTN )
        {
            xres *= 2 ;
            yres *= 2 ;
        }
        else
        {
*/
            if ( modeflag & HalfDCLK )
                xres *= 2;

            if ( modeflag & DoubleScanMode )
                yres *= 2 ;
/* } */
    }

    if ( pVBInfo->VBInfo & SetCRT2ToLCD )
    {
        if ( pVBInfo->IF_DEF_LVDS == 0 )
        {
            if ( pVBInfo->LCDResInfo == Panel1600x1200 )
            {
                if ( !( pVBInfo->LCDInfo & LCDVESATiming ) )
                {
                    if ( yres == 1024 )
                        yres = 1056 ;
	        }
            }

            if ( pVBInfo->LCDResInfo == Panel1280x1024 )
            {
	        if ( yres == 400 )
	            yres = 405 ;
	        else if ( yres == 350 )
	            yres = 360 ;

	        if ( pVBInfo->LCDInfo & LCDVESATiming )
	        {
	            if ( yres == 360 )
	                yres = 375 ;
	        }
            }

            if ( pVBInfo->LCDResInfo == Panel1024x768 )
            {
	        if ( !( pVBInfo->LCDInfo & LCDVESATiming ) )
	        {
	            if ( !( pVBInfo->LCDInfo & LCDNonExpanding ) )
	            {
	                if ( yres == 350 )
	                    yres = 357 ;
	                else if ( yres == 400 )
	                    yres = 420 ;
	                else if ( yres == 480 )
	                    yres = 525 ;
	            }
                }
            }
        }

        if ( xres == 720 )
            xres = 640 ;
    }

    pVBInfo->VGAHDE = xres ;
    pVBInfo->HDE = xres ;
    pVBInfo->VGAVDE = yres ;
    pVBInfo->VDE = yres ;
}


/* --------------------------------------------------------------------- */
/* Function : XGI_IsLCDDualLink */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
BOOLEAN XGI_IsLCDDualLink( PVB_DEVICE_INFO pVBInfo )
{
/*    USHORT flag ;  */

    if ( ( ( (pVBInfo->VBInfo & SetCRT2ToLCD ) | SetCRT2ToLCDA ) ) && ( pVBInfo->LCDInfo & SetLCDDualLink ) ) /* shampoo0129 */
        return ( 1 ) ;

    return( 0 ) ;
}


/* --------------------------------------------------------------------- */
/* Function : XGI_GetCRT2Data */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGI_GetCRT2Data(  USHORT ModeNo , USHORT ModeIdIndex , USHORT RefreshRateTableIndex, PVB_DEVICE_INFO pVBInfo )
{
    USHORT tempax = 0,
           tempbx ,
           modeflag ,
           resinfo ;
      /*     CRT2Index ,
           ResIndex ; */

    XGI_LCDDataStruct *LCDPtr = NULL ;
    XGI_TVDataStruct  *TVPtr = NULL ;

    if ( ModeNo <= 0x13 )
    {
        modeflag = pVBInfo->SModeIDTable[ ModeIdIndex ].St_ModeFlag ;      /* si+St_ResInfo */
        resinfo = pVBInfo->SModeIDTable[ ModeIdIndex ].St_ResInfo ;
    }
    else
    {
        modeflag = pVBInfo->EModeIDTable[ ModeIdIndex ].Ext_ModeFlag ;     /* si+Ext_ResInfo */
        resinfo = pVBInfo->EModeIDTable[ ModeIdIndex ].Ext_RESINFO ;
    }

    pVBInfo->NewFlickerMode = 0 ;
    pVBInfo->RVBHRS = 50 ;

    if ( pVBInfo->VBInfo & SetCRT2ToRAMDAC )
    {
        XGI_GetRAMDAC2DATA( ModeNo , ModeIdIndex , RefreshRateTableIndex,pVBInfo ) ;
        return ;
    }

    tempbx = 4 ;

    if ( pVBInfo->VBInfo & ( SetCRT2ToLCD | SetCRT2ToLCDA ) )
    {
        LCDPtr = (XGI_LCDDataStruct* )XGI_GetLcdPtr( tempbx, ModeNo , ModeIdIndex , RefreshRateTableIndex, pVBInfo ) ;

        pVBInfo->RVBHCMAX = LCDPtr->RVBHCMAX ;
        pVBInfo->RVBHCFACT = LCDPtr->RVBHCFACT ;
        pVBInfo->VGAHT = LCDPtr->VGAHT ;
        pVBInfo->VGAVT = LCDPtr->VGAVT ;
        pVBInfo->HT = LCDPtr->LCDHT ;
        pVBInfo->VT = LCDPtr->LCDVT ;

        if ( pVBInfo->LCDResInfo == Panel1024x768 )
        {
            tempax = 1024 ;
            tempbx = 768 ;

            if ( !( pVBInfo->LCDInfo & LCDVESATiming ) )
            {
                if ( pVBInfo->VGAVDE == 357 )
                    tempbx = 527 ;
	        else if ( pVBInfo->VGAVDE == 420 )
	            tempbx = 620 ;
	        else if ( pVBInfo->VGAVDE == 525 )
	            tempbx = 775 ;
	        else if ( pVBInfo->VGAVDE == 600 )
	            tempbx = 775 ;
                /* else if(pVBInfo->VGAVDE==350) tempbx=560; */
                /* else if(pVBInfo->VGAVDE==400) tempbx=640; */
	        else
	          tempbx = 768 ;
            }
            else
                tempbx = 768 ;
        }
        else if ( pVBInfo->LCDResInfo == Panel1024x768x75 )
        {
            tempax = 1024 ;
            tempbx = 768 ;
        }
        else if ( pVBInfo->LCDResInfo == Panel1280x1024 )
        {
            tempax = 1280 ;
            if ( pVBInfo->VGAVDE == 360 )
                tempbx = 768 ;
            else if ( pVBInfo->VGAVDE == 375 )
                tempbx = 800 ;
            else if ( pVBInfo->VGAVDE == 405 )
                tempbx = 864 ;
            else
                tempbx = 1024 ;
        }
        else if ( pVBInfo->LCDResInfo == Panel1280x1024x75 )
        {
            tempax = 1280 ;
            tempbx = 1024 ;
        }
        else if ( pVBInfo->LCDResInfo == Panel1280x960 )
        {
            tempax = 1280 ;
            if ( pVBInfo->VGAVDE == 350 )
                tempbx = 700 ;
            else if ( pVBInfo->VGAVDE == 400 )
                tempbx = 800 ;
            else if ( pVBInfo->VGAVDE == 1024 )
                tempbx = 960 ;
            else
                tempbx = 960 ;
        }
        else if ( pVBInfo->LCDResInfo == Panel1400x1050 )
        {
            tempax = 1400 ;
            tempbx = 1050 ;

            if ( pVBInfo->VGAVDE == 1024 )
            {
                tempax = 1280 ;
                tempbx = 1024 ;
            }
        }
        else if ( pVBInfo->LCDResInfo == Panel1600x1200 )
        {
            tempax = 1600 ;
            tempbx = 1200 ;  /* alan 10/14/2003 */
            if ( !( pVBInfo->LCDInfo & LCDVESATiming ) )
            {
              if ( pVBInfo->VGAVDE == 350 )
                tempbx = 875 ;
              else if ( pVBInfo->VGAVDE == 400 )
                tempbx = 1000 ;
            }
        }

        if ( pVBInfo->LCDInfo & LCDNonExpanding )
        {
            tempax = pVBInfo->VGAHDE ;
            tempbx = pVBInfo->VGAVDE ;
        }

        pVBInfo->HDE = tempax ;
        pVBInfo->VDE = tempbx ;
        return ;
    }

    if ( pVBInfo->VBInfo & ( SetCRT2ToTV ) )
    {
        tempbx = 4 ;
        TVPtr = ( XGI_TVDataStruct * )XGI_GetTVPtr( tempbx , ModeNo , ModeIdIndex , RefreshRateTableIndex, pVBInfo ) ;

        pVBInfo->RVBHCMAX = TVPtr->RVBHCMAX ;
        pVBInfo->RVBHCFACT = TVPtr->RVBHCFACT ;
        pVBInfo->VGAHT = TVPtr->VGAHT ;
        pVBInfo->VGAVT = TVPtr->VGAVT ;
        pVBInfo->HDE = TVPtr->TVHDE ;
        pVBInfo->VDE = TVPtr->TVVDE ;
        pVBInfo->RVBHRS = TVPtr->RVBHRS ;
        pVBInfo->NewFlickerMode = TVPtr->FlickerMode ;

        if ( pVBInfo->VBInfo & SetCRT2ToHiVisionTV )
        {
            if ( resinfo == 0x08 )
                pVBInfo->NewFlickerMode = 0x40 ;
            else if ( resinfo == 0x09 )
                pVBInfo->NewFlickerMode = 0x40 ;
            else if ( resinfo == 0x12 )
                pVBInfo->NewFlickerMode = 0x40 ;

            if ( pVBInfo->VGAVDE == 350 )
                pVBInfo->TVInfo |= TVSimuMode ;

            tempax = ExtHiTVHT ;
            tempbx = ExtHiTVVT ;

            if ( pVBInfo->VBInfo & SetInSlaveMode )
            {
	        if ( pVBInfo->TVInfo & TVSimuMode )
	        {
	            tempax = StHiTVHT ;
	            tempbx = StHiTVVT ;

	            if ( !( modeflag & Charx8Dot ) )
	            {
	                tempax = StHiTextTVHT ;
	                tempbx = StHiTextTVVT ;
	            }
	        }
            }
        }
        else if ( pVBInfo->VBInfo & SetCRT2ToYPbPr )
        {
            if ( pVBInfo->TVInfo & SetYPbPrMode750p )
            {
                tempax = YPbPrTV750pHT ;	/* Ext750pTVHT */
                tempbx = YPbPrTV750pVT ;	/* Ext750pTVVT */
            }

            if ( pVBInfo->TVInfo & SetYPbPrMode525p )
            {
                tempax = YPbPrTV525pHT ;	/* Ext525pTVHT */
                tempbx = YPbPrTV525pVT ;	/* Ext525pTVVT */
            }
            else if ( pVBInfo->TVInfo & SetYPbPrMode525i )
            {
                tempax = YPbPrTV525iHT ;	/* Ext525iTVHT */
                tempbx = YPbPrTV525iVT ;	/* Ext525iTVVT */
                if ( pVBInfo->TVInfo & NTSC1024x768 )
                    tempax = NTSC1024x768HT ;
            }
        }
        else
        {
            tempax = PALHT ;
            tempbx = PALVT ;
            if ( !( pVBInfo->TVInfo & SetPALTV ) )
            {
                tempax = NTSCHT ;
	        tempbx = NTSCVT ;
	        if ( pVBInfo->TVInfo & NTSC1024x768 )
	            tempax = NTSC1024x768HT ;
            }
        }

        pVBInfo->HT = tempax ;
        pVBInfo->VT = tempbx ;
        return ;
    }
}


/* --------------------------------------------------------------------- */
/* Function : XGI_SetCRT2VCLK */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGI_SetCRT2VCLK( USHORT ModeNo , USHORT ModeIdIndex , USHORT RefreshRateTableIndex, PVB_DEVICE_INFO pVBInfo )
{
    UCHAR di_0 ,
          di_1 ,
          tempal ;

    tempal = XGI_GetVCLKPtr( RefreshRateTableIndex , ModeNo , ModeIdIndex, pVBInfo ) ;
    XGI_GetVCLKLen( tempal, &di_0 , &di_1, pVBInfo ) ;
    XGI_GetLCDVCLKPtr( &di_0 , &di_1, pVBInfo ) ;

    if ( pVBInfo->VBType & VB_XGI301 ) /* shampoo 0129 */
    {				/* 301 */
        XGINew_SetReg1(pVBInfo->Part4Port , 0x0A , 0x10 ) ;
        XGINew_SetReg1(pVBInfo->Part4Port , 0x0B , di_1 ) ;
        XGINew_SetReg1(pVBInfo->Part4Port , 0x0A , di_0 ) ;
    }
    else
    {				/* 301b/302b/301lv/302lv */
        XGINew_SetReg1( pVBInfo->Part4Port , 0x0A , di_0 ) ;
        XGINew_SetReg1( pVBInfo->Part4Port , 0x0B , di_1 ) ;
    }

    XGINew_SetReg1( pVBInfo->Part4Port , 0x00 , 0x12 ) ;

    if ( pVBInfo->VBInfo & SetCRT2ToRAMDAC )
        XGINew_SetRegOR( pVBInfo->Part4Port , 0x12 , 0x28 ) ;
    else
        XGINew_SetRegOR( pVBInfo->Part4Port , 0x12 , 0x08 ) ;
}


/* --------------------------------------------------------------------- */
/* Function : XGI_GETLCDVCLKPtr */
/* Input : */
/* Output : al -> VCLK Index */
/* Description : */
/* --------------------------------------------------------------------- */
void XGI_GetLCDVCLKPtr( UCHAR* di_0 , UCHAR *di_1, PVB_DEVICE_INFO pVBInfo )
{
    USHORT index ;

    if ( pVBInfo->VBInfo & ( SetCRT2ToLCD | SetCRT2ToLCDA ) )
    {
        if ( pVBInfo->IF_DEF_ScaleLCD == 1 )
        {
            if ( pVBInfo->LCDInfo & EnableScalingLCD )
                return ;
        }

        /* index = XGI_GetLCDCapPtr(pVBInfo) ; */
        index = XGI_GetLCDCapPtr1( pVBInfo) ;

        if ( pVBInfo->VBInfo & SetCRT2ToLCD )
        {	/* LCDB */
            *di_0 = pVBInfo->LCDCapList[ index ].LCUCHAR_VCLKData1 ;
            *di_1 = pVBInfo->LCDCapList[ index ].LCUCHAR_VCLKData2 ;
        }
        else
        {	/* LCDA */
            *di_0 = pVBInfo->LCDCapList[ index ].LCDA_VCLKData1 ;
            *di_1 = pVBInfo->LCDCapList[ index ].LCDA_VCLKData2 ;
        }
    }
    return ;
}


/* --------------------------------------------------------------------- */
/* Function : XGI_GetVCLKPtr */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
UCHAR XGI_GetVCLKPtr(USHORT RefreshRateTableIndex,USHORT ModeNo,USHORT ModeIdIndex, PVB_DEVICE_INFO pVBInfo)
{

    USHORT index ,
           modeflag ;


    UCHAR tempal ;

    if ( ModeNo <= 0x13 )
        modeflag = pVBInfo->SModeIDTable[ ModeIdIndex ].St_ModeFlag ;      /* si+St_ResInfo */
    else
        modeflag = pVBInfo->EModeIDTable[ModeIdIndex].Ext_ModeFlag;    /* si+Ext_ResInfo */


    if ( ( pVBInfo->SetFlag & ProgrammingCRT2 ) && ( !( pVBInfo->LCDInfo & EnableScalingLCD ) ) )
    {   /* {LCDA/LCDB} */
        index = XGI_GetLCDCapPtr(pVBInfo) ;
        tempal = pVBInfo->LCDCapList[ index ].LCD_VCLK ;

        if ( pVBInfo->VBInfo & ( SetCRT2ToLCD | SetCRT2ToLCDA ) )
            return tempal ;

	/* {TV} */
        if ( pVBInfo->VBType & ( VB_XGI301B | VB_XGI302B | VB_XGI301LV| VB_XGI302LV| VB_XGI301C ) )
        {
            if(pVBInfo->VBInfo&SetCRT2ToHiVisionTV)
           {
                tempal = HiTVVCLKDIV2;
                if(!(pVBInfo->TVInfo & RPLLDIV2XO))
                    tempal = HiTVVCLK;
	        if(pVBInfo->TVInfo & TVSimuMode)
                {
                    tempal = HiTVSimuVCLK;
                    if(!(modeflag & Charx8Dot))
              	        tempal = HiTVTextVCLK;

                 }
                 return tempal;
             }

            if ( pVBInfo->TVInfo & SetYPbPrMode750p )
            {
                tempal = YPbPr750pVCLK ;
                return tempal ;
            }

            if ( pVBInfo->TVInfo & SetYPbPrMode525p )
            {
                tempal = YPbPr525pVCLK ;
                return tempal ;
            }

            tempal = NTSC1024VCLK ;

            if ( !( pVBInfo->TVInfo & NTSC1024x768 ) )
            {
                tempal = TVVCLKDIV2 ;
                if ( !( pVBInfo->TVInfo & RPLLDIV2XO ) )
                    tempal = TVVCLK ;
            }

            if ( pVBInfo->VBInfo & SetCRT2ToTV )
                return tempal ;
        }
        /*else
        if((pVBInfo->IF_DEF_CH7017==1)&&(pVBInfo->VBType&VB_CH7017))
        {
            if(ModeNo<=0x13)
                *tempal = pVBInfo->SModeIDTable[ModeIdIndex].St_CRT2CRTC;
            else
                *tempal = pVBInfo->RefIndex[RefreshRateTableIndex].Ext_CRT2CRTC;
            *tempal = *tempal & 0x1F;

            tempbx = 0;
            if(pVBInfo->TVInfo & SetPALTV)
                tempbx = tempbx + 2;
            if(pVBInfo->TVInfo & SetCHTVOverScan)
                tempbx++;
            tempbx = tempbx << 1;
        }  */
    }	/* {End of VB} */

    tempal = ( UCHAR )XGINew_GetReg2( ( USHORT )( pVBInfo->P3ca + 0x02 ) ) ;
    tempal = tempal >> 2 ;
    tempal &= 0x03 ;

    if ( ( pVBInfo->LCDInfo & EnableScalingLCD ) && ( modeflag & Charx8Dot ) )  /* for Dot8 Scaling LCD */
        tempal = tempal ^ tempal ;                   /* ; set to VCLK25MHz always */

    if ( ModeNo <= 0x13 )
        return tempal ;

    tempal = pVBInfo->RefIndex[ RefreshRateTableIndex ].Ext_CRTVCLK ;
    return tempal ;
}


/* --------------------------------------------------------------------- */
/* Function : XGI_GetVCLKLen */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGI_GetVCLKLen(UCHAR tempal,UCHAR* di_0,UCHAR* di_1, PVB_DEVICE_INFO pVBInfo)
{
    if ( pVBInfo->VBType & ( VB_XGI301 | VB_XGI301B | VB_XGI302B | VB_XGI301LV | VB_XGI302LV | VB_XGI301C ) )
    {
        if ( ( !( pVBInfo->VBInfo & SetCRT2ToLCDA ) ) && ( pVBInfo->SetFlag & ProgrammingCRT2 ) )
        {
            *di_0 = ( UCHAR )XGI_VBVCLKData[ tempal ].SR2B ;
            *di_1 = XGI_VBVCLKData[ tempal ].SR2C ;
        }
    }
    else
    {
        *di_0 = XGI_VCLKData[ tempal ].SR2B ;
        *di_1 = XGI_VCLKData[ tempal ].SR2C ;
    }
}


/* --------------------------------------------------------------------- */
/* Function : XGI_SetCRT2Offset */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
static void XGI_SetCRT2Offset( USHORT ModeNo ,
				   USHORT ModeIdIndex , USHORT RefreshRateTableIndex , PXGI_HW_DEVICE_INFO HwDeviceExtension, PVB_DEVICE_INFO pVBInfo )
{
    USHORT offset ;
    UCHAR temp ;

    if ( pVBInfo->VBInfo & SetInSlaveMode )
    {
        return ;
    }

    offset = XGI_GetOffset(  ModeNo , ModeIdIndex , RefreshRateTableIndex , HwDeviceExtension, pVBInfo ) ;
    temp = ( UCHAR )( offset & 0xFF ) ;
    XGINew_SetReg1( pVBInfo->Part1Port , 0x07 , temp ) ;
    temp =( UCHAR)( ( offset & 0xFF00 ) >> 8 ) ;
    XGINew_SetReg1( pVBInfo->Part1Port , 0x09 , temp ) ;
    temp =( UCHAR )( ( ( offset >> 3 ) & 0xFF ) + 1 ) ;
    XGINew_SetReg1( pVBInfo->Part1Port , 0x03 , temp ) ;
}


/* --------------------------------------------------------------------- */
/* Function : XGI_GetOffset */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
USHORT XGI_GetOffset(USHORT ModeNo,USHORT ModeIdIndex,USHORT RefreshRateTableIndex,PXGI_HW_DEVICE_INFO HwDeviceExtension,PVB_DEVICE_INFO pVBInfo)
{
    USHORT temp ,
           colordepth ,
           modeinfo ,
           index ,
           infoflag ,
           ColorDepth[] = { 0x01 , 0x02 , 0x04 } ;

    modeinfo = pVBInfo->EModeIDTable[ ModeIdIndex ].Ext_ModeInfo ;
    if ( ModeNo <= 0x14 )
        infoflag = 0 ;
    else
        infoflag = pVBInfo->RefIndex[ RefreshRateTableIndex ].Ext_InfoFlag ;


    index = ( modeinfo >> 8 ) & 0xFF ;

    temp = pVBInfo->ScreenOffset[ index ] ;

    if ( infoflag & InterlaceMode )
    {
        temp = temp << 1 ;
    }

    colordepth = XGI_GetColorDepth( ModeNo , ModeIdIndex, pVBInfo ) ;

    if ( ( ModeNo >= 0x7C ) && ( ModeNo <= 0x7E ) )
    {
        temp = ModeNo - 0x7C ;
	colordepth = ColorDepth[ temp ] ;
	temp = 0x6B ;
	if ( infoflag & InterlaceMode )
	{
            temp = temp << 1 ;
	}
	return( temp * colordepth ) ;
    }
    else
        return( temp * colordepth ) ;
}


/* --------------------------------------------------------------------- */
/* Function : XGI_SetCRT2FIFO */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
static void XGI_SetCRT2FIFO( PVB_DEVICE_INFO pVBInfo)
{
    XGINew_SetReg1( pVBInfo->Part1Port , 0x01 , 0x3B ) ;			/* threshold high ,disable auto threshold */
    XGINew_SetRegANDOR( pVBInfo->Part1Port , 0x02 , ~( 0x3F ) , 0x04 ) ;	/* threshold low default 04h */
}


/* --------------------------------------------------------------------- */
/* Function : XGI_PreSetGroup1 */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGI_PreSetGroup1(USHORT ModeNo , USHORT ModeIdIndex ,PXGI_HW_DEVICE_INFO HwDeviceExtension,
                       USHORT RefreshRateTableIndex, PVB_DEVICE_INFO pVBInfo )
{
    USHORT tempcx = 0 ,
           CRT1Index = 0 ;






    if ( ModeNo > 0x13 )
    {
        CRT1Index = pVBInfo->RefIndex[ RefreshRateTableIndex ].Ext_CRT1CRTC ;
        CRT1Index &= IndexMask ;
    }

    XGI_SetCRT2Offset( ModeNo , ModeIdIndex , RefreshRateTableIndex , HwDeviceExtension, pVBInfo ) ;
    XGI_SetCRT2FIFO(pVBInfo) ;
    /* XGI_SetCRT2Sync(ModeNo,RefreshRateTableIndex); */

    for( tempcx = 4 ; tempcx < 7 ; tempcx++ )
    {
        XGINew_SetReg1( pVBInfo->Part1Port , tempcx , 0x0 ) ;
    }

    XGINew_SetReg1( pVBInfo->Part1Port , 0x02 , 0x44 ) ;	/* temp 0206 */
}


/* --------------------------------------------------------------------- */
/* Function : XGI_SetGroup1 */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGI_SetGroup1( USHORT ModeNo , USHORT ModeIdIndex ,
                            PXGI_HW_DEVICE_INFO HwDeviceExtension , USHORT RefreshRateTableIndex, PVB_DEVICE_INFO pVBInfo )
{
    USHORT temp = 0 ,
           tempax = 0 ,
           tempbx = 0 ,
           tempcx = 0 ,
           pushbx = 0 ,
           CRT1Index = 0 ,
           modeflag ;

    if ( ModeNo > 0x13 )
    {
        CRT1Index = pVBInfo->RefIndex[ RefreshRateTableIndex ].Ext_CRT1CRTC ;
        CRT1Index &= IndexMask ;
    }

    if ( ModeNo <= 0x13 )
    {
        modeflag = pVBInfo->SModeIDTable[ ModeIdIndex ].St_ModeFlag ;
    }
    else
    {
        modeflag = pVBInfo->EModeIDTable[ ModeIdIndex ].Ext_ModeFlag ;
    }

    /* bainy change table name */
    if ( modeflag & HalfDCLK )
    {
        temp = ( pVBInfo->VGAHT / 2 - 1 ) & 0x0FF ;			/* BTVGA2HT 0x08,0x09 */
        XGINew_SetReg1( pVBInfo->Part1Port , 0x08 , temp ) ;
        temp = ( ( ( pVBInfo->VGAHT / 2 - 1 ) & 0xFF00 ) >> 8 ) << 4 ;
        XGINew_SetRegANDOR( pVBInfo->Part1Port , 0x09 , ~0x0F0 , temp ) ;
        temp = ( pVBInfo->VGAHDE / 2 + 16 ) & 0x0FF ;			/* BTVGA2HDEE 0x0A,0x0C */
        XGINew_SetReg1( pVBInfo->Part1Port , 0x0A , temp ) ;
        tempcx = ( ( pVBInfo->VGAHT - pVBInfo->VGAHDE ) / 2 ) >> 2 ;
        pushbx = pVBInfo->VGAHDE / 2 + 16 ;
        tempcx = tempcx >> 1 ;
        tempbx = pushbx + tempcx ;					/* bx BTVGA@HRS 0x0B,0x0C */
        tempcx += tempbx ;

  	if ( pVBInfo->VBInfo & SetCRT2ToRAMDAC )
  	{
            tempbx = pVBInfo->XGINEWUB_CRT1Table[ CRT1Index ].CR[ 4 ] ;
            tempbx |= ( ( pVBInfo->XGINEWUB_CRT1Table[ CRT1Index ].CR[ 14 ] & 0xC0 ) << 2 ) ;
            tempbx = ( tempbx - 3 ) << 3 ;				/* (VGAHRS-3)*8 */
            tempcx = pVBInfo->XGINEWUB_CRT1Table[CRT1Index].CR[ 5 ] ;
            tempcx &= 0x1F ;
            temp = pVBInfo->XGINEWUB_CRT1Table[ CRT1Index ].CR[ 15 ] ;
            temp = ( temp & 0x04 ) << ( 5 - 2 ) ;			/* VGAHRE D[5] */
            tempcx = ( ( tempcx | temp ) - 3 ) << 3 ;			/* (VGAHRE-3)*8 */
        }

        tempbx += 4 ;
        tempcx += 4 ;

        if ( tempcx > ( pVBInfo->VGAHT / 2 ) )
            tempcx = pVBInfo->VGAHT / 2 ;

        temp = tempbx & 0x00FF ;

        XGINew_SetReg1( pVBInfo->Part1Port , 0x0B , temp ) ;
    }
    else
    {
        temp = ( pVBInfo->VGAHT - 1 ) & 0x0FF ;				/* BTVGA2HT 0x08,0x09 */
        XGINew_SetReg1( pVBInfo->Part1Port , 0x08 , temp ) ;
        temp = ( ( ( pVBInfo->VGAHT - 1 ) & 0xFF00 ) >> 8 ) << 4 ;
        XGINew_SetRegANDOR( pVBInfo->Part1Port , 0x09 , ~0x0F0 , temp ) ;
        temp = ( pVBInfo->VGAHDE + 16 ) & 0x0FF ;				/* BTVGA2HDEE 0x0A,0x0C */
        XGINew_SetReg1( pVBInfo->Part1Port , 0x0A , temp ) ;
        tempcx = ( pVBInfo->VGAHT - pVBInfo->VGAHDE ) >> 2 ;		/* cx */
        pushbx = pVBInfo->VGAHDE + 16 ;
        tempcx = tempcx >> 1 ;
        tempbx = pushbx + tempcx ;					/* bx BTVGA@HRS 0x0B,0x0C */
        tempcx += tempbx ;

        if ( pVBInfo->VBInfo & SetCRT2ToRAMDAC )
        {
            tempbx = pVBInfo->XGINEWUB_CRT1Table[ CRT1Index ].CR[ 3 ] ;
            tempbx |= ( ( pVBInfo->XGINEWUB_CRT1Table[ CRT1Index ].CR[ 5 ] & 0xC0 ) << 2 ) ;
            tempbx = ( tempbx - 3 ) << 3 ;				/* (VGAHRS-3)*8 */
            tempcx = pVBInfo->XGINEWUB_CRT1Table[ CRT1Index ].CR[ 4 ] ;
            tempcx &= 0x1F ;
            temp = pVBInfo->XGINEWUB_CRT1Table[ CRT1Index ].CR[ 6 ] ;
            temp = ( temp & 0x04 ) << ( 5 - 2 ) ;			/* VGAHRE D[5] */
            tempcx = ( ( tempcx | temp ) - 3 ) << 3 ;			/* (VGAHRE-3)*8 */
            tempbx += 16 ;
            tempcx += 16 ;
        }

        if ( tempcx > pVBInfo->VGAHT )
            tempcx = pVBInfo->VGAHT ;

        temp = tempbx & 0x00FF ;
        XGINew_SetReg1( pVBInfo->Part1Port , 0x0B , temp ) ;
    }

    tempax = ( tempax & 0x00FF ) | ( tempbx & 0xFF00 ) ;
    tempbx = pushbx ;
    tempbx = ( tempbx & 0x00FF ) | ( ( tempbx & 0xFF00 ) << 4 ) ;
    tempax |= ( tempbx & 0xFF00 ) ;
    temp = ( tempax & 0xFF00 ) >> 8 ;
    XGINew_SetReg1( pVBInfo->Part1Port , 0x0C , temp ) ;
    temp = tempcx & 0x00FF ;
    XGINew_SetReg1( pVBInfo->Part1Port , 0x0D , temp ) ;
    tempcx = ( pVBInfo->VGAVT - 1 ) ;
    temp = tempcx & 0x00FF ;

    if ( pVBInfo->IF_DEF_CH7005 == 1 )
    {
        if ( pVBInfo->VBInfo & 0x0C )
        {
            temp-- ;
	}
    }

    XGINew_SetReg1( pVBInfo->Part1Port , 0x0E , temp ) ;
    tempbx = pVBInfo->VGAVDE - 1 ;
    temp = tempbx & 0x00FF ;
    XGINew_SetReg1( pVBInfo->Part1Port , 0x0F , temp ) ;
    temp = ( ( tempbx & 0xFF00 ) << 3 ) >> 8 ;
    temp |= ( ( tempcx & 0xFF00 ) >> 8 ) ;
    XGINew_SetReg1( pVBInfo->Part1Port , 0x12 , temp ) ;

    tempax = pVBInfo->VGAVDE ;
    tempbx = pVBInfo->VGAVDE ;
    tempcx = pVBInfo->VGAVT ;
    tempbx = ( pVBInfo->VGAVT + pVBInfo->VGAVDE ) >> 1 ;				/* BTVGA2VRS 0x10,0x11 */
    tempcx = ( ( pVBInfo->VGAVT - pVBInfo->VGAVDE ) >> 4 ) + tempbx + 1 ;		/* BTVGA2VRE 0x11 */

    if ( pVBInfo->VBInfo & SetCRT2ToRAMDAC )
    {
        tempbx = pVBInfo->XGINEWUB_CRT1Table[ CRT1Index ].CR[ 10 ] ;
        temp = pVBInfo->XGINEWUB_CRT1Table[ CRT1Index ].CR[ 9 ] ;

        if ( temp & 0x04 )
            tempbx |= 0x0100 ;

        if ( temp & 0x080 )
            tempbx |= 0x0200 ;

        temp = pVBInfo->XGINEWUB_CRT1Table[ CRT1Index ].CR[ 14 ] ;

        if ( temp & 0x08 )
            tempbx |= 0x0400 ;

        temp = pVBInfo->XGINEWUB_CRT1Table[ CRT1Index ].CR[ 11 ] ;
        tempcx = ( tempcx & 0xFF00 ) | ( temp & 0x00FF ) ;
    }

    temp = tempbx & 0x00FF ;
    XGINew_SetReg1( pVBInfo->Part1Port , 0x10 , temp ) ;
    temp = ( ( tempbx & 0xFF00 ) >> 8 ) << 4 ;
    temp = ( ( tempcx & 0x000F ) | ( temp ) ) ;
    XGINew_SetReg1( pVBInfo->Part1Port , 0x11 , temp ) ;
    tempax = 0 ;

    if ( modeflag & DoubleScanMode )
        tempax |= 0x80 ;

    if ( modeflag & HalfDCLK )
        tempax |= 0x40 ;

    XGINew_SetRegANDOR( pVBInfo->Part1Port , 0x2C , ~0x0C0 , tempax ) ;
}


/* --------------------------------------------------------------------- */
/* Function : XGI_SetLockRegs */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void  XGI_SetLockRegs( USHORT ModeNo , USHORT ModeIdIndex ,
                                PXGI_HW_DEVICE_INFO HwDeviceExtension , USHORT RefreshRateTableIndex, PVB_DEVICE_INFO pVBInfo )
{
    USHORT push1 ,
           push2 ,
           tempax ,
           tempbx = 0 ,
           tempcx ,
           temp ,
           resinfo ,
           modeflag ,
           CRT1Index ;

    if ( ModeNo <= 0x13 )
    {
        modeflag = pVBInfo->SModeIDTable[ ModeIdIndex ].St_ModeFlag ;	/* si+St_ResInfo */
        resinfo = pVBInfo->SModeIDTable[ ModeIdIndex ].St_ResInfo ;
    }
    else
    {
        modeflag = pVBInfo->EModeIDTable[ ModeIdIndex ].Ext_ModeFlag ;	/* si+Ext_ResInfo */
        resinfo = pVBInfo->EModeIDTable[ ModeIdIndex ].Ext_RESINFO ;
        CRT1Index = pVBInfo->RefIndex[ RefreshRateTableIndex ].Ext_CRT1CRTC ;
        CRT1Index &= IndexMask;
    }

    if ( !( pVBInfo->VBInfo & SetInSlaveMode ) )
    {
        return ;
    }

    temp = 0xFF ;							/* set MAX HT */
    XGINew_SetReg1( pVBInfo->Part1Port , 0x03 , temp ) ;
    /* if ( modeflag & Charx8Dot ) tempcx = 0x08 ; */
    /* else */
    tempcx=0x08;

    if ( pVBInfo->VBType & ( VB_XGI301LV | VB_XGI302LV | VB_XGI301C ) )
        modeflag |= Charx8Dot ;

    tempax = pVBInfo->VGAHDE ;						/* 0x04 Horizontal Display End */

    if ( modeflag & HalfDCLK )
        tempax = tempax >> 1 ;

    tempax = ( tempax / tempcx ) - 1 ;
    tempbx |= ( ( tempax & 0x00FF ) << 8 ) ;
    temp = tempax & 0x00FF ;
    XGINew_SetReg1( pVBInfo->Part1Port , 0x04 , temp ) ;

    temp = ( tempbx & 0xFF00 ) >> 8 ;

    if ( pVBInfo->VBInfo & SetCRT2ToTV )
    {
        if ( !( pVBInfo->VBType & ( VB_XGI301B | VB_XGI302B | VB_XGI301LV | VB_XGI302LV | VB_XGI301C ) ) )
            temp += 2 ;

        if ( pVBInfo->VBInfo & SetCRT2ToHiVisionTV )
        {
            if ( pVBInfo->VBType & VB_XGI301LV )
            {
                if ( pVBInfo->VBExtInfo == VB_YPbPr1080i )
                {
                    if ( resinfo == 7 )
                        temp -= 2 ;
                }
            }
            else
                if ( resinfo == 7 )
                    temp -= 2 ;
        }
    }

    XGINew_SetReg1( pVBInfo->Part1Port , 0x05 , temp ) ;			/* 0x05 Horizontal Display Start */
    XGINew_SetReg1( pVBInfo->Part1Port , 0x06 , 0x03 ) ;			/* 0x06 Horizontal Blank end */

    if ( !( pVBInfo->VBInfo & DisableCRT2Display ) )
    {                            					/* 030226 bainy */
        if ( pVBInfo->VBInfo & SetCRT2ToTV )
            tempax = pVBInfo->VGAHT ;
        else
            tempax = XGI_GetVGAHT2( pVBInfo) ;
    }

    if ( tempax >= pVBInfo->VGAHT )
    {
        tempax = pVBInfo->VGAHT ;
    }

    if ( modeflag & HalfDCLK )
    {
        tempax = tempax >> 1 ;
    }

    tempax = ( tempax / tempcx ) - 5 ;
    tempcx = tempax ;				/* 20030401 0x07 horizontal Retrace Start */
    if ( pVBInfo->VBInfo & SetCRT2ToHiVisionTV )
    {
        temp = ( tempbx & 0x00FF ) - 1 ;
        if ( !( modeflag & HalfDCLK ) )
        {
            temp -= 6 ;
            if ( pVBInfo->TVInfo & TVSimuMode )
            {
                temp -= 4 ;
                if ( ModeNo > 0x13 )
                    temp -= 10 ;
            }
        }
    }
    else
    {
        /* tempcx = tempbx & 0x00FF ; */
        tempbx = ( tempbx & 0xFF00 ) >> 8 ;
        tempcx = ( tempcx + tempbx ) >> 1 ;
        temp = ( tempcx & 0x00FF ) + 2 ;

        if ( pVBInfo->VBInfo & SetCRT2ToTV )
        {
            temp -= 1 ;
            if ( !( modeflag & HalfDCLK ) )
            {
                if ( ( modeflag & Charx8Dot ) )
                {
                    temp += 4 ;
                    if ( pVBInfo->VGAHDE >= 800 )
                    {
                        temp -= 6 ;
                    }
                }
            }
        }
        else
        {
            if ( !( modeflag & HalfDCLK ) )
            {
                temp -= 4 ;
                if ( pVBInfo->LCDResInfo != Panel1280x960 )
                {
                    if( pVBInfo->VGAHDE >= 800 )
                    {
                        temp -= 7 ;
                        if ( pVBInfo->ModeType == ModeEGA )
                        {
                            if ( pVBInfo->VGAVDE == 1024 )
                            {
                                temp += 15 ;
                                if ( pVBInfo->LCDResInfo != Panel1280x1024 )
                                {
                                    temp += 7 ;
                                }
                            }
                        }

                        if ( pVBInfo->VGAHDE >= 1280 )
                        {
                            if ( pVBInfo->LCDResInfo != Panel1280x960 )
                            {
                                if ( pVBInfo->LCDInfo & LCDNonExpanding )
                                {
                                    temp += 28 ;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    XGINew_SetReg1( pVBInfo->Part1Port , 0x07 , temp ) ;		/* 0x07 Horizontal Retrace Start */
    XGINew_SetReg1( pVBInfo->Part1Port , 0x08 , 0 ) ;		/* 0x08 Horizontal Retrace End */

    if ( pVBInfo->VBInfo & SetCRT2ToTV )
    {
        if ( pVBInfo->TVInfo & TVSimuMode )
        {
            if ( ( ModeNo == 0x06 ) || ( ModeNo == 0x10 ) || ( ModeNo == 0x11 ) || ( ModeNo == 0x13 ) || ( ModeNo == 0x0F ) )
            {
                XGINew_SetReg1( pVBInfo->Part1Port , 0x07 , 0x5b ) ;
                XGINew_SetReg1( pVBInfo->Part1Port , 0x08 , 0x03 ) ;
            }

            if ( ( ModeNo == 0x00 ) || ( ModeNo == 0x01 ) )
            {
                if ( pVBInfo->TVInfo & SetNTSCTV )
                {
                    XGINew_SetReg1( pVBInfo->Part1Port , 0x07 , 0x2A ) ;
                    XGINew_SetReg1( pVBInfo->Part1Port , 0x08 , 0x61 ) ;
                }
                else
                {
                    XGINew_SetReg1( pVBInfo->Part1Port , 0x07 , 0x2A ) ;
                    XGINew_SetReg1( pVBInfo->Part1Port , 0x08 , 0x41 ) ;
                    XGINew_SetReg1( pVBInfo->Part1Port , 0x0C , 0xF0 ) ;
                }
            }

            if ( ( ModeNo == 0x02 ) || ( ModeNo == 0x03 ) || ( ModeNo == 0x07 ) )
            {
                if ( pVBInfo->TVInfo & SetNTSCTV )
                {
                    XGINew_SetReg1( pVBInfo->Part1Port , 0x07 , 0x54 ) ;
                    XGINew_SetReg1( pVBInfo->Part1Port , 0x08 , 0x00 ) ;
                }
                else
                {
                    XGINew_SetReg1( pVBInfo->Part1Port , 0x07 , 0x55 ) ;
                    XGINew_SetReg1( pVBInfo->Part1Port , 0x08 , 0x00 ) ;
                    XGINew_SetReg1( pVBInfo->Part1Port , 0x0C , 0xF0 ) ;
                }
            }

            if ( ( ModeNo == 0x04 ) || ( ModeNo == 0x05 ) || ( ModeNo == 0x0D ) || ( ModeNo == 0x50 ) )
            {
                if ( pVBInfo->TVInfo & SetNTSCTV )
                {
                    XGINew_SetReg1( pVBInfo->Part1Port , 0x07 , 0x30 ) ;
                    XGINew_SetReg1( pVBInfo->Part1Port , 0x08 , 0x03 ) ;
                }
                else
                {
                    XGINew_SetReg1( pVBInfo->Part1Port , 0x07 , 0x2f ) ;
                    XGINew_SetReg1( pVBInfo->Part1Port , 0x08 , 0x02 ) ;
                }
            }
        }
    }

    XGINew_SetReg1( pVBInfo->Part1Port , 0x18 , 0x03 ) ;				/* 0x18 SR0B */
    XGINew_SetRegANDOR( pVBInfo->Part1Port , 0x19 , 0xF0 , 0x00 ) ;
    XGINew_SetReg1( pVBInfo->Part1Port , 0x09 , 0xFF ) ;				/* 0x09 Set Max VT */

    tempbx = pVBInfo->VGAVT ;
    push1 = tempbx ;
    tempcx = 0x121 ;
    tempbx = pVBInfo->VGAVDE ;							/* 0x0E Virtical Display End */

    if ( tempbx == 357 )
        tempbx = 350 ;
    if ( tempbx == 360 )
        tempbx =350 ;
    if ( tempbx == 375 )
        tempbx = 350 ;
    if ( tempbx == 405 )
        tempbx = 400 ;
    if ( tempbx == 525 )
        tempbx = 480 ;

    push2 = tempbx ;

    if ( pVBInfo->VBInfo & SetCRT2ToLCD )
    {
        if ( pVBInfo->LCDResInfo == Panel1024x768 )
        {
            if ( !( pVBInfo->LCDInfo & LCDVESATiming ) )
            {
                if ( tempbx == 350 )
                    tempbx += 5 ;
		if ( tempbx == 480 )
		    tempbx += 5 ;
            }
        }
    }
    tempbx-- ;
    temp = tempbx & 0x00FF ;
    tempbx-- ;
    temp = tempbx & 0x00FF ;
    XGINew_SetReg1( pVBInfo->Part1Port , 0x10 ,temp ) ;				/* 0x10 vertical Blank Start */
    tempbx = push2 ;
    tempbx-- ;
    temp = tempbx & 0x00FF ;
    XGINew_SetReg1( pVBInfo->Part1Port , 0x0E , temp ) ;

    if ( tempbx & 0x0100 )
    {
        tempcx |= 0x0002 ;
    }

    tempax = 0x000B ;

    if ( modeflag & DoubleScanMode )
    {
        tempax |= 0x08000 ;
    }

    if ( tempbx & 0x0200 )
    {
        tempcx |= 0x0040 ;
    }

    temp = ( tempax & 0xFF00 ) >> 8 ;
    XGINew_SetReg1( pVBInfo->Part1Port , 0x0B , temp ) ;

    if ( tempbx & 0x0400 )
    {
        tempcx |= 0x0600 ;
    }

    XGINew_SetReg1( pVBInfo->Part1Port , 0x11 , 0x00 ) ;				/* 0x11 Vertival Blank End */

    tempax = push1 ;
    tempax -= tempbx ;								/* 0x0C Vertical Retrace Start */
    tempax = tempax >> 2 ;
    push1 = tempax ;								/* push ax */

    if ( resinfo != 0x09 )
    {
        tempax = tempax << 1 ;
        tempbx += tempax ;
    }

    if ( pVBInfo->VBInfo & SetCRT2ToHiVisionTV )
    {
        if ( pVBInfo->VBType & VB_XGI301LV )
        {
            if ( pVBInfo->TVInfo & SetYPbPrMode1080i )
                tempbx -= 10 ;
            else
            {
                if ( pVBInfo->TVInfo & TVSimuMode )
                {
                    if ( pVBInfo->TVInfo & SetPALTV )
                    {
                        if ( pVBInfo->VBType & VB_XGI301LV )
                        {
                            if ( !( pVBInfo->TVInfo & ( SetYPbPrMode525p | SetYPbPrMode750p | SetYPbPrMode1080i ) ) )
                                tempbx += 40 ;
                        }
                        else
                            tempbx += 40 ;
                    }
                }
            }
        }
        else
            tempbx -= 10 ;
    }
    else
    {
        if ( pVBInfo->TVInfo & TVSimuMode )
        {
            if ( pVBInfo->TVInfo & SetPALTV )
            {
                if ( pVBInfo->VBType & VB_XGI301LV )
                {
                    if ( !( pVBInfo->TVInfo & ( SetYPbPrMode525p | SetYPbPrMode750p | SetYPbPrMode1080i ) ) )
                        tempbx += 40 ;
                }
                else
                    tempbx += 40 ;
            }
        }
    }
    tempax = push1 ;
    tempax = tempax >> 2 ;
    tempax++ ;
    tempax += tempbx ;
    push1 = tempax ;						/* push ax */

    if ( ( pVBInfo->TVInfo & SetPALTV ) )
    {
        if ( tempbx <= 513 )
        {
            if ( tempax >= 513 )
            {
                tempbx = 513 ;
            }
        }
    }

    temp = tempbx & 0x00FF ;
    XGINew_SetReg1( pVBInfo->Part1Port , 0x0C , temp ) ;
    tempbx-- ;
    temp = tempbx & 0x00FF ;
    XGINew_SetReg1( pVBInfo->Part1Port , 0x10 , temp ) ;

    if ( tempbx & 0x0100 )
    {
        tempcx |= 0x0008 ;
    }

    if ( tempbx & 0x0200 )
    {
        XGINew_SetRegANDOR( pVBInfo->Part1Port , 0x0B , 0x0FF , 0x20 ) ;
    }

    tempbx++ ;

    if ( tempbx & 0x0100 )
    {
        tempcx |= 0x0004 ;
    }

    if ( tempbx & 0x0200 )
    {
        tempcx |= 0x0080 ;
    }

    if ( tempbx & 0x0400 )
    {
        tempcx |= 0x0C00 ;
    }

    tempbx = push1 ;						/* pop ax */
    temp = tempbx & 0x00FF ;
    temp &= 0x0F ;
    XGINew_SetReg1( pVBInfo->Part1Port , 0x0D , temp ) ;		/* 0x0D vertical Retrace End */

    if ( tempbx & 0x0010 )
    {
        tempcx |= 0x2000 ;
    }

    temp = tempcx & 0x00FF ;
    XGINew_SetReg1( pVBInfo->Part1Port , 0x0A , temp ) ;		/* 0x0A CR07 */
    temp = ( tempcx & 0x0FF00 ) >> 8 ;
    XGINew_SetReg1( pVBInfo->Part1Port , 0x17 , temp ) ;		/* 0x17 SR0A */
    tempax = modeflag ;
    temp = ( tempax & 0xFF00 ) >> 8 ;

    temp = ( temp >> 1 ) & 0x09 ;

    if ( pVBInfo->VBType & ( VB_XGI301LV | VB_XGI302LV | VB_XGI301C ) )
        temp |= 0x01 ;

    XGINew_SetReg1( pVBInfo->Part1Port , 0x16 , temp ) ;			/* 0x16 SR01 */
    XGINew_SetReg1( pVBInfo->Part1Port , 0x0F , 0 ) ;			/* 0x0F CR14 */
    XGINew_SetReg1( pVBInfo->Part1Port , 0x12 , 0 ) ;			/* 0x12 CR17 */

    if ( pVBInfo->LCDInfo & LCDRGB18Bit )
        temp = 0x80 ;
    else
        temp = 0x00 ;

    XGINew_SetReg1( pVBInfo->Part1Port , 0x1A , temp ) ;			/* 0x1A SR0E */

    return ;
}


/* --------------------------------------------------------------------- */
/* Function : XGI_SetGroup2 */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGI_SetGroup2( USHORT ModeNo, USHORT ModeIdIndex, USHORT RefreshRateTableIndex,
                    PXGI_HW_DEVICE_INFO HwDeviceExtension, PVB_DEVICE_INFO pVBInfo )
{
    USHORT i ,
           j ,
           tempax ,
           tempbx ,
           tempcx ,
           temp ,
           push1 ,
           push2 ,
           modeflag ;




/*           XGINew_RY1COE = 0 ,
           XGINew_RY2COE = 0 ,
           XGINew_RY3COE = 0 ,
           XGINew_RY4COE = 0 ,
           XGINew_RY5COE = 0 ,
           XGINew_RY6COE = 0 ,
           XGINew_RY7COE = 0 ;
*/
    UCHAR     *TimingPoint ;


    ULONG longtemp ,
          tempeax ,
          tempebx ,
          temp2 ,
          tempecx ;

    if ( ModeNo <= 0x13 )
    {
        modeflag = pVBInfo->SModeIDTable[ ModeIdIndex ].St_ModeFlag ;		/* si+St_ResInfo */
    }
    else
    {
        modeflag = pVBInfo->EModeIDTable[ ModeIdIndex ].Ext_ModeFlag ;		/* si+Ext_ResInfo */
    }

    tempax = 0 ;

    if ( !( pVBInfo->VBInfo & SetCRT2ToAVIDEO ) )
        tempax |= 0x0800 ;

    if ( !( pVBInfo->VBInfo & SetCRT2ToSVIDEO ) )
        tempax |= 0x0400 ;

    if ( pVBInfo->VBInfo & SetCRT2ToSCART )
        tempax |= 0x0200 ;

    if ( !( pVBInfo->TVInfo & SetPALTV ) )
        tempax |= 0x1000 ;

    if ( pVBInfo->VBInfo & SetCRT2ToHiVisionTV )
        tempax |= 0x0100 ;

    if ( pVBInfo->TVInfo & ( SetYPbPrMode525p | SetYPbPrMode750p ) )
        tempax &= 0xfe00 ;

    tempax = ( tempax & 0xff00 ) >> 8 ;

    XGINew_SetReg1( pVBInfo->Part2Port , 0x0 , tempax ) ;
    TimingPoint = pVBInfo->NTSCTiming ;

    if ( pVBInfo->TVInfo & SetPALTV )
    {
        TimingPoint = pVBInfo->PALTiming ;
    }

    if ( pVBInfo->VBInfo & SetCRT2ToHiVisionTV )
    {
        TimingPoint = pVBInfo->HiTVExtTiming ;

        if ( pVBInfo->VBInfo & SetInSlaveMode )
            TimingPoint = pVBInfo->HiTVSt2Timing ;

        if ( pVBInfo->SetFlag & TVSimuMode )
            TimingPoint = pVBInfo->HiTVSt1Timing ;

        if ( !(modeflag & Charx8Dot) )
            TimingPoint = pVBInfo->HiTVTextTiming ;
    }

    if ( pVBInfo->VBInfo & SetCRT2ToYPbPr )
    {
        if ( pVBInfo->TVInfo & SetYPbPrMode525i )
            TimingPoint = pVBInfo->YPbPr525iTiming ;

        if ( pVBInfo->TVInfo & SetYPbPrMode525p )
    	    TimingPoint = pVBInfo->YPbPr525pTiming ;

        if ( pVBInfo->TVInfo & SetYPbPrMode750p )
    	    TimingPoint = pVBInfo->YPbPr750pTiming ;
    }

    for( i = 0x01 , j = 0 ; i <= 0x2D ; i++ , j++ )
    {
        XGINew_SetReg1( pVBInfo->Part2Port , i , TimingPoint[ j ] ) ;
    }

    for( i = 0x39 ; i <= 0x45 ; i++ , j++ )
    {
        XGINew_SetReg1( pVBInfo->Part2Port , i , TimingPoint[ j ] ) ;	/* di->temp2[j] */
    }

    if ( pVBInfo->VBInfo & SetCRT2ToTV )
    {
        XGINew_SetRegANDOR( pVBInfo->Part2Port , 0x3A , 0x1F , 0x00 ) ;
    }

    temp = pVBInfo->NewFlickerMode ;
    temp &= 0x80 ;
    XGINew_SetRegANDOR( pVBInfo->Part2Port , 0x0A , 0xFF , temp ) ;

    if ( pVBInfo->VBInfo & SetCRT2ToHiVisionTV )
        tempax = 950 ;

    if ( pVBInfo->TVInfo & SetPALTV )
        tempax = 520 ;
    else
        tempax = 440 ;

    if ( pVBInfo->VDE <= tempax )
    {
        tempax -= pVBInfo->VDE ;
        tempax = tempax >> 2 ;
        tempax = ( tempax & 0x00FF ) | ( ( tempax & 0x00FF ) << 8 ) ;
        push1 = tempax ;
        temp = ( tempax & 0xFF00 ) >> 8 ;
        temp += ( USHORT )TimingPoint[ 0 ] ;

        if ( pVBInfo->VBType & ( VB_XGI301B | VB_XGI302B | VB_XGI301LV | VB_XGI302LV | VB_XGI301C ) )
        {
            if ( pVBInfo->VBInfo & ( SetCRT2ToAVIDEO | SetCRT2ToSVIDEO | SetCRT2ToSCART | SetCRT2ToYPbPr ) )
            {
                tempcx=pVBInfo->VGAHDE;
                if ( tempcx >= 1024 )
                {
                    temp = 0x17 ;		/* NTSC */
                    if ( pVBInfo->TVInfo & SetPALTV )
                        temp = 0x19 ;		/* PAL */
                }
            }
        }

        XGINew_SetReg1( pVBInfo->Part2Port , 0x01 , temp ) ;
        tempax = push1 ;
        temp = ( tempax & 0xFF00 ) >> 8 ;
        temp += TimingPoint[ 1 ] ;

        if ( pVBInfo->VBType & ( VB_XGI301B | VB_XGI302B | VB_XGI301LV | VB_XGI302LV | VB_XGI301C ) )
        {
            if ( ( pVBInfo->VBInfo & ( SetCRT2ToAVIDEO | SetCRT2ToSVIDEO | SetCRT2ToSCART | SetCRT2ToYPbPr ) ) )
            {
                tempcx = pVBInfo->VGAHDE ;
                if ( tempcx >= 1024 )
                {
                    temp = 0x1D ;		/* NTSC */
                    if ( pVBInfo->TVInfo & SetPALTV )
                        temp = 0x52 ;		/* PAL */
                }
            }
        }
        XGINew_SetReg1( pVBInfo->Part2Port , 0x02 , temp ) ;
    }

    /* 301b */
    tempcx = pVBInfo->HT ;

    if ( XGI_IsLCDDualLink( pVBInfo ) )
        tempcx = tempcx >> 1 ;

    tempcx -= 2 ;
    temp = tempcx & 0x00FF ;
    XGINew_SetReg1( pVBInfo->Part2Port , 0x1B , temp ) ;

    temp = ( tempcx & 0xFF00 ) >> 8 ;
    XGINew_SetRegANDOR( pVBInfo->Part2Port , 0x1D , ~0x0F , temp ) ;

    tempcx = pVBInfo->HT >> 1 ;
    push1 = tempcx ;				/* push cx */
    tempcx += 7 ;

    if ( pVBInfo->VBInfo & SetCRT2ToHiVisionTV )
    {
        tempcx -= 4 ;
    }

    temp = tempcx & 0x00FF ;
    temp = temp << 4 ;
    XGINew_SetRegANDOR( pVBInfo->Part2Port , 0x22 , 0x0F , temp ) ;

    tempbx = TimingPoint[ j ] | ( ( TimingPoint[ j + 1 ] ) << 8 ) ;
    tempbx += tempcx ;
    push2 = tempbx ;
    temp = tempbx & 0x00FF ;
    XGINew_SetReg1( pVBInfo->Part2Port , 0x24 , temp ) ;
    temp = ( tempbx & 0xFF00 ) >> 8 ;
    temp = temp << 4 ;
    XGINew_SetRegANDOR(pVBInfo->Part2Port,0x25,0x0F,temp);

    tempbx=push2;
    tempbx=tempbx+8;
    if ( pVBInfo->VBInfo & SetCRT2ToHiVisionTV )
    {
        tempbx=tempbx-4;
        tempcx=tempbx;
    }

    temp = ( tempbx & 0x00FF ) << 4 ;
    XGINew_SetRegANDOR( pVBInfo->Part2Port , 0x29 , 0x0F , temp ) ;

    j += 2 ;
    tempcx += ( TimingPoint[ j ] | ( ( TimingPoint[ j + 1 ] ) << 8 ) ) ;
    temp = tempcx & 0x00FF ;
    XGINew_SetReg1( pVBInfo->Part2Port , 0x27 , temp ) ;
    temp = ( ( tempcx & 0xFF00 ) >> 8 ) << 4 ;
    XGINew_SetRegANDOR( pVBInfo->Part2Port , 0x28 , 0x0F , temp ) ;

    tempcx += 8 ;
    if ( pVBInfo->VBInfo & SetCRT2ToHiVisionTV )
    {
        tempcx -= 4 ;
    }

    temp = tempcx & 0xFF ;
    temp = temp << 4 ;
    XGINew_SetRegANDOR( pVBInfo->Part2Port , 0x2A , 0x0F , temp ) ;

    tempcx = push1 ;					/* pop cx */
    j += 2 ;
    temp = TimingPoint[ j ] | ( ( TimingPoint[ j + 1 ] ) << 8 ) ;
    tempcx -= temp ;
    temp = tempcx & 0x00FF ;
    temp = temp << 4 ;
    XGINew_SetRegANDOR( pVBInfo->Part2Port , 0x2D , 0x0F ,temp ) ;

    tempcx -= 11 ;

    if ( !( pVBInfo->VBInfo & SetCRT2ToTV ) )
    {
        tempax = XGI_GetVGAHT2( pVBInfo) ;
        tempcx = tempax - 1 ;
    }
    temp = tempcx & 0x00FF ;
    XGINew_SetReg1( pVBInfo->Part2Port , 0x2E , temp ) ;

    tempbx = pVBInfo->VDE ;

    if ( pVBInfo->VGAVDE == 360 )
        tempbx = 746 ;
    if ( pVBInfo->VGAVDE == 375 )
        tempbx = 746 ;
    if ( pVBInfo->VGAVDE == 405 )
        tempbx = 853 ;

    if ( pVBInfo->VBInfo & SetCRT2ToTV )
    {
        if ( pVBInfo->VBType & ( VB_XGI301LV | VB_XGI302LV | VB_XGI301C ) )
        {
            if ( !( pVBInfo->TVInfo & ( SetYPbPrMode525p | SetYPbPrMode750p ) ) )
                tempbx = tempbx >> 1 ;
        }
        else
            tempbx = tempbx >> 1 ;
    }

    tempbx -= 2 ;
    temp = tempbx & 0x00FF ;

    if ( pVBInfo->VBInfo & SetCRT2ToHiVisionTV )
    {
        if ( pVBInfo->VBType & VB_XGI301LV )
        {
            if ( pVBInfo->TVInfo & SetYPbPrMode1080i )
            {
                if ( pVBInfo->VBInfo & SetInSlaveMode )
                {
                    if ( ModeNo == 0x2f )
                        temp += 1 ;
                }
            }
        }
        else
        {
            if ( pVBInfo->VBInfo & SetInSlaveMode )
            {
                if ( ModeNo == 0x2f )
                    temp += 1 ;
            }
        }
    }

    XGINew_SetReg1( pVBInfo->Part2Port , 0x2F , temp ) ;

    temp = ( tempcx & 0xFF00 ) >> 8 ;
    temp |= ( ( tempbx & 0xFF00 ) >> 8 ) << 6 ;

    if ( !( pVBInfo->VBInfo & SetCRT2ToHiVisionTV ) )
    {
        if ( pVBInfo->VBType & VB_XGI301LV )
        {
            if ( pVBInfo->TVInfo & SetYPbPrMode1080i )
            {
                temp |= 0x10 ;

                if ( !( pVBInfo->VBInfo & SetCRT2ToSVIDEO ) )
                    temp |= 0x20 ;
            }
        }
        else
        {
            temp |= 0x10 ;
            if ( !( pVBInfo->VBInfo & SetCRT2ToSVIDEO ) )
                temp |= 0x20 ;
        }
    }

    XGINew_SetReg1( pVBInfo->Part2Port , 0x30 , temp ) ;

    if ( pVBInfo->VBType & ( VB_XGI301B | VB_XGI302B | VB_XGI301LV | VB_XGI302LV | VB_XGI301C ) )	/* TV gatingno */
    {
        tempbx = pVBInfo->VDE ;
        tempcx = tempbx - 2 ;

        if ( pVBInfo->VBInfo & SetCRT2ToTV )
        {
            if ( !( pVBInfo->TVInfo & ( SetYPbPrMode525p | SetYPbPrMode750p ) ) )
                tempbx = tempbx >> 1 ;
        }

        if ( pVBInfo->VBType & ( VB_XGI302LV | VB_XGI301C ) )
        {
            temp=0;
            if( tempcx & 0x0400 )
                temp |= 0x20 ;

            if ( tempbx & 0x0400 )
                temp |= 0x40 ;

            XGINew_SetReg1( pVBInfo->Part4Port , 0x10 , temp ) ;
        }

        temp = ( ( ( tempbx - 3 ) & 0x0300 ) >> 8 ) << 5 ;
        XGINew_SetReg1( pVBInfo->Part2Port , 0x46 , temp ) ;
        temp = ( tempbx - 3 ) & 0x00FF ;
        XGINew_SetReg1( pVBInfo->Part2Port , 0x47 , temp ) ;
    }

    tempbx = tempbx & 0x00FF ;

    if ( !( modeflag & HalfDCLK ) )
    {
        tempcx = pVBInfo->VGAHDE ;
        if ( tempcx >= pVBInfo->HDE )
        {
            tempbx |= 0x2000 ;
            tempax &= 0x00FF ;
        }
    }

    tempcx = 0x0101 ;

    if( pVBInfo->VBInfo & SetCRT2ToTV ) { /*301b*/
        if(pVBInfo->VGAHDE>=1024)
        {
	    tempcx=0x1920;
            if(pVBInfo->VGAHDE>=1280)
            {
	        tempcx=0x1420;
	        tempbx=tempbx&0xDFFF;
            }
        }
    }

    if ( !( tempbx & 0x2000 ) )
    {
        if ( modeflag & HalfDCLK )
        {
            tempcx = ( tempcx & 0xFF00 ) | ( ( tempcx & 0x00FF ) << 1 ) ;
	}

        push1 = tempbx ;
        tempeax = pVBInfo->VGAHDE ;
        tempebx = ( tempcx & 0xFF00 ) >> 8 ;
        longtemp = tempeax * tempebx ;
        tempecx = tempcx & 0x00FF ;
        longtemp = longtemp / tempecx ;

	/* 301b */
        tempecx = 8 * 1024 ;

        if ( pVBInfo->VBType & ( VB_XGI301B | VB_XGI302B | VB_XGI301LV | VB_XGI302LV | VB_XGI301C ) )
        {
            tempecx = tempecx * 8 ;
        }

        longtemp = longtemp * tempecx ;
        tempecx = pVBInfo->HDE ;
        temp2 = longtemp % tempecx ;
        tempeax = longtemp / tempecx ;
        if ( temp2 != 0 )
        {
            tempeax += 1 ;
        }

        tempax = ( USHORT )tempeax ;

	/* 301b */
        if ( pVBInfo->VBType & ( VB_XGI301B | VB_XGI302B | VB_XGI301LV | VB_XGI302LV | VB_XGI301C ) )
        {
            tempcx = ( ( tempax & 0xFF00 ) >> 5 ) >> 8 ;
        }
        /* end 301b */

        tempbx = push1 ;
        tempbx =( USHORT )( ( ( tempeax & 0x0000FF00 ) & 0x1F00 ) | ( tempbx & 0x00FF ) ) ;
        tempax =( USHORT )( ( ( tempeax & 0x000000FF ) << 8 ) | ( tempax & 0x00FF ) ) ;
        temp = ( tempax & 0xFF00 ) >> 8 ;
    }
    else
    {
        temp = ( tempax & 0x00FF ) >> 8 ;
    }

    XGINew_SetReg1( pVBInfo->Part2Port , 0x44 , temp ) ;
    temp = ( tempbx & 0xFF00 ) >> 8 ;
    XGINew_SetRegANDOR( pVBInfo->Part2Port , 0x45 , ~0x03F , temp ) ;
    temp = tempcx & 0x00FF ;

    if ( tempbx & 0x2000 )
        temp = 0 ;

    if ( !( pVBInfo->VBInfo & SetCRT2ToLCD ) )
        temp |= 0x18 ;

    XGINew_SetRegANDOR(pVBInfo->Part2Port,0x46,~0x1F,temp);
    if ( pVBInfo->TVInfo & SetPALTV )
    {
        tempbx = 0x0382 ;
        tempcx = 0x007e ;
    }
    else
    {
        tempbx = 0x0369 ;
        tempcx = 0x0061 ;
    }

    temp = tempbx & 0x00FF ;
    XGINew_SetReg1( pVBInfo->Part2Port , 0x4b , temp ) ;
    temp = tempcx & 0x00FF ;
    XGINew_SetReg1( pVBInfo->Part2Port , 0x4c , temp ) ;

    temp = ( ( tempcx & 0xFF00 ) >> 8 ) & 0x03 ;
    temp = temp << 2 ;
    temp |= ( ( tempbx & 0xFF00 ) >> 8 ) & 0x03 ;

    if ( pVBInfo->VBInfo & SetCRT2ToYPbPr )
    {
        temp |= 0x10 ;

        if ( pVBInfo->TVInfo & SetYPbPrMode525p )
            temp |= 0x20 ;

        if ( pVBInfo->TVInfo & SetYPbPrMode750p )
            temp |= 0x60 ;
    }

    XGINew_SetReg1( pVBInfo->Part2Port , 0x4d , temp ) ;
    temp=XGINew_GetReg1( pVBInfo->Part2Port , 0x43 ) ;		/* 301b change */
    XGINew_SetReg1( pVBInfo->Part2Port , 0x43 , ( USHORT )( temp - 3 ) ) ;

    if ( !( pVBInfo->TVInfo & ( SetYPbPrMode525p | SetYPbPrMode750p ) ) )
    {
        if ( pVBInfo->TVInfo & NTSC1024x768 )
        {
            TimingPoint = XGI_NTSC1024AdjTime ;
            for( i = 0x1c , j = 0 ; i <= 0x30 ; i++ , j++ )
            {
                XGINew_SetReg1( pVBInfo->Part2Port , i , TimingPoint[ j ] ) ;
            }
            XGINew_SetReg1( pVBInfo->Part2Port , 0x43 , 0x72 ) ;
        }
    }

    /* [ycchen] 01/14/03 Modify for 301C PALM Support */
    if ( pVBInfo->VBType & VB_XGI301C )
    {
        if ( pVBInfo->TVInfo & SetPALMTV )
	    XGINew_SetRegANDOR( pVBInfo->Part2Port , 0x4E , ~0x08 , 0x08 ) ;	/* PALM Mode */
    }

    if ( pVBInfo->TVInfo & SetPALMTV )
    {
        tempax = ( UCHAR )XGINew_GetReg1( pVBInfo->Part2Port , 0x01 ) ;
        tempax-- ;
        XGINew_SetRegAND( pVBInfo->Part2Port , 0x01 , tempax ) ;

        /* if ( !( pVBInfo->VBType & VB_XGI301C ) ) */
        XGINew_SetRegAND( pVBInfo->Part2Port , 0x00 , 0xEF ) ;
    }

    if ( pVBInfo->VBInfo & SetCRT2ToHiVisionTV )
    {
        if ( !( pVBInfo->VBInfo & SetInSlaveMode ) )
        {
            XGINew_SetReg1( pVBInfo->Part2Port , 0x0B , 0x00 ) ;
        }
    }
}


/* --------------------------------------------------------------------- */
/* Function : XGI_SetLCDRegs */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void  XGI_SetLCDRegs(USHORT ModeNo,USHORT ModeIdIndex, PXGI_HW_DEVICE_INFO HwDeviceExtension,USHORT RefreshRateTableIndex, PVB_DEVICE_INFO pVBInfo)
{
    USHORT pushbx ,
           tempax ,
           tempbx ,
           tempcx ,
           temp ,
           tempah ,
           tempbh ,
           tempch ,
           CRT1Index ;

    XGI_LCDDesStruct *LCDBDesPtr = NULL ;


    if ( ModeNo >= 0x13 )
    {
        CRT1Index = pVBInfo->RefIndex[ RefreshRateTableIndex ].Ext_CRT1CRTC ;
        CRT1Index &= IndexMask ;
    }

    if ( !( pVBInfo->VBInfo & SetCRT2ToLCD ) )
    {
        return ;
    }

    tempbx = pVBInfo->HDE ;			/* RHACTE=HDE-1 */

    if ( XGI_IsLCDDualLink( pVBInfo ) )
        tempbx = tempbx >> 1 ;

    tempbx -= 1 ;
    temp = tempbx & 0x00FF ;
    XGINew_SetReg1( pVBInfo->Part2Port , 0x2C , temp ) ;
    temp = ( tempbx & 0xFF00 ) >> 8 ;
    temp = temp << 4 ;
    XGINew_SetRegANDOR( pVBInfo->Part2Port , 0x2B , 0x0F , temp ) ;
    temp = 0x01 ;

    if ( pVBInfo->LCDResInfo == Panel1280x1024 )
    {
        if ( pVBInfo->ModeType == ModeEGA )
        {
            if ( pVBInfo->VGAHDE >= 1024 )
            {
                temp = 0x02 ;
                if ( pVBInfo->LCDInfo & LCDVESATiming )
                    temp = 0x01 ;
            }
        }
    }

    XGINew_SetReg1( pVBInfo->Part2Port , 0x0B , temp ) ;
    tempbx = pVBInfo->VDE ;			/* RTVACTEO=(VDE-1)&0xFF */
    tempbx-- ;
    temp = tempbx & 0x00FF ;
    XGINew_SetReg1( pVBInfo->Part2Port , 0x03 , temp ) ;
    temp = ( ( tempbx & 0xFF00 ) >> 8 ) & 0x07 ;
    XGINew_SetRegANDOR( pVBInfo->Part2Port , 0x0C , ~0x07 , temp ) ;

    tempcx = pVBInfo->VT - 1 ;
    temp = tempcx & 0x00FF ;			/* RVTVT=VT-1 */
    XGINew_SetReg1( pVBInfo->Part2Port , 0x19 , temp ) ;
    temp = ( tempcx & 0xFF00 ) >> 8 ;
    temp = temp << 5 ;
    XGINew_SetReg1( pVBInfo->Part2Port , 0x1A , temp ) ;
    XGINew_SetRegANDOR( pVBInfo->Part2Port , 0x09 , 0xF0 , 0x00 ) ;
    XGINew_SetRegANDOR( pVBInfo->Part2Port , 0x0A , 0xF0 , 0x00 ) ;
    XGINew_SetRegANDOR( pVBInfo->Part2Port , 0x17 , 0xFB , 0x00 ) ;
    XGINew_SetRegANDOR( pVBInfo->Part2Port , 0x18 , 0xDF , 0x00 ) ;

    /* Customized LCDB Des no add */
    tempbx = 5 ;
    LCDBDesPtr = ( XGI_LCDDesStruct * )XGI_GetLcdPtr( tempbx , ModeNo , ModeIdIndex , RefreshRateTableIndex, pVBInfo ) ;
    tempah = pVBInfo->LCDResInfo ;
    tempah &= PanelResInfo ;

    if ( ( tempah == Panel1024x768 ) || ( tempah == Panel1024x768x75 ) )
    {
        tempbx = 1024 ;
        tempcx = 768 ;
    }
    else if ( ( tempah == Panel1280x1024 ) || ( tempah == Panel1280x1024x75 ) )
    {
        tempbx = 1280 ;
        tempcx = 1024 ;
    }
    else if ( tempah == Panel1400x1050 )
    {
        tempbx = 1400 ;
        tempcx = 1050 ;
    }
    else
    {
        tempbx = 1600 ;
        tempcx = 1200 ;
    }

    if ( pVBInfo->LCDInfo & EnableScalingLCD )
    {
        tempbx = pVBInfo->HDE ;
        tempcx = pVBInfo->VDE ;
    }

    pushbx = tempbx ;
    tempax = pVBInfo->VT ;
    pVBInfo->LCDHDES = LCDBDesPtr->LCDHDES ;
    pVBInfo->LCDHRS = LCDBDesPtr->LCDHRS ;
    pVBInfo->LCDVDES = LCDBDesPtr->LCDVDES ;
    pVBInfo->LCDVRS = LCDBDesPtr->LCDVRS ;
    tempbx = pVBInfo->LCDVDES ;
    tempcx += tempbx ;

    if ( tempcx >= tempax )
        tempcx -= tempax ;	/* lcdvdes */

    temp = tempbx & 0x00FF ;	/* RVEQ1EQ=lcdvdes */
    XGINew_SetReg1( pVBInfo->Part2Port , 0x05 , temp ) ;
    temp = tempcx & 0x00FF ;
    XGINew_SetReg1( pVBInfo->Part2Port , 0x06 , temp ) ;
    tempch = ( ( tempcx & 0xFF00 ) >> 8 ) & 0x07 ;
    tempbh = ( ( tempbx & 0xFF00 ) >> 8 ) & 0x07 ;
    tempah = tempch ;
    tempah = tempah << 3 ;
    tempah |= tempbh ;
    XGINew_SetReg1( pVBInfo->Part2Port , 0x02 , tempah ) ;

    /* getlcdsync() */
    XGI_GetLCDSync( &tempax , &tempbx,pVBInfo ) ;
    tempcx = tempbx ;
    tempax = pVBInfo->VT ;
    tempbx = pVBInfo->LCDVRS ;

    /* if ( SetLCD_Info & EnableScalingLCD ) */
    tempcx += tempbx ;
    if ( tempcx >= tempax )
        tempcx -= tempax ;

    temp = tempbx & 0x00FF ;	/* RTVACTEE=lcdvrs */
    XGINew_SetReg1( pVBInfo->Part2Port , 0x04 , temp ) ;
    temp = ( tempbx & 0xFF00 ) >> 8 ;
    temp = temp << 4 ;
    temp |= ( tempcx & 0x000F ) ;
    XGINew_SetReg1( pVBInfo->Part2Port , 0x01 , temp ) ;
    tempcx = pushbx ;
    tempax = pVBInfo->HT ;
    tempbx = pVBInfo->LCDHDES ;
    tempbx &= 0x0FFF ;

    if ( XGI_IsLCDDualLink(  pVBInfo ) )
    {
        tempax = tempax >> 1 ;
        tempbx = tempbx >> 1 ;
        tempcx = tempcx >> 1 ;
    }

    if ( pVBInfo->VBType & VB_XGI302LV )
        tempbx += 1 ;

    if ( pVBInfo->VBType & VB_XGI301C )  /* tap4 */
        tempbx += 1 ;

    tempcx += tempbx ;

    if ( tempcx >= tempax )
        tempcx -= tempax ;

    temp = tempbx & 0x00FF ;
    XGINew_SetReg1( pVBInfo->Part2Port , 0x1F , temp ) ;	/* RHBLKE=lcdhdes */
    temp = ( ( tempbx & 0xFF00 ) >> 8 ) << 4 ;
    XGINew_SetReg1( pVBInfo->Part2Port , 0x20 , temp ) ;
    temp = tempcx & 0x00FF ;
    XGINew_SetReg1( pVBInfo->Part2Port , 0x23 , temp ) ;	/* RHEQPLE=lcdhdee */
    temp = ( tempcx & 0xFF00 ) >> 8 ;
    XGINew_SetReg1( pVBInfo->Part2Port , 0x25 , temp ) ;

    /* getlcdsync() */
    XGI_GetLCDSync( &tempax , &tempbx ,pVBInfo) ;
    tempcx = tempax ;
    tempax = pVBInfo->HT ;
    tempbx = pVBInfo->LCDHRS ;
    /* if ( SetLCD_Info & EnableScalingLCD) */
    if ( XGI_IsLCDDualLink( pVBInfo) )
    {
        tempax = tempax >> 1 ;
        tempbx = tempbx >> 1 ;
        tempcx = tempcx >> 1 ;
    }

    if ( pVBInfo->VBType & VB_XGI302LV )
        tempbx += 1 ;

    tempcx += tempbx ;

    if ( tempcx >= tempax )
        tempcx -= tempax ;

    temp = tempbx & 0x00FF ;	/* RHBURSTS=lcdhrs */
    XGINew_SetReg1( pVBInfo->Part2Port , 0x1C , temp ) ;

    temp = ( tempbx & 0xFF00 ) >> 8 ;
    temp = temp << 4 ;
    XGINew_SetRegANDOR( pVBInfo->Part2Port , 0x1D , ~0x0F0 , temp ) ;
    temp = tempcx & 0x00FF ;	/* RHSYEXP2S=lcdhre */
    XGINew_SetReg1( pVBInfo->Part2Port , 0x21 , temp ) ;

    if ( !( pVBInfo->LCDInfo & LCDVESATiming ) )
    {
        if ( pVBInfo->VGAVDE == 525 )
        {
            if ( pVBInfo->VBType & ( VB_XGI301B | VB_XGI302B | VB_XGI301LV | VB_XGI302LV | VB_XGI301C ) )
            {
                temp = 0xC6 ;
            }
            else
                temp = 0xC4 ;

            XGINew_SetReg1( pVBInfo->Part2Port , 0x2f , temp ) ;
            XGINew_SetReg1( pVBInfo->Part2Port , 0x30 , 0xB3 ) ;
        }

        if ( pVBInfo->VGAVDE == 420 )
        {
            if ( pVBInfo->VBType & ( VB_XGI301B | VB_XGI302B | VB_XGI301LV | VB_XGI302LV | VB_XGI301C ) )
            {
                temp = 0x4F ;
            }
            else
                temp = 0x4E ;
            XGINew_SetReg1( pVBInfo->Part2Port , 0x2f , temp ) ;
        }
    }
}


/* --------------------------------------------------------------------- */
/* Function : XGI_GetTap4Ptr */
/* Input : */
/* Output : di -> Tap4 Reg. Setting Pointer */
/* Description : */
/* --------------------------------------------------------------------- */
XGI301C_Tap4TimingStruct* XGI_GetTap4Ptr(USHORT tempcx, PVB_DEVICE_INFO pVBInfo)
{
    USHORT tempax ,
           tempbx ,
           i ;

    XGI301C_Tap4TimingStruct *Tap4TimingPtr ;

    if ( tempcx == 0 )
    {
        tempax = pVBInfo->VGAHDE ;
        tempbx = pVBInfo->HDE ;
    }
    else
    {
        tempax = pVBInfo->VGAVDE ;
        tempbx = pVBInfo->VDE ;
    }

    if ( tempax < tempbx )
        return &EnlargeTap4Timing[ 0 ] ;
    else if( tempax == tempbx )
        return &NoScaleTap4Timing[ 0 ] ; 	/* 1:1 */
    else
        Tap4TimingPtr = NTSCTap4Timing ; 	/* NTSC */

    if ( pVBInfo->TVInfo & SetPALTV )
        Tap4TimingPtr = PALTap4Timing ;


    if ( pVBInfo->VBInfo & SetCRT2ToYPbPr )
    {
        if ( pVBInfo->TVInfo & SetYPbPrMode525i )
            Tap4TimingPtr = YPbPr525iTap4Timing ;
        if ( pVBInfo->TVInfo & SetYPbPrMode525p )
            Tap4TimingPtr = YPbPr525pTap4Timing ;
        if ( pVBInfo->TVInfo & SetYPbPrMode750p )
            Tap4TimingPtr = YPbPr750pTap4Timing ;
    }

    if ( pVBInfo->VBInfo & SetCRT2ToHiVisionTV )
        Tap4TimingPtr = HiTVTap4Timing ;

    i = 0 ;
    while( Tap4TimingPtr[ i ].DE != 0xFFFF )
    {
        if ( Tap4TimingPtr[ i ].DE == tempax )
            break ;
        i++ ;
    }
    return &Tap4TimingPtr[ i ] ;
}


/* --------------------------------------------------------------------- */
/* Function : XGI_SetTap4Regs */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGI_SetTap4Regs( PVB_DEVICE_INFO pVBInfo)
{
    USHORT    i ,
              j ;


    XGI301C_Tap4TimingStruct *Tap4TimingPtr ;

    if ( !( pVBInfo->VBType & VB_XGI301C ) )
        return ;

#ifndef Tap4
    XGINew_SetRegAND( pVBInfo->Part2Port , 0x4E , 0xEB ) ;	/* Disable Tap4 */
#else            /* Tap4 Setting */

    Tap4TimingPtr = XGI_GetTap4Ptr( 0 , pVBInfo) ;  /* Set Horizontal Scaling */
    for( i = 0x80 , j = 0 ; i <= 0xBF ; i++ , j++ )
        XGINew_SetReg1( pVBInfo->Part2Port , i , Tap4TimingPtr->Reg[ j ] ) ;

    if ( ( pVBInfo->VBInfo & SetCRT2ToTV ) && ( !( pVBInfo->VBInfo & SetCRT2ToHiVisionTV ) ) )
    {
        Tap4TimingPtr = XGI_GetTap4Ptr( 1 , pVBInfo);	/* Set Vertical Scaling */
        for( i = 0xC0 , j = 0 ; i < 0xFF ; i++ , j++ )
            XGINew_SetReg1( pVBInfo->Part2Port , i , Tap4TimingPtr->Reg[ j ] ) ;
    }

    if ( ( pVBInfo->VBInfo & SetCRT2ToTV ) && ( !( pVBInfo->VBInfo & SetCRT2ToHiVisionTV ) ) )
        XGINew_SetRegANDOR( pVBInfo->Part2Port , 0x4E , ~0x14 , 0x04 ) ;	/* Enable V.Scaling */
    else
        XGINew_SetRegANDOR( pVBInfo->Part2Port , 0x4E , ~0x14 , 0x10 ) ;	/* Enable H.Scaling */
#endif
}

/* --------------------------------------------------------------------- */
/* Function : XGI_SetGroup3 */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGI_SetGroup3(USHORT ModeNo,USHORT ModeIdIndex, PVB_DEVICE_INFO pVBInfo)
{
    USHORT i;
    UCHAR *tempdi;
    USHORT  modeflag;

    if(ModeNo<=0x13)
    {
        modeflag = pVBInfo->SModeIDTable[ModeIdIndex].St_ModeFlag;      /* si+St_ResInfo */
    }
    else
    {
        modeflag = pVBInfo->EModeIDTable[ModeIdIndex].Ext_ModeFlag;     /* si+Ext_ResInfo */
    }


    XGINew_SetReg1(pVBInfo->Part3Port,0x00,0x00);
    if(pVBInfo->TVInfo&SetPALTV)
    {
        XGINew_SetReg1(pVBInfo->Part3Port,0x13,0xFA);
        XGINew_SetReg1(pVBInfo->Part3Port,0x14,0xC8);
    }
    else
    {
        XGINew_SetReg1(pVBInfo->Part3Port,0x13,0xF5);
        XGINew_SetReg1(pVBInfo->Part3Port,0x14,0xB7);
    }

    if(!(pVBInfo->VBInfo&SetCRT2ToTV))
    {
        return;
    }

    if(pVBInfo->TVInfo&SetPALMTV)
    {
        XGINew_SetReg1(pVBInfo->Part3Port,0x13,0xFA);
        XGINew_SetReg1(pVBInfo->Part3Port,0x14,0xC8);
        XGINew_SetReg1(pVBInfo->Part3Port,0x3D,0xA8);
    }

    if((pVBInfo->VBInfo&SetCRT2ToHiVisionTV)|| (pVBInfo->VBInfo&SetCRT2ToYPbPr))
    {
        if(pVBInfo->TVInfo & SetYPbPrMode525i)
        {
            return;
        }
        tempdi=pVBInfo->HiTVGroup3Data;
        if(pVBInfo->SetFlag&TVSimuMode)
        {
            tempdi=pVBInfo->HiTVGroup3Simu;
            if(!(modeflag&Charx8Dot))
            {
                tempdi=pVBInfo->HiTVGroup3Text;
            }
        }

        if(pVBInfo->TVInfo & SetYPbPrMode525p)
        {
            tempdi=pVBInfo->Ren525pGroup3;
        }
        if(pVBInfo->TVInfo & SetYPbPrMode750p)
        {
             tempdi=pVBInfo->Ren750pGroup3;
        }

        for(i=0;i<=0x3E;i++)
        {
            XGINew_SetReg1(pVBInfo->Part3Port,i,tempdi[i]);
        }
        if(pVBInfo->VBType&VB_XGI301C)  /* Marcovision */
        {
            if(pVBInfo->TVInfo & SetYPbPrMode525p)
            {
                XGINew_SetReg1(pVBInfo->Part3Port,0x28,0x3f);
            }
        }
    }
    return;
}  /* {end of XGI_SetGroup3} */


/* --------------------------------------------------------------------- */
/* Function : XGI_SetGroup4 */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGI_SetGroup4(USHORT ModeNo,USHORT ModeIdIndex,USHORT RefreshRateTableIndex,PXGI_HW_DEVICE_INFO HwDeviceExtension, PVB_DEVICE_INFO pVBInfo)
{
    USHORT tempax ,
           tempcx ,
           tempbx ,
           modeflag ,
           temp ,
           temp2 ;


    ULONG tempebx ,
          tempeax ,
          templong ;


    if ( ModeNo <= 0x13 )
    {
        modeflag = pVBInfo->SModeIDTable[ ModeIdIndex ].St_ModeFlag ;	/* si+St_ResInfo */
    }
    else
    {
        modeflag = pVBInfo->EModeIDTable[ ModeIdIndex ].Ext_ModeFlag ;	/* si+Ext_ResInfo */
    }

    temp = pVBInfo->RVBHCFACT ;
    XGINew_SetReg1( pVBInfo->Part4Port , 0x13 , temp ) ;

    tempbx = pVBInfo->RVBHCMAX ;
    temp = tempbx & 0x00FF ;
    XGINew_SetReg1( pVBInfo->Part4Port , 0x14 , temp ) ;
    temp2 = ( ( tempbx & 0xFF00 ) >> 8 ) << 7 ;
    tempcx = pVBInfo->VGAHT - 1 ;
    temp = tempcx & 0x00FF ;
    XGINew_SetReg1( pVBInfo->Part4Port , 0x16 , temp ) ;

    temp =( ( tempcx & 0xFF00 ) >> 8 ) << 3 ;
    temp2 |= temp ;

    tempcx = pVBInfo->VGAVT - 1 ;
    if ( !( pVBInfo->VBInfo & SetCRT2ToTV ) )
    {
        tempcx -= 5 ;
    }

    temp = tempcx & 0x00FF ;
    XGINew_SetReg1( pVBInfo->Part4Port , 0x17 , temp ) ;
    temp = temp2 | ( ( tempcx & 0xFF00 ) >> 8 ) ;
    XGINew_SetReg1( pVBInfo->Part4Port , 0x15 , temp ) ;
    XGINew_SetRegOR( pVBInfo->Part4Port , 0x0D , 0x08 ) ;
    tempcx = pVBInfo->VBInfo ;
    tempbx = pVBInfo->VGAHDE ;

    if ( modeflag & HalfDCLK )
    {
        tempbx = tempbx >> 1 ;
    }

    if ( XGI_IsLCDDualLink( pVBInfo ) )
        tempbx = tempbx >> 1 ;

    if(tempcx&SetCRT2ToHiVisionTV)
    {
        temp=0;
        if(tempbx<=1024)
            temp=0xA0;
       if(tempbx == 1280)
          temp = 0xC0;
    }
    else if(tempcx&SetCRT2ToTV)
    {
         temp=0xA0;
         if(tempbx <= 800)
             temp=0x80;
    }
    else
    {
	 temp=0x80;
	 if(pVBInfo->VBInfo&SetCRT2ToLCD)
	 {
	     temp=0;
	     if(tempbx>800)
	         temp=0x60;
	 }
    }

    if ( pVBInfo->TVInfo & ( SetYPbPrMode525p | SetYPbPrMode750p ) )
    {
        temp = 0x00 ;
        if ( pVBInfo->VGAHDE == 1280 )
            temp = 0x40 ;
        if ( pVBInfo->VGAHDE == 1024 )
	    temp = 0x20 ;
    }
    XGINew_SetRegANDOR( pVBInfo->Part4Port , 0x0E , ~0xEF , temp ) ;

    tempebx = pVBInfo->VDE ;

    if ( tempcx & SetCRT2ToHiVisionTV )
    {
        if ( !( temp & 0xE000 ) )
            tempbx = tempbx >> 1 ;
    }

    tempcx = pVBInfo->RVBHRS ;
    temp = tempcx & 0x00FF ;
    XGINew_SetReg1( pVBInfo->Part4Port , 0x18 , temp );

    tempeax = pVBInfo->VGAVDE ;
    tempcx |= 0x04000 ;


    if ( tempeax <= tempebx )
    {
        tempcx=(tempcx&(~0x4000));
        tempeax = pVBInfo->VGAVDE ;
    }
    else
    {
        tempeax -= tempebx ;
    }


    templong = ( tempeax * 256 * 1024 ) % tempebx ;
    tempeax = ( tempeax * 256 * 1024 ) / tempebx ;
    tempebx = tempeax ;

    if ( templong != 0 )
    {
        tempebx++ ;
    }


    temp = ( USHORT )( tempebx & 0x000000FF ) ;
    XGINew_SetReg1( pVBInfo->Part4Port , 0x1B , temp ) ;

    temp = ( USHORT )( ( tempebx & 0x0000FF00 ) >> 8 ) ;
    XGINew_SetReg1( pVBInfo->Part4Port , 0x1A , temp ) ;
    tempbx = ( USHORT )( tempebx >> 16 ) ;
    temp = tempbx & 0x00FF ;
    temp = temp << 4 ;
    temp |= ( ( tempcx & 0xFF00 ) >> 8 ) ;
    XGINew_SetReg1( pVBInfo->Part4Port , 0x19 , temp ) ;

    /* 301b */
    if ( pVBInfo->VBType & ( VB_XGI301B | VB_XGI302B | VB_XGI301LV | VB_XGI302LV | VB_XGI301C ) )
    {
        temp = 0x0028 ;
        XGINew_SetReg1( pVBInfo->Part4Port , 0x1C , temp ) ;
        tempax = pVBInfo->VGAHDE ;
        if ( modeflag & HalfDCLK )
        {
            tempax = tempax >> 1 ;
        }

        if ( XGI_IsLCDDualLink( pVBInfo ) )
            tempax = tempax >> 1 ;

        /* if((pVBInfo->VBInfo&(SetCRT2ToLCD))||((pVBInfo->TVInfo&SetYPbPrMode525p)||(pVBInfo->TVInfo&SetYPbPrMode750p))) { */
        if ( pVBInfo->VBInfo & SetCRT2ToLCD )
        {
            if ( tempax > 800 )
                tempax -= 800 ;
        }
        else
        {
            if ( pVBInfo->VGAHDE > 800 )
            {
                if ( pVBInfo->VGAHDE == 1024 )
                    tempax = ( tempax * 25 / 32 ) - 1 ;
                else
                    tempax = ( tempax * 20 / 32 ) - 1 ;
            }
        }
        tempax -= 1 ;

/*
        if ( pVBInfo->VBInfo & ( SetCRT2ToTV | SetCRT2ToHiVisionTV ) )
        {
            if ( pVBInfo->VBType & VB_XGI301LV )
            {
                if ( !( pVBInfo->TVInfo & ( SetYPbPrMode525p | SetYPbPrMode750p | SetYPbPrMode1080i ) ) )
                {
                    if ( pVBInfo->VGAHDE > 800 )
                    {
                        if ( pVBInfo->VGAHDE == 1024 )
                            tempax = ( tempax * 25 / 32 ) - 1 ;
                        else
                            tempax = ( tempax * 20 / 32 ) - 1 ;
                    }
                }
            }
            else
            {
                if ( pVBInfo->VGAHDE > 800 )
                {
                    if ( pVBInfo->VGAHDE == 1024 )
                        tempax = ( tempax * 25 / 32 ) - 1 ;
                    else
                        tempax = ( tempax * 20 / 32 ) - 1 ;
                }
            }
        }
*/

        temp = ( tempax & 0xFF00 ) >> 8 ;
        temp = ( ( temp & 0x0003 ) << 4 ) ;
        XGINew_SetReg1( pVBInfo->Part4Port , 0x1E , temp ) ;
        temp = ( tempax & 0x00FF ) ;
        XGINew_SetReg1( pVBInfo->Part4Port , 0x1D , temp ) ;

        if ( pVBInfo->VBInfo & ( SetCRT2ToTV | SetCRT2ToHiVisionTV ) )
        {
            if ( pVBInfo->VGAHDE > 800 )
            {
                XGINew_SetRegOR( pVBInfo->Part4Port , 0x1E , 0x08 ) ;
            }
        }
        temp = 0x0036 ;

        if ( pVBInfo->VBInfo & SetCRT2ToTV )
        {
            if ( !( pVBInfo->TVInfo & ( NTSC1024x768 | SetYPbPrMode525p | SetYPbPrMode750p | SetYPbPrMode1080i ) ) )
            {
                temp |= 0x0001 ;
                if ( ( pVBInfo->VBInfo & SetInSlaveMode ) && ( !( pVBInfo->TVInfo & TVSimuMode ) ) )
                    temp &= ( ~0x0001 ) ;
            }
        }

        XGINew_SetRegANDOR( pVBInfo->Part4Port , 0x1F , 0x00C0 , temp ) ;
        tempbx = pVBInfo->HT ;
        if ( XGI_IsLCDDualLink( pVBInfo ) )
            tempbx = tempbx >> 1 ;
        tempbx = ( tempbx >> 1 ) - 2 ;
        temp = ( ( tempbx & 0x0700 ) >> 8 ) << 3 ;
        XGINew_SetRegANDOR( pVBInfo->Part4Port , 0x21 , 0x00C0 , temp ) ;
        temp = tempbx & 0x00FF ;
        XGINew_SetReg1( pVBInfo->Part4Port , 0x22 , temp ) ;
    }
    /* end 301b */

    if ( pVBInfo->ISXPDOS == 0 )
        XGI_SetCRT2VCLK( ModeNo , ModeIdIndex , RefreshRateTableIndex, pVBInfo ) ;
}


/* --------------------------------------------------------------------- */
/* Function : XGI_SetGroup5 */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGI_SetGroup5( USHORT ModeNo , USHORT ModeIdIndex , PVB_DEVICE_INFO pVBInfo)
{
    if ( pVBInfo->ModeType == ModeVGA )
    {
        if ( !( pVBInfo->VBInfo & ( SetInSlaveMode | LoadDACFlag | CRT2DisplayFlag ) ) )
        {
            XGINew_EnableCRT2(pVBInfo) ;
            /* LoadDAC2(pVBInfo->Part5Port,ModeNo,ModeIdIndex); */
        }
    }
}


/* --------------------------------------------------------------------- */
/* Function : XGI_GetLcdPtr */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void* XGI_GetLcdPtr( USHORT BX , USHORT ModeNo , USHORT ModeIdIndex , USHORT RefreshRateTableIndex, PVB_DEVICE_INFO pVBInfo )
{
    USHORT i ,
           tempdx ,
           tempbx ,
           tempal ,
           modeflag ,
           table ;

    XGI330_LCDDataTablStruct *tempdi = 0 ;


    tempbx = BX;

    if ( ModeNo <= 0x13 )
    {
        modeflag = pVBInfo->SModeIDTable[ ModeIdIndex ].St_ModeFlag ;
        tempal = pVBInfo->SModeIDTable[ ModeIdIndex ].St_CRT2CRTC ;
    }
    else
    {
        modeflag = pVBInfo->EModeIDTable[ ModeIdIndex ].Ext_ModeFlag ;
        tempal = pVBInfo->RefIndex[ RefreshRateTableIndex ].Ext_CRT2CRTC ;
    }

    tempal = tempal & 0x0f ;

    if ( tempbx <= 1 )		/* ExpLink */
    {
        if ( ModeNo <= 0x13 )
        {
            tempal = pVBInfo->SModeIDTable[ ModeIdIndex ].St_CRT2CRTC ; /* find no Ext_CRT2CRTC2 */
        }
        else
        {
            tempal= pVBInfo->RefIndex[ RefreshRateTableIndex ].Ext_CRT2CRTC ;
        }

        if ( pVBInfo->VBInfo & SetCRT2ToLCDA )
        {
            if ( ModeNo <= 0x13 )
                tempal = pVBInfo->SModeIDTable[ ModeIdIndex ].St_CRT2CRTC2 ;
            else
                tempal= pVBInfo->RefIndex[ RefreshRateTableIndex ].Ext_CRT2CRTC2 ;
        }

        if ( tempbx & 0x01 )
            tempal = ( tempal >> 4 ) ;

        tempal = ( tempal & 0x0f ) ;
    }

    /* mov di, word ptr cs:LCDDataList[bx] */
    /* tempdi=pVideoMemory[LCDDataList+tempbx*2]|(pVideoMemory[LCDDataList+tempbx*2+1]<<8); */

    switch( tempbx )
    {
        case 0:
            tempdi = XGI_EPLLCDCRT1Ptr_H ;
            break ;
        case 1:
            tempdi = XGI_EPLLCDCRT1Ptr_V ;
             break ;
        case 2:
            tempdi = XGI_EPLLCDDataPtr ;
            break ;
        case 3:
            tempdi = XGI_EPLLCDDesDataPtr ;
            break ;
        case 4:
            tempdi = XGI_LCDDataTable ;
            break ;
        case 5:
            tempdi = XGI_LCDDesDataTable ;
            break ;
        case 6:
            tempdi = XGI_EPLCHLCDRegPtr ;
            break ;
        case 7:
        case 8:
        case 9:
            tempdi = 0 ;
            break ;
        default:
        break ;
    }

    if ( tempdi == 0x00 )  /* OEMUtil */
        return 0 ;

    table = tempbx ;
    i = 0 ;

    while( tempdi[ i ].PANELID != 0xff )
    {
        tempdx = pVBInfo->LCDResInfo ;
        if ( tempbx & 0x0080 )     /* OEMUtil */
        {
            tempbx &= ( ~0x0080 ) ;
            tempdx = pVBInfo->LCDTypeInfo ;
        }

        if ( pVBInfo->LCDInfo & EnableScalingLCD )
        tempdx &= ( ~PanelResInfo ) ;

        if ( tempdi[ i ].PANELID == tempdx )
        {
            tempbx = tempdi[ i ].MASK ;
            tempdx = pVBInfo->LCDInfo ;

            if ( ModeNo <= 0x13 )         /* alan 09/10/2003 */
                tempdx |= SetLCDStdMode ;

            if ( modeflag & HalfDCLK )
                tempdx |= SetLCDLowResolution ;

            tempbx &= tempdx;
            if ( tempbx == tempdi[ i ].CAP )
                break ;
        }
        i++ ;
    }

    if ( table == 0 )
    {
        switch( tempdi[ i ].DATAPTR )
        {
            case 0:
                return &XGI_LVDSCRT11024x768_1_H[ tempal ] ;
                break ;
            case 1:
                return &XGI_LVDSCRT11024x768_2_H[ tempal ] ;
                break ;
            case 2:
                return &XGI_LVDSCRT11280x1024_1_H[ tempal ] ;
                break ;
            case 3:
                return &XGI_LVDSCRT11280x1024_2_H[ tempal ] ;
                break ;
            case 4:
                return &XGI_LVDSCRT11400x1050_1_H[ tempal ] ;
                break ;
            case 5:
                return &XGI_LVDSCRT11400x1050_2_H[ tempal ] ;
                break ;
            case 6:
                return &XGI_LVDSCRT11600x1200_1_H[ tempal ] ;
                break ;
            case 7:
                return &XGI_LVDSCRT11024x768_1_Hx75[ tempal ] ;
                break ;
            case 8:
                return &XGI_LVDSCRT11024x768_2_Hx75[ tempal ] ;
                break ;
            case 9:
                return &XGI_LVDSCRT11280x1024_1_Hx75[ tempal ] ;
                break ;
            case 10:
                return &XGI_LVDSCRT11280x1024_2_Hx75[ tempal ] ;
                break ;
            default:
                break ;
        }
    }
    else if ( table == 1 )
    {
        switch( tempdi[ i ].DATAPTR )
        {
            case 0:
                return &XGI_LVDSCRT11024x768_1_V[ tempal ] ;
                break ;
            case 1:
                return &XGI_LVDSCRT11024x768_2_V[ tempal ] ;
                break ;
            case 2:
                return &XGI_LVDSCRT11280x1024_1_V[ tempal ] ;
                break ;
            case 3:
                return &XGI_LVDSCRT11280x1024_2_V[ tempal ] ;
                break ;
            case 4:
                return &XGI_LVDSCRT11400x1050_1_V[ tempal ] ;
                break ;
            case 5:
                return &XGI_LVDSCRT11400x1050_2_V[ tempal ] ;
                break ;
            case 6:
                return &XGI_LVDSCRT11600x1200_1_V[ tempal ] ;
                break ;
            case 7:
                return &XGI_LVDSCRT11024x768_1_Vx75[ tempal ] ;
                break ;
            case 8:
                return &XGI_LVDSCRT11024x768_2_Vx75[ tempal ] ;
                break ;
            case 9:
                return &XGI_LVDSCRT11280x1024_1_Vx75[ tempal ] ;
                break ;
            case 10:
                return &XGI_LVDSCRT11280x1024_2_Vx75[ tempal ] ;
                break ;
            default:
                break ;
        }
    }
    else if ( table == 2 )
    {
        switch( tempdi[ i ].DATAPTR )
        {
            case 0:
                return &XGI_LVDS1024x768Data_1[ tempal ] ;
                break ;
            case 1:
                return &XGI_LVDS1024x768Data_2[ tempal ] ;
                break ;
            case 2:
                return &XGI_LVDS1280x1024Data_1[ tempal ] ;
                break ;
            case 3:
                return &XGI_LVDS1280x1024Data_2[ tempal ] ;
                break ;
            case 4:
                return &XGI_LVDS1400x1050Data_1[ tempal ] ;
                break ;
            case 5:
                return &XGI_LVDS1400x1050Data_2[ tempal ] ;
                break ;
            case 6:
                return &XGI_LVDS1600x1200Data_1[ tempal ] ;
                break ;
            case 7:
                return &XGI_LVDSNoScalingData[ tempal ] ;
                break ;
            case 8:
                return &XGI_LVDS1024x768Data_1x75[ tempal ] ;
                break ;
            case 9:
                return &XGI_LVDS1024x768Data_2x75[ tempal ] ;
                break ;
            case 10:
                return &XGI_LVDS1280x1024Data_1x75[ tempal ] ;
                break ;
            case 11:
                return &XGI_LVDS1280x1024Data_2x75[ tempal ] ;
                break ;
            case 12:
                return &XGI_LVDSNoScalingDatax75[ tempal ] ;
                break ;
            default:
                break ;
        }
    }
    else if ( table == 3 )
    {
        switch( tempdi[ i ].DATAPTR )
        {
            case 0:
                return &XGI_LVDS1024x768Des_1[ tempal ] ;
                break ;
            case 1:
                return &XGI_LVDS1024x768Des_3[ tempal ] ;
                break ;
            case 2:
                return &XGI_LVDS1024x768Des_2[ tempal ] ;
                break ;
            case 3:
                return &XGI_LVDS1280x1024Des_1[ tempal ] ;
                break ;
            case 4:
                return &XGI_LVDS1280x1024Des_2[ tempal ] ;
                break ;
            case 5:
                return &XGI_LVDS1400x1050Des_1[ tempal ] ;
                break ;
            case 6:
                return &XGI_LVDS1400x1050Des_2[ tempal ] ;
                break ;
            case 7:
                return &XGI_LVDS1600x1200Des_1[ tempal ] ;
                break ;
            case 8:
                return &XGI_LVDSNoScalingDesData[ tempal ] ;
                break ;
            case 9:
                return &XGI_LVDS1024x768Des_1x75[ tempal ] ;
                break ;
            case 10:
                return &XGI_LVDS1024x768Des_3x75[ tempal ] ;
                break ;
            case 11:
                return &XGI_LVDS1024x768Des_2x75[ tempal ] ;
                break;
            case 12:
                return &XGI_LVDS1280x1024Des_1x75[ tempal ] ;
                break ;
            case 13:
                return &XGI_LVDS1280x1024Des_2x75[ tempal ] ;
                break ;
            case 14:
                return &XGI_LVDSNoScalingDesDatax75[ tempal ] ;
                break ;
            default:
                break ;
        }
    }
    else if ( table == 4 )
    {
        switch( tempdi[ i ].DATAPTR )
        {
            case 0:
                return &XGI_ExtLCD1024x768Data[ tempal ] ;
                break ;
            case 1:
                return &XGI_StLCD1024x768Data[ tempal ] ;
                break ;
            case 2:
                return &XGI_CetLCD1024x768Data[ tempal ] ;
                break ;
            case 3:
                return &XGI_ExtLCD1280x1024Data[ tempal ] ;
                break ;
            case 4:
                return &XGI_StLCD1280x1024Data[ tempal ] ;
                break ;
            case 5:
                return &XGI_CetLCD1280x1024Data[ tempal ] ;
                break ;
            case 6:
                return &XGI_ExtLCD1400x1050Data[ tempal ] ;
                break ;
            case 7:
                return &XGI_StLCD1400x1050Data[ tempal ] ;
                break ;
            case 8:
                return &XGI_CetLCD1400x1050Data[ tempal ] ;
                break ;
            case 9:
                return &XGI_ExtLCD1600x1200Data[ tempal ] ;
                break  ;
            case 10:
                return &XGI_StLCD1600x1200Data[ tempal ] ;
                break ;
            case 11:
                return &XGI_NoScalingData[ tempal ] ;
                break ;
            case 12:
                return &XGI_ExtLCD1024x768x75Data[ tempal ] ;
                break ;
            case 13:
                return &XGI_ExtLCD1024x768x75Data[ tempal ] ;
                break ;
            case 14:
                return &XGI_CetLCD1024x768x75Data[ tempal ] ;
                break ;
            case 15:
                 return &XGI_ExtLCD1280x1024x75Data[ tempal ] ;
                break ;
            case 16:
                return &XGI_StLCD1280x1024x75Data[ tempal ] ;
                break;
            case 17:
                return &XGI_CetLCD1280x1024x75Data[ tempal ] ;
                break;
            case 18:
                return &XGI_NoScalingDatax75[ tempal ] ;
                break ;
            default:
                break ;
        }
    }
    else if ( table == 5 )
    {
        switch( tempdi[ i ].DATAPTR )
        {
            case 0:
                return &XGI_ExtLCDDes1024x768Data[ tempal ] ;
                break ;
            case 1:
                return &XGI_StLCDDes1024x768Data[ tempal ] ;
                break ;
            case 2:
                return &XGI_CetLCDDes1024x768Data[ tempal ] ;
                break ;
            case 3:
                if ( ( pVBInfo->VBType & VB_XGI301LV ) || ( pVBInfo->VBType & VB_XGI302LV ) )
                    return &XGI_ExtLCDDLDes1280x1024Data[ tempal ] ;
                else
                    return &XGI_ExtLCDDes1280x1024Data[ tempal ] ;
                break ;
            case 4:
                if ( ( pVBInfo->VBType & VB_XGI301LV ) || ( pVBInfo->VBType & VB_XGI302LV ) )
                    return &XGI_StLCDDLDes1280x1024Data[ tempal ] ;
                else
                    return &XGI_StLCDDes1280x1024Data[ tempal ] ;
                break ;
            case 5:
                if ( ( pVBInfo->VBType & VB_XGI301LV ) || ( pVBInfo->VBType & VB_XGI302LV ) )
                    return &XGI_CetLCDDLDes1280x1024Data[ tempal ] ;
                else
                    return &XGI_CetLCDDes1280x1024Data[ tempal ] ;
                break ;
            case 6:
                if ( ( pVBInfo->VBType & VB_XGI301LV ) || ( pVBInfo->VBType & VB_XGI302LV ) )
                    return &XGI_ExtLCDDLDes1400x1050Data[ tempal ] ;
                else
                    return &XGI_ExtLCDDes1400x1050Data[ tempal ] ;
                break ;
            case 7:
                if ( ( pVBInfo->VBType & VB_XGI301LV ) || ( pVBInfo->VBType & VB_XGI302LV ) )
                    return &XGI_StLCDDLDes1400x1050Data[ tempal ] ;
                else
                    return &XGI_StLCDDes1400x1050Data[ tempal ] ;
                break ;
            case 8:
                return &XGI_CetLCDDes1400x1050Data[ tempal ] ;
                break ;
            case 9:
                return &XGI_CetLCDDes1400x1050Data2[ tempal ] ;
                break ;
            case 10:
                if ( ( pVBInfo->VBType & VB_XGI301LV ) || ( pVBInfo->VBType & VB_XGI302LV ) )
                    return &XGI_ExtLCDDLDes1600x1200Data[ tempal ] ;
                else
                    return &XGI_ExtLCDDes1600x1200Data[ tempal ] ;
                break ;
            case 11:
                if ( ( pVBInfo->VBType & VB_XGI301LV ) || ( pVBInfo->VBType & VB_XGI302LV ) )
                    return &XGI_StLCDDLDes1600x1200Data[ tempal ] ;
                else
                    return &XGI_StLCDDes1600x1200Data[ tempal ] ;
                break ;
            case 12:
                return &XGI_NoScalingDesData[ tempal ] ;
                break;
            case 13:
                return &XGI_ExtLCDDes1024x768x75Data[ tempal ] ;
                break ;
            case 14:
                return &XGI_StLCDDes1024x768x75Data[ tempal ] ;
                break ;
            case 15:
                return &XGI_CetLCDDes1024x768x75Data[ tempal ] ;
                break ;
            case 16:
                if ( ( pVBInfo->VBType & VB_XGI301LV ) || ( pVBInfo->VBType & VB_XGI302LV ) )
                    return &XGI_ExtLCDDLDes1280x1024x75Data[ tempal ] ;
                else
                    return &XGI_ExtLCDDes1280x1024x75Data[ tempal ] ;
                break ;
            case 17:
                if ( ( pVBInfo->VBType & VB_XGI301LV ) || ( pVBInfo->VBType & VB_XGI302LV ) )
                    return &XGI_StLCDDLDes1280x1024x75Data[ tempal ] ;
                else
                    return &XGI_StLCDDes1280x1024x75Data[ tempal ] ;
                break ;
            case 18:
                if ( ( pVBInfo->VBType & VB_XGI301LV ) || ( pVBInfo->VBType & VB_XGI302LV ) )
                    return &XGI_CetLCDDLDes1280x1024x75Data[ tempal ] ;
                else
                    return &XGI_CetLCDDes1280x1024x75Data[ tempal ] ;
                break ;
            case 19:
                return &XGI_NoScalingDesDatax75[ tempal ] ;
                break ;
            default:
                break ;
        }
    }
    else if ( table == 6 )
    {
        switch( tempdi[ i ].DATAPTR )
        {
            case 0:
                return &XGI_CH7017LV1024x768[ tempal ] ;
                break ;
            case 1:
                return &XGI_CH7017LV1400x1050[ tempal ] ;
                break ;
            default:
                break ;
        }
    }
    return 0 ;
}


/* --------------------------------------------------------------------- */
/* Function : XGI_GetTVPtr */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void* XGI_GetTVPtr (USHORT BX,USHORT ModeNo,USHORT ModeIdIndex,USHORT RefreshRateTableIndex, PVB_DEVICE_INFO pVBInfo )
{
    USHORT i , tempdx , tempbx , tempal , modeflag , table ;
    XGI330_TVDataTablStruct *tempdi = 0 ;

    tempbx = BX ;

    if ( ModeNo <= 0x13 )
    {
        modeflag = pVBInfo->SModeIDTable[ ModeIdIndex ].St_ModeFlag ;
        tempal = pVBInfo->SModeIDTable[ ModeIdIndex ].St_CRT2CRTC ;
    }
    else
    {
        modeflag = pVBInfo->EModeIDTable[ ModeIdIndex ].Ext_ModeFlag ;
        tempal = pVBInfo->RefIndex[ RefreshRateTableIndex ].Ext_CRT2CRTC ;
    }

    tempal = tempal & 0x3f ;
    table = tempbx ;

    switch( tempbx )
    {
        case 0:
            tempdi = 0 ;	/*EPLCHTVCRT1Ptr_H;*/
            break ;
        case 1:
            tempdi = 0 ;	/*EPLCHTVCRT1Ptr_V;*/
            break ;
        case 2:
            tempdi = XGI_EPLCHTVDataPtr ;
            break ;
        case 3:
            tempdi = 0 ;
            break ;
        case 4:
            tempdi = XGI_TVDataTable ;
            break ;
        case 5:
            tempdi = 0 ;
            break ;
        case 6:
            tempdi = XGI_EPLCHTVRegPtr ;
            break ;
        default:
            break ;
    }

    if ( tempdi == 0x00 )  /* OEMUtil */
        return( 0 ) ;

    tempdx = pVBInfo->TVInfo ;

    if ( pVBInfo->VBInfo & SetInSlaveMode )
        tempdx = tempdx | SetTVLockMode ;

    if ( modeflag & HalfDCLK )
        tempdx = tempdx | SetTVLowResolution ;

    i = 0 ;

    while( tempdi[ i ].MASK != 0xffff )
    {
        if ( ( tempdx & tempdi[ i ].MASK ) == tempdi[ i ].CAP )
            break ;
        i++ ;
    }

    if ( table == 0x04 )
    {
        switch( tempdi[ i ].DATAPTR )
        {
            case 0:
                return &XGI_ExtPALData[ tempal ] ;
		break ;
            case 1:
                return &XGI_ExtNTSCData[ tempal ] ;
 		break ;
      	    case 2:
      	   	return &XGI_StPALData[ tempal ] ;
 		break ;
            case 3:
           	return &XGI_StNTSCData[ tempal ] ;
 		break ;
      	    case 4:
      	   	return &XGI_ExtHiTVData[ tempal ] ;
 		break ;
      	    case 5:
      	   	return &XGI_St2HiTVData[ tempal ] ;
 		break ;
      	    case 6:
      	   	return &XGI_ExtYPbPr525iData[ tempal ] ;
 		break ;
      	    case 7:
      	   	return &XGI_ExtYPbPr525pData[ tempal ] ;
 		break ;
      	    case 8:
      	   	return &XGI_ExtYPbPr750pData[ tempal ] ;
 		break ;
      	    case 9:
      	   	return &XGI_StYPbPr525iData[ tempal ] ;
 		break ;
      	    case 10:
      	   	return &XGI_StYPbPr525pData[ tempal ] ;
 		break ;
      	    case 11:
      	   	return &XGI_StYPbPr750pData[ tempal ] ;
             	break;
            case 12: 	/* avoid system hang */
      	   	return &XGI_ExtNTSCData[ tempal ] ;
             	break ;
            case 13:
      	   	return &XGI_St1HiTVData[ tempal ] ;
             	break ;
            default:
                break ;
        }
    }
    else if( table == 0x02 )
    {
        switch( tempdi[ i ].DATAPTR )
        {
            case 0:
                return &XGI_CHTVUNTSCData[ tempal ] ;
                break ;
            case 1:
      	        return &XGI_CHTVONTSCData[ tempal ] ;
                break ;
            case 2:
                return &XGI_CHTVUPALData[ tempal ] ;
                break ;
            case 3:
                return &XGI_CHTVOPALData[ tempal ] ;
                break ;
            default:
                break ;
        }
    }
    else if( table == 0x06 )
    {
        switch( tempdi[ i ].DATAPTR )
        {
            case 0:
                return &XGI_CHTVRegUNTSC[ tempal ] ;
                break ;
            case 1:
                return &XGI_CHTVRegONTSC[ tempal ] ;
                break ;
            case 2:
                return &XGI_CHTVRegUPAL[ tempal ] ;
                break ;
            case 3:
                return &XGI_CHTVRegOPAL[ tempal ] ;
                break ;
            default:
                break ;
        }
    }
    return( 0 ) ;
}


/* --------------------------------------------------------------------- */
/* Function : XGI_BacklightByDrv */
/* Input : */
/* Output : TRUE -> Skip backlight control */
/* Description : */
/* --------------------------------------------------------------------- */
BOOLEAN XGI_BacklightByDrv( PVB_DEVICE_INFO pVBInfo )
{
    UCHAR tempah ;

    tempah = ( UCHAR )XGINew_GetReg1( pVBInfo->P3d4 , 0x3A ) ;
    if ( tempah & BacklightControlBit )
        return TRUE ;
    else
        return FALSE ;
}


/* --------------------------------------------------------------------- */
/* Function : XGI_FirePWDDisable */
/* Input : */
/* Output : */
/* Description : Turn off VDD & Backlight : Fire disable procedure */
/* --------------------------------------------------------------------- */
/*
void XGI_FirePWDDisable( PVB_DEVICE_INFO pVBInfo )
{
    XGINew_SetRegANDOR( pVBInfo->Part1Port , 0x26 , 0x00 , 0xFC ) ;
}
*/

/* --------------------------------------------------------------------- */
/* Function : XGI_FirePWDEnable */
/* Input : */
/* Output : */
/* Description : Turn on VDD & Backlight : Fire enable procedure */
/* --------------------------------------------------------------------- */
void XGI_FirePWDEnable(PVB_DEVICE_INFO pVBInfo )
{
    XGINew_SetRegANDOR( pVBInfo->Part1Port , 0x26 , 0x03 , 0xFC ) ;
}


/* --------------------------------------------------------------------- */
/* Function : XGI_EnableGatingCRT */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGI_EnableGatingCRT(PXGI_HW_DEVICE_INFO HwDeviceExtension, PVB_DEVICE_INFO pVBInfo)
{
    XGINew_SetRegANDOR( pVBInfo->P3d4 , 0x63 , 0xBF , 0x40 ) ;
}


/* --------------------------------------------------------------------- */
/* Function : XGI_DisableGatingCRT */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGI_DisableGatingCRT(PXGI_HW_DEVICE_INFO HwDeviceExtension, PVB_DEVICE_INFO pVBInfo)
{

    XGINew_SetRegANDOR( pVBInfo->P3d4 , 0x63 , 0xBF , 0x00 ) ;
}


/* --------------------------------------------------------------------- */
/* Function : XGI_SetPanelDelay */
/* Input : */
/* Output : */
/* Description : */
/* I/P : bl : 1 ; T1 : the duration between CPL on and signal on */
/* : bl : 2 ; T2 : the duration signal on and Vdd on */
/* : bl : 3 ; T3 : the duration between CPL off and signal off */
/* : bl : 4 ; T4 : the duration signal off and Vdd off */
/* --------------------------------------------------------------------- */
void XGI_SetPanelDelay(USHORT tempbl, PVB_DEVICE_INFO pVBInfo)
{
    USHORT index ; 
           

    index = XGI_GetLCDCapPtr(pVBInfo) ;

    if ( tempbl == 1 )
        XGINew_LCD_Wait_Time( pVBInfo->LCDCapList[ index ].PSC_S1, pVBInfo ) ;

    if ( tempbl == 2 )
        XGINew_LCD_Wait_Time( pVBInfo->LCDCapList[ index ].PSC_S2, pVBInfo ) ;

    if ( tempbl == 3 )
        XGINew_LCD_Wait_Time( pVBInfo->LCDCapList[ index ].PSC_S3, pVBInfo ) ;

    if ( tempbl == 4 )
        XGINew_LCD_Wait_Time( pVBInfo->LCDCapList[ index ].PSC_S4, pVBInfo ) ;
}


/* --------------------------------------------------------------------- */
/* Function : XGI_SetPanelPower */
/* Input : */
/* Output : */
/* Description : */
/* I/O : ah = 0011b = 03h ; Backlight on, Power on */
/* = 0111b = 07h ; Backlight on, Power off */
/* = 1011b = 0Bh ; Backlight off, Power on */
/* = 1111b = 0Fh ; Backlight off, Power off */
/* --------------------------------------------------------------------- */
void XGI_SetPanelPower(USHORT tempah,USHORT tempbl, PVB_DEVICE_INFO pVBInfo)
{
    if ( pVBInfo->VBType & ( VB_XGI301LV | VB_XGI302LV | VB_XGI301C ) )
        XGINew_SetRegANDOR( pVBInfo->Part4Port , 0x26 , tempbl , tempah ) ;
    else
        XGINew_SetRegANDOR( pVBInfo->P3c4 , 0x11 , tempbl , tempah ) ;
}


/* --------------------------------------------------------------------- */
/* Function : XGI_IsLCDON */
/* Input : */
/* Output : FALSE : Skip PSC Control */
/* TRUE: Disable PSC */
/* Description : */
/* --------------------------------------------------------------------- */
BOOLEAN XGI_IsLCDON(PVB_DEVICE_INFO pVBInfo)
{
    USHORT tempax ;

    tempax = pVBInfo->VBInfo ;
    if ( tempax & SetCRT2ToDualEdge )
        return FALSE ;
    else if ( tempax & ( DisableCRT2Display | SwitchToCRT2 | SetSimuScanMode ) )
        return TRUE ;

    return FALSE ;
}


/* --------------------------------------------------------------------- */
/* Function : XGI_EnablePWD */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGI_EnablePWD(  PVB_DEVICE_INFO pVBInfo )
{
    USHORT index ,
           temp ;

    index = XGI_GetLCDCapPtr(pVBInfo) ;
    temp = pVBInfo->LCDCapList[ index ].PWD_2B ;
    XGINew_SetReg1( pVBInfo->Part4Port , 0x2B , temp ) ;
    XGINew_SetReg1( pVBInfo->Part4Port , 0x2C , pVBInfo->LCDCapList[ index ].PWD_2C ) ;
    XGINew_SetReg1( pVBInfo->Part4Port , 0x2D , pVBInfo->LCDCapList[ index ].PWD_2D ) ;
    XGINew_SetReg1( pVBInfo->Part4Port , 0x2E , pVBInfo->LCDCapList[ index ].PWD_2E ) ;
    XGINew_SetReg1( pVBInfo->Part4Port , 0x2F , pVBInfo->LCDCapList[ index ].PWD_2F ) ;
    XGINew_SetRegOR( pVBInfo->Part4Port , 0x27 , 0x80 ) ; 	/* enable PWD */
}


/* --------------------------------------------------------------------- */
/* Function : XGI_DisablePWD */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGI_DisablePWD( PVB_DEVICE_INFO pVBInfo )
{
    XGINew_SetRegAND( pVBInfo->Part4Port , 0x27 , 0x7F ) ;	/* disable PWD */
}


/* --------------------------------------------------------------------- */
/* Function : XGI_DisableChISLCD */
/* Input : */
/* Output : FALSE -> Not LCD Mode */
/* Description : */
/* --------------------------------------------------------------------- */
BOOLEAN XGI_DisableChISLCD(PVB_DEVICE_INFO pVBInfo)
{
    USHORT tempbx ,
           tempah ;

    tempbx = pVBInfo->SetFlag & ( DisableChA | DisableChB ) ;
    tempah = ~( ( USHORT )XGINew_GetReg1( pVBInfo->Part1Port  , 0x2E ) ) ;

    if ( tempbx & ( EnableChA | DisableChA ) )
    {
        if ( !( tempah & 0x08 ) )		/* Chk LCDA Mode */
            return FALSE ;
    }

    if ( !( tempbx & ( EnableChB | DisableChB ) ) )
        return FALSE ;

    if ( tempah & 0x01 )       /* Chk LCDB Mode */
        return TRUE ;

    return FALSE ;
}


/* --------------------------------------------------------------------- */
/* Function : XGI_EnableChISLCD */
/* Input : */
/* Output : 0 -> Not LCD mode */
/* Description : */
/* --------------------------------------------------------------------- */
BOOLEAN XGI_EnableChISLCD(PVB_DEVICE_INFO pVBInfo)
{
    USHORT tempbx ,
           tempah ;


    tempbx = pVBInfo->SetFlag & ( EnableChA | EnableChB ) ;
    tempah = ~( ( USHORT )XGINew_GetReg1( pVBInfo->Part1Port , 0x2E ) ) ;

    if ( tempbx & ( EnableChA | DisableChA ) )
    {
        if ( !( tempah & 0x08 ) )		/* Chk LCDA Mode */
            return FALSE ;
    }

    if ( !( tempbx & ( EnableChB | DisableChB ) ) )
        return FALSE ;

    if ( tempah & 0x01 )       /* Chk LCDB Mode */
        return TRUE ;

    return FALSE ;
}


/* --------------------------------------------------------------------- */
/* Function : XGI_GetLCDCapPtr */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
USHORT XGI_GetLCDCapPtr(  PVB_DEVICE_INFO pVBInfo )
{
    UCHAR tempal ,
          tempah ,
          tempbl ,
          i ;

    tempah = XGINew_GetReg1( pVBInfo->P3d4 , 0x36 ) ;
    tempal = tempah & 0x0F ;
    tempah = tempah & 0xF0 ;
    i = 0 ;
    tempbl =  pVBInfo->LCDCapList[ i ].LCD_ID ;

    while( tempbl != 0xFF )
    {
        if ( tempbl & 0x80 )   /* OEMUtil */
        {
            tempal = tempah ;
            tempbl = tempbl & ~( 0x80 ) ;
        }

        if ( tempal == tempbl )
            break ;

        i++ ;

        tempbl = pVBInfo->LCDCapList[ i ].LCD_ID ;
    }

    return i ;
}


/* --------------------------------------------------------------------- */
/* Function : XGI_GetLCDCapPtr1 */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
USHORT XGI_GetLCDCapPtr1( PVB_DEVICE_INFO pVBInfo )
{
    USHORT tempah ,
           tempal ,
           tempbl ,
           i ;

    tempal = pVBInfo->LCDResInfo ;
    tempah = pVBInfo->LCDTypeInfo ;

    i = 0 ;
    tempbl =  pVBInfo->LCDCapList[ i ].LCD_ID;

    while( tempbl != 0xFF )
    {
        if ( ( tempbl & 0x80 ) && ( tempbl != 0x80 ) )
        {
            tempal = tempah ;
            tempbl &= ~0x80 ;
        }

        if ( tempal == tempbl )
            break ;

        i++ ;
        tempbl = pVBInfo->LCDCapList[ i ].LCD_ID ;
    }

    if ( tempbl == 0xFF )
    {
        pVBInfo->LCDResInfo = Panel1024x768 ;
        pVBInfo->LCDTypeInfo = 0 ;
        i = 0 ;
    }

    return i ;
}


/* --------------------------------------------------------------------- */
/* Function : XGI_GetLCDSync */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGI_GetLCDSync( USHORT* HSyncWidth , USHORT* VSyncWidth, PVB_DEVICE_INFO pVBInfo )
{
    USHORT Index ;

    Index = XGI_GetLCDCapPtr(pVBInfo) ;
    *HSyncWidth = pVBInfo->LCDCapList[ Index ].LCD_HSyncWidth ;
    *VSyncWidth = pVBInfo->LCDCapList[ Index ].LCD_VSyncWidth ;

    return ;
}



/* --------------------------------------------------------------------- */
/* Function : XGI_EnableBridge */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGI_EnableBridge( PXGI_HW_DEVICE_INFO HwDeviceExtension , PVB_DEVICE_INFO pVBInfo)
{
    USHORT tempbl ,
           tempah ;


    if ( pVBInfo->SetFlag == Win9xDOSMode )
    {
        if ( pVBInfo->VBType & ( VB_XGI301B | VB_XGI302B | VB_XGI301LV | VB_XGI302LV | VB_XGI301C ) )
        {
            XGI_DisplayOn( pVBInfo) ;
            return ;
        }
        else  /* LVDS or CH7017 */
            return ;
    }


    if ( HwDeviceExtension->jChipType < XG40 )
    {
        if ( !XGI_DisableChISLCD(pVBInfo) )
        {
            if ( ( XGI_EnableChISLCD(pVBInfo) ) || ( pVBInfo->VBInfo & ( SetCRT2ToLCD | SetCRT2ToLCDA ) ) )
            {
                if ( pVBInfo->LCDInfo & SetPWDEnable )
                {
                    XGI_EnablePWD( pVBInfo);
                }
                else
                {
                    pVBInfo->LCDInfo &= ( ~SetPWDEnable ) ;
                    if ( pVBInfo->VBType & ( VB_XGI301LV | VB_XGI302LV | VB_XGI301C ) )
                    {
                        tempbl = 0xFD ;
                        tempah = 0x02 ;
                    }
                    else
                    {
                        tempbl = 0xFB ;
                        tempah = 0x00 ;
                    }

                    XGI_SetPanelPower( tempah , tempbl, pVBInfo ) ;
                    XGI_SetPanelDelay( 1,pVBInfo ) ;
                }
            }
        }
    }	/* Not 340 */

 

    if ( pVBInfo->VBType & ( VB_XGI301B | VB_XGI302B | VB_XGI301LV | VB_XGI302LV | VB_XGI301C ) )
    {
        if ( !( pVBInfo->SetFlag & DisableChA ) )
        {
            if ( pVBInfo->SetFlag & EnableChA )
            {
                XGINew_SetReg1( pVBInfo->Part1Port , 0x1E , 0x20 ) ;  /* Power on */
            }
            else
            {
                if ( pVBInfo->VBInfo & SetCRT2ToDualEdge ) /* SetCRT2ToLCDA ) */
                {
                    XGINew_SetReg1(pVBInfo->Part1Port,0x1E,0x20);  /* Power on */
                }
            }
        }

        if ( !( pVBInfo->SetFlag & DisableChB ) )
        {
            if ( ( pVBInfo->SetFlag & EnableChB ) || ( pVBInfo->VBInfo & ( SetCRT2ToLCD | SetCRT2ToTV | SetCRT2ToRAMDAC ) ) )
            {
                tempah = ( UCHAR )XGINew_GetReg1( pVBInfo->P3c4 , 0x32 ) ;
                tempah &= 0xDF;
                if ( pVBInfo->VBInfo & SetInSlaveMode )
                {
                    if ( !( pVBInfo->VBInfo & SetCRT2ToRAMDAC ) )
                        tempah |= 0x20 ;
                }
                XGINew_SetReg1( pVBInfo->P3c4 , 0x32 , tempah ) ;
                XGINew_SetRegOR( pVBInfo->P3c4 , 0x1E , 0x20 ) ;


                tempah = ( UCHAR )XGINew_GetReg1( pVBInfo->Part1Port , 0x2E ) ;

                if ( !( tempah & 0x80 ) )
                    XGINew_SetRegOR( pVBInfo->Part1Port , 0x2E , 0x80 ) ;   	/* BVBDOENABLE = 1 */

                XGINew_SetRegAND( pVBInfo->Part1Port , 0x00 , 0x7F ) ;     	/* BScreenOFF = 0 */
            }
        }

        if ( ( pVBInfo->SetFlag & ( EnableChA | EnableChB ) ) || ( !( pVBInfo->VBInfo & DisableCRT2Display ) ) )
        {
            XGINew_SetRegANDOR( pVBInfo->Part2Port , 0x00 , ~0xE0 , 0x20 ) ;   /* shampoo 0129 */
            if ( pVBInfo->VBType & ( VB_XGI302LV | VB_XGI301C ) )
            {
                if ( !XGI_DisableChISLCD(pVBInfo) )
                {
                    if ( XGI_EnableChISLCD( pVBInfo) || ( pVBInfo->VBInfo & ( SetCRT2ToLCD | SetCRT2ToLCDA ) ) )
                        XGINew_SetRegAND( pVBInfo->Part4Port ,0x2A , 0x7F ) ;  	/* LVDS PLL power on */
                }
                XGINew_SetRegAND( pVBInfo->Part4Port , 0x30 , 0x7F ) ;		/* LVDS Driver power on */
            }
        }

        tempah = 0x00 ;

        if ( !( pVBInfo->VBInfo & DisableCRT2Display ) )
        {
            tempah = 0xc0 ;

            if ( !( pVBInfo->VBInfo & SetSimuScanMode ) )
            {
                if ( pVBInfo->VBInfo & SetCRT2ToLCDA )
                {
                    if ( pVBInfo->VBInfo & SetCRT2ToDualEdge )
                    {
                        tempah = tempah & 0x40;
                        if ( pVBInfo->VBInfo & SetCRT2ToLCDA )
                            tempah = tempah ^ 0xC0 ;

                        if ( pVBInfo->SetFlag & DisableChB )
                            tempah &= 0xBF ;

                        if ( pVBInfo->SetFlag & DisableChA )
                            tempah &= 0x7F ;

                        if ( pVBInfo->SetFlag & EnableChB )
                            tempah |= 0x40 ;

                        if ( pVBInfo->SetFlag & EnableChA )
                            tempah |= 0x80 ;
                    }
                }
            }
        }

        XGINew_SetRegOR( pVBInfo->Part4Port , 0x1F , tempah ) ;          /* EnablePart4_1F */

        if ( pVBInfo->SetFlag & Win9xDOSMode )
        {
            XGI_DisplayOn( pVBInfo) ;
            return ;
        }

        if ( !( pVBInfo->SetFlag & DisableChA ) )
        {
            XGI_VBLongWait( pVBInfo) ;
            if ( !( pVBInfo->SetFlag & GatingCRT ) )
            {
                XGI_DisableGatingCRT( HwDeviceExtension, pVBInfo ) ;
                XGI_DisplayOn( pVBInfo) ;
                XGI_VBLongWait( pVBInfo) ;
            }
        }
    }	/* 301 */
    else 	/* LVDS */
    {
        if ( pVBInfo->VBInfo & ( SetCRT2ToTV | SetCRT2ToLCD | SetCRT2ToLCDA ) )
            XGINew_SetRegOR( pVBInfo->Part1Port , 0x1E , 0x20 ) ;      	/* enable CRT2 */

  

        tempah = ( UCHAR )XGINew_GetReg1( pVBInfo->Part1Port , 0x2E ) ;
        if ( !( tempah & 0x80 ) )
            XGINew_SetRegOR( pVBInfo->Part1Port , 0x2E , 0x80 ) ;	/* BVBDOENABLE = 1 */

        XGINew_SetRegAND(pVBInfo->Part1Port,0x00,0x7F);
        XGI_DisplayOn( pVBInfo);
    } /* End of VB */


    if ( HwDeviceExtension->jChipType < XG40 )
    {
        if ( !XGI_EnableChISLCD(pVBInfo) )
        {
            if ( pVBInfo->VBInfo & ( SetCRT2ToLCD | SetCRT2ToLCDA ) )
            {
                if ( XGI_BacklightByDrv(pVBInfo) )
                    return ;
            }
            else
                return ;
        }

        if ( pVBInfo->LCDInfo & SetPWDEnable )
        {
            XGI_FirePWDEnable(pVBInfo) ;
            return ;
        }

        XGI_SetPanelDelay( 2,pVBInfo ) ;

        if ( pVBInfo->VBType & ( VB_XGI301LV | VB_XGI302LV | VB_XGI301C ) )
        {
            tempah = 0x01 ;
            tempbl = 0xFE ;		/* turn on backlght */
        }
        else
        {
            tempbl = 0xF7 ;
            tempah = 0x00 ;
        }
        XGI_SetPanelPower( tempah , tempbl , pVBInfo) ;
    }
}


/* --------------------------------------------------------------------- */
/* Function : XGI_DisableBridge */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGI_DisableBridge(PXGI_HW_DEVICE_INFO HwDeviceExtension, PVB_DEVICE_INFO pVBInfo)
{
    USHORT tempax ,
           tempbx ,
           tempah = 0 ,
           tempbl = 0 ;

    if ( pVBInfo->SetFlag == Win9xDOSMode )
        return ;


    if ( HwDeviceExtension->jChipType < XG40 )
    {
        if ( ( !( pVBInfo->VBInfo & ( SetCRT2ToLCD | SetCRT2ToLCDA ) ) ) || ( XGI_DisableChISLCD(pVBInfo) ) )
        {
            if ( !XGI_IsLCDON(pVBInfo) )
            {
                if ( pVBInfo->LCDInfo & SetPWDEnable )
                    XGI_EnablePWD( pVBInfo) ;
                else
                {
                    pVBInfo->LCDInfo &= ~SetPWDEnable ;
                    XGI_DisablePWD(pVBInfo) ;
                    if ( pVBInfo->VBType & ( VB_XGI301LV | VB_XGI302LV | VB_XGI301C ) )
                    {
                        tempbx = 0xFE ;  /* not 01h */
                        tempax = 0 ;
                    }
                    else
                    {
                        tempbx = 0xF7 ;  /* not 08h */
                        tempax = 0x08 ;
                    }
                    XGI_SetPanelPower( tempax , tempbx , pVBInfo) ;
                    XGI_SetPanelDelay( 3,pVBInfo ) ;
                }
            }	/* end if(!XGI_IsLCDON(pVBInfo)) */
        }
    }

/*  if ( CH7017 )
    {
        if ( !( pVBInfo->VBInfo & ( SetCRT2ToLCD | SetCRT2toLCDA ) ) || ( XGI_DisableChISLCD(pVBInfo) ) )
        {
            if ( !XGI_IsLCDON(pVBInfo) )
            {
                if ( DISCHARGE )
                {
                    tempbx = XGINew_GetCH7005( 0x61 ) ;
                    if ( tempbx < 0x01 )   //first time we power up
                        XGINew_SetCH7005( 0x0066 ) ;	//and disable power sequence
                    else
                        XGINew_SetCH7005( 0x5f66 ) ; //leave VDD on - disable power
                }
            }
        }
    }        */

    if ( pVBInfo->VBType & ( VB_XGI301B | VB_XGI302B| VB_XGI301LV | VB_XGI302LV | VB_XGI301C ) )
    {
        tempah = 0x3F ;
        if ( !( pVBInfo->VBInfo & ( DisableCRT2Display | SetSimuScanMode ) ) )
        {
            if ( pVBInfo->VBInfo & SetCRT2ToLCDA )
            {
                if ( pVBInfo->VBInfo & SetCRT2ToDualEdge )
                {
                    tempah = 0x7F;			/* Disable Channel A */
                    if ( !( pVBInfo->VBInfo & SetCRT2ToLCDA ) )
                        tempah = 0xBF ;		/* Disable Channel B */

                    if ( pVBInfo->SetFlag & DisableChB )
                        tempah &= 0xBF ;		/* force to disable Cahnnel */

                    if ( pVBInfo->SetFlag & DisableChA )
                        tempah &= 0x7F ;		/* Force to disable Channel B */
                }
            }
        }

        XGINew_SetRegAND( pVBInfo->Part4Port , 0x1F , tempah ) ;   /* disable part4_1f */

        if ( pVBInfo->VBType & ( VB_XGI302LV | VB_XGI301C ) )
        {
            if ( ( ( pVBInfo->VBInfo & ( SetCRT2ToLCD | SetCRT2ToLCDA ) ) ) || ( XGI_DisableChISLCD(pVBInfo) ) || ( XGI_IsLCDON(pVBInfo) ) )
                XGINew_SetRegOR( pVBInfo->Part4Port , 0x30 , 0x80 ) ;  /* LVDS Driver power down */
        }

        if ( ( pVBInfo->SetFlag & DisableChA ) || ( pVBInfo->VBInfo & ( DisableCRT2Display | SetCRT2ToLCDA | SetSimuScanMode ) ) )
        {
            if ( pVBInfo->SetFlag & GatingCRT )
                XGI_EnableGatingCRT( HwDeviceExtension, pVBInfo ) ;
            XGI_DisplayOff( pVBInfo) ;
        }

        if ( pVBInfo->VBInfo & SetCRT2ToLCDA )
        {
            if ( ( pVBInfo->SetFlag & DisableChA ) || ( pVBInfo->VBInfo & SetCRT2ToLCDA ) )
                XGINew_SetRegAND( pVBInfo->Part1Port , 0x1e , 0xdf ) ; 	/* Power down */
        }

        XGINew_SetRegAND( pVBInfo->P3c4 , 0x32 , 0xdf ) ; 		/* disable TV as primary VGA swap */

        if ( ( pVBInfo->VBInfo & ( SetSimuScanMode | SetCRT2ToDualEdge  ) ) )
            XGINew_SetRegAND(pVBInfo->Part2Port,0x00,0xdf);

        if ( ( pVBInfo->SetFlag & DisableChB ) || ( pVBInfo->VBInfo & ( DisableCRT2Display | SetSimuScanMode ) )
        || ( ( !( pVBInfo->VBInfo & SetCRT2ToLCDA ) ) && ( pVBInfo->VBInfo & ( SetCRT2ToRAMDAC | SetCRT2ToLCD | SetCRT2ToTV ) ) ) )
            XGINew_SetRegOR( pVBInfo->Part1Port , 0x00 , 0x80 ) ; 	/* BScreenOff=1 */

        if ( ( pVBInfo->SetFlag & DisableChB ) || ( pVBInfo->VBInfo & ( DisableCRT2Display | SetSimuScanMode ) )
        || ( !( pVBInfo->VBInfo & SetCRT2ToLCDA ) ) || ( pVBInfo->VBInfo & ( SetCRT2ToRAMDAC | SetCRT2ToLCD | SetCRT2ToTV ) ) )
        {
            tempah= XGINew_GetReg1( pVBInfo->Part1Port , 0x00 ) ;   	/* save Part1 index 0 */
            XGINew_SetRegOR( pVBInfo->Part1Port , 0x00 , 0x10 ) ; 	/* BTDAC = 1, avoid VB reset */
            XGINew_SetRegAND( pVBInfo->Part1Port , 0x1E , 0xDF ) ;	/* disable CRT2 */
            XGINew_SetReg1( pVBInfo->Part1Port , 0x00 , tempah ) ;	/* restore Part1 index 0 */
        }
    }
    else /* {301} */
    {
        if ( pVBInfo->VBInfo & ( SetCRT2ToLCD | SetCRT2ToTV ) )
        {
            XGINew_SetRegOR( pVBInfo->Part1Port , 0x00 , 0x80 ) ; 	/* BScreenOff=1 */
            XGINew_SetRegAND( pVBInfo->Part1Port , 0x1E , 0xDF ) ;	/* Disable CRT2 */
            XGINew_SetRegAND( pVBInfo->P3c4 , 0x32 , 0xDF ) ;	/* Disable TV asPrimary VGA swap */
        }

        if ( pVBInfo->VBInfo & ( DisableCRT2Display | SetCRT2ToLCDA | SetSimuScanMode ) )
            XGI_DisplayOff( pVBInfo) ;
    }




    if ( HwDeviceExtension->jChipType < XG40 )
    {
        if ( !( pVBInfo->VBInfo & ( SetCRT2ToLCD | SetCRT2ToLCDA ) ) || ( XGI_DisableChISLCD(pVBInfo) ) || ( XGI_IsLCDON(pVBInfo) ) )
        {
            if ( pVBInfo->LCDInfo & SetPWDEnable )
            {
                if ( pVBInfo->LCDInfo & SetPWDEnable )
                    XGI_BacklightByDrv(pVBInfo) ;
                else
                {
                    XGI_SetPanelDelay( 4 ,pVBInfo) ;
                    if ( pVBInfo->VBType & VB_XGI301LV )
                    {
                        tempbl = 0xFD ;
                        tempah = 0x00 ;
                    }
                    else
                    {
                        tempbl = 0xFB ;
                        tempah = 0x04 ;
                    }
                }
            }
            XGI_SetPanelPower( tempah , tempbl , pVBInfo) ;
        }
    }
}


/* --------------------------------------------------------------------- */
/* Function : XGI_GetTVPtrIndex */
/* Input : */
/* Output : */
/* Description : bx 0 : ExtNTSC */
/* 1 : StNTSC */
/* 2 : ExtPAL */
/* 3 : StPAL */
/* 4 : ExtHiTV */
/* 5 : StHiTV */
/* 6 : Ext525i */
/* 7 : St525i */
/* 8 : Ext525p */
/* 9 : St525p */
/* A : Ext750p */
/* B : St750p */
/* --------------------------------------------------------------------- */
USHORT XGI_GetTVPtrIndex(  PVB_DEVICE_INFO pVBInfo )
{
    USHORT tempbx = 0 ;

    if ( pVBInfo->TVInfo & SetPALTV )
        tempbx = 2 ;
    if ( pVBInfo->TVInfo & SetYPbPrMode1080i )
        tempbx = 4 ;
    if ( pVBInfo->TVInfo & SetYPbPrMode525i )
        tempbx = 6 ;
    if ( pVBInfo->TVInfo & SetYPbPrMode525p )
        tempbx = 8 ;
    if ( pVBInfo->TVInfo & SetYPbPrMode750p )
        tempbx = 10 ;
    if ( pVBInfo->TVInfo & TVSimuMode )
        tempbx++ ;

    return tempbx ;
}


/* --------------------------------------------------------------------- */
/* Function : XGI_OEM310Setting */
/* Input : */
/* Output : */
/* Description : Customized Param. for 301 */
/* --------------------------------------------------------------------- */
void XGI_OEM310Setting( USHORT ModeNo , USHORT ModeIdIndex, PVB_DEVICE_INFO pVBInfo)
{
    if ( pVBInfo->SetFlag & Win9xDOSMode )
        return ;

    /* GetPart1IO(); */
    XGI_SetDelayComp(pVBInfo) ;

    if ( pVBInfo->VBInfo & ( SetCRT2ToLCD | SetCRT2ToLCDA ) )
        XGI_SetLCDCap(pVBInfo) ;

    if ( pVBInfo->VBInfo & SetCRT2ToTV )
    {
        /* GetPart2IO() */
        XGI_SetPhaseIncr(pVBInfo) ;
        XGI_SetYFilter( ModeNo , ModeIdIndex,pVBInfo ) ;
        XGI_SetAntiFlicker( ModeNo , ModeIdIndex,pVBInfo ) ;

        if ( pVBInfo->VBType&VB_XGI301)
            XGI_SetEdgeEnhance( ModeNo , ModeIdIndex ,pVBInfo) ;
    }
}


/* --------------------------------------------------------------------- */
/* Function : XGI_SetDelayComp */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGI_SetDelayComp( PVB_DEVICE_INFO pVBInfo )
{
    USHORT index ;

    UCHAR  tempah ,
           tempbl ,
           tempbh ;
           

    if ( pVBInfo->VBType & ( VB_XGI301B | VB_XGI302B | VB_XGI301LV | VB_XGI302LV | VB_XGI301C ) )
    {
        if ( pVBInfo->VBInfo & ( SetCRT2ToLCD | SetCRT2ToLCDA | SetCRT2ToTV | SetCRT2ToRAMDAC ) )
        {
            tempbl = 0;
            tempbh = 0;

            index = XGI_GetTVPtrIndex(pVBInfo ) ;           /* Get TV Delay */
            tempbl = pVBInfo->XGI_TVDelayList[ index ] ;

            if ( pVBInfo->VBType & ( VB_XGI301B | VB_XGI302B | VB_XGI301LV | VB_XGI302LV | VB_XGI301C ) )
                tempbl = pVBInfo->XGI_TVDelayList2[ index ] ;

            if ( pVBInfo->VBInfo & SetCRT2ToDualEdge )
                tempbl = tempbl >> 4 ;
/*
            if ( pVBInfo->VBInfo & SetCRT2ToRAMDAC )
                tempbl = CRT2Delay1 ;			// Get CRT2 Delay

            if ( pVBInfo->VBType & ( VB_XGI301B | VB_XGI302B | VB_XGI301LV | VB_XGI302LV | VB_XGI301C ) )
                tempbl = CRT2Delay2 ;
*/
            if ( pVBInfo->VBInfo & ( SetCRT2ToLCD | SetCRT2ToLCDA ) )
            {
                index = XGI_GetLCDCapPtr(pVBInfo) ; 		/* Get LCD Delay */
                tempbh=pVBInfo->LCDCapList[ index ].LCD_DelayCompensation ;

                if ( !( pVBInfo->VBInfo & SetCRT2ToLCDA ) )
                     tempbl = tempbh ;
            }

            tempbl  &= 0x0F ;
            tempbh  &= 0xF0 ;
            tempah = XGINew_GetReg1( pVBInfo->Part1Port , 0x2D ) ;

            if ( pVBInfo->VBInfo & ( SetCRT2ToRAMDAC | SetCRT2ToLCD | SetCRT2ToTV ) )  /* Channel B */
            {
                tempah &= 0xF0 ;
                tempah |= tempbl ;
            }

            if ( pVBInfo->VBInfo & SetCRT2ToLCDA )		/* Channel A */
            {
                tempah &= 0x0F ;
                tempah |= tempbh ;
            }
            XGINew_SetReg1(pVBInfo->Part1Port,0x2D,tempah);
        }
    }
    else if ( pVBInfo->IF_DEF_LVDS == 1 )
    {
        tempbl = 0;
        tempbh = 0;
        if ( pVBInfo->VBInfo & SetCRT2ToLCD )
        {
            tempah = pVBInfo->LCDCapList[ XGI_GetLCDCapPtr(pVBInfo) ].LCD_DelayCompensation ;		/* / Get LCD Delay */
            tempah &= 0x0f ;
            tempah = tempah << 4 ;
            XGINew_SetRegANDOR( pVBInfo->Part1Port , 0x2D , 0x0f , tempah ) ;
        }
    }
}


/* --------------------------------------------------------------------- */
/* Function : XGI_SetLCDCap */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGI_SetLCDCap( PVB_DEVICE_INFO pVBInfo )
{
    USHORT tempcx ;

    tempcx = pVBInfo->LCDCapList[ XGI_GetLCDCapPtr(pVBInfo) ].LCD_Capability ;

    if ( pVBInfo->VBType & ( VB_XGI301B | VB_XGI302B | VB_XGI301LV | VB_XGI302LV | VB_XGI301C ) )
    {
        if ( pVBInfo->VBType & ( VB_XGI301LV | VB_XGI302LV | VB_XGI301C ) )
        {        				/* 301LV/302LV only */
            /* Set 301LV Capability */
            XGINew_SetReg1( pVBInfo->Part4Port , 0x24 , ( UCHAR )( tempcx & 0x1F ) ) ;
	}
        /* VB Driving */
        XGINew_SetRegANDOR( pVBInfo->Part4Port , 0x0D , ~( ( EnableVBCLKDRVLOW | EnablePLLSPLOW ) >> 8 ) , ( USHORT )( ( tempcx & ( EnableVBCLKDRVLOW | EnablePLLSPLOW ) ) >> 8 ) ) ;
    }

    if ( pVBInfo->VBType & ( VB_XGI301B | VB_XGI302B | VB_XGI301LV | VB_XGI302LV | VB_XGI301C ) )
    {
        if ( pVBInfo->VBInfo & SetCRT2ToLCD )
            XGI_SetLCDCap_B( tempcx,pVBInfo ) ;
        else if ( pVBInfo->VBInfo & SetCRT2ToLCDA )
            XGI_SetLCDCap_A( tempcx,pVBInfo ) ;

        if ( pVBInfo->VBType & ( VB_XGI302LV | VB_XGI301C ) )
        {
            if ( tempcx & EnableSpectrum )
                SetSpectrum( pVBInfo) ;
        }
    }
    else      /* LVDS,CH7017 */
        XGI_SetLCDCap_A( tempcx, pVBInfo ) ;
}


/* --------------------------------------------------------------------- */
/* Function : XGI_SetLCDCap_A */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGI_SetLCDCap_A(USHORT tempcx,PVB_DEVICE_INFO pVBInfo)
{
    USHORT temp ;

    temp = XGINew_GetReg1( pVBInfo->P3d4 , 0x37 ) ;

    if ( temp & LCDRGB18Bit )
    {
        XGINew_SetRegANDOR( pVBInfo->Part1Port , 0x19 , 0x0F , ( USHORT )( 0x20 | ( tempcx & 0x00C0 ) ) ) ; /* Enable Dither */
        XGINew_SetRegANDOR( pVBInfo->Part1Port , 0x1A , 0x7F , 0x80 ) ;
    }
    else
    {
        XGINew_SetRegANDOR( pVBInfo->Part1Port , 0x19 , 0x0F , ( USHORT )( 0x30 | ( tempcx & 0x00C0 ) ) ) ;
        XGINew_SetRegANDOR( pVBInfo->Part1Port , 0x1A , 0x7F , 0x00 ) ;
    }

/*
    if ( tempcx & EnableLCD24bpp )	// 24bits
    {
        XGINew_SetRegANDOR(pVBInfo->Part1Port,0x19, 0x0F,(USHORT)(0x30|(tempcx&0x00C0)) );
        XGINew_SetRegANDOR(pVBInfo->Part1Port,0x1A,0x7F,0x00);
    }
    else
    {
        XGINew_SetRegANDOR(pVBInfo->Part1Port,0x19, 0x0F,(USHORT)(0x20|(tempcx&0x00C0)) );//Enable Dither
        XGINew_SetRegANDOR(pVBInfo->Part1Port,0x1A,0x7F,0x80);
    }
*/
}


/* --------------------------------------------------------------------- */
/* Function : XGI_SetLCDCap_B */
/* Input : cx -> LCD Capability */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGI_SetLCDCap_B(USHORT tempcx,PVB_DEVICE_INFO pVBInfo)
{
    if ( tempcx & EnableLCD24bpp )	/* 24bits */
        XGINew_SetRegANDOR( pVBInfo->Part2Port , 0x1A , 0xE0 , ( USHORT )( ( ( tempcx & 0x00ff ) >> 6 ) | 0x0c ) ) ;
    else
        XGINew_SetRegANDOR( pVBInfo->Part2Port , 0x1A , 0xE0 , ( USHORT )( ( ( tempcx & 0x00ff ) >> 6 ) | 0x18 ) ) ; /* Enable Dither */
}


/* --------------------------------------------------------------------- */
/* Function : SetSpectrum */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
static void SetSpectrum( PVB_DEVICE_INFO pVBInfo )
{
    USHORT index ;

    index = XGI_GetLCDCapPtr(pVBInfo) ;

    XGINew_SetRegAND( pVBInfo->Part4Port , 0x30 , 0x8F ) ;   /* disable down spectrum D[4] */
    XGI_LongWait(pVBInfo) ;
    XGINew_SetRegOR( pVBInfo->Part4Port , 0x30 , 0x20 ) ;	 /* reset spectrum */
    XGI_LongWait(pVBInfo) ;

    XGINew_SetReg1( pVBInfo->Part4Port , 0x31 , pVBInfo->LCDCapList[ index ].Spectrum_31 ) ;
    XGINew_SetReg1( pVBInfo->Part4Port , 0x32 , pVBInfo->LCDCapList[ index ].Spectrum_32 ) ;
    XGINew_SetReg1( pVBInfo->Part4Port , 0x33 , pVBInfo->LCDCapList[ index ].Spectrum_33 ) ;
    XGINew_SetReg1( pVBInfo->Part4Port , 0x34 , pVBInfo->LCDCapList[ index ].Spectrum_34 ) ;
    XGI_LongWait(pVBInfo) ;
    XGINew_SetRegOR( pVBInfo->Part4Port , 0x30 , 0x40 ) ; /* enable spectrum */
}


/* --------------------------------------------------------------------- */
/* Function : XGI_SetAntiFlicker */
/* Input : */
/* Output : */
/* Description : Set TV Customized Param. */
/* --------------------------------------------------------------------- */
void XGI_SetAntiFlicker( USHORT ModeNo , USHORT ModeIdIndex, PVB_DEVICE_INFO pVBInfo )
{
    USHORT tempbx ,
           index ;

    UCHAR tempah ;

    if (pVBInfo->TVInfo & ( SetYPbPrMode525p | SetYPbPrMode750p ) )
        return ;

    tempbx = XGI_GetTVPtrIndex(pVBInfo ) ;
    tempbx &= 0xFE ;

    if ( ModeNo <= 0x13 )
    {
        index = pVBInfo->SModeIDTable[ ModeIdIndex ].VB_StTVFlickerIndex ;
    }
    else
    {
        index = pVBInfo->EModeIDTable[ ModeIdIndex ].VB_ExtTVFlickerIndex ;
    }

    tempbx += index ;
    tempah = TVAntiFlickList[ tempbx ] ;
    tempah = tempah << 4 ;

    XGINew_SetRegANDOR( pVBInfo->Part2Port , 0x0A , 0x8F , tempah ) ;
}


/* --------------------------------------------------------------------- */
/* Function : XGI_SetEdgeEnhance */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGI_SetEdgeEnhance( USHORT ModeNo , USHORT ModeIdIndex , PVB_DEVICE_INFO pVBInfo)
{
    USHORT tempbx ,
           index ;

    UCHAR tempah ;


    tempbx = XGI_GetTVPtrIndex(pVBInfo ) ;
    tempbx &= 0xFE ;

    if ( ModeNo <= 0x13 )
    {
        index = pVBInfo->SModeIDTable[ ModeIdIndex ].VB_StTVEdgeIndex ;
    }
    else
    {
        index = pVBInfo->EModeIDTable[ ModeIdIndex ].VB_ExtTVEdgeIndex ;
    }

    tempbx += index ;
    tempah = TVEdgeList[ tempbx ] ;
    tempah = tempah << 5 ;

    XGINew_SetRegANDOR( pVBInfo->Part2Port , 0x3A , 0x1F , tempah ) ;
}


/* --------------------------------------------------------------------- */
/* Function : XGI_SetPhaseIncr */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGI_SetPhaseIncr( PVB_DEVICE_INFO pVBInfo )
{
    USHORT tempbx ;

    UCHAR tempcl ,
          tempch ;

    ULONG tempData ;

    XGI_GetTVPtrIndex2( &tempbx , &tempcl , &tempch, pVBInfo ) ; /* bx, cl, ch */
    tempData = TVPhaseList[ tempbx ] ;

    XGINew_SetReg1( pVBInfo->Part2Port , 0x31 , ( USHORT )( tempData & 0x000000FF ) ) ;
    XGINew_SetReg1( pVBInfo->Part2Port , 0x32 , ( USHORT )( ( tempData & 0x0000FF00 ) >> 8 ) ) ;
    XGINew_SetReg1( pVBInfo->Part2Port , 0x33 , ( USHORT )( ( tempData & 0x00FF0000 ) >> 16 ) ) ;
    XGINew_SetReg1( pVBInfo->Part2Port , 0x34 , ( USHORT )( ( tempData & 0xFF000000 ) >> 24 ) ) ;
}


/* --------------------------------------------------------------------- */
/* Function : XGI_SetYFilter */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGI_SetYFilter( USHORT ModeNo , USHORT ModeIdIndex, PVB_DEVICE_INFO pVBInfo)
{
    USHORT tempbx ,
           index ;

    UCHAR tempcl ,
          tempch ,
          tempal ,
          *filterPtr ;

    XGI_GetTVPtrIndex2( &tempbx , &tempcl , &tempch, pVBInfo ) ; /* bx, cl, ch */

    switch( tempbx )
    {
        case 0x00:
        case 0x04:
            filterPtr = NTSCYFilter1 ;
            break ;

        case 0x01:
            filterPtr = PALYFilter1 ;
            break ;

        case 0x02:
        case 0x05:
        case 0x0D:
            filterPtr = PALMYFilter1 ;
            break ;

        case 0x03:
            filterPtr = PALNYFilter1 ;
            break ;

        case 0x08:
        case 0x0C:
            filterPtr = NTSCYFilter2 ;
            break ;

        case 0x0A:
            filterPtr = PALMYFilter2 ;
            break ;

        case 0x0B:
            filterPtr = PALNYFilter2 ;
            break ;

        case 0x09:
            filterPtr = PALYFilter2 ;
            break ;

        default:
            return ;
    }

    if ( ModeNo <= 0x13 )
        tempal = pVBInfo->SModeIDTable[ ModeIdIndex ].VB_StTVYFilterIndex ;
    else
        tempal = pVBInfo->EModeIDTable[ ModeIdIndex ].VB_ExtTVYFilterIndex ;

    if ( tempcl == 0 )
        index = tempal * 4;
    else
        index = tempal * 7;

    if ( ( tempcl == 0 ) && ( tempch == 1 ) )
    {
        XGINew_SetReg1( pVBInfo->Part2Port , 0x35 , 0 ) ;
        XGINew_SetReg1( pVBInfo->Part2Port , 0x36 , 0 ) ;
        XGINew_SetReg1( pVBInfo->Part2Port , 0x37 , 0 ) ;
        XGINew_SetReg1( pVBInfo->Part2Port , 0x38 , filterPtr[ index++ ] ) ;
    }
    else
    {
        XGINew_SetReg1( pVBInfo->Part2Port , 0x35 , filterPtr[ index++ ] ) ;
        XGINew_SetReg1( pVBInfo->Part2Port , 0x36 , filterPtr[ index++ ] ) ;
        XGINew_SetReg1( pVBInfo->Part2Port , 0x37 , filterPtr[ index++ ] ) ;
        XGINew_SetReg1( pVBInfo->Part2Port , 0x38 , filterPtr[ index++ ] ) ;
    }

    if ( pVBInfo->VBType & ( VB_XGI301B | VB_XGI302B | VB_XGI301LV | VB_XGI302LV | VB_XGI301C ) )
    {
        XGINew_SetReg1( pVBInfo->Part2Port , 0x48 , filterPtr[ index++ ] ) ;
        XGINew_SetReg1( pVBInfo->Part2Port , 0x49 , filterPtr[ index++ ] ) ;
        XGINew_SetReg1( pVBInfo->Part2Port , 0x4A , filterPtr[ index++ ] ) ;
    }
}


/* --------------------------------------------------------------------- */
/* Function : XGI_GetTVPtrIndex2 */
/* Input : */
/* Output : bx 0 : NTSC */
/* 1 : PAL */
/* 2 : PALM */
/* 3 : PALN */
/* 4 : NTSC1024x768 */
/* 5 : PAL-M 1024x768 */
/* 6-7: reserved */
/* cl 0 : YFilter1 */
/* 1 : YFilter2 */
/* ch 0 : 301A */
/* 1 : 301B/302B/301LV/302LV */
/* Description : */
/* --------------------------------------------------------------------- */
void XGI_GetTVPtrIndex2(USHORT* tempbx,UCHAR* tempcl,UCHAR* tempch, PVB_DEVICE_INFO pVBInfo)
{
    *tempbx = 0 ;
    *tempcl = 0 ;
    *tempch = 0 ;

    if ( pVBInfo->TVInfo & SetPALTV )
        *tempbx = 1 ;

    if ( pVBInfo->TVInfo & SetPALMTV )
        *tempbx = 2 ;

    if ( pVBInfo->TVInfo & SetPALNTV )
        *tempbx = 3 ;

    if ( pVBInfo->TVInfo & NTSC1024x768 )
    {
        *tempbx = 4 ;
        if ( pVBInfo->TVInfo & SetPALMTV )
            *tempbx = 5 ;
    }

    if ( pVBInfo->VBType & ( VB_XGI301B | VB_XGI302B | VB_XGI301LV | VB_XGI302LV | VB_XGI301C ) )
    {
        if ( ( !( pVBInfo->VBInfo & SetInSlaveMode ) ) || ( pVBInfo->TVInfo & TVSimuMode ) )
        {
            *tempbx += 8 ;
            *tempcl += 1 ;
        }
    }

    if ( pVBInfo->VBType & ( VB_XGI301B | VB_XGI302B | VB_XGI301LV | VB_XGI302LV | VB_XGI301C ) )
        (*tempch)++ ;
}


/* --------------------------------------------------------------------- */
/* Function : XGI_SetCRT2ModeRegs */
/* Input : */
/* Output : */
/* Description : Origin code for crt2group */
/* --------------------------------------------------------------------- */
void XGI_SetCRT2ModeRegs(USHORT ModeNo,PXGI_HW_DEVICE_INFO HwDeviceExtension, PVB_DEVICE_INFO pVBInfo)
{
    USHORT  tempbl ;


    int tempcl ;

    UCHAR  tempah ;

    /* XGINew_SetReg1( pVBInfo->Part1Port , 0x03 , 0x00 ) ; // fix write part1 index 0 BTDRAM bit Bug */
    tempah=0;
    if ( !( pVBInfo->VBInfo & DisableCRT2Display ) )
    {
        tempah=XGINew_GetReg1( pVBInfo->Part1Port , 0x00 ) ;
        tempah &= ~0x10 ;	/* BTRAMDAC */
        tempah |=  0x40 ;	/* BTRAM */

        if ( pVBInfo->VBInfo & ( SetCRT2ToRAMDAC | SetCRT2ToTV | SetCRT2ToLCD ) )
        {
            tempah=0x40;	/* BTDRAM */
            if ( ModeNo > 0x13 )
            {
                tempcl = pVBInfo->ModeType ;
                tempcl -= ModeVGA ;
                if ( tempcl >= 0 )
                {
                    tempah = ( 0x008 >> tempcl ) ;	/* BT Color */
                    if ( tempah == 0 )
                        tempah = 1 ;
                    tempah |= 0x040 ;
                }
            }
            if ( pVBInfo->VBInfo & SetInSlaveMode )
                tempah ^= 0x50 ;	/* BTDAC */
        }
    }

/*	0210 shampoo
    if ( pVBInfo->VBInfo & DisableCRT2Display )
    {
        tempah = 0 ;
    }

    XGINew_SetReg1( pVBInfo->Part1Port , 0x00 , tempah ) ;
    if ( pVBInfo->VBInfo & ( SetCRT2ToRAMDAC | SetCRT2ToTV | SetCRT2ToLCD ) )
    {
        tempcl = pVBInfo->ModeType ;
        if ( ModeNo > 0x13 )
        {
            tempcl -= ModeVGA ;
            if ( ( tempcl > 0 ) || ( tempcl == 0 ) )
            {
                tempah=(0x008>>tempcl) ;
                if ( tempah == 0 )
                    tempah = 1 ;
                tempah |= 0x040;
            }
        }
        else
        {
            tempah = 0x040 ;
        }

        if ( pVBInfo->VBInfo & SetInSlaveMode )
        {
            tempah = ( tempah ^ 0x050 ) ;
        }
    }
*/

    XGINew_SetReg1( pVBInfo->Part1Port , 0x00 , tempah ) ;
    tempah = 0x08 ;
    tempbl = 0xf0 ;

    if ( pVBInfo->VBInfo & DisableCRT2Display )
        XGINew_SetRegANDOR( pVBInfo->Part1Port , 0x2e , tempbl , tempah ) ;
    else
    {
        tempah = 0x00 ;
        tempbl = 0xff ;

        if ( pVBInfo->VBInfo & ( SetCRT2ToRAMDAC | SetCRT2ToTV | SetCRT2ToLCD | SetCRT2ToLCDA ) )
        {
            if ( ( pVBInfo->VBInfo & SetCRT2ToLCDA ) && ( !( pVBInfo->VBInfo & SetSimuScanMode ) ) )
            {
                tempbl &= 0xf7 ;
                tempah |= 0x01 ;
                XGINew_SetRegANDOR( pVBInfo->Part1Port , 0x2e , tempbl , tempah ) ;
            }
            else
            {
                if ( pVBInfo->VBInfo & SetCRT2ToLCDA )
                {
                    tempbl &= 0xf7 ;
                    tempah |= 0x01 ;
                }

                if ( pVBInfo->VBInfo & ( SetCRT2ToRAMDAC | SetCRT2ToTV | SetCRT2ToLCD ) )
                {
                    tempbl &= 0xf8 ;
                    tempah = 0x01 ;

                    if ( !( pVBInfo->VBInfo & SetInSlaveMode ) )
                        tempah |= 0x02 ;

                    if ( !( pVBInfo->VBInfo & SetCRT2ToRAMDAC ) )
                    {
                        tempah = tempah ^ 0x05 ;
                        if ( !( pVBInfo->VBInfo & SetCRT2ToLCD ) )
                            tempah = tempah ^ 0x01 ;
                    }

                    if ( !( pVBInfo->VBInfo & SetCRT2ToDualEdge ) )
                        tempah |= 0x08 ;
                    XGINew_SetRegANDOR( pVBInfo->Part1Port , 0x2e , tempbl , tempah ) ;
                }
                else
                    XGINew_SetRegANDOR( pVBInfo->Part1Port , 0x2e , tempbl , tempah ) ;
            }
        }
        else
            XGINew_SetRegANDOR( pVBInfo->Part1Port , 0x2e , tempbl , tempah ) ;
    }

    if ( pVBInfo->VBInfo & ( SetCRT2ToRAMDAC | SetCRT2ToTV | SetCRT2ToLCD | SetCRT2ToLCDA ) )
    {
        tempah &= ( ~0x08 ) ;
        if ( ( pVBInfo->ModeType == ModeVGA ) && ( !( pVBInfo->VBInfo & SetInSlaveMode ) ) )
        {
            tempah |= 0x010 ;
        }
        tempah |= 0x080 ;

        if ( pVBInfo->VBInfo & SetCRT2ToTV )
        {
            /* if ( !( pVBInfo->TVInfo & ( SetYPbPrMode525p | SetYPbPrMode750p ) ) ) */
            /* { */
            tempah |= 0x020 ;
            if ( ModeNo > 0x13 )
            {
                if ( pVBInfo->VBInfo & DriverMode )
                    tempah = tempah ^ 0x20 ;
            }
        /* } */
        }

        XGINew_SetRegANDOR( pVBInfo->Part4Port , 0x0D , ~0x0BF , tempah ) ;
        tempah = 0 ;

        if ( pVBInfo->LCDInfo & SetLCDDualLink )
            tempah |= 0x40 ;

        if ( pVBInfo->VBInfo & SetCRT2ToTV )
        {
            /* if ( ( !( pVBInfo->VBInfo & SetCRT2ToHiVisionTV ) ) && ( !( pVBInfo->TVInfo & ( SetYPbPrMode525p | SetYPbPrMode750p ) ) ) ) */
            /* { */
                if ( pVBInfo->TVInfo & RPLLDIV2XO )
                    tempah |= 0x40 ;
            /* } */
        }

        if ( ( pVBInfo->LCDResInfo == Panel1280x1024 ) || ( pVBInfo->LCDResInfo == Panel1280x1024x75 ) )
            tempah |= 0x80 ;

        if ( pVBInfo->LCDResInfo == Panel1280x960 )
            tempah |= 0x80 ;

        XGINew_SetReg1( pVBInfo->Part4Port , 0x0C , tempah ) ;
    }

    if ( pVBInfo->VBType & ( VB_XGI301B | VB_XGI302B | VB_XGI301LV | VB_XGI302LV | VB_XGI301C ) )
    {
        tempah = 0 ;
        tempbl = 0xfb ;

        if ( pVBInfo->VBInfo & SetCRT2ToDualEdge )
        {
            tempbl=0xff;
            if ( pVBInfo->VBInfo & SetCRT2ToLCDA )
                tempah |= 0x04 ; /* shampoo 0129 */
        }

        XGINew_SetRegANDOR( pVBInfo->Part1Port , 0x13 , tempbl , tempah ) ;
        tempah = 0x00 ;
        tempbl = 0xcf ;
        if ( !( pVBInfo->VBInfo & DisableCRT2Display ) )
        {
            if ( pVBInfo->VBInfo & SetCRT2ToDualEdge )
                tempah |= 0x30 ;
        }

        XGINew_SetRegANDOR( pVBInfo->Part1Port , 0x2c , tempbl , tempah ) ;
        tempah = 0 ;
        tempbl = 0x3f ;

        if ( !( pVBInfo->VBInfo & DisableCRT2Display ) )
        {
            if ( pVBInfo->VBInfo & SetCRT2ToDualEdge )
                tempah |= 0xc0 ;
        }
        XGINew_SetRegANDOR( pVBInfo->Part4Port , 0x21 , tempbl , tempah ) ;
    }

    tempah = 0 ;
    tempbl = 0x7f ;
    if ( !( pVBInfo->VBInfo & SetCRT2ToLCDA ) )
    {
        tempbl = 0xff ;
        if ( !( pVBInfo->VBInfo & SetCRT2ToDualEdge ) )
            tempah |= 0x80 ;
    }

    XGINew_SetRegANDOR( pVBInfo->Part4Port , 0x23 , tempbl , tempah ) ;

    if ( pVBInfo->VBType & ( VB_XGI302LV | VB_XGI301C ) )
    {
        if ( pVBInfo->LCDInfo & SetLCDDualLink )
        {
            XGINew_SetRegOR( pVBInfo->Part4Port , 0x27 , 0x20 ) ;
            XGINew_SetRegOR( pVBInfo->Part4Port , 0x34 , 0x10 ) ;
        }
    }
}


/* --------------------------------------------------------------------- */
/* Function : XGI_CloseCRTC */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGI_CloseCRTC( PXGI_HW_DEVICE_INFO HwDeviceExtension, PVB_DEVICE_INFO pVBInfo )
{
}


/* --------------------------------------------------------------------- */
/* Function : XGI_OpenCRTC */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGI_OpenCRTC( PXGI_HW_DEVICE_INFO HwDeviceExtension, PVB_DEVICE_INFO pVBInfo )
{
}


/* --------------------------------------------------------------------- */
/* Function : XGI_GetRAMDAC2DATA */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGI_GetRAMDAC2DATA(USHORT ModeNo , USHORT ModeIdIndex , USHORT RefreshRateTableIndex, PVB_DEVICE_INFO pVBInfo )
{
    USHORT tempax ,
           tempbx ,           
           temp1 ,
           temp2 ,
           modeflag = 0 ,
           tempcx ,
           StandTableIndex ,
           CRT1Index ;
           
           


    pVBInfo->RVBHCMAX = 1 ;
    pVBInfo->RVBHCFACT = 1 ;

    if ( ModeNo <= 0x13 )
    {
        modeflag = pVBInfo->SModeIDTable[ ModeIdIndex ].St_ModeFlag ;
        StandTableIndex = XGI_GetModePtr( ModeNo , ModeIdIndex, pVBInfo ) ;
        tempax = pVBInfo->StandTable[ StandTableIndex ].CRTC[ 0 ] ;
        tempbx = pVBInfo->StandTable[StandTableIndex ].CRTC[ 6 ] ;
        temp1 = pVBInfo->StandTable[ StandTableIndex ].CRTC[ 7 ] ;
    }
    else
    {
        modeflag = pVBInfo->EModeIDTable[ ModeIdIndex ].Ext_ModeFlag ;
        CRT1Index = pVBInfo->RefIndex[ RefreshRateTableIndex ].Ext_CRT1CRTC ;
        CRT1Index &= IndexMask ;
        temp1 = ( USHORT )pVBInfo->XGINEWUB_CRT1Table[ CRT1Index ].CR[ 0 ] ;
        temp2 = ( USHORT )pVBInfo->XGINEWUB_CRT1Table[ CRT1Index ].CR[ 5 ] ;
	tempax = ( temp1 & 0xFF ) | ( ( temp2 & 0x03 ) << 8 ) ;
        tempbx = ( USHORT )pVBInfo->XGINEWUB_CRT1Table[ CRT1Index ].CR[ 8 ] ;
        tempcx = ( USHORT )pVBInfo->XGINEWUB_CRT1Table[ CRT1Index ].CR[ 14 ] << 8 ;
	tempcx &= 0x0100 ;
	tempcx = tempcx << 2 ;
	tempbx |= tempcx;
        temp1 = ( USHORT )pVBInfo->XGINEWUB_CRT1Table[ CRT1Index ].CR[ 9 ] ;
    }

    if ( temp1 & 0x01 )
        tempbx |= 0x0100 ;

    if ( temp1 & 0x20 )
        tempbx |= 0x0200 ;
    tempax += 5 ;

    if ( modeflag & Charx8Dot )
        tempax *= 8 ;
    else
        tempax *= 9 ;

    pVBInfo->VGAHT = tempax ;
    pVBInfo->HT = tempax ;
    tempbx++ ;
    pVBInfo->VGAVT = tempbx ;
    pVBInfo->VT = tempbx ;
}



/* --------------------------------------------------------------------- */
/* Function : XGI_GetColorDepth */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
USHORT XGI_GetColorDepth(USHORT ModeNo , USHORT ModeIdIndex, PVB_DEVICE_INFO pVBInfo)
{
    USHORT ColorDepth[ 6 ] = { 1 , 2 , 4 , 4 , 6 , 8 } ;
    SHORT  index ;
    USHORT modeflag ;

    if ( ModeNo <= 0x13 )
    {
        modeflag = pVBInfo->SModeIDTable[ ModeIdIndex ].St_ModeFlag ;
    }
    else
    {
        modeflag = pVBInfo->EModeIDTable[ ModeIdIndex ].Ext_ModeFlag ;
    }

    index=(modeflag&ModeInfoFlag)-ModeEGA;

    if ( index < 0 )
        index = 0 ;

    return( ColorDepth[ index ] ) ;
}



/* --------------------------------------------------------------------- */
/* Function : XGI_UnLockCRT2 */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGI_UnLockCRT2( PXGI_HW_DEVICE_INFO HwDeviceExtension,  PVB_DEVICE_INFO pVBInfo )
{
    
    XGINew_SetRegANDOR( pVBInfo->Part1Port , 0x2f , 0xFF , 0x01 ) ;

}


/* --------------------------------------------------------------------- */
/* Function : XGI_LockCRT2 */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGI_LockCRT2( PXGI_HW_DEVICE_INFO HwDeviceExtension,  PVB_DEVICE_INFO pVBInfo )
{
    
    XGINew_SetRegANDOR( pVBInfo->Part1Port , 0x2F , 0xFE , 0x00 ) ;
    
    
}


/* --------------------------------------------------------------------- */
/* Function : XGINew_EnableCRT2 */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGINew_EnableCRT2( PVB_DEVICE_INFO pVBInfo)
{
    XGINew_SetRegANDOR( pVBInfo->P3c4 , 0x1E , 0xFF , 0x20 ) ;
}



/* --------------------------------------------------------------------- */
/* Function : */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGINew_LCD_Wait_Time(UCHAR DelayTime, PVB_DEVICE_INFO pVBInfo)
{
    USHORT i ,
           j ;

    ULONG temp ,
          flag ;

    flag = 0 ;

    for( i = 0 ; i < DelayTime ; i++ )
    {
        for( j = 0 ; j < 66 ; j++ )
        {
            temp = XGINew_GetReg3( 0x61 ) ;
            temp &= 0x10;

            if ( temp == flag )
                continue ;

            flag = temp ;
        }
    }
}




/* --------------------------------------------------------------------- */
/* Function : XGI_BridgeIsOn */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
BOOLEAN XGI_BridgeIsOn( PVB_DEVICE_INFO pVBInfo )
{
    USHORT flag ;

    if ( pVBInfo->IF_DEF_LVDS == 1 )
    {
        return( 1 ) ;
    }
    else
    {
        flag = XGINew_GetReg1( pVBInfo->Part4Port , 0x00 ) ;
        if ( ( flag == 1 ) || ( flag == 2 ) )
            return( 1 ) ;	/* 301b */
        else
            return( 0 ) ;
    }
}



/* --------------------------------------------------------------------- */
/* Function : XGI_LongWait */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGI_LongWait(PVB_DEVICE_INFO pVBInfo)
{
    USHORT i ;

    i = XGINew_GetReg1( pVBInfo->P3c4 , 0x1F ) ;

    if ( !( i & 0xC0 ) )
    {
        for( i = 0 ; i < 0xFFFF ; i++ )
        {
            if ( !( XGINew_GetReg2( pVBInfo->P3da ) & 0x08 ) )
                break ;
        }

        for( i = 0 ; i < 0xFFFF ; i++ )
        {
            if ( ( XGINew_GetReg2( pVBInfo->P3da ) & 0x08 ) )
                break ;
	}
    }
}


/* --------------------------------------------------------------------- */
/* Function : XGI_VBLongWait */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGI_VBLongWait( PVB_DEVICE_INFO pVBInfo )
{
    USHORT tempal ,
           temp ,
           i ,
           j ;

    if ( !( pVBInfo->VBInfo & SetCRT2ToTV ) )
    {
        temp = 0 ;
        for( i = 0 ; i < 3 ; i++ )
        {
            for( j = 0 ; j < 100 ; j++ )
            {
                tempal = XGINew_GetReg2( pVBInfo->P3da ) ;
                if ( temp & 0x01 )
                {			/* VBWaitMode2 */
                    if ( ( tempal & 0x08 ) )
                    {
                        continue ;
                    }

                    if ( !( tempal & 0x08 ) )
                    {
                        break ;
                    }
                }
                else
                {			/* VBWaitMode1 */
                    if ( !( tempal & 0x08 ) )
                    {
                        continue ;
                    }

                    if ( ( tempal & 0x08 ) )
                    {
                        break ;
                    }
                }
            }
            temp = temp ^ 0x01 ;
        }
    }
    else
    {
        XGI_LongWait(pVBInfo) ;
    }
    return ;
}




/* --------------------------------------------------------------------- */
/* Function : XGI_GetVGAHT2 */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
USHORT XGI_GetVGAHT2( PVB_DEVICE_INFO pVBInfo )
{
    ULONG tempax ,
          tempbx ;

    tempbx = ( ( pVBInfo->VGAVT - pVBInfo->VGAVDE ) * pVBInfo->RVBHCMAX ) & 0xFFFF ;
    tempax = ( pVBInfo->VT - pVBInfo->VDE ) * pVBInfo->RVBHCFACT ;
    tempax = ( tempax * pVBInfo->HT ) /tempbx ;

    return( ( USHORT )tempax ) ;
}


/* --------------------------------------------------------------------- */
/* Function : XGI_GetVCLK2Ptr */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
USHORT XGI_GetVCLK2Ptr( USHORT ModeNo , USHORT ModeIdIndex , USHORT RefreshRateTableIndex , PXGI_HW_DEVICE_INFO HwDeviceExtension ,PVB_DEVICE_INFO pVBInfo)
{
    USHORT tempbx ;

    USHORT LCDXlat1VCLK[ 4 ] = { VCLK65 + 2 , VCLK65 + 2 , VCLK65 + 2 , VCLK65 + 2 } ;
    USHORT LCDXlat2VCLK[ 4 ] = { VCLK108_2 + 5 , VCLK108_2 + 5 , VCLK108_2 + 5 , VCLK108_2 + 5 } ;
    USHORT LVDSXlat1VCLK[ 4 ] = { VCLK40 , VCLK40 , VCLK40 , VCLK40 } ;
    USHORT LVDSXlat2VCLK[ 4 ] = { VCLK65 + 2 , VCLK65 + 2 , VCLK65 + 2 , VCLK65 + 2 } ;
    USHORT LVDSXlat3VCLK[ 4 ] = { VCLK65 + 2 , VCLK65 + 2 , VCLK65 + 2 , VCLK65 + 2 } ;

    USHORT CRT2Index , VCLKIndex=0 ;
    USHORT modeflag ;
    UCHAR *CHTVVCLKPtr = NULL ;

    if ( ModeNo <= 0x13 )
    {
        modeflag = pVBInfo->SModeIDTable[ ModeIdIndex ].St_ModeFlag ;      /* si+St_ResInfo */
        CRT2Index = pVBInfo->SModeIDTable[ ModeIdIndex ].St_CRT2CRTC ;
    }
    else
    {
        modeflag = pVBInfo->EModeIDTable[ ModeIdIndex ].Ext_ModeFlag ;     /* si+Ext_ResInfo */
        CRT2Index = pVBInfo->RefIndex[ RefreshRateTableIndex ].Ext_CRT2CRTC ;
    }

    if ( pVBInfo->IF_DEF_LVDS == 0 )
    {
        CRT2Index = CRT2Index >> 6 ;        /*  for LCD */
        if ( ( ( pVBInfo->VBInfo & SetCRT2ToLCD ) | SetCRT2ToLCDA ) )	 /*301b*/
        {
            if ( pVBInfo->LCDResInfo != Panel1024x768 )
            {
                VCLKIndex = LCDXlat2VCLK[ CRT2Index ] ;
            }
            else
            {
                 VCLKIndex = LCDXlat1VCLK[ CRT2Index ] ;
            }
        }
        else	/* for TV */
        {
            if ( pVBInfo->VBInfo & SetCRT2ToTV )
            {
                if ( pVBInfo->VBInfo & SetCRT2ToHiVisionTV )
                {
                    if ( pVBInfo->SetFlag & RPLLDIV2XO )
                    {
                        VCLKIndex = HiTVVCLKDIV2 ;
                        
                            
                            VCLKIndex += 25 ;   
                      
                    }
                    else
                    {
                        VCLKIndex = HiTVVCLK ;
                        
                            
                            VCLKIndex += 25 ;   
                      
                    }

                    if ( pVBInfo->SetFlag & TVSimuMode )
                    {
                        if( modeflag & Charx8Dot )
                        {
                            VCLKIndex = HiTVSimuVCLK ;
                            
				
                                VCLKIndex += 25 ;	
                          
                        }
                        else
                        {
                            VCLKIndex = HiTVTextVCLK ;

                                
                                VCLKIndex += 25 ;	
                         
                        }
                    }

                    if ( pVBInfo->VBType & VB_XGI301LV )	/* 301lv */
                    {
                        if ( !( pVBInfo->VBExtInfo == VB_YPbPr1080i ) )
                        {
                            VCLKIndex = YPbPr750pVCLK ;
                            if ( !( pVBInfo->VBExtInfo == VB_YPbPr750p ) )
                            {
                                VCLKIndex = YPbPr525pVCLK ;
                                if ( !( pVBInfo->VBExtInfo == VB_YPbPr525p ) )
                                {
                                    VCLKIndex = YPbPr525iVCLK_2 ;
                                    if ( !( pVBInfo->SetFlag & RPLLDIV2XO ) )
                                        VCLKIndex = YPbPr525iVCLK ;
                                }
                            }
                        }
                    }
                }
                else
                {
                    if ( pVBInfo->VBInfo & SetCRT2ToTV )
                    {
                        if ( pVBInfo->SetFlag & RPLLDIV2XO )
			{
                            VCLKIndex = TVVCLKDIV2 ;
                            
			
                            VCLKIndex += 25 ;
                            
                        }
                        else
                        {
                            VCLKIndex = TVVCLK ;
                            
			    
                            VCLKIndex += 25 ;	

                        }
                    }
                }
            }
            else
            {	/* for CRT2 */
                VCLKIndex = ( UCHAR )XGINew_GetReg2( ( USHORT )( pVBInfo->P3ca + 0x02 ) ) ;	/* Port 3cch */
		VCLKIndex = ( ( VCLKIndex >> 2 ) & 0x03 ) ;
                if ( ModeNo > 0x13 )
                {
                    VCLKIndex = pVBInfo->RefIndex[ RefreshRateTableIndex ].Ext_CRTVCLK ;	/* di+Ext_CRTVCLK */
                    VCLKIndex &= IndexMask ;
                }
            }
        }
    }
    else
    {		/* LVDS */
        if ( ModeNo <= 0x13 )
            VCLKIndex = CRT2Index ;
	else
	    VCLKIndex = CRT2Index ;

        if ( pVBInfo->IF_DEF_CH7005 == 1 )
        {
            if ( !( pVBInfo->VBInfo & SetCRT2ToLCD ) )
            {
                VCLKIndex &= 0x1f ;
		tempbx = 0 ;

                if ( pVBInfo->VBInfo & SetPALTV )
                    tempbx += 2 ;

                if ( pVBInfo->VBInfo & SetCHTVOverScan )
                    tempbx += 1 ;

                switch( tempbx )
                {
                    case 0:
                        CHTVVCLKPtr = pVBInfo->CHTVVCLKUNTSC ;
                        break ;
                    case 1:
                        CHTVVCLKPtr = pVBInfo->CHTVVCLKONTSC ;
                        break;
                    case 2:
                        CHTVVCLKPtr = pVBInfo->CHTVVCLKUPAL ;
                        break ;
                    case 3:
                        CHTVVCLKPtr = pVBInfo->CHTVVCLKOPAL ;
                        break ;
                    default:
                        break ;
                }

                VCLKIndex = CHTVVCLKPtr[ VCLKIndex ] ;
            }
        }
        else
        {
            VCLKIndex = VCLKIndex >> 6 ;
            if ( ( pVBInfo->LCDResInfo == Panel800x600 ) || ( pVBInfo->LCDResInfo == Panel320x480 ) )
                VCLKIndex = LVDSXlat1VCLK[ VCLKIndex ] ;
            else if ( ( pVBInfo->LCDResInfo == Panel1024x768 ) || ( pVBInfo->LCDResInfo == Panel1024x768x75 ) )
                VCLKIndex = LVDSXlat2VCLK[ VCLKIndex ] ;
            else
                VCLKIndex = LVDSXlat3VCLK[ VCLKIndex ] ;
        }
    }
    /* VCLKIndex = VCLKIndex&IndexMask ; */

    

    return( VCLKIndex ) ;
}

