/*
 ****************************************************************
 *								*
 *			top/top-file.c				*
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
#pragma	getenv (GEN)

#include <sys/types.h>
#include <sys/stat.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include ../$GEN.gen/config.h

#include "../h/input.h"
#include "../h/tree.h"
#include "../h/c-tree.h"
#include "../h/flags.h"

#include "top-data.h"
#include "top.h"

/*
 ****************************************************************
 *	Compile an entire file of output from cpp		*
 ****************************************************************
 */
/*
 *	Compile an entire file of output from cpp, named NAME.
 *	Write a file of assembly output and various debugging dumps
 */
void
compile_file (char *name)
{
	register tree	decl;
	tree		globals;
	int		start_time;
	int		dump_base_name_length;
	int		name_specified = (name != NOSTR);
	int		otime;

	if (dump_base_name == NOSTR)
		dump_base_name = name ? name : "gccdump";

	dump_base_name_length = strlen (dump_base_name);

	parse_time = 0;
	varconst_time = 0;
	integration_time = 0;
	jump_time = 0;
	cse_time = 0;
	loop_time = 0;
	flow_time = 0;
	combine_time = 0;
	local_alloc_time = 0;
	global_alloc_time = 0;
	dbr_sched_time = 0;
	final_time = 0;
	symout_time = 0;
	dump_time = 0;

	/*
	 *	Open input file
	 */
	if (name == NOSTR || !strcmp (name, "-"))
	{
		finput = stdin;
		name = "stdin";
	}
	else
	{
		finput = fopen (name, "r");
	}

	if (finput == NOFILE)
		pfatal_with_name (name);

	/*
	 *	Initialize data in various passes
	 */
	init_tree ();
	init_lex ();
	init_rtl ();
	init_emit_once ();
	init_decl_processing ();
	init_optabs ();

	/*
	 *	If rtl dump desired, open the output file
 	 */
	if (rtl_dump)
	{
		register char	*dumpname;

		dumpname = xmalloc (dump_base_name_length + 6);
		strcpy (dumpname, dump_base_name);
		strcat (dumpname, ".rtl");
		rtl_dump_file = fopen (dumpname, "w");

		if (rtl_dump_file == NOFILE)
			pfatal_with_name (dumpname);
	}

	/*
	 *	If jump_opt dump desired, open the output file
	 */
	if (jump_opt_dump)
	{
		register char *dumpname;

		dumpname = xmalloc (dump_base_name_length + 6);
		strcpy (dumpname, dump_base_name);
		strcat (dumpname, ".jump");
		jump_opt_dump_file = fopen (dumpname, "w");

		if (jump_opt_dump_file == NOFILE)
			pfatal_with_name (dumpname);
	}

	/*
 	 *	If cse dump desired, open the output file
	 */
	if (cse_dump)
	{
		register char *dumpname;

		dumpname = xmalloc (dump_base_name_length + 6);
		strcpy (dumpname, dump_base_name);
		strcat (dumpname, ".cse");
		cse_dump_file = fopen (dumpname, "w");

		if (cse_dump_file == NOFILE)
			pfatal_with_name (dumpname);
	}

	/*
	 *	If loop dump desired, open the output file
	 */
	if (loop_dump)
	{
		register char *dumpname;

		dumpname = xmalloc (dump_base_name_length + 6);
		strcpy (dumpname, dump_base_name);
		strcat (dumpname, ".loop");
		loop_dump_file = fopen (dumpname, "w");

		if (loop_dump_file == NOFILE)
			pfatal_with_name (dumpname);
	}

	/*
	 *	If flow dump desired, open the output file
	 */
	if (flow_dump)
	{
		register char *dumpname;

		dumpname = xmalloc (dump_base_name_length + 6);
		strcpy (dumpname, dump_base_name);
		strcat (dumpname, ".flow");
		flow_dump_file = fopen (dumpname, "w");

		if (flow_dump_file == NOFILE)
			pfatal_with_name (dumpname);
	}

	/*
	 *	If combine dump desired, open the output file
	 */
	if (combine_dump)
	{
		register char *dumpname;

		dumpname = xmalloc (dump_base_name_length + 10);
		strcpy (dumpname, dump_base_name);
		strcat (dumpname, ".combine");
		combine_dump_file = fopen (dumpname, "w");

		if (combine_dump_file == NOFILE)
			pfatal_with_name (dumpname);
	}

	/*
	 *	If local_reg dump desired, open the output file
	 */
	if (local_reg_dump)
	{
		register char *dumpname;

		dumpname = xmalloc (dump_base_name_length + 6);
		strcpy (dumpname, dump_base_name);
		strcat (dumpname, ".lreg");
		local_reg_dump_file = fopen (dumpname, "w");

		if (local_reg_dump_file == NOFILE)
			pfatal_with_name (dumpname);
	}

	/*
	 *	If global_reg dump desired, open the output file
	 */
	if (global_reg_dump)
	{
		register char *dumpname;

		dumpname = xmalloc (dump_base_name_length + 6);
		strcpy (dumpname, dump_base_name);
		strcat (dumpname, ".greg");
		global_reg_dump_file = fopen (dumpname, "w");

		if (global_reg_dump_file == NOFILE)
			pfatal_with_name (dumpname);
	}

	/*
	 *	If jump2_opt dump desired, open the output file
	 */
	if (jump2_opt_dump)
	{
		register char *dumpname;

		dumpname = xmalloc (dump_base_name_length + 7);
		strcpy (dumpname, dump_base_name);
		strcat (dumpname, ".jump2");
		jump2_opt_dump_file = fopen (dumpname, "w");

		if (jump2_opt_dump_file == NOFILE)
			pfatal_with_name (dumpname);
	}

	/*
	 *	If dbr_sched dump desired, open the output file
	 */
	if (dbr_sched_dump)
	{
		register char *dumpname;

		dumpname = xmalloc (dump_base_name_length + 7);
		strcpy (dumpname, dump_base_name);
		strcat (dumpname, ".dbr");
		dbr_sched_dump_file = fopen (dumpname, "w");

		if (dbr_sched_dump_file == NOFILE)
			pfatal_with_name (dumpname);
	}

	/*
	 *	Open assembler code output file
	 */
	if (! name_specified && asm_file_name == NOSTR)
	{
		asm_out_file = stdout;
	}
	else
	{
		register char	*dumpname;
		int		len;

		dumpname = xmalloc (dump_base_name_length + 6);
		len = strlen (dump_base_name);
		strcpy (dumpname, dump_base_name);

		if   (len > 2 && ! strcmp (".c", dumpname + len - 2))
			dumpname[len - 2] = 0;
		elif (len > 2 && ! strcmp (".i", dumpname + len - 2))
			dumpname[len - 2] = 0;
		elif (len > 3 && ! strcmp (".co", dumpname + len - 3))
			dumpname[len - 3] = 0;

		strcat (dumpname, ".s");

		if (asm_file_name == NOSTR)
		{
			asm_file_name = malloc (strlen (dumpname) + 1);
			strcpy (asm_file_name, dumpname);
		}

		if (!strcmp (asm_file_name, "-"))
			asm_out_file = stdout;
		else
			asm_out_file = fopen (asm_file_name, "w");

		if (asm_out_file == NOFILE)
			pfatal_with_name (asm_file_name);
	}

	input_filename = name;

	/*
	 *	the beginning of the file is a new line; check for #
	 *	With luck, we discover the real source file's name from
	 *	that and put it in input_filename
	 */
	ungetc (check_newline (), finput);

	/*
	 *	If the input doesn't start with a #line, use the input name
	 *	as the official input file name
	 */
	if (main_input_filename == NOSTR)
		main_input_filename = name;

	/*
	 *	Put an entry on the input file stack for the main input file
	 */
	input_file_stack = xmalloc (sizeof (struct file_stack));
	input_file_stack->next = (struct file_stack *)NULL;
	input_file_stack->name = input_filename;

	ASM_FILE_START (asm_out_file);

	/*
	 *	Output something to inform GDB that this compilation was by GCC
	 */
#ifndef	ASM_IDENTIFY_GCC
	fprintf (asm_out_file, "gcc_compiled.:\n");
#else
	ASM_IDENTIFY_GCC (asm_out_file);
#endif	ASM_IDENTIFY_GCC

	/*
	 *	If GDB symbol table desired, open the GDB symbol output file
	 */
	if (write_symbols == GDB_DEBUG)
	{
		register char	*dumpname;
		int		len;

		dumpname = xmalloc (dump_base_name_length + 6);
		len = strlen (dump_base_name);
		strcpy (dumpname, dump_base_name);

		if (len > 2 && ! strcmp (".c", dumpname + len - 2))
			dumpname[len - 2] = 0;
		elif (len > 2 && ! strcmp (".i", dumpname + len - 2))
			dumpname[len - 2] = 0;
		elif (len > 3 && ! strcmp (".co", dumpname + len - 3))
			dumpname[len - 3] = 0;
		strcat (dumpname, ".sym");

		if (sym_file_name == NOSTR)
			sym_file_name = dumpname;

		symout_init (sym_file_name, asm_out_file, main_input_filename);
	}

	/*
	 *	If dbx symbol table desired, initialize writing it
	 *	and output the predefined types
	 */
#ifdef	DBX_DEBUGGING_INFO
	if (write_symbols == DBX_DEBUG)
		dbxout_init (asm_out_file, main_input_filename);
#endif	DBX_DEBUGGING_INFO

#ifdef	SDB_DEBUGGING_INFO
	if (write_symbols == SDB_DEBUG)
		sdbout_init (asm_out_file, main_input_filename);
#endif	SDB_DEBUGGING_INFO

	/*
	 *	Initialize yet another pass
	 */
	init_final (main_input_filename);

	start_time = gettime ();

	/*
	 *	Call the parser, which parses the entire file
	 *	(calling rest_of_compilation for each function)
	 */
	yyparse ();

	/*
	 *	Compilation is now finished except for writing
	 *	what's left of the symbol table output
	 */

	parse_time += gettime () - start_time;

	parse_time -= integration_time;
	parse_time -= varconst_time;

	globals = getdecls ();

	/*
	 *	Really define vars that have had only a tentative definition.
	 *	Really output inline functions that must actually be callable
	 *	and have not been output so far
	 */
if (!flag_syntax_only)
	for (decl = globals; decl; decl = TREE_CHAIN (decl))
	{
		if
		(	TREE_CODE (decl) == VAR_DECL &&
			TREE_STATIC (decl) &&
			! TREE_ASM_WRITTEN (decl)
		)
		{
			/*
			 *	Don't write out static consts,
			 *	unless we used them. (This used to
			 *	write them out only if the address was
			 *	taken, but that was wrong; if the
			 *	variable was simply referred to,
			 *	it still needs to exist or else it
			 *	will be undefined in the linker.)
			 */
			if
			(	! TREE_READONLY (decl) ||
				TREE_USED (decl) ||
				TREE_PUBLIC (decl) ||
				TREE_ADDRESSABLE (decl)
			)
			{
				rest_of_decl_compilation (decl, NOSTR, 1, 1);
			}
#ifdef	DBX_DEBUGGING_INFO
			elif (DECL_INITIAL (decl) && write_symbols == DBX_DEBUG)
			{
				/*
				 *	Otherwise maybe mention them
				 *	just for the debugger
				 */
				TIMEVAR_BEGIN;
				dbxout_symbol (decl, 0);
				TIMEVAR_END (varconst_time);
			}
#endif	DBX_DEBUGGING_INFO
#ifdef	SDB_DEBUGGING_INFO
			elif (DECL_INITIAL (decl) && write_symbols == SDB_DEBUG)
			{
				/*
				 *	Otherwise maybe mention them
				 *	just for the debugger
				 */
				TIMEVAR_BEGIN;
				sdbout_symbol (decl, 0);
				TIMEVAR_END (varconst_time);
			}
#endif	SDB_DEBUGGING_INFO
		}

		if
		(	TREE_CODE (decl) == FUNCTION_DECL &&
			! TREE_ASM_WRITTEN (decl) &&
			DECL_INITIAL (decl) != NULL_TREE &&
			TREE_ADDRESSABLE (decl) &&
			! TREE_EXTERNAL (decl)
		)
			output_inline_function (decl);

		/*
		 *	Warn about any function declared static but not defined
		 */
		if
		(	warn_unused && TREE_CODE (decl) == FUNCTION_DECL &&
			DECL_INITIAL (decl) == NULL_TREE &&
			TREE_EXTERNAL (decl) &&
			! TREE_PUBLIC (decl)
		)
			warning_with_decl (decl, "`%s' declared but never defined");

		/*
		 *	Warn about statics fns or vars defined but not used,
	 	 *	but not about inline functions since unused inline
		 *	statics is normal practice.
		 *	The TREE_USED bit for file-scope decls is kept in
		 *	the identifier, to handle multiple external decls
		 *	in different scopes
		 */
		if
		(	warn_unused && (TREE_CODE (decl) == FUNCTION_DECL ||
			TREE_CODE (decl) == VAR_DECL) &&
			! TREE_EXTERNAL (decl) && ! TREE_PUBLIC (decl) &&
			! TREE_USED (decl) && ! TREE_INLINE (decl) &&
			! TREE_USED (DECL_NAME (decl))
		)
			warning_with_decl (decl, "`%s' defined but not used");

	}	/* end for (decl = ...) */

	/*
	 *	Do dbx symbols
	 */
#ifdef	DBX_DEBUGGING_INFO
	if (write_symbols == DBX_DEBUG)
	{
		TIMEVAR_BEGIN;
		dbxout_tags (gettags ());
		dbxout_types (get_permanent_types ());
		TIMEVAR_END (symout_time);
	}
#endif	DBX_DEBUGGING_INFO

#ifdef	SDB_DEBUGGING_INFO
	if (write_symbols == SDB_DEBUG)
	{
		TIMEVAR_BEGIN;

		sdbout_tags (gettags ());
		sdbout_types (get_permanent_types ());

		/*
		 *	Output first static file-scope vars, then global ones
		 */
		for (decl = globals; decl; decl = TREE_CHAIN (decl))
		{
			if (TREE_CODE (decl) == VAR_DECL && !TREE_PUBLIC (decl))
				sdbout_symbol (decl, 1);
		}

		for (decl = globals; decl; decl = TREE_CHAIN (decl))
		{
			if (TREE_CODE (decl) == VAR_DECL && TREE_PUBLIC (decl))
				sdbout_symbol (decl, 1);
		}

		TIMEVAR_END (symout_time);
	}
#endif	SDB_DEBUGGING_INFO

	/*
	 *	Do gdb symbols
	 */
	if (write_symbols == GDB_DEBUG)
	{
		TIMEVAR_BEGIN;

		{
			STAT		s;

			fstat (fileno (finput), &s);
			symout_types (get_permanent_types ());
			symout_top_blocks (globals, gettags ());
			symout_finish (name, s.st_ctime);
		}

		TIMEVAR_END (symout_time);
	}

	/*
	 *	Output some stuff at end of file if nec
	 */
	end_final (main_input_filename);

#ifdef	ASM_FILE_END
	ASM_FILE_END (asm_out_file);
#endif	ASM_FILE_END

	/*
	 *	Close the dump files
	 */
	if (rtl_dump)
		fclose (rtl_dump_file);

	if (jump_opt_dump)
		fclose (jump_opt_dump_file);

	if (cse_dump)
		fclose (cse_dump_file);

	if (loop_dump)
		fclose (loop_dump_file);

	if (flow_dump)
		fclose (flow_dump_file);

	if (combine_dump)
	{
		dump_combine_total_stats (combine_dump_file);
		fclose (combine_dump_file);
	}

	if (local_reg_dump)
		fclose (local_reg_dump_file);

	if (global_reg_dump)
		fclose (global_reg_dump_file);

	if (jump2_opt_dump)
		fclose (jump2_opt_dump_file);

	if (dbr_sched_dump)
		fclose (dbr_sched_dump_file);

	/*
	 *	Close non-debugging input and output files. Take special
	 *	care to note whether fclose returns an error, since the
	 *	pages might still be on the buffer chain while the file
	 *	is open
	 */
	fclose (finput);

	if (ferror (asm_out_file) != 0 || fclose (asm_out_file) != 0)
		fatal_io_error (asm_file_name);

	/*
	 *	Print the times
	 */
	if (! quiet_flag)
	{
		fprintf (stderr,"\n");
#if (0)	/*************************************/
		print_time ("parse", parse_time);
		print_time ("integration", integration_time);
		print_time ("jump", jump_time);
		print_time ("cse", cse_time);
		print_time ("loop", loop_time);
		print_time ("flow", flow_time);
		print_time ("combine", combine_time);
		print_time ("local-alloc", local_alloc_time);
		print_time ("global-alloc", global_alloc_time);
		print_time ("dbranch", dbr_sched_time);
		print_time ("final", final_time);
		print_time ("varconst", varconst_time);
		print_time ("symout", symout_time);
		print_time ("dump", dump_time);
#endif	/*************************************/
	}

}	/* end compile_file */
