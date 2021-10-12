/*
 ****************************************************************
 *								*
 *			xmandel.c				*
 *								*
 *	Desenha as imagens fractais de Mandelbrot		*
 *								*
 *	Versão	4.5.0, de 27.12.03				*
 *		4.5.0, de 23.03.04				*
 *								*
 *	Módulo: xmandel						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2004 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <fcntl.h>
#include <stat.h>

#include <X11/Xatom.h>
#include <X11/Intrinsic.h>
#include <X11/cursorfont.h>

#include "../h/xmandel.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char		pgversion[] = "Versão: 4.5.0, de 23.03.04";

#define	MSGLINES	2

entry int		vflag;		/* Verbose */

entry int		depth;

entry XStandardColormap	*StandardColormap;

entry Display		*display;

entry Window		root_win;

entry int		present_root_width, present_root_height;

/*
 ******	Tamanhos ************************************************
 */
#define	STD_MANDEL_WIDTH	640
#define	STD_MANDEL_HEIGHT	480

#define	MIN_MANDEL_WIDTH	100
#define	MIN_MANDEL_HEIGHT	100

entry int	WINDOW_WIDTH  = STD_MANDEL_WIDTH;
entry int	WINDOW_HEIGHT = STD_MANDEL_HEIGHT;

/*
 ****** Cores variáveis *****************************************
 */
#define	default_fore_color_str	"black"
#define	default_back_color_str	"gray"
#define	default_font_str	"9x15"

/*
 ****** Cores fixas *********************************************
 */
#define BORDER_COLOR		0	/*** BlackPixel (display, screen) ***/

#define	BLACK_COLOR_NM		"black"
#define	RED_DIR_COLOR_NM	"red1"		/* ou "orangered" */
#define	GREEN_COLOR_NM		"green"
#define	BLUE_COLOR_NM		"blue1"		/* ou "dodgerblue" */

/*
 ******	Valores obtidos das cores *******************************
 */
entry TRIPLEGC		fore_triple, back_triple, black_triple;

entry TRIPLEGC		green_triple, red_dir_triple;

/*
 ******	Geometria ***********************************************
 */
entry XSizeHints	szhint =
{
	0,			/* Bits valid */
	0,	0,		/*		x, y */
	0,	0,		/*		width, height */
	0,	0,		/* PMinSize	min_width, min_height */
	0,	0,		/*		max_width, max_height */
	0,	0,		/* PResizeInc	width_inc, height_inc */
	{1,	1},		/*		min_aspect */
	{0,	0},		/*		max_aspect ratio */
	0,	0,		/* PBaseSize	base_width, base_height */
	NorthWestGravity	/* PWinGravity	gravity */
};

/*
 ******	Estrutura de um SUBMENU *********************************
 */
typedef struct
{
	const char		*m_text;	/* Texto a escrever */
	int			m_value;	/* Valor */

}	SUBMENU_VEC;

#define NOSUBMENU	(SUBMENU *)NULL

typedef struct
{
	WINDATA			windata;
	const SUBMENU_VEC	*vec;

	int			mapped;
	int			line_offset;
	int			longest_text_len;
	int			text_n;
	int			*value_in_use;
	int			highlight_line;

}	SUBMENU;

/*
 ******	Dados da janela de MENU *********************************
 */
void		option_function (int value);
void		quit_function (int value);

typedef struct
{
	const char	*m_text;		/* Texto a escrever */
	void		(*m_func) (int);	/* Função a chamar */
	WINDATA		m_win;			/* Dados da janela */

}	MENUTB;

MENUTB		menutb[] =
{
	{ "Opções",			option_function },	/* 0 */
	{ "Imagem anterior",		return_last_image },	/* 1 */
	{ "Gera a imagem",		compute },		/* 2 */
	{ "Gera o arquivo",		generate_gif_file },	/* 3 */
	{ "Fim",			quit_function }		/* 4 */
};

#define	NMENU	(sizeof (menutb) / sizeof (MENUTB))

/*
 ******	Dados da janela de TAMANHOs *****************************
 */
const SUBMENU_VEC	msize_vec[] =
{
	"640x480",	M640x480,
	"800x600",	M800x600,
	"1024x768",	M1024x768,
	"1280x1024",	M1280x1024,
	"1600x1200",	M1600x1200,
	"3200x2048",	M3200x2048,
};

entry int		msize_in_use = M640x480;	/* Tamanho em uso */

entry SUBMENU	msize =
{
	{ NULL },					/* Windata */
	msize_vec,					/* Text vector */

	0,						/* mapped */
	0,						/* line_offset */
	17,						/* strlen ("Todos os arquivos") */
	(sizeof (msize_vec) / sizeof (SUBMENU_VEC)),	/* text_n */
	&msize_in_use,					/* size_in_use */
	-1						/* highlight_line */
};

/*
 ******	Dados da janela do INTERVALO a pula no mapa de cores ****
 */
const SUBMENU_VEC	cshift_vec[] =
{
	" 1",		0,
	" 2",		1,
	" 4",		2,
	" 8",		3,
	"16",		4,
	"32",		5
};

entry int		color_index_shift = 2;		/* Quantas entradas pula no mapa de cores */

entry SUBMENU	cshift =
{
	{ NULL },					/* Windata */
	cshift_vec,					/* Text vector */

	0,						/* mapped */
	1,						/* line_offset */
	2,						/* strlen ("32") */
	(sizeof (cshift_vec) / sizeof (SUBMENU_VEC)),	/* text_n */
	&color_index_shift,				/* value_in_use */
	-1						/* highlight_line */
};

/*
 ******	Dados da janela do BRILHO das cores *********************
 */
const SUBMENU_VEC	cbrightness_vec[] =
{
	"255, 255",	(255 << 8) | 255,
	"255, 192",	(255 << 8) | 192,
	"255, 128",	(255 << 8) | 128,
	"255,  64",	(255 << 8) | 64,
	"255,   0",	(255 << 8) | 0,

	"192, 255",	(192 << 8) | 255,
	"192, 192",	(192 << 8) | 192,
	"192, 128",	(192 << 8) | 128,
	"192,  64",	(192 << 8) | 64,
	"192,   0",	(192 << 8) | 0,

	"128, 255",	(128 << 8) | 255,
	"128, 192",	(128 << 8) | 192,
	"128, 128",	(128 << 8) | 128,
	"128,  64",	(128 << 8) | 64,
	"128,   0",	(128 << 8) | 0,

	"64,  255",	(64 << 8)  | 255,
	"64,  192",	(64 << 8)  | 192,
	"64,  128",	(64 << 8)  | 128,
	"64,   64",	(64 << 8)  | 64,
	"64,    0",	(64 << 8)  | 0,
};

entry int		color_brightness = (255 << 8) | 255;	/* Briho em uso */

entry SUBMENU	cbrightness =
{
	{ NULL },					/* Windata */
	cbrightness_vec,				/* Text vector */

	0,						/* mapped */
	2,						/* line_offset */
	8,						/* strlen ("255, 255") */
	(sizeof (cbrightness_vec) / sizeof (SUBMENU_VEC)), /* text_n */
	&color_brightness,				/* value_in_use */
	-1						/* highlight_line */
};

/*
 ******	Dados da janela da ordem das CORES **********************
 */
const SUBMENU_VEC	corder_vec[] =
{
	"RGB",	RGB,
	"GBR",	GBR,
	"BRG",	BRG,
	"BGR",	BGR,
	"GRB",	GRB,
	"RBG",	RBG
};

entry int		color_order = RGB;		/* Ordem das cores */

entry SUBMENU	corder =
{
	{ NULL },					/* Windata */
	corder_vec,					/* Text vector */

	0,						/* mapped */
	3,						/* line_offset */
	3,						/* strlen ("RGB") */
	(sizeof (corder_vec) / sizeof (SUBMENU_VEC)),	/* text_n */
	&color_order,					/* value_in_use */
	-1						/* highlight_line */
};

/*
 ******	Dados da janela de RECICLAR a tabela LZW ****************
 */
const SUBMENU_VEC	lzw_vec[] =
{
	"Portátil",	LZW_PORTABLE,
	"Compacta",	LZW_COMPACT
};

entry int		lzw_mode;	/* Esvazia a tabela sempre que ela encher */

entry SUBMENU	lzw =
{
	{ NULL },					/* Windata */
	lzw_vec,					/* Text vector */

	0,						/* mapped */
	7,						/* line_offset */
	8,						/* strlen ("Compacto") */
	(sizeof (lzw_vec) / sizeof (SUBMENU_VEC)),	/* text_n */
	&lzw_mode,					/* value_in_use */
	-1						/* highlight_line */
};

/*
 ******	Dados da janela da VERSÃO *******************************
 */
entry WINDATA		version;

entry int		version_mapped;

#define	version_line_offset	8			/* Das opções, acima */
#define	version_longest_text	pgversion
#define	version_text_n		1

/*
 ******	Dados da janela das OPÇÕES ******************************
 */
#define NOFUNC	(void (*) (int))NULL

entry WINDATA		option;

typedef struct
{
	const char	*m_text;		/* Texto a escrever */
	void		(*f_func) (int);	/* Função a chamar */
	WINDATA		*m_windata;		/* A Janela do SUBMENU */
	SUBMENU		*m_submenu;		/* A Tabela do SUBMENU */

}	OPTIONVEC;

const OPTIONVEC	optionvec[] =
{
    "Tamanho da Imagem: ",		NOFUNC,			&msize.windata,		&msize, 	/* 0 */
    "Gradiente das cores:",		NOFUNC,			&cshift.windata,	&cshift,	/* 1 */
    "Brilho das cores:",		NOFUNC,			&cbrightness.windata,	&cbrightness,	/* 2 */
    "Ordem das cores:",			NOFUNC,			&corder.windata,	&corder,	/* 3 */
    "Desenha o vetor de cores",		draw_color_map,		NOWINDATA,		NOSUBMENU,	/* 4 */
    "Gera a Imagem",			compute,		NOWINDATA,		NOSUBMENU,	/* 5 */
    "Gera o Arquivo",			generate_gif_file,	NOWINDATA,		NOSUBMENU,	/* 6 */
    "Geração LZW:",			NOFUNC,			&lzw.windata,		&lzw,		/* 7 */
    "Versão: ",				NOFUNC,			&version,		NOSUBMENU,	/* 8 */
};

#define	option_longest_text	optionvec[4].m_text
#define	option_text_n		(sizeof (optionvec) / sizeof (OPTIONVEC))

entry int		option_highlight_line = -1;

/*
 ******	Dados da janela de MENSAGENS ****************************
 */
entry WINDATA		msg;

/*
 ******	Dados da janela da imagem MANDELBROT ********************
 */
entry WINDATA		mandel;

/*
 ****** Protótipos de funções ***********************************
 */
void		help (void);
void		set_font_sizes (int width, int height, int descent, ...);
int		set_shadow_colors (Display *, Window, TRIPLEGC *, const char *, Pixel);
void		geometry_to_size_hints (const char *geo_str, XSizeHints *hp);
void		event_loop (int main_event_loop);
void		draw_option_win (void);
void		draw_submenu_win (const SUBMENU *smp);
void		draw_version_win (void);
void		draw_menu_win (const MENUTB *mp);
void		unmap_whole_menu (void);
void		Shadow_up (const WINDATA *);
void		Shadow_down (const WINDATA *);
void		do_option_button_press (int y);
void		do_submenu_button_press (SUBMENU *smp, int y);
int		get_pointer_y (Window win);
void		get_pointer_x_y (Window win, int *x, int *y);
void		motion_submenu_window (SUBMENU *smp, int y);
void		motion_option_window (int y);
void		leave_option_window (void);
void		msize_change_mandel_size (const SUBMENU *smp);

/*
 ****************************************************************
 *	Desenha as imagens fractais de Mandelbrot		*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	static XStandardColormap	stdcmap;
	WINDATA				*wp;
	int				opt;
	const char			*geo_str = NOSTR;
	int				save_argc = argc;
	const char			**save_argv = argv;
	const char			*display_str = NOSTR;
	const char			*font_str, *user_font_str = NOSTR;
	const char			*fore_color_str, *user_fore_color_str = NOSTR;
	const char			*back_color_str, *user_back_color_str = NOSTR;
	XFontStruct			*text_fip;
	int				screen;
	int				font_width, font_height, font_descent;
	Window				root, win;
	XClassHint			classhint;
	XWMHints			wmhint;
	XGCValues			gcv;
	GC				menu_text_gc, file_text_gc;
	const char			*str;
	MENUTB				*mp;
	Cursor				hand_cursor;
	Atom				map;

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
	elif ((str = XGetDefault (display, "XMandel", "foreground")) != NOSTR)
		fore_color_str = str;
	else
		fore_color_str = default_fore_color_str;

	if   (user_back_color_str != NOSTR)
		back_color_str = user_back_color_str;
	elif ((str = XGetDefault (display, "XMandel", "background")) != NOSTR)
		back_color_str = str;
	else
		back_color_str = default_back_color_str;

	if   (user_font_str != NOSTR)
		font_str = user_font_str;
	elif ((str = XGetDefault (display, "XMandel", "menufont")) != NOSTR)
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
	set_shadow_colors (display, root, &black_triple,   BLACK_COLOR_NM,   0x000000);

	set_shadow_colors (display, root, &fore_triple,    fore_color_str,   0x000000);
	set_shadow_colors (display, root, &back_triple,    back_color_str,   0xBEBEBE);

	set_shadow_colors (display, root, &green_triple,   GREEN_COLOR_NM,   0x00CC00);
	set_shadow_colors (display, root, &red_dir_triple, RED_DIR_COLOR_NM, 0xCC0000);

	gcv.font	= text_fip->fid;
	gcv.foreground  = fore_triple.main_pixel;
	gcv.background  = back_triple.main_pixel;

	file_text_gc	= XCreateGC (display, root, GCForeground|GCBackground|GCFont, &gcv);

	/*
	 ****** Atribui os tamanhos das fontes nas janelas ******
	 */
	for (mp = &menutb[0]; mp < &menutb[NMENU]; mp++)
	{
		mp->m_win.font_width	= font_width;
		mp->m_win.font_height	= font_height;
		mp->m_win.font_descent	= font_descent;
	}

	set_font_sizes
	(	font_width, font_height, font_descent,
		&option, &msize.windata, &cshift.windata, &cbrightness.windata,
		&corder.windata, &lzw.windata, &version, &msg, &mandel, NOWINDATA
	);

	/*
	 ****** Cria a janela PRINCIPAL	*************************
	 */
	geometry_to_size_hints (geo_str, &szhint);

	set_window_sizes (0, 0);

        win = XCreateSimpleWindow
	(	display, root,
		0, 0,
		szhint.base_width, szhint.base_height,
		0, BORDER_COLOR,
		BlackPixel (display, screen)
	);

	root_win = win;

	XStoreName (display, win, "Xmandel");

	XSelectInput (display, win, ExposureMask|StructureNotifyMask);

	classhint.res_name  = (char *)pgname;
	classhint.res_class = "XMandel";

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
	 ****** Cria as janelas do MENU *************************
	 */
	gcv.font	= text_fip->fid;
	gcv.foreground  = fore_triple.main_pixel;
	gcv.background  = back_triple.main_pixel;
	menu_text_gc	= XCreateGC (display, win, GCForeground|GCBackground|GCFont, &gcv);

	for (mp = &menutb[0]; mp < &menutb[NMENU]; mp++)
	{
		memmove (&mp->m_win.triple, &back_triple, sizeof (TRIPLEGC));
		mp->m_win.text_gc	= menu_text_gc;

		mp->m_win.win = XCreateSimpleWindow
		(
			display, win,
			mp->m_win.x, mp->m_win.y,
			mp->m_win.width, mp->m_win.height,
			0, BORDER_COLOR,
			mp->m_win.triple.main_pixel
		);

		XSelectInput
		(	display, mp->m_win.win,
			(ExposureMask|ButtonPressMask|EnterWindowMask|LeaveWindowMask)
		);

		XMapWindow (display, mp->m_win.win);
	}

	/*
	 ****** Prepara a janela das OPÇÕES *********************
	 */
	memmove (&option.triple, &back_triple, sizeof (TRIPLEGC));
	option.text_gc      = menu_text_gc;

        option.win = XCreateSimpleWindow
	(	display, win,
		option.x, option.y,
		option.width, option.height,
		0, BORDER_COLOR,
		option.triple.main_pixel
	);

	XSelectInput
	(	display, option.win,
		(ExposureMask|EnterWindowMask|LeaveWindowMask|
		PointerMotionMask|PointerMotionHintMask|ButtonPressMask)
	);

	hand_cursor = XCreateFontCursor (display, XC_hand2);
	XDefineCursor (display, option.win, hand_cursor);

   /***	XMapWindow (display, option.win); ***/

	/*
	 ******	Prepara a janela do tamanho da imagem ***********
	 */
	wp = &msize.windata;

	memmove (&wp->triple, &back_triple, sizeof (TRIPLEGC));
	wp->text_gc      = menu_text_gc;

        wp->win = XCreateSimpleWindow
	(	display, win,
		wp->x, wp->y,
		wp->width, wp->height,
		0, BORDER_COLOR,
		option.triple.main_pixel
	);

	XSelectInput
	(	display, wp->win,
		(ExposureMask|EnterWindowMask|LeaveWindowMask|
		PointerMotionMask|PointerMotionHintMask|ButtonPressMask)
	);

	XDefineCursor (display, wp->win, hand_cursor);

   /***	XMapWindow (display, wp->win); ***/

	/*
	 ******	Prepara a janela do INTERVALO a pular ***********
	 */
	wp = &cshift.windata;

	memmove (&wp->triple, &back_triple, sizeof (TRIPLEGC));
	wp->text_gc      = menu_text_gc;

        wp->win = XCreateSimpleWindow
	(	display, win,
		wp->x, wp->y,
		wp->width, wp->height,
		0, BORDER_COLOR,
		option.triple.main_pixel
	);

	XSelectInput
	(	display, wp->win,
		(ExposureMask|EnterWindowMask|LeaveWindowMask|
		PointerMotionMask|PointerMotionHintMask|ButtonPressMask)
	);

	XDefineCursor (display, wp->win, hand_cursor);

   /***	XMapWindow (display, wp->win); ***/

	/*
	 ******	Prepara a janela do brilho das cores ************
	 */
	wp = &cbrightness.windata;

	memmove (&wp->triple, &back_triple, sizeof (TRIPLEGC));
	wp->text_gc      = menu_text_gc;

        wp->win = XCreateSimpleWindow
	(	display, win,
		wp->x, wp->y,
		wp->width, wp->height,
		0, BORDER_COLOR,
		option.triple.main_pixel
	);

	XSelectInput
	(	display, wp->win,
		(ExposureMask|EnterWindowMask|LeaveWindowMask|
		PointerMotionMask|PointerMotionHintMask|ButtonPressMask)
	);

	XDefineCursor (display, wp->win, hand_cursor);

   /***	XMapWindow (display, wp->win); ***/

	/*
	 ******	Prepara a janela da ordem das cores *************
	 */
	wp = &corder.windata;

	memmove (&wp->triple, &back_triple, sizeof (TRIPLEGC));
	wp->text_gc      = menu_text_gc;

        wp->win = XCreateSimpleWindow
	(	display, win,
		wp->x, wp->y,
		wp->width, wp->height,
		0, BORDER_COLOR,
		option.triple.main_pixel
	);

	XSelectInput
	(	display, wp->win,
		(ExposureMask|EnterWindowMask|LeaveWindowMask|
		PointerMotionMask|PointerMotionHintMask|ButtonPressMask)
	);

	XDefineCursor (display, wp->win, hand_cursor);

   /***	XMapWindow (display, wp->win); ***/

	/*
	 ******	Prepara a janela da geração LZW *****************
	 */
	wp = &lzw.windata;

	memmove (&wp->triple, &back_triple, sizeof (TRIPLEGC));
	wp->text_gc      = menu_text_gc;

        wp->win = XCreateSimpleWindow
	(	display, win,
		wp->x, wp->y,
		wp->width, wp->height,
		0, BORDER_COLOR,
		option.triple.main_pixel
	);

	XSelectInput
	(	display, wp->win,
		(ExposureMask|EnterWindowMask|LeaveWindowMask|
		PointerMotionMask|PointerMotionHintMask|ButtonPressMask)
	);

	XDefineCursor (display, wp->win, hand_cursor);

   /***	XMapWindow (display, wp->win); ***/

	/*
	 ****** Prepara a janela da VERSÃO **********************
	 */
	memmove (&version.triple, &back_triple, sizeof (TRIPLEGC));
	version.text_gc      = menu_text_gc;

        version.win = XCreateSimpleWindow
	(	display, win,
		version.x, version.y,
		version.width, version.height,
		0, BORDER_COLOR,
		option.triple.main_pixel
	);

	XSelectInput
	(	display, version.win,
		(ExposureMask|EnterWindowMask|LeaveWindowMask|
		PointerMotionMask|PointerMotionHintMask|ButtonPressMask)
	);

   /***	XMapWindow (display, version.win); ***/

	/*
	 ****** Cria a janela de MENSAGENS **********************
	 */
	wp = &msg;

	memmove (&wp->triple, &back_triple, sizeof (TRIPLEGC));
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
		ExposureMask
	);

	XMapWindow (display, wp->win);

	/*
	 ****** Cria a janela da imagem MANDEL ******************
	 */
	wp = &mandel;

	memmove (&wp->triple, &back_triple, sizeof (TRIPLEGC));
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

#if (0)	/*******************************************************/
	/*
	 ****** Prepara a cor para apagar linhas ****************
	 */
	gcv.font	= file_text_fip->fid;
	gcv.foreground  = back_triple.main_pixel;
	gcv.background  = fore_triple.main_pixel;
	reverse_text_gc = XCreateGC (display, win, GCForeground|GCBackground|GCFont, &gcv);
#endif	/*******************************************************/

	/*
	 ******	Entra na malha de eventos *******************************
	 */
	event_loop (1);

	/* Não retorna */

	return (0);

}	/* end xmandel */

/*
 ****************************************************************
 *	Malha de eventos					*
 ****************************************************************
 */
void
event_loop (int main_event_loop)
{
	XEvent		event;
	const MENUTB	*mp;

	for (EVER)
	{
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

			if   (event.xany.window == option.win)
			{
				draw_option_win ();
			}
			elif (event.xany.window == msize.windata.win)
			{
				draw_submenu_win (&msize);
			}
			elif (event.xany.window == cshift.windata.win)
			{
				draw_submenu_win (&cshift);
			}
			elif (event.xany.window == cbrightness.windata.win)
			{
				draw_submenu_win (&cbrightness);
			}
			elif (event.xany.window == corder.windata.win)
			{
				draw_submenu_win (&corder);
			}
			elif (event.xany.window == lzw.windata.win)
			{
				draw_submenu_win (&lzw);
			}
			elif (event.xany.window == version.win)
			{
				draw_version_win ();
			}
			elif (event.xany.window == msg.win)
			{
				draw_msg_win ();
			}
			elif (event.xany.window == mandel.win)
			{
				draw_mandel_win ();
			}
			else for (mp = &menutb[0]; mp < &menutb[NMENU]; mp++)
			{
				if (mp->m_win.win == event.xany.window)
				{
					draw_menu_win (mp); break;
				}
			}

			break;

		    case EnterNotify:
			if   (event.xany.window == mandel.win)
			{
				unmap_whole_menu ();
			}
			else for (mp = &menutb[0]; mp < &menutb[NMENU]; mp++)
			{
				if (mp->m_win.win == event.xany.window)
				{
					Shadow_up (&mp->m_win);

					if (mp->m_win.win != option.win)
						unmap_whole_menu ();

					break;
				}
			}

			break;

		    case LeaveNotify:
			if   (event.xany.window == option.win)
			{
				if
				(	msize.mapped || cshift.mapped || cbrightness.mapped ||
					corder.mapped || version_mapped
				)
					break;

			   	XUnmapWindow (display, option.win);

				option_highlight_line = -1;
			}
			elif (event.xany.window == msize.windata.win)
			{
			   	XUnmapWindow (display, option.win);
			   	XUnmapWindow (display, msize.windata.win);

				msize.mapped = 0;
				msize.highlight_line = -1;
			}
			elif (event.xany.window == cshift.windata.win)
			{
			   	XUnmapWindow (display, option.win);
			   	XUnmapWindow (display, cshift.windata.win);

				cshift.mapped = 0;
				cshift.highlight_line = -1;
			}
			elif (event.xany.window == cbrightness.windata.win)
			{
			   	XUnmapWindow (display, option.win);
			   	XUnmapWindow (display, cbrightness.windata.win);

				cbrightness.mapped = 0;
				cbrightness.highlight_line = -1;
			}
			elif (event.xany.window == corder.windata.win)
			{
			   	XUnmapWindow (display, option.win);
			   	XUnmapWindow (display, corder.windata.win);

				corder.mapped = 0;
				corder.highlight_line = -1;
			}
			elif (event.xany.window == lzw.windata.win)
			{
			   	XUnmapWindow (display, option.win);
			   	XUnmapWindow (display, lzw.windata.win);

				lzw.mapped = 0;
				lzw.highlight_line = -1;
			}
			elif (event.xany.window == version.win)
			{
			   	XUnmapWindow (display, option.win);
			   	XUnmapWindow (display, version.win);

				version_mapped = 0;
			}
			elif (event.xany.window == mandel.win)
			{
				leave_mandel_window ();
			}
			else for (mp = &menutb[0]; mp < &menutb[NMENU]; mp++)
			{
				if (mp->m_win.win == event.xany.window)
				{
					Shadow_down (&mp->m_win); break;
				}
			}

			break;

		    case ButtonPress:
			if (main_event_loop == 0)
				return;

			if   (event.xany.window == option.win)
			{
			   	do_option_button_press (event.xbutton.y);
			}
			elif (event.xany.window == msize.windata.win)
			{
			   	do_submenu_button_press (&msize, event.xbutton.y);
			}
			elif (event.xany.window == cshift.windata.win)
			{
			   	do_submenu_button_press (&cshift, event.xbutton.y);
			}
			elif (event.xany.window == cbrightness.windata.win)
			{
			   	do_submenu_button_press (&cbrightness, event.xbutton.y);
			}
			elif (event.xany.window == corder.windata.win)
			{
			   	do_submenu_button_press (&corder, event.xbutton.y);
			}
			elif (event.xany.window == lzw.windata.win)
			{
			   	do_submenu_button_press (&lzw, event.xbutton.y);
			}
			elif (event.xany.window == mandel.win)
			{
			   	do_mandel_button_press (event.xbutton.button, event.xbutton.x, event.xbutton.y);
			}
			else for (mp = &menutb[0]; mp < &menutb[NMENU]; mp++)
			{
				if (mp->m_win.win == event.xany.window)
					{ (*mp->m_func) (0); break; }
			}

			break;

		    case ButtonRelease:
			if (main_event_loop == 0)
				return;

			if (event.xany.window == mandel.win)
			{
			   	do_mandel_button_release (event.xbutton.button, event.xbutton.x, event.xbutton.y);
			}

			break;

		    case MotionNotify:
			if   (event.xany.window == option.win)
			{
				motion_option_window (get_pointer_y (option.win));
			}
			elif (event.xany.window == msize.windata.win)
			{
				motion_submenu_window (&msize, get_pointer_y (msize.windata.win));
			}
			elif (event.xany.window == cshift.windata.win)
			{
				motion_submenu_window (&cshift, get_pointer_y (cshift.windata.win));
			}
			elif (event.xany.window == cbrightness.windata.win)
			{
				motion_submenu_window (&cbrightness, get_pointer_y (cbrightness.windata.win));
			}
			elif (event.xany.window == corder.windata.win)
			{
				motion_submenu_window (&corder, get_pointer_y (corder.windata.win));
			}
			elif (event.xany.window == lzw.windata.win)
			{
				motion_submenu_window (&lzw, get_pointer_y (lzw.windata.win));
			}
			elif (event.xany.window == mandel.win)
			{
				int		x, y;

				get_pointer_x_y (mandel.win, &x, &y);

				motion_mandel_window (x, y);
			}

			break;

#if (0)	/*******************************************************/
		    case KeyPress:
			if (main_event_loop == 0)
				return;

			do_key_press (event);

			break;
#endif	/*******************************************************/

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

#if (0)	/*******************************************************/
/*
 ****************************************************************
 *	Obtém a posição do ponteiro				*
 ****************************************************************
 */
int
get_pointer_x (Window win)
{
	XEvent		event;
	Window		root, child;
	int		root_x, root_y;
	unsigned int	mask;

	while (XCheckTypedWindowEvent (display, win, MotionNotify, &event))
		/* vazio */;

	XQueryPointer
	(	display, win, &root, &child,
		&root_x, &root_y,
		&event.xbutton.x, &event.xbutton.y,
		&mask
	);

	return (event.xbutton.x);

}	/* end get_pointer_x */
#endif	/*******************************************************/

/*
 ****************************************************************
 *	Obtém a posição do ponteiro				*
 ****************************************************************
 */
int
get_pointer_y (Window win)
{
	XEvent		event;
	Window		root, child;
	int		root_x, root_y;
	unsigned int	mask;

	while (XCheckTypedWindowEvent (display, win, MotionNotify, &event))
		/* vazio */;

	XQueryPointer
	(	display, win, &root, &child,
		&root_x, &root_y,
		&event.xbutton.x, &event.xbutton.y,
		&mask
	);

	return (event.xbutton.y);

}	/* end get_pointer_y */

/*
 ****************************************************************
 *	Obtém as posições do ponteiro				*
 ****************************************************************
 */
void
get_pointer_x_y (Window win, int *x, int *y)
{
	XEvent		event;
	Window		root, child;
	int		root_x, root_y;
	unsigned int	mask;

	while (XCheckTypedWindowEvent (display, win, MotionNotify, &event))
		/* vazio */;

	XQueryPointer
	(	display, win, &root, &child,
		&root_x, &root_y,
		&event.xbutton.x, &event.xbutton.y,
		&mask
	);

	*x = event.xbutton.x;
	*y = event.xbutton.y;

}	/* end get_pointer_x_y */

/*
 ****************************************************************
 *	Desenha uma caixa de MENU				*
 ****************************************************************
 */
void
draw_menu_win (const MENUTB *mp)
{
	XDrawString
	(	display, mp->m_win.win,
		mp->m_win.text_gc,
		MENU_BORDER,
		MENU_BORDER + mp->m_win.font_height - mp->m_win.font_descent,
		mp->m_text, strlen (mp->m_text)
	);

}	/* end draw_menu_win */

/*
 ****************************************************************
 *	Desmapeia o MENU e todos filhos				*
 ****************************************************************
 */
void
unmap_whole_menu (void)
{
   	XUnmapWindow (display, option.win);
   	XUnmapWindow (display, msize.windata.win);
   	XUnmapWindow (display, cshift.windata.win);
   	XUnmapWindow (display, cbrightness.windata.win);
   	XUnmapWindow (display, corder.windata.win);
   	XUnmapWindow (display, lzw.windata.win);
   	XUnmapWindow (display, version.win);

	msize.mapped = 0;
	msize.highlight_line = -1;

	cshift.mapped = 0;
	cshift.highlight_line = -1;

	cbrightness.mapped = 0;
	cbrightness.highlight_line = -1;

	corder.mapped = 0;
	corder.highlight_line = -1;

	lzw.mapped = 0;
	lzw.highlight_line = -1;

	version_mapped = 0;

}	/* end unmap_whole_menu */

/*
 ****************************************************************
 *	Desenha a janela de OPÇÕES				*
 ****************************************************************
 */
void
draw_option_win (void)
{
	const WINDATA	*wp = &option;
	int		line;
	const char	*str;

	draw_shadow
	(	wp->win,
		wp->triple.top_gc, wp->triple.bottom_gc,
		0, 0,
		wp->width, wp->height
	);

	for (line = 0; line < option_text_n; line++)
	{
		str = optionvec[line].m_text;

		XDrawString
		(	display, wp->win,
			option.text_gc,
			MENU_BORDER + SHADOW,
			MENU_BORDER + (line + 1) * (wp->font_height + 2 * SHADOW) - (wp->font_descent + SHADOW),
			str, strlen (str)
		);
	}

}	/* end draw_option_win */

/*
 ****************************************************************
 *	Mapeia a janela de opções				*
 ****************************************************************
 */
void
option_function (int value)
{
   	XMapRaised (display, option.win);

}	/* end option_function */

/*
 ****************************************************************
 *	Entrei na janela das OPÇÕES				*
 ****************************************************************
 */
void
motion_option_window (int y)
{
	int			line;
	const OPTIONVEC		*mp;
	const WINDATA		*wp = &option;
	SUBMENU			*smp;

	line = (y - MENU_BORDER) / (wp->font_height + 2 * SHADOW);

	if ((unsigned)line >= option_text_n)
		return;

	if (line == option_highlight_line)
		return;

	if (option_highlight_line >= 0)
		leave_option_window ();

	draw_shadow
	(	wp->win,
		wp->triple.top_gc, wp->triple.bottom_gc,
		MENU_BORDER, MENU_BORDER + line * (wp->font_height + 2 * SHADOW),
		wp->width - 2 * MENU_BORDER, (wp->font_height + 2 * SHADOW)
	);

	option_highlight_line = line;

	/*
	 *	Mapeia e Desmapeia as janelas adequadas
	 */
	for (mp = optionvec, line = 0; line < option_text_n; line++, mp++)
	{
		wp = mp->m_windata; smp = mp->m_submenu;

		if (wp == NOWINDATA && smp != NOSUBMENU)
			wp = &smp->windata;

		if (line == option_highlight_line)
		{
			if (wp != NOWINDATA)
			   	XMapRaised (display, wp->win);

			if (smp != NOSUBMENU)
			{
				smp->mapped = 1;
				smp->highlight_line = -1;
			}
		}
		else
		{
			if (wp != NOWINDATA)
			   	XUnmapWindow (display, wp->win);

			if (smp != NOSUBMENU)
				smp->mapped = 0;
		}
	}

}	/* end motion_option_window */

/*
 ****************************************************************
 *	Saí da janela das OPÇÕES				*
 ****************************************************************
 */
void
leave_option_window (void)
{
	const WINDATA	*wp = &option;
	int		line;

	if ((line = option_highlight_line) < 0)
		return;

	draw_shadow
	(	wp->win,
		wp->triple.main_gc, wp->triple.main_gc,
		MENU_BORDER, MENU_BORDER + line * (wp->font_height + 2 * SHADOW),
		wp->width - 2 * MENU_BORDER, (wp->font_height + 2 * SHADOW)
	);

	option_highlight_line = -1;

}	/* end leave_option_window */

/*
 ****************************************************************
 *	Apertou a tecla das OPÇÕES				*
 ****************************************************************
 */
void
do_option_button_press (int y)
{
	int		line = option_highlight_line;
	const OPTIONVEC	*vp = &optionvec[line];

   	XUnmapWindow (display, option.win);

	option_highlight_line = -1;

	if (vp->f_func != NOFUNC)
		vp->f_func (0);

}	/* end do_option_button_press */

/*
 ****************************************************************
 *	Desenha um SUBMENU					*
 ****************************************************************
 */
void
draw_submenu_win (const SUBMENU *smp)
{
	const WINDATA	*wp = &smp->windata;
	int		line;
	const char	*str;
	const TRIPLEGC	*tp;

	draw_shadow
	(	wp->win,
		wp->triple.top_gc, wp->triple.bottom_gc,
		0, 0,
		wp->width, wp->height
	);

	for (line = 0; line < smp->text_n; line++)
	{
		str = smp->vec[line].m_text;

		XDrawString
		(	display, wp->win,
			wp->text_gc,
			MENU_BORDER + SHADOW,
			MENU_BORDER + (line + 1) * (wp->font_height + 2 * SHADOW) - (wp->font_descent + SHADOW),
			str, strlen (str)
		);

		if (smp->vec[line].m_value == *smp->value_in_use)
			tp = &green_triple;
		else
			tp = &black_triple;

		draw_button
		(	wp, tp,
			wp->width - BUTTON_WIDTH - SHADOW - MENU_BORDER,
			MENU_BORDER + line * (wp->font_height + 2 * SHADOW) + SHADOW,
			BUTTON_WIDTH - 2, wp->font_height - 2
		);
	}

}	/* end draw_submenu_win */

/*
 ****************************************************************
 *	Entrei na janela de um SUBMENU				*
 ****************************************************************
 */
void
motion_submenu_window (SUBMENU *smp, int y)
{
	const WINDATA	*wp = &smp->windata;
	int		line;

	line = (y - MENU_BORDER) / (wp->font_height + 2 * SHADOW);

	if ((unsigned)line >= smp->text_n)
		return;

	if (line == smp->highlight_line)
		return;

	if (smp->highlight_line >= 0)
	{
		draw_shadow
		(	wp->win,
			wp->triple.main_gc, wp->triple.main_gc,
			MENU_BORDER, MENU_BORDER + smp->highlight_line * (wp->font_height + 2 * SHADOW),
			wp->width - 3 * MENU_BORDER - BUTTON_WIDTH, (wp->font_height + 2 * SHADOW)
		);

	   /***	smp->highlight_line = -1;	***/
	}

	draw_shadow
	(	wp->win,
		wp->triple.top_gc, wp->triple.bottom_gc,
		MENU_BORDER, MENU_BORDER + line * (wp->font_height + 2 * SHADOW),
		wp->width - 3 * MENU_BORDER - BUTTON_WIDTH, (wp->font_height + 2 * SHADOW)
	);

	smp->highlight_line = line;

}	/* end motion_submenu_window */

/*
 ****************************************************************
 *	Apertou a tecla de um SUBMENU				*
 ****************************************************************
 */
void
do_submenu_button_press (SUBMENU *smp, int y)
{
	const WINDATA	*wp = &smp->windata;
	int		line;
	const TRIPLEGC	*tp;

	if (smp->highlight_line < 0)
		return;

	*smp->value_in_use = smp->vec[smp->highlight_line].m_value;

	for (line = 0; line < smp->text_n; line++)
	{
		if (line == smp->highlight_line)
			tp = &green_triple;
		else
			tp = &black_triple;

		draw_button
		(	wp, tp,
			wp->width - BUTTON_WIDTH - SHADOW - MENU_BORDER,
			MENU_BORDER + line * (wp->font_height + 2 * SHADOW) + SHADOW,
			BUTTON_WIDTH - 2, wp->font_height - 2
		);
	}

	if   (smp == &msize)
	{
		msize_change_mandel_size (smp);
	}
	elif (smp == &cshift || smp == &cbrightness || smp == &corder)
	{
		unmap_whole_menu ();
		create_color_vector (1);
		draw_count_mandel_window ();
	}

}	/* end do_submenu_button_press */

/*
 ****************************************************************
 *	Desenha a janela da versão				*
 ****************************************************************
 */
void
draw_version_win (void)
{
	const WINDATA	*wp = &version;

	draw_shadow
	(	wp->win,
		wp->triple.top_gc, wp->triple.bottom_gc,
		0, 0,
		wp->width, wp->height
	);

	XDrawString
	(	display, wp->win,
		wp->text_gc,
		MENU_BORDER + SHADOW,
		MENU_BORDER + (wp->font_height + 2 * SHADOW) - (wp->font_descent + SHADOW),
		pgversion, strlen (pgversion)
	);

}	/* end draw_version_win */

/*
 ****************************************************************
 *	Altera o tamanho da janela da Imagem			*
 ****************************************************************
 */
void
msize_change_mandel_size (const SUBMENU *smp)
{
	int		width, height;

	switch (smp->highlight_line)
	{
	    case M640x480:
		width = 640; height = 480;
		break;

	    case M800x600:
		width = 800; height = 600;
		break;

	    case M1024x768:
		width = 1024; height = 768;
		break;

	    case M1280x1024:
		width = 1280; height = 1024;
		break;

	    case M1600x1200:
		width = 1600; height = 1200;
		break;

	    case M3200x2048:
		width = 3200; height = 2048;
		break;

	    default:
		return;

	}	/* end switch */

	set_window_sizes
	(	present_root_width  + width  - WINDOW_WIDTH,
		present_root_height + height - WINDOW_HEIGHT
	);

}	/* end msize_change_mandel_size */

/*
 ****************************************************************
 *	Levanta um "botão"					*
 ****************************************************************
 */
void
Shadow_up (const WINDATA *wp)
{
	draw_shadow
	(	wp->win,
		wp->triple.top_gc, wp->triple.bottom_gc,
		0, 0,
		wp->width, wp->height
	);

}	/* end Shadow_up */

/*
 ****************************************************************
 *	Tira a moldura de um "botão"				*
 ****************************************************************
 */
void
Shadow_down (const WINDATA *wp)
{
	draw_shadow
	(	wp->win,
		wp->triple.main_gc, wp->triple.main_gc,
		0, 0,
		wp->width, wp->height
	);

}	/* end Shadow_down */

/*
 ****************************************************************
 *	Imprime um retângulo com a sombra			*
 ****************************************************************
 */
void
draw_button (const WINDATA *wp, const TRIPLEGC *tp, int x, int y, int width, int height)
{
	XFillRectangle
	(	display, wp->win, tp->main_gc,
		x, y,
		width, height
	);

	draw_shadow
	(	wp->win,
		tp->top_gc, tp->bottom_gc,
		x, y,
		width, height
	);

}	/* end draw_button */

/*
 ****************************************************************
 *	Desenha as 4 sombras					*
 ****************************************************************
 */
void
draw_shadow (Window win, GC topShadow, GC botShadow, int win_x, int win_y, int width, int height)
{
	int		shadow_width = SHADOW;
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
	int			i, std_mandel_width, menu_width;
	int			width_inc, height_inc;
	MENUTB			*mp;
	WINDATA			*wp;
	const SUBMENU		*smp;
	int			msg_height;
	static int		std_width, std_height, menu_height;

	/*
	 ****** Verifica se é a primeira vez, ... ***************
	 */
	if (present_root_width == 0)	/* Primeira vez */
	{
		menu_width = BORDER;

		for (mp = &menutb[0]; mp < &menutb[NMENU]; mp++)
		{
			mp->m_win.width	 = strlen (mp->m_text) * mp->m_win.font_width + 2 * MENU_BORDER;
			mp->m_win.height = mp->m_win.font_height + 2 * MENU_BORDER;
			mp->m_win.x	 = menu_width;
			mp->m_win.y	 = BORDER;

			menu_width += mp->m_win.width + BORDER;
		}

		menu_height = mp[-1].m_win.y + mp[-1].m_win.height + BORDER;

		/*
		 *	Calcula os vários tamanhos
		 */
		wp		= &option;
		wp->width	= strlen (option_longest_text) * wp->font_width + 2 * SHADOW + 2 * MENU_BORDER;
		wp->height	= option_text_n * (wp->font_height + 2 * SHADOW) + 2 * MENU_BORDER;
		wp->x		= 0;
		wp->y		= wp->font_height + 2 * MENU_BORDER + 2 * BORDER;

		smp		= &msize;
		wp		= &msize.windata;
		wp->width	= smp->longest_text_len * wp->font_width + 2 * SHADOW +
				  3 * MENU_BORDER + BUTTON_WIDTH;
		wp->height	= smp->text_n * (wp->font_height + 2 * SHADOW) + 2 * MENU_BORDER;
		wp->x		= option.width;
		wp->y		= wp->font_height + 2 * MENU_BORDER + 2 * BORDER +
				  smp->line_offset * (wp->font_height + 2 * SHADOW) + MENU_BORDER;

		smp		= &cshift;
		wp		= &cshift.windata;
		wp->width	= smp->longest_text_len * wp->font_width + 2 * SHADOW +
				  3 * MENU_BORDER + BUTTON_WIDTH;
		wp->height	= smp->text_n * (wp->font_height + 2 * SHADOW) + 2 * MENU_BORDER;
		wp->x		= option.width;
		wp->y		= wp->font_height + 2 * MENU_BORDER + 2 * BORDER +
				  smp->line_offset * (wp->font_height + 2 * SHADOW) + MENU_BORDER;

		smp		= &cbrightness;
		wp		= &cbrightness.windata;
		wp->width	= smp->longest_text_len * wp->font_width + 2 * SHADOW +
				  3 * MENU_BORDER + BUTTON_WIDTH;
		wp->height	= smp->text_n * (wp->font_height + 2 * SHADOW) + 2 * MENU_BORDER;
		wp->x		= option.width;
		wp->y		= wp->font_height + 2 * MENU_BORDER + 2 * BORDER +
				  smp->line_offset * (wp->font_height + 2 * SHADOW) + MENU_BORDER;

		smp		= &corder;
		wp		= &corder.windata;
		wp->width	= smp->longest_text_len * wp->font_width + 2 * SHADOW +
				  3 * MENU_BORDER + BUTTON_WIDTH;
		wp->height	= smp->text_n * (wp->font_height + 2 * SHADOW) + 2 * MENU_BORDER;
		wp->x		= option.width;
		wp->y		= wp->font_height + 2 * MENU_BORDER + 2 * BORDER +
				  smp->line_offset * (wp->font_height + 2 * SHADOW) + MENU_BORDER;

		smp		= &lzw;
		wp		= &lzw.windata;
		wp->width	= smp->longest_text_len * wp->font_width + 2 * SHADOW +
				  3 * MENU_BORDER + BUTTON_WIDTH;
		wp->height	= smp->text_n * (wp->font_height + 2 * SHADOW) + 2 * MENU_BORDER;
		wp->x		= option.width;
		wp->y		= wp->font_height + 2 * MENU_BORDER + 2 * BORDER +
				  smp->line_offset * (wp->font_height + 2 * SHADOW) + MENU_BORDER;

		wp		= &version;
		wp->width	= strlen (version_longest_text) * wp->font_width + 2 * SHADOW +
				  2 * MENU_BORDER;
		wp->height	= version_text_n * (wp->font_height + 2 * SHADOW) + 2 * MENU_BORDER;
		wp->x		= option.width;
		wp->y		= wp->font_height + 2 * MENU_BORDER + 2 * BORDER +
				  version_line_offset * (wp->font_height + 2 * SHADOW) + MENU_BORDER;

		/*
		 *	Calcula o tamanho das janelas principais
		 */
		std_mandel_width = STD_MANDEL_WIDTH + 2 * SHADOW;

		if ((i = menu_width - (std_mandel_width + 2 * BORDER)) > 0)
			std_mandel_width += i;

		if ((i = szhint.width - (std_mandel_width + 2 * BORDER)) > 0)
			std_mandel_width += i;

		wp		= &msg;

		msg_height      = MSGLINES * (wp->font_height + 2 * SHADOW) + 2 * MENU_BORDER;

		std_width	= std_mandel_width + 2 * BORDER;
		std_height	= menu_height + msg_height + STD_MANDEL_HEIGHT + 2 * SHADOW + 2 * BORDER;

		if (std_height < szhint.height)
			std_height = szhint.height;


	   /***	wp		= &msg;		***/
		wp->width	= std_width - 2 * BORDER;
		wp->height	= msg_height;
		wp->x		= BORDER;
		wp->y		= menu_height;

		wp		= &mandel;
		wp->width	= STD_MANDEL_WIDTH + 2 * SHADOW;
		wp->height	= STD_MANDEL_HEIGHT + 2 * SHADOW;
		wp->x		= BORDER;
		wp->y		= msg.y + msg.height + BORDER;

		/*
		 *	Informa o WM
		 */
		szhint.width_inc   = 1;
		szhint.height_inc  = 1;

		szhint.base_width  = std_width;
		szhint.base_height = std_height;

		szhint.min_width   = std_width +  MIN_MANDEL_WIDTH -  STD_MANDEL_WIDTH;
		szhint.min_height  = std_height + MIN_MANDEL_HEIGHT - STD_MANDEL_HEIGHT;

		szhint.flags = PBaseSize|PMinSize|PResizeInc|PWinGravity;

		present_root_width  = std_width;
		present_root_height = std_height;

		return;
	}

	/*
	 *	Não é a primeira vez
	 */
	if (width == 0)
		width = present_root_width;

	if (height == 0)
		height = present_root_height;

	if (width == present_root_width && height == present_root_height)
		return;

	width_inc  = width  - present_root_width;
	height_inc = height - present_root_height;

	wp		 = &msg;
	wp->width	+= width_inc;
   /***	wp->height	+= height_inc;				***/
   /***	wp->x		 = ...;					***/
   /***	wp->y		 = ...;					***/

	wp		 = &mandel;
	wp->width	+= width_inc;
	wp->height	+= height_inc;
   /***	wp->x		 = ...;					***/
   /***	wp->y		 = ...;					***/

#ifdef	DEBUG
	printf ("Mudando a largura %d para %d\n", present_root_width, width);
#endif	DEBUG

	WINDOW_WIDTH 	+= width_inc;
	WINDOW_HEIGHT	+= height_inc;

#if (0)	/*******************************************************/
	if (msg_len == 0)
#endif	/*******************************************************/
		draw_msg_text ("Tamanho = %dx%d", WINDOW_WIDTH, WINDOW_HEIGHT);

	present_root_width  = width;
	present_root_height = height;

	value.width  = present_root_width;
	value.height = present_root_height;
	XConfigureWindow (display, root_win, CWWidth|CWHeight, &value);

	wp		 = &msg;
	value.width  = wp->width;
	XConfigureWindow (display, wp->win, CWWidth, &value);

	wp		 = &mandel;
	value.width  = wp->width;
	value.height = wp->height;
	XConfigureWindow (display, wp->win, CWWidth|CWHeight, &value);

#ifdef	DEBUG
	printf ("Mandel: %dx%d\n", wp->width, wp->height);
#endif	DEBUG

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
		"%s - desenha as imagens fractais de Mandelbrot\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-g <geo>] [-f <frente>] [-b <fundo>] [-d <video>] [-F <f>]\n",
		pgname, pgversion, pgname
	);
	fprintf
	(	stderr,
		"\nOpções:"
		"\t-g: Geometria <geo> (no formato <largura>x<altura>, em pixeis) \n"
		"\t-f: Use a cor de <frente> dada\n"
		"\t-b: Use a cor de <fundo> dada\n"
		"\t-d: Conecte-se ao <vídeo> dado\n"
		"\t-F: Use a fonte <f> dada\n"
	);

	exit (2);

}	/* end help */
