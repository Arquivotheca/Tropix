/* $XFree86: xc/programs/Xserver/hw/xfree86/drivers/pnozz/pnozz_cursor.c,v 1.1 2007/03/15 02:11:36 tsi Exp $ */
/*
 * SBus Weitek P9100 hardware cursor support
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
/* $NetBSD: pnozz_cursor.c,v 1.3 2005/11/12 23:32:12 macallan Exp $ */

#include "pnozz.h"

/*
 * All of this should probably be moved down to the common layer, or some
 * generic SBUS module.
 */

static void PnozzLoadCursorImage(ScrnInfoPtr pScrn, unsigned char *src);
static void PnozzSetCursorPosition(ScrnInfoPtr pScrn, int x, int y);
static void PnozzSetCursorColors(ScrnInfoPtr pScrn, int bg, int fg);

static void
PnozzLoadCursorImage(ScrnInfoPtr pScrn, unsigned char *src)
{
    PnozzPtr pPnozz = GET_PNOZZ_FROM_SCRN(pScrn);

    xf86SbusSetOsHwCursorImage(pPnozz->psdp, src, src + 0x200);
}

static void
PnozzShowCursor(ScrnInfoPtr pScrn)
{
    PnozzPtr pPnozz = GET_PNOZZ_FROM_SCRN(pScrn);

    if (pPnozz->enabled == 0) {
	pPnozz->enabled = 1;
	xf86SbusSetOsHwCursor(pPnozz->psdp, TRUE);
    }
}

void
PnozzHideCursor(ScrnInfoPtr pScrn)
{
    PnozzPtr pPnozz = GET_PNOZZ_FROM_SCRN(pScrn);

    if (pPnozz->enabled == 1) {
	pPnozz->enabled = 0;
	xf86SbusSetOsHwCursor(pPnozz->psdp, FALSE);
    }
}

static void
PnozzSetCursorPosition(ScrnInfoPtr pScrn, int x, int y)
{
    PnozzPtr pPnozz = GET_PNOZZ_FROM_SCRN(pScrn);

    x += 63;
    y += 63;

    if ((x != pPnozz->x) || (y != pPnozz->y)) {
	pPnozz->x = x;
	pPnozz->y = y;
	xf86SbusSetOsHwCursorPosition(pPnozz->psdp, x, y);
    }
}

static void
PnozzSetCursorColors(ScrnInfoPtr pScrn, int bg, int fg)
{
    PnozzPtr pPnozz = GET_PNOZZ_FROM_SCRN(pScrn);

    if ((bg != pPnozz->bg) || (fg != pPnozz->bg)) {
	pPnozz->bg = bg;
	pPnozz->fg = fg;
	xf86SbusSetOsHwCursorCmap(pPnozz->psdp, bg, fg);
    }
}

Bool
PnozzHWCursorInit(ScreenPtr pScreen)
{
    ScrnInfoPtr pScrn = xf86Screens[pScreen->myNum];
    PnozzPtr pPnozz;
    xf86CursorInfoPtr infoPtr;

    pPnozz = GET_PNOZZ_FROM_SCRN(pScrn);

    infoPtr = xf86CreateCursorInfoRec();
    if(!infoPtr) return FALSE;

    pPnozz->CursorInfoRec = infoPtr;

    infoPtr->MaxWidth = 64;
    infoPtr->MaxHeight = 64;

    xf86SbusSetOsHwCursorHotSpot(pPnozz->psdp, 63, 63);

    infoPtr->Flags = HARDWARE_CURSOR_AND_SOURCE_WITH_MASK |
	HARDWARE_CURSOR_TRUECOLOR_AT_8BPP /* |
	HARDWARE_CURSOR_BIT_ORDER_MSBFIRST |
	HARDWARE_CURSOR_NIBBLE_SWAPPED */ ;

    infoPtr->SetCursorColors = PnozzSetCursorColors;
    infoPtr->SetCursorPosition = PnozzSetCursorPosition;
    infoPtr->LoadCursorImage = PnozzLoadCursorImage;
    infoPtr->HideCursor = PnozzHideCursor;
    infoPtr->ShowCursor = PnozzShowCursor;
    infoPtr->UseHWCursor = NULL;

    return xf86InitCursor(pScreen, infoPtr);
}
