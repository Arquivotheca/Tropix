/* $XFree86: xc/programs/Xserver/hw/xfree86/drivers/xgi/vb_ext.c,v 1.1 2005/05/02 13:28:02 dawes Exp $ */

#include "osdef.h"

#ifdef WIN2000

#include <dderror.h>
#include <devioctl.h>
#include <miniport.h>
#include <ntddvdeo.h>
#include <video.h>
#include "xgiv.h"
#include "dd_i2c.h"
#include "tools.h"
#endif /* WIN2000 */

#ifdef LINUX_XF86
#include "xf86.h"
#include "xf86PciInfo.h"
#include "xgi.h"
#include "xgi_regs.h"
#endif

#ifdef LINUX_KERNEL
#include <linux/version.h>
#include <asm/io.h>
#include <linux/types.h>
#include "XGIfb.h"
/*#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,5,0)
#include <video/XGIfb.h>
#else
#include <linux/XGIfb.h>
#endif*/
#endif



#include "vb_def.h"
#include "vgatypes.h"
#include "vb_struct.h"
#include "vb_util.h"
#include "vb_setmode.h"
#include "vb_ext.h"
extern   UCHAR XGI330_SoftSetting;
extern   UCHAR XGI330_OutputSelect;
extern   USHORT XGI330_RGBSenseData2;
extern   USHORT XGI330_YCSenseData2;
extern   USHORT XGI330_VideoSenseData2;

BOOLEAN  XGINew_GetPanelID(PVB_DEVICE_INFO pVBInfo);
USHORT   XGINew_SenseLCD(PXGI_HW_DEVICE_INFO,PVB_DEVICE_INFO pVBInfo);
BOOLEAN  XGINew_GetLCDDDCInfo(PXGI_HW_DEVICE_INFO HwDeviceExtension,PVB_DEVICE_INFO pVBInfo);
BOOLEAN  XGINew_BridgeIsEnable(PXGI_HW_DEVICE_INFO,PVB_DEVICE_INFO pVBInfo );
BOOLEAN  XGINew_Sense(USHORT tempbx,USHORT tempcx, PVB_DEVICE_INFO pVBInfo);
BOOLEAN  XGINew_SenseHiTV( PXGI_HW_DEVICE_INFO HwDeviceExtension , PVB_DEVICE_INFO pVBInfo ) ;

/**************************************************************
	Dynamic Sense
*************************************************************/

BOOLEAN XGI_Is301C(PVB_DEVICE_INFO);
BOOLEAN XGI_Is301LV(PVB_DEVICE_INFO);

#ifdef WIN2000
UCHAR XGI_SenseLCD(PHW_DEVICE_EXTENSION, PVB_DEVICE_INFO);
UCHAR XGI_GetLCDDDCInfo(PHW_DEVICE_EXTENSION,PVB_DEVICE_INFO);

extern BOOL bGetDdcInfo(
PHW_DEVICE_EXTENSION  pHWDE,
ULONG                 ulWhichOne,
PUCHAR                pjQueryBuffer,
ULONG                 ulBufferSize
   );

#endif


/* --------------------------------------------------------------------- */
/* Function : XGINew_Is301B */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
static BOOLEAN XGINew_Is301B( PVB_DEVICE_INFO pVBInfo )
{
    USHORT flag ;

    flag = XGINew_GetReg1( pVBInfo->Part4Port , 0x01 ) ;

    if ( flag > 0x0B0 )
        return( 0 ) ;	/* 301b */
    else
        return( 1 ) ;
}

/* --------------------------------------------------------------------- */
/* Function : XGI_Is301C */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
BOOLEAN XGI_Is301C( PVB_DEVICE_INFO pVBInfo )
{
    if ( ( XGINew_GetReg1( pVBInfo->Part4Port , 0x01 ) & 0xF0 ) == 0xC0 )
        return( 1 ) ;

    if ( XGINew_GetReg1( pVBInfo->Part4Port , 0x01 ) >= 0xD0 )
    {
        if ( XGINew_GetReg1( pVBInfo->Part4Port , 0x39 ) == 0xE0 )
            return( 1 ) ;
    }

    return( 0 ) ;
}


/* --------------------------------------------------------------------- */
/* Function : XGI_Is301LV */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
BOOLEAN XGI_Is301LV( PVB_DEVICE_INFO pVBInfo )
{
    if ( XGINew_GetReg1( pVBInfo->Part4Port , 0x01 ) >= 0xD0 )
    {
        if ( XGINew_GetReg1( pVBInfo->Part4Port , 0x39 ) == 0xFF )
        {
            return( 1 ) ;
        }
    }
    return( 0 ) ;
}


/* --------------------------------------------------------------------- */
/* Function : XGINew_Sense */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
BOOLEAN XGINew_Sense(  USHORT tempbx , USHORT tempcx, PVB_DEVICE_INFO pVBInfo )
{
    USHORT temp , i , tempch ;

    temp = tempbx & 0xFF ;
    XGINew_SetReg1( pVBInfo->Part4Port , 0x11 , temp ) ;
    temp = ( tempbx & 0xFF00 ) >> 8 ;
    temp |= ( tempcx & 0x00FF ) ;
    XGINew_SetRegANDOR( pVBInfo->Part4Port , 0x10 , ~0x1F , temp ) ;

    for( i = 0 ; i < 10 ; i++ )
        XGI_LongWait( pVBInfo) ;

    tempch = ( tempcx & 0x7F00 ) >> 8 ;
    temp = XGINew_GetReg1( pVBInfo->Part4Port , 0x03 ) ;
    temp = temp ^ ( 0x0E ) ;
    temp &= tempch ;

    if ( temp > 0 )
        return( 1 ) ;
    else
        return( 0 ) ;
}

#ifdef WIN2000
/* --------------------------------------------------------------------- */
/* Function : XGI_SenseLCD */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
UCHAR XGI_SenseLCD( PHW_DEVICE_EXTENSION pHWDE, PVB_DEVICE_INFO pVBInfo)
{
    USHORT tempax , tempbx , tempcx ;
    UCHAR SoftSetting = XGI330_SoftSetting ;

    if ( pVBInfo->VBType & ( VB_XGI301LV | VB_XGI302LV ) )
        return( 1 ) ;


    if ( SoftSetting & HotPlugFunction )	/* Hot Plug Detection */
    {
        XGINew_SetRegAND( pVBInfo->Part4Port , 0x0F , 0x3F ) ;
        tempbx = 0 ;
        tempcx = 0x9010 ;
        if ( XGINew_Sense( tempbx , tempcx, pVBInfo ) )
            return( 1 ) ;

        return( 0 ) ;
    }
    else	/* Get LCD Info from EDID */
        return(XGI_GetLCDDDCInfo(pHWDE, pVBInfo));
}


/* --------------------------------------------------------------------- */
/* Function : XGI_GetLCDDDCInfo */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
UCHAR XGI_GetLCDDDCInfo( PHW_DEVICE_EXTENSION pHWDE , PVB_DEVICE_INFO pVBInfo)
{
    UCHAR tempah , tempbl , tempbh ;
    USHORT tempbx , temp ;
    UCHAR pjEDIDBuf[ 256 ] ;
    ULONG ulBufferSize = 256 ;
    UCHAR bMASK_OUTPUTSTATE_CRT2LCD = 2 ; /* 0423 shampoo */

    bGetDdcInfo( pHWDE , MASK_OUTPUTSTATE_CRT2LCD , pjEDIDBuf , ulBufferSize ) ;
    if ( ( *( ( PULONG )pjEDIDBuf ) == 0xFFFFFF00 ) && ( *( ( PULONG )( pjEDIDBuf + 4 ) ) == 0x00FFFFFF ) )
    {
        tempah = Panel1024x768 ;
        tempbl=( *( pjEDIDBuf + 0x3A ) ) & 0xf0 ;

        if ( tempbl != 0x40 )
        {
            tempah = Panel1600x1200 ;
            if ( tempbl != 0x60 )
            {
                tempah = Panel1280x1024 ;
                tempbh = ( *( pjEDIDBuf + 0x3B ) ) ;
                if ( tempbh != 0x00 )
                {
                    tempah = Panel1280x960 ;
                    if ( tempbh != 0x0C0 )
                    {
                        tempbx = ( ( *( pjEDIDBuf + 0x24 ) ) << 8 ) | ( *( pjEDIDBuf + 0x23 ) ) ;
                        tempah = Panel1280x1024 ;
                        if ( !( tempbx & 0x0100 ) )
                        {
                            tempah = Panel1024x768 ;
                            if ( !( tempbx & 0x0E00 ) )
                            {
                                tempah = Panel1280x1024 ;
                            }
                        }
                    }

                    if ( tempbx & 0x00FF )
                    {
                        temp = ScalingLCD ;
                        XGINew_SetRegOR( pVBInfo->P3d4 , 0x37 , temp ) ;
                    }
                }
            }
        }
        XGINew_SetRegANDOR( pVBInfo->P3d4 , 0x36 , ( ~0x07 ) , tempah ) ;
        tempah = ( ( *( pjEDIDBuf + 0x47 ) ) & 0x06 ) ;		/* Polarity */
        tempah = ( tempah ^ 0x06 ) << 4 ;
        tempah |= LCDSync ;
        XGINew_SetRegANDOR( pVBInfo->P3d4 , 0x37 , ( ~LCDSyncBit ) , tempah ) ;
        tempbh= XGINew_GetReg1( pVBInfo->P3d4 , 0x36 ) ;
        tempbh &= 0x07 ;
        if ( tempbh == Panel1280x960 )
            XGINew_SetRegAND( pVBInfo->P3d4 , 0x37 , 0x0E ) ;
    }
    else if ( *pjEDIDBuf == 0x20 )
    {
        tempah = Panel1024x768 ;
        XGINew_SetRegANDOR( pVBInfo->P3d4 , 0x36 , ( ~0x07 ) , tempah ) ;
    }
    else
    {
        return( 0 ) ;
    }

    return( 1 ) ;
}


/* --------------------------------------------------------------------- */
/* Function : XGI_DySense */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
BOOLEAN XGI_DySense( PHW_DEVICE_EXTENSION pHWDE , PUCHAR ujConnectStatus)
{
    UCHAR pre_CRD,pre_SR1E , pre_Part2_0 , pre_Part4_D ;
    USHORT tempax , tempbx , tempcx , pushax , temp ;
    VB_DEVICE_INFO VBINF;
    PVB_DEVICE_INFO pVBInfo = &VBINF;
    UCHAR OutputSelect = XGI330_OutputSelect ;
    PXGI_HW_DEVICE_INFO HwDeviceExtension= pHWDE->pXGIHWDE ;
    UCHAR   bConnectStatus = 0 ;
    pVBInfo->BaseAddr = ( USHORT )HwDeviceExtension->pjIOAddress ;
    pVBInfo->ROMAddr  = pHWDE->pjVirtualRomBase ;

    pVBInfo->P3c2 = pVBInfo->BaseAddr + 0x12 ;
    pVBInfo->P3c4 = pVBInfo->BaseAddr + 0x14 ;
    pVBInfo->P3d4 = pVBInfo->BaseAddr + 0x24 ;
    pVBInfo->Part2Port = pVBInfo->BaseAddr + XGI_CRT2_PORT_10 ;
    pVBInfo->Part4Port = pVBInfo->BaseAddr + XGI_CRT2_PORT_14 ;
    pushax = XGINew_GetReg1( pVBInfo->P3d4 , 0x17 ) ;	/* 0512 Fix Dysense hanged */
    temp = ( pushax & 0x00FF ) | 0x80 ;
    XGINew_SetRegOR( pVBInfo->P3d4 , 0x17 , temp ) ;
    XGINew_SetReg1( pVBInfo->P3c4 , 0x05 , 0x86 ) ;
    /* beginning of dynamic sense CRT1 */

    if(( pHWDE->jChipID >= XG40 ) || ( pHWDE->jChipID == XG20 ))
    {

        if ( pHWDE->jChipID >= XG40 )
    	    XGINew_SetReg1( pVBInfo->P3d4 , 0x57 , 0x4A ) ;	/* write sense pattern 30->4a */
    	else
            XGINew_SetReg1( pVBInfo->P3d4 , 0x57 , 0x5F ) ;	/* write sense pattern */

        XGINew_SetRegANDOR( pVBInfo->P3d4 , 0x53 , 0xFF , 0x02 ) ;	/* enable sense DAC */
        XGI_WaitDisply(pVBInfo) ;

        if(XGINew_GetReg2( pVBInfo->P3c2 ) & 0x10 )
            bConnectStatus |= Monitor1Sense ;

        XGINew_SetRegAND( pVBInfo->P3d4 , 0x53 , 0xFD ) ;	/* disable sense DAC */
        XGINew_SetRegAND( pVBInfo->P3d4 , 0x57 , 0x00 ) ;	/* clear sense pattern */


        /* ---------- End of dynamic sense CRT1 ----------- */

        /* ---------- beginning of dynamic sense VB ------------ */
        pre_SR1E = XGINew_GetReg1( pVBInfo->P3c4 , 0x1E ) ;
        XGINew_SetRegOR( pVBInfo->P3c4 , 0x1E , 0x20 ) ;	/* Enable CRT2,work-a-round for 301B/301LV/302LV */
        pre_Part2_0 = XGINew_GetReg1( pVBInfo->Part2Port , 0x00 ) ;
        pre_Part4_D = XGINew_GetReg1( pVBInfo->Part4Port , 0x0D ) ;

        if ( XGI_Is301C( pVBInfo ) )	/* 301C only */
            XGINew_SetRegANDOR( pVBInfo->Part4Port , 0x0D , ~0x07 , 0x01 ) ;	/* Set Part4 0x0D D[2:0] to 001b */

        /* tempax = 0 ; */
        if ( !XGI_Is301LV( pVBInfo ) )
        {
           tempbx = XGI330_RGBSenseData2 ;
            tempcx = 0x0E08 ;
            if(XGINew_Sense( tempbx , tempcx, pVBInfo ) )
            {
                bConnectStatus |= Monitor2Sense ;
                if ( OutputSelect & SetSCARTOutput )
                {
                    bConnectStatus ^= ( Monitor2Sense | SCARTSense ) ;
                }
            }
        }
        if ( XGI_Is301C( pVBInfo ) )	/* 301C only */
            XGINew_SetRegOR( pVBInfo->Part4Port , 0x0D , 0x04 ) ;	/* Set Part4 0x0D D[2]=1 for dynamic sense */

        if ( ( XGINew_Is301B( pVBInfo ) ) )
            XGINew_SetRegOR( pVBInfo->Part2Port , 0x00 , 0x0C ) ;    /* ????????? */

	if ( XGINew_SenseHiTV( HwDeviceExtension , pVBInfo) )		/* add by kuku for Dysense HiTV //start */
	{
	    bConnectStatus|= YPbPrSense ;
	}
	else
	{
        tempbx = XGI330_YCSenseData2 ;	/* Y/C Sense Data Ptr */
        tempcx = 0x0604 ;
        if ( XGINew_Sense( tempbx , tempcx , pVBInfo) )
            bConnectStatus |= SVIDEOSense ;

        if ( OutputSelect & BoardTVType )
        {
            tempbx = XGI330_VideoSenseData2 ;
            tempcx = 0x0804 ;
            if ( XGINew_Sense(tempbx , tempcx, pVBInfo) )
                bConnectStatus|= AVIDEOSense ;
        }
        else
        {
            if ( !( bConnectStatus & SVIDEOSense ) )
            {
                tempbx = XGI330_VideoSenseData2 ;
                tempcx = 0x0804 ;
                if ( XGINew_Sense( tempbx , tempcx, pVBInfo ) )
                    bConnectStatus |= AVIDEOSense ;
            }
        }
    	} /* end */
        /* DySenseVBCnt */

        tempbx = 0 ;
        tempcx = 0 ;
        XGINew_Sense(tempbx , tempcx, pVBInfo ) ;

        if ( !( bConnectStatus & Monitor2Sense ) )
        {
            if ( XGI_SenseLCD( pHWDE , pVBInfo ) )
                bConnectStatus |= LCDSense ;
        }

        XGINew_SetRegANDOR( pVBInfo->P3d4 , 0x32 , ~( AVIDEOSense | SVIDEOSense | LCDSense | Monitor2Sense | Monitor1Sense ) , bConnectStatus ) ;

        XGINew_SetReg1( pVBInfo->Part4Port , 0x0D , pre_Part4_D ) ;
        XGINew_SetReg1( pVBInfo->Part2Port , 0x00 , pre_Part2_0 ) ;
        XGINew_SetReg1( pVBInfo->P3c4 , 0x1E , pre_SR1E ) ;

        if ( XGI_Is301C( pVBInfo ) )	/* 301C only */
        {
            tempax = XGINew_GetReg1( pVBInfo->Part2Port , 0x00 ) ;
            if ( tempax & 0x20 )
            {
                /* Reset VBPro */
                for( tempcx = 2 ; tempcx > 0 ; tempcx-- )
                {
                    tempax ^= 0x20 ;
                    XGINew_SetReg1( pVBInfo->Part2Port , 0x00 , tempax ) ;
                }
            }
        }
        /* End of dynamic sense VB */
    }
    else
    {
        XGI_SenseCRT1(pVBInfo) ;
        XGI_GetSenseStatus( HwDeviceExtension, pVBInfo ) ;	/* sense CRT2 */
        bConnectStatus = XGINew_GetReg1( pVBInfo->P3d4 , 0x32 ) ;
    }
    temp = pushax & 0x00FF ;		/* 0512 Fix Dysense hanged */
    XGINew_SetReg1( pVBInfo->P3d4 , 0x17 , temp ) ;
    if ( bConnectStatus )
    {
        *ujConnectStatus = bConnectStatus ;
        return( 1 ) ;
    }
    else
        return( 0 ) ;
}

#endif 

/* --------------------------------------------------------------------- */
/* Function : XGISetDPMS */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
VOID XGISetDPMS( PXGI_HW_DEVICE_INFO pXGIHWDE , ULONG VESA_POWER_STATE )
{
    USHORT ModeNo, ModeIdIndex ;
    UCHAR  temp ;
    VB_DEVICE_INFO VBINF;
    PVB_DEVICE_INFO pVBInfo = &VBINF;
    pVBInfo->BaseAddr = ( USHORT )pXGIHWDE->pjIOAddress ;
    pVBInfo->ROMAddr  = pXGIHWDE->pjVirtualRomBase ;

    if ( ( pXGIHWDE->ujVBChipID == VB_CHIP_301 ) || ( pXGIHWDE->ujVBChipID == VB_CHIP_302 ) )
    {
        pVBInfo->IF_DEF_LVDS = 0 ;
        pVBInfo->IF_DEF_CH7005 = 0 ;
        pVBInfo->IF_DEF_HiVision = 1 ;
        pVBInfo->IF_DEF_LCDA = 1 ;
        pVBInfo->IF_DEF_CH7017 = 0 ;
        pVBInfo->IF_DEF_YPbPr = 1 ;
        pVBInfo->IF_DEF_CRT2Monitor = 0 ;
        pVBInfo->IF_DEF_VideoCapture = 0 ;
        pVBInfo->IF_DEF_ScaleLCD = 0 ;
        pVBInfo->IF_DEF_OEMUtil = 0 ;
        pVBInfo->IF_DEF_PWD = 0 ;

        XGI_InitTo330Pointer( pXGIHWDE->jChipType,  pVBInfo ) ;
   	XGI_ReadVBIOSTablData( pXGIHWDE->jChipType , pVBInfo) ;

        pVBInfo->P3c4 = pVBInfo->BaseAddr + 0x14 ;
        pVBInfo->P3d4 = pVBInfo->BaseAddr + 0x24 ;
        pVBInfo->P3c0 = pVBInfo->BaseAddr + 0x10 ;
        pVBInfo->P3ce = pVBInfo->BaseAddr + 0x1e ;
        pVBInfo->P3c2 = pVBInfo->BaseAddr + 0x12 ;
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

        XGINew_SetModeScratch ( pXGIHWDE , pVBInfo ) ;

        XGINew_SetReg1( pVBInfo->P3c4 , 0x05 , 0x86 ) ;	/* 1.Openkey */
        XGI_UnLockCRT2( pXGIHWDE , pVBInfo) ;
        ModeNo = XGINew_GetReg1( pVBInfo->P3d4 , 0x34 ) ;
        XGI_SearchModeID( ModeNo , &ModeIdIndex, pVBInfo ) ;
        XGI_GetVGAType( pXGIHWDE , pVBInfo ) ;
        XGI_GetVBType( pVBInfo ) ;
        XGI_GetVBInfo( ModeNo , ModeIdIndex , pXGIHWDE, pVBInfo ) ;
        XGI_GetTVInfo( ModeNo , ModeIdIndex, pVBInfo ) ;
        XGI_GetLCDInfo( ModeNo , ModeIdIndex, pVBInfo ) ;
    }

    if ( VESA_POWER_STATE == 0x00000400 )
      XGINew_SetReg1( pVBInfo->Part4Port , 0x31 , ( UCHAR )( XGINew_GetReg1( pVBInfo->Part4Port , 0x31 ) & 0xFE ) ) ;
    else
      XGINew_SetReg1( pVBInfo->Part4Port , 0x31 , ( UCHAR )( XGINew_GetReg1( pVBInfo->Part4Port , 0x31 ) | 0x01 ) ) ;

    temp = ( UCHAR )XGINew_GetReg1( pVBInfo->P3c4 , 0x1f ) ;
    temp &= 0x3f ;
    switch ( VESA_POWER_STATE )
    {
        case 0x00000000:
            if ( ( pXGIHWDE->ujVBChipID == VB_CHIP_301 ) || ( pXGIHWDE->ujVBChipID == VB_CHIP_302 ) )
            {
                XGINew_SetReg1( pVBInfo->P3c4 , 0x1f , ( UCHAR )( temp | 0x00 ) ) ;
                XGI_EnableBridge( pXGIHWDE, pVBInfo ) ;
            }
            else
            {
                temp =  ( UCHAR )XGINew_GetReg1( pVBInfo->P3c4 , 0x01 ) ;
                temp &= 0xbf ;
                XGINew_SetReg1( pVBInfo->P3c4 , 0x1f , temp | 0x00 ) ;
            }
            break ;
        case 0x00000100:
            XGINew_SetReg1( pVBInfo->P3c4 , 0x1f , ( UCHAR )( temp | 0x40 ) ) ;
            break ;
        case 0x00000200:
            XGINew_SetReg1( pVBInfo->P3c4 , 0x1f , ( UCHAR )( temp | 0x80 ) ) ;
            break ;
        case 0x00000400:
            if ( (pXGIHWDE->ujVBChipID == VB_CHIP_301 ) || ( pXGIHWDE->ujVBChipID == VB_CHIP_302 ) )
            {
                XGINew_SetReg1( pVBInfo->P3c4 , 0x1f , ( UCHAR )( temp | 0xc0 ) ) ;
                XGI_DisableBridge( pXGIHWDE, pVBInfo ) ;
            }
            else
            {
                temp = ( UCHAR )XGINew_GetReg1( pVBInfo->P3c4 , 0x01 ) ;
                temp &= 0xbf ;
                XGINew_SetReg1( pVBInfo->P3c4 , 0x1f , temp | 0x20 ) ;
            }
            break ;

        default:
            break ;
    }
    XGI_LockCRT2( pXGIHWDE , pVBInfo ) ;
}



/* --------------------------------------------------------------------- */
/* Function : XGI_GetSenseStatus */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGI_GetSenseStatus( PXGI_HW_DEVICE_INFO HwDeviceExtension , PVB_DEVICE_INFO pVBInfo)
{
    USHORT tempax = 0 , tempbx , tempcx , temp ,
           P2reg0 = 0 , SenseModeNo = 0 , OutputSelect = *pVBInfo->pOutputSelect ,
           ModeIdIndex , i ;
    pVBInfo->BaseAddr = ( USHORT )HwDeviceExtension->pjIOAddress ;

    if ( pVBInfo->IF_DEF_LVDS == 1 )
    {
        tempax = XGINew_GetReg1( pVBInfo->P3c4 , 0x1A ) ;	/* ynlai 02/27/2002 */
        tempbx = XGINew_GetReg1( pVBInfo->P3c4 , 0x1B ) ;
        tempax = ( ( tempax & 0xFE ) >> 1 ) | ( tempbx << 8 ) ;
        if ( tempax == 0x00 )
        {		/* Get Panel id from DDC */
            temp = XGINew_GetLCDDDCInfo( HwDeviceExtension, pVBInfo ) ;
            if ( temp == 1 )
            {		/* LCD connect */
                XGINew_SetRegANDOR( pVBInfo->P3d4 , 0x39 , 0xFF , 0x01 ) ;	/* set CR39 bit0="1" */
                XGINew_SetRegANDOR( pVBInfo->P3d4 , 0x37 , 0xEF , 0x00 ) ;	/* clean CR37 bit4="0" */
                temp = LCDSense ;
            }
            else
            {		/* LCD don't connect */
                temp = 0 ;
            }
        }
        else
        {
            XGINew_GetPanelID(pVBInfo) ;
            temp = LCDSense ;
        }

        tempbx = ~( LCDSense | AVIDEOSense | SVIDEOSense ) ;
        XGINew_SetRegANDOR( pVBInfo->P3d4 , 0x32 , tempbx , temp ) ;
    }
    else
    {		/* for 301 */
        if ( pVBInfo->VBInfo & SetCRT2ToHiVisionTV )
        {	/* for HiVision */
            tempax = XGINew_GetReg1( pVBInfo->P3c4 , 0x38 ) ;
            temp = tempax & 0x01 ;
            tempax = XGINew_GetReg1( pVBInfo->P3c4 , 0x3A ) ;
            temp = temp | ( tempax & 0x02 ) ;
            XGINew_SetRegANDOR( pVBInfo->P3d4 , 0x32 , 0xA0 , temp ) ;
        }
        else
        {
            if ( XGI_BridgeIsOn( pVBInfo ) )
            {
                P2reg0 = XGINew_GetReg1( pVBInfo->Part2Port , 0x00 ) ;
                if ( !XGINew_BridgeIsEnable( HwDeviceExtension, pVBInfo ) )
                {
                    SenseModeNo = 0x2e ;
                    /* XGINew_SetReg1( pVBInfo->P3d4 , 0x30 , 0x41 ) ; */
                    /* XGISetModeNew( HwDeviceExtension , 0x2e ) ; // ynlai InitMode */

                    temp = XGI_SearchModeID( SenseModeNo , &ModeIdIndex, pVBInfo ) ;
                    XGI_GetVGAType( HwDeviceExtension , pVBInfo) ;
                    XGI_GetVBType( pVBInfo ) ;
                    pVBInfo->SetFlag = 0x00 ;
                    pVBInfo->ModeType = ModeVGA ;
                    pVBInfo->VBInfo = SetCRT2ToRAMDAC | LoadDACFlag | SetInSlaveMode ;
                    XGI_GetLCDInfo( 0x2e , ModeIdIndex, pVBInfo ) ;
                    XGI_GetTVInfo(  0x2e , ModeIdIndex, pVBInfo ) ;
                    XGI_EnableBridge( HwDeviceExtension, pVBInfo ) ;
                    XGI_SetCRT2Group301( SenseModeNo , HwDeviceExtension, pVBInfo ) ;
                    XGI_SetCRT2ModeRegs( 0x2e , HwDeviceExtension, pVBInfo ) ;
                    /* XGI_DisableBridge( HwDeviceExtension, pVBInfo ) ; */
                    XGINew_SetRegANDOR( pVBInfo->P3c4 , 0x01 , 0xDF , 0x20 ) ;	/* Display Off 0212 */
                    for( i = 0 ; i < 20 ; i++ )
                    {
                        XGI_LongWait(pVBInfo) ;
                    }
                }
                XGINew_SetReg1( pVBInfo->Part2Port , 0x00 , 0x1c ) ;
                tempax = 0 ;
                tempbx = *pVBInfo->pRGBSenseData ;

                if ( !( XGINew_Is301B( pVBInfo ) ) )
                {
                    tempbx = *pVBInfo->pRGBSenseData2 ;
                }

                tempcx = 0x0E08 ;
                if ( XGINew_Sense(tempbx , tempcx, pVBInfo ) )
                {
                    if ( XGINew_Sense(tempbx , tempcx, pVBInfo ) )
                    {
                        tempax |= Monitor2Sense ;
                    }
                }

                if ( pVBInfo->VBType & VB_XGI301C)
                {
                    XGINew_SetRegOR( pVBInfo->Part4Port , 0x0d , 0x04 ) ;
                }

		if ( XGINew_SenseHiTV( HwDeviceExtension , pVBInfo) )		/* add by kuku for Multi-adapter sense HiTV */
		{
		    tempax |= HiTVSense ;
                    if ( ( pVBInfo->VBType & VB_XGI301C ) )
                    {
                	tempax ^= ( HiTVSense | YPbPrSense ) ;
                    }
                }

		if ( !( tempax & ( HiTVSense | YPbPrSense ) ) )		/* start */
                {

                tempbx = *pVBInfo->pYCSenseData ;

                if ( !( XGINew_Is301B(  pVBInfo ) ) )
                {
                    tempbx=*pVBInfo->pYCSenseData2;
                }

                tempcx = 0x0604 ;
                if ( XGINew_Sense(tempbx , tempcx, pVBInfo ) )
                {
                    if ( XGINew_Sense(tempbx , tempcx, pVBInfo ) )
                    {
                        tempax |= SVIDEOSense ;
                    }
                }

                if ( OutputSelect & BoardTVType )
                {
                    tempbx = *pVBInfo->pVideoSenseData ;

                    if ( !( XGINew_Is301B( pVBInfo ) ) )
                    {
                        tempbx = *pVBInfo->pVideoSenseData2 ;
                    }

                    tempcx = 0x0804 ;
                    if ( XGINew_Sense(tempbx , tempcx, pVBInfo ) )
                    {
                        if ( XGINew_Sense(tempbx , tempcx, pVBInfo ) )
                        {
                            tempax |= AVIDEOSense ;
                        }
                    }
                }
                else
                {
                    if ( !( tempax & SVIDEOSense ) )
                    {
                        tempbx = *pVBInfo->pVideoSenseData ;

                        if ( !( XGINew_Is301B( pVBInfo ) ) )
                        {
                            tempbx=*pVBInfo->pVideoSenseData2;
                        }

                        tempcx = 0x0804 ;
                        if ( XGINew_Sense(tempbx , tempcx, pVBInfo ) )
                        {
                            if ( XGINew_Sense(tempbx , tempcx, pVBInfo ) )
                            {
                                tempax |= AVIDEOSense ;
                            }
                        }
                    }
                }
            }
    	    } /* end */
            if ( !( tempax & Monitor2Sense ) )
            {
                if ( XGINew_SenseLCD( HwDeviceExtension, pVBInfo ) )
                {
                    tempax |= LCDSense ;
                }
            }
            tempbx = 0 ;
            tempcx = 0 ;
            XGINew_Sense(tempbx , tempcx, pVBInfo ) ;

            XGINew_SetRegANDOR( pVBInfo->P3d4 , 0x32 , ~0xDF , tempax ) ;
            XGINew_SetReg1( pVBInfo->Part2Port , 0x00 , P2reg0 ) ;

            if ( !( P2reg0 & 0x20 ) )
            {
                pVBInfo->VBInfo = DisableCRT2Display ;
                /* XGI_SetCRT2Group301( SenseModeNo , HwDeviceExtension, pVBInfo ) ; */
            }
        }
    }
    XGI_DisableBridge( HwDeviceExtension, pVBInfo ) ;		/* shampoo 0226 */
}



/* --------------------------------------------------------------------- */
/* Function : XGINew_SenseLCD */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
USHORT XGINew_SenseLCD( PXGI_HW_DEVICE_INFO HwDeviceExtension ,PVB_DEVICE_INFO pVBInfo)
{
    /* USHORT SoftSetting ; */
    USHORT temp ;

    if ( ( HwDeviceExtension->jChipType==XG20 ) || ( HwDeviceExtension->jChipType >= XG40 ) )
        temp = 0 ;
    else
        temp=XGINew_GetPanelID(pVBInfo) ;

    if( !temp )
        temp = XGINew_GetLCDDDCInfo( HwDeviceExtension, pVBInfo ) ;

    return( temp ) ;
}


/* --------------------------------------------------------------------- */
/* Function : XGINew_GetLCDDDCInfo */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
BOOLEAN XGINew_GetLCDDDCInfo( PXGI_HW_DEVICE_INFO HwDeviceExtension,PVB_DEVICE_INFO pVBInfo)
{
    USHORT temp ;

    /* add lcd sense */
    if ( HwDeviceExtension->ulCRT2LCDType == LCD_UNKNOWN )
    {
        return( 0 ) ;
    }
    else
    {
        temp = ( USHORT )HwDeviceExtension->ulCRT2LCDType ;
        switch( HwDeviceExtension->ulCRT2LCDType )
        {
            case LCD_INVALID:
            case LCD_800x600:
            case LCD_1024x768:
            case LCD_1280x1024:
                break ;

            case LCD_640x480:
            case LCD_1024x600:
            case LCD_1152x864:
            case LCD_1280x960:
            case LCD_1152x768:
                temp = 0 ;
                break ;

            case LCD_1400x1050:
            case LCD_1280x768:
            case LCD_1600x1200:
                break ;

            case LCD_1920x1440:
            case LCD_2048x1536:
                temp = 0 ;
                break ;

            default:
                break ;
        }
        XGINew_SetRegANDOR( pVBInfo->P3d4 , 0x36 , 0xF0 , temp ) ;
        return( 1 ) ;
    }
}


/* --------------------------------------------------------------------- */
/* Function : */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
BOOLEAN XGINew_GetPanelID(PVB_DEVICE_INFO pVBInfo )
{
    USHORT PanelTypeTable[ 16 ] = { SyncNN | PanelRGB18Bit | Panel800x600  | _PanelType00 ,
                                    SyncNN | PanelRGB18Bit | Panel1024x768 | _PanelType01 ,
                                    SyncNN | PanelRGB18Bit | Panel800x600  | _PanelType02 ,
                                    SyncNN | PanelRGB18Bit | Panel640x480  | _PanelType03 ,
                                    SyncNN | PanelRGB18Bit | Panel1024x768 | _PanelType04 ,
                                    SyncNN | PanelRGB18Bit | Panel1024x768 | _PanelType05 ,
                                    SyncNN | PanelRGB18Bit | Panel1024x768 | _PanelType06 ,
                                    SyncNN | PanelRGB24Bit | Panel1024x768 | _PanelType07 ,
                                    SyncNN | PanelRGB18Bit | Panel800x600  | _PanelType08 ,
                                    SyncNN | PanelRGB18Bit | Panel1024x768 | _PanelType09 ,
                                    SyncNN | PanelRGB18Bit | Panel800x600  | _PanelType0A ,
                                    SyncNN | PanelRGB18Bit | Panel1024x768 | _PanelType0B ,
                                    SyncNN | PanelRGB18Bit | Panel1024x768 | _PanelType0C ,
                                    SyncNN | PanelRGB24Bit | Panel1024x768 | _PanelType0D ,
                                    SyncNN | PanelRGB18Bit | Panel1024x768 | _PanelType0E ,
                                    SyncNN | PanelRGB18Bit | Panel1024x768 | _PanelType0F } ;
    USHORT tempax , tempbx , temp ;
    /* USHORT return_flag ; */

    tempax = XGINew_GetReg1( pVBInfo->P3c4 , 0x1A ) ;
    tempbx = tempax & 0x1E ;

    if ( tempax == 0 )
        return( 0 ) ;
    else
    {
/*
        if ( !( tempax & 0x10 ) )
        {
            if ( pVBInfo->IF_DEF_LVDS == 1 )
            {
                tempbx = 0 ;
                temp = XGINew_GetReg1( pVBInfo->P3c4 , 0x38 ) ;
                if ( temp & 0x40 )
                    tempbx |= 0x08 ;
                if ( temp & 0x20 )
                    tempbx |= 0x02 ;
                if ( temp & 0x01 )
                    tempbx |= 0x01 ;

                temp = XGINew_GetReg1( pVBInfo->P3c4 , 0x39 ) ;
                if ( temp & 0x80 )
                    tempbx |= 0x04 ;
            }
            else
            {
                return( 0 ) ;
            }
        }
*/

        tempbx = tempbx >> 1 ;
        temp = tempbx & 0x00F ;
        XGINew_SetReg1( pVBInfo->P3d4 , 0x36 , temp ) ;
        tempbx-- ;
        tempbx = PanelTypeTable[ tempbx ] ;

        temp = ( tempbx & 0xFF00 ) >> 8 ;
        XGINew_SetRegANDOR( pVBInfo->P3d4 , 0x37 , ~( LCDSyncBit | LCDRGB18Bit ) , temp ) ;
        return( 1 ) ;
    }
}


/* --------------------------------------------------------------------- */
/* Function : XGINew_BridgeIsEnable */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
BOOLEAN XGINew_BridgeIsEnable( PXGI_HW_DEVICE_INFO HwDeviceExtension ,PVB_DEVICE_INFO pVBInfo)
{
    USHORT flag ;

    if ( XGI_BridgeIsOn( pVBInfo ) == 0 )
    {
        flag = XGINew_GetReg1( pVBInfo->Part1Port , 0x0 ) ;

        if ( flag & 0x050 )
        {
            return( 1 ) ;
        }
        else
        {
            return( 0 ) ;
        }

    }
    return( 0 ) ;
}

/* ------------------------------------------------------ */
/* Function : XGINew_SenseHiTV */
/* Input : */
/* Output : */
/* Description : */
/* ------------------------------------------------------ */
BOOLEAN XGINew_SenseHiTV( PXGI_HW_DEVICE_INFO HwDeviceExtension , PVB_DEVICE_INFO pVBInfo )
{
    USHORT  tempbx , tempcx , temp , i , tempch;

    tempbx = *pVBInfo->pYCSenseData2 ;

    tempcx = 0x0604 ;

    temp = tempbx & 0xFF ;
    XGINew_SetReg1( pVBInfo->Part4Port , 0x11 , temp ) ;
    temp = ( tempbx & 0xFF00 ) >> 8 ;
    temp |= ( tempcx & 0x00FF ) ;
    XGINew_SetRegANDOR( pVBInfo->Part4Port , 0x10 , ~0x1F , temp ) ;

    for( i = 0 ; i < 10 ; i++ )
        XGI_LongWait(pVBInfo) ;

    tempch = ( tempcx & 0xFF00 ) >> 8;
    temp = XGINew_GetReg1( pVBInfo->Part4Port , 0x03 ) ;
    temp = temp ^ ( 0x0E ) ;
    temp &= tempch ;

    if ( temp !=  tempch )
        return( 0 ) ;

    tempbx = *pVBInfo->pVideoSenseData2 ;

    tempcx = 0x0804 ;
    temp = tempbx & 0xFF ;
    XGINew_SetReg1( pVBInfo->Part4Port , 0x11 , temp ) ;
    temp = ( tempbx & 0xFF00 ) >> 8 ;
    temp |= ( tempcx & 0x00FF ) ;
    XGINew_SetRegANDOR( pVBInfo->Part4Port , 0x10 , ~0x1F , temp ) ;

    for( i = 0 ; i < 10 ; i++ )
        XGI_LongWait(pVBInfo) ;

    tempch = ( tempcx & 0xFF00 ) >> 8;
    temp = XGINew_GetReg1( pVBInfo->Part4Port , 0x03 ) ;
    temp = temp ^ ( 0x0E ) ;
    temp &= tempch ;

    if ( temp !=  tempch )
        return( 0 ) ;
    else
    {
      tempbx = 0x3FF ;
      tempcx = 0x0804 ;
      temp = tempbx & 0xFF ;
      XGINew_SetReg1( pVBInfo->Part4Port , 0x11 , temp ) ;
      temp = ( tempbx & 0xFF00 ) >> 8 ;
      temp |= ( tempcx & 0x00FF ) ;
      XGINew_SetRegANDOR( pVBInfo->Part4Port , 0x10 , ~0x1F , temp ) ;

      for( i = 0 ; i < 10 ; i++ )
          XGI_LongWait(pVBInfo) ;

      tempch = ( tempcx & 0xFF00 ) >> 8;
      temp = XGINew_GetReg1( pVBInfo->Part4Port , 0x03 ) ;
      temp = temp ^ ( 0x0E ) ;
      temp &= tempch ;

      if ( temp != tempch )
          return( 1 ) ;
      else
	  return( 0 ) ;
    }
}
