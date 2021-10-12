/* $XFree86: xc/programs/Xserver/hw/xfree86/etc/mmapw.c,v 1.16 2007/01/01 16:08:21 tsi Exp $ */
/*
 * Copyright 2002 through 2007 by Marc Aurele La France (TSI @ UQV), tsi@xfree86.org
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that copyright
 * notice and this permission notice appear in supporting documentation, and
 * that the name of Marc Aurele La France not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  Marc Aurele La France makes no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as-is" without express or implied warranty.
 *
 * MARC AURELE LA FRANCE DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS.  IN NO
 * EVENT SHALL MARC AURELE LA FRANCE BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THIS SOFTWARE.
 */

#undef _LARGEFILE_SOURCE
#undef _FILE_OFFSET_BITS
#undef __STRICT_ANSI__

#define _LARGEFILE_SOURCE 1
#define _FILE_OFFSET_BITS 64

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

typedef void *ptr;

#ifndef MAP_FAILED
# define MAP_FAILED ((ptr)(-1))
#endif

#if defined(_SCO_DS) && !defined(_SCO_DS_LL)
# define strtoull (unsigned long long)strtoul
#endif

#if !defined(strtoull) && \
    (defined(CSRG_BASED) || \
     (defined(__GNU_LIBRARY__) && \
       (__GNU_LIBRARY__ < 6)))
# define strtoull strtouq
#endif

#ifdef linux
# include <sys/ioctl.h>
  /* Workaround for kernel header breakage since 2.5.62 */
# undef  LINUX_MOD_DEVICETABLE_H
# define LINUX_MOD_DEVICETABLE_H 1
# include <linux/pci.h>

# ifndef PCIIOC_BASE
   /* Selected ioctls for /proc/bus/pci/<bus>/<dfn> nodes */
#  define PCIIOC_BASE		(('P' << 24) | ('C' << 16) | ('I' << 8))

   /* Set mmap state to I/O space */
#  define PCIIOC_MMAP_IS_IO	(PCIIOC_BASE | 0x01)
   /* Set mmap state to memory space */
#  define PCIIOC_MMAP_IS_MEM	(PCIIOC_BASE | 0x02)
# endif
#endif

#ifdef WSCONS_SUPPORT
# include <sys/ioctl.h>
# include <dev/wscons/wsconsio.h>
#endif

#define datab unsigned char
#define dataw unsigned short
#define datal unsigned int
#define dataL unsigned long
#define dataq unsigned long long

static void
usage(void)
{
    fprintf(stderr, "\n"
#ifdef linux
	"mmapw [-{im}] [-{bwlqL}] [-{au}] <file> <offset> <value>\n\n"
	" -i   select /proc/bus/pci/<bus>/<dfn> I/O space\n"
	" -m   select /proc/bus/pci/<bus>/<dfn> memory space\n\n"
#else
#ifdef WSCONS_SUPPORT
	"mmapw [-{emd}] [-{bwlqL}] [-{au}] <file> <offset> <value>\n\n"
	" -e   select wsdisplay emulation mapping\n"
	" -m   select wsdisplay graphics mapping\n"
	" -d   select wsdisplay framebuffer mapping\n\n"
#else
	"mmapw [-{bwlqL}] [-{au}] <file> <offset> <value>\n\n"
#endif
#endif
	"access size flags:\n\n"
	" -b   write one byte\n"
	" -w   write two aligned bytes\n"
	" -l   write four aligned bytes (default)\n"
	" -q   write eight aligned bytes\n");

    switch (sizeof(dataL))
    {
	case sizeof(datab):
	    fprintf(stderr, " -L   same as -b\n\n");
	    break;

	case sizeof(dataw):
	    fprintf(stderr, " -L   same as -w\n\n");
	    break;

	case sizeof(datal):
	    fprintf(stderr, " -L   same as -l\n\n");
	    break;

	case sizeof(dataq):
	    fprintf(stderr, " -L   same as -q\n\n");
	    break;

	default:
	    fprintf(stderr, "\n");
	    break;
    }

    fprintf(stderr,
	" -u   as above but allow unaligned accesses (might crash)\n"
	" -a   only use aligned accesses (default)\n\n");

    exit(1);
}

#ifdef SIGBUS
/*
 * Signal handler to catch unaligned access and print a meaningful message.
 */
static void
sigbus(int signum)
{
    fprintf(stderr,
	"The architecture or OS does not allow unaligned accesses\n");
    exit (128 + SIGBUS);
}
#endif

int
main(int argc, char **argv)
{
    unsigned long long data;
    off_t Offset = 0, offset;
    size_t length;
    char *BadString;
    void *buffer;
    int fd, pagesize, aligned = 1, error;
#ifdef linux
    int mmap_ioctl = 0;
#endif
#ifdef WSCONS_SUPPORT
    int wsdisplay_mode = -1, old_mode;
#endif
    char size = sizeof(datal);

    while (argv[1] && (argv[1][0] == '-') && argv[1][1])
    {
	for (;  argv[1][1];  argv[1]++)
	{
	    switch (argv[1][1])
	    {
		case 'b':
		    size = sizeof(datab);
		    break;

		case 'w':
		    size = sizeof(dataw);
		    break;

		case 'l':
		    size = sizeof(datal);
		    break;

		case 'L':
		    size = sizeof(dataL);
		    break;

		case 'q':
		    size = sizeof(dataq);
		    break;

		case 'u':
		    aligned = 0;
		    break;

		case 'a':
		    aligned = 1;
		    break;
#ifdef linux
		case 'i':
		    mmap_ioctl = PCIIOC_MMAP_IS_IO;
		    break;

		case 'm':
		    mmap_ioctl = PCIIOC_MMAP_IS_MEM;
		    break;
#endif
#ifdef WSCONS_SUPPORT
		case 'e':
		    wsdisplay_mode = WSDISPLAYIO_MODE_EMUL;
		    break;

		case 'm':
		    wsdisplay_mode = WSDISPLAYIO_MODE_MAPPED;
		    break;

		case 'd':
		    wsdisplay_mode = WSDISPLAYIO_MODE_DUMBFB;
		    break;
#endif
		default:
		    usage();
	    }
	}

	argc--;
	argv++;
    }

    if (argc != 4)
	usage();

    BadString = (ptr)0;
    Offset = strtoull(argv[2], &BadString, 0);
    if (errno || (BadString && *BadString) || (Offset & (size - 1)))
	usage();

    BadString = (ptr)0;
    data = strtoull(argv[3], &BadString, 0);
    if (errno || (BadString && *BadString))
	usage();

    if (data & ((unsigned long long)(-1LL) << (size * 8)))
    {
	fprintf(stderr, "Value too large for access size\n");
	exit(1);
    }

    if ((fd = open(argv[1], O_RDWR)) < 0)
    {
	fprintf(stderr, "mmapw:  Unable to open \"%s\":  \"%s\".\n",
	    argv[1], strerror(errno));
	exit(1);
    }

#ifdef linux
    if (mmap_ioctl && (ioctl(fd, mmap_ioctl, 0) < 0))
	fprintf(stderr, "mmapw:  ioctl error:  \"%s\";  Ignored.\n",
	    strerror(errno));
#endif

#ifdef WSCONS_SUPPORT
    if ((wsdisplay_mode >= 0) &&
	((ioctl(fd, WSDISPLAYIO_GMODE, &old_mode) < 0) ||
	 (ioctl(fd, WSDISPLAYIO_SMODE, &wsdisplay_mode) < 0)))
	fprintf(stderr, "mmapw:  ioctl error:  \"%s\";  Ignored.\n",
	    strerror(errno));
#endif

    pagesize = getpagesize();
    offset = Offset & (off_t)(-pagesize);
    length = ((Offset + size + pagesize - 1) & (off_t)(-pagesize)) - offset;
    buffer = mmap((caddr_t)0, length, PROT_READ | PROT_WRITE, MAP_SHARED, fd,
	offset);

    if (buffer == MAP_FAILED)
    {
	fprintf(stderr, "mmapw:  Unable to mmap \"%s\":  \"%s\".\n",
	    argv[1], strerror(errno));
	error = 1;
    }
    else
    {
#ifdef SIGBUS
	if (!aligned)
	    signal(SIGBUS, sigbus);
#endif

	Offset -= offset;
	if ((size == sizeof(datab)) || (aligned && (Offset & sizeof(datab))))
	{
	    do
	    {
		*(volatile unsigned char *)(ptr)((char *)buffer + Offset) =
		    (unsigned char)data;
		data >>= 8 * (sizeof(datab) & (sizeof(data) - 1));
		Offset += sizeof(datab);
		size -= sizeof(datab);
	    } while (size);
	}
	else
	if ((size == sizeof(dataw)) || (aligned && (Offset & sizeof(dataw))))
	{
	    do
	    {
		*(volatile unsigned short *)(ptr)((char *)buffer + Offset) =
		    (unsigned short)data;
		data >>= 8 * (sizeof(dataw) & (sizeof(data) - 1));
		Offset += sizeof(dataw);
		size -= sizeof(dataw);
	    } while (size);
	}
	else
	if ((size == sizeof(datal)) || (aligned && (Offset & sizeof(datal))))
	{
	    do
	    {
		*(volatile unsigned int *)(ptr)((char *)buffer + Offset) =
		    (unsigned int)data;
		data >>= 8 * (sizeof(datal) & (sizeof(data) - 1));
		Offset += sizeof(datal);
		size -= sizeof(datal);
	    } while (size);
	}
	else
	if ((size == sizeof(dataL)) || (aligned && (Offset & sizeof(dataL))))
	{
	    do
	    {
		*(volatile unsigned long *)(ptr)((char *)buffer + Offset) =
		    (unsigned long)data;
		data >>= 8 * (sizeof(dataL) & (sizeof(data) - 1));
		Offset += sizeof(dataL);
		size -= sizeof(dataL);
	    } while (size);
	}
	else
	{
	    *(volatile unsigned long long *)(ptr)((char *)buffer + Offset) =
		(unsigned long long)data;
	}

	error = 0;
    }

    munmap(buffer, length);

#ifdef WSCONS_SUPPORT
    /* Sigh...  This must be done _after_ the data has been accessed */
    if ((wsdisplay_mode >= 0) &&
	(ioctl(fd, WSDISPLAYIO_SMODE, &old_mode) < 0))
	fprintf(stderr, "mmapw:  ioctl error:  \"%s\";  Ignored.\n",
	    strerror(errno));
#endif

    close(fd);

    return error;
}
