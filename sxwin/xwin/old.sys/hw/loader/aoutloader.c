/* $XFree86: xc/programs/Xserver/hw/xfree86/loader/aoutloader.c,v 1.24 2006/04/03 18:08:03 dawes Exp $ */

/*
 *
 * Copyright (c) 1997 Matthieu Herrb
 * Copyright 1995-1998 Metro Link, Inc.
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
 *
 * Modified 21/02/97 by Sebastien Marineau to support OS/2 a.out objects
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

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef __QNX__
#include <fcntl.h>
#else
#include <sys/fcntl.h>
#endif
#include <sys/stat.h>
#include <netinet/in.h>

#ifdef DBMALLOC
#include <debug/malloc.h>
#define Xalloc(size) malloc(size)
#define Xcalloc(size) calloc(1,(size))
#define Xfree(size) free(size)
#endif

#include <X11/Xos.h>
#include "os.h"
#include "aout.h"
#include "sym.h"

#ifndef LOADERDEBUG
#define LOADERDEBUG 0
#endif
#include "loader.h"
#include "aoutloader.h"

#ifndef EXESYMDEBUG
#define EXESYMDEBUG 0 && LOADERDEBUG
#endif

#ifndef MIN
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif

/*
 * This structure contains all of the information about a module
 * that has been loaded.
 */

typedef struct {
    int handle;
    int module;
    int fd;
    LoaderDescPtr desc;
    AOUTHDR *header;		/* file header */
    unsigned char *text;	/* Start address of the text section */
    unsigned int textsize;	/* Size of the text section */
    unsigned char *data;	/* Start address of the data section */
    unsigned int datasize;	/* Size of the data section */
    unsigned char *bss;		/* Start address of the bss data */
    unsigned int bsssize;	/* Size of the bss section */
    struct relocation_info *txtrel;	/* Start address of the text relocation table */
    struct relocation_info *datarel;	/* Start address of the data relocation table */
    AOUT_nlist *symtab;		/* Start address of the symbol table */
    unsigned char *strings;	/* Start address of the string table */
    unsigned long strsize;	/* size of string table */
    unsigned char *common;	/* Start address of the common data */
    unsigned long comsize;	/* size of common data */
} AOUTModuleRec, *AOUTModulePtr;

/*
 * If an relocation is unable to be satisfied, then put it on a list
 * to try later after more modules have been loaded.
 */
typedef struct AOUT_RELOC {
    AOUTModulePtr file;
    struct relocation_info *rel;
    int type;			/* AOUT_TEXT or AOUT_DATA */
    int relocated;
    void *symval;
    int assigned;
    unsigned long olddata;
    struct AOUT_RELOC *next;
} AOUTRelocRec;

/*
 * Symbols with a section number of 0 (N_UNDF) but a value of non-zero
 * need to have space allocated for them.
 *
 * Gather all of these symbols together, and allocate one chunk when we
 * are done.
 */

typedef struct AOUT_COMMON {
    struct AOUT_nlist *sym;
    int index;
    struct AOUT_COMMON *next;
} AOUTCommonRec;

static AOUTCommonPtr listCOMMON = NULL;

/* prototypes for static functions */
static int AOUTHashCleanOut(void *, itemPtr);
static const char *AOUTGetSymbolName(AOUTModulePtr, struct AOUT_nlist *);
static void *AOUTGetSymbolValue(AOUTModulePtr, int, int *);
static AOUTCommonPtr AOUTAddCommon(struct AOUT_nlist *, int);
static LOOKUP *AOUTCreateCommon(AOUTModulePtr);
static LOOKUP *AOUT_GetSymbols(AOUTModulePtr);
static AOUTRelocPtr AOUTDelayRelocation(AOUTModulePtr, int,
					struct relocation_info_i386 *);
static AOUTRelocPtr AOUTCollectRelocations(AOUTModulePtr);
static void AOUT_Relocate(AOUTRelocPtr, unsigned long *, unsigned long, int);
static int AOUT_RelocateEntry(AOUTRelocPtr);

/*
 * Return 1 if the symbol in item belongs to aoutfile
 */
static int
AOUTHashCleanOut(void *voidptr, itemPtr item)
{
    AOUTModulePtr aoutfile = (AOUTModulePtr) voidptr;

    return (aoutfile->handle == item->handle);
}

/*
 * Manage listResolv 
 */
static AOUTRelocPtr
AOUTDelayRelocation(AOUTModulePtr aoutfile, int type,
		    struct relocation_info *rel)
{
    AOUTRelocPtr reloc;

    if ((reloc = xf86loadermalloc(sizeof(AOUTRelocRec))) == NULL) {
	ErrorF("AOUTDelayRelocation() Unable to allocate memory\n");
	return NULL;
    }
    if ((unsigned long)rel < 0x200) {
	ErrorF("bug");
    }
    reloc->file = aoutfile;
    reloc->type = type;
    reloc->rel = rel;
    reloc->relocated = 0;
    reloc->symval = NULL;
    reloc->assigned = 0;
    reloc->olddata = 0;
    reloc->next = NULL;
    return reloc;
}

/*
 * Manage listCOMMON
 */

static AOUTCommonPtr
AOUTAddCommon(struct AOUT_nlist *sym, int index)
{
    AOUTCommonPtr common;

    if ((common = xf86loadermalloc(sizeof(AOUTCommonRec))) == NULL) {
	ErrorF("AOUTAddCommon() Unable to allocate memory\n");
	return 0;
    }
    common->sym = sym;
    common->index = index;
    common->next = 0;
    return common;
}

static LOOKUP *
AOUTCreateCommon(AOUTModulePtr aoutfile)
{
    int numsyms = 0, size = 0, l = 0;
    int offset = 0;
    AOUTCommonPtr common;
    LOOKUP *lookup;

    if (listCOMMON == NULL)
	return NULL;

    common = listCOMMON;
    for (common = listCOMMON; common; common = common->next) {
	/* Ensure long word alignment */
	if ((common->sym->n_value & (sizeof(long) - 1)) != 0)
	    common->sym->n_value = (common->sym->n_value + (sizeof(long) - 1))
		    & ~(sizeof(long) - 1);

	/* accumulate the sizes */
	size += common->sym->n_value;
	numsyms++;
    }				/* while */

#if LOADERDEBUG
    LoaderDebugMsg(LOADER_DEBUG_LOWLEVEL,
		   "AOUTCreateCommon() %d entries (%d bytes) of COMMON data\n",
		   numsyms, size);
#endif

    if ((lookup = xf86loadermalloc((numsyms + 1) * sizeof(LOOKUP))) == NULL) {
	ErrorF("AOUTCreateCommon() Unable to allocate memory\n");
	return NULL;
    }

    aoutfile->comsize = size;
    if ((aoutfile->common = xf86loadercalloc(1, size)) == NULL) {
	ErrorF("AOUTCreateCommon() Unable to allocate memory\n");
	return NULL;
    }

    while (listCOMMON) {
	common = listCOMMON;
	lookup[l].symName = xf86loaderstrdup(AOUTGetSymbolName(aoutfile,
							       common->sym));
	lookup[l].offset = (funcptr) (aoutfile->common + offset);
#if LOADERDEBUG
	LoaderDebugMsg(LOADER_DEBUG_LOWLEVEL, "Adding %p %s\n",
		       (void *)lookup[l].offset, lookup[l].symName);
#endif
	listCOMMON = common->next;
	offset += common->sym->n_value;
	xf86loaderfree(common);
	l++;
    }				/* while */
    /* listCOMMON == NULL */

    lookup[l].symName = NULL;	/* Terminate the list */
    return lookup;
}

/*
 * Symbol Table
 */

static char *
AOUTGetString(AOUTModulePtr aoutfile, int index)
{
    char *symname = (char *)&(aoutfile->strings[index]);

    if (symname[0] == '_') {
	symname++;
    }

    return symname;
}

/*
 * Return the name of a symbol 
 */
static const char *
AOUTGetSymbolName(AOUTModulePtr aoutfile, struct AOUT_nlist *sym)
{
    return AOUTGetString(aoutfile, sym->n_un.n_strx);
}

/*
 * Return the value of a symbol in the loader's symbol table
 */
static void *
AOUTGetSymbolValue(AOUTModulePtr aoutfile, int index, int *pInvariant)
{
    void *symval = NULL;	/* value of the indicated symbol */
    itemPtr symbol = NULL;	/* name/value of symbol */
    const char *name = NULL;

    name = AOUTGetSymbolName(aoutfile, aoutfile->symtab + index);

    if (name)
	symbol = LoaderHashFind(name);

    if (symbol == NULL ||
	!SCOPE_OK(symbol, aoutfile->handle, LOOKUP_SCOPE_GLOBAL)) {
	return NULL;
    }
    symval = (void *)symbol->address;
    if (pInvariant) {
	*pInvariant = ((symbol->handle == aoutfile->handle) ||
		       (symbol->scope & LOOKUP_SCOPE_BUILTIN));
    }

    return symval;
}

/*
 * Perform the actual relocation 
 */
static void
AOUT_Relocate(AOUTRelocPtr p, unsigned long *destl, unsigned long val,
	      int pcrel)
{
#if LOADERDEBUG
    LoaderDebugMsg(LOADER_DEBUG_LOWLEVEL, "AOUT_Relocate %p : %08lx %s",
		   (void *)destl, *destl, pcrel == 1 ? "rel" : "abs");

#endif

    if (p->assigned) {
	*destl = p->olddata;
    } else {
	p->olddata = *destl;
	p->assigned = 1;
    }

    if (pcrel) {
	/* relative to PC */
	*destl = val - ((unsigned long)destl + sizeof(long));
    } else {
	*destl += val;
    }
#if LOADERDEBUG
    LoaderDebugMsg(LOADER_DEBUG_LOWLEVEL, " -> %08lx\n", *destl);
#endif
}

/*
 * Fix the relocation for text or data section
 */
static int
AOUT_RelocateEntry(AOUTRelocPtr p)
{
    AOUTModulePtr aoutfile = p->file;
    int type = p->type;
    struct relocation_info *rel = p->rel;
    AOUTHDR *header = aoutfile->header;
#if LOADERDEBUG
    AOUT_nlist *symtab = aoutfile->symtab;
#endif
    int symnum;
    void *symval = NULL;
    unsigned long *destl;	/* address of the location to be modified */
    int invariant = 0;

    symnum = rel->r_symbolnum;
#if LOADERDEBUG
    {
	if (rel->r_extern) {
	    LoaderDebugMsg(LOADER_DEBUG_LOWLEVEL,
			   "AOUT_RelocateEntry: extern %s\n",
			   AOUTGetSymbolName(aoutfile, symtab + symnum));
	} else {
	    LoaderDebugMsg(LOADER_DEBUG_LOWLEVEL,
			   "AOUT_RelocateEntry: intern\n");
	}
	LoaderDebugMsg(LOADER_DEBUG_LOWLEVEL, "  pcrel: %d", rel->r_pcrel);
	LoaderDebugMsg(LOADER_DEBUG_LOWLEVEL, "  length: %d", rel->r_length);
	LoaderDebugMsg(LOADER_DEBUG_LOWLEVEL, "  baserel: %d", rel->r_baserel);
	LoaderDebugMsg(LOADER_DEBUG_LOWLEVEL,
		       "  jmptable: %d", rel->r_jmptable);
	LoaderDebugMsg(LOADER_DEBUG_LOWLEVEL,
		       "  relative: %d", rel->r_relative);
	LoaderDebugMsg(LOADER_DEBUG_LOWLEVEL, "  copy: %d\n", rel->r_copy);
    }
#endif /* AOUTDEBUG */

    if (rel->r_length != 2) {
	ErrorF("AOUT_ReloateEntry: length != 2\n");
    }
    /* 
     * First find the address to modify
     */
    switch (type) {
    case AOUT_TEXT:
	/* Check that the relocation offset is in the text segment */
	if (rel->r_address > header->a_text) {
	    ErrorF("AOUT_RelocateEntry(): "
		   "text relocation out of text section\n");
	}
	destl = (unsigned long *)(aoutfile->text + rel->r_address);
	break;
    case AOUT_DATA:
	/* Check that the relocation offset is in the data segment */
	if (rel->r_address > header->a_data) {
	    ErrorF("AOUT_RelocateEntry():"
		   "data relocation out of data section\n");
	}
	destl = (unsigned long *)(aoutfile->data + rel->r_address);
	break;
    default:
	ErrorF("AOUT_RelocateEntry(): unknown section type %d\n", type);
	return 0;
    }				/* switch */

    /*
     * Now handle the relocation 
     */
    if (rel->r_extern) {
	/* Lookup the symbol in the loader's symbol table */
	symval = AOUTGetSymbolValue(aoutfile, symnum, &invariant);
	if (symval != NULL) {
	    if (p->relocated) {
		/* Already relocated. */
		return invariant;
	    } else {
		p->relocated = 1;
	    }
	} else {
	    symval = &LoaderDefaultFunc;
	    p->relocated = 0;
	}
	if (symval) {
	    if (p->symval == symval) {
		/* Unchanged. */
		return invariant;
	    } else
		p->symval = symval;
	}
    } else {
	/* intern */
	invariant = 1;
	switch (rel->r_symbolnum) {
	case AOUT_TEXT:
#if LOADERDEBUG
	    LoaderDebugMsg(LOADER_DEBUG_LOWLEVEL, "  AOUT_TEXT\n");
#endif
	    /* Only absolute intern text relocations need to be handled */
	    if (rel->r_pcrel == 0) {
		symval = aoutfile->text;
		p->relocated = 1;
	    } else
		p->relocated = 0;
	    break;

	case AOUT_DATA:
#if LOADERDEBUG
	    LoaderDebugMsg(LOADER_DEBUG_LOWLEVEL, "  AOUT_DATA\n");
#endif
	    if (rel->r_pcrel == 0) {
		symval = (void *)((unsigned long)aoutfile->data - 
			          header->a_text);
		p->relocated = 1;
	    } else {
		ErrorF("AOUT_RelocateEntry(): "
		       "don't know how to handle data pc-relative reloc\n");
		p->relocated = 0;
	    }
	    break;

	case AOUT_BSS:
#if LOADERDEBUG
	    LoaderDebugMsg(LOADER_DEBUG_LOWLEVEL, "  AOUT_BSS\n");
#endif
	    if (rel->r_pcrel == 0) {
		symval = (void *)((unsigned long)aoutfile->bss - 
				  header->a_text - header->a_data);
		p->relocated = 1;
	    } else {
		ErrorF("AOUT_RelocateEntry(): "
		       "don't know how to handle bss pc-relative reloc\n");
		p->relocated = 0;
	    }
	    break;

	default:
	    ErrorF("AOUT_RelocateEntry():"
		   " unknown intern relocation type: %d\n", rel->r_symbolnum);
	    return 0;
	}			/* switch */
    }
    if (symval != NULL)
	AOUT_Relocate(p, destl, (unsigned long)symval, rel->r_pcrel);

    return invariant;
}				/* AOUT_RelocateEntry */

static AOUTRelocPtr
AOUTCollectRelocations(AOUTModulePtr aoutfile)
{
    AOUTHDR *header = aoutfile->header;
    int i, nreloc;
    struct relocation_info *rel;
    AOUTRelocPtr reloc_head = NULL;
    AOUTRelocPtr tmp;

    /* Text relocations */
    if (aoutfile->text != NULL && aoutfile->txtrel != NULL) {
	nreloc = header->a_trsize / sizeof(struct relocation_info);

	for (i = 0; i < nreloc; i++) {
	    rel = aoutfile->txtrel + i;
	    tmp = AOUTDelayRelocation(aoutfile, AOUT_TEXT, rel);
	    if (tmp) {
		tmp->next = reloc_head;
		reloc_head = tmp;
	    }
	}			/* for */
    }
    /* Data relocations */
    if (aoutfile->data != NULL && aoutfile->datarel != NULL) {
	nreloc = header->a_drsize / sizeof(struct relocation_info);

	for (i = 0; i < nreloc; i++) {
	    rel = aoutfile->datarel + i;
	    tmp = AOUTDelayRelocation(aoutfile, AOUT_DATA, rel);
	    tmp->next = reloc_head;
	    reloc_head = tmp;
	}			/* for */
    }
    return reloc_head;
}				/* AOUTCollectRelocations */

/*
 * AOUT_GetSymbols()
 * 
 * add the symbols to the loader's symbol table
 */
static LOOKUP *
AOUT_GetSymbols(AOUTModulePtr aoutfile)
{
    int fd = aoutfile->fd;
    AOUTHDR *header = aoutfile->header;
    int nsyms, soff, i, l;
    const char *symname;
    AOUT_nlist *s;
    LOOKUP *lookup, *lookup_common;
    AOUTCommonPtr tmp;

    aoutfile->symtab = (AOUT_nlist *) _LoaderFileToMem(fd,
						       AOUT_SYMOFF(header),
						       header->a_syms,
						       "symbols");
    nsyms = header->a_syms / sizeof(AOUT_nlist);
    lookup = xf86loadermalloc(nsyms * sizeof(LOOKUP));
    if (lookup == NULL) {
	ErrorF("AOUT_GetSymbols(): can't allocate memory\n");
	return NULL;
    }
    for (i = 0, l = 0; i < nsyms; i++) {
	s = aoutfile->symtab + i;
	soff = s->n_un.n_strx;
	if (soff == 0 || (s->n_type & AOUT_STAB) != 0)
	    continue;
	symname = AOUTGetSymbolName(aoutfile, s);
#if LOADERDEBUG
	LoaderDebugMsg(LOADER_DEBUG_LOWLEVEL,
		       "AOUT_GetSymbols(): %s %02x %02x %08lx\n",
		       symname, s->n_type, s->n_other, s->n_value);
#endif
	switch (s->n_type & AOUT_TYPE) {
	case AOUT_UNDF:
	    if (s->n_value != 0) {
		if (!LoaderHashFind(symname)) {
#if LOADERDEBUG
		    LoaderDebugMsg(LOADER_DEBUG_LOWLEVEL,
				   "Adding common %s\n", symname);
#endif
		    tmp = AOUTAddCommon(s, i);
		    if (tmp) {
			tmp->next = listCOMMON;
			listCOMMON = tmp;
		    }
		}
	    } else {
#if LOADERDEBUG
		LoaderDebugMsg(LOADER_DEBUG_LOWLEVEL,
			       "Adding undef %s\n", symname);
#endif
	    }
	    break;
	case AOUT_TEXT:
	    if (s->n_type & AOUT_EXT) {
		lookup[l].symName = xf86loaderstrdup(symname);
		/* text symbols start at 0 */
		lookup[l].offset = (funcptr) (aoutfile->text + s->n_value);
#if LOADERDEBUG
		LoaderDebugMsg(LOADER_DEBUG_LOWLEVEL,
			       "Adding text %s %p\n", symname,
			  (void *)lookup[l].offset);
#endif
		l++;
	    }
	    break;
	case AOUT_DATA:
	    if (s->n_type & AOUT_EXT) {
		lookup[l].symName = xf86loaderstrdup(symname);
		/* data symbols are following text */
		lookup[l].offset = (funcptr) (aoutfile->data +
					      s->n_value - header->a_text);
#if LOADERDEBUG
		LoaderDebugMsg(LOADER_DEBUG_LOWLEVEL,
			       "Adding data %s %p\n", symname,
			       (void *)lookup[l].offset);
#endif
		l++;
	    }
	    break;
	case AOUT_BSS:
	    if (s->n_type & AOUT_EXT) {
		lookup[l].symName = xf86loaderstrdup(symname);
		/* bss symbols follow both text and data */
		lookup[l].offset = (funcptr) (aoutfile->bss + s->n_value
					      - (header->a_data
						 + header->a_text));
#if LOADERDEBUG
		LoaderDebugMsg(LOADER_DEBUG_LOWLEVEL, "Adding bss %s %p\n",
			       symname, (void *)lookup[l].offset);
#endif
		l++;
	    }
	    break;
	case AOUT_FN:
#if LOADERDEBUG
	    if (s->n_type & AOUT_EXT) {
		LoaderDebugMsg(LOADER_DEBUG_LOWLEVEL,
			       "Ignoring AOUT_FN %s\n", symname);
	    } else {
		LoaderDebugMsg(LOADER_DEBUG_LOWLEVEL,
			       "Ignoring AOUT_WARN %s\n", symname);
	    }
#endif
	    break;
	default:
	    ErrorF("Unknown symbol type %x\n", s->n_type & AOUT_TYPE);
	}			/* switch */
    }				/* for */
    lookup[l].symName = NULL;

    lookup_common = AOUTCreateCommon(aoutfile);
    if (lookup_common) {
	LOOKUP *p;

	for (i = 0, p = lookup_common; p->symName; i++, p++) ;
	memcpy(&(lookup[l]), lookup_common, i * sizeof(LOOKUP));

	xf86loaderfree(lookup_common);
	l += i;
	lookup[l].symName = NULL;
    }
    return lookup;
}				/* AOUT_GetSymbols */

/*
 * Public API for the a.out implementation of the loader
 */
void *
AOUTLoadModule(loaderPtr modrec, int aoutfd, LOOKUP ** ppLookup)
{
    AOUTModulePtr aoutfile = NULL;
    AOUTHDR *header;
    AOUTRelocPtr reloc, tail;

#if LOADERDEBUG
    LoaderDebugMsg(LOADER_DEBUG_FILES, "AOUTLoadModule(%s, %d, %d)\n",
		   modrec->name, modrec->handle, aoutfd);
#endif
    if ((aoutfile = xf86loadercalloc(1, sizeof(AOUTModuleRec))) == NULL) {
	ErrorF("Unable to allocate AOUTModuleRec\n");
	return NULL;
    }

    aoutfile->handle = modrec->handle;
    aoutfile->module = modrec->module;
    aoutfile->fd = aoutfd;
    aoutfile->desc = modrec->desc;

    /*
     *  Get the a.out header
     */
    aoutfile->header =
	    (AOUTHDR *) _LoaderFileToMem(aoutfd, 0, sizeof(AOUTHDR),
					 "header");
    header = (AOUTHDR *) aoutfile->header;

    /* 
     * Load the 6 other sections 
     */
    /* text */
    if (header->a_text != 0) {
	aoutfile->text = _LoaderFileToMem(aoutfile->fd,
					  AOUT_TXTOFF(header),
					  header->a_text, "text");
	aoutfile->textsize = header->a_text;
    } else {
	aoutfile->text = NULL;
    }
    /* data */
    if (header->a_data != 0) {
	aoutfile->data = _LoaderFileToMem(aoutfile->fd,
					  AOUT_DATOFF(header),
					  header->a_data, "data");
	aoutfile->datasize = header->a_data;
    } else {
	aoutfile->data = NULL;
    }
    /* bss */
    if (header->a_bss != 0) {
	aoutfile->bss = xf86loadercalloc(1, header->a_bss);
	aoutfile->bsssize = header->a_bss;
    } else {
	aoutfile->bss = NULL;
    }
    /* Text Relocations */
    if (header->a_trsize != 0) {
	aoutfile->txtrel = _LoaderFileToMem(aoutfile->fd,
					    AOUT_TRELOFF(header),
					    header->a_trsize, "txtrel");
    } else {
	aoutfile->txtrel = NULL;
    }
    /* Data Relocations */
    if (header->a_drsize != 0) {
	aoutfile->datarel = _LoaderFileToMem(aoutfile->fd,
					     AOUT_DRELOFF(header),
					     header->a_drsize, "datarel");
    } else {
	aoutfile->datarel = NULL;
    }
    /* String table */
    _LoaderFileRead(aoutfile->fd, AOUT_STROFF(header),
		    &(aoutfile->strsize), sizeof(int));
    if (aoutfile->strsize != 0) {
	aoutfile->strings = _LoaderFileToMem(aoutfile->fd,
					     AOUT_STROFF(header),
					     aoutfile->strsize, "strings");
    } else {
	aoutfile->strings = NULL;
    }
    /* load symbol table */
    *ppLookup = AOUT_GetSymbols(aoutfile);

    /* Do relocations */
    reloc = AOUTCollectRelocations(aoutfile);

    if (reloc) {
	for (tail = reloc; tail->next; tail = tail->next) ;
	tail->next = *_LoaderGetRelocations(aoutfile->desc);
	*_LoaderGetRelocations(aoutfile->desc) = reloc;
    }

    return (void *)aoutfile;
}

void
AOUTResolveSymbols(LoaderDescPtr desc, int handle)
{
    AOUTRelocPtr p, *pp, tmp;

#if LOADERDEBUG
    LoaderDebugMsg(LOADER_DEBUG_LOWLEVEL, "AOUTResolveSymbols()\n");
#endif

    pp = (AOUTRelocPtr *)_LoaderGetRelocations(desc);
    for (p = *_LoaderGetRelocations(desc); p;) {
	/* If handle is valid, only relocate symbols for that module. */
	if (handle >= 0 && p->file->handle != handle) {
	    pp = &(p->next);
	    p = p->next;
	    continue;
	}
	
	if (AOUT_RelocateEntry(p)) {
	    /*
	     * Remove invariant relocations, since they can't change when
	     * other modules are loaded or unloaded.
	     */
	    *pp = p->next;
	    tmp = p;
	    p = p->next;
	    xf86loaderfree(tmp);
	} else {
	    pp = &(p->next);
	    p = p->next;
	}
    }
}				/* AOUTResolveSymbols */

int
AOUTCheckForUnresolved(LoaderDescPtr desc)
{
    int symnum;
    AOUTRelocPtr crel;
    const char *name;
    int fatalsym = 0;

#if LOADERDEBUG
    LoaderDebugMsg(LOADER_DEBUG_LOWLEVEL, "AOUTCheckForUnResolved()\n");
#endif
    for (crel = *_LoaderGetRelocations(desc); crel; crel = crel->next) {
	if (crel->relocated)
	    continue;

	symnum = crel->rel->r_symbolnum;
	name = AOUTGetSymbolName(crel->file, crel->file->symtab + symnum);
	if (_LoaderHandleUnresolved(name, crel->file->handle))
	    fatalsym = 1;
    }
    return fatalsym;
}

void
AOUTUnloadModule(void *modptr)
{
    AOUTModulePtr aoutfile = (AOUTModulePtr) modptr;
    AOUTRelocPtr relptr, *prevptr;

#if LOADERDEBUG
    LoaderDebugMsg(LOADER_DEBUG_LOWLEVEL, "AOUTUnLoadModule(0x%p)\n", modptr);
#endif

    /*
     * Delete any unresolved relocations
     */

    relptr = *_LoaderGetRelocations(aoutfile->desc);
    prevptr = (AOUTRelocPtr *)_LoaderGetRelocations(aoutfile->desc);

    while (relptr) {
	if (relptr->file == aoutfile) {
	    *prevptr = relptr->next;
	    xf86loaderfree(relptr);
	    relptr = *prevptr;
	} else {
	    prevptr = &(relptr->next);
	    relptr = relptr->next;
	}
    }				/* while */

    /* clean the symbols table */
    LoaderHashTraverse((void *)aoutfile, AOUTHashCleanOut);

#define CheckandFree(ptr,size)  if(ptr) _LoaderFreeFileMem((ptr),(size))

    CheckandFree(aoutfile->strings, aoutfile->strsize);
    CheckandFree(aoutfile->symtab, aoutfile->header->a_syms);
    CheckandFree(aoutfile->datarel, aoutfile->header->a_drsize);
    CheckandFree(aoutfile->txtrel, aoutfile->header->a_trsize);
    CheckandFree(aoutfile->data, aoutfile->header->a_data);
    CheckandFree(aoutfile->text, aoutfile->header->a_text);
    /* Free allocated sections */
    if (aoutfile->bss != NULL) {
	xf86loaderfree(aoutfile->bss);
    }
    if (aoutfile->common != NULL) {
	xf86loaderfree(aoutfile->common);
    }

    /* Free header */
    _LoaderFreeFileMem(aoutfile->header, sizeof(AOUTHDR));

    /* Free the module structure itself */
    xf86loaderfree(aoutfile);

    return;
}

char *
AOUTAddressToSection(void *modptr, unsigned long address)
{
    AOUTModulePtr aoutfile = (AOUTModulePtr) modptr;

    if (address >= (unsigned long)aoutfile->text &&
	address <= (unsigned long)aoutfile->text + aoutfile->textsize) {
	return "text";
    }
    if (address >= (unsigned long)aoutfile->data &&
	address <= (unsigned long)aoutfile->data + aoutfile->datasize) {
	return "data";
    }
    if (address >= (unsigned long)aoutfile->bss &&
	address <= (unsigned long)aoutfile->bss + aoutfile->bsssize) {
	return "bss";
    }

    return NULL;
}

const char *
AOUTFindRelocName(LoaderDescPtr desc, int handle, unsigned long addr)
{
    AOUTRelocPtr p;
    long diff;

    for (p = *_LoaderGetRelocations(desc); p;) {
	/* If handle is valid, only look for symbols for that module. */
	if (handle >= 0 && p->file->handle != handle) {
	    p = p->next;
	    continue;
	}
	diff = addr - ((unsigned long)p->file->text + p->rel->r_address);
	if (diff < 8 && diff > -8) {
	    return AOUTGetSymbolName(p->file,
				     p->file->symtab + p->rel->r_symbolnum);
	}
	p = p->next;
    }
    return NULL;
}

const char *
AOUTAddressToSymbol(void *modptr, unsigned long addr, unsigned long *symaddr,
		    const char **filename, int exe)
{
    AOUTModulePtr aoutfile;
    AOUTHDR *header;
    AOUT_nlist *syms, *s;
    int i, numsyms, soff;
    long bestDiff = MAXINT, diff;
    const char *best = NULL;
    unsigned long saddr, bestAddr = 0;

    if (!modptr)
	return NULL;
    else
	aoutfile = (AOUTModulePtr)modptr;

    header = aoutfile->header;
    syms = aoutfile->symtab;
    numsyms = header->a_syms / sizeof(AOUT_nlist);
    for (i = 0; i < numsyms; i++) {
	s = syms + i;
	soff = s->n_un.n_strx;
	if (soff == 0 || (s->n_type & AOUT_STAB))
	    continue;
	switch (s->n_type & AOUT_TYPE) {
	case AOUT_TEXT:
	case AOUT_DATA:
	case AOUT_BSS:
	    saddr = s->n_value;
	    if (exe || (aoutfile->text != 0 || aoutfile->data != 0)) {
		switch (s->n_type & AOUT_TYPE) {
		case AOUT_TEXT:
		    saddr += (unsigned long)aoutfile->text;
		    break;
		case AOUT_DATA:
		    saddr += ((unsigned long)aoutfile->data - header->a_text);
		    break;
		case AOUT_BSS:
		    saddr += ((unsigned long)aoutfile->bss -
			      (header->a_data + header->a_text));
		    break;
		}
	    }
	    break;
	default:
	    saddr = 0;
	    break;
	}

	if (!saddr)
	    continue;

	diff = addr - saddr;
	if (diff >= 0) {
	    if ((best && diff < bestDiff) || !best) {
		best = AOUTGetSymbolName(aoutfile, s);
		bestDiff = diff;
		bestAddr = saddr;
	    }
	}
    }
    *filename = NULL;
    if (best && bestDiff < 0x10000) {
	*symaddr = bestAddr;
	return best;
    } else {
	return NULL;
    }
}

#if EXESYMDEBUG
static void
ShowExeSyms(AOUTModulePtr aoutfile)
{
    AOUTHDR *header;
    AOUT_nlist *syms, *s;
    int i, numsyms;
    unsigned long saddr;

    header = aoutfile->header;
    syms = aoutfile->symtab;
    numsyms = header->a_syms / sizeof(AOUT_nlist);

    LoaderDebugMsg(LOADER_DEBUG_SHOW_EXE_SYMS,
		   "ShowExeSyms: numsyms: %d\n", numsyms);

    for (i = 0; i < numsyms; i++) {
	s = syms +  i;
	saddr = s->n_value;
	if ((s->n_type & AOUT_ABS) == 0) {
	    switch (s->n_type & AOUT_TYPE) {
	    case AOUT_TEXT:
		saddr += ((unsigned long)aoutfile->text - header->a_text);
		break;
	    case AOUT_DATA:
		saddr += ((unsigned long)aoutfile->data - header->a_data);
		break;
	    case AOUT_BSS:
		saddr += ((unsigned long)aoutfile->bss - header->a_bss);
		break;
	    }
	}
	LoaderDebugMsg(LOADER_DEBUG_SHOW_EXE_SYMS,
		       "ShowExeSyms: value=0x%lx\tsaddr=0x%lx\tTYPE=%x\t%s\n",
		       (unsigned long)s->n_value, saddr, s->n_type,
		       AOUTGetSymbolName(aoutfile, s));

    }
}
#endif

void *
AOUTReadExecutableSyms(int aoutfd)
{
    AOUTModulePtr aoutfile;
    AOUTHDR *header;
  
    aoutfile = xf86loadercalloc(1, sizeof(*aoutfile));
    if (!aoutfile) {
	ErrorF("Cannot allocate space for the main executable symbol table\n");
	return NULL;
    }

    aoutfile->fd = aoutfd;

    /* Get the header. */
    aoutfile->header = (AOUTHDR *)_LoaderFileToMem(aoutfd, 0,
						   sizeof(AOUTHDR), "header");
    header = aoutfile->header;

    /* Get the symbol table. */
    aoutfile->symtab = (AOUT_nlist *)_LoaderFileToMem(aoutfd,
						      AOUT_SYMOFF(header),
						      header->a_syms,
						      "symbols");
    aoutfile->text = 0;
    aoutfile->data = (unsigned char *)AOUT_DATOFF(header);
    aoutfile->bss = 0;

    /* Get the string table. */
    _LoaderFileRead(aoutfd, AOUT_STROFF(header), &(aoutfile->strsize),
		    sizeof(int));
    if (aoutfile->strsize != 0)
	aoutfile->strings = _LoaderFileToMem(aoutfd, AOUT_STROFF(header),
					     aoutfile->strsize, "strings");

#if EXESYMDEBUG
    ShowExeSyms(aoutfile);
#endif
    return aoutfile;
}
