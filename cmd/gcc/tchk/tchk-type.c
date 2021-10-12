/*
 ****************************************************************
 *								*
 *			tchk/tchk-type.c			*
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
 *		3.0.0, de 29.05.95				*
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
#include <string.h>

#include ../$GEN.gen/config.h

#include "../h/flags.h"
#include "../h/tree.h"
#include "../h/c-tree.h"

#define	elif	else if

/*
 ******	Protótipos de funções ***********************************
 */
extern int	abort (void);
extern tree	build (enum tree_code, ...);
extern tree	build_array_type (tree, tree);
extern tree	build_binary_op (enum tree_code, tree, tree);
extern tree	build_function_type (tree, tree);
extern tree	build_int (int);
extern tree	build_pointer_type (tree);
extern tree	build_tree_list (tree, tree);
extern tree	c_build_type_variant (tree, int, int);
extern tree	chainon (tree, tree);
extern tree	convert (tree, tree);
extern tree	convert_for_assignment (tree, tree, char *);
extern tree	convert_units (tree size, int inunits, int outunits);
extern tree	default_conversion (tree);
extern void	error (const char *, ...);
extern tree	fold (tree);
extern void	incomplete_type_error (tree, tree);
extern int	list_length (tree);
extern tree	size_in_bytes (tree);
extern tree	tree_cons (tree, tree, tree);
extern tree	type_for_size (int, int);
extern void 	warning	(char *, ...);

/*
 ****************************************************************
 *	Return the _TYPE node describing the data type		*
 ****************************************************************
 */
/*
 *	Return the _TYPE node describing the data type
 *	of the data which NODE represents as a C expression.
 *	Arrays and functions are converted to pointers
 *	just as they are when they appear as C expressions.
 */
tree
datatype (tree node)
{
	register tree	type = TREE_TYPE (node);

	if (TREE_CODE (type) == ARRAY_TYPE)
		return (TYPE_POINTER_TO (TREE_TYPE (type)));

	if (TREE_CODE (type) == FUNCTION_TYPE)
		return (build_pointer_type (type));

	return (type);

}	/* end datatype */

/*
 ****************************************************************
 *	Do `exp = require_complete_type (exp);'			*
 ****************************************************************
 */
/*
 *	Do `exp = require_complete_type (exp);' to make
 *	sure exp does not have an incomplete type.
 *	(That includes void types.)
 */
tree
require_complete_type (tree value)
{
	tree	type = TREE_TYPE (value);

	/*
	 *	First, detect a valid value with a complete type.
	 */
	if (TYPE_SIZE (type) != NULL_TREE && type != void_type_node)
		return (value);

	incomplete_type_error (value, type);

	return (error_mark_node);

}	/* end require_complete_type */

/*
 ****************************************************************
 *    Print an message for invalid use of an incomplete type	*
 ****************************************************************
 */
/*
 *	Print an error message for invalid use of an
 *	incomplete type. VALUE is the expression that
 *	was used (or 0 if that isn't known) and TYPE
 *	is the type that was invalid.
 */
void
incomplete_type_error (tree value, tree type)
{
	char		*errmsg;

	/*
	 *	Avoid duplicate error message.
	 */
	if (TREE_CODE (type) == ERROR_MARK)
		return;

	if
	(	value != NULL_TREE && (TREE_CODE (value) == VAR_DECL ||
		TREE_CODE (value) == PARM_DECL)
	)
	{
		error
		(	"`%s' has an incomplete type",
			IDENTIFIER_POINTER (DECL_NAME (value))
		);
	}
	else
	{
	    retry:
		/*
		 *	We must print an error message.
		 *	Be clever about what it says.
		 */
		switch (TREE_CODE (type))
		{
		    case RECORD_TYPE:
			errmsg = "invalid use of undefined type `struct %s'";
			break;

		    case UNION_TYPE:
			errmsg = "invalid use of undefined type `union %s'";
			break;

		    case ENUMERAL_TYPE:
			errmsg = "invalid use of undefined type `enum %s'";
			break;

		    case VOID_TYPE:
			error ("invalid use of void expression");
			return;

		    case ARRAY_TYPE:
			if (TYPE_DOMAIN (type))
			{
				type = TREE_TYPE (type);
				goto retry;
			}
			error ("invalid use of array with unspecified bounds");
			return;

		    default:
			abort ();

		}	/* end switch (TREE_CODE (type)) */

		/*
		 *	If this type has a typedef-name,
		 *	the TYPE_NAME is a TYPE_DECL.
		 */
		if (TREE_CODE (TYPE_NAME (type)) == IDENTIFIER_NODE)
		{
			error (errmsg, IDENTIFIER_POINTER (TYPE_NAME (type)));
		}
		else
		{
			error
			(	"invalid use of incomplete typedef `%s'",
				 IDENTIFIER_POINTER (DECL_NAME (TYPE_NAME (type)))
			);
		}
	}

}	/* end incomplete_type_error */

/*
 ****************************************************************
 *	Return a variant of TYPE				*
 ****************************************************************
 */
/*
 *	Return a variant of TYPE which has all the type
 *	qualifiers of LIKE as well as those of TYPE.
 */
tree
qualify_type (tree type, tree like)
{
	int	constflag = TREE_READONLY (type) || TREE_READONLY (like);
	int	volflag = TREE_VOLATILE (type) || TREE_VOLATILE (like);

	return (c_build_type_variant (type, constflag, volflag));

}	/* end qualify_type */

/*
 ****************************************************************
 *	Return the common type of two types			*
 ****************************************************************
 */
/*
 *	Return the common type of two types. We assume that
 *	comptypes has already been done and returned 1; if
 *	that isn't so, this may crash.
 *
 *	This is the type for the result of most arithmetic
 *	operations if the operands have the given two types.
 *
 *	We do not deal with enumeral types here because they
 *	have already been converted to integer types.
 */
tree
commontype (tree t1, tree t2)
{
	register enum tree_code	form1, form2;

	/*
	 *	Save time if the two types are the same.
	 */
	if (t1 == t2)
		return (t1);

	/*
	 *	If one type is nonsense, use the other.
	 */
	if (t1 == error_mark_node)
		return (t2);

	if (t2 == error_mark_node)
		return (t1);

	/*
	 *	Treat an enum type as the unsigned integer
	 *	type of the same width.
	 */
	if (TREE_CODE (t1) == ENUMERAL_TYPE)
		t1 = type_for_size (TYPE_PRECISION (t1), 1);

	if (TREE_CODE (t2) == ENUMERAL_TYPE)
		t2 = type_for_size (TYPE_PRECISION (t2), 1);

	form1 = TREE_CODE (t1);
	form2 = TREE_CODE (t2);

	switch (form1)
	{
		/*
		 *	If only one is real, use it as the result.
		 */
	    case INTEGER_TYPE:
	    case REAL_TYPE:
		if (form1 == REAL_TYPE && form2 != REAL_TYPE)
			return (t1);

		if (form2 == REAL_TYPE && form1 != REAL_TYPE)
			return (t2);

		/*
		 *	Both real or both integers; use the one
		 *	with greater precision.
		 */
		if   (TYPE_PRECISION (t1) > TYPE_PRECISION (t2))
			return (t1);
		elif (TYPE_PRECISION (t2) > TYPE_PRECISION (t1))
			return (t2);

		/*
		 *	Same precision.  Prefer longs to ints
		 *	even when same size.
		 */
		if (t1 == long_unsigned_type_node || t2 == long_unsigned_type_node)
			return (long_unsigned_type_node);

		/*
		 *	But preserve unsignedness from the other type,
		 *	since long cannot hold all the values of an
		 *	unsigned int.
		 */
		if (t1 == long_integer_type_node || t2 == long_integer_type_node)
		{
			if (TREE_UNSIGNED (t1) || TREE_UNSIGNED (t2))
				return (long_unsigned_type_node);

			return (long_integer_type_node);
		}

		/*
		 *	Otherwise prefer the unsigned one.
		 */
		if (TREE_UNSIGNED (t1))
			return (t1);
		else
			return (t2);

		/*
		 *	For two pointers, do this recursively on the
		 *	target type, and combine the qualifiers of the
		 *	two types' targets.
		 */
	    case POINTER_TYPE:
#if (0)	/*************************************/
	    {
		tree target = commontype
		(	TYPE_MAIN_VARIANT (TREE_TYPE (t1)),
			TYPE_MAIN_VARIANT (TREE_TYPE (t2))
		);

		int constp = TREE_READ_ONLY (TREE_TYPE (t1)) ||
						TREE_READ_ONLY (TREE_TYPE (t2));

		int volatilep = TREE_VOLATILE (TREE_TYPE (t1)) ||
						TREE_VOLATILE (TREE_TYPE (t2));

		return
		(	build_pointer_type
			(	c_build_type_variant (target, constp, volatilep)
			)
		);
	    }
#endif	/*************************************/

		return (build_pointer_type (commontype (TREE_TYPE (t1), TREE_TYPE (t2))));

		/*
		 *	Save space: see if the result is identical
		 *	to one of the args. Merge the element types,
		 *	and have a size if either arg has one.
		 */
	    case ARRAY_TYPE:
	    {
		tree	elt = commontype (TREE_TYPE (t1), TREE_TYPE (t2));

		if (elt == TREE_TYPE (t1) && TYPE_DOMAIN (t1))
			return (t1);

		if (elt == TREE_TYPE (t2) && TYPE_DOMAIN (t2))
			return (t2);

		return
		(	build_array_type
			(	elt, TYPE_DOMAIN (TYPE_DOMAIN (t1) ? t1 : t2)
			)	
		);
	    }

		/*
		 *	Function types: prefer the one that
		 *	specified arg types. If both do, merge
		 *	the arg types.  Also merge the return types.
		 */
	    case FUNCTION_TYPE:
	    {
		tree	valtype = commontype (TREE_TYPE (t1), TREE_TYPE (t2));
		tree	p1 = TYPE_ARG_TYPES (t1);
		tree	p2 = TYPE_ARG_TYPES (t2);
		int	len;
		tree	newargs, n;
		int	i;

		/*
		 *	Save space: see if the result is identical
		 *	to one of the args
		 */
		if (valtype == TREE_TYPE (t1) && ! TYPE_ARG_TYPES (t2))
			return (t1);

		if (valtype == TREE_TYPE (t2) && ! TYPE_ARG_TYPES (t1))
			return (t2);

		/*
		 *	Simple way if one arg fails to specify argument types
		 */
		if (TYPE_ARG_TYPES (t1) == NULL_TREE)
			return (build_function_type (valtype, TYPE_ARG_TYPES (t2)));

		if (TYPE_ARG_TYPES (t2) == NULL_TREE)
			return (build_function_type (valtype, TYPE_ARG_TYPES (t1)));

		/*
		 *	If both args specify argument types, we must merge
		 *	the two lists, argument by argument.
		 */
		len = list_length (p1);
		newargs = NULL_TREE;

		for (i = 0; i < len; i++)
			newargs = tree_cons (NULL_TREE, NULL_TREE, newargs);

		n = newargs;

		for
		(	/* vazio */;
			p1 != NULL_TREE;
			p1 = TREE_CHAIN (p1), p2 = TREE_CHAIN (p2),
							n = TREE_CHAIN (n)
		)
			TREE_VALUE (n) = commontype (TREE_VALUE (p1),
							TREE_VALUE (p2));

		return (build_function_type (valtype, newargs));
	    }

	    default:
		return (t1);

	}	/* end switch (form1) */

}	/* end commontype */

/*
 ****************************************************************
 *  Return an unsigned type the same as TYPE in other respects	*
 ****************************************************************
 */
tree
unsigned_type (tree type)
{
	if (type == signed_char_type_node || type == char_type_node)
		return (unsigned_char_type_node);

	if (type == integer_type_node)
		return (unsigned_type_node);

	if (type == short_integer_type_node)
		return (short_unsigned_type_node);

	if (type == long_integer_type_node)
		return (long_unsigned_type_node);

	if (type == long_long_integer_type_node)
		return (long_long_unsigned_type_node);

	return (type);

}	/* end unsigned_type */

/*
 ****************************************************************
 *   Return a signed type the same as TYPE in other respects	*
 ****************************************************************
 */
tree
signed_type (tree type)
{
	if (type == unsigned_char_type_node || type == char_type_node)
		return (signed_char_type_node);

	if (type == unsigned_type_node)
		return (integer_type_node);

	if (type == short_unsigned_type_node)
		return (short_integer_type_node);

	if (type == long_unsigned_type_node)
		return (long_integer_type_node);

	if (type == long_long_unsigned_type_node)
		return (long_long_integer_type_node);

	return (type);

}	/* end signed_type */

/*
 ****************************************************************
 *	Return a type signed according to UNSIGNEDP		*
 ****************************************************************
 */
/*
 *	Return a type the same as TYPE except unsigned or
 *	signed according to UNSIGNEDP.
 */
tree
signed_or_unsigned_type (int unsignedp, tree type)
{
    if (TREE_CODE (type) != INTEGER_TYPE)
	return (type);

    if (TYPE_PRECISION (type) == TYPE_PRECISION (signed_char_type_node))
	return (unsignedp ? unsigned_char_type_node : signed_char_type_node);

    if (TYPE_PRECISION (type) == TYPE_PRECISION (integer_type_node)) 
	return (unsignedp ? unsigned_type_node : integer_type_node);

    if (TYPE_PRECISION (type) == TYPE_PRECISION (short_integer_type_node)) 
	return (unsignedp ? short_unsigned_type_node : short_integer_type_node);

    if (TYPE_PRECISION (type) == TYPE_PRECISION (long_integer_type_node)) 
	return (unsignedp ? long_unsigned_type_node : long_integer_type_node);

    if (TYPE_PRECISION (type) == TYPE_PRECISION (long_long_integer_type_node)) 
	return ((unsignedp ? long_long_unsigned_type_node
		: long_long_integer_type_node));

	return (type);

}	/* end signed_or_unsigned_type */

/*
 ****************************************************************
 *	Return an integer type with BITS bits of precision	*
 ****************************************************************
 */
/*
 *	Return an integer type with BITS bits of precision, that
 *	is unsigned if UNSIGNEDP is nonzero, otherwise signed.
 */
tree
type_for_size (int bits, int unsignedp)
{
    if (bits <= TYPE_PRECISION (signed_char_type_node))
	return (unsignedp ? unsigned_char_type_node : signed_char_type_node);

    if (bits <= TYPE_PRECISION (short_integer_type_node))
	return (unsignedp ? short_unsigned_type_node : short_integer_type_node);

    if (bits <= TYPE_PRECISION (integer_type_node))
	return (unsignedp ? unsigned_type_node : integer_type_node);

    if (bits <= TYPE_PRECISION (long_integer_type_node))
	return (unsignedp ? long_unsigned_type_node : long_integer_type_node);

    if (bits <= TYPE_PRECISION (long_long_integer_type_node))
	return ((unsignedp ? long_long_unsigned_type_node
		: long_long_integer_type_node));

	return (NULL_TREE);

}	/* end type_for_size */

/*
 ****************************************************************
 *	Transform float mode to float type			*
 ****************************************************************
 */
tree
get_floating_type (enum machine_mode mode)
{
	if (mode == TYPE_MODE (float_type_node))
		return (float_type_node);

	if (mode == TYPE_MODE (double_type_node))
		return (double_type_node);

	if (mode == TYPE_MODE (long_double_type_node))
		return (long_double_type_node);

	abort ();
	return (NULL_TREE);	/* To make lint happy */
}

/*
 ****************************************************************
 *	Get the "sizeof" (with warning)				*
 ****************************************************************
 */
tree
c_sizeof (tree type)
{
	enum tree_code	code = TREE_CODE (type);

	if (code == FUNCTION_TYPE)
	{
		if (pedantic || warn_pointer_arith)
			warning ("sizeof applied to a function type");

		return (build_int (1));
	}

	if (code == VOID_TYPE)
	{
		if (pedantic || warn_pointer_arith)
			warning ("sizeof applied to a void type");

		return (build_int (1));
	}

	/*
	 *	Convert in case a char is more than one unit
	 */
	return
	(	convert_units
		(	size_in_bytes (type),
			BITS_PER_UNIT,
			TYPE_PRECISION (char_type_node)
		)
	);

}	/* end c_sizeof */

/*
 ****************************************************************
 *	Get the "sizeof" (without warning)			*
 ****************************************************************
 */
tree
c_sizeof_nowarn (tree type)
{
	enum tree_code	code = TREE_CODE (type);

	if (code == FUNCTION_TYPE || code == VOID_TYPE)
		return (build_int (1));

	/*
	 *	Convert in case a char is more than one unit.
	 */
	return
	(	convert_units
		(	size_in_bytes (type),
			BITS_PER_UNIT,
			TYPE_PRECISION (char_type_node)
		)
	);

}	/* end c_sizeof_nowarn */

/*
 ****************************************************************
 *	Implement the __alignof keyword				*
 ****************************************************************
 */
/*
 *	Implement the __alignof keyword: Return the
 *	minimum required alignment of TYPE, measured in bytes.
 */
tree
c_alignof (tree type)
{
	enum tree_code	code = TREE_CODE (type);

	if (code == FUNCTION_TYPE)
		return (build_int (FUNCTION_BOUNDARY / BITS_PER_UNIT));

	if (code == VOID_TYPE)
		return (build_int (1));

	return (build_int (TYPE_ALIGN (type) / BITS_PER_UNIT));

}	/* end c_alignof */

/*
 ****************************************************************
 *	Return either DECL or its known constant value 		*
 ****************************************************************
 */
/*
 *	Return either DECL or its known constant value
 *	(if it has one).
 */
tree
decl_constant_value (tree decl)
{
	/*
	 *	Don't change a variable array bound or initial
	 *	value to a constant in a place where a variable
	 *	is invalid.
	 *
	 *	This is invalid (LITERAL) if initial value is not
	 *	constant. If it has either a function call, a memory
	 *	reference, or a variable, then re-evaluating it
	 *	could give different results.
	 *
	 *	Check for cases (CONSTRUCTOR) where this is sub-optimal,
	 *	even though valid.
	 */
	if
	(	! TREE_PUBLIC (decl) &&
		current_function_decl != NULL_TREE &&
		! pedantic && ! TREE_THIS_VOLATILE (decl) &&
		DECL_INITIAL (decl) != NULL_TREE &&
		TREE_CODE (DECL_INITIAL (decl)) != ERROR_MARK &&
		TREE_LITERAL (DECL_INITIAL (decl)) &&
		TREE_CODE (DECL_INITIAL (decl)) != CONSTRUCTOR &&
		DECL_MODE (decl) != BLKmode
	)
		return (DECL_INITIAL (decl));

	return (decl);

}	/* end decl_constant_value */

/*
 ****************************************************************
 *	Convert the actual parameter expressions		*
 ****************************************************************
 */
/*
 *	Convert the actual parameter expressions in the
 *	list VALUES to the types in the list TYPELIST.
 *	If parmdecls is exhausted, or when an element has
 *	NULL as its type, perform the default conversions.
 *	NAME is an IDENTIFIER_NODE or 0.  It is used only
 *	for error messages. This is also where warnings
 *	about wrong number of args are generated.
 * 
 *	Return a list of expressions for the parameters as converted.
 *
 *	Both VALUES and the returned value are chains of
 *	TREE_LIST nodes with the elements of the list in
 *	the TREE_VALUE slots of those nodes.
 */
tree
actualparameterlist (tree typelist, tree values, tree name)
{
	register tree	typetail = typelist, valtail;
	register tree	result = NULL_TREE;
	int		i;
	char		area[32];

	/*
	 *	Verifica se foi dado um protótipo
	 */
	if (extra_warnings && typetail == NULL_TREE)
	{
		if (name)
		{
			warning
			(	"função \"%s\" chamada sem protótipos",
				IDENTIFIER_POINTER (name)
			);
		}
		else
		{
			warning ("função chamada sem protótipos");
		}
	}

	/*
	 *	Examina os argumentos
	 */
	for
	(	valtail = values, i = 1;
		valtail != NULL_TREE;
		valtail = TREE_CHAIN (valtail), i++
	)
	{
		register tree	type = typetail ? TREE_VALUE (typetail) : NULL_TREE;
		tree		val = TREE_VALUE (valtail);
		tree		parm;

		if (type == void_type_node)
		{
			if (name)
			{
				error
				(	"too many arguments to function `%s'",
					IDENTIFIER_POINTER (name)
				);
			}
			else
			{
				error ("too many arguments to function");
			}

			break;
		}

		/*
	 	 *	build_c_cast puts on a NOP_EXPR to make
		 *	the result not an lvalue. Strip such NOP_EXPRs,
		 *	since VAL is used in non-lvalue context.
		 */
		if
		(	TREE_CODE (val) == NOP_EXPR &&
			TREE_TYPE (val) == TREE_TYPE (TREE_OPERAND (val, 0))
		)
			val = TREE_OPERAND (val, 0);

		if
		(	TREE_CODE (TREE_TYPE (val)) == ARRAY_TYPE ||
			TREE_CODE (TREE_TYPE (val)) == FUNCTION_TYPE
		)
			val = default_conversion (val);

		val = require_complete_type (val);

		/*
		 *	Formal parm type is specified by a function prototype
		 */
		if   (type != NULL_TREE)
		{
			tree	parmval;

			if (TYPE_SIZE (type) == NULL_TREE)
			{
				error
				(	"parameter type of called function "
					"is incomplete"
				);
				parmval = val;
			}
			else
			{
#ifdef PROMOTE_PROTOTYPES
				/*
				 *	Rather than truncating and then
				 * 	reextending, convert directly to
				 *	int, if that's the type we will want
				 */
				if
				(	! flag_traditional &&
					TREE_CODE (type) == INTEGER_TYPE &&
					(TYPE_PRECISION (type)
					   < TYPE_PRECISION (integer_type_node))
				)
					type = integer_type_node;
#endif
				sprintf (area, "%d. argument passing", i);

				parmval = convert_for_assignment
				(	type, val, area
				);

#ifdef PROMOTE_PROTOTYPES

				if
				(	TREE_CODE (type) == INTEGER_TYPE &&
					(TYPE_PRECISION (type) <
					   TYPE_PRECISION (integer_type_node))
				)
					parmval = default_conversion (parmval);
#endif
			}

			parm = build_tree_list (NULL_TREE, parmval);
		}
		elif
		(	TREE_CODE (TREE_TYPE (val)) == REAL_TYPE &&
			(TYPE_PRECISION (TREE_TYPE (val))
					 < TYPE_PRECISION (double_type_node))
		)
		{
			/*
			 *	Convert `float' to `double'
			 */
			parm = build_tree_list
				(NULL_TREE, convert (double_type_node, val));
		}
		else
		{
			/*
			 *	Convert `short' and `char' to full-size `int'
			 */
			parm = build_tree_list
					(NULL_TREE, default_conversion (val));
		}

		result = chainon (result, parm);

		if (typetail)
			typetail = TREE_CHAIN (typetail);

	}	/* end for (...) */

	if (typetail != NULL_TREE && TREE_VALUE (typetail) != void_type_node)
	{
		if (name)
		{
			error
			(	"too few arguments to function `%s'",
				IDENTIFIER_POINTER (name)
			);
		}
		else
		{
			error ("too few arguments to function");
		}
	}

	return (result);

}	/* end actualparameterlist */

/*
 ****************************************************************
 *	Return a tree for the sum or difference			*
 ****************************************************************
 */
/*
 *	Return a tree for the sum or difference
 *	(RESULTCODE says which) of pointer PTROP
 *	and integer INTOP.
 */
tree
pointer_int_sum (enum tree_code resultcode, register tree ptrop,
						register tree intop)
{
	tree		size_exp;
	register tree	result, folded;

	/*
	 *	The result is a pointer of the same type that is being added
	 */
	tree result_type = datatype (ptrop);

	if   (TREE_CODE (TREE_TYPE (result_type)) == VOID_TYPE)
	{
		if (pedantic || warn_pointer_arith)
			warning ("pointer of type `void *' used in arithmetic");

		size_exp = integer_one_node;
	}
	elif (TREE_CODE (TREE_TYPE (result_type)) == FUNCTION_TYPE)
	{
		if (pedantic || warn_pointer_arith)
			warning ("pointer to a function used in arithmetic");

		size_exp = integer_one_node;
	}
	else
	{
		size_exp = c_sizeof (TREE_TYPE (result_type));
	}

	/*
	 *	If what we are about to multiply by the size of
	 *	the elements contains a constant term, apply
	 *	distributive law and multiply that constant
	 *	term separately. This helps produce common
	 *	subexpressions.
	 */
	if
	(	(TREE_CODE (intop) == PLUS_EXPR ||
				TREE_CODE (intop) == MINUS_EXPR) &&
		! TREE_LITERAL (intop) &&
		TREE_LITERAL (TREE_OPERAND (intop, 1)) &&
		TREE_LITERAL (size_exp)
	)
	{
		enum tree_code	subcode = resultcode;

		if (TREE_CODE (intop) == MINUS_EXPR)
			subcode = (subcode == PLUS_EXPR ? MINUS_EXPR : PLUS_EXPR);

		ptrop = build_binary_op (subcode, ptrop, TREE_OPERAND (intop, 1));
		intop = TREE_OPERAND (intop, 0);
	}

	/*
	 *	Convert the integer argument to a type the same size
	 *	as a pointer so the multiply won't overflow spuriously
	 */
	if (TYPE_PRECISION (TREE_TYPE (intop)) != POINTER_SIZE)
		intop = convert (type_for_size (POINTER_SIZE, 0), intop);

	/*
	 *	Replace the integer argument with a suitable
	 *	product by the object size
	 */
	intop = build_binary_op (MULT_EXPR, intop, size_exp);

	/*
	 *	Create the sum or difference
	 */
	result = build (resultcode, result_type, ptrop, intop);

	folded = fold (result);

	if (folded == result)
		TREE_LITERAL (folded) = TREE_LITERAL (ptrop) &
							TREE_LITERAL (intop);

	return (folded);

}	/* end pointer_int_sum */

/*
 ****************************************************************
 *	Return a tree for the difference of pointers		*
 ****************************************************************
 */
/*
 *	Return a tree for the difference of pointers
 *	OP0 and OP1. The resulting tree has type int
 */
tree
pointer_diff (register tree op0, register tree op1)
{
	tree		dt0 = datatype (op0);
	register tree	result, folded;
	tree		restype = type_for_size (POINTER_SIZE, 0);

	if (pedantic)
	{
		if (TREE_CODE (TREE_TYPE (dt0)) == VOID_TYPE)
			warning ("pointer of type `void *' used in subtraction");

		if (TREE_CODE (TREE_TYPE (dt0)) == FUNCTION_TYPE)
			warning ("pointer to a function used in subtraction");
	}

	/*
	 *	First do the subtraction as integers;
	 *	then drop through to build the divide operator
	 */
	op0 = build_binary_op
	(	MINUS_EXPR,
		convert (restype, op0),
		convert (restype, op1)
	);

	op1 = c_sizeof_nowarn (TREE_TYPE (dt0));

	/*
	 *	Create the sum or difference.
	 */
	result = build (EXACT_DIV_EXPR, restype, op0, op1);

	folded = fold (result);

	if (folded == result)
		TREE_LITERAL (folded) = TREE_LITERAL (op0) & TREE_LITERAL (op1);

	return (folded);

}	/* end pointer_diff */

/*
 ****************************************************************
 *	Print an error message for invalid operands		*
 ****************************************************************
 */
/*
 *	Print an error message for invalid operands to
 *	arith operation CODE. NOP_EXPR is used as a special
 *	case (see truthvalue_conversion).
 */
void
binary_op_error (enum tree_code code)
{
	register char	*opname;

	switch (code)
	{
	    case NOP_EXPR:
		error ("invalid truth-value expression");
		return;

	    case PLUS_EXPR:
		opname = "+";
		break;

	    case MINUS_EXPR:
		opname = "-";
		break;

	    case MULT_EXPR:
		opname = "*";
		break;

	    case MAX_EXPR:
		opname = "max";
		break;

	    case MIN_EXPR:
		opname = "min";
		break;

	    case EQ_EXPR:
		opname = "==";
		break;

	    case NE_EXPR:
		opname = "!=";
		break;

	    case LE_EXPR:
		opname = "<=";
		break;

	    case GE_EXPR:
		opname = ">=";
		break;

	    case LT_EXPR:
		opname = "<";
		break;

	    case GT_EXPR:
		opname = ">";
		break;

	    case LSHIFT_EXPR:
		opname = "<<";
		break;

	    case RSHIFT_EXPR:
		opname = ">>";
		break;

	    case TRUNC_MOD_EXPR:
		opname = "%";
		break;

	    case TRUNC_DIV_EXPR:
		opname = "/";
		break;

	    case BIT_AND_EXPR:
		opname = "&";
		break;

	    case BIT_IOR_EXPR:
		opname = "|";
		break;

	    case TRUTH_ANDIF_EXPR:
		opname = "&&";
		break;

	    case TRUTH_ORIF_EXPR:
		opname = "||";
		break;

	    case BIT_XOR_EXPR:
		opname = "^";
		break;

	}	/* end switch (code) */

	error ("invalid operands to binary %s", opname);

}	/* end binary_op_error */

/*
 ****************************************************************
 *	Warn about storing in something that is `const'		*
 ****************************************************************
 */
void
readonly_warning (tree arg, char *string)
{
	char		buf[80];

	strcpy (buf, string);

	if   (TREE_CODE (arg) == COMPONENT_REF)
	{
		if (TREE_READONLY (TREE_OPERAND (arg, 0)))
		{
			readonly_warning (TREE_OPERAND (arg, 0), string);
		}
		else
		{
			strcat (buf, " of read-only member `%s'");

			warning
			(	buf,
				IDENTIFIER_POINTER
				(	DECL_NAME (TREE_OPERAND (arg, 1))
				)	
			);
		}
	}
	elif (TREE_CODE (arg) == VAR_DECL)
	{
		strcat (buf, " of read-only variable `%s'");
 		warning (buf, IDENTIFIER_POINTER (DECL_NAME (arg)));
	}
	else
	{
		warning ("%s of read-only location", buf);
	}

}	/* end readonly_warning */
