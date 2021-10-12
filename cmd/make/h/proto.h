/*
 ****************************************************************
 *								*
 *			proto.h					*
 *								*
 *	Protótipos das funções visíveis globalmente		*
 *								*
 *	Versão	3.0.0, de 29.03.96				*
 *		3.0.1, de 01.05.97				*
 *								*
 *	Módulo: Make: Construtor de Programas			*
 *		Interpretador de Comandos			*
 *		Categoria D					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 * 								*
 ****************************************************************
 */

/*
 ****	Em src/hash.c  ******************************************
 */
extern	HASH	*hash (char *);

/*
 ****	Em src/error.c  *****************************************
 */
extern	void	err_msg (ERRTYPE, MARKTYPE, char *, ...);
extern	void	msg (char *, ...);
extern	void	set_mark (RANGE *);
extern	void	*emalloc (int);

/*
 ****	Em src/scan.c  ******************************************
 */
extern	int	read_makefile (char *);
extern	MAJOR	get_token (void);
extern	void	get_string (RANGE *);
extern	char	*expand (char *, int *);
extern	void	skip_line (void);

/*
 ****	Em src/parse.c  *****************************************
 */
extern	SYM	*parse_makefile (void);
extern	void	print_target_list (void);

/*
 ****	Em src/update.c  ****************************************
 */
extern	int	make (SYM *);
