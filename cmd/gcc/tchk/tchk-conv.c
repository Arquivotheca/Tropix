/*
 ****************************************************************
 *								*
 *			tchk/tchk-conv.c			*
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
 *		3.0.0, de 19.10.93				*
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
#include "../h/tree.h"
#include "../h/c-tree.h"
#include "../h/flags.h"

#define	NULL	0
#define	EVER	;;
#define	elif	else if

/*
 ******	Protótipos de funções ***********************************
 */
extern tree	build (enum tree_code, ...);
extern tree	build_binary_op_nodefault (enum tree_code, tree, tree, enum tree_code);
extern tree	build_pointer_type (tree);
extern tree	build_unary_op (enum tree_code, tree, int);
extern tree	c_build_type_variant (tree, int, int);
extern tree	commontype (tree, tree);
extern int	compparms (tree, tree);
extern int	compparms1 (tree);
extern tree	convert (tree, tree);
extern tree	convert_for_assignment (tree, tree, char *);
extern tree	decl_constant_value (tree);
extern tree	digest_init (tree, tree, tree *);
extern void	error (const char *, ...);
extern tree	get_narrower (tree, int *);
extern int	initializer_constant_valid_p (tree);
extern int	integer_zerop (tree);
extern int	lvalue_p (tree);
extern int	mark_addressable (tree);
extern tree	require_complete_type (tree);
extern tree	signed_or_unsigned_type (int, tree);
extern tree	signed_type (tree);
extern int	staticp (tree);
extern tree	type_for_size (int, int);
extern tree	unsigned_type (tree);
extern void 	warning	(char *, ...);

/*
 ****************************************************************
 *	Return 1 if the types are compatible for assignment	*
 ****************************************************************
 */
/*
 *	Return 1 if TYPE1 and TYPE2 are compatible types
 *	for assignment or various other operations.
 *	This is what ANSI C speaks of as "being the same".
 */
int
comptypes (tree type1, tree type2)
{
	register tree	t1 = type1;
	register tree	t2 = type2;

	/*
	 *	Suppress errors caused by previously reported errors
	 */
	if
	(	t1 == t2 ||
		TREE_CODE (t1) == ERROR_MARK ||
		TREE_CODE (t2) == ERROR_MARK
	)
		return (1);

	/*
	 *	Treat an enum type as the unsigned integer
	 *	type of the same width.
	 */
	if (TREE_CODE (t1) == ENUMERAL_TYPE)
		t1 = type_for_size (TYPE_PRECISION (t1), 1);

	if (TREE_CODE (t2) == ENUMERAL_TYPE)
		t2 = type_for_size (TYPE_PRECISION (t2), 1);

	if (t1 == t2)
		return (1);

	/*
	 *	Different classes of types can't be compatible.
	 */
	if (TREE_CODE (t1) != TREE_CODE (t2))
		return (0);

	/*
	 *	Qualifiers must match.
	 */
	if (TREE_READONLY (t1) != TREE_READONLY (t2))
		return (0);

	if (TREE_THIS_VOLATILE (t1) != TREE_THIS_VOLATILE (t2))
		return (0);

	switch (TREE_CODE (t1))
	{
	    case POINTER_TYPE:
		return
		(	TREE_TYPE (t1) == TREE_TYPE (t2) ||
			comptypes (TREE_TYPE (t1), TREE_TYPE (t2))
		);

	    case FUNCTION_TYPE:
		return
		(	(TREE_TYPE (t1) == TREE_TYPE (t2) ||
			comptypes (TREE_TYPE (t1), TREE_TYPE (t2))) &&
			  compparms (TYPE_ARG_TYPES (t1), TYPE_ARG_TYPES (t2))
		);

		/*
		 *	Target types must match incl. qualifiers.
		 */
	    case ARRAY_TYPE:
	    {
		tree d1, d2;

		if
		(	!(TREE_TYPE (t1) == TREE_TYPE (t2) ||
			comptypes (TREE_TYPE (t1), TREE_TYPE (t2)))
		)
			return 0;

		d1 = TYPE_DOMAIN (t1);
		d2 = TYPE_DOMAIN (t2);

		/*
		 *	Sizes must match unless one is missing or variable.
		 */
		if
		(	d1 == NULL_TREE || d2 == NULL_TREE || d1 == d2 ||
			TREE_CODE (TYPE_MIN_VALUE (d1)) != INTEGER_CST ||
			TREE_CODE (TYPE_MIN_VALUE (d2)) != INTEGER_CST ||
			TREE_CODE (TYPE_MAX_VALUE (d1)) != INTEGER_CST ||
			TREE_CODE (TYPE_MAX_VALUE (d2)) != INTEGER_CST
		)
			return (1);

		return
		(	(TREE_INT_CST_LOW (TYPE_MIN_VALUE (d1)) ==
				 TREE_INT_CST_LOW (TYPE_MIN_VALUE (d2))) &&
			(TREE_INT_CST_HIGH (TYPE_MIN_VALUE (d1)) ==
				TREE_INT_CST_HIGH (TYPE_MIN_VALUE (d2))) &&
			(TREE_INT_CST_LOW (TYPE_MAX_VALUE (d1)) ==
				TREE_INT_CST_LOW (TYPE_MAX_VALUE (d2))) &&
			(TREE_INT_CST_HIGH (TYPE_MAX_VALUE (d1)) ==
				TREE_INT_CST_HIGH (TYPE_MAX_VALUE (d2)))
		);
	    }

	}	/* end	switch (TREE_CODE (t1)) */

	return (0);

}	/* end comptypes */

/*
 ****************************************************************
 *	Compare types ignoring their qualifiers			*
 ****************************************************************
 */
/*
 *	Return 1 if TTL and TTR are pointers to types that
 *	are equivalent, ignoring their qualifiers.
 */
int
comp_target_types (tree ttl, tree ttr)
{
	return
	(	comptypes
		(	TYPE_MAIN_VARIANT (TREE_TYPE (ttl)),
			TYPE_MAIN_VARIANT (TREE_TYPE (ttr))
		)
	);

}	/* end comp_target_types */

/*
 ****************************************************************
 *	Compare parameters					*
 ****************************************************************
 */
/*
 *	Return 1 if two parameter type lists PARMS1 and PARMS2
 *	are equivalent in the sense that functions with those
 *	parameter types can have equivalent types. If either
 *	list is empty, we win. Otherwise, the two lists must
 *	be equivalent, element by element.
 */
int
compparms (tree parms1, tree parms2)
{
	register tree	t1 = parms1, t2 = parms2;

	/*
	 *	An unspecified parmlist matches any specified
	 *	parmlist whose argument types don't need
	 *	default promotions.
	 */
	if (t1 == NULL_TREE)
		return (compparms1 (t2));

	if (t2 == NULL_TREE)
		return (compparms1 (t1));

	for (EVER)
	{
		if (t1 == NULL_TREE && t2 == NULL_TREE)
			return (1);

		/*
		 *	If one parmlist is shorter than the other,
	 	 *	they fail to match.
		 */
		if (t1 == NULL_TREE || t2 == NULL_TREE)
			return (0);

		if (! comptypes (TREE_VALUE (t1), TREE_VALUE (t2)))
			return (0);

		t1 = TREE_CHAIN (t1);
		t2 = TREE_CHAIN (t2);
	}

}	/* end compparms */

/*
 ****************************************************************
 *  Return 1 if PARMS specifies a fixed number of parameters	*
 ****************************************************************
 */
/*
 *	Return 1 if PARMS specifies a fixed number of
 *	parameters and none of their types is affected
 *	by default promotions.
 */
int
compparms1 (tree parms)
{
	register tree	t;

	for (t = parms; t != NULL_TREE; t = TREE_CHAIN (t))
	{
		register tree	type = TREE_VALUE (t);

		if (TREE_CHAIN (t) == NULL_TREE && type != void_type_node)
			return (0);

		if (type == float_type_node)
			return (0);

		if
		(	TREE_CODE (type) == INTEGER_TYPE &&
			TYPE_PRECISION (type) <
					TYPE_PRECISION (integer_type_node)
		)
			return (0);
	}

	return (1);

}	/* end compparms1 */

/*
 ****************************************************************
 *  Perform default promotions for C data used in expressions	*
 ****************************************************************
 */
/*
 *	Perform default promotions for C data used in
 *	expressions. Arrays and functions are converted
 *	to pointers; enumeral types or short or char, to int.
 *	In addition, manifest constants symbols are replaced
 *	by their values.
 */
tree
default_conversion (tree exp)
{
	register tree		dt = TREE_TYPE (exp);
	register enum tree_code form = TREE_CODE (dt);
	register tree		adr;
	tree			restype, ptrtype;

	/*
	 *	Replace a nonvolatile const static variable with its value.
	 */
	if (TREE_CODE (exp) == CONST_DECL)
	{
		exp = DECL_INITIAL (exp);
	}
	elif (optimize && TREE_CODE (exp) == VAR_DECL && TREE_READONLY (exp))
	{
		exp = decl_constant_value (exp);
	}

	/*
	 *	build_c_cast puts on a NOP_EXPR to make the
	 *	result not an lvalue. Strip such NOP_EXPRs,
	 *	since EXP is being used in non-lvalue context.
	 */
	if
	(	TREE_CODE (exp) == NOP_EXPR &&
		TREE_TYPE (exp) == TREE_TYPE (TREE_OPERAND (exp, 0))
	)
		exp = TREE_OPERAND (exp, 0);

	/*
	 *	Traditionally, unsignedness is preserved
	 *	in default promotions.
	 */
	if
	(	form == ENUMERAL_TYPE ||
		(form == INTEGER_TYPE &&
		  (TYPE_PRECISION (dt) < TYPE_PRECISION (integer_type_node)))
	)
	{
		if (flag_traditional && TREE_UNSIGNED (dt))
			return (convert (unsigned_type_node, exp));

		return (convert (integer_type_node, exp));
	}

	if (flag_traditional && dt == float_type_node)
		return (convert (double_type_node, exp));

	if (form == VOID_TYPE)
	{
		error ("void value not ignored as it ought to be");
		return (error_mark_node);
	}

	if (form == FUNCTION_TYPE)
	{
		return (build_unary_op (ADDR_EXPR, exp, 0));
	}

	if (form != ARRAY_TYPE)
		return (exp);

	/*
	 *	form == ARRAY_TYPE
	 */
	restype = TREE_TYPE (dt);

	if (TREE_CODE (exp) == INDIRECT_REF)
	{
		return
		(	convert
			(	TYPE_POINTER_TO (restype),
				TREE_OPERAND (exp, 0)
			)	
		);
	}

	if (TREE_CODE (exp) == COMPOUND_EXPR)
	{
		tree	op1 = default_conversion (TREE_OPERAND (exp, 1));

		return
		(	build
			(	COMPOUND_EXPR, TREE_TYPE (op1),
				TREE_OPERAND (exp, 0), op1
			)
		);
	}

	if
	(	!lvalue_p (exp) &&
		! (TREE_CODE (exp) == CONSTRUCTOR && TREE_STATIC (exp))
	)
	{
		error ("invalid use of non-lvalue array");
		return (error_mark_node);
	}

	if (TREE_READONLY (exp) || TREE_THIS_VOLATILE (exp))
	{
		restype = c_build_type_variant
		(	restype, TREE_READONLY (exp),
			TREE_THIS_VOLATILE (exp)
		);
	}

	ptrtype = build_pointer_type (restype);

	/*
	 *	??? This is not really quite correct in that
	 *	the type of the operand of ADDR_EXPR is not
	 *	the target type of the type of the ADDR_EXPR
	 *	itself. Question is, can this lossage be avoided?
	 */
	if (TREE_CODE (exp) == VAR_DECL)
	{
		adr = build (ADDR_EXPR, ptrtype, exp);

		if (mark_addressable (exp) == 0)
			return (error_mark_node);

		TREE_LITERAL (adr) = staticp (exp);
		TREE_VOLATILE (adr) = 0; /* Default would be, same as EXP. */

		return (adr);
	}

	/*
	 *	This way is better for a COMPONENT_REF since it can
	 *	simplify the offset for a component.
	 */
	adr = build_unary_op (ADDR_EXPR, exp, 1);

	return (convert (ptrtype, adr));

}	/* end default_conversion */

/*
 ****************************************************************
 *	Compare operations					*
 ****************************************************************
 */
/*
 *	Subroutine of "build_binary_op_nodefault", used for
 *	comparison operations. See if the operands have both
 *	been converted from subword integer types and, if so,
 *	perhaps change them both back to their original type.
 *
 *	The arguments of this function are all pointers to
 *	local variables of build_binary_op_nodefault: OP0_PTR
 *	is &OP0, OP1_PTR is &OP1, RESTYPE_PTR is &RESULT_TYPE
 *	and RESCODE_PTR is &RESULTCODE.
 *
 *	If this function returns nonzero, it means that the
 *	comparison has a constant value.  What this function
 *	returns is an expression for that value.
 */
tree
shorten_compare (tree *op0_ptr, tree *op1_ptr,
			tree *restype_ptr, enum tree_code *rescode_ptr)
{
	register tree	type;
	tree		op0 = *op0_ptr;
	tree		op1 = *op1_ptr;
	int		unsignedp0, unsignedp1;
	int		real1, real2;
	tree		primop0, primop1;
	enum tree_code	code = *rescode_ptr;

	/*
	 *	Throw away any conversions to wider types
	 *	already present in the operands.
	 */
	primop0 = get_narrower (op0, &unsignedp0);
	primop1 = get_narrower (op1, &unsignedp1);

	/*
	 *	Handle the case that OP0 does not *contain*
	 *	a conversion but it *requires* conversion to
	 *	FINAL_TYPE.
	 */
	if (op0 == primop0 && TREE_TYPE (op0) != *restype_ptr)
		unsignedp0 = TREE_UNSIGNED (TREE_TYPE (op0));

	if (op1 == primop1 && TREE_TYPE (op1) != *restype_ptr)
		unsignedp1 = TREE_UNSIGNED (TREE_TYPE (op1));

	/*
	 *	If one of the operands must be floated,
	 *	we cannot optimize.
	 */
	real1 = TREE_CODE (TREE_TYPE (primop0)) == REAL_TYPE;
	real2 = TREE_CODE (TREE_TYPE (primop1)) == REAL_TYPE;

	/*
	 *	If first arg is constant, swap the args
	 *	(changing operation so value is preserved),
	 *	for canonicalization.
	 */
	if (TREE_LITERAL (primop0))
	{
		register tree	tem = primop0;
		register int	temi = unsignedp0;

		primop0 = primop1; 		primop1 = tem;

		tem = op0; 	op0 = op1; 	op1 = tem;

		*op0_ptr = op0; 		*op1_ptr = op1;

		unsignedp0 = unsignedp1; 	unsignedp1 = temi;

		temi = real1; 	real1 = real2; 	real2 = temi;

		switch (code)
		{
		    case LT_EXPR:
			code = GT_EXPR;
			break;

		    case GT_EXPR:
			code = LT_EXPR;
			break;

		    case LE_EXPR:
			code = GE_EXPR;
			break;

		    case GE_EXPR:
			code = LE_EXPR;
			break;

		}	/* end switch (code) */

		*rescode_ptr = code;
	}

	/*
	 *	If comparing an integer against a constant
	 *	more bits wide, maybe we can deduce a value
	 *	of 1 or 0 independent of the data. Or else
	 *	truncate the constant now rather than extend
	 *	the variable at run time.
	 *
	 *	This is only interesting if the constant is
	 *	the wider arg. Also, it is not safe if the
	 *	constant is unsigned and the variable arg is
	 *	signed, since in this case the variable would
	 *	be sign-extended and then regarded as unsigned.
	 *	Our technique fails in this case because the
	 *	lowest/highest possible unsigned results don't
	 *	follow naturally from the lowest/highest possible
	 *	values of the variable operand. For just EQ_EXPR
	 *	and NE_EXPR there is another technique that could
	 *	be used: see if the constant can be faithfully
	 *	represented in the other operand's type, by
	 *	truncating it and reextending it and see if
	 *	that preserves the constant's value.
	 */

	if
	(	!real1 && !real2 &&
		TREE_CODE (primop1) == INTEGER_CST &&
		TYPE_PRECISION (TREE_TYPE (primop0)) <
				TYPE_PRECISION (*restype_ptr)
	)
	{
		int	min_gt, max_gt, min_lt, max_lt;
		tree	maxval, minval;

		/*
		 *	1 if comparison is nominally unsigned.
		 */
		int	unsignedp = TREE_UNSIGNED (*restype_ptr);
		tree	val;

		type = signed_or_unsigned_type (unsignedp0, TREE_TYPE (primop0));

		maxval = TYPE_MAX_VALUE (type);
		minval = TYPE_MIN_VALUE (type);

		if (unsignedp && !unsignedp0)
			*restype_ptr = signed_type (*restype_ptr);

		if (TREE_TYPE (primop1) != *restype_ptr)
			primop1 = convert (*restype_ptr, primop1);

		if (type != *restype_ptr)
		{
			minval = convert (*restype_ptr, minval);
			maxval = convert (*restype_ptr, maxval);
		}

		if (unsignedp && unsignedp0)
		{
			min_gt = INT_CST_LT_UNSIGNED (primop1, minval);
			max_gt = INT_CST_LT_UNSIGNED (primop1, maxval);
			min_lt = INT_CST_LT_UNSIGNED (minval, primop1);
			max_lt = INT_CST_LT_UNSIGNED (maxval, primop1);
		}
		else
		{
			min_gt = INT_CST_LT (primop1, minval);
			max_gt = INT_CST_LT (primop1, maxval);
			min_lt = INT_CST_LT (minval, primop1);
			max_lt = INT_CST_LT (maxval, primop1);
		}

		val = NULL_TREE;

		/*
	 	 *	This used to be a switch, but Genix compiler
		 *	can't handle that.
		 */
		if   (code == NE_EXPR)
		{
			if (max_lt || min_gt)
				val = integer_one_node;
		}
		elif (code == EQ_EXPR)
		{
			if (max_lt || min_gt)
				val = integer_zero_node;
		}
		elif (code == LT_EXPR)
		{
			if (max_lt)
				val = integer_one_node;

			if (!min_lt)
				val = integer_zero_node;
		}
		elif (code == GT_EXPR)
		{
			if (min_gt)
				val = integer_one_node;

			if (!max_gt)
				val = integer_zero_node;
		}
		elif (code == LE_EXPR)
		{
			if (!max_gt)
				val = integer_one_node;

			if (min_gt)
				val = integer_zero_node;
		}
		elif (code == GE_EXPR)
		{
			if (!min_lt)
				val = integer_one_node;

			if (max_lt)
				val = integer_zero_node;
		}

		/*
		 *	If primop0 was sign-extended and unsigned
		 *	comparison specd, we did a signed comparison
		 *	above using the signed type bounds. But the
		 *	comparison we output must be unsigned.
		 *
	 	 *	Also, for inequalities, VAL is no good; but
		 *	if the signed comparison had *any* fixed
		 *	result, it follows that the unsigned comparison
		 *	just tests the sign in reverse (positive values
		 *	are LE, negative ones GE). So we can generate
		 *	an unsigned comparison against an extreme value
		 *	of the signed type.
		 */
		if (unsignedp && !unsignedp0)
		{
			if (val != NULL_TREE) switch (code)
			{
			    case LT_EXPR:
			    case GE_EXPR:
				primop1 = TYPE_MIN_VALUE (type);
				val = NULL_TREE;
				break;

			    case LE_EXPR:
			    case GT_EXPR:
				primop1 = TYPE_MAX_VALUE (type);
				val = NULL_TREE;
				break;
			}

			type = unsigned_type (type);
		}

		/*
		 *	This is the case of (char)x >?< 0x80,
		 *	which people used to use expecting old
		 *	C compilers to change the 0x80 into -0x80.
		 */
		if (max_lt && !unsignedp0)
		{
			if (val == integer_zero_node)
			{
				warning
				(	"comparison is always 0 due "
					"to limited range of data type"
				);
			}

			if (val == integer_one_node)
			{
				warning
				(	"comparison is always 1 due "
					"to limited range of data type"
				);
			}
		}

		/*
		 *	Don't forget to evaluate PRIMOP0 if
		 *	it has side effects.
		 */
		if (val != NULL_TREE)
		{
			if (TREE_VOLATILE (primop0))
			{
				return
				(	build
					(	COMPOUND_EXPR,
						TREE_TYPE (val),
						primop0,
						val
					)
				);
			}

			return (val);
		}
	}
	elif
	(	real1 && real2 &&
		TYPE_PRECISION (TREE_TYPE (primop0)) ==
				TYPE_PRECISION (TREE_TYPE (primop1))
	)
	{
		/*
		 *	Value is not predetermined, but do
		 *	the comparison in the type of the
		 *	operand that is not constant. TYPE
		 *	is already properly set.
		 */
		type = TREE_TYPE (primop0);
	}
	elif
	(	unsignedp0 == unsignedp1 && real1 == real2 &&
		TYPE_PRECISION (TREE_TYPE (primop0)) <
				TYPE_PRECISION (*restype_ptr) &&
		TYPE_PRECISION (TREE_TYPE (primop1)) <
				TYPE_PRECISION (*restype_ptr)
	)
	{
		/*
		 *	If args' natural types are both narrower
		 *	than nominal type and both extend in the
		 *	same manner, compare them in the type of
		 *	the wider arg. Otherwise must actually
		 *	extend both to the nominal common type
		 *	lest different ways of extending alter
		 *	the result. (eg,
		 *	    (short)-1 == (unsigned short)-1
		 *	should be 0.)
		 */
		type = commontype (TREE_TYPE (primop0), TREE_TYPE (primop1));
		type = signed_or_unsigned_type
			(unsignedp0 || TREE_UNSIGNED (*restype_ptr), type);

		/*
		 *	Make sure shorter operand is extended the
		 *	right way to match the longer operand.
		 */
		primop0 = convert
		(	signed_or_unsigned_type
			(	unsignedp0,
				TREE_TYPE (primop0)
			),
			primop0
		);

		primop1 = convert
		(	signed_or_unsigned_type
			(	unsignedp1,
				TREE_TYPE (primop1)
			),
			primop1
		);
	}
	else
	{
		/*
		 *	Here we must do the comparison on the
		 *	nominal type using the args exactly as
		 *	we received them.
		 */
		type = *restype_ptr;
		primop0 = op0;
		primop1 = op1;
	}

	*op0_ptr = convert (type, primop0);
	*op1_ptr = convert (type, primop1);

	*restype_ptr = integer_type_node;

	return NULL_TREE;

}	/* end shorten_compare */

/*
 ****************************************************************
 *	convert with the same conversions in the same order	*
 ****************************************************************
 */
/*
 *	If CONVERSIONS is a conversion expression or a nested
 *	sequence of such, convert ARG with the same conversions
 *	in the same order and return the result.
 */
tree
convert_sequence (tree conversions, tree arg)
{
	switch (TREE_CODE (conversions))
	{
	    case NOP_EXPR:
	    case CONVERT_EXPR:
	    case FLOAT_EXPR:
	    case FIX_TRUNC_EXPR:
	    case FIX_FLOOR_EXPR:
	    case FIX_ROUND_EXPR:
	    case FIX_CEIL_EXPR:
		return
		(	convert
			(	TREE_TYPE (conversions),
				convert_sequence
				(	TREE_OPERAND (conversions, 0),
					arg
				)
			)
		);

	    default:
		return (arg);
	}

}	/* end convert_sequence */

/*
 ****************************************************************
 *	Prepare expr to be an argument of a TRUTH_NOT_EXPR	*
 ****************************************************************
 */
/*
 *	Prepare expr to be an argument of a TRUTH_NOT_EXPR,
 *	or validate its data type for an `if' or `while'
 *	statement or ?..: exp.
 *
 *	This preparation consists of taking the ordinary
 *	representation of an expression expr and producing
 *	a valid tree boolean expression describing whether
 *	expr is nonzero. We could simply always do
 *
 *		build_binary_op (NE_EXPR, expr, integer_zero_node),
 *
 *	but we optimize comparisons, &&, ||, and !
 */
tree
truthvalue_conversion (tree expr)
{
	register enum tree_code form;

	/*
	 *	build_c_cast puts on a NOP_EXPR to make
	 *	the result not an lvalue. Strip such
	 *	NOP_EXPRs, since EXPR is being used in
	 *	non-lvalue context.
	 */
	if
	(	TREE_CODE (expr) == NOP_EXPR &&
		TREE_TYPE (expr) == TREE_TYPE (TREE_OPERAND (expr, 0))
	)
		expr = TREE_OPERAND (expr, 0);

	form = TREE_CODE (expr);

	if (form == EQ_EXPR && integer_zerop (TREE_OPERAND (expr, 1)))
	{
		return
		(	build_unary_op
			(	TRUTH_NOT_EXPR, TREE_OPERAND (expr, 0), 0
			)
		);
	}

	/*
	 *	A one-bit unsigned bit-field is already acceptable.
	 */
	if
	(	form == COMPONENT_REF &&
		1 == TREE_INT_CST_LOW (DECL_SIZE (TREE_OPERAND (expr, 1))) &&
		1 == DECL_SIZE_UNIT (TREE_OPERAND (expr, 1)) &&
		TREE_UNSIGNED (TREE_OPERAND (expr, 1))
	)
		return (expr);

	if
	(	form == TRUTH_ANDIF_EXPR || form == TRUTH_ORIF_EXPR ||
		form == TRUTH_AND_EXPR || form == TRUTH_OR_EXPR ||
		form == TRUTH_NOT_EXPR || form == EQ_EXPR ||
		form == NE_EXPR || form == LE_EXPR || form == GE_EXPR ||
		form == LT_EXPR || form == GT_EXPR || form == ERROR_MARK
	)
		return (expr);

	/*
	 *	Unary minus has no effect on whether its argument is nonzero.
	 */
	if (form == NEGATE_EXPR)
		return (truthvalue_conversion (TREE_OPERAND (expr, 0)));

	/*
	 *	Distribute the conversion into the arms of a COND_EXPR.
	 */
	if (form == COND_EXPR)
	{
		return
		(	build
			(	COND_EXPR, integer_type_node,
				TREE_OPERAND (expr, 0),
				truthvalue_conversion (TREE_OPERAND (expr, 1)),
				truthvalue_conversion (TREE_OPERAND (expr, 2))
			)
		);
	}

	/*
	 *	Sign-extension and zero-extension has no effect.
	 */
	if
	(	form == NOP_EXPR &&
		TREE_CODE (TREE_TYPE (expr)) == INTEGER_TYPE &&
		(TYPE_PRECISION (TREE_TYPE (expr)) >
			TYPE_PRECISION (TREE_TYPE (TREE_OPERAND (expr, 0))))
	)
		return (truthvalue_conversion (TREE_OPERAND (expr, 0)));

	return
	(	build_binary_op_nodefault
		(	NE_EXPR,
			default_conversion (expr),
			integer_zero_node,
			NOP_EXPR
		)
	);

}	/* end truthvalue_conversion */

/*
 ****************************************************************
 *	Convert to a type as preparation for an assignment	*
 ****************************************************************
 */
/*
 *	Convert value RHS to type TYPE as preparation for
 *	an assignment to an lvalue of type TYPE. The real
 *	work of conversion is done by `convert'. The purpose
 *	of this function is to generate error messages for
 *	assignments that are not allowed in C. ERRTYPE is
 *	a string to use in error messages: "assignment",
 *	"return", etc.
 */
tree
convert_for_assignment (tree type, tree rhs, char *errtype)
{
	register enum tree_code	codel = TREE_CODE (type);
	register tree		rhstype;
	register enum tree_code	coder;

	/*
	 *	build_c_cast puts on a NOP_EXPR to make the
	 *	result not an lvalue. Strip such NOP_EXPRs,
	 *	since RHS is used in non-lvalue context.
	 */
	if
	(	TREE_CODE (rhs) == NOP_EXPR &&
		TREE_TYPE (rhs) == TREE_TYPE (TREE_OPERAND (rhs, 0))
	)
		rhs = TREE_OPERAND (rhs, 0);

	if
	(	TREE_CODE (TREE_TYPE (rhs)) == ARRAY_TYPE ||
		TREE_CODE (TREE_TYPE (rhs)) == FUNCTION_TYPE
	)
		rhs = default_conversion (rhs);

	rhstype = TREE_TYPE (rhs);
	coder = TREE_CODE (rhstype);

	if (coder == ERROR_MARK)
		return (error_mark_node);

	if (TYPE_MAIN_VARIANT (type) == TYPE_MAIN_VARIANT (rhstype))
		return (rhs);

	if (coder == VOID_TYPE)
	{
		error ("void value not ignored as it ought to be");
		return (error_mark_node);
	}

	/*
	 *	Analisa os vários casos
	 */
	if
	(	(codel == INTEGER_TYPE || codel == REAL_TYPE ||
							codel == ENUMERAL_TYPE)
			 &&
		(coder == INTEGER_TYPE || coder == REAL_TYPE ||
							coder == ENUMERAL_TYPE)
	)
	{
		/*
		 *	Arithmetic types all interconvert,
		 *	and enum is treated like int.
		 */
		return (convert (type, rhs));
	}
	elif (codel == POINTER_TYPE && coder == POINTER_TYPE)
	{
		/*
		 *	Conversions among pointers
		 */
		register tree	ttl = TREE_TYPE (type);
		register tree	ttr = TREE_TYPE (rhstype);

		/*
		 *	Any non-function converts to a [const][volatile]
		 *	void * and vice versa; otherwise, targets must
		 *	be the same. Meanwhile, the lhs target must have
		 *	all the qualifiers of the rhs.
		 */
		if
		(	TYPE_MAIN_VARIANT (ttl) == void_type_node ||
			TYPE_MAIN_VARIANT (ttr) == void_type_node ||
			comp_target_types (type, rhstype)
		)
		{
			if
			(   pedantic &&
			    (	(TYPE_MAIN_VARIANT (ttl) == void_type_node &&
					TREE_CODE (ttr) == FUNCTION_TYPE) ||
				(TYPE_MAIN_VARIANT (ttr) == void_type_node &&
					TREE_CODE (ttl) == FUNCTION_TYPE)
			    )
			)
			{
				warning
				(	"%s between incompatible pointer types",
					errtype
				);
			}
			else
			{
				if
				(	! TREE_READONLY (ttl) &&
					TREE_READONLY (ttr)
				)
				{
					warning
					(	"%s of non-const * pointer "
						"from const *", errtype
					);
				}

				if
				(	! TREE_VOLATILE (ttl) &&
					TREE_VOLATILE (ttr)
				)
				{
					warning
					(	"%s of non-volatile * pointer "
						"from volatile *", errtype
					);
				}
			}
		}
		else
		{
			warning
			(	"%s between incompatible pointer types",
				errtype
			);
		}

		return (convert (type, rhs));
	}
	elif (codel == POINTER_TYPE && coder == INTEGER_TYPE)
	{
		if (! integer_zerop (rhs))
		{
			warning
			(	"%s of pointer from integer lacks a cast",
				errtype
			);
			return (convert (type, rhs));
		}

		return (null_pointer_node);
	}
	elif (codel == INTEGER_TYPE && coder == POINTER_TYPE)
	{
		warning ("%s of integer from pointer lacks a cast", errtype);
		return (convert (type, rhs));
	}

	error ("incompatible types in %s", errtype);
	return (error_mark_node);

}	/* end convert_for_assignment */

/*
 ****************************************************************
 *	Perform conversions on the initial value of a variable	*
 ****************************************************************
 */
/*
 *	Perform appropriate conversions on the initial value
 *	of a variable, store it in the declaration DECL, and
 *	print any error messages that are appropriate. If the
 *	init is invalid, store an ERROR_MARK.
 */
void
store_init_value (tree decl, tree init)
{
	register tree	value, type;

	/*
	 *	If variable's type was invalidly declared, just ignore it.
	 */
	type = TREE_TYPE (decl);

	if (TREE_CODE (type) == ERROR_MARK)
		return;

	/*
	 *	Digest the specified initializer into an expression.
	 */
	value = digest_init (type, init, (tree *)0);

	/*
	 *	Store the expression if valid; else report error.
	 */
	if   (value == error_mark_node)
	{
		/* vazio */;
	}
	elif (TREE_STATIC (decl) && ! TREE_LITERAL (value))
	{
		error ("initializer for static variable is not constant");
		value = error_mark_node;
	}
	elif (TREE_STATIC (decl) && ! initializer_constant_valid_p (value))
	{
		error ("initializer for static variable uses complicated arithmetic");
		value = error_mark_node;
	}
	else
	{
		if (pedantic && TREE_CODE (value) == CONSTRUCTOR)
		{
			if (! TREE_LITERAL (value))
				warning ("aggregate initializer is not constant");
			elif (! TREE_STATIC (value))
				warning ("aggregate initializer uses complicated arithmetic");
		}
	}

	DECL_INITIAL (decl) = value;

}	/* end store_init_value */
