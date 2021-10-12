/* $XFree86: xc/programs/Xserver/hw/xfree86/drivers/xgi/xgi_cursor.h,v 1.1 2005/05/02 13:28:02 dawes Exp $ */
/*
 * XGI hardware cursor handling
 * Definitions
 *
 * Copyright (C) 2001-2004 by Thomas Winischhofer, Vienna, Austria.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1) Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2) Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3) The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESSED OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Author:   Thomas Winischhofer <thomas@winischhofer.net>
 *
 * Idea based on code by Can-Ru Yeou, XGI Inc.
 *
 */
#include "regs.h"
#define CS(x)   (0x8500 + (x << 2))

/* 300 series, CRT1 */

/* 80000000 = RGB(1) - MONO(0)
 * 40000000 = enable(1) - disable(0)
 * 20000000 = 32(1) / 16(1) bit RGB
 * 10000000 = "ghost"(1) - [other effect](0)
 */

#define xgiG1CRT1_DisableHWCursor()\
	{\
		XGIMMIOLONG(0x8500) &= ~(1<<30) ; \
	}

#define xgiG1CRT1_SetCursorBGColor(color)\
	{\
		XGIMMIOLONG(0x8504) = (color) ;\
	}

#define xgiG1CRT1_SetCursorFGColor(color)\
	{\
		XGIMMIOLONG(0x8508) = (color) ;\
	}

#define xgiG1CRT1_SetCursorPositionX(x,preset)\
	{\
		XGIMMIOLONG(0x850C) = (x) | ((preset)<<16);\
	}

#define xgiG1CRT1_SetCursorPositionY(y,preset)\
	{\
		XGIMMIOLONG(0x8510) = (y) | ((preset)<<16);\
	}

#define xgiG2CRT1_SetCursorAddressPattern(address,pat_id) \
{\
		unsigned long ulTemp ;\
		ulTemp = XGIMMIOLONG(0x8500) ;\
		ulTemp &= 0xF0FE0000 ;\
		ulTemp |= (address) & 0x1FFFF ;\
		ulTemp |= ((pat_id)&0xF)<<24 ;\
		XGIMMIOLONG(0x8500) = ulTemp ;\
}

#define xgiG2CRT2_SetCursorAddressPattern(address,pat_id) \
{\
		unsigned long ulTemp ;\
		ulTemp = XGIMMIOLONG(0x8520) ;\
		ulTemp &= 0xF0FE0000 ;\
		ulTemp |= (address) & 0x1FFFF ;\
		ulTemp |= ((pat_id)&0xF)<<24 ;\
		XGIMMIOLONG(0x8520) = ulTemp ;\
}

#define xgiG2CRT1_SetCursorAddress(address)\
	{\
		unsigned long ulTemp ;\
		ulTemp = XGIMMIOLONG(0x8500) ;\
		ulTemp &= 0xFFFE0000 ;\
		ulTemp |= (address) & 0x1FFFF ;\
		XGIMMIOLONG(0x8500) = ulTemp ;\
	}

#define xgiG2CRT1_SetCursorPatternSelect(pat_id)\
	{\
		unsigned long ulTemp ;\
		ulTemp = XGIMMIOLONG(0x8500) ;\
		ulTemp &= 0xF0FFFFFF ;\
		ulTemp |= ((pat_id)&0xF)<<24 ;\
		XGIMMIOLONG(0x8500) = ulTemp ;\
	}

#define xgiG1CRT2_DisableHWCursor()\
	{\
		XGIMMIOLONG(0x8520) &= ~(1<<30);\
	}

#define xgiG1CRT2_SetCursorBGColor(color)\
	{\
		XGIMMIOLONG(0x8524) = (color) ;\
	}

#define xgiG1CRT2_SetCursorFGColor(color)\
	{\
		XGIMMIOLONG(0x8528) = (color) ;\
	}

#define xgiG1CRT2_SetCursorPositionX(x,preset)\
	{\
		XGIMMIOLONG(0x852C) = (x) | ((preset)<<16);\
	}

#define xgiG1CRT2_SetCursorPositionY(y,preset)\
	{\
		XGIMMIOLONG(0x8530) = (y) | ((preset)<<16);\
	}

#define xgiG1CRT2_SetCursorAddress(address)\
	{\
		unsigned long ulTemp ;\
		ulTemp = XGIMMIOLONG(0x8520) ;\
		ulTemp &= 0xFFFE0000 ;\
		ulTemp |= (address) & 0x1FFFF ;\
		XGIMMIOLONG(0x8520) = ulTemp ;\
	}

#define xgiG1CRT2_SetCursorPatternSelect(pat_id)\
	{\
		unsigned long ulTemp ;\
		ulTemp = XGIMMIOLONG(0x8520) ;\
		ulTemp &= 0xF0FFFFFF ;\
		ulTemp |= ((pat_id)&0xF) << 24 ;\
		XGIMMIOLONG(0x8520) = ulTemp ;\
	}

#define xgiG2CRT1_EnableHWCursor(cursor_base,pat_id)\
	{\
	    CARD32 ulTemp ;\
		ulTemp = XGIMMIOLONG(0x8500) ;\
		ulTemp &= 0x00FE0000 ;\
		ulTemp |= 1<<30 ;\
		ulTemp |= (cursor_base) & 0x1FFFF ;\
		ulTemp |= ((pat_id)&0xF)<<24 ;\
		XGIMMIOLONG(0x8500) = ulTemp ;\
	}

#define xgiG2CRT1_DisableHWCursor()\
	{\
		XGIMMIOLONG(0x8500) &= ~(1<<30);\
	}

#define xgiG2CRT2_EnableHWCursor(cursor_base,pat_id)\
	{\
	    CARD32 ulTemp ;\
		ulTemp = XGIMMIOLONG(0x8520) ;\
		ulTemp &= 0x00FE0000 ;\
		ulTemp |= 1<<30 ;\
		ulTemp |= (cursor_base) & 0x1FFFF ;\
		ulTemp |= ((pat_id)&0xF)<<24 ;\
		XGIMMIOLONG(0x8520) = ulTemp ;\
	}

/*******************************************************************/





