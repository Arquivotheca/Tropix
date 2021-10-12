/*
 * SBUS and OpenPROM access functions.
 *
 * Copyright (C) 2000 Jakub Jelinek (jakub@redhat.com)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * JAKUB JELINEK BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
/* $XFree86: xc/programs/Xserver/hw/xfree86/os-support/bus/Sbus.c,v 1.15 2007/04/09 15:37:18 tsi Exp $ */

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#ifdef sun
#include <sys/utsname.h>
#endif
#include "xf86.h"
#include "xf86Priv.h"
#include "xf86_OSlib.h"

#include "xf86sbusBus.h"
#include "xf86Sbus.h"

int promRootNode;

static int promFd = -1;
static int promOpenCount = 0;
static int promP1275 = -1;

#define MAX_PROP	128
#define MAX_VAL		(4096-128-4)

#if defined(__OpenBSD__) || defined(__NetBSD__)
/* These two use their own OBP access scheme */
static struct opiocdesc promOpio;
static char promval[MAX_VAL + 1];
#else
static struct openpromio *promOpio;
#endif

sbusDevicePtr *xf86SbusInfo = NULL;

struct sbus_devtable sbusDeviceTable[] = {
    { SBUS_DEVICE_BW2, FBTYPE_SUN2BW, "bwtwo", "Sun Monochrome (bwtwo)" },
    { SBUS_DEVICE_CG2, FBTYPE_SUN2COLOR, "cgtwo", "Sun Color2 (cgtwo)" },
    { SBUS_DEVICE_CG3, FBTYPE_SUN3COLOR, "cgthree", "Sun Color3 (cgthree)" },
    { SBUS_DEVICE_CG3, FBTYPE_SUN3COLOR, "cgRDI", "Sun Color3 RDI (cgthree)" },
    { SBUS_DEVICE_CG4, FBTYPE_SUN4COLOR, "cgfour", "Sun Color4 (cgfour)" },
    { SBUS_DEVICE_CG6, FBTYPE_SUNFAST_COLOR, "cgsix", "Sun GX" },
    { SBUS_DEVICE_CG8, FBTYPE_MEMCOLOR, "cgeight", "Sun CG8/RasterOps" },
    { SBUS_DEVICE_CG12, FBTYPE_SUNGP3, "cgtwelve", "Sun GS (cgtwelve)" },
    { SBUS_DEVICE_CG14, FBTYPE_MDICOLOR, "cgfourteen", "Sun SX" },
    { SBUS_DEVICE_GT, FBTYPE_SUNGT, "gt", "Sun Graphics Tower" },
    { SBUS_DEVICE_MGX, -1, "mgx", "Quantum 3D MGXplus" },
    { SBUS_DEVICE_LEO, FBTYPE_SUNLEO, "leo", "Sun ZX or Turbo ZX" },
    { SBUS_DEVICE_TCX, FBTYPE_TCXCOLOR, "tcx", "Sun TCX" },
    { SBUS_DEVICE_FFB, FBTYPE_CREATOR, "ffb", "Sun FFB" },
    { SBUS_DEVICE_FFB, FBTYPE_CREATOR, "afb", "Sun Elite3D" },
    { SBUS_DEVICE_P9100, FBTYPE_P9100, "pnozz", "Weitek P9100" },
    { 0, 0, NULL, NULL }
};

#if defined(__OpenBSD__) || defined(__NetBSD__)
int
promGetSibling(int node)
{
    if (node == -1)
	return 0;

    promOpio.op_nodeid = node;
    if (ioctl(promFd, OPIOCGETNEXT, &promOpio.op_nodeid) < 0)
	return 0;

    return promOpio.op_nodeid;
}

int
promGetChild(int node)
{
    if (!node || (node == -1))
	return 0;

    promOpio.op_nodeid = node;
    if (ioctl(promFd, OPIOCGETCHILD, &promOpio.op_nodeid) < 0)
	return 0;

    return promOpio.op_nodeid;
}

char *
promGetProperty(const char *prop, int *lenp)
{
    /* op_nodeid has already been set */
    promOpio.op_namelen = strlen(prop);
    promOpio.op_name = (char *)prop;
    promOpio.op_buflen = MAX_VAL;
    promOpio.op_buf = promval;

    if (ioctl(promFd, OPIOCGET, &promOpio) < 0)
	return NULL;

    if (lenp)
	*lenp = promOpio.op_buflen;
    return promOpio.op_buf;
}

int
promGetBool(const char *prop)
{
    int length;

    if (!promGetProperty(prop, &length) || (length < 0))
	return 0;

    return 1;
}
#else
int
promGetSibling(int node)
{
    if (node == -1)
	return 0;

    promOpio->oprom_size = sizeof(int);
    *(int *)promOpio->oprom_array = node;
    if (ioctl(promFd, OPROMNEXT, promOpio) < 0)
	return 0;

    return *(int *)promOpio->oprom_array;
}

int
promGetChild(int node)
{
    if (!node || (node == -1))
	return 0;

    promOpio->oprom_size = sizeof(int);
    *(int *)promOpio->oprom_array = node;
    if (ioctl(promFd, OPROMCHILD, promOpio) < 0)
	return 0;

    return *(int *)promOpio->oprom_array;
}

char *
promGetProperty(const char *prop, int *lenp)
{
    promOpio->oprom_size = MAX_VAL;
    strcpy(promOpio->oprom_array, prop);
    if (ioctl(promFd, OPROMGETPROP, promOpio) < 0)
	return NULL;

    if (lenp)
	*lenp = promOpio->oprom_size;
    return promOpio->oprom_array;
}

int
promGetBool(const char *prop)
{
    promOpio->oprom_size = 0;
    *(int *)promOpio->oprom_array = 0;

    for (;;) {
	promOpio->oprom_size = MAX_PROP;
	if (ioctl(promFd, OPROMNXTPROP, promOpio) < 0)
	    return 0;
	if (!promOpio->oprom_size)
	    return 0;
	if (!strcmp(promOpio->oprom_array, prop))
	    return 1;
    }
}
#endif

#define PROM_NODE_SIBLING 0x01
#define PROM_NODE_PREF    0x02
#define PROM_NODE_SBUS    0x04
#define PROM_NODE_EBUS    0x08
#define PROM_NODE_PCI     0x10
#define PROM_NODE_FREE    0x20

static int
promSetNode(sbusPromNodePtr pnode)
{
    int node;

    if (!pnode->node || (pnode->node == -1))
	return -1;

    if (pnode->cookie[0] & PROM_NODE_SIBLING)
	node = promGetSibling(pnode->cookie[1]);
    else
	node = promGetChild(pnode->cookie[1]);

    if (pnode->node != node)
	return -1;

    return 0;
}

static void
promIsP1275(void)
{
#ifdef linux
    FILE *f;
    char buffer[1024];

    if (promP1275 != -1)
	return;
    promP1275 = FALSE;
    f = fopen("/proc/cpuinfo", "r");
    if (!f) return;
    while (fgets(buffer, 1024, f) != NULL)
	if (!strncmp(buffer, "type", 4) &&
	    (strstr(buffer, "sun4u") || strstr(buffer, "sun4v"))) {
	    promP1275 = 1;
	    break;
	}
    fclose(f);
#elif defined(sun)
    struct utsname buffer;

    if (promP1275 != -1)
	return;

    if ((uname(&buffer) >= 0) &&
        (!strcmp(buffer.machine, "sun4u") || !strcmp(buffer.machine, "sun4v")))
	promP1275 = TRUE;
    else
	promP1275 = FALSE;
#elif defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__)
# if defined(__arch64__) || defined(__sparc_v9__) || defined(__sparc64__)
    promP1275 = TRUE;
# else
    /* Might need to check for a 32-bit userland running on a sun4u or sun4v */
    promP1275 = FALSE;
# endif
#else
# error Missing promIsP1275() function for this OS
#endif
}

void
sparcPromClose(void)
{
    if (promOpenCount > 1) {
	promOpenCount--;
	return;
    }

    if (promFd != -1) {
	close(promFd);
	promFd = -1;
    }

#if !defined(__OpenBSD__) && !defined(__NetBSD__)
    if (promOpio) {
	xfree(promOpio);
	promOpio = NULL;
    }
#endif

    promOpenCount = 0;
}

int
sparcPromInit(void)
{
    if (promOpenCount) {
	promOpenCount++;
	return 0;
    }

    if (promFd < 0) {
	promFd = open("/dev/openprom", O_RDONLY, 0);
	if (promFd == -1)
	    return -1;
    }

#if !defined(__OpenBSD__) && !defined(__NetBSD__)
    if (!promOpio) {
	promOpio = (struct openpromio *)xalloc(4096);
	if (!promOpio) {
	    sparcPromClose();
	    return -1;
	}
    }
#endif

    promRootNode = promGetSibling(0);
    if (!promRootNode) {
	sparcPromClose();
	return -1;
    }

    promIsP1275();
    promOpenCount++;

    return 0;
}

char *
sparcPromGetProperty(sbusPromNodePtr pnode, const char *prop, int *lenp)
{
    if (promSetNode(pnode))
	return NULL;

    return promGetProperty(prop, lenp);
}

int
sparcPromGetBool(sbusPromNodePtr pnode, const char *prop)
{
    if (promSetNode(pnode))
	return 0;

    return promGetBool(prop);
}

static void
promWalkAssignNodes(sbusPromNode *pCurrent, sbusPromParentPtr pParent,
		    sbusDevicePtr *devicePtrs)
{
    int len, sbus;
    char *prop;
    int devId, i, j;
    sbusPromNode Node, Node2, newNode;
    sbusPromParentPtr Parent, Parent2;

    newNode = *pCurrent;

    sbus = newNode.cookie[0] & PROM_NODE_SBUS;

    prop = promGetProperty("device_type", &len);
    do {
	if (!prop || (len < 7) || (len > 8) || strcmp(prop, "display"))
	    break;

	prop = promGetProperty("name", &len);
	if (!prop || (len <= 0))
	    break;

	while (((*prop >= 'A') && (*prop <= 'Z')) || (*prop == ','))
	    prop++;

	for (i = 0; sbusDeviceTable[i].devId; i++)
	    if (!strcmp(prop, sbusDeviceTable[i].promName))
		break;

	devId = sbusDeviceTable[i].devId;
	if (!devId)
	    break;

	if (!sbus) {
	    if (devId == SBUS_DEVICE_FFB) {
		/*
		 * All /SUNW,ffb outside of SBUS tree come before all
		 * /SUNW,afb outside of SBUS tree.
		 */
		if (!strcmp(prop, "afb"))
		    newNode.cookie[0] |= PROM_NODE_PREF;
	    } else if (devId != SBUS_DEVICE_CG14)
		break;
	}

	for (i = 0; i < 32; i++) {
	    if (!devicePtrs[i] || (devicePtrs[i]->devId != devId))
		continue;

	    if (devicePtrs[i]->node.node) {
		if ((devicePtrs[i]->node.cookie[0] & ~PROM_NODE_SIBLING) <=
		    (newNode.cookie[0] & ~PROM_NODE_SIBLING))
		    continue;

		Node = devicePtrs[i]->node;
		Parent = devicePtrs[i]->parent;
		for (j = i; ++j < 32; ) {
		    if (!devicePtrs[j] || (devicePtrs[j]->devId != devId))
			continue;

		    Node2 = devicePtrs[j]->node;
		    Parent2 = devicePtrs[j]->parent;
		    devicePtrs[j]->node = Node;
		    devicePtrs[j]->parent = Parent;
		    Node = Node2;
		    Parent = Parent2;
		}
	    }
	    devicePtrs[i]->node = newNode;
	    devicePtrs[i]->parent = pParent;

	    for (Parent = pParent;
		 Parent && (Parent->node.cookie[0] & PROM_NODE_FREE);
		 Parent = Parent->parent)
		Parent->node.cookie[0] &= ~PROM_NODE_FREE;
	    break;
	}
    } while (0);

    prop = promGetProperty("name", &len);
    if (prop && (len >= 3) && (len <= 5) &&
	(!strcmp(prop, "sbus") || !strcmp(prop, "sbi")))
	sbus = PROM_NODE_SBUS;

    newNode.cookie[1] = newNode.node;
    if ((newNode.node = promGetChild(newNode.cookie[1]))) {
	newNode.cookie[0] = sbus;

	Parent = xnfalloc(sizeof(*Parent));
	Parent->node = *pCurrent;
	Parent->node.cookie[0] |= PROM_NODE_FREE;
	Parent->parent = pParent;

	promWalkAssignNodes(&newNode, Parent, devicePtrs);

	if (Parent->node.cookie[0] & PROM_NODE_FREE)
	    xfree(Parent);
    }

    if ((newNode.node = promGetSibling(newNode.cookie[1]))) {
	newNode.cookie[0] = sbus | PROM_NODE_SIBLING;

	promWalkAssignNodes(&newNode, pParent, devicePtrs);
    }
}

void
sparcPromAssignNodes(void)
{
    sbusDevicePtr psdp, *psdpp;
    sbusDevicePtr devicePtrs[32];
    sbusPromNode root;

#if defined(linux)

    int n, holes = 0, i, j;
    FILE *f;

    (void)memset(devicePtrs, 0, sizeof(devicePtrs));
    for (psdpp = xf86SbusInfo, n = 0; (psdp = *psdpp); psdpp++, n++) {
	if (psdp->fbNum != n)
	    holes = 1;
	devicePtrs[psdp->fbNum] = psdp;
    }

    if (holes && (f = fopen("/proc/fb", "r")) != NULL) {
	/*
	 * We could not open one of fb devices, check /proc/fb to see what
	 * were the types of the cards missed.
	 */
	char buffer[64];
	int fbNum, devId;
	static struct {
	    int devId;
	    char *prefix;
	    char *descr;
	} procFbPrefixes[] = {
	    { SBUS_DEVICE_BW2, "BWtwo", "Sun Monochrome (bwtwo)" },
	    { SBUS_DEVICE_CG14, "CGfourteen", "Sun SX" },
	    { SBUS_DEVICE_CG6, "CGsix", "Sun GX" },
	    { SBUS_DEVICE_CG3, "CGthree", "Sun Color3 (cgthree)" },
	    { SBUS_DEVICE_FFB, "Creator", "Sun FFB" },
	    { SBUS_DEVICE_FFB, "Elite 3D", "Sun Elite3D" },
	    { SBUS_DEVICE_LEO, "Leo", "Sun ZX or Turbo ZX" },
	    { SBUS_DEVICE_TCX, "TCX", "Sun TCX" },
	    { 0, NULL, NULL },
	};

	while (fscanf(f, "%d %63s\n", &fbNum, buffer) == 2) {
	    for (i = 0; procFbPrefixes[i].devId; i++)
		if (!strncmp(procFbPrefixes[i].prefix, buffer,
		     strlen(procFbPrefixes[i].prefix)))
		    break;

	    devId = procFbPrefixes[i].devId;
	    if (!devId)
		continue;

	    if (devicePtrs[fbNum]) {
		if (devicePtrs[fbNum]->devId != devId)
		    xf86ErrorF("Inconsistent /proc/fb with FBIOGATTR\n");
	    } else {
		devicePtrs[fbNum] = psdp = xnfcalloc(sizeof(sbusDevice), 1);
		psdp->devId = devId;
		psdp->fbNum = fbNum;
		psdp->fd = -2;
		psdp->descr = procFbPrefixes[i].descr;
	    }
	}

	fclose(f);
    }

#else

    (void)memset(devicePtrs, 0, sizeof(devicePtrs));
    for (psdpp = xf86SbusInfo; (psdp = *psdpp); psdpp++) 
	devicePtrs[psdp->fbNum] = psdp;

#endif

    root.node = promGetSibling(0);
    root.cookie[0] = PROM_NODE_SIBLING | PROM_NODE_PREF;
    root.cookie[1] = 0;
    promWalkAssignNodes(&root, NULL, devicePtrs);

#if defined(linux)

    j = 0;
    for (i = 0; i < 32; i++)
	if (devicePtrs[i] && (devicePtrs[i]->fbNum == -1))
	    j++;

    xf86SbusInfo = xnfrealloc(xf86SbusInfo, sizeof(psdp) * (n + j + 1));
    for (i = 0, psdpp = xf86SbusInfo; i < 32; i++) {
	if (devicePtrs[i]) {
	    if (devicePtrs[i]->fbNum == -1) {
		memmove(psdpp + 1, psdpp, sizeof(psdpp) * (n + 1));
		*psdpp = devicePtrs[i];
	    } else {
		n--;
	    }
	}
    }

#endif

}

static char *
promGetReg(int type)
{
    char *prop;
    int len;
    static char regstr[40];

    regstr[0] = 0;
    prop = promGetProperty("reg", &len);
    if (prop && (len >= 4)) {
	unsigned int *reg = (unsigned int *)prop;

	if (!promP1275 ||
	    (type == PROM_NODE_SBUS) ||
	    (type == PROM_NODE_EBUS)) {
	    sprintf(regstr, "@%x,%x", reg[0], reg[1]);
	} else if (type == PROM_NODE_PCI) {
	    if ((reg[0] >> 8) & 7)
		sprintf(regstr, "@%x,%x",
			(reg[0] >> 11) & 0x1f, (reg[0] >> 8) & 7);
	    else
		sprintf(regstr, "@%x", (reg[0] >> 11) & 0x1f);
	} else if (len == 4) {
	    sprintf(regstr, "@%x", reg[0]);
	} else {
	    unsigned int regs[2];

	    /*
	     * Things get more complicated on UPA.  If upa-portid exists, then
	     * address is @upa-portid,second-int-in-reg, otherwise it is
	     * @first-int-in-reg/16,second-int-in-reg (well, probably
	     * upa-portid always exists, but just to be safe).
	     */
	    memcpy(regs, reg, sizeof(regs));
	    prop = promGetProperty("upa-portid", &len);
	    if (prop && (len == 4)) {
		reg = (unsigned int *)prop;
		sprintf(regstr, "@%x,%x", reg[0], regs[1]);
	    } else {
		sprintf(regstr, "@%x,%x", regs[0] >> 4, regs[1]);
	    }
	}
    }

    return regstr;
}

static int
promWalkNode2Pathname(char *path, int parent, int node, int searchNode, int type)
{
    int nextnode;
    int len = 0, ntype = type;
    char *prop, *p;

    prop = promGetProperty("name", &len);
    *path = '/';

    if (!prop || (len <= 0))
	return 0;

    if (!type && (!strcmp(prop, "sbus") || !strcmp(prop, "sbi")))
	ntype = PROM_NODE_SBUS;
    else if ((type == PROM_NODE_PCI) && !strcmp(prop, "ebus"))
	ntype = PROM_NODE_EBUS;
    else if (!type && !strcmp(prop, "pci"))
	ntype = PROM_NODE_PCI;

    strcpy(path + 1, prop);

    p = promGetReg(type);
    if (*p)
	strcat(path, p);

    if (node == searchNode)
	return 1;

    nextnode = promGetChild(node);
    if (nextnode &&
	promWalkNode2Pathname(strchr(path, 0), node, nextnode, searchNode, ntype))
	return 1;

    nextnode = promGetSibling(node);
    if (nextnode &&
	promWalkNode2Pathname(path, parent, nextnode, searchNode, type))
	return 1;

    return 0;
}

char *
sparcPromNode2Pathname(sbusPromNodePtr pnode)
{
    char *ret;

    if (!pnode->node)
	return NULL;

    ret = xalloc(4096);
    if (!ret)
	return NULL;

    if (promWalkNode2Pathname(ret, promRootNode, promGetChild(promRootNode), pnode->node, 0))
	return ret;

    xfree(ret);
    return NULL;
}

static int
promWalkPathname2Node(char *name, char *regstr, int parent, int type)
{
    int len, node, ret;
    char *prop, *p;

    for (;;) {
	prop = promGetProperty("name", &len);
	if (!prop || (len <= 0))
	    return 0;

	if (!type && (!strcmp(prop, "sbus") || !strcmp(prop, "sbi")))
	    type = PROM_NODE_SBUS;
	else if ((type == PROM_NODE_PCI) && !strcmp(prop, "ebus"))
	    type = PROM_NODE_EBUS;
	else if (!type && !strcmp(prop, "pci"))
	    type = PROM_NODE_PCI;
	for (node = promGetChild(parent); node; node = promGetSibling(node)) {
	    prop = promGetProperty("name", &len);
	    if (!prop || (len <= 0))
		continue;

	    if (*name && strcmp(name, prop))
		continue;

	    if (*regstr) {
		p = promGetReg(type);
		if (!*p || strcmp(p + 1, regstr))
		    continue;
	    }

	    break;
	}
	if (!node) {
	    for (node = promGetChild(parent);
		 node;
		 node = promGetSibling(node)) {
		ret = promWalkPathname2Node(name, regstr, node, type);
		if (ret)
		    return ret;
	    }

	    return 0;
	}

	name = strchr(regstr, 0) + 1;
	if (!*name)
	    return node;

	p = strchr(name, '/');
	if (p)
	    *p = 0;
	else
	    p = strchr(name, 0);

	regstr = strchr(name, '@');
	if (regstr)
	    *regstr++ = 0;
	else
	    regstr = p;

	if (name == regstr)
	    return 0;

	parent = node;
    }
}

int
sparcPromPathname2Node(const char *pathName)
{
    int i;
    char *name, *regstr, *p;

    i = strlen(pathName);
    name = xalloc(i + 2);
    if (!name)
	return 0;

    strcpy(name, pathName);
    name [i + 1] = 0;
    if (name[0] != '/')
	return 0;

    p = strchr(name + 1, '/');
    if (p)
	*p = 0;
    else
	p = strchr(name, 0);

    regstr = strchr(name, '@');
    if (regstr)
	*regstr++ = 0;
    else
	regstr = p;

    if (name + 1 == regstr)
	return 0;

    promGetSibling(0);
    i = promWalkPathname2Node(name + 1, regstr, promRootNode, 0);
    xfree(name);
    return i;
}

Bool
xf86LocateSbusMemoryArea(sbusDevicePtr psdp, char **devName,
			 unsigned int *devOffset, unsigned int *fbSize,
			 unsigned int *fbOffset, unsigned int *flags)
{
    /*
     * Note that SBUS video drivers, contrary to other drivers, can be
     * OS-specific.  That's because the offset(s) needed to mmap(2) SBUS video
     * devices can be also.  One might think this common layer function could
     * translate from a more generic offset scheme to the needed OS-specific
     * one, but this would require adapter-specific knowledge in the common
     * layer, and, for SBUS devices, there's enough of that already...
     *
     * Instead, this simply checks that all offsets are 32-bit.
     */
    unsigned int offset;

    if (!psdp || !devOffset || devOffset[1])
	return FALSE;

    offset = devOffset[0];

    if (fbOffset) {
	if ((offset ^ (unsigned int)(-1L)) < *fbOffset)
	    return FALSE;

	offset += *fbOffset;
    }

    if (fbSize) {
	if ((offset ^ (unsigned int)(-1L)) < *fbSize)
	    return FALSE;
    }

    if (devName)
	*devName = psdp->device;

    if (flags)
	*flags = 0;

    return TRUE;
}

pointer
xf86MapSbusMem(sbusDevicePtr psdp, unsigned long offset, unsigned long size)
{
    pointer ret;
    unsigned long pagemask, off, len;
    unsigned int devOffset[2], fbSize;
#ifdef __OpenBSD__
    static const unsigned int fbmode = WSDISPLAYIO_MODE_MAPPED;
#endif

    if (!psdp || !size)
	return NULL;

    if (psdp->fd == -1) {
	psdp->fd = open(psdp->device, O_RDWR);
	if (psdp->fd == -1)
	    return NULL;
	psdp->mmapCount = 0;
#ifdef __OpenBSD__
	if (ioctl(psdp->fd, WSDISPLAYIO_SMODE, &fbmode) < 0)
	    return NULL;
#endif
    } else if (psdp->fd < 0) {
	return NULL;
    }

    devOffset[0] = offset;
    devOffset[1] = 0;
    fbSize = size;
    if (!xf86LocateSbusMemoryArea(psdp, NULL, devOffset, &fbSize, NULL, NULL) ||
	(devOffset[1] != 0))
	return NULL;

    offset = devOffset[0];
    pagemask = xf86getpagesize() - 1;
    off = offset & ~pagemask;
    len = ((offset + size + pagemask) & ~pagemask) - off;

    ret = (pointer) mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_PRIVATE,
			 psdp->fd, off);
    if (ret == (pointer) -1) {
	ret = (pointer) mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED,
			     psdp->fd, off);
	if (ret == (pointer) -1)
	    return NULL;
    }

    psdp->mmapCount++;
    return (char *)ret + (offset - off);
}

void
xf86UnmapSbusMem(sbusDevicePtr psdp, pointer addr, unsigned long size)
{
    unsigned long mask = xf86getpagesize() - 1;
    unsigned long base = (unsigned long)addr & ~mask;
    unsigned long len = (((unsigned long)addr + size + mask) & ~mask) - base;

    munmap((pointer)base, len);

    if (!--psdp->mmapCount) {
#ifdef __OpenBSD__
	static const unsigned int fbmode = WSDISPLAYIO_MODE_EMUL;

	/* Ignore errors */
	ioctl(psdp->fd, WSDISPLAYIO_SMODE, &fbmode);
#endif
	close(psdp->fd);
	psdp->fd = -1;
    }
}
