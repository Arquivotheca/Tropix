/*
 ****************************************************************
 *								*
 *			src/screen.c				*
 *								*
 *	Manipulação das Estruturas Referentes à Tela		*
 *								*
 *	Versão	3.0.0, de 15.09.97				*
 *		4.8.0, de 18.03.05				*
 *								*
 *	Módulo: cmd/xterm					*
 *		X Windows - Clientes				*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright (c) 2005 TROPIX Technologies Ltd.	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/ioctl.h>

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <assert.h>

#include <X11/Xlib.h>
#include <X11/Xatom.h>

#include "../h/feature.h"
#include "../h/command.h"
#include "../h/misc.h"
#include "../h/main.h"
#include "../h/screen.h"

#define PROP_SIZE	4096
#define	TABSIZE		8		/* default tab size */

#define MEMCOPY		memmove		/* allows overlap -- rgg 04/11/95 */

unsigned int colorfgbg = DEFAULT_RSTYLE;

/* #define DEBUG_SCREEN */
/* to do self-check for internal screen corruption */
/* #define DEBUG_COLORS */
/* to print out current color/renditions as they change */

typedef char		text_t;
typedef	unsigned int	rend_t;

static rend_t rstyle = DEFAULT_RSTYLE;

#define SPACE_CHAR ' '
#ifdef OLD_COLOR_MODEL
# define FILL_STYLE	DEFAULT_RSTYLE
#else
# define FILL_STYLE	(rstyle)
#endif

/* macro prototypes */
void ZERO_LINES (text_t *, rend_t *, int /* nlines */);
void BLANK_FILL (text_t *, rend_t *, int /* nchars */);

/* zero both text and rendition */
#define ZERO_LINES(ptext,prend,nlines)					\
	do								\
	{								\
		int	n = (nlines) * (TermWin.ncol + 1);		\
									\
		if (n > 0)						\
		{							\
			memset ((ptext), 0, (n * sizeof (text_t)));	\
			memset ((prend), 0, (n * sizeof (rend_t)));	\
		}							\
									\
	}	while (0)

/* fill text with spaces and fill rendition */
#define	BLANK_FILL(ptext,prend,nchars)					\
	do								\
	{								\
		int	n = (nchars);					\
									\
		if (n > 0)						\
		{							\
			rend_t * p = prend;				\
			memset						\
			(	(ptext),				\
				SPACE_CHAR,				\
				(n * sizeof (text_t))			\
			);						\
									\
			while (n-- > 0)					\
				*p++ = FILL_STYLE;			\
		}							\
									\
	}	while (0)

/*
 * how the screen accounting works
 *
 * `text' contains text including the scrollback buffer. Each line is a
 * fixed length [TermWin.ncol+1] with the final character of each:
 *	'\n':	for wrapped lines
 *	`\0':	for non-wrapped lines
 *
 * `rend' contains rendition information (font, bold, color, etc)
 *
 * the layout:
 * Rows [0 .. (TermWin.saveLines-1)] == scrollback region
 * Rows [TermWin.saveLines .. TermWin.saveLines + (TermWin.nrow-1)] ==
 *	screen region [0 .. (TermWin.nrow-1)]
 *
 * `row', `tscroll', `bscroll' are bounded by (0, TermWin.nrow)
 *
 * `col' is bounded by (0, TermWin.ncol)
 *
 * `TermWin.saveLines'
 *	is the maximum number of lines to save in the scrollback buffer.
 *	This is a fixed number for any particular xterm instance and is set
 *	by the option `-sl' or resource `saveLines'
 *
 * `TermWin.nscrolled'
 *	how many lines have been scrolled (saved)
 *		0 <= TermWin.nscrolled <= TermWin.saveLines
 *
 * `TermWin.view_start'
 *	the offset back into the scrollback buffer for our current view
 *		-(TermWin.nscrolled) <= TermWin.view_start <= 0
 *
 * The selection region is defined for [0 .. (TermWin.nrow-1)], which
 * corresponds to the regular screen and for [-1 .. -(TermWin.nscrolled)]
 * which corresponds to the scrolled region.
 */

typedef struct
{
	text_t	*text;		/* all the text, including scrollback	*/
	rend_t	*rend;		/* rendition, using the `RS_' flags	*/
	int	row, col;		/* cursor position			*/
	int	tscroll, bscroll;	/* top/bottom of scroll region		*/
	int	charset;		/* character set number [0..3]		*/
	unsigned int flags;

}	screen_t;

#define Screen_Relative		(1<<0)	/* relative origin mode flag	*/
#define Screen_VisibleCursor	(1<<1)	/* cursor visible?		*/
#define Screen_Autowrap		(1<<2)	/* auto-wrap flag		*/
#define Screen_Insert		(1<<3)	/* insert mode (vs. overstrike)	*/
#define Screen_WrapNext		(1<<4)	/* need to wrap for next char?	*/

#define Screen_DefaultFlags	(Screen_VisibleCursor|Screen_Autowrap)

static screen_t screen =
{ NULL, NULL, 0, 0, 0, 0, 0, Screen_DefaultFlags };

#ifdef NO_SECONDARY_SCREEN
# define NSCREENS	0
#else
# define NSCREENS	1
static screen_t swap_screen = { NULL, NULL, 0, 0, 0, 0, 0, Screen_DefaultFlags };
#endif

static int current_screen = PRIMARY;
static int rvideo = 0;	/* reverse video */

static char * tabs = NULL;	/* a 1 for a location with a tab-stop */
static text_t * linebuf = NULL;

/* Data for save-screen */
static struct
{
	int	row, col;
	int	charset;
	char	charset_char;
	rend_t	rstyle;

}	save = { 0, 0, 0, 'B', DEFAULT_RSTYLE };

/* This tells what's actually on the screen */
static text_t * drawn_text = NULL;
static rend_t * drawn_rend = NULL;

static char charsets [4] = {'B','B','B','B'};	/* all ascii */

/* save selection text with '\n' line endings, but translate
 * '\n' to '\r' for pasting */

#define SELECTION_CLEAR	0
#define SELECTION_BEGIN	1
#define SELECTION_INIT	2
#define SELECTION_CONT	3
#define SELECTION_DONE	4

static struct
{
	char	*text;	/* selected text */
	int	len;			/* length of selected text */
	int	op;			/* current operation */

	int	screen;		/* which screen is being used */

	struct
	{
		int	row, col;

	}	beg, end, mark;

}	selection = {NULL, 0, SELECTION_CLEAR, PRIMARY, {0, 0}, {0, 0}, {0, 0} };

/* also could add in these:
 * int firstr, lastr;		-- firstr <= row < lastr
 * if trying to implement X11 mouse highlighting
 */

#ifndef min
#define min(a,b)	(((a) < (b)) ? (a) : (b))
#define max(a,b)	(((a) > (b)) ? (a) : (b))
#endif

static	int		scroll_text (int row1, int row2, int count);
static	void		selection_check (void);

/*
 ****************************************************************
 *	Preenche linhas com brancos				*
 ****************************************************************
 */
static void
blank_lines (text_t *text, rend_t *rend, int count)
{
	int	r, ncol1;

	if (count <= 0)
		return;

	ncol1 = TermWin.ncol + 1;

	/* fill with blank lines */
	BLANK_FILL (text, rend, count * ncol1);

	/* terminate each line */
	text += (TermWin.ncol);
	for (r = 0; r < count; r++, text += ncol1)
		*text = '\0';

}	/* end blank_lines */

/*
 ****************************************************************
 *	Reinicializa as estruturas				*
 ****************************************************************
 *
 * called whenever the screen needs to be repaired due
 * to re-sizing or initialization
 */
void
scr_reset (void)
{
	static int	prev_nrow = -1, prev_ncol = -1;
	int		i, ncol = 0, ncol1;

	TermWin.view_start = 0;

	if (prev_ncol == TermWin.ncol && prev_nrow == TermWin.nrow)
		return;

	if (TermWin.ncol <= 0)
		TermWin.ncol = 80;

	if (TermWin.nrow <= 0)
		TermWin.nrow = 24;

	ncol  = min (prev_ncol, TermWin.ncol);
	ncol1 = TermWin.ncol + 1;

	if (prev_nrow < 0)
		TermWin.nscrolled = 0;

	for (i = 0; i <= NSCREENS; i++)
	{
		text_t		*old_text;
		rend_t		*old_rend;
		int		r, oldr, oldr_max;
		int		histsize = TermWin.saveLines;
		screen_t	*scr = &screen;

		/* allocate space for scrollback (primary screen) + screen */
#if NSCREENS
		if (i)
		{
			histsize = 0;
			scr = &swap_screen;
		}
#endif

		/* copy from old buffer to new buffer, as appropriate. */
		oldr = oldr_max = prev_nrow;
		if (prev_nrow > 0)
		{
			int	n = (TermWin.nrow - prev_nrow);

			if (n > 0)		/* window made taller */
			{
				oldr = (TermWin.nrow);

				if (histsize)
				{
					if (n <= TermWin.nscrolled)   /* enough */
					{
						scr->row += (n);
						oldr = oldr_max;
					}
					else
					{
						scr->row += (TermWin.nscrolled);
						oldr     -= (TermWin.nscrolled);
					}

					TermWin.nscrolled -= (n);
				}
			}
			else if (n < 0)	/* window made shorter */
			{
				if (scr->row < TermWin.nrow)
				{
					oldr_max = TermWin.nrow;
				}
				else		/* put extra into scrolled */
				{
					oldr_max = (scr->row+1);
					if (histsize)
						TermWin.nscrolled += (oldr_max - TermWin.nrow);
				}

				oldr = oldr_max;
			}

			oldr_max += histsize;
			oldr += histsize;
			oldr--;
		}

		if (scr->row < 0)
			scr->row = 0;
		else if (scr->row >= TermWin.nrow)
			scr->row = TermWin.nrow - 1;

		if (scr->col < 0)
			scr->col = 0;
		else if (scr->col >= TermWin.ncol)
			scr->col = TermWin.ncol - 1;

		/* reset scroll regions */
		scr->tscroll = 0;
		scr->bscroll = TermWin.nrow - 1;

		old_text = scr->text;
		old_rend = scr->rend;

		scr->text = xtmalloc (((histsize + TermWin.nrow) * ncol1) * sizeof (text_t));
		scr->rend = xtmalloc (((histsize + TermWin.nrow) * ncol1) * sizeof (rend_t));

		blank_lines (scr->text, scr->rend, histsize + TermWin.nrow);

		if (ncol > 0)
		{
			for
			(	r = TermWin.nrow + histsize - 1;
				r >= 0 && oldr >= 0;
				r--, oldr--
			)
			{
				if (oldr < oldr_max)
				{
					int	roffset  = r    * ncol1;
					int	oroffset = oldr * (prev_ncol+1);

					memcpy
					(	&scr->text [roffset],
						&old_text [oroffset],
						ncol * sizeof (text_t)
					);

					memcpy
					(	&scr->rend [roffset],
						&old_rend [oroffset],
						ncol * sizeof (rend_t)
					);
				}
			}
		}

		xtfree (old_text);
		xtfree (old_rend);
	}

	/* Make sure the cursor is on the screen */
	if (TermWin.nscrolled < 0)
		TermWin.nscrolled = 0;
	else if (TermWin.nscrolled > TermWin.saveLines)
		TermWin.nscrolled = TermWin.saveLines;

	prev_ncol = TermWin.ncol;
	prev_nrow = TermWin.nrow;

	drawn_text =	xtrealloc
			(	drawn_text,
				TermWin.nrow * ncol1 * sizeof (text_t)
			);
	drawn_rend =	xtrealloc
			(	drawn_rend,
				TermWin.nrow * ncol1 * sizeof (rend_t)
			);

	ZERO_LINES (drawn_text, drawn_rend, TermWin.nrow);

	/* ensure the cursor is on the screen */
	if (save.row >= TermWin.nrow)
		save.row = TermWin.nrow - 1;

	if (save.col >= TermWin.ncol)
		save.col = TermWin.ncol - 1;

	tabs    = xtrealloc (tabs,    TermWin.ncol * sizeof (char));
	linebuf = xtrealloc (linebuf, ncol1        * sizeof (text_t));

	memset (tabs, 0, TermWin.ncol * sizeof (char));

	for (i = 0; i < TermWin.ncol; i += TABSIZE)
		tabs [i] = 1;

	stty_resize ();

}	/* end scr_reset */

/*
 ****************************************************************
 *	Volta à Configuração Original				*
 ****************************************************************
 */
void
scr_poweron (void)
{
	screen_t	*scr = &screen;
	int		i;

	TermWin.view_start = 0;
	TermWin.nscrolled = 0;	/* xterm doesn't do this */

	memset (charsets, 'B', sizeof (charsets));
	rvideo = 0;
	scr_rendition (0, ~RS_None);

#if NSCREENS
	scr_change_screen (SECONDARY);
	scr_erase_screen (2);
#endif

	scr_change_screen (PRIMARY);
	scr_erase_screen (2);

	for (i = 0; i <= NSCREENS; i++)
	{
		scr->tscroll = 0;
		scr->bscroll = TermWin.nrow - 1;
		scr->row = scr->col = 0;
		scr->charset = 0;
		scr->flags = Screen_DefaultFlags;
#if NSCREENS
		scr = &swap_screen;
#endif
	}

	scr_cursor (SAVE);
	ZERO_LINES (drawn_text, drawn_rend, TermWin.nrow);

	scr_reset ();
	XClearWindow (dpy, TermWin.vt);
	scr_refresh (SLOW_REFRESH);

}	/* end scr_poweron */

/*
 ****************************************************************
 *	Muda características da fonte				*
 ****************************************************************
 */
static void
set_font_style (void)
{
	rstyle &= ~RS_fontMask;

	switch (charsets [screen.charset])
	{
	    case '0':		/* DEC Special Character and Line Drawing Set */
		rstyle |= RS_acsFont;
		break;

	    case 'A':		/* United Kingdom (UK) */
		rstyle |= RS_ukFont;
		break;

#if (0)	/****************************************************/
	    case 'B': 		/* United States (USASCII) */
		break;

		/* <: Multinational character set */
		/* 5: Finnish character set */
		/* C: Finnish character set */
		/* K: German character set */
#endif	/****************************************************/
	}

}	/* end set_font_style */

/*
 ****************************************************************
 *	Salva/recupera a posição do cursor			*
 ****************************************************************
 */
void
scr_cursor (int mode)
{
	switch (mode)
	{
	    case SAVE:
		save.row = screen.row;
		save.col = screen.col;
		save.rstyle = rstyle;
		save.charset = screen.charset;
		save.charset_char = charsets [save.charset];
		break;

	    case RESTORE:
		screen.row = save.row;
		screen.col = save.col;
		rstyle = save.rstyle;
		screen.charset = save.charset;
		charsets [screen.charset] = save.charset_char;
		set_font_style ();
		break;
	}

}	/* end scr_cursor */

/*
 ****************************************************************
 *	Alterna telas						*
 ****************************************************************
 */
int
scr_change_screen (int scrn)
{
	int	x;

	TermWin.view_start = 0;

	if (current_screen == scrn)
		return (current_screen);
	else
	{
		int	tmp;

		tmp = current_screen;
		current_screen = scrn;
		scrn = tmp;
	}

#if NSCREENS
	/* swap screens, but leave scrollback untouched */
	{
		const int	count   = TermWin.nrow      * (TermWin.ncol + 1);
		const int	roffset = TermWin.saveLines * (TermWin.ncol + 1);

		text_t		*text = &screen.text [roffset];
		rend_t		*rend = &screen.rend [roffset];

		for (x = 0; x < count; x++)
		{
			text_t	t;
			rend_t	r;

			t = text [x];
			text [x] = swap_screen.text [x];
			swap_screen.text [x] = t;

			r = rend [x];
			rend [x] = swap_screen.rend [x];
			swap_screen.rend [x] = r;
		}
	}

	x = screen.row;
	screen.row = swap_screen.row;
	swap_screen.row = x;

	x = screen.col;
	screen.col = swap_screen.col;
	swap_screen.col = x;

	x = screen.charset;
	screen.charset = swap_screen.charset;
	swap_screen.charset = x;

	x = screen.flags;
	screen.flags = swap_screen.flags;
	swap_screen.flags = x;

	screen.flags |= Screen_VisibleCursor;
	swap_screen.flags |= Screen_VisibleCursor;
#else
/* put contents of secondary screen into the scrollback */
	if (current_screen == PRIMARY)
		scroll_text (0, TermWin.nrow - 1, TermWin.nrow);
#endif

	return (scrn);

}	/* end scr_change_screen */

/*
 ****************************************************************
 *	Modifica a cor de frente/fundo				*
 ****************************************************************
 */
/*
 * Intensity:
 *	RS_Bold  = foreground
 *	RS_Blink = background
 */
void
scr_color (unsigned int color, unsigned int Intensity)
{
	switch (color)
	{
	    case restoreFG:
		color = fgColor;
		assert (Intensity == RS_Bold);
		break;

	    case restoreBG:
		color = bgColor;
		assert (Intensity == RS_Blink);
		break;

	    default:
		if (Xdepth <= 2)		/* Monochrome - ignore color changes */
		{
			switch (Intensity)
			{
			    case RS_Bold:
				color = fgColor;
				break;

			    case RS_Blink:
				color = bgColor;
				break;
			}
		}
		else
		{
			if
			(	(rstyle & Intensity) &&
				color >= minColor &&
				color <= maxColor
			)
			{
				color += (minBright - minColor);
			}
			else if (color >= minBright && color <= maxBright)
			{
				if (rstyle & Intensity)
					return;		/* already bold enough! */

				color -= (minBright - minColor);
			}
		}
		break;
	}

	switch (Intensity)
	{
	    case RS_Bold:
		rstyle = SET_FGCOLOR (rstyle, color);
		break;

	    case RS_Blink:
		rstyle = SET_BGCOLOR (rstyle, color);
		break;
	}

}	/* end scr_color */

/*{{{ Change the rendition style */
void
scr_rendition (int set, int style)
{
   unsigned int color;

   if (set)
     {
	rstyle |= style;	/* set rendition */
	switch (style) {
	 case RS_Bold:
	   color = GET_FGCOLOR (rstyle);
	   scr_color ((color == fgColor ?
		       GET_FGCOLOR (colorfgbg) : color), RS_Bold);
	   break;

	 case RS_Blink:
	   color = GET_BGCOLOR (rstyle);
	   scr_color ((color == bgColor ?
		       GET_BGCOLOR (colorfgbg) : color), RS_Blink);
	   break;

	 case RS_RVid:
	   if (rvideo) rstyle &= ~(RS_RVid);	/* reverse video mode */
	   break;
	}
     }
   else
     {
	rstyle &= ~(style);	/* unset rendition */

	switch (style) {
	 case ~RS_None:		/* default fg/bg colors */
	   rstyle = DEFAULT_RSTYLE;
	   if (rvideo) rstyle |= RS_RVid;	/* reverse video mode */
	   break;

	 case RS_Bold:
	   color = GET_FGCOLOR (rstyle);
	   if (color >= minBright && color <= maxBright)
	     {
		scr_color (color, RS_Bold);
		/* scr_color (color - (minBright - minColor), RS_Bold); */
		if ((rstyle & RS_fgMask) == (colorfgbg & RS_fgMask))
		  scr_color (restoreFG, RS_Bold);
	     }
	   break;

	 case RS_Blink:
	   color = GET_BGCOLOR (rstyle);
	   if (color >= minBright && color <= maxBright)
	     {
		scr_color (color, RS_Blink);
		/* scr_color (color - (minBright - minColor), RS_Blink); */
		if ((rstyle & RS_bgMask) == (colorfgbg & RS_bgMask))
		  scr_color (restoreBG, RS_Blink);
	     }
	   break;

	 case RS_RVid:
	   if (rvideo) rstyle |= RS_RVid;	/* reverse video mode */
	   break;
	}
     }
}

/*
 ****************************************************************
 *	Acrescenta um caractere ao texto armazenado		*
 ****************************************************************
 */
void
scr_putchar (int ch)
{
	int	nlines, roffset, ncol1;

	TermWin.view_start = 0;

	ncol1 = TermWin.ncol + 1;

	if (selection.op)
		selection_check ();

	nlines = (ch == '\n') ? 1 : 0;

	if (nlines > 0)
	{
		nlines += (screen.row - screen.bscroll);

		if
		(	nlines > 0 &&
			screen.tscroll == 0 &&
			screen.bscroll == TermWin.nrow - 1
		)
		{
			scroll_text (screen.tscroll, screen.bscroll, nlines);

			screen.row -= nlines;
			if (screen.row < -TermWin.saveLines)
				screen.row = -TermWin.saveLines;
		}
	}

	if (screen.col >= TermWin.ncol)
		screen.col = TermWin.ncol - 1;

	roffset = (screen.row + TermWin.saveLines) * ncol1;

	switch (ch)
	{
	    case '\t':
		scr_tab (1);
		return;

	    case '\n':
		screen.flags &= ~Screen_WrapNext;
		screen.text [roffset + TermWin.ncol] = '\0';

		if (screen.row == screen.bscroll)
		{
			scroll_text (screen.tscroll, screen.bscroll, 1);
		}
		else if (screen.row < TermWin.nrow - 1)
		{
			screen.row++;
			roffset = (screen.row + TermWin.saveLines) * ncol1;
		}

		return;

	    case '\r':
		screen.col = 0;
		screen.flags &= ~Screen_WrapNext;
		screen.text [roffset + TermWin.ncol] = '\0';
		return;
	}

	if (screen.flags & Screen_WrapNext)
	{
		screen.text [roffset + TermWin.ncol] = '\n'; /* wrap line */

		if (screen.row == screen.bscroll)
		{
			scroll_text (screen.tscroll, screen.bscroll, 1);
		}
		else if (screen.row < TermWin.nrow - 1)
		{
			screen.row++;
			roffset = (screen.row + TermWin.saveLines) * ncol1;
		}

		screen.col = 0;
		screen.flags &= ~Screen_WrapNext;
	}

	if (screen.flags & Screen_Insert)
		scr_insdel_chars (1, INSERT);

	screen.text [roffset + screen.col] = ch;
	screen.rend [roffset + screen.col] = rstyle;

	screen.col++;
	if (screen.col == TermWin.ncol)
	{
		screen.col--;
		if (screen.flags & Screen_Autowrap)
			screen.flags |= Screen_WrapNext;
		else
			screen.flags &= ~Screen_WrapNext;
	}

}	/* end scr_putchar */

/*
 ****************************************************************
 *	Rola o texto						*
 ****************************************************************
 */
/*
 * Scroll COUNT lines from ROW1 to ROW2 inclusive (ROW1 <= ROW2)
 * scrolling is up for a +ve COUNT and down for a -ve COUNT
 */
static int
scroll_text (int row1, int row2, int count)
{
	int	r, ncol1;
	text_t	*t_dst, *t_src;
	rend_t	*r_dst, *r_src;

	ncol1 = TermWin.ncol + 1;

	if (selection.op)		/* move selected region too */
	{
		selection.beg.row -= count;
		selection.end.row -= count;
		selection.mark.row -= count;

		/*
		 * could check ranges here and make sure selection is okay
		 * don't scroll into scrollback depending on the region etc,
		 * but leave for now
		 */
	}

	if (count > 0)		/* scroll up */
	{
		int	n, x;

		/*
		 *	if the line scrolls off the top of the screen,
		 *	shift the entire scrollback buffer too
		 */
		if ((row1 == 0) && (current_screen == PRIMARY))
		{
			row1 = -TermWin.saveLines;
			TermWin.nscrolled += count;

			if (TermWin.nscrolled > TermWin.saveLines)
				TermWin.nscrolled = TermWin.saveLines;
		}

		x = (row1 + TermWin.saveLines) * ncol1;
		t_dst = &screen.text [x];
		r_dst = &screen.rend [x];

		n = (row2 - row1 + 1);
		if (count > n)
		{
			count = n;
			n = 0;
		}
		else
		{
			n -= count;
		}

		x += count * ncol1;
		t_src = &screen.text [x];
		r_src = &screen.rend [x];

		/* Forward overlapping memcpy's -- probably OK */
		if (n > 0)
		{
			n *= ncol1;
			MEMCOPY (t_dst, t_src, n * sizeof (text_t));
			MEMCOPY (r_dst, r_src, n * sizeof (rend_t));
			t_dst += n;
			r_dst += n;
		}

#if 0	/* this destroys the '\n' mark for the autowrapped lines */
		*(t_dst-1) = '\0';	/* terminate previous line */
#endif

		/* copy blank lines in at the bottom */
		blank_lines (t_dst, r_dst, count);
	}
	else if (count < 0)		/* scroll down */
	{
		int	x;

		/* do one line at a time to avoid backward overlapping memcpy's */
		x = (row2 + TermWin.saveLines) * ncol1;
		t_dst = &screen.text [x];
		r_dst = &screen.rend [x];

		x += count * ncol1;
		t_src = &screen.text [x];
		r_src = &screen.rend [x];

		for (r = row2; r >= row1 - count; r--)
		{
			MEMCOPY (t_dst, t_src, ncol1 * sizeof (text_t));
			t_dst -= ncol1;
			t_src -= ncol1;

			MEMCOPY (r_dst, r_src, ncol1 * sizeof (rend_t));
			r_dst -= ncol1;
			r_src -= ncol1;
		}

		/* copy blank lines in at the top */
		for (/* vazio */; r >= row1; r--)
		{
			BLANK_FILL (t_dst, r_dst, TermWin.ncol);
			t_dst [TermWin.ncol] = '\0';

			t_dst -= ncol1;
			r_dst -= ncol1;
		}
	}

	return (count);

}	/* end scroll_text */

/*
 ****************************************************************
 *	Trata o Backspace					*
 ****************************************************************
 */
void
scr_backspace (void)
{
	if (selection.op)
		selection_check ();

	if (screen.col == 0 && screen.row > 0)
	{
		screen.row--;
		screen.col = (TermWin.ncol-1);
	}
	else if (screen.flags & Screen_WrapNext)
	{
		int	roffset;

		roffset = (screen.row + TermWin.saveLines) * (TermWin.ncol + 1);
		screen.text [roffset + TermWin.ncol] = '\0';
		screen.flags &= ~Screen_WrapNext;
	}
	else
	{
		scr_gotorc (0, -1, RELATIVE);
	}

}	/* end scr_backspace */

/*
 ****************************************************************
 *	Trata o TAB						*
 ****************************************************************
 */
/*
 * COUNT is +ve, move forward.  COUNT is -ve, move backward
 */
void
scr_tab (int count)
{
	int	i, x;

	x = screen.col;

	if (count > 0) 		/* tab forward */
	{
		for (i = x + 1; i < TermWin.ncol; i++)
		{
			if (tabs [i])
			{
				x = i;
				count--;
				if (!count)
					break;
			}
		}
	}
	else if (count < 0)	/* tab backward */
	{
		for (count = -count, i = x - 1; i >= 0; i--)
		{
			if (tabs [i])
			{
				x = i;
				count--;
				if (!count)
					break;
			}
		}
	}
	else
		return;

	if (x != screen.col)
		scr_gotorc (0, x, R_RELATIVE);

}	/* end scr_tab */

/*
 ****************************************************************
 *	Move o cursor para uma determinada posição		*
 ****************************************************************
 */
/*
 * The relative argument is a pair of flags that specify relative
 * rather than absolute motion.
 */
void
scr_gotorc (int row, int col, int relative)
{
	TermWin.view_start = 0;

	screen.col = (relative & C_RELATIVE) ? screen.col + col : col;

	if (screen.col < 0)
		screen.col = 0;
	else if (screen.col >= TermWin.ncol)
		screen.col = TermWin.ncol - 1;

	if (screen.flags & Screen_WrapNext)
	{
		int	roffset;

		roffset = (screen.row + TermWin.saveLines) * (TermWin.ncol + 1);
		screen.text [roffset + TermWin.ncol] = '\0';
		screen.flags &= ~Screen_WrapNext;
	}

	if (relative & R_RELATIVE)
	{
		if (row > 0)
		{
			if ((screen.row <= screen.bscroll) &&
			(screen.row + row > screen.bscroll))
				screen.row = screen.bscroll;
			else
				screen.row += row;
		}
		else if (row < 0)
		{
			if ((screen.row >= screen.tscroll) &&
			(screen.row + row < screen.tscroll))
				screen.row = screen.tscroll;
			else
				screen.row += row;
		}
	}
	else
	{
		if (screen.flags & Screen_Relative)	/* relative origin mode */
		{
			screen.row = row + screen.tscroll;
			if (screen.row > screen.bscroll)
				screen.row = screen.bscroll;
		}
		else
			screen.row = row;
	}

	if (screen.row < 0)
		screen.row = 0;
	else if (screen.row >= TermWin.nrow)
		screen.row = TermWin.nrow - 1;

}	/* end scr_gotorc */

/*
 ****************************************************************
 *	Move o cursor para baixo e rola o texto, se preciso	*
 ****************************************************************
 */
void
scr_index (int dirn)
{
	int	roffset;

	TermWin.view_start = 0;

	if (screen.flags & Screen_WrapNext)
	{
		roffset = (screen.row + TermWin.saveLines) * (TermWin.ncol + 1);
		screen.text [roffset + TermWin.ncol] = '\0';
		screen.flags &= ~Screen_WrapNext;
	}

	if
	(	(screen.row == screen.bscroll && dirn == UP) ||
		(screen.row == screen.tscroll && dirn == DN)
	)
		scroll_text (screen.tscroll, screen.bscroll, dirn);
	else
		screen.row += dirn;

	if (selection.op)
		selection_check ();

}	/* end scr_index */

/*
 ****************************************************************
 *	Apaga porções de uma linha				*
 ****************************************************************
 */
void
scr_erase_line (int mode)
{
	int	count, roffset;

	roffset = (screen.row + TermWin.saveLines) * (TermWin.ncol + 1);

	TermWin.view_start = 0;

	if (screen.flags & Screen_WrapNext)
	{
		screen.text [roffset + (TermWin.ncol)] = '\0';
		screen.flags &= ~Screen_WrapNext;
	}

	switch (mode)
	{
	    case 0:		/* erase to end */
		roffset += (screen.col);
		count = (TermWin.ncol - screen.col);
		break;

	    case 1:		/* erase to beginning */
		count = (screen.col + 1);
		break;

	    case 2:		/* erase entire */
		count = (TermWin.ncol);
		break;

	    default:
		return;
	}

	BLANK_FILL (&screen.text [roffset], &screen.rend [roffset], count);

	if (selection.op)
		selection_check ();

}	/* end scr_erase_line */

/*
 ****************************************************************
 *	Apaga porções da tela					*
 ****************************************************************
 */
void
scr_erase_screen (int mode)
{
	int	count, roffset;

	roffset = TermWin.saveLines * (TermWin.ncol + 1);
	TermWin.view_start = 0;

	switch (mode)
	{
	    case 0:			/* erase to end */
		scr_erase_line (0);
		count = TermWin.nrow - screen.row - 1;
		roffset += (screen.row + 1) * (TermWin.ncol + 1);
		break;

	    case 1:			/* erase to beginning */
		scr_erase_line (1);
		count = screen.row;
		break;

	    case 2:			/* erase entire */
		count = TermWin.nrow;
		break;

	    default:
		return;
		break;
	}

	blank_lines (&screen.text [roffset], &screen.rend [roffset], count);

}	/* end scr_erase_screen */

/*
 ****************************************************************
 *	Enche a tela com E's					*
 ****************************************************************
 */
void
scr_E (void)
{
	int	r, roffset;

	roffset = (TermWin.saveLines) * (TermWin.ncol + 1);

	TermWin.view_start = 0;

	memset
	(	&screen.text [roffset], 'E',
		TermWin.nrow * (TermWin.ncol + 1) * sizeof (text_t)
	);

	for (r = 0; r < (TermWin.nrow); r++, roffset += (TermWin.ncol + 1))
		screen.text [roffset + TermWin.ncol] = '\0';

}	/* end scr_E */

/*{{{ Insert or Delete COUNT lines and scroll */
/*
 * insdel == +1
 *	delete lines, scroll up the bottom of the screen to fill the gap
 * insdel == -1
 *	insert lines, scroll down the lower lines
 * other values of insdel are undefined
 */
void
scr_insdel_lines (int count, int insdel)
{
	if (screen.row > screen.bscroll)
		return;

	if (count > (screen.bscroll - screen.row + 1))
	{
		if (insdel == DELETE)
			return;

		if (insdel == INSERT)
			count = screen.bscroll - screen.row + 1;
	}

	TermWin.view_start = 0;

	if (screen.flags & Screen_WrapNext)
	{
		int	roffset;

		roffset = (screen.row + TermWin.saveLines) * (TermWin.ncol + 1);

		screen.text [roffset + TermWin.ncol] = '\0';
		screen.flags &= ~Screen_WrapNext;
	}

	scroll_text (screen.row, screen.bscroll, insdel * count);

}	/* end scr_insdel_lines */

/*{{{ Insert or Delete COUNT characters from the current position */
/*
 * insdel == +2, erase  chars
 * insdel == +1, delete chars
 * insdel == -1, insert chars
 */
void
scr_insdel_chars (int count, int insdel)
{
	int	roffset;
	text_t *text, *textend;
	rend_t *rend, *rendend;

	roffset = (screen.row + TermWin.saveLines) * (TermWin.ncol + 1);

	if (insdel == ERASE)
	{
		if (count > screen.col)
			count = screen.col;

		if (count <= 0)
			return;

		screen.col -= count;	/* move backwards */
		insdel = DELETE;	/* delete chars */
	}
	else if (count > TermWin.ncol - screen.col)
	{
		count = TermWin.ncol - screen.col;
	}

	if (count <= 0)
		return;

	TermWin.view_start = 0;

	if (selection.op)
		selection_check ();

	screen.text [roffset + (TermWin.ncol)] = '\0';
	screen.flags &= ~Screen_WrapNext;

	text = &screen.text [roffset + (screen.col)];
	rend = &screen.rend [roffset + (screen.col)];

	if (insdel == DELETE)
	{
		/* overlapping copy */
		for (/* vazio */; *text && text [count]; text++, rend++)
		{
			*text = text [count];
			*rend = rend [count];
		}

		/* fill in the end of the line */
		for (/* vazio */; *text; text++, rend++)
		{
			*text = SPACE_CHAR;
			*rend = FILL_STYLE;
		}
	}
	else
	{
		/* INSERT count characters */
		textend = &screen.text [roffset + (TermWin.ncol-1)];
		rendend = &screen.rend [roffset + (TermWin.ncol-1)];

		for (/* vazio */; textend - count >= text; textend--, rendend--)
		{
			*textend = *(textend-count);
			*rendend = *(rendend-count);
		}

		/* fill in the gap */
		for (/* vazio */; textend >= text; textend--, rendend--)
		{
			*textend = SPACE_CHAR;
			*rendend = FILL_STYLE;
		}
	}

}	/* end scr_insdel_chars */

/*
 ****************************************************************
 *	Estabelece a região de scroll				*
 ****************************************************************
 */
void
scr_scroll_region (int top, int bot)
{
	if (top < 0)
		top = 0;

	if (bot >= TermWin.nrow)
		bot = TermWin.nrow - 1;

	if (top > bot)
		return;

	screen.tscroll = top;
	screen.bscroll = bot;
	scr_gotorc (0, 0, 0);

}	/* end scr_scroll_region */

#if (0)	/*******************************************************/
/*
 ****************************************************************
 *	Modifica a visibilidade do cursor			*
 ****************************************************************
 */
void
scr_cursor_visible (int mode)
{
	if (mode)
		screen.flags |= Screen_VisibleCursor;
	else
		screen.flags &= ~Screen_VisibleCursor;

}	/* end scr_cursor_visible */

/*
 ****************************************************************
 *	x
 ****************************************************************
 */
void
scr_autowrap (int mode)
{
	if (mode)
		screen.flags |= Screen_Autowrap;
	else
		screen.flags &= ~Screen_Autowrap;

}	/* end scr_autowrap */

/*
 ****************************************************************
 *	Estabelece a origem relativa à margem superior		*
 ****************************************************************
 *
 *	In absolute origin mode, line numbers are counted relative to top margin
 *	of screen, the cursor can be moved outside the scrolling region. In
 *	relative mode line numbers are relative to top margin of scrolling
 *	region and the cursor cannot be moved outside
 */
void
scr_relative_origin (int mode)
{
	if (mode)
		screen.flags |= Screen_Relative;
	else
		screen.flags &= ~Screen_Relative;

	scr_gotorc (0, 0, 0);

}	/* end scr_relative_origin */

/*
 ****************************************************************
 *	Estabelece o modo de inserção				*
 ****************************************************************
 */
void
scr_insert_mode (int mode)
{
	if (mode)
		screen.flags |= Screen_Insert;
	else
		screen.flags &= ~Screen_Insert;

}	/* end scr_insert_mode */
#endif	/*******************************************************/

/*
 ****************************************************************
 *	Move o display de acordo com a barra de rolamento	*
 ****************************************************************
 */
int
scr_move_to (int y, int len)
{
	int	start = TermWin.view_start;

	TermWin.view_start = ((len - y) * (TermWin.nrow - 1 + TermWin.nscrolled) / len)
				- (TermWin.nrow - 1);

	if (TermWin.view_start < 0)
		TermWin.view_start = 0;
	else if (TermWin.view_start > TermWin.nscrolled)
		TermWin.view_start = TermWin.nscrolled;

	return (TermWin.view_start - start);

}	/* end scr_move_to */

/*
 ****************************************************************
 *	Pagina a tela para cima/baixo NLINES			*
 ****************************************************************
 */
int
scr_page (int dirn, int nlines)
{
	int	start = TermWin.view_start;

	if (!dirn || !nlines)
		return (0);

	if (nlines <= 0)
		nlines = 1;
	else if (nlines > TermWin.nrow)
		nlines = TermWin.nrow;

	TermWin.view_start += nlines * dirn;

	if (TermWin.view_start < 0)
		TermWin.view_start = 0;
	else if (TermWin.view_start > TermWin.nscrolled)
		TermWin.view_start = TermWin.nscrolled;

	return (TermWin.view_start - start);

}	/* end scr_page */

/*{{{ selection service functions */
/*
 * If (row,col) is within a selected region of text, remove the selection
 */
static void
selection_check (void)
{
   int c1, c2, r1, r2;

   if (current_screen != selection.screen)
     return;

   if ((selection.mark.row < -TermWin.nscrolled) ||
       (selection.mark.row >= TermWin.nrow) ||
       (selection.beg.row < -TermWin.nscrolled) ||
       (selection.beg.row >= TermWin.nrow) ||
       (selection.end.row < -TermWin.nscrolled) ||
       (selection.end.row >= TermWin.nrow))
     {
	selection_reset ();
	return;
     }

   r1 = (screen.row - TermWin.view_start);
   c1 = ((r1 - selection.mark.row) * (r1 - selection.end.row));

   /* selection.mark.row > screen.row - TermWin.view_start
    * or
    * selection.end.row > screen.row - TermWin.view_start
    */
   if (c1 < 0)
     selection_reset ();
   /* selection.mark.row == screen.row || selection.end.row == screen.row */
   else if (c1 == 0)
     {
	/* We're on the same row as the start or end of selection */
	if ((selection.mark.row < selection.end.row) ||
	    ((selection.mark.row == selection.end.row) &&
	     (selection.mark.col < selection.end.col)))
	  {
	     r1 = selection.mark.row;
	     c1 = selection.mark.col;
	     r2 = selection.end.row;
	     c2 = selection.end.col;
	  }
	else
	  {
	     r1 = selection.end.row;
	     c1 = selection.end.col;
	     r2 = selection.mark.row;
	     c2 = selection.mark.col;
	  }
	if ((screen.row == r1) && (screen.row == r2))
	  {
	     if ((screen.col >= c1) && (screen.col <= c2))
	       selection_reset ();
	  }
	else if (((screen.row == r1) && (screen.col >= c1)) ||
		 ((screen.row == r2) && (screen.col <= c2)))
	  selection_reset ();
     }
}

#if 0
static void
selection_range (int firstr, int lastr)
{
   if (firstr >= lastr ||
       firstr < 0 || firstr >= TermWin.nrow ||
       lastr <= 0 || lastr > TermWin.nrow)
    return;
   selection.firstr = firstr;
   selection.lastr = lastr;
}
#endif
/*}}}*/

/*{{{ make selection */
/*
 * make the selection currently delimited by the selection end markers
 */
void
selection_make (Time tm)
{
   char * str;
   int r, startr, startc, endr, endc;
   int roffset;			/* row offset */

   switch (selection.op) {
    case SELECTION_CONT:
      break;

    case SELECTION_INIT:
      selection_reset ();
      selection.end.row = selection.mark.row = selection.beg.row;
      selection.end.col = selection.mark.col = selection.beg.col;
      /*drop*/
    case SELECTION_BEGIN:
      selection.op = SELECTION_DONE;
      /*drop*/
    default:
      return;
      break;
   }
   selection.op = SELECTION_DONE;

   xtfree (selection.text);
   selection.text = NULL;
   selection.len = 0;

   selection.screen = current_screen;
   /* Set start/end row/col to point to the selection endpoints */
   if (selection.end.row < selection.mark.row ||
       (selection.end.row == selection.mark.row &&
	selection.end.col <= selection.mark.col))
     {
	startr = selection.end.row; endr = selection.mark.row;
	startc = selection.end.col; endc = selection.mark.col;
     }
   else
     {
	startr = selection.mark.row; endr = selection.end.row;
	startc = selection.mark.col; endc = selection.end.col;
     }

   if ((startr < -TermWin.nscrolled || endr >= TermWin.nrow))
     {
	selection_reset ();
	return;
     }

   str = xtmalloc (((endr - startr + 1) * (TermWin.ncol + 1) + 1) * sizeof (char));
   selection.text = str;
   *str = '\0';

   /* save all points between start and end with selection flag */
   roffset = ((startr + TermWin.saveLines) * (TermWin.ncol + 1));
   for (r = startr; r <= endr; r++, roffset += (TermWin.ncol + 1))
     {
	int c;
	int c1 = (r == startr ? startc : 0);
	int c2 = (r == endr ? endc : (TermWin.ncol-1));

	for (c = c1; c <= c2; c++)
	  *str++ = screen.text [roffset + c];

	/*
	 * end-of-line and not autowrap
	 * remove trailing space, but don't remove an entire line!
	 */
	if (c2 == (TermWin.ncol-1) && !screen.text [roffset + (TermWin.ncol)])
	  {
	     str--;
	     for (c = c2; c >= c1 && isspace (*str); c--) str--;
	     str++;
	     *str++ = '\n';
	  }
     }
   *str = '\0';

   selection.len = strlen (selection.text);
   if (selection.len <= 0)
     return;
   XSetSelectionOwner (dpy, XA_PRIMARY, TermWin.vt, tm);
   if (XGetSelectionOwner (dpy, XA_PRIMARY) != TermWin.vt)
     msg ("can't get primary selection");

   /* Place in CUT_BUFFER0 for backup */
   XChangeProperty (dpy, Xroot, XA_CUT_BUFFER0,
		    XA_STRING, 8, PropModeReplace,
		    (uchar *)selection.text, selection.len);
}
/*}}}*/

/*{{{ respond to a request for our current selection */
void
selection_send (XSelectionRequestEvent * rq)
{
   XEvent ev;
   static Atom xa_targets = None;
   if (xa_targets == None)
     xa_targets = XInternAtom (dpy, "TARGETS", False);

   ev.xselection.type      = SelectionNotify;
   ev.xselection.property  = None;
   ev.xselection.display   = rq->display;
   ev.xselection.requestor = rq->requestor;
   ev.xselection.selection = rq->selection;
   ev.xselection.target	   = rq->target;
   ev.xselection.time      = rq->time;

    if (rq->target == xa_targets)
     {
	/*
	 * On some systems, the Atom typedef is 64 bits wide.
	 * We need to have a typedef that is exactly 32 bits wide,
	 * because a format of 64 is not allowed by the X11 protocol.
	 */
	typedef ulong Atom32;

	Atom32 target_list[2];

 	target_list[0] = (Atom32) xa_targets;
 	target_list[1] = (Atom32) XA_STRING;

 	XChangeProperty (dpy, rq->requestor, rq->property,
 			 xa_targets, 8*sizeof (target_list[0]), PropModeReplace,
 			 (uchar *)target_list,
			 sizeof (target_list)/sizeof (target_list[0]));
 	ev.xselection.property = rq->property;
     }
   else if (rq->target == XA_STRING)
     {
	XChangeProperty (dpy, rq->requestor, rq->property,
			 XA_STRING, 8, PropModeReplace,
			 (uchar *)selection.text, selection.len);
	ev.xselection.property = rq->property;
     }
   XSendEvent (dpy, rq->requestor, False, 0, &ev);
}
/*}}}*/

/*{{{ paste selection */
static void
PasteIt (char * data, unsigned int nitems)
{
   char * p = data, * pmax = data + nitems;

   for (nitems = 0; p < pmax; p++)
     {
	/* do newline -> carriage-return mapping */
	if (*p == '\n')
	  {
	     char cr = '\r';
	     cmd_write (data, nitems);
	     cmd_write (&cr, 1);
	     data += (nitems+1);
	     nitems = 0;
	  }
	else
	  nitems++;
     }
   if (nitems)
     cmd_write (data, nitems);
}

/*
 * Respond to a notification that a primary selection has been sent
 */
void
selection_paste (Window win, unsigned prop, int Delete)
{
   long nread;
   ulong bytes_after;

   if (prop == None)
     return;

   nread = 0;
   do
     {
	char * data;
	Atom actual_type;
	int actual_fmt;
	ulong nitems;

	if ((XGetWindowProperty (dpy, win, prop,
				 nread/4, PROP_SIZE, Delete,
				 AnyPropertyType, &actual_type, &actual_fmt,
				 &nitems, &bytes_after,
				 (uchar **)&data) != Success) ||
	    (actual_type != XA_STRING))
	  {
	     XFree (data);
	     return;
	  }

	nread += nitems;
	PasteIt (data, nitems);

	XFree (data);
     }
   while (bytes_after > 0);
}
/*}}}*/

/*{{{ Request the current primary selection */
void
selection_request (Time tm, int x, int y)
{
   /* is release within the window? */
   if (x < 0 || y < 0 || x >= TermWin.width || y >= TermWin.height)
     return;

   if (selection.text != NULL)
     {
	/* internal selection */
	PasteIt (selection.text, selection.len);
     }
   else if (XGetSelectionOwner (dpy, XA_PRIMARY) == None)
     {
	/* no primary selection - use CUT_BUFFER0 */
	selection_paste (Xroot, XA_CUT_BUFFER0, False);
     }
   else
     {
	Atom prop = XInternAtom (dpy, "VT_SELECTION", False);
	XConvertSelection (dpy, XA_PRIMARY, XA_STRING,
			   prop, TermWin.vt, tm);
     }
}
/*}}}*/

/*{{{ Clear the current selection */
void
selection_reset (void)
{
   int x, nrow = TermWin.nrow;

   selection.op = SELECTION_CLEAR;
   selection.end.row = selection.mark.row = 0;
   selection.end.col = selection.mark.col = 0;

   if (current_screen == PRIMARY)
     nrow += TermWin.saveLines;

   for (x = 0; x < nrow * (TermWin.ncol + 1); x++)
     screen.rend [x] &= ~(RS_Select);
}

void
selection_clear (void)
{
   xtfree (selection.text);
   selection.text = NULL;
   selection.len = 0;

   selection.op = SELECTION_CLEAR;
   selection_reset ();
}
/*}}}*/

/*{{{ mark selected points (used by selection_extend) */
static void
selection_setclr (int set, int startr, int startc, int endr, int endc)
{
   int r, roffset = ((startr + TermWin.saveLines) * (TermWin.ncol + 1));

   /* startr <= endr */
   if ((startr < -TermWin.nscrolled) || (endr >= TermWin.nrow))
     {
	selection_reset ();
	return;
     }

   for (r = startr; r <= endr; r++)
     {
	int c1 = (r == startr ? startc : 0);
	int c2 = (r == endr ? endc : (TermWin.ncol-1));

	for (/*nil*/; c1 <= c2; c1++)
	  {
	     if (set)
	       screen.rend [roffset + c1] |= RS_Select;
	     else
	       screen.rend [roffset + c1] &= ~(RS_Select);
	  }
	roffset += (TermWin.ncol + 1);
     }
}

/*{{{ start a selection at the specified col/row */
static void
selection_start_colrow (int col, int row)
{
   if (selection.op)
     {
	/* startr <= endr */
	if ((selection.end.row < -TermWin.nscrolled) ||
	    (selection.mark.row < - TermWin.nscrolled))
	  {
	     selection_reset ();
	  }
	else			/* direction of new selection */
	  {
	     if (selection.end.row < selection.mark.row ||
		 (selection.end.row == selection.mark.row &&
		  selection.end.col <= selection.mark.col))
	       selection_setclr (0,    /* up */
				 selection.end.row, selection.end.col,
				 selection.mark.row, selection.mark.col);
	     else
	       selection_setclr (0,	/* down */
				 selection.mark.row, selection.mark.col,
				 selection.end.row, selection.end.col);
	  }
     }
   selection.op = SELECTION_INIT;

   selection.beg.col = col;
   selection.beg.row = row;
   selection.beg.row -= TermWin.view_start;
}

/*
 * start a selection at the specified x/y pixel location
 */
void
selection_start (int x, int y)
{
   selection_start_colrow (Pixel2Col (x), Pixel2Row (y));
}
/*}}}*/

/*{{{ extend the selection to the specified col/row */
static void
selection_extend_colrow (int col, int row)
{
   int old_row, old_col, old_dirn, dirn;

   switch (selection.op) {
    case SELECTION_INIT:
      selection_reset ();
      selection.end.col = selection.mark.col = selection.beg.col;
      selection.end.row = selection.mark.row = selection.beg.row;
      /*drop*/
    case SELECTION_BEGIN:
      selection.op = SELECTION_BEGIN;
      break;

    case SELECTION_DONE:
    case SELECTION_CONT:
      selection.op = SELECTION_CONT;
      break;

    case SELECTION_CLEAR:
      selection_start_colrow (col, row);
      /*drop*/
    default:
      return;
      break;
   }

   /* Remember old selection for virtual removal */
   old_row = selection.end.row;
   old_col = selection.end.col;

   if ((old_row < -TermWin.nscrolled) ||
       (selection.mark.row < -TermWin.nscrolled))
     {
	selection_reset ();
	return;
     }

   /* Figure out where new selection is */
   selection.end.col = col;
   selection.end.row = row;

   if (selection.end.col < 0) selection.end.col = 0;
   else if (selection.end.col >= TermWin.ncol)
     selection.end.col = (TermWin.ncol-1);

   selection.end.row -= TermWin.view_start;
   if (selection.end.row < -TermWin.nscrolled)
     {
	selection_reset ();
	return;
     }
   else if (selection.end.row >= TermWin.nrow)
     selection.end.row = (TermWin.nrow-1);

   if ((selection.op == SELECTION_BEGIN) &&
       ((selection.end.col != selection.mark.col) ||
	(selection.end.row != selection.mark.row)))
     selection.op = SELECTION_CONT;

   /* If new selection is same as old selection just return
    * or if no highlighting was requested
    */
   if (selection.end.row == old_row && selection.end.col == old_col)
     return;

   /* virtual removal -- delete old highlighting and replace with new */

   /* determine direction of old selection */
   old_dirn = ((old_row < selection.mark.row ||
		(old_row == selection.mark.row &&
		 old_col <= selection.mark.col)) ? UP : DN);

   /* determine direction of new selection */
   dirn = ((selection.end.row < selection.mark.row ||
	    (selection.end.row == selection.mark.row &&
	     selection.end.col <= selection.mark.col)) ? UP : DN);

   /* If old and new direction are different, clear old, set new */
   if (dirn != old_dirn)
     {
	if (old_dirn == UP)
	  {
	     selection_setclr (0,
			       old_row, old_col,
			       selection.mark.row, selection.mark.col);
	     selection_setclr (1,
			       selection.mark.row, selection.mark.col,
			       selection.end.row, selection.end.col);
	  }
	else
	  {
	     selection_setclr (0,
			       selection.mark.row, selection.mark.col,
			       old_row, old_col);
	     selection_setclr (1,
			       selection.end.row, selection.end.col,
			       selection.mark.row, selection.mark.col);
	  }
     }
   else
     {
	if (old_dirn == UP)
	  {
	     if (old_row < selection.end.row ||
		 (old_row == selection.end.row &&
		  old_col < selection.end.col))
	       {
		  selection_setclr (0,
				    old_row, old_col,
				    selection.end.row, selection.end.col);
		  selection_setclr (1,
				    selection.end.row, selection.end.col,
				    selection.end.row, selection.end.col);
	       }
	     else
	       {
		  selection_setclr (1,
				    selection.end.row, selection.end.col,
				    old_row, old_col);
	       }
	  }
	else
	  {
	     if (selection.end.row < old_row ||
		 (selection.end.row == old_row &&
		  selection.end.col < old_col))
	       {
		  selection_setclr (0,
				    selection.end.row, selection.end.col,
				    old_row, old_col);
		  selection_setclr (1,
				    selection.end.row, selection.end.col,
				    selection.end.row, selection.end.col);
	       }
	     else
	       {
		  selection_setclr (1,
				    old_row, old_col,
				    selection.end.row, selection.end.col);
	       }
	  }
     }
}

/*
 * extend the selection to the specified x/y pixel location
 */
void
selection_extend (int x, int y)
{
   selection_extend_colrow (Pixel2Col (x), Pixel2Row (y));
}
/*}}}*/

/*{{{ double/triple click selection */
/*
 * by Edward. Der-Hua Liu, Taiwan
 * cut char support added by A. Haritsis <ah@doc.ic.ac.uk>
 */
void
selection_click (int clicks, int x, int y)
{
	int	ncol1 = TermWin.ncol + 1;

   if (clicks <= 1)
     {
	selection_start (x, y);		/* single click */
     }
   else
     {
	int beg_c, end_c, beg_r, end_r;
	text_t * text;

	/* ensure rows/columns are on the screen */
	x = Pixel2Col (x);
	x = (x <= 0 ? 0 : (x >= TermWin.ncol ? (TermWin.ncol-1) : x));
	beg_c = end_c = x;

	y = Pixel2Row (y);
	y = (y <= 0 ? 0 : (y >= TermWin.nrow ? (TermWin.nrow-1) : y));
	beg_r = end_r = y;

	switch (clicks) {
	 case 3:
	   /*
	    * triple click
	    */
	   beg_c = 0;
	   end_c = (TermWin.ncol-1);
	   break;

	 case 2:
	   /*
	    * double click: handle autowrapped lines
	    */
	   for (text = (screen.text +
			(beg_r + TermWin.saveLines - TermWin.view_start) * ncol1);
		/*forever*/;
		beg_r--, text -= ncol1)
	     {
		while (beg_c > 0 &&
		       !strchr (rs_cutchars, text [beg_c-1]))
		  beg_c--;

 		if (beg_c == 0 &&
		    beg_r > (TermWin.view_start - TermWin.nscrolled) &&
		    *(text-1) == '\n' &&
		    !strchr (rs_cutchars, *(text-2)))
		  beg_c = (TermWin.ncol-1);
		else
		  break;
	     }

	   for (text = (screen.text +
			(end_r + TermWin.saveLines - TermWin.view_start) * ncol1);
		/*forever*/;
		end_r++, text += ncol1)
	     {
		while (end_c < (TermWin.ncol-1) &&
		       !strchr (rs_cutchars, text [end_c+1]))
		  end_c++;

		if (end_c == (TermWin.ncol-1) &&
		    end_r < (TermWin.view_start + TermWin.nrow-1) &&
		    text [TermWin.ncol] == '\n' &&
		    !strchr (rs_cutchars, text [ncol1]))
		  end_c = 0;
		else
		  break;
	     }
	   break;

	 default:
	   return;
	   break;
	}
	selection_start_colrow (beg_c, beg_r);
	selection_extend_colrow (end_c, end_r);
     }
}

#if (0)	/*******************************************************/
void
scr_report_position (void)
{
	cmd_printf ("\033[%d;%dR", screen.row + 1, screen.col + 1);

}	/* end scr_report_position */
#endif	/*******************************************************/

/*
 * choose a font
 */
void
scr_charset_choose (int set)
{
	screen.charset = set;
	set_font_style ();

}	/* end scr_charset_choose */

/*
 * Set a font
 */
void
scr_charset_set (int set, unsigned int ch)
{
	charsets [set] = (char) ch;
	set_font_style ();

}	/* end scr_charset_set */

/*
 ****************************************************************
 *	x
 ****************************************************************
 */
/*
 * for the box starting at x, y with size width, height
 * touch the displayed values
 */
void
scr_expose (int x, int y, int width, int height)
{
	int	row, col, end_row, end_col;

	if (drawn_text == NULL)
		return;

	col = Pixel2Col (x);
	row = Pixel2Row (y);

	if (col < 0)
		col = 0;
	else if (col >= TermWin.ncol)
		col = TermWin.ncol - 1;

	if (row < 0)
		row = 0;
	else if (row >= TermWin.nrow)
		row = TermWin.nrow - 1;

	end_col = col + 1 + Pixel2Width (width);
	end_row = row + 1 + Pixel2Height (height);

	if (end_row >= TermWin.nrow)
		end_row = TermWin.nrow - 1;

	if (end_col >= TermWin.ncol)
		end_col = TermWin.ncol - 1;

	width = end_col - col + 1;

	for (/* vazio */; row <= end_row; row++)
	{
		int	roffset = (col + row * (TermWin.ncol + 1));

		memset (&drawn_text [roffset], 0, width * sizeof (text_t));
		memset (&drawn_rend [roffset], 0, width * sizeof (rend_t));
	}

}	/* end scr_expose */

#if (0)	/*******************************************************/
/*
 ****************************************************************
 *	x
 ****************************************************************
 */
void
scr_touch (void)
{
	scr_expose (0, 0, TermWin.width, TermWin.height);

}	/* end scr_touch */
#endif	/*******************************************************/

/*
 ****************************************************************
 *	Atualiza a imagem na tela				*
 ****************************************************************
 */
/*
 * refresh the region defined by rows STARTR and ENDR, inclusively.
 *
 * Actually draws to the X window
 * For X related speed-ups, this is a good place to fiddle.
 * The arrays drawn_text and drawn_rend contain what I
 * believe is currently shown on the screen. The arrays in screen contain
 * what should be displayed. This routine can decide how to refresh the
 * screen. Calls in command.c decide when to refresh.
 */
/*----------------------------------------------------------------------*/

#define drawBuffer			(TermWin.vt)
#define drawString(strFunc)		strFunc				\
					(	dpy, drawBuffer,	\
						TermWin.gc,		\
						xpixel, ypixel,		\
						linebuf, count		\
					)

/* bold characters - order of preference:
 * 1 - change the foreground color to colorBD
 * 2 - change the foreground color to bright
 * 3 - use boldFont
 * 4 - simulate with overstrike
 */
# define MonoBold(x)	(((x) & RS_Bold) && fore == fgColor)

/* # define MonoBold(x)	((((x) & RS_Bold) && fore == fgColor) || (((x) & RS_Blink) && back == bgColor)) */
/*
 * blink simulated by simulated bold (overstrike) seems a bit farfetched
 * comment this out (v2.17 - 04APR96) and see how many people complain
 */

void
scr_refresh (int type)
{
	static int	last_xcursor = 0;
	int		r, roffset, doffset, xcursor, ncol1;

	/*
	 *	Verifica se a janela é visível.
	 */
	if (type == NO_REFRESH)
		return;

	ncol1 = TermWin.ncol + 1;

	if (last_xcursor < (TermWin.nrow * ncol1))
	{
		/* make sure to update it */
	   	drawn_rend [last_xcursor] = RS_attrMask;
	}

	xcursor = ((screen.row + TermWin.saveLines) * ncol1 + screen.col);

	last_xcursor = (screen.row + TermWin.view_start);

	if (last_xcursor >= TermWin.nrow)
	{
		last_xcursor = 0;
	}
	else
	{
		last_xcursor *= ncol1;

		if (screen.flags & Screen_VisibleCursor)
			screen.rend [xcursor] |= RS_Cursor;
	}

	last_xcursor += screen.col;

#ifdef USE_XCOPYAREA
	/*
	 * scroll using bitblt wherever possible
	 * a dirty approximation will ignore the rendition field here
	 * and fix it up later
	 */
	if (type == FAST_REFRESH)
	for (r = 0; r < TermWin.nrow; r++)
	{
		int k;
		int doffset = r * ncol1;
		int roffset = doffset + ((TermWin.saveLines - TermWin.view_start) *
				     ncol1);

		if
		(	!memcmp
			(	&drawn_text [doffset],
				&screen.text [roffset],
				TermWin.ncol * sizeof (text_t)
			)
		)
			continue;

		/*
		 * look for a similar line
		 */
		for
		(	doffset = 0, k = 0;
			k < TermWin.nrow;
			k++, doffset += ncol1
		)
		{
			if
			(	!memcmp
				(	&drawn_text [doffset],
					&screen.text [roffset],
					TermWin.ncol * sizeof (text_t)
				)
			)
			break;
		}

		/* found it */
		if (k < TermWin.nrow)
		{
			int count;
			int j = r;

			roffset += ncol1;
			doffset += ncol1;
			r++;

			for
			(	count = 1;
				(	(r < TermWin.nrow) &&
					!memcmp
					(	&drawn_text [doffset],
						&screen.text [roffset],
						TermWin.ncol * sizeof (text_t)
					)
				);
				count++, r++
			)
			{
				roffset += ncol1;
				doffset += ncol1;
			}

			r--;
			XCopyArea
			(	dpy,
				TermWin.vt,
				TermWin.vt,
				TermWin.gc,
				TermWin_internalBorder, Row2Pixel (k),
				Width2Pixel (1), Height2Pixel (count),
				TermWin_internalBorder, Row2Pixel (j)
			);

			/*
			 * Forward overlapping memcpy's are probably OK,
			 * but backwards doesn't work on SunOS 4.1.3
			 */
			k *= ncol1;
			j *= ncol1;

			if (k > j)
			{
				while (count-- > 0)
				{
					MEMCOPY
					(	&drawn_text [j],
						&drawn_text [k],
						count * ncol1 * sizeof (text_t)
					);

					MEMCOPY
					(	&drawn_rend [j],
						&drawn_rend [k],
						count * ncol1 * sizeof (rend_t)
					);

					k += ncol1;
					j += ncol1;
				}
			}
			else
			{
				k += (count - 1) * ncol1;
				j += (count - 1) * ncol1;

				while (count-- > 0)
				{
					MEMCOPY
					(	&drawn_text [j],
						&drawn_text [k],
						ncol1 * sizeof (text_t)
					);

					MEMCOPY
					(	&drawn_rend [j],
						&drawn_rend [k],
						ncol1 * sizeof (rend_t)
					);

					k -= ncol1;
					j -= ncol1;
				}
			}
		}
	}
#endif	/* USE_XCOPYAREA */

	doffset = 0 * ncol1;
	roffset = doffset + ((TermWin.saveLines - TermWin.view_start) * ncol1);

	/* For a first cut, do it one character at a time */
	for
	(	r = 0;
		r < TermWin.nrow;
		roffset += ncol1, doffset += ncol1, r++
	)
	{
		int	c;
		int	ypixel = TermWin.font->ascent + Row2Pixel (r);

	/* fast way to avoid the next loop (most of the time) ? */
		if
		(	!memcmp
			(	&drawn_text [doffset],
				&screen.text [roffset],
				TermWin.ncol * sizeof (text_t)
			) &&
			!memcmp
			(	&drawn_rend [doffset],
				&screen.rend [roffset],
				TermWin.ncol * sizeof (rend_t)
			)
		)
		continue;

		for (c = 0; c < TermWin.ncol; c++)
		{
			int		count;
			int		x = roffset + c;
			int		x1 = doffset + c;
			int		fore, back, rend;
			XGCValues	gcvalue;	/* GC values */
			ulong		gcmask = 0;	/* GC mask */
			int		outlineCursor = False;	/* block cursor */
			int		xpixel = Col2Pixel (c);

			if
			(	(drawn_text [x1] == screen.text [x]) &&
				(drawn_rend [x1] == screen.rend [x])
			)
				continue;

			drawn_text [x1] = screen.text [x];
			drawn_rend [x1] = screen.rend [x];
			linebuf [0] = screen.text [x];
			rend = screen.rend [x];

			x++; c++, x1++;
			for
			(	count = 1;
				(c < TermWin.ncol &&
				(rend == screen.rend [x]) &&
				(drawn_text [x1] != screen.text [x] ||
				drawn_rend [x1] != screen.rend [x] ||
				(c+1 < TermWin.ncol &&
				drawn_text [x1+1] != screen.text [x+1])));
				count++, x++, c++, x1++
			)
			{
				drawn_text [x1] = screen.text [x];
				drawn_rend [x1] = screen.rend [x];
				linebuf [count] = screen.text [x];
			}

			c--;
			linebuf [count] ='\0';	/* zero-terminate */
			fore = GET_FGCOLOR (rend);
			back = GET_BGCOLOR (rend);
			rend = GET_ATTR (rend);

			if (rend)
			{
				int rvid = 0;

				if ((rend & (RS_RVid|RS_Select)) == (RS_RVid|RS_Select))
					rend &= ~(RS_RVid|RS_Select);
				else if (rend & (RS_RVid|RS_Select))
					rvid = 1;

				if (rend & RS_Cursor)
				{
					if (!TermWin.focus)
					{
						outlineCursor = True;
						rend &= ~(RS_Cursor);
					}
					else
						rvid = (!rvid
							|| (PixColors [cursorColor] != PixColors [bgColor])
						);
				}

				/* swap foreground/background colors */
				if (rvid)
				{
					int tmp = back; back = fore; fore = tmp;
				}

				/*
				 * do some font character switching
				 */
				switch (rend & RS_fontMask)
				{
				    case RS_acsFont:
					for (x = 0; x < count; x++)
					{
						int	ch = linebuf[x];

						if (ch >= 0x5F && ch < 0x7F)
						{
							linebuf [x] = (ch == 0x5F ?
								0x7F : ch - 0x5F);
						}
					}
					break;

				    case RS_ukFont:
					for (x = 0; x < count; x++)
					{
						int	ch = linebuf[x];

						if (ch == '#')
							linebuf [x] = '\036';
					}
					break;
				}
			}

			if (fore != fgColor)
			{
				gcvalue.foreground = PixColors [fore];
				gcmask |= GCForeground;
			}
#ifndef NO_BOLDUNDERLINE
			else
			{
				if (rend & RS_Bold)  /* do bold first */
				{
					gcvalue.foreground = PixColors [colorBD];
					if (gcvalue.foreground != PixColors [fgColor])
					{
						gcmask |= GCForeground;
						rend &= ~RS_Bold;
					}
				}
				else if (rend & RS_Uline)
				{
					gcvalue.foreground = PixColors [colorUL];
					if (gcvalue.foreground != PixColors [fgColor])
					{
						gcmask |= GCForeground;
						rend &= ~RS_Uline;
					}
				}
			}
#endif	/* NO_BOLDUNDERLINE */

			if (back != bgColor)
			{
				gcvalue.background = PixColors [back];
				gcmask |= GCBackground;
			}

			if
			(	(rend & RS_Cursor) &&
				(PixColors [cursorColor] != PixColors [bgColor])
			)
			{
				gcvalue.background = PixColors [cursorColor];
				gcmask |= GCBackground;

				if (PixColors [cursorColor2] != PixColors [fgColor])
				{
					gcvalue.foreground = PixColors [cursorColor2];
					gcmask |= GCForeground;
				}
			}

			if (gcmask)
				XChangeGC (dpy, TermWin.gc, gcmask, &gcvalue);

#ifndef NO_BOLDFONT
			if (MonoBold (rend) && TermWin.boldFont != NULL)
			{
				XSetFont (dpy, TermWin.gc, TermWin.boldFont->fid);

				drawString (XDrawImageString);

				XSetFont (dpy, TermWin.gc, TermWin.font->fid);
			}
			else
#endif	/* NO_BOLDFONT */
			{
				drawString (XDrawImageString);

#ifndef NO_BOLDOVERSTRIKE
				if (MonoBold (rend))
				{
					xpixel++;
					drawString (XDrawString);
					xpixel--;
				}
#endif	/* NO_BOLDOVERSTRIKE */
			}

			/*
			 * On the smallest font, underline overwrites next row
			 */
			if ((rend & RS_Uline) && (TermWin.font->descent > 1))
			{
				XDrawLine
				(	dpy,
					drawBuffer,
					TermWin.gc,
					xpixel,
					ypixel + 1,
					xpixel + Width2Pixel (count) - 1,
					ypixel + 1
				);
			}

			if (outlineCursor)
			{
				if (PixColors [cursorColor] != PixColors [bgColor])
				{
					gcvalue.foreground = PixColors [cursorColor];
					gcmask |= GCForeground;
					XChangeGC (dpy, TermWin.gc, gcmask, &gcvalue);
				}

				XDrawRectangle
				(	dpy,
					drawBuffer,
					TermWin.gc,
					xpixel,
					(ypixel - TermWin.font->ascent),
					Width2Pixel (1) - 1,
					Height2Pixel (1) - 1
				);
			}

			if (gcmask)
			{
				/* restore normal colors */
				gcvalue.foreground = PixColors [fgColor];
				gcvalue.background = PixColors [bgColor];
				XChangeGC (dpy, TermWin.gc, gcmask, &gcvalue);
			}
		}
	}

	if (screen.flags & Screen_VisibleCursor)
		screen.rend [xcursor] &= ~(RS_Cursor);

	/* XFlush (dpy); */

#undef MonoBold
#undef drawStringPrep
#undef drawString
#undef drawBuffer

}	/* end scr_refresh */

/*
 ****************************************************************
 *	Troca a tabulação					*
 ****************************************************************
 */
/*
 *  -1 = clear all tabs
 *  +0 = clear tab stop at current column
 *  +1 = set   tab stop at current column
 */
void
scr_set_tab (int mode)
{
   if (mode < 0)
     memset (tabs, 0, TermWin.ncol * sizeof (char));
   else if (screen.col < TermWin.ncol)
     tabs [screen.col] = (mode != 0);
}
/*}}}*/

#if (0)	/*******************************************************/
/*
 ****************************************************************
 *	Troca o reverso						*
 ****************************************************************
 */
void
scr_rvideo_mode (int mode)
{
   if (rvideo != mode)
     {
	register int x, count;

	rvideo = mode;
	rstyle ^= RS_RVid;

	count = (TermWin.nrow + TermWin.saveLines) * (TermWin.ncol+1);
	for (x = 0; x < count; x++) screen.rend [x] ^= RS_RVid;
	scr_refresh (SLOW_REFRESH);
     }
}
#endif	/*******************************************************/

/*
 ****************************************************************
 *	Trata o caractere Bell					*
 ****************************************************************
 */
void
scr_bell (void)
{
	XBell (dpy, 0);

}	/* end scr_bell */

/*{{{ Mouse Reporting */
/* add the bits:
 * @ 1 - shift
 * @ 2 - meta
 * @ 4 - ctrl
 */
#define ButtonNumber(x) ((x) == AnyButton ? 3 : ((x) - Button1))
#define KeyState(x) ((((x)&(ShiftMask|ControlMask))+(((x)&Mod1Mask)?2:0))<<2)
void
mouse_report (XButtonEvent * ev)
{
   cmd_printf ("\033[M%c%c%c",
	      (040 + ButtonNumber (ev->button) + KeyState (ev->state)),
	      (041 + Pixel2Col (ev->x)),
	      (041 + Pixel2Row (ev->y)));
}

#if 0	/* X11 mouse tracking: not yet - maybe never! */
void
mouse_tracking (int report, int x, int y, int firstrow, int lastrow)
{
   static int top, bot;

   if (report)
     {
	/* If either coordinate is past the end of the line:
	 * "ESC [ T CxCyCxCyCxCy"
	 * The parameters are begx, begy, endx, endy,
	 * mousex, and mousey */

	if ((selection.beg.row < selection.end.row) ||
	    ((selection.beg.row == selection.end.row) &&
	     (selection.beg.col < selection.end.col)))
	  {
	     if (selection.beg.row >= top && selection.end.row <= bot)
	       cmd_printf ("\033[t");	/* start/end are valid locations */
	     else
	       cmd_printf ("\033[T%c%c%c%c",
			  selection.beg.col + 1, selection.beg.row + 1,
			  selection.end.col + 1, selection.end.row + 1);
	  }
	else
	  {
	     if (selection.end.row >= top && selection.beg.row <= bot)
	       cmd_printf ("\033[t");	/* start/end are valid locations */
	     else
	       cmd_printf ("\033[T%c%c%c%c",
			  selection.end.col + 1, selection.end.row + 1,
			  selection.beg.col + 1, selection.beg.row + 1)
	  }
	cmd_printf ("%c%c", Pixel2Col (x) + 1, Pixel2Row (y) + 1);
     }
   else
     {
	selection_start_colrow (x - 1, y - 1);
	top = firstrow;
	bot = lastrow;
     }
}
#endif
