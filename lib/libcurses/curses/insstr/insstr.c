/*
 ****************************************************************
 *								*
 *			insstr.c				*
 *								*
 *	Insere uma cadeia em uma janela				*
 *								*
 *	Versão	2.0.0, de 16.03.88				*
 *		3.0.0, de 10.03.95				*
 *								*
 *	Módulo: insstr						*
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
 *	Insere uma cadeia em uma janela				*
 ****************************************************************
 */
int
winsstr (register WINDOW *win, register const char *str)
{
	register int	sucess = OK;

	while (*str)
	{
		sucess = winsch (win, (chtype)*str);
		str++;
	}

	return (sucess);

}	/* end winsstr */
