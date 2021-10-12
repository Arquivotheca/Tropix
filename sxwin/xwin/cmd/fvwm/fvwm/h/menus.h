/*
 ****************************************************************
 *								*
 *			h/menus.h				*
 *								*
 *	Definições acerca de menus				*
 *								*
 *	Versão	3.0.0, de 16.11.97				*
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
 * This module is based on Twm, but has been siginificantly modified 
 * by Rob Nation 
 ****************************************************************************/
/*****************************************************************************/
/**       Copyright 1988 by Evans & Sutherland Computer Corporation,        **/
/**                          Salt Lake City, Utah                           **/
/**  Portions Copyright 1989 by the Massachusetts Institute of Technology   **/
/**                        Cambridge, Massachusetts                         **/
/**                                                                         **/
/**                           All Rights Reserved                           **/
/**                                                                         **/
/**    Permission to use, copy, modify, and distribute this software and    **/
/**    its documentation  for  any  purpose  and  without  fee is hereby    **/
/**    granted, provided that the above copyright notice appear  in  all    **/
/**    copies and that both  that  copyright  notice  and  this  permis-    **/
/**    sion  notice appear in supporting  documentation,  and  that  the    **/
/**    names of Evans & Sutherland and M.I.T. not be used in advertising    **/
/**    in publicity pertaining to distribution of the  software  without    **/
/**    specific, written prior permission.                                  **/
/**                                                                         **/
/**    EVANS & SUTHERLAND AND M.I.T. DISCLAIM ALL WARRANTIES WITH REGARD    **/
/**    TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES  OF  MERCHANT-    **/
/**    ABILITY  AND  FITNESS,  IN  NO  EVENT SHALL EVANS & SUTHERLAND OR    **/
/**    M.I.T. BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL  DAM-    **/
/**    AGES OR  ANY DAMAGES WHATSOEVER  RESULTING FROM LOSS OF USE, DATA    **/
/**    OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER    **/
/**    TORTIOUS ACTION, ARISING OUT OF OR IN  CONNECTION  WITH  THE  USE    **/
/**    OR PERFORMANCE OF THIS SOFTWARE.                                     **/
/*****************************************************************************/


/***********************************************************************
 *
 * $XConsortium: menus.h,v 1.24 89/12/10 17:46:26 jim Exp $
 *
 * twm menus include file
 *
 * 17-Nov-87 Thomas E. LaStrange		File created
 *
 ***********************************************************************/

#ifndef _MENUS_
#define _MENUS_

/* Function types used for formatting menus */

#define FUNC_NO_WINDOW 0
#define FUNC_NEEDS_WINDOW 0
#define FUNC_POPUP 1
#define FUNC_TITLE 2
#define FUNC_NOP 3

#include "../libs/fvwmlib.h"

typedef struct menuitem	MenuItem;

struct menuitem
{
	MenuItem	*next,		/* next menu item */
			*prev;		/* prev menu item */

	char		*item,		/* the character string displayed on left*/
			*item2,	        /* the character string displayed on right*/
			*action;	/* action to be performed */

	Picture		*picture,	/* Pixmap to show  above label*/
			*lpicture;	/* Pixmap to show to left of label */

	int		item_num,	/* item number of this menu */
			x,		/* x coordinate for text (item) */
			x2,		/* x coordinate for text (item2) */
			xp,		/* x coordinate for picture */
			y_offset,	/* y coordinate for item */
			y_height,	/* y height for item */
			func_type,	/* type of built in function */
			state,		/* video state, 0 = normal, 1 = reversed */
			strlen,		/* strlen(item) */
			strlen2,	/* strlen(item2) */
			hotkey;		/* Hot key offset (pete@tecc.co.uk).
					0 - No hot key
					+ve - offset to hot key char in item
					-ve - offset to hot key char in item2
					(offsets have 1 added, so +1 or -1
					refer to the *first* character) */
};

typedef struct menuroot MenuRoot;

struct menuroot
{
	MenuItem	*first,		/* first item in menu */
			*last;		/* last item in menu */

	MenuRoot	*next;		/* next in list of root menus */

	char		*name;		/* name of root */
	Window		w;		/* the window of the menu */

	int		height,		/* height of the menu */
			width,		/* width of the menu for 1st col */
			width2,		/* width of the menu for 2nd col */
			width0,		/* width of the menu-left-picture col */
			items,		/* number of items in the menu */
			func;

	Bool		in_use;

};

typedef struct binding Binding;

struct binding
{
	Binding		*NextBinding; 

	char		IsMouse,	/* Is it a mouse or key binding 1= mouse; */
			*key_name,	/* In case of keycode, give the key_name too */
			*Action;	/* What to do? */

	int		Button_Key,	/* Mouse Button number of Keycode */
			Context,	/* Contex is Fvwm context, ie titlebar, frame, etc */
			Modifier;	/* Modifiers for keyboard state */   
};

#define MENU_ERROR	-1
#define MENU_NOP	 0
#define MENU_DONE	 1
#define SUBMENU_DONE	 2

/* Types of events for the FUNCTION builtin */
#define MOTION		'm'
#define IMMEDIATE	'i'
#define CLICK		'c'
#define DOUBLE_CLICK	'd'
#define ONE_AND_A_HALF_CLICKS 'o'

extern MenuRoot	*ActiveMenu;
extern MenuItem *ActiveItem;

#endif /* _MENUS_ */
