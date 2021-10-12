/* $XFree86: xc/programs/Xserver/hw/xfree86/vbe/vbe_module.c,v 1.4 2006/03/16 16:50:35 dawes Exp $ */

#ifdef XFree86LOADER

#include "vbe.h"

extern const char *vbe_ddcSymbols[];
extern const char *vbe_int10Symbols[];

/*
 * Vbe.*InfoBlock structures were reworked starting with video driver ABI
 * version number 0.9.
 */
static XF86ModReqInfo ParentModuleRequirements =
{
    MAJOR_UNSPEC,		/* majorversion */
    MINOR_UNSPEC,		/* minorversion */
    PATCH_UNSPEC,		/* patchlevel */
    ABI_CLASS_VIDEODRV,		/* abiclass */
    SET_ABI_VERSION(0, 9),	/* abiversion */
    MOD_CLASS_NONE		/* moduleclass */
};

static pointer
vbeSetup(ModuleDescPtr module, pointer opts, int *errmaj, int *errmin)
{
    /*
     * Tell the loader about symbols from other modules that this module
     * might refer to.
     */
    LoaderModRefSymLists(module, vbe_ddcSymbols, vbe_int10Symbols, NULL);

    /* Cause parent module version to be checked */
    xf86SetParentModuleRequirements(module, &ParentModuleRequirements);

    /*
     * The return value must be non-NULL on success even though there
     * is no TearDownProc.
     */
    return (pointer)1;
}

static XF86ModuleVersionInfo vbeVersRec =
{
    "vbe",			/* modname */
    MODULEVENDORSTRING,		/* vendor */
    MODINFOSTRING1,		/* _modinfo1_ */
    MODINFOSTRING2,		/* _modinfo2_ */
    XF86_VERSION_CURRENT,	/* xf86version */
    2, 0, 0,			/* majorversion, minorversion, patchlevel */
    ABI_CLASS_VIDEODRV,		/* needs the video driver ABI */
    ABI_VIDEODRV_VERSION,	/* abiversion (current) */
    MOD_CLASS_NONE,		/* moduleclass */
    {0,0,0,0}			/* checksum */
};

XF86ModuleData vbeModuleData =
{
    &vbeVersRec,		/* vers */
    vbeSetup,			/* setup */
    NULL			/* teardown */
};

#endif /* XFree86LOADER */
