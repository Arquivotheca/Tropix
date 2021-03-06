/*
 ****************************************************************
 *								*
 *			src/events.c				*
 *								*
 *	Trata os eventos recebidos do Servidor			*
 *								*
 *	Vers?o	3.0.0, de 30.07.97				*
 *		4.4.0, de 20.03.03				*
 *								*
 *	M?dulo: cmd/fvwm					*
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

#include "../global.h"

#include <sys/common.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#if (0)	/*******************************************************/
#include <X11/fd.h>
#endif	/*******************************************************/

#include "fvwm.h"
#include <X11/Xatom.h>
#include "menus.h"
#include "misc.h"
#include "parse.h"
#include "screen.h"
#ifdef SHAPE
#include <X11/extensions/shape.h>
#endif /* SHAPE */
#include "module.h"

/*
 ****************************************************************
 *	Vari?veis globais					*
 ****************************************************************
 */
ulong		mods_used =	(ShiftMask | ControlMask | Mod1Mask |
				 Mod2Mask| Mod3Mask| Mod4Mask| Mod5Mask);

extern	int	menuFromFrameOrWindowOrTitlebar;


int		Context = C_NO_CONTEXT;	/* current button press context */
int		Button = 0;
FVWMWIN	*ButtonWindow;		/* button press window structure */
XEvent		Event;			/* the current event */
FVWMWIN	*Tmp_win;		/* the current fvwm window */

int		last_event_type=0;
Window		last_event_window=0;

#ifdef SHAPE
extern	int	ShapeEventBase;
void		HandleShapeNotify (void);
#endif /* SHAPE */

Window		PressedW;

static int		grave_fd;
static const char	grave_dev[] =	"/dev/grave";

/*
 ****************************************************************
 *	Tabela de eventos					*
 ****************************************************************
 *
 *	LASTEvent is the number of X events defined - it should be defined
 *	in X.h (to be like 35), but since extension (eg SHAPE) events are
 *	numbered beyond LASTEvent, we need to use a bigger number than the
 *	default, so let's undefine the default and use 256 instead.
 */
#undef LASTEvent
#ifndef LASTEvent
#define LASTEvent 256
#endif /* !LASTEvent */

typedef	void	(*PFEH)();

PFEH		EventHandlerJumpTable[LASTEvent];
char		*event_name[LASTEvent];

/*
 ****************************************************************
 *	Inicializa a Tabela para Tratamento de Eventos		*
 ****************************************************************
 */
void
InitEventHandlerJumpTable (void)
{
	int	i;

	for (i = 0; i < LASTEvent; i++)
	{
		EventHandlerJumpTable[i] = NULL;
		event_name[i] = "Evento n?o tratado";
	}

	EventHandlerJumpTable[Expose]		= HandleExpose;
	EventHandlerJumpTable[DestroyNotify]	= HandleDestroyNotify;
	EventHandlerJumpTable[MapRequest]	= HandleMapRequest;
	EventHandlerJumpTable[MapNotify]	= HandleMapNotify;
	EventHandlerJumpTable[UnmapNotify]	= HandleUnmapNotify;
	EventHandlerJumpTable[ButtonPress]	= HandleButtonPress;
	EventHandlerJumpTable[EnterNotify]	= HandleEnterNotify;
	EventHandlerJumpTable[LeaveNotify]	= HandleLeaveNotify;
	EventHandlerJumpTable[FocusIn]		= HandleFocusIn;
	EventHandlerJumpTable[ConfigureRequest]	= HandleConfigureRequest;
	EventHandlerJumpTable[ClientMessage]	= HandleClientMessage;
	EventHandlerJumpTable[PropertyNotify]	= HandlePropertyNotify;
	EventHandlerJumpTable[KeyPress]		= HandleKeyPress;
	EventHandlerJumpTable[VisibilityNotify]	= HandleVisibilityNotify;
	EventHandlerJumpTable[ColormapNotify]	= HandleColormapNotify;

	event_name[Expose]		= "Expose";
	event_name[DestroyNotify]	= "DestroyNotify";
	event_name[MapRequest]		= "MapRequest";
	event_name[MapNotify]		= "MapNotify";
	event_name[UnmapNotify]		= "UnmapNotify";
	event_name[ButtonPress]		= "ButtonPress";
	event_name[EnterNotify]		= "EnterNotify";
	event_name[LeaveNotify]		= "LeaveNotify";
	event_name[FocusIn]		= "FocusIn";
	event_name[ConfigureRequest]	= "ConfigureRequest";
	event_name[ClientMessage]	= "ClientMessage";
	event_name[PropertyNotify]	= "PropertyNotify";
	event_name[KeyPress]		= "KeyPress";
	event_name[VisibilityNotify]	= "VisibilityNotify";
	event_name[ColormapNotify]	= "ColormapNotify";

#ifdef SHAPE
	if (ShapesSupported)
	{
		EventHandlerJumpTable[ShapeEventBase+ShapeNotify] = HandleShapeNotify;
		event_name[ShapeEventBase+ShapeNotify]		  = "ShapeNotify";
	}
#endif /* SHAPE */

}	/* end InitEventHandlerJumpTable */

/*
 ****************************************************************
 *	Trata um evento						*
 ****************************************************************
 */
void
DispatchEvent (void)
{
	Window	w = Event.xany.window;

	StashEventTime (&Event);

	if (XFindContext (dpy, w, FvwmContext, (caddr_t *)&Tmp_win) == XCNOENT)
		Tmp_win = NULL;

	last_event_type		= Event.type;
	last_event_window	= w;

	if (EventHandlerJumpTable[Event.type])
	{
#if (0)	/****************************************************/
		msg
		(	"Evento: %s, janela <%s, %d>",
			event_name [Event.type],
			Tmp_win ? Tmp_win->name : "???",
			w
		);
#endif	/****************************************************/

		(*EventHandlerJumpTable[Event.type]) ();
	}
	
}	/* end DispatchEvent */

/*
 ****************************************************************
 *	Loop de espera por eventos				*
 ****************************************************************
 */
void
HandleEvents (void)
{
	for (EVER)
	{
		last_event_type = 0;

		if (get_next_event (dpy, &Event))
			DispatchEvent ();
	}

}	/* end HandleEvents */

/*
 ****************************************************************
 *	Determina o contexto em que o evento ocorreu		*
 ****************************************************************
 */
int
GetContext (FVWMWIN *t, XEvent *e, Window *w)
{
	int	context, i;

	if (!t)
		return (C_ROOT); 
  
	context = C_NO_CONTEXT;
	*w= e->xany.window;
  
	if (*w == Scr.NoFocusWin)
		return (C_ROOT); 
  
	/*
	 *	Since key presses and button presses are grabbed in the frame
	 *	when we have re-parented windows, we need to find out the real
	 *	window where the event occured
	 */
	if (e->type == KeyPress && e->xkey.subwindow != None)
		*w = e->xkey.subwindow;
  
	if
	(	e->type == ButtonPress && e->xbutton.subwindow != None &&
		(e->xbutton.subwindow == t->w || e->xbutton.subwindow == t->Parent)
	)
		*w = e->xbutton.subwindow;
  
	if (*w == Scr.Root)
		context = C_ROOT;

	if (t)
	{
		if (*w == t->title_w)
			context = C_TITLE;
		if (*w == t->w || *w == t->Parent)
			context = C_WINDOW;
		if (*w == t->icon_w)
			context = C_ICON;
		if (*w == t->icon_pixmap_w)
			context = C_ICON;
		if (*w == t->frame)
			context = C_SIDEBAR;

		for (i = 0; i < 4; i++)
		{
			if (*w == t->corners[i])
			{
				context = C_FRAME;
				Button = i;
			}
		}

		for (i = 0; i < 4; i++)
		{
			if (*w == t->sides[i])
			{
				context = C_SIDEBAR;
				Button = i;
			}
		}

		for (i = 0; i < Scr.nr_left_buttons; i++)
		{
			if (*w == t->left_w[i])
			{
				context = (1 << i) * C_L1;
				Button = i;
			}
		}

		for (i = 0; i < Scr.nr_right_buttons; i++)
		{
			if (*w == t->right_w[i])
			{
				context = (1<<i)*C_R1;
				Button = i;
			}
		}
	}

	return (context);

}	/* end GetContext */

/*
 ****************************************************************
 *	Trata o evento FocusIn					*
 ****************************************************************
 */
void
HandleFocusIn (void)
{
	XEvent	d;
	Window	w;
  
	w = Event.xany.window;
	while (XCheckTypedEvent (dpy, FocusIn, &d))
		w = d.xany.window;

	if (XFindContext (dpy, w, FvwmContext, (caddr_t *)&Tmp_win) == XCNOENT)
		Tmp_win = NULL;
  
	if (Tmp_win == NULL)
	{
		if (w != Scr.NoFocusWin)
		{
			Scr.UnknownWinFocused = w;
		}
		else
		{
			SetBorder (Scr.Hilite, False, True, True, None);

			Broadcast
			(	M_FOCUS_CHANGE, 5, 0, 0, 0,
				Scr.HiColors.fore, Scr.HiColors.back
			);

			if (Scr.ColormapFocus == COLORMAP_FOLLOWS_FOCUS)
			{
				if (Scr.Hilite && (Scr.Hilite->flags & ICONIFIED) == 0)
					InstallWindowColormaps (Scr.Hilite);
				else
					InstallWindowColormaps (NULL);
			}
		}
	}
	else if (Tmp_win != Scr.Hilite)
	{
		SetBorder (Tmp_win, True, True, True, None);

		Broadcast
		(	M_FOCUS_CHANGE, 5, Tmp_win->w,
			Tmp_win->frame, (ulong)Tmp_win, Scr.HiColors.fore,
			Scr.HiColors.back
		);

		if (Scr.ColormapFocus == COLORMAP_FOLLOWS_FOCUS)
		{
			if (Scr.Hilite && (Scr.Hilite->flags & ICONIFIED) == 0)
				InstallWindowColormaps (Scr.Hilite);
			else
				InstallWindowColormaps (NULL);
		}
	}

}	/* end HandleFocusIn */

/*
 ****************************************************************
 *	Trata o evento KeyPress					*
 ****************************************************************
 */
void
HandleKeyPress (void)
{
	Binding		*key;
	unsigned int	modifier;

	modifier = (Event.xkey.state & mods_used);
	ButtonWindow = Tmp_win;
 
	Context = GetContext (Tmp_win, &Event, &PressedW);
	PressedW = None;

	/*
	 *	Here's a real hack - some systems have two keys with the
	 *	same keysym and different keycodes. This converts all
	 *	the cases to one keycode.
	 */
	Event.xkey.keycode =	XKeysymToKeycode
				(	dpy,
					XKeycodeToKeysym
					(	dpy,
						Event.xkey.keycode, 0
					)
				);

	for (key = Scr.AllBindings; key != NULL; key = key->NextBinding)
	{
		if
		(	key->Button_Key == Event.xkey.keycode &&
			(	key->Modifier == (modifier & (~LockMask)) ||
				key->Modifier == AnyModifier
			) &&
			(key->Context & Context) &&
			key->IsMouse == 0
		)
		{
			ExecuteFunction(key->Action,Tmp_win, &Event,Context,-1);
			return;
		}
	}
  
	/*
	 *	if we get here, no function key was bound to the key.  Send it
	 *	to the client if it was in a window we know about.
	 */
	if (Tmp_win)
	{
		if (Event.xkey.window != Tmp_win->w)
		{
			Event.xkey.window = Tmp_win->w;
			XSendEvent
			(	dpy, Tmp_win->w, False,
				KeyPressMask, &Event
			);
		}
	}

	ButtonWindow = NULL;

}	/* end HandleKeyPress */


/*
 ****************************************************************
 *	Trata o evento PropertyNotify				*
 ****************************************************************
 */
#define MAX_NAME_LEN		200		/* truncate to this many */
#define MAX_ICON_NAME_LEN	200		/* ditto */

void
HandlePropertyNotify (void)
{
	XTextProperty	text_prop;
  
	if
	(	!Tmp_win ||
		XGetGeometry
		(	dpy, Tmp_win->w, &JunkRoot, &JunkX, &JunkY,
			&JunkWidth, &JunkHeight, &JunkBW, &JunkDepth
		) == 0
	)
		return;
  
	switch (Event.xproperty.atom) 
	{
	    case XA_WM_NAME:
#if (0)	/****************************************************/
msg ("PROPERTYNOTIFY: janela <%s, %d>, XA_WM_NAME", Tmp_win->name, Tmp_win->w);
#endif	/****************************************************/
		if (!XGetWMName (dpy, Tmp_win->w, &text_prop))
			return;

		free_window_names (Tmp_win, True, False);
      
		Tmp_win->name = (char *)text_prop.value;
		if (Tmp_win->name == NULL)
			Tmp_win->name = NoName;

		BroadcastName
		(	M_WINDOW_NAME,Tmp_win->w,Tmp_win->frame,
			(ulong)Tmp_win,Tmp_win->name
		);
      
		/* fix the name in the title bar */
		if ((Tmp_win->flags & ICONIFIED) == 0)
			SetTitleBar (Tmp_win, Scr.Hilite == Tmp_win, True);
      
		/*
		 *	if the icon name is NoName, set the name of the icon to be
		 *	the same as the window 
		 */
		if (Tmp_win->icon_name == NoName) 
		{
			Tmp_win->icon_name = Tmp_win->name;

			BroadcastName
			(	M_ICON_NAME,Tmp_win->w,Tmp_win->frame,
				(ulong)Tmp_win,Tmp_win->icon_name
			);

			RedoIconName (Tmp_win);
		}
		break;
      
	    case XA_WM_ICON_NAME:
#if (0)	/****************************************************/
msg ("PROPERTYNOTIFY: janela <%s, %d>, XA_WM_ICON_NAME", Tmp_win->name, Tmp_win->w);
#endif	/****************************************************/
		if (!XGetWMIconName (dpy, Tmp_win->w, &text_prop))
			return;

		free_window_names (Tmp_win, False, True);

		Tmp_win->icon_name = (char *) text_prop.value;
		if (Tmp_win->icon_name == NULL)
			Tmp_win->icon_name = NoName;

		BroadcastName
		(	M_ICON_NAME,Tmp_win->w,Tmp_win->frame,
			(ulong)Tmp_win,Tmp_win->icon_name
		);

		RedoIconName(Tmp_win);
		break;
      
	    case XA_WM_HINTS:
#if (0)	/****************************************************/
msg ("PROPERTYNOTIFY: janela <%s, %d>, XA_WM_HINTS", Tmp_win->name, Tmp_win->w);
#endif	/****************************************************/
		if (Tmp_win->wmhints) 
			XFree ((char *)Tmp_win->wmhints);

		Tmp_win->wmhints = XGetWMHints (dpy, Event.xany.window);

		if (Tmp_win->wmhints == NULL)
			return;

		if
		(	(Tmp_win->wmhints->flags & IconPixmapHint) ||
			(Tmp_win->wmhints->flags & IconWindowHint)
		)
			if (Tmp_win->icon_bitmap_file == Scr.DefaultIcon)
				Tmp_win->icon_bitmap_file = (char *)0;
      
		if
		(	(Tmp_win->wmhints->flags & IconPixmapHint) ||
			(Tmp_win->wmhints->flags & IconWindowHint)
		)
		{
			if ((Tmp_win->flags & SUPPRESSICON) == 0)
			{
				if (Tmp_win->icon_w)
					XDestroyWindow (dpy, Tmp_win->icon_w);

				XDeleteContext (dpy, Tmp_win->icon_w, FvwmContext);

				if(Tmp_win->flags & ICON_OURS)
				{
					if(Tmp_win->icon_pixmap_w != None)
					{
						XDestroyWindow
						(	dpy,
							Tmp_win->icon_pixmap_w
						);

						XDeleteContext
						(	dpy,
							Tmp_win->icon_pixmap_w,
							FvwmContext
						);
					}
				}
				else 
				{
					XUnmapWindow (dpy, Tmp_win->icon_pixmap_w);
				}
			}

			Tmp_win->icon_w = None;
			Tmp_win->icon_pixmap_w = None;
			Tmp_win->iconPixmap = (Window)NULL;

			if (Tmp_win->flags & ICONIFIED)
			{
				Tmp_win->flags &= ~ICONIFIED;
				Tmp_win->flags &= ~ICON_UNMAPPED;

				CreateIconWindow
				(	Tmp_win,
					Tmp_win->icon_x_loc,
					Tmp_win->icon_y_loc
				);

				Broadcast
				(	M_ICONIFY, 7, Tmp_win->w,
					Tmp_win->frame,
					(ulong)Tmp_win,
					Tmp_win->icon_x_loc,
					Tmp_win->icon_y_loc,
					Tmp_win->icon_w_width,
					Tmp_win->icon_w_height
				);

				BroadcastConfig (M_CONFIGURE_WINDOW, Tmp_win);
	      
				if ((Tmp_win->flags & SUPPRESSICON) == 0)
				{
					LowerWindow (Tmp_win);
					AutoPlace (Tmp_win);

					if (Tmp_win->Desk == Scr.CurrentDesk)
					{
						if (Tmp_win->icon_w)
							XMapWindow (dpy, Tmp_win->icon_w);

						if (Tmp_win->icon_pixmap_w != None)
							XMapWindow(dpy, Tmp_win->icon_pixmap_w);
					}
				}

				Tmp_win->flags |= ICONIFIED;
				DrawIconWindow (Tmp_win);
			}
		}
		break;
      
	    case XA_WM_NORMAL_HINTS:
#if (0)	/****************************************************/
msg ("PROPERTYNOTIFY: janela <%s, %d>, XA_WM_NORMAL_HINTS", Tmp_win->name, Tmp_win->w);
#endif	/****************************************************/
		{
			int	new_width, new_height;
	
			GetWindowSizeHints (Tmp_win);
			new_width = Tmp_win->frame_width;
			new_height = Tmp_win->frame_height;

			ConstrainSize (Tmp_win, &new_width, &new_height);

			if
			(	(new_width != Tmp_win->frame_width)||
				(new_height != Tmp_win->frame_height)
			)
			{
				SetupFrame
				(	Tmp_win,
					Tmp_win->frame_x, Tmp_win->frame_y,
					new_width,new_height, False
				);
			}

			BroadcastConfig (M_CONFIGURE_WINDOW, Tmp_win);
		}
		break;
      
	    default:
#if (0)	/****************************************************/
msg ("PROPERTYNOTIFY: janela <%s, %d>, outros", Tmp_win->name, Tmp_win->w);
#endif	/****************************************************/
		if (Event.xproperty.atom == _XA_WM_PROTOCOLS)
		{
			FetchWmProtocols (Tmp_win);
		}
		else if (Event.xproperty.atom == _XA_WM_COLORMAP_WINDOWS)
		{
			FetchWmColormapWindows (Tmp_win);	/* frees old data */
			ReInstallActiveColormap ();
		}
		else if (Event.xproperty.atom == _XA_WM_STATE)
		{
			if
			(	Tmp_win != NULL &&
				(Tmp_win->flags & ClickToFocus) &&
				Tmp_win == Scr.Focus
			)
			{
#if (0)	/****************************************************/
msg ("Vou mexer no foco");
#endif	/****************************************************/
				Scr.Focus = NULL;
				SetFocus (Tmp_win->w, Tmp_win, 0);
			}
		}
		break;
	}

}	/* end HandlePropertyNotify */

/*
 ****************************************************************
 *	Trata o evento ClientMessage				*
 ****************************************************************
 */
void
HandleClientMessage (void)
{
	XEvent	button;
  
	if
	(	(Event.xclient.message_type == _XA_WM_CHANGE_STATE) &&
		(Tmp_win)&&(Event.xclient.data.l[0] == IconicState) &&
		!(Tmp_win->flags & ICONIFIED)
	)
	{
		XQueryPointer
		(	dpy, Scr.Root, &JunkRoot, &JunkChild,
			&button.xmotion.x_root,
			&button.xmotion.y_root,
			&JunkX, &JunkY, &JunkMask
		);
			button.type = 0;

		ExecuteFunction ("Iconify", Tmp_win, &button, C_FRAME, -1);
		return;
	}

	/*
	 ** CKH - if we get here, it was an unknown client message, so send
	 ** it to the client if it was in a window we know about.  I'm not so
	 ** sure this should be done or not, since every other window manager
	 ** I've looked at doesn't.  But it might be handy for a free drag and
	 ** drop setup being developed for Linux.
	 */
	if (Tmp_win)
	{
		if (Event.xclient.window != Tmp_win->w)
		{
			Event.xclient.window = Tmp_win->w;
			XSendEvent (dpy, Tmp_win->w, False, NoEventMask, &Event);
		}
	}

}	/* end HandleClientMessage */

/*
 ****************************************************************
 *	Trata o evento HandleExpose				*
 ****************************************************************
 */
void
HandleExpose (void)
{
	if (Event.xexpose.count != 0)
		return;
  
	if (Tmp_win)
	{
		if ((Event.xany.window == Tmp_win->title_w))
		{
			SetTitleBar (Tmp_win, (Scr.Hilite == Tmp_win), False);
		}
		else
		{
			SetBorder
			(	Tmp_win, (Scr.Hilite == Tmp_win),
				True, True, Event.xany.window
			);
		}
	}

}	/* end HandleExpose */

/*
 ****************************************************************
 *	Trata o evento DestroyNotify				*
 ****************************************************************
 */
void
HandleDestroyNotify (void)
{
	Destroy (Tmp_win);

}	/* end HandleDestroyNotify */

/*
 ****************************************************************
 *	Trata o evento MapRequest				*
 ****************************************************************
 */
void
HandleMapRequest (void)
{
	HandleMapRequestKeepRaised (None);

}	/* end HandleMapRequest */

void
HandleMapRequestKeepRaised (Window KeepRaised)
{
	extern long	isIconicState;
	extern Bool	PPosOverride;
  
	Event.xany.window = Event.xmaprequest.window;
  
	if
	(	XFindContext
		(	dpy, Event.xany.window, FvwmContext, 
			(caddr_t *)&Tmp_win
		) == XCNOENT
	)
		Tmp_win = NULL;
  
	if (!PPosOverride)
		XFlush (dpy);
  
	/* If the window has never been mapped before ... */
	if (!Tmp_win)
	{
		/* Add decorations. */
		Tmp_win = AddWindow (Event.xany.window);
		if (Tmp_win == NULL)
			return;
	}

	if (KeepRaised != None)
		XRaiseWindow (dpy, KeepRaised);

	/* If it's not merely iconified, and we have hints, use them. */
	if ((Tmp_win->flags & ICONIFIED) == 0)
	{
		int state;
      
		if (Tmp_win->wmhints && (Tmp_win->wmhints->flags & StateHint))
			state = Tmp_win->wmhints->initial_state;
		else
			state = NormalState;
      
		if (Tmp_win->flags & STARTICONIC)
			state = IconicState;

		if (isIconicState != DontCareState) 
			state = isIconicState;

		XGrabServer (dpy);

		switch (state) 
		{
		    case DontCareState:
		    case NormalState:
		    case InactiveState:
		    default:
			if (Tmp_win->Desk == Scr.CurrentDesk)
			{
				XMapWindow(dpy, Tmp_win->w);
				XMapWindow(dpy, Tmp_win->frame);
				Tmp_win->flags |= MAP_PENDING;
				SetMapStateProp(Tmp_win, NormalState);
				if
				(	(Tmp_win->flags & ClickToFocus) &&
					((!Scr.Focus) ||
					(Scr.Focus->flags & ClickToFocus))
				)
				{
					SetFocus (Tmp_win->w, Tmp_win, 1);
				}
			}
			else
			{
				XMapWindow (dpy, Tmp_win->w);
				SetMapStateProp (Tmp_win, NormalState);
			}
			break;
	  
		    case IconicState:
			Iconify (Tmp_win, 0, 0);
			break;
		}

		if (!PPosOverride)
			XSync (dpy, 0);

		XUngrabServer (dpy);
	}
	else
	{
		/* If no hints, or currently an icon, just "deiconify" */
		DeIconify (Tmp_win);
	}

	if (!PPosOverride)
		KeepOnTop ();

#if (0)	/****************************************************/
	if
	(	Tmp_win->class.res_name != NULL &&
		streq (Tmp_win->class.res_name, "xconsole")
	)
	{
		msg ("MapRequest: %s", Tmp_win->name ? Tmp_win->name : "janela nula");
		XWarpPointer (dpy, None, Tmp_win->w, 0, 0, 0, 0, 10, 10);
	}
#endif	/****************************************************/

}	/* end HandleMapRequestKeepRaised */

/*
 ****************************************************************
 *	Trata o evento MapNotify				*
 ****************************************************************
 */
void
HandleMapNotify (void)
{
	if (!Tmp_win)
	{
		if((Event.xmap.override_redirect == True)&&
			(Event.xmap.window != Scr.NoFocusWin))
		{
#if (0)	/****************************************************/
msg ("HandleMapNotify: XSelectInput para janela %d", Event.xmap.window);
#endif	/****************************************************/
			XSelectInput(dpy,Event.xmap.window,FocusChangeMask);
			Scr.UnknownWinFocused = Event.xmap.window;
		}      
		return;
	}

/* Except for identifying over-ride redirect window mappings, we
 * don't need or want windows associated with the sunstructurenotifymask */
	if(Event.xmap.event != Event.xmap.window)
		return;
   
/*
 * Need to do the grab to avoid race condition of having server send
 * MapNotify to client before the frame gets mapped; this is bad because
 * the client would think that the window has a chance of being viewable
 * when it really isn't.
 */
	XGrabServer (dpy);

	if (Tmp_win->icon_w)
		XUnmapWindow (dpy, Tmp_win->icon_w);

	if (Tmp_win->icon_pixmap_w != None)
		XUnmapWindow (dpy, Tmp_win->icon_pixmap_w);

	XMapSubwindows (dpy, Tmp_win->frame);

	if (Tmp_win->Desk == Scr.CurrentDesk)
		XMapWindow (dpy, Tmp_win->frame);

	if (Tmp_win->flags & ICONIFIED)
		Broadcast (M_DEICONIFY,3,Tmp_win->w,Tmp_win->frame, (ulong)Tmp_win);
	else
		Broadcast (M_MAP,3,Tmp_win->w,Tmp_win->frame, (ulong)Tmp_win);

#if (0)	/****************************************************/
	msg ("MAPNOTIFY: janela <%s, %d>", Tmp_win->name, Tmp_win->w);
#endif	/****************************************************/
  
	if
	(	(Tmp_win->flags & ClickToFocus) &&
		Scr.Focus && (!Scr.Focus || (Scr.Focus->flags & ClickToFocus))
	)
	{
		SetFocus (Tmp_win->w,Tmp_win,1);
	}

	if
	(	(Tmp_win->flags & (BORDER | TITLE)) == 0 &&
		Tmp_win->boundary_width < 2
	)
	{
		SetBorder (Tmp_win, False, True, True, Tmp_win->frame);
	}

	XSync (dpy, 0);

	XUngrabServer (dpy);

	XFlush (dpy);

	Tmp_win->flags |= MAPPED;
	Tmp_win->flags &= ~MAP_PENDING;
	Tmp_win->flags &= ~ICONIFIED;
	Tmp_win->flags &= ~ICON_UNMAPPED;

	KeepOnTop ();

}	/* end HandleMapNotify */


/***********************************************************************
 *
 *  Procedure:
 *	HandleUnmapNotify - UnmapNotify event handler
 *
 ************************************************************************/
void
HandleUnmapNotify (void)
{
  int dstx, dsty;
  Window dumwin;
  XEvent dummy;
  extern FVWMWIN *colormap_win;

  if(Event.xunmap.event != Event.xunmap.window)
    return;
  /*
   * The July 27, 1988 ICCCM spec states that a client wishing to switch
   * to WithdrawnState should send a synthetic UnmapNotify with the
   * event field set to (pseudo-)root, in case the window is already
   * unmapped (which is the case for fvwm for IconicState).  Unfortunately,
   * we looked for the FvwmContext using that field, so try the window
   * field also.
   */
  if (!Tmp_win)
    {
      Event.xany.window = Event.xunmap.window;
      if (XFindContext(dpy, Event.xany.window,
		       FvwmContext, (caddr_t *)&Tmp_win) == XCNOENT)
	Tmp_win = NULL;
    }
  
  if(!Tmp_win)
    return;

  if(Tmp_win ==  Scr.Hilite)
    Scr.Hilite = NULL;

  if(Scr.PreviousFocus == Tmp_win)
    Scr.PreviousFocus = NULL;

  if((Tmp_win == Scr.Focus)&&(Tmp_win->flags & ClickToFocus))
    {
      if(Tmp_win->next)
	{
	  HandleHardFocus(Tmp_win->next);
	}
      else
	SetFocus(Scr.NoFocusWin,NULL,1);
    }
  
  if(Scr.Focus == Tmp_win)
    SetFocus(Scr.NoFocusWin,NULL,1);

  if(Tmp_win == Scr.pushed_window)
    Scr.pushed_window = NULL;

  if(Tmp_win == colormap_win)
    colormap_win = NULL;

  if ((!(Tmp_win->flags & MAPPED)&&!(Tmp_win->flags&ICONIFIED)))
    {
      return;
    }
  
  XGrabServer(dpy);
  
  if(XCheckTypedWindowEvent (dpy, Event.xunmap.window, DestroyNotify,&dummy)) 
    {
      Destroy(Tmp_win);
      XUngrabServer (dpy);
      return;
    } 

  /*
   * The program may have unmapped the client window, from either
   * NormalState or IconicState.  Handle the transition to WithdrawnState.
   *
   * We need to reparent the window back to the root (so that fvwm exiting 
   * won't cause it to get mapped) and then throw away all state (pretend 
   * that we've received a DestroyNotify).
   */
  if (XTranslateCoordinates (dpy, Event.xunmap.window, Scr.Root,
			     0, 0, &dstx, &dsty, &dumwin)) 
    {
      XEvent ev;
      Bool reparented;
      
      reparented = XCheckTypedWindowEvent (dpy, Event.xunmap.window, 
					   ReparentNotify, &ev);
      SetMapStateProp (Tmp_win, WithdrawnState);
      if (reparented) 
	{
	  if (Tmp_win->old_bw)
	    XSetWindowBorderWidth (dpy, Event.xunmap.window, Tmp_win->old_bw);
	  if((!(Tmp_win->flags & SUPPRESSICON))&&
	     (Tmp_win->wmhints && (Tmp_win->wmhints->flags & IconWindowHint)))
	    XUnmapWindow (dpy, Tmp_win->wmhints->icon_window);
	} 
      else
	{
	  RestoreWithdrawnLocation (Tmp_win,False);
	}
      XRemoveFromSaveSet (dpy, Event.xunmap.window);
      XSelectInput (dpy, Event.xunmap.window, NoEventMask);
      Destroy(Tmp_win);		/* do not need to mash event before */
      /*
       * Flush any pending events for the window.
       */
      /* Bzzt! it could be about to re-map */
/*      while(XCheckWindowEvent(dpy, Event.xunmap.window,
			      StructureNotifyMask | PropertyChangeMask |
			      ColormapChangeMask | VisibilityChangeMask |
			      EnterWindowMask | LeaveWindowMask, &dummy));
      */
    } /* else window no longer exists and we'll get a destroy notify */
  XUngrabServer (dpy);
  
  XFlush (dpy);
}


/***********************************************************************
 *
 *  Procedure:
 *	HandleButtonPress - ButtonPress event handler
 *
 ***********************************************************************/
void HandleButtonPress()
{
  unsigned int modifier;
  Binding *MouseEntry;
  Window x;
  int LocalContext;
  
  /* click to focus stuff goes here */
  if((Tmp_win)&&(Tmp_win->flags & ClickToFocus)&&(Tmp_win != Scr.Ungrabbed)&&
     ((Event.xbutton.state&
       (ControlMask|Mod1Mask|Mod2Mask|Mod3Mask|Mod4Mask|Mod5Mask)) == 0))
    {
      if(Tmp_win)
	{
	  SetFocus(Tmp_win->w,Tmp_win,1);
#ifdef CLICKY_MODE_1
	  if((Event.xany.window != Tmp_win->w)&&
	     (Event.xbutton.subwindow != Tmp_win->w)&&
	     (Event.xany.window != Tmp_win->Parent)&&
	     (Event.xbutton.subwindow != Tmp_win->Parent))
#endif
	    {
	      RaiseWindow(Tmp_win);
	    }
	  
	  KeepOnTop();

	  /* Why is this here? Seems to cause breakage with
	   * non-focusing windows! */
	  if(!(Tmp_win->flags & ICONIFIED))
	    {
	      XSync(dpy,0);
#if 1 /* pass click event to just clicked to focus window */
	      XAllowEvents(dpy,ReplayPointer,CurrentTime);
#else /* don't pass click to just focused window */
	      XAllowEvents(dpy,AsyncPointer,CurrentTime);
#endif /* 0 */
	      XSync(dpy,0);
	      return;
	    }

	}
    }
  XSync(dpy,0);
  XAllowEvents(dpy,ReplayPointer,CurrentTime);
  XSync(dpy,0);
  
  Context = GetContext(Tmp_win,&Event, &PressedW);
  LocalContext = Context;
  x= PressedW;
  if(Context == C_TITLE)
    SetTitleBar(Tmp_win,(Scr.Hilite == Tmp_win),False);
  else
    SetBorder(Tmp_win,(Scr.Hilite == Tmp_win),True,True,PressedW);	
  
  ButtonWindow = Tmp_win;
  
  /* we have to execute a function or pop up a menu
   */
  
  modifier = (Event.xbutton.state & mods_used);
  /* need to search for an appropriate mouse binding */
  for (MouseEntry = Scr.AllBindings; MouseEntry != NULL;
       MouseEntry= MouseEntry->NextBinding)
    {
      if(((MouseEntry->Button_Key == Event.xbutton.button)||
	  (MouseEntry->Button_Key == 0))&&
	 (MouseEntry->Context & Context)&&
	 ((MouseEntry->Modifier == AnyModifier)||
	  (MouseEntry->Modifier == (modifier& (~LockMask))))&&
	 (MouseEntry->IsMouse == 1))
	{
	  /* got a match, now process it */
	  ExecuteFunction(MouseEntry->Action,Tmp_win, &Event,Context,-1);
	  break;
	}
    }
  PressedW = None;
  if(LocalContext!=C_TITLE)
    SetBorder(ButtonWindow,(Scr.Hilite == ButtonWindow),True,True,x);
  else
    SetTitleBar(ButtonWindow,(Scr.Hilite==ButtonWindow),False);
  ButtonWindow = NULL;
}

/*
 ****************************************************************
 *	Trata o evento EnterNotify				*
 ****************************************************************
 */
void
HandleEnterNotify (void)
{
	XEnterWindowEvent	*ewp = &Event.xcrossing;
	XEvent			d;
  
	/*
	 *	Procura um evento "LeaveNotify" que anule o atual.
	 */
	if (XCheckTypedWindowEvent (dpy, ewp->window, LeaveNotify, &d))
	{
		StashEventTime (&d);

		if
		(	d.xcrossing.mode == NotifyNormal &&
			d.xcrossing.detail != NotifyInferior
		)
			return;
	}

	/*
	 *	EnterNotify em um dos PanFrames: ativa a pagina??o.
	 */
	if
	(	ewp->window == Scr.PanFrameTop.win 	||
		ewp->window == Scr.PanFrameLeft.win	||
		ewp->window == Scr.PanFrameRight.win	||
		ewp->window == Scr.PanFrameBottom.win
	)
	{
		int	delta_x = 0, delta_y = 0;

		/* this was in the HandleMotionNotify before, HEDU */
		HandlePaging
		(	Scr.EdgeScrollX, Scr.EdgeScrollY,
			&Event.xcrossing.x_root, &Event.xcrossing.y_root,
			&delta_x, &delta_y, True
		);

		return;
	}
 
	if (Event.xany.window == Scr.Root)
	{
		if
		(	Scr.Focus &&
			(Scr.Focus->flags & ClickToFocus) == 0 &&
			(Scr.Focus->flags & SloppyFocus) == 0
		)
			SetFocus (Scr.NoFocusWin, NULL, 1);

		if (Scr.ColormapFocus == COLORMAP_FOLLOWS_MOUSE)
			InstallWindowColormaps (NULL);
		return;
	}
  
	/* make sure its for one of our windows */
	if (!Tmp_win) 
		return;
 
	if ((Tmp_win->flags & ClickToFocus) == 0)
	{
#if (0)	/****************************************************/
		if (Scr.Focus != Tmp_win)
			SetFocus (Tmp_win->w, Tmp_win, 0);
		else
#endif	/****************************************************/
			SetFocus (Tmp_win->w, Tmp_win, 0);
	}

	if (Scr.ColormapFocus == COLORMAP_FOLLOWS_MOUSE)
	{
		InstallWindowColormaps
		(	(Tmp_win->flags & ICONIFIED) == 0 &&
			Event.xany.window == Tmp_win->w ? Tmp_win : NULL
		);
	}

}	/* end HandleEnterNotify */

/*
 ****************************************************************
 *	Trata o evento LeaveNotify				*
 ****************************************************************
 */
void
HandleLeaveNotify (void)
{
	/*
	 *	If we leave the root window, then we're really moving
	 *	another screen on a multiple screen display, and we
	 *	need to de-focus and unhighlight to make sure that we
	 *	don't end up with more than one highlighted window at a time
	 */
	if
	(	Event.xcrossing.window == Scr.Root &&
		Event.xcrossing.mode == NotifyNormal &&
		Event.xcrossing.detail != NotifyInferior
	)
	{

		if (Scr.Focus != NULL)
			SetFocus (Scr.NoFocusWin, NULL, 1);

		if (Scr.Hilite != NULL)
			SetBorder (Scr.Hilite, False, True, True, None);
	}

}	/* end HandleLeaveNotify */

/***********************************************************************
 *
 *  Procedure:
 *	HandleConfigureRequest - ConfigureRequest event handler
 *
 ************************************************************************/
void HandleConfigureRequest()
{
  XWindowChanges xwc;
  ulong xwcm;
  int x, y, width, height;
  XConfigureRequestEvent *cre = &Event.xconfigurerequest;
  
  /*
   * Event.xany.window is Event.xconfigurerequest.parent, so Tmp_win will
   * be wrong
   */
  Event.xany.window = cre->window;	/* mash parent field */
  if (XFindContext (dpy, cre->window, FvwmContext, (caddr_t *) &Tmp_win) ==
      XCNOENT)
    Tmp_win = NULL;
  
  /*
   * According to the July 27, 1988 ICCCM draft, we should ignore size and
   * position fields in the WM_NORMAL_HINTS property when we map a window.
   * Instead, we'll read the current geometry.  Therefore, we should respond
   * to configuration requests for windows which have never been mapped.
   */
  if (!Tmp_win || (Tmp_win->icon_w == cre->window))
    {
      xwcm = cre->value_mask & 
	(CWX | CWY | CWWidth | CWHeight | CWBorderWidth);
      xwc.x = cre->x;
      xwc.y = cre->y;
      if((Tmp_win)&&((Tmp_win->icon_w == cre->window)))
	{
	  Tmp_win->icon_xl_loc = cre->x;
	  Tmp_win->icon_x_loc = cre->x + 
	    (Tmp_win->icon_w_width - Tmp_win->icon_p_width)/2;
	  Tmp_win->icon_y_loc = cre->y - Tmp_win->icon_p_height;
	  if(!(Tmp_win->flags & ICON_UNMAPPED))
	    Broadcast(M_ICON_LOCATION,7,Tmp_win->w,Tmp_win->frame,
		      (ulong)Tmp_win,
		      Tmp_win->icon_x_loc,Tmp_win->icon_y_loc,
		      Tmp_win->icon_w_width, 
		      Tmp_win->icon_w_height + Tmp_win->icon_p_height);
	}
      xwc.width = cre->width;
      xwc.height = cre->height;
      xwc.border_width = cre->border_width;
      XConfigureWindow(dpy, Event.xany.window, xwcm, &xwc);
      
      if(Tmp_win)
	{
	  xwc.x = Tmp_win->icon_x_loc;
	  xwc.y = Tmp_win->icon_y_loc - Tmp_win->icon_p_height;
	  xwcm = cre->value_mask & (CWX | CWY);
	  if(Tmp_win->icon_pixmap_w != None)
	    XConfigureWindow(dpy, Tmp_win->icon_pixmap_w, xwcm, &xwc);
	  xwc.x = Tmp_win->icon_x_loc;
	  xwc.y = Tmp_win->icon_y_loc;
	  xwcm = cre->value_mask & (CWX | CWY);
	  if(Tmp_win->icon_w != None)
	    XConfigureWindow(dpy, Tmp_win->icon_w, xwcm, &xwc);
	}
      return;
    }
  
  if (cre->value_mask & CWStackMode) 
    {
      FVWMWIN *otherwin;
      
      xwc.sibling = (((cre->value_mask & CWSibling) &&
		      (XFindContext (dpy, cre->above, FvwmContext,
				     (caddr_t *) &otherwin) == XCSUCCESS))
		     ? otherwin->frame : cre->above);
      xwc.stack_mode = cre->detail;
      XConfigureWindow (dpy, Tmp_win->frame,
			cre->value_mask & (CWSibling | CWStackMode), &xwc);
    }
  
#ifdef SHAPE
  if (ShapesSupported)
  {
    int xws, yws, xbs, ybs;
    unsigned wws, hws, wbs, hbs;
    int boundingShaped, clipShaped;
    
    XShapeQueryExtents (dpy, Tmp_win->w,&boundingShaped, &xws, &yws, &wws,
			&hws,&clipShaped, &xbs, &ybs, &wbs, &hbs);
    Tmp_win->wShaped = boundingShaped;
  }
#endif /* SHAPE */
  
  /* Don't modify frame_XXX fields before calling SetupWindow! */
  x = Tmp_win->frame_x;
  y = Tmp_win->frame_y;
  width = Tmp_win->frame_width;
  height = Tmp_win->frame_height;
  
  /* for restoring */  
  if (cre->value_mask & CWBorderWidth) 
    {
      Tmp_win->old_bw = cre->border_width; 
    }
  /* override even if border change */
  
  if (cre->value_mask & CWX)
    x = cre->x - Tmp_win->boundary_width - Tmp_win->bw;
  if (cre->value_mask & CWY) 
    y = cre->y - Tmp_win->boundary_width - Tmp_win->title_height - Tmp_win->bw;
  if (cre->value_mask & CWWidth)
    width = cre->width + 2*Tmp_win->boundary_width;
  if (cre->value_mask & CWHeight) 
    height = cre->height+Tmp_win->title_height+2*Tmp_win->boundary_width;
  
  /*
   * SetupWindow (x,y) are the location of the upper-left outer corner and
   * are passed directly to XMoveResizeWindow (frame).  The (width,height)
   * are the inner size of the frame.  The inner width is the same as the 
   * requested client window width; the inner height is the same as the
   * requested client window height plus any title bar slop.
   */
  SetupFrame (Tmp_win, x, y, width, height,FALSE);
  KeepOnTop();
  
}

/***********************************************************************
 *
 *  Procedure:
 *      HandleShapeNotify - shape notification event handler
 *
 ***********************************************************************/
#ifdef SHAPE
void HandleShapeNotify (void)
{
  if (ShapesSupported)
  {
    XShapeEvent *sev = (XShapeEvent *) &Event;
  
    if (!Tmp_win)
      return;
    if (sev->kind != ShapeBounding)
      return;
    Tmp_win->wShaped = sev->shaped;
    SetShape(Tmp_win,Tmp_win->frame_width);
  }
}
#endif  /* SHAPE*/

/*
 ****************************************************************
 *	Trata o evento VisibilityNotify				*
 ****************************************************************
 *
 *	HandleVisibilityNotify - record fully visible windows for
 *      use in the RaiseLower function and the OnTop type windows.
 *
 */
void
HandleVisibilityNotify (void)
{
	XVisibilityEvent	*vevent = (XVisibilityEvent *)&Event;
  
	if(Tmp_win)
	{
		if(vevent->state == VisibilityUnobscured)
			Tmp_win->flags |= VISIBLE;
		else
			Tmp_win->flags &= ~VISIBLE;

#if (0)	/****************************************************/
msg ("Janela %s %s", Tmp_win->name, (Tmp_win->flags & VISIBLE) ? "vis?vel" :
"invis?vel");
#endif	/****************************************************/
      
		/*
		 *	For the most part, we'll raised partially obscured
		 *	ONTOP windows here.
		 *	The exception is ONTOP windows that are obscured by
		 *	other ONTOP windows, which are raised in KeepOnTop().
		 *	This complicated set-up saves us from continually
		 *	re-raising every on top window.
		 */
		if
		(	((vevent->state == VisibilityPartiallyObscured) ||
			 (vevent->state == VisibilityFullyObscured)) &&
			(Tmp_win->flags & ONTOP) &&
			(Tmp_win->flags & RAISED)
		)
		{
			RaiseWindow (Tmp_win);
			Tmp_win->flags &= ~RAISED;
		}
	}

}	/* end HandleVisibilityNotify */

/*
 ****************************************************************
 *	Espera por um evento					*
 ****************************************************************
 */
int
get_next_event (Display *dpy, XEvent *event)
{
	extern int	x_fd;
	Window		targetWindow;
	int		i, count;
	int		nfd, fd, retval, vec[NUFILE];
	int		pid, status;
	STAT		st;

	XFlush (dpy);

	if (XPending (dpy))
	{
		XNextEvent (dpy, event);
		StashEventTime (event);
		return (1);
	}

	FlushAllQueues ();

	nfd = 0;
	vec[nfd++] = grave_fd;		/* 0: tumba */
	vec[nfd++] = x_fd;		/* 1: X     */

	for (i = 0; i < npipes; i++)	/* 2, 3, ...: pipes */
	{
		if ((fd = readPipes[i]) >= 0)
			vec[nfd++] = fd;
	}

again:
	XFlush (dpy);

	retval = attention (nfd, vec, -1, 0);

	if (retval == 0)
	{
		pid = wait (&status);
		msg ("morreu o filho %d, status = %02X", pid, status);
		goto again;
	}

	if (retval > 1)
	{
		fd = vec[retval];

		for (i = 0; i < npipes; i++)
		{
			if (readPipes[i] != fd)
				continue;

			count = read (fd, &targetWindow, sizeof (Window));
			if (count > 0)
				HandleModuleInput (targetWindow, i);
			else
				KillModule (i, 10);
		}
	}

	FlushAllQueues ();

	return (0);

}	/* end get_next_event */

/*
 ****************************************************************
 *	Recolhe filhos mortos (com muito pesar)			*
 ****************************************************************
 */
ReapChildren (void)
{
	int		pid, status;

	while (attention (1, &grave_fd, -1, -1) == 0)
	{
		pid = wait (&status);
		msg ("morreu o filho %d, status = %02X", pid, status);
	}
}

/*
 ****************************************************************
 *	Abre a tumba						*
 ****************************************************************
 */
void
open_grave_device (void)
{
	if ((grave_fd = open (grave_dev, O_RDONLY)) < 0)
		msg ("$N?o consegui abrir o \"%s\"", grave_dev);

}	/* end open_grave_device */
