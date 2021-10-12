/*
 ****************************************************************
 *								*
 *			tree/tree-node.c			*
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
 *		3.0.0, de 12.10.93				*
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
#include "../h/flags.h"

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
 *	Nonzero means all ..._TYPE nodes should be allocated permanently.
 */
entry int	all_types_permanent;

/*
 ****** Classificação do código da árvore ***********************
 *
 *	Table indexed by tree code giving a string containing
 *	a character classifying the tree code.
 *	Possibilities are t, d, s, c, r and e.  See tree.def.v for details.
 */
#define DEFTREECODE(SYM, NAME, TYPE, LENGTH) TYPE,

const char	*tree_code_type[] =
{
#include "../h/tree.def.v"
};

#undef DEFTREECODE

/*
 ****** Número de operandos da árvore ***************************
 *
 *	Table indexed by tree code giving number of expression
 *	operands beyond the fixed part of the node structure.
 *	Not used for types or decls.
 */
#define DEFTREECODE(SYM, NAME, TYPE, LENGTH) LENGTH,

entry int	tree_code_length[] =
{
#include "../h/tree.def.v"
};

#undef DEFTREECODE

/*
 *	Counter for assigning unique ids to all tree nodes.
 */
extern int	tree_node_counter;

/*
 *	Hash table for uniquizing IDENTIFIER_NODEs by name.
 */
#define MAX_HASH_TABLE	1009

extern tree	hash_table[MAX_HASH_TABLE];	/* id hash buckets */

/*
 *	0 while creating built-in identifiers.
 */
entry int	do_identifier_warnings;

/*
 ******	Protótipos de funções ***********************************
 */
extern void 	error (const char *fmt, ...);
extern tree	build (enum tree_code, ...);
extern tree	build_nt (enum tree_code, ...);
extern void	incomplete_type_error (tree value, tree type);
extern int	language_lvalue_valid (tree exp);
extern tree	fold (tree expr);
extern void	warning (char *fmt, ...);
extern int	tree_int_cst_equal (tree t1, tree t2);
extern tree	convert_units (tree size, int inunits, int outunits);

/*
 ****************************************************************
 *	Return a newly allocated node of code CODE		*
 ****************************************************************
 */
/*
 *	Return a newly allocated node of code CODE.
 *	Initialize the node's unique id and its TREE_PERMANENT flag.
 *	For decl and type nodes, some other fields are initialized.
 *	The rest of the node is initialized to zero.
 *
 *	Achoo!  I got a code in the node.
 */
tree
make_node (register enum tree_code code)
{
	register tree	t;
	register int	type = *tree_code_type[code];
	register int	length;
	register OBSTACK *obstack = current_obstack;
	register int	i;

	switch (type)
	{
		/*
		 ******	A decl node *****************************
		 */
	    case 'd':
		length = sizeof (struct tree_decl);

		/*
		 *	All decls in an inline function need to be saved.
		 *
		 *	PARM_DECLs always go on saveable_obstack, not permanent,
		 *	even though we may make them before the function turns
		 *	on temporary allocation.
		 */
		if   (obstack != &permanent_obstack)
			obstack = saveable_obstack;
		elif (code == PARM_DECL)
			obstack = &maybepermanent_obstack;
		break;

		/*
		 ******	A type node *****************************
		 */
	    case 't':
		length = sizeof (struct tree_type);

		/*
		 *	All data types are put where we can preserve
		 *	them if nec.
		 */
		if (obstack != &permanent_obstack)
		{
			obstack = all_types_permanent ?
					&permanent_obstack : saveable_obstack;
		}
		break;

		/*
		 ******	A stmt node *****************************
		 */
	    case 's':
		length = sizeof (struct tree_common) + 2 * sizeof (int) +
			tree_code_length[code] * sizeof (char *);

		/*
		 *	All stmts are put where we can preserve them if nec.
		 */
		if (obstack != &permanent_obstack)
			obstack = saveable_obstack;
		break;

		/*
		 ******	A reference or expression node **********
		 */
	    case 'r':
	    case 'e':
		obstack = expression_obstack;
		length = sizeof (struct tree_exp) +
			(tree_code_length[code] - 1) * sizeof (char *);
		break;

		/*
		 ******	A constant node *************************
		 */
	    case 'c':
		obstack = expression_obstack;

		/*
		 *	We can't use tree_code_length for this,
		 *	since the number of words is machine-dependent
		 *	due to varying alignment of `double'.
		 */
		if (code == REAL_CST)
		{
			length = sizeof (struct tree_real_cst);
			break;
		}

		/*
		 ****** Something random, like an identifier ****
		 */
	    case 'x':
		length = sizeof (struct tree_common) +
			tree_code_length[code] * sizeof (char *);

		/*
		 *	Identifier nodes are always permanent since
		 *	they are unique in a compiler run.
		 */
		if (code == IDENTIFIER_NODE)
			obstack = &permanent_obstack;

	}	/* end switch (type) */

	t = (tree)obstack_alloc (obstack, length);

	TREE_UID (t) = tree_node_counter++;
	TREE_TYPE (t) = NULL_TREE;
	TREE_CHAIN (t) = NULL_TREE;

	for
	(	i = (length / sizeof (int)) - 1;
		i >= sizeof (struct tree_common) / sizeof (int) - 1;
		i--
	)
		((int *) t)[i] = 0;

	TREE_SET_CODE (t, code);

	if (obstack == &permanent_obstack)
		TREE_PERMANENT (t) = 1;

	if (type == 'd')
	{
		extern int lineno;

		DECL_ALIGN (t) = 1;
		DECL_SIZE_UNIT (t) = 1;
		DECL_VOFFSET_UNIT (t) = 1;
		DECL_SOURCE_LINE (t) = lineno;
		DECL_SOURCE_FILE (t) = input_filename;
	}

	if (type == 't')
	{
		TYPE_ALIGN (t) = 1;
		TYPE_SIZE_UNIT (t) = 1;
		TYPE_MAIN_VARIANT (t) = t;
	}

	if (type == 'c')
	{
		TREE_LITERAL (t) = 1;
	}

	return (t);

}	/* end make_node */

/*
 ****************************************************************
 *	Return a new node with the same contents as NODE	*
 ****************************************************************
 */
/*
 *	Return a new node with the same contents as NODE
 *	except that its TREE_CHAIN is zero and it has a fresh uid.
 */
tree
copy_node (register tree node)
{
	register tree		t;
	register enum tree_code	code = TREE_CODE (node);
	register int		length;
	register int		i;

	switch (*tree_code_type[code])
	{
		/*
		 *	A decl node
		 */
	    case 'd':
		length = sizeof (struct tree_decl);
		break;

		/*
		 *	a type node
		 */
	    case 't':
		length = sizeof (struct tree_type);
		break;

	    case 's':
		length = sizeof (struct tree_common) + 2 * sizeof (int) +
			tree_code_length[code] * sizeof (char *);
		break;

		/*
		 *	A reference or a expression
		 */
	    case 'r':
	    case 'e':
		length = sizeof (struct tree_exp) +
			(tree_code_length[code] - 1) * sizeof (char *);
		break;

		/*
		 *	a constant: we can't use tree_code_length for this,
		 *	since the number of words is machine-dependent
		 *	due to varying alignment of `double'.
		 */
	    case 'c':
		if (code == REAL_CST)
		{
			length = sizeof (struct tree_real_cst);
			break;
		}

		/*
		 *	Something random, like an identifier.
		 */
	    case 'x':
		length = sizeof (struct tree_common) +
			tree_code_length[code] * sizeof (char *);

	}	/* end switch */

	t = (tree)obstack_alloc (current_obstack, length);

	for
	(	i = ((length + sizeof (int) - 1) / sizeof (int)) - 1;
		i >= 0;
		i--
	)
		((int *)t)[i] = ((int *) node)[i];

	TREE_UID (t) = tree_node_counter++;
	TREE_CHAIN (t) = NULL_TREE;

	TREE_PERMANENT (t) = (current_obstack == &permanent_obstack);

	return (t);

}	/* end copy_node */

/*
 ****************************************************************
 *	Return a copy of a chain of nodes			*
 ****************************************************************
 */
/*
 *	Return a copy of a chain of nodes, chained through
 *	the TREE_CHAIN field. For example, this can copy a
 *	list made of TREE_LIST nodes.
 */
tree
copy_list (register tree list)
{
	tree		head;
	register tree	prev, next;

	if (list == NULL_TREE)
		return NULL_TREE;

	head = prev = copy_node (list);
	next = TREE_CHAIN (list);

	while (next)
	{
		TREE_CHAIN (prev) = copy_node (next);
		prev = TREE_CHAIN (prev);
		next = TREE_CHAIN (next);
	}

	return (head);

}	/* end copy_list */

/*
 ****************************************************************
 *	Return an IDENTIFIER_NODE whose name is TEXT		*
 ****************************************************************
 */
/*
 *	Return an IDENTIFIER_NODE whose name is TEXT
 *	(a null-terminated string). If an identifier
 *	with that name has previously been referred to,
 *	the same node is returned this time.
 */
#define HASHBITS 30

tree
get_identifier (register char *text)
{
	register int	hi;
	register int	i;
	register tree	idp;
	register int	len, hash_len;

	/*
	 *	Compute length of text in len.
	 */
	for (len = 0; text[len]; len++)
		/* vazio */;

	/*
	 *	Decide how much of that length to hash on
	 */
	hash_len = len;

	if (warn_id_clash && len > id_clash_len)
		hash_len = id_clash_len;

	/*
	 *	Compute hash code
	 */
	hi = hash_len;

	for (i = 0; i < hash_len; i++)
		hi = ((hi * 613) + (unsigned)(text[i]));

	hi &= (1 << HASHBITS) - 1;
	hi %= MAX_HASH_TABLE;

	/*
	 *	Search table for identifier
	 */
	for (idp = hash_table[hi]; idp; idp = TREE_CHAIN (idp))
	{
		if
		(	IDENTIFIER_LENGTH (idp) == len &&
			!strcmp (IDENTIFIER_POINTER (idp), text)
		)
			return (idp);		/* <-- return if found */
	}

	/*
	 *	Not found; optionally warn about a similar identifier
	 */
	if (warn_id_clash && do_identifier_warnings && len >= id_clash_len)
	{
	    for (idp = hash_table[hi]; idp; idp = TREE_CHAIN (idp))
	    {
		if (!strncmp (IDENTIFIER_POINTER (idp), text, id_clash_len))
		{
			warning
			(	"`%s' and `%s' identical in first n characters",
				IDENTIFIER_POINTER (idp), text
			);
			break;
		}
	    }
	}

	/*
	 *	Not found, create one, add to chain
	 */
	idp = make_node (IDENTIFIER_NODE);
	IDENTIFIER_LENGTH (idp) = len;

	IDENTIFIER_POINTER (idp) = obstack_copy0 (&permanent_obstack, text, len);

	TREE_CHAIN (idp) = hash_table[hi];
	hash_table[hi] = idp;

	return (idp);			/* <-- return if created */

}	/* end get_identifier */

/*
 ****************************************************************
 *	Enable warnings on similar identifiers			*
 ****************************************************************
 */
/*
 *	Enable warnings on similar identifiers (if requested).
 *	Done after the built-in identifiers are created.
 */
void
start_identifier_warnings (void)
{
	do_identifier_warnings = 1;

}	/* end start_identifier_warnings */

/*
 ****************************************************************
 *	Record the size of an identifier node			*
 ****************************************************************
 */
/*
 *	Record the size of an identifier node for
 *	the language in use. This is called by the
 *	language-specific files.
 */
void
set_identifier_size (int size)
{
	tree_code_length[IDENTIFIER_NODE] = size;
}

/*
 ****************************************************************
 *	Return 1 if EXPR is the integer constant zero		*
 ****************************************************************
 */
int
integer_zerop (register tree expr)
{
	return
	(	TREE_CODE (expr) == INTEGER_CST &&
		TREE_INT_CST_LOW (expr) == 0 &&
		TREE_INT_CST_HIGH (expr) == 0
	);

}	/* end integer_zerop */

/*
 ****************************************************************
 *	Return 1 if EXPR is the integer constant one		*
 ****************************************************************
 */
int
integer_onep (register tree expr)
{
	return
	(	TREE_CODE (expr) == INTEGER_CST &&
		TREE_INT_CST_LOW (expr) == 1 &&
		TREE_INT_CST_HIGH (expr) == 0
	);

}	/* end integer_onep */

/*
 ****************************************************************
 *	Return 1 if EXPR is an integer containing all 1's	*
 ****************************************************************
 */
/*
 *	Return 1 if EXPR is an integer containing all 1's
 *	in as much precision as it contains.
 */
int
integer_all_onesp (register tree expr)
{
	register int	prec, uns;

	if (TREE_CODE (expr) != INTEGER_CST)
		return (0);

	uns = TREE_UNSIGNED (TREE_TYPE (expr));

	if (!uns)
	{
		return
		(	TREE_INT_CST_LOW  (expr) == -1 &&
			TREE_INT_CST_HIGH (expr) == -1
		);
	}

	prec = TYPE_PRECISION (TREE_TYPE (expr));

	if (prec >= HOST_BITS_PER_INT)
	{
		return
		(	TREE_INT_CST_LOW (expr) == -1 &&
			TREE_INT_CST_HIGH (expr) ==
					(1 << (prec - HOST_BITS_PER_INT)) - 1
		);
	}
	else
	{
		return (TREE_INT_CST_LOW (expr) == (1 << prec) - 1);
	}

}	/* end integer_all_onesp */

/*
 ****************************************************************
 *	Return the length of a chain of nodes			*
 ****************************************************************
 */
/*
 *	Return the length of a chain of nodes chained
 *	through TREE_CHAIN. We expect a null pointer
 *	to mark the end of the chain. This is the Lisp
 *	primitive `length'.
 */
int
list_length (tree t)
{
	register tree	tail;
	register int	len = 0;

	for (tail = t; tail; tail = TREE_CHAIN (tail))
		len++;

	return (len);

}	/* end list_length */

/*
 ****************************************************************
 *	Concatenate two chains of nodes				*
 ****************************************************************
 */
/*
 *	Concatenate two chains of nodes (chained
 *	through TREE_CHAIN) by modifying the last
 *	node in chain 1 to point to chain 2.
 *	This is the Lisp primitive `nconc'.
 */
tree
chainon (tree op1, tree op2)
{
	register tree	t;

	if (op1)
	{
		for (t = op1; TREE_CHAIN (t); t = TREE_CHAIN (t))
		{
			/*
			 *	Circularity being created
			 */
			if (t == op2)
				abort ();
		}

		TREE_CHAIN (t) = op2;
		return (op1);
	}
	else
	{
		return (op2);
	}

}	/* end chainon */

/*
 ****************************************************************
 *	Return the last node in a chain of nodes		*
 ****************************************************************
 */
/*
 *	Return the last node in a chain of nodes
 *	(chained through TREE_CHAIN).
 */
tree
tree_last (register tree chain)
{
	register tree	next;

	if (chain)
	{
		while (next = TREE_CHAIN (chain))
			chain = next;
	}

	return (chain);

}	/* end tree_last */

/*
 ****************************************************************
 *	Reverse the order of elements of a chain		*
 ****************************************************************
 */
/*
 *	Reverse the order of elements in the chain T,
 *	and return the new head of the chain (old last element).
 */
tree
nreverse (tree t)
{
	register tree	prev = NULL_TREE, decl, next;

	for (decl = t; decl; decl = next)
	{
		next = TREE_CHAIN (decl);
		TREE_CHAIN (decl) = prev;
		prev = decl;
	}

	return (prev);

}	/* end nreverse */

/*
 ****************************************************************
 *	Return the size occupied by an object			*
 ****************************************************************
 */
/*
 *	Return the size nominally occupied by an
 *	object of type TYPE when it resides in memory.
 *	The value is measured in units of bytes, and
 *	its data type is that normally used for type sizes
 *	(which is the first type created by make_signed_type
 *	or make_unsigned_type).
 */
tree
size_in_bytes (register tree type)
{
	if (type == error_mark_node)
		return (integer_zero_node);

	type = TYPE_MAIN_VARIANT (type);

	if (TYPE_SIZE (type) == NULL_TREE)
	{
		incomplete_type_error (NULL_TREE, type);
		return (integer_zero_node);
	}

	return
	(	convert_units
		(	TYPE_SIZE (type),
			TYPE_SIZE_UNIT (type),
			BITS_PER_UNIT
		)
	);

}	/* end size_in_bytes */

/*
 ****************************************************************
 *	Return the size of TYPE (in bytes)			*
 ****************************************************************
 */
/*
 *	Return the size of TYPE (in bytes) as an integer,
 *	or return -1 if the size can vary.
 */
int
int_size_in_bytes (register tree type)
{
	int		size;

	if (type == error_mark_node)
		return (0);

	type = TYPE_MAIN_VARIANT (type);

	if (TYPE_SIZE (type) == NULL_TREE)
		return (-1);

	if (TREE_CODE (TYPE_SIZE (type)) != INTEGER_CST)
		return (-1);

	size = TREE_INT_CST_LOW (TYPE_SIZE (type)) * TYPE_SIZE_UNIT (type);

	return ((size + BITS_PER_UNIT - 1) / BITS_PER_UNIT);

}	/* end int_size_in_bytes */

/*
 ****************************************************************
 *	Return the number of elements for TYPE			*
 ****************************************************************
 */
/*
 *	Return, as an INTEGER_CST node, the number
 *	of elements for TYPE (which is an ARRAY_TYPE).
 */
tree
array_type_nelts (tree type)
{
	tree		index_type = TYPE_DOMAIN (type);

	return
	(    tree_int_cst_equal
		(TYPE_MIN_VALUE (index_type), integer_zero_node) ?

	    TYPE_MAX_VALUE (index_type)	:

	    fold (build (MINUS_EXPR, integer_type_node,
		TYPE_MAX_VALUE (index_type), TYPE_MIN_VALUE (index_type)))
	);

}	/* end array_type_nelts */


/*
 ****************************************************************
 *	Return nonzero if arg is static				*
 ****************************************************************
 */
/*
 *	Return nonzero if arg is static -- a reference
 *	to an object in static storage.  This is not
 *	the same as the C meaning of `static'.
 */
int
staticp (register tree arg)
{
	register enum tree_code		code = TREE_CODE (arg);

	if
	(	(code == VAR_DECL || code == FUNCTION_DECL ||
		code == CONSTRUCTOR) && (TREE_STATIC (arg) ||
		TREE_EXTERNAL (arg))
	)
		return (1);

	if (code == STRING_CST)
		return (1);

	if (code == COMPONENT_REF)
	{
		return
		(	DECL_VOFFSET (TREE_OPERAND (arg, 1)) == NULL_TREE &&
			staticp (TREE_OPERAND (arg, 0))
		);
	}

	if (code == INDIRECT_REF)
		return TREE_LITERAL (TREE_OPERAND (arg, 0));

	if (code == ARRAY_REF)
	{
		if
		(	TREE_CODE (TYPE_SIZE (TREE_TYPE (arg))) == INTEGER_CST
			&& TREE_CODE (TREE_OPERAND (arg, 1)) == INTEGER_CST
		)
			return (staticp (TREE_OPERAND (arg, 0)));
	}

	return (0);

}	/* end staticp */

/*
 ****************************************************************
 *	Return nonzero if REF is an lvalue			*
 ****************************************************************
 */
/*
 *	Return nonzero if REF is an lvalue valid for
 *	this language. Lvalues can be assigned, unless
 *	they have TREE_READONLY. Lvalues can have their
 *	address taken, unless they have TREE_REGDECL.
 */
int
lvalue_p (register tree ref)
{
	register enum tree_code		code = TREE_CODE (ref);

	if (language_lvalue_valid (ref)) switch (code)
	{
	    case COMPONENT_REF:
		return (lvalue_p (TREE_OPERAND (ref, 0)));

	    case STRING_CST:
		return (1);

	    case INDIRECT_REF:
	    case ARRAY_REF:
	    case VAR_DECL:
	    case PARM_DECL:
	    case RESULT_DECL:
	    case ERROR_MARK:
		if
		(	TREE_CODE (TREE_TYPE (ref)) != FUNCTION_TYPE &&
			TREE_CODE (TREE_TYPE (ref)) != METHOD_TYPE
		)
			return (1);
		break;

	    case NEW_EXPR:
		return (1);

	    case CALL_EXPR:
		if (TREE_CODE (TREE_TYPE (ref)) == REFERENCE_TYPE)
			return (1);

	}	/* end if (language_lvalue_valid (ref)) switch (code) */

	return (0);

}	/* end lvalue_p */

/*
 ****************************************************************
 *	Return nonzero if REF is an lvalue			*
 ****************************************************************
 */
/*
 *	Return nonzero if REF is an lvalue valid for
 *	this language; otherwise, print an error message
 *	and return zero.
 */
int
lvalue_or_else (tree ref, char *string)
{
	int	win = lvalue_p (ref);

	if (!win)
		error ("invalid lvalue in %s", string);

	return (win);

}	/* end lvalue_or_else */

/*
 ****************************************************************
 *	Save an expression					*
 ****************************************************************
 */
/*
 *	This should be applied to any node which may
 *	be used in more than one place, but must be
 *	evaluated only once. Normally, the code generator
 *	would reevaluate the node each time; this forces
 *	it to compute it once and save the result.  This
 *	is done by encapsulating the node in a SAVE_EXPR.
 */
tree
save_expr (tree expr)
{
	register tree t = fold (expr);

	/*	If the tree evaluates to a constant, then we
	 *	don't want to hide that fact (i.e. this allows
	 *	further folding, and direct checks for constants).
	 *	Since it is no problem to reevaluate literals,
	 *	we just return the literal node.
	 */
	if
	(	TREE_LITERAL (t) || (TREE_READONLY (t) && !TREE_VOLATILE (t)) ||
		TREE_CODE (t) == SAVE_EXPR
	)
		return (t);

	return (build (SAVE_EXPR, TREE_TYPE (expr), t, NULL));

}	/* end save_expr */

/*
 ****************************************************************
 *	Stabilize a reference					*
 ****************************************************************
 */
/*
 *	Stabilize a reference so that we can use it any number
 *	of times without causing its operands to be evaluated
 *	more than once. Returns the stabilized reference.
 *
 *	Also allows conversion expressions whose operands are
 *	references. Any other kind of expression is returned
 *	unchanged.
 */
tree
stabilize_reference (register tree ref)
{
	register tree		result;
	register enum tree_code	code = TREE_CODE (ref);

	switch (code)
	{
	    case VAR_DECL:
	    case PARM_DECL:
	    case RESULT_DECL:
		result = ref;
		break;

	    case NOP_EXPR:
	    case CONVERT_EXPR:
	    case FLOAT_EXPR:
	    case FIX_TRUNC_EXPR:
	    case FIX_FLOOR_EXPR:
	    case FIX_ROUND_EXPR:
	    case FIX_CEIL_EXPR:
		result = build_nt
			(code, stabilize_reference (TREE_OPERAND (ref, 0)));
		break;

	    case INDIRECT_REF:
		result = build_nt
			(INDIRECT_REF, save_expr (TREE_OPERAND (ref, 0)));
		break;

	    case COMPONENT_REF:
		result = build_nt (COMPONENT_REF, stabilize_reference
			(TREE_OPERAND (ref, 0)), TREE_OPERAND (ref, 1));
		break;

	    case ARRAY_REF:
		result = build_nt (ARRAY_REF, stabilize_reference
			(TREE_OPERAND (ref, 0)), save_expr (TREE_OPERAND (ref, 1)));
		break;

		/*
		 *	If arg isn't a kind of lvalue we recognize,
		 *	make no change. Caller should recognize the
		 *	error for an invalid lvalue.
		 */
	    default:
 		 return (ref);

	    case ERROR_MARK:
		return (error_mark_node);

	} 	/* end switch (code) */

	/*
	 *	Prepara o resultado
	 */
	TREE_TYPE (result) = TREE_TYPE (ref);
	TREE_READONLY (result) = TREE_READONLY (ref);
	TREE_VOLATILE (result) = TREE_VOLATILE (ref);
	TREE_THIS_VOLATILE (result) = TREE_THIS_VOLATILE (ref);

	return (result);

}	/* end stabilize_reference */
