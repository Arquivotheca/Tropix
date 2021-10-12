/* $XFree86: xc/programs/Xserver/hw/xfree86/vbe/vbe.h,v 1.13 2007/03/08 01:46:43 tsi Exp $ */

/*
 *                   XFree86 vbe module
 *               Copyright 2000 Egbert Eich
 *
 * The mode query/save/set/restore functions from the vesa driver
 * have been moved here.
 * Copyright (c) 2000 by Conectiva S.A. (http://www.conectiva.com)
 * Authors: Paulo César Pereira de Andrade <pcpa@conectiva.com.br>
 */

#ifndef _VBE_H
#define _VBE_H
#include "xf86.h"
#include "xf86_ansic.h"
#include "compiler.h"
#include "xf86int10.h"
#include "xf86DDC.h"

/* Endianness-neutral unaligned access definitions */
#define B8P(x)   ((CARD8 *)(&(x)))
#define B_O08(x) ((CARD32)(B8P(x)[0]))
#define B_O16(x) ((CARD32)(B8P(x)[0]) | \
		 ((CARD32)(B8P(x)[1]) << 8))
#define B_O32(x) ((CARD32)(B8P(x)[0]) | \
		 ((CARD32)(B8P(x)[1]) << 8) | \
		 ((CARD32)(B8P(x)[2]) << 16) | \
		 ((CARD32)(B8P(x)[3]) << 24))

typedef enum {
    DDC_UNCHECKED,
    DDC_NONE,
    DDC_1,
    DDC_2,
    DDC_1_2
} ddc_lvl;

typedef struct {
    xf86Int10InfoPtr pInt10;
    int version;
    pointer memory;
    int real_mode_base;
    int num_pages;
    Bool init_int10;
    ddc_lvl ddc;
    Bool ddc_blank;
    ModuleDescPtr pInt10Mod;
    ModuleDescPtr pDDCMod;
} vbeInfoRec, *vbeInfoPtr;

#define VBE_VERSION_MAJOR(x) (B8P(x)[1])
#define VBE_VERSION_MINOR(x) (B8P(x)[0])

vbeInfoPtr VBEInit(xf86Int10InfoPtr pInt, int entityIndex);
vbeInfoPtr VBEExtendedInit(xf86Int10InfoPtr pInt, int entityIndex, int Flags);
void vbeFree(vbeInfoPtr pVbe);
xf86MonPtr vbeDoEDID(vbeInfoPtr pVbe, ModuleDescPtr pDDCModule);

#pragma pack(1)

typedef struct vbeControllerInfoBlock {
    CARD8 VbeSignature[4];
    CARD16 VbeVersion;
    CARD32 OemStringPtr;
    CARD8 Capabilities[4];
    CARD32 VideoModePtr;
    CARD16 TotalMem;
    CARD16 OemSoftwareRev;
    CARD32 OemVendorNamePtr;
    CARD32 OemProductNamePtr;
    CARD32 OemProductRevPtr;
    CARD8  Scratch[222];
    CARD8  OemData[256];
} vbeControllerInfoRec, *vbeControllerInfoPtr;

#if defined(__GNUC__) || \
    defined(__SCO__) || \
    defined(__USLC__) || \
    defined(__SUNPRO_C)
#pragma pack()	/* All GCC versions recognise this syntax */
#else
#pragma pack(0)
#endif

/*
 * The three Vbe.*InfoBlock structures below were originally defined as packed
 * in line with their VBE counterparts.  Strictly speaking, this is unnecessary
 * given their contents must be massaged for endianness, alignment, etc. before
 * being used to interact directly with the BIOS.
 *
 * It is also undesirable because their layout then varies by compiler and, in
 * VbeInfoBlock's case, platform pointer size.  Thus the layout of these
 * structures has been changed to remove any lingering expectation that they
 * match their VBE counterparts.
 *
 * The following definitions are meant to deal with the incompatibility between
 * modules that arose when these structures were changed.
 */

#undef xf86LoadVBEModule

#if defined(XFree86LOADER)

    static __inline__ ModuleDescPtr
    xf86LoadVBEModule(ScrnInfoPtr pScreenInfo)
    {
	/*
	 * The Vbe*.InfoBlock structures were reworked starting with vbe module
	 * version number 2.0.0, video driver ABI version 0.9.
	 */
	static XF86ModReqInfo vbeModuleVersionRequirements =
	{
	    2, 0, 0,			/* major, minor, patchlevel */
	    ABI_CLASS_VIDEODRV,		/* abiclass */
	    SET_ABI_VERSION(0, 9),	/* abiversion */
	    MOD_CLASS_NONE		/* moduleclass */
	};

	return xf86LoadSubModuleWithRequirements(pScreenInfo, "vbe",
						 &vbeModuleVersionRequirements);
    }

#    define xf86LoadVBEModule(_pScreenInfo) \
	    xf86LoadVBEModule(_pScreenInfo)
#else

#    define xf86LoadVBEModule(_pScreenInfo) \
	    xf86LoadSubModule(_pScreenInfo, "vbe")

#endif

typedef struct _VbeInfoBlock VbeInfoBlock;
typedef struct _VbeModeInfoBlock VbeModeInfoBlock;
typedef struct _VbeCRTCInfoBlock VbeCRTCInfoBlock;

/*
 * INT 0
 */

struct _VbeInfoBlock {
    /* VESA 1.2 fields */
    CARD8 VESASignature[4];	/* VESA */
    CARD16 VESAVersion;		/* Higher byte major, lower byte minor */
    CARD8 Capabilities[4];	/* Capabilities of the video environment */
    CARD16 TotalMemory;		/* Number of 64kb memory blocks on board */
    char *OEMStringPtr;		/* Pointer to OEM string */
    CARD16 *VideoModePtr;	/* pointer to supported Super VGA modes */

    /* VESA 2 fields */
    char *OemVendorNamePtr;	/* Pointer to Vendor Name String */
    char *OemProductNamePtr;	/* Pointer to Product Name String */
    char *OemProductRevPtr;	/* Pointer to Product Revision String */
    CARD16 OemSoftwareRev;	/* VBE implementation Software revision */

    /* Allow enough space for VESA 1's reserved field */
    CARD8 Reserved[236];	/* Reserved for VBE implementation */
    /* This only exists for VESA 2 */
    CARD8 OemData[256];		/* Data Area for OEM Strings */
};

/* Return Super VGA Information */
VbeInfoBlock *VBEGetVBEInfo(vbeInfoPtr pVbe);
void VBEFreeVBEInfo(VbeInfoBlock *block);

/*
 * INT 1
 */

struct _VbeModeInfoBlock {
    CARD16 ModeAttributes;		/* mode attributes */
    CARD8 WinAAttributes;		/* window A attributes */
    CARD8 WinBAttributes;		/* window B attributes */
    CARD16 WinGranularity;		/* window granularity */
    CARD16 WinSize;			/* window size */
    CARD16 WinASegment;			/* window A start segment */
    CARD16 WinBSegment;			/* window B start segment */
    CARD32 WinFuncPtr;			/* far pointer to window function */
    CARD16 BytesPerScanline;		/* bytes per scanline */

    /* Mandatory information for VBE 1.2 and above */
    CARD16 XResolution;			/* horizontal pixels or characters */
    CARD16 YResolution;			/* vertical pixels or characters */
    CARD8 XCharSize;			/* character cell width in pixels */
    CARD8 YCharSize;			/* character cell height in pixels */
    CARD8 NumberOfPlanes;		/* number of memory planes */
    CARD8 BitsPerPixel;			/* bits per pixel */
    CARD8 NumberOfBanks;		/* number of banks */
    CARD8 MemoryModel;			/* memory model type */
    CARD8 BankSize;			/* bank size in KB */
    CARD8 NumberOfImages;		/* number of images */
    CARD8 Reserved;	/* 1 */		/* reserved for page function */

    /* Direct color fields (required for direct/6 and YUV/7 memory models) */
    CARD8 RedMaskSize;			/* bit size of red mask */
    CARD8 RedFieldPosition;		/* bit position of red mask lsb */
    CARD8 GreenMaskSize;		/* bit size of green mask */
    CARD8 GreenFieldPosition;		/* bit position of green mask lsb */
    CARD8 BlueMaskSize;			/* bit size of blue mask */
    CARD8 BlueFieldPosition;		/* bit position of blue mask lsb */
    CARD8 RsvdMaskSize;			/* bit size of reserved mask */
    CARD8 RsvdFieldPosition;		/* bit position of reserved mask lsb */
    CARD8 DirectColorModeInfo;		/* direct color mode attributes */

    /* Mandatory information for VBE 2.0 and above */
    CARD32 PhysBasePtr;			/* physical address of linear
					   framebuffer */
    CARD32 Reserved32;	/* 0 */		/* Reserved - always set to 0 */
    CARD16 Reserved16;	/* 0 */		/* Reserved - always set to 0 */

    /* Mandatory information for VBE 3.0 and above */
    CARD16 LinBytesPerScanLine;		/* bytes per scanline */
    CARD8 BnkNumberOfImagePages;	/* number of images for banked modes */
    CARD8 LinNumberOfImagePages;	/* number of images for linear modes */
    CARD8 LinRedMaskSize;		/* size of direct color red mask */
    CARD8 LinRedFieldPosition;		/* bit position of red mask lsb */
    CARD8 LinGreenMaskSize;		/* size of direct color green mask */
    CARD8 LinGreenFieldPosition;	/* bit position of green mask lsb */
    CARD8 LinBlueMaskSize;		/* size of direct color blue mask */
    CARD8 LinBlueFieldPosition;		/* bit position of blue mask lsb */
    CARD8 LinRsvdMaskSize;		/* size of direct color reserved mask */
    CARD8 LinRsvdFieldPosition;		/* bit position of reserved mask lsb */

    CARD16 pad;				/* two-byte fluff */
    CARD32 MaxPixelClock;		/* maximum pixel clock (in Hz) for
					   graphics mode */
    /* Allow enough space for VESA 1.1's reserved field */
    CARD8 Reserved2[238];		/* remainder of VbeModeInfoBlock */
};

/* Return VBE Mode Information */
VbeModeInfoBlock *VBEGetModeInfo(vbeInfoPtr pVbe, int mode);
void VBEFreeModeInfo(VbeModeInfoBlock *block);

/*
 * INT2
 */

#define CRTC_DBLSCAN	(1<<0)
#define CRTC_INTERLACE	(1<<1)
#define CRTC_NHSYNC	(1<<2)
#define CRTC_NVSYNC	(1<<3)

struct _VbeCRTCInfoBlock {
    CARD16 HorizontalTotal;	/* Horizontal total in pixels */
    CARD16 HorizontalSyncStart;	/* Horizontal sync start in pixels */
    CARD16 HorizontalSyncEnd;	/* Horizontal sync end in pixels */
    CARD16 VerticalTotal;	/* Vertical total in lines */
    CARD16 VerticalSyncStart;	/* Vertical sync start in lines */
    CARD16 VerticalSyncEnd;	/* Vertical sync end in lines */
    CARD32 PixelClock;		/* Pixel clock in units of Hz */
    CARD16 RefreshRate;		/* Refresh rate in units of 0.01 Hz */
    CARD8 Flags;		/* Flags (Interlaced, Double Scan etc) */
    CARD8 Reserved[40];		/* remainder of ModeInfoBlock */
};

Bool VBESetVBEMode(vbeInfoPtr pVbe, int mode, VbeCRTCInfoBlock *crtc);

/*
 * INT 3
 */

Bool VBEGetVBEMode(vbeInfoPtr pVbe, int *mode);

/*
 * INT 4
 */

/* Save/Restore Super VGA video state */
/* function values are (values stored in VESAPtr):
 *	0 := query & allocate amount of memory to save state
 *	1 := save state
 *	2 := restore state
 *
 *	function 0 called automatically if function 1 called without
 *	a previous call to function 0.
 */

typedef enum {
  MODE_QUERY,
  MODE_SAVE,
  MODE_RESTORE
} vbeSaveRestoreFunction;

Bool
VBESaveRestore(vbeInfoPtr pVbe, vbeSaveRestoreFunction fuction,
	       pointer *memory, int *size, int *real_mode_pages);

/*
 * INT 5
 */

Bool
VBEBankSwitch(vbeInfoPtr pVbe, unsigned int iBank, int window);

/*
 * INT 6
 */

typedef enum {
  SCANWID_SET,
  SCANWID_GET,
  SCANWID_SET_BYTES,
  SCANWID_GET_MAX
} vbeScanwidthCommand;

#define VBESetLogicalScanline(pVbe, width)	\
	VBESetGetLogicalScanlineLength(pVbe, SCANWID_SET, width, \
					NULL, NULL, NULL)
#define VBESetLogicalScanlineBytes(pVbe, width)	\
	VBESetGetLogicalScanlineLength(pVbe, SCANWID_SET_BYTES, width, \
					NULL, NULL, NULL)
#define VBEGetLogicalScanline(pVbe, pixels, bytes, max)	\
	VBESetGetLogicalScanlineLength(pVbe, SCANWID_GET, 0, \
					pixels, bytes, max)
#define VBEGetMaxLogicalScanline(pVbe, pixels, bytes, max)	\
	VBESetGetLogicalScanlineLength(pVbe, SCANWID_GET_MAX, 0, \
					pixels, bytes, max)
Bool VBESetGetLogicalScanlineLength(vbeInfoPtr pVbe,
				    vbeScanwidthCommand command, int width,
				     int *pixels, int *bytes, int *max);

/*
 * INT 7
 */

/* 16 bit code */
Bool VBESetDisplayStart(vbeInfoPtr pVbe, int x, int y, Bool wait_retrace);
Bool VBEGetDisplayStart(vbeInfoPtr pVbe, int *x, int *y);

/*
 * INT 8
 */

/* if bits is 0, then it is a GET */
int VBESetGetDACPaletteFormat(vbeInfoPtr pVbe, int bits);

/*
 * INT 9
 */

/*
 *  If getting a palette, the data argument is not used. It will return
 * the data.
 *  If setting a palette, it will return the pointer received on success,
 * NULL on failure.
 */
CARD32 *VBESetGetPaletteData(vbeInfoPtr pVbe, Bool set, int first, int num,
			     CARD32 *data, Bool secondary, Bool wait_retrace);
#define VBEFreePaletteData(data)	xfree(data)

/*
 * INT A
 */

typedef struct _VBEpmi {
    int seg_tbl;
    int tbl_off;
    int tbl_len;
} VBEpmi;

VBEpmi *VBEGetVBEpmi(vbeInfoPtr pVbe);
#define VESAFreeVBEpmi(pmi)	xfree(pmi)

/* high level helper functions */

int VBEGetPixelClock(vbeInfoPtr pVbe, int mode, int Clock);
Bool VBEDPMSSet(vbeInfoPtr pVbe, int mode);

#endif
