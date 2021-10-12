/*
 * Copyright 2000 by Egbert Eich
 * Copyright 1995 by Robin Cutshaw <robin@XFree86.Org>
 * Copyright 2002 by David Dawes
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of the above listed copyright holder(s)
 * not be used in advertising or publicity pertaining to distribution of
 * the software without specific, written prior permission.  The above listed
 * copyright holder(s) make(s) no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without express or
 * implied warranty.
 *
 * THE ABOVE LISTED COPYRIGHT HOLDER(S) DISCLAIM(S) ALL WARRANTIES WITH REGARD
 * TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS, IN NO EVENT SHALL THE ABOVE LISTED COPYRIGHT HOLDER(S) BE
 * LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY
 * DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
 * IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING
 * OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */
/* $XFree86: xc/programs/Xserver/hw/xfree86/etc/scanpci.c,v 3.99 2007/05/30 15:50:40 tsi Exp $ */

#include <X11/X.h>
#include "os.h"
#include "xf86.h"
#include "xf86Priv.h"
#include "xf86_OSproc.h"
#include "Pci.h"
#include "xf86PciInfo.h"
#include "xf86ScanPci.h"
#include "dummylib.h"

#include <stdarg.h>
#include <stdlib.h>
#ifdef __linux__
/* to get getopt on Linux */
#ifndef __USE_POSIX2
#define __USE_POSIX2
#endif
#endif
#include <unistd.h>

#if defined(ISC) || defined(Lynx)
extern char *optarg;
extern int optind, opterr;
#endif

pciVideoPtr *xf86PciVideoInfo = NULL;

static void usage(void);
static void identify_card(pciConfigPtr pcr, int verbose);
static void dump_config(pciConfigPtr pcr, int verbose);
static void print_header_type_0(pciConfigPtr pcr);
static void print_header_type_1(pciConfigPtr pcr);
static void print_header_type_2(pciConfigPtr pcr);
static void print_simba(pciConfigPtr pcr);
static void print_460gx_sac(pciConfigPtr pcr);
static void print_460gx_pxb(pciConfigPtr pcr);
static void print_460gx_gxb(pciConfigPtr pcr);

#define MAX_DEV_PER_VENDOR 8

typedef struct {
    unsigned int Vendor;
    struct {
	int DeviceID;
	void(*func)(pciConfigPtr);
    } Device[MAX_DEV_PER_VENDOR];
} pciVendorDevFuncInfo;

static pciVendorDevFuncInfo vendorDeviceFuncInfo[] = {
    { PCI_VENDOR_SUN, {
	{ PCI_CHIP_SIMBA, print_simba },
	{ 0x0000, NULL } } },
    { PCI_VENDOR_INTEL, {
	{ PCI_CHIP_460GX_SAC, print_460gx_sac },
	{ PCI_CHIP_460GX_PXB, print_460gx_pxb },
	{ PCI_CHIP_460GX_GXB_1, print_460gx_gxb },
	{ PCI_CHIP_460GX_WXB, print_460gx_pxb },	/* Uncertain */
	{ 0x0000, NULL } } },
    { 0x0000, {
	{ 0x0000, NULL } } }
};

static void
usage(void)
{
    printf("Usage: scanpci [-v12OfV]\n");
    printf("           -v print config space verbosely\n");
    printf("           -x print config space in hexadecimal\n");
    printf("           -1 config type 1\n");
    printf("           -2 config type 2\n");
    printf("           -O use OS config support\n");
    printf("           -f force config type\n");
    printf("           -V set message verbosity level\n");
}

int
main(int argc, char *argv[])
{
    pciConfigPtr pPCI, *pcrpp = NULL;
    int Verbose = 0, force = 0, dump = 0, i = 0, c;

    xf86Info.pciFlags = PCIProbe1;

    while ((c = getopt(argc, argv, "?v12OfdxaV:")) != -1)
	switch(c) {
	case 'v':
	    Verbose++;
	    break;
	case '1':
	    xf86Info.pciFlags = PCIProbe1;
	    break;
	case '2':
	    xf86Info.pciFlags = PCIProbe2;
	    break;
	case 'O':
	    xf86Info.pciFlags = PCIOsConfig;
	    break;
	case 'f':
	    force = 1;
	    break;
	case 'V':
	    xf86Verbose = atoi(optarg);
	    break;
	case 'd':
	case 'x':
	    dump++;
	    break;
	case 'a':	/* Not documented */
	    xf86Info.estimateSizesAggressively++;
	    break;
	case '?':
	default:
	    usage();
	    exit (1);
	    break;
	}

    if (force)
	switch (xf86Info.pciFlags) {
	case PCIProbe1:
	    xf86Info.pciFlags = PCIForceConfig1;
	    break;
	case PCIProbe2:
	    xf86Info.pciFlags = PCIForceConfig2;
	    break;
	default:
	    break;
	}

    xf86EnableIO();
    pcrpp = xf86scanpci(0);

    if (!pcrpp) {
	printf("No PCI devices found\n");
	xf86DisableIO();
	exit (1);
    }

    while ((pPCI = pcrpp[i++])) {
	identify_card(pPCI, Verbose);
	if (dump)
	    dump_config(pPCI, dump);
    }

    if (Verbose > 1) {
	printf("\nPCI bus linkages:\n\n");

	for (i = 0;  i < MAX_PCI_BUSES;  i++) {
	    pciBusInfo_t *pBusInfo;

	    if (!(pBusInfo = pciBusInfo[i]))
		continue;

	    if ((pPCI = pBusInfo->bridge))
		printf("PCI bus 0x%04x has parent bridge 0x%04x:0x%02x:0x%1x\n",
		       i, pPCI->busnum, pPCI->devnum, pPCI->funcnum);
	    else
		printf("PCI bus 0x%04x has no parent\n", i);
	}
    }

    xf86DisableIO();
    exit(0);
}

static void
identify_card(pciConfigPtr pcr, int verbose)
{
    int i, j;
    int foundit = 0;
    int foundvendor = 0;
    const char *vname, *dname, *svname, *sname;

    pciVendorDevFuncInfo *vdf = vendorDeviceFuncInfo;

    if (!ScanPciSetupPciIds()) {
	fprintf(stderr, "xf86SetupPciIds() failed\n");
	exit(1);
    }

    printf("\npci bus 0x%04x cardnum 0x%02x function 0x%02x:"
	   " vendor 0x%04x device 0x%04x\n",
	   pcr->busnum, pcr->devnum, pcr->funcnum,
	   pcr->pci_vendor, pcr->pci_device);

    ScanPciFindPciNamesByDevice(pcr->pci_vendor, pcr->pci_device,
			     pcr->pci_subsys_vendor, pcr->pci_subsys_card,
			     &vname, &dname, &svname, &sname);

    if (vname) {
	printf(" %s ", vname);
	if (dname) {
	    printf("%s", dname);
	    foundit = 1;
	}
    }

    if (!foundit)
	printf(" Device unknown\n");
    else
	printf("\n");

    if (verbose <= 0)
	return;

    if (!(pcr->pci_header_type & 0x7f) &&
	(((pcr->pci_subsys_vendor != 0) &&
	  (pcr->pci_subsys_vendor != 0xffffu)) ||
         ((pcr->pci_subsys_card != 0) &&
	  (pcr->pci_subsys_card != 0xffffu))) &&
	((pcr->pci_vendor != pcr->pci_subsys_vendor) ||
	 (pcr->pci_device != pcr->pci_subsys_card))) {
	foundit = 0;
	foundvendor = 0;
	printf(" CardVendor 0x%04x card 0x%04x",
	       pcr->pci_subsys_vendor, pcr->pci_subsys_card);
	if (svname) {
	    printf(" (%s", svname);
	    foundvendor = 1;
	    if (sname) {
		printf(" %s)", sname);
		foundit = 1;
	    }
	}

	if (!foundit) {
	    if (!foundvendor)
		printf(" (");
	    else
		printf(", ");
	    printf("Card unknown)");
	}
	printf("\n");
    }

    printf("  STATUS    0x%04x  COMMAND 0x%04x\n",
	   pcr->pci_status, pcr->pci_command);
    printf("  CLASS     0x%02x 0x%02x 0x%02x  REVISION 0x%02x\n",
	   pcr->pci_base_class, pcr->pci_sub_class, pcr->pci_prog_if,
	   pcr->pci_rev_id);
    printf("  BIST      0x%02x  HEADER 0x%02x  LATENCY 0x%02x  CACHE 0x%02x\n",
	   pcr->pci_bist, pcr->pci_header_type, pcr->pci_latency_timer,
	   pcr->pci_cache_line_size);

    for (i = 0;  vdf[i].Vendor;  i++) {
	if (vdf[i].Vendor != pcr->pci_vendor)
	    continue;

	for (j = 0;  vdf[i].Device[j].DeviceID;  j++) {
	    if (vdf[i].Device[j].DeviceID != pcr->pci_device)
		continue;

	    (*vdf[i].Device[j].func)(pcr);
	    return;
	}

	break;
    }

    switch (pcr->pci_header_type & 0x7f) {
    case 0:
	print_header_type_0(pcr);
	break;

    case 1:
	print_header_type_1(pcr);
	break;

    case 2:
	print_header_type_2(pcr);
	break;

    default:
	printf("  Unknown header type 0x%02x\n", pcr->pci_header_type);
	break;
    }
}

static void
dump_config(pciConfigPtr pcr, int verbose)
{
    int i, limit;

    if ((pcr->pci_header_type & 0x7f) == 2)
	limit = 32;
    else
	limit = 16;

    for (i = 0;  i < limit;  i++) {
	if (!(i & 3))
	    printf("    0x%03x: ", i * 4);
	if (!(i & 1))
	    printf(" ");
	/* Always display little-endian */
	printf(" %02x%02x%02x%02x",
	       (CARD8)(pcr->cfgspc.dwords[i]),
	       (CARD8)(pcr->cfgspc.dwords[i] >> 8),
	       (CARD8)(pcr->cfgspc.dwords[i] >> 16),
	       (CARD8)(pcr->cfgspc.dwords[i] >> 24));
	if ((i & 3) == 3)
	    printf("\n");
    }

    if (verbose <= 1)
	return;

    i = limit * 4;
    if (verbose > 2)
	limit = 4096;
    else
	limit = 256;

    /* Print the rest */
    for (;  i < limit;  i += 4) {
	CARD32 pcireg = pciReadLong(pcr->tag, i);
	if (!(i & 15))
	    printf("    0x%03x: ", i);
	if (!(i & 7))
	    printf(" ");
	/* Always display little-endian */
	printf(" %02x%02x%02x%02x",
	       (CARD8)(pcireg), (CARD8)(pcireg >> 8),
	       (CARD8)(pcireg >> 16), (CARD8)(pcireg >> 24));
	if ((i & 15) == 12)
	    printf("\n");
    }
}

static void
print_header_type_0(pciConfigPtr pcr)
{
    if (pcr->basesize[0]) {
	if ((pcr->pci_base0 & 0x7) == 0x4) {
	    printf("  BASE0     0x%08x%08x  addr 0x%08x%08x",
		   (int)pcr->pci_base1, (int)pcr->pci_base0,
		   (int)pcr->pci_base1,
		   (int)(pcr->pci_base0 &
			 (pcr->pci_base0 & 0x1 ? 0xFFFFFFFC : 0xFFFFFFF0)));
	    printf("  %ssize ",
		   (pcr->minBasesize & (2 << 0)) ? "" : "estimated ");
	    if (pcr->basesize[0] < 32)
		printf("0x%08x", 1 << pcr->basesize[0]);
	    else
		printf("0x%08x00000000", 1 << (pcr->basesize[0] - 32));
	    printf("  MEM%s 64BIT\n",
		   (pcr->pci_base0 & 0x8) ? " PREFETCHABLE" : "");
	} else {
	    printf("  BASE0     0x%08x  addr 0x%08x",
		   (int)pcr->pci_base0,
		   (int)(pcr->pci_base0 &
			 (pcr->pci_base0 & 0x1 ? 0xFFFFFFFC : 0xFFFFFFF0)));
	    printf("  %ssize 0x%08x",
		   (pcr->minBasesize & (2 << 0)) ? "" : "estimated ",
		   1 << pcr->basesize[0]);
	    printf("  %s%s\n",
		   (pcr->pci_base0 & 0x1) ? "I/O" : "MEM",
		   ((pcr->pci_base0 & 0x9) == 0x8) ? " PREFETCHABLE" : "");
	}
    }

    if (pcr->basesize[1]) {
	if ((pcr->pci_base1 & 0x7) == 0x4) {
	    printf("  BASE1     0x%08x%08x  addr 0x%08x%08x",
		   (int)pcr->pci_base2, (int)pcr->pci_base1,
		   (int)pcr->pci_base2,
		   (int)(pcr->pci_base1 &
			 (pcr->pci_base1 & 0x1 ? 0xFFFFFFFC : 0xFFFFFFF0)));
	    printf("  %ssize ",
		   (pcr->minBasesize & (2 << 1)) ? "" : "estimated ");
	    if (pcr->basesize[1] < 32)
		printf("0x%08x", 1 << pcr->basesize[1]);
	    else
		printf("0x%08x00000000", 1 << (pcr->basesize[1] - 32));
	    printf("  MEM%s 64BIT\n",
		   (pcr->pci_base1 & 0x8) ? " PREFETCHABLE" : "");
	} else {
	    printf("  BASE1     0x%08x  addr 0x%08x",
		   (int)pcr->pci_base1,
		   (int)(pcr->pci_base1 &
			 (pcr->pci_base1 & 0x1 ? 0xFFFFFFFC : 0xFFFFFFF0)));
	    printf("  %ssize 0x%08x",
		   (pcr->minBasesize & (2 << 1)) ? "" : "estimated ",
		   1 << pcr->basesize[1]);
	    printf("  %s%s\n",
		   (pcr->pci_base1 & 0x1) ? "I/O" : "MEM",
		   ((pcr->pci_base1 & 0x9) == 0x8) ? " PREFETCHABLE" : "");
	}
    }

    if (pcr->basesize[2]) {
	if ((pcr->pci_base2 & 0x7) == 0x4) {
	    printf("  BASE2     0x%08x%08x  addr 0x%08x%08x",
		   (int)pcr->pci_base3, (int)pcr->pci_base2,
		   (int)pcr->pci_base3,
		   (int)(pcr->pci_base2 &
			 (pcr->pci_base2 & 0x1 ? 0xFFFFFFFC : 0xFFFFFFF0)));
	    printf("  %ssize ",
		   (pcr->minBasesize & (2 << 2)) ? "" : "estimated ");
	    if (pcr->basesize[2] < 32)
		printf("0x%08x", 1 << pcr->basesize[2]);
	    else
		printf("0x%08x00000000", 1 << (pcr->basesize[2] - 32));
	    printf("  MEM%s 64BIT\n",
		   (pcr->pci_base2 & 0x8) ? " PREFETCHABLE" : "");
	} else {
	    printf("  BASE2     0x%08x  addr 0x%08x",
		   (int)pcr->pci_base2,
		   (int)(pcr->pci_base2 &
			 (pcr->pci_base2 & 0x1 ? 0xFFFFFFFC : 0xFFFFFFF0)));
	    printf("  %ssize 0x%08x",
		   (pcr->minBasesize & (2 << 2)) ? "" : "estimated ",
		   1 << pcr->basesize[2]);
	    printf("  %s%s\n",
		   (pcr->pci_base2 & 0x1) ? "I/O" : "MEM",
		   ((pcr->pci_base2 & 0x9) == 0x8) ? " PREFETCHABLE" : "");
	}
    }

    if (pcr->basesize[3]) {
	if ((pcr->pci_base3 & 0x7) == 0x4) {
	    printf("  BASE3     0x%08x%08x  addr 0x%08x%08x",
		   (int)pcr->pci_base4, (int)pcr->pci_base3,
		   (int)pcr->pci_base4,
		   (int)(pcr->pci_base3 &
			 (pcr->pci_base3 & 0x1 ? 0xFFFFFFFC : 0xFFFFFFF0)));
	    printf("  %ssize ",
		   (pcr->minBasesize & (2 << 3)) ? "" : "estimated ");
	    if (pcr->basesize[3] < 32)
		printf("0x%08x", 1 << pcr->basesize[3]);
	    else
		printf("0x%08x00000000", 1 << (pcr->basesize[3] - 32));
	    printf("  MEM%s 64BIT\n",
		   (pcr->pci_base3 & 0x8) ? " PREFETCHABLE" : "");
	} else {
	    printf("  BASE3     0x%08x  addr 0x%08x",
		   (int)pcr->pci_base3,
		   (int)(pcr->pci_base3 &
			 (pcr->pci_base3 & 0x1 ? 0xFFFFFFFC : 0xFFFFFFF0)));
	    printf("  %ssize 0x%08x",
		   (pcr->minBasesize & (2 << 3)) ? "" : "estimated ",
		   1 << pcr->basesize[3]);
	    printf("  %s%s\n",
		   (pcr->pci_base3 & 0x1) ? "I/O" : "MEM",
		   ((pcr->pci_base3 & 0x9) == 0x8) ? " PREFETCHABLE" : "");
	}
    }

    if (pcr->basesize[4]) {
	if ((pcr->pci_base4 & 0x7) == 0x4) {
	    printf("  BASE4     0x%08x%08x  addr 0x%08x%08x",
		   (int)pcr->pci_base5, (int)pcr->pci_base4,
		   (int)pcr->pci_base5,
		   (int)(pcr->pci_base4 &
			 (pcr->pci_base4 & 0x1 ? 0xFFFFFFFC : 0xFFFFFFF0)));
	    printf("  %ssize ",
		   (pcr->minBasesize & (2 << 4)) ? "" : "estimated ");
	    if (pcr->basesize[4] < 32)
		printf("0x%08x", 1 << pcr->basesize[4]);
	    else
		printf("0x%08x00000000", 1 << (pcr->basesize[4] - 32));
	    printf("  MEM%s 64BIT\n",
		   (pcr->pci_base4 & 0x8) ? " PREFETCHABLE" : "");
	} else {
	    printf("  BASE4     0x%08x  addr 0x%08x",
		   (int)pcr->pci_base4,
		   (int)(pcr->pci_base4 &
			 (pcr->pci_base4 & 0x1 ? 0xFFFFFFFC : 0xFFFFFFF0)));
	    printf("  %ssize 0x%08x",
		   (pcr->minBasesize & (2 << 4)) ? "" : "estimated ",
		   1 << pcr->basesize[4]);
	    printf("  %s%s\n",
		   (pcr->pci_base4 & 0x1) ? "I/O" : "MEM",
		   ((pcr->pci_base4 & 0x9) == 0x8) ? " PREFETCHABLE" : "");
	}
    }

    if (pcr->basesize[5]) {
	if ((pcr->pci_base5 & 0x7) == 0x4) {
	    printf("  BASE5     0x%08x  (invalid)\n", (int)pcr->pci_base5);
	} else {
	    printf("  BASE5     0x%08x  addr 0x%08x",
		   (int)pcr->pci_base5,
		   (int)(pcr->pci_base5 &
			(pcr->pci_base5 & 0x1 ?  0xFFFFFFFC : 0xFFFFFFF0)));
	    printf("  %ssize 0x%08x",
		   (pcr->minBasesize & (2 << 5)) ? "" : "estimated ",
		   1 << pcr->basesize[5]);
	    printf("  %s%s\n",
		   (pcr->pci_base5 & 0x1) ? "I/O" : "MEM",
		   ((pcr->pci_base5 & 0x9) == 0x8) ? " PREFETCHABLE" : "");
	}
    }

    if (pcr->basesize[6]) {
	printf("  BASEROM   0x%08x  addr 0x%08x",
	       (int)pcr->pci_baserom, (int)(pcr->pci_baserom & 0xFFFFF800));
	printf("  %ssize 0x%08x",
	       (pcr->minBasesize & (2 << 6)) ? "" : "estimated ",
	       1 << pcr->basesize[6]);
	printf("  %senabled\n",
	       pcr->pci_baserom & 0x1 ? "" : "not ");
    }

    if (pcr->pci_max_min_ipin_iline)
	printf("  MAX_LAT   0x%02x  MIN_GNT 0x%02x"
	       "  INT_PIN 0x%02x  INT_LINE 0x%02x\n",
	       pcr->pci_max_lat, pcr->pci_min_gnt,
	       pcr->pci_int_pin, pcr->pci_int_line);

    if (pcr->pci_user_config)
	printf("  BYTE_0    0x%02x  BYTE_1  0x%02x"
	       "  BYTE_2  0x%02x  BYTE_3   0x%02x\n",
	       (int)pcr->pci_user_config_0, (int)pcr->pci_user_config_1,
	       (int)pcr->pci_user_config_2, (int)pcr->pci_user_config_3);
}

static void
print_header_type_1(pciConfigPtr pcr)
{
    printf("  PRIBUS    0x%02x  SECBUS 0x%02x  SUBBUS 0x%02x\n",
	   pcr->pci_primary_bus_number, pcr->pci_secondary_bus_number,
	   pcr->pci_subordinate_bus_number);
    printf("  SECLT     0x%02x  SECSTATUS 0x%04x\n",
	   pcr->pci_secondary_latency_timer, pcr->pci_secondary_status);

    if (pcr->pci_io_base || pcr->pci_io_limit ||
	pcr->pci_upper_io_base || pcr->pci_upper_io_limit) {
	if (((pcr->pci_io_base & 0x0f) == 0x01) ||
	    ((pcr->pci_io_limit & 0x0f) == 0x01)) {
	    if ((pcr->pci_upper_io_base < pcr->pci_upper_io_limit) ||
		((pcr->pci_upper_io_base == pcr->pci_upper_io_limit) &&
		 ((pcr->pci_io_base & 0x00f0) <=
		  (pcr->pci_io_limit & 0x00f0))))
		printf("  IOBASE    0x%04x%04x  IOLIM 0x%04x%04x\n",
		       pcr->pci_upper_io_base, (pcr->pci_io_base & 0x00f0) << 8,
		       pcr->pci_upper_io_limit,
		       (pcr->pci_io_limit << 8) | 0x0fff);
	} else if ((pcr->pci_io_base & 0x00f0) <=
		   (pcr->pci_io_limit & 0x00f0)) {
	    printf("  IOBASE    0x%04x  IOLIM 0x%04x\n",
		   (pcr->pci_io_base & 0x00f0) << 8,
		   (pcr->pci_io_limit << 8) | 0x0fff);
	}
    }

    if ((pcr->pci_mem_base || pcr->pci_mem_limit) &&
	((pcr->pci_mem_base & 0x00fff0) <= (pcr->pci_mem_limit & 0x00fff0)))
	printf("  NOPREFETCH_MEMBASE 0x%08x  MEMLIM 0x%08x\n",
	       (pcr->pci_mem_base & 0x00fff0) << 16,
	       (pcr->pci_mem_limit << 16) | 0x0fffff);

    if (pcr->pci_prefetch_mem_base || pcr->pci_prefetch_mem_limit ||
	pcr->pci_prefetch_upper_mem_base ||
	pcr->pci_prefetch_upper_mem_limit) {
	if (((pcr->pci_prefetch_mem_base & 0x0f) == 0x01) ||
	    ((pcr->pci_prefetch_mem_limit & 0x0f) == 0x01)) {
	    if ((pcr->pci_prefetch_upper_mem_base <
		 pcr->pci_prefetch_upper_mem_limit) ||
		((pcr->pci_prefetch_upper_mem_base ==
		  pcr->pci_prefetch_upper_mem_limit) &&
		 ((pcr->pci_prefetch_mem_base & 0x00fff0) <=
		  (pcr->pci_prefetch_mem_limit & 0x00fff0)))) {
		printf("  PREFETCH_MEMBASE   0x%08x%08x  MEMLIM 0x%08x%08x\n",
		       (int)pcr->pci_prefetch_upper_mem_base,
		       (pcr->pci_prefetch_mem_base & 0x00fff0) << 16,
		       (int)pcr->pci_prefetch_upper_mem_limit,
		       (pcr->pci_prefetch_mem_limit << 16) | 0x0fffff);
	    }
	} else if ((pcr->pci_prefetch_mem_base & 0x00fff0) <=
		   (pcr->pci_prefetch_mem_limit & 0x00fff0)) {
	    printf("  PREFETCH_MEMBASE   0x%08x  MEMLIM 0x%08x\n",
		   (pcr->pci_prefetch_mem_base & 0x00fff0) << 16,
		   (pcr->pci_prefetch_mem_limit << 16) | 0x0fffff);
	}
    }

    if (pcr->basesize[0]) {
	if ((pcr->pci_base0 & 0x7) == 0x4) {
	    printf("  BASE0     0x%08x%08x  addr 0x%08x%08x",
		   (int)pcr->pci_base1, (int)pcr->pci_base0,
		   (int)pcr->pci_base1,
		   (int)(pcr->pci_base0 &
			 (pcr->pci_base0 & 0x1 ? 0xFFFFFFFC : 0xFFFFFFF0)));
	    printf("  %ssize ",
		   (pcr->minBasesize & (2 << 0)) ? "" : "estimated ");
	    if (pcr->basesize[0] < 32)
		printf("0x%08x", 1 << pcr->basesize[0]);
	    else
		printf("0x%08x00000000", 1 << (pcr->basesize[0] - 32));
	    printf("  MEM%s 64BIT\n",
		   (pcr->pci_base0 & 0x8) ? " PREFETCHABLE" : "");
	} else {
	    printf("  BASE0     0x%08x  addr 0x%08x",
		   (int)pcr->pci_base0,
		   (int)(pcr->pci_base0 &
			 (pcr->pci_base0 & 0x1 ? 0xFFFFFFFC : 0xFFFFFFF0)));
	    printf("  %ssize 0x%08x",
		   (pcr->minBasesize & (2 << 0)) ? "" : "estimated ",
		   1 << pcr->basesize[0]);
	    printf("  %s%s\n",
		   (pcr->pci_base0 & 0x1) ? "I/O" : "MEM",
		   ((pcr->pci_base0 & 0x9) == 0x8) ? " PREFETCHABLE" : "");
	}
    }

    if (pcr->basesize[1]) {
	if ((pcr->pci_base1 & 0x7) == 0x4) {
	    printf("  BASE1     0x%08x (invalid)\n", (int)pcr->pci_base0);
	} else {
	    printf("  BASE1     0x%08x  addr 0x%08x",
		   (int)pcr->pci_base1,
		   (int)(pcr->pci_base1 &
			 (pcr->pci_base1 & 0x1 ? 0xFFFFFFFC : 0xFFFFFFF0)));
	    printf("  %ssize 0x%08x",
		   (pcr->minBasesize & (2 << 1)) ? "" : "estimated ",
		   1 << pcr->basesize[1]);
	    printf("  %s%s\n",
		   (pcr->pci_base1 & 0x1) ? "I/O" : "MEM",
		   ((pcr->pci_base1 & 0x9) == 0x8) ? " PREFETCHABLE" : "");
	}
    }

    if (pcr->basesize[6]) {
	printf("  BASEROM   0x%08x  addr 0x%08x",
	       (int)pcr->pci_br_rom, (int)(pcr->pci_br_rom & 0xFFFFF800));
	printf("  %ssize 0x%08x",
	       (pcr->minBasesize & (2 << 7)) ? "" : "estimated ",
	       1 << pcr->basesize[6]);
	printf("  %senabled\n",
	       pcr->pci_br_rom & 0x1 ? "" : "not ");
    }

    printf("  %sFAST_B2B %sSEC_BUS_RST %sM_ABRT %sVGA_EN %sISA_EN"
	   " %sSERR_EN %sPERR_EN\n",
	   (pcr->pci_bridge_control & PCI_PCI_BRIDGE_FAST_B2B_EN) ?
		"" : "NO_",
	   (pcr->pci_bridge_control & PCI_PCI_BRIDGE_SECONDARY_RESET) ?
		"" : "NO_",
	   (pcr->pci_bridge_control & PCI_PCI_BRIDGE_MASTER_ABORT_EN) ?
		"" : "NO_",
	   (pcr->pci_bridge_control & PCI_PCI_BRIDGE_VGA_EN) ?
		"" : "NO_",
	   (pcr->pci_bridge_control & PCI_PCI_BRIDGE_ISA_EN) ?
		"" : "NO_",
	   (pcr->pci_bridge_control & PCI_PCI_BRIDGE_SERR_EN) ?
		"" : "NO_",
	   (pcr->pci_bridge_control & PCI_PCI_BRIDGE_PARITY_EN) ?
		"" : "NO_");

    if (pcr->pci_int_pin || pcr->pci_int_line)
	printf("  INT_PIN 0x%02x  INT_LINE 0x%02x\n",
	       pcr->pci_int_pin, pcr->pci_int_line);
}

static void
print_header_type_2(pciConfigPtr pcr)
{
    printf("  PRIBUS    0x%02x  SECBUS 0x%02x  SUBBUS 0x%02x\n",
	   pcr->pci_cb_primary_bus_number, pcr->pci_cb_cardbus_bus_number,
	   pcr->pci_cb_subordinate_bus_number);
    printf("  SECLT     0x%02x  SECSTATUS 0x%04x\n",
	   pcr->pci_cb_latency_timer, pcr->pci_cb_secondary_status);

    if ((pcr->pci_cb_membase0 || pcr->pci_cb_memlimit0) &&
	((pcr->pci_cb_membase0 & 0xFFFFF000) <=
	 (pcr->pci_cb_memlimit0 & 0xFFFFF000)))
	printf("  MEMBASE0 0x%08x  MEMLIM0 0x%08x%s\n",
	       (int)pcr->pci_cb_membase0 & 0xFFFFF000,
	       (int)pcr->pci_cb_memlimit0 | 0x00000FFF,
	       (pcr->pci_bridge_control & PCI_CB_BRIDGE_CTL_PREFETCH_MEM0) ?
		" PREFETCHABLE" : "");

    if ((pcr->pci_cb_membase1 || pcr->pci_cb_memlimit1) &&
	((pcr->pci_cb_membase1 & 0xFFFFF000) <=
	 (pcr->pci_cb_memlimit1 & 0xFFFFF000)))
	printf("  MEMBASE1 0x%08x  MEMLIM1 0x%08x%s\n",
	       (int)pcr->pci_cb_membase1 & 0xFFFFF000,
	       (int)pcr->pci_cb_memlimit1 | 0x00000FFF,
	       (pcr->pci_bridge_control & PCI_CB_BRIDGE_CTL_PREFETCH_MEM1) ?
		" PREFETCHABLE" : "");

    if ((pcr->pci_cb_iobase0 || pcr->pci_cb_iolimit0) &&
	((pcr->pci_cb_iobase0 & 0xFFFFFFFC) <=
	 (pcr->pci_cb_iolimit0 & 0xFFFFFFFC)))
	printf("  IOBASE0  0x%08x      IOLIM0  0x%08x\n",
	       (int)pcr->pci_cb_iobase0 & 0xFFFFFFFC,
	       (int)pcr->pci_cb_iolimit0 | 0x00000003);

    if ((pcr->pci_cb_iobase1 || pcr->pci_cb_iolimit1) &&
	((pcr->pci_cb_iobase1 & 0xFFFFFFFC) <=
	 (pcr->pci_cb_iolimit1 & 0xFFFFFFFC)))
	printf("  IOBASE1  0x%08x      IOLIM1  0x%08x\n",
	       (int)pcr->pci_cb_iobase1 & 0xFFFFFFFC,
	       (int)pcr->pci_cb_iolimit1 | 0x00000003);

    if (pcr->basesize[0]) {
	if ((pcr->pci_base0 & 0x7) == 0x4) {
	    printf("  BASE0     0x%08x (invalid)\n", (int)pcr->pci_base0);
	} else {
	    printf("  BASE0     0x%08x  addr 0x%08x",
		   (int)pcr->pci_base0,
		   (int)(pcr->pci_base0 &
			 (pcr->pci_base0 & 0x1 ? 0xFFFFFFFC : 0xFFFFFFF0)));
	    printf("  %ssize 0x%08x",
		   (pcr->minBasesize & (2 << 0)) ? "" : "estimated ",
		   1 << pcr->basesize[0]);
	    printf("  %s%s\n",
		   (pcr->pci_base0 & 0x1) ? "I/O" : "MEM",
		   ((pcr->pci_base0 & 0x9) == 0x8) ? " PREFETCHABLE" : "");
	}
    }

    printf("  %s16BIT_INT %sSEC_BUS_RST %sM_ABRT %sVGA_EN %sISA_EN"
	   " %sSERR_EN %sPERR_EN %sPOST_WRITES\n",
	   (pcr->pci_bridge_control & PCI_CB_BRIDGE_CTL_16BIT_INT) ?
		"" : "NO_",
	   (pcr->pci_bridge_control & PCI_PCI_BRIDGE_SECONDARY_RESET) ?
		"" : "NO_",
	   (pcr->pci_bridge_control & PCI_PCI_BRIDGE_MASTER_ABORT_EN) ?
		"" : "NO_",
	   (pcr->pci_bridge_control & PCI_PCI_BRIDGE_VGA_EN) ?
		"" : "NO_",
	   (pcr->pci_bridge_control & PCI_PCI_BRIDGE_ISA_EN) ?
		"" : "NO_",
	   (pcr->pci_bridge_control & PCI_PCI_BRIDGE_SERR_EN) ?
		"" : "NO_",
	   (pcr->pci_bridge_control & PCI_PCI_BRIDGE_PARITY_EN) ?
		"" : "NO_",
	   (pcr->pci_bridge_control & PCI_CB_BRIDGE_CTL_POST_WRITES) ?
		"" : "NO_");

    if (pcr->pci_int_pin || pcr->pci_int_line)
	printf("  INT_PIN 0x%02x  INT_LINE 0x%02x\n",
	       pcr->pci_int_pin, pcr->pci_int_line);
}

static void
print_simba(pciConfigPtr pcr)
{
    int   i;
    CARD8 io, mem;

    print_header_type_1(pcr);

    printf("  TICK      0x%08lx         SECCNTL 0x%02x\n", (long)
	   pciReadLong(pcr->tag, 0x00b0), pciReadByte(pcr->tag, 0x00dd));
    printf("  MASTER RETRIES:  PRIMARY 0x%02x,  SECONDARY 0x%02x\n",
	   pciReadByte(pcr->tag, 0x00c0), pciReadByte(pcr->tag, 0x00dc));
    printf("  TARGET RETRIES:  PIO     0x%02x,  DMA       0x%02x\n",
	   pciReadByte(pcr->tag, 0x00d8), pciReadByte(pcr->tag, 0x00da));
    printf("  TARGET LATENCY:  PIO     0x%02x,  DMA       0x%02x\n",
	   pciReadByte(pcr->tag, 0x00d9), pciReadByte(pcr->tag, 0x00db));
    printf("  DMA AFSR  0x%08lx%08lx    AFAR 0x%08lx%08lx\n",
	   (long)pciReadLong(pcr->tag, 0x00cc),
	   (long)pciReadLong(pcr->tag, 0x00c8),
	   (long)pciReadLong(pcr->tag, 0x00d4),
	   (long)pciReadLong(pcr->tag, 0x00d0));
    printf("  PIO AFSR  0x%08lx%08lx    AFAR 0x%08lx%08lx\n",
	   (long)pciReadLong(pcr->tag, 0x00ec),
	   (long)pciReadLong(pcr->tag, 0x00e8),
	   (long)pciReadLong(pcr->tag, 0x00f4),
	   (long)pciReadLong(pcr->tag, 0x00f0));
    printf("  PCI CNTL  0x%08lx%08lx    DIAG 0x%08lx%08lx\n",
	   (long)pciReadLong(pcr->tag, 0x00e4),
	   (long)pciReadLong(pcr->tag, 0x00e0),
	   (long)pciReadLong(pcr->tag, 0x00fc),
	   (long)pciReadLong(pcr->tag, 0x00f8));
    printf("  MAPS:            I/O     0x%02x,  MEM       0x%02x\n",
	   (io  = pciReadByte(pcr->tag, 0x00de)),
	   (mem = pciReadByte(pcr->tag, 0x00df)));
    for (i = 0;  i < 8;  i++)
	if (io & (1 << i))
	    printf("  BUS I/O   0x%06x-0x%06x\n", i << 21, ((i + 1) << 21) - 1);
    for (i = 0;  i < 8;  i++)
	if (mem & (1 << i))
	    printf("  BUS MEM   0x%08x-0x%08x\n", i << 29, ((i + 1) << 29) - 1);
}

static int cbn_460gx = -1;

static void
print_460gx_sac(pciConfigPtr pcr)
{
    CARD32 tmp;

    tmp = pcr->pci_user_config;
    pcr->pci_user_config = 0;
    print_header_type_0(pcr);
    pcr->pci_user_config = tmp;

    /* Only print what XFree86 might be interested in */
    if (pcr->busnum == 0) {
	if ((pcr->devnum != 0x10) || (pcr->funcnum != 0))
	    return;

	/* Get Chipset Bus Number */
	cbn_460gx = (unsigned int)pciReadByte(pcr->tag, 0x0040);
	printf("  CBN       0x%02x  CBUSES 0x%02x\n",
	       cbn_460gx, pciReadByte(pcr->tag, 0x0044));

	return;
    }

    if ((pcr->busnum != cbn_460gx) || (pcr->funcnum != 0))
	return;

    switch (pcr->devnum) {
    case 0:
	printf("  F16NUM    0x%02x  F16CPL 0x%02x  DEVNPRES 0x%08lx\n",
	       pciReadByte(pcr->tag, 0x0060), pciReadByte(pcr->tag, 0x0078),
	       (long)pciReadLong(pcr->tag, 0x0070));

	return;

    case 0x10:
	printf("  TOM       0x%04x  IORD  0x%04x\n",
	       pciReadWord(pcr->tag, 0x0050), pciReadWord(pcr->tag, 0x008E));
	/* Fall through */

    case 0x11:  case 0x12:  case 0x13:
    case 0x14:  case 0x15:  case 0x16:  case 0x17:
	printf("  BUSNO     0x%02x    SUBNO 0x%02x\n",
	       pciReadByte(pcr->tag, 0x0048), pciReadByte(pcr->tag, 0x0049));
	printf("  VGASE     0x%02x    PCIS  0x%02x    IOR 0x%02x\n",
	       pciReadByte(pcr->tag, 0x0080), pciReadByte(pcr->tag, 0x0084),
	       pciReadByte(pcr->tag, 0x008C));
	/* Fall through */

    default:
	return;
    }
}

static void
print_460gx_pxb(pciConfigPtr pcr)
{
    CARD32 tmp;

    tmp = pcr->pci_user_config;
    pcr->pci_user_config = 0;
    print_header_type_0(pcr);
    pcr->pci_user_config = tmp;

    /* Only print what XFree86 might be interested in */
    printf("  ERRCMD    0x%02x  GAPEN 0x%02x\n",
	   pciReadByte(pcr->tag, 0x0046), pciReadByte(pcr->tag, 0x0060));
}

static void
print_460gx_gxb(pciConfigPtr pcr)
{
    CARD32 tmp;

    tmp = pcr->pci_user_config;
    pcr->pci_user_config = 0;
    print_header_type_0(pcr);
    pcr->pci_user_config = tmp;

    /* Only print what XFree86 might be interested in */
    printf("  BAPBASE   0x%08lx%08lx   AGPSIZ  0x%02x   VGAGE     0x%02x\n",
	   (long)pciReadLong(pcr->tag, 0x009C),
	   (long)pciReadLong(pcr->tag, 0x0098),
	   pciReadByte(pcr->tag, 0x00A2), pciReadByte(pcr->tag, 0x0060));
}

#include "xf86getpagesize.c"
