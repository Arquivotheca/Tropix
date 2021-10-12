/* $XFree86: xc/programs/Xserver/hw/xfree86/loader/loader.h,v 1.34 2006/06/27 18:43:59 dawes Exp $ */

/*
 *
 * Copyright 1995-1998 by Metro Link, Inc.
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
/*
 * Copyright (c) 1997-2003 by The XFree86 Project, Inc.
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject
 * to the following conditions:
 *
 *   1.  Redistributions of source code must retain the above copyright
 *       notice, this list of conditions, and the following disclaimer.
 *
 *   2.  Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer
 *       in the documentation and/or other materials provided with the
 *       distribution, and in the same place and form as other copyright,
 *       license and disclaimer information.
 *
 *   3.  The end-user documentation included with the redistribution,
 *       if any, must include the following acknowledgment: "This product
 *       includes software developed by The XFree86 Project, Inc
 *       (http://www.xfree86.org/) and its contributors", in the same
 *       place and form as other third-party acknowledgments.  Alternately,
 *       this acknowledgment may appear in the software itself, in the
 *       same form and location as other such third-party acknowledgments.
 *
 *   4.  Except as contained in this notice, the name of The XFree86
 *       Project, Inc shall not be used in advertising or otherwise to
 *       promote the sale, use or other dealings in this Software without
 *       prior written authorization from The XFree86 Project, Inc.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE XFREE86 PROJECT, INC OR ITS CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 * OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*
 * Copyright 2003-2006 by David H. Dawes.
 * Copyright 2003-2006 by X-Oz Technologies.
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 * 
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions, and the following disclaimer.
 *
 *  2. Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 * 
 *  3. The end-user documentation included with the redistribution,
 *     if any, must include the following acknowledgment: "This product
 *     includes software developed by X-Oz Technologies
 *     (http://www.x-oz.com/)."  Alternately, this acknowledgment may
 *     appear in the software itself, if and wherever such third-party
 *     acknowledgments normally appear.
 *
 *  4. Except as contained in this notice, the name of X-Oz
 *     Technologies shall not be used in advertising or otherwise to
 *     promote the sale, use or other dealings in this Software without
 *     prior written authorization from X-Oz Technologies.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL X-OZ TECHNOLOGIES OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 * OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */

#ifndef _LOADER_H
#define _LOADER_H

#ifndef LOADERDEBUG
#define LOADERDEBUG 0
#endif

#include "sym.h"

#if defined(Lynx) && defined(sun)
#define const /**/
#endif
#if (defined(__i386__) || defined(__ix86)) && !defined(i386)
#define i386
#endif
#include <X11/Xosdefs.h>
#include <X11/Xfuncproto.h>
#include <X11/Xmd.h>
/* For LOOKUP definition */
#include "sym.h"
#define LD_UNKNOWN	-1
#define LD_ARCHIVE	0
#define LD_ELFOBJECT	1
#define LD_COFFOBJECT	2
#define LD_XCOFFOBJECT	3
#define LD_AOUTOBJECT   4
#define LD_AOUTDLOBJECT	5
#define LD_ELFDLOBJECT	6
#define LD_PROCESSED_ARCHIVE -1
/* #define UNINIT_SECTION */
#define HANDLE_IN_HASH_ENTRY

#define TestFree(a) if (a) { xfree (a); a = NULL; }

#if LOADERDEBUG
/*
 * Loader debug levels.
 */
#define LOADER_DEBUG_ADD_SYMBOLS		0x00001
#define LOADER_DEBUG_HASH_ADD 			0x00002
#define LOADER_DEBUG_DIAGNOSTICS 		0x00004
#define LOADER_DEBUG_FILES	 		0x00008
#define LOADER_DEBUG_LOWLEVEL	 		0x00010
#define LOADER_DEBUG_DUMP_HASH			0x00020
#define LOADER_DEBUG_HASH_DETAILS		0x00040
#define LOADER_DEBUG_HASH_SUMMARY		0x00080
#define LOADER_DEBUG_REPORT_RELOC		0x00100
#define LOADER_DEBUG_REQ_REF			0x00200
#define LOADER_DEBUG_SHOW_EXE_SYMS		0x00400
#define LOADER_DEBUG_PLT			0x00800
#define LOADER_DEBUG_ARCHIVE			0x01000
#define LOADER_DEBUG_MEM			0x02000
#define LOADER_DEBUG_EXT			0x04000

#ifdef LOADER_DEBUG_DESCRIPTIONS
static const char *debugDesc [] = {
    "AddSymbols",
    "HashAdd",
    "Diagnostics (memory usage)",
    "Files loaded",
    "Low level object processing",
    "Dump hash table",
    "Hash hit/depth details",
    "Hash hit/depth summary",
    "Report relocations",
    "Req/Ref symbols",
    "Show exe symbols",
    "PLT handling",
    "Archives",
    "Memory",
    "LoadExtensions",
    NULL
};
#endif
#endif

typedef struct _loader_item *itemPtr;
typedef struct _loader_item {
    char *name;
    void *address;
    itemPtr next;
    int handle;
    int module;
    unsigned long scope;
#if defined(__powerpc__)
    /*
     * PowerPC file formats require special routines in some circumstances
     * to assist in the linking process. See the specific loader for
     * more details.  XXX This should be eliminated and handled in the same
     * way as the pseudo PLT is now handled for ELF.
     */
    union {
	unsigned short glink[14];	/* XCOFF */
    } code;
#endif
} itemRec;

/* Values for itemRec.scope. */
#define LOOKUP_SCOPE_SELF	0
#define LOOKUP_SCOPE_GLOBAL	(1 << 0)
#define LOOKUP_SCOPE_BUILTIN	(1 << 1)
#define LOOKUP_SCOPE_AUTO	(~0UL)
#define LOOKUP_SCOPE_ANY	(~0UL)

#define SCOPE_OK(i, h, s)	\
    (((s) == LOOKUP_SCOPE_ANY) || \
     ((i)->scope == LOOKUP_SCOPE_SELF && (i)->handle == (h)) || \
     ((s) & (i)->scope))

/* Special module symbol names. */
#define MODULE_DATA_NAME	"ModuleData"
#define EXPORTED_SYMS_NAME	"ExportedSymbols"

/* The following structures provide an interface to GDB (note that GDB
   has copies of the definitions - if you change anything here make
   sure that the changes are also made to GDB */

typedef struct {
    char *name;			/* Name of this symbol */
    unsigned int namelen;	/* Name of this module */
    void *addr;			/* Start address of the .text section */
} LDRCommon, *LDRCommonPtr;

typedef struct x_LDRModuleRec {
    unsigned int version;	/* Version of this struct */
    char *name;			/* Name of this module */
    unsigned int namelen;	/* Length of name */
    void *text;			/* Start address of the .text section */
    void *data;			/* Start address of the .data section */
    void *rodata;		/* Start address of the .rodata section */
    void *bss;			/* Start address of the .bss section */
    LDRCommonPtr commons;	/* List of commmon symbols */
    int commonslen;		/* Number of common symbols */
    struct x_LDRModuleRec *next;	/* Next module record in chain */
} LDRModuleRec, *LDRModulePtr;

extern char DebuggerPresent;
extern LDRModulePtr ModList;
extern LDRCommonPtr ldrCommons;
extern int nCommons;

/*
 * The loader uses loader specific alloc/calloc/free functions that
 * are mapped to either to the regular Xserver functions, or in a couple
 * of special cases, mapped to the C library functions.
 */
#if !defined(PowerMAX_OS) && !(defined(linux) && (defined(__alpha__) || defined(__powerpc__) || defined(__ia64__))) && 0
#define xf86loadermalloc(size) xalloc(size)
#define xf86loaderrealloc(ptr,size) xrealloc(ptr,size)
#define xf86loadercalloc(num,size) xcalloc(num,size)
#define xf86loaderfree(ptr) xfree(ptr)
#define xf86loaderstrdup(ptr) xstrdup(ptr)
#else
/*
 * On Some OSes, xalloc() et al uses mmap to allocate space for large
 * allocation. This has the effect of placing the text section of some
 * modules very far away from the rest which are placed on the heap.
 * Certain relocations are limited in the size of the offsets that can be
 * handled, and this seperation causes these relocation to overflow. This
 * is fixed by just using the C library allocation functions for the loader
 * to ensure that all text sections are located on the heap. OSes that have
 * this problem are:
 *	PowerMAX_OS/PPC
 * 	Linux/Alpha
 * 	Linux/PPC
 *	Linux/IA-64
 */
#define xf86loadermalloc(size) malloc(size)
#define xf86loaderrealloc(ptr,size) realloc(ptr,size)
#define xf86loadercalloc(num,size) calloc(num,size)
#define xf86loaderfree(ptr) free(ptr)
#define xf86loaderstrdup(ptr) strdup(ptr)
#endif

typedef struct _loader *loaderPtr;

/*
 * _loader_funcs hold the entry points for a module format.
 */

typedef struct _loader_desc LoaderDesc, *LoaderDescPtr;

typedef void *(*LoadModuleProcPtr)(loaderPtr modrec, int fd, LOOKUP **);
typedef void (*ResolveSymbolsProcPtr)(LoaderDescPtr, int);
typedef int (*CheckForUnresolvedProcPtr)(LoaderDescPtr);
typedef char *(*AddressToSectionProcPtr)(void *, unsigned long);
typedef void (*LoaderUnloadProcPtr)(void *);
typedef const char *(*FindRelocNameProcPtr)(LoaderDescPtr, int, unsigned long);
typedef const char *(*AddressToSymbolProcPtr)(void *, unsigned long,
					      unsigned long *, const char **,
					      int);
typedef void *(*ReadExecutableSymsProcPtr)(int);

struct _loader_desc {
    LoadModuleProcPtr LoadModule;
    ResolveSymbolsProcPtr ResolveSymbols;
    CheckForUnresolvedProcPtr CheckForUnresolved;
    AddressToSectionProcPtr AddressToSection;
    LoaderUnloadProcPtr LoaderUnload;
    FindRelocNameProcPtr FindRelocName;
    AddressToSymbolProcPtr AddressToSymbol;
    ReadExecutableSymsProcPtr ReadExecutableSyms;
    void *pRelocs;	/* type specific relocations */
};

/* Each module loaded has a loaderRec */
typedef struct _loader {
    int handle;			/* Unique id used to remove symbols from
				 * this module when it is unloaded */
    int module;			/* Unique id to identify compilation units */
    char *name;
    char *cname;
    void *private;		/* format specific data */
    LoaderDescPtr desc;		/* funcs/data for operating on this module */
    loaderPtr next;
    char *modData;
} loaderRec;

/* Compiled-in version information */
typedef struct {
    int xf86Version;
    int ansicVersion;
    int videodrvVersion;
    int xinputVersion;
    int extensionVersion;
    int fontVersion;
} ModuleVersions;
extern ModuleVersions LoaderVersionInfo;

extern unsigned long LoaderOptions;
extern unsigned long LoaderDebugLevel;
extern void LoaderDebugForModule(const char *module);

/* Internal Functions */

#if (!defined(printf) || defined(printf_is_xf86printf)) && \
    defined(__GNUC__) && \
    ((__GNUC__ > 2) || ((__GNUC__ == 2) && (__GNUC_MINOR__ > 4)))
# define _printf_attribute(a,b) __attribute((format(printf,a,b)))
# undef printf
#else
# define _printf_attribute(a,b) /**/
#endif

void LoaderDebugMsg(unsigned long, const char *f, ...) _printf_attribute(2,3);

void LoaderAddSymbols(int, int, LOOKUP *, unsigned long, char **);
void LoaderDefaultFunc(void);
void LoaderDuplicateSymbol(const char *, const int);
void *LoaderSymbolWithScope(const char *, int, unsigned long);

int LoaderResolveSymbols(int);
int _LoaderHandleUnresolved(const char *, int);
void LoaderHashAdd(itemPtr);
itemPtr LoaderHashDelete(const char *);
itemPtr LoaderHashFind(const char *);
void LoaderHashTraverse(void *, int (*)(void *, itemPtr));
void LoaderPrintAddress(const char *);
void LoaderPrintItem(itemPtr);
void LoaderDumpSymbols(void);
void LoaderDumpHashHits(void);
char *_LoaderModuleToName(int);
int _LoaderAddressToSection(const unsigned long, const char **,
			    const char **);
int LoaderOpen(const char *, const char *, int, int *, int *, int *, char **);
int LoaderHandleOpen(int);
const char *LoaderFindRelocName(int handle, unsigned long addr);
const char *_LoaderAddressToSymbol(unsigned long addr, unsigned long *symaddr,
				   const char **);


/*
 * File interface functions
 */
void *_LoaderFileToMem(int fd, unsigned long offset, int size, char *label);
void _LoaderFreeFileMem(void *addr, int size);
int _LoaderFileRead(int fd, unsigned int offset, void *addr, int size);

/*
 * Relocation list manipulation routines
 */
void **_LoaderGetRelocations(LoaderDescPtr);

/*
 * object to name lookup routines
 */
char *_LoaderHandleToName(int handle);
char *_LoaderHandleToCanonicalName(int handle);

extern void _loader_debug_state(void);

#undef _printf_attribute
#if defined(printf_is_xf86printf) && !defined(printf)
#define printf xf86printf
#endif

#endif /* _LOADER_H */
