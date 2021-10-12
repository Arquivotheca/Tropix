/*
 ****************************************************************
 *								*
 *			xmandel.h				*
 *								*
 *	Gerenciador de arquivos					*
 *								*
 *	Versão	4.5.0, de 24.06.03				*
 *		4.5.0, de 17.01.04				*
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
	int		font_width, font_height, font_descent;

}	WINDATA;

#define	NOWINDATA 	(WINDATA *)NULL

/*
 ****** Diversos ENUMs ******************************************
 */
enum { M640x480, M800x600, M1024x768, M1280x1024, M1600x1200, M3200x2048, MOTHER };

enum { RGB, GBR, BRG, BGR, GRB, RBG };

enum { LZW_PORTABLE, LZW_COMPACT };

/*
 ****** Variáveis globais ***************************************
 */
extern int		depth;

extern XStandardColormap *StandardColormap;

extern int		WINDOW_WIDTH, WINDOW_HEIGHT;

extern int		MANDEL_WIDTH, MANDEL_HEIGHT;

extern int		present_root_width, present_root_height;

extern Display		*display;

extern Window		root_win;

extern WINDATA		mandel, msg;

extern TRIPLEGC		red_dir_triple;

extern int		msg_len;

extern ulong		gif_col_vector[256];

extern int		*count_matrix;

extern char		*image;

extern int		color_index_shift;

extern int		color_brightness;

extern int		color_order;

extern IMAGE_PARAM	*image_param_ptr;

extern int		lzw_mode;

/*
 ******	Protótipos de funções ***********************************
 */
extern void		set_window_sizes (int width, int height);
extern void		draw_msg_win (void);
extern void		draw_shadow (Window, GC, GC, int, int, int width, int height);
extern void		draw_button (const WINDATA *, const TRIPLEGC *, int, int, int, int);
extern void		draw_msg_text (const char *format, ...);
extern void		draw_error_msg_text (const char *format, ...);
extern void		draw_error_msg_win (void);
extern void		compute (int);
extern void		do_mandel_button_press (int button, int x, int y);
extern void		do_mandel_button_release (int button, int x, int y);
extern void		draw_color_map (int);
extern void		draw_mandel_win (void);
extern void		reduce (void);
extern void		generate_gif_file (int);
extern void		create_color_vector (int changed);
extern void		motion_mandel_window (int x, int y);
extern void		draw_count_mandel_window (void);
extern void		return_last_image (int value);
extern void		leave_mandel_window (void);
