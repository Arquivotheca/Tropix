/*
 ****************************************************************
 *								*
 *			tchk/tchk-build0.c			*
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
 *	Versão	3.0.0, de 26.08.93				*
 *		3.0.0, de 15.10.93				*
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
#define	NULL	0

#include "../h/tree.h"
#include "../h/c-tree.h"

#define	elif	else if

/*
 ******	Definições externas *************************************
 */
extern tree	actualparameterlist (tree, tree, tree);
extern void	binary_op_error (enum tree_code);
extern tree	build (enum tree_code, ...);
extern tree	build_binary_op_nodefault (enum tree_code, tree, tree, enum tree_code);
extern tree	build_pointer_type (tree);
extern tree	build_unary_op (enum tree_code, tree, int);
extern tree	commontype (tree, tree);
extern tree	comp_target_types (tree, tree);
extern tree	convert (tree, tree);
extern tree	datatype (tree);
extern tree	default_conversion (tree);
extern void	error (const char *, ...);
extern tree	fold (tree);
extern tree	get_narrower (tree, int *);
extern void	incomplete_type_error (tree, tree);
extern int	int_fits_type_p (tree, tree);
extern int	integer_zerop (tree);
extern int	lvalue_p (tree);
extern int	mark_addressable (tree);
extern tree	pointer_diff (tree, tree);
extern tree	pointer_int_sum (enum tree_code, tree, tree);
extern tree	require_complete_type (tree);
extern tree	shorten_compare (tree *, tree *, tree *, enum tree_code *);
extern tree	signed_or_unsigned_type (int, tree);
extern tree	truthvalue_conversion (tree);
extern void	warning (char *, ...);

/*
 ****************************************************************
 *	Make an expression to refer to the COMPONENT field	*
 ****************************************************************
 */
/*
 *	Make an expression to refer to the COMPONENT field of
 *	structure or union value DATUM.  COMPONENT is an
 *	IDENTIFIER_NODE.
 */
tree
build_component_ref (tree datum, tree component)
{
	register tree		basename = datum;
	register tree		basetype = TREE_TYPE (basename);
	register enum tree_code	form = TREE_CODE (basetype);
	register tree		field = NULL_TREE;
	register tree		ref;

	/*
	 *	First, see if there is a field or component
	 *	with name COMPONENT.
	 */
	if   (form == RECORD_TYPE || form == UNION_TYPE)
	{
		if (TYPE_SIZE (basetype) == NULL_TREE)
		{
			incomplete_type_error (NULL_TREE, basetype);
			return error_mark_node;
		}

		/*
		 *	Look up component name in the structure
		 *	type definition.
		 */
		for
		(	field = TYPE_FIELDS (basetype);
			field;
			field = TREE_CHAIN (field)
		)
		{
			if (DECL_NAME (field) == component)
				break;
		}

		if (!field)
		{
			error
			(	form == RECORD_TYPE ?
					 "structure has no member named `%s'" :
					 "union has no member named `%s'",
				 IDENTIFIER_POINTER (component)
			);

			return (error_mark_node);
		}

		if (TREE_TYPE (field) == error_mark_node)
			return error_mark_node;

		ref = build (COMPONENT_REF, TREE_TYPE (field), basename, field);

		if (TREE_READONLY (basename) || TREE_READONLY (field))
			TREE_READONLY (ref) = 1;

		if (TREE_THIS_VOLATILE (basename) || TREE_VOLATILE (field))
			TREE_THIS_VOLATILE (ref) = 1;

			return (ref);
	}
	elif (form != ERROR_MARK)
	{
		error
		(	"request for member `%s' in something not "
			"a structure or union",
			IDENTIFIER_POINTER (component)
		);
	}

	return error_mark_node;

}	/* end build_component_ref */

/*
 ****************************************************************
 *	Return an expression for the value pointed to		*
 ****************************************************************
 */
/*
 *	Given an expression PTR for a pointer, return an
 *	expression for the value pointed to. ERRORSTRING
 *	is the name of the operator to appear in error messages.
 */
tree
build_indirect_ref (tree ptr, char *errorstring)
{
	register tree	pointer = default_conversion (ptr);
	register tree	dt = TREE_TYPE (pointer);

	if (TREE_CODE (dt) == POINTER_TYPE)
	{
		if
		(	TREE_CODE (pointer) == ADDR_EXPR &&
			(TREE_TYPE (TREE_OPERAND (pointer, 0)) == TREE_TYPE (dt))
		)
		{
			return TREE_OPERAND (pointer, 0);
		}
		else
		{
			tree		t = TREE_TYPE (dt);
			register tree	ref = build
				(INDIRECT_REF, TYPE_MAIN_VARIANT (t), pointer);

			if
			(	TREE_CODE (t) == VOID_TYPE ||
				(TYPE_SIZE (t) == NULL_TREE &&
						TREE_CODE (t) != ARRAY_TYPE)
			)
			{
				error
				(	"dereferencing pointer to "
					"incomplete type"
				);
				return (error_mark_node);
			}

			TREE_READONLY (ref) = TREE_READONLY (t);
			TREE_VOLATILE (ref) = TREE_VOLATILE (t) ||
							TREE_VOLATILE (pointer);
			TREE_THIS_VOLATILE (ref) = TREE_VOLATILE (t);

			return (ref);
		}
	}
	elif (TREE_CODE (pointer) != ERROR_MARK)
	{
		error ("invalid type argument of `%s'", errorstring);
	}

	return (error_mark_node);

}	/* end build_indirect_ref */

/*
 ****************************************************************
 *	This handles array references				*
 ****************************************************************
 */
/*
 *	This handles expressions of the form "a[i]", which
 *	denotes an array reference. This is logically equivalent
 *	in C to *(a+i), but we may do it differently. If A is
 *	a variable or a member, we generate a primitive ARRAY_REF.
 *	This avoids forcing the array out of registers, and can
 *	work on arrays that are not lvalues (for example, members
 *	of structures returned by functions).
 */
tree
build_array_ref (tree array, tree index)
{
	if (index == NULL_TREE)
	{
		error ("subscript missing in array reference");
		return error_mark_node;
	}

	if
	(	TREE_CODE (TREE_TYPE (array)) == ARRAY_TYPE &&
		TREE_CODE (array) != INDIRECT_REF
	)
	{
		tree	rval, type;

		index = default_conversion (index);

		if
		(	index != error_mark_node &&
			TREE_CODE (TREE_TYPE (index)) != INTEGER_TYPE
		)
		{
			error ("array subscript is not an integer");
			return error_mark_node;
		}

		/*
		 *	An array that is indexed by a non-constant
		 *	cannot be stored in a register; we must be
		 *	able to do address arithmetic on its address.
		 *	Likewise an array of elements of variable size.
		 */
		if
		(	TREE_CODE (index) != INTEGER_CST ||
			(TYPE_SIZE (TREE_TYPE (TREE_TYPE (array))) != NULL_TREE
			&& TREE_CODE (TYPE_SIZE
				(TREE_TYPE (TREE_TYPE (array)))) != INTEGER_CST)
		)
		{
			if (mark_addressable (array) == 0)
				return error_mark_node;
		}

		if (pedantic && !lvalue_p (array))
			warning ("ANSI C forbids subscripting non-lvalue array");

		if (pedantic)
		{
			tree	foo = array;

			while (TREE_CODE (foo) == COMPONENT_REF)
				foo = TREE_OPERAND (foo, 0);

			if (TREE_CODE (foo) == VAR_DECL && TREE_REGDECL (foo))
			{
				warning
				(	"ANSI C forbids subscripting "
					"non-lvalue array"
				);
			}
		}

		type = TYPE_MAIN_VARIANT (TREE_TYPE (TREE_TYPE (array)));
		rval = build (ARRAY_REF, type, array, index);

		/*
		 *	Array ref is const/volatile if the array
		 *	elements are, or if the array object is.
		 */
		TREE_READONLY (rval) |= (TREE_READONLY (TREE_TYPE
			(TREE_TYPE (array))) | TREE_READONLY (array));

		TREE_VOLATILE (rval) |= (TREE_VOLATILE (TREE_TYPE
			(TREE_TYPE (array))) | TREE_VOLATILE (array));

		TREE_THIS_VOLATILE (rval) |= TREE_VOLATILE (TREE_TYPE
			(TREE_TYPE (array)));

		return (require_complete_type (fold (rval)));
	}

	/*
	 *	Put the integer in IND to simplify error checking.
	 */
	{
		tree	ar = default_conversion (array);
		tree	ind = default_conversion (index);

		if (TREE_CODE (TREE_TYPE (ar)) == INTEGER_TYPE)
		{
			tree temp = ar;
			ar = ind;
			ind = temp;
		}

		if (TREE_CODE (TREE_TYPE (ar)) != POINTER_TYPE)
		{
			error ("subscripted value is neither array nor pointer");
			return error_mark_node;
		}

		if (TREE_CODE (TREE_TYPE (ind)) != INTEGER_TYPE)
		{
			error ("array subscript is not an integer");
			return error_mark_node;
		}

		return
		(	build_indirect_ref
			(	build_binary_op_nodefault
					(PLUS_EXPR, ar, ind, PLUS_EXPR),
				"array indexing"
			)
		);
	}

}	/* end build_array_ref */

/*
 ****************************************************************
 *	Build a function call to a function			*
 ****************************************************************
 */
/*
 *	Build a function call to function FUNCTION with
 *	parameters PARAMS. PARAMS is a list--a chain of
 *	TREE_LIST nodes--in which the TREE_VALUE of each
 *	node is a parameter-expression. FUNCTION's data
 *	type may be a function type or a pointer-to-function.
 */
tree
build_function_call (tree function, tree params)
{
	register tree	fntype;
	register tree	value_type;
	register tree	coerced_params;
	tree		name = NULL_TREE;

	/*
	 *	build_c_cast puts on a NOP_EXPR to make the result
	 *	not an lvalue. Strip such NOP_EXPRs, since FUNCTION
	 *	is used in non-lvalue context.
	 */
	if
	(	TREE_CODE (function) == NOP_EXPR &&
		TREE_TYPE (function) == TREE_TYPE (TREE_OPERAND (function, 0))
	)
		function = TREE_OPERAND (function, 0);

	/*
	 *	Convert anything with function type to a pointer-to-function.
	 */
	if (TREE_CODE (function) == FUNCTION_DECL)
	{
		name = DECL_NAME (function);

		/*
		 *	Differs from default_conversion by not setting
		 *	TREE_ADDRESSABLE (because calling an inline
		 *	function does not mean the function needs to
		 *	be separately compiled).
		 */
		function = build (ADDR_EXPR, build_pointer_type
				(TREE_TYPE (function)), function);
	}
	else
	{
		function = default_conversion (function);
	}

	fntype = TREE_TYPE (function);

	if (TREE_CODE (fntype) == ERROR_MARK)
		return (error_mark_node);

	if
	(	!(TREE_CODE (fntype) == POINTER_TYPE &&
		TREE_CODE (TREE_TYPE (fntype)) == FUNCTION_TYPE)
	)
	{
		error ("called object is not a function");
		return error_mark_node;
	}

	/*
	 *	fntype now gets the type of function pointed to.
	 */
	fntype = TREE_TYPE (fntype);

	/*
	 *	Convert the parameters to the types declared in the
	 *	function prototype, or apply default promotions.
	 */
	coerced_params = actualparameterlist
				(TYPE_ARG_TYPES (fntype), params, name);

	/*
	 *	Recognize certain built-in functions so we can make
	 *	tree-codes other than CALL_EXPR.  We do this when
	 *	it enables fold-const.c to do something useful.
	 */
	if
	(	TREE_CODE (function) == ADDR_EXPR &&
		TREE_CODE (TREE_OPERAND (function, 0)) == FUNCTION_DECL
	)
	{
		switch (DECL_FUNCTION_CODE (TREE_OPERAND (function, 0)))
		{
		    case BUILT_IN_ABS:
		    case BUILT_IN_LABS:
		    case BUILT_IN_FABS:
			if (coerced_params == NULL_TREE)
				return (integer_zero_node);

			return
			(	build_unary_op
				(	ABS_EXPR,
					TREE_VALUE (coerced_params),
					0
				)
			);
		}
	}

	value_type = TREE_TYPE (fntype) ? TREE_TYPE (fntype) : void_type_node;

	{
		register tree result = build (CALL_EXPR,
			value_type, function, coerced_params, NULL_TREE);

		TREE_VOLATILE (result) = 1;

		if (value_type == void_type_node)
			return (result);

		return (require_complete_type (result));
	}

}	/* end build_function_call */

/*
 ****************************************************************
 *	Build a binary-operation expression			*
 ****************************************************************
 */
/*
 *	Build a binary-operation expression, after performing
 *	default conversions on the operands. CODE is the kind
 *	of expression to build.
 */
tree
build_binary_op (enum tree_code code, tree arg1, tree arg2)
{
	return
	(	build_binary_op_nodefault
		(	code,
			default_conversion (arg1),
			default_conversion (arg2),
			code
		)
	);

}	/* end build_binary_op */

/*
 ****************************************************************
 *	Build a binary-operation expression			*
 ****************************************************************
 */
/*
 *	Build a binary-operation expression without default
 *	conversions. CODE is the kind of expression to build.
 *	This function differs from `build' in several ways:
 *	the data type of the result is computed and recorded
 *	in it, warnings are generated if arg data types are
 *	invalid, special handling for addition and subtraction
 *	of pointers is known, and some optimization is done
 *	(operations on narrow ints are done in the narrower
 *	type when that gives the same result). Constant folding
 *	is also done before the result is returned.
 *
 *	ERROR_CODE is the code that determines what to say in
 *	error messages. It is usually, but not always,
 *	the same as CODE.
 *
 *	Note that the operands will never have enumeral types
 *	because either they have just had the default conversions
 *	performed or they have both just been converted to some
 *	other type in which the arithmetic is to be done.
 */
tree
build_binary_op_nodefault (enum tree_code code, tree op0, tree op1,
				enum tree_code error_code)
{
	tree	dt0 = datatype (op0), dt1 = datatype (op1);

	/*
	 *	The expression codes of the data types of the
	 *	arguments tell us whether the arguments are
	 *	integers, floating, pointers, etc.
	 */
	register enum tree_code	code0 = TREE_CODE (dt0);
	register enum tree_code code1 = TREE_CODE (dt1);

	/*
	 *	Expression code to give to the expression when
	 *	it is built. Normally this is CODE, which is
	 *	what the caller asked for, but in some special
	 *	cases we change it.
	 */
	register enum tree_code resultcode = code;

	/*
	 *	Data type in which the computation is to be performed.
	 *	In the simplest cases this is the common type of
	 *	the arguments.
	 */
	register tree	result_type = NULL_TREE;

	/*
	 *	Nonzero means operands have already been type-converted
	 *	in whatever way is necessary. Zero means they need to
	 *	be converted to RESULT_TYPE.
	 */
	int		converted = 0;

	/*
	 *	Nonzero means after finally constructing the expression
	 *	give it this type. Otherwise, give it type RESULT_TYPE.
	 */
	tree		final_type = NULL_TREE;

	/*
	 *	Nonzero if this is an operation like MIN or MAX which
	 *	can safely be computed in short if both args are
	 *	promoted shorts. Also implies COMMON. -1 indicates a
	 *	bitwise operation; this makes a difference in the exact
	 *	conditions for when it is safe to do the operation
	 *	 in a narrower mode.
	 */
	int		shorten = 0;

	/*
	 *	Nonzero if this is a comparison operation; if both
	 *	args are promoted shorts, compare the original shorts.
	 *	Also implies COMMON.
	 */
	int		short_compare = 0;

	/*
	 *	Nonzero if this is a right-shift operation, which
	 *	can be computed on the original short and then
	 *	promoted if the operand is a promoted short.
	 */
	int		short_shift = 0;

	/*
	 *	Nonzero means set RESULT_TYPE to the common type
	 *	of the args.
	 */
	int		common = 0;

	/*
	 *	If an error was already reported for one of the
	 *	arguments, avoid reporting another error.
	 */
	if (code0 == ERROR_MARK || code1 == ERROR_MARK)
		return (error_mark_node);

	switch (code)
	{
		/*
		 *	Handle the pointer + int case.
		 */
	    case PLUS_EXPR:
		if   (code0 == POINTER_TYPE && code1 == INTEGER_TYPE)
			return (pointer_int_sum (PLUS_EXPR, op0, op1));
		elif (code1 == POINTER_TYPE && code0 == INTEGER_TYPE)
			return (pointer_int_sum (PLUS_EXPR, op1, op0));
		else
			common = 1;
		break;

		/*
		 *	Subtraction of two similar pointers. We must
		 *	subtract them as integers, then divide by
		 *	object size.
		 */
	    case MINUS_EXPR:
		if
		(	code0 == POINTER_TYPE && code1 == POINTER_TYPE &&
			comp_target_types (dt0, dt1)
		)
		{
			return pointer_diff (op0, op1);
		}
		elif (code0 == POINTER_TYPE && code1 == INTEGER_TYPE)
		{
			/*
			 *	Handle pointer minus int.
			 *	Just like pointer plus int.
			 */
			return pointer_int_sum (MINUS_EXPR, op0, op1);
		}
		else
		{
			common = 1;
		}

		break;

	    case MULT_EXPR:
		common = 1;
		break;

	    case TRUNC_DIV_EXPR:
	    case CEIL_DIV_EXPR:
	    case FLOOR_DIV_EXPR:
	    case ROUND_DIV_EXPR:
	    case EXACT_DIV_EXPR:
		if
		(	(code0 == INTEGER_TYPE || code0 == REAL_TYPE) &&
			(code1 == INTEGER_TYPE || code1 == REAL_TYPE)
		)
		{
			/*	When dividing two signed integers,
			 *	you have to promote to int.
			 *	E.g. (short) -32868 / (short) -1
			 *	doesn't fit in a short.
			 */
			if (!(code0 == INTEGER_TYPE && code1 == INTEGER_TYPE))
				resultcode = RDIV_EXPR;
			else
				shorten = TREE_UNSIGNED (dt0);

			common = 1;
		}
		break;

	    case BIT_AND_EXPR:
	    case BIT_ANDTC_EXPR:
	    case BIT_IOR_EXPR:
	    case BIT_XOR_EXPR:
		if (code0 == INTEGER_TYPE && code1 == INTEGER_TYPE)
			shorten = -1;

		/*
		 *	If one operand is a constant, and the other
		 *	is a short type that has been converted to
		 *	an int, really do the work in the short type
		 *	and then convert the result to int. If we are
		 *	lucky, the constant will be 0 or 1 in the short
		 *	type, making the entire operation go away.
		 */
		if
		(	TREE_CODE (op0) == INTEGER_CST &&
			TREE_CODE (op1) == NOP_EXPR &&
			TYPE_PRECISION (dt1) > TYPE_PRECISION
				(TREE_TYPE (TREE_OPERAND (op1, 0))) &&
			TREE_UNSIGNED (TREE_TYPE (TREE_OPERAND (op1, 0)))
		)
		{
			final_type = result_type;
			op1 = TREE_OPERAND (op1, 0);
			result_type = TREE_TYPE (op1);
		}

		if
		(	TREE_CODE (op1) == INTEGER_CST &&
			TREE_CODE (op0) == NOP_EXPR &&
			TYPE_PRECISION (dt0) > TYPE_PRECISION
					(TREE_TYPE (TREE_OPERAND (op0, 0))) &&
			TREE_UNSIGNED (TREE_TYPE (TREE_OPERAND (op0, 0)))
		)
		{
			final_type = result_type;
			op0 = TREE_OPERAND (op0, 0);
			result_type = TREE_TYPE (op0);
		}
		break;

	    case TRUNC_MOD_EXPR:
		if (code0 == INTEGER_TYPE && code1 == INTEGER_TYPE)
			shorten = 1;
		break;

	    case TRUTH_ANDIF_EXPR:
	    case TRUTH_ORIF_EXPR:
	    case TRUTH_AND_EXPR:
	    case TRUTH_OR_EXPR:
		if
		(	(code0 == INTEGER_TYPE || code0 == POINTER_TYPE ||
						code0 == REAL_TYPE) &&
			(code1 == INTEGER_TYPE || code1 == POINTER_TYPE ||
						code1 == REAL_TYPE)
		)
		{
			/*
			 *	Result of these operations is always an int,
			 *	but that does not mean the operands should be
			 *	converted to ints!
			 */
			result_type = integer_type_node;
			op0 = truthvalue_conversion (op0);
			op1 = truthvalue_conversion (op1);
			converted = 1;
		}

		break;

		/*
		 *	Shift operations: result has same type as
		 *	first operand; always convert second operand
		 *	to int. Also set SHORT_SHIFT if shifting rightward.
		 */
	    case RSHIFT_EXPR:
		if (code0 == INTEGER_TYPE && code1 == INTEGER_TYPE)
		{
			result_type = dt0;

			if
			(	TREE_CODE (op1) == INTEGER_CST &&
				TREE_INT_CST_LOW (op1) > 0
			)
				short_shift = 1;

			/*
			 *	Convert the shift-count to an integer,
			 *	regardless of size of value being shifted.
			 */
			if (TREE_TYPE (op1) != integer_type_node)
				op1 = convert (integer_type_node, op1);
		}

		break;

	    case LSHIFT_EXPR:
		if (code0 == INTEGER_TYPE && code1 == INTEGER_TYPE)
		{
			result_type = dt0;

			if
			(	TREE_CODE (op1) == INTEGER_CST &&
				TREE_INT_CST_LOW (op1) < 0
			)
				short_shift = 1;

			/*
			 *	Convert the shift-count to an integer,
			 *	regardless of size of value being shifted.
			 */
			if (TREE_TYPE (op1) != integer_type_node)
				op1 = convert (integer_type_node, op1);
		}

		break;

	    case RROTATE_EXPR:
	    case LROTATE_EXPR:
		if (code0 == INTEGER_TYPE && code1 == INTEGER_TYPE)
		{
			result_type = dt0;

			/*
			 *	Convert the shift-count to an integer,
			 *	regardless of size of value being shifted.
			 */
			if (TREE_TYPE (op1) != integer_type_node)
				op1 = convert (integer_type_node, op1);
		}

		break;

		/*
		 *	Result of comparison is always int,
	 	 *	but don't convert the args to int!
		 */
	    case EQ_EXPR:
	    case NE_EXPR:
		result_type = integer_type_node;
		converted = 1;

		if
		(	(code0 == INTEGER_TYPE || code0 == REAL_TYPE) &&
			(code1 == INTEGER_TYPE || code1 == REAL_TYPE)
		)
		{
			short_compare = 1;
		}
		elif (code0 == POINTER_TYPE && code1 == POINTER_TYPE)
		{
			register tree	tt0 = TREE_TYPE (dt0);
			register tree	tt1 = TREE_TYPE (dt1);

			/*
			 *	Anything compares with void *.
			 *	void * compares with anything.
			 *	Otherwise, the targets must be the same.
			 */
			if (comp_target_types (dt0, dt1))
			{
				/* vazio */;
			}
			elif (TYPE_MAIN_VARIANT (tt0) == void_type_node)
			{
				if
				(	pedantic &&
					TREE_CODE (tt1) == FUNCTION_TYPE
				)
				{
					warning
					(	"ANSI C forbids comparison "
						"of `void *' with function "
						"pointer"
					);
				}
			}
			elif (TYPE_MAIN_VARIANT (tt1) == void_type_node)
			{
				if
				(	pedantic &&
					TREE_CODE (tt0) == FUNCTION_TYPE
				)
				{
					warning
					(	"ANSI C forbids comparison "
						"of `void *' with function "
						"pointer"
					);
				}
			}
			else
			{
				warning
				(	"comparison of distinct pointer "
					"types lacks a cast"
				);
			}
		}
		elif
		(	code0 == POINTER_TYPE &&
			TREE_CODE (op1) == INTEGER_CST &&
			integer_zerop (op1)
		)
		{
			op1 = null_pointer_node;
		}
		elif
		(	code1 == POINTER_TYPE &&
			TREE_CODE (op0) == INTEGER_CST &&
			integer_zerop (op0)
		)
		{
			op0 = null_pointer_node;
		}
		elif (code0 == POINTER_TYPE && code1 == INTEGER_TYPE)
		{
			if (! flag_traditional)
			{
				warning
				(	"comparison between pointer "
					"and integer"
				);
			}

			op1 = convert (TREE_TYPE (op0), op1);
		}
		elif (code0 == INTEGER_TYPE && code1 == POINTER_TYPE)
		{
			if (! flag_traditional)
			{
				warning
				(	"comparison between pointer "
					"and integer"
				);
			}

			op0 = convert (TREE_TYPE (op1), op0);
		}
		else
		{
			/*
			 *	If args are not valid, clear out
			 *	RESULT_TYPE to cause an error message later.
			 */
			result_type = NULL_TREE;
		}

		break;

	    case MAX_EXPR:
	    case MIN_EXPR:
		if
		(	(code0 == INTEGER_TYPE || code0 == REAL_TYPE) &&
			(code1 == INTEGER_TYPE || code1 == REAL_TYPE)
		)
		{
			shorten = 1;
		}
		elif (code0 == POINTER_TYPE && code1 == POINTER_TYPE)
		{
			if (! comp_target_types (dt0, dt1))
			{
				warning
				(	"comparison of distinct pointer "
					"types lacks a cast"
				);
			}
			elif
			(	pedantic &&
				TREE_CODE (TREE_TYPE (dt0)) == FUNCTION_TYPE
			)
			{
				warning
				(	"ANSI C forbids ordered comparisons "
					"of pointers to functions"
				);
			}

			result_type = commontype (dt0, dt1);
		}

		break;

	    case LE_EXPR:
	    case GE_EXPR:
	    case LT_EXPR:
	    case GT_EXPR:
		if
		(	(code0 == INTEGER_TYPE || code0 == REAL_TYPE) &&
			(code1 == INTEGER_TYPE || code1 == REAL_TYPE)
		)
		{
			short_compare = 1;
		}
		elif (code0 == POINTER_TYPE && code1 == POINTER_TYPE)
		{
			if (! comp_target_types (dt0, dt1))
			{
				warning
				(	"comparison of distinct pointer "
					"types lacks a cast"
				);
			}
			elif
			(	pedantic &&
				TREE_CODE (TREE_TYPE (dt0)) == FUNCTION_TYPE
			)
			{
				warning
				(	"ANSI C forbids ordered comparisons "
					"of pointers to functions"
				);
			}

			result_type = integer_type_node;
		}
		elif
		(	code0 == POINTER_TYPE &&
			TREE_CODE (op1) == INTEGER_CST &&
			integer_zerop (op1)
		)
		{
			result_type = integer_type_node;
			op1 = null_pointer_node;

			if (! flag_traditional)
			{
				warning
				(	"ordered comparison of pointer "
					"with integer zero"
				);
			}
		}
		elif
		(	code1 == POINTER_TYPE &&
			TREE_CODE (op0) == INTEGER_CST &&
			integer_zerop (op0)
		)
		{
			result_type = integer_type_node;
			op0 = null_pointer_node;

			if (pedantic)
			{
				warning
				(	"ordered comparison of pointer "
					"with integer zero"
				);
			}
		}
		elif (code0 == POINTER_TYPE && code1 == INTEGER_TYPE)
		{
			result_type = integer_type_node;

			if (! flag_traditional)
				warning ("comparison between pointer and integer");

			op1 = convert (TREE_TYPE (op0), op1);
		}
		elif (code0 == INTEGER_TYPE && code1 == POINTER_TYPE)
		{
			result_type = integer_type_node;

			if (! flag_traditional)
				warning ("comparison between pointer and integer");

			op0 = convert (TREE_TYPE (op1), op0);
		}

		converted = 1;
		break;

	}	/* end switch (code) */

	/*
	 *	For certain operations (which identify themselves by
	 *	shorten != 0) if both args were extended from the same
	 *	smaller type, do the arithmetic in that type and then extend.
	 *
	 *	shorten !=0 and !=1 indicates a bitwise operation.
	 *	For them, this optimization is safe only if both args
	 *	are zero-extended or both are sign-extended. Otherwise,
	 *	we might change the result. Eg,
	 *	(short)-1 | (unsigned short)-1	 is	 (int)-1
	 *	but calculated in
	 *	(unsigned short) it would be (unsigned short)-1.
	 */
	if
	(	(code0 == INTEGER_TYPE || code0 == REAL_TYPE) &&
		(code1 == INTEGER_TYPE || code1 == REAL_TYPE)
	)
	{
		if (shorten || common || short_compare)
			result_type = commontype (dt0, dt1);

		if (shorten)
		{
			int	unsigned0, unsigned1;
			tree	arg0 = get_narrower (op0, &unsigned0);
			tree	arg1 = get_narrower (op1, &unsigned1);
			/*
			 *	UNS is 1 if the operation to be done
			 *	is an unsigned one
			 */
			int	uns = TREE_UNSIGNED (result_type);
			tree 	type;

			final_type = result_type;

			/*
			 *	Handle the case that OP0 does not
			 *	*contain* a conversion but it *requires*
			 *	conversion to FINAL_TYPE
			 */
			if (op0 == arg0 && TREE_TYPE (op0) != final_type)
				unsigned0 = TREE_UNSIGNED (TREE_TYPE (op0));

			if (op1 == arg1 && TREE_TYPE (op1) != final_type)
				unsigned1 = TREE_UNSIGNED (TREE_TYPE (op1));

			/*
			 *	Now UNSIGNED0 is 1 if ARG0 zero-extends
			 *	to FINAL_TYPE.
			 *
			 *	For bitwise operations, signedness of
			 *	nominal type does not matter. Consider
			 *	only how operands were extended.
			 */
			if (shorten == -1)
				uns = unsigned0;

			/*
			 *	Note that in all three cases below
			 *	we refrain from optimizing an unsigned
			 *	operation on sign-extended args.
			 *	That would not be valid. 
			 *
			 *	Both args variable: if both extended
			 *	in same way from same width, do it in
			 *	that width. Do it unsigned if args
			 *	were zero-extended.
			 */
			if
			(	(TYPE_PRECISION (TREE_TYPE (arg0))
					 < TYPE_PRECISION (result_type)) &&
				(TYPE_PRECISION (TREE_TYPE (arg1))
					== TYPE_PRECISION (TREE_TYPE (arg0)))
				&& unsigned0 == unsigned1
				&& (unsigned0 || !uns)
			)
			{
				result_type = signed_or_unsigned_type
				(	unsigned0, commontype
					(TREE_TYPE (arg0), TREE_TYPE (arg1))
				);
			}
			elif
			(	TREE_CODE (arg0) == INTEGER_CST &&
				(unsigned1 || !uns) &&
				(TYPE_PRECISION (TREE_TYPE (arg1))
					 < TYPE_PRECISION (result_type)) &&
				(type = signed_or_unsigned_type
						(unsigned1, TREE_TYPE (arg1)),
					 int_fits_type_p (arg0, type))
			)
			{
				result_type = type;
			}
			elif
			(	TREE_CODE (arg1) == INTEGER_CST &&
				(unsigned0 || !uns) &&
				(TYPE_PRECISION (TREE_TYPE (arg0))
					 < TYPE_PRECISION (result_type)) &&
				(type = signed_or_unsigned_type
						(unsigned0, TREE_TYPE (arg0)),
					 int_fits_type_p (arg1, type))
			)
			{
				result_type = type;
			}
		}

		/*
		 *	Shifts can be shortened if shifting right
		 */
		if (short_shift)
		{
			int	unsigned_arg;
			tree	arg0 = get_narrower (op0, &unsigned_arg);

			final_type = result_type;

			if (arg0 == op0 && final_type == TREE_TYPE (op0))
				unsigned_arg = TREE_UNSIGNED (TREE_TYPE (op0));

			/*
			 *	If arg is sign-extended and then
			 *	unsigned-shifted, we can simulate this
			 *	with a signed shift in arg's type only
			 *	if the extended result is at least twice
			 *	as wide as the arg.  Otherwise, the shift
			 *	could use up all the ones made by sign-
			 *	extension and bring in zeros.
			 *	We can't optimize that case at all,
			 *	but in most machines it never happens
			 *	because available widths are 2**N.
			 */
			if
			(	TYPE_PRECISION (TREE_TYPE (arg0)) <
						TYPE_PRECISION (result_type) &&
				(!TREE_UNSIGNED (final_type) ||
				unsigned_arg ||
				2 * TYPE_PRECISION (TREE_TYPE (arg0)) <=
					TYPE_PRECISION (result_type))
			)
			{
				/*
				 *	Do an unsigned shift if the operand
				 *	was zero-extended.
				 */
				result_type = signed_or_unsigned_type
					(unsigned_arg, TREE_TYPE (arg0));

				/*
				 *	Convert value-to-be-shifted to
				 *	that type.
				 */
				if (TREE_TYPE (op0) != result_type)
					op0 = convert (result_type, op0);

				converted = 1;
			}
		}

		/*
		 *	Comparison operations are shortened too but
		 *	differently. They identify themselves by
		 *	setting short_compare = 1.
		 */
		if (short_compare)
		{
			/*
			 *	Don't write &op0, etc., because that
			 *	would prevent op0 from being kept in
			 *	a register. Instead, make copies of
			 *	the our local variables and pass the
			 *	copies by reference, then copy them
			 *	back afterward.
			 */
			tree		xop0 = op0, xop1 = op1;
			tree		xresult_type = result_type;
			enum tree_code	xresultcode = resultcode;
			tree		val = shorten_compare
				(&xop0, &xop1, &xresult_type, &xresultcode);

			if (val != NULL_TREE)
				return (val);

			op0 = xop0, op1 = xop1, result_type = xresult_type;
			resultcode = xresultcode;
		}
	}

	/*
	 *	At this point, RESULT_TYPE must be nonzero to avoid
	 *	an error message. If CONVERTED is zero, both args
	 *	will be converted to type RESULT_TYPE. Then the
	 *	expression will be built. It will be given type
	 *	FINAL_TYPE if that is nonzero; otherwise, it will
	 *	be given type RESULT_TYPE.
	 */
	if (!result_type)
	{
		binary_op_error (error_code);
		return (error_mark_node);
	}

	if (! converted)
	{
		if (TREE_TYPE (op0) != result_type)
			op0 = convert (result_type, op0);

		if (TREE_TYPE (op1) != result_type)
			op1 = convert (result_type, op1);
	}

	{
		register tree	result = build
					(resultcode, result_type, op0, op1);
		register tree	folded;

		folded = fold (result);

		if (folded == result)
			TREE_LITERAL (folded) =
				TREE_LITERAL (op0) & TREE_LITERAL (op1);

		if (final_type != NULL_TREE)
			return (convert (final_type, folded));

		return (folded);
	}

}	/* end build_binary_op_nodefault */
