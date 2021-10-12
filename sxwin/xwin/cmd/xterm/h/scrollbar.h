/*
 ****************************************************************
 *								*
 *			h/scrollbar.h				*
 *								*
 *	Definições relativas ao módulo "scrollbar.c"		*
 *								*
 *	Versão	3.0.0, de 17.09.97				*
 *								*
 *	Módulo: programs/xterm					*
 *		X Windows - Clientes				*
 *		Baseado no software homônimo do XFree86		*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright (c) 1998 TROPIX Technologies Ltd.	*
 * 								*
 ****************************************************************
 */

typedef struct
{
	int	beg, end;	/* beg/end of slider sub-window */
	int	top, bot;	/* top/bot of slider */
	int	state;		/* scrollbar state */
	Window	win;

}	scrollBar_t;

extern	scrollBar_t	scrollBar;

extern int		scrollbar_mapping (int);
extern int		scrollbar_show (int);

#define scrollbar_visible()	(scrollBar.state)
#define scrollbar_isMotion()	(scrollBar.state == 'm')
#define scrollbar_isUp()	(scrollBar.state == 'U')
#define scrollbar_isDn()	(scrollBar.state == 'D')
#define scrollbar_isUpDn()	isupper (scrollBar.state)
#define isScrollbarWindow(w)	(scrollbar_visible() && (w) == scrollBar.win)

#define scrollbar_setNone()	do { scrollBar.state = 1; } while (0)
#define scrollbar_setMotion()	do { scrollBar.state = 'm'; } while (0)
#define scrollbar_setUp()	do { scrollBar.state = 'U'; } while (0)
#define scrollbar_setDn()	do { scrollBar.state = 'D'; } while (0)

#define scrollbar_upButton(y)	((y) < scrollBar.beg)
#define scrollbar_dnButton(y)	((y) > scrollBar.end)

#define scrollbar_above_slider(y)	((y) < scrollBar.top)
#define scrollbar_below_slider(y)	((y) > scrollBar.bot)
#define scrollbar_position(y)		((y) - scrollBar.beg)
#define scrollbar_size()		(scrollBar.end - scrollBar.beg)

#if !defined (SB_WIDTH) || (SB_WIDTH < 8)
#undef SB_WIDTH
#define SB_WIDTH	10	/* scrollBar width */
#endif
