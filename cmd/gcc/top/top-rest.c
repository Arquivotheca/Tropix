/*
 ****************************************************************
 *								*
 *			top/top-rest.c				*
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

#include <stdio.h>

#include ../$GEN.gen/config.h

#include "../h/tree.h"
#include "../h/c-tree.h"
#include "../h/rtl.h"
#include "../h/flags.h"

#include "top-data.h"
#include "top.h"

/*
 ****************************************************************
 *	Sets up the RTL and outputs assembler code		*
 ****************************************************************
 */
/*
 *	This is called from finish_decl (within yyparse) for
 *	each declaration of a function or variable. This does
 *	nothing for automatic variables. Otherwise, it sets
 *	up the RTL and outputs any assembler code (label
 *	definition, storage allocation and initialization).
 *
 *	DECL is the declaration. If ASMSPEC is nonzero,
 *	it specifies the assembler symbol name to be used.
 *	TOP_LEVEL is nonzero if this declaration is not
 *	within a function
 */
void
rest_of_decl_compilation (tree decl, char *asmspec, int top_level, int at_end)
{
	int		otime;

	/*
	*	Declarations of variables, and of functions defined elsewhere
	*/
	if (TREE_STATIC (decl) || TREE_EXTERNAL (decl))
	{
		TIMEVAR_BEGIN;

		make_decl_rtl (decl, asmspec, top_level);

		/*
		*	Don't output anything when a tentative file-scope
		*	definition is seen. But at end of compilation,
		*	do output code for them
		*/
		if
		(	! (! at_end && top_level &&
			(DECL_INITIAL (decl) == NULL_TREE ||
			DECL_INITIAL (decl) == error_mark_node))
		)
			assemble_variable (decl, top_level, write_symbols, at_end);

		TIMEVAR_END (varconst_time);
	}
	elif (TREE_REGDECL (decl) && asmspec != NOSTR)
	{
		if (decode_reg_name (asmspec) >= 0)
		{
			DECL_RTL (decl) = NULL_RTX;
			make_decl_rtl (decl, asmspec, top_level);
		}
		else
		{
			error ("invalid register name `%s' for register variable", asmspec);
		}
	}
#ifdef	DBX_DEBUGGING_INFO
	elif (write_symbols == DBX_DEBUG && TREE_CODE (decl) == TYPE_DECL)
	{
		TIMEVAR_BEGIN;
		dbxout_symbol (decl, 0);
		TIMEVAR_END (varconst_time);
	}
#endif	DBX_DEBUGGING_INFO
#ifdef	SDB_DEBUGGING_INFO
	elif
	(	write_symbols == SDB_DEBUG &&
		top_level && TREE_CODE (decl) == TYPE_DECL
	)
	{
		TIMEVAR_BEGIN;
		sdbout_symbol (decl, 0);
		TIMEVAR_END (varconst_time);
	}
#endif	SDB_DEBUGGING_INFO

	if (top_level)
	{
		if (write_symbols == GDB_DEBUG)
		{
			TIMEVAR_BEGIN;

			/*
			*	The initizations make types when they contain
			*	string constants. The types are on the
			*	temporary obstack, so output them now before
			*	they go away
			*/
			symout_types (get_temporary_types ());
			TIMEVAR_END (symout_time);
		}
		else
		{
			/*
			*	Clean out the temporary type list,
			*	since the types will go away
			*/
			get_temporary_types ();
		}
	}

}	/* end rest_of_decl_compilation */

/*
 ****************************************************************
 *	Compile a function or variable				*
 ****************************************************************
 */
/*
 *	This is called from finish_function (within yyparse)
 *	after each top-level definition is parsed. It is supposed
 *	to compile that function or variable and output the
 *	assembler code for it. After we return, the tree
 *	storage is freed
 */
void
rest_of_compilation (tree decl)
{
	register rtx	insns;
	int		start_time = gettime ();
	int		tem, otime;

	/*
	*	If we are reconsidering an inline function at the end
	*	of compilation, skip the stuff for making it inline
	*/
	if (DECL_SAVED_INSNS (decl) == NULL_RTX)
	{
		/*
		*	If requested, consider whether to make this
		*	function inline
		*/
		if (flag_inline_functions || TREE_INLINE (decl))
		{
			int		specd;
			char		*lose;

			TIMEVAR_BEGIN;

			specd = TREE_INLINE (decl);
			lose = function_cannot_inline_p (decl);

			if (lose != NOSTR && specd)
				warning_with_decl (decl, lose);
			if (lose == NOSTR)
				save_for_inline (decl);
			else
				TREE_INLINE (decl) = 0;

			TIMEVAR_END (integration_time);
		}

		insns = get_insns ();

		/*
		*	Dump the rtl code if we are dumping rtl
		*/
		if (rtl_dump)
		{
			TIMEVAR_BEGIN;

			fprintf
			(	rtl_dump_file,
				"\n;; Function %s\n\n",
				IDENTIFIER_POINTER (DECL_NAME (decl))
			);

			if (DECL_SAVED_INSNS (decl))
				fprintf (rtl_dump_file, ";; (integrable)\n\n");

			print_rtl (rtl_dump_file, insns);
			fflush (rtl_dump_file);

			TIMEVAR_END (dump_time);
		}

		/*
		*	If function is inline, and we don't yet know
		*	whether to compile it by itself, defer decision
		*	till end of compilation. finish_compilation will
		*	call rest_of_compilation again for those functions
		*	that need to be output
		*/
		if
		(	((! TREE_PUBLIC (decl) && ! TREE_ADDRESSABLE (decl) &&
			! flag_keep_inline_functions) ||
			TREE_EXTERNAL (decl)) && TREE_INLINE (decl)
		)
			goto exit_rest_of_compilation;
	}

	if (rtl_dump_and_exit || flag_syntax_only)
	{
		get_temporary_types ();
		goto exit_rest_of_compilation;
	}

	TREE_ASM_WRITTEN (decl) = 1;

	insns = get_insns ();

	/*
	*	Copy any shared structure that should not be shared
	*/
	unshare_all_rtl (insns);

	/*
	*	See if we have allocated stack slots that are not directly
	*	addressable. If so, scan all the insns and create explicit
	*	address computation for all references to such slots
	*/
#if (0)	/* --------------------------------------------------- */
		fixup_stack_slots ();
#endif 	/* --------------------------------------------------- */

	/*
	*	Do jump optimization the first time, if -opt. Also do it
	*	if -W, but in that case it doesn't change the rtl code,
	*	it only computes whether control can drop off the end of
	*	the function. If function is `volatile', we should warn
	*	if it tries to return
	*/
	if (optimize || extra_warnings || warn_return_type || TREE_THIS_VOLATILE (decl))
	{
		TIMEVAR_BEGIN;
		jump_optimize (insns, 0, 0);
		TIMEVAR_END (jump_time);
	}

	/*
	*	Dump rtl code after jump, if we are doing that
	*/
	if (jump_opt_dump)
	{
		TIMEVAR_BEGIN;

		fprintf
		(	jump_opt_dump_file, "\n;; Function %s\n\n",
			IDENTIFIER_POINTER (DECL_NAME (decl))
		);

		print_rtl (jump_opt_dump_file, insns);
		fflush (jump_opt_dump_file);

		TIMEVAR_END (dump_time);
	}

	/*
	*	Perform common subexpression elimination. Nonzero value
	*	from `cse_main' means that jumps were simplified and some
	*	code may now be unreachable, so do jump optimization again
	*/
	if (optimize)
	{
		TIMEVAR_BEGIN;
		reg_scan (insns, max_reg_num (), 0);
		TIMEVAR_END (cse_time);

		TIMEVAR_BEGIN;
		tem = cse_main (insns, max_reg_num ());
		TIMEVAR_END (cse_time);

		TIMEVAR_BEGIN;
		if (tem)
			jump_optimize (insns, 0, 0);
		TIMEVAR_END (jump_time);
	}

	/*
	*	Dump rtl code after cse, if we are doing that
	*/
	if (cse_dump)
	{
		TIMEVAR_BEGIN;

		fprintf
		(	cse_dump_file, "\n;; Function %s\n\n",
			IDENTIFIER_POINTER (DECL_NAME (decl))
		);

		print_rtl (cse_dump_file, insns);
		fflush (cse_dump_file);

		TIMEVAR_END (dump_time);
	}

	if (loop_dump)
	{
		TIMEVAR_BEGIN;

		fprintf
		(	loop_dump_file, "\n;; Function %s\n\n",
			IDENTIFIER_POINTER (DECL_NAME (decl))
		);

		TIMEVAR_END (dump_time);
	}

	/*
	*	Move constant computations out of loops
	*/
	if (optimize)
	{
		TIMEVAR_BEGIN;

		reg_scan (insns, max_reg_num (), 1);
		loop_optimize (insns, loop_dump ? loop_dump_file : NOFILE);

		TIMEVAR_END (loop_time);
	}

	/*
	*	Dump rtl code after loop opt, if we are doing that
	*/
	if (loop_dump)
	{
		TIMEVAR_BEGIN;

		print_rtl (loop_dump_file, insns);
		fflush (loop_dump_file);

		TIMEVAR_END (dump_time);
	}

	/*
	*	Now we choose between stupid (pcc-like) register allocation
	*	(if we got the -noreg switch and not -opt) and smart register
	*	allocation. Stupid allocation probably won't work if
	*	optimizations being done
	*/
	if (optimize)
		obey_regdecls = 0;

	regclass_init ();

	/*
	*	Print function header into flow dump now because doing the
	*	flow analysis makes some of the dump
	*/
	if (flow_dump)
	{
		TIMEVAR_BEGIN;

		fprintf
		(	flow_dump_file,
			"\n;; Function %s\n\n",
			IDENTIFIER_POINTER (DECL_NAME (decl))
		);

		TIMEVAR_END (dump_time);
	}

	if (obey_regdecls)
	{
		TIMEVAR_BEGIN;

		regclass (insns, max_reg_num ());
		stupid_life_analysis (insns, max_reg_num (), flow_dump_file);

		TIMEVAR_END (flow_time);
	}
	else
	{
		/*
		*	Do control and data flow analysis, and write
		*	some of the results to dump file
		*/
		TIMEVAR_BEGIN;

		flow_analysis (insns, max_reg_num (), flow_dump_file);

		TIMEVAR_END (flow_time);

		if (extra_warnings)
			uninitialized_vars_warning (DECL_INITIAL (decl));
	}

	/*
	*	Dump rtl after flow analysis
	*/
	if (flow_dump)
	{
		TIMEVAR_BEGIN;

		print_rtl (flow_dump_file, insns);
		fflush (flow_dump_file);

		TIMEVAR_END (dump_time);
	}

	/*
	*	If -opt, try combining insns through substitution
	*/
	if (optimize)
	{
		TIMEVAR_BEGIN;

		combine_instructions (insns, max_reg_num ());

		TIMEVAR_END (combine_time);
	}

	/*
	*	Dump rtl code after insn combination
	*/
	if (combine_dump)
	{
		TIMEVAR_BEGIN;

		fprintf
		(	combine_dump_file,
			"\n;; Function %s\n\n",
			IDENTIFIER_POINTER (DECL_NAME (decl))
		);

		dump_combine_stats (combine_dump_file);
		print_rtl (combine_dump_file, insns);
		fflush (combine_dump_file);

		TIMEVAR_END (dump_time);
	}

	/*
	*	Unless we did stupid register allocation, allocate
	*	pseudo-regs that are used only within 1 basic block
	*/
	if (!obey_regdecls)
	{
		TIMEVAR_BEGIN;

		regclass (insns, max_reg_num ());
		local_alloc ();

		TIMEVAR_END (local_alloc_time);
	}

	/*
	*	Dump rtl code after allocating regs within basic blocks
	*/
	if (local_reg_dump)
	{
		TIMEVAR_BEGIN;

		fprintf
		(	local_reg_dump_file,
			"\n;; Function %s\n\n",
			IDENTIFIER_POINTER (DECL_NAME (decl))
		);

		dump_flow_info (local_reg_dump_file);
		dump_local_alloc (local_reg_dump_file);
		print_rtl (local_reg_dump_file, insns);
		fflush (local_reg_dump_file);

		TIMEVAR_END (dump_time);
	}

	if (global_reg_dump)
	{
		TIMEVAR_BEGIN;

		fprintf
		(	global_reg_dump_file,
			"\n;; Function %s\n\n",
			IDENTIFIER_POINTER (DECL_NAME (decl))
		);

		TIMEVAR_END (dump_time);
	}

	/*
	*	Unless we did stupid register allocation, allocate remaining
	*	pseudo-regs, then do the reload pass fixing up any insns that
	*	are invalid
	*/
	TIMEVAR_BEGIN;

	if (!obey_regdecls)
		global_alloc (global_reg_dump ? global_reg_dump_file : NOFILE);
	else
		reload (insns, 0, global_reg_dump ? global_reg_dump_file : NOFILE);

	TIMEVAR_END (global_alloc_time);

	if (global_reg_dump)
	{
		TIMEVAR_BEGIN;

		dump_global_regs (global_reg_dump_file);
		print_rtl (global_reg_dump_file, insns);
		fflush (global_reg_dump_file);

		TIMEVAR_END (dump_time);
	}

	rtx_equal_function_value_matters = 1;
	reload_completed = 1;

	/*
	*	One more attempt to remove jumps to .+1 left by
	*	dead-store-elimination. Also do cross-jumping this time
	*	and delete no-op move insns
	*/
	if (optimize)
	{
		TIMEVAR_BEGIN;

		jump_optimize (insns, 1, 1);

		TIMEVAR_END (jump_time);
	}

	/*
	*	Dump rtl code after jump, if we are doing that
	*/
	if (jump2_opt_dump)
	{
		TIMEVAR_BEGIN;

		fprintf
		(	jump2_opt_dump_file,
			"\n;; Function %s\n\n",
			IDENTIFIER_POINTER (DECL_NAME (decl))
		);

		print_rtl (jump2_opt_dump_file, insns);
		fflush (jump2_opt_dump_file);

		TIMEVAR_END (dump_time);
	}

	/*
	*	If a scheduling pass for delayed branches is to be done,
	*	call the scheduling code
	*/
#ifdef	HAVE_DELAYED_BRANCH
	if (optimize && flag_delayed_branch)
	{
		TIMEVAR_BEGIN;

		dbr_schedule (insns, dbr_sched_dump_file);

		TIMEVAR_END (dbr_sched_time);

		if (dbr_sched_dump)
		{
			TIMEVAR_BEGIN;

			fprintf
			(	dbr_sched_dump_file,
				"\n;; Function %s\n\n",
				IDENTIFIER_POINTER (DECL_NAME (decl))
			);

			print_rtl (dbr_sched_dump_file, insns);
			fflush (dbr_sched_dump_file);

			TIMEVAR_END (dump_time);
		}
	}
#endif	HAVE_DELAYED_BRANCH

	/*
	*	Now turn the rtl into assembler code
	*/
	TIMEVAR_BEGIN;

	assemble_function (decl);
	final_start_function (insns, asm_out_file, write_symbols, optimize);
	final (insns, asm_out_file, write_symbols, optimize, 0);
	final_end_function (insns, asm_out_file, write_symbols, optimize);
	fflush (asm_out_file);

	TIMEVAR_END (final_time);

	/*
	*	Write GDB symbols if requested
	*/
	if (write_symbols == GDB_DEBUG)
	{
		TIMEVAR_BEGIN;

		symout_types (get_permanent_types ());
		symout_types (get_temporary_types ());

		DECL_BLOCK_SYMTAB_ADDRESS (decl) = symout_function (DECL_INITIAL (decl), DECL_ARGUMENTS (decl), 0);
		symout_function_end ();
		TIMEVAR_END (symout_time);
	}
	else
	{
		get_temporary_types ();
	}

	/*
	*	Write DBX symbols if requested
	*/
#ifdef	DBX_DEBUGGING_INFO
	if (write_symbols == DBX_DEBUG)
	{
		TIMEVAR_BEGIN;

		dbxout_function (decl);

		TIMEVAR_END (symout_time);
	}
#endif	DBX_DEBUGGING_INFO

 exit_rest_of_compilation:
	rtx_equal_function_value_matters = 0;
	reload_completed = 0;

	/*
	*	Clear out the real_constant_chain before some of the rtx's
	*	it runs through become garbage
	*/
	clear_const_double_mem ();

	/*
	*	The parsing time is all the time spent in yyparse
	*	*except* what is spent in this function
	*/
	parse_time -= gettime () - start_time;

}	/* end rest_of_compilation */
