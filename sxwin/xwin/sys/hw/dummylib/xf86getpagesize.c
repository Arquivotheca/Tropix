/* $XFree86: xc/programs/Xserver/hw/xfree86/dummylib/xf86getpagesize.c,v 1.3 2007/02/24 15:51:26 tsi Exp $ */

#define DONT_DEFINE_WRAPPERS
#include "xf86_ansic.h"

#if defined(linux)
#define HAS_SC_PAGESIZE
#define HAS_GETPAGESIZE
#elif defined(CSRG_BASED)
#define HAS_GETPAGESIZE
#elif defined(DGUX)
#define HAS_GETPAGESIZE
#elif defined(sun) && !defined(SVR4)
#define HAS_GETPAGESIZE
#endif
#ifdef XNO_SYSCONF
#undef _SC_PAGESIZE
#endif

/*
 * Utility functions required by libxf86_os.
 */

int
xf86getpagesize(void)
{
    static int pagesize = -1;

    if (pagesize != -1)
        return pagesize;

#if defined(_SC_PAGESIZE) || defined(HAS_SC_PAGESIZE)
    pagesize = sysconf(_SC_PAGESIZE);
#endif
#ifdef _SC_PAGE_SIZE
    if (pagesize == -1)
        pagesize = sysconf(_SC_PAGE_SIZE);
#endif
#ifdef HAS_GETPAGESIZE
    if (pagesize == -1)
        pagesize = getpagesize();
#endif
#ifdef PAGE_SIZE
    if (pagesize == -1)
        pagesize = PAGE_SIZE;
#endif
    if (pagesize == -1)
        pagesize = 4096;

    return pagesize;
}

