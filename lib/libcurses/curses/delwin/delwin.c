/*
 ****************************************************************
 *								*
 *			delwin.c				*
 *								*
 *	Remoção de uma janela					*
 *								*
 *	Versão	2.0.0, de 22.05.87				*
 *		3.0.0, de 08.03.95				*
 *								*
 *	Módulo: delwin						*
 *		libcurses/curses				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <termio.h>
#include <curses.h>
#include <terminfo.h>

#include "../../terminfo/h/defs.h"

#include "../h/term.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Remove uma janela					*
 ****************************************************************
 */
void
delwin (register WINDOW *win)
{
	register WINDOW	*wq;
	register TERM	*term;
	register int	y;

	term = (TERM *)(win->w_term);

	/*
	 *	Não é permitido remover a janela padrão
	 */
	if (win == stdwin)
		return;

	/*
	 *	Retira da fila de visibilidade
	 */
	if ((wq = term->t_last) == win)
	{
		term->t_last = win->w_queue;
	}
	else
	{
		while (wq->w_queue != NOWINDOW)
		{
			if (wq->w_queue == win)
			{
				wq->w_queue = win->w_queue;
			}
			else
			{
				wq = wq->w_queue;
			}
		}
	}

	/*
	 *	Atualiza o mapeamento das janelas
	 */
	for ((wq = term->t_last); wq != NOWINDOW; wq = wq->w_queue)
		_touchmap (wq);

	/*
	 *	Atualiza o vídeo
	 */
	for ((wq = term->t_last); wq != NOWINDOW; wq = wq->w_queue)
	{
		for (y = wq->w_maxl ; y >= 0 ; y--)
			wq->w_changed[y] = 1;

		wrefresh (wq);
	}

	/*
	 *	Libera a memória
	 */
	free (win->w_text);
	free (win->w_changed);

	/*
	 *	Libera estrutura WINDOW
	 */
	win->w_flags = W_NOTHING;

}	/* end delwin */
