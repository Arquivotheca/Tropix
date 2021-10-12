/* $XFree86: xc/programs/Xserver/hw/xfree86/drivers/pnozz/pnozz.h,v 1.1 2007/03/15 02:11:36 tsi Exp $ */
/*
 * SBus Weitek P9100 driver - defines
 *
 * Copyright (C) 2005 Michael Lorenz
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * MICHAEL LORENZ BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
/* $NetBSD: pnozz.h,v 1.4 2006/02/27 18:19:53 macallan Exp $ */

#ifndef PNOZZ_H
#define PNOZZ_H

#include "xf86.h"
#include "xf86_OSproc.h"
#include "xf86_ansic.h"
#include "xf86RamDac.h"
#include <X11/Xmd.h>
#include "gcstruct.h"
#include "pnozz_regs.h"
#include "xf86sbusBus.h"
#include "xaa.h"

typedef struct {
	unsigned char	*fb;	/* 2MB framebuffer */
	unsigned char	*fbc;	/* registers, so we can just add a byte offset */
	int		vclipmax;
	int		width;
	int		height, scanlinesize, maxheight;
	int		depthshift;

	sbusDevicePtr	psdp;
	Bool		HWCursor;
	Bool		NoAccel;
	CloseScreenProcPtr CloseScreen;

	xf86CursorInfoPtr CursorInfoRec;
	int		fg, bg, enabled, x, y;

	OptionInfoPtr	Options;

	volatile CARD32	Scratch;
	XAAInfoRecPtr	pXAA;
	CARD32		MaxClip;
	unsigned char	*buffers[2];
	/*
	 * XXX this is enough for everything a SPARCbook could do on it's
	 * internal display but not necessarily for an external one
	 */
	CARD32		Buffer[6400];
	int		words, last_word;
	int		offset_mask;

	int		DidSave;
	unsigned int	SvSysConf;	/* System Configuration Register */
	unsigned int	CRTC[4];	/* CRTC values for horizontal timing */
	unsigned int	SvMemCtl;	/* memory control register */
	unsigned char	SvDAC_MCCR;	/* DAC Misc Clock Ctrl (0x02) */
	unsigned char	SvDAC_PF;	/* DAC Pixel Format (0x0a) */
	unsigned char	SvDAC_MC3;	/* DAC Misc Control 3 */
	unsigned char	SvVCO;		/* DAC System PLL VCO divider */
	unsigned char	SvPLL;		/* clock multiplier / divider */

} PnozzRec, *PnozzPtr;

#define GET_PNOZZ_FROM_SCRN(p)    ((p->driverPrivate))

void pnozz_write_4(PnozzPtr, int, unsigned int);
unsigned int pnozz_read_4(PnozzPtr, int);

int PnozzAccelInit(ScrnInfoPtr);
void PnozzHideCursor(ScrnInfoPtr);
extern Bool PnozzHWCursorInit(ScreenPtr pScreen);

#endif /* CG6_H */
