/*
 ****************************************************************
 *								*
 *			deleteln.c				*
 *								*
 *	Remoção de uma linha de uma janela			*
 *								*
 *	Versão	2.0.0, de 25.04.88				*
 *		3.0.0, de 08.03.95				*
 *								*
 *	Módulo: deleteln					*
 *		libcurses/curses				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <stdio.h>
#include <string.h>
#include <curses.h>

#include "../../terminfo/h/defs.h"

/*
 ****************************************************************
 *	Remoção de uma linha					*
 ****************************************************************
 */
void
wdeleteln (register WINDOW *win)
{
	register int	l;
	register int	maxl;
	register int	curl;
	register chtype	*last;
	register int	b;

	b = check (win->w_flags, W_BOX) ? 1 : 0;
	maxl = win->w_maxl - b;
	curl = win->w_curl;

	/*
	 *	Desloca as linhas
	 */
	last = win->w_text[curl];

	for (l = curl; l < maxl; l++)
	{
		win->w_text[l] = win->w_text[l + 1];
		win->w_changed[l] = win->w_changed[l + 1];
	}

	win->w_text[maxl] = last;


	/*
	 *	Preenche a última linha com brancos
	 */
	memsetw ((short *)&win->w_text[maxl][b], ' ', win->w_maxc + 1 - 2 * b);
	win->w_changed[maxl] = 1;

	/*
	 *	Guarda informação
	 */
	if (win->w_dispcount == 0)
	{
		win->w_displine = curl;
		win->w_dispcount = -1;
	}
	elif (win->w_displine == curl)
	{
		win->w_dispcount--;
	}
	else
	{
		l = (curl < win->w_displine) ? curl : win->w_displine;

		while (l < maxl)
			win->w_changed[l++] = 1;

		win->w_dispcount = 0;
	}

	/*
	 *	Posição corrente no início da linha corrente
	 */
	win->w_curc = b;

}	/* end wdeleteln */
