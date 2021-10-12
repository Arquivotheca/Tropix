/*
 ****************************************************************
 *								*
 *			xpop3.c					*
 *								*
 *	Post Office Protocol - Version 3 (cliente gráfico)	*
 *								*
 *	Versão	4.3.0, de 07.09.02				*
 *		4.5.0, de 23.03.04				*
 *								*
 *	Módulo: xpop3						*
 *		Utilitários especiais				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2004 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/common.h>
#include <sys/disktb.h>
#include <sys/syscall.h>
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

#include "../h/xpop3.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char		pgversion[] =  "Versão: 4.5.0, de 23.03.04";

entry int		vflag;		/* Verbose */

/*
 ****** Tamanhos das margens, sombras, ... **********************
 */
#define	BORDER		1
#define	SHADOW		3
#define BUTTON_WIDTH 	(2 * FONT_WIDTH)
#define	SB_WIDTH	15
#define	MENU_BORDER	10

#define	BLOCK_BORDER	(2 * FONT_WIDTH)
#define	BLOCK_HEIGHT	100

#define	PERCENT_BORDER	(2 * FONT_WIDTH)
#define	PERCENT_HEIGHT	50

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
#define	RCV_COLOR_NM		"green"
#define	RCV_DONE_COLOR_NM	"dodgerblue"
#define	RM_COLOR_NM		"orange"
#define	RM_DONE_COLOR_NM	"orangered"

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

entry int		xwin_fd;

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

typedef struct
{
	Window		win;
	int		width, height;
	int		x, y;
	TRIPLEGC	triple;		/* As tres cores */
	int		state;		/* Estado da Barra de Rolagem */
	int		begin, end;	/* Começo e Final da janela */
	int		top, bottom;	/* Limites da barra (top: valor menor) */

}	SCROLLDATA;

/*
 ******	Dados da janela de MENU *********************************
 */
typedef struct
{
	const char	m_text[16];		/* Texto a escrever */
	void		(*m_func) (int);	/* Função a chamar */
	WINDATA		m_win;			/* Dados da janela */

}	MENUTB;	

MENUTB		menutb[] =
{
	{ "Opções",		option_function },	/* 0 */
	{ "Conecta",		do_connect_and_login },	/* 1 */
	{ "Desconecta",		do_disconnect },	/* 2 */
	{ "Recebe",		do_receive },		/* 3 */
	{ "Remove",		do_delete },		/* 4 */
	{ "Fim",		quit_function },	/* 5 */
};

#define	NMENU	(sizeof (menutb) / sizeof (MENUTB))

/*
 ******	Dados da janela das OPÇÕES ******************************
 */
void		do_receive_all_mail (void);
void		do_receive_no_mail (void);
void		do_delete_all_mail (void);
void		do_delete_no_mail (void);
void		do_undelete_mail (void);
void		do_size_limit (void);
void		do_local_mail_box_nm (void);
void		do_remote_mail_box_nm (void);

entry WINDATA		option;
extern WINDATA		sizem, lmboxnm, rmboxnm, version;

#define	NOFTN	(void (*) (void))NULL

typedef struct
{
	WINDATA		*m_windata;		/* Dados da janela correspondente */
	const char	*m_text;		/* Texto a escrever */
	void		(*m_func) (void);	/* Função a chamar quando entra na janela/aperta botão */
	void		(*m_leave_func) (void);	/* Função a chamar quando sai da janela */

}	OPTIONVEC;

const OPTIONVEC	optionvec[] =
{
   NOWINDATA,	"Recebe todas as cartas",	    do_receive_all_mail,	NOFTN,			/* 0 */
   NOWINDATA,	"NÃO recebe nenhuma carta",	    do_receive_no_mail,		NOFTN,			/* 1 */
   NOWINDATA,	"Remove todas as cartas",	    do_delete_all_mail,		NOFTN,			/* 2 */
   NOWINDATA,	"NÃO Remove nenhuma carta",	    do_delete_no_mail,		NOFTN,			/* 3 */
   NOWINDATA,	"Cancela a remoção das cartas",	    do_undelete_mail,		NOFTN,			/* 4 */
   &sizem,	"Limita o tamanho das cartas",	    do_size_limit,		NOFTN,			/* 5 */
   &lmboxnm,	"Caixa Postal local",		    do_local_mail_box_nm,	update_remote_user_nm,	/* 6 */
   &rmboxnm,	"Caixa Postal remota",		    do_remote_mail_box_nm,	NOFTN,			/* 7 */
   &version,	"Versão",			    do_version,			NOFTN			/* 8 */
};

#define	option_longest_text	optionvec[4].m_text
#define	option_text_n		(sizeof (optionvec) / sizeof (OPTIONVEC))

entry int		option_highlight_line = -1;

/*
 ******	Dados da janela do MENU do tamanho das cartas ***********
 */
entry WINDATA		sizem;

entry int		sizem_mapped;

typedef struct
{
	const char	*m_text;		/* Texto a escrever */
	int		m_size;			/* Tamanho máximo */

}	SIZE_MENU_VEC;

const SIZE_MENU_VEC	sizem_vec[] =
{
	"     1 KB",	  1 * KBSZ,
	"     2 KB",	  2 * KBSZ,
	"     5 KB",	  5 * KBSZ,
	"    10 KB",	 10 * KBSZ,
	"    20 KB",	 20 * KBSZ,
	"    50 KB",	 50 * KBSZ,
	"   100 KB",	100 * KBSZ,
	"Ilimitado",	  	 0,
};

#define	sizem_line_offset	5		/* Do menu anterior */
#define	sizem_longest_text	sizem_vec[0].m_text
#define	sizem_text_n	(sizeof (sizem_vec) / sizeof (SIZE_MENU_VEC))

entry int		sizem_highlight_line = -1;
entry int		sizem_size_in_use = 0;

/*
 ******	Janela do nome da caixa postal local ********************
 */
entry WINDATA		lmboxnm;

entry GC		rev_text_gc;

entry int		lmboxnm_mapped;

#define	lmboxnm_line_offset	6		/* Do menu anterior */

/*
 ******	Janela do nome da caixa postal remota *******************
 */
entry WINDATA		rmboxnm;

entry int		rmboxnm_mapped;

entry char		remote_mail_box_nm[64];

#define	rmboxnm_line_offset	7		/* Do menu anterior */

/*
 ******	Dados da janela da VERSÃO *******************************
 */
entry WINDATA		version;

entry int		version_mapped;

#define	version_line_offset	8			/* Das opções, acima */
#define	version_longest_text	pgversion
#define	version_text_n		1

/*
 ******	Dados da janela das MENSAGENS ***************************
 */
entry WINDATA		msg;

#define	MSGLINEs	8	/* No. de linhas */

entry char		*msg_text[MSGLINEs];

/*
 ******	Dados da janela da PERCENTAGEM **************************
 */
entry WINDATA		percent;

entry GC		percent_gc;

entry int		percent_old_percent;	/* de 0 a 1000 */
entry int		percent_old_width;

/*
 ******	Dados da janela das CARTAs ******************************
 */
entry WINDATA		mail;

#define	MIN_NLINE	12			/* Número mínimo de linhas e colunas */
#define	MIN_NCOL	80

entry int		NLINE = 24, NCOL = 120;	/* Número corrente de linhas e colunas */

entry int		NMAIL, FIRST_LINE;	/* Número total de cartas, no. da primeira linha da tela */

/*
 ******	Dados da janela dos BOTÕES ******************************
 */
entry WINDATA		rcv_button, del_button;

entry TRIPLEGC		rcv_color[NOP];			/* 0: Não, 1: Sim, 2: Feito */
entry TRIPLEGC		del_color[NOP];			/* 0: Não, 1: Sim, 2: Feito */

entry TRIPLEGC		ignore_color, do_color;

entry int		rcv_highlight_line = -1;
entry int		del_highlight_line = -1;

/*
 ******	Dados da janela da barra de Rolagem *********************
 */
entry SCROLLDATA	scroll;

entry int		scroll_button_press_y;

/*
 ******	Estrutura para a leitura do teclado *********************
 */
typedef struct
{
	char		k_code;
	char		k_changed;
	Display		*k_display;
	Window		k_win;
	GC		k_gc, k_rev_gc;
	char		*k_area;
	int		k_count;
	int		k_offset;
	int		k_size;
	int		k_x, k_y;
	void		(*k_function) (void);

}	KEYDATA;

entry KEYDATA		keydata;

/*
 ****** Protótipos de funções ***********************************
 */
void		event_loop (int main_event_loop);
int		get_pointer_y (Window win);
void		draw_menu_win (const MENUTB *mp);
void		Shadow_up (const MENUTB *mp);
void		Shadow_down (const MENUTB *mp);
void		Shadow_off (const MENUTB *mp);
void		draw_option_win (void);
void		motion_option_window (int y);
void		leave_option_window (void);
void		do_option_button_press (int y);
void		draw_sizem_win (void);
void		motion_sizem_window (int y);
void		leave_sizem_window (void);
void		do_sizem_button_press (int y);
void		draw_lmboxnm_win (void);
void		draw_rmboxnm_win (void);
void		draw_version_win (void);
void		write_msg (const char *text);
void		scroll_msg_win (void);
void		draw_msg_win (void);
void		draw_msg_line (int);
void		draw_percent_window (void);
void		draw_connection_state_button (void);
void		scroll_mail_window (int diff);
void		draw_mail_window (void);
void		draw_mail_line (int index);
void		draw_mail_rcv_win (void);
void		draw_mail_rcv_button (const MBOX *mp, int win_index);
void		draw_mail_del_win (void);
void		draw_mail_del_button (const MBOX *mp, int win_index);
void		clear_mail_rcv_del_window (void);
void		motion_rcv_button_window (int y);
void		motion_del_button_window (int y);
void		do_rcv_button_button_press (int y);
void		do_del_button_button_press (int y);
void		draw_scroll_win (void);
void		clear_scroll_window (void);
void		update_scroll_bar (int y, int NEW_FIRST_LINE);
void		reset_scroll_button (void);
void		draw_scroll_button (int y, int state);
void		do_key_press (XEvent event);
int		xlineedit (XEvent event, KEYDATA *kp);
void		xlineedit_cursor_on (const KEYDATA *kp);
void		xlineedit_cursor_off (const KEYDATA *kp);
void		draw_shadow (Window, GC, GC, int, int, int width, int height);
Pixel		get_window_color (const char *color_nm, Pixel def);
int		set_shadow_colors (Display *, Window, TRIPLEGC *, const char *, Pixel);
void		set_window_sizes (int width, int height);
void		geometry_to_size_hints (const char *geo_str, XSizeHints *hp);
void		help (void);

/*
 ****************************************************************
 *	Post Office Protocol - Version 3 (cliente gráfico)	*
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
#if (0)	/*******************************************************/
	Cursor		cursor;
#endif	/*******************************************************/
	MENUTB		*mp;

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
			fputc ('\n', stderr);
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
	xwin_fd	= ConnectionNumber (display);

	error_default_function = quit_function;

	/*
	 ****** Obtém a caixa postal inicial ********************
	 */
	get_first_mailbox_nm (argv[0]);

	/*
	 ****** Abre o arquivo de índice "status" ***************
	 */
	get_default_local_user_nm ();

	/*
	 ****** Tenta obter os valores de "XDefault" ************
	 */
	if   (user_fore_color_str != NOSTR)
		fore_color_str = user_fore_color_str;
	elif ((str = XGetDefault (display, "XPop3", "foreground")) != NOSTR)
		fore_color_str = str;
	else
		fore_color_str = default_fore_color_str;

	if   (user_back_color_str != NOSTR)
		back_color_str = user_back_color_str;
	elif ((str = XGetDefault (display, "XPop3", "background")) != NOSTR)
		back_color_str = str;
	else
		back_color_str = default_back_color_str;

	if   (user_font_str != NOSTR)
		font_str = user_font_str;
	elif ((str = XGetDefault (display, "XPop3", "font")) != NOSTR)
		font_str = str;
	else
		font_str = default_font_str;

	/*
	 ******	Obtém as cores fixas e variáveis ****************
	 */
	set_shadow_colors (display, root, &fore_triple,  fore_color_str,    0x000000);
	set_shadow_colors (display, root, &back_triple,  back_color_str,    0xBEBEBE);

	set_shadow_colors (display, root, &black_triple, BLACK_COLOR_NM,    0x000000);

	set_shadow_colors (display, root, &rcv_color[0], IGNORE_COLOR_NM,   0x000000);
	set_shadow_colors (display, root, &rcv_color[1], RCV_COLOR_NM,	    0x00CC00);
	set_shadow_colors (display, root, &rcv_color[2], RCV_DONE_COLOR_NM, 0x0000CC);

	memmove (&del_color[0], &rcv_color[0], sizeof (TRIPLEGC));
	set_shadow_colors (display, root, &del_color[1], RM_COLOR_NM,	    0x440000);
	set_shadow_colors (display, root, &del_color[2], RM_DONE_COLOR_NM,  0xCC0000);

	memmove (&ignore_color, &rcv_color[0], sizeof (TRIPLEGC));
	memmove (&do_color,     &rcv_color[1], sizeof (TRIPLEGC));

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
		szhint.base_width + mail.width, szhint.base_height + mail.height,
		0, BORDER_COLOR,
		BlackPixel (display, screen)
	);

	main_win = win;

	XStoreName (display, win, "Xpop3");

	XSelectInput (display, win, ExposureMask|KeyPressMask|StructureNotifyMask);

	classhint.res_name  = (char *)pgname;
	classhint.res_class = "XPop3";

	wmhint.input		= True;
	wmhint.initial_state	= NormalState;
	wmhint.flags		= InputHint|StateHint;

	XSetWMProperties
	(	display, win, NULL, NULL, (char **)save_argv, save_argc,
		&szhint, &wmhint, &classhint
	);

#if (0)	/*******************************************************/
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

	gcv.foreground  = back_triple.main_pixel;
	gcv.background  = fore_triple.main_pixel;
	rev_text_gc = XCreateGC (display, root, GCForeground|GCBackground|GCFont, &gcv);

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
	 ****** Prepara a janela do MENU de TAMANHOS ************
	 */
	memmove (&sizem.triple, &back_triple, sizeof (TRIPLEGC));
	sizem.text_gc = text_gc;

        sizem.win = XCreateSimpleWindow
	(	display, win,
		sizem.x, sizem.y,
		sizem.width, sizem.height,
		BORDER, BORDER_COLOR,
		sizem.triple.main_pixel
	);

	XSelectInput
	(	display, sizem.win,
		(ExposureMask|EnterWindowMask|LeaveWindowMask|
		PointerMotionMask|PointerMotionHintMask|ButtonPressMask)
	);

   /***	XMapWindow (display, sizem.win); ***/

	/*
	 *** Prepara a janela do nome da CAIXA POSTAL LOCAL *****
	 */
	memmove (&lmboxnm.triple, &back_triple, sizeof (TRIPLEGC));
	lmboxnm.text_gc = text_gc;

        lmboxnm.win = XCreateSimpleWindow
	(	display, win,
		lmboxnm.x, lmboxnm.y,
		lmboxnm.width, lmboxnm.height,
		BORDER, BORDER_COLOR,
		lmboxnm.triple.main_pixel
	);

	XSelectInput
	(	display, lmboxnm.win,
		(ExposureMask|EnterWindowMask|LeaveWindowMask|
		PointerMotionMask|PointerMotionHintMask|ButtonPressMask)
	);

   /***	XMapWindow (display, lmboxnm.win); ***/

	/*
	 *** Prepara a janela do nome da CAIXA POSTAL REMOTA ****
	 */
	memmove (&rmboxnm.triple, &back_triple, sizeof (TRIPLEGC));
	rmboxnm.text_gc = text_gc;

        rmboxnm.win = XCreateSimpleWindow
	(	display, win,
		rmboxnm.x, rmboxnm.y,
		rmboxnm.width, rmboxnm.height,
		BORDER, BORDER_COLOR,
		rmboxnm.triple.main_pixel
	);

	XSelectInput
	(	display, rmboxnm.win,
		(ExposureMask|EnterWindowMask|LeaveWindowMask|
		PointerMotionMask|PointerMotionHintMask|ButtonPressMask)
	);

   /***	XMapWindow (display, rmboxnm.win); ***/

	/*
	 ****** Prepara a janela do nome da VERSÃO **************
	 */
	memmove (&version.triple, &back_triple, sizeof (TRIPLEGC));
	version.text_gc = text_gc;

        version.win = XCreateSimpleWindow
	(	display, win,
		version.x, version.y,
		version.width, version.height,
		BORDER, BORDER_COLOR,
		version.triple.main_pixel
	);

	XSelectInput
	(	display, version.win,
		(ExposureMask|EnterWindowMask|LeaveWindowMask)
	);

   /***	XMapWindow (display, version.win); ***/

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
	 ****** Cria a janela da PERCENTAGEM ********************
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
	 ****** Cria as janela dos BOTÕES ***********************
	 */
	memmove (&rcv_button.triple, &back_triple, sizeof (TRIPLEGC));

        rcv_button.win = XCreateSimpleWindow
	(	display, win,
		rcv_button.x, rcv_button.y,
		rcv_button.width, rcv_button.height,
		BORDER, BORDER_COLOR,
		rcv_button.triple.main_pixel
	);

	XSelectInput
	(	display, rcv_button.win,
		(ExposureMask|ButtonPressMask|ButtonReleaseMask|
		PointerMotionMask|PointerMotionHintMask|EnterWindowMask|LeaveWindowMask)
	);

	XMapWindow (display, rcv_button.win);

	memmove (&del_button.triple, &back_triple, sizeof (TRIPLEGC));

        del_button.win = XCreateSimpleWindow
	(	display, win,
		del_button.x, del_button.y,
		del_button.width, del_button.height,
		BORDER, BORDER_COLOR,
		del_button.triple.main_pixel
	);

	XSelectInput
	(	display, del_button.win,
		(ExposureMask|ButtonPressMask|ButtonReleaseMask|
		PointerMotionMask|PointerMotionHintMask|EnterWindowMask|LeaveWindowMask)
	);

	XMapWindow (display, del_button.win);

	/*
	 ****** Cria a janela das CARTAS ************************
	 */
	memmove (&mail.triple, &back_triple, sizeof (TRIPLEGC));
	mail.text_gc = text_gc;

        mail.win = XCreateSimpleWindow
	(	display, win,
		mail.x, mail.y,
		mail.width, mail.height,
		BORDER, BORDER_COLOR,
		mail.triple.main_pixel
	);

	XSelectInput
	(	display, mail.win,
		ExposureMask
	);

	XMapWindow (display, mail.win);

	/*
	 ****** Cria a janela da BARRA de ROLAGEM ***************
	 */
	memmove (&scroll.triple, &back_triple, sizeof (TRIPLEGC));

        scroll.win = XCreateSimpleWindow
	(	display, win,
		scroll.x, scroll.y,
		scroll.width, scroll.height,
		BORDER, BORDER_COLOR,
		scroll.triple.main_pixel
	);

	XSelectInput
	(	display, scroll.win,
		(ExposureMask|ButtonPressMask|ButtonReleaseMask|
		Button1MotionMask|PointerMotionHintMask|EnterWindowMask|LeaveWindowMask)
	);

	XMapWindow (display, scroll.win);

	/*
	 ******	Entra na malha de eventos *******************************
	 */
	event_loop (1);

	/* Não retorna */

	return (0);

}	/* end xpop3 */

/*
 ****************************************************************
 *	Malha de eventos					*
 ****************************************************************
 */
void
event_loop (int main_event_loop)
{
	XEvent		event;
	MENUTB		*mp;

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
			elif (event.xany.window == sizem.win)
			{
				draw_sizem_win ();
			}
			elif (event.xany.window == lmboxnm.win)
			{
				draw_lmboxnm_win ();
			}
			elif (event.xany.window == rmboxnm.win)
			{
				draw_rmboxnm_win ();
			}
			elif (event.xany.window == version.win)
			{
				draw_version_win ();
			}
			elif (event.xany.window == msg.win)
			{
				draw_msg_win ();
			}
			elif (event.xany.window == percent.win)
			{
				draw_percent_window ();
			}
			elif (event.xany.window == mail.win)
			{
				draw_mail_window ();
			}
			elif (event.xany.window == rcv_button.win)
			{
				draw_mail_rcv_win ();
			}
			elif (event.xany.window == del_button.win)
			{
				draw_mail_del_win ();
			}
			elif (event.xany.window == scroll.win)
			{
				draw_scroll_win ();
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
			for (mp = &menutb[0]; mp < &menutb[NMENU]; mp++)
			{
				if (mp->m_win.win == event.xany.window)
					{ Shadow_up (mp); break; }
			}

			break;

		    case LeaveNotify:
			if   (event.xany.window == option.win)
			{
#if (0)	/*******************************************************/
				if (option_highlight_line < 0)
#endif	/*******************************************************/
				if (sizem_mapped || lmboxnm_mapped || rmboxnm_mapped || version_mapped)
					break;

			   	XUnmapWindow (display, option.win);

				option_highlight_line = -1;
			}
			elif (event.xany.window == sizem.win)
			{
			   	XUnmapWindow (display, sizem.win);
				sizem_mapped = 0;
				sizem_highlight_line = -1;

			   	XUnmapWindow (display, option.win);
				option_highlight_line = -1;
			}
			elif (event.xany.window == lmboxnm.win)
			{
				KEYDATA		*kp = &keydata;

			   	XUnmapWindow (display, lmboxnm.win);
				lmboxnm_mapped = 0;

				if (kp->k_changed && kp->k_area == local_user_nm)
				{
					update_remote_user_nm ();
					kp->k_changed = 0;
				}

				kp->k_code = 0;

			   	XUnmapWindow (display, option.win);
				option_highlight_line = -1;
			}
			elif (event.xany.window == rmboxnm.win)
			{
				KEYDATA		*kp = &keydata;

			   	XUnmapWindow (display, rmboxnm.win);
				rmboxnm_mapped = 0;
				kp->k_code = 0;

			   	XUnmapWindow (display, option.win);
				option_highlight_line = -1;
			}
			elif (event.xany.window == version.win)
			{
			   	XUnmapWindow (display, version.win);

			   	XUnmapWindow (display, option.win);
				option_highlight_line = -1;
			}
			else for (mp = &menutb[0]; mp < &menutb[NMENU]; mp++)
			{
				if (mp->m_win.win == event.xany.window)
					{ Shadow_off (mp); break; }
#if (0)	/*******************************************************/
					{ Shadow_down (mp); break; }
#endif	/*******************************************************/
			}

			break;

		    case ButtonPress:
			if (main_event_loop == 0)
				return;

			if   (event.xany.window == option.win)
			{
				do_option_button_press (event.xbutton.y);
			}
			elif (event.xany.window == sizem.win)
			{
				do_sizem_button_press (event.xbutton.y);
			}
			elif (event.xany.window == scroll.win)
			{
				scroll_button_press_y = event.xbutton.y;
				update_scroll_bar (event.xbutton.y, 0);
			}
			elif (event.xany.window == rcv_button.win)
			{
				do_rcv_button_button_press (event.xbutton.y);
			}
			elif (event.xany.window == del_button.win)
			{
				do_del_button_button_press (event.xbutton.y);
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

			if (event.xany.window == scroll.win)
			{
				reset_scroll_button ();
			}

			break;

		    case MotionNotify:
			if   (event.xany.window == option.win)
			{
#if (0)	/*******************************************************/
				if (sizem_mapped || lmboxnm_mapped || rmboxnm_mapped)
					break;
#endif	/*******************************************************/

				motion_option_window (get_pointer_y (option.win));
			}
			elif (event.xany.window == sizem.win)
			{
				motion_sizem_window (get_pointer_y (sizem.win));
			}
			elif (event.xany.window == scroll.win)
			{
				update_scroll_bar (get_pointer_y (scroll.win), 0);
			}
			elif (event.xany.window == rcv_button.win)
			{
				motion_rcv_button_window (get_pointer_y (rcv_button.win));
			}
			elif (event.xany.window == del_button.win)
			{
				motion_del_button_window (get_pointer_y (del_button.win));
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

#if (0)	/*******************************************************/
	Shadow_down (mp);
#endif	/*******************************************************/

}	/* end draw_menu_win */

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

#if (0)	/*******************************************************/
/*
 ****************************************************************
 *	Abaixa um "botão"					*
 ****************************************************************
 */
void
Shadow_down (const MENUTB *mp)
{
	draw_shadow
	(	mp->m_win.win,
		mp->m_win.triple.bottom_gc, mp->m_win.triple.top_gc,
		0, 0,
		mp->m_win.width, mp->m_win.height
	);

}	/* end Shadow_down */
#endif	/*******************************************************/

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
 *	Entrei na janela das OPÇÕES				*
 ****************************************************************
 */
void
motion_option_window (int y)
{
	const OPTIONVEC	*op;
	int		line;

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

	/*
	 *	Verifica se precisa apertar o botão ou não
	 */
	op = &optionvec[line];

	if (op->m_windata != NOWINDATA)
		(*op->m_func) ();

#if (0)	/*******************************************************/
	/*
	 *	Experimental
	 */
	if   (line == 5)
		do_size_limit ();
	elif (line == 6)
		do_mail_box_nm ();
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
	const OPTIONVEC	*op;
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

	/*
	 *	Apaga a janela associada
	 */
	op = &optionvec[line];

	if (op->m_leave_func != NOFTN && keydata.k_changed)
		{ (*op->m_leave_func) (); keydata.k_changed = 0; }

	if (op->m_windata != NOWINDATA)
	{

	   	XUnmapWindow (display, op->m_windata->win);

		sizem_mapped = lmboxnm_mapped = rmboxnm_mapped = version_mapped = 0;

#if (0)	/*******************************************************/
		KEYDATA		*kp = &keydata;

		kp->k_code = 0;
#endif	/*******************************************************/
	}


}	/* end leave_option_window */

/*
 ****************************************************************
 *	Apertou a tecla das OPÇÕES				*
 ****************************************************************
 */
void
do_option_button_press (int y)
{
	const OPTIONVEC		*op;

	if (option_highlight_line < 0)
		return;

	op = &optionvec[option_highlight_line];

	if (op->m_windata == NOWINDATA)
	{
		(*op->m_func) ();
	}
	else
	{
	   	XUnmapWindow (display, option.win);

		option_highlight_line = -1;
	}

}	/* end do_option_button_press */

/*
 ****************************************************************
 *	Recebe todas as cartas					*
 ****************************************************************
 */
void
do_receive_all_mail (void)
{
	MBOX		**mpv;
	MBOX		*mp;
	int		index, win_index;

	if ((mpv = mail_vec_tb) == NULL)
		return;

	for (index = 0; index < NMAIL; index++, mpv++)
	{
		if ((mp = *mpv) == NOMBOX)
			continue;

		if (mp->m_rcv != DONE)
			mp->m_rcv = DO;

		if ((unsigned)(win_index = index - FIRST_LINE) < NLINE)
			draw_mail_rcv_button (mp, win_index);
	}

}	/* end do_receive_all_mail */

/*
 ****************************************************************
 *	NÃO recebe nehuma carta					*
 ****************************************************************
 */
void
do_receive_no_mail (void)
{
	MBOX		**mpv;
	MBOX		*mp;
	int		index, win_index;

	if ((mpv = mail_vec_tb) == NULL)
		return;

	for (index = 0; index < NMAIL; index++, mpv++)
	{
		if ((mp = *mpv) == NOMBOX)
			continue;

		if (mp->m_rcv != DONE)
			mp->m_rcv = IGNORE;

		if ((unsigned)(win_index = index - FIRST_LINE) < NLINE)
			draw_mail_rcv_button (mp, win_index);
	}

}	/* end do_receive_no_mail */

/*
 ****************************************************************
 *	Remove todas as cartas					*
 ****************************************************************
 */
void
do_delete_all_mail (void)
{
	MBOX		**mpv;
	MBOX		*mp;
	int		index, win_index;

	if ((mpv = mail_vec_tb) == NULL)
		return;

	for (index = 0; index < NMAIL; index++, mpv++)
	{
		if ((mp = *mpv) == NOMBOX)
			continue;

		if (mp->m_del != DONE)
			mp->m_del = DO;

		if ((unsigned)(win_index = index - FIRST_LINE) < NLINE)
			draw_mail_del_button (mp, win_index);
	}

}	/* end do_delete_all_mail */

/*
 ****************************************************************
 *	NÃO remove nehuma carta					*
 ****************************************************************
 */
void
do_delete_no_mail (void)
{
	MBOX		**mpv;
	MBOX		*mp;
	int		index, win_index;

	if ((mpv = mail_vec_tb) == NULL)
		return;

	for (index = 0; index < NMAIL; index++, mpv++)
	{
		if ((mp = *mpv) == NOMBOX)
			continue;

		if (mp->m_del != DONE)
			mp->m_del = IGNORE;

		if ((unsigned)(win_index = index - FIRST_LINE) < NLINE)
			draw_mail_del_button (mp, win_index);
	}

}	/* end do_delete_no_mail */

/*
 ****************************************************************
 *	Cancela a remoção das cartas				*
 ****************************************************************
 */
void
do_undelete_mail (void)
{
	MBOX		**mpv;
	MBOX		*mp;
	int		index, win_index;

	if ((mpv = mail_vec_tb) == NULL)
		return;

	if (do_reset () < 0)
		return;

	for (index = 0; index < NMAIL; index++, mpv++)
	{
		if ((mp = *mpv) == NOMBOX)
			continue;

		if (mp->m_del == DONE)
			mp->m_del = IGNORE;

		if ((unsigned)(win_index = index - FIRST_LINE) < NLINE)
			draw_mail_del_button (mp, win_index);
	}

}	/* end do_undelete_mail */

/*
 ****************************************************************
 *	Muda o limite de tamanho das cartas			*
 ****************************************************************
 */
void
do_size_limit (void)
{
   	XMapRaised (display, sizem.win);

	sizem_mapped = 1;

}	/* end do_size_limit */

/*
 ****************************************************************
 *	Desenha o MENU de tamanhos				*
 ****************************************************************
 */
void
draw_sizem_win (void)
{
	int		line;
	const char	*str;
	const TRIPLEGC	*tp;

	draw_shadow
	(	sizem.win,
		option.triple.top_gc, option.triple.bottom_gc,
		0, 0,
		sizem.width, sizem.height
	);

	for (line = 0; line < sizem_text_n; line++)
	{
		str = sizem_vec[line].m_text;

		XDrawString
		(	display, sizem.win,
			option.text_gc,
			MENU_BORDER + SHADOW,
			MENU_BORDER + (line + 1) * (FONT_HEIGHT + 2 * SHADOW) - (FONT_DESCENT + SHADOW),
			str, strlen (str)
		);

		if (sizem_vec[line].m_size == sizem_size_in_use)
			tp = &do_color;
		else
			tp = &ignore_color;

		XFillRectangle
		(	display, sizem.win, tp->main_gc,
			sizem.width - BUTTON_WIDTH - SHADOW, 
			MENU_BORDER + line * (FONT_HEIGHT + 2 * SHADOW) + SHADOW,
			BUTTON_WIDTH - 2, FONT_HEIGHT - 2
		);

		draw_shadow
		(	sizem.win,
			tp->top_gc, tp->bottom_gc,
			sizem.width - BUTTON_WIDTH - SHADOW, 
			MENU_BORDER + line * (FONT_HEIGHT + 2 * SHADOW) + SHADOW,
			BUTTON_WIDTH - 2, FONT_HEIGHT - 2
		);
	}

}	/* end draw_sizem_win */

/*
 ****************************************************************
 *	Entrei na janela do MENU de tamanhos			*
 ****************************************************************
 */
void
motion_sizem_window (int y)
{
	int		line;

	line = (y - MENU_BORDER) / (FONT_HEIGHT + 2 * SHADOW);

	if ((unsigned)line >= sizem_text_n)
		return;

	if (line == sizem_highlight_line)
		return;

	if (sizem_highlight_line >= 0)
		leave_sizem_window ();

	draw_shadow
	(	sizem.win,
		option.triple.top_gc, option.triple.bottom_gc,
		MENU_BORDER, MENU_BORDER + line * (FONT_HEIGHT + 2 * SHADOW),
		sizem.width - 2 * MENU_BORDER - BUTTON_WIDTH, (FONT_HEIGHT + 2 * SHADOW)
	);

	sizem_highlight_line = line;

}	/* end enter_menu_window */

/*
 ****************************************************************
 *	Saí da janela do MENU de tamanhos			*
 ****************************************************************
 */
void
leave_sizem_window (void)
{
	int		line;

	if ((line = sizem_highlight_line) < 0)
		return;

	draw_shadow
	(	sizem.win,
		option.triple.main_gc, option.triple.main_gc,
		MENU_BORDER, MENU_BORDER + line * (FONT_HEIGHT + 2 * SHADOW),
		sizem.width - 2 * MENU_BORDER - BUTTON_WIDTH, (FONT_HEIGHT + 2 * SHADOW)
	);

	sizem_highlight_line = -1;

}	/* end leave_sizem_window */

/*
 ****************************************************************
 *	Apertou a tecla no MENU de tamanhos			*
 ****************************************************************
 */
void
do_sizem_button_press (int y)
{
	int		line;
	const TRIPLEGC	*tp;

	if (sizem_highlight_line < 0)
		return;

	sizem_size_in_use = sizem_vec[sizem_highlight_line].m_size;

	for (line = 0; line < sizem_text_n; line++)
	{
		if (line == sizem_highlight_line)
			tp = &do_color;
		else
			tp = &ignore_color;

		XFillRectangle
		(	display, sizem.win, tp->main_gc,
			sizem.width - BUTTON_WIDTH - SHADOW, 
			MENU_BORDER + line * (FONT_HEIGHT + 2 * SHADOW) + SHADOW,
			BUTTON_WIDTH - 2, FONT_HEIGHT - 2
		);

		draw_shadow
		(	sizem.win,
			tp->top_gc, tp->bottom_gc,
			sizem.width - BUTTON_WIDTH - SHADOW, 
			MENU_BORDER + line * (FONT_HEIGHT + 2 * SHADOW) + SHADOW,
			BUTTON_WIDTH - 2, FONT_HEIGHT - 2
		);
	}

}	/* end do_sizem_button_press */

/*
 ****************************************************************
 *	Torna visível a janela do nome da caixa postal local	*
 ****************************************************************
 */
void
do_local_mail_box_nm (void)
{
	int		len;
	KEYDATA		*kp = &keydata;

	/*
	 *	Prepara a estrutura para a edição
	 */
	len = strlen (local_user_nm);

	kp->k_code	= 'M';
	kp->k_changed	= 0;
	kp->k_display	= display;
	kp->k_win	= lmboxnm.win;
	kp->k_gc	= lmboxnm.text_gc;
	kp->k_rev_gc	= rev_text_gc;
	kp->k_area	= local_user_nm;
	kp->k_count	=
	kp->k_offset	= len;
	kp->k_size	= sizeof (local_user_nm);
	kp->k_x		= MENU_BORDER + SHADOW,
	kp->k_y		= MENU_BORDER + (FONT_HEIGHT + 2 * SHADOW) - (FONT_DESCENT + SHADOW);
   /***	kp->k_function	= ...;	***/

	/*
	 *	Mapeia a janela
	 */
   	XMapRaised (display, lmboxnm.win);

	lmboxnm_mapped = 1;

}	/* end do_local_mail_box_nm */

/*
 ****************************************************************
 *	Desenha a janela do nome da caixa postal local		*
 ****************************************************************
 */
void
draw_lmboxnm_win (void)
{
	const KEYDATA	*kp = &keydata;

	draw_shadow
	(	lmboxnm.win,
		option.triple.top_gc, option.triple.bottom_gc,
		0, 0,
		lmboxnm.width, lmboxnm.height
	);

	XDrawString
	(	display, lmboxnm.win,
		lmboxnm.text_gc,
		MENU_BORDER + SHADOW,
		MENU_BORDER + (FONT_HEIGHT + 2 * SHADOW) - (FONT_DESCENT + SHADOW),
		kp->k_area, kp->k_count + 1
	);

	XDrawImageString
	(	display, lmboxnm.win,
		rev_text_gc,
		MENU_BORDER + SHADOW + kp->k_offset * FONT_WIDTH,
		MENU_BORDER + (FONT_HEIGHT + 2 * SHADOW) - (FONT_DESCENT + SHADOW),
		" ", 1
	);

}	/* end draw_lmboxnm_win */

/*
 ****************************************************************
 *	Torna visível a janela do nome da caixa postal remota	*
 ****************************************************************
 */
void
do_remote_mail_box_nm (void)
{
	int		len;
	KEYDATA		*kp = &keydata;

	/*
	 *	Prepara a estrutura para a edição
	 */
	len = strlen (remote_mail_box_nm);

	kp->k_code	= 'M';
	kp->k_changed	= 0;
	kp->k_display	= display;
	kp->k_win	= rmboxnm.win;
	kp->k_gc	= rmboxnm.text_gc;
	kp->k_rev_gc	= rev_text_gc;
	kp->k_area	= remote_mail_box_nm;
	kp->k_count	=
	kp->k_offset	= len;
	kp->k_size	= sizeof (remote_mail_box_nm);
	kp->k_x		= MENU_BORDER + SHADOW,
	kp->k_y		= MENU_BORDER + (FONT_HEIGHT + 2 * SHADOW) - (FONT_DESCENT + SHADOW);
   /***	kp->k_function	= ...;	***/

	/*
	 *	Mapeia a janela
	 */
   	XMapRaised (display, rmboxnm.win);

	rmboxnm_mapped = 1;

}	/* end do_remote_mail_box_nm */

/*
 ****************************************************************
 *	Desenha a janela do nome da caixa postal remota		*
 ****************************************************************
 */
void
draw_rmboxnm_win (void)
{
	const KEYDATA	*kp = &keydata;

	draw_shadow
	(	rmboxnm.win,
		option.triple.top_gc, option.triple.bottom_gc,
		0, 0,
		rmboxnm.width, rmboxnm.height
	);

	XDrawString
	(	display, rmboxnm.win,
		rmboxnm.text_gc,
		MENU_BORDER + SHADOW,
		MENU_BORDER + (FONT_HEIGHT + 2 * SHADOW) - (FONT_DESCENT + SHADOW),
		kp->k_area, kp->k_count + 1
	);

	XDrawImageString
	(	display, rmboxnm.win,
		rev_text_gc,
		MENU_BORDER + SHADOW + kp->k_offset * FONT_WIDTH,
		MENU_BORDER + (FONT_HEIGHT + 2 * SHADOW) - (FONT_DESCENT + SHADOW),
		" ", 1
	);

}	/* end draw_rmboxnm_win */

/*
 ****************************************************************
 *	Mapeia a janela da versão				*
 ****************************************************************
 */
void
do_version (void)
{
#if (0)	/*******************************************************/
   	XUnmapWindow (display, sizem.win);
   	XUnmapWindow (display, mboxnm.win);

	option_highlight_line = -1;
	sizem_mapped = 0;
	mboxnm_mapped = 0;
#endif	/*******************************************************/

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
 *	Escreve uma mensagem					*
 ****************************************************************
 */
void
write_msg (const char *text)
{
	int		len;

	if (msg_text[MSGLINEs - 1] != NOSTR)
		scroll_msg_win ();

	len = strlen (text);

	if ((msg_text[MSGLINEs - 1] = malloc (len + 1)) == NOSTR)
		error (NOSTR);

	strcpy (msg_text[MSGLINEs - 1], text);

	draw_msg_line (MSGLINEs - 1);

}	/* end write_msg */

/*
 ****************************************************************
 *	Rola a janela de mensagens				*
 ****************************************************************
 */
void
scroll_msg_win (void)
{
	int		index;

	XCopyArea
	(	display, msg.win, msg.win, msg.text_gc,
		MENU_BORDER, FONT_HEIGHT + MENU_BORDER,
		msg.width - 2 * MENU_BORDER, (MSGLINEs - 1) * FONT_HEIGHT,
		MENU_BORDER, MENU_BORDER
	);

	XClearArea
	(	display, msg.win,
		MENU_BORDER, (MSGLINEs - 1) * FONT_HEIGHT + MENU_BORDER,
		msg.width - 2 * MENU_BORDER, FONT_HEIGHT,
		False
	);

	if (msg_text[0] != NOSTR)
		free (msg_text[0]);

	for (index = 0; index < MSGLINEs - 1; index++)
		msg_text[index] = msg_text[index + 1];

	msg_text[MSGLINEs - 1] = NOSTR;

}	/* end scroll_msg_win */

/*
 ****************************************************************
 *	(Re)Desenha a janela das mensagens			*
 ****************************************************************
 */
void
draw_msg_win (void)
{
	int		index;

	draw_shadow
	(	msg.win,
		msg.triple.top_gc, msg.triple.bottom_gc,
		0, 0,
		msg.width, msg.height
	);

	for (index = 0; index < MSGLINEs; index++)
		draw_msg_line (index);

}	/* end  draw_msg_window */

/*
 ****************************************************************
 *	Desenha uma linha das mensagens				*
 ****************************************************************
 */
void
draw_msg_line (int index)
{
	const char	*text;
	int		len;

	if ((text = msg_text[index]) == NOSTR)
		return;

	len = strlen (text);

	XDrawImageString
	(	display, msg.win,
		msg.text_gc,
		MENU_BORDER, (index + 1) * FONT_HEIGHT - FONT_DESCENT + MENU_BORDER,
		text, len
	);

}	/* end draw_msg_line */

/*
 ****************************************************************
 *	Prepara para ler a senha				*
 ****************************************************************
 */
void
prepare_to_read_passwd (void (*function) (void))
{
	KEYDATA		*kp = &keydata;

	kp->k_code	= 'P';
	kp->k_changed	= 0;
	kp->k_display	= display;
	kp->k_win	= msg.win;
	kp->k_gc	= msg.text_gc;
	kp->k_rev_gc	= rev_text_gc;
	kp->k_area	= passwd_str;
	kp->k_count	= 0;
	kp->k_offset	= 0;
	kp->k_size	= sizeof (passwd_str);
	kp->k_x		= MENU_BORDER +	(sizeof ("Senha: ") - 1) * FONT_WIDTH;
	kp->k_y		= (MSGLINEs - 1 + 1) * FONT_HEIGHT - FONT_DESCENT + MENU_BORDER;
	kp->k_function	= function;

	XDrawImageString
	(	display, kp->k_win,
		kp->k_rev_gc,
		kp->k_x + kp->k_count * FONT_WIDTH,
		kp->k_y,
		" ", 1
	);

}	/* end prepare_to_read_passwd */

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

	draw_connection_state_button ();

	XFillRectangle
	(	display, percent.win, black_triple.main_gc,
		2 * PERCENT_BORDER + PERCENT_HEIGHT + SHADOW, PERCENT_BORDER + SHADOW,
		percent.width - 2 * (PERCENT_BORDER + SHADOW) - PERCENT_BORDER - PERCENT_HEIGHT,
		percent.height - 2 * (PERCENT_BORDER + SHADOW)
	);

	draw_shadow
	(	percent.win,
		percent.triple.top_gc, percent.triple.bottom_gc,
		2 * PERCENT_BORDER + PERCENT_HEIGHT, PERCENT_BORDER,
		percent.width - 3 * PERCENT_BORDER - PERCENT_HEIGHT,
		percent.height - 2 * PERCENT_BORDER
	);

	percent_old_width = 0;

	draw_percent_percent (percent_old_percent, 0);

}	/* end draw_percent_window */

/*
 ****************************************************************
 *	(Re)Desenha a janela das percentagem			*
 ****************************************************************
 */
void
draw_percent_percent (int done, int total)
{
	int		percent_new_width, percent_new_percent;

	percent_new_percent = done;

	if (total != 0)
		{ percent_new_percent *= 1000; percent_new_percent /= total; }

	percent_new_width = percent_new_percent *
			    (percent.width - 3 * PERCENT_BORDER  - 2 * SHADOW - PERCENT_HEIGHT) / 1000;

	if   (percent_new_width == percent_old_width)
	{
		return;
	}
	elif (percent_new_width < percent_old_width)
	{
		XFillRectangle
		(	display, percent.win, black_triple.main_gc,
			2 * PERCENT_BORDER + SHADOW + PERCENT_HEIGHT + percent_new_width,
			PERCENT_BORDER + SHADOW,
			percent_old_width - percent_new_width,
			percent.height - 2 * (PERCENT_BORDER + SHADOW)
		);
	}
	else	/* percent_new_width > percent_old_width */
	{
		XFillRectangle
		(	display, percent.win, percent_gc,
			2 * PERCENT_BORDER + SHADOW + PERCENT_HEIGHT + percent_old_width,
			PERCENT_BORDER + SHADOW, 
			percent_new_width - percent_old_width,
			percent.height - 2 * (PERCENT_BORDER + SHADOW)
		);
	}

	percent_old_width = percent_new_width; percent_old_percent = percent_new_percent;

}	/* end draw_percent_percent */

/*
 ****************************************************************
 *	Desenha o botão do estado da conexão			*
 ****************************************************************
 */
void
draw_connection_state_button (void)
{
	const TRIPLEGC	*tp;

	switch (connection_state)
	{
	    default:
		tp = &black_triple;
		break;

	    case CONNECTED:
		tp = &rcv_color[1];
		break;

	    case LOGGED:
		tp = &rcv_color[2];
		break;
	}

	XFillRectangle
	(	display, percent.win, tp->main_gc,
		PERCENT_BORDER, PERCENT_BORDER,
		PERCENT_HEIGHT, PERCENT_HEIGHT
	);

	draw_shadow
	(	percent.win,
		percent.triple.top_gc, percent.triple.bottom_gc,
		PERCENT_BORDER, PERCENT_BORDER,
		PERCENT_HEIGHT, PERCENT_HEIGHT
	);

}	/* end draw_connection_state_button */

/*
 ****************************************************************
 *	Avança ou retrocede a janela de cartas			*
 ****************************************************************
 */
void
scroll_mail_window (int diff)
{
	int		index;

	if   ((unsigned)diff >= NLINE)
	{
		draw_mail_window ();
	}
	elif (diff > 0)
	{
		XCopyArea
		(	display, mail.win, mail.win, mail.text_gc,
			SHADOW, diff * FONT_HEIGHT + SHADOW,
			mail.width - 2 * SHADOW, (NLINE - diff) * FONT_HEIGHT,
			SHADOW, SHADOW
		);

		XCopyArea
		(	display, rcv_button.win, rcv_button.win, rcv_color[0].main_gc,
			SHADOW, diff * FONT_HEIGHT + SHADOW,
			rcv_button.width - 2 * SHADOW, (NLINE - diff) * FONT_HEIGHT,
			SHADOW, SHADOW
		);

		XCopyArea
		(	display, del_button.win, del_button.win, del_color[0].main_gc,
			SHADOW, diff * FONT_HEIGHT + SHADOW,
			del_button.width - 2 * SHADOW, (NLINE - diff) * FONT_HEIGHT,
			SHADOW, SHADOW
		);

		for (index = FIRST_LINE + NLINE - diff; index < FIRST_LINE + NLINE; index++)
			draw_mail_line (index);
	}
	elif (diff < 0)
	{
		diff = -diff;

		XCopyArea
		(	display, mail.win, mail.win, mail.text_gc,
			SHADOW, SHADOW,
			mail.width - 2 * SHADOW, (NLINE - diff) * FONT_HEIGHT,
			SHADOW, diff * FONT_HEIGHT + SHADOW
		);

		XCopyArea
		(	display, rcv_button.win, rcv_button.win, rcv_color[0].main_gc,
			SHADOW, SHADOW,
			rcv_button.width - 2 * SHADOW, (NLINE - diff) * FONT_HEIGHT,
			SHADOW, diff * FONT_HEIGHT + SHADOW
		);

		XCopyArea
		(	display, del_button.win, del_button.win, del_color[0].main_gc,
			SHADOW, SHADOW,
			del_button.width - 2 * SHADOW, (NLINE - diff) * FONT_HEIGHT,
			SHADOW, diff * FONT_HEIGHT + SHADOW
		);

		for (index = FIRST_LINE; index < FIRST_LINE + diff; index++)
			draw_mail_line (index);
	}

}	/* end scroll_mail_window */

/*
 ****************************************************************
 *	Apaga a janela de cartas				*
 ****************************************************************
 */
void
clear_mail_window (void)
{
	XFillRectangle
	(	display, mail.win, mail.triple.main_gc,
		SHADOW, SHADOW,
		mail.width - 2 * SHADOW, mail.height - 2 * SHADOW
	);

}	/* end clear_mail_window */

/*
 ****************************************************************
 *	Desenha tôda a janela de cartas				*
 ****************************************************************
 */
void
draw_mail_window (void)
{
	int		index;

	draw_shadow
	(	mail.win,
		mail.triple.top_gc, mail.triple.bottom_gc,
		0, 0,
		mail.width, mail.height
	);

	if (mail_vec_tb == NULL)
		return;

	for (index = FIRST_LINE; index < FIRST_LINE + NLINE; index++)
		draw_mail_line (index);

}	/* end draw_mail_window */

/*
 ****************************************************************
 *	Desenha uma linha de carta				*
 ****************************************************************
 */
void
draw_mail_line (int index)
{
	int		win_index;
	const MBOX	*mp;

	if ((unsigned)index >= NMAIL)
		return;

	if ((unsigned)(win_index = index - FIRST_LINE) >= NLINE)
		return;

	mp = mail_vec_tb[index];

	XDrawImageString
	(	display, mail.win,
		mail.text_gc,
		SHADOW, (win_index + 1) * FONT_HEIGHT - FONT_DESCENT + SHADOW,
		mp->m_text, mp->m_len
	);

	draw_mail_rcv_button (mp, win_index);
	draw_mail_del_button (mp, win_index);

}	/* end draw_mail_line */

/*
 ****************************************************************
 *	Desenha o sombreado dos botões de recepção 		*
 ****************************************************************
 */
void
draw_mail_rcv_win (void)
{
	draw_shadow
	(	rcv_button.win,
		rcv_button.triple.top_gc, rcv_button.triple.bottom_gc,
		0, 0,
		rcv_button.width, rcv_button.height
	);

}	/* end draw_mail_rcv_win */

/*
 ****************************************************************
 *	Desenha o botão de recepção uma linha de carta		*
 ****************************************************************
 */
void
draw_mail_rcv_button (const MBOX *mp, int win_index)
{
	const TRIPLEGC	*tp;

	tp = &rcv_color[mp->m_rcv];

	XFillRectangle
	(	display, rcv_button.win, tp->main_gc,
		1 + SHADOW, win_index * FONT_HEIGHT  + 1 + SHADOW,
		BUTTON_WIDTH - 2, FONT_HEIGHT - 2
	);

	draw_shadow
	(	rcv_button.win,
		tp->top_gc, tp->bottom_gc,
		1 + SHADOW, win_index * FONT_HEIGHT  + 1 + SHADOW,
		BUTTON_WIDTH - 2, FONT_HEIGHT - 2
	);

}	/* end draw_mail_rcv_button */

/*
 ****************************************************************
 *	Desenha o sombreado dos botões de remoção 		*
 ****************************************************************
 */
void
draw_mail_del_win (void)
{
	draw_shadow
	(	del_button.win,
		del_button.triple.top_gc, del_button.triple.bottom_gc,
		0, 0,
		del_button.width, del_button.height
	);

}	/* end draw_mail_del_win */

/*
 ****************************************************************
 *	Desenha o botão de remoção uma linha de carta		*
 ****************************************************************
 */
void
draw_mail_del_button (const MBOX *mp, int win_index)
{
	const TRIPLEGC	*tp;

	tp = &del_color[mp->m_del];

	XFillRectangle
	(	display, del_button.win, tp->main_gc,
		1 + SHADOW, win_index * FONT_HEIGHT  + 1 + SHADOW,
		BUTTON_WIDTH - 2, FONT_HEIGHT - 2
	);

	draw_shadow
	(	del_button.win,
		tp->top_gc, tp->bottom_gc,
		1 + SHADOW, win_index * FONT_HEIGHT  + 1 + SHADOW,
		BUTTON_WIDTH - 2, FONT_HEIGHT - 2
	);

}	/* end draw_mail_del_button */

/*
 ****************************************************************
 *	Entrei na janela de botões de recepção			*
 ****************************************************************
 */
void
motion_rcv_button_window (int y)
{
	int		line;

	line = ((y - SHADOW) / FONT_HEIGHT);

	if (line >= NMAIL)
		return;

	if   (line < 0)
		line = 0;
	elif (line >= NLINE)
		line = NLINE - 1;

	rcv_highlight_line = line;

}	/* end motion_rcv_button_window */

/*
 ****************************************************************
 *	Entrei na janela de botões de remoção			*
 ****************************************************************
 */
void
motion_del_button_window (int y)
{
	int		line;

	line = ((y - SHADOW) / FONT_HEIGHT);

	if (line >= NMAIL)
		return;

	if   (line < 0)
		line = 0;
	elif (line >= NLINE)
		line = NLINE - 1;

	del_highlight_line = line;

}	/* end motion_del_button_window */

/*
 ****************************************************************
 *	Muda objetivo de recepção de uma carta			*
 ****************************************************************
 */
void
do_rcv_button_button_press (int y)
{
	MBOX		*mp;
	const TRIPLEGC	*tp;

	if (rcv_highlight_line < 0)
		return;

	mp = mail_vec_tb[FIRST_LINE + rcv_highlight_line];

	tp = &rcv_color[mp->m_rcv ^= 1];

	draw_mail_rcv_button (mp, rcv_highlight_line);

}	/* end do_rcv_button_button_press */

/*
 ****************************************************************
 *	Muda objetivo de remoção de uma carta			*
 ****************************************************************
 */
void
do_del_button_button_press (int y)
{
	MBOX		*mp;
	const TRIPLEGC	*tp;

	if (del_highlight_line < 0)
		return;

	mp = mail_vec_tb[FIRST_LINE + del_highlight_line];

	tp = &del_color[mp->m_del ^= 1];

	draw_mail_del_button (mp, del_highlight_line);

}	/* end do_del_button_button_press */

/*
 ****************************************************************
 *	Apaga as janelas dos botões das cartas			*
 ****************************************************************
 */
void
clear_mail_rcv_del_window (void)
{
	XFillRectangle
	(	display, rcv_button.win, rcv_button.triple.main_gc,
		SHADOW, SHADOW,
		rcv_button.width - 2 * SHADOW, rcv_button.height - 2 * SHADOW
	);

	XFillRectangle
	(	display, del_button.win, del_button.triple.main_gc,
		SHADOW, SHADOW,
		del_button.width - 2 * SHADOW, del_button.height - 2 * SHADOW
	);

}	/* end clear_mail_rcv_del_window */

/*
 ****************************************************************
 *	Desenha o sombreado da barra de rolagem			*
 ****************************************************************
 */
void
draw_scroll_win (void)
{
	draw_shadow
	(	scroll.win,
		scroll.triple.top_gc, scroll.triple.bottom_gc,
		0, 0,
		scroll.width, scroll.height
	);

}	/* end draw_scroll_win */

/*
 ****************************************************************
 *	Apaga a janela da barra de rolagem			*
 ****************************************************************
 */
void
clear_scroll_window (void)
{
	XFillRectangle
	(	display, scroll.win, scroll.triple.main_gc,
		SHADOW, SHADOW,
		scroll.width - 2 * SHADOW, scroll.height - 2 * SHADOW
	);

}	/* end clear_scroll_window */

/*
 ****************************************************************
 *	Atualiza a barra de rolagem				*
 ****************************************************************
 */
void
update_scroll_bar (int y, int NEW_FIRST_LINE)
{
	int		last_top    = scroll.top;
	int		last_bottom = scroll.bottom;
#if (0)	/*******************************************************/
	int		middle = (last_top + last_bottom) >> 1;
#endif	/*******************************************************/
	int		inc = 0;
	int		state = 0;
	int		size, height;
	int		diff;

	/*
	 *	Verifica se há linhas suficientes
	 */
	if (NMAIL <= NLINE)
		return;

	/*
	 *	Analisa o pedido
	 */
	if   (y < 0)
	{
		/* Continua */
	}
	elif (y == 0)
	{
		if   (NEW_FIRST_LINE < FIRST_LINE)
		{
			if (FIRST_LINE <= 0)
				return;
		}
		elif (NEW_FIRST_LINE > FIRST_LINE)
		{
			if (FIRST_LINE >= NMAIL - NLINE)
				return;
		}
	}
	elif (y < scroll.begin)		/* Sobe uma linha */
	{
		if (FIRST_LINE <= 0)
			return;

		NEW_FIRST_LINE = FIRST_LINE - 1;

		state = 'U';
	}
#if (0)	/*******************************************************/
	elif (y < middle - 20)		/* Sobe meia tela */
#endif	/*******************************************************/
	elif (y < last_top)		/* Sobe meia tela */
	{
		if (FIRST_LINE <= 0)
			return;

		if ((NEW_FIRST_LINE = FIRST_LINE - (NLINE >> 1)) < 0)
			NEW_FIRST_LINE = 0;
	}
	elif (y > scroll.end)		/* Desce uma linha */
	{
		if (FIRST_LINE >= NMAIL - NLINE)
			return;

		NEW_FIRST_LINE = FIRST_LINE + 1;

		state = 'D';
	}
#if (0)	/*******************************************************/
	elif (y > middle + 20)		/* Desce meia tela */
#endif	/*******************************************************/
	elif (y > last_bottom)		/* Desce meia tela */
	{
		if (FIRST_LINE >= NMAIL - NLINE)
			return;

		if ((NEW_FIRST_LINE = FIRST_LINE + (NLINE >> 1)) > NMAIL - NLINE)
			NEW_FIRST_LINE = NMAIL - NLINE;
	}
	else
	{
		return;
	}

	/*
	 *	Calcula o comprimento da barra de rolagem
	 */
	height = scroll.end - scroll.begin;

	size = height * NLINE / NMAIL;

	if   (size > scroll.end - scroll.begin)
		size = scroll.end - scroll.begin;
	elif (size < SB_WIDTH)
		size = SB_WIDTH;

	scroll.top    = scroll.begin + ((height - size) * NEW_FIRST_LINE) / (NMAIL - NLINE);
	scroll.bottom = scroll.top + size;

	/*
	 *	Desenha a barra de rolagem
	 */
	if ((inc = scroll.top - last_top) > 0)
	{
		XClearArea
		(	display, scroll.win,
			2 * SHADOW, last_top,
			SB_WIDTH, inc,
			False
		);
	}

	if ((inc = last_bottom - scroll.bottom) > 0)
	{
		XClearArea
		(	display, scroll.win,
			2 * SHADOW, scroll.bottom,
			SB_WIDTH, inc,
			False
		);
	}

	XFillRectangle
	(	display, scroll.win,
		scroll.triple.main_gc,
		2 * SHADOW, scroll.top,
		SB_WIDTH, (scroll.bottom - scroll.top)
	);


	draw_shadow
	(	scroll.win,
		scroll.triple.top_gc, scroll.triple.bottom_gc,
		2 * SHADOW, scroll.top,
		SB_WIDTH, (scroll.bottom - scroll.top)
	);

	/*
	 *	Redesenha os botões
	 */
	draw_scroll_button ('U', (state == 'U') ? -1 : +1);

	draw_scroll_button ('D', (state == 'D') ? -1 : +1);

	scroll.state = state;

	/*
	 *	Atualiza a janela das cartas
	 */
	diff = NEW_FIRST_LINE - FIRST_LINE;

	FIRST_LINE = NEW_FIRST_LINE;

	scroll_mail_window (diff);

}	/* end update_scroll_bar */

/*
 ****************************************************************
 *	Levanta o botão da barra de rolagem			*
 ****************************************************************
 */
void
reset_scroll_button (void)
{
	switch (scroll.state)
	{
	    case 'U':
		draw_scroll_button ('U', +1);
		break;

	    case 'D':
		draw_scroll_button ('D', +1);
		break;
	}

	scroll.state = 0;

}	/* end reset_scroll_button */

/*
 ****************************************************************
 *	Desenha o Retangulo da barra de rolagem			*
 ****************************************************************
 */
void
draw_scroll_button (int up_down, int state)
{
	int		x = 2 * SHADOW;
	int		y = (up_down == 'U') ? 2 * SHADOW : scroll.end;

	XFillRectangle
	(	display, scroll.win,
		scroll.triple.main_gc,
		x, y,
		SB_WIDTH, SB_WIDTH
	);

	if (state > 0)
	{
		draw_shadow
		(	scroll.win,
			scroll.triple.top_gc, scroll.triple.bottom_gc,
			x, y,
			SB_WIDTH, SB_WIDTH
		);
	}
	else
	{
		draw_shadow
		(	scroll.win,
			scroll.triple.bottom_gc, scroll.triple.top_gc,
			x, y,
			SB_WIDTH, SB_WIDTH
		);
	}

}	/* end draw_scroll_button */

/*
 ****************************************************************
 *	Processa o caracter teclado				*
 ****************************************************************
 */
void
do_key_press (XEvent event)
{
	int		len;
	KeySym		key;
	char		text[8];
	KEYDATA		*kp = &keydata;

	/*
	 *	Se for para editar a linha ...
	 */
	if (kp->k_code == 'M')
	{
		if (xlineedit (event, kp) != 0)
		{
			kp->k_code = 0;

			if   (kp->k_area == local_user_nm)
			{
			   	XUnmapWindow (display, lmboxnm.win);
				lmboxnm_mapped = 0;

				if (kp->k_changed)
				{
					update_remote_user_nm ();
					kp->k_changed = 0;
				}
			}
			elif (kp->k_area == remote_user_nm)
			{
			   	XUnmapWindow (display, rmboxnm.win);
				rmboxnm_mapped = 0;
			}

		   	XUnmapWindow (display, option.win);
			option_highlight_line = -1;
		}

		return;
	}

	/*
	 *	Demais funções
	 */
	len = XLookupString (&event.xkey, text, sizeof (text), &key, 0);

	if (kp->k_code)			/* == 'P' */
	{
		if   (key == XK_Return || key == XK_KP_Enter || key == XK_Linefeed)
		{
			kp->k_area[kp->k_count] = '\0';

			XDrawImageString
			(	display, kp->k_win,
				kp->k_gc,
				kp->k_x + kp->k_count * FONT_WIDTH,
				kp->k_y,
				" ", 1
			);

			kp->k_code = 0;

#if (0)	/*******************************************************/
			send_passwd ();
#endif	/*******************************************************/
			(*kp->k_function) ();

			return;
		}
		elif (key == XK_Delete || key == XK_BackSpace)
		{
			if (kp->k_count <= 0)
				return;

			XDrawImageString
			(	display, kp->k_win,
				kp->k_gc,
				kp->k_x + kp->k_count * FONT_WIDTH,
				kp->k_y,
				" ", 1
			);

			kp->k_count--;

			XDrawImageString
			(	display, kp->k_win,
				kp->k_rev_gc,
				kp->k_x + kp->k_count * FONT_WIDTH,
				kp->k_y,
				" ", 1
			);

			return;
		}

		if (len == 0)
			return;

		if (kp->k_count < kp->k_size - 1)
		{
			kp->k_area[kp->k_count] = text[0];

			XDrawImageString
			(	display, kp->k_win,
				kp->k_gc,
				kp->k_x + kp->k_count * FONT_WIDTH,
				kp->k_y,
				"*", 1
			);

			kp->k_count++;

			XDrawImageString
			(	display, kp->k_win,
				kp->k_rev_gc,
				kp->k_x + kp->k_count * FONT_WIDTH,
				kp->k_y,
				" ", 1
			);
		}
	}
	else	/* Barra de Rolagem */
	{
		if   (key == XK_Home)
		{
			update_scroll_bar (0, 0);
		}
		elif (key == XK_End)
		{
			int		first;

			if ((first = NMAIL - NLINE) < 0)
				first = 0;

			update_scroll_bar (0, first);
		}
		elif (key == XK_Page_Up)
		{
			update_scroll_bar (scroll.begin + 1, 0);
		}
		elif (key == XK_Page_Down)
		{
			update_scroll_bar (scroll.end - 1, 0);
		}
		elif (key == XK_Left || key == XK_Up)
		{
			update_scroll_bar (scroll.begin - 1, 0);

			draw_scroll_button ('U', 1);
		}
		elif (key == XK_Right || key == XK_Down)
		{
			update_scroll_bar (scroll.end + 1, 0);

			draw_scroll_button ('D', +1);
		}

		scroll.state = 0;
	}

}	/* end do_key_press */

/*
 ****************************************************************
 *	Edita uma linha no estilo do "vi"			*
 ****************************************************************
 */
int
xlineedit (XEvent event, KEYDATA *kp)
{
	int		len;
	KeySym		key;
	char		text[8];

	/*
	 *	Obtém o caractere
	 */
	len = XLookupString (&event.xkey, text, sizeof (text), &key, 0);

	/*
	 *	Os diversos caracteres de controle
	 */
	if   (key == XK_Return || key == XK_KP_Enter || key == XK_Linefeed)
	{
		return (1);
	}
	elif (key == XK_Delete)
	{
		if ((len = kp->k_count - kp->k_offset) <= 0)
			return (0);

		memmove (&kp->k_area[kp->k_offset], &kp->k_area[kp->k_offset + 1], len);

		kp->k_count--;

		XDrawImageString
		(	kp->k_display, kp->k_win,
			kp->k_gc,
			kp->k_x + kp->k_offset * FONT_WIDTH,
			kp->k_y,
			&kp->k_area[kp->k_offset], len
		);

		xlineedit_cursor_on (kp);

		kp->k_changed = 1;

		return (0);
	}
	elif (key == XK_BackSpace)
	{
		if (kp->k_offset <= 0)
			return (0);

		len = kp->k_count - kp->k_offset + 1;

		kp->k_offset--; kp->k_count--;

		memmove (&kp->k_area[kp->k_offset], &kp->k_area[kp->k_offset + 1], len);

		XDrawImageString
		(	kp->k_display, kp->k_win,
			kp->k_gc,
			kp->k_x + kp->k_offset * FONT_WIDTH,
			kp->k_y,
			&kp->k_area[kp->k_offset], len + 1
		);

		xlineedit_cursor_on (kp);

		kp->k_changed = 1;

		return (0);
	}
	elif (key == XK_Left)
	{
		if (kp->k_offset <= 0)
			return (0);

		xlineedit_cursor_off (kp);

		kp->k_offset--;

		xlineedit_cursor_on (kp);

		return (0);
	}
	elif (key == XK_Right)
	{
		if (kp->k_offset >= kp->k_count)
			return (0);

		xlineedit_cursor_off (kp);

		kp->k_offset++;

		xlineedit_cursor_on (kp);

		return (0);
	}
	elif (key == XK_Home)
	{
		if (kp->k_offset <= 0)
			return (0);

		xlineedit_cursor_off (kp);

		kp->k_offset = 0;

		xlineedit_cursor_on (kp);

		return (0);
	}
	elif (key == XK_End)
	{
		if (kp->k_offset >= kp->k_count)
			return (0);

		xlineedit_cursor_off (kp);

		kp->k_offset = kp->k_count;

		xlineedit_cursor_on (kp);

		return (0);
	}

	/*
	 *	Um texto; verifica se há espaço
	 */
	if (len == 0 || kp->k_count >= kp->k_size - 1)
		return (0);

	/*
	 *	Insere o texto
	 */
	len = kp->k_count - kp->k_offset + 1;

	memmove (&kp->k_area[kp->k_offset + 1], &kp->k_area[kp->k_offset], len);

	kp->k_area[kp->k_offset] = text[0];

	XDrawImageString
	(	kp->k_display, kp->k_win,
		kp->k_gc,
		kp->k_x + kp->k_offset * FONT_WIDTH,
		kp->k_y,
		&kp->k_area[kp->k_offset], len + 1
	);

	kp->k_count++; kp->k_offset++;

	xlineedit_cursor_on (kp);

	kp->k_changed = 1;

	return (0);

}	/* end xlineedit */

/*
 ****************************************************************
 *	Coloca o cursor						*
 ****************************************************************
 */
void
xlineedit_cursor_on (const KEYDATA *kp)
{
	char		c;

	if ((c = kp->k_area[kp->k_offset]) == '\0')
		c = ' ';

	XDrawImageString
	(	kp->k_display, kp->k_win,
		kp->k_rev_gc,
		kp->k_x + kp->k_offset * FONT_WIDTH,
		kp->k_y,
		&c, 1
	);

}	/* end xlineedit_cursor_on */

/*
 ****************************************************************
 *	Tira o cursor						*
 ****************************************************************
 */
void
xlineedit_cursor_off (const KEYDATA *kp)
{
	char		c;

	if ((c = kp->k_area[kp->k_offset]) == '\0')
		c = ' ';

	XDrawImageString
	(	kp->k_display, kp->k_win,
		kp->k_gc,
		kp->k_x + kp->k_offset * FONT_WIDTH,
		kp->k_y,
		&c, 1
	);

}	/* end xlineedit_cursor_off */

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
	MENUTB			*mp;
	int			base_width;
	int			ncol, nline;
	XWindowChanges		value;

	/*
	 ****** Verifica se é a primeira vez, ... ***************
	 */
	if (width == 0)		/* Primeira vez */
	{
		/*
		 *	Calcula a largura total
		 */
		base_width = 2 * BUTTON_WIDTH + SB_WIDTH + 3 * BORDER + 10 * SHADOW;
		width	   = base_width + NCOL * FONT_WIDTH;

		/*
		 *	Calcula os vários tamanhos
		 */
		for (mp = &menutb[0]; mp < &menutb[NMENU]; mp++)
		{
			mp->m_win.width	 = strlen (mp->m_text) * FONT_WIDTH + 2 * MENU_BORDER;
			mp->m_win.height = FONT_HEIGHT + 2 * MENU_BORDER;
			mp->m_win.x	 = (mp == &menutb[0]) ? 0 :
							mp[-1].m_win.x + mp[-1].m_win.width + BORDER;
			mp->m_win.y	 = 0;
		}

		option.width	= strlen (option_longest_text) * FONT_WIDTH + 2 * SHADOW + 2 * MENU_BORDER;
		option.height	= option_text_n * (FONT_HEIGHT + 2 * SHADOW) + 2 * MENU_BORDER;
		option.x	= 0;
		option.y	= FONT_HEIGHT + 2 * MENU_BORDER + 2 * BORDER;

		sizem.width	= strlen (sizem_longest_text) * FONT_WIDTH + 2 * SHADOW + 2 * MENU_BORDER +
				  BUTTON_WIDTH;
		sizem.height	= sizem_text_n * (FONT_HEIGHT + 2 * SHADOW) + 2 * MENU_BORDER;
		sizem.x		= option.width + 2 * BORDER;
		sizem.y		= FONT_HEIGHT + 2 * MENU_BORDER + 2 * BORDER +
				  sizem_line_offset * (FONT_HEIGHT + 2 * SHADOW) + MENU_BORDER;

		lmboxnm.width	= sizeof (local_user_nm) * FONT_WIDTH + 2 * SHADOW + 2 * MENU_BORDER;
		lmboxnm.height	= (FONT_HEIGHT + 2 * SHADOW) + 2 * MENU_BORDER;
		lmboxnm.x	= option.width + 2 * BORDER;
		lmboxnm.y	= FONT_HEIGHT + 2 * MENU_BORDER + 2 * BORDER +
				  lmboxnm_line_offset * (FONT_HEIGHT + 2 * SHADOW) + MENU_BORDER;

		rmboxnm.width	= sizeof (remote_mail_box_nm) * FONT_WIDTH + 2 * SHADOW + 2 * MENU_BORDER;
		rmboxnm.height	= (FONT_HEIGHT + 2 * SHADOW) + 2 * MENU_BORDER;
		rmboxnm.x	= option.width + 2 * BORDER;
		rmboxnm.y	= FONT_HEIGHT + 2 * MENU_BORDER + 2 * BORDER +
				  rmboxnm_line_offset * (FONT_HEIGHT + 2 * SHADOW) + MENU_BORDER;

		version.width	= strlen (version_longest_text) * FONT_WIDTH + 2 * SHADOW +
				  2 * MENU_BORDER + BUTTON_WIDTH;
		version.height	= version_text_n * (FONT_HEIGHT + 2 * SHADOW) + 2 * MENU_BORDER;
		version.x	= option.width + 2 * BORDER;
		version.y	= FONT_HEIGHT + 2 * MENU_BORDER + 2 * BORDER +
				  version_line_offset * (FONT_HEIGHT + 2 * SHADOW) + MENU_BORDER;

		msg.width	= width;
		msg.height	= MSGLINEs * FONT_HEIGHT + 2 * MENU_BORDER;
		msg.x		= 0;
		msg.y		= FONT_HEIGHT + 2 * MENU_BORDER + BORDER;

		percent.width	= width;
		percent.height	= PERCENT_HEIGHT + 2 * PERCENT_BORDER;
		percent.x	= 0;
		percent.y	= msg.y + msg.height + BORDER;

		rcv_button.width= BUTTON_WIDTH + 2 * SHADOW;
		rcv_button.height= NLINE * FONT_HEIGHT + 2 * SHADOW;
		rcv_button.x	= 0;
		rcv_button.y	= percent.y + percent.height + BORDER;

		del_button.width= BUTTON_WIDTH + 2 * SHADOW;
		del_button.height= NLINE * FONT_HEIGHT + 2 * SHADOW;
		del_button.x	= rcv_button.width + BORDER;
		del_button.y	= percent.y + percent.height + BORDER;

		mail.width	= NCOL * FONT_WIDTH + 2 * SHADOW;
		mail.height	= NLINE * FONT_HEIGHT + 2 * SHADOW;
		mail.x		= del_button.x + del_button.width + BORDER;
		mail.y		= percent.y + percent.height + BORDER;

		scroll.width	= SB_WIDTH + 4 * SHADOW;
		scroll.height	= NLINE * FONT_HEIGHT + 2 * SHADOW;
		scroll.x	= mail.x + mail.width + BORDER;
		scroll.y	= percent.y + percent.height + BORDER;

		scroll.state	= 0;
		scroll.begin	= 2 * SHADOW + SB_WIDTH;
		scroll.end	= scroll.height - (SB_WIDTH + 2 * SHADOW);

	   /***	scroll.bottom	= 0;	***/
		scroll.top	= 100000;

		/*
		 *	Informa o WM
		 */
		szhint.width_inc   = FONT_WIDTH;
		szhint.height_inc  = FONT_HEIGHT;

		szhint.base_width  = base_width + 2 * BORDER;
		szhint.base_height = percent.y + percent.height + 3 * BORDER + 2 * SHADOW;

		szhint.min_width   = szhint.base_width  + MIN_NCOL  * FONT_WIDTH;
		szhint.min_height  = szhint.base_height + MIN_NLINE * FONT_HEIGHT;

		szhint.flags = PBaseSize|PMinSize|PResizeInc|PWinGravity;

		return;
	}

	/*
	 ****** NÃO é a primeira vez ****************************
	 */
	if ((ncol  = (width -  szhint.base_width)  / FONT_WIDTH) < MIN_NCOL)
		ncol = MIN_NCOL;

	if ((nline = (height - szhint.base_height) / FONT_HEIGHT) < MIN_NLINE)
		nline = MIN_NLINE;

	if (ncol == NCOL && nline == NLINE)
		return;

	NCOL = ncol; NLINE = nline;

#if (0)	/*******************************************************/
printf ("NCOL = %d, NLINE = %d\n", NCOL, NLINE);
#endif	/*******************************************************/

	width -= 2 * BORDER; height -= 2 * BORDER;

	/*
	 *	Recalcula os vários tamanhos
	 */
	msg.width	= width;
   /***	msg.height	= MSGLINEs * FONT_HEIGHT + 2 * MENU_BORDER;		***/
   /***	msg.x		= 0;							***/
   /***	msg.y		= FONT_HEIGHT + 2 * MENU_BORDER + BORDER;		***/

	percent.width	= width;
   /***	percent.height	= PERCENT_HEIGHT + 2 * PERCENT_BORDER;			***/
   /***	percent.x	= 0;							***/
   /***	percent.y	= msg.y + msg.height + BORDER;				***/

   /***	rcv_button.width= BUTTON_WIDTH + 2 * SHADOW;				***/
	rcv_button.height= NLINE * FONT_HEIGHT + 2 * SHADOW;
   /***	rcv_button.x	= 0;							***/
   /***	rcv_button.y	= percent.y + percent.height + BORDER;			***/

   /***	del_button.width= BUTTON_WIDTH + 2 * SHADOW;				***/
	del_button.height= NLINE * FONT_HEIGHT + 2 * SHADOW;
   /***	del_button.x	= rcv_button.width + BORDER;				***/
   /***	del_button.y	= percent.y + percent.height + BORDER;			***/

	mail.width	= NCOL * FONT_WIDTH + 2 * SHADOW;
	mail.height	= NLINE * FONT_HEIGHT + 2 * SHADOW;
   /***	mail.x		= del_button.x + del_button.width + BORDER;		***/
   /***	mail.y		= percent.y + percent.height + BORDER;			***/

   /***	scroll.width	= SB_WIDTH + 2 * SHADOW;				***/
	scroll.height	= NLINE * FONT_HEIGHT + 2 * SHADOW;
	scroll.x	= mail.x + mail.width + BORDER;
   /***	scroll.y	= percent.y + percent.height + BORDER;			***/

#if (0)	/*******************************************************/
	scroll.state	= 0;
	scroll.begin	= BORDER + SB_WIDTH + 1;
	scroll.end	= scroll.height - (SB_WIDTH + 1);
#endif	/*******************************************************/

	/*
	 *	Atualiza os tamanhos das outras janelas
	 */
	value.width  = msg.width;
	XConfigureWindow (display, msg.win, CWWidth, &value);

	value.width  = percent.width;
	XConfigureWindow (display, percent.win, CWWidth, &value);

	value.height = rcv_button.height;
	XConfigureWindow (display, rcv_button.win, CWHeight, &value);

	value.height = del_button.height;
	XConfigureWindow (display, del_button.win, CWHeight, &value);

	value.width  = mail.width;
	value.height = mail.height;
	XConfigureWindow (display, mail.win, CWWidth|CWHeight, &value);

	value.height = scroll.height;
	value.x	     = scroll.x;
	XConfigureWindow (display, scroll.win, CWHeight|CWX, &value);

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

	if (user_geo_mask & WidthValue)
		hp->flags |= USSize;
#if (0)	/*******************************************************/
		{ hp->width = width + hp->base_width; hp->flags |= USSize; }
#endif	/*******************************************************/

	if (user_geo_mask & HeightValue)
		hp->flags |= USSize;
#if (0)	/*******************************************************/
		{ hp->height = height + hp->base_height; hp->flags |= USSize; }
#endif	/*******************************************************/

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
	if (connection_state != DISCONNECTED)
		disconnection_center ();

	if (local_status_fp != NOFILE)
		fclose (local_status_fp);

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
		"%s - acessa uma caixa postal remota através do protocolo POP3\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-d <display>] [-g <geometria>] "
		"[-f <frente>] [-b <fundo>] [-F <f>]\n",
		pgname, pgversion, pgname
	);
	fprintf
	(	stderr,
		"\nOpções:"
		"\t-d: Conecta-se ao servidor X <display>\n"
		"\t-g: Define a <geometria>\n"
		"\t-f: Define a cor de <frente>\n"
		"\t-b: Define a cor de <fundo>\n"
		"\t-F: Use a fonte <f>\n"
	);

	exit (2);

}	/* end help */
