/* $XFree86: xc/programs/Xserver/hw/xfree86/os-support/linux/lnx_pci.c,v 3.12 2007/02/13 18:30:10 tsi Exp $ */

#include <stdio.h>
#include <X11/X.h>
#include "os.h"
#include "xf86.h"
#include "xf86Priv.h"
#define XF86_OS_PRIVS
#include "xf86_OSproc.h"
#include "xf86Pci.h"

#ifdef __sparc__
#define PCIADDR_TYPE		long long
#define PCIADDR_FMT		"%llx"
#else
#define PCIADDR_TYPE		long
#define PCIADDR_FMT		"%lx"
#endif

Bool
xf86GetPciSizeFromOS(PCITAG tag, int index, int* bits)
{
    FILE *file;
    char c[0x200];
    char *res;
    unsigned int bus, devfn, dev, fn;
    unsigned PCIADDR_TYPE size[7];
    unsigned int num;
    signed PCIADDR_TYPE Size;

    if ((index < 0) || (index > 7))
	return FALSE;

    if (!(file = fopen("/proc/bus/pci/devices", "r")))
	return FALSE;

    while ((res = fgets(c, sizeof(c) - 1, file))) {
	num = sscanf(res,
		     /*bus+dev vendorid deviceid irq */
		     "%02x%02x\t%*04x%*04x\t%*x"
		     /* 7 resource base addresses */
		     "\t%*x\t%*x\t%*x\t%*x\t%*x\t%*x\t%*x"
		     /* 7 resource sizes, and then an optional driver name */
		     "\t" PCIADDR_FMT
		     "\t" PCIADDR_FMT
		     "\t" PCIADDR_FMT
		     "\t" PCIADDR_FMT
		     "\t" PCIADDR_FMT
		     "\t" PCIADDR_FMT
		     "\t" PCIADDR_FMT,
		     &bus, &devfn, &size[0], &size[1], &size[2], &size[3],
		     &size[4], &size[5], &size[6]);

	if (num != 9)	/* apparently not 2.3 style */
	    break;

	dev = devfn >> 3;
	fn = devfn & 0x7;
	if (tag == pciTag(bus, dev, fn)) {
	    fclose(file);
	    *bits = 0;

	    if (index == 7)	/* P2P bridge ROM pointer */
		index = 6;

	    if (size[index] == 0)
		return TRUE;

	    Size = size[index] - ((PCIADDR_TYPE) 1);
	    if (Size & size[index])
		return FALSE;

	    while (Size & ((PCIADDR_TYPE) 0x01)) {
		Size >>= 1;
		(*bits)++;

	    return TRUE;
	}
    }

    fclose(file);
    return FALSE;
}
