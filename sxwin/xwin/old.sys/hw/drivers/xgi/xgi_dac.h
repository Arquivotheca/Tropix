/* $XFree86: xc/programs/Xserver/hw/xfree86/drivers/xgi/xgi_dac.h,v 1.1 2005/05/02 13:28:02 dawes Exp $ */
/*
 * DAC helper functions (Save/Restore, MemClk, etc)
 * Definitions and prototypes
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
 */

void XGIDACPreInit(ScrnInfoPtr pScrn);
void XGILoadPalette(ScrnInfoPtr pScrn, int numColors, int *indices,
		        LOCO *colors, VisualPtr pVisual);
void XGICalcClock(ScrnInfoPtr pScrn, int clock, int max_VLD,
                        unsigned int *vclk);
void XGIIODump(ScrnInfoPtr pScrn);
int  XG40_MemBandWidth(ScrnInfoPtr pScrn);
int  XGIMclk(XGIPtr pXGI);
void XGIRestoreBridge(ScrnInfoPtr pScrn, XGIRegPtr xgiReg);

extern int      XGICalcVRate(DisplayModePtr mode);

/* Functions from init.c and init301.c */

extern void     XGI_New_EnableBridge(XGI_Private *XGI_Pr, PXGI_HW_DEVICE_INFO);
extern USHORT 	XGI_GetCH700x(XGI_Private *XGI_Pr, USHORT tempbx);
extern void 	XGI_SetCH700x(XGI_Private *XGI_Pr, USHORT tempbx);
extern USHORT 	XGI_GetCH701x(XGI_Private *XGI_Pr, USHORT tempbx);
extern void 	XGI_SetCH701x(XGI_Private *XGI_Pr, USHORT tempbx);
extern USHORT 	XGI_GetCH70xx(XGI_Private *XGI_Pr, USHORT tempbx);
extern void 	XGI_SetCH70xx(XGI_Private *XGI_Pr, USHORT tempbx);
extern void     XGI_SetCH70xxANDOR(XGI_Private *XGI_Pr, USHORT tempax,USHORT tempbh);
extern void     XGI_SetTrumpReg(XGI_Private *XGI_Pr, USHORT tempbx);
extern USHORT   XGI_GetTrumpReg(XGI_Private *XGI_Pr, USHORT tempbx);
extern void     XGI_DDC2Delay(XGI_Private *XGI_Pr, USHORT delaytime);
extern void     XGI_SetChrontelGPIO(XGI_Private *XGI_Pr, USHORT myvbinfo);
extern void     XGI_New_DisplayOn(XGI_Private *XGI_Pr);
extern unsigned char XGI_GetSetModeID(ScrnInfoPtr pScrn, unsigned char id);
extern void     XGI_SetEnableDstn(XGI_Private *XGI_Pr, int enable);
extern void     XGI_SetEnableFstn(XGI_Private *XGI_Pr, int enable);
