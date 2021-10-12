/*
 ****************************************************************
 *								*
 *			erase.c					*
 *								*
 *	Preenche com espaços o texto de uma janela		*
 *								*
 *	Versão	2.0.0, de 02.05.88				*
 *		3.0.0, de 09.03.95				*
 *								*
 *	Módulo: erase						*
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
 *	Preenche com espaços o texto de uma janela		*
 ****************************************************************
 */
void
werase (register WINDOW *win)
{
	register int	width, b, l;
	register chtype	**text;

	b = check (win->w_flags, W_BOX) ? 1 : 0;
	width = win->w_maxc + 1 - 2 * b;
	text = win->w_text;

	/*
	 *	Escreve no texto e indica no vetor de alterações
	 */
	for (l = win->w_maxl - b ; l >= b ; l--)
	{
		memsetw ((short *)&text[l][b], ' ', width);
		win->w_changed[l] = 1;
	}

	win->w_dispcount = 0;

	/*
	 *	Posição Corrente
	 */
	win->w_curl = b;
	win->w_curc = b;

}	/* end werase */
