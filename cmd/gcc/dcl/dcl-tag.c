/*
 ****************************************************************
 *								*
 *			dcl-tag.c				*
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
#include <string.h>

#include "../h/tree.h"
#include "../h/flags.h"
#include "../h/c-tree.h"

#include "../tree/tree.h"
#include "../tchk/tchk.h"
#include "../top/top.h"

#include "dcl.h"

/*
 ******	Protótipos de funções ***********************************
 */
#if (0)	/*************************************/
extern tree	chainon (tree, tree);
extern tree	nreverse (tree);
extern void	*xmalloc (int);
extern void	error (const char *, ...);
extern tree	define_label (char *, int, tree);
extern tree	build_let (char *, int, tree, tree, tree, tree);
extern void	print_node (FILE *, char *, tree, int);
#endif	/*************************************/

/*
 ****************************************************************
 *	Push a definition of struct, union or enum tag "name"	*
 ****************************************************************
 */
/*
 *	Push a definition of struct, union or enum tag
 *	"name". "type" should be the type node. We assume
 *	that the tag "name" is not already defined.
 *
 * 	Note that the definition may really be just a
 *	forward reference. In that case, the TYPE_SIZE
 *	will be zero
 */
void
pushtag (tree name, tree type)
{
	register struct binding_level	*b = current_binding_level;

	while (b->tag_transparent)
		b = b->level_chain;

	if (name)
	{
		/*
		 *	Record the identifier as the type's name
	 	 *	if it has none
		 */
		if (TYPE_NAME (type) == NULL_TREE)
			TYPE_NAME (type) = name;

		if (b == global_binding_level)
			b->tags = perm_tree_cons (name, type, b->tags);
		else
			b->tags = saveable_tree_cons (name, type, b->tags);
	}

}	/* end pushtag */

/*
 ****************************************************************
 * Handle when a new declaration has the same name as an old one*
 ****************************************************************
 */
/*
 *	Handle when a new declaration NEWDECL has the same
 *	name as an old one OLDDECL in the same binding contour.
 *	Prints an error message if appropriate.
 *
 *	If safely possible, alter OLDDECL to look like
 *	NEWDECL, and return 1. Otherwise, return 0
 */
int
duplicate_decls (register tree newdecl, register tree olddecl)
{
	int types_match = comptypes (TREE_TYPE (newdecl), TREE_TYPE (olddecl));

	if
	(	TREE_CODE (TREE_TYPE (newdecl)) == ERROR_MARK ||
		TREE_CODE (TREE_TYPE (olddecl)) == ERROR_MARK
	)
		types_match = 0;

	/*
	 *	If this decl has linkage, and the old one does too,
	 *	maybe no error
	 */
	if (TREE_CODE (olddecl) != TREE_CODE (newdecl))
	{
		error_with_decl (newdecl, "`%s' redeclared as different kind of symbol");
		error_with_decl (olddecl, "previous declaration of `%s'");
	}
	else
	{
		if
		(	flag_traditional &&
			TREE_CODE (newdecl) == FUNCTION_DECL &&
			IDENTIFIER_IMPLICIT_DECL (DECL_NAME (newdecl)) ==
								olddecl &&
			DECL_INITIAL (olddecl) == NULL_TREE
		)
		{
			/*
			 *	If -traditional, avoid error for
			 *	redeclaring fcn after implicit decl
			 */
			/* vazio */;
		}
		elif
		(	TREE_CODE (olddecl) == FUNCTION_DECL &&
			DECL_FUNCTION_CODE (olddecl) != NOT_BUILT_IN
		)
	 	{
	 		if (!types_match)
	 			error_with_decl (newdecl, "conflicting types for built-in function `%s'");
	 		elif (extra_warnings)
	 			warning_with_decl (newdecl, "built-in function `%s' redeclared");
	 	}
		elif (!types_match)
		{
			error_with_decl (newdecl, "conflicting types for `%s'");

			/*
			 *	Check for function type mismatch involving
			 *	an empty arglist vs a nonempty one
			 */
			if
			(	TREE_CODE (olddecl) == FUNCTION_DECL &&
				comptypes (TREE_TYPE (TREE_TYPE (olddecl)),
					TREE_TYPE (TREE_TYPE (newdecl))) &&
				((TYPE_ARG_TYPES (TREE_TYPE (olddecl)) ==
								NULL_TREE &&
				DECL_INITIAL (olddecl) == NULL_TREE) ||
				(TYPE_ARG_TYPES (TREE_TYPE (newdecl)) ==
								NULL_TREE &&
				DECL_INITIAL (newdecl) == NULL_TREE))
			)
			{
				/*
				 *	Classify the problem further
				 */
				register tree t = TYPE_ARG_TYPES (TREE_TYPE (olddecl));

				if (t == NULL_TREE)
					t = TYPE_ARG_TYPES (TREE_TYPE (newdecl));

				for (/* vazio */; t; t = TREE_CHAIN (t))
				{
					register tree type = TREE_VALUE (t);

					if (TREE_CHAIN (t) == NULL_TREE && type != void_type_node)
					{
						error ("A parameter list with an ellipsis can't match");
						error ("an empty parameter name list declaration.");
						break;
					}

					if
					(	type == float_type_node ||
						(TREE_CODE (type) == INTEGER_TYPE &&
						(TYPE_PRECISION (type) < TYPE_PRECISION (integer_type_node)))
					)
					{
						error ("An argument type that has a default promotion");
						error ("can't match an empty parameter name list declaration.");
						break;
					}
				}
			}

			error_with_decl (olddecl, "previous declaration of `%s'");
		}
		else	/* types_match */
		{
			char *errmsg = redeclaration_error_message (newdecl, olddecl);

			if (errmsg)
			{
				error_with_decl (newdecl, errmsg);
				error_with_decl (olddecl, "here is the previous declaration of `%s'");
			}
			elif
			(	TREE_CODE (olddecl) == FUNCTION_DECL &&
				DECL_INITIAL (olddecl) != NULL_TREE &&
				TYPE_ARG_TYPES (TREE_TYPE (olddecl)) == NULL_TREE &&
				TYPE_ARG_TYPES (TREE_TYPE (newdecl)) != NULL_TREE
			)
			{
				/*
				 *	Prototype decl follows defn
				 *	w/o prototype
				 */
				warning_with_decl (newdecl, "prototype for `%s'");
				warning_with_decl (olddecl, "follows non-prototype definition here");
			}

			/*
			 *	These bits are logically part of the type
			 */
			if
			(	pedantic &&
				(TREE_READONLY (newdecl) != TREE_READONLY (olddecl) ||
				TREE_THIS_VOLATILE (newdecl) != TREE_THIS_VOLATILE (olddecl))
			)
				warning_with_decl (newdecl, "type qualifiers for `%s' conflict with previous decl");
		}
	}

	if (TREE_CODE (olddecl) == TREE_CODE (newdecl))
	{
		int new_is_definition = (TREE_CODE (newdecl) == FUNCTION_DECL &&
				DECL_INITIAL (newdecl) != NULL_TREE);

		/*
		 *	Copy all the DECL_... slots specified in the new decl
		 *	except for any that we copy here from the old type
		 */
		if (types_match)
		{
			tree	oldtype = TREE_TYPE (olddecl);

			/*
			 *	Merge the data types specified in
			 *	the two decls
			 */
			TREE_TYPE (newdecl) = TREE_TYPE (olddecl) =
				commontype (TREE_TYPE (newdecl), TREE_TYPE (olddecl));

			/*
			 *	Lay the type out, unless already done
			 */
			if (oldtype != TREE_TYPE (newdecl))
			{
				if (TREE_TYPE (newdecl) != error_mark_node)
					layout_type (TREE_TYPE (newdecl));

				if
				(	TREE_CODE (newdecl) != FUNCTION_DECL &&
					TREE_CODE (newdecl) != TYPE_DECL &&
					TREE_CODE (newdecl) != CONST_DECL
				)
				{
					layout_decl (newdecl, 0);
				}
			}
			else
			{
				/*
				 *	Since the type is OLDDECL's,
				 *	make OLDDECL's size go with
				 */
				DECL_SIZE (newdecl) = DECL_SIZE (olddecl);
				DECL_SIZE_UNIT (newdecl) = DECL_SIZE_UNIT (olddecl);

				if (DECL_ALIGN (olddecl) > DECL_ALIGN (newdecl))
					DECL_ALIGN (newdecl) = DECL_ALIGN (olddecl);
			}

			/*
			 *	Merge the type qualifiers
			 */
			if (TREE_READONLY (newdecl))
				TREE_READONLY (olddecl) = 1;

			if (TREE_THIS_VOLATILE (newdecl))
				TREE_THIS_VOLATILE (olddecl) = 1;

			/*
			 *	Merge the initialization information
			 */
			if (DECL_INITIAL (newdecl) == NULL_TREE)
				DECL_INITIAL (newdecl) = DECL_INITIAL (olddecl);

			/*
			 *	Keep the old rtl since we can safely use it
			 */
			DECL_RTL (newdecl) = DECL_RTL (olddecl);
		}
		else
		{
			/*
			 *	If cannot merge, then use the new type
			 *	and qualifiers, and don't preserve the old rtl
			 */
			TREE_TYPE (olddecl) = TREE_TYPE (newdecl);
			TREE_READONLY (olddecl) = TREE_READONLY (newdecl);
			TREE_THIS_VOLATILE (olddecl) = TREE_THIS_VOLATILE (newdecl);
			TREE_VOLATILE (olddecl) = TREE_VOLATILE (newdecl);
		}

		/*
		 *	Merge the storage class information
		 */
		if (TREE_EXTERNAL (newdecl))
		{
			TREE_STATIC (newdecl) = TREE_STATIC (olddecl);
			TREE_EXTERNAL (newdecl) = TREE_EXTERNAL (olddecl);

			/*
			 *	For functions, static overrides non-static
			 */
			if (TREE_CODE (newdecl) == FUNCTION_DECL)
			{
				TREE_PUBLIC (newdecl) &= TREE_PUBLIC (olddecl);

				/*
				 *	This is since we don't automatically
			 	 *	copy the attributes of NEWDECL into OLDDECL
				 */
				TREE_PUBLIC (olddecl) = TREE_PUBLIC (newdecl);

				/*
				 *	If this clears `static', clear it in
				 *	the identifier too
				 */
				if (! TREE_PUBLIC (olddecl))
					TREE_PUBLIC (DECL_NAME (olddecl)) = 0;
			}
			else
			{
				TREE_PUBLIC (newdecl) = TREE_PUBLIC (olddecl);
#define	ENTRY
#ifdef	ENTRY
				TREE_ENTRY (newdecl)  = TREE_ENTRY (olddecl);
#endif	ENTRY
			}
		}
		else
		{
			TREE_STATIC (olddecl) = TREE_STATIC (newdecl);
			TREE_EXTERNAL (olddecl) = 0;
			TREE_PUBLIC (olddecl) = TREE_PUBLIC (newdecl);
#ifdef	ENTRY
			TREE_ENTRY (olddecl)  = TREE_ENTRY (newdecl);
#endif	ENTRY
		}

		/*
		 *	If either decl says `inline', this fn is inline,
	 	 *	unless its definition was passed already
		 */
		if (TREE_INLINE (newdecl) && DECL_INITIAL (olddecl) == NULL_TREE)
			TREE_INLINE (olddecl) = 1;

		/*
		 *	If redeclaring a builtin function,
		 *	and not a definition, it stays built in.
	 	 *	Also preserve various other info from the definition
		 */
		if (TREE_CODE (newdecl) == FUNCTION_DECL && !new_is_definition)
		{
			DECL_SET_FUNCTION_CODE (newdecl, DECL_FUNCTION_CODE (olddecl));
			DECL_RESULT (newdecl) = DECL_RESULT (olddecl);
			DECL_INITIAL (newdecl) = DECL_INITIAL (olddecl);
			DECL_SAVED_INSNS (newdecl) = DECL_SAVED_INSNS (olddecl);
			DECL_RESULT_TYPE (newdecl) = DECL_RESULT_TYPE (olddecl);
			DECL_ARGUMENTS (newdecl) = DECL_ARGUMENTS (olddecl);
			DECL_FRAME_SIZE (newdecl) = DECL_FRAME_SIZE (olddecl);
		}

		/*
		 *	Don't lose track of having output OLDDECL as GDB symbol
		 */
		DECL_BLOCK_SYMTAB_ADDRESS (newdecl) = DECL_BLOCK_SYMTAB_ADDRESS (olddecl);

		memmove
		(	(char *)olddecl + sizeof (struct tree_common),
			(char *)newdecl + sizeof (struct tree_common),
			sizeof (struct tree_decl) - sizeof (struct tree_common)
		);

		return (1);
	}

	/*
	 *	New decl is completely inconsistent with the old one =>
	 *	tell caller to replace the old one
	 */
	return (0);

}	/* end duplicate_decls */

/*
 ****************************************************************
 * Record a decl-node as belonging to the current lexical scope	*
 ****************************************************************
 */
/*
 *	Record a decl-node X as belonging to the current
 *	lexical scope. Check for errors (such as an incompatible
 *	declaration for the same name already seen in the same scope).
 *
 *	Returns either X or an old decl for the same name.
 *	If an old decl is returned, it may have been smashed
 *	to agree with what X says
 */
tree
pushdecl (tree x)
{
	register tree			t;
	register tree			name = DECL_NAME (x);
	register struct binding_level	*b = current_binding_level;

	if (name)
	{
		char		*file;
		int		line;

		t = lookup_name_current_level (name);

		/*
		 *	error_mark_node is 0 for a while
		 *	during initialization!
		 */
		if (t != NULL_TREE && t == error_mark_node)
		{
			t = NULL_TREE;
			error_with_decl (x, "`%s' used prior to declaration");
		}

		if (t != NULL_TREE)
		{
			file = DECL_SOURCE_FILE (t);
			line = DECL_SOURCE_LINE (t);
		}

		/*
		 *	If this decl is `static' and an implicit
		 *	decl was seen previously, warn.	But don't
		 *	complain if -traditional, since traditional
		 *	compilers don't complain
		 */
		if (t != NULL_TREE && duplicate_decls (x, t))
		{
			/*
			 *	We used to warn also for explicit
			 *	extern followed by static,
		 	 *	but sometimes you need to do it that way
			 */
			if
			(	!flag_traditional && TREE_PUBLIC (name) &&
				!TREE_PUBLIC (x) && !TREE_EXTERNAL (x) &&
				IDENTIFIER_IMPLICIT_DECL (name) != NULL_TREE
			)
			{
				warning
				(	"`%s' was declared implicitly `extern' and later `static'",
					IDENTIFIER_POINTER (name)
				);
				warning_with_file_and_line
				(	file, line,
					"previous declaration of `%s'",
					IDENTIFIER_POINTER (name)
				);
			}

			return (t);
		}

		/*
		 *	If declaring a type as a typedef, and the
		 *	type has no known typedef name, install this
		 *	TYPE_DECL as its typedef name
		 */
		if (TREE_CODE (x) == TYPE_DECL)
		{
			if (TYPE_NAME (TREE_TYPE (x)) == NULL_TREE)
				TYPE_NAME (TREE_TYPE (x)) = x;
		}

		/*
		 *	Multiple external decls of the same
		 *	identifier ought to match
		 */
		if
		(	TREE_EXTERNAL (x) &&
			IDENTIFIER_GLOBAL_VALUE (name) != NULL_TREE &&
			(TREE_EXTERNAL (IDENTIFIER_GLOBAL_VALUE (name)) ||
			TREE_PUBLIC (IDENTIFIER_GLOBAL_VALUE (name)))
		)
		{
			if
			(	!comptypes (TREE_TYPE (x),
				  TREE_TYPE (IDENTIFIER_GLOBAL_VALUE (name)))
			)
			{
				warning_with_decl
				(	x,
					"type mismatch with previous external decl"
				);

				warning_with_decl
				(	IDENTIFIER_GLOBAL_VALUE (name),
					 "previous external decl of `%s'"
				);
			}
		}

		/*
		 *	In PCC-compatibility mode, extern decls
		 *	of vars with no current decl take effect ats
		 *	top level no matter where they are
		 */
		if
		(	flag_traditional && TREE_EXTERNAL (x) &&
			lookup_name (name) == NULL_TREE
		)
		{
			tree type = TREE_TYPE (x);

			/*
			 *	But don't do this if the type
			 *	contains temporary nodes
			 */
			while (type)
			{
				if   (!TREE_PERMANENT (type))
				{
					warning_with_decl (x, "Type of `extern' decl is not global");
					break;
				}
				elif
				(	TREE_CODE (type) == FUNCTION_TYPE &&
					TYPE_ARG_TYPES (type) != NULL_TREE
				)
				{
					/*
					 *	The types might not be truly
					 *	local, but the list of arg
					 *	types certainly is temporary.
			 		 *	Since prototypes are
					 *	nontraditional, ok not to
					 *	do the traditional thing
					 */
					break;
				}

				type = TREE_TYPE (type);
			}

			if (type == NULL_TREE)
				b = global_binding_level;
		}

		/*
		 *	This name is new in its binding level.
		 *	 Install the new declaration and return it
		 */
		if (b == global_binding_level)
		{
			/*
			 *	Install a global value.
			 *	If the first global decl has external
			 *	linkage, warn if we later see static one
			 */
			if
			(	IDENTIFIER_GLOBAL_VALUE (name) == NULL_TREE &&
				TREE_PUBLIC (x)
			)
				TREE_PUBLIC (name) = 1;

			IDENTIFIER_GLOBAL_VALUE (name) = x;

			/*
			 *	Don't forget if the function was
			 *	used via an implicit decl
			 */
			if
			(	IDENTIFIER_IMPLICIT_DECL (name) &&
				TREE_USED (IDENTIFIER_IMPLICIT_DECL (name))
			)
				TREE_USED (x) = 1, TREE_USED (name) = 1;

			/*
			 *	Don't forget if its address was
			 *	taken in that way
			 */
			if
			(	IDENTIFIER_IMPLICIT_DECL (name) &&
				TREE_ADDRESSABLE (IDENTIFIER_IMPLICIT_DECL (name))
			)
				TREE_ADDRESSABLE (x) = 1;

			/*
			 *	Warn about mismatches against previous
			 *	implicit decl. If this real decl matches
			 *	the implicit, don't complain
			 */
			if
			(	IDENTIFIER_IMPLICIT_DECL (name) != NULL_TREE &&
				! (TREE_CODE (x) == FUNCTION_DECL &&
				TREE_TYPE (TREE_TYPE (x)) == integer_type_node)
			)
			{
				warning
				(	"`%s' was previously implicitly declared to return `int'",
					IDENTIFIER_POINTER (name)
				);
			}

			/*
			 *	If this decl is `static' and an `extern'
			 *	was seen previously, that is erroneous
			 */
			if
			(	TREE_PUBLIC (name) && ! TREE_PUBLIC (x) &&
				! TREE_EXTERNAL (x)
			)
			{
				if (IDENTIFIER_IMPLICIT_DECL (name))
				{
					warning
					(	"`%s' was declared implicitly "
						"`extern' and later `static'",
						 IDENTIFIER_POINTER (name)
					);
				}
				else
				{
					warning
					(	"`%s' was declared `extern' "
						"and later `static'",
						IDENTIFIER_POINTER (name)
					);
				}
			}
		}
		else
		{
			/*
			 *	Here to install a non-global value
			 */
			tree oldlocal = IDENTIFIER_LOCAL_VALUE (name);
			tree oldglobal = IDENTIFIER_GLOBAL_VALUE (name);

			IDENTIFIER_LOCAL_VALUE (name) = x;

			/*
			 *	If this is an extern function declaration,
			 *	see if we have a global definition for
			 *	the function
			 */
			if
			(	oldlocal == NULL_TREE &&
				oldglobal != NULL_TREE &&
				TREE_CODE (x) == FUNCTION_DECL &&
				TREE_CODE (oldglobal) == FUNCTION_DECL
			)
			{
				/*
				 *	We have one. Their types must agree
				 */
				if
				(	!comptypes (TREE_TYPE (x),
					 TREE_TYPE (IDENTIFIER_GLOBAL_VALUE (name)))
				)
				{
					warning_with_decl (x, "local declaration of `%s' doesn't match global one");
				}
				elif
				(	TREE_INLINE (oldglobal) ||
					DECL_FUNCTION_CODE (oldglobal) != NOT_BUILT_IN ||
					(TYPE_ARG_TYPES (TREE_TYPE (oldglobal)) != NULL_TREE &&
					TYPE_ARG_TYPES (TREE_TYPE (x)) == NULL_TREE)
				)
				{
					/*
					 *	If the global one is inline,
					 *	make the local one inline
					 */
					IDENTIFIER_LOCAL_VALUE (name) = oldglobal;
				}
			}

			/*
			 *	If we have a local external declaration,
			 *	and no file-scope declaration has yet been
			 *	seen, then if we later have a file-scope
			 *	decl it must not be static
			 */
			if
			(	oldlocal == NULL_TREE &&
				oldglobal == NULL_TREE &&
				TREE_EXTERNAL (x) && TREE_PUBLIC (x)
			)
			{
				TREE_PUBLIC (name) = 1;
			}

			/*
			 *	Warn if shadowing an argument at the
			 *	top level of the body
			 */
			if
			(	oldlocal != NULL_TREE && !TREE_EXTERNAL (x) &&
				TREE_CODE (oldlocal) == PARM_DECL &&
				TREE_CODE (x) != PARM_DECL &&
				current_binding_level->level_chain->parm_flag
			)
			{
				warning
				(	"declaration of `%s' shadows a parameter",
					 IDENTIFIER_POINTER (name)
				);
			}
			elif
			(	warn_shadow && !TREE_EXTERNAL (x) &&
				!TREE_INLINE (x)
			)
			{
				/*
				 *	Maybe warn if shadowing something else
				 *	No shadow warnings for vars made for inlining
				 */
				char *warnstring = NOSTR;

				if (oldlocal != NULL_TREE && TREE_CODE (oldlocal) == PARM_DECL)
					warnstring = "declaration of `%s' shadows a parameter";
				elif (oldlocal != NULL_TREE)
					warnstring = "declaration of `%s' shadows previous local";
				elif (IDENTIFIER_GLOBAL_VALUE (name) != NULL_TREE)
					warnstring = "declaration of `%s' shadows global declaration";

				if (warnstring)
					warning (warnstring, IDENTIFIER_POINTER (name));
			}

			/*
			 *	If storing a local value, there may already
			 *	be one (inherited). If so, record it for
			 *	restoration when this binding level ends
			 */
			if (oldlocal != NULL_TREE)
				b->shadowed = tree_cons (name, oldlocal, b->shadowed);
		}

		/*
		 *	Keep count of variables in this level with
		 *	incomplete type
		 */
		if (TYPE_SIZE (TREE_TYPE (x)) == NULL_TREE)
			++b->n_incomplete;

	}	/* end if (name) */

	/*
	 *	Put decls on list in reverse order.
	 *	We will reverse them later if necessary
	 */
	TREE_CHAIN (x) = b->names;
	b->names = x;

	return (x);

}	/* end pushdecl */

/*
 ****************************************************************
 *	Generate an implicit declaration for identifier		*
 ****************************************************************
 */
/*
 *	Generate an implicit declaration for identifier
 *	FUNCTIONID as a function of type int ().
 *	Print a warning if appropriate
 */
tree
implicitly_declare (tree functionid)
{
	register tree	decl;

	/*
	 *	Save the decl permanently so we can warn if definition follows
	 */

#if 0
	/*
	 *	A temporary implicit decl causes a crash in pushdecl.
	 *	In 1.38, fix pushdecl
	 */
	if
	(	flag_traditional || !warn_implicit ||
		current_binding_level == global_binding_level
	)
#endif

		end_temporary_allocation ();

	/*
	 *	We used to reuse an old implicit decl here, but
	 *	this loses with inline functions because it can
	 *	clobber the saved decl chains
	 */
#if 0
	if (IDENTIFIER_IMPLICIT_DECL (functionid) != NULL_TREE)
		decl = IDENTIFIER_IMPLICIT_DECL (functionid);
	else
#endif
		decl = build_decl (FUNCTION_DECL, functionid, default_function_type);

	TREE_EXTERNAL (decl) = 1;
	TREE_PUBLIC (decl) = 1;

	/*
	 *	ANSI standard says implicit declarations are in the
	 *	innermost block. So we record the decl in the standard
	 *	fashion. If flag_traditional is set, pushdecl does
	 *	it top-level
	 */
	pushdecl (decl);
	rest_of_decl_compilation (decl, 0, 0, 0);

	/*
	 *	Only one warning per identifier
	 */
	if
	(	warn_implicit &&
		IDENTIFIER_IMPLICIT_DECL (functionid) == NULL_TREE
	)
	{
		warning
		(	"implicit declaration of function `%s'",
			IDENTIFIER_POINTER (functionid)
		);
	}

	IDENTIFIER_IMPLICIT_DECL (functionid) = decl;

#if 0
	if
	(	flag_traditional || ! warn_implicit ||
		current_binding_level == global_binding_level
	)
#endif
		resume_temporary_allocation ();

	return (decl);

}	/* end implicitly_declare */

/*
 ****************************************************************
 *	Return zero if the declaration is valid			*
 ****************************************************************
 */
/*
 *	Return zero if the declaration NEWDECL is valid
 *	when the declaration OLDDECL (assumed to be for
 *	the same name) has already been seen. Otherwise
 *	return an error message format string with a %s
 *	where the identifier should go
 */
char *
redeclaration_error_message (tree newdecl, tree olddecl)
{
	if (TREE_CODE (newdecl) == TYPE_DECL)
	{
		/*
		 *	Experimental
		 */
		if (flag_tropix)
			return (NOSTR);

		if
		(	(flag_tropix || flag_traditional) &&
			TREE_TYPE (newdecl) == TREE_TYPE (olddecl)
		)
			return (NOSTR);

		return ("redefinition of `%s'");
	}
	elif (TREE_CODE (newdecl) == FUNCTION_DECL)
	{
		/*
		 *	Declarations of functions can insist on internal
		 *	linkage but they can't be inconsistent with internal
		 *	linkage, so there can be no error on that account.
	 	 *	However defining the same name twice is no good.
		 *	However, defining once as extern inline and a second
		 *	time in another way is ok
		 */
		if
		(	DECL_INITIAL (olddecl) != NULL_TREE &&
			DECL_INITIAL (newdecl) != NULL_TREE &&
			!(TREE_INLINE (olddecl) && TREE_EXTERNAL (olddecl) &&
			!(TREE_INLINE (newdecl) && TREE_EXTERNAL (newdecl)))
		)
			return ("redefinition of `%s'");

		return (NOSTR);
	}
	elif (current_binding_level == global_binding_level)
	{
		/*
		 *	Objects declared at top level:
		 *	If at least one is a reference, it's ok
		 */
		if (TREE_EXTERNAL (newdecl) || TREE_EXTERNAL (olddecl))
			return (NOSTR);

		/*
		 *	Reject two definitions
		 */
		if
		(	DECL_INITIAL (olddecl) != NULL_TREE &&
			DECL_INITIAL (newdecl) != NULL_TREE
		)
			return ("redefinition of `%s'");

		/*
		 *	Now we have two tentative defs, or one tentative
		 *	and one real def. Insist that the linkage match
		 */
		if (TREE_PUBLIC (olddecl) != TREE_PUBLIC (newdecl))
			return ("conflicting declarations of `%s'");

		return (NOSTR);
	}
	else
	{
		/*
		 *	Objects declared with block scope:
		 *	Reject two definitions, and reject a definition
	 	 *	together with an external reference
		 */
		 if (!(TREE_EXTERNAL (newdecl) && TREE_EXTERNAL (olddecl)))
			return ("redeclaration of `%s'");
		return (NOSTR);
	}

}	/* end redeclaration_error_message */

/*
 ****************************************************************
 *	Get the label corresponding to identifier as a label	*
 ****************************************************************
 */
/*
 *	Get the LABEL_DECL corresponding to identifier ID
 *	as a label. Create one if none exists so far for
 *	the current function. This function is called for
 *	both label definitions and label references
 */
tree
lookup_label (tree id)
{
	register tree	decl = IDENTIFIER_LABEL_VALUE (id);

	if (decl != NULL_TREE)
		return (decl);

	decl = build_decl (LABEL_DECL, id, NULL_TREE);
	DECL_MODE (decl) = VOIDmode;

	/*
	 *	Mark that the label's definition has not been seen
	 */
	DECL_SOURCE_LINE (decl) = 0;

	IDENTIFIER_LABEL_VALUE (id) = decl;

	named_labels = tree_cons (NULL_TREE, decl, named_labels);

	return (decl);

}	/* end lookup_label */

/*
 ****************************************************************
 *	Define a label, specifying the loc. in the source file	*
 ****************************************************************
 */
/*
 *	Define a label, specifying the location in the
 *	source file. Return the LABEL_DECL node for the label,
 *	if the definition is valid. Otherwise return 0
 */
tree
define_label (char *filename, int line, tree name)
{
	tree		decl = lookup_label (name);

	if (DECL_SOURCE_LINE (decl) != 0)
	{
		error_with_decl (decl, "duplicate label `%s'");
			return (NULL_TREE);
	}
	else
	{
		/*
		 *	Mark label as having been defined
		 */
		DECL_SOURCE_FILE (decl) = filename;
		DECL_SOURCE_LINE (decl) = line;

		return (decl);
	}

}	/* end define_label */

/*
 ****************************************************************
 *	Return the list of declarations of the current level	*
 ****************************************************************
 */
/*
 *	Return the list of declarations of the current
 *	level. Note that this list is in reverse order
 *	unless/until you nreverse it; and when you do
 *	nreverse it, you must store the result back using
 *	`storedecls' or you will lose
 */
tree
getdecls (void)
{
	return (current_binding_level->names);

}	/* end getdecls */

/*
 ****************************************************************
 *	Return the list of type-tags of the current level 	*
 ****************************************************************
 */
/*
 *	Return the list of type-tags (for structs, etc)
 *	of the current level
 */
tree
gettags (void)
{
	return (current_binding_level->tags);

}	/* end gettags */

/*
 ****************************************************************
 *	Store the list of declarations of the current level	*
 ****************************************************************
 */
/*
 *	Store the list of declarations of the current level.
 *	This is done for the parameter declarations of a
 *	function being defined, after they are modified in
 *	the light of any missing parameters
 */
void
storedecls (tree decls)
{
	current_binding_level->names = decls;

}	/* end storedecls */

/*
 ****************************************************************
 *	Similarly, store the list of tags of the current level 	*
 ****************************************************************
 */
void
storetags (tree tags)
{
	current_binding_level->tags = tags;

}	/* end storetags */

/*
 ****************************************************************
 *	Return the structure definition for a name		*
 ****************************************************************
 */
/*
 *	Given NAME, an IDENTIFIER_NODE, return the structure
 *	(or union or enum) definition for that name. Searches
 *	binding levels from BINDING_LEVEL up to the global level.
 *	If THISLEVEL_ONLY is nonzero, searches only the specified
 *	context (but skips any tag-transparent contexts to find
 *	one that is meaningful for tags). FORM says which kind
 *	of type the caller wants; it is RECORD_TYPE or UNION_TYPE
 *	or ENUMERAL_TYPE. If the wrong kind of type is found,
 *	an error is reported
 */
tree
lookup_tag (enum tree_code form, tree name,
		struct binding_level *binding_level, int thislevel_only)
{
	register struct binding_level	*level;

	for (level = binding_level; level; level = level->level_chain)
	{
		register tree	tail;

		for (tail = level->tags; tail; tail = TREE_CHAIN (tail))
		{
			if (TREE_PURPOSE (tail) == name)
			{
				/*
				 *	Definition isn't the kind we
				 * 	were looking for
				 */
				if (TREE_CODE (TREE_VALUE (tail)) != form)
				{
					error
					(	"`%s' defined as wrong kind of tag",
						IDENTIFIER_POINTER (name)
					);
				}

				return (TREE_VALUE (tail));
			}
		}

		if (thislevel_only && ! level->tag_transparent)
			return (NULL_TREE);
	}

	return (NULL_TREE);

}	/* end lookup_tag */

/*
 ****************************************************************
 *	Find the tag that was defined for a type		*
 ****************************************************************
 */
/*
 *	Given a type, find the tag that was defined for
 *	it and return the tag name. Otherwise return 0.
 *	However, the value can never be 0 in the cases in
 *	which this is used
 */
tree
lookup_tag_reverse (tree type)
{
	register struct binding_level	*level;

	for (level = current_binding_level; level; level = level->level_chain)
	{
		register tree	tail;

		for (tail = level->tags; tail; tail = TREE_CHAIN (tail))
		{
			if (TREE_VALUE (tail) == type)
				return (TREE_PURPOSE (tail));
		}
	}

	return (NULL_TREE);

}	/* end lookup_tag_reverse */

/*
 ****************************************************************
 *	Look up a name in the current binding level		*
 ****************************************************************
 */
/*
 *	Look up NAME in the current binding level and
 *	its superiors in the namespace of variables,
 *	functions and typedefs. Return a ..._DECL node
 *	of some kind representing its definition, or return
 *	0 if it is undefined
 */
tree
lookup_name (tree name)
{
	register tree	val;

	if
	(	current_binding_level != global_binding_level &&
		IDENTIFIER_LOCAL_VALUE (name)
	)
		val = IDENTIFIER_LOCAL_VALUE (name);
	else
		val = IDENTIFIER_GLOBAL_VALUE (name);

	if (val && TREE_TYPE (val) == error_mark_node)
		return (error_mark_node);

	return (val);

}	/* end lookup_name */

/*
 ****************************************************************
 *	Similar as above but look only at current binding level	*
 ****************************************************************
 */
/*
 *	Similar to `lookup_name' but look only at
 *	current binding level
 */
tree
lookup_name_current_level (tree name)
{
	register tree	t;

	if (current_binding_level == global_binding_level)
		return (IDENTIFIER_GLOBAL_VALUE (name));

	if (IDENTIFIER_LOCAL_VALUE (name) == NULL_TREE)
		return (NULL_TREE);

	for (t = current_binding_level->names; t; t = TREE_CHAIN (t))
	{
		if (DECL_NAME (t) == name)
			break;
	}

	return (t);

}	/* end lookup_name_current_level */
