/*
 ****************************************************************
 *								*
 *			sysmap.c				*
 *								*
 *	Desenha um mapa de uso de alguns recursos do sistema	*
 *								*
 *	Versão	1.0.0, de 10.12.86				*
 *		4.9.0, de 04.10.06				*
 *								*
 *	Módulo: sysmap						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2006 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#define	NCPU	1

#include <sys/common.h>
#include <sys/scb.h>
#include <sys/map.h>
#include <sys/mon.h>
#include <sys/kcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <signal.h>
#include <fcntl.h>
#include <a.out.h>
#include <errno.h>

#include <X11/Xatom.h>
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/Xutil.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão: 4.9.0, de 04.10.06";

enum {  BORDER = 1, SHADOW = 3 };

#define	NOVOID	(void *)NULL
#define	elif	else if
#define	EVER	;;
#define STR(x)	# x
#define XSTR(x)	STR (x)

#define	PERCENT(a, b)	((100*(a))/(b))

entry	SCB		scb;

#define Xcmap		DefaultColormap (Xdisplay, Xscreen)
#define Xscreen		DefaultScreen (Xdisplay)

entry int		exit_code = 0;	/* Código de retorno */

entry int		vflag;		/* Verbose */

entry unsigned		CORE_DIFF;	/* Para calcular os endereços de y_map */

typedef struct mon	MON;

entry MON		*mon_ptr;	/* Ponteiro para MON */
entry const MAP		*core_ptr;	/* Ponteiro para COREMAP */

entry pg_t		map_total;
entry int		sasi_per;
entry int		usb_per;
entry int		ed_per;
entry int		ppp_in_per;
entry int		ppp_out_per;
entry double		dirty_per;
entry double		full_per;
entry double		hit_per;
entry double		inode_per;
entry double		inode_dirty_per;
entry double		kfile_per;

entry int		bar_cpu, bar_sasi, bar_usb;
entry int		bar_core, bar_ed, bar_ppp_in;
entry int		bar_ppp_out, bar_full, bar_dirty;
entry int		bar_hit, bar_inode, bar_inode_dirty, bar_kfile;

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

#define	NOWINDATA	(WINDATA *)NULL

entry WINDATA		map;

/*
 ******	Geometria ***********************************************
 */
XSizeHints	szhint =
{
	PMinSize | PResizeInc | PBaseSize | PWinGravity,
	0, 0, 540, 320,			/* x, y, width, height */
	100, 100,			/* Min width, height */
	0, 0,				/* Max width, height - não usado */
	1, 1,				/* incrementos: width, height */
	{1, 1},				/* incrementos: x, y */
	{0, 0},				/* Aspect ratio - não usado */
	0, 0,				/* base size: width, height */
	NorthWestGravity		/* gravity */
};

XClassHint	classhint;
XWMHints	wmhint;

/*
 ******	Cores variáveis *****************************************
 */
#define default_fore_color_str	"black"
#define default_back_color_str	"white"

/*
 ****** Vetor para o "hit" do "cache" de blocos *****************
 */
#define	N_BHM		60

#define	HIT_INTERVAL	5

typedef	struct { int l_hit; int l_total; }	BHM;

entry BHM	block_vector[N_BHM];

entry BHM	*block_vector_ptr = &block_vector[0];

entry int	sum_hit, sum_total;

/*
 ****** Protótipos de funções ***********************************
 */
void		draw_window (Display *, Window, GC, GC, int);
void		draw_bar (Display *, Window, GC fore_gc, GC back_gc, int y, int *old_width, int width);
void		draw_shadow (Display *, Window, GC, GC, int win_x, int win_y, int width, int height);
int		cpu0get (void);
double		coreget (void);
ulong		get_window_color (Display *, const char *, ulong);
int		set_shadow_colors (Display *, Window, TRIPLEGC *, const char *, Pixel);
void		geometry_to_size_hints (Display *, const char *, XSizeHints *);
void		error (const char *format, ...);
void		help (void);

/*
 ****************************************************************
 *	Desenha um mapa de uso de alguns recursos do sistema	*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	int		opt, save_argc = argc;
	SYM		*sp;
	WINDATA		*wp;
	Display		*Xdisplay;
	const char	*display = NOSTR, **save_argv = argv;
	int		screen, pid, fd;
	Window		root, win;
	XGCValues	gcv;
	XEvent		ev;
	GC		fore_gc, back_gc;
	const char	*geometry = NOSTR;
	const char	*fore_color_str, *user_fore_color_str = NOSTR;
	const char	*back_color_str, *user_back_color_str = NOSTR;
	const char	*str;
	KeySym		key;
	int		len;
	TRIPLEGC	back_triple;
	char		text[64];

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "vg:f:b:d:D:H")) != EOF)
	{
		switch (opt)
		{
		    case 'v':			/* Verbose */
			vflag++;
			break;

		    case 'g':			/* Geometria */
			geometry = optarg;
			break;

		    case 'f':			/* Cor de frente */
			user_fore_color_str = optarg;
			break;

		    case 'b':			/* Cor de fundo */
			user_back_color_str = optarg;
			break;

		    case 'd':
			display = optarg;	/* Display */
			break;

		    case 'D':
		/***	desktop = optarg;   ***/
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
	 *	Passa para "background"
	 */
	if ((pid = fork ()) > 0)
		exit (0);

	if (pid < 0)
		error ("$*Não consegui criar outro processo");

	/*
	 *	Ignora o <^C>
	 */
	signal (SIGINT,  SIG_IGN);
	signal (SIGQUIT, SIG_IGN);

	/*
	 *	Obtém o SCB
	 */
	if ((int)getscb (&scb) == -1)
		error ("$Não consegui obter o SCB");

	/*
	 *	Mapeia MON
	 */
	len = strlen ("_mon"); sp = alloca (SYM_SIZEOF (len));

	sp->s_nm_len = len; strcpy (sp->s_name, "_mon");

	if (kcntl (K_GETSYMGN, len, sp) < 0)
		error ("$Não consegui obter o endereço de \"_mon\"");

	mon_ptr = (MON *)phys ((void *)sp->s_value, sizeof (MON), O_KERNEL|O_RDONLY);

	if ((int)mon_ptr == -1)
		error ("$Não consegui mapear \"_mon\"");

	/*
	 *	Mapeia a tabela de memória
	 */
	core_ptr = (MAP *)phys (scb.y_map, scb.y_nmap * sizeof (MAP), O_KERNEL|O_RDONLY);

	if ((int)core_ptr == -1)
		error ("$Não consegui mapear \"coremap\"");

	CORE_DIFF = (int)core_ptr - (int)scb.y_map;

#if (0)	/*******************************************************/
	error ("core_ptr = %P, CORE_DIFF = %P, soma = %P", core_ptr, CORE_DIFF, scb.y_map + CORE_DIFF);
#endif	/*******************************************************/

	map_total = (scb.y_enducore0 - scb.y_ucore0) + (scb.y_enducore1 - scb.y_ucore1);

	/*
	 *	Conecta com o servidor
	 */
	if ((Xdisplay = XOpenDisplay (display)) == NULL)
		error ("$Não consegui conectar com o servidor \"%s\"", XDisplayName (display));

	screen = DefaultScreen (Xdisplay);
	root   = RootWindow (Xdisplay, screen);

	/*
	 ****** Tenta obter os valores de "XDefault" ************
	 */
	if   (user_fore_color_str != NOSTR)
		fore_color_str = user_fore_color_str;
	elif ((str = XGetDefault (Xdisplay, "XSysmap", "foreground")) != NOSTR)
		fore_color_str = str;
	else
		fore_color_str = default_fore_color_str;

	if   (user_back_color_str != NOSTR)
		back_color_str = user_back_color_str;
	elif ((str = XGetDefault (Xdisplay, "XSysmap", "background")) != NOSTR)
		back_color_str = str;
	else
		back_color_str = default_back_color_str;

	/*
	 *	Processa as cores dadas
	 */
	gcv.foreground = get_window_color (Xdisplay, fore_color_str, BlackPixel (Xdisplay, screen));
	gcv.background = get_window_color (Xdisplay, back_color_str, WhitePixel (Xdisplay, screen));

	geometry_to_size_hints (Xdisplay, geometry, &szhint);

	szhint.width = 540 + 2 * SHADOW; szhint.height = 275 + 2 * SHADOW;

        win =	XCreateSimpleWindow
                (	Xdisplay, root,
			szhint.x, szhint.y, szhint.width, szhint.height, 5,
			gcv.foreground,
			gcv.background
		);

	wp = &map;

	wp->win = win; wp->width = szhint.width, wp->height = szhint.height;

	/*
	 ******	Obtém as cores fixas e variáveis ****************
	 */
	set_shadow_colors (Xdisplay, root, &back_triple, back_color_str, 0xBEBEBE);

	memmove (&map.triple, &back_triple, sizeof (TRIPLEGC));

	sprintf (text, "XSysmap (%s)", scb.y_uts.uts_nodename);
	XStoreName (Xdisplay, win, text);

	XSelectInput (Xdisplay, win, ExposureMask|KeyPressMask);

	classhint.res_name  = (char *)pgname;
	classhint.res_class = "SysMap";

	wmhint.input		= True;
	wmhint.initial_state	= NormalState;
	wmhint.flags		= InputHint | StateHint;

	XSetWMProperties
	(	Xdisplay, win, NULL, NULL, (char **)save_argv, save_argc,
		&szhint, &wmhint, &classhint
	);

	fore_gc = XCreateGC (Xdisplay, win, GCForeground|GCBackground, &gcv);
	gcv.foreground = gcv.background;
	back_gc = XCreateGC (Xdisplay, win, GCForeground, &gcv);

	XMapWindow (Xdisplay, win);

	fd = ConnectionNumber (Xdisplay);

	/*
	 *	Entra na malha de eventos
	 */
	for (EVER)
	{
		if (attention (1, &fd, -1, 1) < 0)
		{
			if (errno != EINTR)
				goto case_Exit;

			draw_window (Xdisplay, win, fore_gc, back_gc, 0);
			continue;
		}

		XNextEvent (Xdisplay, &ev);

		switch (ev.type)
		{
		    case Expose:
		    case GraphicsExpose:
			bar_cpu = bar_sasi = bar_usb = 0;
			bar_core = bar_ed = bar_ppp_in = 0;
			bar_ppp_out = bar_full = bar_dirty = 0;
			bar_hit = bar_inode = bar_inode_dirty = bar_kfile = 0;

			XClearWindow (Xdisplay, win);	/* Redesenha */

			draw_window (Xdisplay, win, fore_gc, back_gc, 1);
			break;

		    case KeyPress:
			len = XLookupString (&ev.xkey, text, 10, &key, 0);

			if (text[0] != 'q')
				continue;

			/* continua */
		    case_Exit:
			XDestroyWindow (Xdisplay, win);
			XCloseDisplay (Xdisplay);
			exit (exit_code);
		}
	}

}	/* end sysmap */

/*
 ****************************************************************
 *	Desenha a tela						*
 ****************************************************************
 */
void
draw_window (Display *Xdisplay, Window win, GC fore_gc, GC back_gc, int expose)
{
	WINDATA		*wp;
	int		i;
	double		d;
	char		area[64];
	enum		{ x = 10 + SHADOW };
	int		y = 13 + SHADOW;

	/*
	 *	Põe a sombra
	 */
	if (expose)
	{
		wp = &map;

		draw_shadow
		(	Xdisplay, wp->win,
			wp->triple.top_gc, wp->triple.bottom_gc,
			0, 0,
			wp->width, wp->height
		);
	}

	/*
	 *	CPU
	 */
	i = cpu0get ();

	sprintf (area, "CPU    %3d.00 %%", i);

	XDrawImageString (Xdisplay, win, fore_gc, x, y + 10, area, strlen (area));

	draw_bar (Xdisplay, win, fore_gc, back_gc, y, &bar_cpu, i * 4);

	y += 20;

	/*
	 *	Discos
	 */
	sprintf (area, "HD/SD  %3d.00 %%", sasi_per);

	XDrawImageString (Xdisplay, win, fore_gc, x, y + 10, area, strlen (area));

	draw_bar (Xdisplay, win, fore_gc, back_gc, y, &bar_sasi, sasi_per * 4);

	y += 20;

	/*
	 *	USB
	 */
	sprintf (area, "USB    %3d.00 %%", usb_per);

	XDrawImageString (Xdisplay, win, fore_gc, x, y + 10, area, strlen (area));

	draw_bar (Xdisplay, win, fore_gc, back_gc, y, &bar_usb, usb_per * 4);

	y += 20;

	/*
	 *	Memória
	 */
	d = coreget ();

	sprintf (area, "MEM    %6.2f %%", d);

	XDrawImageString (Xdisplay, win, fore_gc, x, y + 10, area, strlen (area));

	if ((i = d * 4) == 0 && d != 0.0)
		i = 1;

	draw_bar (Xdisplay, win, fore_gc, back_gc, y, &bar_core, i);

	y += 20;

	/*
	 *	ETHERNET
	 */
	sprintf (area, "ETHER  %3d.00 %%", ed_per);

	XDrawImageString (Xdisplay, win, fore_gc, x, y + 10, area, strlen (area));

	draw_bar (Xdisplay, win, fore_gc, back_gc, y, &bar_ed, ed_per * 4);

	y += 20;

	/*
	 *	PPP entrada
	 */
	sprintf (area, "PPPIN  %3d.00 %%", ppp_in_per);

	XDrawImageString (Xdisplay, win, fore_gc, x, y + 10, area, strlen (area));

	draw_bar (Xdisplay, win, fore_gc, back_gc, y, &bar_ppp_in, ppp_in_per * 4);

	y += 20;

	/*
	 *	PPP saída
	 */
	sprintf (area, "PPPOUT %3d.00 %%", ppp_out_per);

	XDrawImageString (Xdisplay, win, fore_gc, x, y + 10, area, strlen (area));

	draw_bar (Xdisplay, win, fore_gc, back_gc, y, &bar_ppp_out, ppp_out_per * 4);

	y += 20;

	/*
	 *	Blocos cheios
	 */
	sprintf (area, "FULL   %6.2f %%", full_per);

	XDrawImageString (Xdisplay, win, fore_gc, x, y + 10, area, strlen (area));

	if ((i = full_per * 4) == 0 && full_per != 0.0)
		i = 1;

	draw_bar (Xdisplay, win, fore_gc, back_gc, y, &bar_full, i);

	y += 20;

	/*
	 *	Blocos sujos
	 */
	sprintf (area, "DIRTY  %6.2f %%", dirty_per);

	XDrawImageString (Xdisplay, win, fore_gc, x, y + 10, area, strlen (area));

	if ((i = dirty_per * 4) == 0 && dirty_per != 0.0)
		i = 1;

	draw_bar (Xdisplay, win, fore_gc, back_gc, y, &bar_dirty, i);

	y += 20;

	/*
	 *	Acerto do "cache" de blocos
	 */
	sprintf (area, "HIT    %6.2f %%", hit_per);

	XDrawImageString (Xdisplay, win, fore_gc, x, y + 10, area, strlen (area));

	if ((i = hit_per * 4) == 0 && hit_per != 0.0)
		i = 1;

	draw_bar (Xdisplay, win, fore_gc, back_gc, y, &bar_hit, i);

	y += 20;

	/*
	 *	Uso de INODEs
	 */
	sprintf (area, "INODE  %6.2f %%", inode_per);

	XDrawImageString (Xdisplay, win, fore_gc, x, y + 10, area, strlen (area));

	if ((i = inode_per * 4) == 0 && inode_per != 0.0)
		i = 1;

	draw_bar (Xdisplay, win, fore_gc, back_gc, y, &bar_inode, i);

	y += 20;

	/*
	 *	INODEs sujos
	 */
	sprintf (area, "IDIRTY %6.2f %%", inode_dirty_per);

	XDrawImageString (Xdisplay, win, fore_gc, x, y + 10, area, strlen (area));

	if ((i = inode_dirty_per * 4) == 0 && inode_dirty_per != 0.0)
		i = 1;

	draw_bar (Xdisplay, win, fore_gc, back_gc, y, &bar_inode_dirty, i);

	y += 20;

	/*
	 *	Uso de KFILES
	 */
	sprintf (area, "KFILE  %6.2f %%", kfile_per);

	XDrawImageString (Xdisplay, win, fore_gc, x, y + 10, area, strlen (area));

	if ((i = kfile_per * 4) == 0 && kfile_per != 0.0)
		i = 1;

	draw_bar (Xdisplay, win, fore_gc, back_gc, y, &bar_kfile, i);

   /***	y += 20; ***/

	/*
	 *	Epílogo
	 */
	XFlush (Xdisplay);

}	/* end draw_window */

/*
 ****************************************************************
 *	Desenha uma barra					*
 ****************************************************************
 */
void
draw_bar (Display *Xdisplay, Window win, GC fore_gc, GC back_gc, int y, int *old_width, int width)
{
	int		inc;

	if   ((inc = (width - *old_width)) == 0)
	{
		return;
	}
	elif (inc > 0)
	{
		XFillRectangle (Xdisplay, win, fore_gc, 108 + *old_width, y, inc, 10);
	}
	else	/* (inc < 0) */
	{
		XFillRectangle (Xdisplay, win, back_gc, 108 + *old_width + inc, y, -inc, 10);
	}

	*old_width = width;

}	/* end draw_bar */

/*
 ****************************************************************
 *	Obtem o Uso da CPU0 e HD				*
 ****************************************************************
 */
int
cpu0get (void)
{
	MON		mon;
	static char	first_time = 1;
	static int	old_ticks, old_stop, old_sasi, old_usb, old_ed,
			old_ppp_in, old_ppp_out,
			old_hit, old_total;
	int		ticks, stop, sasi, usb, ed, ppp_in, ppp_out;
	int		hit, total;
	BHM		*hm_p;
	static int	hit_interval;

	/*
	 *	Obtém os novos valores
	 */
	memmove (&mon, mon_ptr, sizeof (MON));

	/*
	 *	Se for a primeira vez, ...
	 */
	if (first_time)
	{
		first_time = 0;
		old_ticks = mon.y_ticks[0];
	}

	/*
	 *	Procedimento usual
	 */
	ticks	= mon.y_ticks[0] - old_ticks;
	stop	= mon.y_stop[0]  - old_stop;
	sasi	= mon.y_sasi     - old_sasi;
	usb	= mon.y_usb      - old_usb;
	ed	= mon.y_ed       - old_ed;
	ppp_in	= mon.y_ppp_in   - old_ppp_in;
	ppp_out	= mon.y_ppp_out  - old_ppp_out;

	old_ticks   = mon.y_ticks[0];
	old_stop    = mon.y_stop[0];
	old_sasi    = mon.y_sasi;
	old_usb     = mon.y_usb;
	old_ed      = mon.y_ed;
	old_ppp_in  = mon.y_ppp_in;
	old_ppp_out = mon.y_ppp_out;

	if (ticks == 0)
		return (sasi_per = 0);

	sasi_per		= PERCENT (sasi, ticks);
	usb_per			= PERCENT (usb, ticks);
	ed_per			= PERCENT (ed, ticks);
	ppp_in_per		= PERCENT (ppp_in, ticks);
	ppp_out_per		= PERCENT (ppp_out, ticks);
	full_per		= (double)mon.y_block_full_cnt   * 100.0 / scb.y_nbuf;
	dirty_per		= (double)mon.y_block_dirty_cnt  * 100.0 / scb.y_nbuf;
	inode_per		= (double)mon.y_inode_in_use_cnt * 100.0 / scb.y_ninode;
	inode_dirty_per		= (double)mon.y_inode_dirty_cnt  * 100.0 / scb.y_ninode;
	kfile_per		= (double)mon.y_kfile_in_use_cnt * 100.0 / scb.y_nkfile;

	/*
	 *	Faz o cálculo do "hit/total"
	 */
	if (hit_interval++ >= HIT_INTERVAL)
	{
		hit_interval = 0;

		hm_p = block_vector_ptr;

		if (++hm_p >= &block_vector[N_BHM])
			hm_p = &block_vector[0];

		sum_hit   -= hm_p->l_hit;
		sum_total -= hm_p->l_total;

		block_vector_ptr = hm_p;

		hit   = mon.y_block_lru_hit   - old_hit;
		total = mon.y_block_lru_total - old_total;

		old_hit   = mon.y_block_lru_hit;
		old_total = mon.y_block_lru_total;

		hm_p->l_hit   = hit;
		hm_p->l_total = total;

		sum_hit   += hit,
		sum_total += total;

		if (sum_total) 
			hit_per = (double)sum_hit * 100.0 / sum_total;
		else
			hit_per = 0.0;
	}

	return (PERCENT (ticks - stop, ticks));

}	/* end cpuget */

/*
 ****************************************************************
 *	Obtém o uso da memória					*
 ****************************************************************
 */
double
coreget (void)
{
	const MAP	*head, *mp;
	pg_t		unused = 0;

	head = core_ptr; /* A cabeça da memória é a primeira entrada */

	for
	(	mp = (MAP *)((int)head->a_forw + CORE_DIFF);
		mp != head;
		mp = (MAP *)((int)mp->a_forw + CORE_DIFF)
	)
	{
		unused += mp->a_size;
	}

	if (map_total == 0)
		return (0);

	return ((double)(map_total - unused) * 100.0 / map_total);

}	/* end coreget */

/*
 ****************************************************************
 *	Desenha as 4 sombras					*
 ****************************************************************
 */
void
draw_shadow (Display *Xdisplay, Window win, GC topShadow, GC botShadow, int win_x, int win_y, int width, int height)
{
	int		shadow_width;
	int		x, y, w, h;

	w = width + (win_x - 1); h = height + (win_y - 1);
	x = win_x; y = win_y;

	for (shadow_width = SHADOW; shadow_width > 0; shadow_width--, x++, y++, w--, h--)
	{
		XDrawLine (Xdisplay, win, topShadow, x, y, w, y);
		XDrawLine (Xdisplay, win, topShadow, x, y, x, h);
	}

	w = width + (win_x - 1); h = height + (win_y - 1);
	x = win_x + 1; y = win_y + 1;

	for (shadow_width = SHADOW; shadow_width > 0; shadow_width--, x++, y++, w--, h--)
	{
		XDrawLine (Xdisplay, win, botShadow, w, h, w, y);
		XDrawLine (Xdisplay, win, botShadow, w, h, x, h);
	}

}	/* end draw_shadow */

/*
 ****************************************************************
 *	Analisa uma cor dada					*
 ****************************************************************
 */
ulong
get_window_color (Display *Xdisplay, const char *color, ulong def)
{
	XColor			xcol;

	if (color == NOSTR || *color == '\0')
		return (def);

	if
	(	!XParseColor (Xdisplay, Xcmap, color, &xcol) ||
		!XAllocColor (Xdisplay, Xcmap, &xcol)
	)
	{
		error ("Não consegui alocar a cor \"%s\"", color);
		return (def);
	}

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
	Pixel			color_pixel;
	int			value = 0;

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
 *	Analisa a geometria					*
 ****************************************************************
 */
void
geometry_to_size_hints (Display *Xdisplay, const char *geom, XSizeHints *hp)
{
	int		x, y, flags, scr;
	unsigned int	width, height;

	if (geom == NOSTR)
		return;

	scr = DefaultScreen (Xdisplay);

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
			x += DisplayWidth (Xdisplay, scr) - hp->width;
			hp->win_gravity = NorthEastGravity;
		}

		hp->x = x;
		hp->flags |= USPosition;
	}

	if (flags & YValue)
	{
		if (flags & YNegative)
		{
			y += DisplayHeight (Xdisplay, scr) - hp->height;
			hp->win_gravity = hp->win_gravity == NorthEastGravity ?
				SouthEastGravity : SouthWestGravity;
		}

		hp->y = y;
		hp->flags |= USPosition;
	}

}	/* end geometry_to_size_hints */

/*
 ****************************************************************
 *	Imprime uma mensagem de erro (cliente)			*
 ****************************************************************
 */
void
error (const char *format, ...)
{
	va_list		args;
	const char	*fmt = format;
	char		err, abrt;
	int		save_errno = errno;

	va_start (args, format);

	if (fmt == NOSTR)
		fmt = "$*Erro de entrada/saída de TCP";

	if ((abrt = *fmt) == '$')
		fmt++;

	if ((err = *fmt) == '*')
		fmt++;

	fprintf  (stderr, "%s: ", pgname);
	vfprintf (stderr, fmt, args);

	if (err == '*' && save_errno != 0)
		fprintf (stderr, " (%s)", strerror (save_errno));

	putc ('\n', stderr);

	fflush (stderr);

	if (abrt == '$')
		exit (1);

	va_end (args);

}	/* end error */

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
		"%s - desenha um mapa de uso de alguns recursos do sistema\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-v] [-d <display>] [-D <desktop>] [-g <geometria>] [-f <frente>] [-b <fundo>]\n",
		pgname, pgversion, pgname
	);
	fprintf
	(	stderr,
		"\nOpções:"
		"\t-v: Verboso\n"
		"\t-d: Conecta-se ao servidor <display>\n"
		"\t-D: Inicia no Desktop <desktop>\n"
		"\t-g: Define a <geometria>\n"
		"\t-f: Define a cor de <frente>\n"
		"\t-b: Define a cor de <fundo>\n"
	);

	exit (2);

}	/* end help */
