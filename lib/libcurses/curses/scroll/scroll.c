/*
 ****************************************************************
 *								*
 *			scroll.c				*
 *								*
 *	Rolamento de uma janela					*
 *								*
 *	Versão	2.0.0, de 17.05.88				*
 *		3.0.0, de 20.03.95				*
 *								*
 *	Módulo: scroll						*
 *		libcurses/curses				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curses.h>

#include "../../terminfo/h/defs.h"

/*
 ****************************************************************
 *	Rolamento de uma Janela					*
 ****************************************************************
 */
void
wscroll (register WINDOW *win, register int n)
{
	register int	b;
	register int	l;
	register int	lines;
	register int	cols;
	register int	round;
	register chtype	**t;

	b = check (win->w_flags, W_BOX) ? 1 : 0;
	lines = win->w_maxl + 1;
	cols = win->w_maxc + 1 - 2 * b;

	/*
	 *	Muitos rolamentos equivalem a limpar toda a janela
	 */
	if (n > lines - 2 * b)
		{ werase (win); return; }

	/*
	 *	Salva ponteiros originais
	 */
	t = malloc ((win->w_maxl + 1) * sizeof (chtype *));

	for (l = win->w_maxl; l >= 0; l--)
		t[l] = win->w_text[l];

	/*
	 *	Deslocamento e preenchimento com brancos
	 */
	if (n > 0)
	{
		round = n - (lines - 2 * b);

		for (l = lines - n - b; l < lines - b; l++)
		{
			win->w_text[l] = t[l + round];
		}

		for (l = b; l < lines - b - n; l++)
		{
			win->w_text[l] = t[l + n];
			win->w_changed[l] = win->w_changed[l + n];
		}

		for (l = lines - b - n; l < lines - b; l++)
		{
			memsetw ((short *)&win->w_text[l][b], ' ', cols);
			win->w_changed[l] = 1;
		}

		/*
		 *	Guarda informação.
		 */
		if (win->w_dispcount == 0)
		{
			win->w_displine = b;
			win->w_dispcount = -n;
		}
		elif (win->w_displine == b)
		{
			win->w_dispcount -= n;
		}
		else
		{
			for (l = lines - b - 1 ; l >= b ; l--)
				win->w_changed[l] = 1;

			win->w_dispcount = 0;
		}
	}
	else
	{
		n = -n; round = - n + (lines - 2 * b);

		for (l = lines - 1 - b; l >= n + b; l--)
		{
			win->w_text[l] = t[l - n];
			win->w_changed[l] = win->w_changed[l - n];
		}

		for (l = n - 1 + b; l >= b; l--)
		{
			win->w_text[l] = t[l + round];
			memsetw ((short *)&win->w_text[l][b], ' ', cols);
			win->w_changed[l] = 1;
		}

		/*
		 *	Guarda a informação
		 */
		if (win->w_dispcount == 0)
		{
			win->w_displine = b;
			win->w_dispcount = n;
		}
		elif (win->w_displine == b)
		{
			win->w_dispcount += n;
		}
		else
		{
			for (l = lines - b - 1 ; l >= b ; l--)
				win->w_changed[l] = 1;

			win->w_dispcount = 0;
		}

		n = -n;
	}

	/*
	 *	Libera a área para a cópia dos ponteiros originais
	 */
	free (t);

	/*
	 *	Posição Corrente
	 */
	win->w_curl = (n > 0) ? win->w_maxl - b - n + 1 : b;
	win->w_curc = b;

}	/* end wscroll */
