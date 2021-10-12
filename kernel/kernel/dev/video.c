/*
 ****************************************************************
 *								*
 *			video.c					*
 *								*
 *	Pequeno "driver" para o video				*
 *								*
 *	Versão	3.0.0, de 26.06.94				*
 *		4.9.0, de 13.08.06				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2006 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include "../h/common.h"
#include "../h/scb.h"
#include "../h/sync.h"

#include "../h/cpu.h"
#include "../h/video.h"
#include "../h/timeout.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Definições globais					*
 ****************************************************************
 */
enum {	TAB = 8 };					/* Definição da tabulação */

entry VIDEODATA		*video_data;			/* A tabela de video */

entry VIDEODISPLAY	*curr_video_display;		/* Tela virtual corrente */

entry int		video_beep_period;		/* Período em ação */

entry int		video_line = 0;			/* Para controlar a tela cheia */

/*
 ******	Protótipos de funções ***********************************
 */
void			video_up_scroll (int scroll_begin, int scroll_end);
void			video_down_scroll (int scroll_begin, int scroll_end);
void			put_video_char (int c, int line, int col, int char_color, int back_color);
void			video_cursor_on (void);
void			video_cursor_off (void);
void			video_clr_cursor_to_eol (void);
void			video_clr_bol_to_cursor (void);
void			video_clr_line (void);
void			video_clr_begin_of_display_to_cursor (void);
void			video_clr_cursor_to_end_of_display (void);
void			video_beep (int, int);
void			video_beep_stop (int);

/*
 ****************************************************************
 *	Escreve um caractere no video, na posição corrente	*
 ****************************************************************
 */
void
writechar (int c)
{
	const VIDEODATA	*vp = video_data;
	VIDEODISPLAY	*vdp = curr_video_display;
	VIDEOLINE	*vlp;
	VIDEOCHAR	*vcp;
	int		line;

	/*
	 *	Verifica se está no meio do protocolo "vt100"
	 */
	if (vdp->vt100_state != VT100_NORMAL)
		{ video_vt100_protocol (c); return; }

#if (0)	/*******************************************************/
	/*
	 *	Verifica se encheu a tela: repare que
	 *	"wait_for_ctl_Q ()" zera "video_line"
	 */
	if (scb.y_boot_verbose && video_line >= LINE - 1 && !scb.y_std_boot)
		wait_for_ctl_Q ();
#endif	/*******************************************************/

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
		vcp->video_char  = c;
		vcp->video_ch_color = vdp->video_ch_color;

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
	    case '\a':
		video_beep (1193180 / 880, scb.y_hz >> 2);
		return;

	    case '\e':
		video_vt100_protocol (c);
		return;

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

	    case '\f':
		video_clr_display (1 /* copy also */);
		break;

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

}	/* end writechar */

/*
 ****************************************************************
 *	Deslocamento da Tela: Para cima				*
 ****************************************************************
 */
void
video_up_scroll (int scroll_begin, int scroll_end)
{
	const VIDEODATA	*vp = video_data;
	VIDEODISPLAY	*vdp = curr_video_display;
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
 *	Deslocamento da Tela: Para baixo			*
 ****************************************************************
 */
void
video_down_scroll (int scroll_begin, int scroll_end)
{
	const VIDEODATA	*vp = video_data;
	VIDEODISPLAY	*vdp = curr_video_display;
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

	if ((scroll_end_line = scroll_end + vdp->video_line_offset) >= vp->video_LINE)
		scroll_end_line -= vp->video_LINE;

	/*
	 *	Reescreve todas as linhas
	 */
	vlp = first_vlp + scroll_end_line; old_len = vlp->video_len;

	for (i = scroll_end; /* abaixo */; i--)
	{
		if (--vlp < first_vlp)
			vlp = last_vlp;

		if (i <= scroll_begin)
			break;

		for (j = 0, vcp = &vlp->video_line[0]; j < vlp->video_len; j++, vcp++)
			put_video_char (vcp->video_char, i, j, vcp->video_ch_color, vcp->video_bg_color);

		for (/* acima */; j < old_len; j++)
			put_video_char (' ', i, j, DONT_CARE_COLOR, back_color);

		old_len = vlp->video_len;
	}

	/*
	 *	Limpa a primeira linha na tela
	 */
	for (j = 0; j < old_len; j++)
		put_video_char (' ', i, j, DONT_CARE_COLOR, back_color);

	/*
	 *	Caso simples: Avança o deslocamento entre a imagem e a cópia
	 */
	if ((n = scroll_end - scroll_begin) >= vp->video_LINE - 1)
	{
		if (--vdp->video_line_offset < 0)
			vdp->video_line_offset = vp->video_LINE - 1;

		memsetl (vlp->video_line, (back_color << 16) | (char_color << 8) | ' ', vlp->video_len);
		vlp->video_len = 0;
		return;
	}

	/*
	 *	Caso complexo: Faz a cópia necessária
	 */
	if ((scroll_begin_line = scroll_begin + vdp->video_line_offset) >= vp->video_LINE)
		scroll_begin_line -= vp->video_LINE;

	vlp = first_vlp + scroll_begin_line;

	if (n <= (vp->video_LINE >> 1))	/* O "scroll" é pequeno => move as linhas do "scroll" */
	{
		dst_vlp = first_vlp + scroll_end_line; src_vlp = dst_vlp - 1;

		for (/* acima */; n > 0; src_vlp--, dst_vlp--, n--)
		{
			if (src_vlp < first_vlp)
				src_vlp = last_vlp;

			if (dst_vlp < first_vlp)
				dst_vlp = last_vlp;

			memmove (dst_vlp, src_vlp, sizeof (VIDEOLINE));
		}
	}
	else				/* O "scroll" é grande => move as outras linhas */
	{
		dst_vlp = first_vlp + scroll_end_line; src_vlp = dst_vlp + 1;

		for (n = vp->video_LINE - n - 2; n > 0; src_vlp++, dst_vlp++, n--)
		{
			if (src_vlp > last_vlp)
				src_vlp = first_vlp;

			if (dst_vlp > last_vlp)
				dst_vlp = first_vlp;

			memmove (dst_vlp, src_vlp, sizeof (VIDEOLINE));
		}

		if (--vdp->video_line_offset < 0)
			vdp->video_line_offset = vp->video_LINE - 1;

		if (--vlp < first_vlp)
			vlp = last_vlp;
	}

	/*
	 *	Limpa a primeira linha, na cópia
	 */
	memsetl (vlp->video_line, (back_color << 16) | (char_color << 8) | ' ', vlp->video_len);
	vlp->video_len = 0;

}	/* end video_down_scroll */

/*
 ****************************************************************
 *	Desliga o cursor					*
 ****************************************************************
 */
void
video_cursor_off (void)
{
	const VIDEODATA	*vp = video_data;
	VIDEODISPLAY	*vdp = curr_video_display;
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
	const VIDEODATA	*vp = video_data;
	VIDEODISPLAY	*vdp = curr_video_display;
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
	const VIDEODATA		*vp = video_data;
	char			*video_ptr;
	ushort			*vga_ptr;
	const char		*font_tb;
	int			h, w;
	char			one_font_line, mask;

	/*
	 *	Em primeiro lugar, verifica se o protetor de telas está ativo
	 */
	if (screen_saver_state != SCREEN_SAVER_IDLE)
		return;

	/*
	 *	Verifica qual o modo (VGA/SVGA)
	 */
	if   (video_data->video_mode == 1) 	/* Modo SVGA (113x48) */
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
	elif (video_data->video_mode == 0) 	/* Modo VGA (80x25) */
	{
		/* Resta a conversão dos 8 bits/cor para 4 bits/cor */

		vga_ptr = VGA_ADDR + line * vp->video_COL + col;

		vga_ptr[0] = (back_color << 12) | (char_color << 8) | c;
	}

}	/* end put_video_char */

/*
 ****************************************************************
 *	Executa o protocolo "vt100"				*
 ****************************************************************
 */
void
video_vt100_protocol (int c)
{
	const VIDEODATA	*vp = video_data;
	VIDEODISPLAY	*vdp = curr_video_display;
	int		l, f, n;

	switch (vdp->vt100_state)
	{
	    case VT100_NORMAL:
		if (c == '\e')
			vdp->vt100_state = VT100_ESC;
		return;

	    case VT100_ESC:
		switch (c)
		{
		    case '[':
			vdp->vt100_arg_1 = vdp->vt100_arg_2 = 0;
			vdp->vt100_state = VT100_LB;
			return;

		    case '=':		/* Por enquanto ignora */
		    case '>':
			break;

		    case 'A':		/* Uma posição para cima ("cuu1") */
			if (vdp->video_cursor_line > 0)
				{ video_cursor_off (); vdp->video_cursor_line--; video_cursor_on (); }
			break;

		    case 'C':		/* Uma posição para direita ("cuf1") */
			video_cursor_off ();

			if (++vdp->video_cursor_col >= vp->video_COL)
			{
				vdp->video_cursor_col = 0;

				if (++vdp->video_cursor_line >= vp->video_LINE)
					{ vdp->video_cursor_col = vp->video_COL - 1; vdp->video_cursor_line--; }
			}

			video_cursor_on ();
			break;


		    case 'D':		/* Rolamento normal ("ind") */
			video_cursor_off ();
			video_up_scroll (vdp->vt100_scroll_begin, vdp->vt100_scroll_end);
			video_cursor_on ();
			break;

		    case 'M':		/* Rolamento inverso ("ri") */
			video_cursor_off ();
			video_down_scroll (vdp->vt100_scroll_begin, vdp->vt100_scroll_end);
			video_cursor_on ();
			break;

		    case '7':		/* Save cursor position ("sc") */
			vdp->vt100_save_line = vdp->video_cursor_line;
			vdp->vt100_save_col  = vdp->video_cursor_col;
			break;

		    case '8':		/* Restore cursor position ("rc") */
			video_cursor_off ();
			vdp->video_cursor_line = vdp->vt100_save_line;
			vdp->video_cursor_col =  vdp->vt100_save_col;
			video_cursor_on ();
			break;

		}	/* end switch */

		goto restore_normal;

	    case VT100_LB:
		if   (c >= '0' && c <= '9')
		{
			vdp->vt100_arg_1 *= 10;
			vdp->vt100_arg_1 += c - '0';
			return;
		}

		switch (c)
		{
		    case ';':		/* Foi visto "[;" */
			vdp->vt100_state = VT100_SC;
			return;

		    case '?':		/* Foi visto "[?" */
			vdp->vt100_state = VT100_QUESTION;
			return;

		    case 'A':		/* Posições para cima ("cuu") */
			if ((n = vdp->vt100_arg_1) <= 0)
				n = 1;

			video_cursor_off ();

			if ((vdp->video_cursor_line -= n) < 0)
				vdp->video_cursor_line = 0;

			video_cursor_on ();
			break;

		    case 'B':		/* Posições para baixo ("cud") */
			if ((n = vdp->vt100_arg_1) <= 0)
				n = 1;

			video_cursor_off ();

			if ((vdp->video_cursor_line += n) >= vp->video_LINE)
				vdp->video_cursor_line = vp->video_LINE - 1;

			video_cursor_on ();
			break;

		    case 'C':		/* Posições para direita ("cuf") */
			if ((n = vdp->vt100_arg_1) <= 0)
				n = 1;

			video_cursor_off ();

			if ((vdp->video_cursor_col += n) >= vp->video_COL)
			{
				int		line, col;

				line = vdp->video_cursor_col / vp->video_COL;
				col  = vdp->video_cursor_col % vp->video_COL;

				vdp->video_cursor_col = col;

				if ((vdp->video_cursor_line += line) >= vp->video_LINE)
				{
					vdp->video_cursor_col  = vp->video_COL  - 1;
					vdp->video_cursor_line = vp->video_LINE - 1;
				}
			}

			video_cursor_on ();
			break;

		    case 'D':		/* Posições para esquerda ("cub") */
			if ((n = vdp->vt100_arg_1) <= 0)
				n = 1;

			video_cursor_off ();

			if ((vdp->video_cursor_col -= n) < 0)
			{
				int		line, col;

				line = vdp->video_cursor_col / vp->video_COL;
				col  = vdp->video_cursor_col % vp->video_COL;

				vdp->video_cursor_col = vp->video_COL + col;

				if ((vdp->video_cursor_line += (line - 1)) < 0)
					{ vdp->video_cursor_col = 0; vdp->video_cursor_line = 0; }
			}

			video_cursor_on ();
			break;

		    case 'H':		/* Home ("home") */
			video_cursor_off ();
			vdp->video_cursor_line = vdp->video_cursor_col = 0;
			video_cursor_on ();
			break;

		    case 'J':		/* Apaga trechos da tela */
			switch (vdp->vt100_arg_1)
			{
			    case 0:		/* Cursor até final da tela */
				video_clr_cursor_to_end_of_display ();
				break;

			    case 1:		/* Início da tela até cursor */
				video_clr_begin_of_display_to_cursor ();
				break;

			    case 2:		/* Tela toda */
				video_clr_display (1 /* copy also */);
				break;

			    default:
				goto restore_normal;

			}	/* end switch */

			video_cursor_on ();
			break;

		    case 'K':		/* Apaga outros trechos da tela */
			switch (vdp->vt100_arg_1)
			{
			    case 0:		/* Cursor até final da linha */
				video_clr_cursor_to_eol ();
				break;

			    case 1:		/* Início da linha até cursor */
				video_clr_bol_to_cursor ();
				break;

			    case 2:		/* Linha toda */
				video_clr_line ();
				break;

			    default:
				goto restore_normal;

			}	/* end switch */

			video_cursor_on ();
			break;

		    case 'm':		/* Campos diversos (ainda não feito) */
			switch (vdp->vt100_arg_1)
			{
			    case 0:		/* Restaura os atributos */
			   	vdp->video_ch_color = vdp->video_normal_ch_color;
			   	vdp->video_bg_color = vdp->video_normal_bg_color;
				break;

			    case 1:		/* Bold */
			   	vdp->video_ch_color |= 0x08;
				break;

			    case 2:		/* NÃO bold */
			   	vdp->video_ch_color &= ~0x08;
				break;

#if (0)	/*******************************************************/
			    case 5:		/* Blink */
			   	vp->vt100_char_color |= 0x8000;
				break;
#endif	/*******************************************************/

			    case 7:		/* Reverso */
				n = vdp->video_ch_color;
			   	vdp->video_ch_color = vdp->video_bg_color;
			   	vdp->video_bg_color = n;
				break;

			    case 30:		/* Cor de Frente */
			    case 31:
			    case 32:
			    case 33:
			    case 34:
			    case 35:
			    case 36:
			    case 37:
				vdp->video_ch_color = (vdp->vt100_arg_1 - 30);
				break;

			    case 39:		/* Volta a Cor de Frente normal */	
			   	vdp->video_ch_color = vdp->video_normal_ch_color;
				break;

			    case 40:		/* Cor de Fundo */
			    case 41:
			    case 42:
			    case 43:
			    case 44:
			    case 45:
			    case 46:
			    case 47:
				vdp->video_bg_color = (vdp->vt100_arg_1 - 40);
				break;

			    case 49:		/* Volta a Cor de Fundo normal */
			   	vdp->video_bg_color = vdp->video_normal_bg_color;
				break;

			    default:
				goto restore_normal;

			}	/* end switch */

		}	/* end switch */

		goto restore_normal;

	    case VT100_SC:
		if   (c >= '0' && c <= '9')
		{
			vdp->vt100_arg_2 *= 10;
			vdp->vt100_arg_2 += c - '0';
			return;
		}

		switch (c)
		{
		    case 'H':		/* Endereçamento ("cup") */
			video_cursor_off ();

			l = vdp->vt100_arg_1 - 1;
			c = vdp->vt100_arg_2 - 1;

			if   (l < 0)
				l = 0;
			elif (l >= vp->video_LINE)
				l = vp->video_LINE - 1;

			if   (c < 0)
				c = 0;
			elif (c >= vp->video_COL)
				c = vp->video_COL - 1;

			vdp->video_cursor_line = l;
			vdp->video_cursor_col  = c;

			video_cursor_on ();
			break;

		    case 'm':		/* "[1;7m": Estilo bold ("smso") */
			if (vdp->vt100_arg_1 == 1 && vdp->vt100_arg_2 == 7)
			   	vdp->video_ch_color |= 0x08;
			break;

		    case 'r':		/* Prepara um rolamento ("csr") */
			l = vdp->vt100_arg_1 - 1;
			f = vdp->vt100_arg_2 - 1;

			if   (l < 0)
				l = 0;
			elif (l >= vp->video_LINE)
				l = vp->video_LINE-1;

			if   (f < 0)
				f = 0;
			elif (f >= vp->video_LINE)
				f = vp->video_LINE-1;

			if (l < f)
				{ vdp->vt100_scroll_begin = l; vdp->vt100_scroll_end = f; }
			break;

		}	/* end switch */

	   	goto restore_normal;

	    case VT100_QUESTION:	/* Já foi visto "[?" */
		if   (c >= '0' && c <= '9')
		{
			vdp->vt100_arg_2 *= 10;
			vdp->vt100_arg_2 += c - '0';
			return;
		}

		if (c == ';')		/* Para ignorar todos os números */
			return;

		/* Por enquanto ignora toda a série "[?" */

	   /***	goto restore_normal; ***/

	    restore_normal:
	    default:
		vdp->vt100_state = VT100_NORMAL;
		return;

	}	/* end switch (state) */

}	/* end video_vt100_protocol */

/*
 ****************************************************************
 *	Zera a tela toda					*
 ****************************************************************
 */
void
video_clr_display (int copy_also)
{
	const VIDEODATA	*vp = video_data;
	VIDEODISPLAY	*vdp = curr_video_display;
	VIDEOLINE	*vlp;
	const VIDEOLINE	*end_vlp;
	int		color;

	/*
	 *	Zera a tela
	 */
	if   (video_data->video_mode == 1) 	/* Modo SVGA (113x48) */
	{
		color = vdp->video_normal_bg_color;

		memsetl
		(	vp->video_linear_area,
			(color << 24) | (color << 16) | (color << 8) | color,
			vp->video_lines * vp->video_cols / sizeof (long)
		);
	}
	elif (video_data->video_mode == 0) 	/* Modo VGA (80x25) */
	{
		memsetw
		(	VGA_ADDR,
			(vdp->video_normal_bg_color << 12) | (vdp->video_normal_ch_color << 8) | ' ',
			vp->video_LINE * vp->video_COL
		);
	}

	/*
	 *	Zera a cópia
	 */
	if (copy_also)
	{
		for (vlp = &vdp->video_line[0], end_vlp = vlp + vp->video_LINE; vlp < end_vlp; vlp++)
		{
			memsetl
			(	vlp->video_line,
				(vdp->video_normal_bg_color << 16) | (vdp->video_normal_ch_color << 8) | ' ',
				vp->video_COL
			);
			vlp->video_len = 0;
		}

		vdp->video_cursor_line = vdp->video_cursor_col = 0;
	}

}	/* end video_clr_display */

/*
 ****************************************************************
 *	Zera do cursor até o final da linha			*
 ****************************************************************
 */
void
video_clr_cursor_to_eol (void)
{
	const VIDEODATA	*vp = video_data;
	VIDEODISPLAY	*vdp = curr_video_display;
	VIDEOLINE	*vlp;
	int		line, j;

	/*
	 *	Calcula o local do cursor
	 */
	if ((line = vdp->video_cursor_line + vdp->video_line_offset) >= vp->video_LINE)
		line -= vp->video_LINE;

	vlp = &vdp->video_line[line];

	/*
	 *	Zera o resto da linha (no vídeo)
	 */
	for (j = vdp->video_cursor_col; j < vlp->video_len; j++)
		put_video_char (' ', vdp->video_cursor_line, j, DONT_CARE_COLOR, vdp->video_normal_bg_color);

	/*
	 *	Zera o resto da linha (na cópia)
	 */
	memsetl
	(	vlp->video_line + vdp->video_cursor_col,
		(vdp->video_normal_bg_color << 16) | (vdp->video_normal_ch_color << 8) | ' ',
		vlp->video_len - vdp->video_cursor_col
	);

	vlp->video_len = vdp->video_cursor_col;

}	/* end video_clr_cursor_to_eol */

/*
 ****************************************************************
 *	Zera do início da linha até cursor			*
 ****************************************************************
 */
void
video_clr_bol_to_cursor (void)
{
	const VIDEODATA	*vp = video_data;
	VIDEODISPLAY	*vdp = curr_video_display;
	VIDEOLINE	*vlp;
	int		line, j;

	/*
	 *	Calcula o local do cursor
	 */
	if ((line = vdp->video_cursor_line + vdp->video_line_offset) >= vp->video_LINE)
		line -= vp->video_LINE;

	vlp = &vdp->video_line[line];

	/*
	 *	Zera o início da linha até o cursor (no vídeo)
	 */
	for (j = 0; j <= vdp->video_cursor_col; j++)
		put_video_char (' ', vdp->video_cursor_line, j, DONT_CARE_COLOR, vdp->video_normal_bg_color);

	/*
	 *	Zera o início da linha até o cursor (na cópia)
	 */
	memsetl
	(	vlp->video_line,
		(vdp->video_normal_bg_color << 16) | (vdp->video_normal_ch_color << 8) | ' ',
		vdp->video_cursor_col + 1
	);

   /***	vlp->video_len = ...; ***/	/* Não muda */

}	/* end video_clr_bol_to_cursor */

/*
 ****************************************************************
 *	Zera a linha toda					*
 ****************************************************************
 */
void
video_clr_line (void)
{
	const VIDEODATA	*vp = video_data;
	VIDEODISPLAY	*vdp = curr_video_display;
	VIDEOLINE	*vlp;
	int		line, j;

	/*
	 *	Calcula o local do cursor
	 */
	if ((line = vdp->video_cursor_line + vdp->video_line_offset) >= vp->video_LINE)
		line -= vp->video_LINE;

	vlp = &vdp->video_line[line];

	/*
	 *	Zera a linha toda (no vídeo)
	 */
	for (j = 0; j < vlp->video_len; j++)
		put_video_char (' ', vdp->video_cursor_line, j, DONT_CARE_COLOR, vdp->video_normal_bg_color);

	/*
	 *	Zera a linha toda (na cópia)
	 */
	memsetl
	(	vlp->video_line,
		(vdp->video_normal_bg_color << 16) | (vdp->video_normal_ch_color << 8) | ' ',
		vlp->video_len
	);

	vlp->video_len = 0;

#if (0)	/*******************************************************/
	vdp->video_cursor_col = 0; vlp->video_len = 0;
#endif	/*******************************************************/

}	/* end video_clr_line */

/*
 ****************************************************************
 *	Zera do cursor até o final da tela			*
 ****************************************************************
 */
void
video_clr_cursor_to_end_of_display (void)
{
	const VIDEODATA	*vp = video_data;
	VIDEODISPLAY	*vdp = curr_video_display;
	VIDEOLINE	*vlp;
	int		line, i, j;

	/*
	 *	Zera a linha do cursor
	 */
	video_clr_cursor_to_eol ();

	/*
	 *	Zera da linha seguinte ao cursor até o final da tela (no vídeo e cópia)
	 */
	for (i = vdp->video_cursor_line + 1; i < vp->video_LINE; i++)
	{
		if ((line = i + vdp->video_line_offset) >= vp->video_LINE)
			line -= vp->video_LINE;

		vlp = &vdp->video_line[line];

		for (j = 0; j < vlp->video_len; j++)
			put_video_char (' ', i, j, DONT_CARE_COLOR, vdp->video_normal_bg_color);

		memsetl
		(	vlp->video_line,
			(vdp->video_bg_color << 16) | (vdp->video_ch_color << 8) | ' ',
			vlp->video_len
		);

		vlp->video_len = 0;
	}

}	/* end video_clr_cursor_to_end_of_display */

/*
 ****************************************************************
 *	Zera do início da tela até o cursor			*
 ****************************************************************
 */
void
video_clr_begin_of_display_to_cursor (void)
{
	const VIDEODATA	*vp = video_data;
	VIDEODISPLAY	*vdp = curr_video_display;
	VIDEOLINE	*vlp;
	int		line, i, j;

	/*
	 *	Zera do início da tela até a linha anterior ao cursor (no vídeo e cópia)
	 */
	for (i = 0; i < vdp->video_cursor_line; i++)
	{
		if ((line = i + vdp->video_line_offset) >= vp->video_LINE)
			line -= vp->video_LINE;

		vlp = &vdp->video_line[line];

		for (j = 0; j < vlp->video_len; j++)
			put_video_char (' ', i, j, DONT_CARE_COLOR, vdp->video_normal_bg_color);

		memsetl
		(	vlp->video_line,
			(vdp->video_bg_color << 16) | (vdp->video_ch_color << 8) | ' ',
			vlp->video_len
		);

		vlp->video_len = 0;
	}

	/*
	 *	Zera a linha do cursor
	 */
	video_clr_bol_to_cursor ();

}	/* end  video_clr_begin_of_display_to_cursor */

/*
 ****************************************************************
 *	Troca de tela virtual					*
 ****************************************************************
 */
void
video_change_display (int new_display)
{
	VIDEODATA	*vp = video_data;
	VIDEODISPLAY	*old_vdp, *new_vdp;
	VIDEOLINE	*old_vlp, *new_vlp;
	const VIDEOCHAR	*vcp;
	const VIDEOLINE	*end_old_vlp, *end_new_vlp;
	int		i, j, back_color;

	/*
	 *	Inicialização para a escrita na tela
	 */
	video_cursor_off ();

	old_vdp		= curr_video_display;
	old_vlp		= old_vdp->video_line + old_vdp->video_line_offset;
	end_old_vlp	= old_vdp->video_line + vp->video_LINE;

	new_vdp		= vp->video_display[new_display];
	new_vlp		= new_vdp->video_line + new_vdp->video_line_offset;
	end_new_vlp	= new_vdp->video_line + vp->video_LINE;

	back_color	= new_vdp->video_normal_bg_color;

	for (i = 0; i < vp->video_LINE; i++)
	{
		for (j = 0, vcp = &new_vlp->video_line[0]; j < new_vlp->video_len; j++, vcp++)
			put_video_char (vcp->video_char, i, j, vcp->video_ch_color, vcp->video_bg_color);

		for (/* acima */; j < old_vlp->video_len; j++)
			put_video_char (' ', i, j, DONT_CARE_COLOR, back_color);

		if (++old_vlp >= end_old_vlp)
			old_vlp -= vp->video_LINE;

		if (++new_vlp >= end_new_vlp)
			new_vlp -= vp->video_LINE;
	}

	/*
	 *	Troca finalizada
	 */
	curr_video_display = new_vdp;

	video_cursor_on ();

}	/* end video_change_display */

/*
 ****************************************************************
 *	Troca de cor						*
 ****************************************************************
 */
void
video_change_color (char new_ch_color, char new_bg_color)
{
	const VIDEODATA		*vp = video_data;
	VIDEODISPLAY		*vdp = curr_video_display;
	VIDEOLINE		*vlp;
	const VIDEOLINE		*end_vlp;
	char			clr_display = 0;

	/*
	 *	Troca a cor de fundo de toda a parte NÃO escrita
	 */
	if (new_ch_color != vdp->video_normal_ch_color)
	{
		for (vlp = vdp->video_line, end_vlp = vlp + vp->video_LINE; vlp < end_vlp; vlp++)
		{
			memsetl
			(	vlp->video_line + vlp->video_len,
				(new_bg_color << 16) | (new_ch_color << 8) | ' ',
				vp->video_COL - vlp->video_len
			);
		}

#if (0)	/*******************************************************/
		video_load_display ();
#endif	/*******************************************************/
	}

	if (new_bg_color != vdp->video_normal_bg_color)
		clr_display++;

	vdp->video_normal_ch_color = vdp->video_ch_color = new_ch_color;
	vdp->video_normal_bg_color = vdp->video_bg_color = new_bg_color;

	if (clr_display)
		video_clr_display (1 /* copy also */);

}	/* end video_change_color */

/*
 ****************************************************************
 *	Escreve a tela virtual atual				*
 ****************************************************************
 */
void
video_load_display (void)
{
	VIDEODATA	*vp = video_data;
	VIDEODISPLAY	*new_vdp;
	VIDEOLINE	*new_vlp;
	const VIDEOCHAR	*vcp;
	const VIDEOLINE	*end_new_vlp;
	int		i, j;

	/*
	 *	Inicialização para a escrita na tela
	 */
	new_vdp		= curr_video_display;
	new_vlp		= new_vdp->video_line + new_vdp->video_line_offset;
	end_new_vlp	= new_vdp->video_line + vp->video_LINE;

	for (i = 0; i < vp->video_LINE; i++)
	{
		for (j = 0, vcp = &new_vlp->video_line[0]; j < new_vlp->video_len; j++, vcp++)
			put_video_char (vcp->video_char, i, j, vcp->video_ch_color, vcp->video_bg_color);

		if (++new_vlp >= end_new_vlp)
			new_vlp -= vp->video_LINE;
	}

	video_cursor_on ();

}	/* end video_load_display */

/*
 ****************************************************************
 *	Faz o video começar a apitar				*
 ****************************************************************
 */
void
video_beep (int frequency, int period)
{
	if (video_beep_period != 0)
		return;

	write_port (read_port (0x61) | 3, 0x61);
	write_port (0xB6, 0x43);
	
	write_port (frequency,      0x42);
	write_port (frequency >> 8, 0x42);
	
	video_beep_period = period;

	toutset (period, video_beep_stop, period);

}	/* end video_beep */

/*
 ****************************************************************
 *	Faz o video parar de apitar				*
 ****************************************************************
 */
void
video_beep_stop (int period)
{
	write_port (read_port (0x61) & ~3, 0x61);

	video_beep_period = 0;

}	/* end video_beep_stop */

/*
 ****************************************************************
 *    Limpa o início da última linha (que eventualmente suja) *
 ****************************************************************
 */
void
video_clr_last_line (void)
{

}     /* end video_clr_last_line */ 
