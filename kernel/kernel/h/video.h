/*
 ****************************************************************
 *								*
 *			<sys/video.h>				*
 *								*
 *	Definições relativas ao video do PC			*
 *								*
 *	Versão	3.0.0, de 07.01.95				*
 *		4.9.0, de 15.04.06				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *		/usr/include/sys				*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2006 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#define	VIDEO_H			/* Para definir os protótipos */

/*
 ****** Estrutura do VGA/SVGA ***********************************
 */
enum {	NCON_MAX	= 8 };			/* No. Máximo de consoles virtuais */

enum {	MAX_COL		= 1024 / ( 8 + 1) };	/* 113x48 => Conseqüência de 1024x768 com fonte 16x8 */
enum {	MAX_LINE	= 768  / (16 + 0) };

/* Se a estrutura seguinte for modificada, lembre-se dos diversos "memsetl" */

typedef struct	video_char
{
	char		video_char;		/* Um caractere da linha */
	char		video_ch_color;		/* A cor do caractere da linha */
	char		video_bg_color;		/* A cor de fundo do caractere */
	char		video_reser;

}	VIDEOCHAR;

typedef struct	video_line
{
	int		video_len;		/* No. de caracteres na linha */
	VIDEOCHAR	video_line[MAX_COL + 14]; /* Os caracteres da linha (no mínimo 113) */

}	VIDEOLINE;

typedef struct	video_display
{
	int		video_line_offset;	/* Deslocamento da cópia */
	int		video_copy_cursor_line;	/* Posição do cursor na cópia */

	int		video_cursor_line;	/* Posição do cursor */
	int		video_cursor_col;

	int		video_normal_ch_color;	/* Cor do caractere normal */
	int		video_normal_bg_color;	/* Cor do fundo normal */

	int		video_ch_color;		/* Cor do caractere */
	int		video_bg_color;		/* Cor do fundo */

	/* Parte usada pelo protocolo VT100 */

	int		vt100_save_line;	/* Guarda a posição do cursor */
	int		vt100_save_col;	

	int		vt100_state;		/* Estado do protocolo "vt100" */

	int		vt100_arg_1;		/* O primeiro argumento */
	int		vt100_arg_2;		/* O primeiro argumento */

	int		vt100_scroll_begin;	/* Início do rolamento */
	int		vt100_scroll_end;	/* Final do rolamento */

	VIDEOLINE	video_line[MAX_LINE];	/* A Matriz dos caracteres */

}	VIDEODISPLAY;

typedef struct
{
	int		video_mode;		/* 0: VGA (80x25), 1: SVGA (113x48) */

	ulong		video_phys_area;	/* Ponteiro da área (endereço físico) */
	char		*video_linear_area;	/* Ponteiro da área (endereço virtual) */

	int		video_lines;		/* No. de linhas (pixel) */
	int		video_cols;		/* No. de colunas (pixel) */

	int		video_font_lines;	/* No. de linhas da fonte */
	int		video_font_cols;	/* No. de colunas da fonte */

	int		video_line_pixels;	/* No. de pixels de uma linha de caracteres */

	int		video_LINE;		/* No. de linhas (texto) */
	int		video_COL;		/* No. de colunas (texto) */

	/* Os conteúdos das várias telas */

	VIDEODISPLAY	*video_display[NCON_MAX]; /* Ponteiros para as telas */

}	VIDEODATA;

/*
 ****** Endereço do video VGA ***********************************
 */
#define VGA_ADDR	((ushort *)(SYS_ADDR + (736 * KBSZ)))

/*
 ******	Definições do protocolo "vt100" *************************
 */
typedef enum
{
	VT100_NORMAL = 0,	/* Estado normal */
	VT100_ESC,		/* Foi visto um "\e" (escape) */
	VT100_LB,		/* Foi visto um "[" */
	VT100_SC,		/* Foi visto o "[;" */
	VT100_QUESTION		/* Foi visto o "[?" */

}	VT100_STATE_ENUM;

/*
 ****** Tabela das cores ****************************************
 *
 *	0 == Black		Preto
 *	1 == Blue		Azul escuro
 *	2 == Green		Verde
 *	3 == Cyan		Azul claro
 *	4 == Red		Vermelho
 *	5 == Magenta		Violeta
 *	6 == Brown		Laranja
 *	7 == White		Branco
 *	8 == Gray
 *	9 == Bright blue
 *	A == Bright green
 *	B == Bright cyan
 *	C == Bright red
 *	D == Bright magenta
 *	E == Yellow
 *	F == Bright white
 */

enum {	BLACK_COLOR = 0, WHITE_COLOR = 7, DONT_CARE_COLOR = 0 };
