/* $XFree86: xc/programs/Xserver/hw/xfree86/xaa/xaaBitOrder.c,v 1.9 2005/10/14 15:17:10 tsi Exp $ */

#include <X11/Xmd.h>
CARD32 XAAReverseBitOrder(CARD32 v);

CARD32
XAAReverseBitOrder(CARD32 v)
{
 return (((0x01010101 & v) << 7) | ((0x02020202 & v) << 5) | 
         ((0x04040404 & v) << 3) | ((0x08080808 & v) << 1) | 
         ((0x10101010 & v) >> 1) | ((0x20202020 & v) >> 3) | 
         ((0x40404040 & v) >> 5) | ((0x80808080 & v) >> 7));
}
