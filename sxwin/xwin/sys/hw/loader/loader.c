/* $XFree86: xc/programs/Xserver/hw/xfree86/loader/loader.c,v 1.83 2006/08/18 14:36:05 tsi Exp $ */

/*
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
 * Copyright (c) 1997-2005 by The XFree86 Project, Inc.
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

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#if defined(UseMMAP) || (defined(linux) && defined(__ia64__))
#include <sys/mman.h>
#endif
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#if defined(linux) && \
    (defined(__alpha__) || defined(__powerpc__) || defined(__ia64__) \
    || defined(__amd64__) || defined(__x86_64__))
#include <malloc.h>
#endif
#include <stdarg.h>
#if defined(sun) && defined(SVR4)
#include <procfs.h>
#endif
#include "ar.h"
#include "elf.h"
#ifdef COFF_SUPPORT
#include "coff.h"
#endif
#include "os.h"
#include "sym.h"

#ifndef LOADERDEBUG
#define LOADERDEBUG 0
#endif
#define LOADER_DEBUG_DESCRIPTIONS
#include "loader.h"
#include "aoutloader.h"
#ifdef COFF_SUPPORT
#include "coffloader.h"
#endif
#include "elfloader.h"
#ifdef DLOPEN_SUPPORT
#include "dlloader.h"
#endif

#include "loaderProcs.h"
#include "xf86.h"
#include "xf86Priv.h"

#include "compiler.h"

#ifdef STACKTRACE
#include "getstack.h"
#endif

#ifndef LOADER_UNRESOLVED_IS_FATAL
#define LOADER_UNRESOLVED_IS_FATAL 0
#endif

/*
 * procfs feature defines.  procfs is used, where available, to locate the
 * main executable file so that the loader can open it and examine its symbols.
 *
 * Features are:
 *
 * HAVE_PROCFS			- /proc is available.
 * PROCFS_EXE_SYMLINK		- /proc provides a symlink to the executable.
 * PROCFS_EXE_NAME		- The node under /proc/<pid>/ that points to
 *				  the executable.
 * HAVE_PROCFS_PSINFO		- /proc/<pid>/psinfo can be used to get
 *				  the executable name.
 * PROCFS_PSINFO_FULLPATH	- The full executable pathname can be found
 *				  via /proc/<pid>/psinfo.
 */

#if defined(__linux__)
#define HAVE_PROCFS
#define PROCFS_EXE_SYMLINK
#define PROCFS_EXE_NAME "exe"
#elif defined(__FreeBSD__)
#define HAVE_PROCFS
#define PROCFS_EXE_SYMLINK
#define PROCFS_EXE_NAME "file"
#elif defined(__NetBSD__) || defined(__OpenBSD__)
#define HAVE_PROCFS
#define PROCFS_EXE_NAME "file"
#elif defined(sun) && defined(SVR4)
#define HAVE_PROCFS
#define PROCFS_EXE_NAME "object/a.out"
#define HAVE_PROCFS_PSINFO
#endif

extern LOOKUP miLookupTab[];
extern LOOKUP xfree86LookupTab[];
extern LOOKUP dixLookupTab[];
extern LOOKUP fontLookupTab[];
extern LOOKUP extLookupTab[];

#if defined(Lynx) && defined(sun)
/* Cross build machine doesn;t have strerror() */
#define strerror(err) "strerror unsupported"
#endif

#ifdef __UNIXOS2__
void *os2ldcalloc(size_t, size_t);
#endif

#ifdef HANDLE_IN_HASH_ENTRY
/*
 * handles are used to identify files that are loaded. Even archives
 * are counted as a single file.
 */
#define MAX_HANDLE 256
#define HANDLE_FREE 0
#define HANDLE_USED 1
static char freeHandles[MAX_HANDLE];
static int refCount[MAX_HANDLE];
#endif

#if defined(__sparc__) && defined(__GNUC__) && !defined(__FreeBSD__)
# define SYMFUNCDOT(func) { "." #func, (funcptr)&__sparc_dot_ ## func },
# if !defined(__OpenBSD__)
# define SYMFUNCDOT89(func) { "." #func, (funcptr)&func ## _sparcv89 },
# define DEFFUNCDOT(func) 					\
extern void __sparc_dot_ ## func (void) __asm__ ("." #func);	\
extern void func ## _sparcv89 (void);
# else
# define SYMFUNCDOT(func) { "." #func, (funcptr)&__sparc_dot_ ## func },
# define DEFFUNCDOT(func) 					\
extern void __sparc_dot_ ## func (void) __asm__ ("." #func);
#endif
DEFFUNCDOT(rem)
DEFFUNCDOT(urem)
DEFFUNCDOT(mul)
DEFFUNCDOT(umul)
DEFFUNCDOT(div)
DEFFUNCDOT(udiv)
#ifdef linux
static LOOKUP SparcV89LookupTab[] = {
    SYMFUNCDOT89(rem)
    SYMFUNCDOT89(urem)
    SYMFUNCDOT89(mul)
    SYMFUNCDOT89(umul)
    SYMFUNCDOT89(div)
    SYMFUNCDOT89(udiv)
    {0, 0}
};
#endif
static LOOKUP SparcLookupTab[] = {
    SYMFUNCDOT(rem)
    SYMFUNCDOT(urem)
    SYMFUNCDOT(mul)
    SYMFUNCDOT(umul)
    SYMFUNCDOT(div)
    SYMFUNCDOT(udiv)
    {0, 0}
};

#ifdef linux
#if defined(__GNUC__) && defined(__GLIBC__)
#define HWCAP_SPARC_MULDIV	8
extern unsigned long int _dl_hwcap;
#endif

static int
sparcUseHWMulDiv(void)
{
    FILE *f;
    char buffer[1024];
    char *p;

#if defined(__GNUC__) && defined(__GLIBC__)
    unsigned long *hwcap;

    __asm(".weak _dl_hwcap");

    hwcap = &_dl_hwcap;
  __asm("": "=r"(hwcap):"0"(hwcap));
    if (hwcap) {
	if (*hwcap & HWCAP_SPARC_MULDIV)
	    return 1;
	else
	    return 0;
    }
#endif
    f = fopen("/proc/cpuinfo", "r");
    if (!f)
	return 0;
    while (fgets(buffer, 1024, f) != NULL) {
	if (!strncmp(buffer, "type", 4)) {
	    p = strstr(buffer, "sun4");
	    if (p && (p[4] == 'u' || p[4] == 'd' || p[4] == 'm')) {
		fclose(f);
		return 1;
	    }
	}
    }
    fclose(f);
    return 0;
}
#endif
#endif

/*
 * modules are used to identify compilation units (ie object modules).
 * Archives contain multiple modules, each of which is treated seperately.
 */
static int moduleseq = 0;

/*
 * GDB Interface
 * =============
 *
 * Linked list of loaded modules - gdb will traverse this to determine
 * whether it needs to add the symbols for the loaded module.
 */
LDRModulePtr ModList = 0;

/* Flag which gdb sets to let us know we're being debugged */
char DebuggerPresent = 0;

/* List of common symbols */
LDRCommonPtr ldrCommons;
int nCommons;

typedef struct _symlist symlist, *symlistPtr;

struct _symlist {
    int num;
    ModuleDescPtr module;
    const char **list;
    symlistPtr next;
};

/*
 * List of symbols that may be referenced, and which are allowed to be
 * unresolved providing that they don't appear on the "reqired" list.
 */
static symlist refList = { 0, NULL, NULL, NULL };

/* List of symbols that must not be unresolved */
static symlist reqList = { 0, NULL, NULL, NULL };

static int fatalReqSym = 0;

/* Prototypes for static functions. */
static int _GetModuleType(int, long);
static loaderPtr _LoaderListPush(void);
static loaderPtr _LoaderListPop(int);
static void ReadMainExe(void);
static const char *GetExePath(const char **);

static char *exePath = NULL;
static char *exeName = NULL;

#ifndef COFF_SUPPORT
static void *
DUMMYLoadModule(loaderPtr modrec, int fd, LOOKUP **ppLookup)
{
    return NULL;
}
#endif

static void
DUMMYResolveSymbols(LoaderDescPtr desc, int handle)
{
}
static int
DUMMYCheckForUnresolved(LoaderDescPtr desc)
{
    return 0;
}
static char *
DUMMYAddressToSection(void *modptr, unsigned long address)
{
    return NULL;
}
static void
DUMMYUnload(void *v)
{
}

static const char *
DUMMYFindRelocName(LoaderDescPtr desc, int handle, unsigned long addr)
{
    return NULL;
}

static const char *
DUMMYAddressToSymbol(void *modptr, unsigned long addr, unsigned long *symaddr,
		     const char **filename, int exe)
{
    return NULL;
}

static void *
DUMMYReadExecutableSyms(int exefd)
{
    return NULL;
}

static void *ARCHIVELoadModule(loaderPtr modrec, int arfd, LOOKUP ** ppLookup);

/*
 * Array containing entry points for different formats.
 */
#ifndef COFF_SUPPORT
static LoaderDesc DUMMYdesc = {
     DUMMYLoadModule,
     DUMMYResolveSymbols,
     DUMMYCheckForUnresolved,
     DUMMYAddressToSection,
     DUMMYUnload,
     DUMMYFindRelocName,
     DUMMYAddressToSymbol,
     DUMMYReadExecutableSyms,
     NULL
};
#endif

static LoaderDesc ARCHIVEdesc = {
    ARCHIVELoadModule,
    DUMMYResolveSymbols,
    DUMMYCheckForUnresolved,
    DUMMYAddressToSection,
    DUMMYUnload,
    DUMMYFindRelocName,
    DUMMYAddressToSymbol,
    DUMMYReadExecutableSyms,
    NULL
};

static LoaderDesc ELFdesc = {
    ELFLoadModule,
    ELFResolveSymbols,
    ELFCheckForUnresolved,
    ELFAddressToSection,
    ELFUnloadModule,
    ELFFindRelocName,
    ELFAddressToSymbol,
    ELFReadExecutableSyms,
    NULL
};

#ifdef COFF_SUPPORT
static LoaderDesc COFFdesc = {
    COFFLoadModule,
    COFFResolveSymbols,
    COFFCheckForUnresolved,
    COFFAddressToSection,
    COFFUnloadModule,
    COFFFindRelocName,
    COFFAddressToSymbol,
    COFFReadExecutableSyms,
    NULL
};
#endif

static LoaderDesc AOUTdesc = {
    AOUTLoadModule,
    AOUTResolveSymbols,
    AOUTCheckForUnresolved,
    AOUTAddressToSection,
    AOUTUnloadModule,
    AOUTFindRelocName,
    AOUTAddressToSymbol,
    AOUTReadExecutableSyms,
    NULL
};

#ifdef DLOPEN_SUPPORT
static LoaderDesc DLdesc = {
    DLLoadModule,
    DUMMYResolveSymbols,
    DUMMYCheckForUnresolved,
    DUMMYAddressToSection,
    DLUnloadModule,
    DUMMYFindRelocName,
    DLAddressToSymbol,
    DUMMYReadExecutableSyms,
    NULL
};
#endif

static LoaderDescPtr ldesc[] = {
    &ARCHIVEdesc,		/* LD_ARCHIVE */
    &ELFdesc,			/* LD_ELFOBJECT */
#ifdef COFF_SUPPORT
    &COFFdesc,			/* LD_COFFOBJECT */
    &COFFdesc,			/* LD_XCOFFOBJECT */
#else
    &DUMMYdesc,			/* LD_COFFOBJECT */
    &DUMMYdesc,			/* LD_XCOFFOBJECT */
#endif
    &AOUTdesc,			/* LD_AOUTOBJECT */
#ifdef DLOPEN_SUPPORT
    &DLdesc,			/* LD_AOUTDLOBJECT */
    &DLdesc,			/* LD_ELFDLOBJECT */
#else
    &AOUTdesc,			/* LD_AOUTDLOBJECT */
    &ELFdesc,			/* LD_ELFDLOBJECT */
#endif
};

int numloaders = sizeof(ldesc) / sizeof(ldesc[0]);

const char *loaderNames[] = {
    "archive",
    "elf",
    "coff",
    "xcoff",
    "a.out",
    "a.out dl",
    "elf dl"
};

void
LoaderInit(void)
{
    const char *osname = NULL;
#if LOADERDEBUG
    int i;
#endif

    ReadMainExe();
    LoaderAddSymbols(-1, -1, miLookupTab,
		     LOOKUP_SCOPE_GLOBAL | LOOKUP_SCOPE_BUILTIN, NULL);
    LoaderAddSymbols(-1, -1, xfree86LookupTab,
		     LOOKUP_SCOPE_GLOBAL | LOOKUP_SCOPE_BUILTIN, NULL);
    LoaderAddSymbols(-1, -1, dixLookupTab,
		     LOOKUP_SCOPE_GLOBAL | LOOKUP_SCOPE_BUILTIN, NULL);
    LoaderAddSymbols(-1, -1, fontLookupTab,
		     LOOKUP_SCOPE_GLOBAL | LOOKUP_SCOPE_BUILTIN, NULL);
    LoaderAddSymbols(-1, -1, extLookupTab,
		     LOOKUP_SCOPE_GLOBAL | LOOKUP_SCOPE_BUILTIN, NULL);
#if defined(__sparc__) && !defined(__FreeBSD__)
#ifdef linux
    if (sparcUseHWMulDiv())
	LoaderAddSymbols(-1, -1, SparcV89LookupTab,
			 LOOKUP_SCOPE_GLOBAL | LOOKUP_SCOPE_BUILTIN, NULL);
    else
#endif
	LoaderAddSymbols(-1, -1, SparcLookupTab,
			 LOOKUP_SCOPE_GLOBAL | LOOKUP_SCOPE_BUILTIN, NULL);
#endif

    xf86MsgVerb(X_INFO, 2, "Module ABI versions:\n");
    xf86ErrorFVerb(2, "\t%s: %d.%d\n", ABI_CLASS_ANSIC,
		   GET_ABI_MAJOR(LoaderVersionInfo.ansicVersion),
		   GET_ABI_MINOR(LoaderVersionInfo.ansicVersion));
    xf86ErrorFVerb(2, "\t%s: %d.%d\n", ABI_CLASS_VIDEODRV,
		   GET_ABI_MAJOR(LoaderVersionInfo.videodrvVersion),
		   GET_ABI_MINOR(LoaderVersionInfo.videodrvVersion));
    xf86ErrorFVerb(2, "\t%s : %d.%d\n", ABI_CLASS_XINPUT,
		   GET_ABI_MAJOR(LoaderVersionInfo.xinputVersion),
		   GET_ABI_MINOR(LoaderVersionInfo.xinputVersion));
    xf86ErrorFVerb(2, "\t%s : %d.%d\n", ABI_CLASS_EXTENSION,
		   GET_ABI_MAJOR(LoaderVersionInfo.extensionVersion),
		   GET_ABI_MINOR(LoaderVersionInfo.extensionVersion));
    xf86ErrorFVerb(2, "\t%s : %d.%d\n", ABI_CLASS_FONT,
		   GET_ABI_MAJOR(LoaderVersionInfo.fontVersion),
		   GET_ABI_MINOR(LoaderVersionInfo.fontVersion));

    LoaderGetOS(&osname, NULL, NULL, NULL);
    if (osname)
	xf86MsgVerb(X_INFO, 2, "Loader running on %s\n", osname);

#if LOADERDEBUG
    xf86MsgVerb(X_INFO, 3, "Loader Debug Flags:\n");
    for (i = 0; debugDesc[i]; i++)
	xf86MsgVerb(X_INFO, 3, "%6d - %s\n", 1 << i, debugDesc[i]);
    if (LoaderDebugLevel)
	xf86MsgVerb(X_INFO, 2, "Loader debug level set to 0x%lx (%ld).\n",
		    LoaderDebugLevel, LoaderDebugLevel);
#endif

#if defined(linux) && \
    (defined(__alpha__) || defined(__powerpc__) || defined(__ia64__) \
     || ((defined(__amd64__) || defined(__x86_64__)) && \
	 !defined(UseMMAP) && ! defined(DoMMAPedMerge)))
    /*
     * The glibc malloc uses mmap for large allocations anyway. This breaks
     * some relocation types because the offset overflow. See loader.h for more
     * details. We need to turn off this behavior here.
     */
    mallopt(M_MMAP_MAX, 0);
#endif
}

/*
 * Determine what type of object is being loaded.
 * This function is responsible for restoring the offset.
 * The fd and offset are used here so that when Archive processing
 * is enabled, individual elements of an archive can be evaluated
 * so the correct loader_funcs can be determined.
 */
static int
_GetModuleType(int fd, long offset)
{
    unsigned char buf[256];	/* long enough for the largest magic type */

    if (read(fd, buf, sizeof(buf)) < 0) {
	return -1;
    }
#if LOADERDEBUG
    LoaderDebugMsg(LOADER_DEBUG_FILES, "Checking module type %10s\n", buf);
    LoaderDebugMsg(LOADER_DEBUG_FILES, "Checking module type %x %x %x %x\n",
		   buf[0], buf[1], buf[2], buf[3]);
#endif

    lseek(fd, offset, SEEK_SET);

    if (strncmp((char *)buf, ARMAG, SARMAG) == 0) {
	return LD_ARCHIVE;
    }
#if defined(AIAMAG)
    /* LynxOS PPC style archives */
    if (strncmp((char *)buf, AIAMAG, SAIAMAG) == 0) {
	return LD_ARCHIVE;
    }
#endif

    if (strncmp((char *)buf, ELFMAG, SELFMAG) == 0) {
	if (*((Elf32_Half *) (buf + ELFDLOFF)) == ELFDLMAG) {
	    return LD_ELFDLOBJECT;
	} else {
	    return LD_ELFOBJECT;
	}
    }

    if (buf[0] == 0x4c && buf[1] == 0x01) {
	/* I386MAGIC */
	return LD_COFFOBJECT;
    }
    if (buf[0] == 0x01 && buf[1] == 0xdf) {
	/* XCOFFMAGIC */
	return LD_COFFOBJECT;
    }
    if (buf[0] == 0x0d && buf[1] == 0x01) {
	/* ZCOFFMAGIC (LynxOS) */
	return LD_COFFOBJECT;
    }
    if (buf[0] == 0x00 && buf[1] == 0x86 && buf[2] == 0x01 && buf[3] == 0x07) {
	/* AOUTMAGIC */
	return LD_AOUTOBJECT;
    }
    if (buf[0] == 0x07 && buf[1] == 0x01
	&& (buf[2] == 0x64 || buf[2] == 0x86)) {
	/* AOUTMAGIC, (Linux OMAGIC, old impure format, also used by OS/2 */
	return LD_AOUTOBJECT;
    }
    if (buf[0] == 0x07 && buf[1] == 0x01 && buf[2] == 0x00 && buf[3] == 0x00) {
	/* AOUTMAGIC, BSDI */
	return LD_AOUTOBJECT;
    }
    if (buf[1] == 0x86 && buf[2] == 0x01 && buf[3] == 0x0b) {
	/* AOUT BSD/i386 demand paged executable */
	return LD_AOUTOBJECT;
    }
    if (buf[0] == 0xcc && buf[1] == 0x00 && buf[2] == 0x86) {
	/* AOUT FreeBSD/i386 compact demand paged executable */
	return LD_AOUTOBJECT;
    }
    if ((buf[0] == 0xc0 && buf[1] == 0x86) ||	/* big endian form */
	(buf[3] == 0xc0 && buf[2] == 0x86)) {	/* little endian form */
	/* i386 shared object */
	return LD_AOUTDLOBJECT;
    }

    return LD_UNKNOWN;
}

static int offsetbias = 0;	/* offset into archive */

/*
 * _LoaderFileToMem() loads the contents of a file into memory using
 * the most efficient method for a platform.
 */
void *
_LoaderFileToMem(int fd, unsigned long offset, int size, char *label)
{
#ifdef UseMMAP
    unsigned long ret;

# ifdef MmapPageAlign
    unsigned long pagesize;
    unsigned long new_size;
    unsigned long new_off;
    unsigned long new_off_bias;
# endif
# define MMAP_PROT	(PROT_READ|PROT_WRITE|PROT_EXEC)
# if !(defined(__amd64__) || defined(__x86_64__)) || !defined(__linux__)
# define MMAP_FLAGS     (MAP_PRIVATE) 
# else 
# define MMAP_FLAGS     (MAP_PRIVATE | MAP_32BIT)
# endif

# if LOADERDEBUG
    LoaderDebugMsg(LOADER_DEBUG_MEM, "_LoaderFileToMem(%d,%lx(%u),%d,%s)",
		   fd, offset, offsetbias, size, label);
# endif
# ifdef MmapPageAlign
    pagesize = getpagesize();
    new_size = (size + pagesize - 1) / pagesize;
    new_size *= pagesize;
    new_off = (offset + offsetbias) / pagesize;
    new_off *= pagesize;
    new_off_bias = (offset + offsetbias) - new_off;
    if ((new_off_bias + size) > new_size)
	new_size += pagesize;
    ret = (unsigned long)mmap(0, new_size, MMAP_PROT, MMAP_FLAGS, fd,
			      new_off);
#  if LOADERDEBUG
    LoaderDebugMsg(LOADER_DEBUG_MEM,
		   "mmap: new_off %lx, new_off_bias %lx, new_size %lu, ret %lx\n",
		   new_off, new_off_bias, new_size, ret);
#  endif
    if (ret == -1)
	FatalError("mmap() failed: %s\n", strerror(errno));
    return (void *)(ret + new_off_bias);
# else
    ret = (unsigned long)mmap(0, size, MMAP_PROT, MMAP_FLAGS, fd,
			      offset + offsetbias);
#  if LOADERDEBUG
    LoaderDebugMsg(LOADER_DEBUG_MEM,
		   "mmap: fd %d, size %d, offset %lx, offsetbias %u, ret %d\n",
		   fd, size, offset, offsetbias, ret);
#  endif
    if (ret == -1)
	FatalError("mmap() failed: %s\n", strerror(errno));
    return (void *)ret;
# endif
#else
    char *ptr;

# if LOADERDEBUG
    LoaderDebugMsg(LOADER_DEBUG_MEM, "_LoaderFileToMem(%d,%lx(%u),%d,%s)",
		   fd, offset, offsetbias, size, label);
# endif

    if (size == 0) {
# if LOADERDEBUG
	LoaderDebugMsg(LOADER_DEBUG_MEM, "=NULL\n");
# endif
	return NULL;
    }
# ifndef __UNIXOS2__
    if ((ptr = xf86loadercalloc(size, 1)) == NULL)
	FatalError("_LoaderFileToMem() malloc failed\n");
# else
    if ((ptr = os2ldcalloc(size, 1)) == NULL)
	FatalError("_LoaderFileToMem() malloc failed\n");
# endif
# if defined(linux) && defined(__ia64__)
    {
	unsigned long page_size = getpagesize();
	unsigned long round;

	round = (unsigned long)ptr & (page_size - 1);
	mprotect(ptr - round,
		 (size + round + page_size - 1) & ~(page_size - 1),
		 PROT_READ | PROT_WRITE | PROT_EXEC);
    }
# endif

    if (lseek(fd, offset + offsetbias, SEEK_SET) < 0)
	FatalError("\n_LoaderFileToMem() lseek() failed: %s\n",
		   strerror(errno));

    if (read(fd, ptr, size) != size)
	FatalError("\n_LoaderFileToMem() read() failed: %s\n",
		   strerror(errno));

# if (defined(linux) || defined(__NetBSD__) || defined(__OpenBSD__)) \
    && defined(__powerpc__)
    /*
     * Keep the instruction cache in sync with changes in the
     * main memory.
     */
    {
	int i;

	for (i = 0; i < size; i += 16)
	    ppc_flush_icache(ptr + i);
	ppc_flush_icache(ptr + size - 1);
    }
# endif

# if LOADERDEBUG
    LoaderDebugMsg(LOADER_DEBUG_MEM, "=%p\n", ptr);
# endif

    return (void *)ptr;
#endif
}

/*
 * _LoaderFreeFileMem() free the memory in which a file was loaded.
 */
void
_LoaderFreeFileMem(void *addr, int size)
{
#if defined (UseMMAP) && defined (MmapPageAlign)
    unsigned long pagesize = getpagesize();
    memType i_addr = (memType) addr;
    unsigned long new_size;
#endif
#if LOADERDEBUG
    LoaderDebugMsg(LOADER_DEBUG_MEM, "_LoaderFreeFileMem(%p,%d)\n", addr, size);
#endif
#ifdef UseMMAP
# if defined (MmapPageAlign)
    i_addr /= pagesize;
    i_addr *= pagesize;
    new_size = (size + pagesize - 1) / pagesize;
    new_size *= pagesize;
    if (((memType) addr - i_addr + size) > new_size)
	new_size += pagesize;
    munmap((void *)i_addr, new_size);
# else
    munmap((void *)addr, size);
# endif
#else
    if (size == 0)
	return;

    xf86loaderfree(addr);
#endif

    return;
}

int
_LoaderFileRead(int fd, unsigned int offset, void *buf, int size)
{
    if (lseek(fd, offset + offsetbias, SEEK_SET) < 0)
	FatalError("_LoaderFileRead() lseek() failed: %s\n", strerror(errno));

    if (read(fd, buf, size) != size)
	FatalError("_LoaderFileRead() read() failed: %s\n", strerror(errno));

    return size;
}

static loaderPtr listHead = NULL;
static loaderPtr mainExeItem = NULL;
static int mainExeDlType = -1;

static loaderPtr
_LoaderListPush()
{
    loaderPtr item = xf86loadercalloc(1, sizeof(struct _loader));

    item->next = listHead;
    listHead = item;

    return item;
}

static loaderPtr
_LoaderListPop(int handle)
{
    loaderPtr item = listHead;
    loaderPtr *bptr = &listHead;	/* pointer to previous node */

    while (item) {
	if (item->handle == handle) {
	    *bptr = item->next;	/* remove this from the list */
	    return item;
	}
	bptr = &(item->next);
	item = item->next;
    }

    return 0;
}

static loaderPtr
_LoaderHandleToItem(int handle)
{
    loaderPtr item = listHead;
    loaderPtr aritem = NULL;
    loaderPtr lastitem = NULL;

    if (handle < 0) {
	return NULL;
    }
    while (item) {
	if (item->handle == handle) {
	    if (strchr(item->name, ':') == NULL)
		aritem = item;
	    else
		lastitem = item;
	}
	item = item->next;
    }

    if (aritem)
	return aritem;

    if (lastitem)
	return lastitem;

    return NULL;
}

/*
 * _LoaderHandleToName() will return the name of the first module with a
 * given handle. This requires getting the last module on the LIFO with
 * the given handle.
 */
char *
_LoaderHandleToName(int handle)
{
    loaderPtr item;

    if (handle < 0) {
	return "(built-in)";
    }

    item = _LoaderHandleToItem(handle);
    if (item)
	return item->name;
    else
	return NULL;
}

/*
 * _LoaderHandleToCanonicalName() will return the cname of the first module
 * with a given handle. This requires getting the last module on the LIFO with
 * the given handle.
 */
char *
_LoaderHandleToCanonicalName(int handle)
{
    loaderPtr item;

    if (handle < 0) {
	return "(built-in)";
    }

    item = _LoaderHandleToItem(handle);
    if (item)
	return item->cname;
    else
	return NULL;
}

/*
 * _LoaderModuleToName() will return the name of the first module with a
 * given handle. This requires getting the last module on the LIFO with
 * the given handle.
 */
char *
_LoaderModuleToName(int module)
{
    loaderPtr item = listHead;
    loaderPtr aritem = NULL;
    loaderPtr lastitem = NULL;

    if (module < 0) {
	return "(built-in)";
    }
    while (item) {
	if (item->module == module) {
	    if (strchr(item->name, ':') == NULL)
		aritem = item;
	    else
		lastitem = item;
	}
	item = item->next;
    }

    if (aritem)
	return aritem->name;

    if (lastitem)
	return lastitem->name;

    return 0;
}

/*
 * Platform-specific method for finding if an address on the stack is a signal
 * trampoline.
 */

#if defined(__FreeBSD__) || (defined(__NetBSD__) && !defined(__ELF__))
/* For FreeBSD 3.0 and later and NetBSD/a.out.  */
#define SIGTRAMP_START	0xbfbfdf20UL
#define SIGTRAMP_END	0xbfbfdff0UL
#endif

static int
inSigTramp(unsigned long addr)
{
#if defined(SIGTRAMP_START) && defined(SIGTRAMP_END)
    if (addr >= SIGTRAMP_START && addr <= SIGTRAMP_END)
	return 1;
#endif
    return 0;
}

const char *
_LoaderAddressToSymbol(unsigned long address, unsigned long *symaddr,
		       const char **filename)
{
    loaderPtr item = listHead;
    const char *sym = NULL;

    while (item) {
	sym = item->desc->AddressToSymbol(item->private, address, symaddr,
					  filename, 0);
	if (sym)
	    return sym;
	item = item->next;
    }
    if (mainExeItem) {
	sym = mainExeItem->desc->AddressToSymbol(mainExeItem->private,
						 address, symaddr, filename, 1);
	if (!*filename)
	    *filename = GetExePath(NULL);
	if (sym)
	    return sym;

	if (mainExeDlType >= 0) {
	    sym = ldesc[mainExeDlType]->AddressToSymbol(mainExeItem->private,
						        address, symaddr,
						        filename, 1);
	}
	if (!*filename)
	    *filename = GetExePath(NULL);
	if (sym)
	    return sym;
    }
    if (!sym) {
	/* Check for a signal handler trampoline. */
	if (inSigTramp(address)) {
	    *symaddr = 0;
	    return "<signal handler>";
	}
    }
    return NULL;
}

/*
 * _LoaderAddressToSection() will return the name of the file & section
 * that contains the given address.
 */
int
_LoaderAddressToSection(const unsigned long address, const char **module,
			const char **section)
{
    loaderPtr item = listHead;

    while (item) {
	if ((*section =
	     item->desc->AddressToSection(item->private, address)) != NULL) {
	    *module = _LoaderModuleToName(item->module);
	    return 1;
	}
	item = item->next;
    }
    if (mainExeItem) {
	*section = mainExeItem->desc->AddressToSection(mainExeItem->private,
							address);
	if (!*section)
	    *section = "<cannot-find>";
	*module = GetExePath(NULL);
	return 1;
    }
    return 0;
}

#define MODNAME(m) ((m) ? (m)->name : "<global>")

static symlistPtr
GetSymbolList(symlistPtr list, ModuleDescPtr module, int create)
{
    symlistPtr p;

    for (p = list; p; p = p->next) {
	if (module && p->module == module)
	    break;
    }
#if LOADERDEBUG
    LoaderDebugMsg(LOADER_DEBUG_REQ_REF,
		   "GetSymbolList for %p (%s): list is %p, p is %p\n",
		   module, MODNAME(module), list, p);
#endif
    if (!p && create) {
	p = xnfcalloc(1, sizeof(symlist));
	p->next = list->next;
	p->module = module;
	list->next = p;
    }
#if LOADERDEBUG
    LoaderDebugMsg(LOADER_DEBUG_REQ_REF,
		   "GetSymbolList for module %p (%s) returns %p\n",
		   module, MODNAME(module), p);
#endif
    return p;
}

static symlistPtr
GetNextSymbolListByName(symlistPtr list, const char *name)
{
    symlistPtr p;

    if (!list || !name)
	return NULL;

    for (p = list->next; p; p = p->next) {
	if (p->module  && p->module->name && !strcmp(p->module->name, name)) {
#if LOADERDEBUG
	    LoaderDebugMsg(LOADER_DEBUG_REQ_REF,
			   "GetNextSymbolListByName for module %s returns %p\n",
			   name, p);
#endif
	    return p;
	}
    }
    return NULL;
}

static void
RemoveSymbolList(symlistPtr list, ModuleDescPtr module)
{
    symlistPtr p, q;

    p = GetSymbolList(list, module, 0);
#if LOADERDEBUG
    LoaderDebugMsg(LOADER_DEBUG_REQ_REF,
		   "RemoveSymbolList for %p (%s) list %p, p %p\n",
		   module, MODNAME(module), list, p);
#endif
    if (p) {
	if (p->list)
	    xfree(p->list);
	p->num = 0;
	for (q = list; q && q->next != p; q = q->next)
	    ;
	if (!q)
	    ErrorF("RemoveSymbolList: internal error\n");
	else {
	    q->next = p->next;
	    xfree(p);
	}
    }
}

/*
 * Add a list of symbols to the referenced list.
 */

static void
AppendSymbol(symlist *list, const char *sym)
{
    list->list = xnfrealloc(list->list, (list->num + 1) * sizeof(char **));
    list->list[list->num] = xnfstrdup(sym);
    list->num++;
#if LOADERDEBUG
    LoaderDebugMsg(LOADER_DEBUG_REQ_REF, "AppendSymbol for %s: %s\n",
		   MODNAME(list->module), sym);
#endif
}

static void
AppendSymList(symlist *list, const char **syms)
{
    while (*syms) {
	AppendSymbol(list, *syms);
	syms++;
    }
}

static int
SymInList(symlist *list, const char *sym)
{
    int i;

    for (i = 0; i < list->num; i++)
	if (strcmp(list->list[i], sym) == 0)
	    return 1;

    return 0;
}

static void
DuplicateSymbolList(symlistPtr list, ModuleDescPtr old, ModuleDescPtr new)
{
    symlistPtr oldList, newList = NULL;
    int i;

    oldList = GetSymbolList(list, old, 0);
    if (oldList) {
	newList = GetSymbolList(list, new, 1);
	if (newList)
	    for (i = 0; i < oldList->num; i++)
		AppendSymbol(newList, oldList->list[i]);
    }
}

void
DuplicateSymbolLists(ModuleDescPtr old, ModuleDescPtr new)
{
    DuplicateSymbolList(&refList, old, new);
    DuplicateSymbolList(&reqList, old, new);
}

void
LoaderVRefSymbols(ModuleDescPtr module, const char *sym0, va_list args)
{
    const char *s;
    symlistPtr list;

    if (sym0 == NULL)
	return;

#if LOADERDEBUG
    LoaderDebugMsg(LOADER_DEBUG_REQ_REF,
		"LoaderVRefSymbols: module %p, %s\n", module, MODNAME(module));
#endif
    if (module)
	list = GetSymbolList(&refList, module, 1);
    else
	list = &refList;

    s = sym0;
    do {
	AppendSymbol(list, s);
	s = va_arg(args, const char *);
    } while (s != NULL);
}

void
LoaderModRefSymbols(ModuleDescPtr module, const char *sym0, ...)
{
    va_list ap;

    va_start(ap, sym0);
    LoaderVRefSymbols(module, sym0, ap);
    va_end(ap);
}

void
LoaderRefSymbols(const char *sym0, ...)
{
    va_list ap;

    va_start(ap, sym0);
    LoaderVRefSymbols(NULL, sym0, ap);
    va_end(ap);
}

void
LoaderVRefSymLists(ModuleDescPtr module, const char **list0, va_list args)
{
    const char **l;
    symlistPtr list;

#if LOADERDEBUG
    LoaderDebugMsg(LOADER_DEBUG_REQ_REF,
		"LoaderVRefSymLists: module %p, %s\n", module, MODNAME(module));
#endif
    if (list0 == NULL)
	return;

    l = list0;
    if (module)
	list = GetSymbolList(&refList, module, 1);
    else
	list = &refList;
    do {
	AppendSymList(list, l);
	l = va_arg(args, const char **);
    } while (l != NULL);
}

void
LoaderModRefSymLists(ModuleDescPtr module, const char **list0, ...)
{
    va_list ap;

    va_start(ap, list0);
    LoaderVRefSymLists(module, list0, ap);
    va_end(ap);
}

void
LoaderRefSymLists(const char **list0, ...)
{
    va_list ap;

    va_start(ap, list0);
    LoaderVRefSymLists(NULL, list0, ap);
    va_end(ap);
}

int
LoaderVReqSymLists(ModuleDescPtr module, const char **list0, va_list args)
{
    const char **l, **s;
    symlistPtr list;
    itemPtr sym;
    int numUnresolved = 0;

#if LOADERDEBUG
    LoaderDebugMsg(LOADER_DEBUG_REQ_REF,
		"LoaderVReqSymLists: module %p, %s\n",
		module, MODNAME(module));
#endif
    if (list0 == NULL)
	return 0;

    l = list0;
    if (module)
	list = GetSymbolList(&reqList, module, 1);
    else
	list = &reqList;
    do {
	AppendSymList(list, l);
	for (s = l; *s; s++) {
	    sym = LoaderHashFind(*s);
	    if (!sym) {
		xf86Msg(X_WARNING, "Symbol \"%s\" is not provided by "
			"module \"%s\"\n", *s, MODNAME(module));
		numUnresolved++;
	    } else if (module && sym->handle != module->handle) {
		xf86Msg(X_WARNING, "Symbol \"%s\" is provided by a "
			"module (\"%s\") other than \"%s\"\n", *s,
			_LoaderHandleToCanonicalName(sym->handle),
			MODNAME(module));
		numUnresolved++;
	    }
	}
	l = va_arg(args, const char **);
    } while (l);
    return numUnresolved;
}

int
LoaderModReqSymLists(ModuleDescPtr module, const char **list0, ...)
{
    va_list ap;
    int ret;

    va_start(ap, list0);
    ret = LoaderVReqSymLists(module, list0, ap);
    va_end(ap);
    return ret;
}

void
LoaderReqSymLists(const char **list0, ...)
{
    va_list ap;

    va_start(ap, list0);
    LoaderVReqSymLists(NULL, list0, ap);
    va_end(ap);
}

int
LoaderVReqSymbols(ModuleDescPtr module, const char *sym0, va_list args)
{
    const char *s;
    symlistPtr list;
    itemPtr sym;
    int numUnresolved = 0;

    if (sym0 == NULL)
	return 0;

#if LOADERDEBUG
    LoaderDebugMsg(LOADER_DEBUG_REQ_REF,
		"LoaderVRefSymbols: module %p, %s\n",
		module, MODNAME(module));
#endif
    s = sym0;
    if (module)
	list = GetSymbolList(&reqList, module, 1);
    else
	list = &reqList;
    do {
	AppendSymbol(list, s);
	sym = LoaderHashFind(s);
	if (!sym) {
	    xf86Msg(X_WARNING, "Symbol \"%s\" is not provided by "
		    "module \"%s\"\n", s, MODNAME(module));
	    numUnresolved++;
	} else if (module && sym->handle != module->handle) {
	    xf86Msg(X_WARNING, "Symbol \"%s\" is provided by a "
		    "module (\"%s\") other than \"%s\"\n", s,
		    _LoaderHandleToCanonicalName(sym->handle),
		    MODNAME(module));
	    numUnresolved++;
	}
	s = va_arg(args, const char *);
    } while (s);
    return numUnresolved;
}

int
LoaderModReqSymbols(ModuleDescPtr module, const char *sym0, ...)
{
    va_list ap;
    int ret;

    va_start(ap, sym0);
    ret = LoaderVReqSymbols(module, sym0, ap);
    va_end(ap);
    return ret;
}

void
LoaderReqSymbols(const char *sym0, ...)
{
    va_list ap;

    va_start(ap, sym0);
    LoaderVReqSymbols(NULL, sym0, ap);
    va_end(ap);
}

/* 
 * _LoaderHandleUnresolved() decides what to do with an unresolved
 * symbol.  Symbols that are not on the "referenced" or "required" lists
 * get a warning if they are unresolved.  Symbols that are on the "required"
 * list generate a fatal error if they are unresolved.
 */

int
_LoaderHandleUnresolved(const char *symbol, int handle)
{
    int fatalsym = 0;
    int refsym = 0;
    symlistPtr list;
    const char *name, *cname;

    cname = _LoaderHandleToCanonicalName(handle);
    name = _LoaderHandleToName(handle);
    if (xf86ShowUnresolved) {
	int useGlobal = 1;
	for (list = &reqList; list; list = list->next) {
	    if (SymInList(list, symbol)) {
		fatalsym = 1;
		xf86Msg(X_ERROR,
			"Required symbol %s from module %s is unresolved!\n",
			symbol, name);
	        break;
	    }
	}
	list = &refList;
	while ((list = GetNextSymbolListByName(list, cname))) {
	    useGlobal = 0;
	    if (SymInList(list, symbol))
		refsym = 1;
	}
	if (useGlobal && SymInList(&refList, symbol))
	    refsym = 1;
	if (!refsym)
	    xf86MsgVerb(X_WARNING, 0,
			"Symbol %s from module %s is unresolved!\n",
			symbol, name);
    }

    if (fatalsym)
	fatalReqSym = 1;

    return (fatalsym || !refsym);
}

/*
 * Handle an archive.
 */
static void *
ARCHIVELoadModule(loaderPtr modrec, int arfd, LOOKUP ** ppLookup)
{
    loaderPtr tmp = NULL;
    void *ret = NULL;
    unsigned char magic[SARMAG];
    struct ar_hdr hdr;

#if defined(__powerpc__) && defined(Lynx)
    struct fl_hdr fhdr;
    char name[255];
    int namlen;
#endif
    unsigned int size;
    unsigned int offset = 0;
    int arnamesize, modnamesize;
    char *slash, *longname;
    char *nametable = NULL;
    int nametablelen = 0;
    LOOKUP *lookup_ret, *p;
    LOOKUP *myLookup = NULL;	/* Does realloc behave if ptr == 0? */
    int modtype;
    int i;
    int numsyms = 0;

    arnamesize = strlen(modrec->name);

#if !(defined(__powerpc__) && defined(Lynx))
    read(arfd, magic, SARMAG);

    if (strncmp((const char *)magic, ARMAG, SARMAG) != 0) {
	ErrorF("ARCHIVELoadModule: wrong magic!!\n");
	return NULL;
    }
#else
    read(arfd, &fhdr, FL_HSZ);

    if (strncmp(fhdr.fl_magic, AIAMAG, SAIAMAG) != 0) {
	ErrorF("ARCHIVELoadModule: wrong magic!!\n");
	return NULL;
    }
#endif /* __powerpc__ && Lynx */

#if LOADERDEBUG
    LoaderDebugMsg(LOADER_DEBUG_ARCHIVE,
		   "Looking for archive members starting at offset %o\n",
		   offset);
#endif

    while (read(arfd, &hdr, sizeof(struct ar_hdr))) {

	longname = NULL;
	sscanf(hdr.ar_size, "%u", &size);
#if defined(__powerpc__) && defined(Lynx)
	sscanf(hdr.ar_namlen, "%d", &namlen);
	name[0] = hdr.ar_name[0];
	name[1] = hdr.ar_name[1];
	read(arfd, &name[2], namlen);
	name[namlen] = '\0';
	offset = lseek(arfd, 0, SEEK_CUR);
	if (offset & 0x1)	/* odd value */
	    offset = lseek(arfd, 1, SEEK_CUR);	/* make it an even boundary */
#endif
	offset = lseek(arfd, 0, SEEK_CUR);

	/* Check for a Symbol Table */
	if ((hdr.ar_name[0] == '/' && hdr.ar_name[1] == ' ') ||
#if defined(__powerpc__) && defined(Lynx)
	    namlen == 0 ||
#endif
	    strncmp(hdr.ar_name, "__.SYMDEF", 9) == 0) {
	    /* If the file name is NULL, then it is a symbol table */
#if LOADERDEBUG
	    LoaderDebugMsg(LOADER_DEBUG_ARCHIVE,
			   "Symbol Table Member '%16.16s', size %d, "
			   "offset %d\n", hdr.ar_name, size, offset);
	    LoaderDebugMsg(LOADER_DEBUG_ARCHIVE,
			   "Symbol table size %d\n", size);
#endif
	    offset = lseek(arfd, offset + size, SEEK_SET);
	    if (offset & 0x1)	/* odd value */
		offset = lseek(arfd, 1, SEEK_CUR);	/* make it an even boundary */
	    continue;
	}

	/* Check for a String Table */
	if (hdr.ar_name[0] == '/' && hdr.ar_name[1] == '/') {
	    /* If the file name is '/', then it is a string table */
#if LOADERDEBUG
	    LoaderDebugMsg(LOADER_DEBUG_ARCHIVE,
			   "String Table Member '%16.16s', size %d, "
			   "offset %d\n", hdr.ar_name, size, offset);
	    LoaderDebugMsg(LOADER_DEBUG_ARCHIVE,
			   "String table size %d\n", size);
#endif
	    nametablelen = size;
	    nametable = (char *)xf86loadermalloc(nametablelen);
	    read(arfd, nametable, size);
	    offset = lseek(arfd, 0, SEEK_CUR);
	    /* offset=lseek(arfd,offset+size,SEEK_SET); */
	    if (offset & 0x1)	/* odd value */
		offset = lseek(arfd, 1, SEEK_CUR);	/* make it an even boundary */
	    continue;
	}

	if (hdr.ar_name[0] == '/') {
	    /*  SYS V r4 style long member name */
	    int nameoffset = atol(&hdr.ar_name[1]);
	    char *membername;

	    if (!nametable) {
		ErrorF("Missing string table whilst processing %s\n",
		       modrec->name);
		offsetbias = 0;
		return NULL;
	    }
	    if (nameoffset > nametablelen) {
		ErrorF("Invalid string table offset (%s) whilst processing %s\n", hdr.ar_name, modrec->name);
		offsetbias = 0;
		xf86loaderfree(nametable);
		return NULL;
	    }
	    membername = nametable + nameoffset;
	    slash = strchr(membername, '/');
	    if (slash)
		*slash = '\0';
	    longname = xf86loadermalloc(arnamesize + strlen(membername) + 2);
	    strcpy(longname, modrec->name);
	    strcat(longname, ":");
	    strcat(longname, membername);
	} else if (hdr.ar_name[0] == '#' && hdr.ar_name[1] == '1' &&
		   hdr.ar_name[2] == '/') {
	    /* BSD 4.4 style long member name */
	    if (sscanf(hdr.ar_name + 3, "%d", &modnamesize) != 1) {
		ErrorF("Bad archive member %s\n", hdr.ar_name);
		offsetbias = 0;
		return NULL;
	    }
	    /* allocate space for fully qualified name */
	    longname = xf86loadermalloc(arnamesize + modnamesize + 2);
	    strcpy(longname, modrec->name);
	    strcat(longname, ":");
	    i = read(arfd, longname + modnamesize + 1, modnamesize);
	    if (i != modnamesize) {
		ErrorF("Bad archive member %s\n", hdr.ar_name);
		xf86loaderfree(longname);
		offsetbias = 0;
		return NULL;
	    }
	    longname[i] = '\0';
	    offset += i;
	    size -= i;
	} else {
	    /* Regular archive member */
#if LOADERDEBUG
	    LoaderDebugMsg(LOADER_DEBUG_ARCHIVE,
			   "Member '%16.16s', size %d, offset %x\n",
#if !(defined(__powerpc__) && defined(Lynx))
		   hdr.ar_name,
#else
		   name,
#endif
		   size, offset);
#endif

	    slash = strchr(hdr.ar_name, '/');
	    if (slash == NULL) {
		/* BSD format without trailing slash */
		slash = strchr(hdr.ar_name, ' ');
	    }
	    /* SM: Make sure we do not overwrite other parts of struct */

	    if ((slash - hdr.ar_name) > sizeof(hdr.ar_name))
		slash = hdr.ar_name + sizeof(hdr.ar_name) - 1;
	    *slash = '\000';
	}
	if ((modtype = _GetModuleType(arfd, offset)) < 0) {
	    ErrorF("%s is an unrecognized module type\n", hdr.ar_name);
	    offsetbias = 0;
	    if (nametable)
		xf86loaderfree(nametable);
	    return NULL;
	} else {
#if LOADERDEBUG
	    LoaderDebugMsg(LOADER_DEBUG_FILES, "Module %s is type %d (%s)\n",
			   hdr.ar_name, modtype, loaderNames[modtype]);
#endif
	}

	tmp = _LoaderListPush();

	tmp->handle = modrec->handle;
	tmp->module = moduleseq++;
	tmp->cname = xf86loadermalloc(strlen(modrec->cname) + 1);
	strcpy(tmp->cname, modrec->cname);
	tmp->desc = ldesc[modtype];
	if (longname == NULL) {
	    modnamesize = strlen(hdr.ar_name);
	    tmp->name =
		    (char *)xf86loadermalloc(arnamesize + modnamesize + 2);
	    strcpy(tmp->name, modrec->name);
	    strcat(tmp->name, ":");
	    strcat(tmp->name, hdr.ar_name);

	} else {
	    tmp->name = longname;
	}
	offsetbias = offset;

	if ((tmp->private = ldesc[modtype]->LoadModule(tmp, arfd, &lookup_ret))
	    == NULL) {
	    ErrorF("Failed to load %s\n", hdr.ar_name);
	    offsetbias = 0;
	    if (nametable)
		xf86loaderfree(nametable);
	    return NULL;
	}

	offset = lseek(arfd, offset + size, SEEK_SET);
	if (offset & 0x1)	/* odd value */
	    lseek(arfd, 1, SEEK_CUR);	/* make it an even boundary */

	if (tmp->private == (void *)-1L) {
	    ErrorF("Skipping \"%s\":  No symbols found\n", tmp->name);
	    continue;
	} else
	    ret = tmp->private;

	/* Add the lookup table returned from desc->LoadModule to the
	 * one we're going to return.
	 */
	for (i = 0, p = lookup_ret; p && p->symName; i++, p++) ;
	if (i) {
	    myLookup = xf86loaderrealloc(myLookup, (numsyms + i + 1)
					 * sizeof(LOOKUP));
	    if (!myLookup)
		continue;	/* Oh well! */

	    memcpy(&(myLookup[numsyms]), lookup_ret, i * sizeof(LOOKUP));
	    numsyms += i;
	    myLookup[numsyms].symName = 0;
	}
	xf86loaderfree(lookup_ret);
    }
    /* xf86loaderfree(lookup_ret); */
    offsetbias = 0;

    *ppLookup = myLookup;
    if (nametable)
	xf86loaderfree(nametable);

    return ret;
}

/*
 * Relocation list manipulation routines
 */

/*
 * _LoaderGetRelocations() Return a pointer to the list of outstanding
 * relocations.
 */
void **
_LoaderGetRelocations(LoaderDescPtr mod)
{
    return &(mod->pRelocs);
}

static int
MatchScopeSelf(void *dummy, itemPtr entry)
{
    return (entry->scope == LOOKUP_SCOPE_SELF);
}

/*
 * Public Interface to the loader.
 */

int
LoaderOpen(const char *module, const char *cname, int handle,
	   int *errmaj, int *errmin, int *wasLoaded, char **modData)
{
    loaderPtr tmp;
    int new_handle, modtype;
    int fd;
    LOOKUP *pLookup;

#if LOADERDEBUG
    LoaderDebugMsg(LOADER_DEBUG_FILES, "LoaderOpen(%s)\n", module);
#endif

    /*
     * Check to see if the module is already loaded.
     * Only if we are loading it into an existing namespace.
     * If it is to be loaded into a new namespace, don't check.
     * Note: We only have one namespace.
     */
    if (handle >= 0) {
	tmp = listHead;
	while (tmp) {
#if LOADERDEBUG
	    LoaderDebugMsg(LOADER_DEBUG_FILES, "strcmp(%p(%s),{%p} %p(%s))\n",
			   module, module, &(tmp->name), tmp->name, tmp->name);
#endif
	    if (!strcmp(module, tmp->name)) {
		refCount[tmp->handle]++;
		if (wasLoaded)
		    *wasLoaded = 1;
		xf86MsgVerb(X_INFO, 2, "Reloading %s\n", module);
		*modData = tmp->modData;
		return tmp->handle;
	    }
	    tmp = tmp->next;
	}
    }

    /*
     * OK, it's a new one. Add it.
     */
    xf86Msg(X_INFO, "Loading %s\n", module);
    if (wasLoaded)
	*wasLoaded = 0;

    /*
     * Find a free handle.
     */
    new_handle = 1;
    while (freeHandles[new_handle] && new_handle < MAX_HANDLE)
	new_handle++;

    if (new_handle == MAX_HANDLE) {
	xf86Msg(X_ERROR, "Out of loader space\n");	/* XXX */
	if (errmaj)
	    *errmaj = LDR_NOSPACE;
	if (errmin)
	    *errmin = LDR_NOSPACE;
	return -1;
    }

    freeHandles[new_handle] = HANDLE_USED;
    refCount[new_handle] = 1;

    if ((fd = open(module, O_RDONLY)) < 0) {
	xf86Msg(X_ERROR, "Unable to open %s\n", module);
	freeHandles[new_handle] = HANDLE_FREE;
	if (errmaj)
	    *errmaj = LDR_NOMODOPEN;
	if (errmin)
	    *errmin = errno;
	return -1;
    }

    if ((modtype = _GetModuleType(fd, 0)) < 0) {
	xf86Msg(X_ERROR, "%s is an unrecognized module type\n", module);
	freeHandles[new_handle] = HANDLE_FREE;
	if (errmaj)
	    *errmaj = LDR_UNKTYPE;
	if (errmin)
	    *errmin = LDR_UNKTYPE;
	close(fd);
	return -1;
    }

#if LOADERDEBUG
    LoaderDebugMsg(LOADER_DEBUG_FILES, "Module %s is type %d (%s)\n",
		   module, modtype, loaderNames[modtype]);
#endif

    tmp = _LoaderListPush();
    tmp->name = xf86loadermalloc(strlen(module) + 1);
    strcpy(tmp->name, module);
    tmp->cname = xf86loadermalloc(strlen(cname) + 1);
    strcpy(tmp->cname, cname);
    tmp->handle = new_handle;
    tmp->module = moduleseq++;
    tmp->desc = ldesc[modtype];

    if (!(tmp->private = ldesc[modtype]->LoadModule(tmp, fd, &pLookup))) {
	xf86Msg(X_ERROR, "Failed to load %s\n", module);
	_LoaderListPop(new_handle);
	freeHandles[new_handle] = HANDLE_FREE;
	if (errmaj)
	    *errmaj = LDR_NOLOAD;
	if (errmin)
	    *errmin = LDR_NOLOAD;
	close(fd);
	return -1;
    }

    if (tmp->private != (void *)-1L) {
	LoaderAddSymbols(new_handle, tmp->module, pLookup, LOOKUP_SCOPE_AUTO,
			 modData);
	xf86loaderfree(pLookup);
    }
    tmp->modData = *modData;

    close(fd);

    LoaderResolveSymbols(-1);

    /*
     * Remove symbols from the lookup table that have LOOKUP_SCOPE_SELF.
     * These can never be used when resolving other modules.
     */
    LoaderHashTraverse(NULL, MatchScopeSelf);

    return new_handle;
}

int
LoaderHandleOpen(int handle)
{
    if (handle < 0 || handle >= MAX_HANDLE)
	return -1;

    if (freeHandles[handle] != HANDLE_USED)
	return -1;

    refCount[handle]++;
    return handle;
}

void *
LoaderSymbolWithScope(const char *sym, int handle, unsigned long scope)
{
    itemPtr item = NULL;

    item = (itemPtr) LoaderHashFind(sym);

    if (item && SCOPE_OK(item, handle, scope))
	return item->address;
    else if (!item) {
#ifdef DLOPEN_SUPPORT
	return (DLFindSymbol(sym));
#endif
    }
    return NULL;
}

void *
LoaderSymbol(const char *sym)
{
    return LoaderSymbolWithScope(sym, -1, LOOKUP_SCOPE_GLOBAL);
}

int
LoaderResolveSymbols(int handle)
{
    int i;

    for (i = 0; i < numloaders; i++)
	ldesc[i]->ResolveSymbols(ldesc[i], handle);
    return 0;
}

int
LoaderCheckUnresolved(int dummy)
{
    int i, ret = 0;

    for (i = 0; i < numloaders; i++)
	if (ldesc[i]->CheckForUnresolved(ldesc[i]))
	    ret = 1;

    /* This does nothing unless debugging is enabled. */
    LoaderDumpHashHits();

    if (fatalReqSym)
	FatalError("Some required symbols were unresolved.\n");

    return ret;
}

const char *
LoaderFindRelocName(int handle, unsigned long addr)
{
    int i;
    const char *ret;

    for (i = 0; i < numloaders; i++) {
	ret = ldesc[i]->FindRelocName(ldesc[i], handle, addr);
	if (ret)
	    return ret;
    }
    return NULL;
}

void xf86LoaderTrap(void);

void
xf86LoaderTrap(void)
{
}

#ifdef STACKTRACE

#ifndef STACK_LEVELS
#define STACK_LEVELS 16
#endif

void
LoaderDefaultFunc(void)
{
    unsigned long returnStack[STACK_LEVELS];
    int i;
    const char *rname = NULL;

    getStackTrace(returnStack, STACK_LEVELS);
    if (returnStack[1])
	    rname = LoaderFindRelocName(-1, returnStack[1]);
    if (rname)
	    ErrorF("*** Unresolved function \"%s\" called ***\n", rname);
    else
	    ErrorF("*** Unresolved function (unknown name) called ***\n");

    ErrorF("Stack trace:\n");
    for (i = 1; i < STACK_LEVELS && returnStack[i]; i++) {
	    ErrorF("%2d: 0x%lx: ", i, returnStack[i]);
	    if (!LoaderPrintSymbol(returnStack[i]))
		break;
    }
    xf86LoaderTrap();
#if LOADER_UNRESOLVED_IS_FATAL
    FatalError("Aborting because of unresolved function call.\n");
#endif
}

#else

void
LoaderDefaultFunc(void)
{
    ErrorF("*** Unresolved function called ***\n");
    xf86LoaderTrap();
#if LOADER_UNRESOLVED_IS_FATAL
    FatalError("Aborting because of unresolved function call.\n");
#endif
}

#endif
    
int
LoaderUnload(ModuleDescPtr mod)
{
    int handle;
    loaderRec fakeHead;
    loaderPtr tmp = &fakeHead;

    if (!mod)
	return -1;

    handle = mod->handle;
    if (handle < 0 || handle > MAX_HANDLE)
	return -1;

    RemoveSymbolList(&refList, mod);
    RemoveSymbolList(&reqList, mod);
    
    /*
     * check the reference count, only free it if it goes to zero
     */
    if (--refCount[handle])
	return 0;
    /*
     * find the loaderRecs associated with this handle.
     */

    while ((tmp = _LoaderListPop(handle)) != NULL) {
	if (strchr(tmp->name, ':') == NULL) {
	    /* It is not a member of an archive */
	    xf86Msg(X_INFO, "Unloading %s\n", tmp->name);
	}
	tmp->desc->LoaderUnload(tmp->private);
	xf86loaderfree(tmp->name);
	xf86loaderfree(tmp->cname);
	xf86loaderfree(tmp);
    }

    freeHandles[handle] = HANDLE_FREE;

    LoaderResolveSymbols(-1);

    return 0;
}

void
LoaderDuplicateSymbol(const char *symbol, const int handle)
{
    ErrorF("Duplicate symbol %s in %s\n", symbol,
	   listHead ? listHead->name : "(built-in)");
    ErrorF("Also defined in %s\n", _LoaderHandleToName(handle));
    FatalError("Module load failure\n");
}

/* GDB Sync function */
void
_loader_debug_state()
{
}

unsigned long LoaderOptions = 0;

void
LoaderResetOptions(void)
{
    LoaderOptions = 0;
}

void
LoaderSetOptions(unsigned long opts)
{
    LoaderOptions |= opts;
}

void
LoaderClearOptions(unsigned long opts)
{
    LoaderOptions &= ~opts;
}

unsigned long LoaderDebugLevel = 0;
static unsigned long LoaderSaveDebugLevel = 0;
static const char **LoaderDebugList = NULL;
static int numLoaderDebugList = 0;

void
LoaderSetDebug(unsigned long level)
{
    LoaderSaveDebugLevel = LoaderDebugLevel = level;
}

void
LoaderDebugMsg(unsigned long debug, const char *f, ...)
{
    va_list args;

    va_start(args, f);
    if (debug & LoaderDebugLevel)
	VErrorF(f, args);
    va_end(args);
}

static int
LoaderDebugModuleInList(const char *module)
{
    int i;

    /* If no list, debugging applies to all modules. */
    if (!LoaderDebugList || !numLoaderDebugList)
	return 1;

    for (i = 0; i < numLoaderDebugList; i++)
	if (LoaderDebugList[i] && strcmp(module, LoaderDebugList[i]) == 0)
	    return 1;
    return 0;
}

void
LoaderDebugAddModule(const char *module)
{
    const char **l;

    if (LoaderDebugList && LoaderDebugModuleInList(module))
	return;

    numLoaderDebugList++;
    l = xrealloc(LoaderDebugList,
		 numLoaderDebugList * sizeof(*LoaderDebugList));
    if (l) {
	l[numLoaderDebugList - 1] = module;
	LoaderDebugList = l;
    }
}

void
LoaderDebugForModule(const char *module)
{
    if (LoaderDebugModuleInList(module))
	LoaderDebugLevel = LoaderSaveDebugLevel;
    else
	LoaderDebugLevel = 0;
}

/*
 * The return value is the user-recognisable file name, and *path is set
 * to the name of a full path that can be used to open the executable file
 * image.
 */
static const char *
GetExePath(const char **path)
{
#ifdef HAVE_PROCFS
    unsigned int pid;
    char *procPath;
#ifdef HAVE_PROCFS_PSINFO
    char *procName;
#endif
    struct stat sb;
#endif
    const char *reason = NULL;

    if (exePath && exeName) {
	if (path)
	    *path = exePath;
	return exeName;
    }

#ifdef HAVE_PROCFS
    pid = getpid();
    xasprintf(&procPath, "/proc/%d/" PROCFS_EXE_NAME, pid);
#ifdef HAVE_PROCFS_PSINFO
    xasprintf(&procName, "/proc/%d/psinfo", pid);
#endif

    if (procPath) {
	if (stat(procPath, &sb) == -1)
	    reason = "The /proc filesystem is not mounted.";
	else {
#if defined(PROCFS_EXE_SYMLINK)
	    exePath = xnfcalloc(PATH_MAX + 1, 1);
	    if (readlink(procPath, exePath, PATH_MAX) == -1) {
		reason = "readlink failed on /proc entry.";
		xfree(exePath);
		exePath = NULL;
	    } else
		exeName = exePath;
#elif defined(HAVE_PROCFS_PSINFO)
	    if (procName) {
		struct psinfo ps;
		int fd;

		fd = open(procName, O_RDONLY);
		if (fd < 0) {
		    xf86Msg(X_WARNING, "Cannot open \"%s\"\n", procName);
		    reason = "failed to open /proc psinfo entry.";
		} else {
		    if (read(fd, &ps, sizeof(ps)) != sizeof(ps)) {
			xf86Msg(X_WARNING, "Cannot read \"%s\"\n", procName);
			reason = "failed to read /proc psinfo entry. ";
		    } else {
			exeName = xnfstrdup(ps.pr_fname);
#ifdef PROCFS_PSINFO_FULLPATH
			exePath = exeName;
#else
			exePath = xnfstrdup(procPath);
#endif
		    }
		    close(fd);
		}
	    }
#endif
	    if (!exePath)
		exePath = xnfstrdup(procPath);
	    if (!exeName)
		exeName = xnfstrdup(getArgv(0));

	    if (exePath && exeName) {
		if (exePath != exeName)
		    xf86Msg(X_INFO, "Executable is \"%s\", path \"%s\".\n",
			    exeName, exePath);
		else
		    xf86Msg(X_INFO, "Executable is \"%s\".\n", exeName);
		if (path)
		    *path = exePath;
		return exeName;
	    }
	}
    }
#else
    reason = "there is no support for finding it on this platform.";
#endif

    xf86Msg(X_WARNING, "Cannot find the executable path name.\n");
    if (reason)
	xf86Msg(X_NONE, "\t%s\n", reason);
    return NULL;
}

static void
ReadMainExe(void)
{
    const char *fileName = NULL;
    int fd;
    int modtype;

    GetExePath(&fileName);
    if (!fileName) {
	xf86Msg(X_WARNING, "Cannot identify the executable file.\n");
	return;
    }
    xf86Msg(X_INFO, "Reading symbols from \"%s\".\n", fileName);
    fd = open(fileName, O_RDONLY);
    if (fd < 0) {
	xf86Msg(X_WARNING,
		"Cannot open executable file \"%s\" (%s).\n", fileName,
		strerror(errno));
	return;
    }

    if ((modtype = _GetModuleType(fd, 0)) < 0) {
	xf86Msg(X_WARNING, "Executable file type is not recognized.\n");
	close(fd);
	return;
    }

#ifdef HAVE_DLADDR
    /*
     * Use dladdr(3), if available, to find symbols that are neither in
     * modules or the main executable -- e.g., in shared libraries.
     */
    switch (modtype) {
    case LD_ELFOBJECT:
	mainExeDlType = LD_ELFDLOBJECT;
	break;
    case LD_AOUTOBJECT:
	mainExeDlType = LD_AOUTDLOBJECT;
	break;
    default:
	break;
    }
#endif
    mainExeItem = xnfcalloc(1, sizeof(*mainExeItem));
    mainExeItem->desc = ldesc[modtype];
    mainExeItem->private = mainExeItem->desc->ReadExecutableSyms(fd);
    close(fd);
}

