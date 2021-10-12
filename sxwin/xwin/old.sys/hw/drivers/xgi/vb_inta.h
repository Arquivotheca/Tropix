/* $XFree86: xc/programs/Xserver/hw/xfree86/drivers/xgi/vb_inta.h,v 1.1 2005/05/02 13:28:02 dawes Exp $ */

#ifndef  _VBINTA_
#define  _VBINTA_

extern  void     SiSLVDSCH7019ControlCallback(PHW_DEVICE_EXTENSION pHWDE,PVOID Context);
extern  void     SiSTVCH7019ControlCallback(PHW_DEVICE_EXTENSION pHWDE, PVOID Context);
extern  void     XGINew_ExternalToBackup(PHW_DEVICE_EXTENSION pHWDE);
extern  void     XGINew_SetReadExternal(PHW_DEVICE_EXTENSION pHWDE);
extern  void     XGINew_SetReadInternal(PHW_DEVICE_EXTENSION pHWDE);
extern  void     XGINew_DisableWriteBackup(PHW_DEVICE_EXTENSION pHWDE);
extern  void     XGINew_EnableWriteBackup(PHW_DEVICE_EXTENSION pHWDE);
extern  void     XGINew_TurnOnBacklight(PHW_DEVICE_EXTENSION   pHWDE);
extern  int      XGINew_WantToSetMode(PHW_DEVICE_EXTENSION pHWDE);
extern  int      XGINew_WantToChangeTimingRegister(PHW_DEVICE_EXTENSION pHWDE);
extern  int      XGINew_ReSet301VBTiming(PHW_DEVICE_EXTENSION   pHWDE);
extern  void     XGINew_TurnOffBacklight(PHW_DEVICE_EXTENSION   pHWDE);
extern  void     XGINew_EnableWriteInternal(PHW_DEVICE_EXTENSION pHWDE);
extern  void     XGINew_DisableWriteInternal(PHW_DEVICE_EXTENSION pHWDE);
extern  void     XGINew_EnableWriteExternal(PHW_DEVICE_EXTENSION pHWDE);
extern  void     XGINew_DisableWriteExternal(PHW_DEVICE_EXTENSION pHWDE);
extern  void     XGINew_GRandAR13Changed(PHW_DEVICE_EXTENSION   pHWDE);
extern  BOOLEAN  XGINew_IsVAMode(PVB_DEVICE_INFO pVBInfo);

#endif
