/*
 ****************************************************************
 *								*
 *			dcl-dcl.c				*
 *								*
 *	Process declarations and symbol lookup for C front end.	*
 *	Also constructs types; the standard scalar types at	*
 *	initialization, and structure, union, array and enum	*
 *	types when they are declared.				*
 *								*
 *	Versão	3.0.0, de 30.12.93				*
 *		3.0.0, de 30.12.93				*
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
#pragma	getenv (GEN)

#include <stdio.h>

#include ../$GEN.gen/config.h

#include "../h/tree.h"
#include "../h/c-tree.h"
#include "../h/flags.h"
#include "../h/c-parse.h"

#include "../tree/tree.h"
#include "../tchk/tchk.h"
#include "../top/top.h"

#include "dcl.h"

/*
 ****************************************************************
 *	Create the predefined scalar types of C			*
 ****************************************************************
 */
/*
 *	Create the predefined scalar types of C, and some
 *	nodes representing standard constants (0, 1, (void *)0).
 *	Initialize the global binding level. Make definitions
 *	for built-in primitive functions
 */
void
init_decl_processing (void)
{
	register tree	endlink;

	/*
	 *	Make identifier nodes long enough for the
	 *	language-specific slots
	 */
	set_identifier_size (sizeof (struct lang_identifier));

	current_function_decl = NULL_TREE;
	named_labels = NULL_TREE;
	current_binding_level = NULL_BINDING_LEVEL;
	free_binding_level = NULL_BINDING_LEVEL;

	/*
	 *	make the binding_level structure for global names
	 */
	pushlevel (0);
	global_binding_level = current_binding_level;

	value_identifier = get_identifier ("<value>");

	/*
	 *	Define `int' and `char' first so that dbx
	 *	will output them first
	 */
	integer_type_node = make_signed_type (INT_TYPE_SIZE);

	pushdecl (build_decl (TYPE_DECL, ridpointers[RID_INT], integer_type_node));

	/*
	 *	Define `char', which is like either `signed char'
	 *	or `unsigned char' but not the same as either
	 */
	char_type_node = (flag_signed_char ?
		make_signed_type (CHAR_TYPE_SIZE) : make_unsigned_type (CHAR_TYPE_SIZE));
	pushdecl (build_decl (TYPE_DECL, get_identifier ("char"), char_type_node));

	long_integer_type_node = make_signed_type (LONG_TYPE_SIZE);
	pushdecl (build_decl (TYPE_DECL, get_identifier ("long int"),
			long_integer_type_node));

	unsigned_type_node = make_unsigned_type (INT_TYPE_SIZE);
	pushdecl (build_decl (TYPE_DECL, get_identifier ("unsigned int"),
			unsigned_type_node));

	long_unsigned_type_node = make_unsigned_type (LONG_TYPE_SIZE);
	pushdecl (build_decl (TYPE_DECL, get_identifier ("long unsigned int"),
			long_unsigned_type_node));

	/*
	 *	`unsigned long' or `unsigned int' is the standard type
	 *	for sizeof. Traditionally, use a signed type
	 */
	if (INT_TYPE_SIZE != LONG_TYPE_SIZE)
		sizetype = flag_traditional ? long_integer_type_node : long_unsigned_type_node;
	else
		sizetype = flag_traditional ? integer_type_node : unsigned_type_node;

	TREE_TYPE (TYPE_SIZE (integer_type_node)) = sizetype;
	TREE_TYPE (TYPE_SIZE (char_type_node)) = sizetype;
	TREE_TYPE (TYPE_SIZE (unsigned_type_node)) = sizetype;
	TREE_TYPE (TYPE_SIZE (long_unsigned_type_node)) = sizetype;
	TREE_TYPE (TYPE_SIZE (long_integer_type_node)) = sizetype;

	error_mark_node = make_node (ERROR_MARK);
	TREE_TYPE (error_mark_node) = error_mark_node;

	short_integer_type_node = make_signed_type (SHORT_TYPE_SIZE);
	pushdecl (build_decl (TYPE_DECL, get_identifier ("short int"),
			short_integer_type_node));

	long_long_integer_type_node = make_signed_type (LONG_LONG_TYPE_SIZE);
	pushdecl (build_decl (TYPE_DECL, get_identifier ("long long int"),
			long_long_integer_type_node));

	short_unsigned_type_node = make_unsigned_type (SHORT_TYPE_SIZE);
	pushdecl (build_decl (TYPE_DECL, get_identifier ("short unsigned int"),
			short_unsigned_type_node));

	long_long_unsigned_type_node = make_unsigned_type (LONG_LONG_TYPE_SIZE);
	pushdecl (build_decl (TYPE_DECL, get_identifier ("long long unsigned int"),
			long_long_unsigned_type_node));

	/*
	 *	Define both `signed char' and `unsigned char'
	 */
	signed_char_type_node = make_signed_type (CHAR_TYPE_SIZE);
	pushdecl (build_decl (TYPE_DECL, get_identifier ("signed char"),
			signed_char_type_node));

	unsigned_char_type_node = make_unsigned_type (CHAR_TYPE_SIZE);
	pushdecl (build_decl (TYPE_DECL, get_identifier ("unsigned char"),
			unsigned_char_type_node));

	float_type_node = make_node (REAL_TYPE);
	TYPE_PRECISION (float_type_node) = FLOAT_TYPE_SIZE;
	pushdecl (build_decl (TYPE_DECL, ridpointers[RID_FLOAT],
			float_type_node));
	layout_type (float_type_node);

	double_type_node = make_node (REAL_TYPE);
	TYPE_PRECISION (double_type_node) = DOUBLE_TYPE_SIZE;
	pushdecl (build_decl (TYPE_DECL, ridpointers[RID_DOUBLE],
			double_type_node));
	layout_type (double_type_node);

	long_double_type_node = make_node (REAL_TYPE);
	TYPE_PRECISION (long_double_type_node) = LONG_DOUBLE_TYPE_SIZE;
	pushdecl (build_decl (TYPE_DECL, get_identifier ("long double"),
			long_double_type_node));
	layout_type (long_double_type_node);

	integer_zero_node = build_int_2 (0, 0);
	TREE_TYPE (integer_zero_node) = integer_type_node;
	integer_one_node = build_int_2 (1, 0);
	TREE_TYPE (integer_one_node) = integer_type_node;

	size_zero_node = build_int_2 (0, 0);
	TREE_TYPE (size_zero_node) = sizetype;
	size_one_node = build_int_2 (1, 0);
	TREE_TYPE (size_one_node) = sizetype;

	void_type_node = make_node (VOID_TYPE);
	pushdecl (build_decl (TYPE_DECL,
			ridpointers[RID_VOID], void_type_node));
	layout_type (void_type_node);	/* Uses integer_zero_node */

	null_pointer_node = build_int_2 (0, 0);
	TREE_TYPE (null_pointer_node) = build_pointer_type (void_type_node);
	layout_type (TREE_TYPE (null_pointer_node));

	string_type_node = build_pointer_type (char_type_node);

	/*
	 *	make a type for arrays of 256 characters. 256 is picked
	 *	randomly because we have a type for integers from 0 to 255.
	 *	With luck nothing will ever really depend on the length
	 *	of this array type
	 */
	char_array_type_node
		= build_array_type (char_type_node, unsigned_char_type_node);

	/*
	 *	Likewise for arrays of ints
	 */
	int_array_type_node
		= build_array_type (integer_type_node, unsigned_char_type_node);

	default_function_type
		= build_function_type (integer_type_node, NULL_TREE);

	ptr_type_node = build_pointer_type (void_type_node);
	endlink = tree_cons (NULL_TREE, void_type_node, NULL_TREE);

	double_ftype_double = build_function_type
	(	double_type_node,
		tree_cons (NULL_TREE, double_type_node, endlink)
	);

	double_ftype_double_double = build_function_type
	(	double_type_node,
		tree_cons
		(	NULL_TREE,
			double_type_node,
			tree_cons (NULL_TREE, double_type_node, endlink)
		)
	);

	int_ftype_int = build_function_type
	(	integer_type_node,
		tree_cons (NULL_TREE, integer_type_node, endlink)
	);

	long_ftype_long = build_function_type
	(	long_integer_type_node,
		tree_cons (NULL_TREE, long_integer_type_node, endlink)
	);

	void_ftype_ptr_ptr_int = build_function_type
	(	void_type_node,
		tree_cons
		(	NULL_TREE,
			ptr_type_node,
			tree_cons
			(	NULL_TREE,
				ptr_type_node,
				tree_cons
				(	NULL_TREE,
					integer_type_node,
					endlink
				)
			)
		)
	);

	int_ftype_ptr_ptr_int = build_function_type
	(	integer_type_node,
		 tree_cons
		(	NULL_TREE,
			ptr_type_node,
			tree_cons
			(	NULL_TREE,
				ptr_type_node,
				tree_cons
				(	NULL_TREE,
					integer_type_node,
					endlink
				)
			)
		)
	);

	void_ftype_ptr_int_int = build_function_type
	(	void_type_node,
		tree_cons
		(	NULL_TREE,
			ptr_type_node,
			tree_cons
			(	NULL_TREE,
				integer_type_node,
				tree_cons
				(	NULL_TREE,
					integer_type_node,
					endlink
				)
			)
		)
	);

	builtin_function
	(	"__builtin_alloca",
		build_function_type
		(	ptr_type_node,
			tree_cons
			(	NULL_TREE,
				integer_type_node,
				endlink
			)
		),
		BUILT_IN_ALLOCA
	);

	builtin_function ("__builtin_abs", int_ftype_int, BUILT_IN_ABS);
	builtin_function ("__builtin_fabs", double_ftype_double, BUILT_IN_FABS);
	builtin_function ("__builtin_labs", long_ftype_long, BUILT_IN_LABS);
	builtin_function ("__builtin_ffs", int_ftype_int, BUILT_IN_FFS);

	builtin_function
	(	"__builtin_saveregs",
		default_function_type,
		BUILT_IN_SAVEREGS
	);

	builtin_function
	(	"__builtin_classify_type",
		default_function_type,
		BUILT_IN_CLASSIFY_TYPE
	);

	builtin_function
	(	"__builtin_next_arg",
		build_function_type (ptr_type_node, endlink),
		BUILT_IN_NEXT_ARG
	);

#if 0	/* --------------------------------------------------- */
	/*
	 *	Support for these has not been written in
	 *	either expand_builtin or build_function_call
	 */
	builtin_function ("__builtin_div", default_ftype, BUILT_IN_DIV);
	builtin_function ("__builtin_ldiv", default_ftype, BUILT_IN_LDIV);
	builtin_function ("__builtin_ffloor", double_ftype_double, BUILT_IN_FFLOOR);
	builtin_function ("__builtin_fceil", double_ftype_double, BUILT_IN_FCEIL);
	builtin_function ("__builtin_fmod", double_ftype_double_double, BUILT_IN_FMOD);
	builtin_function ("__builtin_frem", double_ftype_double_double, BUILT_IN_FREM);
	builtin_function ("__builtin_memcpy", void_ftype_ptr_ptr_int, BUILT_IN_MEMCPY);
	builtin_function ("__builtin_memcmp", int_ftype_ptr_ptr_int, BUILT_IN_MEMCMP);
	builtin_function ("__builtin_memset", void_ftype_ptr_int_int, BUILT_IN_MEMSET);
	builtin_function ("__builtin_fsqrt", double_ftype_double, BUILT_IN_FSQRT);
	builtin_function ("__builtin_getexp", double_ftype_double, BUILT_IN_GETEXP);
	builtin_function ("__builtin_getman", double_ftype_double, BUILT_IN_GETMAN);
#endif	/* --------------------------------------------------- */

	void_list_node = tree_cons (NULL_TREE, void_type_node, NULL_TREE);
	start_identifier_warnings ();

}	/* end init_decl_processing */

/*
 ****************************************************************
 *	Make a definition for a builtin function		*
 ****************************************************************
 */
/*
 *	Make a definition for a builtin function named
 *	NAME and whose data type is TYPE. TYPE should be
 *	a function type with argument types. FUNCTION_CODE
 *	tells later passes how to compile calls to this
 *	function. See tree.h for its possible values
 */
void
builtin_function (char *name, tree type, enum built_in_function function_code)
{
	tree	decl = build_decl (FUNCTION_DECL, get_identifier (name), type);

	TREE_EXTERNAL (decl) = 1;
	TREE_PUBLIC (decl) = 1;
	make_decl_rtl (decl, 0, 1);
	pushdecl (decl);
	DECL_SET_FUNCTION_CODE (decl, function_code);

}	/* end builtin_function */

/*
 ****************************************************************
 *	Declaration that contains no names to declare		*
 ****************************************************************
 */
/*
 *	Called when a declaration is seen that contains no
 *	names to declare. If its type is a reference to a
 *	structure, union or enum inherited from a containing
 *	scope, shadow that tag name for the current scope
 *	with a forward reference. If its type defines a new
 *	named structure or union or defines an enum, it
 *	is valid but we need not do anything here.
 *	Otherwise, it is an error
 */
void
shadow_tag (tree declspecs)
{
	int		found_tag = 0;
	int		warned = 0;
	register tree	link;

	for (link = declspecs; link; link = TREE_CHAIN (link))
	{
		register tree		value = TREE_VALUE (link);
		register enum tree_code	code = TREE_CODE (value);
		int			ok = 0;

		/*
		 *	Used to test also that TYPE_SIZE (value) != 0.
		 *	That caused warning for `struct foo;' at top
		 *	level in the file
		 */
		if (code == RECORD_TYPE || code == UNION_TYPE || code == ENUMERAL_TYPE)
		{
			register tree	name = lookup_tag_reverse (value);
			register tree	t;

			t = lookup_tag (code, name, current_binding_level, 1);

			if   (t == NULL_TREE)
			{
				t = make_node (code);
				pushtag (name, t);
				ok = 1;
			}
			elif (name != NULL_TREE || code == ENUMERAL_TYPE)
			{
				ok = 1;
			}
		}

		if (ok)
		{
			found_tag++;
		}
		else
		{
			if (!warned)
				warning ("useless keyword or type name in declaration");
			warned = 1;
		}

	}	/* end for (link) */

	if (!warned)
	{
		if (found_tag > 1)
			warning ("multiple types in one declaration");

		if (found_tag == 0)
			warning ("empty declaration");
	}

	}	/* end shadow_tag */

/*
 ****************************************************************
 *	Decode a "typename", such as "int **"			*
 ****************************************************************
 */
/*
 *	Decode a "typename", such as "int **",
 *	returning a ..._TYPE node
 */
tree
groktypename (tree typename)
{
	if (TREE_CODE (typename) != TREE_LIST)
		return (typename);

	return
	(	grokdeclarator
		(	TREE_VALUE (typename),
			TREE_PURPOSE (typename),
			TYPENAME, 0
		)
	);

}	/* end groktypename */

/*
 ****************************************************************
 *  Decode a declarator in an declaration or data definition	*
 ****************************************************************
 */
/*
 *	Decode a declarator in an ordinary declaration or
 *	data definition. This is called as soon as the type
 *	information and variable name have been parsed,
 *	before parsing the initializer if any. Here we create
 *	the ..._DECL node, fill in its type, and put it on
 *	the list of decls for the current context. The ..._DECL
 *	node is returned as the value.
 *
 *	Exception: for arrays where the length is not specified,
 *	the type is left null, to be filled in by `finish_decl'.
 *
 *	Function definitions do not come here; they go to
 *	start_function instead. However, external and forward
 *	declarations of functions do go through here.
 *	Structure field declarations are done by grokfield
 *	and not through here
 */

/*
 *	Set this to zero to debug not using the temporary
 *	obstack to parse initializers
 */
entry int	debug_temp_inits = 1;

tree
start_decl (tree declarator, tree declspecs, int initialized)
{
	register tree	decl;
	register tree	tem;
	int		init_written;

	decl = grokdeclarator (declarator, declspecs, NORMAL, initialized);
	init_written = initialized;

	/*
	 *	Is it valid for this decl to have an initializer
	 *	at all? If not, set INITIALIZED to zero, which
	 *	will indirectly tell `finish_decl' to ignore the
	 	 *	initializer once it is parsed
	 */
	if (initialized) switch (TREE_CODE (decl))
	{
		/*
		 *	typedef foo = bar	means give foo the
		 *	same type as bar. We haven't parsed bar yet,
		 *	so `finish_decl' will fix that up. Any other
		 *	case of an initialization in a TYPE_DECL
		 *	is an error
		 */
	    case TYPE_DECL:
		if (pedantic || list_length (declspecs) > 1)
		{
			error
			(	"typedef `%s' is initialized",
				IDENTIFIER_POINTER (DECL_NAME (decl))
			);

			initialized = 0;
		}

		break;

	    case FUNCTION_DECL:
		error
		(	"function `%s' is initialized like a variable",
			IDENTIFIER_POINTER (DECL_NAME (decl))
		);

		initialized = 0;
		break;

		/*
		 * 	Don't allow initializations for incomplete
		 *	types except for arrays which might be
		 *	completed by the initialization
		 */
	    default:
		if (TYPE_SIZE (TREE_TYPE (decl)) != NULL_TREE)
		{
			/* vazio */;	/* A complete type is ok */
		}
		elif (TREE_CODE (TREE_TYPE (decl)) != ARRAY_TYPE)
		{
			error
			(	"variable `%s' has initializer but incomplete type",
				IDENTIFIER_POINTER (DECL_NAME (decl))
			);
			initialized = 0;
		}
		elif (TYPE_SIZE (TREE_TYPE (TREE_TYPE (decl))) == NULL_TREE)
		{
			error
			(	"elements of array `%s' have incomplete type",
				IDENTIFIER_POINTER (DECL_NAME (decl))
			);
			initialized = 0;
		}

	}	/* end if (initialized) switch */

	if (initialized)
	{
#if 0	/* --------------------------------------------------- */
		/*
		 *	Seems redundant
		 */
		if
		(	current_binding_level != global_binding_level &&
			TREE_EXTERNAL (decl) &&
			TREE_CODE (decl) != FUNCTION_DECL
		)
			warning
			(	"declaration of `%s' has `extern' and is initialized",
				IDENTIFIER_POINTER (DECL_NAME (decl))
			);
#endif	/* --------------------------------------------------- */

		TREE_EXTERNAL (decl) = 0;

		if (current_binding_level == global_binding_level)
			TREE_STATIC (decl) = 1;

		/*
		 *	Tell `pushdecl' this is an initialized decl
		 *	even though we don't yet have the initializer
		 *	expression. Also tell `finish_decl' it may
		 *	store the real initializer
		 */
		DECL_INITIAL (decl) = error_mark_node;
	}

	/*
	 *	Add this decl to the current binding level. TEM
	 *	may equal DECL or it may be a previous decl of
	 *	the same name
	 */
	tem = pushdecl (decl);

	/*
	 *	For a local variable, define the RTL now
	 *
	 *	But not if this is a duplicate decl
	 	 *	and we preserved the rtl from the
	 *	previous one (which may or may not happen)
	 */
	if
	(	current_binding_level != global_binding_level &&
		DECL_RTL (tem) == (struct rtx_def *)NULL
	)
	{
		if (TYPE_SIZE (TREE_TYPE (tem)) != NULL_TREE)
			expand_decl (tem, NULL_TREE);
		elif
		(	TREE_CODE (TREE_TYPE (tem)) == ARRAY_TYPE &&
			DECL_INITIAL (tem) != NULL_TREE
		)
			expand_decl (tem, NULL_TREE);
	}

	/*
	 *	When parsing and digesting the initializer,
 	 *	use temporary storage. Do this even if we
	 *	will ignore the value
	 */
	if (init_written)
	{
		if
		(	current_binding_level == global_binding_level &&
			debug_temp_inits
		)
			temporary_allocation ();
	}

	return (tem);

}	/* end start_decl */

/*
 ****************************************************************
 *	Finish processing of a declaration			*
 ****************************************************************
 */
/*
 *	Finish processing of a declaration; install its
 *	line number and initial value. If the length of
 *	an array type is not known before, it must be
 *	determined now, from the initial value, or it
 *	is an error
 */
void
finish_decl (tree decl, tree init, tree asmspec_tree)
{
	register tree	type = TREE_TYPE (decl);
	int		was_incomplete = (DECL_SIZE (decl) == NULL_TREE);
	int 		temporary = allocation_temporary_p ();
	char		*asmspec = NOSTR;

	if (asmspec_tree)
		asmspec = TREE_STRING_POINTER (asmspec_tree);

	/*
	 *	If `start_decl' didn't like having an
	 *	initialization, ignore it now
	 */
	if (init != NULL_TREE && DECL_INITIAL (decl) == NULL_TREE)
		init = NULL_TREE;

	if (init)
	{
		if (TREE_CODE (decl) != TYPE_DECL)
		{
			store_init_value (decl, init);
		}
		else
		{
			/*
			 *	typedef foo = bar; store the
			 *	type of bar as the type of foo
			 */
			TREE_TYPE (decl) = TREE_TYPE (init);
			DECL_INITIAL (decl) = init = NULL_TREE;
		}
	}

	/*
	 *	For top-level declaration, the initial value was read in
	 *	the temporary obstack.	MAXINDEX, rtl, etc. to be made
	 *	below must go in the permanent obstack; but don't
	 *	discard the temporary data yet
	 */
	if (current_binding_level == global_binding_level && temporary)
		end_temporary_allocation ();

	/*
	 * 	Deduce size of array from initialization,
	 *	if not already known
	 */
	if
	(	TREE_CODE (type) == ARRAY_TYPE &&
		TYPE_DOMAIN (type) == NULL_TREE &&
		TREE_CODE (decl) != TYPE_DECL
	)
	{
#if 0	/* --------------------------------------------------- */
		int	do_default;

		do_default = ! ((!pedantic && TREE_STATIC (decl)) || TREE_EXTERNAL (decl));
#endif	/* --------------------------------------------------- */

		/*
		 *	Even if pedantic, an external linkage array
		 *	may have incomplete type at first
		 */
		int	do_default = (TREE_STATIC (decl) ?
			pedantic && !TREE_PUBLIC (decl) :
			!TREE_EXTERNAL (decl));

		int	failure = complete_array_type
				(type, DECL_INITIAL (decl), do_default);

		if (failure == 1)
		{
			error_with_decl
			(	decl,
				"initializer fails to determine size of `%s'"
			);
		}

		if (failure == 2)
		{
			if   (do_default)
			{
				if (! TREE_PUBLIC (decl))
				{
					error_with_decl
					(	decl,
						"array size missing in `%s'"
					);
				}
			}
			elif (!pedantic && TREE_STATIC (decl))
			{
				TREE_EXTERNAL (decl) = 1;
			}
		}

		if
		(	pedantic && TYPE_DOMAIN (type) != NULL_TREE &&
			tree_int_cst_lt
				(TYPE_MAX_VALUE (TYPE_DOMAIN (type)), integer_zero_node)
		)
			error_with_decl (decl, "zero-size array `%s'");

		layout_decl (decl, 0);
	}

	if (TREE_CODE (decl) == VAR_DECL)
	{
		if (TREE_STATIC (decl) && DECL_SIZE (decl) == NULL_TREE)
		{
			/*
			 *	A static variable with an incomplete type:
			 *	that is an error if it is initialized or
			 *	`static'. Otherwise, let it through, but
			 *	if it is not `extern' then it may cause an
			 *	error message later
			 */
			if
			(	! (TREE_PUBLIC (decl) &&
				DECL_INITIAL (decl) == NULL_TREE)
			)
			{
				error_with_decl
				(	decl,
					"storage size of `%s' isn't known"
				);
			}
		}
		elif (!TREE_EXTERNAL (decl) && DECL_SIZE (decl) == NULL_TREE)
		{
			/*
			 *	An automatic variable with an
			 *	incomplete type: that is an error
			 */
			error_with_decl
			(	decl,
				"storage size of `%s' isn't known"
			);

			TREE_TYPE (decl) = error_mark_node;
		}

		if
		(	(TREE_EXTERNAL (decl) || TREE_STATIC (decl)) &&
			DECL_SIZE (decl) != NULL_TREE &&
			! TREE_LITERAL (DECL_SIZE (decl))
		)
		{
			error_with_decl
			(	decl,
				"storage size of `%s' isn't constant"
			);
		}
	}

	/*
	 *	Output the assembler code and/or RTL code for
	 *	variables and functions, unless the type is an
	 *	undefined structure or union. If not, it will
	 *	get done when the type is completed
	 */
	if (TREE_CODE (decl) == VAR_DECL || TREE_CODE (decl) == FUNCTION_DECL)
	{
		if (flag_traditional && allocation_temporary_p ())
		{
			end_temporary_allocation ();

			rest_of_decl_compilation
			(	decl,
				asmspec,
				current_binding_level == global_binding_level,
				0
			);

			resume_temporary_allocation ();

		}
		else
		{
			rest_of_decl_compilation
			(	decl,
				asmspec,
				current_binding_level == global_binding_level,
				0
			);
		}

		/*
		 *	Recompute the RTL of a local array now
		 *	if it used to be an incomplete type
		 */
		if (current_binding_level != global_binding_level)
		{
			if
			(	was_incomplete &&
				! TREE_STATIC (decl) && ! TREE_EXTERNAL (decl)
			)
			{
				/*
				 *	If we used it already as memory,
				 *	it must stay in memory
				 */
				TREE_ADDRESSABLE (decl) = TREE_USED (decl);

				/*
				 *	If it's still incomplete now,
				 *	no init will save it
				 */
				if (DECL_SIZE (decl) == NULL_TREE)
					DECL_INITIAL (decl) = NULL_TREE;

				expand_decl (decl, NULL_TREE);
			}

			/*
			 *	Compute and store the initial value
			 */
			expand_decl_init (decl);
		}
	}

	if (TREE_CODE (decl) == TYPE_DECL)
	{
		rest_of_decl_compilation
		(	decl,
			0,
			current_binding_level == global_binding_level,
			0
		);
	}

	/*
	 *	Resume permanent allocation, if not within a function
	 */
	if (temporary && current_binding_level == global_binding_level)
	{
		permanent_allocation ();

		/*
		 *	We need to remember that this array HAD
		 *	an initialization, but discard the actual
		 *	nodes, since they are temporary anyway
		 */
		if (DECL_INITIAL (decl) != NULL_TREE)
			DECL_INITIAL (decl) = error_mark_node;
	}

	/*
	 *	At the end of a declaration, throw away any
	 *	variable type sizes of types defined inside
	 *	that declaration. There is no use computing
	 *	them in the following function definition
	 */
	if (current_binding_level == global_binding_level)
		get_pending_sizes ();

}	/* end finish_decl */

/*
 ****************************************************************
 *	Push a parsed parameter declaration			*
 ****************************************************************
 */
/*
 *	Given a parsed parameter declaration, decode
 *	it into a PARM_DECL and push that on the
 *	current binding level
 */
void
push_parm_decl (tree parm)
{
	register tree	decl;

	decl = grokdeclarator (TREE_VALUE (parm), TREE_PURPOSE (parm), PARM, 0);

	/*
	 *	Add this decl to the current binding level
	 */
	finish_decl (pushdecl (decl), NULL_TREE, NULL_TREE);

}	/* end push_parm_decl */
