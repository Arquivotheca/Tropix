/*
 * $XFree86: xc/programs/Xserver/miext/layer/module/laymodule.c,v 1.1 2007/05/12 00:30:18 tsi Exp $
 *
 * Copyright � 2000 Keith Packard
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Keith Packard not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Keith Packard makes no
 * representations about the suitability of this software for any purpose.  It
 * is provided "as is" without express or implied warranty.
 *
 * KEITH PACKARD DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL KEITH PACKARD BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

#ifdef XFree86LOADER

#include "xf86Module.h"
#include <X11/X.h>
#include "scrnintstr.h"
#include "windowstr.h"
#include <X11/fonts/font.h>
#include "dixfontstr.h"
#include <X11/fonts/fontstruct.h>
#include "mi.h"
#include "regionstr.h"
#include "globals.h"
#include "gcstruct.h"
#include "layer.h"

static XF86ModuleVersionInfo VersRec =
{
	"layer",
	MODULEVENDORSTRING,
	MODINFOSTRING1,
	MODINFOSTRING2,
	XF86_VERSION_CURRENT,
	1, 0, 0,
	ABI_CLASS_ANSIC,		/* Only need the ansic layer */
	ABI_ANSIC_VERSION,
	MOD_CLASS_NONE,
	{0,0,0,0}       /* signature, to be patched into the file by a tool */
};

XF86ModuleData layerModuleData = { &VersRec, NULL, NULL };

#endif
