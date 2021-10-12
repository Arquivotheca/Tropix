/*
 ****************************************************************
 *								*
 *			vt100.c					*
 *								*
 *	Executa o protocolo "vt100" para o video		*
 *								*
 *	Versão	3.0.0, de 20.02.95				*
 *		4.3.0, de 06.09.02				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include "../h/common.h"
#include "../h/scb.h"

#include "../h/video.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Executa o protocolo "vt100"				*
 ****************************************************************
 */
void
video_vt100_protocol (int c)
{
	VT100		*vtp = video_vt100;
	int		l, f;
	int		begin, n;
	char		color;

	switch (vtp->vt100_state)
	{
	    case VT100_NORMAL:
		if (c == '\e')
			vtp->vt100_state = VT100_ESC;
		return;

	    case VT100_ESC:
		switch (c)
		{
		    case '[':
			vtp->vt100_arg_1 = vtp->vt100_arg_2 = 0;
			vtp->vt100_state = VT100_LB;
			return;

		    case '=':		/* Por enquanto ignora */
		    case '>':
			break;

		    case 'A':		/* Uma posição para cima ("cuu1") */
			if (vtp->vt100_pos >= COL)
			{
				video_cursor_off (vtp->vt100_pos);
				video_cursor_on (vtp->vt100_pos -= COL);
			}
			break;

		    case 'C':		/* Uma posição para direita ("cuf1") */
			if (vtp->vt100_pos < LINE * COL - 1)
			{
				video_cursor_off (vtp->vt100_pos);
				video_cursor_on (++vtp->vt100_pos);
			}
			break;

		    case 'D':		/* Rolamento normal ("ind") */
			video_cursor_off (vtp->vt100_pos);

			memmove
			(	&VIDEOADDR[vtp->vt100_scroll_begin * COL],
				&VIDEOADDR[vtp->vt100_scroll_begin * COL + COL],
				2 * (vtp->vt100_scroll_end - vtp->vt100_scroll_begin) * COL
			);

			memsetw
			(	&VIDEOADDR[vtp->vt100_scroll_end * COL],
				vtp->vt100_normal_char_color|' ',
				COL
			);

			video_cursor_on (vtp->vt100_pos);
			break;

		    case 'M':		/* Rolamento inverso ("ri") */
			video_cursor_off (vtp->vt100_pos);

			memmove
			(	&VIDEOADDR[vtp->vt100_scroll_begin * COL + COL],
				&VIDEOADDR[vtp->vt100_scroll_begin * COL],
				2 * (vtp->vt100_scroll_end - vtp->vt100_scroll_begin) * COL
			);

			memsetw
			(	&VIDEOADDR[vtp->vt100_scroll_begin * COL],
				vtp->vt100_normal_char_color|' ',
				COL
			);

			video_cursor_on (vtp->vt100_pos);
			break;

		    case '7':		/* Save cursor position ("sc") */
			vtp->vt100_save_pos = vtp->vt100_pos;
			break;

		    case '8':		/* Restore cursor position ("rc") */
			video_cursor_off (vtp->vt100_pos);
			video_cursor_on  (vtp->vt100_pos = vtp->vt100_save_pos);
			break;

		}	/* end switch */

		goto restore_normal;

	    case VT100_LB:
		if   (c >= '0' && c <= '9')
		{
			vtp->vt100_arg_1 *= 10;
			vtp->vt100_arg_1 += c - '0';
			return;
		}

		switch (c)
		{
		    case ';':		/* Foi visto "[;" */
			vtp->vt100_state = VT100_SC;
			return;

		    case '?':		/* Foi visto "[?" */
			vtp->vt100_state = VT100_QUESTION;
			return;

		    case 'A':		/* Posições para cima ("cuu") */
			n = -vtp->vt100_arg_1 * COL;

			if (n >= 0)
				n = -COL;

			if (vtp->vt100_pos + n < 0)
				n = (vtp->vt100_pos % COL) - vtp->vt100_pos;

			video_cursor_off (vtp->vt100_pos);
			video_cursor_on (vtp->vt100_pos += n);
			break;

		    case 'B':		/* Posições para baixo ("cud") */
			n = vtp->vt100_arg_1 * COL;

			if (n <= 0)
				n = COL;

			if (vtp->vt100_pos + n >= LINE * COL)
				n = (LINE-1) * COL + (vtp->vt100_pos % COL) - vtp->vt100_pos;

			video_cursor_off (vtp->vt100_pos);
			video_cursor_on (vtp->vt100_pos += n);
			break;

		    case 'C':		/* Posições para direita ("cuf") */
			n = vtp->vt100_arg_1;

			if (n <= 0)
				n = 1;

			if (vtp->vt100_pos + n >= LINE * COL)
				n = (LINE * COL - 1) - vtp->vt100_pos;

			video_cursor_off (vtp->vt100_pos);
			video_cursor_on (vtp->vt100_pos += n);
			break;

		    case 'D':		/* Posições para esquerda ("cub") */
			n = -vtp->vt100_arg_1;

			if (n >= 0)
				n = -1;

			if (vtp->vt100_pos - n < 0)
				n = 0 - vtp->vt100_pos;

			video_cursor_off (vtp->vt100_pos);
			video_cursor_on (vtp->vt100_pos += n);
			break;

		    case 'H':		/* Home ("home") */
			video_cursor_off (vtp->vt100_pos);
			video_cursor_on (vtp->vt100_pos = 0);
			break;

		    case 'J':		/* Apaga trechos da tela */
			switch (vtp->vt100_arg_1)
			{
			    case 0:		/* Cursor até final da tela */
				begin = vtp->vt100_pos;
				n = LINE * COL - begin;
				break;

			    case 1:		/* Início da tela até cursor */
				begin = 0;
				n = vtp->vt100_pos + 1;
				break;

			    case 2:		/* Tela toda */
				begin = 0;
				n = LINE * COL;
				break;

			    default:
				goto restore_normal;

			}	/* end switch */

		   /***	video_cursor_off (vtp->vt100_pos); ***/
			memsetw (&VIDEOADDR[begin], vtp->vt100_normal_char_color|' ', n);
			video_cursor_on (vtp->vt100_pos);
			break;

		    case 'K':		/* Apaga outros trechos da tela */
			begin = vtp->vt100_pos;

			switch (vtp->vt100_arg_1)
			{
			    case 0:		/* Cursor até final da linha */
			   /***	begin = vtp->vt100_pos; ***/
				n = COL - begin % COL;
				break;

			    case 1:		/* Início da linha até cursor */
				begin -= begin % COL;
				n = vtp->vt100_pos - begin + 1;
				break;

			    case 2:		/* Linha toda */
				begin -= begin % COL;
				n = COL;
				break;

			    default:
				goto restore_normal;

			}	/* end switch */

		   /***	video_cursor_off (vtp->vt100_pos); ***/
			memsetw (&VIDEOADDR[begin], vtp->vt100_normal_char_color|' ', n);
			video_cursor_on (vtp->vt100_pos);
			break;

		    case 'm':		/* Campos diversos (ainda não feito) */
			switch (vtp->vt100_arg_1)
			{
			    case 0:		/* Restaura os atributos */
			   	vtp->vt100_char_color = vtp->vt100_normal_char_color;
				break;

			    case 1:		/* Bold */
			   	vtp->vt100_char_color |= 0x0800;
				break;

			    case 2:		/* NÃO bold */
			   	vtp->vt100_char_color &= ~0x0800;
				break;

			    case 5:		/* Blink */
			   	vtp->vt100_char_color |= 0x8000;
				break;

			    case 7:		/* Reverso */
			   	color = vtp->vt100_char_color >> 8;
			   	color = (color & 0x88) | ((color & 0x70) >> 4) | ((color & 0x07) << 4);
			   	vtp->vt100_char_color = color << 8;
				break;

			    case 30:		/* Cor de Frente */
			    case 31:
			    case 32:
			    case 33:
			    case 34:
			    case 35:
			    case 36:
			    case 37:
			   	vtp->vt100_char_color &= ~0x0700;
			   	vtp->vt100_char_color |= (vtp->vt100_arg_1 - 30) << 8;
				break;

			    case 39:		/* Volta a Cor de Frente normal */	
			   	vtp->vt100_char_color &= ~0x0700;
			   	vtp->vt100_char_color |= vtp->vt100_normal_char_color & 0x0700;
				break;

			    case 40:		/* Cor de Fundo */
			    case 41:
			    case 42:
			    case 43:
			    case 44:
			    case 45:
			    case 46:
			    case 47:
			   	vtp->vt100_char_color &= ~0x7000;
			   	vtp->vt100_char_color |= (vtp->vt100_arg_1 - 40) << 12;
				break;

			    case 49:		/* Volta a Cor de Fundo normal */
			   	vtp->vt100_char_color &= ~0x7000;
			   	vtp->vt100_char_color |= vtp->vt100_normal_char_color & 0x7000;
				break;

			    default:
				goto restore_normal;

			}	/* end switch */

		}	/* end switch */

		goto restore_normal;

	    case VT100_SC:
		if   (c >= '0' && c <= '9')
		{
			vtp->vt100_arg_2 *= 10;
			vtp->vt100_arg_2 += c - '0';
			return;
		}

		switch (c)
		{
		    case 'H':		/* Endereçamento ("cup") */
			l = vtp->vt100_arg_1 - 1;
			c = vtp->vt100_arg_2 - 1;

			if   (l < 0)
				l = 0;
			elif (l >= LINE)
				l = LINE-1;

			if   (c < 0)
				c = 0;
			elif (c >= COL)
				c = COL-1;

			video_cursor_off (vtp->vt100_pos);
			vtp->vt100_pos = l * COL + c;
			video_cursor_on (vtp->vt100_pos);
			break;

		    case 'm':		/* "[1;7m": Estilo bold ("smso") */
			if (vtp->vt100_arg_1 == 1 && vtp->vt100_arg_2 == 7)
			   	vtp->vt100_char_color |= 0x0800;
			break;

		    case 'r':		/* Prepara um rolamento ("csr") */
			l = vtp->vt100_arg_1 - 1;
			f = vtp->vt100_arg_2 - 1;

			if   (l < 0)
				l = 0;
			elif (l >= LINE)
				l = LINE-1;

			if   (f < 0)
				f = 0;
			elif (f >= LINE)
				f = LINE-1;

			if (l < f)
			{
				vtp->vt100_scroll_begin = l;
				vtp->vt100_scroll_end = f;
			}
			break;

		}	/* end switch */

	   	goto restore_normal;

	    case VT100_QUESTION:	/* Já foi visto "[?" */
		if   (c >= '0' && c <= '9')
		{
			vtp->vt100_arg_2 *= 10;
			vtp->vt100_arg_2 += c - '0';
			return;
		}

		if (c == ';')		/* Para ignorar todos os números */
			return;

		/* Por enquanto ignora toda a série "[?" */

	   /***	goto restore_normal; ***/

	    restore_normal:
	    default:
		vtp->vt100_state = VT100_NORMAL;
		return;

	}	/* end switch (state) */

}	/* end video_vt100_protocol */
