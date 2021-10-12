/* $XFree86: xc/programs/Xserver/hw/xfree86/vbe/vbe.c,v 1.19 2006/03/16 16:50:35 dawes Exp $ */

/*
 *                   XFree86 vbe module
 *               Copyright 2000 Egbert Eich
 *
 * The mode query/save/set/restore functions from the vesa driver
 * have been moved here.
 * Copyright (c) 2000 by Conectiva S.A. (http://www.conectiva.com)
 * Authors: Paulo César Pereira de Andrade <pcpa@conectiva.com.br>
 */

/*
 * Copyright (c) 2002-2006 by The XFree86 Project, Inc.
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject
 * to the following conditions:
 *
 *   1.  Redistributions of source code must retain the above copyright
 *       notice, this list of conditions, and the following disclaimer.
 *
 *   2.  Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer
 *       in the documentation and/or other materials provided with the
 *       distribution, and in the same place and form as other copyright,
 *       license and disclaimer information.
 *
 *   3.  The end-user documentation included with the redistribution,
 *       if any, must include the following acknowledgment: "This product
 *       includes software developed by The XFree86 Project, Inc
 *       (http://www.xfree86.org/) and its contributors", in the same
 *       place and form as other third-party acknowledgments.  Alternately,
 *       this acknowledgment may appear in the software itself, in the
 *       same form and location as other such third-party acknowledgments.
 *
 *   4.  Except as contained in this notice, the name of The XFree86
 *       Project, Inc shall not be used in advertising or otherwise to
 *       promote the sale, use or other dealings in this Software without
 *       prior written authorization from The XFree86 Project, Inc.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE XFREE86 PROJECT, INC OR ITS CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 * OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "vbe.h"
#include <X11/Xarch.h>
#define DPMS_SERVER
#include <X11/extensions/dpms.h>

#define VERSION(x) VBE_VERSION_MAJOR(x), VBE_VERSION_MINOR(x)

#define L_ADD(x)  (B_O32(x) & 0xffff) + ((B_O32(x) >> 12) & 0xffff00)

#define FARP(p)		((((unsigned)p & 0xffff0000) >> 12) + (p & 0xffff))
#define R16(v)		((v) & 0xffff)

static unsigned char * vbeReadEDID(vbeInfoPtr pVbe);
static Bool vbeProbeDDC(vbeInfoPtr pVbe);

const char *vbe_ddcSymbols[] = {
    "xf86InterpretEDID",
    "xf86DoEDID_Option",
    NULL
};

const char *vbe_int10Symbols[] = {
    "xf86ExecX86int10",
    "xf86FreeInt10",
    "xf86InitInt10",
    "xf86Int10AllocPages",
    "xf86Int10FreePages",
    "xf86int10Addr",
    NULL
};

static const char vbeVersionString[] = "VBE2";

vbeInfoPtr
VBEInit(xf86Int10InfoPtr pInt, int entityIndex)
{
    return VBEExtendedInit(pInt, entityIndex, 0);
}

vbeInfoPtr
VBEExtendedInit(xf86Int10InfoPtr pInt, int entityIndex, int Flags)
{
    int RealOff;
    pointer page = NULL;
    ScrnInfoPtr pScrn = xf86FindScreenForEntity(entityIndex);
    vbeControllerInfoPtr vbe = NULL;
    Bool init_int10 = FALSE;
    vbeInfoPtr vip = NULL;
    int screen = pScrn->scrnIndex;
    unsigned long TotalMem;
    ModuleDescPtr pMod, pInt10Mod = NULL;

    if (!pInt) {
	if (!(pInt10Mod = xf86LoadSubModule(pScrn, "int10")))
	    goto error;

	xf86LoaderModReqSymLists(pInt10Mod, vbe_int10Symbols, NULL);
	xf86DrvMsg(screen, X_INFO, "initializing int10\n");
	pInt = xf86InitInt10(entityIndex);
	if (!pInt)
	    goto error;
	init_int10 = TRUE;
    } else {
	if ((pMod = xf86GetSubModuleByName(pScrn->module, "int10")))
	    xf86LoaderModReqSymLists(pMod, vbe_int10Symbols, NULL);
    }

    page = xf86Int10AllocPages(pInt, 1, &RealOff);
    if (!page) goto error;
    vbe = (vbeControllerInfoPtr) page;
    (void) memset(vbe, 0, 512);
    (void) memcpy(vbe->VbeSignature, vbeVersionString, 4);

    pInt->ax = 0x4F00;
    pInt->es = SEG_ADDR(RealOff);
    pInt->di = SEG_OFF(RealOff);
    pInt->num = 0x10;

    xf86ExecX86int10(pInt);

    if ((pInt->ax & 0xff) != 0x4f) {
	xf86DrvMsgVerb(screen, X_INFO, 3, "VESA BIOS not detected\n");
	goto error;
    }

    switch (pInt->ax & 0xff00) {
    case 0:
	xf86DrvMsg(screen, X_INFO, "VESA BIOS detected\n");
	break;
    case 0x100:
	xf86DrvMsg(screen, X_INFO, "VESA BIOS function failed\n");
	goto error;
    case 0x200:
	xf86DrvMsg(screen, X_INFO, "VESA BIOS not supported\n");
	goto error;
    case 0x300:
	xf86DrvMsg(screen, X_INFO, "VESA BIOS not supported in current mode\n");
	goto error;
    default:
	xf86DrvMsg(screen, X_INFO, "Invalid\n");
	goto error;
    }

    xf86DrvMsgVerb(screen, X_INFO, 4,
		"VbeVersion is %ld, OemStringPtr is 0x%08lx,\n"
		"\tOemVendorNamePtr is 0x%08lx, OemProductNamePtr is 0x%08lx,\n"
		"\tOemProductRevPtr is 0x%08lx\n",
		(unsigned long)B_O16(vbe->VbeVersion),
		(unsigned long)B_O32(vbe->OemStringPtr),
		(unsigned long)B_O32(vbe->OemVendorNamePtr),
		(unsigned long)B_O32(vbe->OemProductNamePtr),
		(unsigned long)B_O32(vbe->OemProductRevPtr));

    xf86DrvMsgVerb(screen, X_INFO, 3, "VESA VBE Version %i.%i\n",
		   VERSION(vbe->VbeVersion));

    TotalMem = B_O16(vbe->TotalMem);
    /* Some BIOS'es are off by one */
    TotalMem += TotalMem & 1U;

    xf86DrvMsgVerb(screen, X_INFO, 3, "VESA VBE Total Mem: %li kB\n",
		   TotalMem * 64);
    xf86DrvMsgVerb(screen, X_INFO, 3, "VESA VBE OEM: %s\n",
		   (CARD8*)xf86int10Addr(pInt, L_ADD(vbe->OemStringPtr)));

    if (B_O16(vbe->VbeVersion) >= 0x200) {
	xf86DrvMsgVerb(screen, X_INFO, 3, "VESA VBE OEM Software Rev: %i.%i\n",
		    VERSION(vbe->OemSoftwareRev));
	if (vbe->OemVendorNamePtr)
	    xf86DrvMsgVerb(screen, X_INFO, 3, "VESA VBE OEM Vendor: %s\n",
		    (CARD8*)xf86int10Addr(pInt, L_ADD(vbe->OemVendorNamePtr)));
	if (vbe->OemProductNamePtr)
	    xf86DrvMsgVerb(screen, X_INFO, 3, "VESA VBE OEM Product: %s\n",
		    (CARD8*)xf86int10Addr(pInt, L_ADD(vbe->OemProductNamePtr)));
	if (vbe->OemProductRevPtr)
	    xf86DrvMsgVerb(screen, X_INFO, 3, "VESA VBE OEM Product Rev: %s\n",
		    (CARD8*)xf86int10Addr(pInt, L_ADD(vbe->OemProductRevPtr)));
    }
    xf86DrvMsgVerb(screen, X_INFO, 3, "VESA VBE Capabilities: "
		   "0x%02x%02x%02x%02x\n",
		   vbe->Capabilities[3], vbe->Capabilities[2],
		   vbe->Capabilities[1], vbe->Capabilities[0]);
    vip = (vbeInfoPtr)xnfalloc(sizeof(vbeInfoRec));
    vip->version = B_O16(vbe->VbeVersion);
    vip->pInt10 = pInt;
    vip->ddc = DDC_UNCHECKED;
    vip->memory = page;
    vip->real_mode_base = RealOff;
    vip->num_pages = 1;
    vip->init_int10 = init_int10;
    vip->pInt10Mod = pInt10Mod;
    vip->pDDCMod = NULL;

    return vip;

 error:
    if (page)
	xf86Int10FreePages(pInt, page, 1);
    if (init_int10) {
	xf86FreeInt10(pInt);
	if (pInt10Mod)
	    xf86UnloadSubModule(pInt10Mod);
    }
    return NULL;
}

void
vbeFree(vbeInfoPtr pVbe)
{
    if (!pVbe)
	return;

    xf86Int10FreePages(pVbe->pInt10, pVbe->memory, pVbe->num_pages);
    /* If we have initalized int10 we ought to free it, too */
    if (pVbe->init_int10) {
	xf86FreeInt10(pVbe->pInt10);
	if (pVbe->pInt10Mod)
	    xf86UnloadSubModule(pVbe->pInt10Mod);
    }
    if (pVbe->pDDCMod)
	xf86UnloadSubModule(pVbe->pDDCMod);
    xfree(pVbe);
    return;
}

static Bool
vbeProbeDDC(vbeInfoPtr pVbe)
{
    char *ddc_level;
    int screen = pVbe->pInt10->scrnIndex;

    if (!pVbe || (pVbe->ddc == DDC_NONE))
	return FALSE;
    if (pVbe->ddc != DDC_UNCHECKED)
	return TRUE;

    pVbe->pInt10->ax = 0x4F15;
    pVbe->pInt10->bx = 0;
    pVbe->pInt10->cx = 0;
    pVbe->pInt10->es = 0;
    pVbe->pInt10->di = 0;
    pVbe->pInt10->num = 0x10;

    xf86ExecX86int10(pVbe->pInt10);

    if ((pVbe->pInt10->ax & 0xff) != 0x4f) {
	xf86DrvMsgVerb(screen, X_INFO, 3, "VESA VBE DDC not supported\n");
	pVbe->ddc = DDC_NONE;
	return FALSE;
    }

    switch ((pVbe->pInt10->ax >> 8) & 0xff) {
    case 0:
	xf86DrvMsg(screen, X_INFO, "VESA VBE DDC supported\n");
	switch (pVbe->pInt10->bx & 0x3) {
	case 0:
	    ddc_level = " none";
	    pVbe->ddc = DDC_NONE;
	    break;
	case 1:
	    ddc_level = " 1";
	    pVbe->ddc = DDC_1;
	    break;
	case 2:
	    ddc_level = " 2";
	    pVbe->ddc = DDC_2;
	    break;
	case 3:
	    ddc_level = " 1 + 2";
	    pVbe->ddc = DDC_1_2;
	    break;
	default:
	    ddc_level = "";
	    pVbe->ddc = DDC_NONE;
	    break;
	}
	xf86DrvMsgVerb(screen, X_INFO, 3, "VESA VBE DDC Level%s\n", ddc_level);
	if (pVbe->ddc == DDC_NONE)
	    return FALSE;
	if (pVbe->pInt10->bx & 0x4) {
	    xf86DrvMsgVerb(screen, X_INFO, 3, "VESA VBE DDC Screen blanked"
			"for data transfer\n");
	    pVbe->ddc_blank = TRUE;
	} else {
	    pVbe->ddc_blank = FALSE;
	}

	xf86DrvMsgVerb(screen, X_INFO, 3,
		       "VESA VBE DDC transfer in approximately %x second(s)\n",
		       (pVbe->pInt10->bx >> 8) & 0xff);
    }

    return TRUE;
}

typedef enum {
    VBEOPT_NOVBE,
    VBEOPT_NODDC,
    VBEOPT_EDID_DATA
} VBEOpts;

static const OptionInfoRec VBEOptions[] = {
    { VBEOPT_NOVBE,	"NoVBE",	OPTV_BOOLEAN,	{0},	FALSE },
    { VBEOPT_NODDC,	"NoDDC",	OPTV_BOOLEAN,	{0},	FALSE },
    { VBEOPT_EDID_DATA,	"EDID Data",	OPTV_STRING,	{0},	FALSE },
    { -1,		NULL,		OPTV_NONE,	{0},	FALSE },
};

static unsigned char *
vbeReadEDID(vbeInfoPtr pVbe)
{
    int RealOff = pVbe->real_mode_base;
    pointer page = pVbe->memory;
    unsigned char *tmp = NULL;
    Bool novbe = FALSE;
    Bool noddc = FALSE;
    int screen = pVbe->pInt10->scrnIndex;
    OptionInfoPtr options;
    char *edidOption;

    if (!page) return NULL;

    options = xnfalloc(sizeof(VBEOptions));
    (void) memcpy(options, VBEOptions, sizeof(VBEOptions));
    xf86ProcessOptions(screen, xf86Screens[screen]->options, options);
    xf86GetOptValBool(options, VBEOPT_NOVBE, &novbe);
    xf86GetOptValBool(options, VBEOPT_NODDC, &noddc);
    edidOption = xf86GetOptValString(options, VBEOPT_EDID_DATA);
    xfree(options);
    if (novbe || noddc) return NULL;

    if (edidOption)
	tmp = xf86DoEDID_Option(screen, edidOption);
    if (tmp)
	return tmp;

    if (!vbeProbeDDC(pVbe)) goto error;

    (void) memset(page, 0, sizeof(vbeInfoPtr));
    (void) memcpy(page, vbeVersionString, 4);

    pVbe->pInt10->ax = 0x4F15;
    pVbe->pInt10->bx = 0x01;
    pVbe->pInt10->cx = 0;
    pVbe->pInt10->dx = 0;
    pVbe->pInt10->es = SEG_ADDR(RealOff);
    pVbe->pInt10->di = SEG_OFF(RealOff);
    pVbe->pInt10->num = 0x10;

    xf86ExecX86int10(pVbe->pInt10);

    if ((pVbe->pInt10->ax & 0xff) != 0x4f) {
	xf86DrvMsgVerb(screen, X_INFO, 3, "VESA VBE DDC invalid\n");
	goto error;
    }
    switch (pVbe->pInt10->ax & 0xff00) {
    case 0x0:
	xf86DrvMsgVerb(screen, X_INFO, 3, "VESA VBE DDC read successfully\n");
	tmp = (unsigned char *)xnfalloc(128);
	(void) memcpy(tmp, page, 128);
	break;
    case 0x100:
	xf86DrvMsgVerb(screen, X_INFO, 3, "VESA VBE DDC read failed\n");
	break;
    default:
	xf86DrvMsgVerb(screen, X_INFO, 3, "VESA VBE DDC unknown failure %i\n",
		       pVbe->pInt10->ax & 0xff00);
	break;
    }

 error:
    return tmp;
}

xf86MonPtr
vbeDoEDID(vbeInfoPtr pVbe, ModuleDescPtr pDDCModule)
{
    xf86MonPtr    pMonitor;
    ModuleDescPtr pModule;
    unsigned char *DDC_data = NULL;

    if (!pVbe) return NULL;
    if (pVbe->version < 0x200)
	return NULL;

    if (!(pModule = pDDCModule)) {
	pVbe->pDDCMod = pModule =
	    xf86LoadSubModule(xf86Screens[pVbe->pInt10->scrnIndex], "ddc");
	if (!pModule)
	    return NULL;

    }
    xf86LoaderModReqSymLists(pModule, vbe_ddcSymbols, NULL);

    DDC_data = vbeReadEDID(pVbe);

    if (!DDC_data)
	return NULL;

    pMonitor = xf86InterpretEDID(pVbe->pInt10->scrnIndex, DDC_data);

    return pMonitor;
}

VbeInfoBlock *
VBEGetVBEInfo(vbeInfoPtr pVbe)
{
    /*
    Input:
	AH    := 4Fh	Super VGA support
	AL    := 00h	Return Super VGA information
	ES:DI := Pointer to buffer

    Output:
	AX    := status
	(All other registers are preserved)
     */

    VbeInfoBlock *block = NULL;
    int i, pStr, pModes;
    char *str, *pBlock = pVbe->memory;
    CARD16 *modes;

    (void) memset(pBlock, 0, 512);
    (void) memcpy(pBlock, vbeVersionString, 4);

    pVbe->pInt10->num = 0x10;
    pVbe->pInt10->ax = 0x4f00;
    pVbe->pInt10->es = SEG_ADDR(pVbe->real_mode_base);
    pVbe->pInt10->di = SEG_OFF(pVbe->real_mode_base);
    xf86ExecX86int10(pVbe->pInt10);

    if (R16(pVbe->pInt10->ax) != 0x4f)
	return NULL;

    block = xnfcalloc(sizeof(VbeInfoBlock), 1);
    block->VESASignature[0] = pBlock[0];
    block->VESASignature[1] = pBlock[1];
    block->VESASignature[2] = pBlock[2];
    block->VESASignature[3] = pBlock[3];

    block->VESAVersion = B_O16(pBlock[4]);

    pStr = B_O32(pBlock[6]);
    str = xf86int10Addr(pVbe->pInt10, FARP(pStr));
    block->OEMStringPtr = strdup(str);

    block->Capabilities[0] = pBlock[10];
    block->Capabilities[1] = pBlock[11];
    block->Capabilities[2] = pBlock[12];
    block->Capabilities[3] = pBlock[13];

    /*
     * XXX This doesn't check if the mode list overflows.
     */
    pModes = B_O32(pBlock[14]);
    modes = xf86int10Addr(pVbe->pInt10, FARP(pModes));
    for (i = 0;  modes[i] != 0xffff;  i++);
    block->VideoModePtr = xnfalloc(sizeof(CARD16) * i + 1);
    for (i = 0;  modes[i] != 0xffff;  i++)
	block->VideoModePtr[i] = B_O16(modes[i]);
    block->VideoModePtr[i] = 0xffff;

    block->TotalMemory = B_O16(pBlock[18]);
    block->TotalMemory += block->TotalMemory & 1U;

    if (block->VESAVersion < 0x0200) {
	(void) memcpy(&block->Reserved, pBlock + 20, 236);
    } else {
	block->OemSoftwareRev = B_O16(pBlock[20]);

	/*
	 * XXX Consider using strndup for these three.
	 */

	pStr = B_O32(pBlock[22]);
	str = xf86int10Addr(pVbe->pInt10, FARP(pStr));
	block->OemVendorNamePtr = strdup(str);

	pStr = B_O32(pBlock[26]);
	str = xf86int10Addr(pVbe->pInt10, FARP(pStr));
	block->OemProductNamePtr = strdup(str);

	pStr = B_O32(pBlock[30]);
	str = xf86int10Addr(pVbe->pInt10, FARP(pStr));
	block->OemProductRevPtr = strdup(str);

	(void) memcpy(&block->Reserved, pBlock + 34, 222);
	(void) memcpy(&block->OemData, pBlock + 256, 256);
    }

    return block;
}

void
VBEFreeVBEInfo(VbeInfoBlock *block)
{
    if (!block)
	return;

    xfree(block->OEMStringPtr);
    xfree(block->VideoModePtr);
    if (((unsigned)block->VESAVersion >> 8) >= 2) {
	xfree(block->OemVendorNamePtr);
	xfree(block->OemProductNamePtr);
	xfree(block->OemProductRevPtr);
    }
    xfree(block);
}

Bool
VBESetVBEMode(vbeInfoPtr pVbe, int mode, VbeCRTCInfoBlock *block)
{
    /*
    Input:
	AH    := 4Fh	Super VGA support
	AL    := 02h	Set Super VGA video mode
	BX    := Video mode
	    D0-D8  := Mode number
	    D9-D10 := Reserved (must be 0)
	    D11    := 0 Use current default refresh rate
		   := 1 Use user specified CRTC values for refresh rate
	    D12-13	Reserved for VBE/AF (must be 0)
	    D14    := 0 Use windowed frame buffer model
		   := 1 Use linear/flat frame buffer model
	    D15    := 0 Clear video memory
		   := 1 Don't clear video memory
	ES:DI := Pointer to VbeCRTCInfoBlock structure

    Output: AX = Status
	(All other registers are preserved)
    */

    xf86Int10InfoPtr pInt10 = pVbe->pInt10;

    pInt10->num = 0x10;
    pInt10->ax = 0x4f02;
    pInt10->bx = mode & ~(1 << 11);
    if (block) {
	CARD8 *pBlock = pVbe->memory;

	pInt10->bx |= 1 << 11;
	pInt10->es = SEG_ADDR(pVbe->real_mode_base);
	pInt10->di = SEG_OFF(pVbe->real_mode_base);

	/*
	 * Move most fields byte by byte to avoid endianness and alignment
	 * issues.
	 */
	pBlock[0] = block->HorizontalTotal;
	pBlock[1] = block->HorizontalTotal >> 8;
	pBlock[2] = block->HorizontalSyncStart;
	pBlock[3] = block->HorizontalSyncStart >> 8;
	pBlock[4] = block->HorizontalSyncEnd;
	pBlock[5] = block->HorizontalSyncEnd >> 8;
	pBlock[6] = block->VerticalTotal;
	pBlock[7] = block->VerticalTotal >> 8;
	pBlock[8] = block->VerticalSyncStart;
	pBlock[9] = block->VerticalSyncStart >> 8;
	pBlock[10] = block->VerticalSyncEnd;
	pBlock[11] = block->VerticalSyncEnd >> 8;
	pBlock[12] = block->Flags;
	pBlock[13] = block->PixelClock;
	pBlock[14] = block->PixelClock >> 8;
	pBlock[15] = block->PixelClock >> 16;
	pBlock[16] = block->PixelClock >> 24;
	pBlock[17] = block->RefreshRate;
	pBlock[18] = block->RefreshRate >> 8;

	(void) memcpy(pBlock + 19, block->Reserved, 40);
    }

    xf86ExecX86int10(pInt10);

    return (R16(pInt10->ax) == 0x4f);
}

Bool
VBEGetVBEMode(vbeInfoPtr pVbe, int *mode)
{
    /*
    Input:
	AH := 4Fh	Super VGA support
	AL := 03h	Return current video mode

    Output:
	AX := Status
	BX := Current video mode
	(All other registers are preserved)
    */

    pVbe->pInt10->num = 0x10;
    pVbe->pInt10->ax = 0x4f03;

    xf86ExecX86int10(pVbe->pInt10);

    if (R16(pVbe->pInt10->ax) != 0x4f)
	return FALSE;

    *mode = R16(pVbe->pInt10->bx);
    return TRUE;
}

VbeModeInfoBlock *
VBEGetModeInfo(vbeInfoPtr pVbe, int mode)
{
    /*
    Input:
	AH    := 4Fh	Super VGA support
	AL    := 01h	Return Super VGA mode information
	CX    :=	Super VGA video mode
			(mode number must be one returned by Function 0)
	ES:DI := Pointer to buffer

    Output:
	AX    := status
	(All other registers are preserved)
     */

    VbeModeInfoBlock *block;
    char *pBlock = pVbe->memory;

    (void) memset(pBlock, 0, 256);

    pVbe->pInt10->num = 0x10;
    pVbe->pInt10->ax = 0x4f01;
    pVbe->pInt10->cx = mode;
    pVbe->pInt10->es = SEG_ADDR(pVbe->real_mode_base);
    pVbe->pInt10->di = SEG_OFF(pVbe->real_mode_base);
    xf86ExecX86int10(pVbe->pInt10);
    if (R16(pVbe->pInt10->ax) != 0x4f)
	return NULL;

    block = xnfcalloc(sizeof(VbeModeInfoBlock), 1);

    block->ModeAttributes = B_O16(pBlock[0]);
    block->WinAAttributes = pBlock[2];
    block->WinBAttributes = pBlock[3];
    block->WinGranularity = B_O16(pBlock[4]);
    block->WinSize = B_O16(pBlock[6]);
    block->WinASegment = B_O16(pBlock[8]);
    block->WinBSegment = B_O16(pBlock[10]);
    block->WinFuncPtr = B_O32(pBlock[12]);
    block->BytesPerScanline = B_O16(pBlock[16]);

    if (pVbe->version < 0x0102) {
	(void) memcpy(&block->Reserved2, pBlock + 18, 238);
	return block;
    }

    /* mandatory information for VBE 1.2 and above */
    block->XResolution = B_O16(pBlock[18]);
    block->YResolution = B_O16(pBlock[20]);
    block->XCharSize = pBlock[22];
    block->YCharSize = pBlock[23];
    block->NumberOfPlanes = pBlock[24];
    block->BitsPerPixel = pBlock[25];
    block->NumberOfBanks = pBlock[26];
    block->MemoryModel = pBlock[27];
    block->BankSize = pBlock[28];
    block->NumberOfImages = pBlock[29];
    block->Reserved = pBlock[30];

    /* Direct color fields (required for direct/6 and YUV/7 memory models) */
    block->RedMaskSize = pBlock[31];
    block->RedFieldPosition = pBlock[32];
    block->GreenMaskSize = pBlock[33];
    block->GreenFieldPosition = pBlock[34];
    block->BlueMaskSize = pBlock[35];
    block->BlueFieldPosition = pBlock[36];
    block->RsvdMaskSize = pBlock[37];
    block->RsvdFieldPosition = pBlock[38];
    block->DirectColorModeInfo = pBlock[39];

    if (pVbe->version < 0x0200) {
	(void) memcpy(&block->Reserved2, pBlock + 40, 216);
	return block;
    }

    /* Mandatory information for VBE 2.0 and above */
    block->PhysBasePtr = B_O32((pBlock[40]));
    block->Reserved32 = B_O32(pBlock[44]);
    block->Reserved16 = B_O16(pBlock[48]);

    if (pVbe->version < 0x0300) {
	(void) memcpy(&block->Reserved2, pBlock + 50, 206);
	return block;
    }

    /* Mandatory information for VBE 3.0 and above */
    block->LinBytesPerScanLine = B_O16(pBlock[50]);
    block->BnkNumberOfImagePages = pBlock[52];
    block->LinNumberOfImagePages = pBlock[53];
    block->LinRedMaskSize = pBlock[54];
    block->LinRedFieldPosition = pBlock[55];
    block->LinGreenMaskSize = pBlock[56];
    block->LinGreenFieldPosition = pBlock[57];
    block->LinBlueMaskSize = pBlock[58];
    block->LinBlueFieldPosition = pBlock[59];
    block->LinRsvdMaskSize = pBlock[60];
    block->LinRsvdFieldPosition = pBlock[61];
    block->MaxPixelClock = B_O32(pBlock[62]);
    (void) memcpy(&block->Reserved2, pBlock + 66, 190);

    return block;
}

void
VBEFreeModeInfo(VbeModeInfoBlock *block)
{
    xfree(block);
}

Bool
VBESaveRestore(vbeInfoPtr pVbe, vbeSaveRestoreFunction function,
	       pointer *memory, int *size, int *real_mode_pages)
{
    /*
    Input:
	AH    := 4Fh	Super VGA support
	AL    := 04h	Save/restore Super VGA video state
	DL    := 00h	Return save/restore state buffer size
	CX    := Requested states
		D0 = Save/restore video hardware state
		D1 = Save/restore video BIOS data state
		D2 = Save/restore video DAC state
		D3 = Save/restore Super VGA state

    Output:
	AX = Status
	BX = Number of 64-byte blocks to hold the state buffer
	(All other registers are preserved)

    Input:
	AH    := 4Fh	Super VGA support
	AL    := 04h	Save/restore Super VGA video state
	DL    := 01h	Save Super VGA video state
	CX    := Requested states (see above)
	ES:BX := Pointer to buffer

    Output:
	AX    := Status
	(All other registers are preserved)

    Input:
	AH    := 4Fh	Super VGA support
	AL    := 04h	Save/restore Super VGA video state
	DL    := 02h	Restore Super VGA video state
	CX    := Requested states (see above)
	ES:BX := Pointer to buffer

    Output:
	AX     := Status
	(All other registers are preserved)
     */

    xf86Int10InfoPtr pInt10;

    /* Earlier VBE versions won't give us an error indication */
    if (pVbe->version < 0x0200)
	return TRUE;

    pInt10 = pVbe->pInt10;

    if ((function == MODE_QUERY) || ((function == MODE_SAVE) && !*memory)) {
	/* Query amount of memory to save state */

	pInt10->num = 0x10;
	pInt10->ax = 0x4f04;
	pInt10->dx = 0;
	pInt10->cx = 0x000f;
	xf86ExecX86int10(pInt10);
	if (R16(pInt10->ax) != 0x4f)
	    return FALSE;

	if (function == MODE_SAVE) {
	    int npages = (R16(pInt10->bx) * 64) / 4096 + 1;

	    *memory = xf86Int10AllocPages(pInt10, npages, real_mode_pages);
	    if (!*memory) {
		xf86DrvMsg(pInt10->scrnIndex, X_ERROR,
			   "Cannot allocate memory to save SVGA state.\n");
		return FALSE;
	    }
	}

	*size = pInt10->bx * 64;

	if (function == MODE_QUERY)
	    return TRUE;
    }

    /* Save/Restore Super VGA state */
    if (!*memory)
	return FALSE;

    pInt10->num = 0x10;
    pInt10->ax = 0x4f04;
    pInt10->cx = 0x000f;
    pInt10->es = SEG_ADDR(*real_mode_pages);
    pInt10->bx = SEG_OFF(*real_mode_pages);

    switch (function) {
    case MODE_SAVE:
	pInt10->dx = 1;
	break;
    case MODE_RESTORE:
	pInt10->dx = 2;
	break;
    default:
	return FALSE;
    }

    xf86ExecX86int10(pInt10);
    return (R16(pInt10->ax) == 0x4f);
}

Bool
VBEBankSwitch(vbeInfoPtr pVbe, unsigned int iBank, int window)
{
    /*
    Input:
	AH    := 4Fh	Super VGA support
	AL    := 05h
	BH    := 00h	Set memory window
	      := 01h	Get memory window
	BL    := 00h	Window A
	      := 01h	Window B
	DX    := Window number to set

    Output:
	AX    := Status
	DX    := Window number retrieved
     */

    pVbe->pInt10->num = 0x10;
    pVbe->pInt10->ax = 0x4f05;
    pVbe->pInt10->bx = window;
    pVbe->pInt10->dx = iBank;
    xf86ExecX86int10(pVbe->pInt10);

    return (R16(pVbe->pInt10->ax) == 0x4f);
}

Bool
VBESetGetLogicalScanlineLength(vbeInfoPtr pVbe, vbeScanwidthCommand command,
				int width, int *pixels, int *bytes, int *max)
{
    /*
    Input:
	AX := 4F06h VBE Set/Get Logical Scan Line Length
	BL := 00h Set Scan Line Length in Pixels
	   := 01h Get Scan Line Length
	   := 02h Set Scan Line Length in Bytes
	   := 03h Get Maximum Scan Line Length
	CX := If BL=00h Desired Width in Pixels
	      If BL=02h Desired Width in Bytes
	      (Ignored for Get Functions)

    Output:
	AX := VBE Return Status
	BX := Bytes Per Scan Line
	CX := Actual Pixels Per Scan Line
	      (truncated to nearest complete pixel)
	DX := Maximum Number of Scan Lines
     */

    if (command < SCANWID_SET || command > SCANWID_GET_MAX)
	return FALSE;

    pVbe->pInt10->num = 0x10;
    pVbe->pInt10->ax = 0x4f06;
    pVbe->pInt10->bx = command;
    if (command == SCANWID_SET || command == SCANWID_SET_BYTES)
	pVbe->pInt10->cx = width;
    xf86ExecX86int10(pVbe->pInt10);

    if (R16(pVbe->pInt10->ax) != 0x4f)
	return FALSE;

    if (command == SCANWID_GET || command == SCANWID_GET_MAX) {
	if (pixels)
	    *pixels = R16(pVbe->pInt10->cx);
	if (bytes)
	    *bytes = R16(pVbe->pInt10->bx);
	if (max)
	    *max = R16(pVbe->pInt10->dx);
    }

    return TRUE;
}

Bool
VBESetDisplayStart(vbeInfoPtr pVbe, int x, int y, Bool wait_retrace)
{
    pVbe->pInt10->num = 0x10;
    pVbe->pInt10->ax = 0x4f07;
    pVbe->pInt10->bx = wait_retrace ? 0x80 : 0x00;
    pVbe->pInt10->cx = x;
    pVbe->pInt10->dx = y;
    xf86ExecX86int10(pVbe->pInt10);

    return (R16(pVbe->pInt10->ax) == 0x4f);
}

Bool
VBEGetDisplayStart(vbeInfoPtr pVbe, int *x, int *y)
{
    pVbe->pInt10->num = 0x10;
    pVbe->pInt10->ax = 0x4f07;
    pVbe->pInt10->bx = 0x01;
    xf86ExecX86int10(pVbe->pInt10);

    if (R16(pVbe->pInt10->ax) != 0x4f)
	return FALSE;

    *x = pVbe->pInt10->cx;
    *y = pVbe->pInt10->dx;

    return TRUE;
}

int
VBESetGetDACPaletteFormat(vbeInfoPtr pVbe, int bits)
{
    /*
    Input:
	AX := 4F08h VBE Set/Get Palette Format
	BL := 00h Set DAC Palette Format
	   := 01h Get DAC Palette Format
	BH := Desired bits of color per primary
	      (Set DAC Palette Format only)

    Output:
	AX := VBE Return Status
	BH := Current number of bits of color per primary
     */

    pVbe->pInt10->num = 0x10;
    pVbe->pInt10->ax = 0x4f08;
    if (!bits)
	pVbe->pInt10->bx = 0x01;
    else
	pVbe->pInt10->bx = (bits & 0x00ff) << 8;
    xf86ExecX86int10(pVbe->pInt10);

    if (R16(pVbe->pInt10->ax) != 0x4f)
	return 0;

    return (bits != 0 ? bits : (pVbe->pInt10->bx >> 8) & 0x00ff);
}

CARD32 *
VBESetGetPaletteData(vbeInfoPtr pVbe, Bool set, int first, int num,
		      CARD32 *data, Bool secondary, Bool wait_retrace)
{
    /*
    Input:
	AX    := 4F09h VBE Load/Unload Palette Data
	BL    := 00h Set Palette Data
	      := 01h Get Palette Data
	      := 02h Set Secondary Palette Data
	      := 03h Get Secondary Palette Data
	      := 80h Set Palette Data during Vertical Retrace
	CX    := Number of palette registers to update (to a maximum of 256)
	DX    := First of the palette registers to update (start)
	ES:DI := Table of palette values (see below for format)

    Output:
	AX    := VBE Return Status
     */

    pVbe->pInt10->num = 0x10;
    pVbe->pInt10->ax = 0x4f09;
    if (!secondary)
	pVbe->pInt10->bx = set && wait_retrace ? 0x80 : set ? 0 : 1;
    else
	pVbe->pInt10->bx = set ? 2 : 3;
    pVbe->pInt10->cx = num;
    pVbe->pInt10->dx = first;
    pVbe->pInt10->es = SEG_ADDR(pVbe->real_mode_base);
    pVbe->pInt10->di = SEG_OFF(pVbe->real_mode_base);
    if (set)
	(void) memcpy(pVbe->memory, data, num * sizeof(CARD32));
    xf86ExecX86int10(pVbe->pInt10);

    if (R16(pVbe->pInt10->ax) != 0x4f)
	return NULL;

    if (set)
	return data;

    data = xnfalloc(num * sizeof(CARD32));
    (void) memcpy(data, pVbe->memory, num * sizeof(CARD32));

    return data;
}

VBEpmi *
VBEGetVBEpmi(vbeInfoPtr pVbe)
{
    /*
    Input:
	AH    := 4Fh	Super VGA support
	AL    := 0Ah	Protected Mode Interface
	BL    := 00h	Return Protected Mode Table

    Output:
	AX    := Status
	ES    := Real Mode Segment of Table
	DI    := Offset of Table
	CX    := Length of Table including protected mode code in bytes
		 (for copying purposes)
	(All other registers are preserved)
     */

    VBEpmi *pmi;

    pVbe->pInt10->num = 0x10;
    pVbe->pInt10->ax = 0x4f0a;
    pVbe->pInt10->bx = 0;
    pVbe->pInt10->di = 0;
    xf86ExecX86int10(pVbe->pInt10);

    if (R16(pVbe->pInt10->ax) != 0x4f)
	return NULL;

    pmi = xnfalloc(sizeof(VBEpmi));
    pmi->seg_tbl = R16(pVbe->pInt10->es);
    pmi->tbl_off = R16(pVbe->pInt10->di);
    pmi->tbl_len = R16(pVbe->pInt10->cx);

    return pmi;
}

int
VBEGetPixelClock(vbeInfoPtr pVbe, int mode, int clock)
{
    /*
    Input:
	AX := 4F0Bh VBE Get Pixel Clock
	BL := 01h Get Pixel Clock
	ECX := pixel clock in units of Hz
	DX := mode number

    Output:
	AX := VBE Return Status
	ECX := Closest pixel clock
     */

    pVbe->pInt10->num = 0x10;
    pVbe->pInt10->ax = 0x4f0b;
    pVbe->pInt10->bx = 0x01;
    pVbe->pInt10->cx = clock;
    pVbe->pInt10->dx = mode;
    xf86ExecX86int10(pVbe->pInt10);

    if (R16(pVbe->pInt10->ax) != 0x4f)
	return 0;

    return pVbe->pInt10->cx;
}

Bool
VBEDPMSSet(vbeInfoPtr pVbe, int mode)
{
    /*
    Input:
	AX := 4F10h DPMS
	BL := 01h Set Display Power State
	BH := requested power state

    Output:
	AX := VBE Return Status
     */

    pVbe->pInt10->num = 0x10;
    pVbe->pInt10->ax = 0x4f10;
    pVbe->pInt10->bx = 0x01;
    switch (mode) {
    case DPMSModeOn:
	break;
    case DPMSModeStandby:
	pVbe->pInt10->bx |= 0x100;
	break;
    case DPMSModeSuspend:
	pVbe->pInt10->bx |= 0x200;
	break;
    case DPMSModeOff:
	pVbe->pInt10->bx |= 0x400;
	break;
    }
    xf86ExecX86int10(pVbe->pInt10);
    return (R16(pVbe->pInt10->ax) == 0x4f);
}

