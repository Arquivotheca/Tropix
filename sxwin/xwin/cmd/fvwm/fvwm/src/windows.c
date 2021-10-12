/*
 ****************************************************************
 *								*
 *			src/windows.c				*
 *								*
 *	Janelas							*
 *								*
 *	Versão	3.0.0, de 22.07.97				*
 *		4.4.0, de 20.03.03				*
 *								*
 *	Módulo: cmd/fvwm					*
 *		X Windows - Clientes				*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright (c) 2003 TROPIX Technologies Ltd.	*
 * 								*
 ****************************************************************
 */

/****************************************************************************
 * A little of it is borrowed from ctwm.
 * Copyright 1993 Robert Nation. No restrictions are placed on this code,
 * as long as the copyright notice is preserved
 ****************************************************************************/

#include "../global.h"

#include <stdio.h>
#include <signal.h>
#include <string.h>

#include "fvwm.h"
#include "menus.h"
#include "misc.h"
#include "parse.h"
#include "screen.h"

/* I tried to include "limits.h" to get these values, but it
 * didn't work for some reason */
/* Minimum and maximum values a `signed int' can hold.  */
#define MY_INT_MIN (- MY_INT_MAX - 1)
#define MY_INT_MAX 2147483647

/*
 * Change by PRB (pete@tecc.co.uk), 31/10/93.  Prepend a hot key
 * specifier to each item in the list.  This means allocating the
 * memory for each item (& freeing it) rather than just using the window
 * title directly.  */
void
do_windowList (XEvent *eventp, Window w, FVWMWIN *tmp_win,
		ulong context, char *action, int *Module)
{
	MenuRoot	*mr;
	FVWMWIN	*t;
	char		*tname;
	char		loc[40], *name = NULL;
	int		dwidth, dheight;
	char		tlabel[50];
	int		last_desk_done = MY_INT_MIN;
	int		next_desk;
	int		val1, val2, val1_unit, val2_unit, n;
	char		*t_hot;			/* Menu label with hotkey added */
	char		scut = '0';		/* Current short cut key */
	
	n = GetTwoArguments (action, &val1, &val2, &val1_unit, &val2_unit);

	sprintf (tlabel, "Desk corrente: %d\tGeometria", Scr.CurrentDesk);
	mr = NewMenuRoot (tlabel, 0);
	AddToMenu (mr, tlabel, "TITLE");      

	next_desk = 0;
	while (next_desk != MY_INT_MAX)
	{
		/* Sort window list by desktop number */
		if (val1 < 2 && val1 > -2)
		{
			next_desk = MY_INT_MAX;
			for (t = Scr.FvwmRoot.next; t != NULL; t = t->next)
			{
				if
				(	(t->Desk > last_desk_done) &&
					(t->Desk < next_desk)
				)
					next_desk = t->Desk;
			}
		}
		else if (val1 < 4 && val1 > -4)
		{
			if (last_desk_done  == MY_INT_MIN)
				next_desk = Scr.CurrentDesk;
			else
				next_desk = MY_INT_MAX;
		}
		else 
		{
			if (last_desk_done  == MY_INT_MIN)
				next_desk = val2;
			else
				next_desk = MY_INT_MAX;
		}

		last_desk_done = next_desk;
		for (t = Scr.FvwmRoot.next; t != NULL; t = t->next)
		{
			if
			(	t->Desk == next_desk &&
				(t->flags & WINDOWLISTSKIP) == 0
			)
			{
				if (++scut == ('9' + 1))
					scut = 'A';	/* Next shortcut key */

				if (val1 % 2 != 0)
					name = t->icon_name;
				else
					name = t->name;

				t_hot = safemalloc (strlen (name) + 48);
				sprintf (t_hot, "&%c.  %s", scut, name); /* Generate label */
	      
				tname = safemalloc(40);
				tname[0] = 0;

				if(t->flags & ICONIFIED)
					strcpy (tname, "(");

				sprintf (loc, "%d:", t->Desk);
				strcat (tname, loc);

				if (t->frame_x >= 0)
					sprintf (loc, "+%d", t->frame_x);
				else
					sprintf (loc, "%d", t->frame_x);

				strcat (tname, loc);

				if (t->frame_y >=0)
					sprintf (loc, "+%d", t->frame_y);
				else
					sprintf (loc, "%d", t->frame_y);

				strcat (tname, loc);

				dheight = t->frame_height - t->title_height - 2*t->boundary_width;
				dwidth = t->frame_width - 2*t->boundary_width;
	      
				dwidth -= t->hints.base_width;
				dheight -= t->hints.base_height;
	      
				dwidth /= t->hints.width_inc;
				dheight /= t->hints.height_inc;

				sprintf (loc, "x%d", dwidth);
				strcat (tname, loc);
				sprintf (loc, "x%d", dheight);
				strcat (tname, loc);

				if (t->flags & ICONIFIED)
					strcat (tname, ")");

				sprintf (tlabel, "RAISE_IT %ld %ld", t, t->w);
				strcat (t_hot, "\t");
				strcat (t_hot, tname);

				AddToMenu (mr, t_hot, tlabel);

				free (t_hot);
				free (tname);
			}
		}
	}

	MakeMenu (mr);

  /* If the menu is a result of a ButtonPress, then tell do_menu()
     to expect (and ignore) a button release event. Otherwise, it was
     as a result of a keypress or something, so we shouldn't expect
     a button release event. Fixes problem with keyboard short cuts not
     working if window list is popped up by keyboard.
         and1000@cam.ac.uk, 27/6/96 */

	if (eventp->type == ButtonPress)
		do_menu (mr,1);
	else
		do_menu (mr,0);

	DestroyMenu (mr);

}	/* end do_windowList */
