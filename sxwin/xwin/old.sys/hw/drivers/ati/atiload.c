/* $XFree86: xc/programs/Xserver/hw/xfree86/drivers/ati/atiload.c,v 1.23 2006/03/16 16:49:58 dawes Exp $ */
/*
 * Copyright 2000 through 2006 by Marc Aurele La France (TSI @ UQV), tsi@xfree86.org
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

#include "ati.h"
#include "aticursor.h"
#include "atiload.h"
#include "atistruct.h"

#include "vbe.h"

/*
 * All symbol lists belong here.  They are externalised so that they can be
 * referenced elsewhere.  Note the naming convention for these things...
 */

const char *ATIint10Symbols[] =
{
    "xf86FreeInt10",
    "xf86InitInt10",
    "xf86int10Addr",
    NULL
};

const char *ATIddcSymbols[] =
{
    "xf86PrintEDID",
    "xf86SetDDCProperties",
    NULL
};

const char *ATIvbeSymbols[] =
{
    "VBEInit",
    "vbeDoEDID",
    "vbeFree",
    NULL
};

const char *ATIxf1bppSymbols[] =
{
    "xf1bppScreenInit",
    NULL
};

const char *ATIxf4bppSymbols[] =
{
    "xf4bppScreenInit",
    NULL
};

const char *ATIfbSymbols[] =
{
    "fbPictureInit",
    "fbPictureSetSubpixelOrder",
    "fbScreenInit",
    NULL
};

const char *ATIshadowfbSymbols[] =
{
    "ShadowFBInit",
    NULL
};

const char *ATIxaaSymbols[] =
{
    "XAACreateInfoRec",
    "XAADestroyInfoRec",
    "XAAInit",
    NULL
};

const char *ATIramdacSymbols[] =
{
    "xf86CreateCursorInfoRec",
    "xf86DestroyCursorInfoRec",
    "xf86InitCursor",
    NULL
};

const char *ATIi2cSymbols[] =
{
    "xf86CreateI2CBusRec",
    "xf86DestroyI2CBusRec",
    "xf86I2CBusInit",
    "xf86I2CDevInit",
    "xf86I2CFindDev",
    "xf86I2CGetScreenBuses",
    NULL
};

/*
 * ATILoadSubModule --
 *
 * Load a specific module and register with the loader those of its entry
 * points that are referenced by this driver.
 */
ModuleDescPtr
ATILoadSubModule
(
    ScrnInfoPtr  pScreenInfo,
    const char  *Module,
    const char **SymbolList
)
{
    ModuleDescPtr pModule = xf86LoadSubModule(pScreenInfo, Module);

    if (pModule)
        xf86LoaderModReqSymLists(pModule, SymbolList, NULL);

    return pModule;
}

/*
 * ATILoadVBEModule --
 *
 * A variant of ATILoadSubModule() specifically for loading the 'vbe' module.
 */
ModuleDescPtr
ATILoadVBEModule
(
    ScrnInfoPtr  pScreenInfo
)
{
    ModuleDescPtr pModule = xf86LoadVBEModule(pScreenInfo);

    if (pModule)
        xf86LoaderModReqSymLists(pModule, ATIvbeSymbols, NULL);

    return pModule;
}

/*
 * ATILoadSubModules --
 *
 * This function loads other modules required for a screen.
 */
ModuleDescPtr
ATILoadSubModules
(
    ScrnInfoPtr pScreenInfo,
    ATIPtr      pATI
)
{
    /* Load shadow frame buffer code if needed */
    if (pATI->OptionShadowFB &&
        !ATILoadSubModule(pScreenInfo, "shadowfb", ATIshadowfbSymbols))
        return NULL;

    /* Load XAA if needed */
    if (pATI->OptionAccel &&
        !ATILoadSubModule(pScreenInfo, "xaa", ATIxaaSymbols))
        return NULL;

    /* Load ramdac module if needed */
    if ((pATI->Cursor > ATI_CURSOR_SOFTWARE) &&
        !ATILoadSubModule(pScreenInfo, "ramdac", ATIramdacSymbols))
        return NULL;

    /* Load depth-specific entry points */
    switch (pATI->bitsPerPixel)
    {
        case 1:
            return ATILoadSubModule(pScreenInfo, "xf1bpp", ATIxf1bppSymbols);

        case 4:
            return ATILoadSubModule(pScreenInfo, "xf4bpp", ATIxf4bppSymbols);

        case 8:
        case 16:
        case 24:
        case 32:
            return ATILoadSubModule(pScreenInfo, "fb", ATIfbSymbols);

        default:
            return NULL;
    }
}

#endif /* XFree86LOADER */
