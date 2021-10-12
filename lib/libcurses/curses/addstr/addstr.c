/*
 ****************************************************************
 *								*
 *			addstr.c				*
 *								*
 *	Escreve uma cadeia na posição corrente			*
 *								*
 *	Versão	2.0.0, de 14.04.88				*
 *		3.0.0, de 07.03.95				*
 *								*
 *	Módulo: addstr						*
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
 *	Escreve uma cadeia na posição corrente			*
 ****************************************************************
 */
int
waddstr (register WINDOW *win, register const char *str)
{
	/*
	 *	Verifica o modo
	 */
	if (check (win->w_flags, A_REAL))
	{
		/*
		 *	Modo real: escreve diretamente na janela
		 */
		register chtype	*lp, *endp;
		register chtype	attr;

		/*
		 *	Copia a cadeia
		 */
		attr = win->w_attr;

		lp = &win->w_text[win->w_curl][win->w_curc];
		endp = &win->w_text[win->w_curl][win->w_maxc];

		while (*str && lp <= endp)
			*lp++ = *str++ | attr;

		/*
		 *	Marca o final da cadeia
		 */
		if (lp < endp)
			*lp = 0;

		/*
		 *	Marca alteração nesta linha
		 */
		win->w_changed[win->w_curl] = 1;

		/*
		 *	Altera a posição corrente
		 */
		win->w_curc = lp - &win->w_text[win->w_curl][0];

		return (lp <= endp);
	}
	else
	{
		/*
		 *	Modo não real: chama "waddch"
		 */
		register int	success;

		success = OK;

		while (*str && success == OK)
		{
			success = waddch (win, (chtype)*str);
			str++;
		}

		return (success);
	}

}	/* end waddstr */
