/*
 ****************************************************************
 *								*
 *			xview.c					*
 *								*
 *	Desenha as imagens fractais de Mandelbrot		*
 *								*
 *	Versão	4.5.0, de 27.12.03				*
 *		4.8.0, de 16.05.05				*
 *								*
 *	Módulo: xview						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2005 NCE/UFRJ - tecle "man licença"	*
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

#include "../h/xview.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char		pgversion[] = "Versão: 4.8.0, de 16.05.05";

#define	MSGLINES	3

entry int		vflag;		/* Verbose */
entry int		Rflag;		/* Fundo */

entry int		depth;

entry XStandardColormap	*StandardColormap;

entry Display		*display;

entry Window		root_win;

entry int		present_root_width, present_root_height;

entry const char	**given_argv, **first_file_nm_argv, **file_nm_argv;

/*
 ******	Tamanhos ************************************************
 */
enum {	STD_MANDEL_WIDTH = 640, STD_MANDEL_HEIGHT = 480 };

enum {	MIN_MANDEL_WIDTH = 100, MIN_MANDEL_HEIGHT = 100 };

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
	  PBaseSize|PMinSize|PResizeInc|PWinGravity,	/* Bits valid   flags			*/
	  0,	0,					/*		x, y			*/
	  STD_MANDEL_WIDTH,				/*		width			*/
	  STD_MANDEL_HEIGHT,				/*		height			*/
	  0,						/* PMinSize	min_width		*/
	  0,						/* PMinSize	min_height		*/
	  0,	0,					/*		max_width, max_height	*/
	  1,	1,					/* PResizeInc	width_inc, height_inc	*/
	{ 1,	1 },					/*		min_aspect		*/
	{ 0,	0 },					/*		max_aspect ratio	*/
	  0,						/* PBaseSize	base_width		*/
	  0,						/* PBaseSize	base_height		*/
	  NorthWestGravity				/* PWinGravity	gravity			*/
};

/*
 ******	Tabela de cores para os modos ***************************
 */
const char	*mode_color_nm_vector[16] =
{
	/* Cores regulares */

	"Black",		/* 0: black		(#000000) */
	"dodgerblue",		/* 1: blue		(#) */
   /***	"Blue3",		/* 1: blue		(#0000CD) */
	"Green3",		/* 2: green		(#00CD00) */
	"Yellow3",		/* 3: yellow		(#CDCD00) */
	"orangered",		/* 4: red		(#) */
   /***	"Red3",			/* 4: red		(#CD0000) */
	"Magenta3",		/* 5: magenta		(#CD00CD) */
	"Cyan3",		/* 6: cyan		(#00CDCD) */
	"AntiqueWhite",		/* 7: white		(#FAEBD7) */

	/* Cores mais claras */

	"Grey25",		/* 8:  bright black	(#404040) */
	"deepskyblue",		/* 9:  bright blue	(#) */
   /***	"Blue",			/* 9:  bright blue	(#0000FF) */
	"Green",		/* 10: bright green	(#00FF00) */
	"Yellow",		/* 11: bright yellow	(#FFFF00) */
	"Red",			/* 12: bright red	(#FF0000) */
	"Magenta",		/* 13: bright magenta	(#FF00FF) */
	"Cyan",			/* 14: bright cyan	(#00FFFF) */
	"White"			/* 15: bright white	(#FFFFFF) */
};

entry GC		mode_gc_vector[16];

/*
 ******	Dados da janela de carregar/salvar a imagem *************
 */
entry SUBMENU		load_win;

entry WINDATA		dir_win;

entry SCROLLDATA	dir_scroll;

entry WINDATA		dir_nm_win, file_nm_win, ok_win, new_dir_win;

/*
 ******	Dados do menu de carregar/salvar a imagem ***************
 */
entry WINDATA		file_menu;

const OPTIONVEC	file_menu_vec[] =
{
	"Carrega:",	map_load_window,	LOAD_IMAGE,	NOSUBMENU,	/* 0 */
	"Salva:",	map_load_window,	STORE_IMAGE,	NOSUBMENU,	/* 1 */
	"Fim",		quit_function,		0,		NOSUBMENU	/* 2 */
};

#define	file_menu_longest_text	file_menu_vec[0].m_text
#define	file_menu_text_n	(sizeof (file_menu_vec) / sizeof (OPTIONVEC))

entry int		file_menu_highlight_line = -1;

/*
 ******	Dados da janela de MENU *********************************
 */
void		file_menu_function (int value);
void		option_function (int value);
void		quit_function (int value);
void		draw_image (int value);
#if (0)	/*******************************************************/
void		no_function (int value) {}
#endif	/*******************************************************/

typedef struct
{
	const char	*m_text;		/* Texto a escrever */
	void		(*m_func) (int);	/* Função a chamar */
	int		m_value;		/* O valor do argumento da função */
	WINDATA		m_win;			/* Dados da janela */

}	MENUTB;

MENUTB		menutb[] =
{
	{ "Imagem",			file_menu_function },		/* 0 */
	{ "Opções",			option_function },		/* 1 */
	{ "<=",				draw_image,		-1 },	/* 2 */
	{ "=>",				draw_image,		+1 },	/* 3 */
#if (0)	/*******************************************************/
	{ "Imagem anterior",		draw_image,		-1 },	/* 2 */
	{ "Imagem seguinte",		draw_image,		+1 },	/* 3 */
	{ "Fundo da Tela",		draw_background },		/* 4 */
#endif	/*******************************************************/
	{ "Fim",			quit_function	}		/* 5 */
};

#define	NMENU	(sizeof (menutb) / sizeof (MENUTB))

/*
 ******	Dados de AJUSTAR o tamanho da janela da imagem **********
 */
const SUBMENU_VEC	adjust_vec[] =
{
	"Corrente",	ADJUST_PRESENT,
	"Máximo",	ADJUST_MAX
};

entry int		adjust_mode = ADJUST_PRESENT;	/* Usa o tamanho corrente */

entry SUBMENU	adjust =
{
	{ NULL },					/* Windata */
	adjust_vec,					/* Text vector */

	0,						/* mapped */
	0,						/* line_offset */
	8,						/* strlen ("Corrente") */
	(sizeof (adjust_vec) / sizeof (SUBMENU_VEC)),	/* text_n */
	&adjust_mode,					/* value_in_use */
	-1						/* highlight_line */
};

/*
 ******	Dados da janela da VERSÃO *******************************
 */
entry SUBMENU	version;

entry int		version_mapped;

#define	version_line_offset	3			/* Das opções, acima */
#define	version_longest_text	pgversion
#define	version_text_n		1

/*
 ******	Dados da janela das OPÇÕES ******************************
 */
entry WINDATA		option;

const OPTIONVEC	optionvec[] =
{
    "Ajusta o tamanho:",		NOFUNC,			0,	&adjust,	/* 0 */
    "Fundo da Tela",			draw_background,	0,	NULL,		/* 1 */
    "Memória",				mem_function,		0,	NOSUBMENU,	/* 2 */
    "Versão: ",				NOFUNC,			0,	&version,	/* 3 */
};

#define	option_longest_text	optionvec[0].m_text
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
void		set_font_sizes (int width, int height, int ascent, int descent, ...);
int		set_shadow_colors (Display *, Window, TRIPLEGC *, const char *, Pixel);
void		geometry_to_size_hints (const char *geo_str, XSizeHints *hp);
void		event_loop (int main_event_loop);
void		draw_file_menu_win (void);
void		draw_option_win (const WINDATA *wp, int text_n, const OPTIONVEC *vec);
void		draw_submenu_win (const SUBMENU *smp);
void		draw_version_win (void);
void		draw_menu_win (const MENUTB *mp);
void		unmap_whole_menu (void);
void		Shadow_up (const WINDATA *);
void		Shadow_even (const WINDATA *);
void		do_option_button_press (const WINDATA *wp, const OPTIONVEC *vec, int *highlight_line);
void		do_submenu_button_press (SUBMENU *smp);
int		get_pointer_y (Window win);
void		get_pointer_x_y (Window win, int *x, int *y);
void		motion_submenu_window (SUBMENU *smp, int y);
void		motion_option_window (const WINDATA *, int , int *, const OPTIONVEC *vec, int y);
void		msize_change_mandel_size (const SUBMENU *smp);
Pixel		get_window_color (const char *color_nm, Pixel def);

/*
 ****************************************************************
 *	Gerenciador gráfico de arquivos				*
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
	int				font_width, font_height, font_ascent, font_descent;
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
	while ((opt = getopt (argc, argv, "vRg:f:b:d:F:H")) != EOF)
	{
		switch (opt)
		{
		    case 'v':			/* Verbose */
			vflag++;
			break;

		    case 'R':			/* Fundo */
			Rflag++;
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
	elif ((str = XGetDefault (display, "XView", "foreground")) != NOSTR)
		fore_color_str = str;
	else
		fore_color_str = default_fore_color_str;

	if   (user_back_color_str != NOSTR)
		back_color_str = user_back_color_str;
	elif ((str = XGetDefault (display, "XView", "background")) != NOSTR)
		back_color_str = str;
	else
		back_color_str = default_back_color_str;

	if   (user_font_str != NOSTR)
		font_str = user_font_str;
	elif ((str = XGetDefault (display, "XView", "menufont")) != NOSTR)
		font_str = str;
	else
		font_str = default_font_str;

	/*
	 ******	Obtém os dados da fonte *************************
	 */
	if ((text_fip = XLoadQueryFont (display, font_str)) == NULL)
		error ("$Não encontrei a fonte \"%s\"", font_str);

	font_width   = XTextWidth (text_fip, "MMMMMMMMMM", 10) / 10;
	font_ascent  = text_fip->ascent;
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
		mp->m_win.font_ascent	= font_ascent;
		mp->m_win.font_descent	= font_descent;
	}

	set_font_sizes
	(	font_width, font_height, font_ascent, font_descent, &file_menu, &load_win.windata,
		&dir_win, &dir_nm_win, &file_nm_win, &ok_win, &new_dir_win, &option,
		&adjust.windata, &version.windata, &msg, &mandel, NOWINDATA
	);

	/*
	 ****** Cria a janela PRINCIPAL	*************************
	 */
	geometry_to_size_hints (geo_str, &szhint);

	set_window_sizes (0, 0);

        win = XCreateSimpleWindow
	(	display, root,
		0, 0,
		szhint.width, szhint.height,
		0, BORDER_COLOR,
		BlackPixel (display, screen)
	);

	root_win = win;

	XStoreName (display, win, "Xview");

	XSelectInput (display, win, ExposureMask|StructureNotifyMask);

	classhint.res_name  = (char *)pgname;
	classhint.res_class = "XView";

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
	 ****** Prepara a janela do MENU de CARGA da Imagem *****
	 */
	wp = &file_menu;

	memmove (&wp->triple, &back_triple, sizeof (TRIPLEGC));
	wp->text_gc      = menu_text_gc;

        wp->win = XCreateSimpleWindow
	(	display, win,
		wp->x, wp->y,
		wp->width, wp->height,
		0, BORDER_COLOR,
		wp->triple.main_pixel
	);

	XSelectInput
	(	display, wp->win,
		(ExposureMask|EnterWindowMask|LeaveWindowMask|
		PointerMotionMask|PointerMotionHintMask|ButtonPressMask)
	);

	hand_cursor = XCreateFontCursor (display, XC_hand2);
	XDefineCursor (display, wp->win, hand_cursor);

   /***	XMapWindow (display, wp->win); ***/

	/*
	 ****** Prepara a janela para carregar/salvar uma Imagem **
	 */
	wp = &load_win.windata;

	memmove (&wp->triple, &back_triple, sizeof (TRIPLEGC));
	wp->text_gc      = menu_text_gc;

        wp->win = XCreateSimpleWindow
	(	display, win,
		wp->x, wp->y,
		wp->width, wp->height,
		0, BORDER_COLOR,
		wp->triple.main_pixel
	);

	XSelectInput
	(	display, wp->win,
		(ExposureMask|EnterWindowMask|LeaveWindowMask|
		PointerMotionMask|PointerMotionHintMask|ButtonPressMask)
	);

   /***	XMapWindow (display, wp->win); ***/

	/*
	 ****** Prepara a janela do diretório da Imagem ***********
	 */
	wp = &dir_win;

	memmove (&wp->triple, &back_triple, sizeof (TRIPLEGC));
	wp->text_gc      = menu_text_gc;

        wp->win = XCreateSimpleWindow
	(	display, load_win.windata.win,
		wp->x, wp->y,
		wp->width, wp->height,
		0, BORDER_COLOR,
		wp->triple.main_pixel
	);

	XSelectInput
	(	display, wp->win,
		(ExposureMask|EnterWindowMask|LeaveWindowMask|
		PointerMotionMask|PointerMotionHintMask|ButtonPressMask)
	);

	XDefineCursor (display, wp->win, hand_cursor);

	XMapWindow (display, wp->win);

	/*
	 ****** Janela da BARRA de ROLAGEM do diretório *********
	 */
#if (0)	/*******************************************************/
	memmove (&dir_scroll.triple, &back_triple, sizeof (TRIPLEGC));
#endif	/*******************************************************/

	memmove (&dir_scroll.back_triple, &back_triple, sizeof (TRIPLEGC));
	memmove (&dir_scroll.fore_triple, &back_triple, sizeof (TRIPLEGC));

#if (0)	/*******************************************************/
	memmove (&dir_scroll.back_triple, &back_triple, sizeof (TRIPLEGC));
	memmove (&dir_scroll.fore_triple, &fore_triple, sizeof (TRIPLEGC));
#endif	/*******************************************************/

	dir_scroll.win = XCreateSimpleWindow
	(
		display, load_win.windata.win,
		dir_scroll.x, dir_scroll.y,
		dir_scroll.width, dir_scroll.height,
		0, BORDER_COLOR,
		dir_scroll.back_triple.main_pixel
	);

	XSelectInput
	(	display, dir_scroll.win,
		(ExposureMask|ButtonPressMask|ButtonReleaseMask|Button1MotionMask|PointerMotionHintMask)
	);

	XMapWindow (display, dir_scroll.win);

	/*
	 ****** Prepara a janela do nome do diretório de carga ****
	 */
	wp = &dir_nm_win;

	memmove (&wp->triple, &back_triple, sizeof (TRIPLEGC));
	wp->text_gc = menu_text_gc;

        wp->win = XCreateSimpleWindow
	(	display, load_win.windata.win,
		wp->x, wp->y,
		wp->width, wp->height,
		0, BORDER_COLOR,
		wp->triple.main_pixel
	);

	XSelectInput (display, wp->win, ExposureMask);

	XMapWindow (display, wp->win);

	/*
	 ****** Prepara a janela do nome do arquivo de carga ******
	 */
	wp = &file_nm_win;

	memmove (&wp->triple, &back_triple, sizeof (TRIPLEGC));
	wp->text_gc = menu_text_gc;

        wp->win = XCreateSimpleWindow
	(	display, load_win.windata.win,
		wp->x, wp->y,
		wp->width, wp->height,
		0, BORDER_COLOR,
		wp->triple.main_pixel
	);

	XSelectInput (display, wp->win, ExposureMask);

	XMapWindow (display, wp->win);

	/*
	 ****** Prepara a janela do OK do diretório da Imagem *****
	 */
	wp = &ok_win;

	memmove (&wp->triple, &back_triple, sizeof (TRIPLEGC));
	wp->text_gc      = menu_text_gc;

        wp->win = XCreateSimpleWindow
	(	display, load_win.windata.win,
		wp->x, wp->y,
		wp->width, wp->height,
		0, BORDER_COLOR,
		wp->triple.main_pixel
	);

	XSelectInput
	(	display, wp->win,
		(ExposureMask|EnterWindowMask|LeaveWindowMask|ButtonPressMask)
	);

	XMapWindow (display, wp->win);

	/*
	 ****** Prepara a janela do NOVO DIRETÓRIO da Imagem ******
	 */
	wp = &new_dir_win;

	memmove (&wp->triple, &back_triple, sizeof (TRIPLEGC));
	wp->text_gc      = menu_text_gc;

        wp->win = XCreateSimpleWindow
	(	display, load_win.windata.win,
		wp->x, wp->y,
		wp->width, wp->height,
		0, BORDER_COLOR,
		wp->triple.main_pixel
	);

	XSelectInput
	(	display, wp->win,
		(ExposureMask|EnterWindowMask|LeaveWindowMask|ButtonPressMask)
	);

	XMapWindow (display, wp->win);

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

	XDefineCursor (display, option.win, hand_cursor);

   /***	XMapWindow (display, option.win); ***/

	/*
	 ******	Prepara a janela do AJUSTE do tamanho ***********
	 */
	wp = &adjust.windata;

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
	wp = &version.windata;

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

   /***	XMapWindow (display, wp->win); ***/

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
	 ******	Carrega a imagem ****************************************
	 */
	given_argv		= argv;
	first_file_nm_argv	= argv;
	file_nm_argv		= argv - 1;

#if (0)	/*******************************************************/
	draw_image (argv);
#endif	/*******************************************************/

	/*
	 ****** Prepara as cores para o "ls" ********************
	 */
    {
	int		i, j, color;
	char		*color_vector;		/* Para as cores dos modos */

	modecolor (1 /* ativo */, &color_vector, (const char ***)NULL);

	for (i = 0; i < 16; i++)
	{
		/* Verifica se está indefinido */

		if ((color = color_vector[i]) > 15)
			{ mode_gc_vector[i] = file_text_gc; continue; }

		/* Verifica se já foi visto */

		for (j = 0; j < i; j++)
		{
			if (color_vector[j] == color)
				{ mode_gc_vector[i] = mode_gc_vector[j]; goto next_color; }

		}

		/* Cria um novo */

		gcv.font	  = text_fip->fid;
		gcv.foreground    = get_window_color (mode_color_nm_vector[color], fore_triple.main_pixel);
		gcv.background    = back_triple.main_pixel;
		mode_gc_vector[i] = XCreateGC (display, win, GCForeground|GCBackground|GCFont, &gcv);

		/* O próximo */

	    next_color:
		/* vazio */;
	}
    }
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
#if (0)	/*******************************************************/
			if (event.xexpose.count != 0)
				break;
#endif	/*******************************************************/

			if   (event.xany.window == file_menu.win)
			{
				draw_option_win (&file_menu, file_menu_text_n, file_menu_vec);
			}
			elif (event.xany.window == load_win.windata.win)
			{
				draw_load_win ();
			}
			elif (event.xany.window == dir_win.win)
			{
				draw_dir_win ();
			}
			elif (event.xany.window == dir_scroll.win)
			{
				draw_scroll_win (&dir_scroll);
			}
			elif (event.xany.window == ok_win.win)
			{
				draw_ok_win ();
			}
			elif (event.xany.window == new_dir_win.win)
			{
				draw_new_dir_win ();
			}
			elif (event.xany.window == option.win)
			{
				draw_option_win (&option, option_text_n, optionvec);
			}
			elif (event.xany.window == adjust.windata.win)
			{
				draw_submenu_win (&adjust);
			}
			elif (event.xany.window == version.windata.win)
			{
				draw_version_win ();
			}
			elif (event.xany.window == msg.win)
			{
				draw_msg_win ();
			}
			elif (event.xany.window == mandel.win)
			{
				static int	first_time = 1;

				if (first_time)
					{ draw_image (0); first_time = 0; }

				draw_mandel_win (&event.xexpose);
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
			elif (event.xany.window == ok_win.win)
			{
				Shadow_up_with_border (&ok_win);
			}
			elif (event.xany.window == new_dir_win.win)
			{
				Shadow_up_with_border (&new_dir_win);
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
			if   (event.xany.window == file_menu.win)
			{
			   	XUnmapWindow (display, file_menu.win);

				file_menu_highlight_line = -1;
			}
#if (0)	/*******************************************************/
			elif (event.xany.window == load_win.windata.win)
			{
				if (event.xcrossing.detail != NotifyInferior)
				   	XUnmapWindow (display, load_win.windata.win);
			}
#endif	/*******************************************************/
			elif (event.xany.window == option.win)
			{
#if (0)	/*******************************************************/
				if
				(	msize.mapped || cshift.mapped || cbrightness.mapped ||
					corder.mapped || version_mapped
				)
#endif	/*******************************************************/

				if (adjust.mapped || version_mapped)
					break;

			   	XUnmapWindow (display, option.win);

				option_highlight_line = -1;
			}
			elif (event.xany.window == adjust.windata.win)
			{
			   	XUnmapWindow (display, option.win);
			   	XUnmapWindow (display, adjust.windata.win);

				adjust.mapped = 0;
				adjust.highlight_line = -1;
			}
			elif (event.xany.window == version.windata.win)
			{
			   	XUnmapWindow (display, option.win);
			   	XUnmapWindow (display, version.windata.win);
#if (0)	/*******************************************************/
			   	XUnmapWindow (display, version.win);
#endif	/*******************************************************/

				version_mapped = 0;
			}
			elif (event.xany.window == ok_win.win)
			{
				Shadow_even_with_border (&ok_win);
			}
			elif (event.xany.window == new_dir_win.win)
			{
				Shadow_even_with_border (&new_dir_win);
			}
			else for (mp = &menutb[0]; mp < &menutb[NMENU]; mp++)
			{
				if (mp->m_win.win == event.xany.window)
				{
					Shadow_even (&mp->m_win); break;
				}
			}

			break;

		    case ButtonPress:
			if (main_event_loop == 0)
				return;

#if (0)	/*******************************************************/
			clear_msg_win (); unmap_tmp_windows ();
#endif	/*******************************************************/

			if   (event.xany.window == file_menu.win)
			{
				do_option_button_press (&file_menu, file_menu_vec, &file_menu_highlight_line);
			}
			elif (event.xany.window == dir_win.win)
			{
				switch (event.xbutton.button)
				{
				    case Button1:
					button_1_press_dir_window (event.xbutton.y);
					break;

				    case Button4:
					update_dir_scroll_bar (-1, 1);
					break;

				    case Button5:
					update_dir_scroll_bar (-1, 100000);
					break;
				}
			}
			elif (event.xany.window == dir_scroll.win)
			{
				SCROLLDATA      *sp = &dir_scroll;

				switch (event.xbutton.button)
				{
				    case Button1:
					sp->button_offset = event.xbutton.y - sp->top;
					update_dir_scroll_bar (0 /* button */, event.xbutton.y);
					break;

				    case Button4:
					update_dir_scroll_bar (-1, 1);
					break;

				    case Button5:
					update_dir_scroll_bar (-1, 100000);
					break;
				}
			}
			elif (event.xany.window == option.win)
			{
				do_option_button_press (&option, optionvec, &option_highlight_line);
			}
			elif (event.xany.window == adjust.windata.win)
			{
			   	do_submenu_button_press (&adjust);
			}
			elif (event.xany.window == ok_win.win)
			{
				button_1_press_ok_window ();
			}
			else for (mp = &menutb[0]; mp < &menutb[NMENU]; mp++)
			{
				if (mp->m_win.win == event.xany.window)
					{ (*mp->m_func) (mp->m_value); break; }
			}

			break;

		    case ButtonRelease:
			if (main_event_loop == 0)
				return;

			if   (event.xany.window == dir_scroll.win)
			{
				button_release_scroll_button (&dir_scroll);
			}

			break;

		    case MotionNotify:
			if   (event.xany.window == file_menu.win)
			{
				motion_option_window
				(	&file_menu,
					file_menu_text_n,
					&file_menu_highlight_line,
					file_menu_vec,
					get_pointer_y (file_menu.win)
				);
			}
			elif (event.xany.window == option.win)
			{
				motion_option_window
				(	&option,
					option_text_n,
					&option_highlight_line,
					optionvec,
					get_pointer_y (option.win)
				);
			}
			elif (event.xany.window == adjust.windata.win)
			{
				motion_submenu_window (&adjust, get_pointer_y (adjust.windata.win));
			}
			elif (event.xany.window == dir_scroll.win)
			{
				update_dir_scroll_bar (1 /* motion */, get_pointer_y (dir_scroll.win));
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
set_font_sizes (int width, int height, int ascent, int descent, ...)
{
	va_list		ap;
	WINDATA		*wp;

	va_start (ap, descent);

	while ((wp = va_arg (ap, WINDATA *)) != NOWINDATA)
	{
		wp->font_width   = width;
		wp->font_height  = height;
		wp->font_ascent  = ascent;
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

#if (0)	/*******************************************************/
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
#endif	/*******************************************************/

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
#if (0)	/*******************************************************/
   	XUnmapWindow (display, msize.windata.win);
   	XUnmapWindow (display, cshift.windata.win);
   	XUnmapWindow (display, cbrightness.windata.win);
   	XUnmapWindow (display, corder.windata.win);
#endif	/*******************************************************/
   	XUnmapWindow (display, adjust.windata.win);
   	XUnmapWindow (display, version.windata.win);
#if (0)	/*******************************************************/
   	XUnmapWindow (display, version.win);
#endif	/*******************************************************/

	adjust.mapped = 0;
	adjust.highlight_line = -1;

	version_mapped = 0;

}	/* end unmap_whole_menu */

#if (0)	/*******************************************************/
/*
 ****************************************************************
 *	Desenha a janela da escolha da Imagem			*
 ****************************************************************
 */
void
draw_file_menu_win (void)
{
	const WINDATA	*wp = &file_menu;

	draw_shadow
	(	wp->win,
		wp->triple.top_gc, wp->triple.bottom_gc,
		0, 0,
		wp->width, wp->height
	);

}	/* end draw_file_menu_win */
#endif	/*******************************************************/

/*
 ****************************************************************
 *	Mapeia a janela da escolha da Imagem			*
 ****************************************************************
 */
void
file_menu_function (int value)
{
   	XMapRaised (display, file_menu.win);

}	/* end option_function */

/*
 ****************************************************************
 *	Desenha a janela de OPÇÕES				*
 ****************************************************************
 */
void
draw_option_win (const WINDATA *wp, int text_n, const OPTIONVEC *vec)
{
	int		line;
	const char	*str;

	draw_shadow
	(	wp->win,
		wp->triple.top_gc, wp->triple.bottom_gc,
		0, 0,
		wp->width, wp->height
	);

	for (line = 0; line < text_n; line++)
	{
		str = vec[line].m_text;

		XDrawString
		(	display, wp->win,
			wp->text_gc,
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
motion_option_window (const WINDATA *wp, int text_n, int *highlight_line, const OPTIONVEC *vec, int y)
{
	int			line;
	const OPTIONVEC		*mp;
	SUBMENU			*smp;
	const WINDATA		*map_wp;

	line = (y - MENU_BORDER) / (wp->font_height + 2 * SHADOW);

	if ((unsigned)line >= text_n)
		return;

	if (line == *highlight_line)
		return;

	if (*highlight_line >= 0)
	{
		draw_shadow
		(	wp->win,
			wp->triple.main_gc, wp->triple.main_gc,
			MENU_BORDER, MENU_BORDER + *highlight_line * (wp->font_height + 2 * SHADOW),
			wp->width - 2 * MENU_BORDER, (wp->font_height + 2 * SHADOW)
		);

	   /***	*highlight_line = -1;	***/
	}

	draw_shadow
	(	wp->win,
		wp->triple.top_gc, wp->triple.bottom_gc,
		MENU_BORDER, MENU_BORDER + line * (wp->font_height + 2 * SHADOW),
		wp->width - 2 * MENU_BORDER, (wp->font_height + 2 * SHADOW)
	);

	*highlight_line = line;

	/*
	 *	Guarda a janela a mapear
	 */
	if ((smp = vec[line].m_submenu) != NOSUBMENU)
		map_wp = &smp->windata;
	else
		map_wp = NOWINDATA;

	/*
	 *	Mapeia e Desmapeia as janelas adequadas
	 */
	for (mp = vec, line = 0; line < text_n; line++, mp++)
	{
		if ((smp = mp->m_submenu) != NOSUBMENU)
			wp = &smp->windata;
		else
			wp = NOWINDATA;

		if (line == *highlight_line)
		{
			if (wp != NOWINDATA)
			   	XMapRaised (display, wp->win);

			if (smp != NOSUBMENU)
				{ smp->mapped = 1; smp->highlight_line = -1; }
		}
		else
		{
			if (wp != NOWINDATA && wp != map_wp)
			   	XUnmapWindow (display, wp->win);

			if (smp != NOSUBMENU)
				smp->mapped = 0;
		}
	}

}	/* end motion_option_window */

/*
 ****************************************************************
 *	Apertou a tecla das OPÇÕES				*
 ****************************************************************
 */
void
do_option_button_press (const WINDATA *wp, const OPTIONVEC *vec, int *highlight_line)
{
	const OPTIONVEC	*vp;

   	XUnmapWindow (display, wp->win);

	if (*highlight_line != -1)
	{
		vp = &vec[*highlight_line];

		if (vp->f_func != NOFUNC)
			vp->f_func (vp->f_value);

		*highlight_line = -1;
	}

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
do_submenu_button_press (SUBMENU *smp)
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

}	/* end do_submenu_button_press */

/*
 ****************************************************************
 *	Desenha a janela da versão				*
 ****************************************************************
 */
void
draw_version_win (void)
{
	const WINDATA	*wp = &version.windata;

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

#if (0)	/*******************************************************/
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
#endif	/*******************************************************/

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
Shadow_even (const WINDATA *wp)
{
	draw_shadow
	(	wp->win,
		wp->triple.main_gc, wp->triple.main_gc,
		0, 0,
		wp->width, wp->height
	);

}	/* end Shadow_even */

#if (0)	/*******************************************************/
/*
 ****************************************************************
 *	Abaixa um "botão"					*
 ****************************************************************
 */
void
Shadow_down (const WINDATA *wp)
{
	draw_shadow
	(	wp->win,
		wp->triple.bottom_gc, wp->triple.top_gc,
		0, 0,
		wp->width, wp->height
	);

}	/* end Shadow_down */
#endif	/*******************************************************/

/*
 ****************************************************************
 *	Levanta um "botão" com borda				*
 ****************************************************************
 */
void
Shadow_up_with_border (const WINDATA *wp)
{
	draw_border
	(	wp->win,
		0, 0,
		wp->width, wp->height
	);

	draw_shadow
	(	wp->win,
		wp->triple.top_gc, wp->triple.bottom_gc,
		BORDER, BORDER,
		wp->width - 2 * BORDER, wp->height - 2 * BORDER
	);

}	/* end Shadow_up_with_border */

/*
 ****************************************************************
 *	Tira a moldura de um "botão" com borda			*
 ****************************************************************
 */
void
Shadow_even_with_border (const WINDATA *wp)
{
	draw_border
	(	wp->win,
		0, 0,
		wp->width, wp->height
	);

	draw_shadow
	(	wp->win,
		wp->triple.main_gc, wp->triple.main_gc,
		BORDER, BORDER,
		wp->width - 2 * BORDER, wp->height - 2 * BORDER
	);

}	/* end Shadow_even_with_border */

/*
 ****************************************************************
 *	Desenha a borda (moldura)				*
 ****************************************************************
 */
void
draw_border (Window win, int win_x, int win_y, int width, int height)
{
	int		border_width, x, y, w, h;
	GC		gc = black_triple.main_gc;

	w = width + (win_x - 1); h = height + (win_y - 1);
	x = win_x; y = win_y;

	for (border_width = BORDER; border_width > 0; border_width--, x++, y++, w--, h--)
	{
		XDrawLine (display, win, gc, x, y, w, y);
		XDrawLine (display, win, gc, x, y, x, h);
	}

	w = width + (win_x - 1); h = height + (win_y - 1);
	x = win_x + 1; y = win_y + 1;

	for (border_width = BORDER; border_width > 0; border_width--, x++, y++, w--, h--)
	{
		XDrawLine (display, win, gc, w, h, w, y);
		XDrawLine (display, win, gc, w, h, x, h);
	}

}	/* end draw_border */

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
	int			i, menu_width;
	int			width_inc, height_inc;
	MENUTB			*mp;
	WINDATA			*wp;
	const SUBMENU		*smp;
	int			msg_height;
	static int		total_width, total_height, menu_height;

	/*
	 ****** Verifica se é a primeira vez, ... ***************
	 */
	if (present_root_width == 0)	/* Primeira vez */
	{
		mp = &menutb[0];

		menu_width  = BORDER;
		menu_height = BORDER + mp->m_win.font_height + 2 * MENU_BORDER + BORDER;

		for (/* acima */; mp < &menutb[NMENU]; mp++)
		{
			mp->m_win.width	 = strlen (mp->m_text) * mp->m_win.font_width + 2 * MENU_BORDER;
			mp->m_win.height = mp->m_win.font_height + 2 * MENU_BORDER;
			mp->m_win.x	 = menu_width;
			mp->m_win.y	 = BORDER;

			menu_width += mp->m_win.width + BORDER;
		}

		/*
		 *	Calcula os vários tamanhos
		 */
		wp		= &file_menu;
		wp->width	= strlen (file_menu_longest_text) * wp->font_width + 2 * SHADOW + 2 * MENU_BORDER;
		wp->height	= file_menu_text_n * (wp->font_height + 2 * SHADOW) + 2 * MENU_BORDER;
		wp->x		= BORDER;
		wp->y		= wp->font_height + 2 * MENU_BORDER + 2 * BORDER;

		set_load_win_size ();

		wp		= &option;
		wp->width	= strlen (option_longest_text) * wp->font_width + 2 * SHADOW + 2 * MENU_BORDER;
		wp->height	= option_text_n * (wp->font_height + 2 * SHADOW) + 2 * MENU_BORDER;
		wp->x		= 2 * BORDER + menutb[0].m_win.width;
		wp->y		= wp->font_height + 2 * MENU_BORDER + 2 * BORDER;

		smp		= &adjust;
		wp		= &adjust.windata;
		wp->width	= smp->longest_text_len * wp->font_width + 2 * SHADOW +
				  3 * MENU_BORDER + BUTTON_WIDTH;
		wp->height	= smp->text_n * (wp->font_height + 2 * SHADOW) + 2 * MENU_BORDER;
		wp->x		= 2 * BORDER + menutb[0].m_win.width + option.width;
		wp->y		= wp->font_height + 2 * MENU_BORDER + 2 * BORDER +
				  smp->line_offset * (wp->font_height + 2 * SHADOW) + MENU_BORDER;

		wp		= &version.windata;
		wp->width	= strlen (version_longest_text) * wp->font_width + 2 * SHADOW +
				  2 * MENU_BORDER;
		wp->height	= version_text_n * (wp->font_height + 2 * SHADOW) + 2 * MENU_BORDER;
		wp->x		= 2 * BORDER + menutb[0].m_win.width + option.width;
		wp->y		= wp->font_height + 2 * MENU_BORDER + 2 * BORDER +
				  version_line_offset * (wp->font_height + 2 * SHADOW) + MENU_BORDER;

		/*
		 *	Calcula o tamanho das janelas principais
		 */
		if ((WINDOW_WIDTH = szhint.width) < MIN_MANDEL_WIDTH)
			WINDOW_WIDTH = MIN_MANDEL_WIDTH;

		if ((WINDOW_HEIGHT = szhint.height) < MIN_MANDEL_HEIGHT)
			WINDOW_HEIGHT = MIN_MANDEL_HEIGHT;

		if ((i = menu_width - (WINDOW_WIDTH + 2 * BORDER)) > 0)
			WINDOW_WIDTH += i;

		wp		= &msg;

		msg_height      = MSGLINES * (wp->font_height + 2 * SHADOW) + 2 * MENU_BORDER;

		total_width	= WINDOW_WIDTH + 2 * BORDER + 2 * SHADOW;
		total_height	= menu_height + msg_height + WINDOW_HEIGHT + 2 * SHADOW + 2 * BORDER;

	   /***	wp		= &msg;		***/
		wp->width	= WINDOW_WIDTH + 2 * SHADOW;
		wp->height	= msg_height;
		wp->x		= BORDER;
		wp->y		= menu_height;

		wp		= &mandel;
		wp->width	= WINDOW_WIDTH + 2 * SHADOW;
		wp->height	= WINDOW_HEIGHT + 2 * SHADOW;
		wp->x		= BORDER;
		wp->y		= msg.y + msg.height + BORDER;

		/*
		 *	Informa o WM
		 */
		szhint.base_width   = total_width  - WINDOW_WIDTH;
		szhint.base_height  = total_height - WINDOW_HEIGHT;

		szhint.width	    = present_root_width  = total_width;
		szhint.height	    = present_root_height = total_height;

		szhint.min_width    = total_width  + MIN_MANDEL_WIDTH -  STD_MANDEL_WIDTH;
		szhint.min_height   = total_height + MIN_MANDEL_HEIGHT - STD_MANDEL_HEIGHT;

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
		draw_msg_text ("Tamanho = %dx%d", WINDOW_WIDTH, WINDOW_HEIGHT);
#endif	/*******************************************************/

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
	printf ("View: %dx%d\n", wp->width, wp->height);
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
		"%s - gerenciador gráfico de arquivos\n"
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
