/*
 ****************************************************************
 *								*
 *			msg.c					*
 *								*
 *	Gerencia a janela de mensagens				*
 *								*
 *	Versão	4.5.0, de 24.06.03				*
 *		4.6.0, de 08.04.04				*
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

#include <sys/types.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stat.h>
#include <errno.h>

#include <X11/Intrinsic.h>

#include "../h/xview.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
#define	BOX_Y		(SHADOW + 2)
#define	BOX_HEIGHT	(wp->height - 2 * SHADOW - 5)

enum { GOOD_MSG, ERROR_MSG, QUESTION_MSG };

/*
 ****** Dados da janela de mensagens ****************************
 */
entry char	msg_text[256];		/* Texto da janela de mensagens */
entry int	msg_len;

entry char	com_text[256];		/* Texto da janela de comentários */
entry int	com_len;

entry char	err_text[256];		/* Texto da janela de erro */
entry int	err_len;

/*
 ****************************************************************
 *	Limpa a janela de mensagens				*
 ****************************************************************
 */
void
clear_msg_window (void)
{
	msg_len = 0; com_len = 0; err_len =0;

	XClearWindow (display, msg.win);

}	/* end clear_msg_window */

/*
 ****************************************************************
 *	Imprime uma mensagem normal				*
 ****************************************************************
 */
void
draw_msg_text (const char *format, ...)
{
	msg_len = vsnprintf (msg_text, sizeof (msg_text), format, va_first (format));

	draw_msg_win ();

}	/* end draw_msg_text */

/*
 ****************************************************************
 *	Imprime uma mensagem de comentário (2a. linha)		*
 ****************************************************************
 */
void
draw_com_text (const char *format, ...)
{
	com_len = vsnprintf (com_text, sizeof (com_text), format, va_first (format));

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
	char		err;

	if (err_len)
		return;

	if ((err = *format) == '*')
                format++;

	err_len = vsnprintf (err_text, sizeof (err_text), format, va_first (format));

	if (err == '*' && errno != 0)
	{
		err_len += snprintf
		(	err_text + err_len, sizeof (err_text) - err_len,
			" (%s)", strerror (errno)
		);
	}

	draw_msg_win ();

}	/* end draw_error_msg_text */

/*
 ****************************************************************
 *	Desenha a janela de mensagens				*
 ****************************************************************
 */
void
draw_msg_win (void)
{
	const WINDATA	*wp = &msg;
	int		font_x, font_y;

	draw_shadow
	(	wp->win,
		wp->triple.top_gc, wp->triple.bottom_gc,
		0, 0,
		wp->width, wp->height
	);

	if (msg_len == 0 && com_len == 0 && err_len == 0)
		return;

	font_x	= MENU_BORDER + SHADOW;
	font_y	= MENU_BORDER + 1 * (wp->font_height + 2 * SHADOW) - (wp->font_descent + SHADOW);

	if (msg_len) XDrawString
	(	display, wp->win,
		wp->text_gc,
		font_x, font_y,
		msg_text, msg_len
	);

	if (com_len) XDrawString
	(	display, wp->win,
		wp->text_gc,
		font_x, font_y + (wp->font_height + 2 * SHADOW),
		com_text, com_len
	);

	if (err_len) XDrawString
	(	display, wp->win,
		red_gc,
		font_x, font_y + 2 * (wp->font_height + 2 * SHADOW),
		err_text, err_len
	);

}	/* end draw_msg_win */
