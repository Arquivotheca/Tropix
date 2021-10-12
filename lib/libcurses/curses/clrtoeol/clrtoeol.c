/*
 ****************************************************************
 *								*
 *			clrtoeol.c				*
 *								*
 *	Escreve espaços até o final da linha ou janela		*
 *								*
 *	Versão	2.0.0, de 20.04.88				*
 *		3.0.0, de 08.03.95				*
 *								*
 *	Módulo: clrtoeol					*
 *		libcurses/curses				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <stdio.h>
#include <curses.h>

#include "../../terminfo/h/defs.h"

/*
 ****************************************************************
 *	Escreve espaços até o final da janela			*
 ****************************************************************
 */
void
wclrtobot (register WINDOW *win)
{
	register chtype	*cp, *endp;
	register int	l, c;
	register int	b;

	/*
	 *	Inicializações
	 */
	b = check (win->w_flags, W_BOX) ? 1: 0;
	l = win->w_curl;
	c = win->w_curc;

	/*
	 *	Escreve brancos e marca o vetor de alterações
	 */
	while (l <= win->w_maxl - b)
	{
		cp = &win->w_text[l][c];
		endp = &win->w_text[l][win->w_maxc - b];

		while (cp <= endp)
			*cp++ = ' ';

		win->w_changed[l] = 1;

		l++; c = b;
	}

}	/* end wclrtobot */

/*
 ****************************************************************
 *	Escreve espaços até o final da linha corrente		*
 ****************************************************************
 */
void
wclrtoeol (register WINDOW *win)
{
	register chtype	*cp, *endp;
	register int	b;

	/*
	 *	Pontos inicial e final
	 */
	b = check (win->w_flags, W_BOX) ? 1: 0;
	cp = &win->w_text[win->w_curl][win->w_curc];
	endp = &win->w_text[win->w_curl][win->w_maxc - b];

	/*
	 *	Escreve espaços
	 */
	while (cp <= endp)
		*cp++ = ' ';

	/*
	 *	Marca o vetor de alterações
	 */
	win->w_changed[win->w_curl] = 1;

}	/* end wclrtoeol */
