/* $XFree86: xc/programs/Xserver/hw/xfree86/drivers/xgi/xgi_regs.h,v 1.1 2005/05/02 13:28:02 dawes Exp $ */
/*
 * Copyright 1998,1999 by Alan Hourihane, Wigan, England.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Alan Hourihane not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Alan Hourihane makes no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * ALAN HOURIHANE DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL ALAN HOURIHANE BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Authors:  Alan Hourihane, alanh@fairlite.demon.co.uk
 *           Mike Chapman <mike@paranoia.com>,
 *           Juanjo Santamarta <santamarta@ctv.es>,
 *           Mitani Hiroshi <hmitani@drl.mei.co.jp>
 *           David Thomas <davtom@dream.org.uk>.
 */

#ifndef _XGI_REGS_H_
#define _XGI_REGS_H_

#include "vgaHW.h"

#define inXGIREG(base)      inb(base)
#define outXGIREG(base,val) outb(base,val)
#define orXGIREG(base,val)  do { \
                                unsigned char temp = inb(base); \
                                outXGIREG(base, temp | (val)); \
                            } while (0)

#define andXGIREG(base,val) do { \
                                unsigned char temp = inb(base); \
                                outXGIREG(base, temp & (val)); \
                            } while (0)

#define inXGIIDXREG(base,idx,var)\
                    do { \
                        outb(base,idx); var=inb((base)+1); \
                    } while (0)

#define outXGIIDXREG(base,idx,val)\
                    do { \
                      outb(base,idx); outb((base)+1,val); \
                    } while (0)

#define orXGIIDXREG(base,idx,val)\
                    do { \
                        unsigned char temp; \
                        outb(base,idx);    \
                        temp = inb((base)+1)|(val); \
                        outXGIIDXREG(base,idx,temp); \
                    } while (0)
#define andXGIIDXREG(base,idx,and)\
                    do { \
                        unsigned char temp; \
                        outb(base,idx);    \
                        temp = inb((base)+1)&(and); \
                        outXGIIDXREG(base,idx,temp); \
                    } while (0)
#define setXGIIDXREG(base,idx,and,or)\
                    do { \
                        unsigned char temp; \
                        outb(base,idx);    \
                        temp = (inb((base)+1)&(and))|(or); \
                        outXGIIDXREG(base,idx,temp); \
                    } while (0)

#define BITMASK(h,l)             (((unsigned)(1U << ((h)-(l)+1))-1)<<(l))
#define GENMASK(mask)            BITMASK(1?mask,0?mask)

#define GETBITS(var,mask)        (((var) & GENMASK(mask)) >> (0?mask))
#define SETBITS(val,mask)        ((val) << (0?mask))
#define SETBIT(n)                (1<<(n))

#define GETBITSTR(val,from,to)    ((GETBITS(val,from)) << (0?to))
#define SETVARBITS(var,val,from,to)\
                (((var)&(~(GENMASK(to)))) | GETBITSTR(val,from,to))

#define GETVAR8(var)        ((var)&0xFF)
#define SETVAR8(var,val)    (var) =  GETVAR8(val)

#define VGA_RELIO_BASE    0x380

#define AROFFSET       VGA_ATTR_INDEX - VGA_RELIO_BASE
#define ARROFFSET      VGA_ATTR_DATA_R - VGA_RELIO_BASE
#define GROFFSET       VGA_GRAPH_INDEX - VGA_RELIO_BASE
#define SROFFSET       VGA_SEQ_INDEX - VGA_RELIO_BASE
#define CROFFSET       VGA_CRTC_INDEX_OFFSET + VGA_IOBASE_COLOR-VGA_RELIO_BASE
#define MISCROFFSET    VGA_MISC_OUT_R - VGA_RELIO_BASE
#define MISCWOFFSET    VGA_MISC_OUT_W - VGA_RELIO_BASE
#define DACROFFSET     VGA_DAC_READ_ADDR - VGA_RELIO_BASE
#define DACWOFFSET     VGA_DAC_WRITE_ADDR - VGA_RELIO_BASE
#define DACDOFFSET     VGA_DAC_DATA - VGA_RELIO_BASE
#define IS1OFFSET      VGA_IOBASE_COLOR - VGA_RELIO_BASE + VGA_IN_STAT_1_OFFSET

#define XGI_IS1        (pXGI->RelIO+IS1OFFSET)

/**********************************************************************/
#define IS_BIT_DIAGNOSTIC_RB (3<<4)
#define IS_BIT_VERT_ACTIVE (1<<3)
#define IS_BIT_HORZ_NACTIVE (1)
/**********************************************************************/

#define XGIARR         (pXGI->RelIO+ARROFFSET)
#define XGIGR          (pXGI->RelIO+GROFFSET)
#define XGISR          (pXGI->RelIO+SROFFSET)
#define XGICR          (pXGI->RelIO+CROFFSET)
#define XGIMISCR       (pXGI->RelIO+MISCROFFSET)
#define XGIMISCW       (pXGI->RelIO+MISCWOFFSET)
#define XGIDACREAD     (pXGI->RelIO+DACROFFSET)
#define XGIDACWRITE    (pXGI->RelIO+DACWOFFSET)
#define XGIDACDATA     (pXGI->RelIO+DACDOFFSET)
#define XGIVIDEO       (pXGI->RelIO+0x02)
#define XGIPART1       (pXGI->RelIO+0x04)
#define XGIPART2       (pXGI->RelIO+0x10)
#define XGIPART3       (pXGI->RelIO+0x12)
#define XGIPART4       (pXGI->RelIO+0x14)
#define XGIPART5       (pXGI->RelIO+0x16)


/* 3C4 */
#define BankReg 0x06
#define ClockReg 0x07
#define CPUThreshold 0x08
#define CRTThreshold 0x09
#define CRTCOff 0x0A
#define DualBanks 0x0B
#define MMIOEnable 0x0B
#define RAMSize 0x0C
#define Mode64 0x0C
#define ExtConfStatus1 0x0E
#define ClockBase 0x13
#define LinearAdd0 0x20
#define LinearAdd1 0x21
#define GraphEng 0x27
#define MemClock0 0x28
#define MemClock1 0x29
#define XR2A 0x2A
#define XR2B 0x2B
#define TurboQueueBase 0x2C
#define FBSize 0x2F
#define ExtMiscCont5 0x34
#define ExtMiscCont9 0x3C

/* 3x4 */
#define Offset 0x13

#define read_xr(num,var) do {outb(0x3c4, num);var=inb(0x3c5);} while (0)

/*  PART1 */
#define xgiPART1_FUNCTION                     0x00
#define xgiPART1_THRESHOLD_HIGH               0x01
#define xgiPART1_THRESHOLD_LOW                0x02
#define xgiPART1_FIFO_STOP                    0x03
#define xgiPART1_MEM_ADDR_HIGH                0x04
#define xgiPART1_MEM_ADDR_MID                 0x05
#define xgiPART1_MEM_ADDR_LOW                 0x06
#define xgiPART1_SCR_PITCH_LOW                0x07
#define xgiPART1_HORZ_TOTAL_LOW               0x08
#define xgiPART1_SCR_HTOTAL_OVERFLOW          0x09
#define xgiPART1_HORZ_DISP_END                0x0A
#define xgiPART1_HORZ_RETRACE_START           0x0B
#define xgiPART1_HORZ_OVERFLOW                0x0C
#define xgiPART1_HORZ_RETRACE_END             0x0D

#define xgiPART1_VERT_TOTAL_LOW               0x0E
#define xgiPART1_VERT_DISP_END                0x0F
#define xgiPART1_VERT_RETRACE_START           0x10
#define xgiPART1_VERT_RETRACE_END             0x11
#define xgiPART1_VERT_OVERFLOW                0x12

/* 2000/04/10 added by jjtseng */
/* [VBCTL_000410] */
#define xgiPART1_CRT2_FLIP                    0x24
#define xgiPART1_LOWRES_DUALVB_MODE           0x2c
/* ~jjtseng 2000/04/10 */

#define xgiPART1_ENABLEWRITE                  0x2f
#define xgiPART1_VERTRETRACE                  0x30
#define xgiPART1_HORZRETRACE                  0x33

/* Definitions for the XGI engine communication. */

extern int xgiReg32MMIO[];
#define BR(x) xgiReg32MMIO[x]

/* These are done using Memory Mapped IO, of the registers */
/*
 * Modified  by Xavier Ducoin (xavier@rd.lectra.fr)
 */


#define xgiLEFT2RIGHT           0x10
#define xgiRIGHT2LEFT           0x00
#define xgiTOP2BOTTOM           0x20
#define xgiBOTTOM2TOP           0x00

#define xgiSRCSYSTEM            0x03
#define xgiSRCVIDEO        0x02
#define xgiSRCFG        0x01
#define xgiSRCBG        0x00

#define xgiCMDBLT        0x0000
#define xgiCMDBLTMSK        0x0100
#define xgiCMDCOLEXP        0x0200
#define xgiCMDLINE        0x0300

#define xgiCMDENHCOLEXP        0x2000

#define xgiXINCREASE        0x10
#define xgiYINCREASE        0x20
#define xgiCLIPENABL        0x40
#define xgiCLIPINTRN        0x80
#define xgiCLIPEXTRN        0x00


#define xgiPATREG        0x08
#define xgiPATFG        0x04
#define xgiPATBG        0x00

#define xgiLASTPIX        0x0800
#define xgiXMAJOR        0x0400


/* Macros to do useful things with the XGI BitBLT engine */

#define xgiBLTSync \
  while(*(volatile unsigned short *)(pXGI->IOBase + BR(10)+2) & \
    (0x4000)){}

/* According to 6326 2D programming guide, 16 bits position at   */
/* 0x82A8 returns queue free. But this don't work, so don't wait     */
/* anything when turbo-queue is enabled. If there are frequent syncs */
/* this should work. But not for xaa_benchmark :-(     */

#define xgiBLTWAIT \
  if (!pXGI->TurboQueue) {\
    while(*(volatile unsigned short *)(pXGI->IOBase + BR(10)+2) & \
    (0x4000)){}} /* \
    else {while(*(volatile unsigned short *)(pXGI->IOBase + BR(10)) < \
    63){}} */

#define xgiSETPATREG()\
   ((unsigned char *)(pXGI->IOBase + BR(11)))

#define xgiSETPATREGL()\
   ((unsigned long *)(pXGI->IOBase + BR(11)))

#define xgiSETCMD(op) \
  *(volatile unsigned short *)(pXGI->IOBase + BR(10) +2 ) = op

#define xgiSETROPFG(op) \
  *(volatile unsigned int *)(pXGI->IOBase + BR(4)) = ((*(volatile unsigned int *)(pXGI->IOBase + BR(4)))&0xffffff) | (op<<24)

#define xgiSETROPBG(op) \
  *(volatile unsigned int *)(pXGI->IOBase + BR(5)) = ((*(volatile unsigned int *)(pXGI->IOBase + BR(5)))&0xffffff) | (op<<24)

#define xgiSETROP(op) \
   xgiSETROPFG(op);xgiSETROPBG(op);


#define xgiSETSRCADDR(srcAddr) \
  *(volatile unsigned int *)(pXGI->IOBase + BR(0)) = srcAddr&0x3FFFFFL

#define xgiSETDSTADDR(dstAddr) \
  *(volatile unsigned int *)(pXGI->IOBase + BR(1)) = dstAddr&0x3FFFFFL

#define xgiSETPITCH(srcPitch,dstPitch) \
  *(volatile unsigned int *)(pXGI->IOBase + BR(2)) = ((dstPitch&0xFFFF)<<16)| \
      (srcPitch&0xFFFF)

/* according to XGI 2D Engine Programming Guide
 * width -1 independant of Bpp
 */
#define xgiSETHEIGHTWIDTH(Height,Width)\
  *(volatile unsigned int *)(pXGI->IOBase + BR(3)) = (((Height)&0xFFFF)<<16)| \
      ((Width)&0xFFFF)

#define xgiSETCLIPTOP(x,y)\
  *(volatile unsigned int *)(pXGI->IOBase + BR(8)) = (((y)&0xFFFF)<<16)| \
      ((x)&0xFFFF)

#define xgiSETCLIPBOTTOM(x,y)\
  *(volatile unsigned int *)(pXGI->IOBase + BR(9)) = (((y)&0xFFFF)<<16)| \
      ((x)&0xFFFF)

#define xgiSETBGCOLOR(bgColor)\
  *(volatile unsigned int *)(pXGI->IOBase + BR(5)) = (bgColor)

#define xgiSETBGCOLOR8(bgColor)\
  *(volatile unsigned int *)(pXGI->IOBase + BR(5)) = (bgColor&0xFF)

#define xgiSETBGCOLOR16(bgColor)\
  *(volatile unsigned int *)(pXGI->IOBase + BR(5)) = (bgColor&0xFFFF)

#define xgiSETBGCOLOR24(bgColor)\
  *(volatile unsigned int *)(pXGI->IOBase + BR(5)) = (bgColor&0xFFFFFF)


#define xgiSETFGCOLOR(fgColor)\
  *(volatile unsigned int *)(pXGI->IOBase + BR(4)) = (fgColor)

#define xgiSETFGCOLOR8(fgColor)\
  *(volatile unsigned int *)(pXGI->IOBase + BR(4)) = (fgColor&0xFF)

#define xgiSETFGCOLOR16(fgColor)\
  *(volatile unsigned int *)(pXGI->IOBase + BR(4)) = (fgColor&0xFFFF)

#define xgiSETFGCOLOR24(fgColor)\
  *(volatile unsigned int *)(pXGI->IOBase + BR(4)) = (fgColor&0xFFFFFF)

/* Line drawing */

#define xgiSETXStart(XStart) \
  *(volatile unsigned int *)(pXGI->IOBase + BR(0)) = XStart&0xFFFF

#define xgiSETYStart(YStart) \
  *(volatile unsigned int *)(pXGI->IOBase + BR(1)) = YStart&0xFFFF

#define xgiSETLineMajorCount(MajorAxisCount) \
  *(volatile unsigned int *)(pXGI->IOBase + BR(3)) = MajorAxisCount&0xFFFF

#define xgiSETLineSteps(K1,K2) \
  *(volatile unsigned int *)(pXGI->IOBase + BR(6)) = (((K1)&0xFFFF)<<16)| \
      ((K2)&0xFFFF)

#define xgiSETLineErrorTerm(ErrorTerm) \
  *(volatile unsigned short *)(pXGI->IOBase + BR(7)) = ErrorTerm

#define XGIMMIOLONG(offset)  *(volatile unsigned long *)(pXGI->IOBase+(offset))
#define XGIMMIOSHORT(offset) *(volatile unsigned short *)(pXGI->IOBase+(offset))
#define XGIMMIOBYTE(offset)  *(volatile unsigned char *)(pXGI->IOBase+(offset))

#endif  /* _XGI_REGS_H_ */
