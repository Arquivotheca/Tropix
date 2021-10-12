/*
 ****************************************************************
 *								*
 *			xsetroot.c				*
 *								*
 *	Altera características da janela-mãe			*
 *								*
 *	Versão	3.0.0, de 19.07.97				*
 *		4.4.0, de 19.03.03				*
 *								*
 *	Módulo: cmd/xsetroot					*
 *		X Windows - Clientes				*
 *		Baseado no software homônimo do XFree86		*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright (c) 2003 TROPIX Technologies Ltd.	*
 * 								*
 ****************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/cursorfont.h>

#include "gray.v"

/*
 ****************************************************************
 *	Nome e versão do programa				*
 ****************************************************************
 */
entry	const char	pgversion[]	= "Versão 4.4.0, de 19.03.03";

#define Dynamic 1
#define	BMPPATH "/usr/xwin/bitmaps/"

typedef	unsigned int	uint;

/*
 ****************************************************************
 *	Variáveis						*
 ****************************************************************
 */
entry	Display		*dpy;
entry	int		screen;
entry	Window		root;
entry	char		*fore_color = NULL;
entry	char		*back_color = NULL;
entry	int		reverse = 0;
entry	int		save_colors = 0;
entry	int		unsave_past = 0;
entry	Pixmap		save_pixmap = (Pixmap)None;

/*
 ****************************************************************
 *	Protótipos de Funções					*
 ****************************************************************
 */
void	help (int);
void	msg (char *, ...);
void	FixupState (void);

Pixmap	MakeModulaBitmap (int, int);
Pixmap	ReadBitmapFile (char *, uint *, uint *, int *, int *);

XColor	NameToXColor (char *, ulong);
ulong	NameToPixel (char *, ulong);
void	SetBackgroundToBitmap (Pixmap, uint, uint);
Cursor	CreateCursorFromName (char *name);
Cursor	CreateCursorFromFiles (char *, char *);

int	cursorname_to_index (char *);

/*
 ****************************************************************
 *	Programa Principal					*
 ****************************************************************
 */
void
main (int argc, const char *argv[]) 
{
	int	excl = 0;
	int	nonexcl = 0;
	int	restore_defaults = 0;
	char	*display_name = NULL;
	char	*cursor_file = NULL;
	char	*cursor_mask = NULL;
	char	*cursor_name = NULL;
	char	*solid_color = NULL;
	Cursor	cursor;
	int	gray = 0;
	char	*bitmap_file = NULL;
	int	mod_x = 0;
	int	mod_y = 0;
	int	i;
	uint	ww, hh;
	Pixmap	bitmap;

	for (i = 1; i < argc; i++)
	{
		if (streq ("-display", argv[i]) || streq ("-d", argv[i]))
		{
			if (++i>=argc) help (1);
			display_name = (char *)argv[i];
			continue;
		}

		if (streq ("-help", argv[i]) || !strcmp ("-H", argv[i]))
			help (0);

		if (streq ("-def", argv[i]) || streq ("-default", argv[i]))
		{
			restore_defaults = 1;
			continue;
		}

		if (streq ("-cursor", argv[i]))
		{
			if (++i>=argc) help (1);
			cursor_file = (char *)argv[i];
			if (++i>=argc) help (1);
			cursor_mask = (char *)argv[i];
			nonexcl++;
			continue;
		}

		if (streq ("-cursor_name", argv[i]))
		{
			if (++i>=argc) help (1);
			cursor_name = (char *)argv[i];
			nonexcl++;
			continue;
		}

		if (streq ("-fg",argv[i]) || streq ("-foreground",argv[i]))
		{
			if (++i>=argc) help (1);
			fore_color = (char *)argv[i];
			continue;
		}

		if (streq ("-bg",argv[i]) || streq ("-background",argv[i]))
		{
			if (++i>=argc) help (1);
			back_color = (char *)argv[i];
			continue;
		}

		if (streq ("-solid", argv[i]))
		{
			if (++i>=argc) help (1);
			solid_color = (char *)argv[i];
			excl++;
			continue;
		}

		if (streq ("-gray", argv[i]) || streq ("-grey", argv[i]))
		{
			gray = 1;
			excl++;
			continue;
		}
	
		if (streq ("-bitmap", argv[i]))
		{
			if (++i>=argc) help (1);
			bitmap_file = (char *)argv[i];
			excl++;
			continue;
		}

		if (streq ("-mod", argv[i]))
		{
			if (++i>=argc) help (1);
			mod_x = atoi(argv[i]);
			if (mod_x <= 0) mod_x = 1;
			if (++i>=argc) help (1);
			mod_y = atoi(argv[i]);
			if (mod_y <= 0) mod_y = 1;
			excl++;
			continue;
		}

		if (streq ("-rv",argv[i]) || streq ("-reverse",argv[i]))
		{
			reverse = 1;
			continue;
		}

		help (1);
	} 

	/* Check for multiple use of exclusive options */
	if (excl > 1)
	{
		msg
		(	"as opções \"-solid\", \"-gray\", \"-bitmap\" "
			"e \"-mod\" são mutuamente exclusivas"
		);
		help (1);
	}

	if ((dpy = XOpenDisplay (display_name)) == NULL)
	{
		msg
		(	"$Não consegui conexão com o servidor \"%s\"",
			XDisplayName (display_name)
		);
	}

	screen = DefaultScreen (dpy);
	root = RootWindow (dpy, screen);
  
	/* If there are no arguments then restore defaults. */
	if (!excl && !nonexcl)
		restore_defaults = 1;
  
	/* Handle a cursor file */
	if (cursor_file)
	{
		cursor = CreateCursorFromFiles (cursor_file, cursor_mask);
		XDefineCursor (dpy, root, cursor);
		XFreeCursor (dpy, cursor);
	}
  
	if (cursor_name)
	{
		cursor = CreateCursorFromName (cursor_name);
		if (cursor)
		{
			XDefineCursor (dpy, root, cursor);
			XFreeCursor (dpy, cursor);
		}
	}

	/* Handle -gray and -grey options */
	if (gray)
	{
		bitmap =	XCreateBitmapFromData
				(	dpy, root, gray_bits,
					gray_width, gray_height
				);

		SetBackgroundToBitmap (bitmap, gray_width, gray_height);
	}
  
	/* Handle -solid option */
	if (solid_color)
	{
		XSetWindowBackground
		(	dpy,
			root,
			NameToPixel
			(	solid_color,
				BlackPixel (dpy, screen)
			)
		);

		XClearWindow (dpy, root);
		unsave_past = 1;
	}
  
	/* Handle -bitmap option */
	if (bitmap_file)
	{
		bitmap =	ReadBitmapFile
				(	bitmap_file,
					&ww, &hh,
					(int *)NULL, (int *)NULL
				);

		SetBackgroundToBitmap (bitmap, ww, hh);
	}

	/* Handle set background to a modula pattern */
	if (mod_x)
	{
		bitmap = MakeModulaBitmap (mod_x, mod_y);
		SetBackgroundToBitmap (bitmap, 16, 16);
	}
  
	/* Handle restore defaults */
	if (restore_defaults)
	{
		if (!cursor_file)
			XUndefineCursor (dpy, root);

		if (!excl)
		{
			XSetWindowBackgroundPixmap (dpy, root, (Pixmap)None);
			XClearWindow (dpy, root);
			unsave_past = 1;
		}
	}
  
	FixupState();
	XCloseDisplay(dpy);
	exit (0);

}	/* end main */

/*
 ****************************************************************
 *	x
 ****************************************************************
 */
/* Free past incarnation if needed, and retain state if needed. */
void
FixupState (void)
{
	Atom	prop, type;
	int	format;
	ulong	length, after;
	uchar	*data;

	if (!(DefaultVisual(dpy, screen)->class & Dynamic))
		unsave_past = 0;

	if (!unsave_past && !save_colors)
		return;

	prop = XInternAtom(dpy, "_XSETROOT_ID", False);
	if (unsave_past)
	{
		(void)XGetWindowProperty
		(	dpy, root, prop,
			0L, 1L, True, AnyPropertyType,
			&type, &format, &length, &after, &data
		);

		if
		(	(type == XA_PIXMAP) && (format == 32) &&
			(length == 1) && (after == 0)
		)
		{
			XKillClient (dpy, *((Pixmap *)data));
		}
		else if (type != None)
		{
			msg ("warning: _XSETROOT_ID property is garbage");
		}
	}

	if (save_colors)
	{
		if (!save_pixmap)
			save_pixmap = XCreatePixmap (dpy, root, 1, 1, 1);

		XChangeProperty
		(	dpy, root, prop, XA_PIXMAP, 32, PropModeReplace,
			(uchar *) &save_pixmap, 1
		);

		XSetCloseDownMode (dpy, RetainPermanent);
	}

}	/* end FixupState */

/*
 ****************************************************************
 *	Estabelece um "bitmap" como padrão de fundo		*
 ****************************************************************
 */
/*
 * SetBackgroundToBitmap: Set the root window background to a caller supplied 
 *                        bitmap.
 */
void
SetBackgroundToBitmap (Pixmap bitmap, uint width, uint height)
{
	Pixmap		pix;
	GC		gc;
	XGCValues	gc_init;

	gc_init.foreground = NameToPixel (fore_color, BlackPixel(dpy, screen));
	gc_init.background = NameToPixel (back_color, WhitePixel(dpy, screen));

	if (reverse)
	{
		ulong temp = gc_init.foreground;

		gc_init.foreground = gc_init.background;
		gc_init.background = temp;
	}

	gc  =	XCreateGC (dpy, root, GCForeground|GCBackground, &gc_init);

	pix =	XCreatePixmap
		(	dpy, root, width, height,
			(uint)DefaultDepth (dpy, screen)
		);

	XCopyPlane
	(	dpy, bitmap, pix, gc,
		0, 0, width, height, 0, 0, (ulong)1
	);

	XSetWindowBackgroundPixmap (dpy, root, pix);
	XFreeGC (dpy, gc);
	XFreePixmap (dpy, bitmap);

	if (save_colors)
		save_pixmap = pix;
	else
		XFreePixmap (dpy, pix);

	XClearWindow (dpy, root);
	unsave_past = 1;

}	/* end SetBackgroundToBitmap */


/*
 * CreateCursorFromFiles: make a cursor of the right colors from two bitmap
 *                        files.
 */
#define BITMAP_HOT_DEFAULT 8

Cursor
CreateCursorFromFiles (char *cursor_file, char *mask_file)
{
    Pixmap cursor_bitmap, mask_bitmap;
    unsigned int width, height, ww, hh;
    int x_hot, y_hot;
    Cursor cursor;
    XColor fg, bg, temp;

    fg = NameToXColor(fore_color, BlackPixel(dpy, screen));
    bg = NameToXColor(back_color, WhitePixel(dpy, screen));

    if (reverse)
    {
	temp = fg; fg = bg; bg = temp;
    }

    cursor_bitmap =	ReadBitmapFile
			(	cursor_file, &width, &height,
				&x_hot, &y_hot
			);

    mask_bitmap =	ReadBitmapFile
			(	mask_file, &ww, &hh,
				(int *)NULL, (int *)NULL
			);

    if (width != ww || height != hh)
	msg ("$dimensions of cursor bitmap and cursor mask bitmap are different");

    if ((x_hot == -1) && (y_hot == -1)) {
	x_hot = BITMAP_HOT_DEFAULT;
	y_hot = BITMAP_HOT_DEFAULT;
    }

    if ((x_hot < 0) || (x_hot >= width) || (y_hot < 0) || (y_hot >= height))
	msg ("hotspot is outside cursor bounds");

    cursor = XCreatePixmapCursor(dpy, cursor_bitmap, mask_bitmap, &fg, &bg,
				 (unsigned int)x_hot, (unsigned int)y_hot);
    XFreePixmap(dpy, cursor_bitmap);
    XFreePixmap(dpy, mask_bitmap);

    return(cursor);
}

Cursor
CreateCursorFromName (char *name)
{
    XColor fg, bg, temp;
    int	    i;
    Font    fid;

    fg = NameToXColor(fore_color, BlackPixel(dpy, screen));
    bg = NameToXColor(back_color, WhitePixel(dpy, screen));
    if (reverse) {
	temp = fg; fg = bg; bg = temp;
    }
    i = cursorname_to_index (name);
    if (i == -1)
	return (Cursor) 0;
    fid = XLoadFont (dpy, "cursor");
    if (!fid)
	return (Cursor) 0;
    return XCreateGlyphCursor (dpy, fid, fid,
			       i, i+1, &fg, &bg);
}

/*
 ****************************************************************
 *	x
 ****************************************************************
 */
/*
 * MakeModulaBitmap: Returns a modula bitmap based on an x & y mod.
 */
Pixmap
MakeModulaBitmap (int mod_x, int mod_y)
{
	int	i;
	long	pattern_line = 0;
	char	modula_data[16*16/8];

	for (i = 0; i < 16; i++)
	{
		pattern_line <<=1;
		if ((i % mod_x) == 0) pattern_line |= 0x0001;
	}

	for (i = 0; i < 16; i++)
	{
		if ((i % mod_y) == 0)
		{
			modula_data[i*2] = 0xff;
			modula_data[i*2+1] = 0xff;
		}
		else
		{
			modula_data[i*2] = pattern_line & 0xff;
			modula_data[i*2+1] = (pattern_line>>8) & 0xff;
		}
	}

	return (XCreateBitmapFromData (dpy, root, modula_data, 16, 16));

}	/* end MakeModulaBitmap */

/*
 ****************************************************************
 *	x
 ****************************************************************
 */
/*
 * NameToXColor: Convert the name of a color to its Xcolor value.
 */
XColor
NameToXColor (char *name, ulong pixel)
{
	XColor	c;
    
	if (!name || !*name)
	{
		c.pixel = pixel;
		XQueryColor(dpy, DefaultColormap(dpy, screen), &c);
	}
	else if (!XParseColor(dpy, DefaultColormap(dpy, screen), name, &c))
	{
		msg ("$unknown color or bad color format: %s", name);
	}

	return(c);

}	/* end NameToXColor */

/*
 ****************************************************************
 *	x
 ****************************************************************
 */
ulong
NameToPixel (char *name, ulong pixel)
{
	XColor ecolor;

	if (!name || !*name)
		return pixel;

	if (!XParseColor (dpy, DefaultColormap (dpy, screen), name, &ecolor))
		msg ("$cor desconhecida \"%s\"", name);

	if (!XAllocColor (dpy, DefaultColormap (dpy, screen), &ecolor))
		msg ("$unable to allocate color for \"%s\"", name);

	if
	(	(ecolor.pixel != BlackPixel (dpy, screen)) &&
		(ecolor.pixel != WhitePixel (dpy, screen)) &&
		(DefaultVisual (dpy, screen)->class & Dynamic)
	)
		save_colors = 1;

	return (ecolor.pixel);

}	/* end NameToPixel */

/*
 ****************************************************************
 *	Lê um "bitmap"						*
 ****************************************************************
 */
Pixmap
ReadBitmapFile (char *filename, uint *width, uint *height,
			int *x_hot, int *y_hot)
{
	Pixmap	bitmap;
	int	status;

again:
	status =	XReadBitmapFile
			(	dpy, root, filename, width,
				height, &bitmap, x_hot, y_hot
			);

	if (status == BitmapSuccess)
	      return (bitmap);

	if (status == BitmapOpenFailed)
	{
		if (*filename != '/')
		{
			char	*fullname;
			int	dirlen = strlen (BMPPATH);
			int	len = strlen (filename);

			fullname = malloc (dirlen + len + 1);
			strcpy (fullname, BMPPATH);
			strcpy (fullname + dirlen, filename);
			filename = fullname;
			goto again;
		}
		else
			msg ("$não posso abrir \"%s\"", filename);
	}
	else if (status == BitmapFileInvalid)
		msg ("$\"%s\" não é um \"bitmap\"", filename);
	else
		msg ("$memória insuficiente para ler \"%s\"", filename);

	return ((Pixmap)NULL);

}	/* end ReadBitmapFile */

/*
 ****************************************************************
 *	Resumo de Utilizacao do Programa			*
 ****************************************************************
 */
static void
help (int exitval)
{
	fprintf
	(	stderr,
		"%s - modifica características da janela-mãe\n\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-d <display>] [-fg <frente>] [-bg <fundo>] "
		"[-rv] [-def]\n"
		"\t         [-solid <fundo>] [-gray] "
		"[-bitmap <arquivo>] [-mod <x> <y>]\n",
		pgname, pgversion, pgname
	);

	fprintf
	(	stderr,
		"\nOpções:"
		"\t-d:      contacta o servidor <display>\n"
		"\t-fg:     define <frente> como cor de frente (default: preto)\n"
		"\t-bg:     define <fundo>  como cor de fundo  (default: branco)\n"
		"\t-rv:     permuta as cores <frente> e <fundo>\n"
		"\t-def:    volta tudo ao \"default\"\n"
	);

	fprintf
	(	stderr,
#if (0)	/****************************************************/
		"\t-cursor: <cursor file> <mask file>\n"
		"\t-cursor_name <cursor-font name>\n"
#endif	/****************************************************/
		"\t-solid:  estabelece <fundo> como padrão de fundo\n"
		"\t-gray:   estabelece um padrão 1x1 com as cores "
		"<fundo> e <frente>\n"
		"\t-bitmap: estabelece o bitmap <arquivo> como padrão de fundo\n"
		"\t-mod:    faz um reticulado de espessura <x> <y>\n"
		"\t         com as cores <fundo> e <frente>\n"
	);

	exit (exitval);

}	/* end help */

/*
 ****************************************************************
 *	Converte uma cadeia para um tipo de cursor		*
 ****************************************************************
 */
typedef struct
{
	char		*name;
	unsigned int	shape;

}	CNAME;

CNAME	cursor_names[] =
{
	{"x_cursor",		XC_X_cursor},
	{"arrow",		XC_arrow},
	{"based_arrow_down",	XC_based_arrow_down},
	{"based_arrow_up",	XC_based_arrow_up},
	{"boat",		XC_boat},
	{"bogosity",		XC_bogosity},
	{"bottom_left_corner",	XC_bottom_left_corner},
	{"bottom_right_corner",	XC_bottom_right_corner},
	{"bottom_side",		XC_bottom_side},
	{"bottom_tee",		XC_bottom_tee},
	{"box_spiral",		XC_box_spiral},
	{"center_ptr",		XC_center_ptr},
	{"circle",		XC_circle},
	{"clock",		XC_clock},
	{"coffee_mug",		XC_coffee_mug},
	{"cross",		XC_cross},
	{"cross_reverse",	XC_cross_reverse},
	{"crosshair",		XC_crosshair},
	{"diamond_cross",	XC_diamond_cross},
	{"dot",			XC_dot},
	{"dotbox",		XC_dotbox},
	{"double_arrow",	XC_double_arrow},
	{"draft_large",		XC_draft_large},
	{"draft_small",		XC_draft_small},
	{"draped_box",		XC_draped_box},
	{"exchange",		XC_exchange},
	{"fleur",		XC_fleur},
	{"gobbler",		XC_gobbler},
	{"gumby",		XC_gumby},
	{"hand1",		XC_hand1},
	{"hand2",		XC_hand2},
	{"heart",		XC_heart},
	{"icon",		XC_icon},
	{"iron_cross",		XC_iron_cross},
	{"left_ptr",		XC_left_ptr},
	{"left_side",		XC_left_side},
	{"left_tee",		XC_left_tee},
	{"leftbutton",		XC_leftbutton},
	{"ll_angle",		XC_ll_angle},
	{"lr_angle",		XC_lr_angle},
	{"man",			XC_man},
	{"middlebutton",	XC_middlebutton},
	{"mouse",		XC_mouse},
	{"pencil",		XC_pencil},
	{"pirate",		XC_pirate},
	{"plus",		XC_plus},
	{"question_arrow",	XC_question_arrow},
	{"right_ptr",		XC_right_ptr},
	{"right_side",		XC_right_side},
	{"right_tee",		XC_right_tee},
	{"rightbutton",		XC_rightbutton},
	{"rtl_logo",		XC_rtl_logo},
	{"sailboat",		XC_sailboat},
	{"sb_down_arrow",	XC_sb_down_arrow},
	{"sb_h_double_arrow",	XC_sb_h_double_arrow},
	{"sb_left_arrow",	XC_sb_left_arrow},
	{"sb_right_arrow",	XC_sb_right_arrow},
	{"sb_up_arrow",		XC_sb_up_arrow},
	{"sb_v_double_arrow",	XC_sb_v_double_arrow},
	{"shuttle",		XC_shuttle},
	{"sizing",		XC_sizing},
	{"spider",		XC_spider},
	{"spraycan",		XC_spraycan},
	{"star",		XC_star},
	{"target",		XC_target},
	{"tcross",		XC_tcross},
	{"top_left_arrow",	XC_top_left_arrow},
	{"top_left_corner",	XC_top_left_corner},
	{"top_right_corner",	XC_top_right_corner},
	{"top_side",		XC_top_side},
	{"top_tee",		XC_top_tee},
	{"trek",		XC_trek},
	{"ul_angle",		XC_ul_angle},
	{"umbrella",		XC_umbrella},
	{"ur_angle",		XC_ur_angle},
	{"watch",		XC_watch},
	{"xterm",		XC_xterm},
	{NULL,			0}
};

CNAME	*last_entry = &cursor_names[sizeof (cursor_names) / sizeof (CNAME) - 1];

int
cursorname_to_index (char *name)
{
	CNAME	*tp;
	char	*tmp, *cp, c;

	tmp = strdup (name);
	for (cp = tmp; (c = *cp) != '\0'; cp++)
	{
		if (c >= 'A' && c <= 'Z')
			*cp = c + ('a' - 'A');
	}

#if (0)	/****************************************************/
	int	i;
	char	tmp[40];

	if (strlen (name) >= sizeof (tmp))
		return (-1);
	XmuCopyISOLatin1Lowered (tmp, name);
#endif	/****************************************************/

	last_entry->name = name;
	for (tp = cursor_names; strcmp (tmp, tp->name); tp++)
		/* vazio */;

	free (tmp);

	return (tp == last_entry ? -1 : tp->shape);
}
