/*
 * Copyright (c) 2005 Bruno Schwander
 * Author: Bruno Schwander <bruno@tinkerbox.org>
 * Template driver used: dmc:
 *
 * Copyright (c) 1999  Machine Vision Holdings Incorporated
 * Author: Mayk Langer <langer@vsys.de>
 *
 * Template driver used: Copyright (c) 1998  Metro Link Incorporated
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
 * OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */
/* $XFree86: xc/programs/Xserver/hw/xfree86/input/magictouch/xf86MagicTouch.h,v 1.1 2005/06/11 02:40:55 dawes Exp $ */

#ifndef	_MGT_H_
#define _MGT_H_

#define MGT_PACKET_SIZE		5
#define MGT_TOUCH               0x40

typedef enum
{
	MGT_byte0, MGT_byte1, MGT_byte2, MGT_byte3, MGT_byte4,
	MGT_Response0
}
MGTState;

typedef struct _MGTPrivateRec
{
	int min_x;				/* Minimum x reported by calibration        */
	int max_x;				/* Maximum x                    */
	int min_y;				/* Minimum y reported by calibration        */
	int max_y;				/* Maximum y                    */
	Bool button_down;			/* is the "button" currently down */
	int button_number;			/* which button to report */
	int reporting_mode;			/* MGT_stream or MGT_point */

	int screen_num;				/* Screen associated with the device        */
	int screen_width;			/* Width of the associated X screen     */
	int screen_height;			/* Height of the screen             */
	int proximity;
	int swap_xy;

        XISBuffer *buffer;
	unsigned char packet[MGT_PACKET_SIZE];	/* packet being/just read */
	MGTState lex_mode;
	char pen_down;
}
MGTPrivateRec, *MGTPrivatePtr;

/******************************************************************************
 *		Declarations
 *****************************************************************************/

static Bool DeviceControl (DeviceIntPtr, int);
static void ReadInput (InputInfoPtr);
static int ControlProc (InputInfoPtr, xDeviceCtl *);
static void CloseProc (InputInfoPtr);
static int SwitchMode (ClientPtr, DeviceIntPtr, int);
static Bool ConvertProc (InputInfoPtr, int, int, int, int, int, int, int, int, int *, int *);
static Bool QueryHardware (MGTPrivatePtr);
static Bool MGTGetPacket (MGTPrivatePtr priv);
static Bool MGTSendPacket (MGTPrivatePtr priv, unsigned char *buf, int len );

static InputInfoPtr
MGTPreInit(InputDriverPtr drv, IDevPtr dev, int flags);

static void
MGTPtrCtrl(DeviceIntPtr device, PtrCtrl *ctrl);


#endif /* _MGT_H_ */
