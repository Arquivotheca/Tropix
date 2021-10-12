/*
 ****************************************************************
 *								*
 *			proto.h					*
 *								*
 *	Protótipos de todas as funções				*
 *								*
 *	Versão	3.2.2, de 16.09.99				*
 *		4.5.0, de 10.10.03				*
 *								*
 *	Módulo: SHELL						*
 *		Interpretador de Comandos			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2003 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

/*
 ******	Em "src/exec.c" *****************************************
 */
int		do_exec (CMD *, int, int, int);
int		exec (CMD *, int, int, int);
int		await (int, const char *);
int		exec_command_file (const char *, int, int);
void		save_fds (void);
void		restore_fds (void);

/*
 ******	Em "src/expand.c" ***************************************
 */
char		**expand_args (char **, int *);
void		free_args (char **);

/*
 ******	Em "src/fnrgets.c" **************************************
 */
int		fnrgets (char **, int *, char **, FILE *);
void		handle_sigwinch (int, ...);

/*
 ******	Em "src/func.c" ****************************************
 */
FUNCID		check_func (const char *);
int		exec_func (CMD *, int, int, int);
int		ask_yes_no (const char *);
void		update_cwd (void);

/*
 ******	Em "src/hist.c" *****************************************
 */
int		put_history (const char *);
void		show_history (FILE *);
char		*get_history (int);
int		set_history_size (int);
char		*select_command_from_history (int *);

/*
 ******	Em "src/parse.c" ****************************************
 */
CMD		*get_command (int);
int		getargv (void);

/*
 ******	Em "src/salloc.c" ***************************************
 */
void		*salloc (int);
void		sfree (void);

/*
 ******	Em "src/scan.c" *****************************************
 */
MAJOR		get_token (void);
void		restart_scanner (void);
void		insert_line (const char *);
void		skip_line (void);
int		backmemstr (char *);
int		copyupto (const char *, const char *);
char		*all_params (void);
char		*get_variable_value (char **, int);
int		hasmeta (char *);
void		putmeta (char *);

/*
 ******	Em "src/sh.c" *******************************************
 */
int		define_params (int, int, const char *[]);
void		print_params (void);
void		exec_hash_create (int);
const char	*exec_hash_search (const char *, int, const char *);
void		handle_signals (int, ...);
