/*
 ****************************************************************
 *								*
 *			move.c					*
 *								*
 *	Move a posição corrente					*
 *								*
 *	Versão	2.0.0, de 17.08.88				*
 *		3.0.1, de 30.04.97				*
 *								*
 *	Módulo: EX/VI						*
 *		Editores de texto				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <curses.h>

#include "../h/global.h"
#include "../h/text.h"
#include "../h/visual.h"
#include "../h/obj.h"
#include "../h/char.h"
#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Executa comandos de janela				*
 ****************************************************************
 */
void	
vis_window (VIS *vp)
{
	BASE		*newbase = NOBASE;	/* CUIDADO */
	int		neworder = 0;		/* CUIDADO */

	if (vp->v_count == 0)
	{
		switch (vp->v_com)
		{
		    case CTLF:
		    case CTLB:
			vp->v_count = 1;
			break;

		    case CTLD:
		    case CTLU:
			vp->v_count = View->v_lines/2;
			break;
		}
	}

	switch (vp->v_com)
	{
	    case CTLF:
		newbase = txt_walk (View->v_base, vp->v_count * (View->v_lines - 1));
		pg_display (newbase);
		neworder = str_nonwhite (newbase->ch);
		break;

	    case CTLB:
		newbase = txt_walk (View->v_base, -vp->v_count * (View->v_lines - 1));
		pg_display (newbase);
		neworder = str_nonwhite (newbase->ch);
		break;

	    case CTLE:
		vp->v_count = 1;

	    case CTLD:
		pg_scroll (vp->v_count);

		if (pg_present(View->v_base) < 0)
		{
			newbase = View->v_last;
			neworder = str_nonwhite (View->v_last->ch);
		}
		else
		{
			newbase = View->v_base;
			neworder = View->v_curc;
		}

		break;

	    case CTLY:
		vp->v_count = 1;

	    case CTLU:
		pg_scroll (-vp->v_count);

		if (pg_present(View->v_base) < 0)
		{
			newbase = View->v_first;
			neworder = str_nonwhite (View->v_first->ch);
		}
		else
		{
			newbase = View->v_base;
			neworder = View->v_curc;
		}
		break;

	    case 'G':
	    case 'g':
		if (vp->v_count == 0)
			newbase = LAST ();
		else
			newbase = txt_advance (FIRST (), vp->v_count - 1);

		if (newbase == NOBASE)
			{ beep (); return; }

		neworder = str_nonwhite (newbase->ch);
		break;

	    case CTLL:
	    case CTLR:
		wrefon (View->v_win, R_CLEAR);
		newbase = View->v_base;
		neworder = View->v_curc;
		wrefresh (View->v_win);
		break;

	    default:
		sleep (1);
		wprintw (View->v_err, "Comando window estranho: %c", vp->v_com);

	}	/* switch vp->v_com */

	pg_move (newbase, neworder, true);

}	/* end exec_window */

/*
 ****************************************************************
 *	Executa "move"						*
 ****************************************************************
 */
void	
vis_move (VIS *vp)
{
	int		o;
	int		order;
	BASE		*base;

	if (vp->v_count == 0)
		vp->v_count = 1;

	o = object (&vp->v_obj, vp->v_count, false, false, &base, &order);

	if (o == OBJ_FLINE || o == OBJ_BLINE)
		order = str_nonwhite (base->ch);

	if (o != OBJ_ERROR)
		pg_move (base, order, true);

}	/* end exec_move */
