/*
 ****************************************************************
 *								*
 *			addch.c					*
 *								*
 *	Escreve um caractere na posição corrente		*
 *								*
 *	Versão	2.0.0, de 10.05.88				*
 *		3.0.0, de 07.03.95				*
 *								*
 *	Módulo: addch						*
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
#include <termio.h>
#include <ctype.h>
#include <curses.h>
#include <terminfo.h>

#include "../../terminfo/h/defs.h"
#include "../h/term.h"

/*
 ****************************************************************
 *	Definições locais					*
 ****************************************************************
 */
#define	DEL	0xFF

/*
 ****************************************************************
 *	Escreve um caractere na posição corrente		*
 ****************************************************************
 */
int
waddch (register WINDOW *win, chtype ch)
{
	register chtype	chtext;
	register short	b;
	chtype		chattr;
	TERM		*term;

	/*
	 *	Verifica a posição corrente
	 */
	b = check (win->w_flags, W_BOX) ? 1 : 0;

	if (win->w_curc > win->w_maxc - b || win->w_curl > win->w_maxl - b)
		return (ERR);

	/*
	 *	Separa os atributos do caractere
	 */
	ch |= win->w_attr;
	chtext = ch & A_CHARTEXT;
	chattr = ch & A_ATTRIBUTES;

	/*
	 *	Analisa o caso
	 */
	if (isgraph (chtext) || chtext == ' ')
	{
		/*
		 *	Caractere comum
		 */
		win->w_text[win->w_curl][win->w_curc] = ch;
		win->w_changed[win->w_curl] = 1;
	}
	elif (chtext == '\n')
	{
		/*
		 *	New line: limpa até o fim da linha
		 */
		wclrtoeol (win);
		goto nextline;
	}
	elif (check (win->w_flags, A_REAL))
	{
		/*
		 *	Modo Real
		 */
		win->w_text[win->w_curl][win->w_curc] = ch;
		win->w_changed[win->w_curl] = 1;
	}
	elif (chtext == '\t')
	{
		/*
		 *	Tabulação: escreve espaços
		 */
		register short	c;
		register short	t;

		t = ((win->w_curc - b) & (~7)) + 8 + b;

		if (t <= win->w_maxc - b)
		{
			for (c = win->w_curc ; c < t ; c++)
				win->w_text[win->w_curl][c] = ' ' | chattr;

			win->w_curc = t;

			win->w_changed[win->w_curl] = 1;
			return (OK);
		}
		else
		{
			t = win->w_maxc - b;

			for (c = win->w_curc ; c <= t ; c++)
				win->w_text[win->w_curl][c] = ' ' | chattr;

			win->w_changed[win->w_curl] = 1;
			win->w_curc = win->w_maxc - b;
		}
	}
	elif (chtext == '\b')
	{
		/*
		 *	Back Space: Retorna um Espaço
		 */
		if (win->w_curc > b)
		{
			win->w_curc--;
		}
		elif (win->w_curl > b)
		{
			win->w_curc = win->w_maxc - b;
			win->w_curl--;
		}
		else
		{
			return (ERR);
		}

		return (OK);
	}
	else
	{
		/*
		 *	Caracteres de Controle: escreve na forma "^X"
		 */
		if (waddch (win, '^' | chattr) == ERR)
			return (ERR);

		chtext = (chtext == DEL) ? '?' : chtext + '@';

		if (waddch (win, (chtext) | chattr) == ERR)
			return (ERR);

		return (OK);
	}

	/*
	 *	Incrementa a posição corrente.
	 *	Se a janela tem contorno, respeitamo-lo
	 */
	if (win->w_curc < win->w_maxc - b)
	{
		/*
		 *	Basta incrementar a coluna
		 */
		win->w_curc++;
	}
	else
	{
		/*
		 *	Última coluna: verifica "wrapping"
		 */
		if (!check (win->w_flags, A_WRAP))
			return (ERR);

		/*
		 *	Paginação
		 */
	    nextline:
		if (check (win->w_flags, A_PAGE))
		{
			if (win->w_newlines >= win->w_maxl)
			{
				term = (TERM *)(win->w_term);
				wrefresh (win);
				fflush (term->t_outfp);

				if (getc (term->t_infp) == term->t_termio.t_eol)
					win->w_newlines = -1;
				else
					win->w_newlines = 0;
			}
			elif (win->w_newlines >= 0)
			{
				win->w_newlines++;
			}
		}
		else
		{
			win->w_newlines = 0;
		}

		/*
		 *	Analisa o avanço
		 */
		if (win->w_curl < win->w_maxl - b)
		{
			/*
			 *	Avança para a próxima linha
			 */
			win->w_curl++;
			win->w_curc = b;
		}
		elif (check (win->w_flags, A_SCROLL))
		{
			/*
			 *	Avança rolando a janela
			 */
			wscroll (win, 1);
			win->w_curc = b;
		}
	}

	return (OK);

}	/* end waddch */
