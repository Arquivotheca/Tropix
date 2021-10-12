/*
 ****************************************************************
 *								*
 *			xpm.c					*
 *								*
 *	Leitura do Formato XPM					*
 *								*
 *	Versão	3.0.0, de 05.04.98				*
 *		4.4.0, de 08.04.03				*
 *								*
 *	Módulo: cmd/xpaint					*
 *		X Windows - Clientes				*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright (c) 2003 TROPIX Technologies Ltd.	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>

#include <stdio.h>
#include <string.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/xpm.h>

#include "../h/xpaint.h"

/*
 ****************************************************************
 *	Testa se o arquivo é XPM				*
 ****************************************************************
 */
int
is_a_XPM_file (FILE *fp)
{
	char		line[80];

	if (fngets (line, sizeof (line), fp) == NOSTR)
		return (0);

	return (streq (line, "/* XPM */"));

}	/* end is_a_XPM_file */

/*
 ****************************************************************
 *	Lê o arquivo XPM, gerando uma XImage			*
 ****************************************************************
 */
XImage *
load_XPM_file (FILE *fp, int image_number, Display *dpy,
		XStandardColormap *xstdcmap, IMAGE *ip)
{
	XImage			*ximage, *shapeimage;
	XWindowAttributes	root_attr;
	XpmAttributes		attr;
	int			val;
	Window			root;

	if (image_number != 1)
	{
		msg ("Arquivo XPM \"%s\" tem apenas uma imagem", ip->i_name);
		return (NULL);
	}

	root = RootWindow (dpy, DefaultScreen (dpy));

	XGetWindowAttributes (dpy, root, &root_attr);

	attr.colormap  = root_attr.colormap;
	attr.valuemask = XpmInfos | XpmReturnPixels | XpmColormap;

	fclose (fp);

	val = XpmReadFileToImage (dpy, (char *)ip->i_name, &ximage, &shapeimage, &attr);

	switch (val)
	{
	    case XpmSuccess:
		break;

	    case XpmOpenFailed:
		msg ("*Não consegui abrir \"%s\"", ip->i_name);
		break;

	    case XpmColorFailed:
		msg ("Impossível alocar as cores pedidas");
		break;

	    case XpmFileInvalid:
		msg ("Formato inválido do arquivo \"%s\"", ip->i_name);
		break;

	    case XpmColorError:
		msg ("Especificação de cor inválida em \"%s\"", ip->i_name);
		break;

	    case XpmNoMemory:
		msg ("*Memória insuficiente");
		break;
	}

	/*
	 *	Epílogo.
	 */
	if (ip != NULL)
	{
		ip->i_width	= attr.width;
		ip->i_height	= attr.height;
		ip->i_ncolors	= attr.ncolors;
	}

	if (val != XpmSuccess)
		ximage = NULL;

	return (ximage);

}	/* end load_XPM_file */
