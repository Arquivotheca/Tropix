/*
 ****************************************************************
 *								*
 *			top/top-error.c				*
 *								*
 *	This is the top level of cc1.				*
 * 	It parses command args, opens files, invokes the	*
 *	various passes in the proper order, and counts the	*
 *	time used by each. Error messages and low-level		*
 *	interface to malloc also handled here.			*
 *								*
 *	Versão	3.0.0, de 26.08.93				*
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
 ******	Inclusões ***********************************************
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "../h/input.h"
#include "../h/tree.h"
#include "../h/rtl.h"
#include "../h/flags.h"

#include "top-data.h"
#include "top.h"

/*
 ******	Variáveis locais ****************************************
 */
entry int	need_error_newline;

/*
 *	Function of last error message; more generally,
 *	function such that if next error message is in
 *	it then we don't have to mention the function name
 */
entry char		*last_error_function_name = NOSTR;

/*
 *	Used to detect when input_file_stack
 *	has changed since last described
 */
entry int		last_error_tick;

/*
 ******	Protótipos de funções ***********************************
 */
void			v_error_with_file_and_line (char *, int, char *, va_list);
void			v_warning_with_file_and_line (char *, int, char *, va_list);

extern const char	**color_vector;	/* Para as cores dos modos */ 

/*
 ****************************************************************
 *	Conta o número de erros					*
 ****************************************************************
 */
int
count_error (int warningp)
{
	if (warningp && inhibit_warnings)
		return (0);

	if (warningp)
		warningcount++;
	else
		errorcount++;

	return (1);

}	/* end count_error */

/*
 ****************************************************************
 *	Print a fatal error message				*
 ****************************************************************
 */
/*
 *	Print a fatal error message. NAME is the text.
 *	Also include a system error message based on `errno'
 */
void
pfatal_with_name (char *name)
{
	fprintf (stderr, "%s: ", progname);
	perror (name);
	exit (35);

}	/* end pfatal_with_name */

/*
 ****************************************************************
 *	Erro de entrada/saída					*
 ****************************************************************
 */
void
fatal_io_error (char *name)
{
	fprintf (stderr, "%s: %s: *** I/O error\n", progname, name);
	exit (35);

}	/* end fatal_io_error */

/*
 ****************************************************************
 *	Erro fatal						*
 ****************************************************************
 */
void
fatal (char *fmt, ...)
{
	va_list		args;

	va_start (args, fmt);

	v_error_with_file_and_line (input_filename, lineno, fmt, args);

	va_end (args);

	exit (34);

}	/* end fatal */

/*
 ****************************************************************
 *	Tenta dar uma mensagem melhor				*
 ****************************************************************
 */
/*
 *	Called from insn-extract to give a better error
 *	message when we don't have an insn to match what
 *	we are looking for, rather than just calling abort()
 */
void
fatal_insn_not_found (rtx insn)
{
	error ("The following insn was not recognizable:");
	debug_rtx (insn);
	abort ();

}	/* end fatal_insn_not_found */

/*
 ****************************************************************
 *	Anuncia a função sendo compilada			*
 ****************************************************************
 */
/*
 *	Called when the start of a function definition is parsed,
 *	this function prints on stderr the name of the function.
 */
void
announce_function (tree decl)
{
	if (! quiet_flag)
	{
		char	*name = DECL_PRINT_NAME (decl);

		fprintf (stderr, " %s", name);
		fflush (stderr);
		need_error_newline = 1;

		last_error_function_name = name;
	}

}	/* end announce_function */

/*
 ****************************************************************
 *	Prints out, the name of the current function		*
 ****************************************************************
 */
/*
 *	Prints out, if necessary, the name of the
 *	current function which caused an error.
 *	Called from all error and warning functions
 */
void
report_error_function (char *file)
{
	struct file_stack	*p;
	char			*name;

	if (current_function_decl != NULL_TREE)
		name = DECL_PRINT_NAME (current_function_decl);
	else
		name = NOSTR;

	if (need_error_newline)
	{
		fprintf (stderr, "\n");
		need_error_newline = 0;
	}

	if (last_error_function_name != name)
	{
		if (file)
			fprintf (stderr, "%s: ", file);

		if   (current_function_decl == NULL_TREE)
			fprintf (stderr, "At top level:\n");
		elif (TREE_CODE (TREE_TYPE (current_function_decl)) == METHOD_TYPE)
			fprintf (stderr, "In method %s:\n", name);
		else
			fprintf (stderr, "In function %s:\n", name);

		last_error_function_name = name;
	}

	if
	(	input_file_stack &&
		input_file_stack->next != (struct file_stack *)NULL &&
		input_file_stack_tick != last_error_tick
	)
	{
		fprintf (stderr, "In file included");

		for (p = input_file_stack->next; p; p = p->next)
		{
			fprintf (stderr, " from %s:%d", p->name, p->line);

			if (p->next)
				fprintf (stderr, ",");
		}

		fprintf (stderr, ":\n");
		last_error_tick = input_file_stack_tick;
	}

}	/* end report_error_function */

/*
 ****************************************************************
 *	Report an error at the current line number		*
 ****************************************************************
 */
void
error (const char *fmt, ...)
{
	va_list		args;

	va_start (args, fmt);

	v_error_with_file_and_line (input_filename, lineno, fmt, args);

	va_end (args);

}	/* end error */

/*
 ****************************************************************
 *	Report an error at line LINE of file FILE		*
 ****************************************************************
 */
void
error_with_file_and_line (char *file, int line, char *fmt, ...)
{
	va_list		args;

	va_start (args, fmt);

	v_error_with_file_and_line (file, line, fmt, args);

	va_end (args);

}	/* end error_with_file_and_line */

/*
 ****************************************************************
 *	Report an error at line LINE of file FILE (versão "v")	*
 ****************************************************************
 */
void
v_error_with_file_and_line (char *file, int line, char *fmt, va_list args)
{
	count_error (0);

	report_error_function (file);

	if (file)
		fprintf (stderr, "%s:%d: ", file, line);
	else
		fprintf (stderr, "%s: ", progname);

#if (0)	/*******************************************************/
	fprintf  (stderr, "*** ERROR: ");
	fprintf  (stderr, "*** \e[34mERROR\e[0m: ");
#endif	/*******************************************************/
	fprintf  (stderr, "*** %sERROR%s: ", color_vector[MC_ERR], color_vector[MC_DEF]);
	vfprintf (stderr, fmt, args);
	fprintf  (stderr, "\n");

}	/* end error_with_file_and_line */

/*
 ****************************************************************
 *	Report an error at the declaration DECL			*
 ****************************************************************
 */
/*
 *	Report an error at the declaration DECL. FMT is
 *	a string which uses %s to substitute the declaration name
 */
void
error_with_decl (register tree decl, char *fmt, ...)
{
	char		*format = fmt;
	char		*dcl_name;

	/*
	 *	Repare que superpõe "dcl_name" em "fmt",
	 *	para acessar os argumentos depois de "fmt"
	 */
	count_error (0);

	report_error_function (DECL_SOURCE_FILE (decl));

	fprintf
	(	stderr, "%s:%d: ",
		DECL_SOURCE_FILE (decl),
		DECL_SOURCE_LINE (decl)
	);

	if   (DECL_PRINT_NAME (decl))
		dcl_name = DECL_PRINT_NAME (decl);
	elif (DECL_NAME (decl))
		dcl_name = IDENTIFIER_POINTER (DECL_NAME (decl));
	else
		dcl_name = "((anonymous))";

	fmt = dcl_name;	/* Superpõe */

#if (0)	/*******************************************************/
	fprintf  (stderr, "*** ERROR: ");
	fprintf  (stderr, "*** \e[34mERROR\e[0m: ");
#endif	/*******************************************************/
	fprintf  (stderr, "*** %sERROR%s: ", color_vector[MC_ERR], color_vector[MC_DEF]);
	vfprintf (stderr, format, (va_list)&fmt);
	fprintf (stderr, "\n");

}	/* end error_with_decl */

/*
 ****************************************************************
 *	Report an error at the line number of the insn INSN	*
 ****************************************************************
 */
/*
 *	Report an error at the line number of the
 *	insn INSN. S and V are a string and an arg
 *	for `printf'. This is used only when INSN is
 *	an `asm' with operands, and each ASM_OPERANDS
 *	records its own source file and line
 */
void
error_for_asm (rtx insn, char *fmt, ...)
{
	char		*filename;
	int		line;
	register rtx	body = PATTERN (insn);
	rtx		asmop;
	va_list		args;

	va_start (args, fmt);

	/*
 	 *	Find the (or one of the) ASM_OPERANDS in the insn
	 */
	if (GET_CODE (body) == SET && GET_CODE (SET_SRC (body)) == ASM_OPERANDS)
		asmop = SET_SRC (body);
	elif (GET_CODE (body) == ASM_OPERANDS)
		asmop = body;
	elif (GET_CODE (body) == PARALLEL && GET_CODE (XVECEXP (body, 0, 0)) == SET)
		asmop = SET_SRC (XVECEXP (body, 0, 0));
	elif (GET_CODE (body) == PARALLEL && GET_CODE (XVECEXP (body, 0, 0)) == ASM_OPERANDS)
		asmop = XVECEXP (body, 0, 0);

	filename = ASM_OPERANDS_SOURCE_FILE (asmop);
	line	 = ASM_OPERANDS_SOURCE_LINE (asmop);

	v_error_with_file_and_line (filename, line, fmt, args);

	va_end (args);

}	/* end error_for_asm */

/*
 ****************************************************************
 *	Report an warning at the current line number		*
 ****************************************************************
 */
void
warning (char *fmt, ...)
{
	va_list		args;

	va_start (args, fmt);

	v_warning_with_file_and_line (input_filename, lineno, fmt, args);

	va_end (args);

}	/* end warning */

/*
 ****************************************************************
 *	Report an warning at line LINE of file FILE		*
 ****************************************************************
 */
void
warning_with_file_and_line (char *file, int line, char *fmt, ...)
{
	va_list		args;

	va_start (args, fmt);

	v_warning_with_file_and_line (file, line, fmt, args);

	va_end (args);

}	/* end warning_with_file_and_line */

/*
 ****************************************************************
 *   Report an warning at line LINE of file FILE (versão "v")	*
 ****************************************************************
 */
void
v_warning_with_file_and_line (char *file, int line, char *fmt, va_list args)
{
	if (count_error (1) == 0)
		return;

	report_error_function (file);

	if (file)
		fprintf (stderr, "%s:%d: ", file, line);
	else
		fprintf (stderr, "%s: ", progname);

#if (0)	/*******************************************************/
	fprintf  (stderr, "*** WARNING: ");
	fprintf  (stderr, "*** \e[31mWARNING\e[0m: ");
#endif	/*******************************************************/
	fprintf  (stderr, "*** %sWARNING%s: ", color_vector[MC_WARN], color_vector[MC_DEF]);
	vfprintf (stderr, fmt, args);
	fprintf  (stderr, "\n");

}	/* end warning_with_file_and_line */

/*
 ****************************************************************
 *	Report a warning at the declaration DECL		*
 ****************************************************************
 */
/*
 *	Report a warning at the declaration DECL. S is string
 *	which uses %s to substitute the declaration name.
 */
void
warning_with_decl (register tree decl, char *fmt, ...)
{
	char		*format = fmt;
	char		*dcl_name;

	/*
	 *	Repare que superpõe "dcl_name" em "fmt",
	 *	para acessar os argumentos depois de "fmt"
	 */
	if (count_error (1) == 0)
		return;

	report_error_function (DECL_SOURCE_FILE (decl));

	fprintf
	(	stderr, "%s:%d: ",
		DECL_SOURCE_FILE (decl),
		DECL_SOURCE_LINE (decl)
	);

	if   (DECL_PRINT_NAME (decl))
		dcl_name = DECL_PRINT_NAME (decl);
	elif (DECL_NAME (decl))
		dcl_name = IDENTIFIER_POINTER (DECL_NAME (decl));
	else
		dcl_name = "((anonymous))";

	fmt = dcl_name;	/* Superpõe */

#if (0)	/*******************************************************/
	fprintf  (stderr, "*** WARNING: ");
	fprintf  (stderr, "*** \e[31mWARNING\e[0m: ");
#endif	/*******************************************************/
	fprintf  (stderr, "*** %sWARNING%s: ", color_vector[MC_WARN], color_vector[MC_DEF]);
	vfprintf (stderr, format, (va_list)&fmt);
	fprintf  (stderr, "\n");

}	/* end warning_with_decl */

/*
 ****************************************************************
 *	Report a warning at the line number of the insn INSN	*
 ****************************************************************
 */
/*
 *	Report a warning at the line number of the insn INSN.
 *	FMT is a string for `printf'. This is
 *	used only when INSN is an `asm' with operands, and each
 *	ASM_OPERANDS records its own source file and line
 */
void
warning_for_asm (rtx insn, char *fmt, ...)
{
	char		*filename;
	int		line;
	register rtx	body = PATTERN (insn);
	rtx		asmop;
	va_list		args;

	va_start (args, fmt);

	/*
	 *	Find the (or one of the) ASM_OPERANDS in the insn
	 */
	if (GET_CODE (body) == SET && GET_CODE (SET_SRC (body)) == ASM_OPERANDS)
		asmop = SET_SRC (body);
	elif (GET_CODE (body) == ASM_OPERANDS)
		asmop = body;
	elif (GET_CODE (body) == PARALLEL && GET_CODE (XVECEXP (body, 0, 0)) == SET)
		asmop = SET_SRC (XVECEXP (body, 0, 0));
	elif (GET_CODE (body) == PARALLEL && GET_CODE (XVECEXP (body, 0, 0)) == ASM_OPERANDS)
		asmop = XVECEXP (body, 0, 0);

	filename = ASM_OPERANDS_SOURCE_FILE (asmop);
	line	 = ASM_OPERANDS_SOURCE_LINE (asmop);

	v_warning_with_file_and_line (filename, line, fmt, args);

	va_end (args);

}	/* end warning_for_asm */

/*
 ****************************************************************
 *	Apologize for not implementing some feature		*
 ****************************************************************
 */
/*
 *	Apologize for not implementing some feature.
 *	FMT is a string for `printf'
 */
void
sorry (char *fmt, ...)
{
	register va_list args;

	va_start (args, fmt);

	sorrycount++;

	if (input_filename)
		fprintf (stderr, "%s:%d: ", input_filename, lineno);
	else
		fprintf (stderr, "%s: ", progname);

	fprintf  (stderr, "sorry, not implemented: ");
	vfprintf (stderr, fmt, args);
	fprintf  (stderr, "\n");

	va_end (args);

}	/* end sorry */

/*
 ****************************************************************
 *	Apologize for not implementing some feature, then quit	*
 ****************************************************************
 */
/*
 *	Apologize for not implementing some feature, then quit.
 *	S, V, and V2 are a string and args for `printf'
 */
void
really_sorry (char *fmt, ...)
{
	register va_list args;

	va_start (args, fmt);

	if (input_filename)
		fprintf (stderr, "%s:%d: ", input_filename, lineno);
	else
		fprintf (stderr, "c++: ");

	fprintf (stderr, "sorry, not implemented: ");
	vfprintf (stderr, fmt, args);
	fatal (" (fatal)\n");

	va_end (args);

}	/* end really_sorry */

/*
 ****************************************************************
 *	More 'friendly' abort that prints the line and file	*
 ****************************************************************
 */
/*
 *	More 'friendly' abort that prints the line and file.
 *	config.h can #define abort fancy_abort if you like
 *	that sort of thing
 */
void
fancy_abort (void)
{
	fatal ("Internal gcc abort.");

}	/* end fancy_abort */

/*
 ****************************************************************
 *	Para o erro de "malloc"					*
 ****************************************************************
 */
/*
 *	When `malloc.c' is compiled with `rcheck' defined,
 *	it calls this function to report clobberage
 */
void
botch (int s)
{
	abort ();

}	/* end botch */
