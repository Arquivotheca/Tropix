/*
 ****************************************************************
 *								*
 *			src/xterm.c				*
 *								*
 *	Programa Principal					*
 *								*
 *	Versão	3.0.0, de 23.02.98				*
 *		4.8.0, de 18.03.05				*
 *								*
 *	Módulo: cmd/xterm					*
 *		X Windows - Clientes				*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright (c) 2005 TROPIX Technologies Ltd.	*
 * 								*
 ****************************************************************
 */

#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <assert.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/cursorfont.h>

#include "../h/feature.h"
#include "../h/command.h"
#include "../h/misc.h"
#include "../h/main.h"
#include "../h/screen.h"
#include "../h/scrollbar.h"
#include "../h/xdefaults.h"

/*
 ****************************************************************
 *	Nome e versão do programa				*
 ****************************************************************
 */
const char	pgversion[]	=	"Versão 4.8.0, de 18.03.05";

/*
 ****************************************************************
 *	Variáveis Globais					*
 ****************************************************************
 */
TermWin_t	TermWin;
Display		*dpy;	/* display */

const char	*rs_color [NRS_COLORS];
Pixel		PixColors [NRS_COLORS + NSHADOWCOLORS];

ulong		Options = (Opt_scrollBar);

#define NFONTS		11

const char	*display_name;
const char	*home_directory;
const char	*desktop_number;
const char	*rs_name = NULL;	/* client instance (resource name) */
const char	*rs_font [NFONTS];
const char	*rs_cutchars = NULL;

const char	color_not_available[]	= "cor \"%s\" não disponível";
const char	font_not_available[]	= "fonte \"%s\" não disponível";

int		saved_nice;		/* Salva o valor original do "nice" */

#define BORDERWIDTH	1	/* borderWidth [pixels] */
#define SAVELINES	64	/* saveLines [lines] */

/* character class of separating chars for multiple-click selection */
#define CUTCHARS	"\t \"&'()*,;<=>?@[\\]^`{|}~"

/* fonts used */

/* the logical position of font0 in the list */
#define FONT0_IDX 2

#ifndef NO_BOLDFONT
const char	*rs_boldFont = NULL;
#endif

Cursor	TermWin_cursor;	/* cursor for vt window */

XSizeHints szHint =
{
	PMinSize | PResizeInc | PBaseSize | PWinGravity,
	0, 0, 80, 24,	/* x, y, width, height */
	1, 1,		/* Min width, height */
	0, 0,		/* Max width, height - unused*/
	1, 1,		/* increments: width, height */
	{1, 1},		/* increments: x, y */
	{0, 0},		/* Aspect ratio - unused */
	0, 0,		/* base size: width, height */
	NorthWestGravity	/* gravity */
};

const char	*def_colorName [] =
{
	"Black", "White",	/* fg/bg */
	"Black",		/* 0: black		(#000000) */

	/* low-intensity colors */
	"Blue3",		/* 1: blue		(#0000CD) */
	"Green3",		/* 2: green		(#00CD00) */
	"Yellow3",		/* 3: yellow		(#CDCD00) */
	"Red3",			/* 4: red		(#CD0000) */
	"Magenta3",		/* 5: magenta		(#CD00CD) */
	"Cyan3",		/* 6: cyan		(#00CDCD) */
	"AntiqueWhite",		/* 7: white		(#FAEBD7) */

	/* high-intensity colors */
	"Grey25",		/* 8: bright black	(#404040) */
	"Blue",			/* 1/9:  bright blue	(#0000FF) */
	"Green",		/* 2/10: bright green	(#00FF00) */
	"Yellow",		/* 3/11: bright yellow	(#FFFF00) */
	"Red",			/* 4/12: bright red	(#FF0000) */
	"Magenta",		/* 5/13: bright magenta	(#FF00FF) */
	"Cyan",			/* 6/14: bright cyan	(#00FFFF) */
	"White",		/* 7/15: bright white	(#FFFFFF) */

	NULL, NULL,

#ifndef NO_BOLDUNDERLINE
	NULL, NULL,
#endif	/* NO_BOLDUNDERLINE */
	"#B2B2B2"		/* scrollColor: match Netscape color */
};

const char	*def_fontName[] =
{
	"6x10",
	"6x13",
	"7x13",
	"7x13bold",
	"8x13",
	"9x15",
	"9x15bold",

	"-adobe-courier-medium-r-normal--12-120-75-75-m-70-iso8859-1",
	"-adobe-courier-medium-r-normal--14-140-75-75-m-90-iso8859-1"
	"-adobe-courier-medium-r-normal--18-180-75-75-m-110-iso8859-1",
	"-adobe-courier-medium-r-normal--24-240-75-75-m-150-iso8859-1",
};
#if (0)	/*******************************************************/
	"-adobe-courier-medium-r-normal--8-80-75-75-m-50-iso8859-1",
	"-adobe-courier-medium-r-normal--10-100-75-75-m-60-iso8859-1",
#endif	/*******************************************************/

/*
 ****************************************************************
 *	Protótipos de Funções					*
 ****************************************************************
 */
void	create_windows (int, const char *[]);
void	resize (void);

/*
 ****************************************************************
 *	Ponto de Entrada					*
 ****************************************************************
 */
int
main (int argc, const char *argv [])
{
	int		i;
	char		*val;
	const char	**cmd_argv = NULL;

/***	read_symtb ("/usr/xwin/bin/xterm");	***/

	for (i = 0; i < argc; i++)
	{
		if (streq (argv [i], "-e"))
		{
			argc = i;
			argv [argc] = NULL;
			if (argv [argc + 1] != NULL)
			{
				cmd_argv = (argv + argc + 1);

				if (cmd_argv [0] != NULL)
					rs_title = basename (cmd_argv [0]);
			}

			break;
		}
	}

	rs_name = basename (argv [0]);

	get_options (argc, argv);

	if (Options & Opt_console)
		rs_name = "xconsole";

	if ((dpy = XOpenDisplay (display_name)) == NULL)
	{
		msg
		(	"$Não consegui contactar o servidor \"%s\"",
			XDisplayName (display_name)
		);
	}

	extract_resources (dpy, rs_name);

	/*
	 * set any defaults not already set
	 */
	if (!rs_title)
		rs_title = rs_name;
	if (!rs_saveLines || (TermWin.saveLines = atoi (rs_saveLines)) < 0)
		TermWin.saveLines = SAVELINES;

	/* no point having a scrollbar without having any scrollback! */
	if (!TermWin.saveLines)
		Options &= ~Opt_scrollBar;

	if (!rs_cutchars)
		rs_cutchars = CUTCHARS;

#ifndef NO_BOLDFONT
	if (rs_font [0] == NULL && rs_boldFont != NULL)
	{
		rs_font [0] = rs_boldFont;
		rs_boldFont = NULL;
	}
#endif
	for (i = 0; i < NFONTS; i++)
	{
		if (!rs_font [i])
			rs_font [i] = def_fontName [i];
	}

#ifdef XTERM_REVERSE_VIDEO
	/* this is how xterm implements reverseVideo */
	if (Options & Opt_reverseVideo)
	{
		if (!rs_color [fgColor])
			rs_color [fgColor] = def_colorName [bgColor];

		if (!rs_color [bgColor])
			rs_color [bgColor] = def_colorName [fgColor];
	}
#endif

	for (i = 0; i < NRS_COLORS; i++)
	{
		if (rs_color [i] == NOSTR)
			rs_color [i] = def_colorName [i];
	}

#ifndef XTERM_REVERSE_VIDEO
	if (Options & Opt_reverseVideo)
	{
		const char	*name;
		/* swap foreground/background colors */

		name = rs_color [fgColor];
		rs_color [fgColor] = rs_color [bgColor];
		rs_color [bgColor] = name;

		name = def_colorName [fgColor];
		def_colorName [fgColor] = def_colorName [bgColor];
		def_colorName [bgColor] = name;
	}
#endif

	create_windows (argc, argv);
	scr_reset ();		/* initialize screen */

	/* add scrollBar, do it directly to avoid resize() */
	scrollbar_mapping (Options & Opt_scrollBar);
	resize ();
	
	val = DisplayString (dpy);

	if (display_name == NOSTR)
		display_name = val;	/* use broken `:0' value */

	saved_nice = nice (0);

	if (nice (10 - saved_nice) < 0)
		msg ("Não consegui ajustar o \"nice\"");

	if (exec_command (cmd_argv) < 0)
		{ msg ("abortando"); return (1); }

	main_loop ();		/* main processing loop */

	return (0);

}	/* end main */

/*
 * find if fg/bg matches any of the normal (low-intensity) colors
 */
/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
set_colorfgbg (void)
{
   unsigned int i;
   int fg = -1, bg = -1;

   for (i = blackColor; i <= whiteColor; i++)
     {
	if (PixColors [fgColor] == PixColors [i])
	  {
	     fg = (i - blackColor);
	     break;
	  }
     }
   for (i = blackColor; i <= whiteColor; i++)
     {
	if (PixColors [bgColor] == PixColors [i])
	  {
	     bg = (i - blackColor);
	     break;
	  }
     }

   colorfgbg = DEFAULT_RSTYLE;
   for (i = minColor; i <= maxColor; i++)
     {
	if (PixColors [fgColor] == PixColors [i]
# ifndef NO_BOLDUNDERLINE
	    && PixColors [fgColor] == PixColors [colorBD]
# endif	/* NO_BOLDUNDERLINE */
	    /* if we wanted boldFont to have precedence */
# if 0	/* ifndef NO_BOLDFONT */
	    && TermWin.boldFont == NULL
# endif	/* NO_BOLDFONT */
	    )
	  colorfgbg = SET_FGCOLOR (colorfgbg, i);
	if (PixColors [bgColor] == PixColors [i])
	  colorfgbg = SET_BGCOLOR (colorfgbg, i);
     }
}

/*
 ****************************************************************
 *	Escolhe as cores para a barra de rolamento		*
 ****************************************************************
 */
#ifndef min
#define min(a,b) (((a)<(b)) ? (a) : (b))
#define max(a,b) (((a)>(b)) ? (a) : (b))
#endif

void
set_scrollbar_color (void)
{
	if (Xdepth <= 2)		/* Monocromático */
	{
		PixColors[scrollColor]		= PixColors[bgColor];
		PixColors[topShadowColor]	= PixColors[fgColor];
		PixColors[bottomShadowColor]	= PixColors[fgColor];
	}
	else
	{
		XColor	xcol, white;

		/* bottomShadowColor */
		xcol.pixel = PixColors [scrollColor];
		XQueryColor (dpy, Xcmap, &xcol);

		xcol.red   >>= 1;
		xcol.green >>= 1;
		xcol.blue  >>= 1;

		if (!XAllocColor (dpy, Xcmap, &xcol))
		{
			msg ("não consegui alocar \"bottomShadowColor\"");
			xcol.pixel = PixColors [minColor];
		}

		PixColors [bottomShadowColor] = xcol.pixel;

		/* topShadowColor */
		white.pixel = WhitePixel (dpy, Xscreen);
		XQueryColor (dpy, Xcmap, &white);

		xcol.pixel = PixColors [scrollColor];
		XQueryColor (dpy, Xcmap, &xcol);

		xcol.red   = max (white.red   / 5, xcol.red);
		xcol.green = max (white.green / 5, xcol.green);
		xcol.blue  = max (white.blue  / 5, xcol.blue);

		xcol.red   = min (white.red,   xcol.red   * 7 / 5);
		xcol.green = min (white.green, xcol.green * 7 / 5);
		xcol.blue  = min (white.blue,  xcol.blue  * 7 / 5);

		if (!XAllocColor (dpy, Xcmap, &xcol))
		{
			msg ("não consegui alocar \"topShadowColor\"");
			xcol.pixel = PixColors [whiteColor];
		}

		PixColors [topShadowColor] = xcol.pixel;
	}

}	/* end set_scrollbar_colors */

/*
 ****************************************************************
 *	Cria e mapeia as diversas janelas			*
 ****************************************************************
 */
void
create_windows (int argc, const char *argv [])
{
	Cursor		cursor;
	XClassHint	classHint;
	XWMHints	wmHint;
	int		i, x, y, flags, lim;
	unsigned int	width, height;
	XColor		xcol;
	XGCValues	gcvalue;

	/*
	 *	Aloca as cores necessárias.
	 */
	lim = Xdepth <= 2 ? 2 : NRS_COLORS;
	for (i = 0; i < lim; i++)
	{
		if (rs_color [i] == NOSTR)
			continue;

		if
		(	XParseColor (dpy, Xcmap, rs_color[i], &xcol) &&
			XAllocColor (dpy, Xcmap, &xcol)
		)
		{
			PixColors [i] = xcol.pixel;
			continue;
		}

		msg (color_not_available, rs_color [i]);

		if ((rs_color [i] = def_colorName [i]) == NOSTR)
			continue;

		if
		(	XParseColor (dpy, Xcmap, rs_color[i], &xcol) &&
			XAllocColor (dpy, Xcmap, &xcol)
		)
		{
			PixColors [i] = xcol.pixel;
			continue;
		}

		msg (color_not_available, rs_color [i]);

		switch (i)
		{
		    case fgColor:
		    case bgColor:
			msg ("$Impossível prosseguir");
			/* não volta */

		    case cursorColor:
			xcol.pixel = PixColors [bgColor];
			break;

		    case cursorColor2:
			xcol.pixel = PixColors [fgColor];
			break;

		    default:
			xcol.pixel = PixColors [bgColor];	/* None */
			break;
		}
	}

	if (Xdepth <= 2 || !rs_color [cursorColor])
		PixColors [cursorColor] = PixColors [bgColor];

	if (Xdepth <= 2 || !rs_color [cursorColor2])
		PixColors [cursorColor2] = PixColors [fgColor];

#ifndef NO_BOLDUNDERLINE
	if (Xdepth <= 2 || !rs_color [colorBD])
		PixColors [colorBD] = PixColors [fgColor];

	if (Xdepth <= 2 || !rs_color [colorUL])
		PixColors [colorUL] = PixColors [fgColor];
#endif	/* NO_BOLDUNDERLINE */

	set_scrollbar_color ();

	szHint.base_width  = (2 * TermWin_internalBorder +
				(Options & Opt_scrollBar ? SB_WIDTH : 0));
	szHint.base_height = (2 * TermWin_internalBorder);

	flags = rs_geometry ?
		XParseGeometry (rs_geometry, &x, &y, &width, &height) : 0;

	if (flags & WidthValue)
	{
		szHint.width = width;
		szHint.flags |= USSize;
	}

	if (flags & HeightValue)
	{
		szHint.height = height;
		szHint.flags |= USSize;
	}

	TermWin.ncol = szHint.width;
	TermWin.nrow = szHint.height;

	change_font (1, NULL);

	if (flags & XValue)
	{
		if (flags & XNegative)
		{
			x += (DisplayWidth (dpy, Xscreen)
				- (szHint.width + TermWin_internalBorder));
			szHint.win_gravity = NorthEastGravity;
		}
		szHint.x = x;
		szHint.flags |= USPosition;
	}

	if (flags & YValue)
	{
		if (flags & YNegative)
		{
			y += (DisplayHeight (dpy, Xscreen)
				- (szHint.height + TermWin_internalBorder));
			szHint.win_gravity = (szHint.win_gravity == NorthEastGravity ?
				SouthEastGravity : SouthWestGravity);
		}
		szHint.y = y;
		szHint.flags |= USPosition;
	}

	/*
	 *	parent window - reverse video so we can see placement errors
	 *	sub-window placement & size in resize_subwindows()
	 */
	TermWin.parent =	XCreateSimpleWindow
				(	dpy, Xroot,
					szHint.x, szHint.y,
					szHint.width, szHint.height,
 					BORDERWIDTH,
					PixColors [bgColor],
					PixColors [fgColor]
				);

/***	XStoreName   (dpy, TermWin.parent, rs_title);	***/
/***	XSetIconName (dpy, TermWin.parent, rs_title);	***/

	/* ignore warning about discarded `const' */
	classHint.res_name  = (char *)rs_name;
	classHint.res_class = APL_CLASS;

	wmHint.input = True;
	wmHint.initial_state = (Options & Opt_iconic ? IconicState : NormalState);
	wmHint.flags = (InputHint | StateHint);

	XSetWMProperties
	(	dpy, TermWin.parent, NULL, NULL, (char **)argv, argc,
		&szHint, &wmHint, &classHint
	);

	XSelectInput
	(	dpy, TermWin.parent,
		(KeyPressMask|FocusChangeMask|
		 StructureNotifyMask|VisibilityChangeMask)
	);

	TermWin_cursor = XCreateFontCursor (dpy, XC_xterm);
	{
		XColor	fg, bg;

		fg.pixel = PixColors [fgColor];
		XQueryColor (dpy, Xcmap, &fg);

		bg.pixel = PixColors [bgColor];
		XQueryColor (dpy, Xcmap, &bg);

		XRecolorCursor (dpy, TermWin_cursor, &fg, &bg);
	}

	cursor = XCreateFontCursor (dpy, XC_left_ptr);

	TermWin.vt =	XCreateSimpleWindow
			(	dpy, TermWin.parent,
				0, 0,
				szHint.width, szHint.height,
				0,
				PixColors [fgColor],
				PixColors [bgColor]
			);

	XDefineCursor (dpy, TermWin.vt, TermWin_cursor);
	XSelectInput
	(	dpy, TermWin.vt,
		(ExposureMask|ButtonPressMask|ButtonReleaseMask|
		 Button1MotionMask|Button3MotionMask)
	);

	XMapWindow (dpy, TermWin.vt);
	XMapWindow (dpy, TermWin.parent);

	scrollBar.win =	XCreateSimpleWindow
			(	dpy, TermWin.parent,
				0, 0,
				1, 1,	/* tamanho irrelevante, por enquanto */
				0,
				PixColors [fgColor],
				PixColors [bgColor]
			);

	XDefineCursor (dpy, scrollBar.win, cursor);

	XSelectInput
	(	dpy, scrollBar.win,
		(ExposureMask|ButtonPressMask|ButtonReleaseMask|
		 Button1MotionMask|Button2MotionMask|Button3MotionMask)
	);

	gcvalue.font	   = TermWin.font->fid;
	gcvalue.foreground = PixColors [fgColor];
	gcvalue.background = PixColors [bgColor];

	TermWin.gc =	XCreateGC
			(	dpy, TermWin.vt,
				GCForeground|GCBackground|GCFont,
				&gcvalue
			);

}	/* end create_windows */

/*
 ****************************************************************
 *	Redimensiona as janelas filhas				*
 ****************************************************************
 */
void
resize_subwindows (int width, int height)
{
	int	x = 0, y = 0;

	TermWin.width  = TermWin.ncol * TermWin.fwidth;
	TermWin.height = TermWin.nrow * TermWin.fheight;

	/* size and placement */
	if (scrollbar_visible ())
	{
		scrollBar.beg = 0;
		scrollBar.end = height;

		/* arrows are as high as wide - leave 1 pixel gap */
		scrollBar.beg += (SB_WIDTH + 1);
		scrollBar.end -= (SB_WIDTH + 1);

		width -= SB_WIDTH;
		XMoveResizeWindow
		(	dpy, scrollBar.win,
#ifdef SCROLLBAR_RIGHT
			width, 0,
#else
			x, 0,
#endif
			SB_WIDTH, height
		);

#ifndef SCROLLBAR_RIGHT
		x = SB_WIDTH;		/* placement of vt window */
#endif
	}

	XMoveResizeWindow
	(	dpy, TermWin.vt,
		x, y,
		width, height + 1
	);

	XClearWindow (dpy, TermWin.vt);
	XSync (dpy, 0);

}	/* end resize_subwindows */

/*
 ****************************************************************
 *	Redimensiona todas as janelas				*
 ****************************************************************
 */
void
resize (void)
{
	szHint.base_width  = (2 * TermWin_internalBorder);
	szHint.base_height = (2 * TermWin_internalBorder);

	szHint.base_width += (scrollbar_visible () ? SB_WIDTH : 0);

	szHint.min_width  = szHint.base_width  + szHint.width_inc;
	szHint.min_height = szHint.base_height + szHint.height_inc;

	szHint.width  = szHint.base_width  + TermWin.width;
	szHint.height = szHint.base_height + TermWin.height;

	szHint.flags |= PMinSize|PResizeInc|PBaseSize|PWinGravity;

	XSetWMNormalHints (dpy, TermWin.parent, &szHint);
	XResizeWindow (dpy, TermWin.parent, szHint.width, szHint.height);

	resize_subwindows (szHint.width, szHint.height);

}	/* end resize */

#if (0)	/*******************************************************/
/*
 ****************************************************************
 *	x
 ****************************************************************
 */
void
map_scrollBar (int map)
{
	if (scrollbar_mapping (map))
	{
		scr_touch ();
		resize ();
	}

}	/* end map_scrollBar */
#endif	/*******************************************************/

/*
 ****************************************************************
 *	Redesenha a janela					*
 ****************************************************************
 */
void
resize_window1 (unsigned int width, unsigned int height)
{
	static int	first_time = 1;
	int		new_ncol, new_nrow;

	new_ncol = (width  - szHint.base_width)  / TermWin.fwidth;
	new_nrow = (height - szHint.base_height) / TermWin.fheight;

	if
	(	first_time || (new_ncol != TermWin.ncol) ||
		(new_nrow != TermWin.nrow)
	)
	{
		int curr_screen = -1;

		/* scr_reset only works on the primary screen */
		if (!first_time)	/* this is not the first time thru */
		{
			selection_clear ();
			curr_screen = scr_change_screen (PRIMARY);
		}

		TermWin.ncol = new_ncol;
		TermWin.nrow = new_nrow;

		resize_subwindows (width, height);
		scr_reset ();

		if (curr_screen >= 0)	/* this is not the first time thru */
			scr_change_screen (curr_screen);

		first_time = 0;
	}

}	/* end resize_window1 */

/*
 ****************************************************************
 *	Redesenha a janela					*
 ****************************************************************
 */
void
resize_window2 (unsigned int nrow, unsigned int ncol)
{
	int	width, height;

	width  = szHint.base_width  + ncol * TermWin.fwidth;
	height = szHint.base_height + nrow * TermWin.fheight;

	XResizeWindow (dpy, TermWin.parent, width, height);
	resize_window1 (width, height);

}	/* end resize_window2 */

#if (0)	/*******************************************************/
/*
 ****************************************************************
 *	Muda apenas a largura					*
 ****************************************************************
 */
void
set_width (unsigned int width)
{
	unsigned int	height = TermWin.nrow;

	if (width != TermWin.ncol)
	{
		width  = szHint.base_width  + width  * TermWin.fwidth;
		height = szHint.base_height + height * TermWin.fheight;

		XResizeWindow (dpy, TermWin.parent, width, height);
		resize_window1 (width, height);
	}

}	/* end set_width */
#endif	/*******************************************************/

/*
 ****************************************************************
 *	Redesenha a janela					*
 ****************************************************************
 */
void
resize_window (void)
{
	Window		root;
	XEvent		dummy;
	int		x, y;
	unsigned int	border, depth, width, height;

	while
	(	XCheckTypedWindowEvent
		(	dpy, TermWin.parent,
			ConfigureNotify, &dummy
		)
	)
		/* vazio */;

	XGetGeometry
	(	dpy, TermWin.parent, &root,
		&x, &y, &width, &height,
		&border, &depth
	);

	/* parent already resized */
	resize_window1 (width, height);

}	/* end resize_window */

/*
 ****************************************************************
 *	Altera cores						*
 ****************************************************************
 */
void
set_window_color (int idx, char *color)
{
	XColor	xcol, fg, bg;
	int	i;

	if (color == NULL || *color == '\0')
		return;

	/* handle color aliases */
	if (isdigit (*color))
	{
		i = atoi (color);
		if (i >= 8 && i <= 15)	/* bright colors */
		{
			i -= 8;
			PixColors [idx] = PixColors [minBright + i];
			goto Done;
		}

		if (i >= 0 && i <= 7)	/* normal colors */
		{
			PixColors [idx] = PixColors [minColor + i];
			goto Done;
		}
	}

	if
	(	!XParseColor (dpy, Xcmap, color, &xcol) ||
		!XAllocColor (dpy, Xcmap, &xcol)
	)
	{
		msg (color_not_available, color);
		return;
	}

   /* XStoreColor (dpy, Xcmap, XColor*); */

   /*
    * FIXME: should free colors here, but no idea how to do it so instead,
    * so just keep gobbling up the colormap
    */
# if 0
   for (i = blackColor; i <= whiteColor; i++)
     if (PixColors [idx] == PixColors [i])
       break;
   if (i > whiteColor)
     {
	/* fprintf (stderr, "XFreeColors: PixColors [%d] = %lu\n", idx, PixColors [idx]); */
	XFreeColors (dpy, Xcmap, (PixColors + idx), 1,
		     DisplayPlanes(dpy, Xscreen));
     }
# endif

	PixColors [idx] = xcol.pixel;

   /* XSetWindowAttributes attr; */
   /* Cursor cursor; */
Done:
	if (idx == bgColor)
		XSetWindowBackground (dpy, TermWin.vt, PixColors [bgColor]);

   /* handle colorBD, scrollbar background, etc. */

	set_colorfgbg ();

	fg.pixel = PixColors [fgColor];
	XQueryColor (dpy, Xcmap, &fg);

	bg.pixel = PixColors [bgColor];
	XQueryColor (dpy, Xcmap, &bg);

	XRecolorCursor (dpy, TermWin_cursor, &fg, &bg);

	/* the only reasonable way to enforce a clean update */
	scr_poweron ();

}	/* end set_window_color */

/*
 ****************************************************************
 *	Processa seqüências de ESC próprias do Xterm		*
 ****************************************************************
 *
 * XTerm escape sequences: ESC ] Ps;Pt BEL
 *	 0 = change iconName/title
 *	 1 = change iconName
 *	 2 = change title
 *	46 = change logfile (not implemented)
 *	50 = change font
 *
 * xterm extensions:
 *	10 = menu
 *	20 = bg pixmap
 *	39 = change default fg color
 *	49 = change default bg color
 */
void
xterm_seq (int op, char * str)
{
	unsigned int	ncol, nrow;

	assert (str != NULL);

	switch (op)
	{
	    case XTerm_name:
		XStoreName (dpy, TermWin.parent, str);
		/* drop */

	    case XTerm_iconName:
		XSetIconName (dpy, TermWin.parent, str);
		break;

	    case XTerm_title:
		XStoreName (dpy, TermWin.parent, str);
		break;

	    case XTerm_Menu:
		/* menubar_dispatch (str); */
		break;

	    case XTerm_logfile:
		sscanf (str, "%dx%d", &ncol, &nrow);
		resize_window2 (nrow, ncol);
		break;

	    case XTerm_Pixmap:
		break;

	    case XTerm_restoreFG:
		set_window_color (fgColor, str);
		set_window_color (cursorColor, str);
		break;

	    case XTerm_restoreBG:
		set_window_color (bgColor, str);
		set_window_color (cursorColor2, str);
		break;

	    case XTerm_font:
		change_font (0, str);
		break;
	}

}	/* end xterm_seq */

/*
 ****************************************************************
 *	Muda dinamicamente a fonte				*
 ****************************************************************
 */
void
change_font (int init, const char * fontname)
{
	XFontStruct		*xfont;
	static char		*newfont[NFONTS];
#ifndef NO_BOLDFONT
	static XFontStruct	*boldFont = NULL;
#endif
	static int		fnum = FONT0_IDX;	/* logical font number */
	int			idx = 0;		/* index into rs_font[] */

#if (FONT0_IDX == 0)
# define IDX2FNUM(i) (i)
# define FNUM2IDX(f) (f)
#else
# define IDX2FNUM(i) (i == 0? FONT0_IDX : (i <= FONT0_IDX? (i-1) : i))
# define FNUM2IDX(f) (f == FONT0_IDX ? 0 : (f < FONT0_IDX ? (f+1) : f))
#endif
#define FNUM_RANGE(i)	(i <= 0 ? 0 : (i >= NFONTS ? (NFONTS-1) : i))

	if (!init)
	{
		switch (fontname [0])
		{
		    case '\0':
			fnum = FONT0_IDX;
			fontname = NULL;
			break;

		    /* special (internal) prefix for font commands */
		    case FONT_CMD:
			idx = atoi (fontname+1);
			switch (fontname [1])
			{
			    case '+':		/* corresponds to FONT_UP */
				fnum += (idx ? idx : 1);
				fnum = FNUM_RANGE (fnum);
				break;

			    case '-':		/* corresponds to FONT_DN */
				fnum += (idx ? idx : -1);
				fnum = FNUM_RANGE (fnum);
				break;

			    default:
				if (fontname [1] != '\0' && !isdigit (fontname [1]))
					return;
				if (idx < 0 || idx >= (NFONTS))
					return;
				fnum = IDX2FNUM (idx);
				break;
			}
			fontname = NULL;
			break;

		    default:
			if (fontname != NULL)
			{
				/* search for existing fontname */
				for (idx = 0; idx < NFONTS; idx++)
				{
					if (!strcmp (rs_font [idx], fontname))
					{
						fnum = IDX2FNUM (idx);
						fontname = NULL;
						break;
					}
				}
			}
			else
				return;
			break;
		}

		/* re-position around the normal font */
		idx = FNUM2IDX (fnum);

		if (fontname != NULL)
		{
			char * name;
			xfont = XLoadQueryFont (dpy, fontname);
			if (!xfont)
				return;

			if ((name = strdup (fontname)) == NOSTR)
			{
				XFreeFont (dpy, xfont);
				return;
			}

			if (newfont [idx] != NULL)
				xtfree (newfont [idx]);
			newfont [idx] = name;
			rs_font [idx] = newfont [idx];
		}
	}

	if (TermWin.font)
		XFreeFont (dpy, TermWin.font);

	/* load font or substitute */
	xfont = XLoadQueryFont (dpy, rs_font [idx]);
	if (!xfont)
	{
		msg (font_not_available, rs_font [idx]);
		rs_font [idx] = "fixed";
		xfont = XLoadQueryFont (dpy, rs_font [idx]);
		if (!xfont)
			msg ("$Nem a fonte \"fixed\" está disponível !!!");
	}

	TermWin.font = xfont;

#ifndef NO_BOLDFONT
	/* fail silently */
	if (init && rs_boldFont != NULL)
		boldFont = XLoadQueryFont (dpy, rs_boldFont);
#endif

	/* alter existing GC */
	if (!init)
		XSetFont (dpy, TermWin.gc, TermWin.font->fid);

	/* set the sizes */
	{
		int fw = XTextWidth (TermWin.font, "MMMMMMMMMM", 10) / 10;
		int fh = TermWin.font->ascent + TermWin.font->descent;

		/* not the first time thru and sizes haven't changed */
		if (fw == TermWin.fwidth && fh == TermWin.fheight)
			return;

		TermWin.fwidth  = fw;
		TermWin.fheight = fh;
	}

	/* check that size of boldFont is okay */
#ifndef NO_BOLDFONT
	if
	(	boldFont != NULL &&
		TermWin.fwidth == (XTextWidth (boldFont, "MMMMMMMMMM", 10) / 10) &&
		TermWin.fheight == (boldFont->ascent + boldFont->descent)
	)
		TermWin.boldFont = boldFont;
	else
		TermWin.boldFont = NULL;
#endif	/* NO_BOLDFONT */

	set_colorfgbg ();

	TermWin.width  = TermWin.ncol * TermWin.fwidth;
	TermWin.height = TermWin.nrow * TermWin.fheight;

	szHint.width_inc  = TermWin.fwidth;
	szHint.height_inc = TermWin.fheight;

	szHint.min_width  = szHint.base_width  + szHint.width_inc;
	szHint.min_height = szHint.base_height + szHint.height_inc;

	szHint.width  = szHint.base_width  + TermWin.width;
	szHint.height = szHint.base_height + TermWin.height;

	szHint.flags |= PMinSize|PResizeInc|PBaseSize|PWinGravity;

	if (!init)
		resize ();

#undef IDX2FNUM
#undef FNUM2IDX
#undef FNUM_RANGE
}	/* end change_font */
