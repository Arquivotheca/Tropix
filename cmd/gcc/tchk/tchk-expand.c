/*
 ****************************************************************
 *								*
 *			tchk/tchk-expand.c			*
 *								*
 *	Build expressions with type checking for C compiler.	*
 *								*
 *	This file is part of the C front end.			*
 *	It contains routines to build C expressions given	*
 *	their operands, including computing the types of	*
 *	the result, C-specific error checks, and some		*
 *	optimization.						*
 *								*
 *	There are also routines to build RETURN_STMT nodes	*
 *	and CASE_STMT nodes, and to process initializations	*
 *	in declarations (since they work like a strange sort	*
 *	of assignment).						*
 *								*
 *	Versão	3.0.0, de 17.12.93				*
 *		3.0.0, de 17.12.93				*
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
#include <stdlib.h>

#include ../$GEN.gen/config.h

#include "../h/tree.h"
#include "../h/c-tree.h"
#include "../h/flags.h"

#include "../h/rtl.h"

#define	elif	else if
#define	EVER	;;

/*
 ******	Protótipos de funções ***********************************
 */
tree 		process_init_constructor (tree, tree, tree *);



extern tree	build_conditional_expr (tree, tree, tree);
#if (0)	/*************************************/
extern rtx	expand_expr (tree, rtx, enum machine_mode, enum expand_modifier);
#endif	/*************************************/
extern void	emit_queue (void);
extern void	expand_start_case (int, tree, tree);
extern void	expand_asm_operands (tree, tree, tree, tree, int, char *, int);
extern tree	nreverse (tree);
extern int	comptypes (tree, tree);
extern tree	c_expand_start_case (tree);
extern void	expand_null_return (void);
extern void	put_var_into_stack (tree decl);
extern void	expand_return (tree);

extern tree	build (enum tree_code, ...);
extern tree	build_nt (enum tree_code, ...);
extern tree	convert (tree type, tree expr);
extern tree	convert_units (tree size, int inunits, int outunits);
extern tree	fold (tree expr);
extern tree	type_for_size (int bits, int unsignedp);
extern tree 	copy_node (tree node);
extern tree 	make_node (enum tree_code code);
extern void	incomplete_type_error (tree value, tree type);
extern void	warning (char *fmt, ...);
extern void 	error (const char *fmt, ...);
extern void 	layout_decl (tree decl, unsigned known_align);
extern void 	layout_type (tree type);


#if (0)	/*************************************/
extern int	abort (void);
#endif	/*************************************/
extern tree	actualparameterlist (tree, tree, tree);
extern void	binary_op_error (enum tree_code);
extern tree	build (enum tree_code, ...);
extern tree	build_array_type (tree, tree);
extern tree	build_binary_op (enum tree_code, tree, tree);
extern tree	build_binary_op_nodefault (enum tree_code, tree, tree, enum tree_code);
extern tree	build_function_type (tree, tree);
extern tree	build_int (int);
extern tree	build_int_2 (int, int);
extern tree	build_modify_expr (tree, enum tree_code, tree);
extern tree	build_pointer_type (tree);
extern tree	build_tree_list (tree, tree);
extern tree	build_unary_op (enum tree_code, tree, int);
extern tree	c_build_type_variant (tree, int, int);
extern tree	c_sizeof_nowarn (tree);
extern tree	chainon (tree, tree);
extern tree	commontype (tree, tree);
extern tree	comp_target_types (tree, tree);
extern int	compparms (tree, tree);
extern int	compparms1 (tree);
extern tree	convert (tree, tree);
extern tree	convert_for_assignment (tree, tree, char *);
extern tree	convert_units (tree size, int inunits, int outunits);
extern tree	copy_node (tree);
extern tree	datatype (tree);
extern tree	decl_constant_value (tree);
extern tree	default_conversion (tree);
extern tree	digest_init (tree, tree, tree *);
extern void	error (const char *, ...);
extern tree	fold (tree);
extern tree	get_narrower (tree, int *);
extern tree	get_unwidened (tree, tree);
extern void	incomplete_type_error (tree, tree);
extern int	initializer_constant_valid_p (tree);
extern int	int_fits_type_p (tree, tree);
extern int	integer_zerop (tree);
extern tree	invert_truthvalue (tree);
extern int	list_length (tree);
extern int	lvalue_or_else (tree, char *);
extern int	lvalue_p (tree);
extern int	mark_addressable (tree);
extern tree	pointer_diff (tree, tree);
extern tree	pointer_int_sum (enum tree_code, tree, tree);
extern tree	qualify_type (tree, tree);
extern void	readonly_warning (tree, char *);
extern tree	require_complete_type (tree);
extern tree	save_expr (tree);
extern tree	shorten_compare (tree *, tree *, tree *, enum tree_code *);
extern tree	signed_or_unsigned_type (int, tree);
extern tree	signed_type (tree);
extern tree	size_in_bytes (tree);
extern tree	stabilize_reference (tree);
extern int	staticp (tree);
extern tree	tree_cons (tree, tree, tree);
extern tree	truthvalue_conversion (tree);
extern tree	type_for_size (int, int);
extern tree	unary_complex_lvalue (enum tree_code, tree);
extern tree	unsigned_type (tree);
extern void 	warning	(char *, ...);


/*
 ****************************************************************
 *	Apply unary lvalue-demanding operator 			*
 ****************************************************************
 */
/*
 *	Apply unary lvalue-demanding operator CODE to the
 *	expression ARG for certain kinds of expressions which
 *	are not really lvalues but which we can accept as lvalues.
 *
 *	If ARG is not a kind of expression we can handle, return zero.
 */
tree
unary_complex_lvalue (enum tree_code code, tree arg)
{
	if (pedantic)
		return NULL_TREE;

	/*
	 *	Handle (a, b) used as an "lvalue"
	 */
	if (TREE_CODE (arg) == COMPOUND_EXPR)
	{
		tree real_result =
				build_unary_op (code, TREE_OPERAND (arg, 1), 0);

		return
		(	build
			(	COMPOUND_EXPR,
				TREE_TYPE (real_result),
				TREE_OPERAND (arg, 0),
				real_result
			)
		);
	}

	/*
	 *	Handle (a ? b : c) used as an "lvalue"
	 */
	if (TREE_CODE (arg) == COND_EXPR)
	{
		return
		(	build_conditional_expr
			(	TREE_OPERAND (arg, 0),
				build_unary_op (code, TREE_OPERAND (arg, 1), 0),
				build_unary_op (code, TREE_OPERAND (arg, 2), 0)
			)
		);
	}

	return (NULL_TREE);

}	/* end unary_complex_lvalue */

/*
 ****************************************************************
 *	Return a simplified tree node for the truth-negation	*
 ****************************************************************
 */
/*
 *	Return a simplified tree node for the truth-negation
 *	of ARG (perhaps by altering ARG). If it can't be
 *	simplified, return NULL_TREE
 */
tree
invert_truthvalue (tree arg)
{
	switch (TREE_CODE (arg))
	{
	    case NE_EXPR:
		TREE_SET_CODE (arg, EQ_EXPR);
		return (arg);

	    case EQ_EXPR:
		TREE_SET_CODE (arg, NE_EXPR);
		return (arg);

	    case GE_EXPR:
		TREE_SET_CODE (arg, LT_EXPR);
		return (arg);

	    case GT_EXPR:
		TREE_SET_CODE (arg, LE_EXPR);
		return (arg);

	    case LE_EXPR:
		TREE_SET_CODE (arg, GT_EXPR);
		return (arg);

	    case LT_EXPR:
		TREE_SET_CODE (arg, GE_EXPR);
		return (arg);

	    case TRUTH_AND_EXPR:
		return
		(	build
			(	TRUTH_OR_EXPR,
				TREE_TYPE (arg),
				build_unary_op
				(	TRUTH_NOT_EXPR,
					TREE_OPERAND (arg, 0), 0
				),
				build_unary_op
				(	TRUTH_NOT_EXPR,
					TREE_OPERAND (arg, 1), 0
				)
			)
		);

	    case TRUTH_OR_EXPR:
		return
		(	build
			(	TRUTH_AND_EXPR, TREE_TYPE (arg),
				build_unary_op
				(	TRUTH_NOT_EXPR,
					TREE_OPERAND (arg, 0), 0
				),
				build_unary_op
				(	TRUTH_NOT_EXPR,
					TREE_OPERAND (arg, 1), 0
				)
			)
		);

	    case TRUTH_ANDIF_EXPR:
		return
		(	build
			(	TRUTH_ORIF_EXPR, TREE_TYPE (arg),
				build_unary_op
				(	TRUTH_NOT_EXPR,
					TREE_OPERAND (arg, 0), 0
				),
				build_unary_op
				(	TRUTH_NOT_EXPR,
					TREE_OPERAND (arg, 1), 0
				)
			)
		);

	    case TRUTH_ORIF_EXPR:
		return
		(	build
			(	TRUTH_ANDIF_EXPR, TREE_TYPE (arg),
				build_unary_op
				(	TRUTH_NOT_EXPR,
					TREE_OPERAND (arg, 0), 0
				),
				build_unary_op
				(	TRUTH_NOT_EXPR,
					TREE_OPERAND (arg, 1), 0
				)
			)
		);

	    case TRUTH_NOT_EXPR:
		return (TREE_OPERAND (arg, 0));

	    case COND_EXPR:
		return
		(	build
			(	COND_EXPR,
				TREE_TYPE (arg),
				TREE_OPERAND (arg, 0),
				build_unary_op
				(	TRUTH_NOT_EXPR,
					TREE_OPERAND (arg, 1),
					0
				),
				build_unary_op
				(	TRUTH_NOT_EXPR,
					TREE_OPERAND (arg, 2),
					0
				)
			)
		);

	}	/* end switch */

	return (NULL_TREE);

}	/* end invert_truthvalue */

/*
 ****************************************************************
 *	The "exp" should not be allocated in a register		*
 ****************************************************************
 */
/*
 *	Mark EXP saying that we need to be able to take the
 *	address of it; it should not be allocated in a
 *	register. Value is 1 if successful.
 */
int
mark_addressable (tree exp)
{
	register tree	x = exp;

	for (EVER) switch (TREE_CODE (x))
	{
	    case ADDR_EXPR:
            case COMPONENT_REF:
	    case ARRAY_REF:
		x = TREE_OPERAND (x, 0);
		break;

	    case VAR_DECL:
	    case CONST_DECL:
	    case PARM_DECL:
	    case RESULT_DECL:
		if (TREE_REGDECL (x) && !TREE_ADDRESSABLE (x))
		{
			if (TREE_PUBLIC (x))
			{
				error
				(	"address of global register "
					"variable `%s' requested",
					IDENTIFIER_POINTER (DECL_NAME (x))
				);
				return (0);
			}

			warning
			(	"address of register variable `%s' requested",
				IDENTIFIER_POINTER (DECL_NAME (x))
			);
		}

		put_var_into_stack (x);

		/* drops in */
	    case FUNCTION_DECL:
		TREE_ADDRESSABLE (x) = 1;
		TREE_ADDRESSABLE (DECL_NAME (x)) = 1;
	
	    default:
		return (1);

	}	/* end for (EVER) switch */

}	/* end mark_addressable */

/*
 ****************************************************************
 *	Return 0 if EXP is not a valid lvalue in this language	*
 ****************************************************************
 */
/*
 *	Return 0 if EXP is not a valid lvalue in this language
 *	even though `lvalue_or_else' would accept it.
 */
int
language_lvalue_valid (tree exp)
{
	return (1);

}	/* end language_lvalue_valid */

/*
 ****************************************************************
 *	Return nonzero if VALUE is a valid constant-valued exp.	*
 ****************************************************************
 */
/*
 *	Return nonzero if VALUE is a valid constant-valued
 *	expression for use in initializing a static variable;
 *	one that can be an element of a "constant" initializer.
 *
 *	Return 1 if the value is absolute; return 2 if it is
 *	relocatable. We assume that VALUE has been folded as
 *	much as possible; therefore, we do not need to check
 *	for such things as arithmetic-combinations of integers
 */
int
initializer_constant_valid_p (tree value)
{
	switch (TREE_CODE (value))
	{
	    case CONSTRUCTOR:
		return (TREE_STATIC (value));

	    case INTEGER_CST:
	    case REAL_CST:
	    case STRING_CST:
		return (1);

	    case ADDR_EXPR:
		return (2);

		/*
		 *	Allow conversions between types of the same kind
		 */
	    case CONVERT_EXPR:
	    case NOP_EXPR:
		if
		(	TREE_CODE (TREE_TYPE (value)) ==
			TREE_CODE (TREE_TYPE (TREE_OPERAND (value, 0)))
		)
		{
			return
			(	initializer_constant_valid_p
				(	TREE_OPERAND (value, 0)
				)
			);
		}

		/*
		 *	Allow (int) &foo
		 */
		if
		(	TREE_CODE (TREE_TYPE (value)) == INTEGER_TYPE &&
			TREE_CODE (TREE_TYPE (TREE_OPERAND (value, 0))) ==
						POINTER_TYPE
		)
		{
			return
			(	initializer_constant_valid_p
				(	TREE_OPERAND (value, 0)
				)
			);
		}

		return (0);

	    case PLUS_EXPR:
	    {
		int valid0 = initializer_constant_valid_p (TREE_OPERAND (value, 0));
		int valid1 = initializer_constant_valid_p (TREE_OPERAND (value, 1));

		if (valid0 == 1 && valid1 == 2)
			return (2);

		if (valid0 == 2 && valid1 == 1)
			return (2);

		return (0);
	    }

	    case MINUS_EXPR:
	    {
		int valid0 = initializer_constant_valid_p (TREE_OPERAND (value, 0));
		int valid1 = initializer_constant_valid_p (TREE_OPERAND (value, 1));

		if (valid0 == 2 && valid1 == 1)
			return (2);

		return (0);
	    }
	}

	return (0);

}	/* end initializer_constant_valid_p */


/*
 ****************************************************************
 *	Digest the parser output INIT as an initializer		*
 ****************************************************************
 */
/*
 *	Digest the parser output INIT as an initializer
 *	for type TYPE. Return a C expression of type TYPE
 *	to represent the initial value.
 *
 *	If TAIL is nonzero, it points to a variable holding
 *	a list of elements of which INIT is the first. 
 *	We update the list stored there by removing from
 *	the head all the elements that we use. Normally this
 *	is only one; we use more than one element only if
 *	TYPE is an aggregate and INIT is not a constructor
 */
tree
digest_init (tree type, tree init, tree *tail)
{
	enum tree_code	code = TREE_CODE (type);
	tree		element = NULL_TREE;
	tree		old_tail_contents;

	/*
	 *	Nonzero if INIT is a braced grouping, which comes
	 *	in as a CONSTRUCTOR tree node which has no TREE_TYPE
	 */
	int	raw_constructor = (TREE_CODE (init) == CONSTRUCTOR &&
					TREE_TYPE (init) == NULL_TREE);

	/*
	 *	By default, assume we use one element from a list.
	 *	We correct this later in the sole case where it is not true
	 */
	if (tail)
	{
		old_tail_contents = *tail;
		*tail = TREE_CHAIN (*tail);
	}

	if (init == error_mark_node)
		return (init);

	if
	(	init && raw_constructor &&
		CONSTRUCTOR_ELTS (init) != NULL_TREE &&
		TREE_CHAIN (CONSTRUCTOR_ELTS (init)) == NULL_TREE
	)
		element = TREE_VALUE (CONSTRUCTOR_ELTS (init));

	/*
	 *	Initialization of an array of chars from a
	 *	string constant optionally enclosed in braces
	 */
	if (code == ARRAY_TYPE)
	{
		tree	typ1 = TYPE_MAIN_VARIANT (TREE_TYPE (type));

		if
		(	(typ1 == char_type_node ||
			typ1 == signed_char_type_node ||
			typ1 == unsigned_char_type_node ||
			typ1 == unsigned_type_node ||
			typ1 == integer_type_node) &&
			((init && TREE_CODE (init) == STRING_CST) ||
			(element && TREE_CODE (element) == STRING_CST))
		)
		{
			tree	string = element ? element : init;

			if
			(	(TYPE_MAIN_VARIANT (TREE_TYPE (TREE_TYPE (string)))
						!= char_type_node) &&
				TYPE_PRECISION (typ1) ==
						TYPE_PRECISION (char_type_node)
			)
			{
				error ("char-array initialized from wide string");
				return (error_mark_node);
			}

			if
			(	(TYPE_MAIN_VARIANT (TREE_TYPE (TREE_TYPE (string)))
						== char_type_node) &&
				TYPE_PRECISION (typ1) ==
						TYPE_PRECISION (integer_type_node)
			)
			{
				error ("int-array initialized from non-wide string");
				return (error_mark_node);
			}

			TREE_TYPE (string) = type;

			if
			(	TYPE_DOMAIN (type) != NULL_TREE &&
				TREE_LITERAL (TYPE_SIZE (type))
			)
			{
				register int	size =
					TREE_INT_CST_LOW (TYPE_SIZE (type)) * TYPE_SIZE_UNIT (type);

				size = (size + BITS_PER_UNIT - 1) / BITS_PER_UNIT;

				/*
				 *	Subtract 1 because it's ok to
				 *	ignore the terminating null char
				 *	that is counted in the length of
				 *	the constant
				 */
				if (size < TREE_STRING_LENGTH (string) - 1)
				{
					warning
					(	"initializer-string for "
						"array of chars is too long"
					);
				}
			}

			return (string);
		}
	}

	/*
	 *	Any type can be initialized from an expression of
	 *	the same type, optionally with braces
	 */
	if
	(	init && (TREE_TYPE (init) == type ||
		(code == ARRAY_TYPE && TREE_TYPE (init) &&
		comptypes (TREE_TYPE (init), type)))
	)
	{
		if (code == ARRAY_TYPE && TREE_CODE (init) != STRING_CST)
		{
			error ("array initialized from non-constant array expression");
			return (error_mark_node);
		}

		if
		(	optimize &&
			TREE_READONLY (init) && TREE_CODE (init) == VAR_DECL
		)
			return (decl_constant_value (init));

		return (init);
	}

	if
	(	element && (TREE_TYPE (element) == type ||
		(code == ARRAY_TYPE && TREE_TYPE (element) &&
		comptypes (TREE_TYPE (element), type)))
	)
	{
		if (code == ARRAY_TYPE)
		{
			error ("array initialized from non-constant array expression");
			return (error_mark_node);
		}

		if (pedantic && (code == RECORD_TYPE || code == UNION_TYPE))
			warning ("single-expression nonscalar initializer has braces");

		if (optimize && TREE_READONLY (element) && TREE_CODE (element) == VAR_DECL)
			return (decl_constant_value (element));

		return (element);
	}

	/*
	 *	Check for initializing a union by its first field.
	 *	Such an initializer must use braces
	 */
	if (code == UNION_TYPE)
	{
		if (TYPE_FIELDS (type) == NULL_TREE)
		{
			error ("union with no members cannot be initialized");
			return (error_mark_node);
		}

		if   (raw_constructor)
			return (process_init_constructor (type, init, (tree *)NULL));

		if (tail)
		{
			*tail = old_tail_contents;
			return (process_init_constructor (type, NULL_TREE, tail));
		}
	}

	/*
	 *	Handle scalar types, including conversions
	 */
	if
	(	code == INTEGER_TYPE || code == REAL_TYPE ||
		code == POINTER_TYPE || code == ENUMERAL_TYPE
	)
	{
		if (raw_constructor)
		{
			if (element == NULL_TREE)
			{
				error ("initializer for scalar variable requires one element");
				return (error_mark_node);
			}

			init = element;
		}

		if (TREE_CODE (init) == CONSTRUCTOR)
		{
			error ("initializer for scalar has extra braces");
			return (error_mark_node);
		}

		return
		(	convert_for_assignment
			(	type,
				default_conversion (init),
				"initialization"
			)
		);
	}

	/*
	 *	Come here only for records and arrays
	 */
	if (TYPE_SIZE (type) && ! TREE_LITERAL (TYPE_SIZE (type)))
	{
		error ("variable-sized object may not be initialized");
		return (error_mark_node);
	}

	if (code == ARRAY_TYPE || code == RECORD_TYPE)
	{
		if (raw_constructor)
			return (process_init_constructor (type, init, (tree *)NULL));

		if   (tail)
		{
			*tail = old_tail_contents;
			return (process_init_constructor (type, NULL_TREE, tail));
		}
		elif (flag_traditional)
		{
			/*
			 *	Traditionally one can say `char x[100] = 0;'
		 	 */
			return
			(	process_init_constructor
				(	type,
					build_nt
					(	CONSTRUCTOR,
						0,
						tree_cons
						(	NULL_TREE,
							init,
							NULL_TREE
						)
					),
					(tree *)NULL
				)	
			);
		}
	}

	error ("invalid initializer");

	return (error_mark_node);

}	/* end digest_init */

/*
 ****************************************************************
 *	Process a constructor for a variable of type TYPE	*
 ****************************************************************
 */
/*
 *	Process a constructor for a variable of type TYPE.
 *	The constructor elements may be specified either
 *	with INIT or with ELTS, only one of which should
 *	be non-null.
 *
 *	If INIT is specified, it is a CONSTRUCTOR node which
 *	is specifically and solely for initializing this datum.
 *
 *	If ELTS is specified, it is the address of a variable
 *	containing a list of expressions.  We take as many
 *	elements as we need from the head of the list and
 *	update the list.
 *
 *	In the resulting constructor, TREE_LITERAL is set
 *	if all elts are constant, and TREE_STATIC is set
 *	if, in addition, all elts are simple enough constants
 *	that the assembler and linker can compute them
 */
tree
process_init_constructor (tree type, tree init, tree *elts)
{
	/*
	 *	List of the elements of the result
	 *	constructor, in reverse order
	 */
	register tree	tail;
	register tree	members = NULL_TREE;
	tree		result;
	int		allconstant = 1;
	int		allsimple = 1;
	int		error_flag = 0;

	/*
	 *	Make TAIL be the list of elements to use for
	 *	the initialization, no matter how the data
	 *	was given to us
	 */
	if (elts)
		tail = *elts;
	else
		tail = CONSTRUCTOR_ELTS (init);

	/*
	 *	Gobble as many elements as needed, and make a
	 *	constructor or initial value for each element
	 *	of this aggregate. Chain them together in result.
	 *	If there are too few, use 0 for each scalar
	 *	ultimate component
	 */
	if (TREE_CODE (type) == ARRAY_TYPE)
	{
		tree		domain = TYPE_DOMAIN (type);
		register long	len;
		register int	i;

		if (domain)
		{
			len = TREE_INT_CST_LOW (TYPE_MAX_VALUE (domain)) -
				TREE_INT_CST_LOW (TYPE_MIN_VALUE (domain)) + 1;
		}
		else 		/* Take as many as there are */
		{
			len = -1;
		}

		for (i = 0; (len < 0 || i < len) && tail != NULL_TREE; i++)
		{
			register tree	next1;

			if (TREE_VALUE (tail) != NULL_TREE)
			{
				tree	tail1 = tail;

				next1 = digest_init
					(	TYPE_MAIN_VARIANT (TREE_TYPE (type)),
						TREE_VALUE (tail),
						&tail1
					);

				if (tail1 != NULL_TREE && TREE_CODE (tail1) != TREE_LIST)
					abort ();

				if (tail == tail1 && len < 0)
				{
					error
					(	"non-empty initializer for "
						"array of empty elements"
					);

					/*
					 *	Just ignore what we were
					 *	supposed to use
					 */
					tail1 = NULL_TREE;
				}

				tail = tail1;
			}
			else
			{
				next1 = error_mark_node;
				tail = TREE_CHAIN (tail);
			}

			if (next1 == error_mark_node)
				error_flag = 1;
			elif (!TREE_LITERAL (next1))
				allconstant = 0;
			elif (! initializer_constant_valid_p (next1))
				allsimple = 0;

			members = tree_cons (NULL_TREE, next1, members);
		}
	}

	if (TREE_CODE (type) == RECORD_TYPE)
	{
		register tree field;

		for
		(	field = TYPE_FIELDS (type);
			field && tail;
			field = TREE_CHAIN (field)
		)
		{
			register tree	next1;

			if (! DECL_NAME (field))
			{
				members = tree_cons (field, integer_zero_node, members);
				continue;
			}

			if (TREE_VALUE (tail) != NULL_TREE)
			{
				tree	tail1 = tail;

				next1 = digest_init (TREE_TYPE (field),
						TREE_VALUE (tail), &tail1);

				if (tail1 != NULL_TREE && TREE_CODE (tail1) != TREE_LIST)
					abort ();

				tail = tail1;
			}
			else
			{
				next1 = error_mark_node;
				tail = TREE_CHAIN (tail);
			}

			if (next1 == error_mark_node)
				error_flag = 1;
			elif (!TREE_LITERAL (next1))
				allconstant = 0;
			elif (! initializer_constant_valid_p (next1))
				allsimple = 0;

			members = tree_cons (field, next1, members);
		}
	}

	if (TREE_CODE (type) == UNION_TYPE)
	{
		register tree	field = TYPE_FIELDS (type);
		register tree	next1;

		/*
		 *	For a union, get the initializer for 1 fld
		 */

		if (TREE_VALUE (tail) != NULL_TREE)
		{
			tree	tail1 = tail;

			next1 = digest_init (TREE_TYPE (field),
						TREE_VALUE (tail), &tail1);

			if (tail1 != NULL_TREE && TREE_CODE (tail1) != TREE_LIST)
				abort ();

			tail = tail1;
		}
		else
		{
			next1 = error_mark_node;
			tail = TREE_CHAIN (tail);
		}

		if (next1 == error_mark_node)
			error_flag = 1;
		elif (!TREE_LITERAL (next1))
			allconstant = 0;
		elif (! initializer_constant_valid_p (next1))
			allsimple = 0;

		members = tree_cons (field, next1, members);
	}

	/*
	 *	If arguments were specified as a list,
	 *	just remove the ones we used
	 *
	 *	If arguments were specified as a constructor,
	 *	complain unless we used all the elements of
	 *	the constructor
	 */
	if (elts)
		*elts = tail;
	elif (tail)
		warning ("excess elements in aggregate initializer");

	if (error_flag)
		return (error_mark_node);

	result = build (CONSTRUCTOR, type, NULL_TREE, nreverse (members));

	if (allconstant)
		TREE_LITERAL (result) = 1;

	if (allconstant && allsimple)
		TREE_STATIC (result) = 1;

	return (result);

}	/* end process_init_constructor */

/*
 ****************************************************************
 *	Expand an ASM statement with operands			*
 ****************************************************************
 */
/*
 *	Expand an ASM statement with operands, handling
 *	output operands that are not variables or
 *	INDIRECT_REFS by transforming such cases into
 *	cases that expand_asm_operands can handle.
 *	Arguments are same as for expand_asm_operands
 */
void
c_expand_asm_operands (tree string, tree outputs, tree inputs,
			tree clobbers, int vol, char *filename, int line)
{
	/*
	 *	o[I] is the place that output number I should be written
	 */
	int		noutputs = list_length (outputs);
	register int	i;
	register tree	*o = (tree *) alloca (noutputs * sizeof (tree));
	register tree	tail;

	/*
	 *	Record the contents of OUTPUTS before it is modifed
	 */
	for (i = 0, tail = outputs; tail; tail = TREE_CHAIN (tail), i++)
		o[i] = TREE_VALUE (tail);

#if 0
	/*
	 *	Don't do this--it screws up operands expected
	 *	to be in memory
	 */
	/*
	 *	Perform default conversions on all inputs
	 */
	for (i = 0, tail = inputs; tail; tail = TREE_CHAIN (tail), i++)
		TREE_VALUE (tail) = default_conversion (TREE_VALUE (tail));
#endif

	/*
	 *	Generate the ASM_OPERANDS insn; store into the
	 *	TREE_VALUEs of OUTPUTS some trees for where the
	 *	values were actually stored
	 */
	expand_asm_operands (string, outputs, inputs, clobbers, vol, filename, line);

	/*
	 *	Copy all the intermediate outputs into the specified outputs
	 */
	for (i = 0, tail = outputs; tail; tail = TREE_CHAIN (tail), i++)
	{
		/*
		 *	Detect modification of read-only values.
		 *	(Otherwise done by build_modify_expr.)
		 */
		if (o[i] != TREE_VALUE (tail))
		{
			expand_expr
			(	build_modify_expr
				(	o[i], NOP_EXPR, TREE_VALUE (tail)
				),
				NULL_RTX,
				VOIDmode,
				0
			);
		}
		else
		{
			tree	type = TREE_TYPE (o[i]);

			if
			(	TREE_READONLY (o[i]) ||
				((TREE_CODE (type) == RECORD_TYPE ||
				TREE_CODE (type) == UNION_TYPE) &&
				C_TYPE_FIELDS_READONLY (type))
			)
				readonly_warning (o[i], "modification by `asm'");
		}
	}

	/*
	 *	Those MODIFY_EXPRs could do autoincrements
	 */
	emit_queue ();

}	/* end c_expand_asm_operands */

/*
 ****************************************************************
 *	Expand a C `return' statement				*
 ****************************************************************
 */
/*
 *	Expand a C `return' statement. RETVAL is the
 *	expression for what to return, or a null pointer
 *	for `return;' with no value
 */
void
c_expand_return (tree retval)
{
	tree	valtype = TREE_TYPE (TREE_TYPE (current_function_decl));

	if (TREE_THIS_VOLATILE (current_function_decl))
		warning ("function declared `volatile' has a `return' statement");

	if (!retval)
	{
		current_function_returns_null = 1;

		if
		(	warn_return_type && valtype != NULL_TREE &&
			TREE_CODE (valtype) != VOID_TYPE
		)
			warning
			(	"`return' with no value, in function "
				"returning non-void"
			);

			expand_null_return ();
	}
	elif (valtype == NULL_TREE || TREE_CODE (valtype) == VOID_TYPE)
	{
		current_function_returns_null = 1;

		if (pedantic || TREE_CODE (TREE_TYPE (retval)) != VOID_TYPE)
			warning ("`return' with a value, in function returning void");

		expand_return (retval);
	}
	else
	{
		tree	t = convert_for_assignment (valtype, retval, "return");
		tree	res = DECL_RESULT (current_function_decl);

		t =	build
			(	MODIFY_EXPR,
				TREE_TYPE (res),
				res,
				convert (TREE_TYPE (res), t)
			);

		expand_return (t);
		current_function_returns_value = 1;
	}

}	/* end c_expand_return */

/*
 ****************************************************************
 *	Start a C switch statement, testing expression EXP	*
 ****************************************************************
 */
/*
 *	Start a C switch statement, testing expression EXP.
 *	Return EXP if it is valid, an error node otherwise
 */
tree
c_expand_start_case (tree exp)
{
	register enum	tree_code code = TREE_CODE (TREE_TYPE (exp));
	tree		type = TREE_TYPE (exp);

	if (code != INTEGER_TYPE && code != ENUMERAL_TYPE && code != ERROR_MARK)
	{
		error ("switch quantity not an integer");
		exp = error_mark_node;
	}
	else
	{
		tree	index;

		exp = default_conversion (exp);
		type = TREE_TYPE (exp);
		index = get_unwidened (exp, NULL_TREE);

		/*
		 *	We can't strip a conversion from a signed
		 *	type to an unsigned, because if we did,
		 *	int_fits_type_p would do the wrong thing
		 *	when checking case values for being in range,
		 *	and it's too hard to do the right thing
		 */
		if
		(	TREE_UNSIGNED (TREE_TYPE (exp)) ==
				TREE_UNSIGNED (TREE_TYPE (index))
		)
			exp = index;
	}

	expand_start_case (1, exp, type);

	return (exp);

}	/* end c_expand_start_case */
