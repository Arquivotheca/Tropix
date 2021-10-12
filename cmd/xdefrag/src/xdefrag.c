/*
 ****************************************************************
 *								*
 *			xdefrag.c				*
 *								*
 *  Melhora a alocação dos blocos de um sistema de arquivos T1	*
 *								*
 *	Versão	4.3.0, de 09.10.02				*
 *		4.5.0, de 02.11.03				*
 *								*
 *	Módulo: xdefrag						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2003 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/common.h>
#include <sys/disktb.h>
#include <sys/syscall.h>
#include <sys/scb.h>
#include <sys/t1.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Intrinsic.h>
#include <X11/keysym.h>
#include <X11/cursorfont.h>

#include "../h/xdefrag.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char		pgversion[] =  "Versão: 4.5.0, de 02.11.03";

#define	NOVOID		(void *)NULL
#define	elif		else if
#define	EVER		;;
#define STR(x)		# x
#define XSTR(x)		STR (x)

entry int		vflag;		/* Verbose */
entry int		rflag;		/* Somente leituras */

/*
 ****** Tamanhos das margens, sombras, ... **********************
 */
#define	BORDER		1
#define	SHADOW		3
#define BUTTON_WIDTH 	(2 * FONT_WIDTH)

#define	BLOCK_BORDER	(2 * FONT_WIDTH)
#define	BLOCK_HEIGHT	100

#define	PERCENT_BORDER	(2 * FONT_WIDTH)
#define	PERCENT_HEIGHT	50

entry int		BLOCK_WIDTH;

/*
 ****** Cores variáveis *****************************************
 */
#define	default_fore_color_str	"black"
#define	default_back_color_str	"gray"
#define	default_font_str	"9x15"

/*
 ****** Cores fixas *********************************************
 */
#define BORDER_COLOR		BlackPixel (display, screen)

#define	BLACK_COLOR_NM		"black"

#define	IGNORE_COLOR_NM		"black"
#define	BAD_COLOR_NM		"red"
#define	MOUNTED_COLOR_NM	"yellow"
#define	DEFRAG_COLOR_NM		"green"

#define	GOOD_BLOCK_COLOR_NM	"blue1"		/* ou "dodgerblue" */
#define	BAD_BLOCK_COLOR_NM	"red1"		/* ou "orangered"  */
#define	NO_BLOCK_COLOR_NM	"black"

#define	PERCENT_COLOR_NM	"green"

/*
 ******	Valores obtidos das cores *******************************
 */
typedef	struct
{
	Pixel		main_pixel, top_pixel,  bottom_pixel;
	GC		main_gc,    top_gc,	bottom_gc;

}	TRIPLEGC;

entry TRIPLEGC		fore_triple, back_triple, black_triple;

/*
 ******	Variáveis globais ***************************************
 */
entry Display		*display;

entry Window		main_win;

/*
 ******	Geometria ***********************************************
 */
XSizeHints	szhint =
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
 ******	Dados da Fonte ******************************************
 */
entry int		FONT_WIDTH, FONT_HEIGHT;	/* Tamanho da fonte */
entry int		FONT_DESCENT;

entry XFontStruct	*text_fip;

/*
 ******	Estrura de uma janela ***********************************
 */
typedef struct
{
	Window		win;
	int		width, height;
	int		x, y;
	TRIPLEGC	triple;
	GC		text_gc;

}	WINDATA;

#define	NOWINDATA 	(WINDATA *)NULL

/*
 ******	Dados da janela de MENU *********************************
 */
void		option_function (int value);
void		quit_function (int value);
void		examine_function (int value);
void		defrag_function (int value);

typedef struct
{
	const char	m_text[16];		/* Texto a escrever */
	void		(*m_func) (int);	/* Função a chamar */
	WINDATA		m_win;			/* Dados da janela */

}	MENUTB;	

MENUTB		menutb[] =
{
	{ "Opções",		option_function },	/* 0 */
	{ "Analisa",		examine_function },	/* 1 */
	{ "Desfragmenta",	defrag_function },	/* 2 */
	{ "Fim",		quit_function }		/* 3 */
};

#define	NMENU	(sizeof (menutb) / sizeof (MENUTB))

#define	MENU_BORDER	10

/*
 ******	Dados da janela das OPÇÕES ******************************
 */
void		do_method (void);
void		do_version (void);

entry WINDATA		option;

typedef struct
{
	const char	*m_text;		/* Texto a escrever */
	void		(*m_func) (void);	/* Função a chamar */

}	OPTIONVEC;

const OPTIONVEC	optionvec[] =
{
	"Método: ",		do_method,	/* 0 */
	"Versão: ",		do_version	/* 1 */
};

#define	option_longest_text	optionvec[1].m_text
#define	option_text_n		(sizeof (optionvec) / sizeof (OPTIONVEC))

entry int		option_highlight_line = -1;

/*
 ******	Dados da janela do MENU do método da desframentação *****
 */
entry WINDATA		method;

entry int		method_mapped;

typedef struct
{
	const char	*m_text;		/* Texto a escrever */
	int		m_method;		/* Método */

}	METHOD_VEC;

const METHOD_VEC	method_vec[] =
{
	"Apenas os arquivos",	  METHOD_FILE,			/* 0 */
	"Arquivos e lacunas",	  METHOD_FILE_AND_HOLE,		/* 1 */
	"Completo",		  METHOD_FTW			/* 2 */
};

#define	method_line_offset	0			/* Das opções, acima*/
#define	method_longest_text	method_vec[0].m_text
#define	method_text_n		(sizeof (method_vec) / sizeof (METHOD_VEC))

entry int		method_in_use = METHOD_FILE;	/* Método da defragmentação */

entry int		method_highlight_line = -1;

/*
 ******	Dados da janela da VERSÃO *******************************
 */
entry WINDATA		version;

entry int		version_mapped;

#define	version_line_offset	2			/* Das opções, acima */
#define	version_longest_text	pgversion
#define	version_text_n		1

/*
 ******	Dados da janela das MENSAGENS ***************************
 */
entry WINDATA		msg;

entry char		*msg_text[MSGLINES];

/*
 ******	Dados da janela dos DISPOSITIVOS ************************
 */
entry int		NDEV;			/* Uma quasi constante */

entry DEVTB		*devtb;			/* A tabela */

entry WINDATA		dev;

TRIPLEGC		button_ignore_triple,  button_defrag_triple,
			button_mounted_triple, button_bad_triple;

const char		*dev_state_nm[] = FS_STATUS_NM;

entry int		dev_highlight_index = -1;

entry int		dev_defrag_index = -1;

/*
 ******	Dados da janela de ERROS ********************************
 */
entry WINDATA		fatal;

char			fatal_msg[64], fatal_errno[64];

/*
 ******	Dados da janela da alocação ANTES ***********************
 */
entry GC		no_block_gc, good_block_gc, bad_block_gc;

entry WINDATA		before;

entry signed char	*before_block_map;

/*
 ******	Dados da janela da alocação DEPOIS **********************
 */
entry WINDATA		after;

entry signed char	*after_block_map;

/*
 ******	Dados da janela do PROGRESSO da desfragmentação *********
 */
entry WINDATA		percent;

entry GC		percent_gc;

entry int		percent_old_width;

/*
 ****** Protótipos de funções ***********************************
 */
void		event_loop (int main_event_loop);
int		get_pointer_y (Window win);
void		draw_menu_win (const MENUTB *mp);
void		draw_option_win (void);
void		motion_option_window (int y);
void		leave_option_window (void);
void		draw_method_win (void);
void		do_method_button_press (int y);
void		motion_method_window (int y);
void		leave_method_window (void);
void		draw_version_win (void);
void		Shadow_up (const MENUTB *mp);
void		Shadow_off (const MENUTB *mp);
const TRIPLEGC	*get_button_triple (const DEVTB *dp, int index);
void		draw_dev_win (void);
void		draw_dev_line (const DEVTB *dp, int index);
void		motion_dev_window (int y);
void		leave_dev_window (void);
void		update_dev_window (int y);
void		draw_dev_button (int index, const TRIPLEGC *tp);
void		draw_msg_win (void);
void		draw_fatal_window (void);
void		draw_before_window (void);
void		draw_after_window (void);
void		draw_percent_window (void);
void		draw_shadow (Window, GC, GC, int, int, int width, int height);
int		set_shadow_colors (Display *, Window, TRIPLEGC *, const char *, Pixel);
void		set_window_sizes (int width, int height);
Pixel		get_window_color (const char *color_nm, Pixel def);
void		geometry_to_size_hints (const char *geo_str, XSizeHints *hp);
void		do_XFlush (void);
void		quit_function (int value);
const char	*edit_event_nm (int event);
void		help (void);

/*
 ****************************************************************
 *  Melhora a alocação dos blocos de um sistema de arquivos T1	*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	int		opt;
	const char	*geo_str = NOSTR;
	int		save_argc = argc;
	const char	**save_argv = argv;
	const char	*display_str = NOSTR;
	const char	*font_str, *user_font_str = NOSTR;
	const char	*fore_color_str, *user_fore_color_str = NOSTR;
	const char	*back_color_str, *user_back_color_str = NOSTR;
	int		screen;
	Window		root, win;
	int		text_fid;
	XClassHint	classhint;
	XWMHints	wmhint;
	XGCValues	gcv;
	GC		text_gc;
	const char	*str;
	MENUTB		*mp;
	char		text[64];
	SCB		scb;

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "rvg:f:b:d:F:H")) != EOF)
	{
		switch (opt)
		{
		    case 'r':			/* Somente leituras */
			rflag++;
			break;

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
	 *	Obtém o SCB
	 */
	if ((int)getscb (&scb) == -1)
		error ("$Não consegui obter o SCB");

	/*
	 ****** Obtém logo uma primeira versão da tabela ********
	 */
	get_devtb ();

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
	 ****** Tenta obter os valores de "XDefault" ************
	 */
	if   (user_fore_color_str != NOSTR)
		fore_color_str = user_fore_color_str;
	elif ((str = XGetDefault (display, "XDefrag", "foreground")) != NOSTR)
		fore_color_str = str;
	else
		fore_color_str = default_fore_color_str;

	if   (user_back_color_str != NOSTR)
		back_color_str = user_back_color_str;
	elif ((str = XGetDefault (display, "XDefrag", "background")) != NOSTR)
		back_color_str = str;
	else
		back_color_str = default_back_color_str;

	if   (user_font_str != NOSTR)
		font_str = user_font_str;
	elif ((str = XGetDefault (display, "XDefrag", "font")) != NOSTR)
		font_str = str;
	else
		font_str = default_font_str;

	/*
	 ******	Obtém as cores fixas e variáveis ****************
	 */
	set_shadow_colors (display, root, &fore_triple, fore_color_str, 0x000000);
	set_shadow_colors (display, root, &back_triple, back_color_str, 0xBEBEBE);

	set_shadow_colors (display, root, &black_triple, BLACK_COLOR_NM, 0x000000);

	/*
	 ******	Obtém os dados da fonte *************************
	 */
	if ((text_fip = XLoadQueryFont (display, font_str)) == NULL)
		error ("$Não encontrei a fonte \"%s\"", font_str);

	text_fid = text_fip->fid;

	FONT_WIDTH	= XTextWidth (text_fip, "MMMMMMMMMM", 10) / 10;
	FONT_DESCENT	= text_fip->descent;
	FONT_HEIGHT	= text_fip->ascent + text_fip->descent;

	/*
	 ****** Cria a janela PRINCIPAL	*************************
	 */
	geometry_to_size_hints (geo_str, &szhint);

	set_window_sizes (0, 0);

        win = XCreateSimpleWindow
	(	display, root,
		0, 0,
		szhint.min_width, szhint.min_height,
		0, BORDER_COLOR,
		BlackPixel (display, screen)
	);

	main_win = win;

	sprintf (text, "Xdefrag (%s)", scb.y_uts.uts_nodename);
	XStoreName (display, win, text);

	XSelectInput (display, win, ExposureMask|KeyPressMask|StructureNotifyMask);

	classhint.res_name  = (char *)pgname;
	classhint.res_class = "XDefrag";

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
	gcv.font	= text_fid;
	gcv.line_width  = 1;

	gcv.foreground  = fore_triple.main_pixel;
	gcv.background  = back_triple.main_pixel;
	text_gc = XCreateGC (display, win, GCForeground|GCBackground|GCFont, &gcv);

	for (mp = &menutb[0]; mp < &menutb[NMENU]; mp++)
	{
		memmove (&mp->m_win.triple, &back_triple, sizeof (TRIPLEGC));
		mp->m_win.text_gc = text_gc;

		mp->m_win.win = XCreateSimpleWindow
		(
			display, win,
			mp->m_win.x, mp->m_win.y,
			mp->m_win.width, mp->m_win.height,
			BORDER, BORDER_COLOR,
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
	option.text_gc = text_gc;

        option.win = XCreateSimpleWindow
	(	display, win,
		option.x, option.y,
		option.width, option.height,
		BORDER, BORDER_COLOR,
		option.triple.main_pixel
	);

	XSelectInput
	(	display, option.win,
		(ExposureMask|EnterWindowMask|LeaveWindowMask|
		PointerMotionMask|PointerMotionHintMask|ButtonPressMask)
	);

   /***	XMapWindow (display, option.win); ***/

	/*
	 ****** Prepara a janela do MENU de MÉTODOS *************
	 */
	memmove (&method.triple, &back_triple, sizeof (TRIPLEGC));
	method.text_gc = text_gc;

        method.win = XCreateSimpleWindow
	(	display, win,
		method.x, method.y,
		method.width, method.height,
		BORDER, BORDER_COLOR,
		option.triple.main_pixel
	);

	XSelectInput
	(	display, method.win,
		(ExposureMask|EnterWindowMask|LeaveWindowMask|
		PointerMotionMask|PointerMotionHintMask|ButtonPressMask)
	);

   /***	XMapWindow (display, method.win); ***/

	/*
	 ****** Prepara a janela da VERSÃO **********************
	 */
	memmove (&version.triple, &back_triple, sizeof (TRIPLEGC));
	version.text_gc = text_gc;

        version.win = XCreateSimpleWindow
	(	display, win,
		version.x, version.y,
		version.width, version.height,
		BORDER, BORDER_COLOR,
		option.triple.main_pixel
	);

	XSelectInput
	(	display, version.win,
		(ExposureMask|EnterWindowMask|LeaveWindowMask|
		PointerMotionMask|PointerMotionHintMask|ButtonPressMask)
	);

   /***	XMapWindow (display, version.win); ***/

	/*
	 ****** Cria as janelas dos DISPOSITIVOS ****************
	 */
	set_shadow_colors (display, win, &button_ignore_triple,  IGNORE_COLOR_NM,  0x000000);
	set_shadow_colors (display, win, &button_defrag_triple,  DEFRAG_COLOR_NM,  0x0000CC);
	set_shadow_colors (display, win, &button_mounted_triple, MOUNTED_COLOR_NM, 0xCCCC00);
	set_shadow_colors (display, win, &button_bad_triple,     BAD_COLOR_NM,     0xCC0000);

	memmove (&dev.triple, &back_triple, sizeof (TRIPLEGC));
	dev.text_gc = text_gc;

	dev.win = XCreateSimpleWindow
	(
		display, win,
		dev.x, dev.y,
		dev.width, dev.height,
		BORDER, BORDER_COLOR,
		dev.triple.main_pixel
	);

	XSelectInput
	(	display, dev.win,
		(ExposureMask|LeaveWindowMask|PointerMotionMask|PointerMotionHintMask|ButtonPressMask)
	);

	XMapWindow (display, dev.win);

	/*
	 ****** Cria a janela das MENSAGENS *********************
	 */
	memmove (&msg.triple, &back_triple, sizeof (TRIPLEGC));
	msg.text_gc = text_gc;

	msg.win = XCreateSimpleWindow
	(
		display, win,
		msg.x, msg.y,
		msg.width, msg.height,
		BORDER, BORDER_COLOR,
		msg.triple.main_pixel
	);

	XSelectInput
	(	display, msg.win,
		ExposureMask
	);

	XMapWindow (display, msg.win);

	/*
	 ****** Cria a janela de ERRO ***************************
	 */
	memmove (&fatal.triple, &button_bad_triple, sizeof (TRIPLEGC));
	fatal.text_gc = text_gc;

	fatal.win = XCreateSimpleWindow
	(
		display, win,
		fatal.x, fatal.y,
		fatal.width, fatal.height,
		BORDER, BORDER_COLOR,
		fatal.triple.main_pixel
	);

	XSelectInput
	(	display, fatal.win,
		(ExposureMask|ButtonPressMask)
	);

   /***	XMapWindow (display, fatal.win); ***/

	/*
	 ****** Cria a janela da alocação ANTES *****************
	 */
	gcv.foreground  = get_window_color (NO_BLOCK_COLOR_NM, 0x000000);
	no_block_gc = XCreateGC (display, win, GCForeground, &gcv);

	gcv.foreground  = get_window_color (GOOD_BLOCK_COLOR_NM, 0xFFFFFF);
	good_block_gc = XCreateGC (display, win, GCForeground, &gcv);

	gcv.foreground  = get_window_color (BAD_BLOCK_COLOR_NM, 0x777777);
	bad_block_gc = XCreateGC (display, win, GCForeground, &gcv);

	memmove (&before.triple, &back_triple, sizeof (TRIPLEGC));
	before.text_gc = text_gc;

	before.win = XCreateSimpleWindow
	(
		display, win,
		before.x, before.y,
		before.width, before.height,
		BORDER, BORDER_COLOR,
		before.triple.main_pixel
	);

	XSelectInput
	(	display, before.win,
		ExposureMask
	);

	XMapWindow (display, before.win);

	/*
	 ****** Cria a janela da alocação DEPOIS ****************
	 */
	memmove (&after.triple, &back_triple, sizeof (TRIPLEGC));
	after.text_gc = text_gc;

	after.win = XCreateSimpleWindow
	(
		display, win,
		after.x, after.y,
		after.width, after.height,
		BORDER, BORDER_COLOR,
		after.triple.main_pixel
	);

	XSelectInput
	(	display, after.win,
		ExposureMask
	);

	XMapWindow (display, after.win);

	/*
	 ****** Cria a janela de PROGRESSO da alocação **********
	 */
	gcv.foreground  = get_window_color (PERCENT_COLOR_NM, 0x00CC00);
	percent_gc = XCreateGC (display, win, GCForeground, &gcv);

	memmove (&percent.triple, &back_triple, sizeof (TRIPLEGC));
	percent.text_gc = text_gc;

	percent.win = XCreateSimpleWindow
	(
		display, win,
		percent.x, percent.y,
		percent.width, percent.height,
		BORDER, BORDER_COLOR,
		percent.triple.main_pixel
	);

	XSelectInput
	(	display, percent.win,
		ExposureMask
	);

	XMapWindow (display, percent.win);

	/*
	 ******	Entra na malha de eventos *******************************
	 */
	event_loop (1);

	/* Não retorna */

	return (0);

}	/* end xdefrag */

/*
 ****************************************************************
 *	Malha de eventos					*
 ****************************************************************
 */
void
event_loop (int main_event_loop)
{
	XEvent		ev;
	MENUTB		*mp;

	for (EVER)
	{
		XNextEvent (display, &ev);

#undef	DEBUG
#ifdef	DEBUG
		printf ("Recebi o Evento \"%s\" (%d)\n", edit_event_nm (ev.type), ev.type);
#endif	DEBUG

		switch (ev.type)
		{
		    case Expose:
		    case GraphicsExpose:
			if (ev.xexpose.count != 0)
				break;

			if   (ev.xany.window == option.win)
			{
				draw_option_win ();
			}
			elif (ev.xany.window == method.win)
			{
				draw_method_win ();
			}
			elif (ev.xany.window == version.win)
			{
				draw_version_win ();
			}
			elif (ev.xany.window == dev.win)
			{
				draw_dev_win ();
			}
			elif (ev.xany.window == msg.win)
			{
				draw_msg_win ();
			}
			elif (ev.xany.window == fatal.win)
			{
				draw_fatal_window ();
			}
			elif (ev.xany.window == before.win)
			{
				draw_before_window ();
			}
			elif (ev.xany.window == after.win)
			{
				draw_after_window ();
			}
			elif (ev.xany.window == percent.win)
			{
				draw_percent_window ();
			}
			else for (mp = &menutb[0]; mp < &menutb[NMENU]; mp++)
			{
				if (mp->m_win.win == ev.xany.window)
				{
					draw_menu_win (mp); break;
				}
			}

			break;

		    case EnterNotify:
			if   (ev.xany.window == dev.win)
			{
				goto unmap_option;
			}
			else for (mp = &menutb[0]; mp < &menutb[NMENU]; mp++)
			{
				if (mp->m_win.win == ev.xany.window)
				{
					Shadow_up (mp);

					if (mp->m_win.win != option.win)
					{
					    unmap_option:
					   	XUnmapWindow (display, option.win);
						option_highlight_line = -1;

					   	XUnmapWindow (display, method.win);
						method_mapped = 0;
						method_highlight_line = -1;

					   	XUnmapWindow (display, version.win);
						version_mapped = 0;
					}

					break;
				}
			}

			break;

		    case LeaveNotify:
			if   (ev.xany.window == option.win)
			{
				if (method_mapped || version_mapped)
					break;

			   	XUnmapWindow (display, option.win);

				option_highlight_line = -1;
			}
			elif (ev.xany.window == method.win)
			{
			   	XUnmapWindow (display, option.win);
			   	XUnmapWindow (display, method.win);

				method_mapped = 0;
				method_highlight_line = -1;
			}
			elif (ev.xany.window == version.win)
			{
			   	XUnmapWindow (display, option.win);
			   	XUnmapWindow (display, version.win);

				version_mapped = 0;
			}
			elif (ev.xany.window == dev.win)
			{
				leave_dev_window ();
			}
			else for (mp = &menutb[0]; mp < &menutb[NMENU]; mp++)
			{
				if (mp->m_win.win == ev.xany.window)
				{
					Shadow_off (mp); break;
				}
			}

			break;

		    case ButtonPress:
			if (main_event_loop == 0)
				return;

			if   (ev.xany.window == method.win)
			{
			   	do_method_button_press (ev.xbutton.y);
			}
			elif (ev.xany.window == dev.win)
			{
				update_dev_window (ev.xbutton.y);
			}
			elif (ev.xany.window == fatal.win)
			{
				XUnmapWindow (display, fatal.win);
			}
			else for (mp = &menutb[0]; mp < &menutb[NMENU]; mp++)
			{
				if (mp->m_win.win == ev.xany.window)
					{ (*mp->m_func) (0); break; }
			}

			break;

		    case MotionNotify:
			if   (ev.xany.window == option.win)
				motion_option_window (get_pointer_y (option.win));
			elif (ev.xany.window == method.win)
				motion_method_window (get_pointer_y (method.win));
			elif (ev.xany.window == dev.win)
				motion_dev_window (get_pointer_y (dev.win));

			break;

		    case ConfigureNotify:
		    {
			XEvent		Unused_xevent;
			int		x, y;
			unsigned int	width, height, border;
			Window		root;
			unsigned int	depth;

			if (main_event_loop == 0)
				return;

			while
			(	XCheckTypedWindowEvent
				(	display, main_win,
					ConfigureNotify, &Unused_xevent
				)
			)
				/* vazio */;

			XGetGeometry
			(	display, main_win, &root,
				&x, &y, &width, &height,
				&border, &depth
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
		MENU_BORDER + FONT_HEIGHT - FONT_DESCENT,
		mp->m_text, sizeof (mp->m_text) - 1
	);

}	/* end draw_menu_win */

/*
 ****************************************************************
 *	Desenha a janela de OPÇÕES				*
 ****************************************************************
 */
void
draw_option_win (void)
{
	int		line;
	const char	*str;

	draw_shadow
	(	option.win,
		option.triple.top_gc, option.triple.bottom_gc,
		0, 0,
		option.width, option.height
	);

	for (line = 0; line < option_text_n; line++)
	{
		str = optionvec[line].m_text;

		XDrawString
		(	display, option.win,
			option.text_gc,
			MENU_BORDER + SHADOW,
			MENU_BORDER + (line + 1) * (FONT_HEIGHT + 2 * SHADOW) - (FONT_DESCENT + SHADOW),
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

	line = (y - MENU_BORDER) / (FONT_HEIGHT + 2 * SHADOW);

	if ((unsigned)line >= option_text_n)
		return;

	if (line == option_highlight_line)
		return;

	if (option_highlight_line >= 0)
		leave_option_window ();

	draw_shadow
	(	option.win,
		option.triple.top_gc, option.triple.bottom_gc,
		MENU_BORDER, MENU_BORDER + line * (FONT_HEIGHT + 2 * SHADOW),
		option.width - 2 * MENU_BORDER, (FONT_HEIGHT + 2 * SHADOW)
	);

	option_highlight_line = line;

	mp = &optionvec[line];

	(*mp->m_func) ();

}	/* end motion_option_window */

/*
 ****************************************************************
 *	Saí da janela das OPÇÕES				*
 ****************************************************************
 */
void
leave_option_window (void)
{
	int		line;

	if ((line = option_highlight_line) < 0)
		return;

	draw_shadow
	(	option.win,
		option.triple.main_gc, option.triple.main_gc,
		MENU_BORDER, MENU_BORDER + line * (FONT_HEIGHT + 2 * SHADOW),
		option.width - 2 * MENU_BORDER, (FONT_HEIGHT + 2 * SHADOW)
	);

	option_highlight_line = -1;

}	/* end leave_option_window */

/*
 ****************************************************************
 *	Mapeia a janela de escolher o método			*
 ****************************************************************
 */
void
do_method (void)
{
   	XUnmapWindow (display, version.win);
	version_mapped = 0;

   	XMapRaised (display, method.win);
	method_mapped = 1;

}	/* end do_method */

/*
 ****************************************************************
 *	Desenha o menu de métodos				*
 ****************************************************************
 */
void
draw_method_win (void)
{
	int		line;
	const char	*str;
	const TRIPLEGC	*tp;

	draw_shadow
	(	method.win,
		method.triple.top_gc, method.triple.bottom_gc,
		0, 0,
		method.width, method.height
	);

	for (line = 0; line < method_text_n; line++)
	{
		str = method_vec[line].m_text;

		XDrawString
		(	display, method.win,
			method.text_gc,
			MENU_BORDER + SHADOW,
			MENU_BORDER + (line + 1) * (FONT_HEIGHT + 2 * SHADOW) - (FONT_DESCENT + SHADOW),
			str, strlen (str)
		);

		if (method_vec[line].m_method == method_in_use)
			tp = &button_defrag_triple;
		else
			tp = &button_ignore_triple;

		XFillRectangle
		(	display, method.win, tp->main_gc,
			method.width - BUTTON_WIDTH - MENU_BORDER - SHADOW, 
			MENU_BORDER + line * (FONT_HEIGHT + 2 * SHADOW) + SHADOW,
			BUTTON_WIDTH - 2, FONT_HEIGHT - 2
		);

		draw_shadow
		(	method.win,
			tp->top_gc, tp->bottom_gc,
			method.width - BUTTON_WIDTH - MENU_BORDER - SHADOW, 
			MENU_BORDER + line * (FONT_HEIGHT + 2 * SHADOW) + SHADOW,
			BUTTON_WIDTH - 2, FONT_HEIGHT - 2
		);
	}

}	/* end draw_method_win */

/*
 ****************************************************************
 *	Entrei na janela dos MÉTODOS				*
 ****************************************************************
 */
void
motion_method_window (int y)
{
	int		line;

	line = (y - MENU_BORDER) / (FONT_HEIGHT + 2 * SHADOW);

	if ((unsigned)line >= method_text_n)
		return;

	if (line == method_highlight_line)
		return;

	if (method_highlight_line >= 0)
		leave_method_window ();

	draw_shadow
	(	method.win,
		method.triple.top_gc, method.triple.bottom_gc,
		MENU_BORDER, MENU_BORDER + line * (FONT_HEIGHT + 2 * SHADOW),
		method.width - 3 * MENU_BORDER - BUTTON_WIDTH, (FONT_HEIGHT + 2 * SHADOW)
	);

	method_highlight_line = line;

}	/* end enter method_window */

/*
 ****************************************************************
 *	Saí da janela dos MÉTODOS				*
 ****************************************************************
 */
void
leave_method_window (void)
{
	int		line;

	if ((line = method_highlight_line) < 0)
		return;

	draw_shadow
	(	method.win,
		method.triple.main_gc, method.triple.main_gc,
		MENU_BORDER, MENU_BORDER + line * (FONT_HEIGHT + 2 * SHADOW),
		method.width - 3 * MENU_BORDER - BUTTON_WIDTH, (FONT_HEIGHT + 2 * SHADOW)
	);

	method_highlight_line = -1;

}	/* end leave_method_window */

/*
 ****************************************************************
 *	Apertou a tecla de um MÉTODO				*
 ****************************************************************
 */
void
do_method_button_press (int y)
{
	int		line;
	const TRIPLEGC	*tp;

	if (method_highlight_line < 0)
		return;

	method_in_use = method_vec[method_highlight_line].m_method;

	for (line = 0; line < method_text_n; line++)
	{
		if (line == method_highlight_line)
			tp = &button_defrag_triple;
		else
			tp = &button_ignore_triple;

		XFillRectangle
		(	display, method.win, tp->main_gc,
			method.width - BUTTON_WIDTH - MENU_BORDER - SHADOW, 
			MENU_BORDER + line * (FONT_HEIGHT + 2 * SHADOW) + SHADOW,
			BUTTON_WIDTH - 2, FONT_HEIGHT - 2
		);

		draw_shadow
		(	method.win,
			tp->top_gc, tp->bottom_gc,
			method.width - BUTTON_WIDTH - MENU_BORDER - SHADOW, 
			MENU_BORDER + line * (FONT_HEIGHT + 2 * SHADOW) + SHADOW,
			BUTTON_WIDTH - 2, FONT_HEIGHT - 2
		);
	}

}	/* end do_method_button_press */

/*
 ****************************************************************
 *	Mapeia a janela da versão				*
 ****************************************************************
 */
void
do_version (void)
{
   	XUnmapWindow (display, method.win);
	method_mapped = 0;
	method_highlight_line = -1;

   	XMapRaised (display, version.win);
	version_mapped = 1;

}	/* end do_version */

/*
 ****************************************************************
 *	Desenha a janela da versão				*
 ****************************************************************
 */
void
draw_version_win (void)
{
	draw_shadow
	(	version.win,
		version.triple.top_gc, version.triple.bottom_gc,
		0, 0,
		version.width, version.height
	);

	XDrawString
	(	display, version.win,
		version.text_gc,
		MENU_BORDER + SHADOW,
		MENU_BORDER + (FONT_HEIGHT + 2 * SHADOW) - (FONT_DESCENT + SHADOW),
		pgversion, strlen (pgversion)
	);

}	/* end draw_version_win */

/*
 ****************************************************************
 *	Levanta um "botão"					*
 ****************************************************************
 */
void
Shadow_up (const MENUTB *mp)
{
	draw_shadow
	(	mp->m_win.win,
		mp->m_win.triple.top_gc, mp->m_win.triple.bottom_gc,
		0, 0,
		mp->m_win.width, mp->m_win.height
	);

}	/* end Shadow_up */

/*
 ****************************************************************
 *	Tira a moldura de um "botão"				*
 ****************************************************************
 */
void
Shadow_off (const MENUTB *mp)
{
	draw_shadow
	(	mp->m_win.win,
		mp->m_win.triple.main_gc, mp->m_win.triple.main_gc,
		0, 0,
		mp->m_win.width, mp->m_win.height
	);

}	/* end Shadow_up */

/*
 ****************************************************************
 *	Desenha a janela de dispositivos			*
 ****************************************************************
 */
void
draw_dev_win (void)
{
	int		index;
	const DEVTB	*dp;
	const TRIPLEGC	*tp;

	draw_shadow
	(	dev.win,
		dev.triple.top_gc, dev.triple.bottom_gc,
		0, 0,
		dev.width, dev.height
	);

	for (index = 0, dp = devtb; index < NDEV; index++, dp++)
	{
		tp = get_button_triple (dp, index);

		draw_dev_button (index, tp);

		draw_dev_line (dp, index);
	}

}	/* end draw_dev_win */

/*
 ****************************************************************
 *	Obtém a cor correta do botão				*
 ****************************************************************
 */
const TRIPLEGC *
get_button_triple (const DEVTB *dp, int index)
{
	if   (index == dev_defrag_index)
		return (&button_defrag_triple);

	switch (dp->d_state)
	{
	    case FS_NOSTAT:
		return (&button_bad_triple);

	    case FS_MOUNTED:
		return (&button_mounted_triple);

	    case FS_NOTREADY:
		return (&button_bad_triple);

	    case FS_RONLY:
		return (&button_mounted_triple);

	    case FS_NOREAD:
	    case FS_NOTT1:
		return (&button_bad_triple);

	    case FS_OK:
		return (&button_ignore_triple);

	    default:
		return (&button_bad_triple);

	}	/* end switch (dp->d_state) */


}	/* end get_button_triple */

/*
 ****************************************************************
 *	Escreve a linha de um dispositivo			*
 ****************************************************************
 */
void
draw_dev_line (const DEVTB *dp, int index)
{
	char		text[64];

	snprintf
	(	text, sizeof (text),
		"%-8s %-11s %7s MB  %s",
		dp->d_disktb.p_name,
		dp->d_fname,
		cv_blocks_to_MB (dp->d_disktb.p_size),
		dev_state_nm[dp->d_state]
	);

	XDrawImageString
	(	display, dev.win,
		dev.text_gc,
		BUTTON_WIDTH + 3 * BORDER + MENU_BORDER + SHADOW,
		(index + 1) * (FONT_HEIGHT + 2 * SHADOW) - FONT_DESCENT + MENU_BORDER - SHADOW,
		text, strlen (text)
	);

}	/* end draw_dev_line */

/*
 ****************************************************************
 *	Entrando em uma linha de DISPOSITIVOS			*
 ****************************************************************
 */
void
motion_dev_window (int y)
{
	int		index;

	index = (y - MENU_BORDER) / (FONT_HEIGHT + 2 * SHADOW);

	if ((unsigned)index >= NDEV)
		return;

	if (index == dev_highlight_index)
		return;

	if (dev_highlight_index >= 0)
		leave_dev_window ();

	draw_shadow
	(	dev.win,
		dev.triple.top_gc, dev.triple.bottom_gc,
		BUTTON_WIDTH + 2 * BORDER + MENU_BORDER,
		MENU_BORDER + index * (FONT_HEIGHT + 2 * SHADOW),
		dev.width - 2 * MENU_BORDER - BUTTON_WIDTH - 2 * BORDER, (FONT_HEIGHT + 2 * SHADOW)
	);

	dev_highlight_index = index;

}	/* end motion_dev_window */

/*
 ****************************************************************
 *	Sainda de uma linha de DISPOSITIVOS			*
 ****************************************************************
 */
void
leave_dev_window (void)
{
	int		index;

	if ((index = dev_highlight_index) < 0)
		return;

	draw_shadow
	(	dev.win,
		dev.triple.main_gc, dev.triple.main_gc,
		BUTTON_WIDTH + 2 * BORDER + MENU_BORDER,
		MENU_BORDER + index * (FONT_HEIGHT + 2 * SHADOW),
		dev.width - 2 * MENU_BORDER - BUTTON_WIDTH - 2 * BORDER, (FONT_HEIGHT + 2 * SHADOW)
	);

	dev_highlight_index = -1;

}	/* end leave_dev_window */

/*
 ****************************************************************
 *	Escolhe de um dispositivo				*
 ****************************************************************
 */
void
update_dev_window (int y)
{
	const TRIPLEGC	*tp;
	int		index, old_state, code;
	DEVTB		*dp;
	char		path[16] = "/dev/";
	char		*cp_point = path + 5;

	/*
	 *	Examina o novo candidato
	 */
	if ((index = dev_highlight_index) < 0)
		return;

	dp = &devtb[index]; old_state = dp->d_state;

	examine_device (dp);	/* Reexamina o dispositivo */

	if  (dp->d_state != old_state)
		draw_dev_line (dp, index);

	switch (dp->d_state)
	{
	    case FS_NOSTAT:
		return;

	    case FS_MOUNTED:
		code = O_RDONLY;
		break;

	    case FS_NOTREADY:
		return;

	    case FS_RONLY:
		code = O_RDONLY;
		break;

	    case FS_NOREAD:
	    case FS_NOTT1:
		return;

	    case FS_OK:
		code = O_RDWR;
		break;

	    default:
		return;

	}	/* end switch (dp->d_state) */

	/*
	 *	Põe o dispositivo anterior com a cor original
	 */
	if (dev_defrag_index >= 0)
	{
		tp = get_button_triple (&devtb[dev_defrag_index], -1);

		draw_dev_button (dev_defrag_index, tp);
	}

	/*
	 *	O Candidato foi aprovado
	 */
	draw_dev_button (index, &button_defrag_triple);

	dev_defrag_index = index;

	if (dev_fd >= 0)
		close (dev_fd);

	strcpy (cp_point, dp->d_disktb.p_name);

	if ((dev_fd = open (path, code)) < 0)
		xerror ("*Não consegui abrir o dispositivo");

}	/* end update_dev_window */

/*
 ****************************************************************
 *	Desenha um botão da janela de DISPOSITIVOS		*
 ****************************************************************
 */
void
draw_dev_button (int index, const TRIPLEGC *tp)
{
	XFillRectangle
	(	display, dev.win, tp->main_gc,
		MENU_BORDER + BORDER + 1,
		index * (FONT_HEIGHT + 2 * SHADOW) + MENU_BORDER + SHADOW + 1,
		BUTTON_WIDTH - 2, FONT_HEIGHT - 2
	);

	draw_shadow
	(	dev.win,
		tp->top_gc, tp->bottom_gc,
		MENU_BORDER + BORDER + 1,
		index * (FONT_HEIGHT + 2 * SHADOW) + MENU_BORDER + SHADOW + 1,
		BUTTON_WIDTH - 2, FONT_HEIGHT - 2
	);

}	/* end Draw_dev_button */

/*
 ****************************************************************
 *	Limpa a janela de MENSAGENS				*
 ****************************************************************
 */
void
clear_msg_window (void)
{
	XClearWindow (display, msg.win);

	draw_shadow
	(	msg.win,
		msg.triple.top_gc, msg.triple.bottom_gc,
		0, 0,
		msg.width, msg.height
	);

}	/* end clear_msg_window */

/*
 ****************************************************************
 *	Desenha a janela de MENSAGENS				*
 ****************************************************************
 */
void
draw_msg_win (void)
{
	int		index;
	const char	*text;

	draw_shadow
	(	msg.win,
		msg.triple.top_gc, msg.triple.bottom_gc,
		0, 0,
		msg.width, msg.height
	);

	for (index = 0; index < MSGLINES; index++)
	{
		if ((text = msg_text[index]) == NOSTR)
			continue;

		XDrawImageString
		(	display, msg.win,
			msg.text_gc,
			BUTTON_WIDTH + 3 * BORDER + MENU_BORDER + SHADOW,
			(index + 1) * (FONT_HEIGHT + 2 * SHADOW) - FONT_DESCENT + MENU_BORDER - SHADOW,
			text, strlen (text)
		);
	}

}	/* end draw_msg_win */

/*
 ****************************************************************
 *	Imprime uma mensagem de erro				*
 ****************************************************************
 */
int
xerror (const char *format, ...)
{
	va_list		args;
	const char	*fmt = format;
	char		err, abrt;
	int		save_errno = errno;

	va_start (args, format);

	if (fmt == NOSTR)
		fmt = "Memória esgotada";

	if ((abrt = *fmt) == '$')
		fmt++;

	if ((err = *fmt) == '*')
		fmt++;

	vsnprintf (fatal_msg, sizeof (fatal_msg), fmt, args);

	if (err == '*' && save_errno != 0)
		snprintf (fatal_errno, sizeof (fatal_errno), "(%s)", strerror (save_errno));
	else
		fatal_errno[0] = '\0';

	va_end (args);

	XMapRaised (display, fatal.win);

#if (0)	/*******************************************************/
	if (abrt == '$')
		return (-1);
#endif	/*******************************************************/

	return (0);

}	/* end xerror */

/*
 ****************************************************************
 *	Desenha a janela de ERRO FATAL				*
 ****************************************************************
 */
void
draw_fatal_window (void)
{
	int		len;

	if (fatal_msg == NOSTR)
		return;

	draw_shadow
	(	fatal.win,
		fatal.triple.top_gc, fatal.triple.bottom_gc,
		0, 0,
		fatal.width, fatal.height
	);

	len = strlen (fatal_msg);

	XDrawString
	(	display, fatal.win,
		fatal.text_gc,
		(fatal.width - len * FONT_WIDTH) >> 1,
		MENU_BORDER + 0 * (FONT_HEIGHT + 2 * SHADOW) + SHADOW + FONT_HEIGHT - FONT_DESCENT,
		fatal_msg, len
	);

	if ((len = strlen (fatal_errno)) > 0)
	{
		XDrawString
		(	display, fatal.win,
			fatal.text_gc,
			(fatal.width - len * FONT_WIDTH) >> 1,
			MENU_BORDER + 1 * (FONT_HEIGHT + 2 * SHADOW) + SHADOW + FONT_HEIGHT - FONT_DESCENT,
			fatal_errno, len
		);
	}

	XFillRectangle
	(	display, fatal.win,
		button_defrag_triple.main_gc,
		(fatal.width - 10 * FONT_WIDTH) >> 1,
		MENU_BORDER + 3 * (FONT_HEIGHT + 2 * SHADOW),
		10 * FONT_WIDTH,
		(FONT_HEIGHT + 2 * SHADOW)
	);

	draw_shadow
	(	fatal.win,
		button_defrag_triple.top_gc, button_defrag_triple.bottom_gc,
		(fatal.width - 10 * FONT_WIDTH) >> 1,
		MENU_BORDER + 3 * (FONT_HEIGHT + 2 * SHADOW),
		10 * FONT_WIDTH,
		(FONT_HEIGHT + 2 * SHADOW)
	);

	XDrawString
	(	display, fatal.win,
		fatal.text_gc,
		(fatal.width - 8 * FONT_WIDTH) >> 1,
		MENU_BORDER + 3 * (FONT_HEIGHT + 2 * SHADOW) + SHADOW + FONT_HEIGHT - FONT_DESCENT,
#if (0)	/*******************************************************/
		(fatal.height >> 1) + SHADOW + FONT_HEIGHT - FONT_DESCENT,
#endif	/*******************************************************/
		"Continua", sizeof ("Continua") - 1
	);

}	/* end Draw_fatal_window */

/*
 ****************************************************************
 *	Limpa a janela da alocação ANTES			*
 ****************************************************************
 */
void
clear_before_window (void)
{
	XFillRectangle
	(	display, before.win, no_block_gc,
		BLOCK_BORDER + SHADOW, BLOCK_BORDER + SHADOW,
		BLOCK_WIDTH, BLOCK_HEIGHT
	);

}	/* end clear_before_window */

/*
 ****************************************************************
 *	Desenha a janela da alocação ANTES			*
 ****************************************************************
 */
void
draw_before_window (void)
{
	draw_shadow
	(	before.win,
		before.triple.top_gc, before.triple.bottom_gc,
		0, 0,
		before.width, before.height
	);

	draw_shadow
	(	before.win,
		before.triple.top_gc, before.triple.bottom_gc,
		BLOCK_BORDER, BLOCK_BORDER,
		before.width - 2 * BLOCK_BORDER, before.height - 2 * BLOCK_BORDER
	);

	clear_before_window ();

	if (before_map_ready)
		draw_before_blocks ();

}	/* end draw_before_window */

/*
 ****************************************************************
 *	Calcula as barras da alocação ANTES			*
 ****************************************************************
 */
int
compute_before_blocks (void)
{
	int			blocks_per_line, i;
	signed char		*cp;
	const signed char	*bp, *end_bp, *end_cp;
	signed char		code;

	/*
	 *	0: Vago, 1: bem alocado, -1: mal alocado
	 *
	 *	Aloca o vetor
	 */
	if (before_block_map != NULL)
		{ free (before_block_map); before_block_map = NULL; }

	blocks_per_line = (fs_sz + BLOCK_WIDTH - 1) / BLOCK_WIDTH;

#ifdef	DEBUG
	printf ("BLOCK_WIDTH = %d, fs_sz = %d, blocks_per_line = %d\n", BLOCK_WIDTH, fs_sz, blocks_per_line);
#endif	DEBUG

	if ((before_block_map = cp = malloc (BLOCK_WIDTH)) == NULL)
		{ xerror (NOSTR); return (-1); }

	memset (before_block_map, 0, BLOCK_WIDTH);

	/*
	 *	Varre o mapa de blocos
	 */
	bp = before_block_code; end_bp = bp + fs_sz; end_cp = cp + BLOCK_WIDTH;

	for (/* acima */; cp < end_cp; /* abaixo */)
	{
		code = 0;

		for (i = blocks_per_line; i > 0 && bp < end_bp; i--, bp++)
		{
			if (code < 0 || *bp == 0)
				continue;

			code = *bp;
		}

		*cp++ = code;
	}

	return (0);

}	/* compute_before_blocks */

/*
 ****************************************************************
 *	Desenha as barras da alocação ANTES			*
 ****************************************************************
 */
void
draw_before_blocks (void)
{
	int			x;
	const signed char	*cp;
	GC			gc;

	for (cp = before_block_map, x = 0; x < BLOCK_WIDTH; x++, cp++)
	{
		if (*cp == 0)
			continue;

		if (*cp > 0)
			gc = good_block_gc;
		else
			gc = bad_block_gc;

		XDrawLine
		(	display, before.win, gc,
			BLOCK_BORDER + SHADOW + x, BLOCK_BORDER + SHADOW,
			BLOCK_BORDER + SHADOW + x, BLOCK_BORDER + SHADOW + BLOCK_HEIGHT - 1
		);
	}

}	/* end draw_before_blocks */

/*
 ****************************************************************
 *	Limpa a janela da alocação DEPOIS			*
 ****************************************************************
 */
void
clear_after_window (void)
{
	XFillRectangle
	(	display, after.win, no_block_gc,
		BLOCK_BORDER + SHADOW, BLOCK_BORDER + SHADOW,
		BLOCK_WIDTH, BLOCK_HEIGHT
	);

}	/* end clear_after_window */

/*
 ****************************************************************
 *	Desenha a janela da alocação DEPOIS			*
 ****************************************************************
 */
void
draw_after_window (void)
{
	draw_shadow
	(	after.win,
		after.triple.top_gc, after.triple.bottom_gc,
		0, 0,
		after.width, after.height
	);

	draw_shadow
	(	after.win,
		after.triple.top_gc, after.triple.bottom_gc,
		BLOCK_BORDER, BLOCK_BORDER,
		after.width - 2 * BLOCK_BORDER, after.height - 2 * BLOCK_BORDER
	);

	clear_after_window ();

	if (after_map_ready)
		draw_after_blocks ();

}	/* end draw_after_window */

/*
 ****************************************************************
 *	Inicializa as barras da alocação DEPOIS			*
 ****************************************************************
 */
int
alloc_after_blocks (int copy)
{
	/*
	 *	0: Vago, 1: bem alocado, -1: mal alocado
	 *
	 *	Aloca o vetor
	 */
	if (after_block_map != NULL)
		{ free (after_block_map); after_block_map = NULL; }

	if ((after_block_map = malloc (BLOCK_WIDTH)) == NULL)
		{ xerror (NOSTR); return (-1); }

	if (copy)
		memmove (after_block_map, before_block_map, BLOCK_WIDTH);

	return (0);

}	/* alloc_after_blocks */

/*
 ****************************************************************
 *	Desenha as barras da alocação APÓS			*
 ****************************************************************
 */
void
draw_after_blocks (void)
{
	int			x;
	const signed char	*cp;
	GC			gc;

	for (cp = after_block_map, x = 0; x < BLOCK_WIDTH; x++, cp++)
	{
		if (*cp == 0)
			continue;

		if (*cp > 0)
			gc = good_block_gc;
		else
			gc = bad_block_gc;

		XDrawLine
		(	display, after.win, gc,
			BLOCK_BORDER + SHADOW + x, BLOCK_BORDER + SHADOW,
			BLOCK_BORDER + SHADOW + x, BLOCK_BORDER + SHADOW + BLOCK_HEIGHT - 1
		);
	}

}	/* end draw_after_blocks */

/*
 ****************************************************************
 *	Atualiza um bloco					*
 ****************************************************************
 */
void
redraw_after_block (daddr_t bno)
{
	int		blocks_per_line, code, i, x;
	DOUBLE		**dpp;
	const DOUBLE	*ap;
	GC		gc;

	blocks_per_line = (fs_sz + BLOCK_WIDTH - 1) / BLOCK_WIDTH;

	x	= bno / blocks_per_line;
	bno	= x * blocks_per_line; 

	dpp	= &block_map[bno];

	code = 0;

	for (i = blocks_per_line; i > 0; i--, bno++, dpp++)
	{
		if (code < 0)
			continue;

		if ((ap = *dpp) == NODOUBLE)
			continue;

		if (ap->a_new_bno == ap->a_old_bno)
			code = 1;	/* Bem alocado */
		else
			code = -1;	/* Continua mal alocado */
	}

	if (after_block_map[x] != code)
	{
		after_block_map[x] = code;

		if   (code == 0)
			gc = no_block_gc;
		elif (code > 0)
			gc = good_block_gc;
		else
			gc = bad_block_gc;

		XDrawLine
		(	display, after.win, gc,
			BLOCK_BORDER + SHADOW + x, BLOCK_BORDER + SHADOW,
			BLOCK_BORDER + SHADOW + x, BLOCK_BORDER + SHADOW + BLOCK_HEIGHT - 1
		);
	}

	draw_percent_percent
	(	percent_action_done,
		percent_action_block + percent_action_inode + percent_action_dir
	);

	XFlush (display);

}	/* end redraw_after_block */

/*
 ****************************************************************
 *	Recalcula o bloco DEPOIS para um novo tamanho		*
 ****************************************************************
 */
void
compute_after_block_map (void)
{
	int			blocks_per_line, i, code;
	signed char		*cp;
	const signed char	*end_cp;
	DOUBLE			**dpp, **end_dpp;
	const DOUBLE		*ap;

	/*
	 *	Prólogo
	 */
	blocks_per_line = (fs_sz + BLOCK_WIDTH - 1) / BLOCK_WIDTH;

	memset (after_block_map, 0, BLOCK_WIDTH);

	/*
	 *	Varre o mapa de blocos
	 */
	dpp = block_map; end_dpp = dpp + fs_sz;

	cp = after_block_map; end_cp = cp + BLOCK_WIDTH;

	for (/* acima */; cp < end_cp; cp++)
	{
		code = 0;

		for (i = blocks_per_line; i > 0 && dpp < end_dpp; i--, dpp++)
		{
			if (code < 0)	/* Mal alocado */
				continue;

			if ((ap = *dpp) == NODOUBLE)
				continue;

			if (ap->a_new_bno == ap->a_old_bno)
				code = 1;	/* Bem alocado */
			else
				code = -1;	/* Mal alocado */
		}

		*cp = code;
	}

}	/* end compute_after_block_map */

/*
 ****************************************************************
 *	Limpa a janela da PERCENTAGEM				*
 ****************************************************************
 */
void
clear_percent_window (void)
{
	XFillRectangle
	(	display, percent.win, no_block_gc,
		PERCENT_BORDER + SHADOW, PERCENT_BORDER + SHADOW,
		percent.width - 2 * (PERCENT_BORDER + SHADOW), percent.height - 2 * (PERCENT_BORDER + SHADOW)
	);

	percent_old_width = 0;

}	/* end clear_percent_window */

/*
 ****************************************************************
 *	Desenha a janela do PROGRESSO da alocação		*
 ****************************************************************
 */
void
draw_percent_window (void)
{
	draw_shadow
	(	percent.win,
		percent.triple.top_gc, percent.triple.bottom_gc,
		0, 0,
		percent.width, percent.height
	);

	draw_shadow
	(	percent.win,
		percent.triple.top_gc, percent.triple.bottom_gc,
		PERCENT_BORDER, PERCENT_BORDER,
		percent.width - 2 * PERCENT_BORDER, percent.height - 2 * PERCENT_BORDER
	);

	clear_percent_window ();

	if (percent_action_block + percent_action_inode + percent_action_dir)
	{
		draw_percent_percent
		(	percent_action_done,
			percent_action_block + percent_action_inode + percent_action_dir
		);
	}

}	/* end draw_percent_window */

/*
 ****************************************************************
 *	(Re)Desenha a janela das percentagem			*
 ****************************************************************
 */
void
draw_percent_percent (int done, int total)
{
	int		percent_new_width;

	percent_new_width = done * (percent.width - 2 * (PERCENT_BORDER + SHADOW)) / total;

	if   (percent_new_width == percent_old_width)
	{
		return;
	}
	elif (percent_new_width < percent_old_width)
	{
		XFillRectangle
		(	display, percent.win, no_block_gc,
			PERCENT_BORDER + SHADOW + percent_new_width,
			PERCENT_BORDER + SHADOW,
			percent_old_width - percent_new_width,
			percent.height - 2 * (PERCENT_BORDER + SHADOW)
		);
	}
	else	/* percent_new_width > percent_old_width */
	{
		XFillRectangle
		(	display, percent.win, percent_gc,
			PERCENT_BORDER + SHADOW + percent_old_width,
			PERCENT_BORDER + SHADOW, 
			percent_new_width - percent_old_width,
			percent.height - 2 * (PERCENT_BORDER + SHADOW)
		);
	}

	percent_old_width = percent_new_width;

}	/* end draw_percent_percent */

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
	int			min_dev_width, min_menu_width;
	MENUTB			*mp;
	static int		min_width, min_height, present_width;

	/*
	 ****** Verifica se é a primeira vez, ... ***************
	 */
	if (min_width == 0)	/* Primeira vez */
	{
		/* Largura */

		min_dev_width = sizeof ("sda4b   ONEGOODHOME      2048 MB  Não consegui ler") *
				FONT_WIDTH + BUTTON_WIDTH + 2 * BORDER + 2 * MENU_BORDER;

		for (mp = &menutb[0]; mp < &menutb[NMENU]; mp++)
		{
			mp->m_win.width	 = strlen (mp->m_text) * FONT_WIDTH + 2 * MENU_BORDER;
			mp->m_win.height = FONT_HEIGHT + 2 * MENU_BORDER;
			mp->m_win.x	 = (mp == &menutb[0]) ? 0 : mp[-1].m_win.x + mp[-1].m_win.width;
			mp->m_win.y	 = 0;
		}

		mp--; min_menu_width = mp->m_win.x + mp->m_win.width;

		/* Altura */

		if (min_dev_width > min_menu_width)
			min_width = min_dev_width;
		else
			min_width = min_menu_width;

		if (min_width < szhint.width)
			min_width = szhint.width;

		/*
		 *	Calcula os vários tamanhos
		 */
		option.width	= strlen (option_longest_text) * FONT_WIDTH + 2 * SHADOW + 2 * MENU_BORDER;
		option.height	= option_text_n * (FONT_HEIGHT + 2 * SHADOW) + 2 * MENU_BORDER;
		option.x	= 0;
		option.y	= FONT_HEIGHT + 2 * MENU_BORDER + 2 * BORDER;

		method.width	= strlen (method_longest_text) * FONT_WIDTH + 2 * SHADOW +
				  3 * MENU_BORDER + BUTTON_WIDTH;
		method.height	= method_text_n * (FONT_HEIGHT + 2 * SHADOW) + 2 * MENU_BORDER;
		method.x	= option.width + 2 * BORDER;
		method.y	= FONT_HEIGHT + 2 * MENU_BORDER + 2 * BORDER +
				  method_line_offset * (FONT_HEIGHT + 2 * SHADOW) + MENU_BORDER;

		version.width	= strlen (version_longest_text) * FONT_WIDTH + 2 * SHADOW +
				  2 * MENU_BORDER;
		version.height	= version_text_n * (FONT_HEIGHT + 2 * SHADOW) + 2 * MENU_BORDER;
		version.x	= option.width + 2 * BORDER;
		version.y	= FONT_HEIGHT + 2 * MENU_BORDER + 2 * BORDER +
				  version_line_offset * (FONT_HEIGHT + 2 * SHADOW) + MENU_BORDER;

		dev.width	= min_width;
		dev.height	= NDEV * (FONT_HEIGHT + 2 * SHADOW) + 2 * MENU_BORDER;
		dev.x		= 0;
		dev.y		= FONT_HEIGHT + 2 * MENU_BORDER;

		msg.width	= min_width;
		msg.height	= MSGLINES * (FONT_HEIGHT + 2 * SHADOW) + 2 * MENU_BORDER;
		msg.x		= 0;
		msg.y		= dev.y + dev.height;

		fatal.width	= 2 * MENU_BORDER + 50 * FONT_WIDTH;
		fatal.height	= 2 * MENU_BORDER + 5 * (FONT_HEIGHT + 2 * SHADOW);
		fatal.x		= dev.x + ((dev.width - fatal.width) >> 1);
		fatal.y		= dev.y + (dev.width >> 2);

		BLOCK_WIDTH	= min_width - 2 * BLOCK_BORDER - 2 * SHADOW;

		before.width	= min_width;
		before.height	= BLOCK_HEIGHT + 2 * BLOCK_BORDER + 2 * SHADOW;
		before.x	= 0;
		before.y	= msg.y + msg.height;

		after.width	= min_width;
		after.height	= BLOCK_HEIGHT + 2 * BLOCK_BORDER + 2 * SHADOW;
		after.x		= 0;
		after.y		= before.y + before.height;

		percent.width	= min_width;
		percent.height	= PERCENT_HEIGHT + 2 * PERCENT_BORDER;
		percent.x	= 0;
		percent.y	= after.y + after.height;

		min_height = percent.y + percent.height;

		if (min_height < szhint.height)
			min_height = szhint.height;

		/*
		 *	Informa o WM
		 */
		szhint.width_inc   = 1;
		szhint.height_inc  = 1;

		szhint.base_width  = 0;
		szhint.base_height = 0;

		szhint.min_width   = width  = min_width;
		szhint.min_height  = height = min_height;

		szhint.flags = PBaseSize|PMinSize|PResizeInc|PWinGravity;

		present_width = width;

		return;
	}
	else			/* NÃO é a primeira vez */
	{
		height = min_height;

		if (width < min_width)
			width = min_width;

		if (width == present_width)
			return;
	}

	/*
	 *	Recalcula os vários tamanhos
	 */
	dev.width	= width;
   /***	dev.height	= NDEV * (FONT_HEIGHT + 2 * SHADOW) + 2 * MENU_BORDER;	***/
   /***	dev.x		= 0;					***/
   /***	dev.y		= FONT_HEIGHT + 2 * MENU_BORDER;	***/

	msg.width	= width;
   /***	msg.height	= MSGLINES * (FONT_HEIGHT + 2 * SHADOW) + 2 * MENU_BORDER;	***/
   /***	msg.x		= 0;					***/
   /***	msg.y		= dev.y + dev.height;			***/

   /***	fatal.width	= 2 * MENU_BORDER + 50 * FONT_WIDTH;	***/
   /***	fatal.height	= 2 * MENU_BORDER + 5 * (FONT_HEIGHT + 2 * SHADOW);	***/
	fatal.x		= dev.x + ((dev.width - fatal.width) >> 1);
	fatal.y		= dev.y + (dev.width >> 2);

	BLOCK_WIDTH	= width - 2 * BLOCK_BORDER - 2 * SHADOW;

	before.width	= width;
   /***	before.height	= BLOCK_HEIGHT + 2 * BLOCK_BORDER + 2 * SHADOW;		***/
   /***	before.x	= 0;					***/
   /***	before.y	= msg.y + msg.height;			***/

	after.width	= width;
   /***	after.height	= BLOCK_HEIGHT + 2 * BLOCK_BORDER + 2 * SHADOW;		***/
   /***	after.x		= 0;					***/
   /***	after.y		= before.y + before.height;		***/

	percent.width	= width;
   /***	percent.height	= PERCENT_HEIGHT + 2 * PERCENT_BORDER;	***/
   /***	percent.x	= 0;					***/
   /***	percent.y	= after.y + after.height;		***/

	/*
	 *	Atualiza os tamanhos das outras janelas
	 */
	value.width = dev.width;
	XConfigureWindow (display, dev.win, CWWidth, &value);

	value.width = msg.width;
	XConfigureWindow (display, msg.win, CWWidth, &value);

	value.width = fatal.width;
	XConfigureWindow (display, fatal.win, CWWidth, &value);

	value.width = before.width;
	XConfigureWindow (display, before.win, CWWidth, &value);

	value.width = after.width;
	XConfigureWindow (display, after.win, CWWidth, &value);

	value.width = percent.width;
	XConfigureWindow (display, percent.win, CWWidth, &value);

#ifdef	DEBUG
	printf ("Mudando a largura %d para %d\n", present_width, width);
#endif	DEBUG

	compute_before_blocks ();

	alloc_after_blocks (0);		/* Sem cópia */

	compute_after_block_map ();

	present_width = width;

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
 *	Envia imediatamente para o servidor			*
 ****************************************************************
 */
void
do_XFlush (void)
{
	XFlush (display);

}	/* end do_XFlush */

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
		"%s - melhora a alocação dos blocos de um sistema de arquivos T1\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-r] [-g <geo>] [-f <frente>] [-b <fundo>] [-d <video>] [-F <fonte>]\n",
		pgname, pgversion, pgname
	);
	fprintf
	(	stderr,
		"\nOpções:"
		"\t-r: Os dispositivos NÃO são modificados (para testes)\n"
		"\t-g: Geometria <geo> (no formato <largura>x<altura>, em pixeis) \n"
		"\t-f: Use a cor de <frente> dada\n"
		"\t-b: Use a cor de <fundo> dada\n"
		"\t-d: Conecte-se ao <vídeo> dado\n"
		"\t-F: Use a <fonte> dada\n"
	);

	exit (2);

}	/* end help */
