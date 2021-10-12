/*
 ****************************************************************
 *								*
 *			beep.c					*
 *								*
 *	Emite um sinal sonoro					*
 *								*
 *	Versão	2.0.0, de 16.03.88				*
 *		3.0.0, de 08.03.95				*
 *								*
 *	Módulo: beep 						*
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
#include <curses.h>
#include <terminfo.h>

#include "../../terminfo/h/defs.h"

#include "../h/term.h"

/*
 ****************************************************************
 *	Emite um sinal sonoro					*
 ****************************************************************
 */
void
beep (void)
{
	register FILE		*fp;
	register const  char	*bell;

	fp = _curterm->t_outfp;

	if ((bell = _curterm->t_info.i_strings[s_bel]) != NOSTR)
		{ fputs (bell, fp); fflush (fp); }

}	/* end beep */
