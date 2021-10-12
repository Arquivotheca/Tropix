/*
 ****************************************************************
 *								*
 *			mvwin.c					*
 *								*
 *	Translação de uma janela				*
 *								*
 *	Versão	2.0 0, de 22.04.88				*
 *		3.0.0, de 10.03.95				*
 *								*
 *	Módulo: mvwin						*
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
#include <termio.h>
#include <curses.h>
#include <terminfo.h>

#include "../../terminfo/h/defs.h"

#include "../h/term.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Translação de uma janela				*
 ****************************************************************
 */
void
mvwin (register WINDOW *win, int y, int x)
{
	register WINDOW	*wq;
	register TERM	*term;
	register int	l;
	register int	b;
	int		minx, miny, maxx, maxy;

	/*
	 *	Verifica coordenadas da janela
	 */
	if (win->w_minx == x && win->w_miny == y)
		return;

	term = (TERM *)(win->w_term);

	/*
	 *	Atualiza coordenadas da janela
	 */
	win->w_minx = x;
	win->w_miny = y;

	/*
	 *	Atualiza mapeamento das janelas
	 */
	for (wq = term->t_last; wq != NOWINDOW; wq = wq->w_queue)
		_touchmap (wq);

	/*
	 *	Verifica cobertura horizontal
	 */
	if (win->w_minx <= 0 && win->w_minx + win->w_maxc >= term->t_cols - 1)
		set (win->w_flags, W_WIDE);

	/*
	 *	Acerta atributo W_TOP de todas as janelas
	 */
	for (win = term->t_last; win != NOWINDOW; win = win->w_queue)
		set (win->w_flags, W_TOP);

	for (win = term->t_last; win != NOWINDOW; win = win->w_queue)
	{
		b = (check (win->w_flags, W_BOX)) ? 1 : 0;

		minx = win->w_minx + b;
		miny = win->w_miny + b;
		maxx = minx + win->w_maxc - b;
		maxy = miny + win->w_maxl - b;

		for (wq = win->w_queue; wq != NOWINDOW; wq = wq->w_queue)
		{
			b = (check (wq->w_flags, W_BOX)) ? 1 : 0;

			if
			(
				wq->w_minx + b <= maxx &&
				wq->w_miny + b <= maxy &&
				wq->w_minx + wq->w_maxc - b >= minx &&
				wq->w_miny + wq->w_maxl - b >= miny
			)
			{
				reset (win->w_flags, W_TOP);
				break;
			}
		}
	}

	/*
	 *	Atualiza  o vídeo
	 */
	for (wq = term->t_last; wq != NOWINDOW; wq = wq->w_queue)
	{
		long	flags;

		/*
		 *	Com a exceção da janela em movimento,
		 *	não é necessário atualizar a posição do cursor
		 */
		flags = wq->w_flags;

#if (0)	/*************************************/
		if (wq != win)
			reset (wq->w_flags, R_CURSOR);
#endif	/*************************************/

		for (l = wq->w_maxl ; l >= 0 ; l--)
			wq->w_changed[l] = 1;

		wrefresh (wq);
		wq->w_flags = flags;
	}

}	/* end mvwin */
