/* $XFree86: xc/programs/Xserver/hw/xfree86/os-support/bus/xf86Sbus.h,v 1.13 2007/03/15 14:38:36 tsi Exp $ */
/*
 * Platform specific SBUS and OpenPROM access declarations.
 *
 * Copyright (C) 2000 Jakub Jelinek (jakub@redhat.com)
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
 * JAKUB JELINEK BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef _XF86_SBUS_H
#define _XF86_SBUS_H

#ifdef XFree86Module
# error XFree86 module code must not #include "xf86Sbus.h"
#else

#if defined(linux)
# include <asm/types.h>
# include <asm/fbio.h>
# include <asm/openpromio.h>
#elif defined(SVR4)
# include <sys/fbio.h>
# include <sys/openpromio.h>
#elif defined(__OpenBSD__)
# include <machine/openpromio.h>
# include <dev/wscons/wsconsio.h>
  /* Compatibility #defines */
# define FBTYPE_SUN2BW		WSDISPLAY_TYPE_SUNBW
# define FBTYPE_SUN2COLOR	WSDISPLAY_TYPE_SUNCG2
# define FBTYPE_SUN3COLOR	WSDISPLAY_TYPE_SUNCG3
# define FBTYPE_SUN4COLOR	WSDISPLAY_TYPE_SUNCG4
# define FBTYPE_SUNFAST_COLOR	WSDISPLAY_TYPE_SUNCG6
# define FBTYPE_MEMCOLOR	WSDISPLAY_TYPE_SUNCG8
# define FBTYPE_SUNGP3		WSDISPLAY_TYPE_SUNCG12
# define FBTYPE_SUNGT		(-1)	/* Doesn't seem to exist */
# define FBTYPE_SUNLEO		WSDISPLAY_TYPE_SUNLEO
# define FBTYPE_MDICOLOR	WSDISPLAY_TYPE_SUNCG14
# define FBTYPE_TCXCOLOR	WSDISPLAY_TYPE_SUNTCX
# define FBTYPE_CREATOR		WSDISPLAY_TYPE_SUNFFB

# define fbcmap			wsdisplay_cmap

# define FBIOGETCMAP		WSDISPLAYIO_GETCMAP
# define FBIOPUTCMAP		WSDISPLAYIO_PUTCMAP

# define fbcursor		wsdisplay_cursor
# define set			which

# define FBIOSCURSOR		WSDISPLAYIO_SCURSOR
# define FBIOGCURSOR		WSDISPLAYIO_GCURSOR

# define FB_CUR_SETCUR		WSDISPLAY_CURSOR_DOCUR
# define FB_CUR_SETPOS		WSDISPLAY_CURSOR_DOPOS
# define FB_CUR_SETHOT		WSDISPLAY_CURSOR_DOHOT
# define FB_CUR_SETCMAP		WSDISPLAY_CURSOR_DOCMAP
# define FB_CUR_SETSHAPE	WSDISPLAY_CURSOR_DOSHAPE
# define FB_CUR_SETALL		WSDISPLAY_CURSOR_DOALL

# define FBIOSVIDEO		WSDISPLAYIO_SVIDEO
#elif defined(CSRG_BASED)
# if defined(__FreeBSD__)
#  include <sys/types.h>
#  include <sys/fbio.h>
#  include <dev/ofw/openpromio.h>
# elif defined(__NetBSD__)
#  include <dev/sun/fbio.h>
#  include <dev/ofw/openfirmio.h>
   /*
    * Translate from openpromio to openfirmio.  This could likely be avoided by
    * #include'ing <machine/openpromio.h> or <sparc/openpromio.h> instead.
    */
#  define opiocdesc		ofiocdesc
#  define op_nodeid		of_nodeid
#  define op_namelen		of_namelen
#  define op_name		of_name
#  define op_buflen		of_buflen
#  define op_buf		of_buf

#  define OPIOCGET		OFIOCGET
#  define OPIOCSET		OFIOCSET
#  define OPIOCNEXTPROP		OFIOCNEXTPROP
#  define OPIOCGETOPTNODE	OFIOCGETOPTNODE
#  define OPIOCGETNEXT		OFIOCGETNEXT
#  define OPIOCGETCHILD		OFIOCGETCHILD
#  define OPIOCFINDDEVICE	OFIOCFINDDEVICE
# else
#  include <machine/fbio.h>
# endif
#else
# include <sun/fbio.h>
#endif

/*
 * Some of these vary by OS (or are non-existent on some).  Also,
 * adapter-specific headers are not always installed/available, so don't
 * #include them.
 */

#ifndef   FBTYPE_SUN2BW
# define  FBTYPE_SUN2BW 2
#endif

#ifndef   FBTYPE_SUN2COLOR
# define  FBTYPE_SUN2COLOR 3
#endif

#ifndef   FBTYPE_SUN3COLOR
# define  FBTYPE_SUN3COLOR 6
#endif

#ifndef   FBTYPE_SUNFAST_COLOR
# define  FBTYPE_SUNFAST_COLOR 12
#endif

#ifndef   FBTYPE_SUNGP3
# define  FBTYPE_SUNGP3 17
#endif

#ifndef   FBTYPE_SUNGT
# define  FBTYPE_SUNGT 18
#endif

#ifndef   FBTYPE_SUNLEO
# define  FBTYPE_SUNLEO 19
#endif

#ifndef   FBTYPE_MDICOLOR
# if defined(CSRG_BASED) && !defined(__NetBSD__)
#  define FBTYPE_MDICOLOR 28
# else
#  define FBTYPE_MDICOLOR 20
# endif
#endif

#ifndef   FBTYPE_TCXCOLOR
# ifdef CSRG_BASED
#  define FBTYPE_TCXCOLOR 29
# else
#  define FBTYPE_TCXCOLOR 21
# endif
#endif

#ifndef   FBTYPE_CREATOR
# if defined(linux) || defined(__NetBSD__)
#  define FBTYPE_CREATOR 22
# elif defined(CSRG_BASED)
#  define FBTYPE_CREATOR 30
# elif defined(sun)
#  define FBTYPE_CREATOR 65535	/* Larger than life ... */
# endif
#endif

#ifndef   FBTYPE_P9100
# ifdef __NetBSD__
#  define  FBTYPE_P9100 21
# else
#  define  FBTYPE_P9100 -1	/* Not supported */
# endif
#endif

#endif /* XFree86Module */

#endif /* _XF86_SBUS_H */
