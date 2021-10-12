/*
 ****************************************************************
 *								*
 *			h/main.h				*
 *								*
 *	Definições globais					*
 *								*
 *	Versão	3.0.0, de 17.09.97				*
 *		4.5.0, de 18.08.03				*
 *								*
 *	Módulo: programs/xterm					*
 *		X Windows - Clientes				*
 *		Baseado no software homônimo do XFree86		*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright (c) 2003 TROPIX Technologies Ltd.	*
 * 								*
 ****************************************************************
 */

#define	VERSION	"1.0.0"

typedef struct
{
	int		width, height;	/* window size [pixels] */
	int		fwidth, fheight;/* font width and height [pixels] */
	int		ncol, nrow;	/* window size [characters] */
	int		focus;		/* window has focus */
	int		saveLines;	/* number of lines that fit in scrollback */
	int		nscrolled;	/* number of line actually scrolled */
	int		view_start;	/* scrollback view starts here */

	Window		parent, vt;	/* parent (main) and vt100 window */
	GC		gc;		/* GC for drawing text */

	XFontStruct	*font;		/* main font structure */

#ifndef NO_BOLDFONT
	XFontStruct	*boldFont;	/* bold font */
#endif

}	TermWin_t;

extern	TermWin_t	TermWin;

/* gap between text and window edges (could be configurable) */
#define TermWin_internalBorder	2
#define MAX_COLS	200
#define MAX_ROWS	128

/* width of scrollBar, menuBar shadow ... don't change! */
#define SHADOW	2

/* convert pixel dimensions to row/column values */
#define Pixel2Width(x)	((x) / TermWin.fwidth)
#define Pixel2Height(y)	((y) / TermWin.fheight)
#define Pixel2Col(x)	Pixel2Width((x) - TermWin_internalBorder)
#define Pixel2Row(y)	Pixel2Height((y) - TermWin_internalBorder)

#define Width2Pixel(n)	((n) * TermWin.fwidth)
#define Height2Pixel(n)	((n) * TermWin.fheight)
#define Col2Pixel(col)	(Width2Pixel(col) + TermWin_internalBorder)
#define Row2Pixel(row)	(Height2Pixel(row) + TermWin_internalBorder)

#define TermWin_TotalWidth()	(TermWin.width  + 2 * TermWin_internalBorder)
#define TermWin_TotalHeight()	(TermWin.height + 2 * TermWin_internalBorder)

extern Display		*dpy;

#define Xscreen		DefaultScreen(dpy)
#define Xcmap		DefaultColormap(dpy,Xscreen)
#define Xdepth		DefaultDepth(dpy,Xscreen)
#define Xroot		DefaultRootWindow(dpy)

#ifdef DEBUG_DEPTH
# undef Xdepth
# define Xdepth		DEBUG_DEPTH
#endif

#define	Opt_console	(1LU<<0)
#define Opt_loginShell	(1LU<<1)
#define Opt_iconic	(1LU<<2)
#define Opt_visualBell	(1LU<<3)
#define Opt_mapAlert	(1LU<<4)
#define Opt_reverseVideo (1LU<<5)
#define Opt_utmpInhibit	(1LU<<6)
#define Opt_scrollBar	(1LU<<7)
#define Opt_meta8	(1LU<<8)
/* place holder used for parsing command-line options */
#define Opt_Boolean	(1LU<<31)

extern	unsigned long	Options;

extern	const char	*display_name;
extern	const char	*home_directory;
extern	const char	*rs_name;	/* client instance (resource name) */
extern	const char	*desktop_number;

/*
 * XTerm escape sequences: ESC ] Ps;Pt BEL
 */
#define XTerm_name	0
#define XTerm_iconName	1
#define XTerm_title	2
#define XTerm_logfile	46	/* not implemented */
#define XTerm_font	50

/*
 * rxvt extensions of XTerm escape sequences: ESC ] Ps;Pt BEL
 */
#define XTerm_Menu	10	/* set menu item */
#define XTerm_Pixmap	20	/* new bg pixmap */
#define XTerm_restoreFG	39	/* change default fg color */
#define XTerm_restoreBG	49	/* change default bg color */

#define restoreFG	39	/* restore default fg color */
#define restoreBG	49	/* restore default bg color */

#define fgColor		0
#define bgColor		1
/* 0-7: black, red, green, yellow, blue, magenta, cyan, white */
#define minColor	2
#define maxColor	(minColor+7)
#define blackColor	(minColor)

/* 10-17: Bright black, red, green, yellow, blue, magenta, cyan, white */
# define minBright	(maxColor+1)
# define maxBright	(minBright+7)
# define whiteColor	(maxBright)
#define NCOLORS		(whiteColor+1)

#define cursorColor	(NCOLORS)
#define cursorColor2	(cursorColor+1)
#define NCURSOR	2

#ifdef NO_BOLDUNDERLINE
# define NBOLDULINE	0
#else
# define colorBD	(NCOLORS + NCURSOR)
# define colorUL	(colorBD+1)
# define NBOLDULINE	2
#endif

# define scrollColor		(NCOLORS + NCURSOR + NBOLDULINE)
# define topShadowColor		(scrollColor + 1)
# define bottomShadowColor	(scrollColor + 2)
# define NSCROLLCOLORS	1
# define NSHADOWCOLORS	2

extern const char	pgversion[];

#define NRS_COLORS	(NCOLORS + NCURSOR + NBOLDULINE + NSCROLLCOLORS)
extern	const char	*rs_color [NRS_COLORS];

typedef ulong	Pixel;
extern Pixel PixColors [NRS_COLORS + NSHADOWCOLORS];

extern	const char	*rs_font[];

#ifndef NO_BOLDFONT
extern	const char	*rs_boldFont;
#endif

extern	const char	*rs_cutchars;

extern	int		saved_nice;

extern	void		map_menuBar (int);
extern	void		map_scrollBar (int);
extern	void		xterm_seq (int, char *);

/* special (internal) prefix for font commands */
#define FONT_CMD	'#'
#define FONT_DN		"#-"
#define FONT_UP		"#+"

extern	void		change_font (int, const char *);
extern	void		set_width (unsigned int);
extern	void		resize_window (void);
