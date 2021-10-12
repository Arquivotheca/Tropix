/*
 ****************************************************************
 *								*
 *			newwin.c				*
 *								*
 *	Criação de janelas					*
 *								*
 *	Versão	2.0.0, de 22.04.88				*
 *		3.0.0, de 05.03.95				*
 *								*
 *	Módulo: newwin						*
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
#include <stdlib.h>
#include <termio.h>
#include <curses.h>
#include <terminfo.h>

#include "../../terminfo/h/defs.h"

#include "../h/term.h"
#include "../h/local.h"

/*
 ****************************************************************
 *	Criação de uma nova Janela				*
 ****************************************************************
 */
WINDOW	*
newwin (int lines, int cols, int bline, int bcol)
{
	register WINDOW	*win;
	register chtype	*text;
	register int	l;

	/*
	 *	Aloca uma Janela Disponível em _windows
	 */
	for (win = _windows; /* vazio */; win++)
	{
		if (win >= &_windows[_NWINDOWS])
			return (NOWINDOW);

		if (win->w_flags == W_NOTHING)
			break;
	}

	/*
	 *	Aloca memória para o Texto
	 */
	if ((win->w_text = malloc (lines * sizeof (chtype *))) == (chtype **)NULL)
		return (NOWINDOW);

	if ((text = malloc (lines * cols * sizeof (chtype))) == (chtype *)NULL)
		{ free (win->w_text); return (NOWINDOW); }

	for (l = 0; l < lines; l++)
		win->w_text[l] = text + l * cols;

	/*
	 *	Aloca o vetor de alterações
	 */
	if ((win->w_changed = malloc (lines)) == NOSTR)
		{ free (text); free (win->w_text); return (NOWINDOW); }

	/*
	 *	Controle de Entrada e Saída
	 */
	waddset (win, A_SCROLL|A_WRAP);
	wgetset (win, G_TONL|G_ECHO|G_EDIT|G_IGXONXOFF);
	wrefset (win, R_CURSOR);

	if ((_curterm->t_flags & T_ISOKEY) == 0)
		wgeton (win, G_TOISO);

	win->w_attr = (chtype)0;

	/*
	 *	Dimensões da janela e posição corrente
	 */
	win->w_minx = bcol;
	win->w_miny = bline;
	win->w_maxc = cols - 1;
	win->w_maxl = lines - 1;
	win->w_curc = 0;
	win->w_curl = 0;

	/*
	 *	Verifica cobertura horizontal
	 */
	if (bcol <= 0 && bcol + cols >= _curterm->t_cols)
		set (win->w_flags, W_WIDE);

	/*
	 *	Inicializa o restante da estrutura
	 */
	win->w_inqueue = NOSTR;
	win->w_term = (char *)_curterm;
	win->w_dispcount = 0;

	/*
	 *	Inicializa Texto
	 */
	werase (win);

	return (win);

}	/* end newwin */
