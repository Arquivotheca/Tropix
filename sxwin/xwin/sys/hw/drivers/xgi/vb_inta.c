/* $XFree86: xc/programs/Xserver/hw/xfree86/drivers/xgi/vb_inta.c,v 1.1 2005/05/02 13:28:02 dawes Exp $ */

/* #ifdef WIN2000 // INTA solution for WIN2000 */

#include <dderror.h>
#include <devioctl.h>
#include <miniport.h>
#include <ntddvdeo.h>
#include <video.h>

#include "osdef.h"
#include "vb_def.h"
#include "vgatypes.h"
#include "vb_struct.h"

#include "xgiv.h"
#include "dd_i2c.h"
#include "tools.h"

#include "vb_util.h"
#include "vb_setmode.h"

void    SiSLVDSCH7019ControlCallback(PHW_DEVICE_EXTENSION pHWDE,PVOID Context);
void    SiSTVCH7019ControlCallback(PHW_DEVICE_EXTENSION pHWDE, PVOID Context);
void    XGINew_ExternalToBackup(PHW_DEVICE_EXTENSION pHWDE);
void    XGINew_SetReadExternal(PHW_DEVICE_EXTENSION pHWDE);
void    XGINew_SetReadInternal(PHW_DEVICE_EXTENSION pHWDE);
void    XGINew_DisableWriteBackup(PHW_DEVICE_EXTENSION pHWDE);
void    XGINew_EnableWriteBackup(PHW_DEVICE_EXTENSION pHWDE);
void    XGINew_TurnOnBacklight(PHW_DEVICE_EXTENSION   pHWDE);
int     XGINew_WantToSetMode(PHW_DEVICE_EXTENSION pHWDE);
int     XGINew_WantToChangeTimingRegister(PHW_DEVICE_EXTENSION pHWDE);
int     XGINew_ReSet301VBTiming(PHW_DEVICE_EXTENSION   pHWDE, PVB_DEVICE_INFO );
void    XGINew_TurnOffBacklight(PHW_DEVICE_EXTENSION   pHWDE);
void    XGINew_EnableWriteInternal(PHW_DEVICE_EXTENSION pHWDE);
void    XGINew_DisableWriteInternal(PHW_DEVICE_EXTENSION pHWDE);
void    XGINew_EnableWriteExternal(PHW_DEVICE_EXTENSION pHWDE);
void    XGINew_DisableWriteExternal(PHW_DEVICE_EXTENSION pHWDE);
void    XGINew_GRandAR13Changed(PHW_DEVICE_EXTENSION   pHWDE);
BOOLEAN XGINew_IsVAMode(PVB_DEVICE_INFO pVBInfo  );
BOOLEAN XGINew_IsDualEdge(PVB_DEVICE_INFO pVBInfo);


/* --------------------------------------------------------------------- */
/* Function : bVBIOSCanWorkWithINTA */
/* Input : */
/* Output : */
/* Description : add INTA 03/18/03 */
/* --------------------------------------------------------------------- */
BOOLEAN bVBIOSCanWorkWithINTA(PXGI_HW_DEVICE_INFO pXGIHWDE)
{
    PUCHAR      XGI_P3d4;
    UCHAR       bCR40,bCR41,bCR7D,bCR7E,bCR3A;
    UCHAR      bSaved3D4;
    UCHAR       temp;

    XGI_P3d4 = (PUCHAR) (pXGIHWDE->pjIOAddress);
    XGI_P3d4 += CRTC_ADDRESS_PORT_COLOR;

    bSaved3D4 = VideoPortReadPortUchar(XGI_P3d4);

    if (pXGIHWDE->jChipType >= XG40)
    {
      VideoPortWritePortUchar(XGI_P3d4,0x7D);
      bCR7D = VideoPortReadPortUchar(XGI_P3d4+1);
      VideoPortWritePortUchar(XGI_P3d4,0x7E);
      bCR7E = VideoPortReadPortUchar(XGI_P3d4+1);
    }
    else
    {
      VideoPortWritePortUchar(XGI_P3d4,0x40);
      bCR40 = VideoPortReadPortUchar(XGI_P3d4+1);
      VideoPortWritePortUchar(XGI_P3d4,0x41);
      bCR41 = VideoPortReadPortUchar(XGI_P3d4+1);
    }
    VideoPortWritePortUchar(XGI_P3d4,0x3A);
    bCR3A = VideoPortReadPortUchar(XGI_P3d4+1);

    VideoPortWritePortUchar(XGI_P3d4, bSaved3D4);

    if(pXGIHWDE->jChipType == SIS_330)
      return FALSE;
    if(bCR3A & 0x20)	/* SMI??? */
      return FALSE;

    if (pXGIHWDE->jChipType >= XG40)
    {
      if ((bCR7D==0xFE)&&((bCR7E&0x0F)==0x0F))
      {
        return TRUE;
      }
      else
      {
        return FALSE;
      }
    }

    if ((bCR40==0xFE)&&((bCR41&0xF0)==0xF0))
    {    /* close CRTC */
        if((pXGIHWDE->jChipType == SIS_660)||(pXGIHWDE->jChipType == SIS_661)||(pXGIHWDE->jChipType == SIS_650M)||(pXGIHWDE->jChipType == SIS_760))
        {
    	    VideoPortWritePortUchar(XGI_P3d4,0x51);
    	    temp = VideoPortReadPortUchar(XGI_P3d4+1);
    	    temp = (temp & (~0xE0)|0xA0);
    	    VideoPortWritePortUchar(XGI_P3d4+1,temp); /* set write permitted */

            VideoPortWritePortUchar(XGI_P3d4,0x56);
    	    temp = VideoPortReadPortUchar(XGI_P3d4+1);
    	    temp = (temp & (~0x18)|0x08);
    	    VideoPortWritePortUchar(XGI_P3d4+1,temp); /* set read permitted */

        }
        return TRUE;
    }
    else
      return FALSE;
}

/* --------------------------------------------------------------------- */
/* Function : DisableTimingRegWriteProtectINTA */
/* Input : */
/* Output : */
/* Description : add INTA 03/18/03 */
/* --------------------------------------------------------------------- */
VOID DisableTimingRegWriteProtectINTA(PHW_DEVICE_EXTENSION   pHWDE)
{
    PUCHAR      XGI_P3d4;
    UCHAR       bCR51,bCR56,bCR61;
    UCHAR      bSaved3D4;

    XGI_P3d4 = pHWDE->pjIOAddress + CRTC_ADDRESS_PORT_COLOR;

    bSaved3D4 = VideoPortReadPortUchar(XGI_P3d4);

    VideoPortWritePortUchar(XGI_P3d4,0x61);
    bCR61 = VideoPortReadPortUchar(XGI_P3d4+1);
    VideoPortWritePortUchar(XGI_P3d4+1,(UCHAR)(bCR61&0xF7));

    VideoPortWritePortUchar(XGI_P3d4,0x56);
    bCR56 = VideoPortReadPortUchar(XGI_P3d4+1);
    VideoPortWritePortUchar(XGI_P3d4+1,(UCHAR)(bCR56&0xE7));

    VideoPortWritePortUchar(XGI_P3d4,0x51);
    bCR51 = VideoPortReadPortUchar(XGI_P3d4+1);
    VideoPortWritePortUchar(XGI_P3d4+1,(UCHAR)(bCR51&0x1F));

    VideoPortWritePortUchar(XGI_P3d4, bSaved3D4);
}

/* --------------------------------------------------------------------- */
/* Function : EnableTimingRegWriteProtectINTA */
/* Input : */
/* Output : */
/* Description : add INTA 03/18/03 */
/* --------------------------------------------------------------------- */
VOID EnableTimingRegWriteProtectINTA(PHW_DEVICE_EXTENSION   pHWDE)
{

    PUCHAR      XGI_P3d4;
    UCHAR       bCR40,bCR41,bCR7D,bCR7E;
    UCHAR       bCR51,bCR56,bCR61;
    UCHAR      bSaved3D4;

    XGI_P3d4 = pHWDE->pjIOAddress + CRTC_ADDRESS_PORT_COLOR;

    bSaved3D4 = VideoPortReadPortUchar(XGI_P3d4);

    if (pHWDE->jChipID >= XG40)
    {
      VideoPortWritePortUchar(XGI_P3d4,0x7D);
      bCR7D = VideoPortReadPortUchar(XGI_P3d4+1);
      VideoPortWritePortUchar(XGI_P3d4,0x7E);
      bCR7E = VideoPortReadPortUchar(XGI_P3d4+1);

      if ((bCR7D==0x00)&&((bCR7E&0x0F)==0x00))
      {
        VideoPortWritePortUchar(XGI_P3d4,0x7D);
        VideoPortWritePortUchar(XGI_P3d4+1,0xFF);
        VideoPortWritePortUchar(XGI_P3d4,0x7E);
        VideoPortWritePortUchar(XGI_P3d4+1,(UCHAR)(VideoPortReadPortUchar(XGI_P3d4+1)|0x0F));
      }
    }
    else
    {
      VideoPortWritePortUchar(XGI_P3d4,0x40);
      bCR40 = VideoPortReadPortUchar(XGI_P3d4+1);
      VideoPortWritePortUchar(XGI_P3d4,0x41);
      bCR41 = VideoPortReadPortUchar(XGI_P3d4+1);

      if ((bCR40==0x00)&&((bCR41&0xF0)==0x00))
      {
        VideoPortWritePortUchar(XGI_P3d4,0x40);
        VideoPortWritePortUchar(XGI_P3d4+1,0xFF);
        VideoPortWritePortUchar(XGI_P3d4,0x41);
        VideoPortWritePortUchar(XGI_P3d4+1,(UCHAR)(VideoPortReadPortUchar(XGI_P3d4+1)|0xF0));
      }
    }


    VideoPortWritePortUchar(XGI_P3d4,0x61);
    bCR61 = VideoPortReadPortUchar(XGI_P3d4+1);
    VideoPortWritePortUchar(XGI_P3d4+1,(UCHAR)(bCR61|0x08));

    VideoPortWritePortUchar(XGI_P3d4,0x56);
    bCR56 = VideoPortReadPortUchar(XGI_P3d4+1);
    VideoPortWritePortUchar(XGI_P3d4+1,(UCHAR)((bCR56&0xE7)|0x8));

    VideoPortWritePortUchar(XGI_P3d4,0x51);
    bCR51 = VideoPortReadPortUchar(XGI_P3d4+1);
    VideoPortWritePortUchar(XGI_P3d4+1,(UCHAR)((bCR51&0x1F)|0xA0));

    VideoPortWritePortUchar(XGI_P3d4, bSaved3D4);
}

/* --------------------------------------------------------------------- */
/* Function : vModifyLCDTiming301LVDS_INTANew */
/* Input : */
/* Output : */
/* Description : add INTA 03/18/03 bainy */
/* --------------------------------------------------------------------- */
VOID vModifyLCDTiming301LVDS_INTANew( PHW_DEVICE_EXTENSION pHWDE )
{
    PUCHAR XGINew_P3c4 , XGINew_P3d4 ;
    UCHAR bSaved3C4 , bSaved3D4,temp ;
    VB_DEVICE_INFO VBINF;
    PVB_DEVICE_INFO pVBInfo = &VBINF;
    XGINew_P3c4 = pHWDE->pjIOAddress + SEQ_ADDRESS_PORT ;
    XGINew_P3d4 = pHWDE->pjIOAddress + CRTC_ADDRESS_PORT_COLOR ;

    VideoDebugPrint( ( 0 , "vModifyLCDTiming301LVDS_INTA: pHWDE->ulMonitorPowerState=%lx\n" , pHWDE->ulMonitorPowerState ) ) ;
    VideoDebugPrint( ( 0 , "vModifyLCDTiming301LVDS_INTA: pHWDE->bMonitorPoweredOn=%x\n" , pHWDE->bMonitorPoweredOn ) ) ;
    if ( pHWDE->ulMonitorPowerState == VideoPowerHibernate )
        return ;

    if ( !( pHWDE->bMonitorPoweredOn ) )
        return ;

    bSaved3C4 = VideoPortReadPortUchar( XGINew_P3c4 ) ;
    bSaved3D4 = VideoPortReadPortUchar( XGINew_P3d4 ) ;


    if ( XGINew_WantToSetMode( pHWDE ) )
    {
        XGI_Newdebugcode( 0x66 ) ;
        XGINew_TurnOffBacklight( pHWDE ) ;
        XGI_DisplayOff(pVBInfo) ;
        pHWDE->bCR34 = 0xFF ;
    }
    else
    {
        if ( XGINew_WantToChangeTimingRegister( pHWDE ) )
        {
            XGI_Newdebugcode( 0x55 ) ;
            XGINew_DisableWriteExternal( pHWDE ) ;
            XGINew_EnableWriteInternal( pHWDE ) ;
            XGINew_DisableWriteBackup( pHWDE ) ;
            XGINew_SetReadInternal( pHWDE ) ;
            VideoDebugPrint( ( 0 , "start ModifyLCD: %lx\n" , pHWDE->ulMonitorPowerState ) ) ;
            XGINew_ReSet301VBTiming( pHWDE, pVBInfo ) ;
            XGI_DisplayOn(pVBInfo) ;               /* alan 2003/10/01 */
            XGINew_ExternalToBackup( pHWDE ) ;
            XGINew_SetReadExternal( pHWDE ) ;
            XGINew_DisableWriteInternal( pHWDE ) ;
            XGINew_EnableWriteExternal( pHWDE ) ;
            XGINew_DisableWriteBackup( pHWDE ) ;
            XGINew_TurnOnBacklight( pHWDE ) ;
        }
    }

    VideoPortWritePortUchar( XGINew_P3c4 , bSaved3C4 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 , bSaved3D4 ) ;

    XGINew_GRandAR13Changed( pHWDE ) ;


    bSaved3C4 = VideoPortReadPortUchar( XGINew_P3c4) ;
    bSaved3D4 = VideoPortReadPortUchar( XGINew_P3d4 ) ;

    VideoPortWritePortUchar( XGINew_P3c4 , bSaved3C4 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 , bSaved3D4 ) ;
    return ;
}


/* --------------------------------------------------------------------- */
/* Function : vModifyLCDTiming301B_INTANew */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
VOID vModifyLCDTiming301B_INTANew( PHW_DEVICE_EXTENSION pHWDE )
{
    PUCHAR XGINew_P3c4 , XGINew_P3d4 ;
    UCHAR bSaved3C4 , bSaved3D4,temp ;
    VB_DEVICE_INFO VBINF;
    PVB_DEVICE_INFO pVBInfo = &VBINF;
    XGINew_P3c4 = pHWDE->pjIOAddress + SEQ_ADDRESS_PORT ;
    XGINew_P3d4 = pHWDE->pjIOAddress + CRTC_ADDRESS_PORT_COLOR ;

    VideoDebugPrint( ( 0 , "vModifyLCDTiming301B_INTANew: pHWDE->ulMonitorPowerState=%lx\n" , pHWDE->ulMonitorPowerState ) ) ;
    VideoDebugPrint( ( 0 , "vModifyLCDTiming301B_INTANew: pHWDE->bMonitorPoweredOn=%x\n" , pHWDE->bMonitorPoweredOn ) ) ;
    if ( pHWDE->ulMonitorPowerState == VideoPowerHibernate )
        return ;

    if ( !( pHWDE->bMonitorPoweredOn ) )
        return ;

    bSaved3C4 = VideoPortReadPortUchar( XGINew_P3c4) ;
    bSaved3D4 = VideoPortReadPortUchar( XGINew_P3d4 ) ;


    if ( XGINew_WantToSetMode( pHWDE ) )
    {
        XGI_Newdebugcode( 0x66 ) ;
        XGINew_TurnOffBacklight( pHWDE ) ;
        XGI_DisplayOff(pVBInfo) ;
        pHWDE->bCR34 = 0xFF ;
    }
    else
    {
        if ( XGINew_WantToChangeTimingRegister( pHWDE ) )
        {
            XGI_Newdebugcode( 0x55 ) ;
            XGINew_DisableWriteExternal( pHWDE ) ;
            XGINew_EnableWriteInternal( pHWDE ) ;
            XGINew_DisableWriteBackup( pHWDE ) ;
            XGINew_SetReadInternal( pHWDE ) ;
            VideoDebugPrint( ( 0 , "start ModifyLCD: %lx\n" , pHWDE->ulMonitorPowerState ) ) ;
            XGINew_ReSet301VBTiming( pHWDE, pVBInfo ) ;
            XGI_DisplayOn(pVBInfo) ;               /* alan 2003/10/01 */
            XGINew_ExternalToBackup( pHWDE ) ;
            XGINew_SetReadExternal( pHWDE ) ;
            XGINew_DisableWriteInternal( pHWDE ) ;
            XGINew_EnableWriteExternal( pHWDE ) ;
            XGINew_DisableWriteBackup( pHWDE ) ;
            XGINew_TurnOnBacklight( pHWDE ) ;
        }
    }

    VideoPortWritePortUchar( XGINew_P3c4 , bSaved3C4 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 , bSaved3D4 ) ;

    XGINew_GRandAR13Changed( pHWDE ) ;


    bSaved3C4 = VideoPortReadPortUchar( XGINew_P3c4 ) ;
    bSaved3D4 = VideoPortReadPortUchar( XGINew_P3d4 ) ;

    VideoPortWritePortUchar( XGINew_P3c4 , bSaved3C4 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 , bSaved3D4 ) ;
    return ;
}


/* --------------------------------------------------------------------- */
/* Function : vModifyCRT1Timing301LVDS_INTANew */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
VOID vModifyCRT1Timing301LVDS_INTANew( PHW_DEVICE_EXTENSION pHWDE )
{
    return ;
}


/* --------------------------------------------------------------------- */
/* Function : vModifyTVTiming301LVDS_INTANew */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
VOID vModifyTVTiming301LVDS_INTANew( PHW_DEVICE_EXTENSION pHWDE )
{
    PUCHAR XGINew_P3c4 , XGINew_P3d4 ;
    UCHAR bSaved3C4 , bSaved3D4 , temp ;
    VB_DEVICE_INFO VBINF;
    PVB_DEVICE_INFO pVBInfo = &VBINF;
    XGINew_P3c4 = pHWDE->pjIOAddress + SEQ_ADDRESS_PORT ;
    XGINew_P3d4 = pHWDE->pjIOAddress + CRTC_ADDRESS_PORT_COLOR ;

    VideoDebugPrint( ( 0 , "vModifyTVTiming301LVDS_INTA: pHWDE->ulMonitorPowerState=%lx\n" , pHWDE->ulMonitorPowerState ) ) ;
    VideoDebugPrint( ( 0 , "vModifyTVTiming301LVDS_INTA: pHWDE->bMonitorPoweredOn=%x\n" , pHWDE->bMonitorPoweredOn ) ) ;
    if ( pHWDE->ulMonitorPowerState == VideoPowerHibernate )
        return ;

    if ( !( pHWDE->bMonitorPoweredOn ) )
        return;

    bSaved3C4 = VideoPortReadPortUchar( XGINew_P3c4 ) ;
    bSaved3D4 = VideoPortReadPortUchar( XGINew_P3d4 ) ;


    if ( XGINew_WantToSetMode( pHWDE ) )
    {
        XGI_Newdebugcode( 0x66 ) ;
        XGINew_TurnOffBacklight( pHWDE ) ;
        XGI_DisplayOff(pVBInfo) ;
        pHWDE->bCR34 = 0xFF ;
    }
    else
    {
        if ( XGINew_WantToChangeTimingRegister( pHWDE ) )
        {
            XGI_Newdebugcode( 0x55 ) ;
            XGINew_DisableWriteExternal( pHWDE ) ;
            XGINew_EnableWriteInternal( pHWDE ) ;
            XGINew_DisableWriteBackup( pHWDE ) ;
            XGINew_SetReadInternal( pHWDE ) ;
            VideoDebugPrint( ( 0 , "start ModifyLCD: %lx\n" , pHWDE->ulMonitorPowerState ) ) ;
            XGINew_ReSet301VBTiming( pHWDE, pVBInfo ) ;
            XGI_DisplayOn(pVBInfo) ;               /* alan 2003/10/01 */
            XGINew_ExternalToBackup( pHWDE ) ;
            XGINew_SetReadExternal( pHWDE ) ;
            XGINew_DisableWriteInternal( pHWDE ) ;
            XGINew_EnableWriteExternal( pHWDE ) ;
            XGINew_DisableWriteBackup( pHWDE ) ;
            XGINew_TurnOnBacklight(pHWDE) ;
        }
    }

    VideoPortWritePortUchar( XGINew_P3c4 , bSaved3C4 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 , bSaved3D4 ) ;

    /* XGINew_GRandAR13Changed(pHWDE) ; */

    bSaved3C4 = VideoPortReadPortUchar( XGINew_P3c4 ) ;
    bSaved3D4 = VideoPortReadPortUchar( XGINew_P3d4 ) ;

    VideoPortWritePortUchar( XGINew_P3c4 , bSaved3C4 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 , bSaved3D4 ) ;
    XGIDelayUS( 1000 ) ;  /* for CRT+TV quit debug in winxp dos. */

    return ;
}

/* --------------------------------------------------------------------- */
/* Function : vModifyTVTiming301B_INTANew */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
VOID vModifyTVTiming301B_INTANew( PHW_DEVICE_EXTENSION pHWDE )
{
    PUCHAR XGINew_P3c4 , XGINew_P3d4 ;
    UCHAR bSaved3C4 , bSaved3D4 , temp ;
    VB_DEVICE_INFO VBINF;
    PVB_DEVICE_INFO pVBInfo = &VBINF;
    XGINew_P3c4 = pHWDE->pjIOAddress + SEQ_ADDRESS_PORT ;
    XGINew_P3d4 = pHWDE->pjIOAddress + CRTC_ADDRESS_PORT_COLOR ;

    VideoDebugPrint( ( 0 , "vModifyTVTiming301LVDS_INTA: pHWDE->ulMonitorPowerState=%lx\n" , pHWDE->ulMonitorPowerState ) ) ;
    VideoDebugPrint( ( 0 , "vModifyTVTiming301LVDS_INTA: pHWDE->bMonitorPoweredOn=%x\n" , pHWDE->bMonitorPoweredOn ) ) ;
    if ( pHWDE->ulMonitorPowerState == VideoPowerHibernate )
        return ;

    if ( !( pHWDE->bMonitorPoweredOn ) )
        return;

    bSaved3C4 = VideoPortReadPortUchar( XGINew_P3c4 ) ;
    bSaved3D4 = VideoPortReadPortUchar( XGINew_P3d4 ) ;


    if ( XGINew_WantToSetMode( pHWDE ) )
    {
        XGI_Newdebugcode( 0x66 ) ;
        XGINew_TurnOffBacklight( pHWDE ) ;
        XGI_DisplayOff(pVBInfo) ;
        pHWDE->bCR34 = 0xFF ;
    }
    else
    {
        if ( XGINew_WantToChangeTimingRegister( pHWDE ) )
        {
            XGI_Newdebugcode( 0x55 ) ;
            XGINew_DisableWriteExternal( pHWDE ) ;
            XGINew_EnableWriteInternal( pHWDE ) ;
            XGINew_DisableWriteBackup( pHWDE ) ;
            XGINew_SetReadInternal( pHWDE ) ;
            VideoDebugPrint( ( 0 , "start ModifyLCD: %lx\n" , pHWDE->ulMonitorPowerState ) ) ;
            XGINew_ReSet301VBTiming( pHWDE, pVBInfo ) ;
            XGI_DisplayOn(pVBInfo) ;               /* alan 2003/10/01 */
            XGINew_ExternalToBackup( pHWDE ) ;
            XGINew_SetReadExternal( pHWDE ) ;
            XGINew_DisableWriteInternal( pHWDE ) ;
            XGINew_EnableWriteExternal( pHWDE ) ;
            XGINew_DisableWriteBackup( pHWDE ) ;
            XGINew_TurnOnBacklight(pHWDE) ;
        }
    }

    VideoPortWritePortUchar( XGINew_P3c4 , bSaved3C4 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 , bSaved3D4 ) ;

    /* XGINew_GRandAR13Changed(pHWDE) ; */

    bSaved3C4 = VideoPortReadPortUchar( XGINew_P3c4 ) ;
    bSaved3D4 = VideoPortReadPortUchar( XGINew_P3d4 ) ;

    VideoPortWritePortUchar( XGINew_P3c4 , bSaved3C4 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 , bSaved3D4 ) ;
    XGIDelayUS( 1000 ) ;  /* for CRT+TV quit debug in winxp dos. */


}

/* --------------------------------------------------------------------- */
/* Function : vModifyLCDTimingLVDSNew */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
VOID vModifyLCDTimingLVDSNew( PHW_DEVICE_EXTENSION pHWDE )
{
}


/* --------------------------------------------------------------------- */
/* Function : vModifyCRT1TimingLVDSNew */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
VOID vModifyCRT1TimingLVDSNew( PHW_DEVICE_EXTENSION pHWDE )
{
}


/* --------------------------------------------------------------------- */
/* Function : vModifyCRT1TimingLVDS_INTANew */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
VOID vModifyCRT1TimingLVDS_INTANew(PHW_DEVICE_EXTENSION   pHWDE)
{
    return ;
}


/* --------------------------------------------------------------------- */
/* Function : vModifyLCDTimingLVDS_INTANew */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
VOID vModifyLCDTimingLVDS_INTANew(PHW_DEVICE_EXTENSION   pHWDE)
{
    return ;
}


/* --------------------------------------------------------------------- */
/* Function : vModifyTVTimingLVDS_INTANew */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
VOID vModifyTVTimingLVDS_INTANew(PHW_DEVICE_EXTENSION   pHWDE)
{
    return ;
}


/* --------------------------------------------------------------------- */
/* Function : vModifyCRT1TimingCH7019New */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
VOID vModifyCRT1TimingCH7019New(PHW_DEVICE_EXTENSION   pHWDE)
{
}


/* --------------------------------------------------------------------- */
/* Function : vModifyCRT1TimingCH7019_INTANew */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
VOID vModifyCRT1TimingCH7019_INTANew(PHW_DEVICE_EXTENSION   pHWDE)
{
    return ;
}


/* --------------------------------------------------------------------- */
/* Function : vModifyLCDTimingCH7019_INTANew */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
VOID vModifyLCDTimingCH7019_INTANew(PHW_DEVICE_EXTENSION   pHWDE)
{
    return ;
}


/* --------------------------------------------------------------------- */
/* Function : vModifyLCDTimingCH7019New */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
VOID vModifyLCDTimingCH7019New(PHW_DEVICE_EXTENSION   pHWDE)
{
    return ;
}


/* --------------------------------------------------------------------- */
/* Function : vModifyTVTimingCH7019_INTANew */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
VOID vModifyTVTimingCH7019_INTANew(PHW_DEVICE_EXTENSION   pHWDE)
{
    return ;
}


/* --------------------------------------------------------------------- */
/* Function : vModifyTVTimingCH7019New */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
VOID vModifyTVTimingCH7019New(PHW_DEVICE_EXTENSION   pHWDE)
{
    return ;
}


/* --------------------------------------------------------------------- */
/* Function : vModifyLCDTiming301BNew */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
VOID vModifyLCDTiming301BNew(PHW_DEVICE_EXTENSION   pHWDE)
{
    return ;
}


/* --------------------------------------------------------------------- */
/* Function : vModifyCRT1Timing301LVDSNew */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
VOID vModifyCRT1Timing301LVDSNew(PHW_DEVICE_EXTENSION   pHWDE)
{
    return ;
}


/* --------------------------------------------------------------------- */
/* Function : vModifyLCDTiming301LVDSNew */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
VOID vModifyLCDTiming301LVDSNew(PHW_DEVICE_EXTENSION   pHWDE)
{
    return ;
}


/* --------------------------------------------------------------------- */
/* Function : vModifyTVTiming301LVDSNew */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
VOID vModifyTVTiming301LVDSNew(PHW_DEVICE_EXTENSION   pHWDE)
{
}


/* --------------------------------------------------------------------- */
/* Function : XGINew_WantToSetMode */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
int XGINew_WantToSetMode(PHW_DEVICE_EXTENSION pHWDE)
{
    PUCHAR XGINew_P3d4 ;

    XGINew_P3d4 = pHWDE->pjIOAddress + CRTC_ADDRESS_PORT_COLOR ;
    VideoPortWritePortUchar( XGINew_P3d4 , 0x3A ) ;
    if ( VideoPortReadPortUchar( XGINew_P3d4 + 1 ) & 8 )
        return( 1 ) ;
    else
        return( 0 ) ;
}


/* --------------------------------------------------------------------- */
/* Function : XGINew_WantToChangeTimingRegister */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
int XGINew_WantToChangeTimingRegister(PHW_DEVICE_EXTENSION pHWDE)
{
    PUCHAR XGINew_P3c2 , XGINew_P3cc , XGINew_P3c4 , XGINew_P3d4 , XGINew_P3da , XGINew_P3c0 ;
    UCHAR  bCR56 ;
    UCHAR temp , temp1 ;
    UCHAR bCR0 , bCR1 , bCR2 , bCR3 , bCR4 , bCR5 , bCR6 , bCR7 ;
    UCHAR bCR9 , bCR10 , bCR11 , bCR12 , bCR13 , bCR15 , bCR16 , bCR34 ;
    UCHAR bSR0A , bSR0B , bSR0C ;
    /* UCHAR bSR1 ; */
    UCHAR b3CC ;
    /* UCHAR bAR13 ; */
    UCHAR Key ;
    ULONG VGAHDE0 , VGAVDE0 , VGAHDE1 , VGAVDE1 ;
    BOOL bWantToChange = 0 ;

    XGINew_P3c4 = pHWDE->pjIOAddress + SEQ_ADDRESS_PORT ;
    XGINew_P3c2 = pHWDE->pjIOAddress + MISC_OUTPUT_REG_WRITE_PORT ;
    XGINew_P3cc = pHWDE->pjIOAddress + MISC_OUTPUT_REG_READ_PORT ;
    XGINew_P3d4 = pHWDE->pjIOAddress + CRTC_ADDRESS_PORT_COLOR ;
    XGINew_P3da = pHWDE->pjIOAddress + 0x2A ;
    XGINew_P3c0 = pHWDE->pjIOAddress + ATT_ADDRESS_PORT ;
    VideoPortWritePortUchar( XGINew_P3d4 , 0x56 ) ;
    bCR56 = VideoPortReadPortUchar( XGINew_P3d4 + 1 ) ;

    VideoPortWritePortUchar( XGINew_P3d4 , 0x56 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 + 1 , ( UCHAR )( ( bCR56 & 0xE7 ) | 0x08 ) ) ;
    VideoPortWritePortUchar( XGINew_P3d4 , 0x00 ) ;
    bCR0 = VideoPortReadPortUchar( XGINew_P3d4 + 1 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 , 0x56 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 + 1 , ( UCHAR )( ( bCR56 & 0xE7 ) | 0x00 ) ) ;
    VideoPortWritePortUchar( XGINew_P3d4 , 0x00 ) ;
    temp = VideoPortReadPortUchar( XGINew_P3d4 + 1 ) ;

    if ( ( bCR0 != temp ) || ( pHWDE->bCR34 == 0xFF ) )
    {
        bWantToChange = 1 ;
    }

    pHWDE->bCR0 = bCR0 ;

    VideoPortWritePortUchar( XGINew_P3d4 , 0x56 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 + 1 , ( UCHAR )( ( bCR56 & 0xE7 ) | 0x08 ) ) ;
    VideoPortWritePortUchar( XGINew_P3d4 , 0x01 ) ;
    bCR1 = VideoPortReadPortUchar( XGINew_P3d4 + 1 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 , 0x56 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 + 1 , ( UCHAR )( ( bCR56 & 0xE7 ) | 0x00 ) ) ;
    VideoPortWritePortUchar( XGINew_P3d4 , 0x01 ) ;
    temp = VideoPortReadPortUchar( XGINew_P3d4 + 1 ) ;

    VGAHDE0 = temp ;
    if ( ( bCR1 != temp ) || ( pHWDE->bCR34 == 0xFF ) )
    {
        bWantToChange = 1 ;
    }

    pHWDE->bCR1 = bCR1 ;

    VideoPortWritePortUchar( XGINew_P3d4 , 0x56 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 + 1 , ( UCHAR )( ( bCR56&0xE7 ) | 0x08 ) ) ;
    VideoPortWritePortUchar( XGINew_P3d4 , 0x02 ) ;
    bCR2 = VideoPortReadPortUchar( XGINew_P3d4 + 1 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 , 0x56 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 + 1 , ( UCHAR )( ( bCR56 & 0xE7 ) | 0x00 ) ) ;
    VideoPortWritePortUchar( XGINew_P3d4 , 0x02 ) ;
    temp = VideoPortReadPortUchar( XGINew_P3d4 + 1 ) ;

    if ( ( bCR2 != temp ) || ( pHWDE->bCR34 == 0xFF ) )
    {
        bWantToChange = 1 ;
    }
        pHWDE->bCR2 = bCR2 ;

    VideoPortWritePortUchar( XGINew_P3d4 , 0x56 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 + 1 , ( UCHAR )( ( bCR56 & 0xE7 ) | 0x08 ) ) ;
    VideoPortWritePortUchar( XGINew_P3d4 , 0x03 ) ;
    bCR3 = VideoPortReadPortUchar( XGINew_P3d4 + 1 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 , 0x56 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 + 1 , ( UCHAR )( ( bCR56 & 0xE7 ) | 0x00 ) ) ;
    VideoPortWritePortUchar( XGINew_P3d4 , 0x03 ) ;
    temp = VideoPortReadPortUchar( XGINew_P3d4 + 1 ) ;
    if ( ( ( bCR3 & 0x1F ) != ( temp & 0x1F ) ) || ( pHWDE->bCR34 == 0xFF ) )
    {
        bWantToChange = 1 ;
    }
        pHWDE->bCR3 = bCR3 ;

    VideoPortWritePortUchar( XGINew_P3d4 , 0x56 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 + 1 , ( UCHAR )( ( bCR56 & 0xE7 ) | 0x08 ) ) ;
    VideoPortWritePortUchar( XGINew_P3d4 , 0x04 ) ;
    bCR4 = VideoPortReadPortUchar( XGINew_P3d4 + 1 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 , 0x56 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 + 1 , ( UCHAR )( ( bCR56 & 0xE7 ) | 0x00 ) ) ;
    VideoPortWritePortUchar( XGINew_P3d4 , 0x04 ) ;
    temp = VideoPortReadPortUchar( XGINew_P3d4 + 1 ) ;
    if ( ( bCR4 != temp ) || ( pHWDE->bCR34 == 0xFF ) )
    {
        bWantToChange = 1 ;
    }
        pHWDE->bCR4 = bCR4 ;

    VideoPortWritePortUchar( XGINew_P3d4 , 0x56 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 + 1 , ( UCHAR )( ( bCR56 & 0xE7 ) | 0x08 ) ) ;
    VideoPortWritePortUchar( XGINew_P3d4 , 0x05 ) ;
    bCR5 = VideoPortReadPortUchar( XGINew_P3d4 + 1 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 , 0x56 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 + 1 , ( UCHAR )( ( bCR56 & 0xE7 ) | 0x00 ) ) ;
    VideoPortWritePortUchar( XGINew_P3d4 , 0x05 ) ;
    temp = VideoPortReadPortUchar( XGINew_P3d4 + 1 ) ;
    if ( ( bCR5 != temp ) || ( pHWDE->bCR34 == 0xFF ) )
    {
        bWantToChange = 1 ;
    }
        pHWDE->bCR5 = bCR5 ;

    VideoPortWritePortUchar( XGINew_P3d4 , 0x56 ) ;
    VideoPortWritePortUchar( XGINew_P3d4+1 , ( UCHAR )( ( bCR56 & 0xE7 ) | 0x08 ) ) ;
    VideoPortWritePortUchar( XGINew_P3d4 , 0x06 ) ;
    bCR6 = VideoPortReadPortUchar( XGINew_P3d4 + 1 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 , 0x56 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 + 1 , ( UCHAR )( ( bCR56 & 0xE7 ) | 0x00 ) ) ;
    VideoPortWritePortUchar( XGINew_P3d4 , 0x06 ) ;
    temp = VideoPortReadPortUchar( XGINew_P3d4 + 1 ) ;
    if ( ( bCR6 != temp ) || ( pHWDE->bCR34 == 0xFF ) )
    {
        bWantToChange = 1 ;
    }
        pHWDE->bCR6 = bCR6 ;

    VideoPortWritePortUchar( XGINew_P3d4 , 0x56 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 + 1 , ( UCHAR )( ( bCR56 & 0xE7 ) | 0x08 ) ) ;
    VideoPortWritePortUchar( XGINew_P3d4 , 0x12 ) ;
    bCR12 = VideoPortReadPortUchar( XGINew_P3d4 + 1 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 , 0x56 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 + 1 , ( UCHAR )( ( bCR56 & 0xE7 ) | 0x00 ) ) ;
    VideoPortWritePortUchar( XGINew_P3d4 , 0x012 ) ;
    temp = VideoPortReadPortUchar( XGINew_P3d4 + 1 ) ;
    VGAVDE0 = temp ;
    if ( ( bCR12 != temp ) || ( pHWDE->bCR34 == 0xFF ) )
    {
        bWantToChange = 1 ;
    }
    pHWDE->bCR12 = bCR12 ;

    VideoPortWritePortUchar( XGINew_P3d4 , 0x56 ) ;	/* CR13 is not in INTA protect but the hardware do protect */
    VideoPortWritePortUchar( XGINew_P3d4 + 1 , ( UCHAR )( ( bCR56 & 0xE7 ) | 0x08 ) ) ;
    VideoPortWritePortUchar( XGINew_P3d4 , 0x13 ) ;
    bCR13 = VideoPortReadPortUchar( XGINew_P3d4 + 1 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 , 0x56 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 + 1 , ( UCHAR )( ( bCR56 & 0xE7 ) | 0x00 ) ) ;
    VideoPortWritePortUchar( XGINew_P3d4 , 0x13 ) ;
    temp = VideoPortReadPortUchar( XGINew_P3d4 + 1 ) ;

    if ( bCR13 != temp )
    {
        VideoDebugPrint( ( 0 , "modify CR13\n" ) ) ;
        VideoPortWritePortUchar( XGINew_P3d4 , 0x51 ) ;
        temp = VideoPortReadPortUchar( XGINew_P3d4 + 1 ) ; /* save cr51 */
        VideoPortWritePortUchar( XGINew_P3d4 + 1 , ( UCHAR )( 0x60 ) ) ; /* write permitted */
        VideoPortWritePortUchar( XGINew_P3d4 , 0x13 ) ;
        VideoPortWritePortUchar( XGINew_P3d4 + 1 , bCR13 ) ;
        VideoPortWritePortUchar( XGINew_P3d4 , 0x51 ) ;
        VideoPortWritePortUchar( XGINew_P3d4 + 1 , temp ) ; /* restore cr51 */
    }

    VideoPortWritePortUchar( XGINew_P3d4 , 0x56 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 + 1 , ( UCHAR )( ( bCR56 & 0xE7 ) | 0x08 ) ) ;
    VideoPortWritePortUchar( XGINew_P3d4 , 0x07 ) ;
    bCR7 = VideoPortReadPortUchar( XGINew_P3d4 + 1 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 , 0x56 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 + 1 , ( UCHAR )( ( bCR56 & 0xE7 ) | 0x00 ) ) ;
    VideoPortWritePortUchar( XGINew_P3d4 , 0x07 ) ;
    temp = VideoPortReadPortUchar( XGINew_P3d4 + 1 ) ;
    VGAVDE0 |= ( ( temp & 0x02 ) << 7 ) ;
    VGAVDE0 |= ( ( temp & 0x40 ) << 3 ) ;
    if ( ( ( bCR7 & 0xEF ) != ( temp & 0xEF ) ) || ( pHWDE->bCR34 == 0xFF ) )
    {
        bWantToChange = 1 ;
    }
    pHWDE->bCR7 = bCR7 ;
    VideoPortWritePortUchar( XGINew_P3d4 , 0x56 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 + 1 , ( UCHAR )( ( bCR56 & 0xE7 ) | 0x08 ) ) ;
    VideoPortWritePortUchar( XGINew_P3d4 , 0x09 ) ;
    bCR9 = VideoPortReadPortUchar( XGINew_P3d4 + 1 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 , 0x56 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 + 1 , ( UCHAR )( ( bCR56 & 0xE7 ) | 0x00 ) ) ;
    VideoPortWritePortUchar( XGINew_P3d4 , 0x09 ) ;
    temp = VideoPortReadPortUchar( XGINew_P3d4 + 1 ) ;
    if ( ( ( bCR9 & 0x20 ) != ( temp & 0x20 ) ) || ( pHWDE->bCR34 == 0xFF ) )
    {
        bWantToChange = 1 ;
    }
        pHWDE->bCR9 = bCR9 ;

    VideoPortWritePortUchar( XGINew_P3d4 , 0x56 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 + 1 , ( UCHAR )( ( bCR56 & 0xE7 ) | 0x08 ) ) ;
    VideoPortWritePortUchar( XGINew_P3d4 , 0x10 ) ;
    bCR10 = VideoPortReadPortUchar(XGINew_P3d4 + 1 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 , 0x56 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 + 1 , ( UCHAR )( ( bCR56 & 0xE7 ) | 0x00 ) ) ;
    VideoPortWritePortUchar( XGINew_P3d4 , 0x010 ) ;
    temp = VideoPortReadPortUchar( XGINew_P3d4 + 1 ) ;
    if ( ( bCR10 != temp ) || ( pHWDE->bCR34 == 0xFF ) )
    {
        bWantToChange = 1 ;
    }
        pHWDE->bCR10 = bCR10 ;

    VideoPortWritePortUchar( XGINew_P3d4 , 0x56 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 + 1 , ( UCHAR )( ( bCR56 & 0xE7 ) | 0x08 ) ) ;
    VideoPortWritePortUchar( XGINew_P3d4 , 0x11 ) ;
    bCR11 = VideoPortReadPortUchar( XGINew_P3d4 + 1 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 , 0x56 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 + 1 , ( UCHAR )( ( bCR56 & 0xE7 ) | 0x00 ) ) ;
    VideoPortWritePortUchar( XGINew_P3d4 , 0x011 ) ;
    temp = VideoPortReadPortUchar( XGINew_P3d4 + 1 ) ;
    if ( ( ( bCR11 & 0xF ) != ( temp & 0xF ) ) || ( pHWDE->bCR34 == 0xFF ) )
    {
        bWantToChange = 1 ;
    }
    pHWDE->bCR11 = bCR11 ;
    VideoPortWritePortUchar( XGINew_P3d4 , 0x56 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 + 1 , ( UCHAR )( ( bCR56 & 0xE7 ) | 0x08 ) ) ;
    VideoPortWritePortUchar( XGINew_P3d4 , 0x15 ) ;
    bCR15 = VideoPortReadPortUchar( XGINew_P3d4 + 1 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 , 0x56 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 + 1 , ( UCHAR )( ( bCR56 & 0xE7 ) | 0x00 ) ) ;
    VideoPortWritePortUchar( XGINew_P3d4 , 0x015 ) ;
    temp = VideoPortReadPortUchar( XGINew_P3d4 + 1 ) ;
    if ( ( bCR15 != temp ) || ( pHWDE->bCR34 == 0xFF ) )
    {
        bWantToChange = 1 ;
    }

    pHWDE->bCR15 = bCR15 ;
    VideoPortWritePortUchar( XGINew_P3d4 , 0x56 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 + 1 , ( UCHAR )( ( bCR56 & 0xE7 ) | 0x08 ) ) ;
    VideoPortWritePortUchar( XGINew_P3d4 , 0x16 ) ;
    bCR16 = VideoPortReadPortUchar( XGINew_P3d4 + 1 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 , 0x56 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 + 1 , ( UCHAR )( ( bCR56 & 0xE7 ) | 0x00 ) ) ;
    VideoPortWritePortUchar( XGINew_P3d4 , 0x016 ) ;
    temp = VideoPortReadPortUchar( XGINew_P3d4 + 1 ) ;
    if ( ( bCR16!= temp ) || ( pHWDE->bCR34 == 0xFF ) )
    {
        bWantToChange = 1 ;
    }
        pHWDE->bCR16 = bCR16 ;

    VideoPortWritePortUchar( XGINew_P3c4 , 0x5 ) ;
    Key = VideoPortReadPortUchar( XGINew_P3c4 + 1 ) ;
    VideoPortWritePortUshort( ( PUSHORT )XGINew_P3c4 , 0x8605 ) ;

    VideoPortWritePortUchar( XGINew_P3d4 , 0x56 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 + 1 , ( UCHAR )( ( bCR56 & 0xE7 ) | 0x08 ) ) ;
    VideoPortWritePortUchar( XGINew_P3c4 , 0x0A ) ;
    bSR0A = VideoPortReadPortUchar( XGINew_P3c4 + 1 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 , 0x56 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 + 1 , ( UCHAR )( ( bCR56 & 0xE7 ) | 0x00 ) ) ;
    VideoPortWritePortUchar( XGINew_P3c4 , 0x0A ) ;
    temp = VideoPortReadPortUchar( XGINew_P3c4 + 1 ) ;
    VGAVDE0 |= ( (temp & 0x02 ) << 9 ) ;
    if ( ( bSR0A != temp ) || ( pHWDE->bCR34 == 0xFF ) )
    {
        bWantToChange = 1 ;
    }
        pHWDE->bSR0A = bSR0A ;

    VideoPortWritePortUchar( XGINew_P3d4 , 0x56 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 + 1 , ( UCHAR )( ( bCR56 & 0xE7 ) | 0x08 ) ) ;
    VideoPortWritePortUchar( XGINew_P3c4,0x0B ) ;
    bSR0B = VideoPortReadPortUchar( XGINew_P3c4 + 1 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 , 0x56 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 + 1 ,( UCHAR )( ( bCR56 & 0xE7 ) | 0x00 ) ) ;
    VideoPortWritePortUchar( XGINew_P3c4 , 0x0B ) ;
    temp = VideoPortReadPortUchar( XGINew_P3c4 + 1 ) ;
    VGAHDE0 |= ( ( temp & 0x0C ) << 6 ) ;
    if ( ( bSR0B!= temp ) || ( pHWDE->bCR34 == 0xFF ) )
    {
        bWantToChange = 1 ;
    }
        pHWDE->bSR0B = bSR0B ;

    VideoPortWritePortUchar( XGINew_P3d4 , 0x56 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 + 1 , ( UCHAR )( ( bCR56 & 0xE7 ) | 0x08 ) ) ;
    VideoPortWritePortUchar( XGINew_P3c4 , 0x0C ) ;
    bSR0C = VideoPortReadPortUchar( XGINew_P3c4 + 1 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 , 0x56 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 + 1 , ( UCHAR )( ( bCR56 & 0xE7 ) | 0x00 ) ) ;
    VideoPortWritePortUchar( XGINew_P3c4,0x0C);
    temp = VideoPortReadPortUchar( XGINew_P3c4 + 1 ) ;
    if ( ( bSR0C != temp ) || ( pHWDE->bCR34 == 0xFF ) )
    {
        bWantToChange = 1 ;
    }
        pHWDE->bSR0C = bSR0C ;

    if ( Key != 0xA1 )
        VideoPortWritePortUshort( ( PUSHORT )XGINew_P3c4 , 0x0005 ) ;

/*
    VideoPortWritePortUchar( XGINew_P3d4 , 0x56 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 + 1 , ( UCHAR )( ( bCR56 & 0xE7 ) | 0x08 ) ) ;
    VideoPortWritePortUchar( XGINew_P3c4 , 0x01 ) ;
    bSR1 = VideoPortReadPortUchar( XGINew_P3c4 + 1 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 , 0x56 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 + 1 , ( UCHAR )( ( bCR56 & 0xE7 ) | 0x00 ) ) ;
    VideoPortWritePortUchar( XGINew_P3c4 , 0x01 ) ;
    temp = VideoPortReadPortUchar( XGINew_P3c4 + 1 ) ;
    if( ( bSR1 & 0x01 ) != ( temp & 0x01 ) )
    {
        VideoDebugPrint( ( 0 , "modify SR01 %x to %x \n" , temp , bSR1 ) ) ;
        VideoPortWritePortUchar( XGINew_P3d4 , 0x51 ) ;
        temp = VideoPortReadPortUchar( XGINew_P3d4 + 1 ) ; // save  cr51
        VideoPortWritePortUchar( XGINew_P3d4 + 1 , ( UCHAR )( 0x60 ) ) ; //write permitted
        VideoPortWritePortUchar( XGINew_P3c4 , 0x01 ) ;
        temp1 = VideoPortReadPortUchar( XGINew_P3c4 + 1 ) ;
        temp1 = ( ( temp1 & 0xFE ) | ( bSR1 & 0x01) ) ;
        VideoPortWritePortUchar( XGINew_P3c4 + 1 , temp1 ) ;
        VideoPortWritePortUchar( XGINew_P3d4 , 0x51 ) ;
        VideoPortWritePortUchar( XGINew_P3d4 + 1 , temp ) ; //restore cr51
    }

    VideoPortWritePortUchar( XGINew_P3d4 , 0x56 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 + 1 , ( UCHAR )( ( bCR56 & 0xE7 ) | 0x08 ) ) ; //E
    bAR13 = VideoPortReadPortUchar( XGINew_P3da ) ;
    VideoPortWritePortUchar( XGINew_P3c0 , 0x33 ) ;
    bAR13 = VideoPortReadPortUchar( XGINew_P3c0 + 1 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 , 0x56 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 + 1 , ( UCHAR )( ( bCR56 & 0xE7 ) | 0x00 ) ) ; //I
    temp = VideoPortReadPortUchar( XGINew_P3da ) ;
    VideoPortWritePortUchar( XGINew_P3c0 , 0x33 ) ;
    temp = VideoPortReadPortUchar( XGINew_P3c0 + 1 ) ;
    if ( bAR13 != temp )
    {
        VideoDebugPrint( ( 0 , "modify AR13 %x to %x\n" , temp , bAR13 ) ) ;
        VideoPortWritePortUchar( XGINew_P3d4 , 0x51 ) ;
        temp1 = VideoPortReadPortUchar( XGINew_P3d4 + 1 ) ; // save  cr51
        VideoPortWritePortUchar( XGINew_P3d4 + 1 , ( UCHAR )( 0x60 ) ) ; //write permitted
    	bAR13 = VideoPortReadPortUchar( XGINew_P3da ) ;
    	VideoPortWritePortUchar( XGINew_P3c0 , 0x33 ) ;
    	VideoPortWritePortUchar( XGINew_P3c0 + 1 , bAR13 ) ;
        VideoPortWritePortUchar( XGINew_P3d4 , 0x51 ) ;
        VideoPortWritePortUchar( XGINew_P3d4 + 1 , temp ) ; //restore cr51
    }
*/

    VideoPortWritePortUchar( XGINew_P3d4 , 0x56 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 + 1 , ( UCHAR )( ( bCR56 & 0xE7 ) | 0x08 ) ) ;
    b3CC = VideoPortReadPortUchar( XGINew_P3cc ) ;
    VideoPortWritePortUchar( XGINew_P3d4 , 0x56 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 + 1 , ( UCHAR )( ( bCR56 & 0xE7 ) | 0x00 ) ) ;
    temp = VideoPortReadPortUchar( XGINew_P3cc ) ;
    if ( ( ( b3CC & 0x0C ) != ( temp & 0x0C ) ) || ( pHWDE->bCR34 == 0xFF ) )
    {
        bWantToChange = 1 ;
    }

    pHWDE->b3CC = b3CC ;
    VideoPortWritePortUchar( XGINew_P3d4 , 0x34 ) ;
    bCR34 = VideoPortReadPortUchar( XGINew_P3d4 + 1 ) ;

    bWantToChange = 0 ;  /* we hope that change timing as little as possible */
    if ( bCR34!=pHWDE->bCR34 )
    {
        bWantToChange = 1 ;
    }

    pHWDE->bCR34 = bCR34 ;
    VideoPortWritePortUchar( XGINew_P3d4 , 0x56 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 + 1 , bCR56 ) ;

    VGAHDE1 = ( pHWDE->bCR1 ) | ( ( pHWDE->bSR0B & 0x0C ) << 6 ) ;
    VGAVDE1 = ( pHWDE->bCR12) | ( ( pHWDE->bCR7 & 0x02 ) << 7 ) | ( ( pHWDE->bCR7 & 0x40 ) << 3 ) | ( ( pHWDE->bSR0A & 0x02 ) << 9 ) ;

    if ( VGAHDE0 != VGAHDE1 )
    {
        bWantToChange = 1 ;
    }
    if (VGAVDE0!=VGAVDE1)
    {
        bWantToChange = 1 ;
    }

    VideoPortWritePortUchar( XGINew_P3d4 + 1 , ( UCHAR )( ( bCR56 & 0xE7 ) | 0x08 ) ) ;

    return( bWantToChange ) ;
}


/* --------------------------------------------------------------------- */
/* Function : XGINew_SetVBInfobyCR */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
BOOLEAN XGINew_SetVBInfobyCR( PXGI_HW_DEVICE_INFO HwDeviceExtension, PVB_DEVICE_INFO pVBInfo )
{

    UCHAR   temp ;
    UCHAR   bCR30 , bCR31 , ModeNo , bpart1_13 ;
    USHORT  ModeIdIndex , RefreshRateTableIndex ;
    pVBInfo->ROMAddr  = HwDeviceExtension->pjVirtualRomBase ;
    pVBInfo->FBAddr   = HwDeviceExtension->pjVideoMemoryAddress ;
    pVBInfo->BaseAddr = (USHORT)HwDeviceExtension->pjIOAddress ;

    if ( pVBInfo->FBAddr == 0 )
        return( FALSE ) ;

    if ( pVBInfo->BaseAddr == 0 )
        return( FALSE ) ;

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
    XGI_GetVBType(pVBInfo) ;
    XGI_InitTo330Pointer( HwDeviceExtension->jChipType, pVBInfo ) ;

    bCR30 = ( UCHAR )XGINew_GetReg1( pVBInfo->P3d4 , 0x30 ) ;
    bCR31 = ( UCHAR )XGINew_GetReg1( pVBInfo->P3d4 , 0x31 ) ;

    XGI_UnLockCRT2( HwDeviceExtension, pVBInfo) ;
    pVBInfo->VBInfo = bCR30 + ( bCR31 << 8 ) ;

    pVBInfo->IF_DEF_LCDA = 1 ;  /* alan 09/10/2003 */
    pVBInfo->IF_DEF_LVDS = 0 ;
    pVBInfo->IF_DEF_CH7005 = 0 ;
    pVBInfo->IF_DEF_HiVision = 0 ;
/* pVBInfo->IF_DEF_FSTN = 0 ; */
    pVBInfo->IF_DEF_YPbPr = 0 ;

    ModeNo = (UCHAR)XGINew_GetReg1( pVBInfo->P3d4 , 0x34 ) ;
    temp = XGI_SearchModeID( ModeNo , &ModeIdIndex, pVBInfo ) ;	/* 2.Get ModeID Table */
    if ( temp == 0 )
        return( 0 ) ;
    XGI_GetVGAType( HwDeviceExtension,  pVBInfo ) ;  /* alan 09/12/2003 */
    XGI_GetVBType(  pVBInfo ) ; /* alan 09/12/2003 */
    XGI_GetVBInfo( ModeNo , ModeIdIndex , HwDeviceExtension, pVBInfo ) ;
    XGI_GetTVInfo( ModeNo , ModeIdIndex, pVBInfo ) ;
    XGI_GetLCDInfo( ModeNo , ModeIdIndex, pVBInfo ) ;

    return( 1 ) ;
}


/* --------------------------------------------------------------------- */
/* Function : XGINew_ModLCDALockMode */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGINew_ModLCDALockMode(  USHORT ModeNo , USHORT ModeIdIndex , USHORT RefreshRateTableIndex ,
                             PXGI_HW_DEVICE_INFO HwDeviceExtension , USHORT HDE,USHORT VDE )
{
    return ;
}


/* --------------------------------------------------------------------- */
/* Function : XGINew_IsVAMode */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
BOOLEAN XGINew_IsVAMode( PVB_DEVICE_INFO pVBInfo )
{
    USHORT flag ;

    flag = XGINew_GetReg1( pVBInfo->P3d4 , 0x38 ) ;

#ifdef XGI315H
    if ( ( flag & EnableDualEdge ) && ( flag & SetToLCDA ) )
        return( 0 ) ;
    else
        return( 1 ) ;
#endif
    return( 1 ) ;
}


/* --------------------------------------------------------------------- */
/* Function : XGINew_IsDualEdge */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
BOOLEAN XGINew_IsDualEdge( PVB_DEVICE_INFO pVBInfo )
{
    USHORT flag ;

#ifdef XGI315H
    flag = XGINew_GetReg1( pVBInfo->P3d4 , 0x38 ) ;
    if ( flag & EnableDualEdge )
        return( 0 ) ;
    else
        return( 1 ) ;
#endif
    return( 1 ) ;
}

/* --------------------------------------------------------------------- */
/* Function : XGINew_ReSet301VBTiming */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
int XGINew_ReSet301VBTiming( PHW_DEVICE_EXTENSION pHWDE, PVB_DEVICE_INFO pVBInfo  )
{
    BOOLEAN   bReturn , bCHA ;
    UCHAR     ModeNo , temp , bCR12 , bCR39 , Part1_13 ;
    USHORT    HDE , VDE ;
    USHORT    ModeIdIndex , RefreshRateTableIndex ;

    PXGI_HW_DEVICE_INFO HwDeviceExtension = pHWDE->pXGIHWDE ;
    pVBInfo->ROMAddr  = HwDeviceExtension->pjVirtualRomBase ;
    pVBInfo->FBAddr   = HwDeviceExtension->pjVideoMemoryAddress ;
    pVBInfo->BaseAddr = ( USHORT )HwDeviceExtension->pjIOAddress ;

#ifdef WIN2000
    pVBInfo->ISXPDOS = 1 ;
#endif
    bReturn = XGINew_SetVBInfobyCR( HwDeviceExtension,pVBInfo ) ;
    if ( bReturn == 0 )
        return( ERROR_INVALID_PARAMETER ) ;

    bCR39     = ( UCHAR )XGINew_GetReg1( pVBInfo->P3d4 , 0x39 ) ;
    ModeNo    = ( UCHAR )XGINew_GetReg1( pVBInfo->P3d4 , 0x34 ) ;
    bCR12     = ( UCHAR )XGINew_GetReg1( pVBInfo->P3d4 , 0x12 ) ;
    Part1_13  = ( UCHAR )XGINew_GetReg1( pVBInfo->Part1Port , 0x13 ) ;
    bCHA      = Part1_13 & 0x04;

    XGINew_SetReg1( pVBInfo->P3c4 , 0x05 , 0x86 ) ;

   /* alan reset vb */
    if(pVBInfo->VBInfo & SetCRT2ToTV )
    {
        temp = XGI_SearchModeID(  ModeNo , &ModeIdIndex, pVBInfo ) ;
        if ( temp != 0 )
        {
            RefreshRateTableIndex = XGI_GetRatePtrCRT2( ModeNo , ModeIdIndex, pVBInfo ) ;
            if ( RefreshRateTableIndex != 0xFFFF )
            {
                XGINew_SetRegANDOR(pVBInfo->P3c4,0x06,~0x020,0x00);
                XGI_SetCRT1Offset(  ModeNo , ModeIdIndex , RefreshRateTableIndex , HwDeviceExtension, pVBInfo ) ;
            }
            bReturn = XGI_SetCRT2Group301( ModeNo , HwDeviceExtension, pVBInfo ) ; 	/* add for CRT2 */
            XGI_SetCRT2ModeRegs(  ModeNo , HwDeviceExtension, pVBInfo ) ;
            XGI_EnableBridge( HwDeviceExtension, pVBInfo ) ;
            if ( bCR12 == 0xDA )
            {
                XGINew_SetReg1( pVBInfo->Part1Port , 0x2F , 0x01 ) ;
                XGINew_SetReg1( pVBInfo->Part1Port , 0x0E , bCR12 ) ;
            }
        }
    }
    else  /* lcd */
    {
        if ( !XGINew_IsDualEdge( pVBInfo ) )
        {
            if ( !XGINew_IsVAMode(pVBInfo) )
                temp = 0x7F ;
            else
                temp = 0xBF ;
        }
        else
        {
            temp = 0x3F ;
        }
        XGINew_SetRegAND ( pVBInfo->Part4Port , 0x1F , temp ) ;

        HDE = 1024 ;
        VDE = 768 ;

        switch( pVBInfo->LCDResInfo )
        {
            case 0:
                HDE = 1024 ;
                VDE=768 ;
                if( !bCHA )
                {
                    temp = XGI_SearchModeID( ModeNo , &ModeIdIndex , pVBInfo) ;
                    if ( temp != 0 )
                    {
                        RefreshRateTableIndex = XGI_GetRatePtrCRT2( ModeNo , ModeIdIndex, pVBInfo ) ;
                        if ( RefreshRateTableIndex != 0xFFFF )
                        {
                            XGINew_SetRegANDOR( pVBInfo->P3c4 , 0x06 , ~0x020 , 0x00 ) ;
                            XGI_SetCRT1Offset( ModeNo , ModeIdIndex , RefreshRateTableIndex , HwDeviceExtension, pVBInfo ) ;
                        }
                        bReturn = XGI_SetCRT2Group301( ModeNo , HwDeviceExtension, pVBInfo ) ;                      /*add for CRT2 */
                        XGI_SetCRT2ModeRegs(  ModeNo , HwDeviceExtension, pVBInfo ) ;
                        XGI_EnableBridge( HwDeviceExtension, pVBInfo ) ;
                        if ( bCR12 == 0xDA )
                        {
                            XGINew_SetReg1( pVBInfo->Part1Port , 0x2F , 0x01 ) ;
                            XGINew_SetReg1( pVBInfo->Part1Port , 0x0E , bCR12 ) ;
                        }
                    }
                }
                break ;

            case 1:
                HDE = 800 ;
                VDE = 600 ;
                break ;

            case 2:
                HDE = 1024 ;
                VDE = 768 ;
                if( !bCHA )
                {
                    temp = XGI_SearchModeID( ModeNo , &ModeIdIndex, pVBInfo ) ;	/* 2.Get ModeID Table */
                    if ( temp != 0 )
                    {
                        RefreshRateTableIndex = XGI_GetRatePtrCRT2( ModeNo , ModeIdIndex, pVBInfo ) ;
                        if ( RefreshRateTableIndex != 0xFFFF )
                        {
                            XGINew_SetRegANDOR( pVBInfo->P3c4 , 0x06 , ~0x020 , 0x00 ) ;
                            XGI_SetCRT1Offset( ModeNo , ModeIdIndex , RefreshRateTableIndex , HwDeviceExtension, pVBInfo ) ;
                        }
                        bReturn = XGI_SetCRT2Group301( ModeNo , HwDeviceExtension, pVBInfo ) ;	/* add for CRT2 */
                        XGI_SetCRT2ModeRegs(  ModeNo , HwDeviceExtension, pVBInfo ) ;
                        XGI_EnableBridge( HwDeviceExtension, pVBInfo ) ;
                        if ( bCR12 == 0xDA )
                        {
                            XGINew_SetReg1( pVBInfo->Part1Port , 0x2F , 0x01 ) ;
                            XGINew_SetReg1( pVBInfo->Part1Port , 0x0E , bCR12 ) ;
                        }
                    }
                }
                break ;

            case 3:
                HDE = 1280 ;
                VDE = 1024 ;
                XGINew_SetReg1(pVBInfo->P3d4 , 0x39 , ( UCHAR )( bCR39 | 0x04 ) ) ;
                /* pVBInfo->VBInfo = XGINew_VBInfo | SetCRT2ToLCDA ; // alan 09/12/2003 */
                temp = XGI_SearchModeID( ModeNo , &ModeIdIndex, pVBInfo ) ;   /* 2.Get ModeID Table  */
                if ( temp != 0 )
                {
                    XGINew_SetReg1( pVBInfo->P3c4 , 0x2B , pHWDE->bSR2B ) ;
                    XGINew_SetReg1( pVBInfo->P3c4 , 0x2C , pHWDE->bSR2C ) ;
                    XGINew_SetReg1( pVBInfo->P3c4 , 0x2D , 0x01 ) ;
                    RefreshRateTableIndex = XGI_GetRatePtrCRT2(  ModeNo , ModeIdIndex, pVBInfo ) ;
                    if ( RefreshRateTableIndex != 0xFFFF )
                    {
                        XGINew_SetRegANDOR( pVBInfo->P3c4 , 0x06 , ~0x020 , 0x00 ) ;
                        XGI_SetCRT1Offset(  ModeNo , ModeIdIndex , RefreshRateTableIndex , HwDeviceExtension, pVBInfo ) ;
                    }
                    if (pVBInfo->VBInfo & SetCRT2ToLCDA) /* alan 09/12/2003 */
                    {
                      XGI_SetLCDAGroup(  ModeNo , ModeIdIndex , HwDeviceExtension, pVBInfo ) ;
                      if ( bCR12 == 0xDA )
                          XGINew_SetReg1( pVBInfo->P3d4 , 0x12 , bCR12 ) ;
                    }
                    else
                    {
                      bReturn = XGI_SetCRT2Group301( ModeNo , HwDeviceExtension, pVBInfo ) ;	/* add for CRT2 */
                      XGI_SetCRT2ModeRegs(  ModeNo , HwDeviceExtension, pVBInfo ) ;
                      XGI_EnableBridge( HwDeviceExtension, pVBInfo ) ;
                      if ( bCR12 == 0xDA )
                      {
                          XGINew_SetReg1( pVBInfo->Part1Port , 0x2F , 0x01 ) ;
                          XGINew_SetReg1( pVBInfo->Part1Port , 0x0E , bCR12 ) ;
                      }
                    }

                    bReturn = 1 ;
                }
                else
                    bReturn = 0 ;
                break ;

            case 4:
                break ;
            case 5:
                break ;
            case 6:
                break ;
            case 7:
                break ;
            case 8:
                break ;

            case 9:
                HDE = 1400 ;
                VDE = 1050 ;
                XGINew_SetReg1( pVBInfo->P3d4 , 0x39 , ( UCHAR )( bCR39 | 0x04 ) ) ;
                /* if ( HwDeviceExtension->jChipType == SIS_650M ) */
                /* XGINew_1400x1050Des_2 = ( XGI_LVDSDesStruct * )XGI651_1400x1050Des_2 ; */
                pVBInfo->VBInfo |= SetCRT2ToLCDA ;
                temp = XGI_SearchModeID( ModeNo , &ModeIdIndex, pVBInfo ) ;	/* 2.Get ModeID Table */
                if ( temp != 0 )
                {
                    XGINew_SetReg1( pVBInfo->P3c4 , 0x2B , pHWDE->bSR2B ) ;
                    XGINew_SetReg1( pVBInfo->P3c4 , 0x2C , pHWDE->bSR2C ) ;
                    XGINew_SetReg1( pVBInfo->P3c4 , 0x2D , 0x01 ) ;
                    RefreshRateTableIndex = XGI_GetRatePtrCRT2( ModeNo , ModeIdIndex, pVBInfo ) ;
                    if ( RefreshRateTableIndex != 0xFFFF )
                    {
                        XGINew_SetRegANDOR( pVBInfo->P3c4 , 0x06 , ~0x020 , 0x00 ) ;
                        XGI_SetCRT1Offset( ModeNo , ModeIdIndex , RefreshRateTableIndex , HwDeviceExtension, pVBInfo ) ;
                    }
                    XGI_SetLCDAGroup( ModeNo , ModeIdIndex , HwDeviceExtension, pVBInfo ) ;
                    if ( bCR12 == 0xDA )
                        XGINew_SetReg1( pVBInfo->P3d4 , 0x12 , bCR12 ) ;
                    bReturn = 1 ;
                }
                else
                    bReturn = 0;
                /* XGINew_SetReg1( pVBInfo->P3d4 , 0x39 , ( UCHAR )( bCR39 | 0x04 ) ) ; */
                break ;

            case 10:
                HDE = 1280 ;
                VDE = 768 ;
                pVBInfo->VBInfo |= SetCRT2ToLCDA ;
                temp = XGI_SearchModeID( ModeNo , &ModeIdIndex, pVBInfo ) ;	/* 2.Get ModeID Table */
                if ( temp != 0 )
                {
                    XGINew_SetReg1( pVBInfo->P3c4 , 0x2B , pHWDE->bSR2B ) ;
                    XGINew_SetReg1( pVBInfo->P3c4 , 0x2C , pHWDE->bSR2C ) ;
                    XGINew_SetReg1( pVBInfo->P3c4 , 0x2D , 0x01 ) ;
                    RefreshRateTableIndex = XGI_GetRatePtrCRT2( ModeNo , ModeIdIndex, pVBInfo ) ;
                    if ( RefreshRateTableIndex != 0xFFFF )
                    {
                        XGINew_SetRegANDOR( pVBInfo->P3c4 , 0x06 , ~0x020 , 0x00 ) ;
                        XGI_SetCRT1Offset( ModeNo , ModeIdIndex , RefreshRateTableIndex , HwDeviceExtension, pVBInfo ) ;
                    }
                    XGI_SetLCDAGroup( ModeNo , ModeIdIndex , HwDeviceExtension, pVBInfo ) ;
                    if ( bCR12 == 0xDA )
                        XGINew_SetReg1( pVBInfo->P3d4 , 0x12 , bCR12 ) ;

                    bReturn = 1 ;
                }
                else
                    bReturn = 0 ;

                break ;

            case 11:
                HDE = 1600 ;
                VDE = 1200 ;
                XGINew_SetReg1( pVBInfo->P3d4 , 0x39 , ( UCHAR )( bCR39 | 0x04 ) ) ;
                temp = XGI_SearchModeID( ModeNo , &ModeIdIndex, pVBInfo ) ;	/* 2.Get ModeID Table */
                if ( temp != 0 )
                {
                    RefreshRateTableIndex = XGI_GetRatePtrCRT2( ModeNo , ModeIdIndex, pVBInfo ) ;
                    if ( RefreshRateTableIndex != 0xFFFF )
                    {
                        XGINew_SetRegANDOR( pVBInfo->P3c4 , 0x06 , ~0x020 , 0x00 ) ;
                        XGI_SetCRT1Offset(  ModeNo , ModeIdIndex , RefreshRateTableIndex , HwDeviceExtension, pVBInfo ) ;
                    }
                    bReturn = XGI_SetCRT2Group301(ModeNo , HwDeviceExtension, pVBInfo ) ;	/* add for CRT2 */
                    XGI_SetCRT2ModeRegs(  ModeNo , HwDeviceExtension, pVBInfo ) ;
                    XGI_EnableBridge( HwDeviceExtension, pVBInfo ) ;
                    if ( bCR12 == 0xDA )
                    {
                        XGINew_SetReg1( pVBInfo->Part1Port , 0x2F , 0x01 ) ;
                        XGINew_SetReg1( pVBInfo->Part1Port , 0x0E , bCR12 ) ;
                    }
                }
                break ;

            default:
                break ;
        }

	/* alan reset vb */

        if ( !XGINew_IsDualEdge( pVBInfo ) )
        {
            if ( !XGINew_IsVAMode( pVBInfo) )
                temp = 0x80 ;
            else
                temp = 0x40 ;
        }
        else
        {
            temp = 0xC0 ;
        }
    }

    XGINew_SetRegOR( pVBInfo->Part4Port , 0x1F , temp ) ;

    if ( bReturn == 0 )
        return( ERROR_INVALID_PARAMETER ) ;
    else
        return( 1 ) ;
}


/* --------------------------------------------------------------------- */
/* Function : XGINew_TurnOffBacklight */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGINew_TurnOffBacklight( PHW_DEVICE_EXTENSION pHWDE )
{
    VIDEO_X86_BIOS_ARGUMENTS BIOSArguments ;
    ULONG       ultemp ;
    PUCHAR      pjCRTAddrPort , pjSeqAddrPort , pjCRT2Part1Port , pjCRT2Part4Port ;
    VP_STATUS   VPStatus ;

    pjCRTAddrPort = pHWDE->pjIOAddress + CRTC_ADDRESS_PORT_COLOR ;
    pjSeqAddrPort = pHWDE->pjIOAddress + SEQ_ADDRESS_PORT ;
    pjCRT2Part1Port = pHWDE->pjIOAddress + VB_PART1_OFFSET_04 ;
    pjCRT2Part4Port = pHWDE->pjIOAddress + VB_PART4_OFFSET_14 ;

    /* Turn-off LVDS panel backlight */
    if ( pHWDE->ulDispActualState & MASK_OUTPUTSTATE_LCD )
    {
        switch( pHWDE->bHasVB )
        {
            case VB2_301:
                switch( pHWDE->ujVBRevID )
                {
                    case 0xd0:	/* 301LV */
                    case 0xd1:
                    case 0xe0:	/* 302LV */
                    case 0xe1:
                        ExtPortSet( pjCRT2Part4Port , 0x26 , 0xFE , 0 ) ;
                        break ;

                    default:
                        break ;
                }
                break ;

            case VB_LVDS:	/* External LVDS chip */
                ExtPortSet( pjSeqAddrPort , IND_SR11_DDC_REG , 0xF7 , 0x08 ) ;
                break ;

            case VB2_Chrontel:	/* Chrontel LVDS */
                VideoPortZeroMemory( &BIOSArguments , sizeof( VIDEO_X86_BIOS_ARGUMENTS ) ) ;
                BIOSArguments.Eax = 0x140B;
                BIOSArguments.Ebx = 0x01;
                /* VPStatus = VideoPortInt10( pHWDE , &BIOSArguments ) ; */
                break ;

            default:
                break ;
        }
    }
}


/* --------------------------------------------------------------------- */
/* Function : XGINew_TurnOnBacklight */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGINew_TurnOnBacklight( PHW_DEVICE_EXTENSION pHWDE )
{
    VIDEO_X86_BIOS_ARGUMENTS BIOSArguments ;
    ULONG       ultemp ;
    PUCHAR      pjCRTAddrPort , pjSeqAddrPort , pjCRT2Part1Port , pjCRT2Part4Port ;
    VP_STATUS   VPStatus ;

    pjCRTAddrPort = pHWDE->pjIOAddress + CRTC_ADDRESS_PORT_COLOR ;
    pjSeqAddrPort = pHWDE->pjIOAddress + SEQ_ADDRESS_PORT ;
    pjCRT2Part1Port = pHWDE->pjIOAddress + VB_PART1_OFFSET_04 ;
    pjCRT2Part4Port = pHWDE->pjIOAddress + VB_PART4_OFFSET_14 ;

    if ( pHWDE->bMonitorPoweredOn )
    {
        /* Turn-on LVDS panel backlight */
        if ( pHWDE->ulDispActualState & MASK_OUTPUTSTATE_LCD )
        {
            switch (pHWDE->bHasVB)
            {
                case VB2_301:
                    switch( pHWDE->ujVBRevID )
                    {
                        case 0xd0:	/* 301LV */
                        case 0xd1:
                        case 0xe0:	/* 302LV */
                        case 0xe1:
                            for( ultemp = 0 ; ultemp < 0x10000 ; ultemp++ )
                            {
                                ExtPortIn( pjSeqAddrPort , 0x05 ) ;
                            }
                            ExtPortSet( pjCRT2Part4Port , 0x26 , 0xFF , 0x01 ) ;
                            break ;

                        default:
                            break ;
                   }
                   break;

                case VB_LVDS:	/* External LVDS chip */
                    ExtPortSet( pjSeqAddrPort , IND_SR11_DDC_REG , 0xF7 , 0x00 ) ;
                    break ;

                case VB2_Chrontel:	/* Chrontel LVDS */
                    VideoPortZeroMemory(&BIOSArguments, sizeof( VIDEO_X86_BIOS_ARGUMENTS ) ) ;
                    BIOSArguments.Eax = 0x140B ;
                    BIOSArguments.Ebx = 0x0 ;
                    /* VPStatus = VideoPortInt10( pHWDE , &BIOSArguments ) ; */
                    break ;

                default:
                    break ;
            }
        }
    }
}


/* --------------------------------------------------------------------- */
/* Function : XGINew_EnableWriteInternal */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGINew_EnableWriteInternal( PHW_DEVICE_EXTENSION pHWDE )
{
    PUCHAR XGINew_P3d4 ;
    UCHAR bCR51 , bCR56 ;

    XGINew_P3d4 = pHWDE->pjIOAddress + CRTC_ADDRESS_PORT_COLOR ;

    VideoPortWritePortUchar( XGINew_P3d4 , 0x51 ) ;
    bCR51 = VideoPortReadPortUchar( XGINew_P3d4 + 1 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 + 1 , ( UCHAR )( bCR51 & 0x7F ) ) ;
}


/* --------------------------------------------------------------------- */
/* Function : XGINew_DisableWriteInternal */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGINew_DisableWriteInternal( PHW_DEVICE_EXTENSION pHWDE )
{
    PUCHAR XGINew_P3d4 ;
    UCHAR bCR51 , bCR56 ;


    XGINew_P3d4 = pHWDE->pjIOAddress + CRTC_ADDRESS_PORT_COLOR ;

    VideoPortWritePortUchar( XGINew_P3d4 , 0x51 ) ;
    bCR51 = VideoPortReadPortUchar( XGINew_P3d4 + 1 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 + 1 , ( UCHAR )( bCR51 | 0x80 ) ) ;
}


/* --------------------------------------------------------------------- */
/* Function : XGINew_EnableWriteExternal */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGINew_EnableWriteExternal( PHW_DEVICE_EXTENSION pHWDE )
{
    PUCHAR XGINew_P3c2 , XGINew_P3cc , XGINew_P3c4 , XGINew_P3d4 ;
    UCHAR bCR51 , bCR56 ;

    XGINew_P3d4 = pHWDE->pjIOAddress + CRTC_ADDRESS_PORT_COLOR ;

    VideoPortWritePortUchar( XGINew_P3d4 , 0x51 ) ;
    bCR51 = VideoPortReadPortUchar( XGINew_P3d4 + 1 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 + 1 , ( UCHAR )( bCR51 & 0xBF ) ) ;
}


/* --------------------------------------------------------------------- */
/* Function : XGINew_DisableWriteExternal */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGINew_DisableWriteExternal( PHW_DEVICE_EXTENSION pHWDE )
{
    PUCHAR XGINew_P3c2 , XGINew_P3cc , XGINew_P3c4 , XGINew_P3d4 ;
    UCHAR bCR51 , bCR56 ;

    XGINew_P3d4 = pHWDE->pjIOAddress + CRTC_ADDRESS_PORT_COLOR ;

    VideoPortWritePortUchar( XGINew_P3d4 , 0x51 ) ;
    bCR51 = VideoPortReadPortUchar( XGINew_P3d4 + 1 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 + 1 , ( UCHAR )( bCR51 | 0x40 ) ) ;
}


/* --------------------------------------------------------------------- */
/* Function : XGINew_EnableWriteBackup */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGINew_EnableWriteBackup( PHW_DEVICE_EXTENSION pHWDE )
{
    PUCHAR XGINew_P3d4 ;
    UCHAR bCR51 , bCR56 ;

    XGINew_P3d4 = pHWDE->pjIOAddress + CRTC_ADDRESS_PORT_COLOR ;

    VideoPortWritePortUchar( XGINew_P3d4 , 0x51 ) ;
    bCR51 = VideoPortReadPortUchar( XGINew_P3d4 + 1 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 + 1 , ( UCHAR )( bCR51 & 0xDF ) ) ;
}


/* --------------------------------------------------------------------- */
/* Function : XGINew_DisableWriteBackup */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGINew_DisableWriteBackup( PHW_DEVICE_EXTENSION pHWDE )
{
    PUCHAR XGINew_P3d4 ;
    UCHAR bCR51 , bCR56 ;

    XGINew_P3d4 = pHWDE->pjIOAddress + CRTC_ADDRESS_PORT_COLOR ;

    VideoPortWritePortUchar( XGINew_P3d4 , 0x51 ) ;
    bCR51 = VideoPortReadPortUchar( XGINew_P3d4 + 1 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 + 1 , ( UCHAR )( bCR51 | 0x20 ) ) ;
}


/* --------------------------------------------------------------------- */
/* Function : XGINew_SetReadInternal */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGINew_SetReadInternal( PHW_DEVICE_EXTENSION pHWDE )
{
    PUCHAR XGINew_P3d4 ;
    UCHAR bCR51 , bCR56 ;

    XGINew_P3d4 = pHWDE->pjIOAddress + CRTC_ADDRESS_PORT_COLOR ;

    VideoPortWritePortUchar( XGINew_P3d4 , 0x56 ) ;
    bCR56 = VideoPortReadPortUchar( XGINew_P3d4 + 1 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 + 1 , ( UCHAR )( bCR56 & 0xE7 ) ) ;
}


/* --------------------------------------------------------------------- */
/* Function : */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGINew_SetReadExternal( PHW_DEVICE_EXTENSION pHWDE )
{
    PUCHAR XGINew_P3d4 ;
    UCHAR bCR51 , bCR56 ;

    XGINew_P3d4 = pHWDE->pjIOAddress + CRTC_ADDRESS_PORT_COLOR ;

    VideoPortWritePortUchar( XGINew_P3d4 , 0x56 ) ;
    bCR56 = VideoPortReadPortUchar( XGINew_P3d4 + 1 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 + 1 , ( UCHAR )( ( bCR56 & 0xE7 ) | 0x08 ) ) ;
}


/* --------------------------------------------------------------------- */
/* Function : XGINew_ExternalToBackup */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGINew_ExternalToBackup( PHW_DEVICE_EXTENSION pHWDE )
{
    PUCHAR      XGINew_P3c2 , XGINew_P3cc , XGINew_P3c4 , XGINew_P3d4 ;
    UCHAR       bCR56 , bCR51 ;
    UCHAR       temp ;
    UCHAR       bCR0 , bCR1 , bCR2 , bCR3 , bCR4 , bCR5 , bCR6 , bCR7 ;
    UCHAR       bCR9 , bCR10 , bCR11 , bCR12 , bCR15 , bCR16 , bCR34 ;
    UCHAR       bSR0A , bSR0B , bSR0C ;
    UCHAR       bSR1 ;
    UCHAR       b3CC ;
    UCHAR       Key ;

    XGINew_P3c4 = pHWDE->pjIOAddress + SEQ_ADDRESS_PORT ;
    XGINew_P3c2 = pHWDE->pjIOAddress + MISC_OUTPUT_REG_WRITE_PORT ;
    XGINew_P3cc = pHWDE->pjIOAddress + MISC_OUTPUT_REG_READ_PORT ;
    XGINew_P3d4 = pHWDE->pjIOAddress + CRTC_ADDRESS_PORT_COLOR ;

    VideoPortWritePortUchar( XGINew_P3d4 , 0x56 ) ;
    bCR56 = VideoPortReadPortUchar( XGINew_P3d4 + 1 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 + 1 , ( UCHAR )( ( bCR56 & 0xE7 ) | 0x10 ) ) ;

    VideoPortWritePortUchar( XGINew_P3d4 , 0x51 ) ;
    bCR51 = VideoPortReadPortUchar( XGINew_P3d4 + 1 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 + 1 , ( UCHAR )( ( bCR51 & 0xDF ) | 0xC0 ) ) ;

    VideoPortWritePortUchar( XGINew_P3d4 , 0x34 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 + 1 , pHWDE->bCR34 ) ;

    VideoPortWritePortUchar( XGINew_P3d4 ,0x11);
    VideoPortWritePortUchar( XGINew_P3d4 + 1 , ( UCHAR )( ( VideoPortReadPortUchar( XGINew_P3d4 + 1 ) & 0x7F ) ) ) ;

    VideoPortWritePortUchar( XGINew_P3d4 , 0x0 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 + 1 , pHWDE->bCR0 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 , 0x1 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 + 1 , pHWDE->bCR1 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 , 0x2 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 + 1 , pHWDE->bCR2 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 , 0x3 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 + 1 , pHWDE->bCR3 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 , 0x4 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 + 1 , pHWDE->bCR4 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 , 0x5 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 + 1 , pHWDE->bCR5 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 , 0x6 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 + 1 , pHWDE->bCR6 ) ;

    VideoPortWritePortUchar( XGINew_P3d4 , 0x7 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 + 1 , ( UCHAR )( ( VideoPortReadPortUchar( XGINew_P3d4 + 1 ) & 0x10 ) | ( ( pHWDE->bCR7 ) & 0xEF ) ) ) ;
    VideoPortWritePortUchar( XGINew_P3d4 , 0x9 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 + 1 , ( UCHAR )( ( VideoPortReadPortUchar( XGINew_P3d4 + 1 ) & 0x5F ) | ( ( pHWDE->bCR9 ) & 0xA0 ) ) ) ;

    VideoPortWritePortUchar( XGINew_P3d4 , 0x10 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 + 1 , pHWDE->bCR10 ) ;

    VideoPortWritePortUchar( XGINew_P3d4 , 0x11 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 + 1 , ( UCHAR )( ( VideoPortReadPortUchar( XGINew_P3d4 + 1 ) & 0xF0 ) | ( ( pHWDE->bCR11 ) & 0x0F ) ) ) ;

    VideoPortWritePortUchar( XGINew_P3d4 , 0x12 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 + 1 , pHWDE->bCR12 ) ;

    VideoPortWritePortUchar( XGINew_P3d4 , 0x15 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 + 1 , pHWDE->bCR15 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 , 0x16 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 + 1 , pHWDE->bCR16 ) ;


    VideoPortWritePortUchar( XGINew_P3c2 , ( UCHAR )( ( VideoPortReadPortUchar( XGINew_P3cc ) & 0x33 ) | ( ( pHWDE->b3CC ) & 0xCC ) ) ) ;

    VideoPortWritePortUchar( XGINew_P3c4 , 0x5 ) ;
    Key = VideoPortReadPortUchar( XGINew_P3c4 + 1 ) ;
    VideoPortWritePortUshort( ( PUSHORT )XGINew_P3c4 , 0x8605 ) ;

    VideoPortWritePortUchar( XGINew_P3c4   , 0x0A ) ;
    VideoPortWritePortUchar( XGINew_P3c4 + 1 , pHWDE->bSR0A ) ;
    VideoPortWritePortUchar( XGINew_P3c4 , 0x0B ) ;
    VideoPortWritePortUchar( XGINew_P3c4 + 1 , pHWDE->bSR0B ) ;
    VideoPortWritePortUchar( XGINew_P3c4 , 0x0C ) ;
    VideoPortWritePortUchar( XGINew_P3c4 + 1 , ( UCHAR )( ( VideoPortReadPortUchar( XGINew_P3c4 + 1 ) & 0xF8 ) | ( ( pHWDE->bSR0C ) & 0x07 ) ) ) ;

    if ( Key != 0xA1 )
        VideoPortWritePortUshort( ( PUSHORT )XGINew_P3c4 , 0x0005 ) ;

    VideoPortWritePortUchar( XGINew_P3d4 , 0x51 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 + 1 , bCR51 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 , 0x56 ) ;
    VideoPortWritePortUchar( XGINew_P3d4 + 1 , bCR56 ) ;
}


/* --------------------------------------------------------------------- */
/* Function : XGINew_GRandAR13Changed */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGINew_GRandAR13Changed(PHW_DEVICE_EXTENSION pHWDE )
{
    PUCHAR XGINew_P3ce , XGINew_P3da , XGINew_P3c0 , XGINew_P3d4 , XGINew_P3c4 ;
    UCHAR AR13 , GR6 , ModeNo , GRdata , temp , SRdata ;
    USHORT ModeIdIndex , i , modeflag , index ;
    BOOL bTimingChanged = 0 ;

/*
    PXGI_HW_DEVICE_INFO HwDeviceExtension = pHWDE->pXGIHWDE;
    PUCHAR   pVBInfo->ROMAddr  = (ULONG)HwDeviceExtension->pjVirtualRomBase;

    XGINew_SModeIDTable = (XGI_StStruct *) XGI310_SModeIDTable;
    XGINew_StandTable = (XGI_StandTableStruct *) XGI310_StandTable;
    XGINew_EModeIDTable = (XGI_ExtStruct *) XGI310_EModeIDTable;

    XGINew_P3ce = pHWDE->pjIOAddress + GRAPH_ADDRESS_PORT;
    XGINew_P3da = pHWDE->pjIOAddress + 0x2A;
    XGINew_P3c0 = pHWDE->pjIOAddress + ATT_ADDRESS_PORT;
    XGINew_P3c4 = pHWDE->pjIOAddress + SEQ_ADDRESS_PORT;
    XGINew_P3d4 = pHWDE->pjIOAddress + CRTC_ADDRESS_PORT_COLOR;

    VideoPortWritePortUchar(XGINew_P3d4,0x34);
    ModeNo = VideoPortReadPortUchar(XGINew_P3d4+1);
    temp = XGI_SearchModeID(pVBInfo->ROMAddr,ModeNo,&ModeIdIndex, pVBInfo);
    if(temp!=0)
    {
        if (ModeNo<=0x13)
        {
            index = XGINew_SModeIDTable[ModeIdIndex].St_StTableIndex;
            modeflag = XGINew_SModeIDTable[ModeIdIndex].St_ModeFlag;
            modeflag = modeflag&ModeInfoFlag;
        }
        else
        {
            modeflag = XGINew_EModeIDTable[ModeIdIndex].Ext_ModeFlag;
            modeflag = modeflag&ModeInfoFlag;
            if ( modeflag <= 0x02 )
                index=0x1B;
            else
                index=0x0F;
        }
        VideoPortWritePortUchar(XGINew_P3ce,0x06);
        GR6 = VideoPortReadPortUchar(XGINew_P3ce+1);
        GRdata = XGINew_StandTable[index].GRC[6];

        if ( GR6 != GRdata )
        {
            for(i=0;i<=0x08;i++)
            {
                GRdata = XGINew_StandTable[index].GRC[i];
                VideoPortWritePortUchar(XGINew_P3ce,(UCHAR)i);
                VideoPortWritePortUchar(XGINew_P3ce+1,GRdata);
            }
            if(modeflag>ModeVGA)
            {
                VideoPortWritePortUchar(XGINew_P3ce,0x05);
                GRdata = VideoPortReadPortUchar(XGINew_P3ce+1);
                GRdata = GRdata&0xBF;
                VideoPortWritePortUchar(XGINew_P3ce+1,GRdata);
            }

            for(i=02;i<=04;i++)
            {
                SRdata=XGINew_StandTable[index].SR[i-1];
                VideoPortWritePortUchar(XGINew_P3c4,(UCHAR)i);
                VideoPortWritePortUchar(XGINew_P3c4+1,SRdata);
            }
        }
    }

    if(pHWDE->bAR13==0)
    {
        AR13 = VideoPortReadPortUchar(XGINew_P3da);
        VideoPortWritePortUchar(XGINew_P3c0,0x33);
        AR13 = VideoPortReadPortUchar(XGINew_P3c0+1);
        if (AR13!=0)
        {
            AR13 = VideoPortReadPortUchar(XGINew_P3da);
            VideoPortWritePortUchar(XGINew_P3c0,0x33);
            VideoPortWritePortUchar(XGINew_P3c0,0x00);
        }
    }
*/
}

/* #endif WIN2000 INTA solution for WIN2000 */
