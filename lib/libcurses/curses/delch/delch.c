/*
 ****************************************************************
 *								*
 *			delch.c					*
 *								*
 *	Remoção de um caractere de uma janela			*
 *								*
 *	Versão	2.0.0, de 20.04.88				*
 *		3.0.0, de 08.03.95				*
 *								*
 *	Módulo: delch						*
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
 *	Remove um caractere de uma janela			*
 ****************************************************************
 */
void
wdelch (register WINDOW *win)
{
	register const chtype	*source;
	register chtype		*dest, *end;

	/*
	 *	Remove o caractere: 
	 *		os caracteres à esquerda são deslocados
	 *		para a direita de uma posição.
	 */
	source = &win->w_text[win->w_curl][win->w_curc + 1];

	if (win->w_flags & W_BOX)
		end = &win->w_text[win->w_curl][win->w_maxc - 1];
	else
		end = &win->w_text[win->w_curl][win->w_maxc];

	dest = &win->w_text[win->w_curl][win->w_curc];

	while (source <= end)
		*dest++ = *source++;

	/*
	 *	A última posição da linha corrente é preenchida com espaço
	 */
	*end = (chtype)' ';

	/*
	 *	Marca o vetor de alterações
	 */
	win->w_changed[win->w_curl] = 1;

}	/* end wdelch */
