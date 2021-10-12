/*
 ****************************************************************
 *								*
 *			top/top.h				*
 *								*
 *	This is the top level of cc1.				*
 * 	It parses command args, opens files, invokes the	*
 *	various passes in the proper order, and counts the	*
 *	time used by each. Error messages and low-level		*
 *	interface to malloc also handled here.			*
 *								*
 *	Versão	3.0.0, de 02.01.94				*
 *		3.0.0, de 02.01.94				*
 *								*
 *	Módulo: GCC						*
 *		Compilador "C"					*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright (c) 1993 UNITEC Informática Ltda	*
 * 		Baseado em software homônimo do GNU		*
 *								*
 ****************************************************************
 */

/*
 ******	Protótipos de funções ***********************************
 */
#if (0)	/*************************************/
extern void	fatal_insn_not_found (rtx);
extern void	error_for_asm (rtx, char *, ...);
extern void	warning_for_asm (rtx, char *, ...);
#endif	/*************************************/

extern int	count_error (int);
extern void	pfatal_with_name (char *);
extern void	fatal_io_error (char *);
extern void	fatal (char *, ...);
extern void	announce_function (tree);
extern void	report_error_function (char *);
extern void	error (const char *, ...);
extern void	error_with_file_and_line (char *, int, char *, ...);
extern void	error_with_decl (tree, char *, ...);
extern void	warning (char *, ...);
extern void	warning_with_file_and_line (char *, int, char *, ...);
extern void	warning_with_decl (tree, char *, ...);
extern void	sorry (char *, ...);
extern void	really_sorry (char *, ...);
extern void	fancy_abort (void);
extern void	botch (int);
extern void	*xmalloc (int);
extern void	compile_file (char *);
extern void	rest_of_decl_compilation (tree, char *, int, int);
extern void	rest_of_compilation (tree);
extern int	gettime (void);
extern void	print_time (char *, int);
extern void	*xmalloc (int);
extern void	*xrealloc (void *, int);
extern int	exact_log2 (unsigned int);
extern int	floor_log2 (unsigned int);
#if (0)	/*************************************/
extern void	set_float_handler (jmp_buf);
#endif	/*************************************/
extern void	float_signal (void);
extern void	pipe_closed (void);
extern void	set_target_switch (char *);
extern void	print_target_switch_defaults (void);
