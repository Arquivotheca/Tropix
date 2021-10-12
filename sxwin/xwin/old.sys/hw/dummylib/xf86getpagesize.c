/* $XFree86: xc/programs/Xserver/hw/xfree86/dummylib/xf86getpagesize.c,v 1.2 2005/10/14 15:16:50 tsi Exp $ */

#include <X11/X.h>
#include "os.h"
#include "xf86.h"
#include "xf86Priv.h"

/*
 * Utility functions required by libxf86_os. 
 */

int xf86getpagesize(void);

int
xf86getpagesize(void)
{
    return 4096;	/* not used */
}

