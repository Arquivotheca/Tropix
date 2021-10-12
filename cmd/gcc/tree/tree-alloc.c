/*
 ****************************************************************
 *								*
 *			tree/tree-alloc.c			*
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
 *		3.0.0, de 08.10.93				*
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

#include <string.h>

#include "../h/tree.h"
#include "../h/obstack.h"

/*
 ******	Alocadores de memória ***********************************
 */
#define obstack_chunk_alloc	xmalloc
#define obstack_chunk_free	free

extern void	*xmalloc (long);
extern void	free (void);

/*
 ******	Diversas definições de OBSTACK **************************
 */
/*
 *	Tree nodes of permanent duration are allocated in this obstack.
 *	They are the identifier nodes, and everything outside of
 *	the bodies and parameters of function definitions.
 */
entry OBSTACK	permanent_obstack;

/*
 *	The initial RTL, and all ..._TYPE nodes, in a function
 *	are allocated in this obstack.  Usually they are freed at the
 *	end of the function, but if the function is inline they are saved.
 */
entry OBSTACK	maybepermanent_obstack;

/*
 *	The contents of the current function definition are allocated
 *	in this obstack, and all are freed at the end of the function.
 */
entry OBSTACK	temporary_obstack;

/*
 *	The tree nodes of an expression are allocated
 *	in this obstack, and all are freed at the end of the expression.
 */
entry OBSTACK	momentary_obstack;

/*
 *	This points at either permanent_obstack or maybepermanent_obstack.
 */
entry OBSTACK	*saveable_obstack;

/*
 *	This is same as saveable_obstack during parse and expansion phase;
 *	it points to temporary_obstack during optimization.
 *	This is the obstack to be used for creating rtl objects.
 */
entry OBSTACK	*rtl_obstack;

/*
 *	This points at either permanent_obstack or temporary_obstack.
 */
entry OBSTACK	*current_obstack;

/*
 *	This points at either permanent_obstack or temporary_obstack
 *	or momentary_obstack.
 */
entry OBSTACK	*expression_obstack;

/*
 *	Addresses of first objects in some obstacks.
 *	This is for freeing their entire contents.
 */
entry char	*maybepermanent_firstobj;
entry char	*temporary_firstobj;
entry char	*momentary_firstobj;

/*
 ******	Stack of places to restore the momentary obstack back to *******
 */
typedef struct momentary_level	MOMENTARY_LEVEL;

struct momentary_level
{
	MOMENTARY_LEVEL *prev;  /* Pointer back to previous such level */
	char		*base;	/* First object allocated within this level */
	OBSTACK		*obstack; /* Val. of exp._obstack saved at entry to this level */
};

entry MOMENTARY_LEVEL *momentary_stack;

/*
 *	Counter for assigning unique ids to all tree nodes.
 */
entry int	tree_node_counter = 0;

/*
 *	Hash table for uniquizing IDENTIFIER_NODEs by name.
 */
#define MAX_HASH_TABLE	1009

entry tree	hash_table[MAX_HASH_TABLE];	/* id hash buckets */

/*
 ****************************************************************
 * Init data for node creation, at the beginning of compilation *
 ****************************************************************
 */
void
init_tree (void)
{
	obstack_init (&permanent_obstack);

	obstack_init (&temporary_obstack);
	temporary_firstobj = (char *)obstack_alloc (&temporary_obstack, 0);
	obstack_init (&momentary_obstack);
	momentary_firstobj = (char *)obstack_alloc (&momentary_obstack, 0);
	obstack_init (&maybepermanent_obstack);
	maybepermanent_firstobj
		= (char *)obstack_alloc (&maybepermanent_obstack, 0);

	current_obstack = &permanent_obstack;
	expression_obstack = &permanent_obstack;
	rtl_obstack = saveable_obstack = &permanent_obstack;
	tree_node_counter = 1;
	memsetl ((long *)hash_table, 0, sizeof (hash_table) / sizeof (long));

}	/* end init_tree */

/*
 ****************************************************************
 *  Start allocating on the temporary (per function) obstack	*
 ****************************************************************
 */
/*
 *	Start allocating on the temporary (per function) obstack.
 *	This is done in start_function before parsing the function body,
 *	and before each initialization at top level, and to go back
 *	to temporary allocation after doing end_temporary_allocation.
 */
void
temporary_allocation (void)
{
	current_obstack = &temporary_obstack;
	expression_obstack = &temporary_obstack;
	rtl_obstack = saveable_obstack = &maybepermanent_obstack;
	momentary_stack = (MOMENTARY_LEVEL *)0;

}	/* end temporary_allocation */

/*
 ****************************************************************
 *	Start allocating on the permanent obstack		*
 ****************************************************************
 */
/*
 *	Start allocating on the permanent obstack but don't
 *	free the temporary data.  After calling this, call
 *	`permanent_allocation' to fully resume permanent allocation status.
 */
void
end_temporary_allocation (void)
{
	current_obstack = &permanent_obstack;
	expression_obstack = &permanent_obstack;
	rtl_obstack = saveable_obstack = &permanent_obstack;

}	/* end end_temporary_allocation */

/*
 ****************************************************************
 *	Resume allocating on the temporary obstack		*
 ****************************************************************
 */
/*
 *	Resume allocating on the temporary obstack, undoing
 *	effects of `end_temporary_allocation'.
 */
void
resume_temporary_allocation (void)
{
	current_obstack = &temporary_obstack;
	expression_obstack = &temporary_obstack;
	rtl_obstack = saveable_obstack = &maybepermanent_obstack;
}

/*
 ****************************************************************
 *	Nonzero if temporary allocation is currently in effect	*
 ****************************************************************
 */
/*
 *	Nonzero if temporary allocation is currently in effect.
 *	Zero if currently doing permanent allocation.
 */
int
allocation_temporary_p (void)
{
	return (current_obstack == &temporary_obstack);

}	/* end allocation_temporary_p */

/*
 ****************************************************************
 *	Go back to allocating on the permanent obstack		*
 ****************************************************************
 */
/*
 *	Go back to allocating on the permanent obstack
 *	and free everything in the temporary obstack.
 *	This is done in finish_function after fully compiling a function.
 */
void
permanent_allocation (void)
{
 	/*
	 *	Free up previous temporary obstack data
	 */
	obstack_free (&temporary_obstack, temporary_firstobj);
	obstack_free (&momentary_obstack, momentary_firstobj);
	obstack_free (&maybepermanent_obstack, maybepermanent_firstobj);

	current_obstack = &permanent_obstack;
	expression_obstack = &permanent_obstack;
	rtl_obstack = saveable_obstack = &permanent_obstack;

}	/* end permanent_allocation */

/*
 ****************************************************************
 *  Save permanently everything on the maybepermanent_obstack	*
 ****************************************************************
 */
void
preserve_data (void)
{
	maybepermanent_firstobj
		= (char *)obstack_alloc (&maybepermanent_obstack, 0);

}	/* end preserve_data */

/*
 ****************************************************************
 *	Allocate SIZE bytes in the current obstack		*
 ****************************************************************
 */
/*
 *	Allocate SIZE bytes in the current obstack
 *	and return a pointer to them.
 *	In practice the current obstack is always the temporary one.
 */
void *
oballoc (int size)
{
	return ((void *)obstack_alloc (current_obstack, size));

}	/* end oballoc */

/*
 ****************************************************************
 *	Free the object PTR in the current obstack		*
 ****************************************************************
 */
/*
 *	Free the object PTR in the current obstack
 *	as well as everything allocated since PTR.
 *	In practice the current obstack is always the temporary one.
 */
void
obfree (char *ptr)
{
	obstack_free (current_obstack, ptr);

}	/* end obfree */

/*
 ****************************************************************
 *	Allocate SIZE bytes in the permanent obstack		*
 ****************************************************************
 */
/*
 *	Allocate SIZE bytes in the permanent obstack
 *	and return a pointer to them.
 */
char *
permalloc (long size)
{
	return ((char *)obstack_alloc (&permanent_obstack, size));

}	/* end permalloc */

/*
 ****************************************************************
 *	Allocate SIZE bytes in the saveable obstack		*
 ****************************************************************
 */
/*
 *	Allocate SIZE bytes in the saveable obstack
 *	and return a pointer to them.
 */
char *
savealloc (int size)
{
	return ((char *)obstack_alloc (saveable_obstack, size));

}	/* end savealloc */

/*
 ****************************************************************
 *	Start a level of momentary allocation			*
 ****************************************************************
 */
/*
 *	Start a level of momentary allocation.
 *	In C, each compound statement has its own level
 *	and that level is freed at the end of each statement.
 *	All expression nodes are allocated in the momentary allocation level.
 */
void
push_momentary (void)
{
	MOMENTARY_LEVEL	*tem = (MOMENTARY_LEVEL *)obstack_alloc
				(&momentary_obstack, sizeof (MOMENTARY_LEVEL));
	tem->prev = momentary_stack;
	tem->base = (char *)obstack_base (&momentary_obstack);
	tem->obstack = expression_obstack;
	momentary_stack = tem;
	expression_obstack = &momentary_obstack;

}	/* end push_momentary */

/*
 ****************************************************************
 *Free all the storage in the current momentary-allocation level*
 ****************************************************************
 */
/*
 *	Free all the storage in the current momentary-allocation level.
 *	In C, this happens at the end of each statement.
 */
void
clear_momentary (void)
{
	obstack_free (&momentary_obstack, momentary_stack->base);

}	/* end clear_momentary */

/*
 ****************************************************************
 *	Discard a level of momentary allocation			*
 ****************************************************************
 */
/*
 *	Discard a level of momentary allocation.
 *	In C, this happens at the end of each compound statement.
 *	Restore the status of expression node allocation
 *	that was in effect before this level was created.
 */
void
pop_momentary (void)
{
	MOMENTARY_LEVEL *tem = momentary_stack;
	momentary_stack = tem->prev;
	obstack_free (&momentary_obstack, tem);
	expression_obstack = tem->obstack;

}	/* end pop_momentary */

/*
 ****************************************************************
 *	Call when starting to parse a declaration		*
 ****************************************************************
 */
/*
 *	Call when starting to parse a declaration:
 *	make expressions in the declaration last the length of the function.
 *	Returns an argument that should be passed to resume_momentary later.
 */
int
suspend_momentary (void)
{
	register int	tem = (expression_obstack == &momentary_obstack);

	expression_obstack = saveable_obstack;
	return (tem);
}

/*
 ****************************************************************
 *	Call when finished parsing a declaration		*
 ****************************************************************
 */
/*
 *	Call when finished parsing a declaration:
 *	restore the treatment of node-allocation that was
 *	in effect before the suspension.
 *	YES should be the value previously returned by suspend_momentary.
 */
void
resume_momentary (int yes)
{
	if (yes)
		expression_obstack = &momentary_obstack;

}	/* end resume_momentary */
