/*
 ****************************************************************
 *								*
 *			top/top-data.h				*
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
 ******	Definições úteis ****************************************
 */
#define	elif	else if
#define	NOSTR	(char *)NULL

#define TIMEVAR_BEGIN		otime = gettime ()

#define TIMEVAR_END(var)	var += gettime () - otime

#if (0)	/*************************************/
#define TIMEVAR(VAR, BODY)    \
do { int otime = gettime (); BODY; VAR += gettime () - otime; } while (0)
#endif	/*************************************/

/*
 ******	Dados ***************************************************
 */
/*
 *	Bit flags that specify the machine subtype we
 *	are compiling for. Bits are tested using macros
 *	TARGET_... defined in the tm-...h file and set
 *	by `-m...' switches
 */
extern int	target_flags;

/*
 *	Name of current original source file (what was input to cpp).
 *	This comes from each #-command in the actual input
 */
extern char	*input_filename;

/*
 *	Current line number in real source file
 */
extern int	lineno;

/*
 *	Stack of currently pending input files
 */
extern struct file_stack	*input_file_stack;

/*
 *	Incremented on each change to input_file_stack
 */
extern int	input_file_stack_tick;

/*
 *	Name to use as base of names for dump output files
 */
extern char	*dump_base_name;

/*
 *	Flags saying which kinds of debugging dump have been requested
 */
extern int	rtl_dump;
extern int	rtl_dump_and_exit;
extern int	jump_opt_dump;
extern int	cse_dump;
extern int	loop_dump;
extern int	flow_dump;
extern int	combine_dump;
extern int	local_reg_dump;
extern int	global_reg_dump;
extern int	jump2_opt_dump;
extern int	dbr_sched_dump;

/*
 *	1 => write gdb debugging output (using symout.c).	-g
 *	2 => write dbx debugging output (using dbxout.c).	-G
 *	3 => write sdb debugging output (using sdbout.c).	-g
 */
extern enum debugger	write_symbols;

/*
 *	Number of error messages and warning messages so far
 */
extern int	errorcount;
extern int	warningcount;
extern int	sorrycount;

/*
 *	Name of program invoked, sans directories
 */
extern char	*progname;

/*
 *	Copy of arguments to main
 */
extern int	save_argc;
extern char	**save_argv;

/*
 *	Name for output file of assembly code, specified with -o
 */
extern char	*asm_file_name;

/*
 *	Name for output file of GDB symbol segment, specified with -symout
 */
extern char	*sym_file_name;

/*
 *	Table of language-independent -f options. STRING is the option name.
 *	 VARIABLE is the address of the variable. ON_VALUE is the value to
 *	store in VARIABLE if `-fSTRING' is seen as an option.
 *	(If `-fno-STRING' is seen as an option, the opposite value is stored.)
 */
struct f_options
{	
	char	*string;
	int	*variable;
	int	on_value;
};

extern const struct f_options	f_options[];

/*
 *	Output files for assembler code (real compiler output)
 *	and debugging dumps
 */
extern FILE	*asm_out_file;
extern FILE	*rtl_dump_file;
extern FILE	*jump_opt_dump_file;
extern FILE	*cse_dump_file;
extern FILE	*loop_dump_file;
extern FILE	*flow_dump_file;
extern FILE	*combine_dump_file;
extern FILE	*local_reg_dump_file;
extern FILE	*global_reg_dump_file;
extern FILE	*jump2_opt_dump_file;
extern FILE	*dbr_sched_dump_file;

/*
 *	Time accumulators, to count the total time spent in various passes
 */
extern int	parse_time;
extern int	varconst_time;
extern int	integration_time;
extern int	jump_time;
extern int	cse_time;
extern int	loop_time;
extern int	flow_time;
extern int	combine_time;
extern int	local_alloc_time;
extern int	global_alloc_time;
extern int	dbr_sched_time;
extern int	final_time;
extern int	symout_time;
extern int	dump_time;

/*
 ******	Definições externas *************************************
 */
/*
 *	FUNCTION_DECL for function now being parsed or compiled
 */
extern tree	current_function_decl;

extern int	yydebug;

extern FILE	*finput;

extern int	reload_completed;
extern int	rtx_equal_function_value_matters;

/*
 ******	Protótipos de funções ***********************************
 */
extern void	init_lex ();
extern void	init_decl_processing ();
extern void	init_tree ();
extern void	init_rtl ();
extern void	init_optabs ();
extern void	init_reg_sets ();
extern void	dump_flow_info ();
extern void	dump_local_alloc ();

void		rest_of_decl_compilation ();
void		set_target_switch ();
void		print_target_switch_defaults ();

extern void	float_signal (void);
extern void	pipe_closed (void);
