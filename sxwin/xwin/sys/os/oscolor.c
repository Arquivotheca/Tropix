/* $XFree86: xc/programs/Xserver/os/oscolor.c,v 3.15 2005/10/14 15:17:26 tsi Exp $ */
/***********************************************************

Copyright 1987, 1998  The Open Group

Permission to use, copy, modify, distribute, and sell this software and its
documentation for any purpose is hereby granted without fee, provided that
the above copyright notice appear in all copies and that both that
copyright notice and this permission notice appear in supporting
documentation.

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
OPEN GROUP BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of The Open Group shall not be
used in advertising or otherwise to promote the sale, use or other dealings
in this Software without prior written authorization from The Open Group.


Copyright 1987 by Digital Equipment Corporation, Maynard, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the name of Digital not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/
/*
 * Copyright © 1995-2005 by The XFree86 Project, Inc.
 * All Rights Reserved.
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
 *
 */

#include <stdio.h>
#include "os.h"
#include "opaque.h"
#include "oscolor.h"
#include "dix.h"
#define XK_LATIN1
#include <X11/keysymdef.h>

static dbEntryPtr *colorTab = NULL;
static dbEntryPtr hashTab[HASHSIZE];

/*
 * Make a normalised copy of the colour name.  Case is lowered, and space,
 * '-' and '_' characters are removed.  Colours are matched by comparing
 * their normalised names.
 */

static void
stringCopy(char *dest, char *source, int length)
{
    int i;

    for (i = 0; i < length; i++) {
	unsigned char c = (unsigned char)*source++;

	if ((c >= XK_A) && (c <= XK_Z))
	    *dest++ = (char)c + (XK_a - XK_A);
	else if ((c >= XK_Agrave) && (c <= XK_Odiaeresis))
	    *dest++ = (char)c + (XK_agrave - XK_Agrave);
	else if ((c >= XK_Ooblique) && (c <= XK_Thorn))
	    *dest++ = (char)c + (XK_oslash - XK_Ooblique);
	else if ((c != XK_space) && (c != XK_minus) && (c != XK_underscore))
	    *dest++ = (char)c;
    }

    *dest = '\0';
}

static dbEntryPtr
lookup(char *name, int len, Bool create)
{
    unsigned int h = 0, g;
    dbEntryPtr entry, *prev = NULL;
    char *str = name;

    if (!colorTab)
	return NULL;

    if (!(name = ALLOCATE_LOCAL(len + 1)))
	return NULL;
    stringCopy(name, str, len);
    name[len] = '\0';

    for (str = name; *str; str++) {
	h = (h << 4) + *str;
	if ((g = h) & 0xf0000000)
	    h ^= (g >> 24);
	h &= g;
    }
    h %= HASHSIZE;

    if ((entry = colorTab[h])) {
	for (; entry; prev = &entry->link, entry = entry->link) {
	    if (!strcmp(name, entry->name))
		break;
	}
    } else
	prev = &(colorTab[h]);

    if (!entry && create && (entry = xalloc(sizeof(dbEntry)))) {
	*prev = entry;
	entry->link = NULL;
	entry->name = xstrdup(name);
    }

    DEALLOCATE_LOCAL(name);

    return entry;
}

#ifndef __UNIXOS2__
#define RGB_SCANF_FMT "%d %d %d %[^\n]\n"
#else
#define RGB_SCANF_FMT "%d %d %d %[^\n\r]\n"
#endif
#define RGB_SCALE_FACTOR (65535 / 255)

Bool
OsInitColors(void)
{
    FILE *rgb;
    char *path;
    const char *fullpath;
    char line[BUFSIZ];
    char name[BUFSIZ];
    int red, green, blue, lineno = 0;
    dbEntryPtr entry;

    if (serverGeneration != 1)
	return TRUE;

#ifndef __UNIXOS2__
    fullpath = rgbPath;
#else
    fullpath = __XOS2RedirRoot(rgbPath);
#endif
    xasprintf(&path, "%s.txt", fullpath);
    if (!path)
	return FALSE;

    if (!(rgb = fopen(path, "r"))) {
#ifdef BUILTIN_RGB
	LogMessageVerb(X_WARNING, 0,
			"Could not open RGB file \"%s\"; "
			"will use built-in copy.\n", path);
	xfree(path);
	colorTab = builtinRGBhashTab;
	return TRUE;
#else
	LogMessage(X_ERROR, "Could not open RGB_DB '%s'.\n", path);
	xfree(path);
	return FALSE;
#endif
    }

    colorTab = hashTab;
    while (fgets(line, sizeof(line), rgb)) {
	lineno++;
	if (sscanf(line, RGB_SCANF_FMT, &red, &green, &blue, name) == 4) {
	    if (red >= 0   && red <= 0xff &&
		green >= 0 && green <= 0xff &&
		blue >= 0  && blue <= 0xff) {
		if ((entry = lookup(name, strlen(name), TRUE))) {
		    entry->red = red * RGB_SCALE_FACTOR;
		    entry->green = green * RGB_SCALE_FACTOR;
		    entry->blue = blue * RGB_SCALE_FACTOR;
		}
	    } else {
		LogMessageVerb(X_WARNING, 0,
			       "OsInitColors: %s:%d: Bad rgb value: "
			       "(%d:%d:%d).\n",
			       path, lineno, red, green, blue);
	    }
	} else if (*line && *line != '#' && *line != '!') {
	    LogMessageVerb(X_WARNING, 0,
			   "OsInitColors: %s:%d: Syntax error:\n",
			   path, lineno);
	    LogMessageVerb(X_WARNING, 0, "\t\"%.*s\"\n",
			   (int)strlen(line) - 1, line);
	}
    }

    fclose(rgb);
    xfree(path);

    return TRUE;
}



Bool
OsLookupColor(int screen, char *name, unsigned int len, 
    unsigned short *pred, unsigned short *pgreen, unsigned short *pblue)
{
    dbEntryPtr entry;

    if ((entry = lookup(name, len, FALSE))) {
	*pred   = entry->red;
	*pgreen = entry->green;
	*pblue  = entry->blue;
	return TRUE;
    }

    return FALSE;
}

