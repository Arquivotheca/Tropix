/*
 ****************************************************************
 *								*
 *			h/xdefaults.h				*
 *								*
 *	Definições relativas ao módulo "xdefaults.c"		*
 *								*
 *	Versão	3.0.0, de 17.09.97				*
 *		3.0.0, de 26.07.98				*
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

extern	const char	*rs_title;	/* title name for window */
extern	const char	*rs_geometry;	/* window geometry */
extern	const char	*rs_saveLines;	/* scrollback buffer [lines] */

#ifdef KEYSYM_RESOURCE
extern	const char	*KeySym_map [256];
#endif

extern void		extract_resources (Display *, const char *);
extern void		get_options (int, const char *[]);
