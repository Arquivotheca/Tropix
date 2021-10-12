/* $XFree86: xc/programs/Xserver/mi/miinitext.c,v 3.79 2006/09/02 16:44:23 dawes Exp $ */
/***********************************************************

Copyright 1987, 1998  The Open Group

Permission to use, copy, modify, distribute, and sell this software and its
documentation for any purpose is hereby granted without fee, provided that
the above copyright notice appear in all copies and that both that
copyright notice and this permission notice appear in supporting
documentation.

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
OPEN GROUP BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of The Open Group shall not be
used in advertising or otherwise to promote the sale, use or other dealings
in this Software without prior written authorization from The Open Group.


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

#include "misc.h"
#include "extnsionst.h"
#include "micmap.h"

#ifdef PANORAMIX
extern Bool noPanoramiXExtension;
#endif
extern Bool noTestExtensions;
#ifdef XKB
extern Bool noXkbExtension;
#endif

#ifdef DMXSERVER
extern Bool dmxNoRender;
#endif

#ifdef XFree86LOADER
#include "loaderProcs.h"
#endif

#ifdef MITSHM
#define _XSHM_SERVER_
#include <X11/extensions/shmstr.h>
#endif
#ifdef XTEST
#define _XTEST_SERVER_
#include <X11/extensions/XTest.h>
#endif
#ifdef XKB
#include <X11/extensions/XKB.h>
#endif
#ifdef LBX
#define _XLBX_SERVER_
#include <X11/extensions/lbxstr.h>
#endif
#ifdef XPRINT
#undef  _XP_PRINT_SERVER_
#define _XP_PRINT_SERVER_
#include <X11/extensions/Print.h>
#endif
#ifdef XAPPGROUP
#define _XAG_SERVER_
#include <X11/extensions/Xagstr.h>
#endif
#ifdef XCSECURITY
#define _SECURITY_SERVER
#include <X11/extensions/securstr.h>
#endif
#ifdef PANORAMIX
#include <X11/extensions/panoramiXproto.h>
#endif
#ifdef XF86BIGFONT
#include <X11/extensions/xf86bigfstr.h>
#endif

#ifndef XFree86LOADER
#ifdef RES
#include <X11/extensions/XResproto.h>
#endif
#ifdef XRECORD
#include "recordproc.h"
#endif
#ifdef DBE
#define DBE_EXT_INIT_ONLY
#include "dbeproc.h"
#endif
#endif

/* FIXME: this whole block of externs should be from the appropriate headers */
#ifdef MITSHM
extern void ShmExtensionInit(INITARGS);
#endif
#ifdef XTESTEXT1
extern void XTestExtension1Init(INITARGS);
#endif
#ifdef PANORAMIX
extern void PanoramiXExtensionInit(INITARGS);
#endif
#ifdef XTEST
extern void XTestExtensionInit(INITARGS);
#endif
#ifdef XKB
extern void XkbExtensionInit(INITARGS);
#endif
#ifdef XINPUT
extern void XInputExtensionInit(INITARGS);
#endif
#ifdef LBX
extern void LbxExtensionInit(INITARGS);
#endif
#ifdef XAPPGROUP
extern void XagExtensionInit(INITARGS);
#endif
#ifdef XCSECURITY
extern void SecurityExtensionInit(INITARGS);
#endif
#ifdef XPRINT
extern void XpExtensionInit(INITARGS);
#endif
#ifdef XF86BIGFONT
extern void XFree86BigfontExtensionInit(INITARGS);
#endif
#ifdef RENDER
extern void RenderExtensionInit(INITARGS);
#endif
#ifdef RANDR
extern void RRExtensionInit(INITARGS);
#endif

#ifndef XFree86LOADER
#ifdef SHAPE
extern void ShapeExtensionInit(INITARGS);
#endif
#ifdef EVI
extern void EVIExtensionInit(INITARGS);
#endif
#ifdef MULTIBUFFER
extern void MultibufferExtensionInit(INITARGS);
#endif
#ifdef BIGREQS
extern void BigReqExtensionInit(INITARGS);
#endif
#ifdef MITMISC
extern void MITMiscExtensionInit(INITARGS);
#endif
#ifdef XTRAP
extern void DEC_XTRAPInit(INITARGS);
#endif
#ifdef SCREENSAVER
extern void ScreenSaverExtensionInit (INITARGS);
#endif
#ifdef XV
extern void XvExtensionInit(INITARGS);
#endif
#ifdef XVMC
extern void XvMCExtensionInit(INITARGS);
#endif
#ifdef XSYNC
extern void SyncExtensionInit(INITARGS);
#endif
#ifdef XCMISC
extern void XCMiscExtensionInit(INITARGS);
#endif
#ifdef XF86VIDMODE
extern void XFree86VidModeExtensionInit(INITARGS);
#endif
#ifdef XF86MISC
extern void XFree86MiscExtensionInit(INITARGS);
#endif
#ifdef XFreeXDGA
extern void XFree86DGAExtensionInit(INITARGS);
#endif
#ifdef GLXEXT
#ifndef __DARWIN__
extern void GlxExtensionInit(INITARGS);
extern void GlxWrapInitVisuals(miInitVisualsProcPtr *);
#else
extern void DarwinGlxExtensionInit(INITARGS);
extern void DarwinGlxWrapInitVisuals(miInitVisualsProcPtr *);
#endif
#endif
#ifdef XF86DRI
extern void XFree86DRIExtensionInit(INITARGS);
#endif
#ifdef TOGCUP
extern void XcupExtensionInit(INITARGS);
#endif
#ifdef DPMSExtension
extern void DPMSExtensionInit(INITARGS);
#endif
#ifdef DPSEXT
extern void DPSExtensionInit(INITARGS);
#endif
#ifdef FONTCACHE
extern void FontCacheExtensionInit(INITARGS);
#endif
#ifdef RES
extern void ResExtensionInit(INITARGS);
#endif
#ifdef DMXEXT
extern void DMXExtensionInit(INITARGS);
#endif

/*ARGSUSED*/
void
InitExtensions(int argc, const char *argv[])
{
#ifdef PANORAMIX
  if (!noPanoramiXExtension) PanoramiXExtensionInit();
#endif
#ifdef XTESTEXT1
    if (!noTestExtensions) XTestExtension1Init();
#endif
#ifdef SHAPE
    ShapeExtensionInit();
#endif
#ifdef MITSHM
    ShmExtensionInit();
#endif
#ifdef EVI
    EVIExtensionInit();
#endif
#ifdef MULTIBUFFER
    MultibufferExtensionInit();
#endif
#ifdef XINPUT
    XInputExtensionInit();
#endif
#ifdef XTEST
    if (!noTestExtensions) XTestExtensionInit();
#endif
#ifdef BIGREQS
    BigReqExtensionInit();
#endif
#ifdef MITMISC
    MITMiscExtensionInit();
#endif
#ifdef XTRAP
    if (!noTestExtensions) DEC_XTRAPInit();
#endif
#ifdef SCREENSAVER
    ScreenSaverExtensionInit ();
#endif
#ifdef XV
    XvExtensionInit();
#endif
#ifdef XVMC
    XvMCExtensionInit();
#endif
#ifdef XSYNC
    SyncExtensionInit();
#endif
#ifdef XKB
    if (!noXkbExtension) XkbExtensionInit();
#endif
#ifdef XCMISC
    XCMiscExtensionInit();
#endif
#ifdef XRECORD
    if (!noTestExtensions) RecordExtensionInit(); 
#endif
#ifdef LBX
    LbxExtensionInit();
#endif
#ifdef DBE
    DbeExtensionInit();
#endif
#ifdef XAPPGROUP
    XagExtensionInit();
#endif
#ifdef XCSECURITY
    SecurityExtensionInit();
#endif
#ifdef XPRINT
    XpExtensionInit();
#endif
#ifdef TOGCUP
    XcupExtensionInit();
#endif
#ifdef DPMSExtension
    DPMSExtensionInit();
#endif
#ifdef FONTCACHE
    FontCacheExtensionInit();
#endif
#ifdef XF86BIGFONT
    XFree86BigfontExtensionInit();
#endif
#ifdef XF86VIDMODE
    XFree86VidModeExtensionInit();
#endif
#ifdef XF86MISC
    XFree86MiscExtensionInit();
#endif
#ifdef XFreeXDGA
    XFree86DGAExtensionInit();
#endif
#ifdef XF86DRI
    XFree86DRIExtensionInit();
#endif
#ifdef GLXEXT
#ifndef __DARWIN__
    GlxExtensionInit();
#else
    DarwinGlxExtensionInit();
#endif
#endif
#ifdef DPSEXT
    DPSExtensionInit();
#endif
#ifdef RENDER
#ifdef DMXSERVER
    if (!dmxNoRender)
#endif
    RenderExtensionInit();
#endif
#ifdef RANDR
    RRExtensionInit();
#endif
#ifdef RES
    ResExtensionInit();
#endif
#ifdef DMXEXT
    DMXExtensionInit();
#endif
}

void
InitVisualWrap()
{
    miResetInitVisuals();
#ifdef GLXEXT
#ifndef __DARWIN__
    GlxWrapInitVisuals(&miInitVisualsProc);
#else
    DarwinGlxWrapInitVisuals(&miInitVisualsProc);
#endif
#endif
}

#else /* XFree86LOADER */

/* List of built-in (statically linked) extensions */
static ExtensionModule staticExtensions[] = {
#ifdef XTESTEXT1
    {
	XTestExtension1Init,
	"XTEST1",
	&noTestExtensions,
	NULL,
	NULL
    },
#endif
#ifdef MITSHM
    {
	ShmExtensionInit,
	SHMNAME,
	NULL,
	NULL,
	NULL
    },
#endif
#ifdef XINPUT
    {
	XInputExtensionInit,
	"XInputExtension",
	NULL,
	NULL,
	NULL
    },
#endif
#ifdef XTEST
    {
	XTestExtensionInit,
	XTestExtensionName,
	&noTestExtensions,
	NULL,
	NULL
    },
#endif
#ifdef XKB
    {
	XkbExtensionInit,
	XkbName,
	&noXkbExtension,
	NULL,
	NULL
    },
#endif
#ifdef LBX
    {
	LbxExtensionInit,
	LBXNAME,
	NULL,
	NULL,
	NULL
    },
#endif
#ifdef XAPPGROUP
    {
	XagExtensionInit,
	XAGNAME,
	NULL,
	NULL,
	NULL
    },
#endif
#ifdef XCSECURITY
    {
	SecurityExtensionInit,
	SECURITY_EXTENSION_NAME,
	NULL,
	NULL,
	NULL
    },
#endif
#ifdef XPRINT
    {
	XpExtensionInit,
	XP_PRINTNAME,
	NULL,
	NULL,
	NULL
    },
#endif
#ifdef PANORAMIX
    {
	PanoramiXExtensionInit,
	PANORAMIX_PROTOCOL_NAME,
	&noPanoramiXExtension,
	NULL,
	NULL
    },
#endif
#ifdef XF86BIGFONT
    {
	XFree86BigfontExtensionInit,
	XF86BIGFONTNAME,
	NULL,
	NULL,
	NULL
    },
#endif
#ifdef RENDER
    {
	RenderExtensionInit,
	"RENDER",
	NULL,
	NULL,
	NULL
    },
#endif
#ifdef RANDR
    {
	RRExtensionInit,
	"RANDR",
	NULL,
	NULL,
	NULL
    },
#endif
    {
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
    }
};
    
/*ARGSUSED*/
void
InitExtensions(int argc, const char *argv[])
{
    int i;
    ExtensionModule *ext;
    static Bool listInitialised = FALSE;

    if (!listInitialised) {
	/* Add built-in extensions to the list. */
	for (i = 0; staticExtensions[i].name; i++)
	    LoadExtension(&staticExtensions[i], TRUE);

	/* Sort the extensions according the init dependencies. */
	LoaderSortExtensions();
	listInitialised = TRUE;
    }

    for (i = 0; ExtensionModuleList[i].name != NULL; i++) {
	ext = &ExtensionModuleList[i];
	if (ext->initFunc != NULL && 
	    (ext->disablePtr == NULL || 
	     (ext->disablePtr != NULL && !*ext->disablePtr))) {
	    (ext->initFunc)();
	}
    }
}

static void (*__miHookInitVisualsFunction)(miInitVisualsProcPtr *);

void
InitVisualWrap()
{
    miResetInitVisuals();
    if (__miHookInitVisualsFunction)
	(*__miHookInitVisualsFunction)(&miInitVisualsProc);
}

void miHookInitVisuals(void (**old)(miInitVisualsProcPtr *),
		       void (*new)(miInitVisualsProcPtr *))
{
    if (old)
	*old = __miHookInitVisualsFunction;
    __miHookInitVisualsFunction = new;
}

#endif /* XFree86LOADER */
