/* $XFree86: xc/programs/Xserver/hw/xfree86/drivers/xgi/vstruct.h,v 1.1 2005/05/02 13:28:02 dawes Exp $ */
/*
 * General structure definitions for universal mode switching modules
 *
 * Copyright (C) 2001-2004 by Thomas Winischhofer, Vienna, Austria
 *
 * If distributed as part of the Linux kernel, the following license terms
 * apply:
 *
 * * This program is free software; you can redistribute it and/or modify
 * * it under the terms of the GNU General Public License as published by
 * * the Free Software Foundation; either version 2 of the named License,
 * * or any later version.
 * *
 * * This program is distributed in the hope that it will be useful,
 * * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * * GNU General Public License for more details.
 * *
 * * You should have received a copy of the GNU General Public License
 * * along with this program; if not, write to the Free Software
 * * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA
 *
 * Otherwise, the following license terms apply:
 *
 * * Redistribution and use in source and binary forms, with or without
 * * modification, are permitted provided that the following conditions
 * * are met:
 * * 1) Redistributions of source code must retain the above copyright
 * *    notice, this list of conditions and the following disclaimer.
 * * 2) Redistributions in binary form must reproduce the above copyright
 * *    notice, this list of conditions and the following disclaimer in the
 * *    documentation and/or other materials provided with the distribution.
 * * 3) The name of the author may not be used to endorse or promote products
 * *    derived from this software without specific prior written permission.
 * *
 * * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESSED OR
 * * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Author: 	Thomas Winischhofer <thomas@winischhofer.net>
 *
 */
/*
#ifdef _INIT_
#define EXTERN
#else
#define EXTERN extern
#endif 
*/
#ifndef _VSTRUCT_
#define _VSTRUCT_

typedef struct _XGI_New_PanelDelayTblStruct
{
 	UCHAR timer[2];
} XGI_New_PanelDelayTblStruct;
/*
typedef struct _XGI_LCDDataStruct
{
	USHORT RVBHCMAX;
	USHORT RVBHCFACT;
	USHORT VGAHT;
	USHORT VGAVT;
	USHORT LCDHT;
	USHORT LCDVT;
} XGI_LCDDataStruct;

typedef struct _XGI_TVDataStruct
{
	USHORT RVBHCMAX;
	USHORT RVBHCFACT;
	USHORT VGAHT;
	USHORT VGAVT;
	USHORT TVHDE;
	USHORT TVVDE;
	USHORT RVBHRS;
	UCHAR  FlickerMode;
	USHORT HALFRVBHRS;
	UCHAR  RY1COE;
	UCHAR  RY2COE;
	UCHAR  RY3COE;
	UCHAR  RY4COE;
} XGI_TVDataStruct;

typedef struct _XGI_LVDSDataStruct
{
	USHORT VGAHT;
	USHORT VGAVT;
	USHORT LCDHT;
	USHORT LCDVT;
} XGI_LVDSDataStruct;

typedef struct _XGI_LVDSDesStruct
{
	USHORT LCDHDES;
	USHORT LCDVDES;
} XGI_LVDSDesStruct;

typedef struct _XGI_LVDSCRT1DataStruct
{
	UCHAR  CR[15];
} XGI_LVDSCRT1DataStruct;
*/
typedef struct _XGI_LCDACRT1DataStruct
{
	UCHAR  CR[17];
} XGI_LCDACRT1DataStruct;

typedef struct _XGI_CHTVRegDataStruct
{
	UCHAR  Reg[16];
} XGI_CHTVRegDataStruct;
/*
typedef struct _XGI_StStruct
{
	UCHAR  St_ModeID;
	USHORT St_ModeFlag;
	UCHAR  St_StTableIndex;
	UCHAR  St_CRT2CRTC;
	UCHAR  St_ResInfo;
	UCHAR  VB_StTVFlickerIndex;
	UCHAR  VB_StTVEdgeIndex;
	UCHAR  VB_StTVYFilterIndex;
	UCHAR  St_PDC;
} XGI_StStruct;
*/
typedef struct _XGI_VBModeStruct
{
	UCHAR  ModeID;
	UCHAR  VB_TVDelayIndex;
	UCHAR  VB_TVFlickerIndex;
	UCHAR  VB_TVPhaseIndex;
	UCHAR  VB_TVYFilterIndex;
	UCHAR  VB_LCDDelayIndex;
	UCHAR  _VB_LCDHIndex;
	UCHAR  _VB_LCDVIndex;
} XGI_VBModeStruct;
/*
typedef struct _XGI_New_StandTableStruct
{
	UCHAR  CRT_COLS;
	UCHAR  ROWS;
	UCHAR  CHAR_HEIGHT;
	USHORT CRT_LEN;
	UCHAR  SR[4];
	UCHAR  MISC;
	UCHAR  CRTC[0x19];
	UCHAR  ATTR[0x14];
	UCHAR  GRC[9];
} XGI_New_StandTableStruct;

typedef struct _XGI_ExtStruct
{
	UCHAR  Ext_ModeID;
	USHORT Ext_ModeFlag;
	USHORT Ext_VESAID;
	UCHAR  Ext_RESINFO;
	UCHAR  VB_ExtTVFlickerIndex;
	UCHAR  VB_ExtTVEdgeIndex;
	UCHAR  VB_ExtTVYFilterIndex;
	UCHAR  VB_ExtTVYFilterIndexROM661;
	UCHAR  REFindex;
} XGI_ExtStruct;

typedef struct _XGI_Ext2Struct
{
	USHORT Ext_InfoFlag;
	UCHAR  Ext_CRT1CRTC;
	UCHAR  Ext_CRTVCLK;
	UCHAR  Ext_CRT2CRTC;
	UCHAR  Ext_CRT2CRTC_NS;
	UCHAR  ModeID;
	USHORT XRes;
	USHORT YRes;
	UCHAR  Ext_PDC;
} XGI_Ext2Struct;
*/
typedef struct _XGI_Part2PortTblStruct
{
 	UCHAR  CR[12];
} XGI_Part2PortTblStruct;
/*
typedef struct _XGI_CRT1TableStruct
{
	UCHAR  CR[17];
} XGI_CRT1TableStruct;

typedef struct _XGI_MCLKDataStruct
{
	UCHAR  SR28,SR29,SR2A;
	USHORT CLOCK;
} XGI_MCLKDataStruct;

typedef struct _XGI_VCLKDataStruct
{
	UCHAR  SR2B,SR2C;
	USHORT CLOCK;
} XGI_VCLKDataStruct;

typedef struct _XGI_VBVCLKDataStruct
{
	UCHAR  Part4_A,Part4_B;
	USHORT CLOCK;
} XGI_VBVCLKDataStruct;
*/
typedef struct _XGI_New_StResInfoStruct
{
	USHORT HTotal;
	USHORT VTotal;
} XGI_New_StResInfoStruct;

typedef struct _XGI_New_ModeResInfoStruct
{
	USHORT HTotal;
	USHORT VTotal;
	UCHAR  XChar;
	UCHAR  YChar;
} XGI_New_ModeResInfoStruct;



typedef UCHAR DRAM4sType[4];

/* Defines for XGI_CustomT */
/* Never change these for xgifb compatibility */
#define CUT_NONE          0
#define CUT_FORCENONE     1
#define CUT_BARCO1366     2
#define CUT_BARCO1024     3
#define CUT_COMPAQ1280    4
#define CUT_COMPAQ12802   5
#define CUT_PANEL848      6
#define CUT_CLEVO1024     7
#define CUT_CLEVO10242    8
#define CUT_CLEVO1400     9
#define CUT_CLEVO14002    10
#define CUT_UNIWILL1024   11
#define CUT_ASUSL3000D    12
#define CUT_UNIWILL10242  13
#define CUT_ACER1280      14
#define CUT_COMPAL1400_1  15
#define CUT_COMPAL1400_2  16
#define CUT_ASUSA2H_1     17
#define CUT_ASUSA2H_2     18

typedef struct _XGI_Private
{
#ifdef LINUX_KERNEL
        XGIIOADDRESS RelIO;
#endif
	XGIIOADDRESS XGI_P3c4;
	XGIIOADDRESS XGI_P3d4;
	XGIIOADDRESS XGI_P3c0;
	XGIIOADDRESS XGI_P3ce;
	XGIIOADDRESS XGI_P3c2;
	XGIIOADDRESS XGI_P3ca;
	XGIIOADDRESS XGI_P3c6;
	XGIIOADDRESS XGI_P3c7;
	XGIIOADDRESS XGI_P3c8;
	XGIIOADDRESS XGI_P3c9;
	XGIIOADDRESS XGI_P3cb;
	XGIIOADDRESS XGI_P3cd;
	XGIIOADDRESS XGI_P3da;
	XGIIOADDRESS XGI_Part1Port;
	XGIIOADDRESS XGI_Part2Port;
	XGIIOADDRESS XGI_Part3Port;
	XGIIOADDRESS XGI_Part4Port;
	XGIIOADDRESS XGI_Part5Port;
	XGIIOADDRESS XGI_VidCapt;
	XGIIOADDRESS XGI_VidPlay;
	USHORT XGI_IF_DEF_LVDS;
	USHORT XGI_IF_DEF_CH70xx;
	USHORT XGI_IF_DEF_CONEX;
	USHORT XGI_IF_DEF_TRUMPION;
	USHORT XGI_IF_DEF_DSTN;
	USHORT XGI_IF_DEF_FSTN;
	USHORT XGI_SysFlags;
	UCHAR  XGI_VGAINFO;
#ifndef LINUX_KERNEL
        USHORT XGI_CP1, XGI_CP2, XGI_CP3, XGI_CP4;
#endif
	BOOLEAN XGI_UseROM;
	BOOLEAN XGI_ROMNew;
	BOOLEAN PanelSelfDetected;
	int     XGI_CHOverScan;
	BOOLEAN XGI_CHSOverScan;
	BOOLEAN XGI_ChSW;
	BOOLEAN XGI_UseLCDA;
	int     XGI_UseOEM;
	ULONG   XGI_CustomT;
	USHORT  XGI_Backup70xx;
	BOOLEAN HaveEMI;
	BOOLEAN HaveEMILCD;
	BOOLEAN OverruleEMI;
	UCHAR  EMI_30,EMI_31,EMI_32,EMI_33;
	SHORT  PDC, PDCA;
	UCHAR  XGI_MyCR63;
	USHORT XGI_CRT1Mode;
	USHORT XGI_flag_clearbuffer;
	int    XGI_RAMType;
	UCHAR  XGI_ChannelAB;
	UCHAR  XGI_DataBusWidth;
	USHORT XGI_ModeType;
	USHORT XGI_VBInfo;
	USHORT XGI_TVMode;
	USHORT XGI_LCDResInfo;
	USHORT XGI_LCDTypeInfo;
	USHORT XGI_LCDInfo;
	USHORT XGI_LCDInfo661;
	USHORT XGI_VBType;
	USHORT XGI_VBExtInfo;
	USHORT XGI_YPbPr;
	USHORT XGI_SelectCRT2Rate;
	USHORT XGI_SetFlag;
	USHORT XGI_RVBHCFACT;
	USHORT XGI_RVBHCMAX;
	USHORT XGI_RVBHRS;
	USHORT XGI_VGAVT;
	USHORT XGI_VGAHT;
	USHORT XGI_VT;
	USHORT XGI_HT;
	USHORT XGI_VGAVDE;
	USHORT XGI_VGAHDE;
	USHORT XGI_VDE;
	USHORT XGI_HDE;
	USHORT XGI_NewFlickerMode;
	USHORT XGI_RY1COE;
	USHORT XGI_RY2COE;
	USHORT XGI_RY3COE;
	USHORT XGI_RY4COE;
	USHORT XGI_LCDHDES;
	USHORT XGI_LCDVDES;
	USHORT XGI_DDC_Port;
	USHORT XGI_DDC_Index;
	USHORT XGI_DDC_Data;
	USHORT XGI_DDC_NData;
	USHORT XGI_DDC_Clk;
	USHORT XGI_DDC_NClk;
	USHORT XGI_DDC_DeviceAddr;
	USHORT XGI_DDC_ReadAddr;
	USHORT XGI_DDC_SecAddr;
	USHORT XGI_ChrontelInit;
	BOOLEAN XGI_SensibleSR11;
	

	USHORT XGI_PanelMinLVDS;
	USHORT XGI_PanelMin301;

	const XGI_StStruct          *XGI_SModeIDTable;
	XGI_New_StandTableStruct        *XGI_StandTable;
	const XGI_ExtStruct         *XGI_EModeIDTable;
	const XGI_Ext2Struct        *XGI_RefIndex;
	const XGI_VBModeStruct      *XGI_VBModeIDTable;
	const XGI_CRT1TableStruct   *XGI_CRT1Table;
	const XGI_MCLKDataStruct    *XGI_MCLKData_0;
	const XGI_MCLKDataStruct    *XGI_MCLKData_1;
	XGI_VCLKDataStruct    	    *XGI_VCLKData;
	XGI_VBVCLKDataStruct        *XGI_VBVCLKData;
	const XGI_New_StResInfoStruct   *XGI_StResInfo;
	const XGI_New_ModeResInfoStruct *XGI_ModeResInfo;

	const UCHAR                 *pXGI_OutputSelect;
	const UCHAR                 *pXGI_SoftSetting;

	const DRAM4sType *XGI_SR15; /* pointer : point to array */
#ifndef LINUX_XF86
	UCHAR *pXGI_SR07;
	const DRAM4sType *XGI_CR40; /* pointer : point to array */
	UCHAR *XGI_CR49;
	UCHAR *XGI_SR25;
	UCHAR *pXGI_SR1F;
	UCHAR *pXGI_SR21;
	UCHAR *pXGI_SR22;
	UCHAR *pXGI_SR23;
	UCHAR *pXGI_SR24;
	UCHAR *pXGI_SR31;
	UCHAR *pXGI_SR32;
	UCHAR *pXGI_SR33;
	UCHAR *pXGI_CRT2Data_1_2;
	UCHAR *pXGI_CRT2Data_4_D;
	UCHAR *pXGI_CRT2Data_4_E;
	UCHAR *pXGI_CRT2Data_4_10;
	const USHORT *pXGI_RGBSenseData;
	const USHORT *pXGI_VideoSenseData;
	const USHORT *pXGI_YCSenseData;
	const USHORT *pXGI_RGBSenseData2;
	const USHORT *pXGI_VideoSenseData2;
	const USHORT *pXGI_YCSenseData2;
#endif

	const XGI_New_PanelDelayTblStruct *XGI_PanelDelayTbl;
	const XGI_New_PanelDelayTblStruct *XGI_PanelDelayTblLVDS;

	/*  bridge */

	const UCHAR *XGI_NTSCPhase;
	const UCHAR *XGI_PALPhase;
	const UCHAR *XGI_NTSCPhase2;
	const UCHAR *XGI_PALPhase2;
	const UCHAR *XGI_PALMPhase;
	const UCHAR *XGI_PALNPhase;
	const UCHAR *XGI_PALMPhase2;
	const UCHAR *XGI_PALNPhase2;
	const UCHAR *XGI_SpecialPhase;
	const UCHAR *XGI_SpecialPhaseM;
	const UCHAR *XGI_SpecialPhaseJ;
	const XGI_LCDDataStruct  *XGI_ExtLCD1024x768Data;
	const XGI_LCDDataStruct  *XGI_St2LCD1024x768Data;
	const XGI_LCDDataStruct  *XGI_LCD1280x720Data;
	const XGI_LCDDataStruct  *XGI_StLCD1280x768_2Data;
	const XGI_LCDDataStruct  *XGI_ExtLCD1280x768_2Data;
	const XGI_LCDDataStruct  *XGI_LCD1280x768_3Data;
	const XGI_LCDDataStruct  *XGI_LCD1280x800Data;
	const XGI_LCDDataStruct  *XGI_LCD1280x960Data;
	const XGI_LCDDataStruct  *XGI_ExtLCD1280x1024Data;
	const XGI_LCDDataStruct  *XGI_St2LCD1280x1024Data;
	const XGI_LCDDataStruct  *XGI_New_StLCD1400x1050Data;
	const XGI_LCDDataStruct  *XGI_New_ExtLCD1400x1050Data;
	const XGI_LCDDataStruct  *XGI_New_StLCD1600x1200Data;
	const XGI_LCDDataStruct  *XGI_New_ExtLCD1600x1200Data;
	const XGI_LCDDataStruct  *XGI_LCD1680x1050Data;
	const XGI_LCDDataStruct  *XGI_NoScaleData;
	const XGI_TVDataStruct   *XGI_New_StPALData;
	const XGI_TVDataStruct   *XGI_New_ExtPALData;
	const XGI_TVDataStruct   *XGI_New_StNTSCData;
	const XGI_TVDataStruct   *XGI_New_ExtNTSCData;
	const XGI_TVDataStruct   *XGI_St1HiTVData;
	const XGI_TVDataStruct   *XGI_New_St2HiTVData;
	const XGI_TVDataStruct   *XGI_New_ExtHiTVData;
	const XGI_TVDataStruct   *XGI_St525iData;
	const XGI_TVDataStruct   *XGI_St525pData;
	const XGI_TVDataStruct   *XGI_St750pData;
	const XGI_TVDataStruct   *XGI_Ext525iData;
	const XGI_TVDataStruct   *XGI_Ext525pData;
	const XGI_TVDataStruct   *XGI_Ext750pData;
	const UCHAR *XGI_NTSCTiming;
	const UCHAR *XGI_PALTiming;
	const UCHAR *XGI_HiTVExtTiming;
	const UCHAR *XGI_HiTVSt1Timing;
	const UCHAR *XGI_HiTVSt2Timing;
	const UCHAR *XGI_HiTVGroup3Data;
	const UCHAR *XGI_HiTVGroup3Simu;

	const XGI_Part2PortTblStruct *XGI_CRT2Part2_1024x768_1;
	const XGI_Part2PortTblStruct *XGI_CRT2Part2_1280x1024_1;
	const XGI_Part2PortTblStruct *XGI_CRT2Part2_1024x768_2;
	const XGI_Part2PortTblStruct *XGI_CRT2Part2_1280x1024_2;
	const XGI_Part2PortTblStruct *XGI_CRT2Part2_1024x768_3;
	const XGI_Part2PortTblStruct *XGI_CRT2Part2_1280x1024_3;

	/* LVDS, Chrontel */

	const XGI_LVDSDataStruct  *XGI_LVDS800x600Data_1;
	const XGI_LVDSDataStruct  *XGI_LVDS800x600Data_2;
	const XGI_LVDSDataStruct  *XGI_New_LVDS1024x768Data_1;
	const XGI_LVDSDataStruct  *XGI_New_LVDS1024x768Data_2;
	const XGI_LVDSDataStruct  *XGI_New_LVDS1280x1024Data_1;
	const XGI_LVDSDataStruct  *XGI_New_LVDS1280x1024Data_2;
	const XGI_LVDSDataStruct  *XGI_LVDS1280x960Data_1;
	const XGI_LVDSDataStruct  *XGI_LVDS1280x960Data_2;
	const XGI_LVDSDataStruct  *XGI_New_LVDS1400x1050Data_1;
	const XGI_LVDSDataStruct  *XGI_New_LVDS1400x1050Data_2;
	const XGI_LVDSDataStruct  *XGI_New_LVDS1600x1200Data_1;
	const XGI_LVDSDataStruct  *XGI_LVDS1600x1200Data_2;
	const XGI_LVDSDataStruct  *XGI_LVDS1280x768Data_1;
	const XGI_LVDSDataStruct  *XGI_LVDS1280x768Data_2;
	const XGI_LVDSDataStruct  *XGI_LVDS1024x600Data_1;
	const XGI_LVDSDataStruct  *XGI_LVDS1024x600Data_2;
	const XGI_LVDSDataStruct  *XGI_LVDS1152x768Data_1;
	const XGI_LVDSDataStruct  *XGI_LVDS1152x768Data_2;
	const XGI_LVDSDataStruct  *XGI_LVDS640x480Data_1;
	const XGI_LVDSDataStruct  *XGI_LVDS640x480Data_2;
	const XGI_LVDSDataStruct  *XGI_LVDS320x480Data_1;
	const XGI_LVDSDataStruct  *XGI_LVDSXXXxXXXData_1;
	const XGI_LVDSDataStruct  *XGI_LVDSBARCO1366Data_1;
	const XGI_LVDSDataStruct  *XGI_LVDSBARCO1366Data_2;
	const XGI_LVDSDataStruct  *XGI_LVDSBARCO1024Data_1;
	const XGI_LVDSDataStruct  *XGI_LVDSBARCO1024Data_2;
	const XGI_LVDSDataStruct  *XGI_LVDS848x480Data_1;
	const XGI_LVDSDataStruct  *XGI_LVDS848x480Data_2;
	const XGI_LVDSDataStruct  *XGI_New_CHTVUNTSCData;
	const XGI_LVDSDataStruct  *XGI_New_CHTVONTSCData;
	const XGI_LVDSDataStruct  *XGI_CHTVUPALData;
	const XGI_LVDSDataStruct  *XGI_CHTVOPALData;
	const XGI_LVDSDataStruct  *XGI_CHTVUPALMData;
	const XGI_LVDSDataStruct  *XGI_CHTVOPALMData;
	const XGI_LVDSDataStruct  *XGI_CHTVUPALNData;
	const XGI_LVDSDataStruct  *XGI_CHTVOPALNData;
	const XGI_LVDSDataStruct  *XGI_CHTVSOPALData;

	const XGI_LVDSDesStruct  *XGI_PanelType00_1;
	const XGI_LVDSDesStruct  *XGI_PanelType01_1;
	const XGI_LVDSDesStruct  *XGI_PanelType02_1;
	const XGI_LVDSDesStruct  *XGI_PanelType03_1;
	const XGI_LVDSDesStruct  *XGI_PanelType04_1;
	const XGI_LVDSDesStruct  *XGI_PanelType05_1;
	const XGI_LVDSDesStruct  *XGI_PanelType06_1;
	const XGI_LVDSDesStruct  *XGI_PanelType07_1;
	const XGI_LVDSDesStruct  *XGI_PanelType08_1;
	const XGI_LVDSDesStruct  *XGI_PanelType09_1;
	const XGI_LVDSDesStruct  *XGI_PanelType0a_1;
	const XGI_LVDSDesStruct  *XGI_PanelType0b_1;
	const XGI_LVDSDesStruct  *XGI_PanelType0c_1;
	const XGI_LVDSDesStruct  *XGI_PanelType0d_1;
	const XGI_LVDSDesStruct  *XGI_PanelType0e_1;
	const XGI_LVDSDesStruct  *XGI_PanelType0f_1;
	const XGI_LVDSDesStruct  *XGI_PanelTypeNS_1;
	const XGI_LVDSDesStruct  *XGI_PanelType00_2;
	const XGI_LVDSDesStruct  *XGI_PanelType01_2;
	const XGI_LVDSDesStruct  *XGI_PanelType02_2;
	const XGI_LVDSDesStruct  *XGI_PanelType03_2;
	const XGI_LVDSDesStruct  *XGI_PanelType04_2;
	const XGI_LVDSDesStruct  *XGI_PanelType05_2;
	const XGI_LVDSDesStruct  *XGI_PanelType06_2;
	const XGI_LVDSDesStruct  *XGI_PanelType07_2;
	const XGI_LVDSDesStruct  *XGI_PanelType08_2;
	const XGI_LVDSDesStruct  *XGI_PanelType09_2;
	const XGI_LVDSDesStruct  *XGI_PanelType0a_2;
	const XGI_LVDSDesStruct  *XGI_PanelType0b_2;
	const XGI_LVDSDesStruct  *XGI_PanelType0c_2;
	const XGI_LVDSDesStruct  *XGI_PanelType0d_2;
	const XGI_LVDSDesStruct  *XGI_PanelType0e_2;
	const XGI_LVDSDesStruct  *XGI_PanelType0f_2;
	const XGI_LVDSDesStruct  *XGI_PanelTypeNS_2;
	const XGI_LVDSDesStruct  *XGI_CHTVUNTSCDesData;
	const XGI_LVDSDesStruct  *XGI_CHTVONTSCDesData;
	const XGI_LVDSDesStruct  *XGI_CHTVUPALDesData;
	const XGI_LVDSDesStruct  *XGI_CHTVOPALDesData;

	const XGI_LVDSCRT1DataStruct  *XGI_LVDSCRT1800x600_1;
	const XGI_LVDSCRT1DataStruct  *XGI_LVDSCRT11024x768_1;
	const XGI_LVDSCRT1DataStruct  *XGI_LVDSCRT11280x1024_1;
	const XGI_LVDSCRT1DataStruct  *XGI_LVDSCRT11400x1050_1;
	const XGI_LVDSCRT1DataStruct  *XGI_LVDSCRT11280x768_1;
	const XGI_LVDSCRT1DataStruct  *XGI_LVDSCRT11024x600_1;
	const XGI_LVDSCRT1DataStruct  *XGI_LVDSCRT11152x768_1;
	const XGI_LVDSCRT1DataStruct  *XGI_LVDSCRT11600x1200_1;
	const XGI_LVDSCRT1DataStruct  *XGI_LVDSCRT1800x600_1_H;
	const XGI_LVDSCRT1DataStruct  *XGI_LVDSCRT11024x768_1_H;
	const XGI_LVDSCRT1DataStruct  *XGI_LVDSCRT11280x1024_1_H;
	const XGI_LVDSCRT1DataStruct  *XGI_LVDSCRT11400x1050_1_H;
	const XGI_LVDSCRT1DataStruct  *XGI_LVDSCRT11280x768_1_H;
	const XGI_LVDSCRT1DataStruct  *XGI_LVDSCRT11024x600_1_H;
	const XGI_LVDSCRT1DataStruct  *XGI_LVDSCRT11152x768_1_H;
	const XGI_LVDSCRT1DataStruct  *XGI_LVDSCRT11600x1200_1_H;
	const XGI_LVDSCRT1DataStruct  *XGI_LVDSCRT1800x600_2;
	const XGI_LVDSCRT1DataStruct  *XGI_LVDSCRT11024x768_2;
	const XGI_LVDSCRT1DataStruct  *XGI_LVDSCRT11280x1024_2;
	const XGI_LVDSCRT1DataStruct  *XGI_LVDSCRT11400x1050_2;
	const XGI_LVDSCRT1DataStruct  *XGI_LVDSCRT11280x768_2;
	const XGI_LVDSCRT1DataStruct  *XGI_LVDSCRT11024x600_2;
	const XGI_LVDSCRT1DataStruct  *XGI_LVDSCRT11152x768_2;
	const XGI_LVDSCRT1DataStruct  *XGI_LVDSCRT11600x1200_2;
	const XGI_LVDSCRT1DataStruct  *XGI_LVDSCRT1800x600_2_H;
	const XGI_LVDSCRT1DataStruct  *XGI_LVDSCRT11024x768_2_H;
	const XGI_LVDSCRT1DataStruct  *XGI_LVDSCRT11280x1024_2_H;
	const XGI_LVDSCRT1DataStruct  *XGI_LVDSCRT11400x1050_2_H;
	const XGI_LVDSCRT1DataStruct  *XGI_LVDSCRT11280x768_2_H;
	const XGI_LVDSCRT1DataStruct  *XGI_LVDSCRT11024x600_2_H;
	const XGI_LVDSCRT1DataStruct  *XGI_LVDSCRT11152x768_2_H;
	const XGI_LVDSCRT1DataStruct  *XGI_LVDSCRT11600x1200_2_H;
	const XGI_LVDSCRT1DataStruct  *XGI_LVDSCRT1XXXxXXX_1;
	const XGI_LVDSCRT1DataStruct  *XGI_LVDSCRT1XXXxXXX_1_H;
	const XGI_LVDSCRT1DataStruct  *XGI_LVDSCRT1640x480_1;
	const XGI_LVDSCRT1DataStruct  *XGI_LVDSCRT1640x480_1_H;
	const XGI_LVDSCRT1DataStruct  *XGI_LVDSCRT1640x480_2;
	const XGI_LVDSCRT1DataStruct  *XGI_LVDSCRT1640x480_2_H;
	const XGI_LVDSCRT1DataStruct  *XGI_LVDSCRT1640x480_3;
	const XGI_LVDSCRT1DataStruct  *XGI_LVDSCRT1640x480_3_H;
	const XGI_LVDSCRT1DataStruct  *XGI_LVDSCRT1320x480_1;
	const XGI_LVDSCRT1DataStruct  *XGI_CHTVCRT1UNTSC;
	const XGI_LVDSCRT1DataStruct  *XGI_CHTVCRT1ONTSC;
	const XGI_LVDSCRT1DataStruct  *XGI_CHTVCRT1UPAL;
	const XGI_LVDSCRT1DataStruct  *XGI_CHTVCRT1OPAL;
	const XGI_LVDSCRT1DataStruct  *XGI_CHTVCRT1SOPAL;

	const XGI_CHTVRegDataStruct *XGI_CHTVReg_UNTSC;
	const XGI_CHTVRegDataStruct *XGI_CHTVReg_ONTSC;
	const XGI_CHTVRegDataStruct *XGI_CHTVReg_UPAL;
	const XGI_CHTVRegDataStruct *XGI_CHTVReg_OPAL;
	const XGI_CHTVRegDataStruct *XGI_CHTVReg_UPALM;
	const XGI_CHTVRegDataStruct *XGI_CHTVReg_OPALM;
	const XGI_CHTVRegDataStruct *XGI_CHTVReg_UPALN;
	const XGI_CHTVRegDataStruct *XGI_CHTVReg_OPALN;
	const XGI_CHTVRegDataStruct *XGI_CHTVReg_SOPAL;

	const UCHAR *XGI_CHTVVCLKUNTSC;
	const UCHAR *XGI_CHTVVCLKONTSC;
	const UCHAR *XGI_CHTVVCLKUPAL;
	const UCHAR *XGI_CHTVVCLKOPAL;
	const UCHAR *XGI_CHTVVCLKUPALM;
	const UCHAR *XGI_CHTVVCLKOPALM;
	const UCHAR *XGI_CHTVVCLKUPALN;
	const UCHAR *XGI_CHTVVCLKOPALN;
	const UCHAR *XGI_CHTVVCLKSOPAL;

	USHORT  PanelXRes, PanelHT;
	USHORT  PanelYRes, PanelVT;
	USHORT  PanelHRS,  PanelHRE;
  	USHORT 	PanelVRS,  PanelVRE;
	USHORT  PanelVCLKIdx300;
	USHORT  PanelVCLKIdx315;

	BOOLEAN UseCustomMode;
	BOOLEAN CRT1UsesCustomMode;
	USHORT  CHDisplay;
	USHORT  CHSyncStart;
	USHORT  CHSyncEnd;
	USHORT  CHTotal;
	USHORT  CHBlankStart;
	USHORT  CHBlankEnd;
	USHORT  CVDisplay;
	USHORT  CVSyncStart;
	USHORT  CVSyncEnd;
	USHORT  CVTotal;
	USHORT  CVBlankStart;
	USHORT  CVBlankEnd;
	ULONG   CDClock;
	ULONG   CFlags;
	UCHAR   CCRT1CRTC[17];
	UCHAR   CSR2B;
	UCHAR   CSR2C;
	USHORT  CSRClock;
	USHORT  CSRClock_CRT1;
	USHORT  CModeFlag;
	USHORT  CModeFlag_CRT1;
	USHORT  CInfoFlag;

	int	LVDSHL;

	BOOLEAN Backup;
	UCHAR Backup_Mode;
	UCHAR Backup_14;
	UCHAR Backup_15;
	UCHAR Backup_16;
	UCHAR Backup_17;
	UCHAR Backup_18;
	UCHAR Backup_19;
	UCHAR Backup_1a;
	UCHAR Backup_1b;
	UCHAR Backup_1c;
	UCHAR Backup_1d;

	int     UsePanelScaler;
	int	CenterScreen;

	USHORT  CP_Vendor, CP_Product;
	BOOLEAN CP_HaveCustomData;
	int     CP_PreferredX, CP_PreferredY, CP_PreferredIndex;
	int	CP_MaxX, CP_MaxY, CP_MaxClock;
	BOOLEAN CP_Supports64048075;
	int     CP_HDisplay[7], CP_VDisplay[7];	/* For Custom LCD panel dimensions */
    	int     CP_HTotal[7], CP_VTotal[7];
    	int     CP_HSyncStart[7], CP_VSyncStart[7];
    	int     CP_HSyncEnd[7], CP_VSyncEnd[7];
	int     CP_HBlankStart[7], CP_VBlankStart[7];
	int     CP_HBlankEnd[7], CP_VBlankEnd[7];
    	int     CP_Clock[7];
	BOOLEAN CP_DataValid[7];
	BOOLEAN CP_HSync_P[7], CP_VSync_P[7], CP_SyncValid[7];
} XGI_Private;

#endif

