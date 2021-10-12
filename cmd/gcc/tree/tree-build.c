/*
 ****************************************************************
 *								*
 *			tree-build.c				*
 *								*
 *	Language-independent   node   constructors  for		*
 *	parse phase of GNU compiler.				*
 *								*
 *	This file contains the low level primitives for		*
 *	operating on  tree nodes, including allocation,		*
 *	list  operations,  interning   of  identifiers,		*
 *	construction  of  data type nodes and statement		*
 *	nodes,  and  construction  of  type  conversion		*
 *	nodes.  It also  contains  tables index by tree		*
 *	code  that  describe how to take apart nodes of		*
 *	that code.						*
 *								*
 *	It is intended  to be language-independent, but		*
 *	occasionally calls  language-dependent routines		*
 *	defined (for C) in typecheck.c.				*
 *								*
 *	The low-level  allocation  routines oballoc and		*
 *	permalloc  are  used  also  for allocating many		*
 *	other  kinds  of  objects  by all passes of the		*
 *	compiler.						*
 *								*
 *	Versão	3.0.0, de 26.08.93				*
 *		3.0.0, de 09.10.93				*
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

#pragma	getenv (GEN)

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include ../$GEN.gen/config.h

#include "../h/tree.h"
#include "../h/obstack.h"

#define	elif	else if

/*
 ******	Diversas definições de OBSTACK **************************
 */
/*
 *	Tree nodes of permanent duration are allocated in this obstack.
 *	They are the identifier nodes, and everything outside of
 *	the bodies and parameters of function definitions.
 */
extern OBSTACK	permanent_obstack;

/*
 *	The initial RTL, and all ..._TYPE nodes, in a function
 *	are allocated in this obstack.  Usually they are freed at the
 *	end of the function, but if the function is inline they are saved.
 */
extern OBSTACK	maybepermanent_obstack;

/*
 *	The contents of the current function definition are allocated
 *	in this obstack, and all are freed at the end of the function.
 */
extern OBSTACK	temporary_obstack;

/*
 *	The tree nodes of an expression are allocated
 *	in this obstack, and all are freed at the end of the expression.
 */
extern OBSTACK	momentary_obstack;

/*
 *	This points at either permanent_obstack or maybepermanent_obstack.
 */
extern OBSTACK	*saveable_obstack;

/*
 *	This is same as saveable_obstack during parse and expansion phase;
 *	it points to temporary_obstack during optimization.
 *	This is the obstack to be used for creating rtl objects.
 */
extern OBSTACK	*rtl_obstack;

/*
 *	This points at either permanent_obstack or temporary_obstack.
 */
extern OBSTACK	*current_obstack;

/*
 *	This points at either permanent_obstack or temporary_obstack
 *	or momentary_obstack.
 */
extern OBSTACK	*expression_obstack;

/*
 ****** Número de operandos da árvore ***************************
 *
 *	Table indexed by tree code giving number of expression
 *	operands beyond the fixed part of the node structure.
 *	Not used for types or decls.
 */
extern int	tree_code_length[];

/*
 *	Here is how primitive or already-canonicalized types' hash
 *	codes are made.
 */
#define TYPE_HASH(TYPE) TREE_UID (TYPE)

/*
 ******	Protótipos de funções ***********************************
 */
extern tree	convert (tree type, tree expr);
extern int 	type_hash_list (tree list);
extern tree 	type_hash_canon (int hashcode, tree type);
extern void 	error (const char *fmt, ...);
extern tree 	copy_node (tree node);
extern tree 	make_node (enum tree_code code);
extern void 	layout_decl (tree decl, unsigned known_align);
extern void 	layout_type (tree type);

/*
 ****************************************************************
 *	Return a newly constructed INTEGER_CST			*
 ****************************************************************
 */
/*
 *	Return a newly constructed INTEGER_CST node whose
 *	constant value is specified by the two ints LOW and HI.
 *	The TREE_TYPE is set to `int'.
 */
tree
build_int_2 (int low, int hi)
{
	register tree	t = make_node (INTEGER_CST);

	TREE_INT_CST_LOW (t) = low;
	TREE_INT_CST_HIGH (t) = hi;
	TREE_TYPE (t) = integer_type_node;

	return (t);

}	/* end build_int_2 */

/*
 ****************************************************************
 *	Return a new REAL_CST node				*
 ****************************************************************
 */
/*
 *	Return a new REAL_CST node whose type
 *	is TYPE and value is D.
 */
tree
build_real (tree type, REAL_VALUE_TYPE d)
{
	tree		v;

	/*
 	 *	Check for valid float value for this type
	 *	on this target machine; if not, can print
	 *	error message and store a valid value in D.
	 */
#ifdef CHECK_FLOAT_VALUE
	CHECK_FLOAT_VALUE (TYPE_MODE (type), d);
#endif

	v = make_node (REAL_CST);
	TREE_TYPE (v) = type;
	TREE_REAL_CST (v) = d;

	return v;

}	/* end build_real */

#if !defined (REAL_IS_NOT_DOUBLE) || defined (REAL_ARITHMETIC)
/*
 ****************************************************************
 *	Return a new REAL_CST node whose type is TYPE		*
 ****************************************************************
 */
/*
 *	Return a new REAL_CST node whose type is TYPE
 *	and whose value is the integer value of the
 *	INTEGER_CST node I.
 */
REAL_VALUE_TYPE
real_value_from_int_cst (tree i)
{
	REAL_VALUE_TYPE		d;

#ifdef REAL_ARITHMETIC

	REAL_VALUE_FROM_INT (d, TREE_INT_CST_LOW (i), TREE_INT_CST_HIGH (i));

#else /* not REAL_ARITHMETIC */
#define MASK ((unsigned)1 << (HOST_BITS_PER_INT - 1))

	if (TREE_INT_CST_HIGH (i) < 0)
	{
		d = (double) (~ TREE_INT_CST_HIGH (i));
		d *= ((double) (1 << (HOST_BITS_PER_INT / 2)) *
			(double) (1 << (HOST_BITS_PER_INT / 2)));
		/*
		 *	The following four lines are equivalent
		 *	to converting ~ TREE_INT_CST_LOW (i) from
		 *	unsigned to double, but that is broken in
		 *	some compilers.
		 */
		if (((~ TREE_INT_CST_LOW (i)) & MASK) != 0)
			d += ((double) MASK +
				(double) ((~ MASK) & ~ TREE_INT_CST_LOW (i)));
		else
			d += (double) (unsigned) (~ TREE_INT_CST_LOW (i));

		d = (- d - 1.0);
	}
	else
	{
		d = (double) TREE_INT_CST_HIGH (i);
		d *= ((double) (1 << (HOST_BITS_PER_INT / 2)) *
			(double) (1 << (HOST_BITS_PER_INT / 2)));

		if ((TREE_INT_CST_LOW (i) & MASK) != 0)
			d += ((double) MASK +
				(double) ((~ MASK) & TREE_INT_CST_LOW (i)));
		else
			d += (double) (unsigned) TREE_INT_CST_LOW (i);
	}

#undef MASK
#endif /* not REAL_ARITHMETIC */

	return (d);

}	/* end real_value_from_int_cst */

/*
 ****************************************************************
 *	Convert a integer to real constant			*
 ****************************************************************
 */
/*
 *	This function can't be implemented if we can't
 *	do arithmetic on the float representation.
 */
tree
build_real_from_int_cst (tree type, tree i)
{
	tree		v;
	REAL_VALUE_TYPE	d;

	v = make_node (REAL_CST);
	TREE_TYPE (v) = type;

	d = real_value_from_int_cst (i);

	/*
	 *	Check for valid float value for this type
	 *	on this target machine; if not, can print
	 *	error message and store a valid value in D.
	 */
#ifdef CHECK_FLOAT_VALUE
	CHECK_FLOAT_VALUE (TYPE_MODE (type), d);
#endif

	TREE_REAL_CST (v) = d;
	return (v);

}	/* end build_real_from_int_cst */

#endif /* not REAL_IS_NOT_DOUBLE, or REAL_ARITHMETIC */

/*
 ****************************************************************
 *	Return a newly constructed STRING_CST			*
 ****************************************************************
 */
/*
 *	Return a newly constructed STRING_CST
 *	node whose value is the LEN characters at STR.
 *	The TREE_TYPE is not initialized.
 */
tree
build_string (int len, char *str)
{
	register tree	s = make_node (STRING_CST);

	TREE_STRING_LENGTH (s) = len;
	TREE_STRING_POINTER (s) = obstack_copy0 (saveable_obstack, str, len);

	return (s);

}	/* end build_string */

/*
 ****************************************************************
 *	Return a newly constructed COMPLEX_CST node		*
 ****************************************************************
 */
/*
 *	Return a newly constructed COMPLEX_CST
 *	node whose value is specified by the real
 *	and imaginary parts REAL and IMAG.
 *	Both REAL and IMAG should be constant nodes.
 *	The TREE_TYPE is not initialized.
 */
tree
build_complex (tree real, tree imag)
{
	register tree	t = make_node (COMPLEX_CST);

	TREE_REALPART (t) = real;
	TREE_IMAGPART (t) = imag;

	return (t);

}	/* end build_complex */

/*
 ****************************************************************
 *	Return a newly created TREE_LIST node			*
 ****************************************************************
 */
/*
 *	Return a newly created TREE_LIST node whose
 *	purpose and value fields are PARM and VALUE.
 */
tree
build_tree_list (tree parm, tree value)
{
	register tree	t = make_node (TREE_LIST);

	TREE_PURPOSE (t) = parm;
	TREE_VALUE (t) = value;

	return (t);

}	/* end build_tree_list */

/*
 ****************************************************************
 *	Return a newly created TREE_LIST node			*
 ****************************************************************
 */
/*
 *	Return a newly created TREE_LIST node whose
 *	purpose and value fields are PARM and VALUE
 *	and whose TREE_CHAIN is CHAIN.
 */
tree
tree_cons (tree purpose, tree value, tree chain)
{
	register tree	node = make_node (TREE_LIST);

	TREE_CHAIN (node) = chain;
	TREE_PURPOSE (node) = purpose;
	TREE_VALUE (node) = value;

	return (node);

}	/* end tree_cons */

/*
 ****************************************************************
 *	Same as `tree_cons' but make a permanent object		*
 ****************************************************************
 */
tree
perm_tree_cons (tree purpose, tree value, tree chain)
{
	register tree	 node;
	register OBSTACK *ambient_obstack = current_obstack;

	current_obstack = &permanent_obstack;

	node = make_node (TREE_LIST);
	TREE_CHAIN (node) = chain;
	TREE_PURPOSE (node) = purpose;
	TREE_VALUE (node) = value;

	current_obstack = ambient_obstack;

	return (node);

}	/* end perm_tree_cons */

/*
 ****************************************************************
 *	Same as `tree_cons', but make this node temporary	*
 ****************************************************************
 */
tree
temp_tree_cons (tree purpose, tree value, tree chain)
{
	register tree	node;
	register OBSTACK *ambient_obstack = current_obstack;

	current_obstack = &temporary_obstack;

	node = make_node (TREE_LIST);
	TREE_CHAIN (node) = chain;
	TREE_PURPOSE (node) = purpose;
	TREE_VALUE (node) = value;

	current_obstack = ambient_obstack;
	return (node);

}	/* end temp_tree_cons */

/*
 ****************************************************************
 *	Same as `tree_cons', but ... 				*
 ****************************************************************
 */
/*
 *	Same as `tree_cons', but save this node
 *	if the function's RTL is saved.
 */
tree
saveable_tree_cons (tree purpose, tree value, tree chain)
{
	register tree	node;
	register OBSTACK *ambient_obstack = current_obstack;

	current_obstack = saveable_obstack;

	node = make_node (TREE_LIST);
	TREE_CHAIN (node) = chain;
	TREE_PURPOSE (node) = purpose;
	TREE_VALUE (node) = value;

	current_obstack = ambient_obstack;
	return (node);

}	/* end saveable_tree_cons */

/*
 ****************************************************************
 *	Low-level constructors for expressions			*
 ****************************************************************
 */
/*
 *	Build an expression of code CODE, data type TYPE,
 *	and operands as specified by the arguments ARG1
 *	and following arguments. Expressions and reference
 *	nodes can be created this way. Constants, decls,
 *	types and misc nodes cannot be.
 */
tree
build (enum tree_code code, ...)
{
	register va_list	p;
	register tree		t;
	register int		length;
	register int		i;

	va_start (p, code);

	t = make_node (code);
	length = tree_code_length[(int) code];
	TREE_TYPE (t) = va_arg (p, tree);

	if (length == 2)
	{
		/*
		 *	This is equivalent to the loop below, but faster
		 */
		register tree arg0 = va_arg (p, tree);
		register tree arg1 = va_arg (p, tree);
		TREE_OPERAND (t, 0) = arg0;
		TREE_OPERAND (t, 1) = arg1;
		TREE_VOLATILE (t) = (arg0 && TREE_VOLATILE (arg0)) ||
					(arg1 && TREE_VOLATILE (arg1));
	}
	else
	{
		for (i = 0; i < length; i++)
		{
			register tree	operand = va_arg (p, tree);

			TREE_OPERAND (t, i) = operand;

			if (operand && TREE_VOLATILE (operand))
				TREE_VOLATILE (t) = 1;
		}
	}

	va_end (p);
	return (t);

}	/* end build */

/*
 ****************************************************************
 *	Similar to "build", but ...				*
 ****************************************************************
 */
/*
 *	Similar except don't specify the TREE_TYPE
 *	and leave the TREE_VOLATILE as 0.
 *	It is permissible for arguments to be null,
 *	or even garbage if their values do not matter.
 */
tree
build_nt (enum tree_code code, ...)
{
	register va_list	p;
	register tree		t;
	register int		length;
	register int		i;

	va_start (p, code);

	t = make_node (code);
	length = tree_code_length[code];

	for (i = 0; i < length; i++)
		TREE_OPERAND (t, i) = va_arg (p, tree);

	va_end (p);
	return (t);

}	/* end build_nt */

/*
 ****************************************************************
 *	Create a OP_IDENTIFIER node				*
 ****************************************************************
 */
tree
build_op_identifier (tree op1, tree op2)
{
	register tree	t = make_node (OP_IDENTIFIER);

	TREE_PURPOSE (t) = op1;
	TREE_VALUE (t) = op2;

	return (t);

}	/* end build_op_identifier */

/*
 ****************************************************************
 *	Create a DECL_... node of code CODE			*
 ****************************************************************
 */
/*
 *	Create a DECL_... node of code CODE, name NAME
 *	and data type TYPE. We do NOT enter this node
 *	in any sort of symbol table.
 *
 *	layout_decl is used to set up the decl's storage layout.
 *	Other slots are initialized to 0 or null pointers.
 */
tree
build_decl (enum tree_code code, tree name, tree type)
{
	register tree	t;

	t = make_node (code);

#if (0)	/*************************************/
	/*
	 *	That is not done, deliberately, so that
	 *	having error_mark_node as the type can
	 *	suppress useless errors in the use of
	 *	this variable.
	 */
	if (type == error_mark_node)
		type = integer_type_node;
#endif	/*************************************/

	DECL_NAME (t) = name;

	if (name)
	{
		DECL_PRINT_NAME (t) = IDENTIFIER_POINTER (name);
		DECL_ASSEMBLER_NAME (t) = IDENTIFIER_POINTER (name);
	}

	TREE_TYPE (t) = type;
	DECL_ARGUMENTS (t) = NULL_TREE;
	DECL_INITIAL (t) = NULL_TREE;

	if (code == VAR_DECL || code == PARM_DECL || code == RESULT_DECL)
		layout_decl (t, 0);
	elif (code == FUNCTION_DECL)
		DECL_MODE (t) = FUNCTION_MODE;

	return (t);

}	/* end build_decl */

/*
 ****************************************************************
 *	Create a LET_... node					*
 ****************************************************************
 */
/*
 *	LET_STMT nodes are used to represent the structure
 *	of binding contours and declarations, once those
 *	contours have been exited and their contents compiled.
 *	This information is used for outputting debugging info.
 */
tree
build_let (char *filename, int line, tree vars, tree subblocks,
					tree supercontext, tree tags)
{
	register tree	t = make_node (LET_STMT);

	STMT_SOURCE_FILE (t) = filename;
	STMT_SOURCE_LINE (t) = line;
	STMT_VARS (t) = vars;
	STMT_SUBBLOCKS (t) = subblocks;
	STMT_SUPERCONTEXT (t) = supercontext;
	STMT_BIND_SIZE (t) = NULL_TREE;
	STMT_TYPE_TAGS (t) = tags;

	return (t);

}	/* end build_let */

/*
 ****************************************************************
 *	Return a type like TYPE					*
 ****************************************************************
 */
/*
 *	Return a type like TYPE except that its TREE_READONLY
 *	is CONSTP and its TREE_VOLATILE is VOLATILEP.
 *
 *	Such variant types already made are recorded so that
 *	duplicates are not made.
 *
 *	A variant types should never be used as the type of
 *	an expression. Always copy the variant information into
 *	the TREE_READONLY and TREE_VOLATILE of the expression,
 *	and then give the expression as its type the
 *	"main variant", the variant whose TREE_READONLY
 *	and TREE_VOLATILE are zero.  Use TYPE_MAIN_VARIANT
 *	to find the main variant.
 */
tree
build_type_variant (tree type, int constp, int volatilep)
{
	register tree	t, m = TYPE_MAIN_VARIANT (type);
	register OBSTACK *ambient_obstack = current_obstack;

	/*
 	 *	Treat any nonzero argument as 1.
	 */
	constp = !!constp;
	volatilep = !!volatilep;

	/*
	 *	First search the chain variants for one
	 *	that is what we want.
	 */
	for (t = m; t; t = TYPE_NEXT_VARIANT (t))
	{
		if (constp == TREE_READONLY (t) && volatilep == TREE_VOLATILE (t))
			return t;
	}

	/*
	 *	We need a new one.
	 */
	current_obstack = TREE_PERMANENT (type) ?
				&permanent_obstack : saveable_obstack;

	t = copy_node (type);
	TREE_READONLY (t) = constp;
	TREE_VOLATILE (t) = volatilep;
	TYPE_POINTER_TO (t) = NULL_TREE;
	TYPE_REFERENCE_TO (t) = NULL_TREE;

	/*
	 *	Add this type to the chain of variants of TYPE.
	 */
	TYPE_NEXT_VARIANT (t) = TYPE_NEXT_VARIANT (m);
	TYPE_NEXT_VARIANT (m) = t;

	current_obstack = ambient_obstack;

	return (t);

}	/* end build_type_variant */

/*
 ****************************************************************
 *	Constructors for pointer, array and function types 	*
 ****************************************************************
 */
/*
 *	Constructors for pointer, array and function types.
 *	(RECORD_TYPE, UNION_TYPE and ENUMERAL_TYPE nodes are
 *	constructed by language-dependent code, not here.)
 *
 *	Construct, lay out and return the type of pointers
 *	to TO_TYPE. If such a type has already been
 *	constructed, reuse it.
 */
tree
build_pointer_type (tree to_type)
{
	register tree		t = TYPE_POINTER_TO (to_type);
	register OBSTACK	*ambient_obstack = current_obstack;
	register OBSTACK	*ambient_saveable_obstack = saveable_obstack;

	/*
	 *	First, if we already have a type for
	 *	pointers to TO_TYPE, use it.
	 */
	if (t)
		return t;

	/*
	 *	We need a new one.
	 *	If TO_TYPE is permanent, make this permanent too.
	 */
	if (TREE_PERMANENT (to_type))
	{
		current_obstack = &permanent_obstack;
		saveable_obstack = &permanent_obstack;
	}

	t = make_node (POINTER_TYPE);
	TREE_TYPE (t) = to_type;

	/*
	 *	Record this type as the pointer to TO_TYPE.
	 */
	TYPE_POINTER_TO (to_type) = t;

	/*
	 *	Lay out the type. This function has many callers
	 *	that are concerned with expression-construction,
	 *	and this simplifies them all. Also, it guarantees
	 *	the TYPE_SIZE is permanent if the type is.
	 */
	layout_type (t);

	current_obstack = ambient_obstack;
	saveable_obstack = ambient_saveable_obstack;

	return (t);

}	/* end build_pointer_type */

/*
 ****************************************************************
 *	Create a type of integers to be the TYPE_DOMAIN		*
 ****************************************************************
 */
/*
 *	Create a type of integers to be the TYPE_DOMAIN
 *	of an ARRAY_TYPE. MAXVAL should be the maximum
 *	value in the domain (one less than the length of
 *	the array).
 */
tree
build_index_type (tree maxval)
{
	register tree	itype = make_node (INTEGER_TYPE);

	TYPE_PRECISION (itype) = BITS_PER_WORD;
	TYPE_MIN_VALUE (itype) = build_int_2 (0, 0);
	TREE_TYPE (TYPE_MIN_VALUE (itype)) = sizetype;
	TYPE_MAX_VALUE (itype) = convert (sizetype, maxval);
	TYPE_MODE (itype) = SImode;
	TYPE_SIZE (itype) = TYPE_SIZE (sizetype);
	TYPE_SIZE_UNIT (itype) = TYPE_SIZE_UNIT (sizetype);
	TYPE_ALIGN (itype) = TYPE_ALIGN (sizetype);

	if (TREE_CODE (maxval) == INTEGER_CST)
	{
		int maxint = TREE_INT_CST_LOW (maxval);

		return (type_hash_canon (maxint > 0 ? maxint : - maxint, itype));
	}
	else
	{
		return (itype);
	}

}	/* end build_index_type */

/*
 ****************************************************************
 *	Construct, lay out and return the type of arrays	*
 ****************************************************************
 */
/*
 *	Construct, lay out and return the type of arrays
 *	of elements with ELT_TYPE and number of elements
 *	specified by the range of values of INDEX_TYPE.
 *	If such a type has already been constructed, reuse it.
 */
tree
build_array_type (tree elt_type, tree index_type)
{
	register tree	t = make_node (ARRAY_TYPE);
	int		hashcode;

	if (TREE_CODE (elt_type) == FUNCTION_TYPE)
	{
		error ("arrays of functions are not meaningful");
		elt_type = integer_type_node;
	}

	TREE_TYPE (t) = elt_type;
	TYPE_DOMAIN (t) = index_type;

	/*
	 *	Make sure TYPE_POINTER_TO (elt_type) is filled in.
	 */
	build_pointer_type (elt_type);

#if (0)	/*************************************/
	/*
	 *	Also that of the main variant,
	 *	which is the type the element will have.
	 */
	build_pointer_type (TYPE_MAIN_VARIANT (elt_type));
#endif	/*************************************/

	if (index_type == NULL_TREE)
		return (t);

	hashcode = TYPE_HASH (elt_type) + TYPE_HASH (index_type);
	t = type_hash_canon (hashcode, t);

	if (TYPE_SIZE (t) == NULL_TREE)
		layout_type (t);

	return (t);

}	/* end build_array_type */

/*
 ****************************************************************
 *	Construct the type of functions				*
 ****************************************************************
 */
/*
 *	Construct, lay out and return the type of functions
 *	returning type VALUE_TYPE given arguments of types
 *	ARG_TYPES. ARG_TYPES is a chain of TREE_LIST nodes
 *	whose TREE_VALUEs are data type nodes for the
 *	arguments of the function. If such a type has
 *	already been constructed, reuse it.
 */
tree
build_function_type (tree value_type, tree arg_types)
{
	register tree	t;
	int		hashcode;

	if
	(	TREE_CODE (value_type) == FUNCTION_TYPE ||
		TREE_CODE (value_type) == ARRAY_TYPE
	)
	{
		error ("function return type cannot be function or array");
		value_type = integer_type_node;
	}

	/*
	 *	Make a node of the sort we want.
	 */
	t = make_node (FUNCTION_TYPE);
	TREE_TYPE (t) = value_type;
	TYPE_ARG_TYPES (t) = arg_types;

	/*
	 *	If we already have such a type, use the old one
	 *	and free this one.
	 */
	hashcode = TYPE_HASH (value_type) + type_hash_list (arg_types);
	t = type_hash_canon (hashcode, t);

	if (TYPE_SIZE (t) == NULL_TREE)
		layout_type (t);

	return (t);

}	/* end build_function_type */

/*
 ****************************************************************
 *	Build the node for the type of references-to-TO_TYPE	*
 ****************************************************************
 */
tree
build_reference_type (tree to_type)
{
	register tree		t = TYPE_REFERENCE_TO (to_type);
	register OBSTACK	*ambient_obstack = current_obstack;
	register OBSTACK	*ambient_saveable_obstack = saveable_obstack;

	/*
	 *	First, if we already have a type for pointers
	 *	to TO_TYPE, use it.
	 */
	if (t)
		return t;

	/*
	 *	We need a new one.
	 *	If TO_TYPE is permanent, make this permanent too.
	 */
	if (TREE_PERMANENT (to_type))
	{
			current_obstack = &permanent_obstack;
			saveable_obstack = &permanent_obstack;
	}

	t = make_node (REFERENCE_TYPE);
	TREE_TYPE (t) = to_type;

	/*
	 *	Record this type as the pointer to TO_TYPE.
	 */
	TYPE_REFERENCE_TO (to_type) = t;

	layout_type (t);

	current_obstack = ambient_obstack;
	saveable_obstack = ambient_saveable_obstack;

	return (t);

}	/* end build_reference_type */

/*
 ****************************************************************
 *	Construct, lay out and return the type of methods	*
 ****************************************************************
 */
/*
 *	Construct, lay out and return the type of methods
 *	belonging to class BASETYPE and whose arguments and
 *	values are described by TYPE. If that type exists
 *	already, reuse it. TYPE must be a FUNCTION_TYPE node.
 */
tree
build_method_type (tree basetype, tree type)
{
	register tree	t;
	int		hashcode;

	/*
	 *	Make a node of the sort we want.
	 */
	t = make_node (METHOD_TYPE);

	if (TREE_CODE (type) != FUNCTION_TYPE)
		abort ();

	TYPE_METHOD_BASETYPE (t) = basetype;
	TREE_TYPE (t) = TREE_TYPE (type);

	/*
	 *	The actual arglist for this function includes
	 *	a "hidden" argument which is "this".  Put it
	 *	into the list of argument types.
	 */
	TYPE_ARG_TYPES (t) = tree_cons
		(NULL_TREE, build_pointer_type (basetype), TYPE_ARG_TYPES (type));

	/*
	 *	If we already have such a type, use the old one
	 *	and free this one.
	 */
	hashcode = TYPE_HASH (basetype) + TYPE_HASH (type);
	t = type_hash_canon (hashcode, t);

	if (TYPE_SIZE (t) == NULL_TREE)
		layout_type (t);

	return (t);

}	/* end build_method_type */

/*
 ****************************************************************
 *	Construct, lay out and return the type of methods	*
 ****************************************************************
 */
/*
 *	Construct, lay out and return the type of methods
 *	belonging to class BASETYPE and whose arguments and
 *	values are described by TYPE. If that type exists
 *	already, reuse it. TYPE must be a FUNCTION_TYPE node.
 */
tree
build_offset_type (tree basetype, tree type)
{
	register tree	t;
	int		hashcode;

	/*
	 *	Make a node of the sort we want.
	 */
	t = make_node (OFFSET_TYPE);

	TYPE_OFFSET_BASETYPE (t) = basetype;
	TREE_TYPE (t) = type;

	/*
	 *	If we already have such a type, use the
	 *	old one and free this one.
	 */
	hashcode = TYPE_HASH (basetype) + TYPE_HASH (type);
	t = type_hash_canon (hashcode, t);

	if (TYPE_SIZE (t) == NULL_TREE)
		layout_type (t);

	return (t);

}	/* end build_offset_type */
