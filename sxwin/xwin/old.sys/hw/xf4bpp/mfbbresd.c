/* $XFree86: xc/programs/Xserver/hw/xfree86/xf4bpp/mfbbresd.c,v 1.5 2005/03/28 02:51:07 dawes Exp $ */
/***********************************************************

Copyright (c) 1987  X Consortium

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of the X Consortium shall not be
used in advertising or otherwise to promote the sale, use or other dealings
in this Software without prior written authorization from the X Consortium.


Copyright 1987 by Digital Equipment Corporation, Maynard, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the name of Digital not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/
/* GJA -- modified this file for vga16 */

#include "xf4bpp.h"
#include "OScompiler.h"
#include "mfbmap.h"
#include "mfb.h"
#include "maskbits.h"
#include "miline.h"
#include "wm3.h"
#include "xf86.h"

/* Dashed bresenham line */

#define NO_INK (-1) /* GJA -- means: dash is off */

#define StepDash\
    if (!--dashRemaining) { \
	if (++ dashIndex == numInDashList) \
	    dashIndex = 0; \
	dashRemaining = pDash[dashIndex]; \
	ink = fgink; \
	if (dashIndex & 1) \
	    ink = bgink; \
	if (isDoubleDash) \
	    WM3_SET_INK(ink); \
    }

void
xf4bppBresD(DrawablePtr pDrawable, int fgink, int bgink, int *pdashIndex,
	    unsigned char *pDash, int numInDashList, int *pdashOffset,
	    int isDoubleDash, PixelType *addrlbase, int nlwidth,
	    int signdx, int signdy, int axis, int x1, int y1, int e, int e1,
	    int e2, int len)
{
    IOADDRESS REGBASE =
	xf86Screens[pDrawable->pScreen->myNum]->domainIOBase + 0x300;
    int yinc;	/* increment to next scanline, in bytes */
    PixelType *addrl;
    int e3 = e2-e1;
    unsigned long bit;
    PixelType leftbit = mask[0]; /* leftmost bit to process in new word */
    PixelType rightbit = mask[PPW-1]; /* rightmost bit to process in new word */
    int dashIndex;
    int dashOffset;
    int dashRemaining;
    int	ink;

    fgink &= 0x0F; bgink &= 0x0F; /* GJA -- so they're != NO_INK */

    dashOffset = *pdashOffset;
    dashIndex = *pdashIndex;
    dashRemaining = pDash[dashIndex] - dashOffset;
    ink = fgink;
    if (!isDoubleDash)
	bgink = NO_INK;
    if (dashIndex & 1)
	ink = bgink;
    if ( ink != NO_INK ) WM3_SET_INK(ink);

    /* point to longword containing first point */
    addrl = mfbScanline(addrlbase, x1, y1, nlwidth);
    yinc = signdy * nlwidth;
    e = e-e1;			/* to make looping easier */
    bit = mask[x1 & PIM];
    if (axis == X_AXIS)
    {
	if (signdx > 0)
	{
	    while(len--)
	    { 
		if ( ink != NO_INK ) UPDRW(addrl,bit);
		e += e1;
		if (e >= 0)
		{
		    addrl += yinc;
		    e += e3;
		}
		bit = SCRRIGHT(bit,1);
		if (!bit) { bit = leftbit; addrl++; }
		StepDash
	    }
	}
	else
	{
	    while(len--)
	    { 
		
		if ( ink != NO_INK ) UPDRW(addrl,bit);
		e += e1;
		if (e >= 0)
		{
		    addrl += yinc;
		    e += e3;
		}
		bit = SCRLEFT(bit,1);
		if (!bit) { bit = rightbit; addrl--; }
		StepDash
	    }
	}
    } /* if X_AXIS */
    else
    {
	if (signdx > 0)
	{
	    while(len--)
	    {
		if ( ink != NO_INK ) UPDRW(addrl,bit);
		e += e1;
		if (e >= 0)
		{
		    bit = SCRRIGHT(bit,1);
		    if (!bit) { bit = leftbit; addrl++; }
		    e += e3;
		}
		addrl += yinc;
		StepDash
	    }
	}
	else
	{
	    while(len--)
	    {
		
		if ( ink != NO_INK ) UPDRW(addrl,bit);
		e += e1;
		if (e >= 0)
		{
		    bit = SCRLEFT(bit,1);
		    if (!bit) { bit = rightbit; addrl--; }
		    e += e3;
		}
		addrl += yinc;
		StepDash
	    }
	}
    } /* else Y_AXIS */
    *pdashIndex = dashIndex;
    *pdashOffset = pDash[dashIndex] - dashRemaining;
} 
