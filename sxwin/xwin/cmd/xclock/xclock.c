/*
 ****************************************************************
 *								*
 *			xclock.c				*
 *								*
 *	Relógio Analógico					*
 *								*
 *	Versão	3.0.2, de 16.12.98				*
 *		4.6.0, de 22.10.04				*
 *								*
 *	Módulo: xc/programs/xclock				*
 *		X Windows - Clientes				*
 *		Baseado no software homônimo do XFree86		*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright (c) 2004 TROPIX Technologies Ltd.	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <ctype.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Intrinsic.h>

/*
 ****************************************************************
 *	Nome e versão do programa				*
 ****************************************************************
 */
entry	const char	pgversion[] =	"Versão 4.6.0, de 22.10.04";

entry	int		save_argc;	/* Para XSetWMProperties */
entry	const char	**save_argv;

/*
 ****************************************************************
 *	Definições gerais					*
 ****************************************************************
 */
#define	elif			else if
#define	EVER			;;
#define	MIN(a,b)		((a) < (b) ? (a) : (b))
#define	MAX(a,b)		((a) > (b) ? (a) : (b))

#define MAIL_DIR		"/var/mail/"
#define MAIL_STATUS		"/status"

#define	THICKNESS		4		/* grossura dos ponteiros */

#define	MAX_CLOCK		60
#define	DEFAULT_CLOCK_UPDATE	1
#define	DEFAULT_MAIL_UPDATE	60

enum {	BORDER = 1, SHADOW = 3 };

/*
 ****** Cores variáveis *****************************************
 */
#define	default_fore_color_str	"black"
#define	default_back_color_str	"white"

/*
 ****************************************************************
 *	Declarações de Tipos					*
 ****************************************************************
 */
typedef struct
{
	Window	win;
	int	width, height;

}	mywindow_t;

typedef struct
{
	int	h_x, h_y,		/* hora */
		m_x, m_y,		/* minuto */
		s_x, s_y;		/* segundo */

}	hands_t;		/* hand positions (x,y) */

/*
 ******	Valores obtidos das cores *******************************
 */
typedef	struct
{
	Pixel		main_pixel, top_pixel,  bottom_pixel;
	GC		main_gc,    top_gc,	bottom_gc;

}	TRIPLEGC;

/*
 ****************************************************************
 *	Variáveis globais					*
 ****************************************************************
 */
entry	Display		*dpy;
entry	int		screen;
entry	Window		root;

entry	XGCValues	gcv, rgcv;
entry	GC		Xgc, Xrvgc;
entry	Atom		wmDeleteWindow;
entry	TRIPLEGC	back_triple;

entry	mywindow_t	Clock = {None, 200, 200};	/* parent window */
entry	mywindow_t	Icon = {None, 65, 65};		/* icon window */

entry	int		iconic_state = NormalState;	/* iconic startup? */
entry	int		clock_update = DEFAULT_CLOCK_UPDATE;
entry	int		mail_update  = DEFAULT_MAIL_UPDATE;

entry	int		mail_fd = -1;
entry	STAT		mail_st;

entry	XSizeHints	szHint =
{
	PMinSize | PResizeInc | PBaseSize | PWinGravity,
	0, 0, 200, 200,		/* x, y, width and height */
	1, 1,			/* Min width and height */
	0, 0,			/* Max width and height */
	1, 1,			/* Width and height increments */
	{1, 1},			/* x, y increments */
	{1, 1},			/* Aspect ratio - not used */
	0, 0,			/* base size */
	NorthWestGravity	/* gravity */
};

/*
 ****************************************************************
 *	Protótipos de Funções					*
 ****************************************************************
 */
void	create_windows (int, const char *[], const char *);
void	process_events (void);
void	draw_window (mywindow_t *, int);
void	draw_shadow (Window, GC, GC, int, int, int, int);
void	help (int);
ulong	get_color (Display *, const char *, ulong);
int	set_shadow_colors (Display *, Window, TRIPLEGC *, const char *, Pixel);
void	geometry_to_size_hints (Display *, const char *, XSizeHints *);
void	msg (const char *, ...);

/*
 ****************************************************************
 *	Ponto de Entrada					*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	int		opt;
	char		*mail_file;
	const char	*user;
	const char	*display = NOSTR, *geometry = NOSTR;
	const char	*fore_color_str, *user_fore_color_str = NOSTR;
	const char	*back_color_str, *user_back_color_str = NOSTR;
	const char	*str;

	save_argc = argc;
	save_argv = argv;

	/*
	 *	Analisa os argumentos para o programa.
	 */
	while ((opt = getopt (argc, argv, "MHid:D:g:b:f:u:m:")) != EOF)
	{
		switch (opt)
		{
		    case 'M':
			exit (0);

		    case 'H':
			help (0);

		    case 'i':
			iconic_state = IconicState;
			break;

		    case 'd':
			display = optarg;
			break;

		    case 'D':
		/***	desktop = optarg;	***/
			break;

		    case 'g':
			geometry = optarg;
			break;

		    case 'b':
			user_back_color_str = optarg;
			break;

		    case 'f':
			user_fore_color_str = optarg;
			break;

		    case 'u':
			clock_update = atol (optarg);
			if (clock_update < 1 || clock_update > MAX_CLOCK)
			{
				msg
				(	"$Argumento fora dos limites para"
					" opção '-u'"
				);
			}
			break;

		    case 'm':
			mail_update = atol (optarg);
			if (mail_update < 1 || mail_update > MAX_CLOCK)
			{
				msg
				(	"$Argumento fora dos limites para"
					" opção '-m'"
				);
			}
			break;

		    default:			/* Erro */
			putc ('\n', stderr);
			help (1);
		}
	}

	argv += optind;
	argc -= optind;

	if (argc != 0)
		help (1);

	/*
	 *	Abre a conexão com o servidor.
	 */
	if ((dpy = XOpenDisplay (display)) == NULL)
	{
		msg
		(	"$Não consegui conexão com o servidor \"%s\"",
			XDisplayName (display)
		);
	}

	/*
	 *	Guarda algumas informações da conexão.
	 */
	screen	= DefaultScreen (dpy);
	root	= RootWindow (dpy, screen);

	/*
	 ****** Tenta obter os valores de "XDefault" ************
	 */
	if   (user_fore_color_str != NOSTR)
		fore_color_str = user_fore_color_str;
	elif ((str = XGetDefault (dpy, "XClock", "foreground")) != NOSTR)
		fore_color_str = str;
	else
		fore_color_str = default_fore_color_str;

	if   (user_back_color_str != NOSTR)
		back_color_str = user_back_color_str;
	elif ((str = XGetDefault (dpy, "XClock", "background")) != NOSTR)
		back_color_str = str;
	else
		back_color_str = default_back_color_str;

	/*
	 *	Aloca as cores de frente e fundo.
	 */
	rgcv.background = gcv.foreground = get_color (dpy, fore_color_str, BlackPixel (dpy, screen));
	rgcv.foreground = gcv.background = get_color (dpy, back_color_str, WhitePixel (dpy, screen));

	set_shadow_colors (dpy, root, &back_triple, back_color_str, 0xBEBEBE);

	/*
	 *	Cria as janelas.
	 */
	create_windows (save_argc, save_argv, geometry);

	/*
	 *	Cria 2 GC's: um para vídeo normal e outro para
	 *	vídeo reverso.
	 */
	Xgc =	XCreateGC
		(	dpy, Clock.win,
			GCForeground | GCBackground, &gcv
		);

	Xrvgc =	XCreateGC
		(	dpy, Clock.win,
			GCForeground | GCBackground, &rgcv
		);

	/*
	 *	Procura a caixa postal do usuário
	 */
	if ((user = getenv ("USER")) != NOSTR)
	{
		mail_file =	malloc
				(	strlen (MAIL_DIR) +
					strlen (user) +
					strlen (MAIL_STATUS) + 1
				);

		if (mail_file != NOSTR)
		{
			strcpy (mail_file, MAIL_DIR);
			strcat (mail_file, user);
			strcat (mail_file, MAIL_STATUS);

			mail_fd = open (mail_file, O_RDONLY);

			if (fstat (mail_fd, &mail_st) < 0)
				mail_st.st_mtime = 0;
		}
	}

	/*
	 *	Malha de eventos.
	 */
	process_events ();

	XCloseDisplay (dpy);

	return (0);

}	/* end main */

/*
 ****************************************************************
 *	Cria as janelas						*
 ****************************************************************
 */
void
create_windows (int argc, const char *argv[], const char *geometry)
{
	XClassHint	classHint;
	XWMHints	wmHint;

	geometry_to_size_hints (dpy, geometry, &szHint);

	Clock.width  =	szHint.width;
	Clock.height =	szHint.height;

	Clock.win =	XCreateSimpleWindow
			(	dpy, root,
				szHint.x, szHint.y,
				szHint.width, szHint.height,
				0,
				gcv.foreground, gcv.background
			);

	Icon.win =	XCreateSimpleWindow
			(	dpy, root,
				szHint.x, szHint.y,
				Icon.width, Icon.height,
				0,
				gcv.foreground, gcv.background
			);

	wmHint.initial_state	= iconic_state;
	wmHint.icon_window	= Icon.win;
	wmHint.flags		= InputHint | StateHint | IconWindowHint;
	wmHint.input		= True;

	classHint.res_name	= (char *)pgname;
	classHint.res_class	= "Clock";

	XSetWMProperties
	(	dpy, Clock.win, NULL, NULL, (char **)argv, argc,
		&szHint, &wmHint, &classHint
	);

	XSelectInput
	(	dpy, Clock.win,
		ExposureMask | StructureNotifyMask | ButtonPressMask
	);

	XSelectInput (dpy, Icon.win, ExposureMask | ButtonPressMask);

	XMapWindow (dpy, Clock.win);

	wmDeleteWindow = XInternAtom (dpy, "WM_DELETE_WINDOW", False);
	XSetWMProtocols (dpy, Clock.win, &wmDeleteWindow, 1);
	XSetWMProtocols (dpy, Icon.win,  &wmDeleteWindow, 1);

}	/* end create_windows */

/*
 ****************************************************************
 *	Processa os eventos					*
 ****************************************************************
 */
void
process_events (void)
{
	XEvent	ev;
	int	fd;

	fd = XConnectionNumber (dpy);

	/*
	 *	Malha de eventos.
	 */
	for (EVER)
	{
		while (XPending (dpy))
		{
			XNextEvent (dpy, &ev);

			switch (ev.type)
			{
			    case ClientMessage:
				/* check for delete window requests */
				if
				(	(ev.xclient.format == 32) &&
					(ev.xclient.data.l[0] == wmDeleteWindow)
				)
					return;
				break;

			    case Expose:
			    case GraphicsExpose:
				/* need to re-draw a window */
				if (ev.xany.window == Clock.win)
					draw_window (&Clock, 1);
				else if (ev.xany.window == Icon.win)
					draw_window (&Icon, 1);
				break;

			    case ConfigureNotify:
				/* window has been re-sized */
				if (ev.xany.window == Clock.win)
				{
					Clock.width  = ev.xconfigure.width;
					Clock.height = ev.xconfigure.height;
				}
				break;

			    case ButtonPress:
				/* button press to dismiss message window */
				if (ev.xany.window == Clock.win)
				{
#ifdef MAIL_SPAWN
					/* left button action - spawn a mail reader */
					if (ev.xbutton.button == Button1)
						system (MAIL_SPAWN);
#endif
					/* redraw the window */
					draw_window (&Clock, 1);
				}
				break;
			}
		}

		attention (1, &fd, -1, clock_update);

		draw_window (&Clock, 0);
		draw_window (&Icon, 0);
	}

}	/* end process_events */

/*
 ****************************************************************
 *	Obtém o tempo equivalente em minutos			*
 ****************************************************************
 */
time_t
mk_time (TM *tmval)
{
	return
	(	tmval->tm_min
		+ 60 * (tmval->tm_hour
		+ 24 * (tmval->tm_mday
		+ 31 * ((tmval->tm_mon + 1)
		+ 12 * tmval->tm_year)))
	);

}	/* end mk_time */

/*
 ****************************************************************
 *	Redesenha a janela					*
 ****************************************************************
 */
#define XPOS(i,val) (xcenter + ((W->width  - SHADOW) * Sin[i%60] * (val) + 100000) / 200000)
#define YPOS(i,val) (ycenter - ((W->height - SHADOW) * Sin[(i+15)%60] * (val) + 100000) / 200000)

/*
 ******	Tabela de senos (x 1000), para evitar ponto flutuante.
 */
const int Sin[60] =
{
	0,
	105, 208, 309, 407, 500, 588, 669,
	743, 809, 866, 914, 951, 978, 995,
	1000,
	995, 978, 951, 914, 866, 809, 743,
	669, 588, 500, 407, 309, 208, 105,
	0,
	-105, -208, -309, -407, -500, -588, -669,
	-743, -809, -866, -914, -951, -978, -995,
	-1000,
	-995, -978, -951, -914, -866, -809, -743,
	-669, -588, -500, -407, -309, -208, -105
};

void
draw_window (mywindow_t *W, int full_redraw)
{
	time_t		currentTime;
	TM		*tmval;
	int		xcenter, ycenter, angle, *pMailUp, i, j;
	hands_t		HandsNow, *pHandsOld;
	GC		X_gc, X_rvgc;

	static int	savedDay = -1;
	static hands_t	HandsOld = { -1 };
	static hands_t	HandsOld_icon = { -1 };
	static time_t	mailTime = 0;
	static int	MailUp = 0, MailUp_rvideo = 0;
	static int	MailUp_icon = 0;

	currentTime = time (NULL);
	tmval = localtime (&currentTime);

	pMailUp = W->win == Icon.win ? &MailUp_icon : &MailUp;

	if ((currentTime - mailTime) >= mail_update)
	{
		STAT	st;
		char	c;

		if
		(	mail_fd >= 0 && fstat (mail_fd, &st) >= 0 &&
			st.st_size > 0 && st.st_mtime > mail_st.st_mtime &&
			read (mail_fd, &c, 1) == 1 &&
			lseek (mail_fd, 0, L_SET) >= 0 && c == 'N'
		)
		{	/*
			 *	Chegou correspondência.
			 */
			mail_st.st_mtime = st.st_mtime;

			if (!*pMailUp)
			{
				*pMailUp = 1;
				full_redraw++;

				XSetWindowBackground
				(	dpy,
					W->win,
					gcv.foreground
				);

				XBell (dpy, 0);
			}
		}
		else
		{
			if (*pMailUp)
			{
				*pMailUp = 0;
				full_redraw++;

				XSetWindowBackground
				(	dpy,
					W->win,
					gcv.background
				);
			}
		}

		if (MailUp == MailUp_icon)
			mailTime = currentTime;

		MailUp_rvideo = *pMailUp;
	}

	if (tmval->tm_yday != savedDay)
	{	/*
		 *	Uma vez por dia atualiza o nome da janela.
		 */
		char	str[20];

		savedDay = tmval->tm_yday;
		strftime (str, sizeof(str), "%x", tmval);

		XStoreName (dpy, Clock.win, str);
		XSetIconName (dpy, Clock.win, str);
	}

	if (full_redraw)
		XClearWindow (dpy, W->win);

	/*
	 *	Convert multi-field time info to a single integer with a resolution
	 *	in minutes.
	 */
	currentTime = mk_time (tmval);

	/*
	 ******	Desenha o relógio *******************************
	 */
	xcenter = (W->width  / 2);
	ycenter = (W->height / 2);

	/*
	 *	Calcula a posição dos ponteiros.
	 */
	angle = (tmval->tm_hour % 12) * 5 + (tmval->tm_min / 12);
	HandsNow.h_x = XPOS (angle, 60);
	HandsNow.h_y = YPOS (angle, 60);

	angle = tmval->tm_min;
	HandsNow.m_x = XPOS (angle, 80);
	HandsNow.m_y = YPOS (angle, 80);

	if (clock_update == 1)
	{	/*
		 *	Se o período de atualização for 1s, posiciona
		 *	também o ponteiro dos segundos.
		 */
		angle = tmval->tm_sec;
		HandsNow.s_x = XPOS (angle, 85);
		HandsNow.s_y = YPOS (angle, 85);
	}

	pHandsOld = W->win == Icon.win ?  &HandsOld_icon : &HandsOld;

	if (MailUp_rvideo)
	{
		X_gc = Xrvgc;
		X_rvgc = Xgc;
	}
	else
	{
		X_gc = Xgc;
		X_rvgc = Xrvgc;
	}

	if (full_redraw)
	{	/*
		 *	Desenha os marcadores.
		 */
		for (angle = 0; angle < 60; angle++)
		{
			XDrawPoint
			(	dpy, W->win, X_gc,
				XPOS (angle, 95),
				YPOS (angle, 95)
			);
		}

		for (angle = 0; angle < 60; angle += 5)
		{
			XDrawLine
			(	dpy, W->win, X_gc,
				XPOS (angle, 90),
				YPOS (angle, 90),
				XPOS (angle, 100),
				YPOS (angle, 100)
			);
		}
	}
	else if (memcmp (pHandsOld, &HandsNow, sizeof (hands_t)))
	{	/*
		 *	Apaga os ponteiros antigos.
		 */
		for (i = -THICKNESS; i <= THICKNESS; i++)
		{
			for (j = -THICKNESS; j <= THICKNESS; j++)
			{
				/* hour/minute hands */
				XDrawLine
				(	dpy, W->win, X_rvgc,
					xcenter + i,
					ycenter + j,
					pHandsOld->h_x, pHandsOld->h_y
				);

				XDrawLine
				(	dpy, W->win, X_rvgc,
					xcenter + i,
					ycenter + j,
					pHandsOld->m_x, pHandsOld->m_y
				);
			}
		}

		if (clock_update == 1)
		{
			XDrawLine
			(	dpy, W->win, X_rvgc,
				xcenter,
				ycenter,
				pHandsOld->s_x, pHandsOld->s_y
			);
		}
	}

	if (full_redraw || memcmp (pHandsOld, &HandsNow, sizeof(hands_t)))
	{	/*
		 *	Desenha novos ponteiros.
		 */
		for (i = -THICKNESS; i <= THICKNESS; i++)
		{
			for (j = -THICKNESS; j <= THICKNESS; j++)
			{
				/* hour/minute hands */
				XDrawLine
				(	dpy, W->win, X_gc,
					xcenter + i,
					ycenter + j,
					HandsNow.h_x, HandsNow.h_y
				);

				XDrawLine
				(	dpy, W->win, X_gc,
					xcenter + i,
					ycenter + j,
					HandsNow.m_x, HandsNow.m_y
				);
			}
		}

		if (clock_update == 1)
		{
			XDrawLine
			(	dpy, W->win, X_gc,
				xcenter,
				ycenter,
				HandsNow.s_x, HandsNow.s_y
			);
		}

		*pHandsOld = HandsNow;
	}

	if (full_redraw)
		draw_shadow (W->win, back_triple.top_gc, back_triple.bottom_gc, 0, 0, W->width, W->height);


}	/* end draw_window */

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
		XDrawLine (dpy, win, topShadow, x, y, w, y);
		XDrawLine (dpy, win, topShadow, x, y, x, h);
	}

	w = width + (win_x - 1); h = height + (win_y - 1);
	x = win_x + 1; y = win_y + 1;

	for (shadow_width = SHADOW; shadow_width > 0; shadow_width--, x++, y++, w--, h--)
	{
		XDrawLine (dpy, win, botShadow, w, h, w, y);
		XDrawLine (dpy, win, botShadow, w, h, x, h);
	}

}	/* end draw_shadow */

/*
 ****************************************************************
 *	Analisa a geometria					*
 ****************************************************************
 */
void
geometry_to_size_hints (Display *dp, const char *geom, XSizeHints *hp)
{
	int		x, y, flags, scr;
	unsigned int	width, height;

	if (geom == NOSTR)
		return;

	scr = DefaultScreen (dp);

	flags = XParseGeometry (geom, &x, &y, &width, &height);

	if (flags & WidthValue)
	{
		hp->width = width + hp->base_width;
		hp->flags |= USSize;
	}

	if (flags & HeightValue)
	{
		hp->height = height + hp->base_height;
		hp->flags |= USSize;
	}

	if (flags & XValue)
	{
		if (flags & XNegative)
		{
			x += DisplayWidth (dp, scr) - hp->width;
			hp->win_gravity = NorthEastGravity;
		}

		hp->x = x;
		hp->flags |= USPosition;
	}

	if (flags & YValue)
	{
		if (flags & YNegative)
		{
			y += DisplayHeight (dp, scr) - hp->height;
			hp->win_gravity = hp->win_gravity == NorthEastGravity ?
				SouthEastGravity : SouthWestGravity;
		}

		hp->y = y;
		hp->flags |= USPosition;
	}

}	/* end geometry_to_size_hints */

ulong
get_color (Display *dp, const char *color_str, ulong default_pixel)
{
	XColor		xcol;
	Colormap	cmap;

	if (color_str == NOSTR)
		return (default_pixel);

	cmap = DefaultColormap (dp, DefaultScreen (dp));

	if
	(	!XParseColor (dp, cmap, color_str, &xcol) ||
		!XAllocColor (dp, cmap, &xcol)
	)
	{
		msg ("Não consegui alocar a cor \"%s\"", color_str);
		return (default_pixel);
	}

	return (xcol.pixel);

}	/* end get_color */

/*
 ****************************************************************
 *	Obtém as cores para o sombreado				*
 ****************************************************************
 */
int
set_shadow_colors (Display *Xdisplay, Window win, TRIPLEGC *tp, const char *color_nm, Pixel def)
{
	XGCValues		sgcv;
	XColor			xcol, white;
	Colormap		Xcmap;
	Pixel			color_pixel;
	int			value = 0;

	/*
	 *	Prepara a cor principal
	 */
	Xcmap = DefaultColormap (Xdisplay, DefaultScreen (Xdisplay));

	color_pixel = get_color (Xdisplay, color_nm, def);

	tp->main_pixel  = sgcv.foreground = color_pixel;
	tp->main_gc	= XCreateGC (Xdisplay, win, GCForeground, &sgcv);

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

	tp->bottom_pixel = sgcv.foreground = xcol.pixel;
	tp->bottom_gc	 = XCreateGC (Xdisplay, win, GCForeground, &sgcv);

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

	tp->top_pixel   = sgcv.foreground = xcol.pixel;
	tp->top_gc	= XCreateGC (Xdisplay, win, GCForeground, &sgcv);

	return (value);

}	/* end set_shadow_colors */

/*
 ****************************************************************
 *	Resumo de utilização do programa			*
 ****************************************************************
 */
void
help (int exitval)
{
	fprintf
	(	stderr,
		"%s - relógio analógico para o X-windows\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-i] [-d <display>] [-D <desktop>] [-g <geometria>] [-b <cor>]\n"
		"\t       [-f <cor>] [-u <t>] [-m <t>]\n",
		pgname, pgversion, pgname
	);

	fprintf
	(	stderr,
		"\nOpções:"
		"\t-i: inicie a execução como um ícone\n"
		"\t-d: contacta o servidor <display>\n"
		"\t-D: inicia no Desktop <desktop>\n"
		"\t-g: utiliza <geometria> para o posicionamento\n"
		"\t-b: utiliza <cor> como cor de fundo\n"
		"\t-f: utiliza <cor> como cor de frente\n"
		"\t-u: atualiza a janela    a cada <t> segundos"
		" (<t> entre 1 e %d)\n"
		"\t-m: verifica a \"mailbox\" a cada <t> segundos"
		" (<t> entre 1 e %d)\n",
		MAX_CLOCK, MAX_CLOCK
	);

	exit (exitval);

}	/* end help */
