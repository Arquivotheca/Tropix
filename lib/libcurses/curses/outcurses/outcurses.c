/*
 ****************************************************************
 *								*
 *			outcurses.c				*
 *								*
 *	Encerra o modo "curses" para o terminal corrente	*
 *								*
 *	Versão	2.0.0, de 10.05.88				*
 *		3.0.0, de 05.03.95				*
 *								*
 *	Módulo: outcurses					*
 *		libcurses/curses				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <termio.h>
#include <curses.h>
#include <terminfo.h>

#include "../../terminfo/h/defs.h"

#include "../h/term.h"
#include "../h/local.h"

/*
 ****************************************************************
 *	Encerra o modo "curses" para o terminal corrente	*
 ****************************************************************
 */
void
outcurses (void)
{
	register TERM	*term;

	term = _curterm;

	ioctl (fileno (term->t_infp), TCSETNR, &term->t_oldtermio);

	memmove (&term->t_termio, &term->t_oldtermio, sizeof (TERMIO));

	fputs (parmexec (term->t_info.i_strings[s_cup], LINES - 1, 0), term->t_outfp);
	putc ('\n', term->t_outfp);
	fflush (term->t_outfp);

	/*
	 *	Entrada não bloqueada
	 */
	if (check (term->t_oflags, O_NDELAY))
	{
		reset (term->t_oflags, O_NDELAY);
		fcntl (fileno (term->t_infp), F_SETFL, term->t_oflags);
	}

	set (term->t_flags, T_OUT);

}	/* end outcurses */

#if (0)	/*******************************************************/
/*
 ****************************************************************
 *	Desaloca todos os recursos da "curses"			*
 ****************************************************************
 */
void
freecurses (void)
{
	TERM		*tp, *next_tp;
	WINDOW		*wp;

	for (tp = _firstterm; tp != NOTERM; tp = next_tp)
	{
		if (wp = tp->t_stdwin)
		{
			if (wp->w_changed)
				free (wp->w_changed);

			if (wp->w_text)
			{
				if (wp->w_text[0])
					free (wp->w_text[0]);

				free (wp->w_text);
			}
		}

		if (tp->t_screen)
		{
			if (tp->t_screen[0])
				free (tp->t_screen[0]);

			free (tp->t_screen);
		}

		if (tp->t_map)
		{
			if (tp->t_map[0])
				free (tp->t_map[0]);

			free (tp->t_map);
		}

		if (tp->t_keys)
			free (tp->t_keys);

		next_tp = tp->t_list; free (tp);
	}

	if (_windows != NULL)
		{ free (_windows); _windows = NULL; }

	_curterm = _firstterm = NULL;

}	/* end freecurses */
#endif	/*******************************************************/
