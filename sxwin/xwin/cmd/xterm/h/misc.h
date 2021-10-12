/*
 ****************************************************************
 *								*
 *			h/misc.h				*
 *								*
 *	Definições relativas ao módulo "misc.c"			*
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

extern	const char	*basename (const char *);
extern	int		escaped_string (char *);
extern	void		Draw_Shadow (Window, GC, GC, int, int, int, int);
extern	void		Draw_Triangle (Window, GC, GC, int, int, int, int);

extern	void		*xtmalloc (size_t);
extern	void		*xtcalloc (size_t, size_t);
extern	void		*xtrealloc (void *, size_t);
extern	void		xtfree (void *);

extern	void		msg (const char *, ...);
