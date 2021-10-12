/*
 ****************************************************************
 *								*
 *			tree/tree-type.c			*
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
 *		3.0.0, de 05.10.93				*
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
 ******	Protótipos de funções ***********************************
 */
int		type_list_equal (tree l1, tree l2);
int 		simple_cst_equal (tree t1, tree t2);
int		tree_int_cst_equal (tree t1, tree t2);

extern tree	build (enum tree_code, ...);
extern tree	type_for_size (int bits, int unsignedp);

/*
 ******	Hashing of types so that we don't make duplicates *******
 */
/*
 *	Hashing of types so that we don't make duplicates.
 *	The entry point is `type_hash_canon'.
 *
 *	Each hash table slot is a bucket containing a chain
 *	of these structures.
 */
typedef struct type_hash	TYPE_HASH_S;

struct type_hash
{
	TYPE_HASH_S	*next;		/* Next structure in the bucket */
	int		hashcode;	/* Hash code of this type */
	tree		type;		/* The type recorded here */
};

/*
 *	Now here is the hash table.  When recording a type, it is added
 *	to the slot whose index is the hash code mod the table size.
 *	Note that the hash table is used for several kinds of types
 *	(function types, array types and array index range types, for now).
 *	While all these live in the same table, they are completely independent,
 *	and the hash code is computed differently for each of these.
 */
#define TYPE_HASH_SIZE 59

entry TYPE_HASH_S	*type_hash_table[TYPE_HASH_SIZE];

/*
 *	Here is how primitive or already-canonicalized
 *	types' hash codes are made.
 */
#define TYPE_HASH(TYPE) TREE_UID (TYPE)

/*
 ****************************************************************
 *	Compute a hash code for a list of types			*
 ****************************************************************
 */
/*
 *	Compute a hash code for a list of types (chain of TREE_LIST
 *	nodes with types in the TREE_VALUE slots), by adding the
 *	hash codes of the individual types.
 */
int
type_hash_list (tree list)
{
	register int	hashcode;
	register tree	tail;

	for (hashcode = 0, tail = list; tail; tail = TREE_CHAIN (tail))
		hashcode += TYPE_HASH (TREE_VALUE (tail));

	return (hashcode);

}	/* end type_hash_list */

/*
 ****************************************************************
 *	Look in the type hash table for a isomorphic type	*
 ****************************************************************
 */
/*
 *	Look in the type hash table for a type isomorphic to TYPE.
 *	If one is found, return it.  Otherwise return 0.
 */
tree
type_hash_lookup (register int hashcode, register tree type)
{
	register TYPE_HASH_S	*h;

	for (h = type_hash_table[hashcode % TYPE_HASH_SIZE]; h; h = h->next)
	{
		if (h->hashcode != hashcode)
			continue;

		if (TREE_CODE (h->type) != TREE_CODE (type))
			continue;

		if (TREE_TYPE (h->type) != TREE_TYPE (type))
			continue;

		if
		(	TYPE_MAX_VALUE (h->type) != TYPE_MAX_VALUE (type) &&
			tree_int_cst_equal (TYPE_MAX_VALUE (h->type),
					 TYPE_MAX_VALUE (type)) == 0
		)
			continue;

		if
		(	TYPE_MIN_VALUE (h->type) != TYPE_MIN_VALUE (type) &&
			tree_int_cst_equal (TYPE_MIN_VALUE (h->type),
					 TYPE_MIN_VALUE (type)) == 0
		)
			continue;

		if
		(	TYPE_DOMAIN (h->type) == TYPE_DOMAIN (type) ||
			(TREE_CODE (TYPE_DOMAIN (h->type)) == TREE_LIST &&
			TREE_CODE (TYPE_DOMAIN (type)) == TREE_LIST &&
			type_list_equal
				(TYPE_DOMAIN (h->type), TYPE_DOMAIN (type)))
		)
			return (h->type);
	}

	return (NULL_TREE);

}	/* end type_hash_lookup */

/*
 ****************************************************************
 *	Add an entry to the type-hash-table			*
 ****************************************************************
 */
/*
 *	Add an entry to the type-hash-table
 *	for a type TYPE whose hash code is HASHCODE.
 */
void
type_hash_add (register int hashcode, tree type)
{
	register TYPE_HASH_S	*h;

	h = oballoc (sizeof (TYPE_HASH_S));

	h->hashcode = hashcode;
	h->type = type;
	h->next = type_hash_table[hashcode % TYPE_HASH_SIZE];

	type_hash_table[hashcode % TYPE_HASH_SIZE] = h;

}	/* end type_hash_add */

/*
 ****************************************************************
 *	Return the canonical object for an identical type	*
 ****************************************************************
 */
/*
 *	Set to 1 to debug without canonicalization.
 *	Never set by program.
 */
entry int	debug_no_type_hash = 0;

/*
 *	Given TYPE, and HASHCODE its hash code, return the
 *	canonical object for an identical type if one already
 *	exists. Otherwise, return TYPE, and record it as the
 *	canonical object if it is a permanent object.
 *
 *	To use this function, first create a type of the sort
 *	you want. Then compute its hash code from the fields
 *	of the type that make it different from other similar
 *	types. Then call this function and use the value.
 *	This function frees the type you pass in if it is
 *	a duplicate.
 */
tree
type_hash_canon (int hashcode, register tree type)
{
	tree	t1;

	if (debug_no_type_hash)
		return (type);

	if ((t1 = type_hash_lookup (hashcode, type)) != NULL_TREE)
	{
		obstack_free
		(	TREE_PERMANENT (type) ? &permanent_obstack :
							saveable_obstack,
			type
		);

		return (t1);
	}

	/*
	 *	If this is a new type, record it for later reuse.
	 */
	if (current_obstack == &permanent_obstack)
		type_hash_add (hashcode, type);

	return (type);

}	/* end type_hash_canon */

/*
 ****************************************************************
 *	Return 1 if the lists contain the same types		*
 ****************************************************************
 */
/*
 *	Given two lists of types (chains of TREE_LIST
 *	nodes with types in the TREE_VALUE slots)
 *	return 1 if the lists contain the same types
 *	in the same order. Also, the TREE_PURPOSEs must match.
 */
int
type_list_equal (tree l1, tree l2)
{
	register tree	t1 = l1, t2 = l2;

	for (/* acima */; t1 && t2; t1 = TREE_CHAIN (t1), t2 = TREE_CHAIN (t2))
	{
		if (TREE_VALUE (t1) != TREE_VALUE (t2))
			return (0);

		if
		(	TREE_PURPOSE (t1) != TREE_PURPOSE (t2) &&
			!simple_cst_equal (TREE_PURPOSE (t1), TREE_PURPOSE (t2))
		)
			return 0;
	}

	return (t1 == t2);

}	/* end type_list_equal */

/*
 ****************************************************************
 *	Compara constantes					*
 ****************************************************************
 */
/*
 *	Nonzero if integer constants T1 and T2
 *	represent the same constant value.
 */
int
tree_int_cst_equal (register tree t1, register tree t2)
{
	if (t1 == t2)
		return (1);

	if (t1 == NULL_TREE || t2 == NULL_TREE)
		return (0);

	if
	(	TREE_CODE (t1) == INTEGER_CST &&
		TREE_CODE (t2) == INTEGER_CST &&
		TREE_INT_CST_LOW  (t1) == TREE_INT_CST_LOW (t2) &&
		TREE_INT_CST_HIGH (t1) == TREE_INT_CST_HIGH (t2)
	)
		return (1);

	return (0);

}	/* end tree_int_cst_equal */

/*
 ****************************************************************
 *	Nonzero if integers satisfy "<"				*
 ****************************************************************
 */
/*
 *	Nonzero if integer constants T1 and T2 represent
 *	values that satisfy <. The precise way of comparison
 *	depends on their data type.
 */
int
tree_int_cst_lt (register tree t1, register tree t2)
{
	if (t1 == t2)
		return (0);

	if (!TREE_UNSIGNED (TREE_TYPE (t1)))
		return (INT_CST_LT (t1, t2));

	return (INT_CST_LT_UNSIGNED (t1, t2));

}	/* end tree_int_cst_lt */

/*
 ****************************************************************
 *	Compare two constructor-element-type constants		*
 ****************************************************************
 */
int
simple_cst_equal (register tree t1, register tree t2)
{
	register enum tree_code	code1, code2;

	if (t1 == t2)
		return (1);

	if (t1 == NULL_TREE || t2 == NULL_TREE)
		return (0);

	code1 = TREE_CODE (t1);
	code2 = TREE_CODE (t2);

	if   (code1 == NOP_EXPR || code1 == CONVERT_EXPR)
	{
		if (code2 == NOP_EXPR || code2 == CONVERT_EXPR)
			return (simple_cst_equal
				(TREE_OPERAND (t1, 0), TREE_OPERAND (t2, 0)));
		else
			return (simple_cst_equal (TREE_OPERAND (t1, 0), t2));
	}
	elif (code2 == NOP_EXPR || code2 == CONVERT_EXPR)
	{
		return (simple_cst_equal (t1, TREE_OPERAND (t2, 0)));
	}

	if (code1 != code2)
		return (0);

	/*
	 *	...
	 */
	switch (code1)
	{
	    case INTEGER_CST:
		return
		(	TREE_INT_CST_LOW (t1) == TREE_INT_CST_LOW (t2) &&
			TREE_INT_CST_HIGH (t1) == TREE_INT_CST_HIGH (t2)
		);

	    case REAL_CST:
		return (REAL_VALUES_EQUAL
				(TREE_REAL_CST (t1), TREE_REAL_CST (t2)));

	    case STRING_CST:
		return
		(	TREE_STRING_LENGTH (t1) == TREE_STRING_LENGTH (t2) &&
		   !strcmp (TREE_STRING_POINTER (t1), TREE_STRING_POINTER (t2))
		);

	    case CONSTRUCTOR:
		abort ();

	    case VAR_DECL:
	    case PARM_DECL:
	    case CONST_DECL:
		return (0);

	    case PLUS_EXPR:
	    case MINUS_EXPR:
	    case MULT_EXPR:
	    case TRUNC_DIV_EXPR:
	    case TRUNC_MOD_EXPR:
	    case LSHIFT_EXPR:
	    case RSHIFT_EXPR:
		return
		(	simple_cst_equal
				(TREE_OPERAND (t1, 0), TREE_OPERAND (t2, 0)) &&
			simple_cst_equal
				(TREE_OPERAND (t1, 1), TREE_OPERAND (t2, 1))
		);

	    case NEGATE_EXPR:
	    case ADDR_EXPR:
	    case REFERENCE_EXPR:
	    case INDIRECT_REF:
		return (simple_cst_equal
			(TREE_OPERAND (t1, 0), TREE_OPERAND (t2, 0)));

	    default:
		abort ();

	}	/* end switch (code1) */

	/*
	 *	To make LINT happy
	 */
	return (0);

}	/* end simple_cst_equal */

/*
 ****************************************************************
 *	Return OP, stripped of any conversions to wider types	*
 ****************************************************************
 */
/*
 *	Return OP, stripped of any conversions to wider
 *	types as much as is safe. Converting the value back
 *	to OP's type makes a value equivalent to OP.
 *
 *	If FOR_TYPE is nonzero, we return a value which,
 *	if converted to type FOR_TYPE, would be equivalent
 *	to converting OP to type FOR_TYPE.
 *
 *	If FOR_TYPE is nonzero, unaligned bit-field references
 *	may be changed to the narrowest type that can hold
 *	the value, even if they don't exactly fit. Otherwise,
 *	bit-field references are changed to a narrower type
 *	only if they can be fetched directly from memory in that type.
 *
 *	OP must have integer, real or enumeral type.  Pointers
 *	are not allowed!
 *
 *	There are some cases where the obvious value we could return
 *	would regenerate to OP if converted to OP's type, but
 *	would not extend like OP to wider types. If FOR_TYPE
 *	indicates such extension is contemplated, we eschew such
 *	values. For example, if OP is (unsigned short)(signed char)-1,
 *	we avoid returning (signed char)-1 if FOR_TYPE is int,
 *	even though extending that to an unsigned short would
 *	regenerate OP, since the result of extending (signed char)-1
 *	to (int) is different from (int) OP.
 */
tree
get_unwidened (register tree op, tree for_type)
{
	/*
	 *	Set UNS initially if converting OP to FOR_TYPE
	 *	is a zero-extension. TYPE_PRECISION is safe in
	 *	place of type_precision since pointer types are
	 *	not allowed.
	 */
	register tree	type = TREE_TYPE (op);
	register int	final_prec =
			TYPE_PRECISION (for_type != NULL_TREE ? for_type : type);
	register int	uns = (for_type != NULL_TREE && for_type != type &&
				final_prec > TYPE_PRECISION (type) &&
				TREE_UNSIGNED (type));
	register tree	win = op;

	while (TREE_CODE (op) == NOP_EXPR)
	{
		register int	bitschange = TYPE_PRECISION (TREE_TYPE (op)) -
			TYPE_PRECISION (TREE_TYPE (TREE_OPERAND (op, 0)));

		/*
		 *	Truncations are many-one so cannot be removed.
	 	 *	Unless we are later going to truncate down even farther
		 */
		if
		(	bitschange < 0 &&
			final_prec > TYPE_PRECISION (TREE_TYPE (op))
		)
			break;

		/*
		 *	See what's inside this conversion.
		 *	If we decide to strip it, we will set WIN.
		 */
		op = TREE_OPERAND (op, 0);

		/*
		 *	If we have not stripped any zero-extensions
		 *	(uns is 0), we can strip any kind of extension.
	 	 *	If we have previously stripped a zero-extension,
	 	 *	only zero-extensions can safely be stripped.
	 	 *	Any extension can be stripped if the bits it
		 *	would produce are all going to be discarded
		 *	later by truncating to FOR_TYPE.
		 */
		if (bitschange > 0)
		{
			if
			(	!uns ||
				final_prec <= TYPE_PRECISION (TREE_TYPE (op))
			)
				win = op;

			/*
			 *	TREE_UNSIGNED says whether this is a
			 *	zero-extension. Let's avoid computing
			 *	it if it does not affect WIN and if UNS
			 *	will not be needed again.
			 */
			if
			(	(uns || TREE_CODE (op) == NOP_EXPR) &&
				TREE_UNSIGNED (TREE_TYPE (op))
			)
				{ uns = 1; win = op; }
		}
	}

	/*
	 *	Since type_for_size always gives an integer type.
	 */
	if (TREE_CODE (op) == COMPONENT_REF && TREE_CODE (type) != REAL_TYPE)
	{
		int	innerprec =
			(TREE_INT_CST_LOW (DECL_SIZE (TREE_OPERAND (op, 1))) *
				 DECL_SIZE_UNIT (TREE_OPERAND (op, 1)));

		type = type_for_size
			(innerprec, TREE_UNSIGNED (TREE_OPERAND (op, 1)));

		/*
		 *	We can get this structure field in the narrowest
		 *	type it fits in. If FOR_TYPE is 0, do this only
		 *	for a field that matches the narrower type exactly
		 *	and is aligned for it (i.e. mode isn't BI).
	 	 *	The resulting extension to its nominal type (a
		 *	fullword type) must fit the same conditions as
		 *	for other extensions.
		 */
		if
		(	innerprec < TYPE_PRECISION (TREE_TYPE (op)) &&
			(for_type || DECL_MODE (TREE_OPERAND (op, 1)) != BImode)
			&& (! uns || final_prec <= innerprec ||
				TREE_UNSIGNED (TREE_OPERAND (op, 1))) &&
			type != NULL_TREE
		)
		{
			win = build (COMPONENT_REF, type, TREE_OPERAND (op, 0),
						 TREE_OPERAND (op, 1));
			TREE_VOLATILE (win) = TREE_VOLATILE (op);
			TREE_THIS_VOLATILE (win) = TREE_THIS_VOLATILE (op);
		}
	}

	return (win);

}	/* end get_unwidened */

/*
 ****************************************************************
 *	Return OP or a simpler expression for a narrower value	*
 ****************************************************************
 */
/*
 *	Return OP or a simpler expression for a narrower value
 *	which can be sign-extended or zero-extended to give back OP.
 *	Store in *UNSIGNEDP_PTR either 1 if the value should be
 *	zero-extended or 0 if the value should be sign-extended.
 */
tree
get_narrower (register tree op, int *unsignedp_ptr)
{
	register int	uns = 0;
	int		first = 1;
	register tree	win = op;

	while (TREE_CODE (op) == NOP_EXPR)
	{
		register int	bitschange = TYPE_PRECISION (TREE_TYPE (op)) -
			TYPE_PRECISION (TREE_TYPE (TREE_OPERAND (op, 0)));

		/*
		 *	Truncations are many-one so cannot be removed.
		 */
		if (bitschange < 0)
			break;

		/*
		 *	See what's inside this conversion.
		 *	If we decide to strip it, we will set WIN.
		 */
		op = TREE_OPERAND (op, 0);

		if (bitschange > 0)
		{
			/*
			 *	An extension: the outermost one can
			 *	be stripped, but remember whether it
			 *	is zero or sign extension.
			 * 
			 *	Otherwise, if a sign extension has been
			 *	stripped, only sign extensions can now
			 *	be stripped; if a zero extension has
			 *	been stripped, only zero-extensions.
			 */
			if   (first)
				uns = TREE_UNSIGNED (TREE_TYPE (op));
			elif (uns != TREE_UNSIGNED (TREE_TYPE (op)))
				break;

			first = 0;
		}

		/*
		 *	A change in nominal type can always be stripped.
		 */
		win = op;

	}	/* end while (TREE_CODE (op) == NOP_EXPR) */

	/*
	 *	Since type_for_size always gives an integer type.
	 */
	if
	(	TREE_CODE (op) == COMPONENT_REF &&
		TREE_CODE (TREE_TYPE (op)) != REAL_TYPE
	)
		{
			int	innerprec = (TREE_INT_CST_LOW
				(DECL_SIZE (TREE_OPERAND (op, 1))) *
				 DECL_SIZE_UNIT (TREE_OPERAND (op, 1)));
			tree	type = type_for_size
					(innerprec, TREE_UNSIGNED (op));

			/*
			 *	We can get this structure field in a
			 *	narrower type that fits it, but the
			 *	resulting extension to its nominal type
			 *	(a fullword type) must satisfy the same
			 *	conditions as for other extensions.
			 *
	 		 *	Do this only for fields that are aligned
			 *	(not BImode), because when bit-field insns
			 *	will be used there is no advantage in doing
			 *	this.
			 */
			if
			(	innerprec < TYPE_PRECISION (TREE_TYPE (op)) &&
				DECL_MODE (TREE_OPERAND (op, 1)) != BImode &&
				(first || uns == TREE_UNSIGNED
						(TREE_OPERAND (op, 1))) &&
				type != NULL_TREE
			)
			{
				if (first)
					uns = TREE_UNSIGNED
							(TREE_OPERAND (op, 1));

				win = build
				(	COMPONENT_REF,
					type,
					TREE_OPERAND (op, 0),
					TREE_OPERAND (op, 1)
				);

				TREE_VOLATILE (win) = TREE_VOLATILE (op);
				TREE_THIS_VOLATILE (win) = TREE_THIS_VOLATILE (op);
			}
		}

	*unsignedp_ptr = uns;

	return (win);

}	/* end get_narrower */

/*
 ****************************************************************
 *	Return the precision of a type				*
 ****************************************************************
 */
/*
 *	Return the precision of a type, for arithmetic purposes.
 *	Supports all types on which arithmetic is possible
 *	(including pointer types). It's not clear yet what will
 *	be right for complex types.
 */
int
type_precision (register tree type)
{
	return
	(	(TREE_CODE (type) == INTEGER_TYPE ||
		 TREE_CODE (type) == ENUMERAL_TYPE ||
		 TREE_CODE (type) == REAL_TYPE) ?
		TYPE_PRECISION (type) : POINTER_SIZE
	);

}	/* end type_precision */

/*
 ****************************************************************
 *	Nonzero if integer constant C has a permissible value	*
 ****************************************************************
 */
/*
 *	Nonzero if integer constant C has a value that is
 *	permissible for type TYPE (an INTEGER_TYPE).
 */
int
int_fits_type_p (register tree c, register tree type)
{
	if (TREE_UNSIGNED (type))
	{
		return
		(	!INT_CST_LT_UNSIGNED (TYPE_MAX_VALUE (type), c) &&
			!INT_CST_LT_UNSIGNED (c, TYPE_MIN_VALUE (type))
		);
	}
	else
	{
		return
		(	!INT_CST_LT (TYPE_MAX_VALUE (type), c) &&
			!INT_CST_LT (c, TYPE_MIN_VALUE (type))
		);
	}

}	/* end int_fits_type_p */
