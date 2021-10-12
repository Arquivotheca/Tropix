/* $XFree86: xc/programs/Xserver/hw/xfree86/os-support/linux/lnx_ev56.c,v 3.10 2006/02/20 00:14:37 dawes Exp $ */

#include <X11/X.h>
#include "input.h"
#include "scrnintstr.h"
#include "compiler.h"

#include "xf86.h"
#include "xf86Priv.h"
#include "xf86_OSlib.h"
#include "xf86OSpriv.h"
#include <unistd.h>
#include "lnx.h"
#include "lnx_axp.h"

int
readDense8(volatile void *Base, register unsigned long Offset)
{
    mem_barrier();
    return *(volatile CARD8*) ((unsigned long)Base+(Offset));
}

int
readDense16(volatile void *Base, register unsigned long Offset)
{
    mem_barrier();
    return *(volatile CARD16*) ((unsigned long)Base+(Offset));
}

int
readDense32(volatile void *Base, register unsigned long Offset)
{
    mem_barrier();
    return *(volatile CARD32*)((unsigned long)Base+(Offset));
}

void
writeDenseNB8(int Value, volatile void *Base, register unsigned long Offset)
{
    *(volatile CARD8*)((unsigned long)Base+(Offset)) = Value;
}

void
writeDenseNB16(int Value, volatile void *Base, register unsigned long Offset)
{
    *(volatile CARD16*)((unsigned long)Base + (Offset)) = Value;
}

void
writeDenseNB32(int Value, volatile void *Base, register unsigned long Offset)
{
    *(volatile CARD32*)((unsigned long)Base+(Offset)) = Value;
}

void
writeDense8(int Value, volatile void *Base, register unsigned long Offset)
{
    write_mem_barrier();
    *(volatile CARD8 *)((unsigned long)Base+(Offset)) = Value;
}

void
writeDense16(int Value, volatile void *Base, register unsigned long Offset)
{
    write_mem_barrier();
    *(volatile CARD16 *)((unsigned long)Base+(Offset)) = Value;
}

void
writeDense32(int Value, volatile void *Base, register unsigned long Offset)
{
    write_mem_barrier();
    *(volatile CARD32 *)((unsigned long)Base+(Offset)) = Value;
}


#ifndef INCLUDE_XF86_NO_DOMAIN

void
_dense_outb(unsigned char val, unsigned long port)
{
    if ((port & ~0xffff) == 0) {
	_outb(val, port);
    } else {
	write_mem_barrier();
	*(volatile CARD8 *)port = val;
    }
}

void
_dense_outw(unsigned short val, unsigned long port)
{
    if ((port & ~0xffff) == 0) {
	_outw(val, port);
    } else {
	write_mem_barrier();
	*(volatile CARD16 *)port = val;
    }
}

void
_dense_outl(unsigned int val, unsigned long port)
{
    if ((port & ~0xffff) == 0) {
	_outl(val, port);
    } else {
	write_mem_barrier();
	*(volatile CARD32 *)port = val;
    }
}

unsigned char
_dense_inb(unsigned long port)
{
  if ((port & ~0xffff) == 0) return _inb(port);

  mem_barrier();
  return *(volatile CARD8 *)port;
}

unsigned short
_dense_inw(unsigned long port)
{
  if ((port & ~0xffff) == 0) return _inw(port);

  mem_barrier();
  return *(volatile CARD16 *)port;
}

unsigned int
_dense_inl(unsigned long port)
{
  if ((port & ~0xffff) == 0) return _inl(port);

  mem_barrier();
  return *(volatile CARD32 *)port;
}

#endif /* !INCLUDE_XF86_NO_DOMAIN */

