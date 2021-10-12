/* $XFree86: xc/programs/Xserver/hw/xfree86/testmod/test2mod1.c,v 1.2 2006/03/16 16:50:35 dawes Exp $ */
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

#include "xf86Module.h"
#include "xf86.h"

static XF86ModuleVersionInfo VersRec = {
    "test2mod",
    MODULEVENDORSTRING,
    MODINFOSTRING1,
    MODINFOSTRING2,
    XF86_VERSION_CURRENT,
    0, 1, 1,
    ABI_CLASS_ANSIC,
    ABI_ANSIC_VERSION,
    MOD_CLASS_NONE,
    {0, 0, 0, 0}
};

static MODULESETUPPROTO(test2modSetup);
static MODULETEARDOWNPROTO(test2modTearDown);

XF86ModuleData test2modModuleData =
	{ &VersRec, test2modSetup, test2modTearDown };

static pointer
test2modSetup(ModuleDescPtr module, pointer opts, int *errmaj, int *errmin)
{
    static Bool setupDone = FALSE;

    ErrorF("test2modSetup: This module (%p) has %salready been loaded\n",
		module, setupDone ? "" : "NOT ");
    setupDone = TRUE;
    xf86LoaderModRefSymbols(module, "testmodFunction1", NULL);

    return (pointer)1;
}

static void
test2modTearDown(pointer data)
{
    ErrorF("test2modTearDown: called with data %p\n", data);
}

extern int testmodFunction1(int value);
extern int testmodFunction2(int value);

int test2modFunction1(int value);
int test2modFunction2(int value);

#if 1
char *test2modExportedSymbols[] = {
    "test2modFunction1",
    NULL
};
#endif

int
test2modFunction1(int value)
{
    ErrorF("test2modFunction1: called with value %d\n", value);
    return testmodFunction1(value);
}

int
test2modFunction2(int value)
{
    ErrorF("test2modFunction2: called with value %d\n", value);
    return testmodFunction2(value);
}


