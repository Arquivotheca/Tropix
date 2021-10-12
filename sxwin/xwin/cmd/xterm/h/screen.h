/*
 ****************************************************************
 *								*
 *			h/screen.h				*
 *								*
 *	Definições relativas ao módulo "screen.c"		*
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

/* Screen refresh methods */
#define NO_REFRESH	2	/* Window not visible at all! */
#define SLOW_REFRESH	1	/* No Bitblt */
#define FAST_REFRESH	0

#define	SAVE		's'
#define	RESTORE		'r'

/* flags for scr_gotorc() */
#define C_RELATIVE	1	/* col movement is relative */
#define R_RELATIVE	2	/* row movement is relative */
#define RELATIVE (R_RELATIVE|C_RELATIVE)	/* move row/col relative */

/* modes for scr_insdel_chars(), scr_insdel_lines() */
#define INSERT	-1		/* don't change these values */
#define DELETE	+1
#define ERASE	+2

/* modes for scr_page() */
#define UP	+1		/* don't change these values */
#define DN	-1

/* arguments for scr_change_screen() */
#define PRIMARY		0
#define SECONDARY	1

/* rendition style flags */
#define RS_None		0		/* Normal */
#define RS_Cursor	0x01000000u	/* cursor location */
#define RS_Select	0x02000000u	/* selected text */
#define RS_RVid		0x04000000u	/* reverse video */
#define RS_Uline	0x08000000u	/* underline */
#define RS_acsFont	0x10000000u	/* ACS - graphics character set */
#define RS_ukFont	0x20000000u	/* UK character set */
#define RS_fontMask	(RS_acsFont|RS_ukFont)

#define RS_fgMask	0x00001F00u	/* 32 colors */
#define RS_Bold		0x00008000u	/* bold */
#define RS_bgMask	0x001F0000u	/* 32 colors */
#define RS_Blink	0x00800000u	/* blink */

#define RS_attrMask	(0xFF000000u|RS_Bold|RS_Blink)

/* how to build & extract colors and attributes */
#define GET_FGCOLOR(r)	(((r) & RS_fgMask)>>8)
#define GET_BGCOLOR(r)	(((r) & RS_bgMask)>>16)
#define GET_ATTR(r)	(((r) & RS_attrMask))

#define SET_FGCOLOR(r,fg)	(((r) & ~RS_fgMask)  | ((fg)<<8))
#define SET_BGCOLOR(r,bg)	(((r) & ~RS_bgMask)  | ((bg)<<16))
#define SET_ATTR(r,a)		(((r) & ~RS_attrMask)| (a))
#define DEFAULT_RSTYLE		(RS_None | (fgColor<<8) | (bgColor<<16))

extern	unsigned int	colorfgbg;

extern	void		scr_reset (void);
extern	void		scr_poweron (void);
extern	void		scr_backspace (void);
extern	void		scr_bell (void);
extern	void		scr_rendition (int /* set */, int /* style */);
extern	void		scr_add_lines (const char * /* str */, int /* nlines */, int /* cnt */);
extern	void		scr_putchar (int ch);
extern	void		scr_tab (int /* count */);
extern	void		scr_gotorc (int /* row */, int /* col */, int /* relative */);
extern	void		scr_index (int /* direction */);
extern	void		scr_cursor (int /* mode */);
extern	void		scr_cursor_visible (int /* mode */);
extern	void		scr_erase_line (int /* mode */);
extern	void		scr_erase_screen (int /* mode */);
extern	void		scr_insdel_lines (int /* count */, int /* insdel */);
extern	void		scr_insdel_chars (int /* count */, int /* insdel */);
extern	void		scr_scroll_region (int /* top */, int /* bot */);
extern	void		scr_report_position (void);
extern	void		scr_expose (int /* x */, int /* y */, int /* width */, int /* height */);
extern	void		scr_touch (void);
extern	void		scr_charset_choose (int /* set */);
extern	void		scr_charset_set (int /* set */, unsigned int /* ch */);
extern	void		scr_refresh (int /* type */);
extern	void		scr_set_tab (int /* mode */);
extern	void		scr_E (void);
extern	void		scr_color (unsigned int /* color */, unsigned int /* Intensity */);
extern	int		scr_move_to (int /* y */, int /* len */);
extern	int		scr_page (int /* dirn */, int /* n */);
extern	int		scr_change_screen (int /* scrn */);
extern	int		scr_get_fgcolor (void);
extern	int		scr_get_bgcolor (void);
extern	void		mouse_report (XButtonEvent * /* ev */);

/* mode selections */

extern	void		scr_autowrap (int /* mode */);
extern	void		scr_relative_origin (int /* mode */);
extern	void		scr_insert_mode (int /* mode */);
extern	void		scr_rvideo_mode (int /* mode */);
extern	void		selection_make (Time /* tm */);
extern	void		selection_send (XSelectionRequestEvent * /* rq */);
extern	void		selection_paste (Window /* win */, unsigned /* prop */, int /* Delete */);
extern	void		selection_request (Time /* tm */, int /* x */, int /* y */);
extern	void		selection_reset (void);
extern	void		selection_clear (void);
extern	void		selection_extend (int /* x */, int /* y */);
extern	void		selection_start (int /* x */, int /* y */);
extern	void		selection_click (int /* clicks */, int /* x */, int /* y */);
