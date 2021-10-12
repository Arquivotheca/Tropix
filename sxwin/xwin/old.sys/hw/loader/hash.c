/* $XFree86: xc/programs/Xserver/hw/xfree86/loader/hash.c,v 1.29 2006/04/08 17:53:40 dawes Exp $ */

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
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE COPYRIGHT HOLDER(S) OR AUTHOR(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of the copyright holder(s)
 * and author(s) shall not be used in advertising or otherwise to promote
 * the sale, use or other dealings in this Software without prior written
 * authorization from the copyright holder(s) and author(s).
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

#include "os.h"
#include <X11/Xos.h>
#undef abs
#include <stdlib.h>
#include "sym.h"

#ifndef LOADERDEBUG
#define LOADERDEBUG 0
#endif
#include "loader.h"
#include "loaderProcs.h"
#include "hash.h"

#if defined(Lynx)
#define MAXINT	32000
#else
#include <limits.h>
#undef MAXINT
#define MAXINT INT_MAX
#endif

/* Prototypes for static functions. */
static unsigned int hashFunc(const char *);
static itemPtr LoaderHashFindNearest(unsigned long);

static itemPtr LoaderhashTable[HASHSIZE];

#if LOADERDEBUG
static int hashhits[HASHSIZE];
static int hashdepth[HASHSIZE];
static int hashmaxdepth[HASHSIZE];
#endif

void
LoaderDumpHashHits(void)
{
#if LOADERDEBUG
    int i;
    int hits = 0;
    int depth = 0;
    int maxdepth = 0;
    int hitsdev = 0, hitsmaxdev = 0, hitsmaxdevi = 0;
    int depthdev = 0, depthmaxdev = 0, depthmaxdevi = 0;
    int maxdepthdev = 0, maxdepthmaxdev = 0, maxdepthmaxdevi = 0;

    for (i = 0; i < HASHSIZE; i++) {
	LoaderDebugMsg(LOADER_DEBUG_HASH_DETAILS,
		       "hashhits[%d] = %d, hashdepth[%d] = %d, "
		       "hashmaxdepth[%d] = %d\n",
		       i, hashhits[i], i, hashdepth[i], i, hashmaxdepth[i]);
	hits += hashhits[i];
	depth += hashdepth[i];
	maxdepth += hashmaxdepth[i];
    }

    hits /= HASHSIZE;
    depth /= HASHSIZE;
    maxdepth /= HASHSIZE;

    for (i = 0; i < HASHSIZE; i++) {
	int delta;

	if (hashhits[i] < hits)
	    delta = hits - hashhits[i];
	else
	    delta = hashhits[i] - hits;
	hitsdev += delta;
	if (delta > hitsmaxdev) {
	    hitsmaxdev = delta;
	    hitsmaxdevi = i;
	}
	if (hashdepth[i] < depth)
	    delta = depth - hashdepth[i];
	else
	    delta = hashdepth[i] - depth;
	depthdev += delta;
	if (delta > depthmaxdev) {
	    depthmaxdev = delta;
	    depthmaxdevi = i;
	}
	if (hashmaxdepth[i] < maxdepth)
	    delta = maxdepth - hashmaxdepth[i];
	else
	    delta = hashmaxdepth[i] - maxdepth;
	maxdepthdev += delta;
	if (delta > maxdepthmaxdev) {
	    maxdepthmaxdev = delta;
	    maxdepthmaxdevi = i;
	}
    }

    hitsdev /= HASHSIZE;
    depthdev /= HASHSIZE;
    maxdepthdev /= HASHSIZE;
    LoaderDebugMsg(LOADER_DEBUG_HASH_SUMMARY,
		   "Average hash hits = %d\n", hits);
    LoaderDebugMsg(LOADER_DEBUG_HASH_SUMMARY,
		   "Average hash hits deviation = %d\n", hitsdev);
    LoaderDebugMsg(LOADER_DEBUG_HASH_SUMMARY,
		   "Maximum hash hits deviation = %d for bucket %d\n",
		   hitsmaxdev, hitsmaxdevi);
    LoaderDebugMsg(LOADER_DEBUG_HASH_SUMMARY,
		   "Average hash depth = %d\n", depth);
    LoaderDebugMsg(LOADER_DEBUG_HASH_SUMMARY,
		   "Average hash depth deviation = %d\n", depthdev);
    LoaderDebugMsg(LOADER_DEBUG_HASH_SUMMARY,
		   "Maximum hash depth deviation = %d for bucket %d\n",
		   depthmaxdev, depthmaxdevi);
    LoaderDebugMsg(LOADER_DEBUG_HASH_SUMMARY,
		   "Average hash max depth = %d\n", maxdepth);
    LoaderDebugMsg(LOADER_DEBUG_HASH_SUMMARY,
		   "Average hash max depth deviation = %d\n", maxdepthdev);
    LoaderDebugMsg(LOADER_DEBUG_HASH_SUMMARY,
		   "Maximum hash max depth deviation = %d for bucket %d\n",
		   maxdepthmaxdev, maxdepthmaxdevi);
    LoaderDumpSymbols();
#else
    return;
#endif
}

static unsigned int
hashFunc(const char *string)
{
    int i, i1, i2, i3, i4;
    int hash = 0;
    int len;

    i = len = strlen(string);

    if (len < 5) {
	hash = len;
    } else {
	i1 = 5;
	i2 = 4;
	i3 = 3;
	i4 = 2;
	if (len < 25) {
	    i1 >>= 1;
	    i2 >>= 1;
	    i3 >>= 1;
	    i4 >>= 1;
	} else if (len < 45) {
	    i1 >>= 2;
	    i2 >>= 2;
	    i3 >>= 2;
	    i4 >>= 2;
	} else {
	    i1 >>= 3;
	    i2 >>= 3;
	    i3 >>= 3;
	    i4 >>= 3;
	}
	if (len > 40)
	    i = 40;
	else if (len > 20)
	    i = 20;
	else if (len > 10)
	    i = 10;
	hash = (string[len - i1] * string[i - i2] +
		string[i - i3] * string[len - i4]) & (HASHSIZE - 1);
    }

#if LOADERDEBUG
    hashhits[hash]++;
#endif

    return hash;
}

void
LoaderHashAdd(itemPtr entry)
{
    int bucket = hashFunc(entry->name);
    itemPtr oentry;

#if LOADERDEBUG
    LoaderDebugMsg(LOADER_DEBUG_HASH_ADD, "LoaderHashAdd: %s\n", entry->name);
#endif

    if ((oentry = LoaderHashFind(entry->name)) != NULL)
	LoaderDuplicateSymbol(entry->name, oentry->handle);

    entry->next = LoaderhashTable[bucket];
    LoaderhashTable[bucket] = entry;
#if LOADERDEBUG
    hashdepth[bucket]++;
    if (hashdepth[bucket] > hashmaxdepth[bucket])
	hashmaxdepth[bucket] = hashdepth[bucket];
#endif
    return;
}

void
LoaderAddSymbols(int handle, int module, LOOKUP *list, unsigned long scope,			 char **modData)
{
    LOOKUP *l = list, *exports = NULL, *moddata = NULL;
    itemPtr i;
    char *modname;
    const char **exportedSymbols = NULL;
    const char **s = NULL;

    if (!list)
	return;

    /*
     * First look for a symbol called <name>ExportedSymbols.  If it exists,
     * only export the symbols that are listed in that array.  Otherwise
     * export all of the external symbols.
     */
    modname = _LoaderHandleToCanonicalName(handle);

    if (modname) {
	char *exportName = NULL;
	char *moddataName = NULL;

#if LOADERDEBUG
	LoaderDebugMsg(LOADER_DEBUG_ADD_SYMBOLS,
		       "LoaderAddSymbols: module %s\n", modname);
#endif

	exportName = xf86loadermalloc(strlen(EXPORTED_SYMS_NAME) +
				      strlen(modname) + 1);
	if (!exportName)
	    return;

	if (modData) {
	    moddataName = xf86loadermalloc(strlen(MODULE_DATA_NAME) +
					   strlen(modname) + 1);
	    if (!moddataName) {
		xf86loaderfree(exportName);
		return;
	    }
	    sprintf(moddataName, "%s" MODULE_DATA_NAME, modname);
	}

	sprintf(exportName, "%s" EXPORTED_SYMS_NAME, modname);

	while (l->symName) {
	    if (strcmp(l->symName, exportName) == 0)
		exports = l;
	    else if (modData && strcmp(l->symName, moddataName) == 0)
		moddata = l;
	    if (exports && (!modData || moddata))
		break;
	    l++;
	}
	xf86loaderfree(exportName);
	if (moddataName)
	    xf86loaderfree(moddataName);
    }

    /*
     * Allocate the exports and moddata list items first.
     */
    if (moddata) {
	*modData = (char *)moddata->offset;
    }
    if (exports) {
	LoaderResolveSymbols(handle);
	exportedSymbols = (const char **)exports->offset;
#if LOADERDEBUG
	if (exportedSymbols) {
	    for (s = exportedSymbols; *s; s++) {
		LoaderDebugMsg(LOADER_DEBUG_ADD_SYMBOLS,
			       "ExportedSymbols: %s\n", *s);
	    }
	}
#endif
    }

    if (scope == LOOKUP_SCOPE_AUTO && !exports)
	scope = LOOKUP_SCOPE_GLOBAL;

    for (l = list; l->symName; l++) {
	if ((exports && l == exports) || (moddata && l == moddata))
	    continue;

	i = xf86loadercalloc(1, sizeof(itemRec));
	if (exports && scope == LOOKUP_SCOPE_AUTO) {
	    /* Check if the symbols is in the exports list. */
	    for (s = exportedSymbols; *s; s++) {
		if (strcmp(*s, l->symName) == 0) {
		    i->scope = LOOKUP_SCOPE_GLOBAL;
		    break;
		}
	    }
	    if (!*s)
		i->scope = LOOKUP_SCOPE_SELF;
	} else {
	    i->scope = scope;
	}
	i->name = l->symName;
	i->address = (char *)l->offset;
	i->handle = handle;
	i->module = module;
#if LOADERDEBUG
	LoaderDebugMsg(LOADER_DEBUG_ADD_SYMBOLS,
		       "LoaderAddSymbols: module \"%s\" adding symbol %s (%p), "
		       "scope 0x%lx\n",
		       modname, l->symName, (void *)l->offset, i->scope);
#endif
	LoaderHashAdd(i);
    }
}

itemPtr
LoaderHashDelete(const char *string)
{
    int bucket = hashFunc(string);
    itemPtr entry;
    itemPtr *entry2;

    entry = LoaderhashTable[bucket];
    entry2 = &(LoaderhashTable[bucket]);
    while (entry) {
	if (!strcmp(entry->name, string)) {
	    *entry2 = entry->next;
	    xf86loaderfree(entry->name);
	    xf86loaderfree(entry);
	    return 0;
	}
	entry2 = &(entry->next);
	entry = entry->next;
    }
#if LOADERDEBUG
    hashdepth[bucket]--;
#endif
    return 0;
}

itemPtr
LoaderHashFind(const char *string)
{
    int bucket = hashFunc(string);
    itemPtr entry;

    entry = LoaderhashTable[bucket];
    while (entry) {
	if (!strcmp(entry->name, string)) {
	    return entry;
	}
	entry = entry->next;
    }
    return 0;
}

static itemPtr
LoaderHashFindNearest(unsigned long address)
{
    int i;
    itemPtr entry, best_entry = 0;
    long best_difference = MAXINT;

    for (i = 0; i < HASHSIZE; i++) {
	entry = LoaderhashTable[i];
	while (entry) {
	    long difference = (long)address - (long)entry->address;

	    if (difference >= 0) {
		if (best_entry) {
		    if (difference < best_difference) {
			best_entry = entry;
			best_difference = difference;
		    }
		} else {
		    best_entry = entry;
		    best_difference = difference;
		}
	    }
	    entry = entry->next;
	}
    }
    return best_entry;
}

/* Returns 0 when the symbol's name is 'main' */

int
LoaderPrintSymbol(unsigned long address)
{
    itemPtr entry;
    unsigned long symaddr = 0;
    const char *symname = NULL;
    const char *module = NULL, *section = NULL;

    symname = _LoaderAddressToSymbol(address, &symaddr, &module);
    if (!symname) {
	entry = LoaderHashFindNearest(address);
	if (entry) {
	    symname = entry->name;
	    symaddr = (unsigned long)entry->address;
	}
    }
    if (!module)
	_LoaderAddressToSection(address, &module, &section);

    if (symname) {
#if defined(__alpha__) || defined(__ia64__)
	if (symaddr != 0)
	    ErrorF("0x%016lx %s + 0x%lx\n",
		   symaddr, symname, address - symaddr);
	else
	    ErrorF("%s\n", symname);
#else
	if (symaddr != 0 && address - symaddr > 0x10000) {
	    ErrorF("<cannot guess>\n");
	    return 1;
	}

	if (symaddr != 0)
	    ErrorF("0x%lx %s + 0x%lx\n", symaddr, symname, address - symaddr);
	else
	    ErrorF("%s\n", symname);
#endif
	if (module)
	    ErrorF("\tModule \"%s\"\n", module);
	if (section)
	    ErrorF("\tSection \"%s\"\n", section);
	if (!strcmp(symname, "main"))
	    return 0;
	else
	    return 1;
    } else {
	ErrorF("<cannot guess>\n");
	return 1;
    }
}

void
LoaderPrintItem(itemPtr pItem)
{
    if (pItem) {
	const char *module, *section;

#if defined(__alpha__) || defined(__ia64__)
	ErrorF("0x%016lx %s", (unsigned long)pItem->address, pItem->name);
#else
	ErrorF("0x%lx %s", (unsigned long)pItem->address, pItem->name);
#endif
	ErrorF(" (scope 0x%lx", pItem->scope);
	if (pItem->handle >= 0)
	    ErrorF(", handle %d", pItem->handle);
	ErrorF(")\n");
	if (pItem->address &&
	    _LoaderAddressToSection((unsigned long)pItem->address,
				    &module, &section))
	    ErrorF("\tModule \"%s\"\n\tSection \"%s\"\n", module, section);
    } else
	ErrorF("<null>\n");
}

void
LoaderPrintAddress(const char *symbol)
{
    itemPtr entry;

    entry = LoaderHashFind(symbol);
    LoaderPrintItem(entry);
}

void
LoaderHashTraverse(void *card, int (*fnp)(void *, itemPtr))
{
    int i;
    itemPtr entry, last_entry = 0;

    for (i = 0; i < HASHSIZE; i++) {
	last_entry = NULL;
	entry = LoaderhashTable[i];
	while (entry) {
	    if ((*fnp) (card, entry)) {
		if (last_entry) {
		    last_entry->next = entry->next;
		    xf86loaderfree(entry->name);
		    xf86loaderfree(entry);
		    entry = last_entry->next;
		} else {
		    LoaderhashTable[i] = entry->next;
		    xf86loaderfree(entry->name);
		    xf86loaderfree(entry);
		    entry = LoaderhashTable[i];
		}
#if LOADERDEBUG
		hashdepth[i]--;
#endif
	    } else {
		last_entry = entry;
		entry = entry->next;
	    }
	}
    }
}

#if LOADERDEBUG
void
LoaderDumpSymbols()
{
    itemPtr entry;
    int j;

    if (!(LoaderDebugLevel & LOADER_DEBUG_DUMP_HASH))
	return;

    for (j = 0; j < HASHSIZE; j++) {
	entry = LoaderhashTable[j];
	while (entry) {
	    ErrorF("Hash bucket %d:\n", j);
	    LoaderPrintItem(entry);
	    entry = entry->next;
	}
    }
}
#endif
