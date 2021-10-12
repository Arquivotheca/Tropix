/*
 ****************************************************************
 *								*
 *			xfm.c					*
 *								*
 *	Gerenciador gráfico de arquivos				*
 *								*
 *	Versão	4.5.0, de 24.06.03				*
 *		4.8.0, de 23.09.05				*
 *								*
 *	Módulo: xfm						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2005 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include <sys/common.h>
#include <sys/disktb.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <dirent.h>
#include <stat.h>

#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Intrinsic.h>
#include <X11/keysym.h>
#include <X11/cursorfont.h>

#include "../h/xfm.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char		pgversion[] = "Versão: 4.8.0, de 23.09.05";

entry int		vflag;		/* Verbose */

entry int		real_uid;	/* O usuário REAL */

/*
 ****** Cores variáveis *****************************************
 */
#define	default_fore_color_str	"black"
#define	default_back_color_str	"gray"
#define	default_menu_font_str	"9x15"
#define	default_file_font_str	"9x15"

/*
 ****** Cores fixas *********************************************
 */
#define BORDER_COLOR		0	/*** BlackPixel (display, screen) ***/

#define	BLACK_COLOR_NM		"black"

#define	FULL_DIR_COLOR_NM	"blue1"
#define	EMPTY_DIR_COLOR_NM	"red1"

#define	IGNORE_COLOR_NM		"black"
#define	BAD_COLOR_NM		"red"
#define	MOUNTED_COLOR_NM	"yellow"
#define	DEFRAG_COLOR_NM		"green"

#define	GOOD_BLOCK_COLOR_NM	"blue1"		/* ou "dodgerblue" */
#define	BAD_BLOCK_COLOR_NM	"red1"		/* ou "orangered" */
#define	NO_BLOCK_COLOR_NM	"black"

#define PERCENT_COLOR_NM	"green"

/*
 ******	Valores obtidos das cores *******************************
 */
entry TRIPLEGC		fore_triple, back_triple, black_triple;

entry TRIPLEGC		button_ignore_triple,  button_defrag_triple,
			button_mounted_triple, button_bad_triple;

entry TRIPLEGC		full_dir_triple, empty_dir_triple;

/*
 ******	Variáveis globais ***************************************
 */
entry Display		*display;

entry Window		root_win;

entry int		present_root_width, present_root_height;

entry GC		rubber_band_gc;

entry GC		reverse_text_gc;

entry GC		absent_text_gc;

entry int		last_rubber_band_x = -1;

entry time_t		last_compare_tree;

/*
 ****** Quasi-constantes dos tamanhos, ... **********************
 */
entry int	LINE_HEIGHT;	/* Altura total da linha */

entry int	TEXT_OFFSET;	/* Deslocamento do texto */

entry int	ICON_OFFSET;	/* Deslocamento do ícone */

entry int	BUTTON_OFFSET;	/* Deslocamento do botão */

entry int	TREE_OFFSET;	/* Deslocamento do texto do "tree" */

/*
 ******	Geometria ***********************************************
 */
entry XSizeHints	szhint =
{
	  PBaseSize|PMinSize|PResizeInc|PWinGravity,	/* Bits valid   flags			*/
	  0,	0,					/*		x, y			*/
	  0,						/*		width			*/
	  MIN_DIR_LINE_Y,				/*		height			*/
	  0,						/* PMinSize	min_width		*/
	  0,						/* PMinSize	min_height		*/
	  0,	0,					/*		max_width, max_height	*/
	  1,	0 /* LINE_HEIGHT */,			/* PResizeInc	width_inc, height_inc	*/
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
 ******	Dados da janela de MENU *********************************
 */
void		option_function (int value);
void		mem_function (int value);
void		quit_function (int value);

void		no_function (int value) {}

typedef struct
{
	const char	*m_text;		/* Texto a escrever */
	void		(*m_func) (int);	/* Função a chamar */
	WINDATA		m_win;			/* Dados da janela */

}	MENUTB;

MENUTB		menutb[] =
{
	{ "Opções",		option_function },	/* 0 */
	{ "Memória",		mem_function },		/* 1 */
	{ "Atualiza a árvore",	compare_root_tree },	/* 2 */
	{ "Relê a árvore",	recreate_root_tree },	/* 3 */
	{ "Cancela",		no_function },		/* 4 */
	{ "Fim",		quit_function }		/* 5 */
};

#define	NMENU	(sizeof (menutb) / sizeof (MENUTB))

/*
 ******	Dados da janela dos nomes dos VOLUMEs *******************
 */
const SUBMENU_VEC	volume_vec[] =
{
	"Inclui",	VOLUME_ON,
	"NÃO Inclui",	VOLUME_OFF
};

entry int		volume_in_use = VOLUME_ON;	/* Inclui VOLUMEs */

entry SUBMENU	volume =
{
	{ NULL },					/* Windata */
	volume_vec,					/* Text vector */

	0,						/* mapped */
	2,						/* line_offset */
	10,						/* strlen ("NÃO Inclui") */
	(sizeof (volume_vec) / sizeof (SUBMENU_VEC)),	/* text_n */
	&volume_in_use,					/* value_in_use */
	-1						/* highlight_line */
};

/*
 ******	Dados da janela dos PADRÕES dos nomes dos arquivos ******
 */
const SUBMENU_VEC	pattern_vec[] =
{
	"Todos os arquivos",	PATTERN_ALL,
	"Apenas os fontes",	PATTERN_SOURCE
};

entry int		pattern_in_use = PATTERN_ALL;	/* padrões em uso */

entry SUBMENU	pattern =
{
	{ NULL },					/* Windata */
	pattern_vec,					/* Text vector */

	0,						/* mapped */
	3,						/* line_offset */
	17,						/* strlen ("Todos os arquivos") */
	(sizeof (pattern_vec) / sizeof (SUBMENU_VEC)),	/* text_n */
	&pattern_in_use,				/* value_in_use */
	-1						/* highlight_line */
};

/*
 ******	Dados da janela de arquivos OCULTOS *********************
 */
const SUBMENU_VEC	hidden_vec[] =
{
	"Ignora",	DOT_FLAG_OFF,
	"Processa",	DOT_FLAG_ON
};

entry int		hidden_in_use = DOT_FLAG_OFF;		/* Ignora/processa os ocultos */

entry SUBMENU	hidden =
{
	{ NULL },					/* Windata */
	hidden_vec,					/* Text vector */

	0,						/* mapped */
	4,						/* line_offset */
	8,						/* strlen ("Processa") */
	(sizeof (hidden_vec) / sizeof (SUBMENU_VEC)),	/* text_n */
	&hidden_in_use,					/* value_in_use */
	-1						/* highlight_line */
};

/*
 ******	Dados da janela do AJUSTE da janela do diretório ********
 */
const SUBMENU_VEC	extension_vec[] =
{
	"Estende",	WIN_EXTENSION_ON,
	"NÃO Estende",	WIN_EXTENSION_OFF
};

entry int		extension_in_use = WIN_EXTENSION_ON;	/* Estende ou não a janela de DIR */

entry SUBMENU	extension =
{
	{ NULL },					/* Windata */
	extension_vec,					/* Text vector */

	0,						/* mapped */
	5,						/* line_offset */
	11,						/* strlen ("NÃO Estende") */
	(sizeof (extension_vec) / sizeof (SUBMENU_VEC)), /* text_n */
	&extension_in_use,				/* value_in_use */
	-1						/* highlight_line */
};

#if (0)	/*******************************************************/
/*
 ******	Dados da janela do GRUPO dos arquivos do diretório ******
 */
const SUBMENU_VEC	group_vec[] =
{
	"Inclui",	GROUP_ON,
	"NÃO Inclui",	GROUP_OFF
};

entry int		group_in_use = GROUP_OFF,	/* Inclui ou não o grupo dos arquivos */
			last_group_in_use = GROUP_OFF;

entry SUBMENU	group =
{
	{ NULL },					/* Windata */
	group_vec,					/* Text vector */

	0,						/* mapped */
	6,						/* line_offset */
	10,						/* strlen ("NÃO Estende") */
	(sizeof (group_vec) / sizeof (SUBMENU_VEC)),	/* text_n */
	&group_in_use,					/* value_in_use */
	-1						/* highlight_line */
};
#endif	/*******************************************************/

/*
 ******	Dados da janela do MODO COMPLETO arquivos do diretório **
 */
const SUBMENU_VEC	mode_vec[] =
{
	"Resumido",	MODE_9,
	"Completo",	MODE_13
};

entry int		mode_in_use = MODE_9,	/* Inclui ou não o modo completo dos arquivos */
			last_mode_in_use = MODE_9;

entry SUBMENU	mode =
{
	{ NULL },					/* Windata */
	mode_vec,					/* Text vector */

	0,						/* mapped */
	6,						/* line_offset */
	8,						/* strlen ("Completo") */
	(sizeof (mode_vec) / sizeof (SUBMENU_VEC)),	/* text_n */
	&mode_in_use,					/* value_in_use */
	-1						/* highlight_line */
};

/*
 ******	Dados da janela da VERSÃO *******************************
 */
entry WINDATA		version;

entry int		version_mapped;

#define	version_line_offset	7			/* Das opções, acima */
#define	version_longest_text	pgversion
#define	version_text_n		1

/*
 ******	Dados da janela das OPÇÕES ******************************
 */
#define NOFUNCTION	(void (*) (int))NULL

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
    "Seleciona todos os arquivos",	select_all,	NULL,			NOSUBMENU,	/* 0 */
    "Desseleciona os arquivos",		disselect_all,	NULL,			NOSUBMENU,	/* 1 */
    "Nomes dos volumes: ",		NOFUNCTION,	&volume.windata,	&volume,	/* 2 */
    "Padrões: ",			NOFUNCTION,	&pattern.windata,	&pattern,	/* 3 */
    "Arquivos ocultos:",		NOFUNCTION,	&hidden.windata,	&hidden,	/* 4 */
    "Largura das janelas:",		NOFUNCTION,	&extension.windata,	&extension,	/* 5 */
#if (0)	/*******************************************************/
    "Grupos dos arquivos:",		NOFUNCTION,	&group.windata,		&group,		/* 6 */
#endif	/*******************************************************/
    "Modo dos arquivos:",		NOFUNCTION,	&mode.windata,		&mode,		/* 7 */
    "Versão: ",				NOFUNCTION,	&version,		NOSUBMENU,	/* 8 */
};

#define	option_longest_text	optionvec[0].m_text
#define	option_text_n		(sizeof (optionvec) / sizeof (OPTIONVEC))

entry int		option_highlight_line = -1;

/*
 ******	Dados da janela da MENSAGEM *****************************
 */
entry WINDATA		msg, msg_yes, msg_no;

/*
 ******	Dados da janela da PERCENTAGEM **************************
 */
entry WINDATA		percent;

entry GC		percent_gc;

/*
 ******	Dados da janela da ÁRVORE *******************************
 */
entry WINDATA		tree, tree_menu;

/*
 ******	Dados da janela do INTERVALO ****************************
 */
entry WINDATA		gap;

entry int		gap_button_state;

/*
 ******	Dados da janela do DIRETÓRIO ****************************
 */
entry WINDATA		dir, name, dir_menu;

/*
 ******	Dados das barras de rolagem *****************************
 */
entry SCROLLDATA	tree_scroll, dir_scroll;

/*
 ******	Dados da janela de edição de um nome ********************
 */
entry WINDATA		nm_edit;

/*
 ******	Dados da janela de edição do DF *************************
 */
entry WINDATA		df_data;

/*
 ******	Dados da janela de edição do DU *************************
 */
entry WINDATA		du_data;

/*
 ****** Protótipos de funções ***********************************
 */
int		set_shadow_colors (Display *, Window, TRIPLEGC *, const char *, Pixel);
void		geometry_to_size_hints (const char *geo_str, XSizeHints *hp);
void		event_loop (int main_event_loop);
void		process_expose_event (const XEvent *event);
void		draw_option_win (void);
void		set_font_sizes (int width, int height, int ascent, int descent, ...);
#if (0)	/*******************************************************/
void		draw_pattern_win (void);
void		draw_hidden_win (void);
void		do_hidden_button_press (int y);
void		motion_hidden_window (int y);
void		leave_hidden_window (void);
#endif	/*******************************************************/
void		button_press_gap_window (int);
void		button_release_gap_window (int);
void		motion_gap_window (int y);
void		help (void);

void		do_submenu (SUBMENU *smp);
void		draw_submenu_win (const SUBMENU *smp);
void		motion_submenu_window (SUBMENU *smp, int y);
void		do_submenu_button_press (SUBMENU *smp, int y);

int		get_pointer_x (Window win);
int		get_pointer_y (Window win);
void		get_pointer_x_y (Window win, int *x, int *y);
void		draw_menu_win (const MENUTB *mp);
void		unmap_whole_menu (void);
void		unmap_tmp_windows (void);
void		motion_option_window (int y);
void		leave_option_window (void);
void		do_option_button_press (int y);
void		do_pattern_button_press (int y);
void		motion_pattern_window (int y);
void		leave_pattern_window (void);
void		draw_version_win (void);
void		Shadow_up (const WINDATA *);
void		Shadow_down (const WINDATA *);
#if (0)	/*******************************************************/
void		motion_tree_window (int y);
void		leave_tree_window (void);
#endif	/*******************************************************/
void		update_dir_window (int y);
void		draw_fatal_window (void);
void		draw_before_window (void);
void		draw_after_window (void);
Pixel		get_window_color (const char *color_nm, Pixel def);
void		quit_function (int value);
const char	*edit_event_nm (int event);

/*
 ****************************************************************
 *	Gerenciador gráfico de arquivos				*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	WINDATA		*wp;
	int		opt;
	const char	*geo_str = NOSTR;
	int		save_argc = argc;
	const char	**save_argv = argv;
	const char	*display_str = NOSTR;
	const char	*menu_font_str, *user_menu_font_str = NOSTR;
	const char	*file_font_str, *user_file_font_str = NOSTR;
	const char	*fore_color_str, *user_fore_color_str = NOSTR;
	const char	*back_color_str, *user_back_color_str = NOSTR;
	XFontStruct	*menu_text_fip, *file_text_fip;
	int		screen;
	int		menu_font_width, menu_font_height, menu_font_ascent, menu_font_descent;
	int		file_font_width, file_font_height, file_font_ascent, file_font_descent;
	Window		root, win;
	XClassHint	classhint;
	XWMHints	wmhint;
	XGCValues	gcv;
	GC		menu_text_gc, file_text_gc;
	const char	*str;
	MENUTB		*mp;
	Cursor		double_arrow_cursor, hand_cursor;

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "vg:f:b:d:M:F:H")) != EOF)
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

		    case 'M':
			user_menu_font_str = optarg;	/* Fonte do MENU */
			break;

		    case 'F':
			user_file_font_str = optarg;	/* Fonte dos ARQUIVOS */
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
	 ****** Verifica o Usuário Efetivo **********************
	 */
	if (geteuid () != 0)
		error ("$O usuário efetivo NÃO é o SUPERUSUÁRIO");

	real_uid = getuid ();

	if (setruid (0) < 0)
		error ("*Não consegui alterar o usuário real");

	if (seteuid (real_uid) < 0)
		error ("*Não consegui alterar o usuário efetivo");

	/*
	 ****** Lê o arquivo FSTAB e SBs ************************
	 */
	read_fstab (); create_SB_list ();

	/*
	 ****** Prepara logo a lista da RAIZ ********************
	 */
	create_root_tree ();

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
	 ******	Guarda o tempo inicial **************************
	 */
	time (&last_compare_tree);

	/*
	 ****** Tenta obter os valores de "XDefault" ************
	 */
	if   (user_fore_color_str != NOSTR)
		fore_color_str = user_fore_color_str;
	elif ((str = XGetDefault (display, "XFm", "foreground")) != NOSTR)
		fore_color_str = str;
	else
		fore_color_str = default_fore_color_str;

	if   (user_back_color_str != NOSTR)
		back_color_str = user_back_color_str;
	elif ((str = XGetDefault (display, "XFm", "background")) != NOSTR)
		back_color_str = str;
	else
		back_color_str = default_back_color_str;

	if   (user_menu_font_str != NOSTR)
		menu_font_str = user_menu_font_str;
	elif ((str = XGetDefault (display, "XFm", "menufont")) != NOSTR)
		menu_font_str = str;
	else
		menu_font_str = default_menu_font_str;

	if   (user_file_font_str != NOSTR)
		file_font_str = user_file_font_str;
	elif ((str = XGetDefault (display, "XFm", "filefont")) != NOSTR)
		file_font_str = str;
	else
		file_font_str = default_file_font_str;

	/* Prepara logo o fundo dos ÍCONES XPM */

	sprintf (background_line, " 	c %s", back_color_str);

	/*
	 ******	Obtém as cores fixas e variáveis ****************
	 */
	set_shadow_colors (display, root, &fore_triple, fore_color_str, 0x000000);
	set_shadow_colors (display, root, &back_triple, back_color_str, 0xBEBEBE);

	set_shadow_colors (display, root, &black_triple, BLACK_COLOR_NM, 0x000000);

	/*
	 ******	Obtém os dados da fonte *************************
	 */
	if ((menu_text_fip = XLoadQueryFont (display, menu_font_str)) == NULL)
		error ("$Não encontrei a fonte \"%s\"", menu_font_str);

	menu_font_width   = XTextWidth (menu_text_fip, "MMMMMMMMMM", 10) / 10;
	menu_font_ascent  = menu_text_fip->ascent;
	menu_font_descent = menu_text_fip->descent;
	menu_font_height  = menu_text_fip->ascent + menu_text_fip->descent;

	if ((file_text_fip = XLoadQueryFont (display, file_font_str)) == NULL)
		error ("$Não encontrei a fonte \"%s\"", file_font_str);

	file_font_width   = XTextWidth (file_text_fip, "MMMMMMMMMM", 10) / 10;
	file_font_ascent  = file_text_fip->ascent;
	file_font_descent = file_text_fip->descent;
	file_font_height  = file_text_fip->ascent + file_text_fip->descent;

	/*
	 ****** Atribui os tamanhos das fontes nas janelas ******
	 */
	for (mp = &menutb[0]; mp < &menutb[NMENU]; mp++)
	{
		mp->m_win.font_width	= menu_font_width;
		mp->m_win.font_height	= menu_font_height;
		mp->m_win.font_ascent	= menu_font_ascent;
		mp->m_win.font_descent	= menu_font_descent;
	}

	set_font_sizes
	(	menu_font_width, menu_font_height, menu_font_ascent, menu_font_descent,
		&option, &volume.windata, &pattern.windata, &hidden.windata, &extension.windata,
		&mode.windata, &zip.windata, &version, NOWINDATA
#if (0)	/*******************************************************/
		&group.windata, &mode.windata, &zip.windata, &version, NOWINDATA
#endif	/*******************************************************/
	);

	set_font_sizes
	(	file_font_width, file_font_height, file_font_ascent, file_font_descent,
		&msg, &msg_yes, &msg_no, &percent, &tree, &tree_menu, &name, 
		&dir_menu, &dir, &nm_edit, &df_data, &du_data, &gap, NOWINDATA
	);

	/*
	 ****** Calcula as "quasi-constantes" *******************
	 */
	LINE_HEIGHT	= file_font_height + 6;

	TEXT_OFFSET	= ((LINE_HEIGHT - file_font_height) >> 1) + file_font_ascent;

	ICON_OFFSET	= (LINE_HEIGHT - ICON_HEIGHT) >> 1;

	BUTTON_OFFSET	= (LINE_HEIGHT - BUTTON_HEIGHT) >> 1;

	/*
	 ****** Converte o formato XPM para Ximage **************
	 */
	convert_xpm_images ();

	/*
	 ****** Prepara as datas para o "ls" ********************
	 */
	calculate_today_time ();

	/*
	 ****** Cria a janela PRINCIPAL	*************************
	 */
	geometry_to_size_hints (geo_str, &szhint);

	set_window_sizes (0, 0, 0);

        win = XCreateSimpleWindow
	(	display, root,
		0, 0,
		present_root_width, present_root_height,
		0, BORDER_COLOR,
		BlackPixel (display, screen)
	);

	root_win = win;

	XStoreName (display, win, "Xfm");

	XSelectInput (display, win, ExposureMask|KeyPressMask|StructureNotifyMask);

	classhint.res_name  = (char *)pgname;
	classhint.res_class = "XFm";

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
	gcv.font	= menu_text_fip->fid;
	gcv.foreground  = fore_triple.main_pixel;
	gcv.background  = back_triple.main_pixel;
	menu_text_gc	= XCreateGC (display, win, GCForeground|GCBackground|GCFont, &gcv);

{
	XColor		xcol;
	unsigned long	fore_sum, back_sum;
	Colormap	Xcmap;

	Xcmap = DefaultColormap (display, DefaultScreen (display));

	xcol.pixel = fore_triple.main_pixel; XQueryColor (display, Xcmap, &xcol);
	fore_sum = xcol.red + xcol.green + xcol.blue;

	xcol.pixel = back_triple.main_pixel; XQueryColor (display, Xcmap, &xcol);
	back_sum = xcol.red + xcol.green + xcol.blue;

#if (0)	/*******************************************************/
printf ("fore_sum = %d, back_sum = %d\n", fore_sum, back_sum);
#endif	/*******************************************************/

	if (fore_sum > back_sum)
		gcv.foreground  = fore_triple.bottom_pixel;
	else
		gcv.foreground  = fore_triple.top_pixel;

}

   /***	gcv.font	= menu_text_fip->fid;		***/
#if (0)	/*******************************************************/
	gcv.foreground  = fore_triple.bottom_pixel;
#endif	/*******************************************************/
   /***	gcv.background  = back_triple.main_pixel;	***/
	absent_text_gc	= XCreateGC (display, win, GCForeground|GCBackground|GCFont, &gcv);

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
	 ******	Prepara a janela dos VOLUMEs ********************
	 */
	wp = &volume.windata;

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
	 ******	Prepara a janela dos padrões dos arquivos *******
	 */
	wp = &pattern.windata;

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
	 ******	Prepara a janela de arquivos OCULTOS ************
	 */
	wp = &hidden.windata;

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
	 ******	Prepara a janela da EXTENSÃO do diretório *******
	 */
	wp = &extension.windata;

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

#if (0)	/*******************************************************/
	/*
	 ******	Prepara a janela do GRUPO dos arquivos **********
	 */
	wp = &group.windata;

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
#endif	/*******************************************************/

	/*
	 ******	Prepara a janela do MODO dos arquivos ***********
	 */
	wp = &mode.windata;

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
	 ******	Prepara a janela do ZIP *************************
	 */
	wp = &zip.windata;

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
	 ****** Prepara algumas cores ***************************
	 */
	set_shadow_colors (display, win, &full_dir_triple,	 FULL_DIR_COLOR_NM,  0x0000CC);
	set_shadow_colors (display, win, &empty_dir_triple,	 EMPTY_DIR_COLOR_NM, 0xCC0000);

	set_shadow_colors (display, win, &button_ignore_triple,  IGNORE_COLOR_NM,  0x000000);
	set_shadow_colors (display, win, &button_defrag_triple,  DEFRAG_COLOR_NM,  0x0000CC);
	set_shadow_colors (display, win, &button_mounted_triple, MOUNTED_COLOR_NM, 0xCCCC00);
	set_shadow_colors (display, win, &button_bad_triple,     BAD_COLOR_NM,     0xCC0000);

	gcv.font	= file_text_fip->fid;
	gcv.foreground  = fore_triple.main_pixel;
	gcv.background  = back_triple.main_pixel;
	file_text_gc	= XCreateGC (display, win, GCForeground|GCBackground|GCFont, &gcv);

	/*
	 ****** Cria a janela da MENSAGEM ***********************
	 */
	memmove (&msg.triple, &back_triple, sizeof (TRIPLEGC));
	msg.text_gc      = file_text_gc;

	msg.win = XCreateSimpleWindow
	(
		display, win,
		msg.x, msg.y,
		msg.width, msg.height,
		0, BORDER_COLOR,
		msg.triple.main_pixel
	);

	XSelectInput
	(	display, msg.win,
		(ExposureMask|ButtonPressMask)
	);

	XMapWindow (display, msg.win);

	/*
	 ****** Cria a janela da PERCENTAGEM*********************
	 */
	gcv.foreground  = get_window_color (PERCENT_COLOR_NM, 0x00CC00);
	percent_gc = XCreateGC (display, win, GCForeground, &gcv);

	memmove (&percent.triple, &back_triple, sizeof (TRIPLEGC));
	percent.text_gc      = file_text_gc;

	percent.win = XCreateSimpleWindow
	(
		display, win,
		percent.x, percent.y,
		percent.width, percent.height,
		0, BORDER_COLOR,
		percent.triple.main_pixel
	);

	XSelectInput
	(	display, percent.win,
		ExposureMask
	);

	XMapWindow (display, percent.win);

	/*
	 ****** Cria a janela do botão AFIRMATIVO da MENSAGEM ***
	 */
	memmove (&msg_yes.triple, &back_triple, sizeof (TRIPLEGC));
	msg_yes.text_gc      = file_text_gc;

	msg_yes.win = XCreateSimpleWindow
	(
		display, msg.win,
		msg_yes.x, msg_yes.y,
		msg_yes.width, msg_yes.height,
		1, BORDER_COLOR,
		msg_yes.triple.main_pixel
	);

	XSelectInput
	(	display, msg_yes.win,
		(ExposureMask|EnterWindowMask|LeaveWindowMask|ButtonPressMask)
	);

   /***	XMapWindow (display, msg_yes.win); ***/

	/*
	 ****** Cria a janela do botão NEGATIVO da MENSAGEM *****
	 */
	memmove (&msg_no.triple, &back_triple, sizeof (TRIPLEGC));
	msg_no.text_gc      = file_text_gc;

	msg_no.win = XCreateSimpleWindow
	(
		display, msg.win,
		msg_no.x, msg_no.y,
		msg_no.width, msg_no.height,
		1, BORDER_COLOR,
		msg_no.triple.main_pixel
	);

	XSelectInput
	(	display, msg_no.win,
		(ExposureMask|EnterWindowMask|LeaveWindowMask|ButtonPressMask)
	);

   /***	XMapWindow (display, msg_no.win); ***/

	/*
	 ****** Cria a janela da ÁRVORE *************************
	 */
	memmove (&tree.triple, &back_triple, sizeof (TRIPLEGC));
	tree.text_gc      = file_text_gc;

	tree.win = XCreateSimpleWindow
	(
		display, win,
		tree.x, tree.y,
		tree.width, tree.height,
		0, BORDER_COLOR,
		tree.triple.main_pixel
	);

	XSelectInput
	(	display, tree.win,
		(ExposureMask|LeaveWindowMask|PointerMotionMask|PointerMotionHintMask|ButtonPressMask)
	);

	XMapWindow (display, tree.win);

	/*
	 ****** Cria a janela do MENU da ÁRVORE *****************
	 */
	memmove (&tree_menu.triple, &back_triple, sizeof (TRIPLEGC));
	tree_menu.text_gc      = file_text_gc;

	tree_menu.win = XCreateSimpleWindow
	(
		display, win,
		0, 0,
		100, 100,
		0, BORDER_COLOR,
		tree_menu.triple.main_pixel
	);

	XSelectInput
	(	display, tree_menu.win,
		(ExposureMask|LeaveWindowMask|
		PointerMotionMask|PointerMotionHintMask|ButtonPressMask)
	);

	XDefineCursor (display, tree_menu.win, hand_cursor);

	/*
	 ****** Cria a janela do NOME do diretório **************
	 */
	memmove (&name.triple, &back_triple, sizeof (TRIPLEGC));
	name.text_gc      = file_text_gc;

	name.win = XCreateSimpleWindow
	(
		display, win,
		name.x, name.y,
		name.width, name.height,
		0, BORDER_COLOR,
		name.triple.main_pixel
	);

	XSelectInput
	(	display, name.win,
		ExposureMask
	);

	XMapWindow (display, name.win);

	/*
	 ****** Cria a janela do DIRETÓRIO **********************
	 */
	memmove (&dir.triple, &back_triple, sizeof (TRIPLEGC));
	dir.text_gc      = file_text_gc;

	dir.win = XCreateSimpleWindow
	(
		display, win,
		dir.x, dir.y,
		dir.width, dir.height,
		0, BORDER_COLOR,
		dir.triple.main_pixel
	);

	XSelectInput
	(	display, dir.win,
		(ExposureMask|ButtonPressMask|ButtonReleaseMask|Button1MotionMask|PointerMotionHintMask)
	);

	XMapWindow (display, dir.win);

	/*
	 ****** Janela da BARRA de ROLAGEM da ÁRVORE ************
	 */
	memmove (&tree_scroll.triple, &back_triple, sizeof (TRIPLEGC));

	tree_scroll.win = XCreateSimpleWindow
	(
		display, win,
		tree_scroll.x, tree_scroll.y,
		tree_scroll.width, tree_scroll.height,
		0, BORDER_COLOR,
		tree_scroll.triple.main_pixel
	);

	XSelectInput
	(	display, tree_scroll.win,
		(ExposureMask|ButtonPressMask|ButtonReleaseMask|Button1MotionMask|PointerMotionHintMask)
	);

	XMapWindow (display, tree_scroll.win);

	/*
	 ****** Cria a janela do MENU do DIRETÓRIO **************
	 */
	memmove (&dir_menu.triple, &back_triple, sizeof (TRIPLEGC));
	dir_menu.text_gc      = file_text_gc;

	dir_menu.win = XCreateSimpleWindow
	(
		display, win,
		0, 0,
		100, 100,
		0, BORDER_COLOR,
		dir_menu.triple.main_pixel
	);

	XSelectInput
	(	display, dir_menu.win,
		(ExposureMask|LeaveWindowMask|
		PointerMotionMask|PointerMotionHintMask|ButtonPressMask)
	);

	XDefineCursor (display, dir_menu.win, hand_cursor);

	/*
	 ****** Janela da BARRA de ROLAGEM do diretório *********
	 */
	memmove (&dir_scroll.triple, &back_triple, sizeof (TRIPLEGC));

	dir_scroll.win = XCreateSimpleWindow
	(
		display, win,
		dir_scroll.x, dir_scroll.y,
		dir_scroll.width, dir_scroll.height,
		0, BORDER_COLOR,
		dir_scroll.triple.main_pixel
	);

	XSelectInput
	(	display, dir_scroll.win,
		(ExposureMask|ButtonPressMask|ButtonReleaseMask|Button1MotionMask|PointerMotionHintMask)
	);

	XMapWindow (display, dir_scroll.win);

	/*
	 ****** Cria a janela do INTERVALO **********************
	 */
	gap.win = XCreateWindow
	(
		display, win,
		gap.x, gap.y,
		gap.width, gap.height,
		0, CopyFromParent, InputOnly,
		CopyFromParent, 0, NULL
	);

	XSelectInput
	(	display, gap.win,
		(ButtonPressMask|ButtonReleaseMask|PointerMotionMask|PointerMotionHintMask)
	);

	double_arrow_cursor = XCreateFontCursor (display, XC_sb_h_double_arrow);
	XDefineCursor (display, gap.win, double_arrow_cursor);

	XMapWindow (display, gap.win);

	/*
	 ****** Prepara a cor para mover a janela do INTERVALO **
	 */
	gcv.foreground		= WhitePixel (display, screen);
	gcv.subwindow_mode	= IncludeInferiors;
	gcv.function		= GXxor;

	rubber_band_gc = XCreateGC (display, win, GCForeground|GCSubwindowMode|GCFunction, &gcv);

	/*
	 ****** Cria a janela da EDIÇÃO de NOMES ****************
	 */
	memmove (&nm_edit.triple, &back_triple, sizeof (TRIPLEGC));
	nm_edit.text_gc      = file_text_gc;

	nm_edit.win = XCreateSimpleWindow
	(
		display, win,
		0, 0,
		100, 100,
		0, BORDER_COLOR,
		nm_edit.triple.main_pixel
	);

	XSelectInput
	(	display, nm_edit.win,
		(ExposureMask|ButtonPressMask|LeaveWindowMask)
	);

   /***	XMapWindow (display, nm_edit.win);	***/

	/*
	 ****** Cria a janela do dados do DF ********************
	 */
	memmove (&df_data.triple, &back_triple, sizeof (TRIPLEGC));
	df_data.text_gc      = file_text_gc;

	df_data.win = XCreateSimpleWindow
	(
		display, win,
		0, 0,
		100, 100,
		0, BORDER_COLOR,
		df_data.triple.main_pixel
	);

	XSelectInput
	(	display, df_data.win,
		ExposureMask
	);

   /***	XMapWindow (display, df_data.win);	***/

	/*
	 ****** Cria a janela do dados do DU ********************
	 */
	memmove (&du_data.triple, &back_triple, sizeof (TRIPLEGC));
	du_data.text_gc      = file_text_gc;

	du_data.win = XCreateSimpleWindow
	(
		display, win,
		0, 0,
		100, 100,
		0, BORDER_COLOR,
		du_data.triple.main_pixel
	);

	XSelectInput
	(	display, du_data.win,
		ExposureMask
	);

   /***	XMapWindow (display, du_data.win);	***/

	/*
	 ****** Prepara a cor para apagar linhas ****************
	 */
	gcv.font	= file_text_fip->fid;
	gcv.foreground  = back_triple.main_pixel;
	gcv.background  = fore_triple.main_pixel;
	reverse_text_gc = XCreateGC (display, win, GCForeground|GCBackground|GCFont, &gcv);

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

		gcv.font	  = file_text_fip->fid;
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

}	/* end xfm */

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
#if (0)	/*******************************************************/
	time_t		now;
#endif	/*******************************************************/

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

			process_expose_event (&event);
			break;

		    case EnterNotify:
			if   (event.xany.window == tree.win)
			{
				unmap_whole_menu ();
			}
			elif (event.xany.window == msg_yes.win)
			{
				Shadow_up (&msg_yes);
			}
			elif (event.xany.window == msg_no.win)
			{
				Shadow_up (&msg_no);
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
				(	volume.mapped || pattern.mapped || hidden.mapped || extension.mapped ||
					mode.mapped || version_mapped
#if (0)	/*******************************************************/
					group.mapped || mode.mapped || version_mapped
#endif	/*******************************************************/
				)
					break;

				XUnmapWindow (display, option.win);

				option_highlight_line = -1;
			}
			elif (event.xany.window == volume.windata.win)
			{
				XUnmapWindow (display, option.win);
				XUnmapWindow (display, volume.windata.win);

				volume.mapped = 0;
				volume.highlight_line = -1;
			}
			elif (event.xany.window == pattern.windata.win)
			{
				XUnmapWindow (display, option.win);
				XUnmapWindow (display, pattern.windata.win);

				pattern.mapped = 0;
				pattern.highlight_line = -1;
			}
			elif (event.xany.window == hidden.windata.win)
			{
				XUnmapWindow (display, option.win);
				XUnmapWindow (display, hidden.windata.win);

				hidden.mapped = 0;
				hidden.highlight_line = -1;
			}
			elif (event.xany.window == extension.windata.win)
			{
				XUnmapWindow (display, option.win);
				XUnmapWindow (display, extension.windata.win);

				extension.mapped = 0;
				extension.highlight_line = -1;
			}
#if (0)	/*******************************************************/
			elif (event.xany.window == group.windata.win)
			{
				XUnmapWindow (display, option.win);
				XUnmapWindow (display, group.windata.win);

				group.mapped = 0;
				group.highlight_line = -1;
			}
#endif	/*******************************************************/
			elif (event.xany.window == mode.windata.win)
			{
				XUnmapWindow (display, option.win);
				XUnmapWindow (display, mode.windata.win);

				mode.mapped = 0;
				mode.highlight_line = -1;
			}
			elif (event.xany.window == zip.windata.win)
			{
				XUnmapWindow (display, zip.windata.win);

				zip.mapped = 0;
				zip.highlight_line = -1;

				XUnmapWindow (display, nm_edit.win);
			}
			elif (event.xany.window == version.win)
			{
				XUnmapWindow (display, option.win);
				XUnmapWindow (display, version.win);

				version_mapped = 0;
			}
			elif (event.xany.window == msg_yes.win)
			{
				Shadow_down (&msg_yes);
			}
			elif (event.xany.window == msg_no.win)
			{
				Shadow_down (&msg_no);
			}
#if (0)	/*******************************************************/
			elif (event.xany.window == tree.win)
			{
				leave_tree_window ();
			}
#endif	/*******************************************************/
			elif (event.xany.window == tree_menu.win)
			{
				leave_tree_or_dir_menu_window (TREE_MENU);
			}
			elif (event.xany.window == dir_menu.win)
			{
				leave_tree_or_dir_menu_window (DIR_MENU);
			}
			elif (event.xany.window == nm_edit.win)
			{
				XUnmapWindow (display, nm_edit.win);
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

#if (0)	/*******************************************************/
			if ((now = time ((time_t *)NULL)) - last_compare_tree > 30)
				{ last_compare_tree = now; compare_root_tree (0); }
#endif	/*******************************************************/

			if   (event.xany.window == gap.win)
				{ button_press_gap_window (event.xbutton.y); break; }
			else
				{ clear_msg_win (); unmap_tmp_windows (); }

			if   (event.xany.window == msg_yes.win)
			{
				do_msg_button_press (+1);
			}
			elif (event.xany.window == msg_no.win)
			{
				do_msg_button_press (-1);
			}
			elif (event.xany.window == option.win)
			{
				do_option_button_press (event.xbutton.y);
			}
			elif (event.xany.window == volume.windata.win)
			{
				do_submenu_button_press (&volume, event.xbutton.y);
			}
			elif (event.xany.window == pattern.windata.win)
			{
				do_submenu_button_press (&pattern, event.xbutton.y);
			}
			elif (event.xany.window == hidden.windata.win)
			{
				do_submenu_button_press (&hidden, event.xbutton.y);
			}
			elif (event.xany.window == extension.windata.win)
			{
				do_submenu_button_press (&extension, event.xbutton.y);
			}
#if (0)	/*******************************************************/
			elif (event.xany.window == group.windata.win)
			{
				do_submenu_button_press (&group, event.xbutton.y);
			}
#endif	/*******************************************************/
			elif (event.xany.window == mode.windata.win)
			{
				do_submenu_button_press (&mode, event.xbutton.y);
			}
			elif (event.xany.window == zip.windata.win)
			{
				do_submenu_button_press (&zip, event.xbutton.y);
			}
			elif (event.xany.window == tree.win)
			{
				unmap_whole_menu ();

				button_press_tree_window
				(
					event.xbutton.button,
					event.xbutton.x, event.xbutton.y
				);
			}
			elif (event.xany.window == tree_menu.win)
			{
				button_press_tree_or_dir_menu_window (TREE_MENU);
			}
			elif (event.xany.window == tree_scroll.win)
			{
				if (event.xbutton.button == Button1)
					update_tree_scroll_bar (0 /* button */, event.xbutton.y);
			}
			elif (event.xany.window == dir.win)
			{
				unmap_whole_menu ();

				button_press_dir_window
				(	event.xbutton.button, event.xbutton.state,
					event.xbutton.x, event.xbutton.y
				);
			}
			elif (event.xany.window == dir_menu.win)
			{
				button_press_tree_or_dir_menu_window (DIR_MENU);
			}
			elif (event.xany.window == dir_scroll.win)
			{
				if (event.xbutton.button == Button1)
					update_dir_scroll_bar (0 /* button */, event.xbutton.y);
			}
#if (0)	/*******************************************************/
			elif (event.xany.window == gap.win)
			{
				button_press_gap_window (event.xbutton.y);
			}
#endif	/*******************************************************/
			else for (mp = &menutb[0]; mp < &menutb[NMENU]; mp++)
			{
				if (mp->m_win.win == event.xany.window)
					{ (*mp->m_func) (0); break; }
			}

			break;

		    case ButtonRelease:
			if (main_event_loop == 0)
				return;

			if   (event.xany.window == tree_scroll.win)
			{
				button_release_scroll_button (&tree_scroll);
			}
			elif (event.xany.window == dir.win)
			{
				if (event.xbutton.button == 1)
					button_release_dir_window (event.xbutton.y);
			}
			elif (event.xany.window == dir_scroll.win)
			{
				button_release_scroll_button (&dir_scroll);
			}
			elif (event.xany.window == gap.win)
			{
				button_release_gap_window (event.xbutton.y);
			}

			break;


		    case MotionNotify:
			if   (event.xany.window == option.win)
			{
				motion_option_window (get_pointer_y (option.win));
			}
			elif (event.xany.window == volume.windata.win)
			{
				motion_submenu_window (&volume, get_pointer_y (volume.windata.win));
			}
			elif (event.xany.window == pattern.windata.win)
			{
				motion_submenu_window (&pattern, get_pointer_y (pattern.windata.win));
			}
			elif (event.xany.window == hidden.windata.win)
			{
				motion_submenu_window (&hidden, get_pointer_y (hidden.windata.win));
			}
			elif (event.xany.window == extension.windata.win)
			{
				motion_submenu_window (&extension, get_pointer_y (extension.windata.win));
			}
#if (0)	/*******************************************************/
			elif (event.xany.window == group.windata.win)
			{
				motion_submenu_window (&group, get_pointer_y (group.windata.win));
			}
#endif	/*******************************************************/
			elif (event.xany.window == mode.windata.win)
			{
				motion_submenu_window (&mode, get_pointer_y (mode.windata.win));
			}
			elif (event.xany.window == zip.windata.win)
			{
				motion_submenu_window (&zip, get_pointer_y (zip.windata.win));
			}
			elif (event.xany.window == tree_menu.win)
			{
				motion_tree_or_dir_menu_window (TREE_MENU, get_pointer_y (tree_menu.win));
			}
			elif (event.xany.window == dir_menu.win)
			{
				motion_tree_or_dir_menu_window (DIR_MENU, get_pointer_y (dir_menu.win));
			}
			elif (event.xany.window == tree_scroll.win)
			{
				update_tree_scroll_bar (1 /* motion */, get_pointer_y (tree_scroll.win));
			}
			elif (event.xany.window == dir_scroll.win)
			{
				update_dir_scroll_bar (1 /* motion */, get_pointer_y (dir_scroll.win));
			}
			elif (event.xany.window == gap.win)
			{
				motion_gap_window (get_pointer_x (gap.win));
			}

			break;

		    case KeyPress:
			if (main_event_loop == 0)
				return;

			do_key_press (event);

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
				(	display, root_win,
					ConfigureNotify, &Unused_xevent
				)
			)
				/* vazio */;

			XGetGeometry
			(	display, root_win, &root,
				&x, &y, &width, &height,
				&border, &depth
			);

			set_window_sizes (width, height, 0);

			break;
		    }

		}	/* end switch */

		if (main_event_loop == 0)
			return;

	}	/* end for (EVER) */

}	/* end event_loop */

/*
 ****************************************************************
 *	Malha de eventos (se a operação foi cancelada)		*
 ****************************************************************
 */
int
cancel_event_pending (void)
{
	XEvent		event;
	const MENUTB	*mp;
	char		need_flush = 0;
	int		cancela = 0;

	while (XCheckMaskEvent (display, ExposureMask|EnterWindowMask|LeaveWindowMask|ButtonPressMask, &event))
	{
		switch (event.type)
		{
		    case Expose:
		    case GraphicsExpose:
			if (event.xexpose.count != 0)
				break;

			process_expose_event (&event);
			break;

		    case EnterNotify:
			for (mp = &menutb[0]; mp < &menutb[NMENU]; mp++)
			{
				if (mp->m_win.win == event.xany.window)
					{ Shadow_up (&mp->m_win); need_flush++; break; }
			}

			break;

		    case LeaveNotify:
			for (mp = &menutb[0]; mp < &menutb[NMENU]; mp++)
			{
				if (mp->m_win.win == event.xany.window)
					{ Shadow_down (&mp->m_win); need_flush++; break; }
			}

			break;

		    case ButtonPress:
			if (event.xany.window == menutb[4].m_win.win)	/* Cancela */
				cancela++;
		}
	}

	if (need_flush)
		XFlush (display);

	return (cancela);

}	/* end event_pending */

/*
 ****************************************************************
 *	Eventos de exposição					*
 ****************************************************************
 */
void
process_expose_event (const XEvent *event)
{
	const MENUTB	*mp;

	if   (event->xany.window == option.win)
	{
		draw_option_win ();
	}
	elif (event->xany.window == volume.windata.win)
	{
		draw_submenu_win (&volume);
	}
	elif (event->xany.window == pattern.windata.win)
	{
		draw_submenu_win (&pattern);
	}
	elif (event->xany.window == hidden.windata.win)
	{
		draw_submenu_win (&hidden);
	}
	elif (event->xany.window == extension.windata.win)
	{
		draw_submenu_win (&extension);
	}
#if (0)	/*******************************************************/
	elif (event->xany.window == group.windata.win)
	{
		draw_submenu_win (&group);
	}
#endif	/*******************************************************/
	elif (event->xany.window == mode.windata.win)
	{
		draw_submenu_win (&mode);
	}
	elif (event->xany.window == zip.windata.win)
	{
		draw_submenu_win (&zip);
	}
	elif (event->xany.window == version.win)
	{
		draw_version_win ();
	}
	elif (event->xany.window == tree.win)
	{
		draw_tree_win ();
	}
	elif (event->xany.window == tree_menu.win)
	{
		draw_tree_or_dir_menu_win (TREE_MENU);
	}
	elif (event->xany.window == tree_scroll.win)
	{
		draw_scroll_win (&tree_scroll);
	}
	elif (event->xany.window == name.win)
	{
		draw_name_win ();
	}
	elif (event->xany.window == dir.win)
	{
		draw_dir_win ();
	}
	elif (event->xany.window == dir_menu.win)
	{
		draw_tree_or_dir_menu_win (DIR_MENU);
	}
	elif (event->xany.window == dir_scroll.win)
	{
		draw_scroll_win (&dir_scroll);
	}
	elif (event->xany.window == msg.win)
	{
		draw_msg_win ();
	}
	elif (event->xany.window == msg_yes.win)
	{
		draw_msg_yes_or_no_win (&msg_yes);
	}
	elif (event->xany.window == msg_no.win)
	{
		draw_msg_yes_or_no_win (&msg_no);
	}
	elif (event->xany.window == percent.win)
	{
		draw_percent_win ();
	}
	elif (event->xany.window == nm_edit.win)
	{
		draw_nm_edit_win ();
	}
	elif (event->xany.window == df_data.win)
	{
		draw_df_data_window ();
	}
	elif (event->xany.window == du_data.win)
	{
		draw_du_data_window ();
	}
	else for (mp = &menutb[0]; mp < &menutb[NMENU]; mp++)
	{
		if (mp->m_win.win == event->xany.window)
		{
			draw_menu_win (mp); break;
		}
	}

}	/* end process_expose_event */

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
	XUnmapWindow (display, volume.windata.win);
	XUnmapWindow (display, pattern.windata.win);
	XUnmapWindow (display, hidden.windata.win);
	XUnmapWindow (display, extension.windata.win);
#if (0)	/*******************************************************/
	XUnmapWindow (display, group.windata.win);
#endif	/*******************************************************/
	XUnmapWindow (display, mode.windata.win);
	XUnmapWindow (display, version.win);

	volume.mapped = 0;
	volume.highlight_line = -1;

	pattern.mapped = 0;
	pattern.highlight_line = -1;

	hidden.mapped = 0;
	hidden.highlight_line = -1;

	extension.mapped = 0;
	extension.highlight_line = -1;

#if (0)	/*******************************************************/
	group.mapped = 0;
	group.highlight_line = -1;
#endif	/*******************************************************/

	mode.mapped = 0;
	mode.highlight_line = -1;

	version_mapped = 0;

}	/* end unmap_whole_menu */

/*
 ****************************************************************
 *	Desmapeia as janelas temporárias			*
 ****************************************************************
 */
void
unmap_tmp_windows (void)
{
	XUnmapWindow (display, df_data.win);
	XUnmapWindow (display, du_data.win);
	XUnmapWindow (display, nm_edit.win);

}	/* end unmap_tmp_windows */

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

#if (0)	/*******************************************************/
	XUnmapWindow (display, pattern.windata.win);
#endif	/*******************************************************/

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

#if (0)	/*******************************************************/
	mp = &optionvec[line];

	(*mp->m_func) ();

	if (mp->m_unmap)
	{
		XUnmapWindow (display, option.win);

		option_highlight_line = -1;
	}
#endif	/*******************************************************/

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

	if (vp->f_func != NOFUNCTION)
		vp->f_func (1);

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
			tp = &button_defrag_triple;
		else
			tp = &button_ignore_triple;

		draw_button
		(	wp, tp,
			wp->width - (BUTTON_WIDTH + 2) - SHADOW - MENU_BORDER,
			MENU_BORDER + line * (wp->font_height + 2 * SHADOW) + SHADOW,
			BUTTON_WIDTH, wp->font_height - 2
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
			wp->width - 3 * MENU_BORDER - (BUTTON_WIDTH + 2), (wp->font_height + 2 * SHADOW)
		);

	   /***	smp->highlight_line = -1;	***/
	}

	draw_shadow
	(	wp->win,
		wp->triple.top_gc, wp->triple.bottom_gc,
		MENU_BORDER, MENU_BORDER + line * (wp->font_height + 2 * SHADOW),
		wp->width - 3 * MENU_BORDER - (BUTTON_WIDTH + 2), (wp->font_height + 2 * SHADOW)
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
			tp = &button_defrag_triple;
		else
			tp = &button_ignore_triple;

		draw_button
		(	wp, tp,
			wp->width - (BUTTON_WIDTH + 2) - SHADOW - MENU_BORDER,
			MENU_BORDER + line * (wp->font_height + 2 * SHADOW) + SHADOW,
			BUTTON_WIDTH, wp->font_height - 2
		);
	}

	if   (smp == &zip)
	{
		zip_tst_and_change_protection ();
	}
	elif (smp == &volume)
	{
		int		n;
		extern int	tree_max_line_offset;

		XClearWindow (display, tree.win); draw_tree_win ();

		if (extension_in_use == WIN_EXTENSION_ON && (n = tree_max_line_offset - tree.width) < 0)
		{
			set_window_sizes (0, 0, n);
			return;
		}
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

#if (0)	/*******************************************************/
/*
 ****************************************************************
 *	Movendo na janela da ÁRVORE				*
 ****************************************************************
 */
void
motion_tree_window (int y)
{

}	/* end motion_tree_window */

/*
 ****************************************************************
 *	Saindo da janela da ÁRVORE				*
 ****************************************************************
 */
void
leave_tree_window (void)
{

}	/* end leave_tree_window */
#endif	/*******************************************************/

/*
 ****************************************************************
 *	Apertou o botão na janela do INTERVALO			*
 ****************************************************************
 */
void
button_press_gap_window (int y)
{
	gap_button_state = 1;

}	/* end button_press_gap_window */

/*
 ****************************************************************
 *	Soltou o botão na janela do INTERVALO			*
 ****************************************************************
 */
void
button_release_gap_window (int y)
{
	int		x;

	gap_button_state = 0;

	if ((x = last_rubber_band_x) < 0)
		return;

	XDrawLine (display, root_win, rubber_band_gc, x, gap.y, x, gap.y + gap.height);

	set_window_sizes (0, 0, x - gap.x);

	last_rubber_band_x = -1;

}	/* end button_release_gap_window */

/*
 ****************************************************************
 *	Moveu o cursor na janela do INTERVALO			*
 ****************************************************************
 */
void
motion_gap_window (int motion_x)
{
	Window		win = root_win;
	int		x;

	if (gap_button_state == 0)
		return;

	if ((x = last_rubber_band_x) >= 0)
		XDrawLine (display, win, rubber_band_gc, x, gap.y, x, gap.y + gap.height);

	x = gap.x + motion_x;

	XDrawLine (display, win, rubber_band_gc, x, gap.y, x, gap.y + gap.height);

	last_rubber_band_x = x;

}	/* end motion_gap_window */

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
 *	Desenha somente a sombra da direita			*
 ****************************************************************
 */
void
draw_only_right_shadow (Window win, GC botShadow, int win_x, int win_y, int width, int height)
{
	int		shadow_width = SHADOW;
	int		y, w, h;

	w = width + (win_x - 1); h = height + (win_y - 1);
	y = win_y + 1;

	for (shadow_width = SHADOW; shadow_width > 0; shadow_width--, y++, w--, h--)
	{
		XDrawLine (display, win, botShadow, w, h, w, y);
	}

}	/* end draw_only_right_shadow */

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
set_window_sizes (int width, int height, int tree_width_inc)
{
	XWindowChanges		value;
	int			dir_width, menu_width;
	int			root_win_width_inc = 0, msg_height;
	int			percent_height, name_height, dir_height, tree_height;
	int			width_inc, height_inc;
	MENUTB			*mp;
	WINDATA			*wp;
	SCROLLDATA		*sp;
	const SUBMENU		*smp;
	char			root_width_changed = 0;
	static int		min_width, min_height, menu_height;

	/*
	 ****** Verifica se é a primeira vez, ... ***************
	 */
	if (present_root_width == 0)	/* Primeira vez */
	{
		mp = &menutb[0];

		menu_width  = BORDER;
		menu_height = mp->m_win.font_height + 2 * MENU_BORDER;

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
		wp		= &option;
		wp->width	= strlen (option_longest_text) * wp->font_width + 2 * SHADOW + 2 * MENU_BORDER;
		wp->height	= option_text_n * (wp->font_height + 2 * SHADOW) + 2 * MENU_BORDER;
		wp->x		= 0;
		wp->y		= wp->font_height + 2 * MENU_BORDER + 2 * BORDER;

		smp		= &zip;
		wp		= &zip.windata;
		wp->width	= smp->longest_text_len * wp->font_width + 2 * SHADOW +
				  3 * MENU_BORDER + BUTTON_WIDTH + 2;
		wp->height	= smp->text_n * (wp->font_height + 2 * SHADOW) + 2 * MENU_BORDER;

		smp		= &volume;
		wp		= &volume.windata;
		wp->width	= smp->longest_text_len * wp->font_width + 2 * SHADOW +
				  3 * MENU_BORDER + BUTTON_WIDTH + 2;
		wp->height	= smp->text_n * (wp->font_height + 2 * SHADOW) + 2 * MENU_BORDER;
		wp->x		= option.width;
		wp->y		= wp->font_height + 2 * MENU_BORDER + 2 * BORDER +
				  smp->line_offset * (wp->font_height + 2 * SHADOW) + MENU_BORDER;

		smp		= &pattern;
		wp		= &pattern.windata;
		wp->width	= smp->longest_text_len * wp->font_width + 2 * SHADOW +
				  3 * MENU_BORDER + BUTTON_WIDTH + 2;
		wp->height	= smp->text_n * (wp->font_height + 2 * SHADOW) + 2 * MENU_BORDER;
		wp->x		= option.width;
		wp->y		= wp->font_height + 2 * MENU_BORDER + 2 * BORDER +
				  smp->line_offset * (wp->font_height + 2 * SHADOW) + MENU_BORDER;

		smp		= &hidden;
		wp		= &hidden.windata;
		wp->width	= smp->longest_text_len * wp->font_width + 2 * SHADOW +
				  3 * MENU_BORDER + BUTTON_WIDTH + 2;
		wp->height	= smp->text_n * (wp->font_height + 2 * SHADOW) + 2 * MENU_BORDER;
		wp->x		= option.width;
		wp->y		= wp->font_height + 2 * MENU_BORDER + 2 * BORDER +
				  smp->line_offset * (wp->font_height + 2 * SHADOW) + MENU_BORDER;

		smp		= &extension;
		wp		= &extension.windata;
		wp->width	= smp->longest_text_len * wp->font_width + 2 * SHADOW +
				  3 * MENU_BORDER + BUTTON_WIDTH + 2;
		wp->height	= smp->text_n * (wp->font_height + 2 * SHADOW) + 2 * MENU_BORDER;
		wp->x		= option.width;
		wp->y		= wp->font_height + 2 * MENU_BORDER + 2 * BORDER +
				  smp->line_offset * (wp->font_height + 2 * SHADOW) + MENU_BORDER;

#if (0)	/*******************************************************/
		smp		= &group;
		wp		= &group.windata;
		wp->width	= smp->longest_text_len * wp->font_width + 2 * SHADOW +
				  3 * MENU_BORDER + BUTTON_WIDTH + 2;
		wp->height	= smp->text_n * (wp->font_height + 2 * SHADOW) + 2 * MENU_BORDER;
		wp->x		= option.width;
		wp->y		= wp->font_height + 2 * MENU_BORDER + 2 * BORDER +
				  smp->line_offset * (wp->font_height + 2 * SHADOW) + MENU_BORDER;
#endif	/*******************************************************/

		smp		= &mode;
		wp		= &mode.windata;
		wp->width	= smp->longest_text_len * wp->font_width + 2 * SHADOW +
				  3 * MENU_BORDER + BUTTON_WIDTH + 2;
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
		wp		= &tree;

		msg_height	= 2 * (MENU_BORDER + SHADOW) + msg.font_height;
		percent_height	= PERCENT_HEIGHT + 2 * PERCENT_BORDER;

		dir_width	= MIN_DIR_WIDTH;
		name_height	= wp->font_height + 2 * MENU_BORDER;

	   /***	dir_line_first	= 0;					***/
		dir_line_vis	= MIN_DIR_LINE_Y;
		dir_height	= MIN_DIR_LINE_Y * LINE_HEIGHT + 2 * SHADOW;
		tree_height	= dir_height + name_height + BORDER;

		/* Os tamanho totais provisórios */

		min_width	= MIN_TREE_WIDTH + dir_width + 2 * TOTAL_SCROLL_WIDTH + 5 * BORDER;
		min_height	= menu_height + msg_height + percent_height + name_height + dir_height + 6 * BORDER;

		/* Verifica se há lugar para o MENU */

		if ((width_inc = menu_width - min_width) > 0)
			{ dir_width += width_inc; min_width += width_inc; }

		present_root_width  = min_width;
		present_root_height = min_height;

		/* Verifica se foi dada uma largura maior */

		if ((width_inc = szhint.width - min_width) > 0)
		{
			dir_width	   += width_inc;
			present_root_width += width_inc;
		}

		/* Verifica se foi dada uma altura maior */

		if ((height_inc = szhint.height - MIN_DIR_LINE_Y) > 0)
		{
			dir_line_vis	    += height_inc;

			height_inc	    *= LINE_HEIGHT;
			dir_height	    += height_inc;
			tree_height	    += height_inc;

			present_root_height += height_inc;
		}

		tree_line_vis	= (tree_height - 2 * SHADOW) / LINE_HEIGHT;
		TREE_OFFSET	= ((tree_height - 2 * SHADOW) % LINE_HEIGHT) >> 1;

		wp		= &msg;
		wp->width	= present_root_width - 2 * BORDER;
		wp->height	= msg_height;
		wp->x		= BORDER;
		wp->y		= menu_height + 2 * BORDER;

		wp		= &msg_no;
		wp->width	= 7 * wp->font_width;
		wp->height	= msg.height - 2 * SHADOW - 5;
		wp->x		= msg.width - wp->width - 3 * wp->font_width - 1;
		wp->y		= SHADOW + 2 - 1;

		wp		= &msg_yes;
		wp->width	= 7 * wp->font_width;
		wp->height	= msg.height - 2 * SHADOW - 5;
		wp->x		= msg_no.x - wp->width - 3 * wp->font_width - 1;
		wp->y		= SHADOW + 2 - 1;

		wp		= &percent;
		wp->width	= present_root_width - 2 * BORDER;
		wp->height	= percent_height;
		wp->x		= BORDER;
		wp->y		= menu_height + msg_height + 3 * BORDER;

		wp		= &tree;
		wp->width	= MIN_TREE_WIDTH;
		wp->height	= tree_height;
		wp->x		= BORDER;
		wp->y		= menu_height + msg_height + percent_height + 4 * BORDER;

		sp		= &tree_scroll;
		sp->width	= TOTAL_SCROLL_WIDTH;
		sp->height	= tree_height;
		sp->x		= MIN_TREE_WIDTH + 2 * BORDER;
		sp->y		= menu_height + msg_height + percent_height + 4 * BORDER;

	   /***	sp->button_no	= 0;					***/
		sp->begin	= 2 * SHADOW + SCROLL_WIDTH;
		sp->end		= sp->height - (2 * SCROLL_WIDTH + 2 * SHADOW);
	   /***	sp->bottom	= 0;					***/
		sp->top		= 100000;
		sp->last_offset	= -1;

		wp		= &name;
		wp->width	= dir_width + TOTAL_SCROLL_WIDTH + BORDER;
		wp->height	= name_height;
		wp->x		= MIN_TREE_WIDTH + TOTAL_SCROLL_WIDTH + 3 * BORDER;
		wp->y		= menu_height + msg_height + percent_height + 4 * BORDER;

		wp		= &dir;
		wp->width	= dir_width;
		wp->height	= dir_height;
		wp->x		= MIN_TREE_WIDTH + TOTAL_SCROLL_WIDTH + 3 * BORDER;
		wp->y		= menu_height + msg_height + percent_height + name_height + 5 * BORDER;

		sp		= &dir_scroll;
		sp->width	= TOTAL_SCROLL_WIDTH;
		sp->height	= dir_height;
		sp->x		= MIN_TREE_WIDTH + TOTAL_SCROLL_WIDTH + dir.width + 4 * BORDER;
		sp->y		= menu_height + msg_height + percent_height + name_height + 5 * BORDER;

	   /***	sp->button_no	= 0;					***/
		sp->begin	= 2 * SHADOW + SCROLL_WIDTH;
		sp->end		= sp->height - (2 * SCROLL_WIDTH + 2 * SHADOW);
	   /***	sp->bottom	= 0;					***/
		sp->top		= 100000;
		sp->last_offset	= -1;

		wp		= &gap;
		wp->width	= GAP_WIDTH;
		wp->height	= tree_height;
		wp->x		= MIN_TREE_WIDTH + TOTAL_SCROLL_WIDTH + 2 * BORDER + ((BORDER - GAP_WIDTH) >> 1);
		wp->y		= menu_height + msg_height + percent_height + 4 * BORDER;

		/*
		 *	Informa o WM
		 */
		wp		   = &tree;	/* Para "LINE_HEIGHT" */

	   /***	szhint.width_inc   = 1;			***/
		szhint.height_inc  = LINE_HEIGHT;

		szhint.base_width  = 0;
		szhint.base_height = min_height - MIN_DIR_LINE_Y * LINE_HEIGHT;

		szhint.min_width   = min_width;
		szhint.min_height  = min_height;

		return;
	}

	/*
	 *	Não é a primeira vez
	 */
	if (tree_width_inc == 0)		/* Alterando os tamanhos das janelas */
	{
		if (width == 0)
			width = present_root_width;

		if (height == 0)
			height = present_root_height;

		if (width < min_width)
			width = min_width;

		if (height < min_height)
			height = min_height;

		if (width == present_root_width && height == present_root_height)
			return;

		width_inc  = width  - present_root_width;
		height_inc = height - present_root_height;

		dir_line_vis	+= height_inc / LINE_HEIGHT;
		tree_line_vis	+= height_inc / LINE_HEIGHT;

		wp		 = &msg;
		wp->width	+= width_inc;
	   /***	wp->height	 = ...;					***/
	   /***	wp->x		 = ...;					***/
	   /***	wp->y		 = ...;					***/

		wp		 = &msg_yes;
	   /***	sp->width	 = ...;					***/
	   /***	wp->height	 = ...;					***/
		wp->x		+= width_inc;
	   /***	wp->y		 = ...;					***/

		wp		 = &msg_no;
	   /***	sp->width	 = ...;					***/
	   /***	wp->height	 = ...;					***/
		wp->x		+= width_inc;
	   /***	wp->y		 = ...;					***/

		wp		 = &percent;
		wp->width	+= width_inc;
	   /***	wp->height	 = ...;					***/
	   /***	wp->x		 = ...;					***/
	   /***	wp->y		 = ...;					***/

		wp		 = &tree;
	   /***	wp->width	 = ...;					***/
		wp->height	+= height_inc;
	   /***	wp->x		 = ...;					***/
	   /***	wp->y		 = ...;					***/

		sp		 = &tree_scroll;
	   /***	sp->width	 = ...;					***/
		sp->height	+= height_inc;
	   /***	sp->x		 = ...;					***/
	   /***	sp->y		 = ...;					***/

	   /***	sp->button_no	 = ...;					***/
	   /***	sp->begin	 = ...;					***/
		sp->end		+= height_inc;
	   /***	sp->bottom	 = ...;					***/
	   /***	sp->top		 = ...;					***/
	   /***	sp->last_offset	 = ...;					***/

		wp		 = &name;
		wp->width	+= width_inc;
	   /***	wp->height	 = ...;					***/
	   /***	wp->x		 = ...;					***/
	   /***	wp->y		 = ...;					***/

		wp		 = &dir;
		wp->width	+= width_inc;
		wp->height	+= height_inc;
	   /***	wp->x		 = ...;					***/
	   /***	wp->y		 = ...;					***/

		sp		 = &dir_scroll;
	   /***	sp->width	 = ...;					***/
		sp->height	+= height_inc;
		sp->x		+= width_inc;
		sp->y		+= height_inc;

		sp->button_no	 = 0;
	   /***	sp->begin	 = ...;					***/
		sp->end		+= height_inc;
		sp->bottom	 = 0;
		sp->top		 = 100000;
		sp->last_offset	 = -1;

		wp		 = &gap;
	   /***	wp->width	 = ...;					***/
		wp->height	+= height_inc;
	   /***	wp->x		 = ...;					***/
	   /***	wp->y		 = ...;					***/

#ifdef	DEBUG
		printf ("Mudando a largura %d para %d\n", present_root_width, width);
#endif	DEBUG
		if (present_root_width != width)
			root_width_changed++;

		present_root_width  = width;
		present_root_height = height;
	}
	else					/* Movendo a separação */
	{
	   /***	wp		 = &msg;				***/
	   /***	sp->width	 = ...;					***/
	   /***	wp->height	 = ...;					***/
	   /***	wp->x		 = ...;					***/
	   /***	wp->y		 = ...;					***/

	   /***	wp		 = &msg_yes;				***/
	   /***	sp->width	 = ...;					***/
	   /***	wp->height	 = ...;					***/
	   /***	wp->x		 = ...;					***/
	   /***	wp->y		 = ...;					***/

	   /***	wp		 = &msg_no;				***/
	   /***	sp->width	 = ...;					***/
	   /***	wp->height	 = ...;					***/
	   /***	wp->x		 = ...;					***/
	   /***	wp->y		 = ...;					***/

	   /***	wp		 = &percent;				***/
	   /***	sp->width	 = ...;					***/
	   /***	wp->height	 = ...;					***/
	   /***	wp->x		 = ...;					***/
	   /***	wp->y		 = ...;					***/

		wp		 = &tree;
		wp->width	+= tree_width_inc;
	   /***	wp->height	 = ...;					***/
	   /***	wp->x		 = ...;					***/
	   /***	wp->y		 = ...;					***/

		sp		 = &tree_scroll;
	   /***	sp->width	 = ...;					***/
	   /***	sp->height	 = ...;					***/
		sp->x		 = tree.x + tree.width + BORDER;
	   /***	sp->y		 = ...;					***/

	   /***	sp->button_no	 = ...;					***/
	   /***	sp->begin	 = ...;					***/
	   /***	sp->end		 = ...;					***/
	   /***	sp->bottom	 = ...;					***/
	   /***	sp->top		 = ...;					***/
	   /***	sp->last_offset	 = ...;					***/

		wp		 = &name;
		wp->width	-= tree_width_inc;
	   /*** wp->height	 = ...;					***/
		wp->x		+= tree_width_inc;
	   /***	wp->y		 = ...;					***/

		wp		 = &dir;
		wp->width	-= tree_width_inc;
	   /*** wp->height	 = ...;					***/
		wp->x		+= tree_width_inc;
	   /***	wp->y		 = ...;					***/

		/* Verifica se deve aumentar o tamanho da janela mãe */

		if ((root_win_width_inc = MIN_DIR_WIDTH - wp->width) > 0)
		{
			name.width	+= root_win_width_inc;
			wp->width	+= root_win_width_inc;
			msg.width	+= root_win_width_inc;
			msg_yes.x	+= root_win_width_inc;
			msg_no.x	+= root_win_width_inc;
			percent.width	+= root_win_width_inc;
		}

		sp		 = &dir_scroll;
	   /***	sp->width	 = ...;					***/
	   /***	sp->height	 = ...;					***/
		sp->x		 = dir.x + dir.width + BORDER;
	   /***	sp->y		 = ...;					***/

	   /***	sp->button_no	 = ...;					***/
	   /***	sp->begin	 = ...;					***/
	   /***	sp->end		 = ...;					***/
	   /***	sp->bottom	 = ...;					***/
	   /***	sp->top		 = ...;					***/
	   /***	sp->last_offset	 = ...;					***/

		wp		 = &gap;
	   /***	wp->width	 = ...;					***/
	   /***	wp->height	 = ...;					***/
		wp->x		+= tree_width_inc;
	   /***	wp->y		 = ...;					***/

		if (root_win_width_inc > 0)
			{ present_root_width += root_win_width_inc; root_width_changed++; }

	   /***	present_root_height  = height;				***/
	}

#ifdef	DEBUG
	printf ("tree_width_inc = %d, width_sum = %d\n", tree_width_inc, tree.width + gap.width + dir.width);
#endif	DEBUG

	/*
	 *	Atualiza os tamanhos das outras janelas
	 */
	value.width  = msg.width;
	XConfigureWindow (display, msg.win, CWWidth, &value);

	value.x  = msg_yes.x;
	XConfigureWindow (display, msg_yes.win, CWX, &value);

	value.x  = msg_no.x;
	XConfigureWindow (display, msg_no.win, CWX, &value);

	value.width  = percent.width;
	XConfigureWindow (display, percent.win, CWWidth, &value);

	value.width  = tree.width;
	value.height = tree.height;
	XConfigureWindow (display, tree.win, CWWidth|CWHeight, &value);

	value.height = tree_scroll.height;
	value.x	     = tree_scroll.x;
	XConfigureWindow (display, tree_scroll.win, CWHeight|CWX, &value);

	value.width  = name.width;
	value.height = name.height;
	value.x	     = name.x;
	XConfigureWindow (display, name.win, CWWidth|CWHeight|CWX, &value);

	value.width  = dir.width;
	value.height = dir.height;
	value.x	     = dir.x;
	XConfigureWindow (display, dir.win, CWWidth|CWHeight|CWX, &value);

	value.height = dir_scroll.height;
	value.x	     = dir_scroll.x;
	XConfigureWindow (display, dir_scroll.win, CWHeight|CWX, &value);

	value.height = gap.height;
	value.x	     = gap.x;
	XConfigureWindow (display, gap.win, CWHeight|CWX, &value);

	if (root_width_changed)
	{
		value.width = present_root_width;
		XConfigureWindow (display, root_win, CWWidth, &value);
	}

#if (0)	/*******************************************************/

	/* Verifica se deve aumentar o tamanho da janela mãe */

	if (root_win_width_inc > 0)
	{
		value.width  = present_root_width + root_win_width_inc;
		XConfigureWindow (display, root_win, CWWidth, &value);

		present_root_width += root_win_width_inc;
	}

#endif	/*******************************************************/

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
		"\t%s [-g <geo>] [-f <frente>] [-b <fundo>] [-d <video>] [-M <m>] [-F <f>]\n",
		pgname, pgversion, pgname
	);
	fprintf
	(	stderr,
		"\nOpções:"
		"\t-g: Geometria <geo> (no formato <largura>x<altura>, em pixeis) \n"
		"\t-f: Use a cor de <frente> dada\n"
		"\t-b: Use a cor de <fundo> dada\n"
		"\t-d: Conecte-se ao <vídeo> dado\n"
		"\t-M: Use a fonte <m> para as janelas do menu\n"
		"\t-F: Use a fonte <f> para as janelas dos arquivos\n"
	);

	exit (2);

}	/* end help */
