/*
 *
 * Copyright 1997,1998 by Metro Link, Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Metro Link, Inc. not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Metro Link, Inc. makes no
 * representations about the suitability of this software for any purpose.
 *  It is provided "as is" without express or implied warranty.
 *
 * METRO LINK, INC. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL METRO LINK, INC. BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

/* $XFree86: xc/programs/Xserver/hw/xfree86/loader/coffloader.h,v 1.6 2006/03/16 16:50:34 dawes Exp $ */

#ifndef _COFFLOADER_H
#define _COFFLOADER_H

typedef struct _coff_reloc *COFFRelocPtr;
typedef struct _coff_COMMON *COFFCommonPtr;

/* coffloader.c */
extern void *COFFLoadModule(loaderPtr, int, LOOKUP **);
extern void COFFResolveSymbols(LoaderDescPtr, int);
extern int COFFCheckForUnresolved(LoaderDescPtr);
extern char *COFFAddressToSection(void *, unsigned long);
extern void COFFUnloadModule(void *);
extern const char *COFFFindRelocName(LoaderDescPtr, int, unsigned long);
extern const char *COFFAddressToSymbol(void *, unsigned long, unsigned long *,
				       const char **, int);
extern void *COFFReadExecutableSyms(int);
#endif /* _COFFLOADER_H */
