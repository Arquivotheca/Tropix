/*
 ****************************************************************
 *								*
 *			dcl/data.c				*
 *								*
 *	Process declarations and symbol lookup for C front end.	*
 *	Also constructs types; the standard scalar types at	*
 *	initialization, and structure, union, array and enum	*
 *	types when they are declared.				*
 *								*
 *	Versão	3.0.0, de 26.08.93				*
 *		3.0.0, de 28.12.93				*
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
 ******	Inclusões de arquivos ***********************************
 */
#include <stdio.h>

#include "../h/tree.h"

#include "dcl.h"

/*
 *	a node which has tree code ERROR_MARK, and whose type is itself.
 *	All erroneous expressions are replaced with this node.
 *	All functions that accept nodes as arguments should avoid
 *	generating error messages if this node is one of the
 *	arguments, since it is undesirable to get multiple error
 *	messages from one error in the input
 */
entry tree	error_mark_node;

/*
 *	INTEGER_TYPE and REAL_TYPE nodes for the standard data types
 */
entry tree	short_integer_type_node;
entry tree	integer_type_node;
entry tree	long_integer_type_node;
entry tree	long_long_integer_type_node;

entry tree	short_unsigned_type_node;
entry tree	unsigned_type_node;
entry tree	long_unsigned_type_node;
entry tree	long_long_unsigned_type_node;

entry tree	unsigned_char_type_node;
entry tree	signed_char_type_node;
entry tree	char_type_node;

entry tree	float_type_node;
entry tree	double_type_node;
entry tree	long_double_type_node;

/*
 *	a VOID_TYPE node
 */
entry tree	void_type_node;

/*
 *	A node for type `void *'
 */
entry tree	ptr_type_node;

/*
 *	A node for type `char *'
 */
entry tree	string_type_node;

/*
 *	Type `char[256]' or something like it.
 *	Used when an array of char is needed
 *	and the size is irrelevant
 */
entry tree	char_array_type_node;

/*
 *	Type `int[256]' or something like it.
 *	Used when an array of int needed and
 *	the size is irrelevant
 */
entry tree	int_array_type_node;

/*
 *	type `int ()' -- used for implicit
 *	declaration of functions
 */
entry tree	default_function_type;

/*
 *	function types `double (double)' and
 *	`double (double, double)', etc
 */
entry tree	double_ftype_double,
		double_ftype_double_double;
entry tree	int_ftype_int,
		long_ftype_long;

/*
 *	Function type `void (void *, void *, int)' and similar ones
 */
entry tree	void_ftype_ptr_ptr_int,
		int_ftype_ptr_ptr_int,
		void_ftype_ptr_int_int;

/*
 *	Two expressions that are constants with value zero.
 *	The first is of type `int', the second of type `void *'
 */
entry tree	integer_zero_node;
entry tree	null_pointer_node;

/*
 *	A node for the integer constant 1
 */
entry tree	integer_one_node;

/*
 *	An identifier whose name is <value>.
 *	This is used as the "name" of the
 *	RESULT_DECLs for values of functions
 */
entry tree	value_identifier;

/*
 *	A permanent tree_list whose value slot is the void type
 */
entry tree	void_list_node;

/*
 *	While defining an enum type, this is 1
 *	plus the last enumerator constant value
 */
entry tree	enum_next_value;

/*
 *	Parsing a function declarator leaves a list
 *	of parameter names or a chain or parameter decls here
 */
entry tree	last_function_parms;

/*
 *	Parsing a function declarator leaves here a chain
 *	of structure and enum types declared in the parmlist
 */
entry tree	last_function_parm_tags;

/*
 *	After parsing the declarator that starts a function
 *	definition, `start_function' puts here the list of
 *	parameter names or chain of decls. `store_parm_decls'
 *	finds it here
 */
entry tree	current_function_parms;

/*
 *	Similar, for last_function_parm_tags
 */
entry tree	current_function_parm_tags;

/*
 *	A list (chain of TREE_LIST nodes) of all LABEL_STMTs
 *	in the function that have names. Here so we can clear
 *	out their names' definitions at the end of the function
 */
entry tree	named_labels;

/*
 *	The FUNCTION_DECL for the function currently being compiled,
 *	or 0 if between functions
 */
entry tree	current_function_decl;

/*
 *	Set to 0 at beginning of a function definition, set to 1 if
 *	a return statement that specifies a return value is seen
 */
entry int	current_function_returns_value;

/*
 *	Set to 0 at beginning of a function definition, set to 1 if
 *	a return statement with no argument is seen
 */
entry int	current_function_returns_null;

/*
 *	Set to nonzero by `grokdeclarator' for a function whose
 *	return type is defaulted, if warnings for this are desired
 */
entry int	warn_about_return_type;

/*
 *	Nonzero when starting a function delcared `extern inline'
 */
entry int	current_extern_inline;

/*
 *	For each binding contour we allocate a binding_level structure
 *	which records the names defined in that contour
 *	Contours include:
 *
 *		0) the global one
 *		1) one for each function definition,
 *		   where internal declarations of the parameters appear
 *		2) one for each compound statement,
 *		   to record its declarations
 *
 *	The current meaning of a name can be found by searching the
 *	levels from the current one out to the global one
 */

/*
 *	The binding level currently in effect
 */
entry struct binding_level *current_binding_level;

/*
 *	A chain of binding_level structures awaiting reuse
 */
entry struct binding_level *free_binding_level;

/*
 *	The outermost binding level, for names of file scope.
 *	This is created when the compiler is started and exists
 *	through the entire run
 */
entry struct binding_level *global_binding_level;

/*
 *	Binding level structures are initialized by copying this one
 */
entry struct binding_level clear_binding_level =
{
	NULL_TREE, NULL_TREE, NULL_TREE, NULL_TREE,
	(struct binding_level *)NULL, 0, 0, 0
};

/*
 *	Nonzero means unconditionally make a LET_STMT for
 *	the next level pushed
 */
entry int	keep_next_level_flag;

/*
 *	Nonzero means make a LET_STMT for the
 *	next level pushed if it has subblocks
 */
entry int	keep_next_if_subblocks;

/*
 ******	C-specific option variables *****************************
 */
/*
 *	Nonzero means allow type mismatches in conditional
 *	expressions; just make their values `void'
 */
entry int	flag_cond_mismatch;

/*
 *	Nonzero means don't recognize the keyword `asm'
 */
entry int	flag_no_asm;

/*
 *	Nonzero means do some things the same way PCC does
 */
entry int	flag_traditional;

/*
 *	Nonzero means warn about implicit declarations
 */
entry int	warn_implicit;

/*
 *	Nonzero means warn about function definitions that default
 *	the return type or that use a null return and have a
 *	return-type other than void
 */
entry int	warn_return_type;

/*
 *	Nonzero means give string constants the type `const char *'
 *	to get extra warnings from them. These warnings will be
 *	too numerous to be useful, except in thoroughly ANSIfied programs
 */
entry int	warn_write_strings;

/*
 *	Nonzero means warn about pointer casts that can drop a
 *	type qualifier from the pointer target type
 */
entry int	warn_cast_qual;

/*
 *	Nonzero means warn about sizeof(function) or
 *	addition/subtraction of function pointers
 */
entry int	warn_pointer_arith;

/*
 *	Nonzero means warn for all old-style non-prototype function decls
 */
entry int	warn_strict_prototypes;

/*
 *	Nonzero means `$' can be in an identifier.
 *	 See cccp.c for reasons why this breaks some
 *	obscure ANSI C programs
 */
entry int	dollars_in_ident = DOLLARS_IN_IDENTIFIERS;

entry char	*language_string = "GNU C";
