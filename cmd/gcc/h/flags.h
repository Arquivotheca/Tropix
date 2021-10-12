/*
 ****************************************************************
 *								*
 *			flags.h					*
 *								*
 *	Compilation switch flag definitions for GNU CC		*
 *								*
 *	Versão	3.0.0, de 05.10.93				*
 *		3.0.0, de 05.10.93				*
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
 *	Name of top-level original source file (what was input to cpp).
 *	This comes from the #-command at the beginning of the actual input.
 *	If there isn't any there, then this is the cc1 input file name
 */
extern char	*main_input_filename;

/*
 *	1 => write gdb debugging output (using symout.c).	-g
 *	2 => write dbx debugging output (using dbxout.c).	-G
 *	3 => write sdb debugging output (using sdbout.c).	-g
 */
enum debugger
{
	NO_DEBUG = 0,
	GDB_DEBUG = 1,
	DBX_DEBUG = 2,
	SDB_DEBUG = 3,
	EXTENDED_DBX_DEBUG = 4
};

extern enum debugger	write_symbols;

/*
 *	Nonzero means can use our own extensions to DBX format.
 *	Relevant only with write_symbols == DBX_DEBUG
 */
extern int	use_gdb_dbx_extensions;

/*
 *	Nonzero means do optimizations.  -opt.
 */
extern int	optimize;

/*
 *	Nonzero means do stupid register allocation.  -noreg.
 *	This and `optimize' are controlled by different switches in cc1,
 *	but normally cc controls them both with the -O switch.
 */
extern int	obey_regdecls;

/*
 *	Don't print functions as they are compiled and don't print
 *	times taken by the various passes.  -quiet.
 */
extern int	quiet_flag;

/*
 *	Don't print warning messages.  -w.
 */
extern int	inhibit_warnings;

/*
 *	Do print extra warnings (such as for uninitialized variables).  -W.
 */
extern int	extra_warnings;

/*
 *	Nonzero to warn about unused local variables.
 */
extern int	warn_unused;

/*
 *	Nonzero means warn about all declarations which shadow others.
 */
extern int	warn_shadow;

/*
 *	Warn if a switch on an enum fails to have a case for every enum value.
 */
extern int	warn_switch;

/*
 *	Nonzero means warn about any identifiers that match in the first N
 *	characters.  The value N is in `id_clash_len'.
 */
extern int	warn_id_clash;
extern int	id_clash_len;

/*
 *	Nonzero if generating code to do profiling.
 */
extern int	profile_flag;

/*
 *	Nonzero if generating code to do profiling on the basis of basic blocks.
 */
extern int	profile_block_flag;

/*
 *	Nonzero for -pedantic switch: warn about anything
 *	that standard C forbids.
 */
extern int	pedantic;

/*
 *	Nonzero for -tropix switch: use the way the fantastic
 *	TROPIX operating system likes
 */
extern int	flag_tropix;

/*
 ******	Now the symbols that are set with `-f' switches *********
 */

/*
 *	Nonzero means `char' should be signed
 */
extern int	flag_signed_char;

/*
 *	Nonzero means give an enum type only as many bytes as it needs
 */
extern int	flag_short_enums;

/*
 *	Nonzero for -fcaller-saves: allocate values in regs that need to
 *	be saved across function calls, if that produces overall better code.
 *	Optional now, so people can test it.
 */
extern int	flag_caller_saves;

/*
 *	Nonzero for -fpcc-struct-return: return values the same way PCC does.
 */
extern int	flag_pcc_struct_return;

/*
 *	Nonzero for -fforce-mem: load memory value into a register
 *	before arithmetic on it.  This makes better cse but slower compilation.
 */
extern int	flag_force_mem;

/*
 *	Nonzero for -fforce-addr: load memory address into a register before
 *	reference to memory.  This makes better cse but slower compilation.
 */
extern int	flag_force_addr;

/*
 *	Nonzero for -fdefer-pop: don't pop args after each function call;
 *	instead save them up to pop many calls' args with one insns.
 */
extern int	flag_defer_pop;

/*
 *	Nonzero for -ffloat-store: don't allocate floats and doubles
 *	in extended-precision registers.
 */
extern int	flag_float_store;

/*
 *	Nonzero for -fcombine-regs:
 *	allow instruction combiner to combine an insn
 *	that just copies one reg to another.
 */
extern int	flag_combine_regs;

/*
 *	Nonzero enables strength-reduction in loop.c.
 */
extern int	flag_strength_reduce;

/*
 *	Nonzero for -fwritable-strings:
 *	store string constants in data segment and don't uniquize them.
 */
extern int	flag_writable_strings;

/*
 *	Nonzero means don't put addresses of constant functions in registers.
 *	Used for compiling the Unix kernel, where strange substitutions are
 *	done on the assembly output.
 */
extern int	flag_no_function_cse;

/*
 *	Nonzero for -fomit-frame-pointer:
 *	don't make a frame pointer in simple functions that don't require one.
 */
extern int	flag_omit_frame_pointer;

/*
 *	This isn't a flag, but everyone who needs flag_omit_frame_pointer
 *	also needs this.
 *	Nonzero means current function must be given a frame pointer.
 *	Set in stmt.c if anything is allocated on the stack there.
 *	Set in reload1.c if anything is allocated on the stack there.
 */
extern int	frame_pointer_needed;

/*
 *	Nonzero to inhibit use of define_optimization peephole opts.
 */
extern int	flag_no_peephole;

/*
 *	Nonzero means all references through pointers are volatile.
 */
extern int	flag_volatile;

/*
 *	Nonzero means make functions that look like good inline candidates
 *	go inline.
 */
extern int	flag_inline_functions;

/*
 *	Nonzero for -fkeep-inline-functions: even if we make a function
 *	go inline everywhere, keep its defintion around for debugging
 *	purposes.
 */
extern int	flag_keep_inline_functions;

/*
 *	Nonzero if we are only using compiler to check syntax errors.
 */
extern int	flag_syntax_only;

/*
 *	Nonzero means make the text shared if supported.
 */
extern int	flag_shared_data;

/*
 *	Nonzero means put things in delayed-branch slots if supported.
 */
extern int	flag_delayed_branch;
