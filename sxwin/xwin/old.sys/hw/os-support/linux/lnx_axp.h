/* $XFree86: xc/programs/Xserver/hw/xfree86/os-support/linux/lnx_axp.h,v 1.2 2006/02/20 00:14:37 dawes Exp $ */

#ifndef _LNX_AXP_H_
#define _LNX_AXP_H_

extern void _dense_outb(unsigned char, unsigned long);
extern void _dense_outw(unsigned short, unsigned long);
extern void _dense_outl(unsigned int, unsigned long);
extern unsigned char _dense_inb(unsigned long);
extern unsigned short _dense_inw(unsigned long);
extern unsigned int _dense_inl(unsigned long);

extern int readDense8(volatile void *Base, register unsigned long Offset);
extern int readDense16(volatile void *Base, register unsigned long Offset);
extern int readDense32(volatile void *Base, register unsigned long Offset);
extern void writeDenseNB8(int Value, volatile void *Base,
			  register unsigned long Offset);
extern void writeDenseNB16(int Value, volatile void *Base,
			   register unsigned long Offset);
extern void writeDenseNB32(int Value, volatile void *Base,
			   register unsigned long Offset);
extern void writeDense8(int Value, volatile void *Base,
			register unsigned long Offset);
extern void writeDense16(int Value, volatile void *Base,
			 register unsigned long Offset);
extern void writeDense32(int Value, volatile void *Base,
			 register unsigned long Offset);

#endif /* _LNX_AXP_H_ */
