/* $XFree86: xc/programs/Xserver/hw/xfree86/os-support/misc/xf86_IlHack.c,v 3.6 2006/01/09 15:00:22 dawes Exp $ */
/*
 * This file is an incredible crock to get the normally-inline functions
 * built into the server so that things can be debugged properly.
 *
 * Note: this doesn't work when using a compiler other than GCC.
 */

#define static /**/
#define __inline__ /**/
#undef NO_INLINE
#define DO_PROTOTYPES
#include "compiler.h"
