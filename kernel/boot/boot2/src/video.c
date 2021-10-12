/*
 ****************************************************************
 *								*
 *			video.c					*
 *								*
 *	Pequeno "driver" para o video VGA/SVGA do PC		*
 *								*
 *	Versão	4.9.0, de 06.04.06				*
 *		4.9.0, de 03.08.06				*
 *								*
 *	Módulo: Boot2						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2006 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include <common.h>
#include <bcb.h>
#include <video.h>

#include "../h/common.h"
#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Definições globais					*
 ****************************************************************
 */
#define BOOT_SVGA_ADDR	(2040 * MBSZ)	/* Deve ser igual à "h/mmu.h"	*/

#define	BOOT_VGA_ADDR	((ushort *)(736 * KBSZ))

/*
 ****** Parâmetros do video *************************************
 */
#define TAB		8	/* COL deve ser múltiplo de TAB */

entry int       video_line = 0;

/*
 ******	Estrutura de retorno do INT 10, 4F01 ********************
 */
extern struct supervga
{
/* 0 */	ushort	svga_mode_attr;
	char	svga_win_attr_A;
	char	svga_win_attr_B;
	ushort	svga_gran;
	ushort	svga_size;
	ushort	svga_seg_A;
	ushort	svga_seg_B;
	ulong	svga_func;
	ushort	svga_scan_line;

/* 18 */ushort	svga_width;
	ushort	svga_height;
	char	svga_cell_width;
	char	svga_cell_height;
	char	svga_mem_planes;
	char	svga_bits_per_pixel;
	char	svga_banks;
	char	svga_model_type;
	char	svga_bank_size;
	char	svga_image_pages;
	char	svga_reser;

/* 31 */char	svga_red_mask_sz;
	char	svga_red_field_pos;
	char	svga_green_mask_sz;
	char	svga_green_field_sz;
	char	svga_blue_mask_sz;
	char	svga_blue_field_sz;
	char	svga_reser_mask_sz;
	char	svga_reser_mask_pos;
	char	svga_direct_color_mode;

/* 40 */ulong	svga_linear_mem_addr;
	ulong	svga_off_screen_addr;
	ushort	svga_off_screen_sz;

}	supervga_info;

/*
 ****** Estrutura do VGA/SVGA ***********************************
 */
entry VIDEODATA		video_data;

entry VIDEODISPLAY	video_display;

/*
 ****** Variáveis ***********************************************
 */
enum  {	DMESG_SZ = 4 * KBSZ };

entry char	dmesg_area[DMESG_SZ];	/* Área de DMESG */

/*
 ******	Protótipos de funções ***********************************
 */
void		put_video_char (int c, int line, int col, int char_color, int back_color);
void		video_up_scroll (int scroll_begin, int scroll_end);
void		video_cursor_off (void);
void		video_cursor_on (void);

/*
 ****************************************************************
 *	Inicializa toda a tela com a cor de fundo		*
 ****************************************************************
 */
void
init_video (void)
{
	VIDEODATA	*vp = &video_data;
	VIDEODISPLAY	*vdp;
	VIDEOLINE	*vlp;
	const VIDEOLINE	*end_vlp;

	/*
	 *	Verifica qual foi o modo de vídeo pedido/obtido
	 */
	if (video_mode)			/* Modo SVGA (113x48) */
	{
		vp->video_mode		= 1;	/* SVGA */

		/* A própria BIOS limpa a tela */

		vp->video_phys_area	= supervga_info.svga_linear_mem_addr;
		vp->video_linear_area	= (char *)(BOOT_SVGA_ADDR | (vp->video_phys_area & (PGSZ-1)));

		vp->video_lines		= 768;		/* Definição do serviço 0x4105 da INT 0x10 */
		vp->video_cols		= 1024;

		vp->video_font_lines	= 16;		/* Definição a partir da fonte em uso */
		vp->video_font_cols	= 8;

		vp->video_line_pixels	= (vp->video_font_lines + 0) * vp->video_cols;

		vp->video_LINE		= vp->video_lines / (vp->video_font_lines + 0);
		vp->video_COL		= vp->video_cols  / (vp->video_font_cols  + 1);
	}
	else					/* Modo VGA (80x25) */
	{
		vp->video_mode		= 0;	/* VGA */

		memsetw (BOOT_VGA_ADDR, (BLACK_COLOR << 12) | (WHITE_COLOR << 8) | ' ', 25 * 80);

	   /***	vp->video_phys_area	= ...; ***/	/* Não usado neste caso */
	   /***	vp->video_linear_area	= ...; ***/	/* Não usado neste caso */

	   /***	vp->video_lines		= ...; ***/	/* Não usado neste caso */
	   /***	vp->video_cols		= ...;

	   /***	vp->video_font_lines	= ...; ***/	/* Não usado neste caso */
	   /***	vp->video_font_cols	= ...; ***/

	   /***	vp->video_line_pixels	= ...; ***/	/* Não usado neste caso */

		vp->video_LINE		= 25 - 1;		/* Última linha não usada */
		vp->video_COL		= 80;
	}

	/*
	 *	Preenche o video_diplay[0]
	 */
	vp->video_display[0]		= vdp = &video_display;

   /***	vdp->video_line_offset		= 0; ***/
   /***	vdp->video_copy_cursor_line	= 0; ***/

	vdp->video_cursor_line		= vp->video_LINE - 1;
   /***	vdp->video_cursor_col		= 0; ***/

	vdp->video_normal_ch_color	= WHITE_COLOR;	/* Branco normal */
	vdp->video_normal_bg_color	= BLACK_COLOR;	/* Preto */

	vdp->video_ch_color		= WHITE_COLOR;	/* Branco normal */
	vdp->video_bg_color		= BLACK_COLOR;	/* Preto */

	/* Parte usada pelo protocolo VT100 */

   /***	vdp->vt100_save_line		= ...; ***/
   /***	vdp->vt100_save_col		= ...; ***/

   /***	vdp->vt100_state		= ...; ***/

   /***	vdp->vt100_arg_1		= ...; ***/
   /***	vdp->vt100_arg_2		= ...; ***/

   /***	vdp->vt100_scroll_begin		= 0; ***/
	vdp->vt100_scroll_end		= vp->video_LINE - 1;

	/*
	 *	Preenche a cópia com espaços da cor branca
	 */
	for (vlp = &vdp->video_line[0], end_vlp = vlp + vp->video_LINE; vlp < end_vlp; vlp++)
		memsetl (vlp->video_line, (BLACK_COLOR << 16) | (WHITE_COLOR << 8) | ' ', vp->video_COL);

	/*
	 *	Prepara a área de DMESG
	 */
	bcb.y_dmesg_area =
	bcb.y_dmesg_ptr  = dmesg_area;
	bcb.y_dmesg_end  = dmesg_area + DMESG_SZ;

	bcb.y_videodata  = vp;

}	/* end init_video */

/*
 ****************************************************************
 *	Escreve um caractere no video, na posição corrente	*
 ****************************************************************
 */
void
putchar (int c)
{
	const VIDEODATA	*vp = &video_data;
	VIDEODISPLAY	*vdp = vp->video_display[0];
	VIDEOLINE	*vlp;
	VIDEOCHAR	*vcp;
	int		line;
	static int	video_state = VT100_NORMAL;
	static int	video_arg = 0;

	/*
	 *	Guarda o caractere
	 */
	if (bcb.y_dmesg_ptr < bcb.y_dmesg_end)
		*bcb.y_dmesg_ptr++ = c;

	/*
	 *	Examina o protocolo VT100 de cores
	 */
	switch (video_state)
	{
	    case VT100_NORMAL:
		break;

	    case VT100_ESC:
		if (c == '[')
			{ video_state = VT100_LB; video_arg = 0; return; }

		video_state = VT100_NORMAL;
		break;

	    case VT100_LB:
		if   (c >= '0' && c <= '9')
			{ video_arg *= 10; video_arg += c - '0'; return; }

		video_state = VT100_NORMAL;

		if (c == 'm')
		{
			if (video_arg == 0)
				vdp->video_ch_color = WHITE_COLOR;
			else
				vdp->video_ch_color = video_arg - 30;
			return;
		}

		break;
	}

	/*
	 *	Localiza o cursor
	 */
	if ((line = vdp->video_cursor_line + vdp->video_line_offset) >= vp->video_LINE)
		line -= vp->video_LINE;

	vlp = &vdp->video_line[line];
	vcp = &vlp->video_line[vdp->video_cursor_col];

	/*
	 *	Se o caractere for gráfico, imprime e termina
	 */
	if (c >= ' ')
	{
		vcp->video_char	    = c;
		vcp->video_ch_color = vdp->video_ch_color;
	   /***	vcp->video_bg_color = vdp->video_bg_color; ***/

		put_video_char
		(	c,
			vdp->video_cursor_line,
			vdp->video_cursor_col,
			vdp->video_ch_color,
			vdp->video_bg_color
		);

		if (vlp->video_len < ++vdp->video_cursor_col)
			vlp->video_len = vdp->video_cursor_col;

		goto set_cursor;
	}

	/*
	 *	O caractere não é gráfico
	 */
	switch (c)
	{

	    case '\n':		/* Na verdade <nl> + <cr> */
		video_cursor_off ();
		vdp->video_cursor_line++;
		vdp->video_cursor_col = 0;
		break;

	    case '\r':
		video_cursor_off ();
		vdp->video_cursor_col = 0;
		break;

	    case '\t':
		video_cursor_off ();

		if ((vdp->video_cursor_col = (vdp->video_cursor_col + TAB) / TAB * TAB) > vp->video_COL)
			vdp->video_cursor_col = vp->video_COL;

		break;

	    case '\b':
		if (vdp->video_cursor_col <= 0)
			return;

		video_cursor_off ();
		vdp->video_cursor_col--;
		break;

	    case '\e':
		video_state = VT100_ESC;
		return;

	    default:				/* Ignora os outros caracteres */
		return;

	}	/* end switch */
	
	/*
	 *	Dá scrolls, se necessário
	 */
    set_cursor:
	if (vdp->video_cursor_col >= vp->video_COL)
		{ vdp->video_cursor_line++; vdp->video_cursor_col -= vp->video_COL; }

	if (vdp->video_cursor_line >= vp->video_LINE)
		{ video_up_scroll (0, vp->video_LINE - 1); vdp->video_cursor_line--; }

	/*
	 *	Põe o cursor
	 */
	video_cursor_on ();

}	/* end putchar */

/*
 ****************************************************************
 *	Deslocamento da Tela: Para cima				*
 ****************************************************************
 */
void
video_up_scroll (int scroll_begin, int scroll_end)
{
	const VIDEODATA	*vp = &video_data;
	VIDEODISPLAY	*vdp = vp->video_display[0];
	VIDEOLINE	*first_vlp, *last_vlp;
	VIDEOLINE	*src_vlp, *dst_vlp;
	VIDEOLINE	*vlp;
	VIDEOCHAR	*vcp;
	int		char_color = vdp->video_normal_ch_color;
	int		back_color = vdp->video_normal_bg_color;
	int		scroll_begin_line, scroll_end_line;
	int		n, i, j, old_len;

	/*
	 *	Prólogo
	 */
	first_vlp = &vdp->video_line[0]; last_vlp = first_vlp + vp->video_LINE - 1;

	if ((scroll_begin_line = scroll_begin + vdp->video_line_offset) >= vp->video_LINE)
		scroll_begin_line -= vp->video_LINE;

	/*
	 *	Reescreve todas as linhas
	 */
	vlp = first_vlp + scroll_begin_line; old_len = vlp->video_len;

	for (i = scroll_begin; /* abaixo */; i++)
	{
		if (++vlp > last_vlp)
			vlp = first_vlp;

		if (i >= scroll_end)
			break;

		for (j = 0, vcp = &vlp->video_line[0]; j < vlp->video_len; j++, vcp++)
			put_video_char (vcp->video_char, i, j, vcp->video_ch_color, vcp->video_bg_color);

		for (/* acima */; j < old_len; j++)
			put_video_char (' ', i, j, DONT_CARE_COLOR, back_color);

		old_len = vlp->video_len;
	}

	/*
	 *	Limpa a última linha na tela
	 */
	for (j = 0; j < old_len; j++)
		put_video_char (' ', i, j, DONT_CARE_COLOR, back_color);

	/*
	 *	Caso simples: Avança o deslocamento entre a imagem e a cópia
	 */
	if ((n = scroll_end - scroll_begin) >= vp->video_LINE - 1)
	{
		if (++vdp->video_line_offset >= vp->video_LINE)
			vdp->video_line_offset = 0;

		memsetl (vlp->video_line, (back_color << 16) | (char_color << 8) | ' ', vlp->video_len);
		vlp->video_len = 0;
		return;
	}

	/*
	 *	Caso complexo: Faz a cópia necessária
	 */
	if ((scroll_end_line = scroll_end + vdp->video_line_offset) >= vp->video_LINE)
		scroll_end_line -= vp->video_LINE;

	vlp = first_vlp + scroll_end_line;

	if (n <= (vp->video_LINE >> 1))	/* O "scroll" é pequeno => move as linhas do "scroll" */
	{
		dst_vlp = first_vlp + scroll_begin_line; src_vlp = dst_vlp + 1;

		for (/* acima */; n > 0; src_vlp++, dst_vlp++, n--)
		{
			if (src_vlp > last_vlp)
				src_vlp = first_vlp;

			if (dst_vlp > last_vlp)
				dst_vlp = first_vlp;

			memmove (dst_vlp, src_vlp, sizeof (VIDEOLINE));
		}
	}
	else 				/* O "scroll" é grande => move as outras linhas */
	{
		dst_vlp = first_vlp + scroll_begin_line; src_vlp = dst_vlp - 1;

		for (n = vp->video_LINE - n - 2; n > 0; src_vlp--, dst_vlp--, n--)
		{
			if (src_vlp < first_vlp)
				src_vlp = last_vlp;

			if (dst_vlp < first_vlp)
				dst_vlp = last_vlp;

			memmove (dst_vlp, src_vlp, sizeof (VIDEOLINE));
		}

		if (++vdp->video_line_offset >= vp->video_LINE)
			vdp->video_line_offset = 0;

		if (++vlp > last_vlp)
			vlp = first_vlp;
	}

	/*
	 *	Limpa a primeira linha, na cópia
	 */
	memsetl (vlp->video_line, (back_color << 16) | (char_color << 8) | ' ', vlp->video_len);
	vlp->video_len = 0;

}	/* end video_up_scroll */

/*
 ****************************************************************
 *	Desliga o cursor					*
 ****************************************************************
 */
void
video_cursor_off (void)
{
	const VIDEODATA	*vp = &video_data;
	VIDEODISPLAY	*vdp = vp->video_display[0];
	VIDEOLINE	*vlp;
	VIDEOCHAR	*vcp;
	int		line;

	if ((line = vdp->video_cursor_line + vdp->video_line_offset) >= vp->video_LINE)
		line -= vp->video_LINE;

	vlp = &vdp->video_line[line];
	vcp = &vlp->video_line[vdp->video_cursor_col];

	put_video_char
	(
		vcp->video_char,
		vdp->video_cursor_line,
		vdp->video_cursor_col,
		vcp->video_ch_color,
		vdp->video_bg_color
	);

}	/* end video_cursor_off */

/*
 ****************************************************************
 *	Liga o cursor						*
 ****************************************************************
 */
void
video_cursor_on (void)
{
	const VIDEODATA	*vp = &video_data;
	VIDEODISPLAY	*vdp = vp->video_display[0];
	VIDEOLINE	*vlp;
	VIDEOCHAR	*vcp;
	int		line;

	if ((line = vdp->video_cursor_line + vdp->video_line_offset) >= vp->video_LINE)
		line -= vp->video_LINE;

	vlp = &vdp->video_line[line];
	vcp = &vlp->video_line[vdp->video_cursor_col];

	put_video_char
	(
		vcp->video_char,
		vdp->video_cursor_line,
		vdp->video_cursor_col,
		vdp->video_bg_color,
		vcp->video_ch_color
	);

}	/* end video_cursor_on */

/*
 ****************************************************************
 *	Escreve um caractere na tela				*
 ****************************************************************
 */
void
put_video_char (int c, int line, int col, int char_color, int back_color)
{
	const VIDEODATA		*vp = &video_data;
	char			*video_ptr;
	ushort			*vga_ptr;
	const char		*font_tb;
	int			h, w;
	char			one_font_line, mask;

	/*
	 *	Verifica qual o modo (VGA/SVGA)
	 */
	if (vp->video_mode)			/* Modo SVGA (113x48) */
	{
		video_ptr = vp->video_linear_area +
				line * vp->video_line_pixels +
				col  * (vp->video_font_cols + 1);

		font_tb = &video_8x16_tb[c << 4];

		/* Gera o caractere, pixel a pixel */

		for (h = vp->video_font_lines; h > 0; h--)
		{
			one_font_line = *font_tb++;

			for (w = vp->video_font_cols, mask = 0x80; w > 0; w--, mask >>= 1)
			{
				if (one_font_line & mask)
					*video_ptr++ = char_color;
				else
					*video_ptr++ = back_color;
			}

			video_ptr += vp->video_cols - vp->video_font_cols;
		}
	}
	else					/* Modo VGA (80x25) */
	{
		/* Resta a conversão dos 8 bits/cor para 4 bits/cor */

		vga_ptr = BOOT_VGA_ADDR + line * vp->video_COL + col;

		vga_ptr[0] = (back_color << 12) | (char_color << 8) | c;
	}

}	/* end put_video_char */

#ifdef	PRINT_SVGA_TABLE
/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
	printf
	(	"\n"
		"r0			= %04X\n\n"
		"modeattr		= %04X\n"
		"win_attr_A		= %02X\n"
		"win_attr_B		= %02X\n"
		"gran			= %04X\n"
		"size			= %04X\n"
		"seg_A			= %04X\n"
		"seg_B			= %04X\n"
		"func			= %P\n"
		"scan_line		= %04X\n\n"

		"svga_width		= %04X\n"
		"svga_height		= %04X\n"
		"svga_cell_width	= %02X\n"
		"svga_cell_height	= %02X\n"
		"svga_mem_planes	= %02X\n"
		"svga_bits_per_pixel	= %02X\n"
		"svga_banks		= %02X\n"
		"svga_model_type	= %02X\n"
		"svga_bank_size		= %02X\n"
		"svga_image_pages	= %02X\n"
		"svga_reser		= %02X\n\n"

		"svga_red_mask_sz	= %02X\n"
		"svga_red_field_pos	= %02X\n"
		"svga_green_mask_sz	= %02X\n"
		"svga_green_field_sz	= %02X\n"
		"svga_blue_mask_sz	= %02X\n"
		"svga_blue_field_sz	= %02X\n"
		"svga_reser_mask_sz	= %02X\n"
		"svga_reser_mask_pos	= %02X\n"
		"svga_direct_color_mode	= %02X\n\n"

		"svga_linear_mem_addr	= %P\n"
		"svga_off_screen_addr	= %P\n"
		"svga_off_screen_sz	= %04X\n\n",

		supervga_r0,

		supervga_info.svga_mode_attr,
		supervga_info.svga_win_attr_A,
		supervga_info.svga_win_attr_B,
		supervga_info.svga_gran,
		supervga_info.svga_size,
		supervga_info.svga_seg_A,
		supervga_info.svga_seg_B,
		supervga_info.svga_func,
		supervga_info.svga_scan_line,

		supervga_info.svga_width,
		supervga_info.svga_height,
		supervga_info.svga_cell_width,
		supervga_info.svga_cell_height,
		supervga_info.svga_mem_planes,
		supervga_info.svga_bits_per_pixel,
		supervga_info.svga_banks,
		supervga_info.svga_model_type,
		supervga_info.svga_bank_size,
		supervga_info.svga_image_pages,
		supervga_info.svga_reser,

		supervga_info.svga_red_mask_sz,
		supervga_info.svga_red_field_pos,
		supervga_info.svga_green_mask_sz,
		supervga_info.svga_green_field_sz,
		supervga_info.svga_blue_mask_sz,
		supervga_info.svga_blue_field_sz,
		supervga_info.svga_reser_mask_sz,
		supervga_info.svga_reser_mask_pos,
		supervga_info.svga_direct_color_mode,

		supervga_info.svga_linear_mem_addr,
		supervga_info.svga_off_screen_addr,
		supervga_info.svga_off_screen_sz
	);
#endif	PRINT_SVGA_TABLE
