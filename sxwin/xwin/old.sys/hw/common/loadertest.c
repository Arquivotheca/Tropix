/* $XFree86: xc/programs/Xserver/hw/xfree86/common/loadertest.c,v 1.1 2006/03/02 03:00:36 dawes Exp $ */

/*
 * Copyright © 2003-2006 David H. Dawes.
 * Copyright © 2003-2006 X-Oz Technologies.
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

#include "loaderProcs.h"
#define XF86_OS_PRIVS
#include "xf86.h"
#include "xf86Priv.h"
#include "loadertest.h"

typedef void (*msgProcPtr)(char *, ...);
typedef int (*testProcPtr)(int);

#ifndef CATCH_SEGV
#define CATCH_SEGV 0
#endif

#if CATCH_SEGV
#include <setjmp.h>

jmp_buf ltEnv;

static void
ltSignal(int sig)
{
    ErrorF("ltSignal: caught signal %d\n", sig);
    longjmp(ltEnv, sig);
    signal(SIGSEGV, ltSignal);
}
#else
#define setjmp(x) 0
#endif

void
LoaderTest(void)
{
    void *mod1, *mod2, *sym1 = NULL, *sym2 = NULL, *sym3 = NULL;
    msgProcPtr fptr;
    testProcPtr tptr;
    int status = 0;

#if CATCH_SEGV
    ErrorF("LoaderTest: Installing signal handlers.\n");
    signal(SIGSEGV, ltSignal);
#endif

    ErrorF("LoaderTest: Loading module \"testmod\".\n");
    mod2 = xf86LoadOneModule("testmod", NULL);
    if (!mod2) {
	ErrorF("LoaderTest: Cannot load \"testmod\".  Exiting.\n");
	exit(1);
    }
    tptr = (testProcPtr)LoaderSymbol("testmodFunction3");
    if (!tptr) {
	ErrorF("LoaderTest: Cannot find symbol \"testmodFunction3\"."
		"  Exiting.\n");
	exit(1);
    }

    status = setjmp(ltEnv);
    if (!status) {
	tptr(0);
    } else {
	ErrorF("LoaderTest: caught signal %d in tptr(0) call.\n", status);
    }

    ErrorF("LoaderTest: Loading module \"bitmap\".\n");
    mod1 = xf86LoadOneModule("bitmap", NULL);
    if (!mod1)
	ErrorF("LoaderTest: Loading module \"bitmap\" failed.\n");

    if (mod1) {
	sym1 = LoaderSymbol("bdfError");
	ErrorF("LoaderTest: bdfError is %p.\n", sym1);
	ErrorF("LoaderTest: CheckUnresolved.\n");
	LoaderCheckUnresolved(0);
	fptr = (msgProcPtr)sym1;
	fptr("Hello from bdfError\n");
	status = setjmp(ltEnv);
	if (!status) {
	    tptr(1);
	} else {
	    ErrorF("LoaderTest: caught signal %d in tptr(1) call.\n", status);
	}
	ErrorF("LoaderTest: Unloading module \"bitmap\".\n");
	UnloadModule(mod1);
	ErrorF("LoaderTest: CheckUnresolved.\n");
	LoaderCheckUnresolved(0);
	sym2 = LoaderSymbol("bdfError");
	ErrorF("LoaderTest: bdfError is %p.\n", sym2);
	status = setjmp(ltEnv);
	if (!status) {
	    tptr(2);
	} else {
	    ErrorF("LoaderTest: caught signal %d in tptr(2) call.\n", status);
	}
    }

    ErrorF("LoaderTest: Loading module \"bitmap\" again.\n");
    mod1 = xf86LoadOneModule("bitmap", NULL);
    if (!mod1)
	ErrorF("LoaderTest: Loading module \"bitmap\" failed.\n");

    if (mod1) {
	sym3 = LoaderSymbol("bdfError");
	ErrorF("LoaderTest: bdfError is %p (was %p, %p).\n",
		sym3, sym1, sym2);
	ErrorF("LoaderTest: CheckUnresolved.\n");
	LoaderCheckUnresolved(0);
	fptr = (msgProcPtr)sym3;
	fptr("Hello from bdfError\n");
	status = setjmp(ltEnv);
	if (!status) {
	    tptr(3);
	} else {
	    ErrorF("LoaderTest: caught signal %d in tptr(3) call.\n", status);
	}
	ErrorF("LoaderTest: Unloading module \"bitmap\" again.\n");
	UnloadModule(mod1);
	ErrorF("LoaderTest: CheckUnresolved.\n");
	LoaderCheckUnresolved(0);
	sym2 = LoaderSymbol("bdfError");
	ErrorF("LoaderTest: bdfError is %p.\n", sym2);
	status = setjmp(ltEnv);
	if (!status) {
	    tptr(4);
	} else {
	    ErrorF("LoaderTest: caught signal %d in tptr(4) call.\n", status);
	}
    }

    ErrorF("LoaderTest: exiting normally.\n");
    exit(0);
}

