/*
Copyright (C) 1994-1999 The XFree86 Project, Inc.  All Rights Reserved.
Copyright (C) 2000 Silicon Motion, Inc.  All Rights Reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FIT-
NESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
XFREE86 PROJECT BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the names of the XFree86 Project and
Silicon Motion shall not be used in advertising or otherwise to promote the
sale, use or other dealings in this Software without prior written
authorization from the XFree86 Project and SIlicon Motion.
*/
/* $XFree86: xc/programs/Xserver/hw/xfree86/drivers/siliconmotion/regsmi.h,v 1.5 2006/03/09 17:37:07 tsi Exp $ */

#ifndef _REGSMI_H
#define _REGSMI_H

#define SMI_LYNX_SERIES(chip)	((chip & 0xF0F0) == 0x0010)
#define SMI_LYNX3D_SERIES(chip)	((chip & 0xF0F0) == 0x0020)
#define SMI_COUGAR_SERIES(chip)	((chip & 0xF0F0) == 0x0030)
#define SMI_LYNXEM_SERIES(chip) ((chip & 0xFFF0) == 0x0710)
#define SMI_LYNXM_SERIES(chip)	((chip & 0xFF00) == 0x0700)
#define SMI_MSOC_SERIES(chip)	((chip & 0xFF00) == 0x0500)

/* Chip tags */
#define PCI_SMI_VENDOR_ID	PCI_VENDOR_SMI
#define SMI_UNKNOWN		0
#define SMI_LYNX		PCI_CHIP_SMI910
#define SMI_LYNXE		PCI_CHIP_SMI810
#define SMI_LYNX3D		PCI_CHIP_SMI820
#define SMI_LYNXEM		PCI_CHIP_SMI710
#define SMI_LYNXEMplus		PCI_CHIP_SMI712
#define SMI_LYNX3DM		PCI_CHIP_SMI720
#define SMI_COUGAR3DR		PCI_CHIP_SMI731
#define SMI_MSOC		PCI_CHIP_SMI501

/* I/O Functions */
static __inline__ CARD8
VGAIN8_INDEX(SMIPtr pSmi, int indexPort, int dataPort, CARD8 index)
{
	if (pSmi->IOBase)
	{
		MMIO_OUT8(pSmi->IOBase, indexPort, index);
		return(MMIO_IN8(pSmi->IOBase, dataPort));
	}
	else
	{
		outb(pSmi->PIOBase + indexPort, index);
		return(inb(pSmi->PIOBase + dataPort));
	}
}

static __inline__ void
VGAOUT8_INDEX(SMIPtr pSmi, int indexPort, int dataPort, CARD8 index, CARD8 data)
{
	if (pSmi->IOBase)
	{
		MMIO_OUT8(pSmi->IOBase, indexPort, index);
		MMIO_OUT8(pSmi->IOBase, dataPort, data);
	}
	else
	{
		outb(pSmi->PIOBase + indexPort, index);
		outb(pSmi->PIOBase + dataPort, data);
	}
}

static __inline__ CARD8
VGAIN8(SMIPtr pSmi, int port)
{
	if (pSmi->IOBase)
	{
		return(MMIO_IN8(pSmi->IOBase, port));
	}
	else
	{
		return(inb(pSmi->PIOBase + port));
	}
}

static __inline__ void
VGAOUT8(SMIPtr pSmi, int port, CARD8 data)
{
	if (pSmi->IOBase)
	{
		MMIO_OUT8(pSmi->IOBase, port, data);
	}
	else
	{
		outb(pSmi->PIOBase + port, data);
	}
}

#define OUT_SEQ(pSmi, index, data)		\
	VGAOUT8_INDEX((pSmi), VGA_SEQ_INDEX, VGA_SEQ_DATA, (index), (data))
#define IN_SEQ(pSmi, index)			\
	VGAIN8_INDEX((pSmi), VGA_SEQ_INDEX, VGA_SEQ_DATA, (index))

#define WRITE_DPR(pSmi, dpr, data)		\
	MMIO_OUT32(pSmi->DPRBase, dpr, data);	\
	DEBUG((VERBLEV, "DPR%02X = %08X\n", dpr, data))
#define READ_DPR(pSmi, dpr)			MMIO_IN32(pSmi->DPRBase, dpr)

#define WRITE_VPR(pSmi, vpr, data)		\
	MMIO_OUT32(pSmi->VPRBase, vpr, data);	\
	DEBUG((VERBLEV, "VPR%02X = %08X\n", vpr, data))
#define READ_VPR(pSmi, vpr)			MMIO_IN32(pSmi->VPRBase, vpr)

#define WRITE_CPR(pSmi, cpr, data)		\
	MMIO_OUT32(pSmi->CPRBase, cpr, data);	\
	DEBUG((VERBLEV, "CPR%02X = %08X\n", cpr, data))
#define READ_CPR(pSmi, cpr)			MMIO_IN32(pSmi->CPRBase, cpr)

#define WRITE_FPR(pSmi, fpr, data)		\
	MMIO_OUT32(pSmi->FPRBase, fpr, data);	\
	DEBUG((VERBLEV, "FPR%02X = %08X\n", fpr, data))
#define READ_FPR(pSmi, fpr)			MMIO_IN32(pSmi->FPRBase, fpr)

#define WRITE_DCR(pSmi, dcr, data)		\
	MMIO_OUT32(pSmi->DCRBase, dcr, data);	\
	DEBUG((VERBLEV, "DCR%02X = %08X\n", dcr, data))
#define READ_DCR(pSmi, dcr)			MMIO_IN32(pSmi->DCRBase, dcr)

#define WRITE_SCR(pSmi, scr, data)		\
	MMIO_OUT32(pSmi->SCRBase, scr, data);	\
	DEBUG((VERBLEV, "SCR%02X = %08X\n", scr, data))
#define READ_SCR(pSmi, scr)			MMIO_IN32(pSmi->SCRBase, scr)

/* 2D Engine commands */
#define SMI_TRANSPARENT_SRC	0x00000100
#define SMI_TRANSPARENT_DEST	0x00000300

#define SMI_OPAQUE_PXL		0x00000000
#define SMI_TRANSPARENT_PXL	0x00000400

#define SMI_MONO_PACK_8		0x00001000
#define SMI_MONO_PACK_16	0x00002000
#define SMI_MONO_PACK_32	0x00003000

#define SMI_ROP2_SRC		0x00008000
#define SMI_ROP2_PAT		0x0000C000
#define SMI_ROP3		0x00000000

#define SMI_BITBLT		0x00000000
#define SMI_RECT_FILL		0x00010000
#define SMI_TRAPEZOID_FILL	0x00030000
#define SMI_SHORT_STROKE	0x00060000
#define SMI_BRESENHAM_LINE	0x00070000
#define SMI_HOSTBLT_WRITE	0x00080000
#define SMI_HOSTBLT_READ	0x00090000
#define SMI_ROTATE_BLT		0x000B0000

#define SMI_SRC_COLOR		0x00000000
#define SMI_SRC_MONOCHROME	0x00400000

#define SMI_GRAPHICS_STRETCH	0x00800000

#define SMI_ROTATE_CW		0x01000000
#define SMI_ROTATE_CCW		0x02000000

#define SMI_MAJOR_X		0x00000000
#define SMI_MAJOR_Y		0x04000000

#define SMI_LEFT_TO_RIGHT	0x00000000
#define SMI_RIGHT_TO_LEFT	0x08000000

#define SMI_COLOR_PATTERN	0x40000000
#define SMI_MONO_PATTERN	0x00000000

#define SMI_QUICK_START		0x10000000
#define SMI_START_ENGINE	0x80000000

#define MAXLOOP 0x100000	/* timeout value for engine waits */

#define ENGINE_IDLE()					\
	((pSmi->Chipset != SMI_MSOC) ?			\
	     ((VGAIN8_INDEX(pSmi, VGA_SEQ_INDEX, VGA_SEQ_DATA, 0x16) & 0x08) == 0) : \
	     ((READ_SCR(pSmi, SCR00) & 0x00080000) == 0))

#define FIFO_EMPTY()					\
	((pSmi->Chipset != SMI_MSOC) ?			\
	    ((VGAIN8_INDEX(pSmi, VGA_SEQ_INDEX, VGA_SEQ_DATA, 0x16) & 0x10) != 0) : \
	    ((READ_SCR(pSmi, SCR00) & 0x00100000) != 0))

/* Wait until "v" queue entries are free */
#define	WaitQueue(v)							   \
	do								   \
	{								   \
		if (pSmi->NoPCIRetry)					   \
		{							   \
			int loop = MAXLOOP; mem_barrier();		   \
			while (!FIFO_EMPTY())				   \
				if (loop-- == 0) break;			   \
			if (loop <= 0)					   \
				SMI_GEReset(pScrn, 1, __LINE__, __FILE__); \
		}							   \
	} while (0)

/* Wait until GP is idle */
#define WaitIdle()							   \
	do								   \
	{								   \
		int loop = MAXLOOP; mem_barrier();			   \
		while (!ENGINE_IDLE())					   \
			if (loop-- == 0) break;				   \
		if (loop <= 0)						   \
			SMI_GEReset(pScrn, 1, __LINE__, __FILE__);	   \
	}								   \
	while (0)

/* Wait until GP is idle and queue is empty */
#define	WaitIdleEmpty()							   \
	do								   \
	{								   \
		WaitQueue(MAXFIFO);					   \
		WaitIdle();						   \
	}								   \
	while (0)

#define RGB8_PSEUDO		(-1)
#define RGB16_565		0
#define RGB16_555		1
#define RGB32_888		2

/* register defines so we're not hardcoding numbers */

#define FPR00			0x0000

/* video window formats - I=indexed, P=packed */
#define FPR00_FMT_8I		0x0
#define FPR00_FMT_15P		0x1
#define FPR00_FMT_16P		0x2
#define FPR00_FMT_32P		0x3
#define FPR00_FMT_24P		0x4
#define FPR00_FMT_8P		0x5
#define FPR00_FMT_YUV422	0x6
#define FPR00_FMT_YUV420	0x7

/* possible bit definitions for FPR00 - VWI = Video Window 1 */
#define FPR00_VWIENABLE		0x00000008
#define FPR00_VWITILE		0x00000010
#define FPR00_VWIFILTER2	0x00000020
#define FPR00_VWIFILTER4	0x00000040
#define FPR00_VWIKEYENABLE	0x00000080
#define FPR00_VWIGDF_SHIFT	16
#define FPR00_VWIGDENABLE	0x00080000
#define FPR00_VWIGDTILE		0x00100000

#define FPR00_MASKBITS		0x0000FFFF

#define FPR04			0x0004
#define FPR08			0x0008
#define FPR0C			0x000C
#define FPR10			0x0010
#define FPR14			0x0014
#define FPR18			0x0018
#define FPR1C			0x001C
#define FPR20			0x0020
#define FPR24			0x0024
#define FPR58			0x0058
#define FPR5C			0x005C
#define FPR68			0x0068
#define FPRB0			0x00B0
#define FPRB4			0x00B4
#define FPRC4			0x00C4
#define FPRCC			0x00CC

#define FPR158                  0x0158
#define FPR158_MASK_MAXBITS     0x07FF
#define FPR158_MASK_BOUNDARY    0x0800
#define FPR15C                  0x015C
#define FPR15C_MASK_HWCCOLORS   0x0000FFFF
#define FPR15C_MASK_HWCADDREN   0xFFFF0000
#define FPR15C_MASK_HWCENABLE   0x80000000

/* SM501 System Configuration Registers */
#define SCR00			0x0000
#define SCR04			0x0004
#define SCR08			0x0008
#define SCR0C			0x000C
#define SCR10			0x0010
#define   SCR10_LOCAL_MEM_SIZE        0x0000E000
#define   SCR10_LOCAL_MEM_SIZE_SHIFT  13
#define SCR14			0x0014
#define SCR18			0x0018
#define SCR1C			0x001C
#define SCR20			0x0020
#define SCR24			0x0024
#define SCR28			0x0028
#define SCR2C			0x002C
#define SCR30			0x0030
#define SCR34			0x0034
#define SCR38			0x0038
#define SCR3C			0x003C
#define SCR40			0x0040
#define SCR44			0x0044
#define SCR48			0x0048
#define SCR4C			0x004C
#define SCR50			0x0050
#define SCR54			0x0054
#define SCR58			0x0058
#define SCR5C			0x005C
#define SCR60			0x0060
#define SCR64			0x0064
#define SCR68			0x0068
#define SCR6C			0x006C

/* SM501 Panel Graphics Control */
#define DCR00			0x0000
#define DCR04			0x0004
#define DCR08			0x0008
#define DCR0C			0x000C
#define DCR10			0x0010
#define DCR14			0x0014
#define DCR18			0x0018
#define DCR1C			0x001C
#define DCR20			0x0020
#define DCR24			0x0024
#define DCR28			0x0028
#define DCR2c			0x002c
#define DCR30			0x0030
#define DCR34			0x0034

/* SM 501 Video Control */
#define DCR40			0x0040
#define DCR44			0x0044
#define DCR48			0x0048
#define DCR4C			0x004C
#define DCR50			0x0050
#define DCR54			0x0054
#define DCR58			0x0058
#define DCR5C			0x005C
#define DCR60			0x0060
#define DCR64			0x0064
#define DCR68			0x0068

/* SM501 Video Alpha Control */
#define DCR80			0x0080
#define DCR84			0x0084
#define DCR88			0x0088
#define DCR8C			0x008C
#define DCR90			0x0090
#define DCR94			0x0094
#define DCR98			0x0098
#define DCR9C			0x009C
#define DCRA0			0x00A0
#define DCRA4			0x00A4

/* SM501 Panel Cursor Control */
#define DCRF0			0x00F0
#define DCRF4			0x00F4
#define DCRF8			0x00F8
#define DCRFC			0x00FC

/* SM 501 Alpha Control */
#define DCR100			0x0100
#define DCR104			0x0104
#define DCR108			0x0108
#define DCR10C			0x010C
#define DCR110			0x0110
#define DCR114			0x0114
#define DCR118			0x0118

/* SM 501 CRT Graphics Control */
#define DCR200			0x0200
#define   DCR200_CRT_BLANK		0x00000400
#define   DCR200_CRT_GRAPHICS_PLN_FMT	0x00000003
#define     CRT_GRAPHICS_PLN_FMT_8		0x00
#define     CRT_GRAPHICS_PLN_FMT_16		0x01
#define     CRT_GRAPHICS_PLN_FMT_32		0x10
#define DCR204			0x0204
#define DCR208			0x0208
#define DCR20C			0x020C
#define DCR210			0x0210
#define DCR214			0x0214
#define DCR218			0x0218
#define DCR21C			0x021C
#define DCR220			0x0220
#define DCR224			0x0224

/* SM 501 CRT Cursor Control */
#define DCR230			0x0230
#define DCR234			0x0234
#define DCR238			0x0238
#define DCR23C			0x023C

/* SM 501 Palette Ram */
#define DCR400			0x0400      /* Panel */
#define DCR800			0x0800      /* Video */
#define DCRC00			0x0C00      /* CRT   */

/* HWCursor definitons for Panel AND CRT */
#define SMI501_MASK_HWCENABLE	0x80000000
#define SMI501_MASK_MAXBITS	0x000007FF
#define SMI501_MASK_BOUNDARY	0x00000800
#define SMI501_HWCFBADDR_MASK	0x0CFFFFFF

/* panel sizes returned by the bios */

#define PANEL_640x480	0x00
#define PANEL_800x600	0x01
#define PANEL_1024x768	0x02
#define PANEL_1280x1024 0x03
#define PANEL_1600x1200 0x04
#define PANEL_1400x1050 0x0A

#endif /* _REGSMI_H */
