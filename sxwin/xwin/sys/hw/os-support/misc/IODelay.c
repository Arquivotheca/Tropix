/* $XFree86: xc/programs/Xserver/hw/xfree86/os-support/misc/IODelay.c,v 1.5 2006/01/09 15:00:22 dawes Exp $ */
 
/*******************************************************************************
  Stub for Alpha Linux
*******************************************************************************/

#include <X11/X.h>
#include "xf86.h"
#include "xf86Priv.h"
#include "xf86_OSlib.h"

/* 
 *   All we really need is a delay of about 40ns for I/O recovery for just
 *   about any occasion, but we'll be more conservative here:  On a
 *   100-MHz CPU, produce at least a delay of 1,000ns.
 */ 
void
xf86IODelay()
{
	xf86UDelay(1);
}

