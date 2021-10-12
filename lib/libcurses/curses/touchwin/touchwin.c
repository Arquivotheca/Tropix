/*
 ****************************************************************
 *								*
 *			touchwin.c				*
 *								*
 *	Prioridade de Visibilidade				*
 *								*
 *	Versão	2.0.0, de 17.05.88				*
 *		3.0.0, de 05.03.95				*
 *								*
 *	Módulo: touchwin					*
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
#include <string.h>
#include <termio.h>
#include <curses.h>
#include <terminfo.h>

#include "../../terminfo/h/defs.h"

#include "../h/term.h"
#include "../h/local.h"

/*
 ******	Protótipos de funções ***********************************
 */
#undef	DEBUG
#ifdef	DEBUG
static void	prqueue (TERM *, char *);
#endif	DEBUG

/*
 ****************************************************************
 *	Atualiza mapeamento das janelas (Função interna)	*
 ****************************************************************
 */
void
_touchmap (register WINDOW *win)
{
	register int	y;
	register int	miny;
	register int	minx;
	register char	**map;
	register int	w;
	register int	width;
	int		maxy, maxx;

	/*
	 *	Inicializações
	 */
	map = ((TERM *)(win->w_term))->t_map;
	maxx = ((TERM *)(win->w_term))->t_cols - 1;
	maxy = ((TERM *)(win->w_term))->t_lines - 1;
	miny = win->w_miny;
	minx = win->w_minx;
	w = win - _windows;

	/*
	 *	Verifica os argumentos
	 */
	if (miny + win->w_maxl < 0 || miny > maxy)
		return;

	if (minx + win->w_maxc < 0 || minx > maxx)
		return;

	/*
	 *	Limites
	 */
	if (maxy > miny + win->w_maxl)
		maxy = miny + win->w_maxl;

	if (maxx > minx + win->w_maxc)
		maxx = minx + win->w_maxc;

	if (miny < 0)
		miny = 0;
	if (minx < 0)
		minx = 0;

	/*
	 *	Realiza o mapeamento
	 */
	width = maxx - minx + 1;

	for (y = maxy; y >= miny; y--)
		memset (&map[y][minx], w, width);

}	/* end _touchmap */

/*
 ****************************************************************
 *	Máxima prioridade de visibilidade			*
 ****************************************************************
 */
void
touchwin (register WINDOW *win)
{
	register WINDOW	*wq;
	register TERM	*term;
	register int	y;
	register int	b;
	int		minx, miny, maxx, maxy;

	term = (TERM *)(win->w_term);

#ifdef	DEBUG
fprintf (stderr, "\ntouchwin: %04X\n", win);
prqueue (term, "Início: ");
#endif	DEBUG

	/*
	 *	Retira a janela da fila
	 */
	if ((wq = term->t_last) == win)
		term->t_last = win->w_queue;

	while (wq->w_queue != NOWINDOW)
	{
		if (wq->w_queue == win)
			wq->w_queue = win->w_queue;
		else
			wq = wq->w_queue;
	}

#ifdef	DEBUG
prqueue (term, "Retirada: ");
fprintf (stderr, "stdwin: %04X, wq: %04X\n", stdwin, wq);
#endif	DEBUG

	/*
	 *	Põe a janela no início da fila (última posição)
	 */
	if (term->t_last == NOWINDOW)
	{
		/*
		 *	Fila vazia
		 */
		term->t_last = win;
	}
	else
	{
		wq->w_queue = win;
		win->w_queue = NOWINDOW;
	}
	term->t_first = win;

#ifdef	DEBUG
prqueue (term, "Fim: ");
#endif	DEBUG

	/*
	 *	Atualiza o mapeamento de janelas
	 */
	_touchmap (win);

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
	 *	Atualiza o Vídeo: só é necessário atualizar a própria janela
	 */
	win = term->t_first;

	for (y = win->w_maxl; y >= 0; y--)
		win->w_changed[y] = 1;

	wrefresh (win);

}	/* end touchwin */

/*
 ****************************************************************
 *	Mínima prioridade de visibilidade			*
 ****************************************************************
 */
void
untouchwin (register WINDOW *win)
{
	register WINDOW	*wq;
	register TERM	*term;
	register int	y;
	register int	b;
	int		minx, miny, maxx, maxy;

	term = (TERM *)(win->w_term);

	/*
	 *	Verifica se a janela já não está no fim da fila
	 */
	if ((wq = term->t_last) != win)
	{
		/*
		 *	Retira a janela da fila
		 */
		while (wq->w_queue != NOWINDOW)
		{
			if (wq->w_queue == win)
				wq->w_queue = win->w_queue;
			else
				wq = wq->w_queue;
		}

		term->t_first = wq;
		
		/*
		 *	Põe a janela no fim da fila (primeira posição)
		 */
		wq = term->t_last;
		term->t_last = win;
		win->w_queue = wq;

		/*
		 *	Atualiza o mapeamento de janelas
		 */
		for (wq = win; wq != NOWINDOW; wq = wq->w_queue)
			_touchmap (wq);
	}

	/*
	 *	Acerta o atributo W_TOP de todas as janelas
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
	 *	Atualiza o vídeo
	 */
	for (wq = term->t_last; wq != NOWINDOW; wq = wq->w_queue)
	{
		for (y = wq->w_maxl; y >= 0; y--)
			wq->w_changed[y] = 1;

		wrefresh (wq);
	}

}	/* end untouchwin */

#ifdef	DEBUG
/*
 ****************************************************************
 *	x					*
 ****************************************************************
 */
static void
prqueue (TERM *term, char *message)
{
	register WINDOW	*wq;

	fprintf (stderr, "%-10s", message);

	fprintf (stderr, "Fila: %04X ", term->t_last);

	for (wq = term->t_last; wq != NOWINDOW; wq = wq->w_queue)
	{
		fprintf (stderr, "-> %04X ", wq->w_queue);
	}
	putc ('\n', stderr);

	fflush (stderr);

}	/* end prqueue */
#endif	DEBUG
