/*
 ****************************************************************
 *								*
 *			proto.h					*
 *								*
 *	Declaração de funções					*
 *								*
 *	Versão	3.0.0, de 23.04.95				*
 *		3.0.1, de 30.04.95				*
 *								*
 *	Módulo: EX/VI						*
 *		Editores de texto				*
 *		Categoria D					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

/*
 ******	Protótipos de funções ***********************************
 */
extern bool	re_compile (const char *, char **);
extern bool	scn_command (void);
extern int	re_execute (char *, char *, char *);
extern void	buf_sput (const char *, int, int, int);
extern char	*scn_name (void);
extern char	*scn_string (int);
extern int	scn_lower (void);
extern void	txt_init (void);
extern void	txt_usave (void);
extern bool	txt_included ();
extern void	vis_initget (void);
extern char	vis_chget (void);
extern void	vis_ungetch (int);
extern void	vis_getstr (char *, char, bool);
extern void	vis_endget (void);
extern void	vis_message (char *);
extern void	vis_error (char *, char *);
extern void	pg_update (void);
extern void	pg_scroll (int);
extern void	vis_switch (int, ...);
extern void	vis_cursor (int, int);
extern bool	scn_command (void);
extern char	*scn_name (void);
extern char	*scn_string (int);
extern int	scn_lower (void);
extern void	ln_list (const char *);
extern int	str_coln (int, char *);
extern int	str_try (int, char *);
extern int	str_nonwhite (char *);
extern int	Visual (void);

#ifdef	H_CURSES
extern int	Commands (WINDOW *, bool);
#endif	H_CURSES

#ifdef	H_OBJ
extern void	vis_change (VIS *);
extern void	vis_delete (VIS *);
extern void	vis_insert (VIS *);
extern void	vis_join (VIS *);
extern void	vis_put (VIS *);
extern void	vis_replace (VIS *);
extern bool	vis_shift (VIS *);
extern void	vis_undo (VIS *);
extern void	vis_window (VIS *);
extern void	vis_move (VIS *);
#endif	H_OBJ

#ifdef	H_SCAN
extern void	scn_error (const char *, ERROR);
extern void	putmark (ERROR);
#endif	H_SCAN

#ifdef	H_TEXT
extern BASE	*get_address (BASE *);
extern bool	buf_lput (BASE *, BASE *, int);
extern bool	buf_get (BASE *, int, bool, int);
extern TXTIO	txt_write (const char *, BASE *, BASE *, bool, bool);
extern TXTIO	txt_read (char *, BASE *, SIZE *);
extern void	txt_size (BASE *, BASE *, SIZE *);
extern void	txt_free (BASE *, BASE *);
extern int	txt_order (BASE *);
extern BASE	*txt_base (int);
extern int	txt_inorder (BASE *, BASE *);
extern BASE	*txt_advance (BASE *, int);
extern BASE	*txt_walk (BASE *, int);
extern void	txt_move (BASE *, BASE *, BASE *);
extern bool	txt_copy (BASE *, BASE *, BASE *);
extern bool	txt_insert (BASE *, char *);
extern bool	txt_new (BASE *, int);
extern void	txt_remove (BASE *, BASE *);
extern int	txt_join (BASE *, BASE *, int);
extern void	txt_lsave (BASE *, BASE *);
extern void	txt_fsave (BASE *);
extern void	txt_bsave (BASE *);
extern bool	txt_change (BASE **, BASE **, int *, int *);
extern BASE	*txt_restore (void);
extern void	mrk_define (int, BASE *, int);
extern bool	mrk_pos (int, BASE **, int *);
extern BASE	*mrk_base (int);
extern int	pg_present (BASE *);
extern void	pg_display (BASE *);
extern void	pg_move (BASE *, int, bool);
extern void	pg_remap (BASE *);
#endif	H_TEXT

#ifdef	H_COMMANDS
extern void	sub_compile (const char *, REPL *);
#endif	H_COMMANDS

#if	defined (H_COMMANDS) && defined (H_TEXT)
extern char	*sub_execute (BASE *, char *, char *, REPL *);
#endif	defined (H_COMMANDS) && defined (H_TEXT)

#if	defined (H_OBJ) && defined (H_TEXT)
extern int	object (OBJ *, int, bool, bool, BASE **, int *);
#endif	defined (H_OBJ) && defined (H_TEXT)
