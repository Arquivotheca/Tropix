/* $XFree86: xc/programs/Xserver/hw/xfree86/drivers/ati/r128_misc.c,v 1.11 2007/01/01 16:08:18 tsi Exp $ */
/*
 * Copyright 2000 through 2007 by Marc Aurele La France (TSI @ UQV), tsi@xfree86.org
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that copyright
 * notice and this permission notice appear in supporting documentation, and
 * that the name of Marc Aurele La France not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  Marc Aurele La France makes no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as-is" without express or implied warranty.
 *
 * MARC AURELE LA FRANCE DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS.  IN NO
 * EVENT SHALL MARC AURELE LA FRANCE BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

#ifdef XFree86LOADER

#include "ativersion.h"

#include "r128_probe.h"
#include "r128_version.h"

#include "xf86.h"

/* Module loader interface for subsidiary driver module */

static XF86ModuleVersionInfo R128VersionRec =
{
    R128_DRIVER_NAME,
    MODULEVENDORSTRING,
    MODINFOSTRING1,
    MODINFOSTRING2,
    XF86_VERSION_CURRENT,
    R128_VERSION_MAJOR, R128_VERSION_MINOR, R128_VERSION_PATCH,
    ABI_CLASS_VIDEODRV,
    ABI_VIDEODRV_VERSION,
    MOD_CLASS_VIDEODRV,
    {0, 0, 0, 0}
};

pointer R128Module = NULL;

/*
 * R128Setup --
 *
 * This function is called every time the module is loaded.
 */
static pointer
R128Setup
(
    ModuleDescPtr Module,
    pointer Options,
    int     *ErrorMajor,
    int     *ErrorMinor
)
{
    static Bool Inited = FALSE;

    if (!Inited)
    {
        /* Ensure main driver module is loaded, but not as a submodule */
        if (!xf86ServerIsOnlyDetecting() && !LoaderSymbol(ATI_NAME))
            xf86LoadOneModule(ATI_DRIVER_NAME, Options);

        R128LoaderRefSymLists(Module);

        Inited = TRUE;
    }

    return (pointer)TRUE;
}

/* The following record must be called r128ModuleData */
XF86ModuleData r128ModuleData =
{
    &R128VersionRec,
    R128Setup,
    NULL
};

const char *r128ExportedSymbols[] = {
    "R128PreInit",
    "R128ScreenInit",
    "R128SwitchMode",
    "R128AdjustFrame",
    "R128EnterVT",
    "R128LeaveVT",
    "R128FreeScreen",
    "R128ValidMode",
    "R128Options",
    "R128Module",
    NULL
};

#endif /* XFree86LOADER */
