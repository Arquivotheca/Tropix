/*
 ****************************************************************
 *								*
 *			tchk/tchk-build1.c			*
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
#pragma	getenv (GEN)

#include ../$GEN.gen/config.h

#include "../h/tree.h"
#include "../h/c-tree.h"

#define	NULL	0
#define	NOSTR	(char *)0
#define	elif	else if

/*
 ******	Definições externas *************************************
 */
extern tree	build (enum tree_code, ...);
extern tree	build_binary_op (enum tree_code, tree, tree);
extern tree	build_int_2 (int, int);
extern tree	build_modify_expr (tree, enum tree_code, tree);
extern tree	build_pointer_type (tree);
extern tree	c_build_type_variant (tree, int, int);
extern tree	c_sizeof_nowarn (tree);
extern tree	commontype (tree, tree);
extern tree	comp_target_types (tree, tree);
extern tree	convert (tree, tree);
extern tree	convert_for_assignment (tree, tree, char *);
extern tree	copy_node (tree);
extern tree	default_conversion (tree);
extern void	error (const char *, ...);
extern tree	fold (tree);
extern tree	get_unwidened (tree, tree);
extern int	integer_zerop (tree);
extern tree	invert_truthvalue (tree);
extern int	lvalue_or_else (tree, char *);
extern int	lvalue_p (tree);
extern int	mark_addressable (tree);
extern tree	qualify_type (tree, tree);
extern void	readonly_warning (tree, char *);
extern tree	require_complete_type (tree);
extern tree	save_expr (tree);
extern tree	stabilize_reference (tree);
extern int	staticp (tree);
extern tree	truthvalue_conversion (tree);
extern tree	unary_complex_lvalue (enum tree_code, tree);
extern void 	warning	(char *, ...);

/*
 ****************************************************************
 *	Construct and perhaps optimize a tree representation	*
 ****************************************************************
 */
/*
 *	Construct and perhaps optimize a tree representation
 *	for a unary operation.  CODE, a tree_code, specifies
 *	the operation and XARG is the operand. NOCONVERT
 *	nonzero suppresses the default promotions (such as
 *	from short to int).
 */
tree
build_unary_op (enum tree_code code, tree xarg, int noconvert)
{
	/*
	 *	No default_conversion here.
	 *	It causes trouble for ADDR_EXPR.
	 */
	register tree		arg = xarg;
	register tree		argtype = NULL_TREE;
	register enum tree_code	typecode = TREE_CODE (TREE_TYPE (arg));
	char			*errstring = NOSTR;
	tree			val;

	if (typecode == ERROR_MARK)
		return (error_mark_node);

	if (typecode == ENUMERAL_TYPE)
		typecode = INTEGER_TYPE;

	switch (code)
	{
		/*
		 *	This is used for unary plus, because a
		 *	CONVERT_EXPR is enough to prevent anybody
		 *	from looking inside for associativity, but
		 *	won't generate any code.
		 */
	    case CONVERT_EXPR:
		if   (!(typecode == INTEGER_TYPE || typecode == REAL_TYPE))
			errstring = "wrong type argument to unary plus";
		elif (!noconvert)
			arg = default_conversion (arg);
		break;

	    case NEGATE_EXPR:
		if (!(typecode == INTEGER_TYPE || typecode == REAL_TYPE))
			errstring = "wrong type argument to unary minus";
		elif (!noconvert)
			arg = default_conversion (arg);
		break;

	    case BIT_NOT_EXPR:
		if (typecode != INTEGER_TYPE)
			errstring = "wrong type argument to bit-complement";
		elif (!noconvert)
			arg = default_conversion (arg);
		break;

	    case ABS_EXPR:
		if (!(typecode == INTEGER_TYPE || typecode == REAL_TYPE))
			errstring = "wrong type argument to abs";
		elif (!noconvert)
			arg = default_conversion (arg);
		break;

	    case TRUTH_NOT_EXPR:
		if
		(	typecode != INTEGER_TYPE &&
			typecode != REAL_TYPE && typecode != POINTER_TYPE &&
			/* This will convert to a pointer.	*/
			typecode != ARRAY_TYPE && typecode != FUNCTION_TYPE
		)
		{
			errstring =
				"wrong type argument to unary exclamation mark";
			break;
		}

		arg = truthvalue_conversion (arg);
		val = invert_truthvalue (arg);

		if (val)
			return (val);
		break;

	    case NOP_EXPR:
		break;

		/*
		 *	Handle complex lvalues (when permitted)
	 	 *	by reduction to simpler cases.
		 */
	    case PREINCREMENT_EXPR:
	    case POSTINCREMENT_EXPR:
	    case PREDECREMENT_EXPR:
	    case POSTDECREMENT_EXPR:
	    {
		register tree	inc;
		tree		result_type;

		val = unary_complex_lvalue (code, arg);

		if (val != NULL_TREE)
			return (val);

		/*
		 *	Report invalid types.
		 */
		if
		(	typecode != POINTER_TYPE &&
			typecode != INTEGER_TYPE && typecode != REAL_TYPE
		)
		{
			if (code == PREINCREMENT_EXPR || code == POSTINCREMENT_EXPR)
				errstring ="wrong type argument to increment";
			else
				errstring ="wrong type argument to decrement";
			break;
		}

		/*
		 *	Report something read-only.
		 */
		if (TREE_READONLY (arg))
		{
			readonly_warning
			(	arg,
				((code == PREINCREMENT_EXPR ||
					code == POSTINCREMENT_EXPR) ?
				 "increment" : "decrement")
			);
		}

		result_type = TREE_TYPE (arg);

		arg = get_unwidened (arg, NULL_TREE);
		argtype = TREE_TYPE (arg);

		/*
		 *	Compute the increment.
		 */
		if (typecode == POINTER_TYPE)
		{
			if
			(	pedantic &&
				(TREE_CODE (TREE_TYPE (result_type)) ==
						FUNCTION_TYPE ||
				TREE_CODE (TREE_TYPE (result_type)) ==
						VOID_TYPE)
			)
			{
				warning
				(	"wrong type argument to %s",
					 ((code == PREINCREMENT_EXPR ||
					 code == POSTINCREMENT_EXPR) ?
					"increment" : "decrement")
				);
			}

			inc = c_sizeof_nowarn (TREE_TYPE (result_type));
		}
		else
		{
			inc = integer_one_node;
		}

		inc = convert (argtype, inc);

		/*
		 *	Handle incrementing a cast-expression.
		 */
		if (!pedantic) switch (TREE_CODE (arg))
		{
		    case NOP_EXPR:
		    case CONVERT_EXPR:
		    case FLOAT_EXPR:
		    case FIX_TRUNC_EXPR:
		    case FIX_FLOOR_EXPR:
		    case FIX_ROUND_EXPR:
		    case FIX_CEIL_EXPR:
		    {
			tree	incremented, modify, value;

			arg = stabilize_reference (arg);

			if (code == PREINCREMENT_EXPR || code == PREDECREMENT_EXPR)
				value = arg;
			else
				value = save_expr (arg);

			incremented = build
			(	((code == PREINCREMENT_EXPR ||
						code == POSTINCREMENT_EXPR) ?
						PLUS_EXPR : MINUS_EXPR),
				 argtype, value, inc
			);

			TREE_VOLATILE (incremented) = 1;
			modify = build_modify_expr (arg, NOP_EXPR, incremented);

			return
			(	build
				(COMPOUND_EXPR, TREE_TYPE (arg), modify, value)
			);
		    }
		}

		/*
		 *	Complain about anything else that is
		 *	not a true lvalue.
		 */
		if
		(	!lvalue_or_else
			(	arg, ((code == PREINCREMENT_EXPR ||
						code == POSTINCREMENT_EXPR) ?
						"increment" : "decrement")
			)
		)
			return (error_mark_node);

		val = build (code, TREE_TYPE (arg), arg, inc);
		TREE_VOLATILE (val) = 1;

		return (convert (result_type, val));
	    }

		/*
		 *	Note that this operation never does
		 *	default_conversion regardless of NOCONVERT.
		 *
		 *	Let &* cancel out to simplify resulting code.
		 */
	    case ADDR_EXPR:
	    {
		tree addr;

		if (TREE_CODE (arg) == INDIRECT_REF)
		{
			/*
			 *	Don't let this be an lvalue.
			 */
			if (lvalue_p (TREE_OPERAND (arg, 0)))
			{
				return
				(	build
					(	NOP_EXPR,
						TREE_TYPE (TREE_OPERAND (arg, 0)),
						TREE_OPERAND (arg, 0)
					)
				);
			}

			return TREE_OPERAND (arg, 0);
		}

		/*
		 *	For &x[y], return x+y
		 */
		if (TREE_CODE (arg) == ARRAY_REF)
		{
			if (mark_addressable (TREE_OPERAND (arg, 0)) == 0)
				return (error_mark_node);

			return
			(	build_binary_op
				(	PLUS_EXPR,
					TREE_OPERAND (arg, 0),
					TREE_OPERAND (arg, 1)
				)
			);
		}

		/*
		 *	Handle complex lvalues (when permitted)
		 *	by reduction to simpler cases.
		 */
		val = unary_complex_lvalue (code, arg);

		if (val != NULL_TREE)
			return (val);

#if (0)	/*************************************/
		/*
		 *	Turned off because inconsistent;
		 *	float f; *&(int)f = 3.4 stores in int format
		 *	whereas (int)f = 3.4 stores in float format.
		 */

		/*
		 *	Address of a cast is just a cast of the address
		 *	of the operand of the cast.
		 */
	 	switch (TREE_CODE (arg))
		{
		    case NOP_EXPR:
		    case CONVERT_EXPR:
		    case FLOAT_EXPR:
		    case FIX_TRUNC_EXPR:
		    case FIX_FLOOR_EXPR:
		    case FIX_ROUND_EXPR:
		    case FIX_CEIL_EXPR:
			if (pedantic)
			{
				warning
				(	"ANSI C forbids the address "
					"of a cast expression"
				);
			}

			return
			(	convert
				(	build_pointer_type (TREE_TYPE (arg)),
					build_unary_op
					(	ADDR_EXPR,
						TREE_OPERAND (arg, 0),
						0
					)
				)
			);
		}
#endif	/*************************************/

		/*
		 *	Allow the address of a constructor if all
		 *	the elements are constant.
		 *
		 *	Anything not already handled and not a true
		 *	memory reference is an error.
		 */
		if (TREE_CODE (arg) == CONSTRUCTOR && TREE_LITERAL (arg))
			/* vazio */;
		elif (typecode != FUNCTION_TYPE && !lvalue_or_else (arg, "unary `&'"))
			return (error_mark_node);

		/*
		 *	Ordinary case; arg is a COMPONENT_REF or a decl.
		 */
		argtype = TREE_TYPE (arg);

		if (TREE_READONLY (arg) || TREE_THIS_VOLATILE (arg))
		{
			argtype = c_build_type_variant
			(	argtype,
				TREE_READONLY (arg),
				TREE_THIS_VOLATILE (arg)
			);
		}

		argtype = build_pointer_type (argtype);

		if (mark_addressable (arg) == 0)
			return (error_mark_node);

		if (TREE_CODE (arg) == COMPONENT_REF)
		{
			tree field = TREE_OPERAND (arg, 1);

			addr = build_unary_op
			(	ADDR_EXPR, TREE_OPERAND (arg, 0), 0
			);

			if (TREE_PACKED (field))
			{
				error
				(	"attempt to take address of bit-field "
					"structure member `%s'",
					 IDENTIFIER_POINTER (DECL_NAME (field))
				);

				return (error_mark_node);
			}

			addr = convert (argtype, addr);

			if (DECL_OFFSET (field) != 0)
			{
				tree offset = build_int_2
				(	(DECL_OFFSET (field) / BITS_PER_UNIT),
					0
				);

				TREE_TYPE (offset) = argtype;

				addr = fold
				(	build
					(	PLUS_EXPR, argtype, addr, offset
					)	
				);
			}
		}
		else
		{
			addr = build (code, argtype, arg);
		}

		/*
		 *	Address of a static or external variable or
		 *	function counts as a constant
		 */
		TREE_LITERAL (addr) = staticp (arg);
		return (addr);

	    }

	}	/* end switch (code) */

	if (!errstring)
	{
		if (argtype == NULL_TREE)
			argtype = TREE_TYPE (arg);

		return (fold (build (code, argtype, arg)));
	}

	error (errstring);
	return (error_mark_node);

}	/* end build_unary_op */

/*
 ****************************************************************
 *	Build and return a conditional expression		*
 ****************************************************************
 */
/*
 *	Build and return a conditional expression IFEXP ? OP1 : OP2.
 */
tree
build_conditional_expr (tree ifexp, tree op1, tree op2)
{
	register tree		type1, type2;
	register enum tree_code	code1, code2;
	register tree		result_type = NULL_TREE;

	/*
	 *	If second operand is omitted, it is the same
	 *	as the first one; make sure it is calculated
	 *	only once.
	 */
	if (op1 == NULL_TREE)
	{
		if (pedantic)
		{
			warning
			(	"ANSI C forbids omitting the middle "
				"term of a ?: expression"
			);
		}
		ifexp = op1 = save_expr (ifexp);
	}

	ifexp = truthvalue_conversion (default_conversion (ifexp));

	if
	(	TREE_CODE (ifexp) == ERROR_MARK ||
		TREE_CODE (TREE_TYPE (op1)) == ERROR_MARK ||
		TREE_CODE (TREE_TYPE (op2)) == ERROR_MARK
	)
		return (error_mark_node);

#if (0)	/*************************************/
	/*
	 *	Produces wrong result if within sizeof!!!
	 */
	/*
	 *	Don't promote the operands separately if they
	 *	promote the same way. Return the unpromoted type
	 *	and let the combined value get promoted if necessary.
	 */
	if
	(	TREE_TYPE (op1) == TREE_TYPE (op2) &&
		TREE_CODE (TREE_TYPE (op1)) != ARRAY_TYPE &&
		TREE_CODE (TREE_TYPE (op1)) != ENUMERAL_TYPE &&
		TREE_CODE (TREE_TYPE (op1)) != FUNCTION_TYPE
	)
	{
		if
		(	TREE_LITERAL (ifexp) &&
			(TREE_CODE (ifexp) == INTEGER_CST ||
			TREE_CODE (ifexp) == ADDR_EXPR)
		)
			return (integer_zerop (ifexp) ? op2 : op1);

		return build (COND_EXPR, TREE_TYPE (op1), ifexp, op1, op2);
	}
#endif	/*************************************/

	/*
	 *	They don't match; promote them both and then
	 *	try to reconcile them.
	 */
	if (TREE_CODE (TREE_TYPE (op1)) != VOID_TYPE)
		op1 = default_conversion (op1);

	if (TREE_CODE (TREE_TYPE (op2)) != VOID_TYPE)
		op2 = default_conversion (op2);

	type1 = TREE_TYPE (op1); 	code1 = TREE_CODE (type1);
	type2 = TREE_TYPE (op2); 	code2 = TREE_CODE (type2);

	/*
	 *	Quickly detect the usual case where op1 and op2
	 *	have the same type after promotion.
	 */
	if (type1 == type2)
	{
		result_type = type1;
	}
	elif
	(	(code1 == INTEGER_TYPE || code1 == REAL_TYPE) &&
		(code2 == INTEGER_TYPE || code2 == REAL_TYPE)
	)
	{
		result_type = commontype (type1, type2);
	}
	elif (code1 == VOID_TYPE || code2 == VOID_TYPE)
	{
		if (pedantic && (code1 != VOID_TYPE || code2 != VOID_TYPE))
		{
			warning
			(	"ANSI C forbids conditional expr "
				"with only one void side"
			);
		}
		result_type = void_type_node;
	}
	elif (code1 == POINTER_TYPE && code2 == POINTER_TYPE)
	{
		if (comp_target_types (type1, type2))
		{
			result_type = commontype (type1, type2);
		}
		elif (integer_zerop (op1) && TREE_TYPE (type1) == void_type_node)
		{
			result_type = qualify_type (type2, type1);
		}
		elif (integer_zerop (op2) && TREE_TYPE (type2) == void_type_node)
		{
			result_type = qualify_type (type1, type2);
		}
		elif (TYPE_MAIN_VARIANT (TREE_TYPE (type1)) == void_type_node)
		{
			if (pedantic && TREE_CODE (type2) == FUNCTION_TYPE)
			{
				warning
				(	"ANSI C forbids conditional expr "
					"between `void *' and function pointer"
				);
			}
			result_type = qualify_type (type1, type2);
		}
		elif (TYPE_MAIN_VARIANT (TREE_TYPE (type2)) == void_type_node)
		{
			if (pedantic && TREE_CODE (type1) == FUNCTION_TYPE)
			{
				warning
				(	"ANSI C forbids conditional expr "
					"between `void *' and function pointer"
				);
			}
			result_type = qualify_type (type2, type1);
		}
		else
		{
			warning
			(	"pointer type mismatch in conditional "
				"expression"
			);

			result_type = build_pointer_type (void_type_node);
		}
	}
	elif (code1 == POINTER_TYPE && code2 == INTEGER_TYPE)
	{
		if (!integer_zerop (op2))
		{
			warning
			(	"pointer/integer type mismatch in "
				"conditional expression"
			);
		}
		else
		{
			op2 = null_pointer_node;

#if (0)	/*************************************/
			/*
			 *	The spec seems to say this is permitted.
			 */
			if (pedantic && TREE_CODE (type1) == FUNCTION_TYPE)
			{
				warning
				(	"ANSI C forbids conditional expr "
					"between 0 and function pointer"
				);
			}
#endif	/*************************************/

		}
			result_type = type1;
	}
	elif (code2 == POINTER_TYPE && code1 == INTEGER_TYPE)
	{
		if (!integer_zerop (op1))
		{
			warning
			(	"pointer/integer type mismatch "
				"in conditional expression"
			);
		}
		else
		{
			op1 = null_pointer_node;

#if (0)	/*************************************/
			/*
			 *	The spec seems to say this is permitted.
			 */
			if (pedantic && TREE_CODE (type2) == FUNCTION_TYPE)
			{
				warning
				(	"ANSI C forbids conditional expr "
					"between 0 and function pointer"
				);
			}
#endif	/*************************************/
		}
			result_type = type2;
	}

	if (!result_type)
	{
		if (flag_cond_mismatch)
		{
			result_type = void_type_node;
		}
		else
		{
			error ("type mismatch in conditional expression");
			return error_mark_node;
		}
	}

	if (result_type != TREE_TYPE (op1))
		op1 = convert (result_type, op1);

	if (result_type != TREE_TYPE (op2))
		op2 = convert (result_type, op2);

#if (0)	/*************************************/
	if (code1 == RECORD_TYPE || code1 == UNION_TYPE)
	{
		result_type = TREE_TYPE (op1);

		if (TREE_LITERAL (ifexp))
			return (integer_zerop (ifexp) ? op2 : op1);

		if (TYPE_MODE (result_type) == BLKmode)
		{
			register tree	tempvar = build_decl
					(VAR_DECL, NULL_TREE, result_type);
			register tree	xop1 = build_modify_expr (tempvar, op1);
			register tree	xop2 = build_modify_expr (tempvar, op2);
			register tree	result = build
				(COND_EXPR, result_type, ifexp, xop1, xop2);

			layout_decl (tempvar);

			/*
			 *	No way to handle variable-sized objects here.
			 *	I fear that the entire handling of BLKmode
			 *	conditional exprs needs to be redone.
			 */
			if (! TREE_LITERAL (DECL_SIZE (tempvar)))
				abort ();

			DECL_RTL (tempvar) = assign_stack_local
			(	DECL_MODE (tempvar),
				(TREE_INT_CST_LOW (DECL_SIZE (tempvar))
				 * DECL_SIZE_UNIT (tempvar)
				 + BITS_PER_UNIT - 1)
				/ BITS_PER_UNIT
			);

			TREE_VOLATILE (result) = TREE_VOLATILE (ifexp) |
				TREE_VOLATILE (op1) | TREE_VOLATILE (op2);

			return
			(	build
				(	COMPOUND_EXPR, result_type,
					result, tempvar
				)
			);
		}
	}
#endif	/*************************************/

	if (TREE_CODE (ifexp) == INTEGER_CST)
		return (integer_zerop (ifexp) ? op2 : op1);

	return (build (COND_EXPR, result_type, ifexp, op1, op2));

}	/* end build_conditional_expr */

/*
 ****************************************************************
 *	Build a compound expression				*
 ****************************************************************
 */
/*
 *	Given a list of expressions, return a compound
 *	expression that performs them all and returns the
 *	value of the last of them.
 */
tree
build_compound_expr (tree list)
{
	register tree	rest;

	/*
	 *	build_c_cast puts on a NOP_EXPR to make the
	 *	result not an lvalue. Strip such NOP_EXPRs,
	 *	since LIST is used in non-lvalue context.
	 */
	if (TREE_CHAIN (list) == NULL_TREE)
	{
		if
		(	TREE_CODE (list) == NOP_EXPR &&
			TREE_TYPE (list) == TREE_TYPE (TREE_OPERAND (list, 0))
		)
			list = TREE_OPERAND (list, 0);

		return (TREE_VALUE (list));
	}

	rest = build_compound_expr (TREE_CHAIN (list));

	/*
	 *	This is patched out so that sizeof (0, array) is
	 *	distinguishable from sizeof array.
	 */
#if (0)	/*************************************/
	if (! TREE_VOLATILE (TREE_VALUE (list)))
		return (rest);
#endif	/*************************************/

	return build (COMPOUND_EXPR, TREE_TYPE (rest), TREE_VALUE (list), rest);

}	/* end build_compound_expr */

/*
 ****************************************************************
 *	Build a cast to type TYPE of expression EXPR		*
 ****************************************************************
 */
/*
 *	Build an expression representing a cast to
 *	type TYPE of expression EXPR.
 */
tree
build_c_cast (register tree type, tree expr)
{
	register tree	value = expr;

	if (type == error_mark_node || expr == error_mark_node)
		return (error_mark_node);

	type = TYPE_MAIN_VARIANT (type);

	/*
	 *	build_c_cast puts on a NOP_EXPR to make
	 *	the result not an lvalue. Strip such NOP_EXPRs,
	 *	since VALUE is being used in non-lvalue context.
	 */
	if
	(	TREE_CODE (value) == NOP_EXPR &&
		TREE_TYPE (value) == TREE_TYPE (TREE_OPERAND (value, 0))
	)
		value = TREE_OPERAND (value, 0);

	if (TREE_CODE (type) == ARRAY_TYPE)
	{
		error ("cast specifies array type");
		return (error_mark_node);
	}

	if (type == TREE_TYPE (value))
	{
		if (pedantic)
		{
			if
			(	TREE_CODE (type) == RECORD_TYPE ||
				TREE_CODE (type) == UNION_TYPE
			)
			{
				warning
				(	"ANSI C forbids casting nonscalar "
					"to the same type"
				);
			}
		}
	}
	else
	{
		tree otype;

		/*
		 *	Convert functions and arrays to pointers,
	 	 *	but don't convert any other types.
		 */
		if
		(	TREE_CODE (TREE_TYPE (value)) == FUNCTION_TYPE ||
			TREE_CODE (TREE_TYPE (value)) == ARRAY_TYPE
		)
			value = default_conversion (value);

		otype = TREE_TYPE (value);

		/*
		 *	Optionally warn about potentially worrysome casts.
		 */
		if
		(	warn_cast_qual &&
			TREE_CODE (type) == POINTER_TYPE &&
			TREE_CODE (otype) == POINTER_TYPE
		)
		{
			if
			(	TREE_VOLATILE (TREE_TYPE (otype)) &&
				! TREE_VOLATILE (TREE_TYPE (type))
			)
			{
				warning
				(	"cast discards `volatile' from "
					"pointer target type"
				);
			}

			if
			(	TREE_READONLY (TREE_TYPE (otype)) &&
				! TREE_READONLY (TREE_TYPE (type))
			)
			{
				warning
				(	"cast discards `const' from "
					"pointer target type"
				);
			}
		}

		value = convert (type, value);

	}

	/*
	 *	Always produce some operator for an explicit cast,
	 *	so we can tell (for -pedantic) that the cast is no lvalue.
	 */
	if (value == expr)
	{
		tree	nvalue = build (NOP_EXPR, type, value);

		TREE_LITERAL (nvalue) = TREE_LITERAL (value);
		return (nvalue);
	}

	return (value);

}	/* end build_c_cast */

/*
 ****************************************************************
 *	Build an assignment expression				*
 ****************************************************************
 */
/*
 *	Build an assignment expression of lvalue LHS
 *	from value RHS. MODIFYCODE is the code for a binary
 *	operator that we use to combine the old value of LHS
 *	with RHS to get the new value. Or else MODIFYCODE is
 *	NOP_EXPR meaning do a simple assignment.
 */
tree
build_modify_expr (tree lhs, enum tree_code modifycode, tree rhs)
{
	register tree	result;
	tree		newrhs;
	tree		lhstype = TREE_TYPE (lhs);
	tree		olhstype = lhstype;

	/*
	 *	Types that aren't fully specified cannot
	 *	be used in assignments.
	 */
	lhs = require_complete_type (lhs);

	/*
	 *	Avoid duplicate error messages from operands that had errors.
	 */
	if (TREE_CODE (lhs) == ERROR_MARK || TREE_CODE (rhs) == ERROR_MARK)
		return (error_mark_node);

	/*
	 *	build_c_cast puts on a NOP_EXPR to make the result
	 *	not an lvalue. Strip such NOP_EXPRs, since RHS is
 	 *	being used in non-lvalue context.
	 */
	if
	(	TREE_CODE (rhs) == NOP_EXPR &&
		TREE_TYPE (rhs) == TREE_TYPE (TREE_OPERAND (rhs, 0))
	)
		rhs = TREE_OPERAND (rhs, 0);

	newrhs = rhs;

	/*
	 *	Handle control structure constructs used as "lvalues".
	 */
	if (!pedantic) switch (TREE_CODE (lhs))
	{
		/*
		 *	Handle (a, b) used as an "lvalue".
		 */
	    case COMPOUND_EXPR:
		return
		(	build
			(	COMPOUND_EXPR,
				lhstype,
				TREE_OPERAND (lhs, 0),
				build_modify_expr
				(	TREE_OPERAND (lhs, 1),
					modifycode, rhs
				)
			)
		);

		/*
		 *	Handle (a ? b : c) used as an "lvalue".
		 */
	    case COND_EXPR:
	    {
		tree	cond;

		rhs = save_expr (rhs);

		/*
		 *	Produce (a ? (b = rhs) : (c = rhs))
		 *	except that the RHS goes through a
		 *	save-expr so the code to compute it
		 *	is only emitted once.
		 */
		cond = build_conditional_expr
		(	TREE_OPERAND (lhs, 0),
			build_modify_expr
				(TREE_OPERAND (lhs, 1), modifycode, rhs),
			build_modify_expr
				(TREE_OPERAND (lhs, 2), modifycode, rhs)
		);

		/*
		 *	Make sure the code to compute the rhs comes out
		 *	before the split.
		 *
		 *	Cast to void to suppress warning
		 *	from warn_if_unused_value.
		 */
		return
		(	build
			(	COMPOUND_EXPR, TREE_TYPE (lhs),
				convert (void_type_node, rhs),
				cond
			)
		);
	    }

 	}	/* end if (!pedantic) switch (TREE_CODE (lhs)) */

	/*
	 *	If a binary op has been requested, combine the
	 *	old LHS value with the RHS producing the value
	 *	we should actually store into the LHS.
	 */
	if (modifycode != NOP_EXPR)
	{
		lhs = stabilize_reference (lhs);
		newrhs = build_binary_op (modifycode, lhs, rhs);
	}

	/*
	 *	Handle a cast used as an "lvalue". We have already
	 *	performed any binary operator using the value as cast.
	 *	Now convert the result to the true type of the lhs
	 *	and store there; then cast the result back to the
	 *	specified type to be the value of the assignment.
	 */
	if (!pedantic) switch (TREE_CODE (lhs))
	{
	    case NOP_EXPR:
	    case CONVERT_EXPR:
	    case FLOAT_EXPR:
	    case FIX_TRUNC_EXPR:
	    case FIX_FLOOR_EXPR:
	    case FIX_ROUND_EXPR:
	    case FIX_CEIL_EXPR:
	    {
		tree	inner_lhs, result;

		if
		(	TREE_CODE (TREE_TYPE (newrhs)) == ARRAY_TYPE ||
			TREE_CODE (TREE_TYPE (newrhs)) == FUNCTION_TYPE
		)
			newrhs = default_conversion (newrhs);

		inner_lhs = TREE_OPERAND (lhs, 0);

		result = build_modify_expr
		(	inner_lhs, NOP_EXPR,
			convert (TREE_TYPE (inner_lhs), newrhs)
		);

		return (convert (TREE_TYPE (lhs), result));
	    }

	}	 /* end if (!pedantic) switch (TREE_CODE (lhs)) */

	/*
	 *	Now we have handled acceptable kinds of LHS that
	 *	are not truly lvalues. Reject anything strange now.
	 */
	if (!lvalue_or_else (lhs, "assignment"))
		return (error_mark_node);

	/*
	 *	Warn about storing in something that is `const'.
	 */
	if
	(	TREE_READONLY (lhs) || ((TREE_CODE (lhstype) == RECORD_TYPE ||
		TREE_CODE (lhstype) == UNION_TYPE) &&
					C_TYPE_FIELDS_READONLY (lhstype))
	)
		readonly_warning (lhs, "assignment");

	/*
	 *	If storing into a structure or union member, it has
	 *	probably been given type `int'. Compute the type that
	 *	would go with the actual amount of storage the member
	 *	occupies.
	 */
	if
	(	TREE_CODE (lhs) == COMPONENT_REF &&
				(TREE_CODE (lhstype) == INTEGER_TYPE ||
		TREE_CODE (lhstype) == REAL_TYPE ||
		TREE_CODE (lhstype) == ENUMERAL_TYPE)
	)
		lhstype = TREE_TYPE (get_unwidened (lhs, NULL_TREE));

	/*
	 *	If storing in a field that is in actuality a short
	 *	or narrower than one, we must store in the field in
	 *	its actual type.
	 */
	if (lhstype != TREE_TYPE (lhs))
	{
		lhs = copy_node (lhs);
		TREE_TYPE (lhs) = lhstype;
	}

	/*
	 *	Convert new value to destination type.
	 */
	newrhs = convert_for_assignment (lhstype, newrhs, "assignment");

	if (TREE_CODE (newrhs) == ERROR_MARK)
		return (error_mark_node);

	result = build (MODIFY_EXPR, lhstype, lhs, newrhs);

	TREE_VOLATILE (result) = 1;

	/*
	 *	If we got the LHS in a different type for storing in,
	 *	convert the result back to the nominal type of LHS
	 *	so that the value we return always has the same type
	 *	as the LHS argument.
	 */
	if (olhstype == TREE_TYPE (result))
		return (result);

	return (convert_for_assignment (olhstype, result, "assignment"));

}	/* end build_modify_expr */
