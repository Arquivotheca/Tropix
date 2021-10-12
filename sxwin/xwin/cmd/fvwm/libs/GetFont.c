/*
 ****************************************************************
 *								*
 *			GetFont.c				*
 *								*
 *	Localiza uma Fonte, pelo nome				*
 *								*
 *	Versão	1.0.0, de 27.08.97				*
 *		4.4.0, de 20.03.03				*
 *								*
 *	Módulo: cmd/fvwm/libs					*
 *		X Windows - Clientes				*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright (c) 2003 TROPIX Technologies Ltd.	*
 * 								*
 ****************************************************************
 */

#include "../global.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "fvwmlib.h"

XFontStruct *
GetFontOrFixed (Display *disp, char *fontname)
{
	XFontStruct	*fnt;

	if ((fnt = XLoadQueryFont (disp, fontname)) == NULL)
	{
		msg
		(	"Fonte \"%s\" não localizada; tentando \"fixed\"",
			fontname
		);

		if ((fnt = XLoadQueryFont (disp, "fixed")) == NULL) 
			msg ("Fonte \"fixed\" não localizada!");
	}

	return (fnt);

}	/* end GetFontOrFixed */
