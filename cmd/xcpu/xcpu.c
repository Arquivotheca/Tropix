/*
 ****************************************************************
 *								*
 *			xcpu.c					*
 *								*
 *	Desenha o gráfico de uso da CPU				*
 *								*
 *	Versão	4.6.0, de 16.10.04				*
 *		4.6.0, de 19.10.04				*
 *								*
 *	Módulo: xcpu						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2004 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/common.h>
#include <sys/scb.h>
#include <sys/syscall.h>
#include <sys/kcntl.h>
#include <sys/mon.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <a.out.h>
#include <fcntl.h>
#include <stat.h>
#include <errno.h>

#include <X11/Xatom.h>
#include <X11/Intrinsic.h>
#include <X11/cursorfont.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char		pgversion[] = "Versão: 4.6.0, de 19.10.04";

#define	NOVOID		(void *)NULL
#define	elif		else if
#define	EVER		;;
#define STR(x)		# x
#define XSTR(x)		STR (x)
#define	MIN(a,b)	((a) < (b) ? (a) : (b))
#define	MAX(a,b)	((a) > (b) ? (a) : (b))

entry int		vflag;		/* Verbose */

entry int		depth;

entry XStandardColormap	*StandardColormap;

entry Display		*display;

entry Window		root_win;

entry const MON		*mon_ptr;

entry char		*cpu_use_map;

entry int		cpu_use_map_offset;

/*
 ******	Tamanhos ************************************************
 */
enum {	STD_WIDTH = 640, STD_HEIGHT = 100 };

enum {	MIN_WIDTH = 100, MIN_HEIGHT = 100 };

enum {	VEC_SIZE = 60 * 60 };		/* Uma hora */

enum {	BORDER = 1, SHADOW = 3, MAP_OFFSET = 15 };

enum {	SCALE_SZ = 20 };

entry int	MAP_WIDTH, MAP_HEIGHT;

/*
 ******	Valores obtidos das cores *******************************
 */
typedef	struct
{
	Pixel		main_pixel, top_pixel,  bottom_pixel;
	GC		main_gc,    top_gc,	bottom_gc;

}	TRIPLEGC;

/*
 ******	Estrura de uma janela ***********************************
 */
typedef struct
{
	Window		win;
	int		width, height;
	int		x, y;
	TRIPLEGC	triple;
	GC		fore_gc, text_gc;
	int		font_width, font_height, font_descent;

}	WINDATA;

#define	NOWINDATA 	(WINDATA *)NULL

entry WINDATA		cpu;

/*
 ****** Cores variáveis *****************************************
 */
#define	default_fore_color_str	"black"
#define	default_back_color_str	"gray"
#define	default_font_str	"fixed"

#define BORDER_COLOR		0	/*** BlackPixel (display, screen) ***/

/*
 ******	Valores obtidos das cores *******************************
 */
entry TRIPLEGC		fore_triple, back_triple;

/*
 ******	Geometria ***********************************************
 */
entry XSizeHints	szhint =
{
	  PResizeInc|PMinSize|PBaseSize|PWinGravity,		/* Bits valid	flags			*/
	  0,	0,						/*		x, y			*/
	  STD_WIDTH  + 2 * SHADOW + 2 * BORDER,			/*              width          		*/
	  STD_HEIGHT + 2 * SHADOW + 2 * BORDER + SCALE_SZ,	/*		height         	 	*/
	  MIN_WIDTH  + 2 * SHADOW + 2 * BORDER,			/* PMinSize	min_width		*/
	  MIN_HEIGHT + 2 * SHADOW + 2 * BORDER + SCALE_SZ,	/* PMinSize	min_height		*/
	  0,	0,						/*		max_width, max_height	*/
	  1,	1,						/* PResizeInc	width_inc, height_inc	*/
	{ 1,	1 },						/*		min_aspect		*/
	{ 0,	0 },						/*		max_aspect ratio	*/
	  0,							/* PBaseSize	base_width		*/
	  0,							/* PBaseSize	base_height		*/
	  NorthWestGravity					/* PWinGravity	gravity			*/
};

/*
 ******	Protótipos de funções ***********************************
 */
void		shift_cpu_use_map (void);
void		set_font_sizes (int width, int height, int descent, ...);
void		draw_shadow (Window, GC, GC, int, int, int, int);
void		draw_cpu_win (void);
void		event_loop (int main_event_loop);
void		set_window_sizes (int width, int height);
int		set_shadow_colors (Display *, Window, TRIPLEGC *, const char *, Pixel);
void		geometry_to_size_hints (const char *geo_str, XSizeHints *hp);
void		quit_function (int value);
void		help (void);
const char	*edit_event_nm (int event);

/*
 ****************************************************************
 *	Desenha o gráfico de uso da CPU				*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	static XStandardColormap	stdcmap;
	WINDATA				*wp;
	SYM				*sp;
	int				opt;
	const char			*geo_str = NOSTR;
	int				save_argc = argc;
	const char			**save_argv = argv;
	const char			*display_str = NOSTR;
	const char			*font_str, *user_font_str = NOSTR;
	const char			*fore_color_str, *user_fore_color_str = NOSTR;
	const char			*back_color_str, *user_back_color_str = NOSTR;
	XFontStruct			*text_fip;
	int				screen, len;
	int				font_width, font_height, font_descent;
	Window				root, win;
	XClassHint			classhint;
	XWMHints			wmhint;
	XGCValues			gcv;
	GC				file_text_gc;
	const char			*str;
	Atom				map;
	char				text[64];
	SCB				scb;

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
			user_fore_color_str = optarg;
			break;

		    case 'b':			/* Cor de fundo */
			user_back_color_str = optarg;
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
	 ****** Obtém o SCB *************************************
	 */
	if ((int)getscb (&scb) == -1)
		error ("$Não consegui obter o SCB");

	/*
	 ******	Mapeia MON **************************************
	 */
	len = strlen ("_mon"); sp = alloca (SYM_SIZEOF (len));

	sp->s_nm_len = len; strcpy (sp->s_name, "_mon");

	if (kcntl (K_GETSYMGN, len, sp) < 0)
		error ("$Não consegui obter o endereço de \"_mon\"");

	mon_ptr = phys ((void *)sp->s_value, sizeof (MON), O_KERNEL|O_RDONLY);

	if ((int)mon_ptr == -1)
		error ("$Não consegui mapear \"_mon\"");

	/*
	 ****** Cria o mapa inicial vazio ***********************
	 */
	if ((cpu_use_map = calloc (VEC_SIZE, sizeof (char))) == NOSTR)
		error (NOSTR);

	/*
	 ******	Conecta com o servidor **************************
	 */
	if (display_str == NOSTR && open ("/dev/xconsole", O_RDWR) >= 0)
		error ("$O Sistema X-Window não está ativo");

	if ((display = XOpenDisplay (display_str)) == NULL)
		error ("$*Não consegui conectar com o servidor \"%s\"", XDisplayName (display_str));

	screen	= DefaultScreen (display);
	root	= RootWindow (display, screen);

	error_default_function = quit_function;

	/*
	 ******	Obtém o mapa de cores de acordo com a profundidade ******
	 */
	depth  = DefaultDepth (display, screen);

	map = (depth <= 8) ? XA_RGB_DEFAULT_MAP : XA_RGB_BEST_MAP;

	if (XGetStandardColormap (display, root, &stdcmap, map))
		StandardColormap = &stdcmap;
	else
		error ("*Mapa de cores padrão para profundidade %d não disponível", depth);
	
	/*
	 ****** Tenta obter os valores de "XDefault" ************
	 */
	if   (user_fore_color_str != NOSTR)
		fore_color_str = user_fore_color_str;
	elif ((str = XGetDefault (display, "XCpu", "foreground")) != NOSTR)
		fore_color_str = str;
	else
		fore_color_str = default_fore_color_str;

	if   (user_back_color_str != NOSTR)
		back_color_str = user_back_color_str;
	elif ((str = XGetDefault (display, "XCpu", "background")) != NOSTR)
		back_color_str = str;
	else
		back_color_str = default_back_color_str;

	if   (user_font_str != NOSTR)
		font_str = user_font_str;
	elif ((str = XGetDefault (display, "XCpu", "font")) != NOSTR)
		font_str = str;
	else
		font_str = default_font_str;

	/*
	 ******	Obtém os dados da fonte *************************
	 */
	if ((text_fip = XLoadQueryFont (display, font_str)) == NULL)
		error ("$Não encontrei a fonte \"%s\"", font_str);

	font_width   = XTextWidth (text_fip, "MMMMMMMMMM", 10) / 10;
	font_descent = text_fip->descent;
	font_height  = text_fip->ascent + text_fip->descent;

	/*
	 ******	Obtém as cores fixas e variáveis ****************
	 */
	set_shadow_colors (display, root, &fore_triple,    fore_color_str,   0x000000);
	set_shadow_colors (display, root, &back_triple,    back_color_str,   0xBEBEBE);

	gcv.font	= text_fip->fid;
	gcv.foreground  = fore_triple.main_pixel;
	gcv.background  = back_triple.main_pixel;

	file_text_gc	= XCreateGC (display, root, GCForeground|GCBackground|GCFont, &gcv);

	/*
	 ****** Atribui os tamanhos das fontes nas janelas ******
	 */
	set_font_sizes
	(	font_width, font_height, font_descent, 
		&cpu, NOWINDATA
	);

	/*
	 ****** Cria a janela PRINCIPAL	*************************
	 */
	geometry_to_size_hints (geo_str, &szhint);

	set_window_sizes (0, 0);

        win = XCreateSimpleWindow
	(	display, root,
		szhint.x, szhint.y,
		szhint.width, szhint.height,
		0, BORDER_COLOR, BlackPixel (display, screen)
	);

	root_win = win;

	snprintf (text, sizeof (text), "XCpu (%s)", scb.y_uts.uts_nodename);
	XStoreName (display, win, text);

	XSelectInput (display, win, ExposureMask|StructureNotifyMask|KeyPressMask);

	classhint.res_name  = (char *)pgname;
	classhint.res_class = "XCpu";

	wmhint.input		= True;
	wmhint.initial_state	= NormalState;
	wmhint.flags		= InputHint|StateHint;

	XSetWMProperties
	(	display, win, NULL, NULL, (char **)save_argv, save_argc,
		&szhint, &wmhint, &classhint
	);

#if (0)	/*******************************************************/
	Cursor		cursor;
	cursor = XCreateFontCursor (display, XC_left_ptr);
	XDefineCursor (display, win, cursor);
#endif	/*******************************************************/

	XMapWindow (display, win);

	/*
	 ****** Cria a janela da imagem CPU *********************
	 */
	wp = &cpu;

	memmove (&wp->triple, &back_triple, sizeof (TRIPLEGC));
	wp->fore_gc      = fore_triple.main_gc;
	wp->text_gc      = file_text_gc;

	wp->win = XCreateSimpleWindow
	(
		display, win,
		wp->x, wp->y,
		wp->width, wp->height,
		0, BORDER_COLOR,
		wp->triple.main_pixel
	);

	XSelectInput
	(	display, wp->win,
		(ExposureMask|PointerMotionMask|PointerMotionHintMask|
		ButtonPressMask|ButtonReleaseMask|LeaveWindowMask)
	);

	XMapWindow (display, wp->win);

	/*
	 ******	Entra na malha de eventos *******************************
	 */
	event_loop (1);

	/* Não retorna */

	return (0);

}	/* end xcpu */

/*
 ****************************************************************
 *	Malha de eventos					*
 ****************************************************************
 */
void
event_loop (int main_event_loop)
{
	XEvent		event;
	int		fd = ConnectionNumber (display);

	/*
	 *	Espera um evento ou passar 1 segundo
	 */
	for (EVER)
	{
		if (attention (1, &fd, -1, 1) < 0)
		{
			if (errno != EINTR)
				quit_function (0);

			shift_cpu_use_map ();

			continue;
		}

		XNextEvent (display, &event);
#undef	DEBUG
#ifdef	DEBUG
		printf ("Recebi o Evento \"%s\" (%d)\n", edit_event_nm (event.type), event.type);
#endif	DEBUG
		switch (event.type)
		{
		    case Expose:
		    case GraphicsExpose:
			if (event.xexpose.count != 0)
				break;

			if   (event.xany.window == cpu.win)
			{
				draw_cpu_win ();
			}

			break;

		    case KeyPress:
		    {
			KeySym		key;
			char		text[10];

			XLookupString (&event.xkey, text, 10, &key, 0);

			if (text[0] != 'q')
				continue;

			quit_function (0);

			break;
		    }

		    case ConfigureNotify:
		    {
			XEvent		Unused_xevent;
			int		x, y;
			unsigned int	width, height, border;
			Window		root;
			unsigned int	gdepth;

			if (main_event_loop == 0)
				return;

			while
			(	XCheckTypedWindowEvent
				(	display, root_win,
					ConfigureNotify, &Unused_xevent
				)
			)
				/* vazio */;

			XGetGeometry
			(	display, root_win, &root,
				&x, &y, &width, &height,
				&border, &gdepth
			);

			set_window_sizes (width, height);

			break;
		    }

		}	/* end switch */

		if (main_event_loop == 0)
			return;

	}	/* end for (EVER) */

}	/* end event_loop */

/*
 ****************************************************************
 *	Atribui os tamanhos da fonte				*
 ****************************************************************
 */
void
set_font_sizes (int width, int height, int descent, ...)
{
	va_list		ap;
	WINDATA		*wp;

	va_start (ap, descent);

	while ((wp = va_arg (ap, WINDATA *)) != NOWINDATA)
	{
		wp->font_width   = width;
		wp->font_height  = height;
		wp->font_descent = descent;
	}

	va_end (ap);

}	/* end set_font_sizes */

/*
 ****************************************************************
 *	Passou um segundo					*
 ****************************************************************
 */
void
shift_cpu_use_map (void)
{
	const WINDATA	*wp = &cpu;
	static int	old_ticks, old_stop;
	int		ticks, stop;
	int		end_x, end_y, height;

	ticks	  = mon_ptr->y_ticks[0] - old_ticks;
	stop	  = mon_ptr->y_stop[0]  - old_stop;

	old_ticks = mon_ptr->y_ticks[0];
	old_stop  = mon_ptr->y_stop[0];

	if (++cpu_use_map_offset >= VEC_SIZE)
		cpu_use_map_offset = 0;

	cpu_use_map[cpu_use_map_offset] = 100 * (ticks - stop) / ticks;

	height = cpu_use_map[cpu_use_map_offset] * MAP_HEIGHT / 100;

	end_x = SHADOW + MAP_OFFSET + MAP_WIDTH  - 1;
	end_y = SHADOW + MAP_OFFSET + MAP_HEIGHT - 1;

	XCopyArea
	(	display, wp->win, wp->win, wp->text_gc,
		SHADOW + MAP_OFFSET + 1,	SHADOW + MAP_OFFSET,
		MAP_WIDTH - 1,			MAP_HEIGHT,
		SHADOW + MAP_OFFSET,		SHADOW + MAP_OFFSET
	);

	XDrawLine
	(	display, wp->win, wp->text_gc,
		end_x,	end_y,
		end_x,	end_y - height
	);

	XDrawLine
	(	display, wp->win, wp->triple.main_gc,
		end_x,	end_y - height - 1,
		end_x,	SHADOW + MAP_OFFSET
	);

	XFlush (display);

}	/* end shift_cpu_use_map */

/*
 ****************************************************************
 *	Desenha a janela do gráfico				*
 ****************************************************************
 */
void
draw_cpu_win (void)
{
	const WINDATA	*wp = &cpu;
	int		x, begin_x, end_x, end_y, index, count, height;

	draw_shadow
	(	wp->win,
		wp->triple.top_gc, wp->triple.bottom_gc,
		0, 0,
		wp->width, wp->height
	);

	begin_x = SHADOW + MAP_OFFSET;
	end_x   = SHADOW + MAP_OFFSET + MAP_WIDTH  - 1;

	end_y   = SHADOW + MAP_OFFSET + MAP_HEIGHT - 1;

	for (count = VEC_SIZE, index = cpu_use_map_offset, x = end_x; x >= begin_x; x--, index--, count--)
	{
		if (index < 0)
			index = VEC_SIZE - 1;

		if   (count <= 0)
			height = 0;
		else
			height = cpu_use_map[index] * MAP_HEIGHT / 100;

		XDrawLine
		(	display, wp->win, wp->text_gc,
			x, end_y,
			x, end_y - height
		);

		XDrawLine
		(	display, wp->win, wp->triple.main_gc,
			x, end_y - height - 1,
			x, SHADOW + MAP_OFFSET
		);
	}

	for (index = 0, x = end_x; x > SHADOW + MAP_OFFSET; x -= 60, index++)
	{
		char		text[8];

		sprintf (text, "%2d.0", index);

		XDrawLine
		(	display, wp->win, wp->text_gc,
			x, end_y,
			x, end_y + 3
		);

		XDrawString
		(	display, wp->win, wp->text_gc,
                        x - 2 * wp->font_width - 1,	end_y + (SCALE_SZ) + wp->font_descent,
                        text,				4
                );
	}

	XFlush (display);

}	/* end draw_cpu_win */

/*
 ****************************************************************
 *	Desenha as 4 sombras					*
 ****************************************************************
 */
void
draw_shadow (Window win, GC topShadow, GC botShadow, int win_x, int win_y, int width, int height)
{
	int		shadow_width;
	int		x, y, w, h;

	w = width + (win_x - 1); h = height + (win_y - 1);
	x = win_x; y = win_y;

	for (shadow_width = SHADOW; shadow_width > 0; shadow_width--, x++, y++, w--, h--)
	{
		XDrawLine (display, win, topShadow, x, y, w, y);
		XDrawLine (display, win, topShadow, x, y, x, h);
	}

	w = width + (win_x - 1); h = height + (win_y - 1);
	x = win_x + 1; y = win_y + 1;

	for (shadow_width = SHADOW; shadow_width > 0; shadow_width--, x++, y++, w--, h--)
	{
		XDrawLine (display, win, botShadow, w, h, w, y);
		XDrawLine (display, win, botShadow, w, h, x, h);
	}

}	/* end draw_shadow */

/*
 ****************************************************************
 *	Analisa uma cor dada					*
 ****************************************************************
 */
Pixel
get_window_color (const char *color_nm, Pixel def)
{
	XColor			xcol;
	Colormap		Xcmap;

	if (color_nm == NOSTR || color_nm[0] == '\0')
		return (def);

	Xcmap = DefaultColormap (display, DefaultScreen (display));

	if (!XParseColor (display, Xcmap, color_nm, &xcol))
		{ error ("Não consegui alocar a cor \"%s\"", color_nm); return (def); }

	if (!XAllocColor (display, Xcmap, &xcol))
		{ error ("Não consegui alocar a cor \"%s\"", color_nm); return (def); }

	return (xcol.pixel);

}	/* end get_window_color */

/*
 ****************************************************************
 *	Obtém as cores para o sombreado				*
 ****************************************************************
 */
int
set_shadow_colors (Display *Xdisplay, Window win, TRIPLEGC *tp, const char *color_nm, Pixel def)
{
	XGCValues		gcv;
	XColor			xcol, white;
	Colormap		Xcmap;
	Pixel			color_pixel;
	int			value = 0;

	/*
	 *	Prepara a cor principal
	 */
	Xcmap = DefaultColormap (Xdisplay, DefaultScreen (Xdisplay));

	color_pixel = get_window_color (color_nm, def);

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
		value = -1;
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
		value = -1;
	}

	tp->top_pixel   = gcv.foreground = xcol.pixel;
	tp->top_gc	= XCreateGC (Xdisplay, win, GCForeground, &gcv);

	return (value);

}	/* end set_shadow_colors */

/*
 ****************************************************************
 *	Calcula os tamanhos das janelas				*
 ****************************************************************
 */
void
set_window_sizes (int width, int height)
{
	XWindowChanges		value;
	WINDATA			*wp;
	int			width_inc, height_inc;

	/*
	 ****** Verifica se é a primeira vez, ... ***************
	 */
	if (width == 0 && height == 0)	/* Primeira vez */ 
	{
		/*
		 *	Calcula o tamanho das janelas principais
		 */
		width  = szhint.width;
		height = szhint.height;

		wp		= &cpu;
		wp->width	= width  - 2 * BORDER;
		wp->height	= height - 2 * BORDER;
		wp->x		= BORDER;
		wp->y		= BORDER;

		MAP_WIDTH	= wp->width  - 2 * SHADOW - 2 * MAP_OFFSET;
		MAP_HEIGHT	= wp->height - 2 * SHADOW - 2 * MAP_OFFSET - SCALE_SZ;

		return;
	}

	/*
	 *	Não é a primeira vez
	 */
#if (0)	/*******************************************************/
	if (width == 0)
		width  = szhint.width;

	if (height == 0)
		height = szhint.height;
#endif	/*******************************************************/

	if (width == szhint.width && height == szhint.height)
		return;

	width_inc  = width  - szhint.width;
	height_inc = height - szhint.height;

#ifdef	DEBUG
	printf ("Mudando a largura %d para %d\n", szhint.width, width);
#endif	DEBUG

	wp		 = &cpu;
	wp->width	+= width_inc;
	wp->height	+= height_inc;
   /***	wp->x		 = ...;					***/
   /***	wp->y		 = ...;					***/

   /***	wp		 = &cpu;				***/
	value.width  = wp->width;
	value.height = wp->height;
	XConfigureWindow (display, wp->win, CWWidth|CWHeight, &value);

	MAP_WIDTH	+= width_inc;
	MAP_HEIGHT	+= height_inc;

	value.width  = szhint.width  = width;
	value.height = szhint.height = height;

	XConfigureWindow (display, root_win, CWWidth|CWHeight, &value);

}	/* end set_window_sizes */

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

#ifdef	DEBUG
	printf ("geo_str = %s, mask = %X, width = %d, height = %d\n", geo_str, user_geo_mask, width, height);
#endif	DEBUG

	if (user_geo_mask & WidthValue)
		{ hp->width = width; hp->flags |= USSize; }

	if (user_geo_mask & HeightValue)
		{ hp->height = height; hp->flags |= USSize; }

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
 *	Para encerrar, em caso de erro				*
 ****************************************************************
 */
void
quit_function (int value)
{
	XCloseDisplay (display);
	exit (value);

}	/* end quit_function */

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
		"%s - desenha o gráfico de uso da UCP\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-g <geo>] [-f <frente>] [-b <fundo>] [-d <video>]\n",
		pgname, pgversion, pgname
	);
	fprintf
	(	stderr,
		"\nOpções:"
		"\t-g: Geometria <geo> (no formato <largura>x<altura>, em pixeis) \n"
		"\t-f: Use a cor de <frente> dada\n"
		"\t-b: Use a cor de <fundo> dada\n"
		"\t-d: Conecte-se ao <vídeo> dado\n"
	);

	exit (2);

}	/* end help */

#ifdef	DEBUG
/*
 ****************************************************************
 *	Edita o nome do evento					*
 ****************************************************************
 */
const char	* const event_nm[] =
{
	"Event 0",		/*  0	*/
	"Event 1",		/*  1	*/
	"KeyPress",		/*  2	*/
	"KeyRelease",		/*  3	*/
	"ButtonPress",		/*  4	*/
	"ButtonRelease",	/*  5	*/
	"MotionNotify",		/*  6	*/
	"EnterNotify",		/*  7	*/
	"LeaveNotify",		/*  8	*/
	"FocusIn",		/*  9	*/
	"FocusOut",		/* 10	*/
	"KeymapNotify",		/* 11	*/
	"Expose",		/* 12	*/
	"GraphicsExpose",	/* 13	*/
	"NoExpose",		/* 14	*/
	"VisibilityNotify",	/* 15	*/
	"CreateNotify",		/* 16	*/
	"DestroyNotify",	/* 17	*/
	"UnmapNotify",		/* 18	*/
	"MapNotify",		/* 19	*/
	"MapRequest",		/* 20	*/
	"ReparentNotify",	/* 21	*/
	"ConfigureNotify",	/* 22	*/
	"ConfigureRequest",	/* 23	*/
	"GravityNotify",	/* 24	*/
	"ResizeRequest",	/* 25	*/
	"CirculateNotify",	/* 26	*/
	"CirculateRequest",	/* 27	*/
	"PropertyNotify",	/* 28	*/
	"SelectionClear",	/* 29	*/
	"SelectionRequest",	/* 30	*/
	"SelectionNotify",	/* 31	*/
	"ColormapNotify",	/* 32	*/
	"ClientMessage",	/* 33	*/
	"MappingNotify",	/* 34	*/
	"LASTEvent"		/* 35	*/
};

const char *
edit_event_nm (int event)
{
	if ((unsigned)event >= 35)
		return ("???????");
	else
		return (event_nm[event]);

}	/* end edit_event_nm */
#endif	DEBUG

