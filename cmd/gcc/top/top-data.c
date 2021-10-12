/*
 ****************************************************************
 *								*
 *			top/top-data.c				*
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
 ******	Inclusões ***********************************************
 */
#pragma	getenv (GEN)

#include <stdio.h>

#include ../$GEN.gen/config.h

#include "../h/tree.h"
#include "../h/flags.h"

#include "top-data.h"

/*
 ******	Dados ***************************************************
 */
/*
 *	Bit flags that specify the machine subtype we
 *	are compiling for. Bits are tested using macros
 *	TARGET_... defined in the tm-...h file and set
 *	by `-m...' switches
 */
entry int	target_flags;

/*
 *	Name of current original source file (what was input to cpp).
 *	This comes from each #-command in the actual input
 */
entry char	*input_filename;

/*
 *	Name of top-level original source file (what was input to cpp).
 *	This comes from the #-command at the beginning of the actual input.
 *	If there isn't any there, then this is the cc1 input file name
 */
entry char	*main_input_filename;

/*
 *	Current line number in real source file
 */
entry int	lineno;

/*
 *	Stack of currently pending input files
 */
entry struct file_stack	*input_file_stack;

/*
 *	Incremented on each change to input_file_stack
 */
entry int	input_file_stack_tick;

/*
 *	Name to use as base of names for dump output files
 */
entry char	*dump_base_name;

/*
 *	Flags saying which kinds of debugging dump have been requested
 */
entry int	rtl_dump = 0;
entry int	rtl_dump_and_exit = 0;
entry int	jump_opt_dump = 0;
entry int	cse_dump = 0;
entry int	loop_dump = 0;
entry int	flow_dump = 0;
entry int	combine_dump = 0;
entry int	local_reg_dump = 0;
entry int	global_reg_dump = 0;
entry int	jump2_opt_dump = 0;
entry int	dbr_sched_dump = 0;

/*
 *	1 => write gdb debugging output (using symout.c).	-g
 *	2 => write dbx debugging output (using dbxout.c).	-G
 *	3 => write sdb debugging output (using sdbout.c).	-g
 */
entry enum debugger	write_symbols = NO_DEBUG;

/*
 *	Nonzero means can use our own extensions to DBX format.
 *	Relevant only with write_symbols == DBX_DEBUG
 */
entry int	use_gdb_dbx_extensions;

/*
 *	Nonzero means do optimizations.	-opt
 */
entry int	optimize = 0;

/*
 *	Nonzero means `char' should be signed
 */
entry int	flag_signed_char;

/*
 *	Nonzero means give an enum type only as many bytes as it needs
 */
entry int	flag_short_enums;

/*
 *	Nonzero for -fcaller-saves: allocate values in regs that need to
 *	be saved across function calls, if that produces overall better code.
 *	Optional now, so people can test it
 */
#ifdef DEFAULT_CALLER_SAVES
entry int	flag_caller_saves = 1;
#else
entry int	flag_caller_saves = 0;
#endif

/*
 *	Nonzero for -fpcc-struct-return: return values the same way PCC does
 */
entry int	flag_pcc_struct_return = 0;

/*
 *	Nonzero for -fforce-mem: load memory value into a register
 *	before arithmetic on it.	This makes better cse but slower compilation
 */
entry int	flag_force_mem = 0;

/*
 *	Nonzero for -fforce-addr: load memory address into a register before
 *	reference to memory.	This makes better cse but slower compilation
 */
entry int	flag_force_addr = 0;

/*
 *	Nonzero for -fdefer-pop: don't pop args after each function call;
 *	instead save them up to pop many calls' args with one insns
 */
entry int	flag_defer_pop = 1;

/*
 *	Nonzero for -ffloat-store: don't allocate floats and doubles
 *	in extended-precision registers
 */
entry int	flag_float_store = 0;

/*
 *	Nonzero for -fcombine-regs: allow instruction combiner to
 *	combine an insn that just copies one reg to another
 */
entry int	flag_combine_regs = 0;

/*
 *	Nonzero enables strength-reduction in loop.c
 */
entry int	flag_strength_reduce = 0;

/*
 *	Nonzero for -fwritable-strings: store string
 *	constants in data segment and don't uniquize them
 */
entry int	flag_writable_strings = 0;

/*
 *	Nonzero means don't put addresses of constant functions
 *	in registers. Used for compiling the Unix kernel, where
 *	strange substitutions are done on the assembly output
 */
entry int	flag_no_function_cse = 0;

/*
 *	Nonzero for -fomit-frame-pointer: don't make a frame
 *	pointer in simple functions that don't require one
 */
entry int	flag_omit_frame_pointer = 0;

/*
 *	Nonzero to inhibit use of define_optimization peephole opts
 */
entry int	flag_no_peephole = 0;

/*
 *	Nonzero means all references through pointers are volatile
 */
entry int	flag_volatile;

/*
 *	Nonzero means just do syntax checking; don't output anything
 */
entry int	flag_syntax_only = 0;

/*
 *	Nonzero means do stupid register allocation.	-noreg.
 *	This and `optimize' are controlled by different switches in cc1,
 *	but normally cc controls them both with the -O switch
 */
entry int	obey_regdecls = 0;

/*
 *	Don't print functions as they are compiled and don't print
 *	times taken by the various passes.	-quiet
 */
entry int	quiet_flag = 0;

/*
 *	Don't print warning messages.	-w
 */
entry int	inhibit_warnings = 0;

/*
 *	Do print extra warnings (such as for uninitialized variables).	-W
 */
entry int	extra_warnings = 0;

/*
 *	Nonzero to warn about unused local variables
 */
entry int	warn_unused;

/*
 *	Nonzero means warn about all declarations which shadow others
 */
entry int	warn_shadow;

/*
 *	Warn if a switch on an enum fails to have a case for every enum value
 */
entry int	warn_switch;

/*
 *	Nonzero means warn about any identifiers that match in the first N
 *	characters.	The value N is in `id_clash_len'
 */
entry int	warn_id_clash;
entry int	id_clash_len;

/*
 *	Number of error messages and warning messages so far
 */
entry int	errorcount = 0;
entry int	warningcount = 0;
entry int	sorrycount = 0;

/*
 *	Name of program invoked, sans directories
 */
entry char	*progname;

/*
 *	Nonzero if generating code to do profiling
 */
entry int	profile_flag = 0;

/*
 *	Nonzero if generating code to do profiling on a line-by-line basis
 */
entry int	profile_block_flag;

/*
 *	Nonzero for -pedantic switch: warn about anything
 *	that standard spec forbids
 */
entry int	pedantic = 0;

/*
 *	Nonzero for -tropix switch: use the way the fantastic
 *	TROPIX operating system likes
 */
entry int	flag_tropix = 0;

/*
 *	Nonzero for -finline-functions: ok to inline
 *	functions that look like good inline candidates
 */
entry int	flag_inline_functions;

/*
 *	Nonzero for -fkeep-inline-functions: even if we make a function
 *	go inline everywhere, keep its defintion around for debugging
 *	purposes
 */
entry int	flag_keep_inline_functions;

/*
 *	Nonzero means make the text shared if supported
 */
entry int	flag_shared_data;

/*
 *	Nonzero means schedule into delayed branch slots if supported
 */
entry int	flag_delayed_branch;

/*
 *	Copy of arguments to main
 */
entry int	save_argc;
entry char	**save_argv;

/*
 *	Name for output file of assembly code, specified with -o
 */
entry char	*asm_file_name;

/*
 *	Name for output file of GDB symbol segment, specified with -symout
 */
entry char	*sym_file_name;

/*
 *	Table of language-independent -f options. STRING is the option name.
 *	 VARIABLE is the address of the variable. ON_VALUE is the value to
 *	store in VARIABLE if `-fSTRING' is seen as an option.
 *	(If `-fno-STRING' is seen as an option, the opposite value is stored.)
 */
const struct f_options	f_options[] =
{
	{ "float-store",	&flag_float_store,		1},
	{ "volatile",		&flag_volatile,			1},
	{ "defer-pop",		&flag_defer_pop,		1},
	{ "omit-frame-pointer",	&flag_omit_frame_pointer,	1},
	{ "strength-reduce",	&flag_strength_reduce,		1},
	{ "writable-strings",	&flag_writable_strings,		1},
	{ "peephole",		&flag_no_peephole,		0},
	{ "force-mem",		&flag_force_mem,		1},
	{ "force-addr",		&flag_force_addr,		1},
	{ "combine-regs",	&flag_combine_regs,		1},
	{ "function-cse",	&flag_no_function_cse,		0},
	{ "inline-functions",	&flag_inline_functions,		1},
	{ "keep-inline-functions", &flag_keep_inline_functions,	1},
	{ "syntax-only",	&flag_syntax_only,		1},
	{ "shared-data",	&flag_shared_data,		1},
	{ "caller-saves",	&flag_caller_saves,		1},
	{ "pcc-struct-return",	&flag_pcc_struct_return,	1},
	{ "delayed-branch",	&flag_delayed_branch,		1},
	{ NOSTR	}
};

/*
 *	Output files for assembler code (real compiler output)
 *	and debugging dumps
 */
entry FILE	*asm_out_file;
entry FILE	*rtl_dump_file;
entry FILE	*jump_opt_dump_file;
entry FILE	*cse_dump_file;
entry FILE	*loop_dump_file;
entry FILE	*flow_dump_file;
entry FILE	*combine_dump_file;
entry FILE	*local_reg_dump_file;
entry FILE	*global_reg_dump_file;
entry FILE	*jump2_opt_dump_file;
entry FILE	*dbr_sched_dump_file;

/*
 *	Time accumulators, to count the total time spent in various passes
 */
entry int	parse_time;
entry int	varconst_time;
entry int	integration_time;
entry int	jump_time;
entry int	cse_time;
entry int	loop_time;
entry int	flow_time;
entry int	combine_time;
entry int	local_alloc_time;
entry int	global_alloc_time;
entry int	dbr_sched_time;
entry int	final_time;
entry int	symout_time;
entry int	dump_time;
