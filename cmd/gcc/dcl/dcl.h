/*
 ****************************************************************
 *								*
 *			dcl/dcl.h				*
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
 ******	Definições externas *************************************
 */
/*
 *	In grokdeclarator, distinguish syntactic
 *	contexts of declarators
 */
enum decl_context
{
	NORMAL,		/* Ordinary declaration */
	FUNCDEF,	/* Function definition */
	PARM,		/* Declaration of parm before function body */
	FIELD,		/* Declaration inside struct or union */
	TYPENAME	/* Typename (inside cast or sizeof) */
};

/*
 *	??? not all decl nodes are given the most useful possible
 *	line numbers. For example, the CONST_DECLs for enum values
 */

/*
 ******	Definições úteis ****************************************
 */
#undef NULL
#define NULL 0

#define	elif	else if
#define	NOSTR	(char *)NULL

#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))
#define MAX(X,Y) ((X) > (Y) ? (X) : (Y))

#ifndef CHAR_TYPE_SIZE
#define CHAR_TYPE_SIZE BITS_PER_UNIT
#endif

#ifndef SHORT_TYPE_SIZE
#define SHORT_TYPE_SIZE (BITS_PER_UNIT * MIN ((UNITS_PER_WORD + 1) / 2, 2))
#endif

#ifndef INT_TYPE_SIZE
#define INT_TYPE_SIZE BITS_PER_WORD
#endif

#ifndef LONG_TYPE_SIZE
#define LONG_TYPE_SIZE BITS_PER_WORD
#endif

#ifndef LONG_LONG_TYPE_SIZE
#define LONG_LONG_TYPE_SIZE (BITS_PER_WORD * 2)
#endif

#ifndef FLOAT_TYPE_SIZE
#define FLOAT_TYPE_SIZE BITS_PER_WORD
#endif

#ifndef DOUBLE_TYPE_SIZE
#define DOUBLE_TYPE_SIZE (BITS_PER_WORD * 2)
#endif

#ifndef LONG_DOUBLE_TYPE_SIZE
#define LONG_DOUBLE_TYPE_SIZE (BITS_PER_WORD * 2)
#endif

/*
 *	a node which has tree code ERROR_MARK, and whose type is itself.
 *	All erroneous expressions are replaced with this node.
 *	All functions that accept nodes as arguments should avoid
 *	generating error messages if this node is one of the
 *	arguments, since it is undesirable to get multiple error
 *	messages from one error in the input
 */
extern tree	error_mark_node;

/*
 *	INTEGER_TYPE and REAL_TYPE nodes for the standard data types
 */
extern tree	short_integer_type_node;
extern tree	integer_type_node;
extern tree	long_integer_type_node;
extern tree	long_long_integer_type_node;

extern tree	short_unsigned_type_node;
extern tree	unsigned_type_node;
extern tree	long_unsigned_type_node;
extern tree	long_long_unsigned_type_node;

extern tree	unsigned_char_type_node;
extern tree	signed_char_type_node;
extern tree	char_type_node;

extern tree	float_type_node;
extern tree	double_type_node;
extern tree	long_double_type_node;

/*
 *	a VOID_TYPE node
 */
extern tree	void_type_node;

/*
 *	A node for type `void *'
 */
extern tree	ptr_type_node;

/*
 *	A node for type `char *'
 */
extern tree	string_type_node;

/*
 *	Type `char[256]' or something like it.
 *	Used when an array of char is needed
 *	and the size is irrelevant
 */
extern tree	char_array_type_node;

/*
 *	Type `int[256]' or something like it.
 *	Used when an array of int needed and
 *	the size is irrelevant
 */
extern tree	int_array_type_node;

/*
 *	type `int ()' -- used for implicit
 *	declaration of functions
 */
extern tree	default_function_type;

/*
 *	function types `double (double)' and
 *	`double (double, double)', etc
 */
extern tree	double_ftype_double,
		double_ftype_double_double;
extern tree	int_ftype_int,
		long_ftype_long;

/*
 *	Function type `void (void *, void *, int)' and similar ones
 */
extern tree	void_ftype_ptr_ptr_int,
		int_ftype_ptr_ptr_int,
		void_ftype_ptr_int_int;

/*
 *	Two expressions that are constants with value zero.
 *	The first is of type `int', the second of type `void *'
 */
extern tree	integer_zero_node;
extern tree	null_pointer_node;

/*
 *	A node for the integer constant 1
 */
extern tree	integer_one_node;

/*
 *	An identifier whose name is <value>.
 *	This is used as the "name" of the
 *	RESULT_DECLs for values of functions
 */
extern tree	value_identifier;

/*
 *	A permanent tree_list whose value slot is the void type
 */
extern tree	void_list_node;

/*
 *	While defining an enum type, this is 1
 *	plus the last enumerator constant value
 */
extern tree	enum_next_value;

/*
 *	Parsing a function declarator leaves a list
 *	of parameter names or a chain or parameter decls here
 */
extern tree	last_function_parms;

/*
 *	Parsing a function declarator leaves here a chain
 *	of structure and enum types declared in the parmlist
 */
extern tree	last_function_parm_tags;

/*
 *	After parsing the declarator that starts a function
 *	definition, `start_function' puts here the list of
 *	parameter names or chain of decls. `store_parm_decls'
 *	finds it here
 */
extern tree	current_function_parms;

/*
 *	Similar, for last_function_parm_tags
 */
extern tree	current_function_parm_tags;

/*
 *	A list (chain of TREE_LIST nodes) of all LABEL_STMTs
 *	in the function that have names. Here so we can clear
 *	out their names' definitions at the end of the function
 */
extern tree	named_labels;

/*
 *	The FUNCTION_DECL for the function currently being compiled,
 *	or 0 if between functions
 */
extern tree	current_function_decl;

/*
 *	Set to 0 at beginning of a function definition, set to 1 if
 *	a return statement that specifies a return value is seen
 */
extern int	current_function_returns_value;

/*
 *	Set to 0 at beginning of a function definition, set to 1 if
 *	a return statement with no argument is seen
 */
extern int	current_function_returns_null;

/*
 *	Set to nonzero by `grokdeclarator' for a function whose
 *	return type is defaulted, if warnings for this are desired
 */
extern int	warn_about_return_type;

/*
 *	Nonzero when starting a function delcared `extern inline'
 */
extern int	current_extern_inline;

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
 *	Note that the information in the `names' component of the
 *	global contour is duplicated in the IDENTIFIER_GLOBAL_VALUEs
 *	of all identifiers
 */
struct binding_level
{
	/*
	 *	A chain of _DECL nodes for all variables, constants,
	 *	functions, and typedef types. These are in the reverse
	 *	of the order supplied
	 */
	tree	names;

	/*
	 *	A list of structure, union and enum definitions,
	 *	for looking up tag names. It is a chain of TREE_LIST
	 *	nodes, each of whose TREE_PURPOSE is a name, or
	 *	NULL_TREE; and whose TREE_VALUE is a RECORD_TYPE,
	 *	UNION_TYPE, or ENUMERAL_TYPE node
	 */
	tree	tags;

	/*
	 *	For each level, a list of shadowed outer-level local
	 *	definitions to be restored when this level is popped.
	 *	Each link is a TREE_LIST whose TREE_PURPOSE is an
	 *	identifier and whose TREE_VALUE is its old definition
	 *	(a kind of ..._DECL node)
	 */
	tree	shadowed;

	/*
	 *	For each level (except not the global one), a chain of
	 *	LET_STMT nodes for all the levels that were entered and
	 *	exited one level down
	 */
	tree	blocks;

	/*
	 *	The binding level which this one is contained in
	 *	(inherits from)
	 */
	struct binding_level	*level_chain;

	/*
	 *	Nonzero for the level that holds the parameters of a function
	 */
	char		parm_flag;

	/*
	 *	Nonzero if this level "doesn't exist" for tags
	 */
	char		tag_transparent;

	/*
	 *	Nonzero means make a LET_STMT for this level
	 *	regardless of all else
	 */
	char		 keep;

	/*
	 *	Nonzero means make a LET_STMT if this level has any subblocks
	 */
	char		keep_if_subblocks;

	/*
	 *	Number of decls in `names' that have incomplete 
	 *	structure or union types
	 */
	int		n_incomplete;
};

#define NULL_BINDING_LEVEL (struct binding_level *)NULL
	
/*
 *	The binding level currently in effect
 */
extern struct binding_level *current_binding_level;

/*
 *	A chain of binding_level structures awaiting reuse
 */
extern struct binding_level *free_binding_level;

/*
 *	The outermost binding level, for names of file scope.
 *	This is created when the compiler is started and exists
 *	through the entire run
 */
extern struct binding_level *global_binding_level;

/*
 *	Binding level structures are initialized by copying this one
 */
extern struct binding_level clear_binding_level;

/*
 *	Nonzero means unconditionally make a LET_STMT for
 *	the next level pushed
 */
extern int	keep_next_level_flag;

/*
 *	Nonzero means make a LET_STMT for the
 *	next level pushed if it has subblocks
 */
extern int	keep_next_if_subblocks;

/*
 ******	C-specific option variables *****************************
 */
/*
 *	Nonzero means allow type mismatches in conditional
 *	expressions; just make their values `void'
 */
extern int	flag_cond_mismatch;

/*
 *	Nonzero means don't recognize the keyword `asm'
 */
extern int	flag_no_asm;

/*
 *	Nonzero means do some things the same way PCC does
 */
extern int	flag_traditional;

/*
 *	Nonzero means warn about implicit declarations
 */
extern int	warn_implicit;

/*
 *	Nonzero means warn about function definitions that default
 *	the return type or that use a null return and have a
 *	return-type other than void
 */
extern int	warn_return_type;

/*
 *	Nonzero means give string constants the type `const char *'
 *	to get extra warnings from them. These warnings will be
 *	too numerous to be useful, except in thoroughly ANSIfied programs
 */
extern int	warn_write_strings;

/*
 *	Nonzero means warn about pointer casts that can drop a
 *	type qualifier from the pointer target type
 */
extern int	warn_cast_qual;

/*
 *	Nonzero means warn about sizeof(function) or
 *	addition/subtraction of function pointers
 */
extern int	warn_pointer_arith;

/*
 *	Nonzero means warn for all old-style non-prototype function decls
 */
extern int	warn_strict_prototypes;

/*
 *	Nonzero means `$' can be in an identifier.
 *	 See cccp.c for reasons why this breaks some
 *	obscure ANSI C programs
 */
#ifndef DOLLARS_IN_IDENTIFIERS
#define DOLLARS_IN_IDENTIFIERS 0
#endif

extern int	dollars_in_ident;

extern char	*language_string;

/*
 ******	Protótipos de funções ***********************************
 */
extern tree	build_enumerator (tree, tree);
extern void	builtin_function (char *, tree, enum built_in_function);
extern tree	c_build_type_variant (tree, int, int);
extern int	complete_array_type (tree, tree, int);
extern void	declare_parm_level (void);
extern tree	define_label (char *, int, tree);
extern int	duplicate_decls (tree, tree);
extern void	finish_decl (tree, tree, tree);
extern tree	finish_enum (tree, tree);
extern void	finish_function (int);
extern tree	finish_struct (tree, tree);
extern tree	get_parm_info (int);
extern tree	getdecls (void);
extern tree	gettags (void);
extern int	global_bindings_p (void);
extern tree	grokdeclarator (tree, tree, enum decl_context, int);
extern tree	grokfield (char *, int, tree, tree, tree);
extern tree	grokparms (tree, int);
extern tree	groktypename (tree);
extern tree	implicitly_declare (tree);
extern int	in_parm_level_p (void);
extern void	init_decl_processing (void);
extern void	keep_next_level (void);
extern int	kept_level_p (void);
extern int	lang_decode_option (char *);
extern void	layout_array_type (tree);
extern tree	lookup_label (tree);
extern tree	lookup_name (tree);
extern tree	lookup_name_current_level (tree);
extern tree	lookup_tag (enum tree_code, tree, struct binding_level *, int);
extern tree	lookup_tag_reverse (tree);
extern struct binding_level	*make_binding_level (void);
extern void	parmlist_tags_warning (void);
extern tree	poplevel (int, int, int);
extern void	print_lang_identifier (FILE *, tree, int);
extern void	push_parm_decl (tree);
extern tree	pushdecl (tree);
extern void	pushlevel (int);
extern void	pushtag (tree, tree);
extern char	*redeclaration_error_message (tree, tree);
extern void	shadow_tag (tree);
extern tree	start_decl (tree, tree, int);
extern tree	start_enum (tree);
extern int	start_function (tree, tree);
extern tree	start_struct (enum tree_code, tree);
extern void	store_parm_decls (void);
extern void	storedecls (tree);
extern void	storetags (tree);
extern tree	xref_tag (enum tree_code, tree);
