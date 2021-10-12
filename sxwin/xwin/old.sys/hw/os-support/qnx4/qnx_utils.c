/* This includes various utility functions which are missing otherwise 
 * $XFree86: xc/programs/Xserver/hw/xfree86/os-support/qnx4/qnx_utils.c,v 1.2 2005/10/14 15:17:06 tsi Exp $
 */

#include <limits.h>
#include <string.h>
#include <i86.h>
#include <conio.h>
#include <time.h>

#include <X11/X.h>
#include <xf86.h>

void usleep (unsigned long interval) { delay( interval/1000 ); } 

/* We have our own version of GetTimeInMillis, because the normal
 * version calls gettimeofday(), which seems to thrash ES...
 */
CARD32
GetTimeInMillis()
{
struct timespec tp;

	qnx_getclock(0, CLOCK_REALTIME, &tp);
	return((tp.tv_sec * 1000) + (tp.tv_nsec / 1000000));
}
