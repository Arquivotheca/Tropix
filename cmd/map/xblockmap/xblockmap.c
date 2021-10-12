/*
 ****************************************************************
 *								*
 *			xblockmap.c				*
 *								*
 *	Mostra graficamente detalhes do sistema de arquivo	*
 *								*
 *	Versão	4.3.0, de 10.08.02				*
 *		4.3.0, de 10.08.02				*
 *								*
 *	Módulo: xblockmap					*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/common.h>
#include <sys/t1.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Intrinsic.h>
#include <X11/keysym.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão: 4.3.0, de x.02";

#define	default_font_str "9x15"
#if (0)	/*******************************************************/
		font_str = "6x13";
		font_str = "-adobe-courier-medium-r-normal--14-140-75-75-m-90-iso8859-1";
		font_str = "5x8";
#endif	/*******************************************************/

#define	NOVOID	(void *)NULL
#define	elif	else if
#define	EVER	;;
#define STR(x)	# x
#define XSTR(x)	STR (x)

#define MAP_SZ(sz)      ((((sz) >> 1) + BL4MASK) & ~BL4MASK)

typedef struct t1sb	T1SB;

entry int	exit_code = 0;	/* Código de retorno */

entry int	vflag;		/* Verbose */

entry char	dev_nm[128];

entry char	dev_nm_len;

entry char	reading_dev_nm;

/*
 ****** Cores Usadas ********************************************
 */
#if (0)	/*******************************************************/
#define	DEV_COLOR_NM			"lightgoldenrod"
#endif	/*******************************************************/
#define	DEV_COLOR_NM			"indianred"
#define	TEXT_COLOR_NM			"green1"
#define	MENU_TEXT_COLOR_NM		"black"
#define	SCROLL_COLOR_NM			"gray60"

#define	MAP_FREE_COLOR_NM		"black"
#define	MAP_MAP_COLOR_NM		"white"
#define	MAP_DATA_COLOR_NM		"blue1"
#define	MAP_INDIR_COLOR_NM		"green1"
#define	MAP_INODE_EMPTY_COLOR_NM	"red3"
#define	MAP_INODE_FULL_COLOR_NM		"red1"

/*
 ******	Definições do video *************************************
 */
#define	NOGC			NULL

#define InternalBorder		2

#define	SB_WIDTH		15
#define	SHADOW_WIDTH		3

#define	MAIN_WIDTH		1000
#define	MAIN_HEIGHT		600

#define	BLOCK_X			100
#define	BLOCK_Y			300

#define	BLOCK_WIDTH		800

#define	BLOCK_HEIGHT		100
#define	MB_HEIGHT		FONT_HEIGHT
#define	SB_HEIGHT		SB_WIDTH

char				*disk_block_map;

daddr_t				NBLOCK, FIRST_BLOCK;

/*
 ******	Variáveis globais ***************************************
 */
entry Display		*display;

entry int		text_fid;

entry int		FONT_WIDTH, FONT_HEIGHT;	/* Tamanho da fonte */
entry int		FONT_DESCENT;

entry GC		text_gc, menu_text_gc;

typedef	struct
{
	Pixel		main_pixel, top_pixel, bottom_pixel;
	GC		main_gc, top_gc, bottom_gc;

}	TRIPLEGC;

/*
 ******	Geometria ***********************************************
 */
XSizeHints	szhint =
{
	PMinSize|PResizeInc|PBaseSize|PWinGravity,

	0,	0,		/*		x, y */
	0,	0,		/*		width, height */
	1,	1,		/* PMinSize	min_width, min_height */
	0,	0,		/*		max_width, max_height */
	1,	1,		/* PResizeInc	width_inc, height_inc */
	{1,	1},		/*		min_aspect */
	{0,	0},		/*		max_aspect ratio */
	0,	0,		/* PBaseSize	base_width, base_height */
	NorthWestGravity	/* PWinGravity	gravity */
};

entry Window		main_win;

/*
 ******	Dados da Janela do Dispositivo **************************
 */
typedef struct
{
	Window		win;
	int		width, height;
	TRIPLEGC	gc;

}	BOX;

BOX	dev, quit;

#define	BOX_BORDER	10

/*
 ******	Dados da Janela de botões *******************************
 */
entry Window		block_win;

entry GC		white_gc;

entry GC		free_gc, map_gc, data_gc, indir_gc, inode_empty_gc, inode_full_gc;

/*
 ******	Estrutura para a Barra de Rolagem ***********************
 */
typedef struct
{
	Window		win;
	TRIPLEGC	gc;		/* As tres cores */
	int		state;		/* Estado da Barra de Rolagem */
	int		begin, end;	/* Começo e Final da janela */
	int		top, bottom;	/* Limites da barra (top: valor menor) */

}	SCROLLBAR;

entry SCROLLBAR		scrollbar;

/*
 ****** Protótipos de funções ***********************************
 */
Pixel		get_window_color (Display *Xdisplay, const char *color_nm, Pixel def);
void		geometry_to_size_hints (const char *, XSizeHints *);
void		read_bitmap (void);
void		display_dev_box (void);
void		display_quit_box (void);
void		read_dev_char (XEvent *ev);
void		begin_read_dev (XEvent *ev);
void		display_block_colors (void);
void		Draw_Shadow (Window win, GC topShadow, GC botShadow, int x, int y, int w, int h);
void		Draw_button (int x, int y, int state);
void		set_shadow_colors (Display *Xdisplay, Window, TRIPLEGC *tp, const char *color_nm, Pixel def);
void		update_scrollbar (int y);
void		help (void);

/*
 ****************************************************************
 *	Mostra graficamente detalhes do sistema de arquivo	*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	int		save_argc = argc;
	const char	**save_argv = argv;
	int		opt, fd;
	const char	*geo_str = NOSTR;
	const char	*display_str = NOSTR;
	const char	*fgc = NOSTR, *bgc = NOSTR;
	const char	*str, *font_str, *user_font_str = NOSTR;
	unsigned int	depth;
	int		screen;
	Window		root, win;
	XFontStruct	*text_fip;
	XClassHint	classhint;
	XWMHints	wmhint;
	XGCValues	gcv;
	XEvent		ev;
	char		text[64];
	int		len;

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "vg:f:b:d:F:H")) != EOF)
	{
		switch (opt)
		{
		    case 'v':			/* Verbose */
			vflag++;
			break;

		    case 'g':			/* Geometria */
			geo_str = optarg;
			break;

		    case 'f':			/* Cor de frente */
			fgc = optarg;
			break;

		    case 'b':			/* Cor de fundo */
			bgc = optarg;
			break;

		    case 'd':
			display_str = optarg;	/* Display */
			break;

		    case 'F':
			user_font_str = optarg;	/* Fonte */
			break;

		    case 'H':			/* Help */
			help ();

		    default:			/* Erro */
			putc ('\n', stderr);
			help ();

		}	/* end switch */

	}	/* end while */

	argv += optind;
	argc -= optind;

	/*
	 *	Verifica se o X-Window está ativo
	 */
	if (display_str == NOSTR && open ("/dev/xconsole", O_RDWR) >= 0)
		error ("$O Sistema X-Window não está ativo");

	/*
	 *	Lê o Mapa de Bits
	 */
	if (argv[0])
	{
		strcpy (dev_nm, argv[0]);

		read_bitmap ();
	}

	/*
	 *	Conecta com o servidor
	 */
	if ((display = XOpenDisplay (display_str)) == NULL)
		error ("$*Não consegui conectar com o servidor \"%s\"", XDisplayName (display_str));

	screen	= DefaultScreen (display);
	depth 	= DefaultDepth (display, screen);
	root	= RootWindow (display, screen);
	fd	= ConnectionNumber (display);

	/*
	 *	Instala a fonte pedida, obtendo o tamanho
	 *	de um caractere. Supõe passo fixo.
	 */
	if   (user_font_str != NOSTR)
		font_str = user_font_str;
	elif ((str = XGetDefault (display, "XBlockmap", "font")) != NOSTR)
		font_str = str;
	else
		font_str = default_font_str;

	if ((text_fip = XLoadQueryFont (display, font_str)) == NULL)
		error ("$Não encontrei a fonte \"%s\"", font_str);

	text_fid = text_fip->fid;

	FONT_WIDTH	= XTextWidth (text_fip, "MMMMMMMMMM", 10) / 10;
	FONT_DESCENT	= text_fip->descent;
	FONT_HEIGHT	= text_fip->ascent + text_fip->descent;

	szhint.base_width  = (2 * InternalBorder);
	szhint.base_height = (2 * InternalBorder);

	szhint.width_inc  = FONT_WIDTH;
	szhint.height_inc = FONT_HEIGHT;

	szhint.width  = szhint.base_width  + MAIN_WIDTH;
	szhint.height = szhint.base_height + MAIN_HEIGHT;

	szhint.flags |= PMinSize|PResizeInc|PBaseSize|PWinGravity;

	/*
	 *	Obtém as cores
	 */
	geometry_to_size_hints (geo_str, &szhint);

        win =	XCreateSimpleWindow
                (	display, root,
			szhint.x, szhint.y, szhint.width, szhint.height, 5,
			0x00CC00,
			BlackPixel (display, screen)
		);

	main_win = win;

	XStoreName (display, win, "XBlockMap");

	XSelectInput (display, win, ExposureMask|KeyPressMask|StructureNotifyMask);

	classhint.res_name  = (char *)pgname;
	classhint.res_class = "XBlockMap";

	wmhint.input		= True;
	wmhint.initial_state	= NormalState;
	wmhint.flags		= InputHint|StateHint;

	XSetWMProperties
	(	display, win, NULL, NULL, (char **)save_argv, save_argc,
		&szhint, &wmhint, &classhint
	);

	XMapWindow (display, win);

	/*
	 ******	Obtém tôdas as cores ************************************
	 */
	gcv.font	= text_fid;
	gcv.line_width  = 1;

	gcv.foreground  = get_window_color (display, TEXT_COLOR_NM, 0xFFFFFF);
	gcv.background  = BlackPixel (display, screen);
	text_gc = XCreateGC (display, win, GCForeground|GCBackground|GCFont, &gcv);

	gcv.foreground  = get_window_color (display, MENU_TEXT_COLOR_NM, 0xFFFFFF);
	gcv.background  = BlackPixel (display, screen);
	menu_text_gc = XCreateGC (display, win, GCForeground|GCBackground|GCFont, &gcv);

	gcv.foreground = get_window_color (display, "white", 0xFFFFFF);
	white_gc = XCreateGC (display, win, GCForeground|GCLineWidth, &gcv);

	gcv.foreground = get_window_color (display, MAP_FREE_COLOR_NM, 0x000000);
	free_gc = XCreateGC (display, win, GCForeground|GCLineWidth, &gcv);

	gcv.foreground = get_window_color (display, MAP_MAP_COLOR_NM, 0xFFFFFF);
	map_gc = XCreateGC (display, win, GCForeground|GCLineWidth, &gcv);

	gcv.foreground = get_window_color (display, MAP_DATA_COLOR_NM, 0x0000CC);
	data_gc = XCreateGC (display, win, GCForeground|GCLineWidth, &gcv);

	gcv.foreground = get_window_color (display, MAP_INDIR_COLOR_NM, 0x00CC00);
	indir_gc = XCreateGC (display, win, GCForeground|GCLineWidth, &gcv);

	gcv.foreground = get_window_color (display, MAP_INODE_EMPTY_COLOR_NM, 0xCC0000);
	inode_empty_gc = XCreateGC (display, win, GCForeground|GCLineWidth, &gcv);

	gcv.foreground = get_window_color (display, MAP_INODE_FULL_COLOR_NM, 0xFF0000);
	inode_full_gc = XCreateGC (display, win, GCForeground|GCLineWidth, &gcv);

	/*
	 ******	Prepara a janela do DISPOSITIVO *************************
	 */
	dev.width  = XTextWidth (text_fip, "Dispositivo", 11) + 2 * BOX_BORDER;
	dev.height = FONT_HEIGHT + 2 * BOX_BORDER,

	set_shadow_colors (display, main_win, &dev.gc, DEV_COLOR_NM, 0xCCCCCC);

        dev.win = XCreateSimpleWindow
                (	display, win,
			InternalBorder, InternalBorder,
			dev.width, dev.height,
			0,
			BlackPixel (display, screen),
			dev.gc.main_pixel
		);

	XSelectInput
	(	display, dev.win,
		(ExposureMask|ButtonPressMask|ButtonReleaseMask|
		EnterWindowMask|LeaveWindowMask)
	);

	XMapWindow (display, dev.win);

	/*
	 ******	Prepara a janela de TÉRMINO *****************************
	 */
	quit.width  = XTextWidth (text_fip, "Fim", 3) + 2 * BOX_BORDER;
	quit.height = FONT_HEIGHT + 2 * BOX_BORDER,

	memmove (&quit.gc, &dev.gc, sizeof (quit.gc));

        quit.win = XCreateSimpleWindow
                (	display, win,
			InternalBorder + dev.width + InternalBorder, InternalBorder,
			quit.width, quit.height,
			0,
			BlackPixel (display, screen),
			dev.gc.main_pixel
		);

	XSelectInput
	(	display, quit.win,
		(ExposureMask|ButtonPressMask|ButtonReleaseMask|
		EnterWindowMask|LeaveWindowMask)
	);

	XMapWindow (display, quit.win);

	/*
	 ******	Prepara a janela do MAPA ********************************
	 */
        block_win = XCreateSimpleWindow
                (	display, win,
			0, 0,
			1, 1,
			1,
			WhitePixel (display, screen),
			BlackPixel (display, screen)
		);

	XSelectInput
	(	display, block_win,
		(ExposureMask|ButtonPressMask|ButtonReleaseMask|
		PointerMotionMask|EnterWindowMask|LeaveWindowMask)
	);

	XMoveResizeWindow
	(	display, block_win,
		BLOCK_X, BLOCK_Y,
		BLOCK_WIDTH + 2 * InternalBorder, BLOCK_HEIGHT + MB_HEIGHT + SB_HEIGHT + 2 * InternalBorder
	);

	XMapWindow (display, block_win);

	/*
	 ******	Prepara a barra de rolagem ******************************
	 */
        scrollbar.win = XCreateSimpleWindow
                (	display, block_win,
			0, 0,
			1, 1,
			0,
			WhitePixel (display, screen),
			BlackPixel (display, screen)
		);

	XSelectInput
	(	display, scrollbar.win,
		(ExposureMask|ButtonPressMask|ButtonReleaseMask|
		Button1MotionMask|Button2MotionMask|Button3MotionMask)
	);

	XMoveResizeWindow
	(	display, scrollbar.win,
		InternalBorder, InternalBorder + BLOCK_HEIGHT + MB_HEIGHT,
		BLOCK_WIDTH, SB_WIDTH
	);

	scrollbar.state	 = 0;
	scrollbar.begin	 = InternalBorder + SB_WIDTH + 1;
	scrollbar.end	 = BLOCK_WIDTH - InternalBorder - (SB_WIDTH + 1);

	set_shadow_colors (display, scrollbar.win, &scrollbar.gc, SCROLL_COLOR_NM, 0xCCCCCC);

	XMapWindow (display, scrollbar.win);

	/*
	 ******	Entra na malha de eventos *******************************
	 */
	for (EVER)
	{
		while (XPending (display))
		{
			XNextEvent (display, &ev);

			switch (ev.type)
			{
			    case Expose:
			    case GraphicsExpose:
				if   (ev.xany.window == win)
				{
					len = sprintf
					(	text,
						"Mapa de blocos de \"%s\" (%4.3f MB)",
						argv[0], (double)NBLOCK / 256
					);

					XDrawImageString
					(	display, win,
						text_gc,
						BLOCK_X, 100,
						text, len
					);
				}
				elif (ev.xany.window == dev.win)
				{
					display_dev_box ();
				}
				elif (ev.xany.window == quit.win)
				{
					display_quit_box ();
				}
				elif (ev.xany.window == block_win)
				{
					display_block_colors ();
				}
				elif (ev.xany.window == scrollbar.win)
				{
					update_scrollbar (-1);
				}

				break;
	
			    case EnterNotify:
				if   (ev.xany.window == dev.win)
				{
					Draw_Shadow
					(	dev.win,
						dev.gc.top_gc, dev.gc.bottom_gc,
						0, 0,
						dev.width, dev.height
					);
				}
				elif (ev.xany.window == quit.win)
				{
					Draw_Shadow
					(	quit.win,
						quit.gc.top_gc, quit.gc.bottom_gc,
						0, 0,
						quit.width, quit.height
					);
				}

				break;

			    case LeaveNotify:
				if   (ev.xany.window == dev.win)
				{
					Draw_Shadow
					(	dev.win,
						dev.gc.main_gc, dev.gc.main_gc,
						0, 0,
						dev.width, dev.height
					);
				}
				elif (ev.xany.window == quit.win)
				{
					Draw_Shadow
					(	quit.win,
						quit.gc.main_gc, quit.gc.main_gc,
						0, 0,
						quit.width, quit.height
					);
				}

				break;

			    case MotionNotify:
			    {
				Window		unused_root, unused_child;
				int		unused_root_x, unused_root_y;
				unsigned int	unused_mask;

				if (ev.xany.window == scrollbar.win)
				{
					while
					(	XCheckTypedWindowEvent
						(	display, scrollbar.win,
							MotionNotify, &ev
						)
					)
						/* vazio */;

					XQueryPointer
					(	display, scrollbar.win,
						&unused_root, &unused_child,
						&unused_root_x, &unused_root_y,
						&(ev.xbutton.x), &(ev.xbutton.y),
						&unused_mask
					);


					update_scrollbar (ev.xbutton.x);
				}

				break;
			    }

			    case ButtonPress:
				if   (ev.xany.window == scrollbar.win)
				{
					update_scrollbar (ev.xbutton.x);
				}
				elif (ev.xany.window == dev.win)
				{
					Draw_Shadow
					(	dev.win,
						dev.gc.bottom_gc, dev.gc.top_gc,
						0, 0,
						dev.width, dev.height
					);

					begin_read_dev (&ev);
				}
				elif (ev.xany.window == quit.win)
				{
					Draw_Shadow
					(	quit.win,
						quit.gc.bottom_gc, quit.gc.top_gc,
						0, 0,
						quit.width, quit.height
					);

					XCloseDisplay (display);
					exit (exit_code);
				}

				break;

			    case ButtonRelease:
				if   (ev.xany.window == scrollbar.win)
				{
					if   (scrollbar.state == 'U')
						Draw_button (0, 0, +1);
					elif (scrollbar.state == 'D')
						Draw_button (scrollbar.end + 1, 0, +1);

					scrollbar.state = 0;
				}
				elif (ev.xany.window == dev.win)
				{
					Draw_Shadow
					(	dev.win,
						dev.gc.top_gc, dev.gc.bottom_gc,
						0, 0,
						dev.width, dev.height
					);
				}
				elif (ev.xany.window == quit.win)
				{
					Draw_Shadow
					(	quit.win,
						quit.gc.top_gc, quit.gc.bottom_gc,
						0, 0,
						quit.width, quit.height
					);
				}

				break;

			    case KeyPress:
			    {
				KeySym		key;
	
				if (reading_dev_nm)
				{
					read_dev_char (&ev);
				}
				else
				{
					len = XLookupString (&ev.xkey, text, 10, &key, 0);

					if (key == XK_Home)
					{
						FIRST_BLOCK = 0;
						update_scrollbar (-1);
					}
					elif (key == XK_Page_Up)
					{
						update_scrollbar (scrollbar.begin + 1);
						Draw_button (0, 0, +1);
					}
					elif (key == XK_Page_Down)
					{
						update_scrollbar (scrollbar.end - 1);
						Draw_button (scrollbar.end + 1, 0, +1);
					}
					elif (key == XK_Left)
					{
						update_scrollbar (scrollbar.begin - 1);
						Draw_button (0, 0, +1);
					}
					elif (key == XK_Right)
					{
						update_scrollbar (scrollbar.end + 1);
						Draw_button (scrollbar.end + 1, 0, +1);
					}
					elif (text[0] == 'q')
					{
						XCloseDisplay (display);
						exit (exit_code);
					}

					break;
				}
			    }
			}
		}

		attention (1, &fd, -1, 1);

	}	/* end for (EVER) */

	XCloseDisplay (display);

	return (exit_code);

}	/* end xblockmap */

/*
 ****************************************************************
 *	Analisa uma cor dada					*
 ****************************************************************
 */
Pixel
get_window_color (Display *Xdisplay, const char *color_nm, Pixel def)
{
	XColor			xcol;
	Colormap		Xcmap;

	if (color_nm == NOSTR || color_nm[0] == '\0')
		return (def);

	Xcmap = DefaultColormap (Xdisplay, DefaultScreen (Xdisplay));

	if (!XParseColor (Xdisplay, Xcmap, color_nm, &xcol))
		{ error ("Não consegui alocar a cor \"%s\"", color_nm); return (def); }

	if (!XAllocColor (Xdisplay, Xcmap, &xcol))
		{ error ("Não consegui alocar a cor \"%s\"", color_nm); return (def); }

	return (xcol.pixel);

}	/* end get_window_color */

/*
 ****************************************************************
 *	Analisa a geometria					*
 ****************************************************************
 */
void
geometry_to_size_hints (const char *geo_str, XSizeHints *hp)
{
	int		x, y, user_geo_mask, screen;
	unsigned int	width, height;

	if (geo_str == NOSTR)
		return;

	screen = DefaultScreen (display);

	user_geo_mask = XParseGeometry (geo_str, &x, &y, &width, &height);

	if (user_geo_mask & WidthValue)
		{ hp->width = width + hp->base_width; hp->flags |= USSize; }

	if (user_geo_mask & HeightValue)
		{ hp->height = height + hp->base_height; hp->flags |= USSize; }

	if (user_geo_mask & XValue)
	{
		if (user_geo_mask & XNegative)
		{
			x += DisplayWidth (display, screen) - hp->width;

			hp->win_gravity = NorthEastGravity;
		}

		hp->x = x; hp->flags |= USPosition;
	}

	if (user_geo_mask & YValue)
	{
		if (user_geo_mask & YNegative)
		{
			y += DisplayHeight (display, screen) - hp->height;

			if (hp->win_gravity == NorthEastGravity)
				hp->win_gravity = SouthEastGravity;
			else
				hp->win_gravity = SouthWestGravity;
		}

		hp->y = y; hp->flags |= USPosition;
	}

}	/* end geometry_to_size_hints */

/*
 ****************************************************************
 *	Desenha a caixa do dispositivo				*
 ****************************************************************
 */
void
display_dev_box (void)
{
	XDrawString
	(	display, dev.win,
		menu_text_gc,
		BOX_BORDER,
		BOX_BORDER + FONT_HEIGHT - FONT_DESCENT,
		"Dispositivo", 11
	);

}	/* end display_dev_box */

/*
 ****************************************************************
 *	Desenha a caixa de Término				*
 ****************************************************************
 */
void
display_quit_box (void)
{
	XDrawString
	(	display, quit.win,
		menu_text_gc,
		BOX_BORDER,
		BOX_BORDER + FONT_HEIGHT - FONT_DESCENT,
		"Fim", 3
	);

}	/* end display_dev_box */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
begin_read_dev (XEvent *ev)
{
	strcpy (dev_nm, "/dev/"); dev_nm_len = 5;

	XDrawImageString
	(	display, main_win,
		text_gc,
		BLOCK_X, 150,
		"DEV = /dev/                                 ", 42
	);

	reading_dev_nm++;

}	/* end read_dev */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
read_dev_char (XEvent *ev)
{
	char		text[80];
	int		len;
	KeySym		key;

	len = XLookupString (&ev->xkey, text, 10, &key, 0);

	if (key == XK_Return || key == XK_KP_Enter || key == XK_Linefeed)
	{
		dev_nm[dev_nm_len] = '\0';

		reading_dev_nm = 0;

		read_bitmap ();

		len = sprintf
		(	text,
			"Mapa de blocos de \"%s\" (%4.3f MB)",
			dev_nm, (double)NBLOCK / 256
		);

		XDrawImageString
		(	display, main_win,
			text_gc,
			BLOCK_X, 100,
			text, len
		);

		FIRST_BLOCK = 0;

		update_scrollbar (-1);

		return;
	}

	dev_nm[dev_nm_len] = text[0];

	XDrawImageString
	(	display, main_win,
		text_gc,
		BLOCK_X + (6 + dev_nm_len) * FONT_WIDTH, 150,
		&text[0], 1
	);

	dev_nm_len++;

}	/* end read_dev */

/*
 ****************************************************************
 *	Lê o mapa de bits					*
 ****************************************************************
 */
void
read_bitmap (void)
{
	int		dev_fd;
	T1SB		sb;
	char		*bp;
	daddr_t		bno;

	/*
	 *	Abre o arquivo
	 */
	if ((dev_fd = open (dev_nm, O_RDONLY)) < 0)
		error ("$*Não consegui abrir \"%s\"", dev_nm);

	if (lseek (dev_fd, T1_SBNO << BL4SHIFT, L_SET) < 0)
		error ("$*Erro de posicionamento para o bloco %d", T1_SBNO);

	if (read (dev_fd, &sb, sizeof (T1SB)) != sizeof (T1SB))
		error ("$*Não consegui ler o SUPERBLOCO de \"%s\"", dev_nm);

	if (sb.s_magic != T1_SBMAGIC)
		error ("$O dispositivo \"%s\" NÃO contém um sistema de arquivos T1", dev_nm);

	NBLOCK = sb.s_fs_sz;

	/*
	 *	Lê os mapas de bits do disco
	 */
	if ((bp = disk_block_map = malloc (MAP_SZ (sb.s_fs_sz))) == NOVOID)
		error (NOSTR);

	for (bno = T1_BMNO; bno < sb.s_fs_sz; bno += T1_ZONE_SZ, bp += BL4SZ)
	{
		if (llseek (dev_fd, (loff_t)bno << BL4SHIFT, NULL, L_SET) < 0)
			error ("$*Erro de posicionamento para o bloco %d", bno);

		if (read (dev_fd, bp, BL4SZ) != BL4SZ)
			error ("$*Erro de leitura do bloco %d", bno);

		if (T1_GET_MAP (bp, T1_BMNO) != T1_MAP_MAP)
		{
			printf ("Código inválido do próprio mapa no bloco %d\n", bno);

			T1_PUT_MAP (bp, T1_BMNO, T1_MAP_MAP);
		}
	}

	close (dev_fd);

}	/* end read_bitmap */

/*
 ****************************************************************
 *	Atualiza as cores dos códigos do blocos			*
 ****************************************************************
 */
void
display_block_colors (void)
{
	daddr_t		bno, bno_begin, bno_end;
	char		*cp = disk_block_map;
	GC		gc;
	int		len, x;
	char		text[32];

	/*
	 *	x
	 */
	bno_begin = FIRST_BLOCK;

	if ((bno_end = bno_begin + BLOCK_WIDTH - 4) >= NBLOCK)
		bno_end = NBLOCK - 1;

#if (0)	/*******************************************************/
	XDrawRectangle
	(	display, block_win, white_gc,
		0, 0,
		BLOCK_WIDTH + 2 * InternalBorder - 1,
		BLOCK_HEIGHT + MB_HEIGHT + SB_HEIGHT + 2 * InternalBorder - 1
	);
#endif	/*******************************************************/

	len = sprintf (text, "%4.3f MB  ", (double)bno_begin / 256);

	XDrawImageString
	(	display, block_win,
		text_gc,
		InternalBorder + 4,
		BLOCK_HEIGHT + FONT_HEIGHT - FONT_DESCENT,
		text, len
	);

	len = sprintf (text, "  %4.3f MB", ((double)bno_end + 1) / 256);

	XDrawImageString
	(	display, block_win,
		text_gc,
		BLOCK_WIDTH - len * FONT_WIDTH - InternalBorder - 4,
		BLOCK_HEIGHT + FONT_HEIGHT - FONT_DESCENT,
		text, len
	);

	for (x = 2, bno = bno_begin; bno < bno_end; bno++, x++)
	{
		switch (T1_GET_MAP (cp, bno))
		{
		    case T1_MAP_MAP:
			gc = map_gc;
			break;

		    case T1_MAP_INODE_EMPTY:
			gc = inode_empty_gc;
			break;

		    case T1_MAP_INODE_FULL:
			gc = inode_full_gc;
			break;

		    case T1_MAP_DATA:
			gc = data_gc;
			break;

		    case T1_MAP_INDIR:
			gc = indir_gc;
			break;

		    default:
			gc = free_gc;
			break;
		}

		XDrawLine
		(	display, block_win, gc,
			x, InternalBorder + 3,
			x, BLOCK_HEIGHT - 3
		);
	}

}	/* end display_block_colors */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
update_scrollbar (int y)
{
	int		last_top    = scrollbar.top;
	int		last_bottom = scrollbar.bottom;
	int		middle = (last_top + last_bottom) >> 1;
	int		inc = 0;
	int		state = 0;
	int		size, height;

	if (NBLOCK <= BLOCK_WIDTH)
		return;

	if   (y < 0)
	{
		/* continua */
	}
	elif (y < scrollbar.begin)	/* Sobe 1/16 da tela */
	{
		if (FIRST_BLOCK <= 0)
			return;

		if ((FIRST_BLOCK -= (BLOCK_WIDTH >> 4)) < 0)
			FIRST_BLOCK = 0;

		state = 'U';
	}
	elif (y < middle - 20)		/* Sobe meia tela */
	{
		if (FIRST_BLOCK <= 0)
			return;

		if ((FIRST_BLOCK -= (BLOCK_WIDTH >> 1)) < 0)
			FIRST_BLOCK = 0;
	}
	elif (y > scrollbar.end)	/* Desce 1/16 da tela */
	{
		if (FIRST_BLOCK >= NBLOCK - BLOCK_WIDTH)
			return;

		if ((FIRST_BLOCK += (BLOCK_WIDTH >> 4)) > NBLOCK - BLOCK_WIDTH)
			FIRST_BLOCK = NBLOCK - BLOCK_WIDTH;

		state = 'D';
	}
	elif (y > middle + 20)		/* Desce meia tela */
	{
		if (FIRST_BLOCK >= NBLOCK - BLOCK_WIDTH)
			return;

		if ((FIRST_BLOCK += (BLOCK_WIDTH >> 1)) > NBLOCK - BLOCK_WIDTH)
			FIRST_BLOCK = NBLOCK - BLOCK_WIDTH;
	}
	else
	{
		return;
	}

	/*
	 *	x
	 */
	height = scrollbar.end - scrollbar.begin;

#if (0)	/*******************************************************/
error ("NBLOCK = %d, BLOCK_WIDTH = %d", NBLOCK, BLOCK_WIDTH);
	size = height >> 1;
	size = height * BLOCK_WIDTH / (NBLOCK - BLOCK_WIDTH);
#endif	/*******************************************************/
	size = height * BLOCK_WIDTH / NBLOCK;

	if (size > scrollbar.end - scrollbar.begin)
		size = scrollbar.end - scrollbar.begin;

	scrollbar.top	 = scrollbar.begin + ((height - size) * FIRST_BLOCK) / (NBLOCK - BLOCK_WIDTH);
	scrollbar.bottom = scrollbar.top + size;

#if (0)	/*******************************************************/
error ("FIRST_BLOCK = %d, size = %d, top = %d, botton = %d",
FIRST_BLOCK, size, scrollbar.top, scrollbar.bottom);
#endif	/*******************************************************/

	if ((inc = scrollbar.top - last_top) > 0)
	{
		XClearArea
		(	display, scrollbar.win,
			last_top, 0,
			inc, SB_WIDTH,
			False
		);
	}

	if ((inc = last_bottom - scrollbar.bottom) > 0)
	{
		XClearArea
		(	display, scrollbar.win,
			scrollbar.bottom, 0,
			inc, SB_WIDTH,
			False
		);
	}

	XFillRectangle
	(	display, scrollbar.win,
		scrollbar.gc.main_gc,
		scrollbar.top, 0,
		(scrollbar.bottom - scrollbar.top), SB_WIDTH
	);


	/* shadow for scrollbar slider */

	Draw_Shadow
	(	scrollbar.win,
		scrollbar.gc.top_gc, scrollbar.gc.bottom_gc,
		scrollbar.top, 0,
		(scrollbar.bottom - scrollbar.top), SB_WIDTH 
	);

	/*
	 *	Redesenha as setinhas
	 */
	Draw_button (0, 0, (state == 'U') ? -1 : +1);
	Draw_button (scrollbar.end + 1, 0, (state == 'D') ? -1 : +1);

	scrollbar.state = state;

	/*
	 *	Atualiza a janela de barras
	 */
	display_block_colors ();

}	/* end update_scrollbar */

#if (0)	/*******************************************************/
/*
 ****************************************************************
 *	Desenha a sombra acima/esquerda de uma janela		*
 ****************************************************************
 */
void
Draw_Top_Left (Window win, GC gc, int x, int y, int width, int height)
{
	int		shadow_width = SHADOW_WIDTH;

	if (width == 0 || height == 0)
		shadow_width = 1;

	width += (x - 1); height += (y - 1);

	for (/* vazio */; shadow_width > 0; shadow_width--, x++, y++, width--, height--)
	{
		XDrawLine (display, win, gc, x, y, width, y);
		XDrawLine (display, win, gc, x, y, x,     height);
	}

}	/* end DrawTop_Left */

/*
 ****************************************************************
 *	Desenha a sombra abaixo/direita de uma janela		*
 ****************************************************************
 */
void
Draw_Bottom_Right (Window win, GC gc, int x, int y, int width, int height)
{
	int		shadow_width = SHADOW_WIDTH;

	if (width == 0 || height == 0)
		shadow_width = 1;

	width += (x - 1); height += (y - 1);

	for (x++, y++; shadow_width > 0; shadow_width--, x++, y++, width--, height--)
	{
		XDrawLine (display, win, gc, width, height, width, y);
		XDrawLine (display, win, gc, width, height, x,     height);
	}

}	/* end Draw_Bottom_Right */
#endif	/*******************************************************/

/*
 ****************************************************************
 *	Desenha as 4 sombras					*
 ****************************************************************
 */
void
Draw_Shadow (Window win, GC topShadow, GC botShadow, int win_x, int win_y, int width, int height)
{
	int		shadow_width = SHADOW_WIDTH;
	int		x, y, w, h;

#if (0)	/*******************************************************/
	Draw_Top_Left     (win, topShadow, x, y, width, height);
#endif	/*******************************************************/

	w = width + (win_x - 1); h = height + (win_y - 1);
	x = win_x; y = win_y;

	for (shadow_width = SHADOW_WIDTH; shadow_width > 0; shadow_width--, x++, y++, w--, h--)
	{
		XDrawLine (display, win, topShadow, x, y, w, y);
		XDrawLine (display, win, topShadow, x, y, x, h);
	}

#if (0)	/*******************************************************/
	Draw_Bottom_Right (win, botShadow, x, y, width, height);
#endif	/*******************************************************/

	w = width + (win_x - 1); h = height + (win_y - 1);
	x = win_x + 1; y = win_y + 1;

	for (shadow_width = SHADOW_WIDTH; shadow_width > 0; shadow_width--, x++, y++, w--, h--)
	{
		XDrawLine (display, win, botShadow, w, h, w, y);
		XDrawLine (display, win, botShadow, w, h, x, h);
	}
}	/* end Draw_Shadow */

/*
 ****************************************************************
 *	Desenha o triângulo para cima, com sombra		*
 ****************************************************************
 */
void
Draw_button (int x, int y, int state)
{
	XFillRectangle
	(	display, scrollbar.win,
		scrollbar.gc.main_gc,
		x, y,
		SB_WIDTH, SB_WIDTH
	);

	if (state > 0)
	{
		Draw_Shadow
		(	scrollbar.win,
			scrollbar.gc.top_gc, scrollbar.gc.bottom_gc,
			x, y,
			SB_WIDTH, SB_WIDTH
		);
	}
	else
	{
		Draw_Shadow
		(	scrollbar.win,
			scrollbar.gc.bottom_gc, scrollbar.gc.top_gc,
			x, y,
			SB_WIDTH, SB_WIDTH
		);
	}

}	/* end Draw_button */

/*
 ****************************************************************
 *	Obtém as cores para o sombreado				*
 ****************************************************************
 */
void
set_shadow_colors (Display *Xdisplay, Window win, TRIPLEGC *tp, const char *color_nm, Pixel def)
{
	XGCValues		gcv;
	XColor			xcol, white;
	Colormap		Xcmap;
	Pixel			color_pixel;

	/*
	 *	Prepara a cor principal
	 */
	Xcmap = DefaultColormap (Xdisplay, DefaultScreen (Xdisplay));

	color_pixel = get_window_color (Xdisplay, color_nm, def);

	tp->main_pixel  = gcv.foreground = color_pixel;
	tp->main_gc	= XCreateGC (Xdisplay, win, GCForeground, &gcv);

	/*
	 *	Prepara a cor da sombra do lado de baixo/direita
	 */
	xcol.pixel = color_pixel; XQueryColor (Xdisplay, Xcmap, &xcol);

	xcol.red   >>= 1;
	xcol.green >>= 1;
	xcol.blue  >>= 1;

	if (!XAllocColor (Xdisplay, Xcmap, &xcol))
	{
		error ("Não consegui alocar a cor da sombra de baixo/direita");
		xcol.pixel = color_pixel;
	}

	tp->bottom_pixel = gcv.foreground = xcol.pixel;
	tp->bottom_gc	 = XCreateGC (Xdisplay, win, GCForeground, &gcv);

	/*
	 *	Prepara a cor da sombra do lado de cima/esquerda
	 */
	white.pixel = WhitePixel (Xdisplay, DefaultScreen (Xdisplay));
	XQueryColor (Xdisplay, Xcmap, &white);

	xcol.pixel = color_pixel; XQueryColor (Xdisplay, Xcmap, &xcol);

	xcol.red   = MAX (white.red   / 5, xcol.red);
	xcol.green = MAX (white.green / 5, xcol.green);
	xcol.blue  = MAX (white.blue  / 5, xcol.blue);

	xcol.red   = MIN (white.red,   xcol.red   * 7 / 5);
	xcol.green = MIN (white.green, xcol.green * 7 / 5);
	xcol.blue  = MIN (white.blue,  xcol.blue  * 7 / 5);

	if (!XAllocColor (Xdisplay, Xcmap, &xcol))
	{
		error ("Não consegui alocar a cor da sombra de cima/esquerda");
		xcol.pixel = white.pixel;
	}

	tp->top_pixel   = gcv.foreground = xcol.pixel;
	tp->top_gc	= XCreateGC (Xdisplay, win, GCForeground, &gcv);

}	/* end set_shadow_colors */

/*
 ****************************************************************
 *	Resumo de utilização do programa			*
 ****************************************************************
 */
void
help (void)
{
	fprintf
	(	stderr,
		"%s - programa\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-opções] arg\n",
		pgname, pgversion, pgname
	);
	fprintf
	(	stderr,
		"\nOpções:"
		"\t-x: opção\n"
		"\t-y: opção\n"
		"\t-N: Lê os nomes dos <arquivo>s de \"stdin\"\n"
		"\t    Esta opção é implícita se não forem dados argumentos\n"
		"\nObs.:\tobservação\n"
	);

	exit (2);

}	/* end help */
