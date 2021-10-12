/*
 ****************************************************************
 *								*
 *			src/scrollbar.c				*
 *								*
 *	Trata a barra de "scroll"				*
 *								*
 *	Versão	3.0.0, de 16.06.97				*
 *		4.4.0, de 19.03.03				*
 *								*
 *	Módulo: cmd/xterm					*
 *		X Windows - Clientes				*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright (c) 2003 TROPIX Technologies Ltd.	*
 * 								*
 ****************************************************************
 */

#include <stdio.h>

#include <X11/Xlib.h>

#include "../h/feature.h"
#include "../h/misc.h"
#include "../h/main.h"
#include "../h/scrollbar.h"

scrollBar_t	scrollBar;

static GC	scrollbarGC;
static GC	topShadowGC, botShadowGC;

/*
 ****************************************************************
 *	Desenha o triângulo para cima, com sombra		*
 ****************************************************************
 */
static void
Draw_up_button (int x, int y, int state)
{
	const unsigned int	sz = (SB_WIDTH), sz2 = (SB_WIDTH / 2);
	XPoint			pt [3];
	GC			top, bot;

	switch (state)
	{
	    case +1:
		top = topShadowGC;
		bot = botShadowGC;
		break;

	    case -1:
		top = botShadowGC;
		bot = topShadowGC;
		break;

	    case 0:
		top = bot = scrollbarGC;
		break;

	    default:
		msg ("Draw_up_button: Estado inválido");
		return;
	}

	pt[0].x = x;		pt[0].y = y + sz - 1;
	pt[1].x = x + sz - 1;	pt[1].y = y + sz - 1;
	pt[2].x = x + sz2;	pt[2].y = y;
	XFillPolygon
	(	dpy, scrollBar.win, scrollbarGC,
		pt, 3, Convex, CoordModeOrigin
	);

	XDrawLine
	(	dpy, scrollBar.win, bot,
		pt[0].x, pt[0].y, pt[1].x, pt[1].y
	);

	pt[1].x = x + sz2 - 1;	pt[1].y = y;
	XDrawLine
	(	dpy, scrollBar.win, top,
		pt[0].x, pt[0].y, pt[1].x, pt[1].y
	);

#if (SHADOW > 1)
	pt[0].x++; pt[0].y--; pt[1].y++;
	XDrawLine
	(	dpy, scrollBar.win, top,
		pt[0].x, pt[0].y, pt[1].x, pt[1].y
	);
#endif

	pt[0].x = x + sz2;	pt[0].y = y;
	pt[1].x = x + sz - 1;	pt[1].y = y + sz - 1;
	XDrawLine
	(	dpy, scrollBar.win, bot,
		pt[0].x, pt[0].y, pt[1].x, pt[1].y
	);

#if (SHADOW > 1)
	pt[0].y++; pt[1].x--; pt[1].y--;
	XDrawLine
	(	dpy, scrollBar.win, bot,
		pt[0].x, pt[0].y, pt[1].x, pt[1].y
	);
#endif
}	/* end Draw_up_button */

/*
 ****************************************************************
 *	Desenha o triângulo para baixo, com sombra		*
 ****************************************************************
 */
static void
Draw_dn_button (int x, int y, int state)
{
	const unsigned int	sz = (SB_WIDTH), sz2 = (SB_WIDTH / 2);
	XPoint			pt [3];
	GC			top, bot;

	switch (state)
	{
	    case +1:
		top = topShadowGC;
		bot = botShadowGC;
		break;

	    case -1:
		top = botShadowGC;
		bot = topShadowGC;
		break;

	    case 0:
		top = bot = scrollbarGC;
		break;

	    default:
		msg ("Draw_dn_button: Estado inválido");
		return;
	}

	pt[0].x = x;		pt[0].y = y;
	pt[1].x = x + sz - 1;	pt[1].y = y;
	pt[2].x = x + sz2;	pt[2].y = y + sz;
	XFillPolygon
	(	dpy, scrollBar.win, scrollbarGC,
		pt, 3, Convex, CoordModeOrigin
	);

	XDrawLine
	(	dpy, scrollBar.win, top,
		pt[0].x, pt[0].y, pt[1].x, pt[1].y
	);

	pt[1].x = x + sz2 - 1;		pt[1].y = y + sz - 1;
	XDrawLine
	(	dpy, scrollBar.win, top,
		pt[0].x, pt[0].y, pt[1].x, pt[1].y
	);

#if (SHADOW > 1)
	pt[0].x++; pt[0].y++; pt[1].y--;
	XDrawLine
	(	dpy, scrollBar.win, top,
		pt[0].x, pt[0].y, pt[1].x, pt[1].y
	);
#endif

	pt[0].x = x + sz2;		pt[0].y = y + sz - 1;
	pt[1].x = x + sz - 1;		pt[1].y = y;
	XDrawLine
	(	dpy, scrollBar.win, bot,
		pt[0].x, pt[0].y, pt[1].x, pt[1].y
	);

# if (SHADOW > 1)
	pt[0].y--; pt[1].x--; pt[1].y++;
	XDrawLine
	(	dpy, scrollBar.win, bot,
		pt[0].x, pt[0].y, pt[1].x, pt[1].y
	);
# endif
}	/* end Draw_dn_button */

/*
 ****************************************************************
 *	Mapeia/Desmapeia a janela da barra de "scroll"		*
 ****************************************************************
 */
int
scrollbar_mapping (int map)
{
	if (map && !scrollbar_visible ())
	{
		scrollBar.state = 1;
		XMapWindow (dpy, scrollBar.win);
		return (1);
	}

	if (!map && scrollbar_visible ())
	{
		scrollBar.state = 0;
		XUnmapWindow (dpy, scrollBar.win);
		return (1);
	}

	return (0);

}	/* end scrollbar_mapping */

/*
 ****************************************************************
 *	Exibe a barra de "scroll"				*
 ****************************************************************
 */
int
scrollbar_show (int update)
{
	static int	last_top, last_bot;	/* old (drawn) values */

	if (!scrollbar_visible ())
		return (0);

	if (scrollbarGC == None)
	{
		XGCValues gcvalue;

		gcvalue.foreground = Xdepth <= 2
					? PixColors [fgColor]
					: PixColors [scrollColor];
		scrollbarGC =	XCreateGC
				(	dpy, scrollBar.win,
					GCForeground, &gcvalue
				);

		gcvalue.foreground = PixColors [topShadowColor];
		topShadowGC =	XCreateGC
				(	dpy, scrollBar.win,
					GCForeground, &gcvalue
				);

		gcvalue.foreground = PixColors [bottomShadowColor];
		botShadowGC =	XCreateGC
				(	dpy, scrollBar.win,
					GCForeground, &gcvalue
				);
	}

	if (update)
	{
		int	top = (TermWin.nscrolled - TermWin.view_start);
		int	bot = top + (TermWin.nrow-1);
		int	len = (TermWin.nscrolled + (TermWin.nrow-1));

		scrollBar.top = (scrollBar.beg +
					(top * scrollbar_size ()) / len);
		scrollBar.bot = (scrollBar.beg +
					(bot * scrollbar_size ()) / len);
		/* no change */
		if ((scrollBar.top == last_top) && (scrollBar.bot == last_bot))
			return (0);
	}

	/* instead of XClearWindow (dpy, scrollBar.win); */
	if (last_top < scrollBar.top)
	{
		XClearArea
		(	dpy, scrollBar.win,
			0, last_top, SB_WIDTH, (scrollBar.top - last_top),
			False
		);
	}

	if (scrollBar.bot < last_bot)
	{
		XClearArea
		(	dpy, scrollBar.win,
			0, scrollBar.bot, SB_WIDTH, (last_bot - scrollBar.bot),
			False
		);
	}

	last_top = scrollBar.top;
	last_bot = scrollBar.bot;

	/* scrollbar slider */
	XFillRectangle
	(	dpy, scrollBar.win, scrollbarGC,
		0, scrollBar.top, SB_WIDTH,
		(scrollBar.bot - scrollBar.top)
	);

	/* shadow for scrollbar slider */
	Draw_Shadow
	(	scrollBar.win,
		topShadowGC, botShadowGC,
		0, scrollBar.top, SB_WIDTH,
		(scrollBar.bot - scrollBar.top)
	);

	/*
	 *	Redesenha as setinhas.
	 */
	Draw_up_button (0, 0, (scrollbar_isUp () ? -1 : +1));
	Draw_dn_button (0, (scrollBar.end + 1), (scrollbar_isDn () ? -1 : +1));

	return (1);

}	/* end scrollbar_show */
