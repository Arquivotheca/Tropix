/* $XFree86: xc/programs/Xserver/hw/xfree86/drivers/ati/atiendian.c,v 1.5 2006/01/05 18:55:29 tsi Exp $ */
/*
 * Copyright 2005 through 2006 by Marc Aurele La France (TSI @ UQV), tsi@xfree86.org
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

#include "atiendian.h"

/*
 * Definitions related to endianness.
 */
const ATIEndianType ATIEndian =
{
    {0x01U, 0x23U, 0x45U, 0x67U}
};

/*
 * ATIEndianCopy --
 *
 * This function applies a null endianness transformation on an integer.
 */
int
ATIEndianCopy
(
    const int value
)
{
    return value;
}

/*
 * ATIEndianSwap --
 *
 * This function applies a big-endian transformation on a 32-bit value.
 */
int
ATIEndianSwap32
(
    const int value
)
{
    union
    {
        unsigned int  word;
        unsigned char byte[4];
    } copy;

    copy.word = (unsigned int)value;
    return (int)((((((copy.byte[3] << 8) |
                     copy.byte[2]) << 8) |
                   copy.byte[1]) << 8) |
                 copy.byte[0]);
}

/*
 * ATIEndianSwap24 --
 *
 * This function applies a big-endian transformation on a 24-bit value.
 */
int
ATIEndianSwap24
(
    const int value
)
{
    return ((value & 0x000000FFU) << 16) | (value & 0x0000FF00U) |
           ((value & 0x00FF0000U) >> 16);
}
