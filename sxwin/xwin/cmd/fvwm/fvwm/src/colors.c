/*
 ****************************************************************
 *								*
 *			src/colors.c				*
 *								*
 *	Tratamento de cores					*
 *								*
 *	Versão	3.0.0, de 22.07.97				*
 *		4.4.0, de 20.03.03				*
 *								*
 *	Módulo: cmd/fvwm					*
 *		X Windows - Clientes				*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright (c) 2003 TROPIX Technologies Ltd.	*
 * 								*
 ****************************************************************
 */

/****************************************************************************
 * Copyright 1993 Robert Nation. No restrictions are placed on this code,
 * as long as the copyright notice is preserved
 ****************************************************************************/

#include "../global.h"

#include <sys/types.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <pwd.h>

#include <X11/Xproto.h>
#include <X11/Xatom.h>


#include "fvwm.h"
#include "menus.h"
#include "misc.h"
#include "parse.h"
#include "screen.h"

char *white = "white";
char *black = "black";

extern char *Hiback;
extern char *Hifore;

/*
 ****************************************************************
 *	Calcula a cor de sombreamento de uma dada cor		*
 ****************************************************************
 */
Pixel
GetShadow (Pixel background) 
{
	XColor			bg_color;
	XWindowAttributes	attributes;

	XGetWindowAttributes (dpy, Scr.Root, &attributes);
  
	bg_color.pixel = background;
	XQueryColor (dpy, attributes.colormap, &bg_color);
  
	bg_color.red	= (bg_color.red   % 0xffff) >> 1;
	bg_color.green	= (bg_color.green % 0xffff) >> 1;
	bg_color.blue	= (bg_color.blue  % 0xffff) >> 1;

	if(!XAllocColor (dpy, attributes.colormap, &bg_color))
	{
		nocolor ("alloc shadow","");
		bg_color.pixel = background;
	}
  
	return (bg_color.pixel);

}	/* end GetShadow */

/*
 ****************************************************************
 *	Determina a cor de realce de uma dada cor		*
 ****************************************************************
 */
Pixel
GetHilite (Pixel background) 
{
	XColor bg_color, white_p;
	XWindowAttributes attributes;
  
	XGetWindowAttributes(dpy,Scr.Root,&attributes);
  
	bg_color.pixel = background;
	XQueryColor(dpy,attributes.colormap,&bg_color);

	white_p.pixel = GetColor(white);
	XQueryColor(dpy,attributes.colormap,&white_p);
  
#ifndef min
#define min(a,b) (((a)<(b)) ? (a) : (b))
#define max(a,b) (((a)>(b)) ? (a) : (b))
#endif

	bg_color.red	= max ((white_p.red   / 5), bg_color.red);
	bg_color.green	= max ((white_p.green / 5), bg_color.green);
	bg_color.blue	= max ((white_p.blue  / 5), bg_color.blue);
  
	bg_color.red	= min (white_p.red,   (bg_color.red   * 140) / 100);
	bg_color.green	= min (white_p.green, (bg_color.green * 140) / 100);
	bg_color.blue	= min (white_p.blue,  (bg_color.blue  * 140) / 100);

#undef min
#ifdef max
#undef max
#endif
  
	if(!XAllocColor (dpy, attributes.colormap, &bg_color))
	{
		nocolor("alloc hilight","");
		bg_color.pixel = background;
	}

	return (bg_color.pixel);

}	/* end GetHilite */

/*
 ****************************************************************
 *	Cria os GCs						*
 ****************************************************************
 */
void
CreateGCs (void)
{
	XGCValues gcv;
	ulong gcm;
  
	/* create scratch GC's */
	gcm = GCFunction|GCPlaneMask|GCGraphicsExposures|GCLineWidth;
	gcv.line_width = 0;
	gcv.function = GXcopy;
	gcv.plane_mask = AllPlanes;
	gcv.graphics_exposures = False;
  
	Scr.ScratchGC1 = XCreateGC(dpy, Scr.Root, gcm, &gcv);
	Scr.ScratchGC2 = XCreateGC(dpy, Scr.Root, gcm, &gcv);
	Scr.ScratchGC3 = XCreateGC(dpy, Scr.Root, gcm, &gcv);

	/* create button transparency GC */
	Scr.ButtonMaskGC = XCreateGC(dpy, Scr.Root, gcm, &gcv);

}	/* end CreateGCs */

/*
 ****************************************************************
 *	Carrega uma cor						*
 ****************************************************************
 */
Pixel
GetColor (char *name)
{
	XColor	color;

	color.pixel = 0;

	if (!XParseColor (dpy, Scr.FvwmRoot.attr.colormap, name, &color)) 
		nocolor("parse",name);
	else if(!XAllocColor (dpy, Scr.FvwmRoot.attr.colormap, &color)) 
		nocolor("alloc",name);

	return (color.pixel);

}	/* end GetColor */

/*
 ****************************************************************
 *	Queixa-se acerca de cores				*
 ****************************************************************
 */
void
nocolor (char *note, char *name)
{
	fvwm_msg (ERR, "nocolor", "can't %s color \"%s\"", note,name);

}	/* end nocolor */
