/*
 ****************************************************************
 *								*
 *			dcl-parm.c				*
 *								*
 *	Process declarations and symbol lookup for C front end.	*
 *	Also constructs types; the standard scalar types at	*
 *	initialization, and structure, union, array and enum	*
 *	types when they are declared.				*
 *								*
 *	Versão	3.0.0, de 01.01.94				*
 *		3.0.0, de 01.01.94				*
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

#include ../$GEN.gen/config.h

#include "../h/tree.h"
#include "../h/c-tree.h"

#include "../tree/tree.h"
#include "../top/top.h"

#include "dcl.h"

/*
 ****************************************************************
 *	Make TYPE a complete type based on INITIAL_VALUE	*
 ****************************************************************
 */
/*
 *	Make TYPE a complete type based on INITIAL_VALUE.
 *	Return 0 if successful, 1 if INITIAL_VALUE can't
 *	be decyphered, 2 if there was no information
 *	(in which case assume 1 if DO_DEFAULT)
 */
int
complete_array_type (tree type, tree initial_value, int do_default)
{
	register tree	maxindex = NULL_TREE;
	int		value = 0, temporary;

	temporary = (TREE_PERMANENT (type) && allocation_temporary_p ());

	/*
	 *	Don't put temporary nodes in permanent type
	 */
	if (temporary)
		end_temporary_allocation ();

	/*
	 *	Note MAXINDEX is really the maximum index,
	 *	one less than the size
	 */
	if (initial_value)
	{
		if (TREE_CODE (initial_value) == STRING_CST)
		{
			int	eltsize = TREE_INT_CST_LOW (TYPE_SIZE (TREE_TYPE (TREE_TYPE (initial_value))));

			maxindex = build_int_2 (TREE_STRING_LENGTH (initial_value) / eltsize - 1, 0);
		}
		elif (TREE_CODE (initial_value) == CONSTRUCTOR)
		{
			register int	nelts = list_length (CONSTRUCTOR_ELTS (initial_value));

			maxindex = build_int_2 (nelts - 1, 0);
		}
		else
		{
			/*
			 *	Make an error message unless that
			 *	happened already
			 */
			if (initial_value != error_mark_node)
				value = 1;

			/*
			 *	Prevent further error messages
			 */
			maxindex = build_int_2 (1, 0);
		}
	}

	if (!maxindex)
	{
		if (do_default)
			maxindex = build_int_2 (1, 0);

		value = 2;
	}

	if (maxindex)
	{
		TYPE_DOMAIN (type) = build_index_type (maxindex);

		if (!TREE_TYPE (maxindex))
			TREE_TYPE (maxindex) = TYPE_DOMAIN (type);
	}

	/*
	 *	Lay out the type now that we can get the real answer
	 */
	layout_type (type);

	if (temporary)
		resume_temporary_allocation ();

	return (value);

}	/* end complete_array_type */

/*
 ****************************************************************
 *	Make a variant type in the proper way for C		*
 ****************************************************************
 */
/*
 *	Make a variant type in the proper way for C,
 *	propagating qualifiers down to the element
 *	type of an array
 */
tree
c_build_type_variant (tree type, int constp, int volatilep)
{
	if (TREE_CODE (type) != ARRAY_TYPE)
		return (build_type_variant (type, constp, volatilep));

	return
	(	build_array_type
		(	c_build_type_variant
			(	TREE_TYPE (type),
				constp,
				volatilep
			),
		TYPE_DOMAIN (type)
		)
	);

}	/* end c_build_type_variant */

/*
 ****************************************************************
 *	Decode the parameter					*
 ****************************************************************
 */
/*
 *	Decode the parameter-list info for a function type
 *	or function definition. The argument is the value
 *	returned by `get_parm_info' (or made in parse.y if
 *	there is an identifier list instead of a parameter
 *	decl list). These two functions are separate because
 *	when a function returns or receives functions then
 *	each is called multiple times but the order of calls
 *	is different. The last call to `grokparms' is always
 *	the one that contains the formal parameter names of
 *	a function definition.
 *
 *	Store in `last_function_parms' a chain of the decls
 *	of parms. Also store in `last_function_parm_tags' a
 *	chain of the struct and union tags declared among the parms.
 *
 *	Return a list of arg types to use in the FUNCTION_TYPE
 *	for this function.
 *
 *	FUNCDEF_FLAG is nonzero for a function definition, 0
 *	for a mere declaration. A nonempty identifier-list
 *	gets an error message when FUNCDEF_FLAG is zero
 */
tree
grokparms (tree parms_info, int funcdef_flag)
{
	tree		first_parm = TREE_CHAIN (parms_info);

	last_function_parms = TREE_PURPOSE (parms_info);
	last_function_parm_tags = TREE_VALUE (parms_info);

	if (warn_strict_prototypes && first_parm == NULL_TREE && !funcdef_flag)
		warning ("function declaration isn't a prototype");

	if
	(	first_parm != NULL_TREE &&
		TREE_CODE (TREE_VALUE (first_parm)) == IDENTIFIER_NODE
	)
	{
		if (! funcdef_flag)
			warning ("parameter names (without types) in function declaration");

		last_function_parms = first_parm;
		return (NULL_TREE);
	}
	else
	{
		tree		parm, typelt;

		/*
		 *	We no longer test FUNCDEF_FLAG. If the arg
		 *	types are incomplete in a declaration, they
		 *	must include undefined tags. These tags can
		 *	never be defined in the scope of the declaration,
	 	 *	so the types can never be completed, and no
		 *	call can be compiled successfully
		 */
#if 0	/* --------------------------------------------------- */
		/*
		 *	In a fcn definition, arg types must be complete
		 */
		if (funcdef_flag)
#endif	/* --------------------------------------------------- */

		for
		(	parm = last_function_parms, typelt = first_parm;
			parm;
			parm = TREE_CHAIN (parm)
		)
		{
			/*
			 *	Skip over any enumeration constants
			 *	declared here
			 */
			if (TREE_CODE (parm) == PARM_DECL)
			{
				/*
				 *	Barf if the parameter itself
				 *	has an incomplete type
				 */
				tree		type = TREE_VALUE (typelt);

				if (TYPE_SIZE (type) == NULL_TREE)
				{
					if (funcdef_flag && DECL_NAME (parm) != NULL_TREE)
					{
						error
						(	"parameter `%s' has incomplete type",
							IDENTIFIER_POINTER (DECL_NAME (parm))
						);
					}
					else
					{
						warning ("parameter has incomplete type");
					}

					if (funcdef_flag)
					{
						TREE_VALUE (typelt) = error_mark_node;
						TREE_TYPE (parm) = error_mark_node;
					}
				}
#if 0	/* --------------------------------------------------- */
				else
				{
					/*
					 *	This has been replaced by
					 *	parm_tags_warning which uses a
					 *	more accurate criterion for what
					 *	to warn about
					 */
					/*
					 *	Now warn if is a pointer to
					 *	an incomplete type
					 */
					while
					(	TREE_CODE (type) == POINTER_TYPE ||
						TREE_CODE (type) == REFERENCE_TYPE
					)
						type = TREE_TYPE (type);

					type = TYPE_MAIN_VARIANT (type);

					if (TYPE_SIZE (type) == 0)
					{
						if (DECL_NAME (parm) != 0)
						{
							warning
							(	"parameter `%s' points to incomplete type",
								 IDENTIFIER_POINTER (DECL_NAME (parm))
							);
						}
						else
						{
							warning ("parameter points to incomplete type");
						}
					}
				}
#endif	/* --------------------------------------------------- */
				typelt = TREE_CHAIN (typelt);
			}
		}

		return (first_parm);
	}

}	/* end grokparms */

/*
 ****************************************************************
 *	Return a tree_list node with info on a parameter list	*
 ****************************************************************
 */
/*
 *	Return a tree_list node with info on a parameter
 *	list just parsed. The TREE_PURPOSE is a chain of
 *	decls of those parms. The TREE_VALUE is a list of
 *	structure, union and enum tags defined. The TREE_CHAIN
 *	is a list of argument types to go in the FUNCTION_TYPE.
 *	This tree_list node is later fed to `grokparms'.
 *
 *	VOID_AT_END nonzero means append `void' to the end
 *	of the type-list. Zero means the parmlist ended
 *	with an ellipsis so don't append `void'
 */
tree
get_parm_info (int void_at_end)
{
	register tree	decl, types = NULL_TREE;
	int		erred = 0;
	tree		tags = gettags ();
	tree		parms = nreverse (getdecls ());

	/*
	 *	Just `void' (and no ellipsis) is special
	 *	There are really no parms
	 */
	if
	(	void_at_end && parms != NULL_TREE &&
		TREE_CHAIN (parms) == NULL_TREE &&
		TREE_TYPE (parms) == void_type_node &&
		DECL_NAME (parms) == NULL_TREE
	)
	{
		parms = NULL_TREE;
		storedecls (NULL_TREE);
		return (saveable_tree_cons (NULL_TREE, NULL_TREE, void_list_node));
	}

	storedecls (parms);

	/*
	 *	There may also be declarations for enumerators if
	 *	an enumeration type is declared among the parms.
	 *	Ignore them here
	 */
	for (decl = parms; decl; decl = TREE_CHAIN (decl))
	{
		if (TREE_CODE (decl) == PARM_DECL)
		{
			/*
			 *	Since there is a prototype, args are
			 *	passed in their declared types
			 */
			tree		type = TREE_TYPE (decl);

			DECL_ARG_TYPE (decl) = type;

#ifdef PROMOTE_PROTOTYPES
			if
			(	TREE_CODE (type) == INTEGER_TYPE &&
				TYPE_PRECISION (type) < TYPE_PRECISION (integer_type_node)
			)
				DECL_ARG_TYPE (decl) = integer_type_node;
#endif

			types = saveable_tree_cons (NULL_TREE, TREE_TYPE (decl), types);

			if
			(	TREE_VALUE (types) == void_type_node &&
				! erred && DECL_NAME (decl) == NULL_TREE
			)
			{
				error ("`void' in parameter list must be the entire list");
				erred = 1;
			}
		}
	}

	if (void_at_end)
	{
		return
		(	saveable_tree_cons
			(	parms,
				tags,
				nreverse
				(	saveable_tree_cons
					(	NULL_TREE,
						void_type_node,
						types
					)
				)
			)
		);
	}

	return (saveable_tree_cons (parms, tags, nreverse (types)));

}	/* end get_parm_info */

/*
 ****************************************************************
 *  Warn about any struct, union or enum tags defined within	*
 ****************************************************************
 */
/*
 *	At end of parameter list, warn about any struct,
 *	union or enum tags defined within. Do so because
 *	these types cannot ever become complete
 */
void
parmlist_tags_warning (void)
{
	tree		elt;
	static int	already;

	for (elt = current_binding_level->tags; elt; elt = TREE_CHAIN (elt))
	{
		enum		tree_code code = TREE_CODE (TREE_VALUE (elt));

		warning
		(	"`%s %s' declared inside parameter list",
			(code == RECORD_TYPE ? "struct" :
			code == UNION_TYPE ? "union" : "enum"),
			IDENTIFIER_POINTER (TREE_PURPOSE (elt))
		);

		if (! already)
		{
			warning ("its scope is only this definition or declaration,");
			warning ("which is probably not what you want.");
			already = 1;
		}
	}

}	/* end parmlist_tags_warning */

/*
 ****************************************************************
 *	Get the struct, enum or union with tag NAME		*
 ****************************************************************
 */
/*
 *	Get the struct, enum or union (CODE says which)
 *	with tag NAME. Define the tag as a forward-reference
 *	if it is not defined
 */
tree
xref_tag (enum tree_code code, tree name)
{
	/*
	 *	If a cross reference is requested, look up the type
	 *	already defined for this tag and return it
	 */
	int		temporary = allocation_temporary_p ();
	register tree	ref = lookup_tag (code, name, current_binding_level, 0);

	if (ref)
		return (ref);

	if (current_binding_level == global_binding_level && temporary)
		end_temporary_allocation ();

	/*
	 *	If no such tag is yet defined, create a forward-reference
	 *	node and record it as the "definition". When a real
	 *	declaration of this type is found, the forward-reference
	 *	will be altered into a real type
	 */
	ref = make_node (code);

	if (code == ENUMERAL_TYPE)
	{
		/*
		 *	(In ANSI, Enums can be referred to only if
		 *	already defined.)
		 */
		if (pedantic)
			warning ("ANSI C forbids forward references to `enum' types");

		/*
		 *	Give the type a default layout like unsigned int
	 	 *	to avoid crashing if it does not get defined
		 */
		TYPE_MODE (ref) = SImode;
		TYPE_ALIGN (ref) = TYPE_ALIGN (unsigned_type_node);
		TREE_UNSIGNED (ref) = 1;
		TYPE_PRECISION (ref) = TYPE_PRECISION (unsigned_type_node);
		TYPE_MIN_VALUE (ref) = TYPE_MIN_VALUE (unsigned_type_node);
		TYPE_MAX_VALUE (ref) = TYPE_MAX_VALUE (unsigned_type_node);
	}

	pushtag (name, ref);

	if (current_binding_level == global_binding_level && temporary)
		resume_temporary_allocation ();

	return (ref);

}	/* end xref_tag */

/*
 ****************************************************************
 *	Make sure that the tag is defined			*
 ****************************************************************
 */
/*
 *	Make sure that the tag NAME is defined *in the
 *	current binding level* at least as a forward
 *	reference. CODE says which kind of tag NAME ought to be
 */
tree
start_struct (enum tree_code code, tree name)
{
	/*
	 *	If there is already a tag defined at this binding
	 *	level (as a forward reference), just return it
	 */
	register tree	ref = NULL_TREE;

	if (name != NULL_TREE)
		ref = lookup_tag (code, name, current_binding_level, 1);

	if (ref && TREE_CODE (ref) == code)
	{
		if (TYPE_FIELDS (ref))
			error
			(	(code == UNION_TYPE ?
					"redefinition of `union %s'" :
					"redefinition of `struct %s'"
				),
				IDENTIFIER_POINTER (name)
			);

			return (ref);
	}

	/*
	 *	Otherwise create a forward-reference just
	 *	so the tag is in scope
	 */
	ref = make_node (code);
	pushtag (name, ref);
	return (ref);

}	/* end start_struct */

/*
 ****************************************************************
 *	Process the specs					*
 ****************************************************************
 */
/*
 *	Process the specs, declarator (NULL if omitted) and
 *	width (NULL if omitted) of a structure component,
 *	returning a FIELD_DECL node. WIDTH is non-NULL for
 *	bit fields only, and is an INTEGER_CST node.
 *
 *	This is done during the parsing of the struct
 *	declaration. The FIELD_DECL nodes are chained together
 *	and the lot of them are ultimately passed to
 *	`build_struct' to make the RECORD_TYPE node
 */
tree
grokfield (char *filename, int line, tree declarator, tree declspecs, tree width)
{
	register tree	value;

	value = grokdeclarator (declarator, declspecs, FIELD, 0);
	finish_decl (value, NULL_TREE, NULL_TREE);
	DECL_INITIAL (value) = width;

	return (value);

}	/* end grokfield */

/*
 ****************************************************************
 *	Fill in the fields of a RECORD_TYPE or UNION_TYPE node	*
 ****************************************************************
 */
/*
 *	Fill in the fields of a RECORD_TYPE or UNION_TYPE node,
 *	T. FIELDLIST is a chain of FIELD_DECL nodes for the fields
 */
tree
finish_struct (register tree t, register tree fieldlist)
{
	register tree	x;
	int		old_momentary, round_up_size = 1;

	/*
 	 *	If this type was previously laid out as a
	 *	forward reference, make sure we lay it out again
	 */
	TYPE_SIZE (t) = NULL_TREE;

	if (in_parm_level_p ())
	{
		warning
		(	(TREE_CODE (t) == UNION_TYPE ?
			"union defined inside parms" :
			"structure defined inside parms")
		);
	}

	old_momentary = suspend_momentary ();

	if (fieldlist == NULL_TREE && pedantic)
	{
		warning
		(	(TREE_CODE (t) == UNION_TYPE ?
			"union has no members" : "structure has no members")
		);
	}

	/*
	 *	Install struct as DECL_CONTEXT of each field decl.
	 *	Also process specified field sizes. Set DECL_SIZE_UNIT
	 *	to the specified size, or 0 if none specified. The
	 *	specified size is found in the DECL_INITIAL. Store
	 *	0 there, except for ": 0" fields (so we can find them
	 *	and delete them, below)
	 */
	for (x = fieldlist; x; x = TREE_CHAIN (x))
	{
		DECL_CONTEXT (x) = t;
		DECL_SIZE_UNIT (x) = 0;

		/*
		 *	If any field is const, the structure type
		 *	is pseudo-const
		 */
		if (TREE_READONLY (x))
		{
			C_TYPE_FIELDS_READONLY (t) = 1;
		}
		else
		{
			/*
			 *	A field that is pseudo-const
			 *	makes the structure likewise
			 */
			tree		t1 = TREE_TYPE (x);

			while (TREE_CODE (t1) == ARRAY_TYPE)
				t1 = TREE_TYPE (t1);

			if
			(	(TREE_CODE (t1) == RECORD_TYPE ||
				TREE_CODE (t1) == UNION_TYPE) &&
				C_TYPE_FIELDS_READONLY (t1)
			)
				C_TYPE_FIELDS_READONLY (t) = 1;
		}

		/*
		 *	Detect invalid bit-field size
		 */
		if
		(	DECL_INITIAL (x) &&
			TREE_CODE (DECL_INITIAL (x)) != INTEGER_CST
		)
		{
			error_with_decl (x, "bit-field `%s' width not an integer constant");
			DECL_INITIAL (x) = NULL_TREE;
		}

		/*
		 *	Detect invalid bit-field type
		 */
		if
		(	DECL_INITIAL (x) &&
			TREE_CODE (TREE_TYPE (x)) != INTEGER_TYPE &&
			TREE_CODE (TREE_TYPE (x)) != ENUMERAL_TYPE
		)
		{
			error_with_decl (x, "bit-field `%s' has invalid type");
			DECL_INITIAL (x) = NULL_TREE;
		}

		if
		(	DECL_INITIAL (x) && pedantic &&
			TREE_TYPE (x) != integer_type_node &&
			TREE_TYPE (x) != unsigned_type_node
		)
		 	warning_with_decl (x, "bit-field `%s' type invalid in ANSI C");

		/*
		 *	Detect and ignore out of range field width
		 */
		if (DECL_INITIAL (x))
		{
			register int	width;

			width = TREE_INT_CST_LOW (DECL_INITIAL (x));

			if (width < 0)
			{
				DECL_INITIAL (x) = NULL_TREE;
				warning_with_decl (x, "negative width in bit-field `%s'");
			}
			elif (width == 0 && DECL_NAME (x) != NULL_TREE)
			{
				error_with_decl (x, "zero width for bit-field `%s'");
				DECL_INITIAL (x) = NULL_TREE;
			}
			elif (width > TYPE_PRECISION (TREE_TYPE (x)))
			{
				DECL_INITIAL (x) = NULL_TREE;
				warning_with_decl (x, "width of `%s' exceeds its type");
			}
		}

		/*
		 *	Process valid field width
		 */
		if (DECL_INITIAL (x))
		{
			register int	width;

			width = TREE_INT_CST_LOW (DECL_INITIAL (x));

			if (width == 0)
			{
				/*
				 *	field size 0 => mark following
				 *	field as "aligned"
				 */
				if (TREE_CHAIN (x))
				{
					DECL_ALIGN (TREE_CHAIN (x)) =
						MAX (DECL_ALIGN (TREE_CHAIN (x)), EMPTY_FIELD_BOUNDARY);
				}
				else
				{

					/*
					 *	field of size 0 at the end =>
					 *	round up the size
					 */
					round_up_size = EMPTY_FIELD_BOUNDARY;
				}
			}
			else
			{
				DECL_INITIAL (x) = NULL_TREE;
				DECL_SIZE_UNIT (x) = width;
				TREE_PACKED (x) = 1;

				/*
				 *	Traditionally a bit field is
				 *	unsigned even if declared signed
				 */
				if
				(	flag_traditional &&
					TREE_CODE (TREE_TYPE (x)) == INTEGER_TYPE
				)
					TREE_TYPE (x) = unsigned_type_node;
			}
		}
		else
		{
			/*
			 *	Non-bit-fields are aligned for their type
			 */
			DECL_ALIGN (x) = MAX (DECL_ALIGN (x), TYPE_ALIGN (TREE_TYPE (x)));
		}

	}	/* end for (x = ...) */

	/*
	 *	Now DECL_INITIAL is null on all members except
	 *	for zero-width bit-fields. And they have already
	 *	done their work
	 *
	 *	Delete all zero-width bit-fields from the front
	 *	of the fieldlist
	 */
	while (fieldlist && DECL_INITIAL (fieldlist))
		fieldlist = TREE_CHAIN (fieldlist);

	/*
	 *	Delete all such members from the rest of the fieldlist
	 */
	for (x = fieldlist; x; /* vazio */)
	{
		if (TREE_CHAIN (x) && DECL_INITIAL (TREE_CHAIN (x)))
			TREE_CHAIN (x) = TREE_CHAIN (TREE_CHAIN (x));
		else
			x = TREE_CHAIN (x);
	}

	/*
	 *	Delete all duplicate fields from the fieldlist.
	 *	Anonymous fields aren't duplicates
	 */
	for (x = fieldlist; x && TREE_CHAIN (x); /* vazio */)
	{
		if (DECL_NAME (TREE_CHAIN (x)) == NULL_TREE)
		{
			x = TREE_CHAIN (x);
		}
		else
		{
			register tree	y = fieldlist;

			while (1)
			{
				if (DECL_NAME (y) == DECL_NAME (TREE_CHAIN (x)))
					break;

				if (y == x)
					break;

				y = TREE_CHAIN (y);
			}

			if (DECL_NAME (y) == DECL_NAME (TREE_CHAIN (x)))
			{
				error_with_decl (TREE_CHAIN (x), "duplicate member `%s'");
				TREE_CHAIN (x) = TREE_CHAIN (TREE_CHAIN (x));
			}
			else
			{
				x = TREE_CHAIN (x);
			}
		}
	}

	/*
	 *	Now we have the final fieldlist. Record it, then
	 *	lay out the structure or union (including the fields)
	 */
	TYPE_FIELDS (t) = fieldlist;

	/*
	 *	If there's a :0 field at the end, round
	 *	the size to the EMPTY_FIELD_BOUNDARY
	 */
	TYPE_ALIGN (t) = round_up_size;

	for (x = TYPE_MAIN_VARIANT (t); x; x = TYPE_NEXT_VARIANT (x))
	{
		TYPE_FIELDS (x) = TYPE_FIELDS (t);
		TYPE_ALIGN (x) = TYPE_ALIGN (t);
	}

	layout_type (t);

	/*
	 *	Promote each bit-field's type to int if
	 *	it is narrower than that
	 */
	for (x = fieldlist; x; x = TREE_CHAIN (x))
	{
		if
		(	TREE_PACKED (x) &&
			TREE_CODE (TREE_TYPE (x)) == INTEGER_TYPE &&
			(TREE_INT_CST_LOW (DECL_SIZE (x)) * DECL_SIZE_UNIT (x)
				< TYPE_PRECISION (integer_type_node))
		)
			TREE_TYPE (x) = integer_type_node;
	}

	/*
	 *	If this structure or union completes the type of any
	 *	previous variable declaration, lay it out and output its rtl
	 */
	if (current_binding_level->n_incomplete != 0)
	{
		tree		decl;

		for
		(	decl = current_binding_level->names;
			decl;
			decl = TREE_CHAIN (decl)
		)
		{
			if
			(	TREE_TYPE (decl) == t &&
				TREE_CODE (decl) != TYPE_DECL
			)
			{
				int		toplevel;

				toplevel = global_binding_level == current_binding_level;

				layout_decl (decl, 0);
				rest_of_decl_compilation (decl, 0, toplevel, 0);

				if (! toplevel)
					expand_decl (decl, NULL_TREE);

				--current_binding_level->n_incomplete;
			}
			elif
			(	TYPE_SIZE (TREE_TYPE (decl)) == NULL_TREE &&
				TREE_CODE (TREE_TYPE (decl)) == ARRAY_TYPE
			)
			{
				tree		element = TREE_TYPE (decl);

				while (TREE_CODE (element) == ARRAY_TYPE)
					element = TREE_TYPE (element);

				if (element == t)
					layout_array_type (TREE_TYPE (decl));
			}
		}
	}

	resume_momentary (old_momentary);

	return (t);

}	/* end finish_struct */
