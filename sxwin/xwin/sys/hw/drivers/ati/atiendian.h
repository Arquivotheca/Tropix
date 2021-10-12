/* $XFree86: xc/programs/Xserver/hw/xfree86/drivers/ati/atiendian.h,v 1.5 2007/01/01 16:08:16 tsi Exp $ */
/*
 * Copyright 2005 through 2007 by Marc Aurele La France (TSI @ UQV), tsi@xfree86.org
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

#ifndef ___ATIENDIAN_H___
#define ___ATIENDIAN_H___ 1

#include "atiproto.h"

/*
 * Declarations related to endianness.
 */
typedef union
{
    const unsigned char byte[4];
    const unsigned int  endian;
} ATIEndianType;

extern const ATIEndianType ATIEndian;

#define ATI_LITTLE_ENDIAN 0x67452301U
#define ATI_BIG_ENDIAN    0x01234567U

typedef int ATIApplyEndianProc FunctionPrototype((const int));

extern ATIApplyEndianProc ATIEndianCopy;
extern ATIApplyEndianProc ATIEndianSwap32;
extern ATIApplyEndianProc ATIEndianSwap24;

#endif /* ___ATIENDIAN_H___ */
