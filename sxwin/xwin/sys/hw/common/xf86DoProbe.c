/* $XFree86: xc/programs/Xserver/hw/xfree86/common/xf86DoProbe.c,v 1.21 2007/02/05 15:03:46 tsi Exp $ */
/*
 * Copyright (c) 1999-2005 by The XFree86 Project, Inc.
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
 * finish setting up the server
 * Load the driver modules and call their probe functions.
 */

#include <ctype.h>
#include <stdlib.h>
#include <X11/X.h>
#include <X11/Xmd.h>
#include "os.h"
#ifdef XFree86LOADER
#include "loaderProcs.h"
#endif /* XFree86LOADER */
#include "xf86.h"
#include "xf86Priv.h"
#ifdef XFree86LOADER
#include "xf86Config.h"
#endif /* XFree86LOADER */

void
DoProbeArgs(int argc, const char **argv, int i)
{
}

void
DoProbe()
{
    int i;
    Bool probeResult;

#ifdef XFree86LOADER
    /* Find the list of video driver modules. */
    const char **list = xf86DriverlistFromCompile();
    const char **l;

    if (list) {
	ErrorF("List of video driver modules:\n");
	for (l = list; *l; l++)
	    ErrorF("\t%s\n", *l);
    } else {
	ErrorF("No video driver modules found\n");
    }

    /* Load all the drivers that were found. */
    xf86LoadModules(list, NULL);
#endif /* XFree86LOADER */

    /* Disable PCI devices */
    xf86AccessInit();

    /* Call all of the probe functions, reporting the results. */
    for (i = 0; i < xf86NumDrivers; i++) {
	if (xf86DriverList[i]->Probe == NULL) continue;

	xf86MsgVerb(X_INFO, 3, "Probing in driver %s\n",
	    xf86DriverList[i]->driverName);
	probeResult =
	    (*xf86DriverList[i]->Probe)(xf86DriverList[i], PROBE_DETECT);
	if (!probeResult) {
	    xf86ErrorF("Probe in driver `%s' returns FALSE\n",
		xf86DriverList[i]->driverName);
	} else {
	    xf86ErrorF("Probe in driver `%s' returns TRUE\n",
		xf86DriverList[i]->driverName);

	    /* If we have a result, then call driver's Identify function */
	    if (xf86DriverList[i]->Identify != NULL) {
		int verbose = xf86SetVerbosity(1);
		(*xf86DriverList[i]->Identify)(0);
		xf86SetVerbosity(verbose);
	    }
	}
    }

    CloseWellKnownConnections();
    OsCleanup(TRUE);
    AbortDDX();
    fflush(stderr);
    exit(0);
}
