/* $XFree86: xc/programs/Xserver/hw/xfree86/drivers/xgi/vb_setmode.h,v 1.1 2005/05/02 13:28:02 dawes Exp $ */

#ifndef  _VBSETMODE_
#define  _VBSETMODE_

extern   void     XGI_InitTo330Pointer(UCHAR,PVB_DEVICE_INFO);
extern   void     XGI_UnLockCRT2(PXGI_HW_DEVICE_INFO HwDeviceExtension, PVB_DEVICE_INFO );
extern   void     XGI_LockCRT2(PXGI_HW_DEVICE_INFO HwDeviceExtension, PVB_DEVICE_INFO );
extern   void     XGI_LongWait( PVB_DEVICE_INFO );
extern   void     XGI_SetCRT2ModeRegs(USHORT ModeNo,PXGI_HW_DEVICE_INFO,  PVB_DEVICE_INFO  );
extern   void     XGI_DisableBridge(PXGI_HW_DEVICE_INFO HwDeviceExtension, PVB_DEVICE_INFO );
extern   void  	  XGI_EnableBridge(PXGI_HW_DEVICE_INFO HwDeviceExtension, PVB_DEVICE_INFO );
extern   void     XGI_DisplayOff( PVB_DEVICE_INFO );
extern   void     XGI_DisplayOn( PVB_DEVICE_INFO );
extern   void     XGI_GetVBType(PVB_DEVICE_INFO);
extern   void     XGI_SenseCRT1(PVB_DEVICE_INFO );
extern   void     XGI_GetVGAType(PXGI_HW_DEVICE_INFO HwDeviceExtension, PVB_DEVICE_INFO );
extern   void     XGI_GetVBInfo(USHORT ModeNo,USHORT ModeIdIndex,PXGI_HW_DEVICE_INFO HwDeviceExtension, PVB_DEVICE_INFO );
extern   void     XGI_GetTVInfo(USHORT ModeNo,USHORT ModeIdIndex, PVB_DEVICE_INFO );
extern   void     XGI_SetCRT1Offset(USHORT ModeNo,USHORT ModeIdIndex,USHORT RefreshRateTableIndex,PXGI_HW_DEVICE_INFO HwDeviceExtension, PVB_DEVICE_INFO );
extern   void     XGI_SetLCDAGroup(USHORT ModeNo,USHORT ModeIdIndex,PXGI_HW_DEVICE_INFO HwDeviceExtension, PVB_DEVICE_INFO );
extern   void     XGI_WaitDisply( PVB_DEVICE_INFO );

extern   BOOLEAN  XGISetModeNew( PXGI_HW_DEVICE_INFO HwDeviceExtension , USHORT ModeNo ) ;
extern   BOOLEAN  XGI_CheckDualChip(PVB_DEVICE_INFO );
extern   BOOLEAN  XGI_SearchModeID( USHORT ModeNo,USHORT  *ModeIdIndex, PVB_DEVICE_INFO );
extern   BOOLEAN  XGI_GetLCDInfo(USHORT ModeNo,USHORT ModeIdIndex,PVB_DEVICE_INFO );
extern   BOOLEAN  XGI_BridgeIsOn( PVB_DEVICE_INFO );
extern   BOOLEAN  XGI_SetCRT2Group301(USHORT ModeNo, PXGI_HW_DEVICE_INFO HwDeviceExtension, PVB_DEVICE_INFO);
extern   USHORT   XGI_GetRatePtrCRT2( USHORT ModeNo,USHORT ModeIdIndex, PVB_DEVICE_INFO );


#endif
