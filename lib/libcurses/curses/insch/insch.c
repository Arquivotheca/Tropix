/*
 ****************************************************************
 *								*
 *			insch.c					*
 *								*
 *	Insere um caractere em uma janela			*
 *								*
 *	Versão	2.0.0, de 20.04.88				*
 *		3.0.0, de 10.03.95				*
 *								*
 *	Módulo: insch						*
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
#include <ctype.h>
#include <curses.h>

#include "../../terminfo/h/defs.h"

/*
 ****************************************************************
 *	Insere um caractere em uma janela			*
 ****************************************************************
 */
int
winsch (register WINDOW	*win, register chtype ch)
{
	register chtype	*source, *first, *dest;
	register chtype	chtext, chattr;
	register int	b;
	chtype		*rest = (chtype *)NULL;
	int		size = 0;

	chtext = ch & A_CHARTEXT; chattr = ch & A_ATTRIBUTES;

	b = check (win->w_flags, W_BOX) ? 1 : 0;

	/*
	 *	Processamento de caracteres especiais
	 *	------------- -- ---------- ---------
	 */
	if (!check (win->w_flags, A_REAL))
	{
		/*
		 *	Fora do modo real
		 */
		if (chtext == '\t')
		{
			/*
			 *	Tabulação: insere espaços
			 */
			register int	i, e;

			i = win->w_curc;

			for (e = ((i + b) & (~7)) + 8 + b ; i < e ; i++)
			{
				if (winsch (win, ' ' | chattr) == ERR)
					return (ERR);
			}

			return (OK);
		}
		elif (!isgraph (chtext) && chtext != ' ' && chtext != '\n')
		{
			/*
			 *	Caracteres de Controle: escreve na forma "^X"
			 */
			if (winsch (win, '^' | chattr) == ERR)
				return (ERR);

			if (winsch (win, (chtext + '@') | chattr) == ERR)
				return (ERR);

			return (OK);
		}
	}

	/*
	 *	Final de linha
	 */
	if (chtext == '\n')
	{
		/*
		 *	Guarda o restante da linha
		 */
		size = (win->w_maxc - win->w_curc + 1 - b) * sizeof (chtype);
		rest = malloc (size);

		memmove (rest, &win->w_text[win->w_curl][win->w_curc], size);

		/*
		 *	Limpa o restante da linha
		 */
		wclrtoeol (win);
			
		goto nextline;
	}

	/*
	 *	Processamento de caracteres normais:
	 *	------------- -- ---------- -------
	 *	Verifica a posição corrente
	 */
	if (win->w_curc >= win->w_maxc - b || win->w_curl > win->w_maxl - b)
		return (ERR);

	/*
	 *	Abre espaço para o caractere: 
	 *		os caracteres à direita são deslocados
	 *		para a direita de uma posição.
	 */
	first = &win->w_text[win->w_curl][win->w_curc];
	source = &win->w_text[win->w_curl][win->w_maxc - 1 - b];
	dest = source + 1;

	while (source >= first)
		*dest-- = *source--;

	/*
	 *	Copia o caractere para a posição corrente.
	 */
	win->w_text[win->w_curl][win->w_curc] = ch;

	/*
	 *	Marca o vetor de alterações
	 */
	win->w_changed[win->w_curl] = 1;

	/*
	 *	Incrementa a posição corrente.
	 *	Se a janela tem contorno, respeitamo-lo
	 */
	if (win->w_curc < win->w_maxc - b)
	{
		/*
		 *	Basta incrementar a Coluna
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
	    nextline:
		if (win->w_curl < win->w_maxl - b)
		{
			/*
			 *	Avança criando a próxima linha
			 */
			win->w_curl++;
			win->w_curc = b;

			winsertln (win);
		}
		elif (win->w_flags & A_SCROLL)
		{
			/*
			 *	Avança rolando a janela
			 */
			wscroll (win, 1);
			win->w_curc = b;
		}

		/*
		 *	Copia o restante da linha anterior
		 */
		if (rest != (chtype *)NULL)
		{
			memmove (&win->w_text[win->w_curl][win->w_curc], rest, size);
			free (rest);
		}
	}

	return (OK);

}	/* end winsch */
