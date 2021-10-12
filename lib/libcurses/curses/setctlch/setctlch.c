/*
 ****************************************************************
 *								*
 *			setctlch.c				*
 *								*
 *	Atribuição dos caracteres de controle			*
 *								*
 *	Versão	2.1.0, de 14.02.89				*
 *		3.0.0, de 20.03.95				*
 *								*
 *	Módulo: setctlch					*
 *		libcurses/curses				*
 *		Categoria B					*
 * 								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <termio.h>
#include <curses.h>
#include <terminfo.h>

#include "../../terminfo/h/defs.h"

#include "../h/term.h"

/*
 ****************************************************************
 *	Variáveis externas					*
 ****************************************************************
 */
extern	TERM	*_curterm;	/* Terminal correntemente em uso */

/*
 ****************************************************************
 *	Atribui o valor de um caractere de controle		*
 ****************************************************************
 */
void
setctlch (int ctlname, int ctlch)
{
	register TERMIO	*tp;

	tp = &_curterm->t_termio;

	switch (ctlname)
	{
	    case C_INTR:
		tp->t_intr = ctlch;
		break;

	    case C_QUIT:
		tp->t_quit = ctlch;
		break;

	    case C_SWITCH:
		tp->t_swtch = ctlch;
		break;

	    case C_ERASE:
		tp->t_erase = ctlch;
		break;

	    case C_AERASE:
		tp->t_aerase = ctlch;
		break;

	    case C_WORD:
		tp->t_word = ctlch;
		break;

	    case C_KILL:
		tp->t_kill = ctlch;
		break;

	    case C_RETYPE:
		tp->t_retype = ctlch;
		break;

	    case C_EOF:
		tp->t_eof = ctlch;
		break;

	}	/* end switch */

	ioctl (fileno (_curterm->t_infp), TCSETNR, tp);

}	/* end setctlch */
