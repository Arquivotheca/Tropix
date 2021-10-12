/* $XFree86: xc/programs/Xserver/hw/xfree86/os-support/linux/lnx.h,v 3.4 2006/02/17 18:04:38 dawes Exp $ */

#ifndef LNX_H_

#ifdef __ia64__

#include "compiler.h"
#include <sys/io.h>

#elif !defined(__powerpc__) && \
      !defined(__mc68000__) && \
      !defined(__sparc__) && \
      !defined(__mips__)

/*
 * Due to conflicts with "compiler.h", don't rely on <sys/io.h> to declare
 * these.
 */
extern int ioperm(unsigned long __from, unsigned long __num, int __turn_on);
extern int iopl(int __level);

#endif

# ifdef __alpha__
extern unsigned long _bus_base __P ((void)) __attribute__ ((const));
extern unsigned long _bus_base_sparse __P ((void)) __attribute__ ((const));

/* new pciconfig_iobase syscall added in 2.2.15 and 2.3.99 */
#  include <linux/unistd.h>
#  include <asm/pci.h>
extern long (*_iobase)(unsigned, int, int, int);

extern unsigned char _inb (unsigned long port);
extern unsigned short _inw (unsigned long port);
extern unsigned int   _inl (unsigned long port);
extern void _outb (unsigned char b,unsigned long port);
extern void _outw (unsigned short w,unsigned long port);
extern void _outl (unsigned int l,unsigned long port);


/*
 * _iobase deals with the case the __NR_pciconfig_iobase is either undefined
 * or unsupported by the kernel, but we need to make sure that the `which'
 * argument symbols are defined.
 */
#  ifndef IOBASE_HOSE
#   define IOBASE_HOSE 		0
#  endif
#  ifndef IOBASE_SPARSE_MEM
#   define IOBASE_SPARSE_MEM	1
#  endif
#  ifndef IOBASE_DENSE_MEM
#   define IOBASE_DENSE_MEM	2
#  endif
#  ifndef IOBASE_SPARSE_IO
#   define IOBASE_SPARSE_IO	3
#  endif
#  ifndef IOBASE_DENSE_IO
#   define IOBASE_DENSE_IO	4
#  endif
#  ifndef IOBASE_ROOT_BUS
#   define IOBASE_ROOT_BUS	5
#  endif
#  ifndef IOBASE_FROM_HOSE
#   define IOBASE_FROM_HOSE	0x10000
#  endif
# endif /* __alpha__ */

#define LNX_H_

#endif
