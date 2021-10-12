/*
 ****************************************************************
 *								*
 *			dcl-level.c				*
 *								*
 *	Process declarations and symbol lookup for C front end.	*
 *	Also constructs types; the standard scalar types at	*
 *	initialization, and structure, union, array and enum	*
 *	types when they are declared.				*
 *								*
 *	Versão	3.0.0, de 26.08.93				*
 *		3.0.0, de 26.08.93				*
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../h/tree.h"
#include "../h/flags.h"
#include "../h/c-tree.h"

#include "dcl.h"

/*
 ******	Protótipos de funções ***********************************
 */
extern tree	chainon (tree, tree);
extern tree	nreverse (tree);
extern void	*xmalloc (int);
extern void	error (const char *, ...);
extern void	warning_with_decl (tree, char *, ...);
extern tree	define_label (char *, int, tree);
extern tree	build_let (char *, int, tree, tree, tree, tree);
extern void	print_node (FILE *, char *, tree, int);

/*
 ****************************************************************
 *	Decode the string P as a language-specific option	*
 ****************************************************************
 */
/*
 *	Decode the string P as a language-specific option.
 *	Return 1 if it is recognized (and handle it);
 *	return 0 if not recognized.
 */
int
lang_decode_option (register char *p)
{
	if   (!strcmp (p, "-ftraditional") || !strcmp (p, "-traditional"))
		flag_traditional = 1, dollars_in_ident = 1, flag_writable_strings = 1;
	elif (!strcmp (p, "-ftropix"))
		flag_tropix = 1;
	elif (!strcmp (p, "-fsigned-char"))
		flag_signed_char = 1;
	elif (!strcmp (p, "-funsigned-char"))
		flag_signed_char = 0;
	elif (!strcmp (p, "-fno-signed-char"))
		flag_signed_char = 0;
	elif (!strcmp (p, "-fno-unsigned-char"))
		flag_signed_char = 1;
	elif (!strcmp (p, "-fshort-enums"))
		flag_short_enums = 1;
	elif (!strcmp (p, "-fno-short-enums"))
		flag_short_enums = 0;
	elif (!strcmp (p, "-fcond-mismatch"))
		flag_cond_mismatch = 1;
	elif (!strcmp (p, "-fno-cond-mismatch"))
		flag_cond_mismatch = 0;
	elif (!strcmp (p, "-fasm"))
		flag_no_asm = 0;
	elif (!strcmp (p, "-fno-asm"))
		flag_no_asm = 1;
	elif (!strcmp (p, "-ansi"))
		flag_no_asm = 1, dollars_in_ident = 0;
	elif (!strcmp (p, "-Wimplicit"))
		warn_implicit = 1;
	elif (!strcmp (p, "-Wreturn-type"))
		warn_return_type = 1;
	elif (!strcmp (p, "-Wwrite-strings"))
		warn_write_strings = 1;
	elif (!strcmp (p, "-Wcast-qual"))
		warn_cast_qual = 1;
	elif (!strcmp (p, "-Wpointer-arith"))
		warn_pointer_arith = 1;
	elif (!strcmp (p, "-Wstrict-prototypes"))
		warn_strict_prototypes = 1;
	elif (!strcmp (p, "-Wcomment"))
		/* cpp handles this one */;
	elif (!strcmp (p, "-Wcomments"))
		/* cpp handles this one */;
	elif (!strcmp (p, "-Wtrigraphs"))
		/* cpp handles this one */;
	elif (!strcmp (p, "-Wall"))
	{
		extra_warnings = 1;
		warn_implicit = 1;
		warn_return_type = 1;
		warn_unused = 1;
		warn_switch = 1;
	}
	else
		return (0);

	return (1);

}	/* end lang_decode_option */

/*
 ****************************************************************
 *	Imprime o identificador da linguagem			*
 ****************************************************************
 */
void
print_lang_identifier (FILE *file, tree node, int indent)
{
	print_node (file, "global", IDENTIFIER_GLOBAL_VALUE (node), indent + 4);
	print_node (file, "local", IDENTIFIER_LOCAL_VALUE (node), indent + 4);
	print_node (file, "label", IDENTIFIER_LABEL_VALUE (node), indent + 4);
	print_node (file, "implicit", IDENTIFIER_IMPLICIT_DECL (node), indent + 4);
	print_node (file, "error locus", IDENTIFIER_ERROR_LOCUS (node), indent + 4);

}	/* end print_lang_identifier */

/*
 ****************************************************************
 *	Create a new `struct binding_level'			*
 ****************************************************************
 */
struct binding_level *
make_binding_level (void)
{
	return ((struct binding_level *)xmalloc (sizeof (struct binding_level)));

}	/* end make_binding_level */

/*
 ****************************************************************
 *	Nonzero if we are currently in the global binding level	*
 ****************************************************************
 */
int
global_bindings_p (void)
{
	return (current_binding_level == global_binding_level);

}	/* end global_bindings_p */

/*
 ****************************************************************
 *	Guarda o próximo nível					*
 ****************************************************************
 */
void
keep_next_level (void)
{
	keep_next_level_flag = 1;

}	/* end keep_next_level */

/*
 ****************************************************************
 *  Nonzero if the current level needs to have a LET_STMT made	*
 ****************************************************************
 */
int
kept_level_p (void)
{
	return
	(	(current_binding_level->keep_if_subblocks &&
				current_binding_level->blocks != NULL_TREE) ||
		current_binding_level->keep ||
		current_binding_level->names != NULL_TREE
	);

}	/* end kept_level_p */

/*
 ****************************************************************
 *	Identify this binding level as a level of parameters	*
 ****************************************************************
 */
void
declare_parm_level (void)
{
	current_binding_level->parm_flag = 1;

}	/* end declare_parm_level */

/*
 ****************************************************************
 *	Nonzero if currently making parm declarations		*
 ****************************************************************
 */
int
in_parm_level_p (void)
{
	return (current_binding_level->parm_flag);

}	/* end in_parm_level_p */

/*
 ****************************************************************
 *	Enter a new binding level				*
 ****************************************************************
 */
/*
 *	Enter a new binding level. If TAG_TRANSPARENT is
 *	nonzero, do so only for the name space of variables,
 *	not for that of tags
 */
void
pushlevel (int tag_transparent)
{
	register struct binding_level	*newlevel = NULL_BINDING_LEVEL;

	/*
	 *	If this is the top level of a function,
	 *	just make sure that NAMED_LABELS is 0.
	 *	They should have been set to 0 at the end
	 *	of the previous function
	 */
	if (current_binding_level == global_binding_level)
	{
		if (named_labels)
			abort ();
	}

	/*
	 *	Reuse or create a struct for this binding level
	 */
	if (free_binding_level)
	{
		newlevel = free_binding_level;
		free_binding_level = free_binding_level->level_chain;
	}
	else
	{
		newlevel = make_binding_level ();
	}

	/*
	 *	Add this level to the front of the chain (stack)
	 *	of levels that are active
	 */
	*newlevel = clear_binding_level;
	newlevel->level_chain = current_binding_level;
	current_binding_level = newlevel;
	newlevel->tag_transparent = tag_transparent;
	newlevel->keep = keep_next_level_flag;
	keep_next_level_flag = 0;
	newlevel->keep_if_subblocks = keep_next_if_subblocks;
	keep_next_if_subblocks = 0;

}	/* end pushlevel */

/*
 ****************************************************************
 *	Exit a binding level					*
 ****************************************************************
 */
/*
 *	Exit a binding level. Pop the level off, and restore
 *	the state of the identifier-decl mappings that were
 *	in effect when this level was entered.
 *
 *	If KEEP is nonzero, this level had explicit declarations,
 *	so and create a "block" (a LET_STMT node) for the level
 *	to record its declarations and subblocks for symbol table
 *	output.
 *
 *	If FUNCTIONBODY is nonzero, this level is the body
 *	of a function, so create a block as if KEEP were set
 *	and also clear out all label names.
 *
 *	If REVERSE is nonzero, reverse the order of decls
 *	before putting them into the LET_STMT
 */
tree
poplevel (int keep, int reverse, int functionbody)
{
	register tree	link;
	/*
	 *	The chain of decls was accumulated in reverse order.
	 *	Put it into forward order, just for cleanliness
	 */
	tree		decls;
	tree		tags = current_binding_level->tags;
	tree		subblocks = current_binding_level->blocks;
	tree		block = NULL_TREE;

	keep |= current_binding_level->keep;

	/*
	 *	This warning is turned off because it causes
	 *	warnings for declarations like `extern struct foo *x'
	 */
#if 0
	/*
	 *	Warn about incomplete structure types in this level
	 */
	for (link = tags; link; link = TREE_CHAIN (link))
	{
		tree	type = TREE_VALUE (link);
		char	*errmsg;

		if (TYPE_SIZE (TREE_VALUE (link)) != 0)
			continue;

		switch (TREE_CODE (type))
		{
		    case RECORD_TYPE:
			errmsg = "`struct %s' incomplete in scope ending here";
			break;

		    case UNION_TYPE:
			errmsg = "`union %s' incomplete in scope ending here";
			break;

		    case ENUMERAL_TYPE:
			errmsg = "`enum %s' incomplete in scope ending here";
			break;
		}

		/*
		 *	If this type has a typedef-name,
		 *	the TYPE_NAME is a TYPE_DECL
		 */
		if (TREE_CODE (TYPE_NAME (type)) == IDENTIFIER_NODE)
			error (errmsg, IDENTIFIER_POINTER (TYPE_NAME (type)));
		else
			error (errmsg, IDENTIFIER_POINTER (DECL_NAME (TYPE_NAME (type))));
	}
#endif /* 0 */

	/*
	 *	Get the decls in the order they were written.
	 *	Usually current_binding_level->names is in reverse order.
	 *	But parameter decls were previously put in forward order.
	 */
	if (reverse)
		current_binding_level->names
			= decls = nreverse (current_binding_level->names);
	else
		decls = current_binding_level->names;

	/*
	 *	If there were any declarations or structure tags
	 *	in that level, or if this level is a function body,
	 *	create a LET_STMT to record them for the life of
	 *	this function
	 */
	if
	(	keep || functionbody ||
		(current_binding_level->keep_if_subblocks &&
						subblocks != NULL_TREE)
	)
	{
		block = build_let
			(	NOSTR,
				0,
				keep ? decls : NULL_TREE,
				subblocks,
				NULL_TREE,
				keep ? tags : NULL_TREE
			);
	}

	/*
	 *	In each subblock, record that this is its superior
	 */
	for (link = subblocks; link; link = TREE_CHAIN (link))
		STMT_SUPERCONTEXT (link) = block;

	/*
	 *	Clear out the meanings of the local variables of this level;
	 *	also record in each decl which block it belongs to
	 */
	for (link = decls; link; link = TREE_CHAIN (link))
	{
		/*
		 *	If the ident. was used via a local extern decl,
		 *	don't forget that fact
		 */
		if (DECL_NAME (link) != NULL_TREE)
		{
			if (TREE_USED (link) && TREE_EXTERNAL (link))
				TREE_USED (DECL_NAME (link)) = 1;

			IDENTIFIER_LOCAL_VALUE (DECL_NAME (link)) = NULL_TREE;
		}

		DECL_CONTEXT (link) = block;
	}

	/*
	 *	Restore all name-meanings of the outer levels
	 *	that were shadowed by this level
	 */
	for (link = current_binding_level->shadowed; link; link = TREE_CHAIN (link))
		IDENTIFIER_LOCAL_VALUE (TREE_PURPOSE (link)) = TREE_VALUE (link);

	/*
	 *	If the level being exited is the top level of a function,
	 *	check over all the labels
	 */
	if (functionbody)
	{
		/*
		 *	Clear out the definitions of all label names,
		 *	since their scopes end here
		 */
		for (link = named_labels; link; link = TREE_CHAIN (link))
		{
			if (DECL_SOURCE_LINE (TREE_VALUE (link)) == 0)
			{
				error
				(	"label `%s' used somewhere above "
					"but not defined",
					 IDENTIFIER_POINTER (DECL_NAME (TREE_VALUE (link)))
				);

				/*
				 *	Avoid crashing later
				 */
				define_label (input_filename, 1, DECL_NAME (TREE_VALUE (link)));
			}
			elif (warn_unused && !TREE_USED (TREE_VALUE (link)))
			{
				warning_with_decl
				(	TREE_VALUE (link), 
					"label `%s' defined but not used"
				);
			}

		IDENTIFIER_LABEL_VALUE (DECL_NAME (TREE_VALUE (link))) = NULL_TREE;

		}

		named_labels = NULL_TREE;
	}

	/*
	 *	Pop the current level, and free the structure for reuse
	 */
	{
		register struct binding_level	*level = current_binding_level;

		current_binding_level = current_binding_level->level_chain;
		level->level_chain = free_binding_level;
		free_binding_level = level;
	}

	if (functionbody)
	{
		DECL_INITIAL (current_function_decl) = block;

		/*
		 *	If this is the top level block of a function,
	 	 *	the vars are the function's parameters.
	 	 *	Don't leave them in the LET_STMT because they are
	 	 *	found in the FUNCTION_DECL instead
		 */
		STMT_VARS (block) = NULL_TREE;
	}
	elif (block)
	{
		current_binding_level->blocks =
			chainon (current_binding_level->blocks, block);

	}
	elif (subblocks)
	{
		/*
		 *	If we did not make a block for the level
		 *	just exited, any blocks made for inner levels
		 *	(since they cannot be recorded as subblocks
		 *	in that level) must be carried forward so they
		 *	will later become subblocks of something else
		 */
		current_binding_level->blocks
			= chainon (current_binding_level->blocks, subblocks);
	}

	if (block)
		TREE_USED (block) = 1;

	return (block);

}	/* end poplevel */
