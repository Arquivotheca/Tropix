/*
 ****************************************************************
 *								*
 *			msg.c					*
 *								*
 *	Gerencia a janela de mensagens				*
 *								*
 *	Versão	4.5.0, de 24.06.03				*
 *		4.5.0, de 22.11.03				*
 *								*
 *	Módulo: xfm						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2003 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stat.h>
#include <errno.h>

#include <X11/Intrinsic.h>

#include "../h/xfm.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */

#define	BOX_Y		(SHADOW + 2)
#define	BOX_HEIGHT	(wp->height - 2 * SHADOW - 5)

/*
 ****** Dados da janela de mensagens ****************************
 */
entry char		msg_text[256];		/* Texto da janela de mensagens */
entry int		msg_len;

entry int		msg_type;		/* GOOD_MSG, ERROR_MSG ou QUESTION_MSG */

entry MSGFUNC		msg_func;

entry int		msg_rm;			/* Remove o arquivo não atualizado? */

entry const NODE	*msg_rm_node;		/* Arquivos a remover */

/*
 ******	Protótipos de funções ***********************************
 */

/*
 ****************************************************************
 *	Imprime uma mensagem normal				*
 ****************************************************************
 */
void
draw_msg_text (const char *format, ...)
{
	const WINDATA	*wp = &msg;

	msg_len = vsnprintf (msg_text, sizeof (msg_text), format, va_first (format));

	msg_type = GOOD_MSG;

	XClearWindow (display, wp->win);

	draw_msg_win ();

}	/* end draw_msg_text */

/*
 ****************************************************************
 *	Imprime uma mensagem de erro				*
 ****************************************************************
 */
void
draw_error_msg_text (const char *format, ...)
{
	const WINDATA	*wp = &msg;
	char		err;

	if ((err = *format) == '*')
                format++;

	msg_len = vsnprintf (msg_text, sizeof (msg_text), format, va_first (format));

	if (err == '*' && errno != 0)
	{
		msg_len += snprintf
		(	msg_text + msg_len, sizeof (msg_text) - msg_len,
			" (%s)", strerror (errno)
		);
	}

	msg_type = ERROR_MSG;

	XClearWindow (display, wp->win);

	draw_msg_win ();

}	/* end draw_error_msg_text */

/*
 ****************************************************************
 *	Imprime uma mensagem com pergunta 			*
 ****************************************************************
 */
void
draw_question_msg_text (int rm, const char *question, const char *format, ...)
{
	const WINDATA	*wp = &msg;
	char		err;

	msg_rm = rm;

	if ((err = *format) == '*')
                format++;

	msg_len = vsnprintf (msg_text, sizeof (msg_text), format, va_first (format));

	if (err == '*' && errno != 0)
	{
		msg_len += snprintf
		(	msg_text + msg_len, sizeof (msg_text) - msg_len,
			" (%s)", strerror (errno)
		);
	}

	if (question != NOSTR)
		msg_len += snprintf (msg_text + msg_len, sizeof (msg_text) - msg_len, question);

	msg_type = QUESTION_MSG;

	XClearWindow (display, wp->win);

	draw_msg_win ();

}	/* end draw_msg_text */

/*
 ****************************************************************
 *	Desenha a janela de mensagens				*
 ****************************************************************
 */
void
draw_msg_win (void)
{
	const WINDATA	*wp = &msg;
	int		font_x, font_y, box_x, box_y, box_width, box_height;

	draw_shadow
	(	wp->win,
		wp->triple.top_gc, wp->triple.bottom_gc,
		0, 0,
		wp->width, wp->height
	);

	if (msg_len == 0)
		return;

	if (msg_type != GOOD_MSG)
		draw_button (wp, &empty_dir_triple, 0, 0, wp->width, wp->height);

	font_x	= MENU_BORDER + SHADOW;
	font_y	= MENU_BORDER + 1 * (wp->font_height + 2 * SHADOW) - (wp->font_descent + SHADOW);

	box_x	   = font_x + (msg_len + 2) * wp->font_width;
	box_y 	   = BOX_Y;

	box_width  = 7 * wp->font_width;
	box_height = BOX_HEIGHT;

	XDrawString
	(	display, wp->win,
		wp->text_gc,
		font_x, font_y,
		msg_text, msg_len
	);

	/*
	 *	Se pede a confirmação, ...
	 */
	if (msg_type == QUESTION_MSG)
	{
		XMapWindow (display, msg_yes.win);
		XMapWindow (display, msg_no.win);
	}

}	/* end draw_msg_win */

/*
 ****************************************************************
 *	Desenha o botão de "SIM" ou "NÃO"			*
 ****************************************************************
 */
void
draw_msg_yes_or_no_win (const WINDATA *wp)
{

	XDrawString
	(	display, wp->win,
		wp->text_gc,
		2 * wp->font_width, MENU_BORDER + wp->font_height - wp->font_descent - 2,
		wp == &msg_yes ? "SIM" : "NÃO", 3
	);

}	/* end draw_msg_yes_win */

/*
 ****************************************************************
 *	Foi apertado um botão ("sim" ou "não") na janela MSG	*
 ****************************************************************
 */
void
do_msg_button_press (int code)
{
	static int		msg_first_code;

	/*
	 *	Code:	+1 para "SIM"
	 *	 	-1 para "NÃO"
	 */
	switch (msg_rm)
	{
	    case 0:		/* Caso normal: Não é um arquivo inexistente */
		break;

	    case 1:		/* Caso de um arquivo inexistente que não será copiado */
		if (code > 0)
			{ msg_rm = 0; break; }

		msg_first_code = code;
		draw_question_msg_text (2, "", "Remove o arquivo \"%s\"?", msg_rm_node->n_path);
		return;

	    case 2:		/* Resposta da consulta de remoção */
		if (code > 0)
		{
			/* As linhas seguintes são muito dependentes de outras funções!! */

			if (cmp_next_src_np == msg_rm_node)
				cmp_next_src_np = cmp_next_src_np->n_forw;

			rm_file_dir_2 ((NODE *)msg_rm_node);

			if (selected_file_cnt)
			{
				XClearWindow (display, dir.win);
			   /***	dir_line_first = 0; ***/

				dir_scroll.button_no	= 0;
				dir_scroll.last_offset	= -1;

				draw_dir_win ();
			}
		}

		msg_rm = 0; code = msg_first_code; break;
	}

	if (msg_func != NOMSGFUNC)
		(*msg_func) (code);

}	/* end do_msg_button_press */

/*
 ****************************************************************
 *	Limpa a janela de mensagens				*
 ****************************************************************
 */
void
clear_msg_win (void)
{
	const WINDATA	*wp = &msg;

	if (msg_len == 0)
		return;

	XUnmapWindow (display, msg_yes.win);
	XUnmapWindow (display, msg_no.win);

	XClearWindow (display, wp->win);

	draw_shadow
	(	wp->win,
		wp->triple.top_gc, wp->triple.bottom_gc,
		0, 0,
		wp->width, wp->height
	);

	msg_len = 0;

}	/* end clear_msg_win */
