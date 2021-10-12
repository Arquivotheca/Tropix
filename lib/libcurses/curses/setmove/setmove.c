/*
 ****************************************************************
 *								*
 *			setmove.c				*
 *								*
 *	Atribui a função de "move"				*
 *								*
 *	Versão	2.2.0, de 31.08.89				*
 *		3.0.0, de 20.03.95				*
 *								*
 *	Módulo: setmove						*
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

/*
 ****************************************************************
 *	Atribui a função de "move"				*
 ****************************************************************
 */
void
setmove (WINDOW *win, void (*func) ())
{
	win->w_move = func;

}	/* end setmove */
