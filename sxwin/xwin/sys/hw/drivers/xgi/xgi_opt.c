/* $XFree86: xc/programs/Xserver/hw/xfree86/drivers/xgi/xgi_opt.c,v 1.1 2005/05/02 13:28:02 dawes Exp $ */
/*
 * Video driver option evaluation
 *
 * Copyright (C) 2001-2004 by Thomas Winischhofer, Vienna, Austria
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
 * "NoAccel", "NoXVideo", "SWCursor", "HWCursor" and "Rotate" option portions
 * Copyright (C) 1999-2004 The XFree86 Project, Inc. Licensed under the terms
 * of the XFree86 license (http://www.xfree86.org/current/LICENSE1.html).
 *
 * Authors:  	Thomas Winischhofer <thomas@winischhofer.net>
 *              ?
 */

#include "xf86.h"
#include "xf86PciInfo.h"
#include "xf86str.h"
#include "xf86Cursor.h"

#include "xgi.h"

extern const customttable mycustomttable[];

typedef enum {
    OPTION_SW_CURSOR,
    OPTION_HW_CURSOR,
    OPTION_NOACCEL,
    OPTION_TURBOQUEUE,
    OPTION_FAST_VRAM,
    OPTION_NOHOSTBUS,
    OPTION_RENDER,
    OPTION_FORCE_CRT1TYPE,
    OPTION_FORCE_CRT2TYPE,
    OPTION_YPBPRAR,
    OPTION_SHADOW_FB,
    OPTION_DRI,
    OPTION_AGP_SIZE,
    OPTION_AGP_SIZE2,
    OPTION_ROTATE,
    OPTION_NOXVIDEO,
    OPTION_VESA,
    OPTION_MAXXFBMEM,
    OPTION_FORCECRT1,
    OPTION_XVONCRT2,
    OPTION_PDC,
    OPTION_PDCA,
    OPTION_PDCS,
    OPTION_PDCAS,
    OPTION_EMI,
    OPTION_TVSTANDARD,
    OPTION_USEROMDATA,
    OPTION_NOINTERNALMODES,
    OPTION_USEOEM,
    OPTION_NOYV12,
    OPTION_CHTVOVERSCAN,
    OPTION_CHTVSOVERSCAN,
    OPTION_CHTVLUMABANDWIDTHCVBS,
    OPTION_CHTVLUMABANDWIDTHSVIDEO,
    OPTION_CHTVLUMAFLICKERFILTER,
    OPTION_CHTVCHROMABANDWIDTH,
    OPTION_CHTVCHROMAFLICKERFILTER,
    OPTION_CHTVCVBSCOLOR,
    OPTION_CHTVTEXTENHANCE,
    OPTION_CHTVCONTRAST,
    OPTION_XGITVEDGEENHANCE,
    OPTION_XGITVANTIFLICKER,
    OPTION_XGITVSATURATION,
    OPTION_XGITVCHROMAFILTER,
    OPTION_XGITVLUMAFILTER,
    OPTION_XGITVCOLCALIBFINE,
    OPTION_XGITVCOLCALIBCOARSE,
    OPTION_TVXPOSOFFSET,
    OPTION_TVYPOSOFFSET,
    OPTION_TVXSCALE,
    OPTION_TVYSCALE,
    OPTION_XGI6326ANTIFLICKER,
    OPTION_XGI6326ENABLEYFILTER,
    OPTION_XGI6326YFILTERSTRONG,
    OPTION_XGI6326FORCETVPPLUG,
    OPTION_XGI6326FSCADJUST,
    OPTION_CHTVTYPE,
    OPTION_USERGBCURSOR,
    OPTION_USERGBCURSORBLEND,
    OPTION_USERGBCURSORBLENDTH,
    OPTION_RESTOREBYSET,
    OPTION_NODDCFORCRT2,
    OPTION_FORCECRT2REDETECTION,
    OPTION_SENSEYPBPR,
    OPTION_CRT1GAMMA,
    OPTION_CRT2GAMMA,
    OPTION_XVGAMMA,
    OPTION_XVDEFCONTRAST,
    OPTION_XVDEFBRIGHTNESS,
    OPTION_XVDEFHUE,
    OPTION_XVDEFSATURATION,
    OPTION_XVDEFDISABLEGFX,
    OPTION_XVDEFDISABLEGFXLR,
    OPTION_XVMEMCPY,
    OPTION_XVUSECHROMAKEY,
    OPTION_XVCHROMAMIN,
    OPTION_XVCHROMAMAX,
    OPTION_XVDISABLECOLORKEY,
    OPTION_XVINSIDECHROMAKEY,
    OPTION_XVYUVCHROMAKEY,
    OPTION_SCALELCD,
    OPTION_CENTERLCD,
    OPTION_SPECIALTIMING,
    OPTION_LVDSHL,
    OPTION_ENABLEHOTKEY,
    OPTION_MERGEDFB,
    OPTION_MERGEDFBAUTO,
    OPTION_CRT2HSYNC,
    OPTION_CRT2VREFRESH,
    OPTION_CRT2POS,
    OPTION_METAMODES,
    OPTION_MERGEDFB2,
    OPTION_CRT2HSYNC2,
    OPTION_CRT2VREFRESH2,
    OPTION_CRT2POS2,
    OPTION_NOXGIXINERAMA,
    OPTION_NOXGIXINERAMA2,
    OPTION_CRT2ISSCRN0,
    OPTION_MERGEDDPI,
    OPTION_ENABLEXGICTRL,
    OPTION_STOREDBRI,
    OPTION_STOREDPBRI,
#ifdef XGI_CP
    XGI_CP_OPT_OPTIONS
#endif
    OPTION_PSEUDO
} XGIOpts;

static const OptionInfoRec XGIOptions[] = {
    { OPTION_SW_CURSOR,         	"SWcursor",               OPTV_BOOLEAN,   {0}, FALSE },
    { OPTION_HW_CURSOR,         	"HWcursor",               OPTV_BOOLEAN,   {0}, FALSE },
    { OPTION_NOACCEL,           	"NoAccel",                OPTV_BOOLEAN,   {0}, FALSE },
    { OPTION_TURBOQUEUE,        	"TurboQueue",             OPTV_BOOLEAN,   {0}, FALSE },
    { OPTION_FAST_VRAM,         	"FastVram",               OPTV_BOOLEAN,   {0}, FALSE },
    { OPTION_NOHOSTBUS,         	"NoHostBus",              OPTV_BOOLEAN,   {0}, FALSE },
    { OPTION_RENDER,        		"RenderAcceleration",     OPTV_BOOLEAN,   {0}, FALSE },
    { OPTION_FORCE_CRT1TYPE,    	"ForceCRT1Type",          OPTV_STRING,    {0}, FALSE },
    { OPTION_FORCE_CRT2TYPE,    	"ForceCRT2Type",          OPTV_STRING,    {0}, FALSE },
    { OPTION_YPBPRAR,  		  	"YPbPrAspectRatio",       OPTV_STRING,    {0}, FALSE },
    { OPTION_SHADOW_FB,         	"ShadowFB",               OPTV_BOOLEAN,   {0}, FALSE },
    { OPTION_DRI,         		"DRI",               	  OPTV_BOOLEAN,   {0}, FALSE },
    { OPTION_AGP_SIZE,			"AGPSize",      	  OPTV_INTEGER,   {0}, FALSE },
    { OPTION_AGP_SIZE2,			"GARTSize",      	  OPTV_INTEGER,   {0}, FALSE },
    { OPTION_ROTATE,            	"Rotate",                 OPTV_STRING,    {0}, FALSE },
    { OPTION_NOXVIDEO,          	"NoXvideo",               OPTV_BOOLEAN,   {0}, FALSE },
    { OPTION_VESA,			"Vesa",		          OPTV_BOOLEAN,   {0}, FALSE },
    { OPTION_MAXXFBMEM,         	"MaxXFBMem",              OPTV_INTEGER,   {0}, -1    },
    { OPTION_FORCECRT1,         	"ForceCRT1",              OPTV_BOOLEAN,   {0}, FALSE },
    { OPTION_XVONCRT2,          	"XvOnCRT2",               OPTV_BOOLEAN,   {0}, FALSE },
    { OPTION_PDC,               	"PanelDelayCompensation", OPTV_INTEGER,   {0}, -1    },
    { OPTION_PDCA,               	"PanelDelayCompensation1",OPTV_INTEGER,   {0}, -1    },
    { OPTION_PDCS,               	"PDC", 			  OPTV_INTEGER,   {0}, -1    },
    { OPTION_PDCAS,               	"PDC1",			  OPTV_INTEGER,   {0}, -1    },
    { OPTION_EMI,               	"EMI", 			  OPTV_INTEGER,   {0}, -1    },
    { OPTION_LVDSHL,			"LVDSHL", 	  	  OPTV_INTEGER,   {0}, -1    },
    { OPTION_SPECIALTIMING,        	"SpecialTiming",          OPTV_STRING,    {0}, -1    },
    { OPTION_TVSTANDARD,        	"TVStandard",             OPTV_STRING,    {0}, -1    },
    { OPTION_USEROMDATA,		"UseROMData",	          OPTV_BOOLEAN,   {0}, -1    },
    { OPTION_NOINTERNALMODES,   	"NoInternalModes",        OPTV_BOOLEAN,   {0}, FALSE },
    { OPTION_USEOEM, 			"UseOEMData",		  OPTV_BOOLEAN,   {0}, -1    },
    { OPTION_NOYV12, 			"NoYV12",		  OPTV_BOOLEAN,   {0}, -1    },
    { OPTION_CHTVTYPE,			"CHTVType",	          OPTV_BOOLEAN,   {0}, -1    },
    { OPTION_CHTVOVERSCAN,		"CHTVOverscan",	          OPTV_BOOLEAN,   {0}, -1    },
    { OPTION_CHTVSOVERSCAN,		"CHTVSuperOverscan",      OPTV_BOOLEAN,   {0}, -1    },
    { OPTION_CHTVLUMABANDWIDTHCVBS,	"CHTVLumaBandwidthCVBS",  OPTV_INTEGER,   {0}, -1    },
    { OPTION_CHTVLUMABANDWIDTHSVIDEO,	"CHTVLumaBandwidthSVIDEO",OPTV_INTEGER,   {0}, -1    },
    { OPTION_CHTVLUMAFLICKERFILTER,	"CHTVLumaFlickerFilter",  OPTV_INTEGER,   {0}, -1    },
    { OPTION_CHTVCHROMABANDWIDTH,	"CHTVChromaBandwidth",    OPTV_INTEGER,   {0}, -1    },
    { OPTION_CHTVCHROMAFLICKERFILTER,	"CHTVChromaFlickerFilter",OPTV_INTEGER,   {0}, -1    },
    { OPTION_CHTVCVBSCOLOR,		"CHTVCVBSColor",          OPTV_BOOLEAN,   {0}, -1    },
    { OPTION_CHTVTEXTENHANCE,		"CHTVTextEnhance",	  OPTV_INTEGER,   {0}, -1    },
    { OPTION_CHTVCONTRAST,		"CHTVContrast",		  OPTV_INTEGER,   {0}, -1    },
    { OPTION_XGITVEDGEENHANCE,		"XGITVEdgeEnhance",	  OPTV_INTEGER,   {0}, -1    },
    { OPTION_XGITVANTIFLICKER,		"XGITVAntiFlicker",	  OPTV_STRING,    {0}, FALSE },
    { OPTION_XGITVSATURATION,		"XGITVSaturation",	  OPTV_INTEGER,   {0}, -1    },
    { OPTION_XGITVCHROMAFILTER,		"XGITVCFilter",      	  OPTV_BOOLEAN,   {0}, -1    },
    { OPTION_XGITVLUMAFILTER,		"XGITVYFilter",	  	  OPTV_INTEGER,   {0}, -1    },
    { OPTION_XGITVCOLCALIBFINE,		"XGITVColorCalibFine",	  OPTV_INTEGER,   {0}, -1    },
    { OPTION_XGITVCOLCALIBCOARSE,	"XGITVColorCalibCoarse",  OPTV_INTEGER,   {0}, -1    },
    { OPTION_TVXSCALE,			"XGITVXScale", 	  	  OPTV_INTEGER,   {0}, -1    },
    { OPTION_TVYSCALE,			"XGITVYScale", 	  	  OPTV_INTEGER,   {0}, -1    },
    { OPTION_TVXPOSOFFSET,		"TVXPosOffset", 	  OPTV_INTEGER,   {0}, -1    },
    { OPTION_TVYPOSOFFSET,		"TVYPosOffset", 	  OPTV_INTEGER,   {0}, -1    },
    { OPTION_XGI6326ANTIFLICKER,	"XGI6326TVAntiFlicker",   OPTV_STRING,    {0}, FALSE  },
    { OPTION_XGI6326ENABLEYFILTER,	"XGI6326TVEnableYFilter", OPTV_BOOLEAN,   {0}, -1    },
    { OPTION_XGI6326YFILTERSTRONG,	"XGI6326TVYFilterStrong", OPTV_BOOLEAN,   {0}, -1    },
    { OPTION_XGI6326FORCETVPPLUG,	"XGI6326TVForcePlug",     OPTV_STRING,    {0}, -1    },
    { OPTION_XGI6326FSCADJUST,		"XGI6326FSCAdjust", 	  OPTV_INTEGER,   {0}, -1    },
    { OPTION_USERGBCURSOR, 		"UseColorHWCursor",	  OPTV_BOOLEAN,   {0}, -1    },
    { OPTION_USERGBCURSORBLEND,		"ColorHWCursorBlending",  OPTV_BOOLEAN,   {0}, -1    },
    { OPTION_USERGBCURSORBLENDTH,	"ColorHWCursorBlendThreshold", OPTV_INTEGER,{0},-1   },
    { OPTION_RESTOREBYSET,		"RestoreBySetMode", 	  OPTV_BOOLEAN,   {0}, -1    },
    { OPTION_NODDCFORCRT2,		"NoCRT2Detection", 	  OPTV_BOOLEAN,   {0}, -1    },
    { OPTION_FORCECRT2REDETECTION,	"ForceCRT2ReDetection",   OPTV_BOOLEAN,   {0}, -1    },
    { OPTION_SENSEYPBPR,		"SenseYPbPr",   	  OPTV_BOOLEAN,   {0}, -1    },
    { OPTION_CRT1GAMMA,			"CRT1Gamma", 	  	  OPTV_BOOLEAN,   {0}, -1    },
    { OPTION_CRT2GAMMA,			"CRT2Gamma", 	  	  OPTV_BOOLEAN,   {0}, -1    },
    { OPTION_STOREDBRI,			"StoredGammaBrightness",  OPTV_STRING,    {0}, -1    },
    { OPTION_STOREDPBRI,		"StoredGammaPreBrightness",OPTV_STRING,   {0}, -1    },
    { OPTION_XVGAMMA,			"XvGamma", 	  	  OPTV_STRING,    {0}, -1    },
    { OPTION_XVDEFCONTRAST,		"XvDefaultContrast", 	  OPTV_INTEGER,   {0}, -1    },
    { OPTION_XVDEFBRIGHTNESS,		"XvDefaultBrightness", 	  OPTV_INTEGER,   {0}, -1    },
    { OPTION_XVDEFHUE,			"XvDefaultHue", 	  OPTV_INTEGER,   {0}, -1    },
    { OPTION_XVDEFSATURATION,		"XvDefaultSaturation", 	  OPTV_INTEGER,   {0}, -1    },
    { OPTION_XVDEFDISABLEGFX,		"XvDefaultDisableGfx", 	  OPTV_BOOLEAN,   {0}, -1    },
    { OPTION_XVDEFDISABLEGFXLR,		"XvDefaultDisableGfxLR",  OPTV_BOOLEAN,   {0}, -1    },
    { OPTION_XVCHROMAMIN,		"XvChromaMin", 	  	  OPTV_INTEGER,   {0}, -1    },
    { OPTION_XVCHROMAMAX,		"XvChromaMax", 	  	  OPTV_INTEGER,   {0}, -1    },
    { OPTION_XVUSECHROMAKEY,		"XvUseChromaKey",         OPTV_BOOLEAN,   {0}, -1    },
    { OPTION_XVINSIDECHROMAKEY,		"XvInsideChromaKey",      OPTV_BOOLEAN,   {0}, -1    },
    { OPTION_XVYUVCHROMAKEY,		"XvYUVChromaKey",         OPTV_BOOLEAN,   {0}, -1    },
    { OPTION_XVDISABLECOLORKEY,		"XvDisableColorKey",      OPTV_BOOLEAN,   {0}, -1    },
    { OPTION_XVMEMCPY,			"XvUseMemcpy",  	  OPTV_BOOLEAN,   {0}, -1    },
    { OPTION_SCALELCD,			"ScaleLCD",	   	  OPTV_BOOLEAN,   {0}, -1    },
    { OPTION_CENTERLCD,			"CenterLCD",	   	  OPTV_BOOLEAN,   {0}, -1    },
    { OPTION_ENABLEHOTKEY,		"EnableHotkey",	   	  OPTV_BOOLEAN,   {0}, -1    },
    { OPTION_ENABLEXGICTRL,		"EnableXGICtrl",   	  OPTV_BOOLEAN,   {0}, -1    },
#ifdef XGIMERGED
    { OPTION_MERGEDFB,			"MergedFB",		  OPTV_BOOLEAN,	  {0}, FALSE },
    { OPTION_MERGEDFB2,			"TwinView",		  OPTV_BOOLEAN,	  {0}, FALSE },	  /* alias */
    { OPTION_MERGEDFBAUTO,		"MergedFBAuto",		  OPTV_BOOLEAN,	  {0}, FALSE },
    { OPTION_CRT2HSYNC,			"CRT2HSync",		  OPTV_STRING,	  {0}, FALSE },
    { OPTION_CRT2HSYNC2,		"SecondMonitorHorizSync", OPTV_STRING,	  {0}, FALSE },   /* alias */
    { OPTION_CRT2VREFRESH,		"CRT2VRefresh",		  OPTV_STRING,    {0}, FALSE },
    { OPTION_CRT2VREFRESH2,		"SecondMonitorVertRefresh", OPTV_STRING,  {0}, FALSE },   /* alias */
    { OPTION_CRT2POS,   		"CRT2Position",		  OPTV_STRING,	  {0}, FALSE },
    { OPTION_CRT2POS2,   		"TwinViewOrientation",	  OPTV_STRING,	  {0}, FALSE },   /* alias */
    { OPTION_METAMODES,   		"MetaModes",  		  OPTV_STRING,	  {0}, FALSE },
    { OPTION_MERGEDDPI,			"MergedDPI", 		  OPTV_STRING,	  {0}, FALSE },
#ifdef XGIXINERAMA
    { OPTION_NOXGIXINERAMA,		"NoMergedXinerama",	  OPTV_BOOLEAN,	  {0}, FALSE },
    { OPTION_NOXGIXINERAMA2,		"NoTwinviewXineramaInfo", OPTV_BOOLEAN,   {0}, FALSE },   /* alias */
    { OPTION_CRT2ISSCRN0,		"MergedXineramaCRT2IsScreen0",OPTV_BOOLEAN,{0},FALSE },
#endif
#endif
#ifdef XGI_CP
    XGI_CP_OPTION_DETAIL
#endif
    { -1,                       	NULL,                     OPTV_NONE,      {0}, FALSE }
};

void
xgiOptions(ScrnInfoPtr pScrn)
{
    XGIPtr      pXGI = XGIPTR(pScrn);
    MessageType from;
    char        *strptr;
    static const char *mybadparm = "\"%s\" is is not a valid parameter for option \"%s\"\n";
    static const char *disabledstr = "disabled";
    static const char *enabledstr = "enabled";
    static const char *ilrangestr = "Illegal %s parameter. Valid range is %d through %d\n";

    /* Collect all of the relevant option flags (fill in pScrn->options) */
    xf86CollectOptions(pScrn, NULL);

    /* Process the options */
    if(!(pXGI->Options = xalloc(sizeof(XGIOptions)))) return;

    memcpy(pXGI->Options, XGIOptions, sizeof(XGIOptions));

    xf86ProcessOptions(pScrn->scrnIndex, pScrn->options, pXGI->Options);

    /* Set defaults */

    pXGI->newFastVram = -1;
    pXGI->NoHostBus = FALSE;
    pXGI->TurboQueue = TRUE;
#ifdef XGIVRAMQ
    /* TODO: Option (315 series VRAM command queue) */
    /* But beware: xgifb does not know about this!!! */
    pXGI->cmdQueueSize = 512*1024;
#endif
    pXGI->doRender = TRUE;
    pXGI->HWCursor = TRUE;
    pXGI->Rotate = FALSE;
    pXGI->ShadowFB = FALSE;
    pXGI->loadDRI = FALSE;
    pXGI->agpWantedPages = AGP_PAGES;
    pXGI->VESA = -1;
    pXGI->NoXvideo = FALSE;
    pXGI->maxxfbmem = 0;
    pXGI->forceCRT1 = -1;
    pXGI->DSTN = FALSE;
    pXGI->FSTN = FALSE;
    pXGI->XvOnCRT2 = FALSE;
    pXGI->NoYV12 = -1;
    pXGI->PDC = -1;
    pXGI->PDCA = -1;
    pXGI->EMI = -1;
    pXGI->OptTVStand = -1;
    pXGI->OptROMUsage = -1;
    pXGI->noInternalModes = FALSE;
    pXGI->OptUseOEM = -1;
    pXGI->OptTVOver = -1;
    pXGI->OptTVSOver = -1;
    pXGI->chtvlumabandwidthcvbs = -1;
    pXGI->chtvlumabandwidthsvideo = -1;
    pXGI->chtvlumaflickerfilter = -1;
    pXGI->chtvchromabandwidth = -1;
    pXGI->chtvchromaflickerfilter = -1;
    pXGI->chtvcvbscolor = -1;
    pXGI->chtvtextenhance = -1;
    pXGI->chtvcontrast = -1;
    pXGI->xgitvedgeenhance = -1;
    pXGI->xgitvantiflicker = -1;
    pXGI->xgitvsaturation = -1;
    pXGI->xgitvcfilter = -1;
    pXGI->xgitvyfilter = 1; /* 0 = off, 1 = default, 2-8 = filter no */
    pXGI->xgitvcolcalibc = 0;
    pXGI->xgitvcolcalibf = 0;
    pXGI->xgi6326enableyfilter = -1;
    pXGI->xgi6326yfilterstrong = -1;
    pXGI->xgi6326tvplug = -1;
    pXGI->xgi6326fscadjust = 0;
    pXGI->tvxpos = 0;
    pXGI->tvypos = 0;
    pXGI->tvxscale = 0;
    pXGI->tvyscale = 0;
    pXGI->NonDefaultPAL = pXGI->NonDefaultNTSC = -1;
    pXGI->chtvtype = -1;
    pXGI->restorebyset = TRUE;
    pXGI->nocrt2ddcdetection = FALSE;
    pXGI->forcecrt2redetection = TRUE;   /* default changed since 13/09/2003 */
    pXGI->SenseYPbPr = TRUE;
    pXGI->ForceCRT1Type = CRT1_VGA;
    pXGI->ForceCRT2Type = CRT2_DEFAULT;
    pXGI->ForceYPbPrAR = TV_YPBPR169;
    pXGI->ForceTVType = -1;
    pXGI->CRT1gamma = TRUE;
    pXGI->CRT1gammaGiven = FALSE;
    pXGI->CRT2gamma = TRUE;
    pXGI->XvGamma = FALSE;
    pXGI->XvGammaGiven = FALSE;
    pXGI->enablexgictrl = FALSE;
 
       pXGI->XvDefBri = 0;
       pXGI->XvDefCon = 4;

    pXGI->XvDefHue = 0;
    pXGI->XvDefSat = 0;
    pXGI->XvDefDisableGfx = FALSE;
    pXGI->XvDefDisableGfxLR = FALSE;
    pXGI->UsePanelScaler = -1;
    pXGI->CenterLCD = -1;
    pXGI->XvUseMemcpy = TRUE;
    pXGI->XvUseChromaKey = FALSE;
    pXGI->XvDisableColorKey = FALSE;
    pXGI->XvInsideChromaKey = FALSE;
    pXGI->XvYUVChromaKey = FALSE;
    pXGI->XvChromaMin = 0x000101fe;
    pXGI->XvChromaMax = 0x000101ff;
    pXGI->XvGammaRed = pXGI->XvGammaGreen = pXGI->XvGammaBlue =
       pXGI->XvGammaRedDef = pXGI->XvGammaGreenDef = pXGI->XvGammaBlueDef = 1000;
    pXGI->GammaBriR = pXGI->GammaBriG = pXGI->GammaBriB = 1000;
    pXGI->GammaPBriR = pXGI->GammaPBriG = pXGI->GammaPBriB = 1000;
    pXGI->HideHWCursor = FALSE;
    pXGI->HWCursorIsVisible = FALSE;
#ifdef XGIMERGED
    pXGI->MergedFB = pXGI->MergedFBAuto = FALSE;
    pXGI->CRT2Position = xgiRightOf;
    pXGI->CRT2HSync = NULL;
    pXGI->CRT2VRefresh = NULL;
    pXGI->MetaModes = NULL;
    pXGI->MergedFBXDPI = pXGI->MergedFBYDPI = 0;
#ifdef XGIXINERAMA
    pXGI->UsexgiXinerama = TRUE;
    pXGI->CRT2IsScrn0 = FALSE;
#endif
#endif
#ifdef XGI_CP
    XGI_CP_OPT_DEFAULT
#endif

#if XF86_VERSION_CURRENT < XF86_VERSION_NUMERIC(4,2,99,0,0)
    pXGI->OptUseColorCursor = 0;
#endif

    /* Collect the options */

    /* FastVRAM (5597/5598, 6326 and 530/620 only)
     */
    if((pXGI->VGAEngine == XGI_OLD_VGA) || (pXGI->VGAEngine == XGI_530_VGA)) {
       from = X_DEFAULT;
       if(xf86GetOptValBool(pXGI->Options, OPTION_FAST_VRAM, &pXGI->newFastVram)) {
          from = X_CONFIG;
       }
       xf86DrvMsg(pScrn->scrnIndex, from, "Fast VRAM %s\n",
                   (pXGI->newFastVram == -1) ?
		         ((pXGI->oldChipset == OC_XGI620) ? "enabled (for read only)" :
			                                    "enabled (for write only)") :
		   	 (pXGI->newFastVram ? "enabled (for read and write)" : disabledstr));
    }

    /* MaxXFBMem
     * This options limits the amount of video memory X uses for screen
     * and off-screen buffers. This option should be used if using DRI
     * is intended. The kernel framebuffer driver required for DRM will
     * start its memory heap at 12MB if it detects more than 16MB, at 8MB if
     * between 8 and 16MB are available, otherwise at 4MB. So, if the amount
     * of memory X uses, a clash between the framebuffer's memory heap
     * and X is avoided. The amount is to be specified in KB.
     */
    if(xf86GetOptValULong(pXGI->Options, OPTION_MAXXFBMEM,
                                &pXGI->maxxfbmem)) {
            xf86DrvMsg(pScrn->scrnIndex, X_CONFIG,
                    "MaxXFBMem: Framebuffer memory shall be limited to %ld KB\n",
		    pXGI->maxxfbmem);
	    pXGI->maxxfbmem *= 1024;
    }

    /* NoAccel
     * Turns off 2D acceleration
     */
    if(xf86ReturnOptValBool(pXGI->Options, OPTION_NOACCEL, FALSE)) {
        pXGI->NoAccel = TRUE;
#if XF86_VERSION_CURRENT < XF86_VERSION_NUMERIC(4,2,99,0,0)
	pXGI->NoXvideo = TRUE;
	xf86DrvMsg(pScrn->scrnIndex, X_CONFIG, "2D Acceleration and Xv disabled\n");
#else
	xf86DrvMsg(pScrn->scrnIndex, X_CONFIG, "2D Acceleration disabled\n");
#endif

    }

    /* Disable XV support for XGI Z1 chip */
    if (PCI_CHIP_XGIXG20 == pXGI->Chipset)
        pXGI->NoXvideo = TRUE;

    /* SWCursor
     * HWCursor
     * Chooses whether to use the hardware or software cursor
     */
    from = X_DEFAULT;
    if(xf86GetOptValBool(pXGI->Options, OPTION_HW_CURSOR, &pXGI->HWCursor)) {
        from = X_CONFIG;
    }
    if(xf86ReturnOptValBool(pXGI->Options, OPTION_SW_CURSOR, FALSE)) {
        from = X_CONFIG;
        pXGI->HWCursor = FALSE;
	pXGI->OptUseColorCursor = 0;
    }
    xf86DrvMsg(pScrn->scrnIndex, from, "Using %s cursor\n",
                                pXGI->HWCursor ? "HW" : "SW");

    /*
     * MergedFB
     *
     * Enable/disable and configure merged framebuffer mode
     *
     */
#ifdef XGIMERGED
#ifdef XGIDUALHEAD
    if(pXGI->DualHeadMode) {
       Bool val;
       if(xf86GetOptValBool(pXGI->Options, OPTION_MERGEDFB, &val)) {
          xf86DrvMsg(pScrn->scrnIndex, X_WARNING,
	     "Option \"MergedFB\" cannot be used in Dual Head mode\n");
       }
    } else
#endif
#endif

    /* Some options can only be specified in the Master Head's Device
     * section. Here we give the user a hint in the log.
     */
#ifdef XGIDUALHEAD
    if((pXGI->DualHeadMode) && (pXGI->SecondHead)) {
       static const char *mystring = "Option \"%s\" is only accepted in Master Head's device section\n";
       Bool val;
       int vali;
       if(pXGI->VGAEngine != XGI_315_VGA) {
          if(xf86GetOptValBool(pXGI->Options, OPTION_TURBOQUEUE, &val)) {
             xf86DrvMsg(pScrn->scrnIndex, X_WARNING, mystring, "TurboQueue");
          }
       }
       if(xf86GetOptValBool(pXGI->Options, OPTION_RESTOREBYSET, &val)) {
	  xf86DrvMsg(pScrn->scrnIndex, X_WARNING, mystring, "RestoreBySetMode");
       }
       if(xf86GetOptValBool(pXGI->Options, OPTION_ENABLEHOTKEY, &val)) {
          xf86DrvMsg(pScrn->scrnIndex, X_WARNING, mystring, "EnableHotKey");
       }
       if(xf86GetOptValBool(pXGI->Options, OPTION_ENABLEXGICTRL, &val)) {
          xf86DrvMsg(pScrn->scrnIndex, X_WARNING, mystring, "EnableXGICtrl");
       }
       if(xf86GetOptValBool(pXGI->Options, OPTION_USEROMDATA, &val)) {
          xf86DrvMsg(pScrn->scrnIndex, X_WARNING, mystring, "UseROMData");
       }
       if(xf86GetOptValBool(pXGI->Options, OPTION_USEOEM, &val)) {
          xf86DrvMsg(pScrn->scrnIndex, X_WARNING, mystring, "UseOEMData");
       }
       if(xf86GetOptValBool(pXGI->Options, OPTION_FORCECRT1, &val)) {
          xf86DrvMsg(pScrn->scrnIndex, X_WARNING, mystring, "ForceCRT1");
       }
       if(xf86GetOptValBool(pXGI->Options, OPTION_NODDCFORCRT2, &val)) {
          xf86DrvMsg(pScrn->scrnIndex, X_WARNING, mystring, "NoCRT2Detection");
       }
       if(xf86GetOptValBool(pXGI->Options, OPTION_FORCECRT2REDETECTION, &val)) {
          xf86DrvMsg(pScrn->scrnIndex, X_WARNING, mystring, "ForceCRT2ReDetection");
       }
       if(xf86GetOptValBool(pXGI->Options, OPTION_SENSEYPBPR, &val)) {
          xf86DrvMsg(pScrn->scrnIndex, X_WARNING, mystring, "SenseYPbPr");
       }
       if(xf86GetOptValString(pXGI->Options, OPTION_FORCE_CRT1TYPE)) {
          xf86DrvMsg(pScrn->scrnIndex, X_WARNING, mystring, "ForceCRT1Type");
       }
       if(xf86GetOptValString(pXGI->Options, OPTION_FORCE_CRT2TYPE)) {
          xf86DrvMsg(pScrn->scrnIndex, X_WARNING, mystring, "ForceCRT2Type");
       }
       if(xf86GetOptValString(pXGI->Options, OPTION_YPBPRAR)) {
          xf86DrvMsg(pScrn->scrnIndex, X_WARNING, mystring, "YPbPrAspectRatio");
       }
       if(xf86GetOptValBool(pXGI->Options, OPTION_SCALELCD, &val)) {
          xf86DrvMsg(pScrn->scrnIndex, X_WARNING, mystring, "ScaleLCD");
       }
        if(xf86GetOptValBool(pXGI->Options, OPTION_CENTERLCD, &val)) {
          xf86DrvMsg(pScrn->scrnIndex, X_WARNING, mystring, "CenterLCD");
       }
       if((xf86GetOptValInteger(pXGI->Options, OPTION_PDC, &vali)) ||
          (xf86GetOptValInteger(pXGI->Options, OPTION_PDCS, &vali))) {
          xf86DrvMsg(pScrn->scrnIndex, X_WARNING, mystring, "PanelDelayCompensation (PDC)");
       }
       if((xf86GetOptValInteger(pXGI->Options, OPTION_PDCA, &vali)) ||
          (xf86GetOptValInteger(pXGI->Options, OPTION_PDCAS, &vali))) {
          xf86DrvMsg(pScrn->scrnIndex, X_WARNING, mystring, "PanelDelayCompensation1 (PDC1)");
       }
       if(xf86GetOptValInteger(pXGI->Options, OPTION_EMI, &vali)) {
          xf86DrvMsg(pScrn->scrnIndex, X_WARNING, mystring, "EMI");
       }
       if(xf86GetOptValString(pXGI->Options, OPTION_SPECIALTIMING)) {
          xf86DrvMsg(pScrn->scrnIndex, X_WARNING, mystring, "SpecialTiming");
       }
       if(xf86GetOptValString(pXGI->Options, OPTION_LVDSHL)) {
          xf86DrvMsg(pScrn->scrnIndex, X_WARNING, mystring, "LVDSHL");
       }
       if(xf86GetOptValString(pXGI->Options, OPTION_TVSTANDARD)) {
          xf86DrvMsg(pScrn->scrnIndex, X_WARNING, mystring, "TVStandard");
       }
       if(xf86GetOptValString(pXGI->Options, OPTION_CHTVTYPE)) {
          xf86DrvMsg(pScrn->scrnIndex, X_WARNING, mystring, "CHTVType");
       }
       if(xf86GetOptValBool(pXGI->Options, OPTION_CHTVOVERSCAN, &val)) {
          xf86DrvMsg(pScrn->scrnIndex, X_WARNING, mystring, "CHTVOverscan");
       }
       if(xf86GetOptValBool(pXGI->Options, OPTION_CHTVSOVERSCAN, &val)) {
          xf86DrvMsg(pScrn->scrnIndex, X_WARNING, mystring, "CHTVSuperOverscan");
       }
       if((xf86GetOptValInteger(pXGI->Options, OPTION_CHTVLUMABANDWIDTHCVBS, &vali)) ||
	  (xf86GetOptValInteger(pXGI->Options, OPTION_CHTVLUMABANDWIDTHSVIDEO, &vali)) ||
	  (xf86GetOptValInteger(pXGI->Options, OPTION_CHTVLUMAFLICKERFILTER, &vali)) ||
	  (xf86GetOptValInteger(pXGI->Options, OPTION_CHTVCHROMABANDWIDTH, &vali)) ||
	  (xf86GetOptValInteger(pXGI->Options, OPTION_CHTVCHROMAFLICKERFILTER, &vali)) ||
          (xf86GetOptValBool(pXGI->Options, OPTION_CHTVCVBSCOLOR, &val)) ||
	  (xf86GetOptValInteger(pXGI->Options, OPTION_CHTVTEXTENHANCE, &vali)) ||
	  (xf86GetOptValInteger(pXGI->Options, OPTION_CHTVCONTRAST, &vali)) ||
	  (xf86GetOptValInteger(pXGI->Options, OPTION_XGITVEDGEENHANCE, &vali)) ||
	  (xf86GetOptValString(pXGI->Options, OPTION_XGITVANTIFLICKER)) ||
	  (xf86GetOptValInteger(pXGI->Options, OPTION_XGITVSATURATION, &vali)) ||
	  (xf86GetOptValBool(pXGI->Options, OPTION_XGITVCHROMAFILTER, &val)) ||
	  (xf86GetOptValInteger(pXGI->Options, OPTION_XGITVLUMAFILTER, &vali)) ||
	  (xf86GetOptValInteger(pXGI->Options, OPTION_XGITVCOLCALIBCOARSE, &vali)) ||
	  (xf86GetOptValInteger(pXGI->Options, OPTION_XGITVCOLCALIBFINE, &vali)) ||
	  (xf86GetOptValInteger(pXGI->Options, OPTION_TVXPOSOFFSET, &vali)) ||
	  (xf86GetOptValInteger(pXGI->Options, OPTION_TVYPOSOFFSET, &vali)) ||
	  (xf86GetOptValInteger(pXGI->Options, OPTION_TVXSCALE, &vali)) ||
	  (xf86GetOptValInteger(pXGI->Options, OPTION_TVYSCALE, &vali))) {
	  xf86DrvMsg(pScrn->scrnIndex, X_WARNING,
	      "TV related options are only accepted in Master Head's device section");
       }
       if(xf86GetOptValBool(pXGI->Options, OPTION_CRT2GAMMA, &val)) {
          xf86DrvMsg(pScrn->scrnIndex, X_WARNING, mystring, "CRT2Gamma");
       }
       if(xf86GetOptValBool(pXGI->Options, OPTION_XVONCRT2, &val)) {
	  xf86DrvMsg(pScrn->scrnIndex, X_WARNING, mystring, "XvOnCRT2");
       }
#ifdef XGI_CP
       XGI_CP_OPT_DH_WARN
#endif
    } else
#endif
    {
	  /* TurboQueue */

          from = X_DEFAULT;
          if(xf86GetOptValBool(pXGI->Options, OPTION_TURBOQUEUE, &pXGI->TurboQueue)) {
    	     from = X_CONFIG;
          }
          xf86DrvMsg(pScrn->scrnIndex, from, "TurboQueue %s\n",
                    pXGI->TurboQueue ? enabledstr : disabledstr);

#ifdef XGI_CP
       XGI_CP_OPT_DOOPT
#endif

    }  /* DualHead */

    /* CRT1Gamma - enable/disable gamma correction for CRT1
     */
    {
       Bool val;
       if(xf86GetOptValBool(pXGI->Options, OPTION_CRT1GAMMA, &val)) {
	  pXGI->CRT1gamma = val;
	  pXGI->CRT1gammaGiven = TRUE;
       }
    }

    /* VESA - DEPRECATED
     * This option is for forcing the driver to use
     * the VESA BIOS extension for mode switching.
     */
    {
	Bool val;
	if(xf86GetOptValBool(pXGI->Options, OPTION_VESA, &val)) {
	    pXGI->VESA = val ? 1 : 0;
	    xf86DrvMsg(pScrn->scrnIndex, X_CONFIG,
	        "VESA: VESA usage shall be %s\n",
		val ? enabledstr : disabledstr);
 	    xf86DrvMsg(pScrn->scrnIndex, X_WARNING,
	    	"*** Option \"VESA\" is deprecated. *** \n");
	    if(pXGI->VESA) pXGI->ForceCRT1Type = CRT1_VGA;
	}
    }

    /* ShadowFB */
    from = X_DEFAULT;
    if(xf86GetOptValBool(pXGI->Options, OPTION_SHADOW_FB, &pXGI->ShadowFB)) {
#ifdef XGIMERGED
       if(pXGI->MergedFB) {
          pXGI->ShadowFB = FALSE;
	  xf86DrvMsg(pScrn->scrnIndex, X_WARNING,
	      "Shadow Frame Buffer not supported in MergedFB mode\n");
       } else
#endif
          from = X_CONFIG;
    }
    if(pXGI->ShadowFB) {
	pXGI->NoAccel = TRUE;
#if XF86_VERSION_CURRENT < XF86_VERSION_NUMERIC(4,2,99,0,0)
	pXGI->NoXvideo = TRUE;
    	xf86DrvMsg(pScrn->scrnIndex, from,
	   "Using \"Shadow Frame Buffer\" - 2D acceleration and Xv disabled\n");
#else
    	xf86DrvMsg(pScrn->scrnIndex, from,
	   "Using \"Shadow Frame Buffer\" - 2D acceleration disabled\n");
#endif
    }

    /* Rotate */
    if((strptr = (char *)xf86GetOptValString(pXGI->Options, OPTION_ROTATE))) {
#ifdef XGIMERGED
       if(pXGI->MergedFB) {
	  xf86DrvMsg(pScrn->scrnIndex, X_WARNING,
	      "Screen rotation not supported in MergedFB mode\n");
       } else
#endif
       if(!xf86NameCmp(strptr, "CW")) {
          pXGI->Rotate = 1;
       } else if(!xf86NameCmp(strptr, "CCW")) {
          pXGI->Rotate = -1;
       } else {
          xf86DrvMsg(pScrn->scrnIndex, X_WARNING, mybadparm, strptr, "Rotate");
          xf86DrvMsg(pScrn->scrnIndex, X_INFO,
              "Valid parameters are \"CW\" or \"CCW\"\n");
       }

       if(pXGI->Rotate) {
          pXGI->ShadowFB = TRUE;
          pXGI->NoAccel  = TRUE;
          pXGI->HWCursor = FALSE;
#if XF86_VERSION_CURRENT < XF86_VERSION_NUMERIC(4,2,99,0,0)
	  pXGI->NoXvideo = TRUE;
          xf86DrvMsg(pScrn->scrnIndex, X_CONFIG,
              "Rotating screen %sclockwise; (2D acceleration and Xv disabled)\n",
	      (pXGI->Rotate == -1) ? "counter " : "");
#else
	  xf86DrvMsg(pScrn->scrnIndex, X_CONFIG,
              "Rotating screen %sclockwise (2D acceleration %sdisabled)\n",
	      (pXGI->Rotate == -1) ? "counter " : "",
#if XF86_VERSION_CURRENT >= XF86_VERSION_NUMERIC(4,3,0,0,0)
              "and RandR extension "
#else
	      ""
#endif
	      );

#endif

       }
    }

#ifdef XF86DRI
    /* DRI */
    from = X_DEFAULT;
    if(xf86GetOptValBool(pXGI->Options, OPTION_DRI, &pXGI->loadDRI)) {
       from = X_CONFIG;
    }
    xf86DrvMsg(pScrn->scrnIndex, from, "DRI %s\n",
       pXGI->loadDRI ? enabledstr : disabledstr);

    /* AGPSize */
    {
       int vali;
       Bool gotit = FALSE;
       if(xf86GetOptValInteger(pXGI->Options, OPTION_AGP_SIZE, &vali)) {
          gotit = TRUE;
       } else if(xf86GetOptValInteger(pXGI->Options, OPTION_AGP_SIZE2, &vali)) {
          gotit = TRUE;
       }
       if(gotit) {
	  if((vali >= 8) && (vali <= 512)) {
	     pXGI->agpWantedPages = (vali * 1024 * 1024) / AGP_PAGE_SIZE;
	  } else {
	     xf86DrvMsg(pScrn->scrnIndex, X_WARNING, ilrangestr, "AGPSize (alias GARTSize)", 8, 512);
	  }
       }
    }
#endif

    /* NoXVideo
     * Set this to TRUE to disable Xv hardware video acceleration
     */
#if XF86_VERSION_CURRENT < XF86_VERSION_NUMERIC(4,2,99,0,0)
    if((!pXGI->NoAccel) && (!pXGI->NoXvideo)) {
#else
    if(!pXGI->NoXvideo) {
#endif
       if(xf86ReturnOptValBool(pXGI->Options, OPTION_NOXVIDEO, FALSE)) {
          pXGI->NoXvideo = TRUE;
          xf86DrvMsg(pScrn->scrnIndex, X_CONFIG, "XVideo extension disabled\n");
       }

       if(!pXGI->NoXvideo) {
          Bool val;
	  int tmp;

	  if((pXGI->VGAEngine == XGI_OLD_VGA) || (pXGI->VGAEngine == XGI_530_VGA)) {
	     /* NoYV12 (for 5597/5598, 6326 and 530/620 only)
	      * YV12 has problems with videos larger than 384x288. So
	      * allow the user to disable YV12 support to force the
	      * application to use YUV2 instead.
	      */
             if(xf86GetOptValBool(pXGI->Options, OPTION_NOYV12, &val)) {
	        pXGI->NoYV12 = val ? 1 : 0;
		xf86DrvMsg(pScrn->scrnIndex, X_CONFIG,
			"Xv YV12/I420 support is %s\n",
			pXGI->NoYV12 ? disabledstr : enabledstr);
	     }
	  }

	  /* Some Xv properties' defaults can be set by options */
          if(xf86GetOptValInteger(pXGI->Options, OPTION_XVDEFCONTRAST, &tmp)) {
             if((tmp >= 0) && (tmp <= 7)) pXGI->XvDefCon = tmp;
             else xf86DrvMsg(pScrn->scrnIndex, X_WARNING, ilrangestr,
       		      "XvDefaultContrast" ,0, 7);
          }
          if(xf86GetOptValInteger(pXGI->Options, OPTION_XVDEFBRIGHTNESS, &tmp)) {
             if((tmp >= -128) && (tmp <= 127)) pXGI->XvDefBri = tmp;
             else xf86DrvMsg(pScrn->scrnIndex, X_WARNING, ilrangestr,
       		      "XvDefaultBrightness", -128, 127);
          }
          
	  if(xf86GetOptValBool(pXGI->Options, OPTION_XVDEFDISABLEGFX, &val)) {
	     if(val)  pXGI->XvDefDisableGfx = TRUE;
	     xf86DrvMsg(pScrn->scrnIndex, X_CONFIG,
	        "Graphics display will be %s during Xv usage\n",
	     	val ? disabledstr : enabledstr);
          }
	  
	  if(xf86GetOptValBool(pXGI->Options, OPTION_XVMEMCPY, &val)) {
	     pXGI->XvUseMemcpy = val ? TRUE : FALSE;
	     xf86DrvMsg(pScrn->scrnIndex, X_CONFIG, "Xv will %suse memcpy()\n",
	     	val ? "" : "not ");
          }
       }
    }
}

const OptionInfoRec *
XGIAvailableOptions(int chipid, int busid)
{
    return XGIOptions;
}
