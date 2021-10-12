/*
 ****************************************************************
 *								*
 *			xview.h					*
 *								*
 *	Gerenciador de arquivos					*
 *								*
 *	Versão	4.5.0, de 24.06.03				*
 *		4.5.0, de 27.01.04				*
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

/*
 ******	Definições globais **************************************
 */
#define	NOVOID		(void *)NULL
#define	elif		else if
#define	EVER		;;
#define STR(x)		# x
#define XSTR(x)		STR (x)
#define	MIN(a,b)	((a) < (b) ? (a) : (b))
#define	MAX(a,b)	((a) > (b) ? (a) : (b))

/*
 ****** Tamanhos das margens, sombras, ... **********************
 */
#define	BORDER		1
#define	SHADOW		3
#define BUTTON_WIDTH 	(2 * wp->font_width)
#define	MENU_BORDER	10

#define	SCROLL_WIDTH		15
#define	TOTAL_SCROLL_WIDTH	(SCROLL_WIDTH + 4 * SHADOW)

/*
 ******	Coordenadas da imagem ***********************************
 */
enum { N_IMAGE_PARAM = 16 };

typedef struct
{
	double	i_CENTER_X;
	double	i_CENTER_Y;
	double	i_SIZE_X;

}	IMAGE_PARAM;

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
	GC		text_gc;
	int		font_width, font_height, font_ascent, font_descent;

}	WINDATA;

#define	NOWINDATA 	(WINDATA *)NULL

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
 ******	Estrutura de um menu de OPÇÕES **************************
 */
#define NOFUNC	(void (*) (int))NULL

typedef struct
{
	const char	*m_text;		/* Texto a escrever */
	void		(*f_func) (int);	/* Função a chamar */
	int		f_value;		/* Valor */
	SUBMENU		*m_submenu;		/* A Tabela do SUBMENU */

}	OPTIONVEC;

/*
 ******	Estrura de uma janela (barra de rolagem) ****************
 */
typedef struct
{
	Window		win;
	int		width, height;
	int		x, y;
	TRIPLEGC	back_triple;	/* As tres cores do fundo */
	TRIPLEGC	fore_triple;	/* As tres cores dos botões */
	int		button_no;	/* No. do botão apertado */
	int		begin, end;	/* Começo e Final da janela */
	int		top, bottom;	/* Limites da barra (top: valor menor) */
	int		button_offset;	/* Último valor em cima da barra */

}	SCROLLDATA;

/*
 ****** Diversos ENUMs ******************************************
 */
enum { M640x480, M800x600, M1024x768, M1280x1024, M1600x1200, M3200x2048, MOTHER };

enum { RGB, GBR, BRG, BGR, GRB, RBG };

enum { ADJUST_PRESENT, ADJUST_MAX };

enum { LOAD_IMAGE, STORE_IMAGE };

/*
 ****** Variáveis globais ***************************************
 */
extern int		vflag;		/* Verbose */
extern int		Rflag;		/* Fundo */

extern int		depth;

extern XStandardColormap *StandardColormap;

extern GC		mode_gc_vector[16];

#define	red_gc		mode_gc_vector[4]

extern int		WINDOW_WIDTH, WINDOW_HEIGHT;

extern int		MANDEL_WIDTH, MANDEL_HEIGHT;

extern int		present_root_width, present_root_height;

extern Display		*display;

extern Window		root_win;

extern WINDATA		mandel, msg;

extern WINDATA		file_menu;

extern SUBMENU		load_win;

extern WINDATA		dir_win;

extern SCROLLDATA	dir_scroll;

extern WINDATA		dir_nm_win, file_nm_win, ok_win, new_dir_win;

extern TRIPLEGC		red_dir_triple;

extern int		msg_len;

extern ulong		gif_col_vector[256];

extern int		*count_matrix;

extern int		com_len;

extern int		color_index_shift;

extern int		color_brightness;

extern int		color_order;

extern IMAGE_PARAM	*image_param_ptr;

extern int		adjust_mode;

extern XImage		*ximage;

extern const char	**given_argv, **first_file_nm_argv, **file_nm_argv;

/*
 ******	Protótipos de funções ***********************************
 */
extern void		set_window_sizes (int width, int height);
extern void		draw_msg_win (void);
extern void		draw_com_win (void);
extern void		draw_shadow (Window, GC, GC, int, int, int width, int height);
extern void		draw_border (Window win, int win_x, int win_y, int width, int height);
extern void		draw_button (const WINDATA *, const TRIPLEGC *, int, int, int, int);
extern void		draw_msg_text (const char *format, ...);
extern void		draw_com_text (const char *format, ...);
extern void		draw_error_msg_text (const char *format, ...);
extern void		draw_error_msg_win (void);
extern void		compute (int);
extern void		do_mandel_button_press (int button, int x, int y);
extern void		do_mandel_button_release (int button, int x, int y);
extern void		draw_color_map (int);
extern void		draw_mandel_win (XExposeEvent *xexpose);
extern void		reduce (void);
extern void		generate_gif_file (int);
extern void		create_color_vector (int changed);
extern void		motion_mandel_window (int x, int y);
extern void		draw_count_mandel_window (void);
extern void		return_last_image (int value);
extern void		leave_mandel_window (void);
extern void		quit_function (int value);
extern void		map_load_window (int);
extern void		set_load_win_size (void);
extern void		draw_load_win (void);
extern void		draw_dir_win (void);
extern void		draw_scroll_win (const SCROLLDATA *sp);
extern void		update_dir_scroll_bar (int motion, int y);
extern void		button_release_scroll_button (SCROLLDATA *sp);
extern void		button_1_press_dir_window (int y);
extern void		draw_ok_win (void);
extern void		draw_new_dir_win (void);
extern void		Shadow_up (const WINDATA *wp);
extern void		Shadow_even (const WINDATA *wp);
extern void		Shadow_down (const WINDATA *wp);
extern void		Shadow_up_with_border (const WINDATA *wp);
extern void		Shadow_even_with_border (const WINDATA *wp);
extern void		Shadow_down_with_border (const WINDATA *wp);
extern void		button_1_press_ok_window (void);
extern int		good_file_nm (const char *file_nm);
extern void		adjust_win_size (int width, int height);
extern void		mem_function (int);
extern void		clear_msg_window (void);
extern void		draw_image (int value);
extern int		is_a_GIF_file (FILE *fp);
extern int		is_a_JPEG_file (FILE *fp);
extern XImage		*alloc_image (int width, int height);
extern int		load_GIF_file (const char *file_nm, FILE *fp);
extern int		load_JPEG_file (const char *file_nm, FILE *fp, const XStandardColormap *stdcmap);
extern void		draw_background (int);
