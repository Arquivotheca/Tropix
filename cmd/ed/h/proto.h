/*
 ****************************************************************
 *								*
 *			h/proto.h				*
 *								*
 *	Definições Gerais para todo ED				*
 *								*
 *	Versão	1.0.0, de 13.11.86				*
 *		3.0.0, de 26.04.97				*
 *								*
 *	Modulo: ED						*
 *		Editores de Texto				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Multiprocessado		*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

extern BASE		*get_address (BASE *);
extern bool		command (void);
extern bool		c_global (bool, int);
extern bool		c_substitute (int, bool);
extern void		c_edit (const char *);
extern bool		c_join (bool);
extern void		c_delete (BASE *, BASE *, bool);
extern bool		c_append (BASE *);
extern bool		c_insert (BASE *);
extern void		c_print (BASE *, BASE *);
extern void		c_list (BASE *, BASE *);
extern void		c_quit (void);
extern int		read_line (FILE *, char *);
extern bool		append (BASE *);
extern void		ln_print (char *);
extern void		ln_list (const char *);
extern int		txt_inorder (BASE *, BASE *);
extern int		txt_order (BASE *);
extern BASE		*txt_base (int);
extern BASE		*txt_advance (BASE *, int);
extern void		txt_size (BASE *, BASE *, SIZE *);
extern void		txt_free (BASE *);
extern void		txt_move (BASE *, BASE *, BASE *);
extern bool		txt_copy (BASE *, BASE *, BASE *);
extern bool		txt_new (int, BASE *);
extern bool		txt_insert (BASE *, const char *);
extern bool		txt_join (BASE *, BASE *);
extern void		txt_remove (BASE *, BASE *);
extern void		txt_init (void);
extern void		mrk_define (int, BASE *);
extern BASE		*mrk_base (int);
extern bool		re_compile (const char *, AUTO);
extern const char	*re_execute (const char *, const char *, const char *);
extern bool		match (const char *, const char *, const char *, int);
extern bool		read_command (void);
extern char		*get_name (void);
extern char		*get_string (int);
extern int		get_lower (void);
extern void		sub_compile (const char *, REPL *);
extern char		*sub_execute (BASE *, char *, char *, REPL *);

#ifdef	SCAN_H
extern void		ed_error (const char *, ERR);
extern void		putmark (ERR);
#endif	SCAN_H

#ifdef	TXTIO_H
extern TXTIO		txt_read (const char *, BASE *, SIZE *);
extern TXTIO		txt_write (const char *, BASE *, BASE *, bool, bool);
#endif	TXTIO_H
