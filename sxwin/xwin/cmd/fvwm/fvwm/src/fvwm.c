/*
 ****************************************************************
 *								*
 *			src/fvwm.c				*
 *								*
 *	Gerenciador de Janelas - Módulo Principal		*
 *								*
 *	Versão	3.0.0, de 13.09.97				*
 *		4.4.0, de 25.03.03				*
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
 * This module is all original code 
 * by Rob Nation 
 * Copyright 1993, Robert Nation
 *     You may use this code for any purpose, as long as the original
 *     copyright remains in the source code and all documentation
 ****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "../global.h"

#include "fvwm.h"
#include "menus.h"
#include "misc.h"
#include "screen.h"
#include "parse.h"
#include "module.h"

#include <X11/Xproto.h>
#include <X11/Xatom.h>

#ifdef SHAPE
#include <X11/extensions/shape.h>
#endif /* SHAPE */

#define MAXHOSTNAME 255

/*
 ****************************************************************
 *	Nome e Versão do Programa				*
 ****************************************************************
 */
entry	const char	pgversion[]	= "Versão 4.4.0, de 25.03.03";

/*
 ****************************************************************
 *	Variáveis Globais					*
 ****************************************************************
 */
int		master_pid;		/* process number of 1st fvwm process */

ScreenInfo	Scr;		        /* structures for the screen */
Display		*dpy;			/* which display are we talking to */

Window		BlackoutWin=None;        /* window to hide window captures */

char		*config_command = "Read " FVWMRC;

XContext	FvwmContext;		/* context for fvwm windows */
XContext	MenuContext;		/* context for fvwm menus */

Window		JunkRoot, JunkChild;		/* junk window */

int		JunkX = 0, JunkY = 0;
unsigned int	JunkWidth, JunkHeight,
		JunkBW, JunkDepth, JunkMask;

Boolean		debugging = False,
		PPosOverride,
		Blackout = False;

const char	**save_argv;

#ifdef SHAPE
int		ShapeEventBase,
		ShapeErrorBase;
Boolean		ShapesSupported = False;
#endif

long		isIconicState = 0;
Bool		Restarting = False;
int		fd_width, x_fd;

Atom		_XA_MIT_PRIORITY_COLORS,
		_XA_WM_CHANGE_STATE,
		_XA_WM_STATE,
		_XA_WM_COLORMAP_WINDOWS,
		_XA_WM_PROTOCOLS,
		_XA_WM_TAKE_FOCUS,
		_XA_WM_DELETE_WINDOW,
		_XA_WM_DESKTOP,
		_XA_MwmAtom,
		_XA_MOTIF_WM,
		_XA_OL_WIN_ATTR,
		_XA_OL_WT_BASE,
		_XA_OL_WT_CMD,
		_XA_OL_WT_HELP,
		_XA_OL_WT_NOTICE,
		_XA_OL_WT_OTHER,
		_XA_OL_DECOR_ADD,
		_XA_OL_DECOR_DEL,
		_XA_OL_DECOR_CLOSE,
		_XA_OL_DECOR_RESIZE,
		_XA_OL_DECOR_HEADER,
		_XA_OL_DECOR_ICON_NAME;

extern	XEvent	Event;

/*
 ****************************************************************
 *	Definições Locais					*
 ****************************************************************
 */
#define	fatom(x)	XInternAtom (dpy, x, False)
#define	fexec(x)	ExecuteFunction (x, NULL, &Event, C_ROOT, -1)
#define	fcursor(x)	XCreateFontCursor (dpy, x)

/* assorted gray bitmaps for decorative borders */
#define g_width		2
#define g_height	2
#define l_g_width	4
#define l_g_height	2
#define s_g_width	4
#define s_g_height	4

static	char	g_bits[]   = {0x02, 0x01};
static	char	l_g_bits[] = {0x08, 0x02};
static	char	s_g_bits[] = {0x01, 0x02, 0x04, 0x08};

#define	newhandler(sig)		if (signal (sig, SIG_IGN) != SIG_IGN)	\
					signal (sig, SigDone);

/*
 ****************************************************************
 *	Protótipos de Funções					*
 ****************************************************************
 */
void		CreateStandardColormaps (const char *);
void		help (int);
XErrorHandler	FvwmErrorHandler (Display *, XErrorEvent *);
int		close_x_connection (Display *);
XErrorHandler	CatchRedirectError (Display *, XErrorEvent *);
void		InternUsefulAtoms  (void);
void		InitVariables (void);
void		CreateCursors (void);
void		ChildDied (int nonsense);
void		SaveDesktopState (void);
void		SetMWM_INFO (Window window);
void		SetRCDefaults (void);
void		StartupStuff (void);
void		SigDone (int, ...);
void		Restart (int, ...);
void		DeadPipe (int, ...);

/*
 ****************************************************************
 *	Ponto de Entrada					*
 ****************************************************************
 */
void
main (int argc, const char *argv[])
{
	ulong			valuemask;	/* mask for create windows */
	XSetWindowAttributes	attributes;	/* attributes for create windows */
	int			opt;
	const char		*display = NOSTR;
	int			restart_flag = 0;

	save_argv = argv;

	while ((opt = getopt (argc, argv, "HMbd:f:c:r")) != EOF)
	{
		switch (opt)
		{
		    case 'H':
			help (0);

		    case 'M':
			exit (0);

		    case 'b':
			Blackout = True;
			break;

		    case 'd':
			display = optarg;
			break;

		    case 'f':
			config_command = malloc (6 + strlen (optarg));
			if (config_command != NOSTR)
			{
				strcpy (config_command, "Read ");
				strcat (config_command, optarg);
			}
			else
				msg ("$Memória esgotada");
			break;

		    case 'c':
			config_command = (char *)optarg;
			break;

		    case 'r':
			restart_flag++;
			break;

		    default:
			putc ('\n', stderr);
			help (2);
		}
	}

	if (argc - optind > 0)
		help (1);

	if (restart_flag)
		msg ("Reinicializando");
	else
		msg ("Entrando no ar");

	newhandler (SIGINT);
	newhandler (SIGHUP);
	newhandler (SIGQUIT);
	newhandler (SIGTERM);
	signal (SIGUSR1, Restart);
	signal (SIGPIPE, DeadPipe);

	open_grave_device ();

	ReapChildren ();

	if (!restart_flag)
		CreateStandardColormaps (display);

	if (dpy == NULL && (dpy = XOpenDisplay (display)) == NULL)
		msg ("$Não consegui contactar o servidor \"%s\"", XDisplayName (display));

#if (0)	/*******************************************************/
	XChangePointerControl (dpy, True, True, 1, 1, 4);
#endif	/*******************************************************/
	XChangePointerControl (dpy, True, True, 3, 1, 32);

	Scr.screen		= DefaultScreen (dpy);
	Scr.NumberOfScreens	= ScreenCount (dpy);

	if ((Scr.Root = RootWindow (dpy, Scr.screen)) == None)
		msg ("$Não obtive a identificação da janela mãe");

	master_pid = getpid ();

	x_fd	 = XConnectionNumber (dpy);
	fd_width = GetFdWidth ();
    
	if (fcntl (x_fd, F_SETFD, 1) < 0)
	{
		fvwm_msg (ERR, "main", "close-on-exec failed");
		exit (1);
	}

#ifdef	SHAPE
	ShapesSupported =	XShapeQueryExtension
				(	dpy,
					&ShapeEventBase, &ShapeErrorBase
				);
#endif	SHAPE

	InternUsefulAtoms ();

	/*
	 *	Certifica-se de que a propriedade PRIORITY_COLORS esteja vazia.
	 */
	XChangeProperty
	(	dpy, Scr.Root, _XA_MIT_PRIORITY_COLORS,
		XA_CARDINAL, 32, PropModeReplace, NULL, 0
	);

	XSetErrorHandler ((XErrorHandler)CatchRedirectError);
	XSetIOErrorHandler (close_x_connection);

	XSelectInput
	(	dpy, Scr.Root,
		LeaveWindowMask| EnterWindowMask | PropertyChangeMask | 
		SubstructureRedirectMask | KeyPressMask | 
		SubstructureNotifyMask |
		ButtonPressMask | ButtonReleaseMask
	);

	XSync (dpy, 0);

	XSetErrorHandler ((XErrorHandler)FvwmErrorHandler);
	BlackoutScreen ();

	CreateCursors ();
	InitVariables ();
	InitEventHandlerJumpTable ();
	initModules ();

	Scr.gray_bitmap =	XCreateBitmapFromData
				(	dpy, Scr.Root, g_bits,
					g_width,g_height
				);

	SetRCDefaults ();
	fexec (config_command);

	if (Scr.d_depth < 2)
	{
		Scr.gray_pixmap =	XCreatePixmapFromBitmapData
					(	dpy, Scr.Root, g_bits,
						g_width, g_height,
						Scr.MenuColors.fore,
						Scr.MenuColors.back,
						Scr.d_depth
					);

		Scr.light_gray_pixmap = XCreatePixmapFromBitmapData
					(	dpy, Scr.Root, l_g_bits,
						l_g_width, l_g_height,
						Scr.MenuColors.fore,
						Scr.MenuColors.back,
						Scr.d_depth
					);
	}

	/*
	 *	Cria uma janela que aceitará o foco do teclado quando
	 *	nenhuma outra o possuir.
	 */
	attributes.event_mask = KeyPressMask|FocusChangeMask;
	attributes.override_redirect = True;

	Scr.NoFocusWin =	XCreateWindow
				(	dpy, Scr.Root,
					-10, -10, 10, 10, 0, 0,
					InputOnly, CopyFromParent,
					CWEventMask | CWOverrideRedirect,
					&attributes
				);

	XMapWindow (dpy, Scr.NoFocusWin);

	SetMWM_INFO (Scr.NoFocusWin);

	XSetInputFocus (dpy, Scr.NoFocusWin, RevertToParent, CurrentTime);

	XSync (dpy, 0);

	if (debugging)
		XSynchronize (dpy, 1);

	Scr.SizeStringWidth =	XTextWidth
				(	Scr.StdFont.font,
					" +8888 x +8888 ", 15
				);

	attributes.border_pixel = Scr.MenuColors.fore;
	attributes.background_pixel = Scr.MenuColors.back;
	attributes.bit_gravity = NorthWestGravity;

	valuemask = (CWBorderPixel | CWBackPixel | CWBitGravity);

	if ((Scr.flags & MWMMenus) == 0)
	{
		Scr.SizeWindow =	XCreateWindow
					(	dpy, Scr.Root,
						0, 0, 
						(unsigned int)(Scr.SizeStringWidth +
							SIZE_HINDENT*2),
						(unsigned int)(Scr.StdFont.height +
							SIZE_VINDENT*2),
						0, 0,
						(unsigned int)CopyFromParent,
						(Visual *)CopyFromParent,
						valuemask, &attributes
					);
	}
	else
	{
		Scr.SizeWindow =	XCreateWindow
					(	dpy, Scr.Root,
						Scr.MyDisplayWidth/2 - 
						(Scr.SizeStringWidth +
							SIZE_HINDENT*2)/2,
						Scr.MyDisplayHeight/2 -
						(Scr.StdFont.height + 
							SIZE_VINDENT*2)/2, 
						(unsigned int)(Scr.SizeStringWidth +
							SIZE_HINDENT*2),
						(unsigned int)(Scr.StdFont.height +
							SIZE_VINDENT*2),
						0, 0,
						(unsigned int)CopyFromParent,
						(Visual *)CopyFromParent,
						valuemask, &attributes
					);
	}

	initPanFrames ();

	XGrabServer (dpy);
	checkPanFrames ();
	XUngrabServer (dpy);
	UnBlackoutScreen ();
	HandleEvents ();

}	/* end main */

/*
 ****************************************************************
 *	Cria os Mapas de Cores "default"			*
 ****************************************************************
 */
void
CreateStandardColormaps (const char *display)
{
	int	 	nvisuals, scr;
	long		vinfo_mask;
	XVisualInfo	template, *vp;

	if ((dpy = XOpenDisplay (display)) == NULL)
		return;

	template.screen = scr = DefaultScreen (dpy);
	vinfo_mask	= VisualScreenMask;

	if ((vp = XGetVisualInfo (dpy, vinfo_mask, &template, &nvisuals)) != NULL)
	{
		if (XmuVisualStandardColormaps (dpy, scr, vp->visualid,	(unsigned)vp->depth, 1, 1))
			msg ("Mapas de cores corretamente criados");
		else
			msg ("Erro na criação dos mapas de cores");

		XFree ((char *)vp);

		XFlush (dpy);
	}

}	/* end CreateStandardColormaps */

/*
 ****************************************************************
 *	Inicialização						*
 ****************************************************************
 */
void
StartupStuff (void)
{
	MenuRoot	*mr;

	CaptureAllWindows ();
	MakeMenus ();
      
	if (Restarting)
	{
		if ((mr = FindPopup ("RestartFunction")) != NULL)
			fexec ("Function RestartFunction");
	}
	else
	{
		if ((mr = FindPopup ("InitFunction")) != NULL)
			fexec ("Function InitFunction");
	}

}	/* end StartupStuff */

/*
 ****************************************************************
 *	Captura as janelas já existentes			*
 ****************************************************************
 */
void
CaptureAllWindows (void)
{
	int		i,j;
	unsigned int	nchildren;
	Window		root, parent, *children;

	PPosOverride = TRUE;

	if
	(	!XQueryTree
		(	dpy, Scr.Root, &root, &parent,
			&children, &nchildren
		)
	)
		return;

	/*
	 *	weed out icon windows
	 */
	for (i = 0; i < nchildren; i++) 
	{
		if (children[i]) 
		{
			XWMHints	*wmhintsp;

			wmhintsp = XGetWMHints (dpy, children[i]);

			if (wmhintsp) 
			{
				if (wmhintsp->flags & IconWindowHint) 
				{
					for (j = 0; j < nchildren; j++) 
					{
						if (children[j] == wmhintsp->icon_window) 
						{
							children[j] = None;
							break;
						}
					}
				}

				XFree ((char *)wmhintsp);
			}
		}
	}

	/*
	 *	map all of the non-override windows
	 */
	for (i = 0; i < nchildren; i++)
	{
		if (children[i] && MappedNotOverride (children[i]))
		{
			XUnmapWindow (dpy, children[i]);
			Event.xmaprequest.window = children[i];
			HandleMapRequestKeepRaised (BlackoutWin);
		}
	}
  
	isIconicState = DontCareState;

	if (nchildren > 0)
		XFree ((char *)children);

	/*
	 *	After the windows already on the screen are in place,
	 *	don't use PPosition
	 */
	PPosOverride = FALSE;
	Scr.flags   |= WindowsCaptured;

	KeepOnTop ();

}	/* end CaptureAllWindows */

/*
 ****************************************************************
 *	Inicializa o essencial (caso não tenha .fvwmrc)		*
 ****************************************************************
 */
void
SetRCDefaults (void)
{
	/* set up default colors, fonts, etc */

	fexec ("HilightColor black grey");
	fexec ("XORValue 255");

	fexec ("MenuStyle black grey slategrey fixed fvwm");

	fexec ("TitleStyle Centered Raised");
	fexec ("IconFont fixed");
	fexec ("WindowFont fixed");

#if (0)	/****************************************************/
	fexec ("Style \"*\" Color lightgrey/dimgrey, Title");
	fexec ("Style \"*\" RandomPlacement, SmartPlacement");
#endif	/****************************************************/

	fexec ("AddToMenu builtin_menu \"Builtin Menu\" Title");
	fexec ("+ \"Sair do Fvwm\" Quit");
	fexec ("Mouse 1 R N Popup builtin_menu");

}	/* end SetRCDefaults */

/*
 ****************************************************************
 *	Verifica se uma janela tem interseção com outras ?	*
 ****************************************************************
 */
int
MappedNotOverride (Window w)
{
	XWindowAttributes	wa;
	Atom			atype;
	int			aformat;
	ulong			nitems, bytes_remain;
	uchar			*prop;

	isIconicState = DontCareState;

	if (w == Scr.NoFocusWin || !XGetWindowAttributes (dpy, w, &wa))
		return (False);

	if
	(	XGetWindowProperty
		(	dpy, w, _XA_WM_STATE, 0, 3, False, _XA_WM_STATE,
			&atype, &aformat, &nitems, &bytes_remain, &prop
		) == Success
	)
	{
		if (prop != NULL)
		{
			isIconicState = *(long *)prop;
			XFree (prop);
		}
	}

	if (wa.override_redirect == True)
		XSelectInput (dpy, w, FocusChangeMask);

	return
	(	(isIconicState == IconicState || wa.map_state != IsUnmapped) &&
		wa.override_redirect != True
	);

}	/* end MappedNotOverride */

/*
 ****************************************************************
 *	Cria diversos átomos (possivelmente moléculas também)	*
 ****************************************************************
 */
void
InternUsefulAtoms (void)
{
	_XA_MIT_PRIORITY_COLORS	= fatom ("_MIT_PRIORITY_COLORS");   
	_XA_WM_CHANGE_STATE	= fatom ("WM_CHANGE_STATE");
	_XA_WM_STATE		= fatom ("WM_STATE");
	_XA_WM_COLORMAP_WINDOWS = fatom ("WM_COLORMAP_WINDOWS");
	_XA_WM_PROTOCOLS	= fatom ("WM_PROTOCOLS");
	_XA_WM_TAKE_FOCUS	= fatom ("WM_TAKE_FOCUS");
	_XA_WM_DELETE_WINDOW	= fatom ("WM_DELETE_WINDOW");
	_XA_WM_DESKTOP		= fatom ("WM_DESKTOP");
	_XA_MwmAtom		= fatom ("_MOTIF_WM_HINTS");
	_XA_MOTIF_WM		= fatom ("_MOTIF_WM_INFO");

	_XA_OL_WIN_ATTR		= fatom ("_OL_WIN_ATTR");
	_XA_OL_WT_BASE		= fatom ("_OL_WT_BASE");
	_XA_OL_WT_CMD		= fatom ("_OL_WT_CMD");
	_XA_OL_WT_HELP		= fatom ("_OL_WT_HELP");
	_XA_OL_WT_NOTICE	= fatom ("_OL_WT_NOTICE");
	_XA_OL_WT_OTHER		= fatom ("_OL_WT_OTHER");
	_XA_OL_DECOR_ADD	= fatom ("_OL_DECOR_ADD");
	_XA_OL_DECOR_DEL	= fatom ("_OL_DECOR_DEL");
	_XA_OL_DECOR_CLOSE	= fatom ("_OL_DECOR_CLOSE");
	_XA_OL_DECOR_RESIZE	= fatom ("_OL_DECOR_RESIZE");
	_XA_OL_DECOR_HEADER	= fatom ("_OL_DECOR_HEADER");
	_XA_OL_DECOR_ICON_NAME	= fatom ("_OL_DECOR_ICON_NAME");

}	/* end InternUsefulAtoms */

/*
 ****************************************************************
 *	Reinicialização						*
 ****************************************************************
 */
void
Restart (int nonsense, ...)
{
	Done (1, *save_argv);

}	/* end Restart */

/*
 ****************************************************************
 *	Seleciona os diversos tipos de cursor			*
 ****************************************************************
 */
void
CreateCursors (void)
{
	Scr.FvwmCursors[POSITION]	= fcursor (XC_top_left_corner);
	Scr.FvwmCursors[DEFAULT]	= fcursor (XC_top_left_arrow);
	Scr.FvwmCursors[SYS]		= fcursor (XC_hand2);
	Scr.FvwmCursors[TITLE_CURSOR]	= fcursor (XC_top_left_arrow);
	Scr.FvwmCursors[MOVE]		= fcursor (XC_fleur);
	Scr.FvwmCursors[MENU]		= fcursor (XC_sb_left_arrow);
	Scr.FvwmCursors[WAIT]		= fcursor (XC_watch);
	Scr.FvwmCursors[SELECT]		= fcursor (XC_dot);
	Scr.FvwmCursors[DESTROY]	= fcursor (XC_pirate);
	Scr.FvwmCursors[LEFT]		= fcursor (XC_left_side);
	Scr.FvwmCursors[RIGHT]		= fcursor (XC_right_side);
	Scr.FvwmCursors[TOP]		= fcursor (XC_top_side);
	Scr.FvwmCursors[BOTTOM]		= fcursor (XC_bottom_side);
	Scr.FvwmCursors[TOP_LEFT]	= fcursor (XC_top_left_corner);
	Scr.FvwmCursors[TOP_RIGHT]	= fcursor (XC_top_right_corner);
	Scr.FvwmCursors[BOTTOM_LEFT]	= fcursor (XC_bottom_left_corner);
	Scr.FvwmCursors[BOTTOM_RIGHT]	= fcursor (XC_bottom_right_corner);

}	/* end CreateCursors */

/*
 ****************************************************************
 *	Inicializa diversas variáveis				*
 ****************************************************************
 */
void
InitVariables (void)
{
	FvwmContext = XUniqueContext ();
	MenuContext = XUniqueContext ();

	Scr.AllBindings = NULL;
	Scr.AllMenus = NULL;
	Scr.TheList = NULL;
  
	Scr.DefaultIcon = NULL;

	CreateGCs ();

	Scr.d_depth = DefaultDepth (dpy, Scr.screen);
	Scr.FvwmRoot.w = Scr.Root;
	Scr.FvwmRoot.next = 0;
	XGetWindowAttributes (dpy, Scr.Root, &Scr.FvwmRoot.attr);
	Scr.root_pushes = 0;
	Scr.pushed_window = &Scr.FvwmRoot;
	Scr.FvwmRoot.number_cmap_windows = 0;

	Scr.MyDisplayWidth = DisplayWidth (dpy, Scr.screen);
	Scr.MyDisplayHeight = DisplayHeight (dpy, Scr.screen);
    
	Scr.NoBoundaryWidth = 1;
	Scr.BoundaryWidth = BOUNDARY_WIDTH;
	Scr.CornerWidth = CORNER_WIDTH;
	Scr.Hilite = NULL;
	Scr.Focus = NULL;
	Scr.Ungrabbed = NULL;
  
	Scr.StdFont.font = NULL;
	Scr.HiReliefGC = NULL;
	Scr.HiShadowGC = NULL;

	Scr.TitleStyle.justify = JUSTIFY_CENTER;
	Scr.TitleStyle.appearance = TITLE_RAISED;

	Scr.VxMax = 2*Scr.MyDisplayWidth;
	Scr.VyMax = 2*Scr.MyDisplayHeight;

	Scr.Vx = Scr.Vy = 0;

	Scr.SizeWindow = None;

	/*
	 *	Sets the current desktop number to zero.
	 *	Multiple desks are available even in non-virtual
	 *	compilations
	 */
	{
		Atom	atype;
		int	aformat;
		ulong	nitems, bytes_remain;
		uchar	*prop;
    
		Scr.CurrentDesk = 0;

		if
		(	XGetWindowProperty
			(	dpy, Scr.Root, _XA_WM_DESKTOP, 0, 1, True,
				_XA_WM_DESKTOP, &atype, &aformat, &nitems,
				&bytes_remain, &prop
			) == Success
		)
		{
			if (prop != NULL)
			{
				Restarting = True;
				Scr.CurrentDesk = *(ulong *)prop;
			}
		}
	}

	Scr.EdgeScrollX = Scr.EdgeScrollY = 100;
	Scr.ScrollResistance = Scr.MoveResistance = 0;
	Scr.OpaqueSize = 5;
	Scr.ClickTime = 150;
	Scr.ColormapFocus = COLORMAP_FOLLOWS_MOUSE;

	/* set major operating modes */
	Scr.NumBoxes = 0;

	Scr.randomx = Scr.randomy = 0;
	Scr.buttons2grab = 7;

	Scr.left_x_coords[0][0] = 22;
	Scr.left_y_coords[0][0] = 39;
	Scr.left_line_style[0][0] = 1;
	Scr.left_x_coords[0][1] = 78;
	Scr.left_y_coords[0][1] = 39;
	Scr.left_line_style[0][1] = 1;
	Scr.left_x_coords[0][2] = 78;
	Scr.left_y_coords[0][2] = 61;
	Scr.left_line_style[0][2] = 0;
	Scr.left_x_coords[0][3] = 22;
	Scr.left_y_coords[0][3] = 61;
	Scr.left_line_style[0][3] = 0;
	Scr.left_x_coords[0][4] = 22;
	Scr.left_y_coords[0][4] = 39;
	Scr.left_line_style[0][4] = 1;

	Scr.left_num_coords[0] = 5;

	Scr.left_x_coords[1][0] = 32;
	Scr.left_y_coords[1][0] = 45;
	Scr.left_line_style[1][0] = 0;
	Scr.left_x_coords[1][1] = 68;
	Scr.left_y_coords[1][1] = 45;
	Scr.left_line_style[1][1] = 0;
	Scr.left_x_coords[1][2] = 68;
	Scr.left_y_coords[1][2] = 55;
	Scr.left_line_style[1][2] = 1;
	Scr.left_x_coords[1][3] = 32;
	Scr.left_y_coords[1][3] = 55;
	Scr.left_line_style[1][3] = 1;
	Scr.left_x_coords[1][4] = 32;
	Scr.left_y_coords[1][4] = 45;
	Scr.left_line_style[1][4] = 0;
	Scr.left_num_coords[1] = 5;

	Scr.left_x_coords[2][0] = 49;
	Scr.left_y_coords[2][0] = 49;
	Scr.left_line_style[2][0] = 1;
	Scr.left_x_coords[2][1] = 51;
	Scr.left_y_coords[2][1] = 49;
	Scr.left_line_style[2][1] = 1;
	Scr.left_x_coords[2][2] = 51;
	Scr.left_y_coords[2][2] = 51;
	Scr.left_line_style[2][2] = 0;
	Scr.left_x_coords[2][3] = 49;
	Scr.left_y_coords[2][3] = 51;
	Scr.left_line_style[2][3] = 0;
	Scr.left_x_coords[2][4] = 49;
	Scr.left_y_coords[2][4] = 49;
	Scr.left_line_style[2][4] = 1;
	Scr.left_num_coords[2] = 5;

	Scr.left_x_coords[3][0] = 32;
	Scr.left_y_coords[3][0] = 45;
	Scr.left_line_style[3][0] = 1;
	Scr.left_x_coords[3][1] = 68;
	Scr.left_y_coords[3][1] = 45;
	Scr.left_line_style[3][1] = 1;
	Scr.left_x_coords[3][2] = 68;
	Scr.left_y_coords[3][2] = 55;
	Scr.left_line_style[3][2] = 0;
	Scr.left_x_coords[3][3] = 32;
	Scr.left_y_coords[3][3] = 55;
	Scr.left_line_style[3][3] = 0;
	Scr.left_x_coords[3][4] = 32;
	Scr.left_y_coords[3][4] = 45;
	Scr.left_line_style[3][4] = 1;
	Scr.left_num_coords[3] = 5;

	Scr.left_x_coords[4][0] = 22;
	Scr.left_y_coords[4][0] = 39;
	Scr.left_line_style[4][0] = 1;
	Scr.left_x_coords[4][1] = 78;
	Scr.left_y_coords[4][1] = 39;
	Scr.left_line_style[4][1] = 1;
	Scr.left_x_coords[4][2] = 78;
	Scr.left_y_coords[4][2] = 61;
	Scr.left_line_style[4][2] = 0;
	Scr.left_x_coords[4][3] = 22;
	Scr.left_y_coords[4][3] = 61;
	Scr.left_line_style[4][3] = 0;
	Scr.left_x_coords[4][4] = 22;
	Scr.left_y_coords[4][4] = 39;
	Scr.left_line_style[4][4] = 1;

	Scr.left_num_coords[4] = 5;

	Scr.right_x_coords[0][0] = 25;
	Scr.right_y_coords[0][0] = 25;
	Scr.right_line_style[0][0] = 1;
	Scr.right_x_coords[0][1] = 75;
	Scr.right_y_coords[0][1] = 25;
	Scr.right_line_style[0][1] = 1;
	Scr.right_x_coords[0][2] = 75;
	Scr.right_y_coords[0][2] = 75;
	Scr.right_line_style[0][2] = 0;
	Scr.right_x_coords[0][3] = 25;
	Scr.right_y_coords[0][3] = 75;
	Scr.right_line_style[0][3] = 0;
	Scr.right_x_coords[0][4] = 25;
	Scr.right_y_coords[0][4] = 25;
	Scr.right_line_style[0][4] = 1;

	Scr.right_num_coords[0] = 5;

	Scr.right_x_coords[1][0] = 39;
	Scr.right_y_coords[1][0] = 39;
	Scr.right_line_style[1][0] = 1;
	Scr.right_x_coords[1][1] = 61;
	Scr.right_y_coords[1][1] = 39;
	Scr.right_line_style[1][1] = 1;
	Scr.right_x_coords[1][2] = 61;
	Scr.right_y_coords[1][2] = 61;
	Scr.right_line_style[1][2] = 0;
	Scr.right_x_coords[1][3] = 39;
	Scr.right_y_coords[1][3] = 61;
	Scr.right_line_style[1][3] = 0;
	Scr.right_x_coords[1][4] = 39;
	Scr.right_y_coords[1][4] = 39;
	Scr.right_line_style[1][4] = 1;
	Scr.right_num_coords[1] = 5;

	Scr.right_x_coords[2][0] = 49;
	Scr.right_y_coords[2][0] = 49;
	Scr.right_line_style[2][0] = 1;
	Scr.right_x_coords[2][1] = 51;
	Scr.right_y_coords[2][1] = 49;
	Scr.right_line_style[2][1] = 1;
	Scr.right_x_coords[2][2] = 51;
	Scr.right_y_coords[2][2] = 51;
	Scr.right_line_style[2][2] = 0;
	Scr.right_x_coords[2][3] = 49;
	Scr.right_y_coords[2][3] = 51;
	Scr.right_line_style[2][3] = 0;
	Scr.right_x_coords[2][4] = 49;
	Scr.right_y_coords[2][4] = 49;
	Scr.right_line_style[2][4] = 1;
	Scr.right_num_coords[2] = 5;

	Scr.right_x_coords[3][0] = 25;
	Scr.right_y_coords[3][0] = 25;
	Scr.right_line_style[3][0] = 1;
	Scr.right_x_coords[3][1] = 75;
	Scr.right_y_coords[3][1] = 25;
	Scr.right_line_style[3][1] = 1;
	Scr.right_x_coords[3][2] = 75;
	Scr.right_y_coords[3][2] = 75;
	Scr.right_line_style[3][2] = 0;
	Scr.right_x_coords[3][3] = 25;
	Scr.right_y_coords[3][3] = 75;
	Scr.right_line_style[3][3] = 0;
	Scr.right_x_coords[3][4] = 25;
	Scr.right_y_coords[3][4] = 25;
	Scr.right_line_style[3][4] = 1;
	Scr.right_num_coords[3] = 5;

	Scr.right_x_coords[4][0] = 36;
	Scr.right_y_coords[4][0] = 36;
	Scr.right_line_style[4][0] = 1;
	Scr.right_x_coords[4][1] = 64;
	Scr.right_y_coords[4][1] = 36;
	Scr.right_line_style[4][1] = 1;
	Scr.right_x_coords[4][2] = 64;
	Scr.right_y_coords[4][2] = 64;
	Scr.right_line_style[4][2] = 0;
	Scr.right_x_coords[4][3] = 36;
	Scr.right_y_coords[4][3] = 64;
	Scr.right_line_style[4][3] = 0;
	Scr.right_x_coords[4][4] = 36;
	Scr.right_y_coords[4][4] = 36;
	Scr.right_line_style[4][4] = 1;

	Scr.right_num_coords[4] = 5;

	{
		int	i;

		for (i = 0; i < 10; i++) 
		{
			Scr.button_style[i]=NormalButton;
			Scr.pixmap_button[i].up=NULL;
			Scr.pixmap_button[i].down=NULL;
		}
	}

	/* Not the right place for this, should only be called once somewhere .. */
	InitPictureCMap (dpy, Scr.Root);
 
}	/* end InitVariables */

/*
 ****************************************************************
 *	Remove as bordas de todas as janelas			*
 ****************************************************************
 */
void
Reborder (void)
{
	FVWMWIN	*tmp;	/* temp fvwm window structure */

	/* put a border back around all windows */
	XGrabServer (dpy);

	InstallWindowColormaps (&Scr.FvwmRoot);	/* force reinstall */

	for (tmp = Scr.FvwmRoot.next; tmp != NULL; tmp = tmp->next)
	{
		RestoreWithdrawnLocation (tmp,True);
		XUnmapWindow (dpy, tmp->frame);
		XDestroyWindow (dpy, tmp->frame);
	}

	XUngrabServer (dpy);

	XSetInputFocus (dpy, PointerRoot, RevertToPointerRoot, CurrentTime);

	XSync (dpy, 0);

}	/* end Reborder */

/*
 ****************************************************************
 *	Sai à francesa						*
 ****************************************************************
 */
void
SigDone (int nonsense, ...)
{
	Done (0, NULL);

}	/* end SigDone */

void
Done (int restart, const char *command)
{
	MenuRoot	*mr;

	MoveViewport (0, 0, False);

	if ((mr = FindPopup ("ExitFunction")) != NULL)
		fexec ("Function ExitFunction");

	/* Close all my pipes */
	ClosePipes ();

	Reborder ();

	if (restart)
	{
		SaveDesktopState ();		/* I wonder why ... */

		/* Really make sure that the connection is closed and cleared! */
		XSelectInput (dpy, Scr.Root, 0);
		XSync (dpy, 0);
		XCloseDisplay (dpy);

		execlp (command, "fvwm", "-r", NOSTR);
		msg ("$*Não consegui reinicializar");
	}
	else
	{
		XCloseDisplay (dpy);
		exit (0);
	}

}	/* end Done */

/*
 ****************************************************************
 *	x
 ****************************************************************
 */
XErrorHandler
CatchRedirectError (Display *dpy, XErrorEvent *event)
{
	fvwm_msg (ERR, "CatchRedirectError", "another WM is running");
	exit (1);

}	/* end CatchRedirectError */

/*
 ****************************************************************
 *	Tratamento de erros (chamada pela Xlib)			*
 ****************************************************************
 */
int
close_x_connection (Display *dpy)
{
	MenuRoot	*mr;

	if (errno == EPIPE || errno == NOERR)
	{
#if (0)	/****************************************************/
		msg
		(	"o servidor X (\"%s\") encerrou a conexão.",
			DisplayString (dpy)
		);
#endif	/****************************************************/
	}
	else
	{
		msg
		(	"erro na conexão com servidor X (\"%s\"): %s.",
			DisplayString (dpy),
			strerror (errno)
		);
	}

	if ((mr = FindPopup ("ExitFunction")) != NULL)
		fexec ("Function ExitFunction");

	/* Close all my pipes */
	ClosePipes ();

	XDisconnectDisplay (dpy);
	exit (0);

	/*
	 *	Não chega aqui.
	 */
	return (0);

}	/* end close_x_connection */

/*
 ****************************************************************
 *	Emite mensagens de erro					*
 ****************************************************************
 */
XErrorHandler
FvwmErrorHandler (Display *dpy, XErrorEvent *event)
{
	extern int	last_event_type;

	/* some errors are acceptable, mostly they're caused by 
	 * trying to update a lost  window */
	if
	(	event->error_code == BadWindow ||
		event->request_code == X_GetGeometry ||
		event->error_code == BadDrawable ||
		event->request_code == X_SetInputFocus ||
		event->request_code == X_GrabButton ||
		event->request_code == X_ChangeWindowAttributes ||
		event->request_code == X_InstallColormap
	)
		return (0);


	fvwm_msg (ERR, "FvwmErrorHandler", "*** internal error ***");
	fvwm_msg
	(	ERR, "FvwmErrorHandler",
		"Request %d, Error %d, EventType: %d",
		event->request_code,
		event->error_code,
		last_event_type
	);

	return (0);

}	/* end FvwmErrorHandler */

/*
 ****************************************************************
 *	Ensina a usar o programa				*
 ****************************************************************
 */
void
help (int exitval)
{
	fprintf
	(	stderr,
		"%s - gerenciador de janelas para o X-Windows\n\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-d <display>]\n",
		pgname, pgversion, pgname
	);

	fprintf
	(	stderr,
		"\nOpções:"
		"\t-d: contacta o servidor <display>\n"
	);


	exit (exitval);

}	/* end help */

/*
 ****************************************************************
 *	x
 ****************************************************************
 */
void
SaveDesktopState (void)
{
	FVWMWIN	*t;
	ulong		data[1];

	for (t = Scr.FvwmRoot.next; t != NULL; t = t->next)
	{
		data[0] = (ulong) t->Desk;
		XChangeProperty (dpy, t->w, _XA_WM_DESKTOP, _XA_WM_DESKTOP, 32,
		PropModeReplace, (uchar *) data, 1);
	}

	data[0] = (ulong) Scr.CurrentDesk;
	XChangeProperty
	(	dpy, Scr.Root, _XA_WM_DESKTOP, _XA_WM_DESKTOP, 32,
		PropModeReplace, (uchar *)data, 1
	);

	XSync(dpy, 0);

}	/* end SaveDesktopState */

/*
 ****************************************************************
 *	x
 ****************************************************************
 */
void
SetMWM_INFO (Window window)
{
#ifdef MODALITY_IS_EVIL
	struct mwminfo
	{
		long	flags;
		Window	win;

	}	motif_wm_info;
  
	/* Set Motif WM_INFO atom to make motif relinquish 
	 * broken handling of modal dialogs */
	motif_wm_info.flags     = 2;
	motif_wm_info.win = window;
  
	XChangeProperty
	(	dpy, Scr.Root, _XA_MOTIF_WM, _XA_MOTIF_WM, 32,
		PropModeReplace, (char *)&motif_wm_info, 2
	);
#endif

}	/* end SetMWM_INFO */

/*
 ****************************************************************
 *	x
 ****************************************************************
 */
void
BlackoutScreen (void)
{
	XSetWindowAttributes	attributes;
	ulong			valuemask;

	if (Blackout && (BlackoutWin == None) && !debugging)
	{
		/* blackout screen */
		attributes.border_pixel = BlackPixel (dpy, Scr.screen);
		attributes.background_pixel = BlackPixel (dpy, Scr.screen);
		attributes.bit_gravity = NorthWestGravity;
		attributes.override_redirect = True; /* is override redirect needed? */

		valuemask = CWBorderPixel |
				CWBackPixel   |
				CWBitGravity  |
				CWOverrideRedirect;

		BlackoutWin =	XCreateWindow
				(	dpy, Scr.Root, 0, 0,
					DisplayWidth (dpy, Scr.screen),
					DisplayHeight (dpy, Scr.screen), 0,
					CopyFromParent,
					CopyFromParent, CopyFromParent,
					valuemask, &attributes
				);

		XMapWindow (dpy, BlackoutWin);
		XSync (dpy, 0);
	}

}	/* end BlackoutScreen */

/*
 ****************************************************************
 *	x
 ****************************************************************
 */
void
UnBlackoutScreen (void)
{
	if (Blackout && (BlackoutWin != None) && !debugging)
	{
		XDestroyWindow (dpy, BlackoutWin); /* unblacken the screen */
		XSync (dpy, 0);
		BlackoutWin = None;
	}

}	/* end UnBlackoutScreen */
