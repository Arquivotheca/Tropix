/*
 ****************************************************************
 *								*
 *			dcl-grok.c				*
 *								*
 *	Process declarations and symbol lookup for C front end.	*
 *	Also constructs types; the standard scalar types at	*
 *	initialization, and structure, union, array and enum	*
 *	types when they are declared.				*
 *								*
 *	Versão	3.0.0, de 31.12.93				*
 *		3.0.0, de 17.01.94				*
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
#include <stdlib.h>
#include <string.h>

#include ../$GEN.gen/config.h

#include "../h/tree.h"
#include "../h/c-tree.h"
#include "../h/c-parse.h"

#include "../tree/tree.h"
#include "../tchk/tchk.h"
#include "../top/top.h"

#include "dcl.h"

/*
 ****************************************************************
 *	Determine the name and type of the object declared	*
 ****************************************************************
 */
/*
 *	Given declspecs and a declarator, determine the name
 *	and type of the object declared and construct a ..._DECL
 *	node for it. (In one case we can return a ..._TYPE
 *	node instead. For invalid input we sometimes return 0.)
 *
 *	DECLSPECS is a chain of tree_list nodes whose value
 *	fields are the storage classes and type specifiers.
 *
 * 	DECL_CONTEXT says which syntactic context this
 *	declaration is in:
 *
 *		NORMAL for most contexts. Make a VAR_DECL
 *			or FUNCTION_DECL or TYPE_DECL.
 *		FUNCDEF for a function definition. Like NORMAL
 *			but a few different error messages in
 *			each case. Return value may be zero
 *			meaning this definition is too screwy
 *			to try to parse.
 *		PARM for a parameter declaration (either
 *			within a function prototype or before
 *			a function body). Make a PARM_DECL,
 *			or return void_type_node.
 *		TYPENAME if for a typename (in a cast or sizeof).
 *			Don't make a DECL node; just return the
 *			..._TYPE node.
 *		FIELD for a struct or union field; make a
 *			FIELD_DECL.
 *		INITIALIZED is 1 if the decl has an initializer.
 *
 *	 In the TYPENAME case, DECLARATOR is really an absolute
 *	declarator. It may also be so in the PARM case, for a
 *	prototype where the argument type is specified but not
 *	the name.
 *
 *	This function is where the complicated C meanings of
 *	`static' and `extern' are intrepreted
 */
tree
grokdeclarator (tree declarator, tree declspecs,
			enum decl_context decl_context, int initialized)
{
	register int	specbits = 0;
	tree		spec;
	tree		type = NULL_TREE;
	int		longlong = 0;
	int		constp;
	int		volatilep;
	int		inlinep;
	int		explicit_int = 0;
	int		explicit_char = 0;
	char		*name;
	tree		typedef_type = NULL_TREE;
	int		funcdef_flag = 0;
	int		resume_temporary = 0;
	enum tree_code	innermost_code = ERROR_MARK;

	if (decl_context == FUNCDEF)
		funcdef_flag = 1, decl_context = NORMAL;

	if (flag_traditional && allocation_temporary_p ())
	{
		resume_temporary = 1;
		end_temporary_allocation ();
	}

	/*
	 *	Look inside a declarator for the name being declared
	 *	and get it as a string, for an error message
	 */
	{
		register tree	decl = declarator;

		name = NOSTR;

		while (decl) switch (TREE_CODE (decl))
		{
		    case ARRAY_REF:
		    case INDIRECT_REF:
		    case CALL_EXPR:
			innermost_code = TREE_CODE (decl);
			decl = TREE_OPERAND (decl, 0);
			break;

		    case IDENTIFIER_NODE:
			name = IDENTIFIER_POINTER (decl);
			decl = NULL_TREE;
			break;

		    default:
			abort ();
		}

		if (name == NOSTR)
			name = "type name";
	}

	/*
	 *	A function definition's declarator must
	 *	have the form of a function declarator
	 */
	if (funcdef_flag && innermost_code != CALL_EXPR)
		return (NULL_TREE);

	/*
	 *	Anything declared one level down from the
	 *	top level must be one of the parameters of
	 *	a function (because the body is at least
	 *	two levels down)
	 */
	if
	(	decl_context == NORMAL &&
		current_binding_level->level_chain == global_binding_level
	)
		decl_context = PARM;

	/*
	 *	Look through the decl specs and record which ones
	 *	appear. Some typespecs are defined as built-in
	 *	typenames. Others, the ones that are modifiers of
	 *	other types, are represented by bits in SPECBITS:
	 *	set the bits for the modifiers that appear. Storage
	 *	class keywords are also in SPECBITS.
	 *
	 *	If there is a typedef name or a type, store the
	 *	type in TYPE. This includes builtin typedefs
	 *	such as `int'.
	 *
	 *	Set EXPLICIT_INT if the type is `int' or `char'
	 *	and did not come from a user typedef.
	 *
	 *	Set LONGLONG if `long' is mentioned twice
	 */
	for (spec = declspecs; spec; spec = TREE_CHAIN (spec))
	{
		register int	i;
		register tree	id = TREE_VALUE (spec);

		if (id == ridpointers[RID_INT])
			explicit_int = 1;

		if (id == ridpointers[RID_CHAR])
			explicit_char = 1;

		if (TREE_CODE (id) == IDENTIFIER_NODE)
			for (i = RID_FIRST_MODIFIER; i < RID_MAX; i++)
		{
			if (ridpointers[i] == id)
			{
				if (i == RID_LONG && specbits & (1 << i))
				{
					if (pedantic)
						warning ("duplicate `%s'", IDENTIFIER_POINTER (id));
					elif (longlong)
						warning ("`long long long' is too long for GCC");
					else
						longlong = 1;
				}
				elif (specbits & (1 << i))
				{
					warning ("duplicate `%s'", IDENTIFIER_POINTER (id));
				}

				specbits |= 1 << i;
				goto found;
			}
		}

		if   (type)
		{
			error ("two or more data types in declaration of `%s'", name);
		}
		elif (TREE_CODE (id) == IDENTIFIER_NODE)
		{
			register tree	t = lookup_name (id);

			if (!t || TREE_CODE (t) != TYPE_DECL)
				error ("`%s' fails to be a typedef or built in type", IDENTIFIER_POINTER (id));
			else
				type = TREE_TYPE (t);
		}
		elif (TREE_CODE (id) != ERROR_MARK)
		{
			type = id;
		}

		found: {}

	}	/* end for (spec = ...) */

	typedef_type = type;

	/*
	 *	No type at all: default to `int', and set
	 *	EXPLICIT_INT because it was not a user-defined typedef
	 */
	if (type == NULL_TREE)
	{
		if
		(	funcdef_flag && warn_return_type &&
			! (specbits & ((1 << RID_LONG) | (1 << RID_SHORT)
				  | (1 << RID_SIGNED) | (1 << RID_UNSIGNED)))
		)
			warn_about_return_type = 1;

		explicit_int = 1;
		type = integer_type_node;
	}

	/*
	 *	Now process the modifiers that were specified
	 *	and check for invalid combinations
	 *
	 *	Long double is a special combination
	 */
	if ((specbits & 1 << RID_LONG) && type == double_type_node)
	{
		specbits &= ~ (1 << RID_LONG);
		type = long_double_type_node;
	}

	/*
	 *	Check all other uses of type modifiers
	 */
	if
	(	specbits & ((1 << RID_LONG) | (1 << RID_SHORT) |
				(1 << RID_UNSIGNED) | (1 << RID_SIGNED))
	)
	{
		if   (TREE_CODE (type) != INTEGER_TYPE)
		{
			error ("long, short, signed or unsigned invalid for `%s'", name);
		}
		elif ((specbits & 1 << RID_LONG) && (specbits & 1 << RID_SHORT))
		{
			error ("long and short specified together for `%s'", name);
		}
		elif (((specbits & 1 << RID_LONG) || (specbits & 1 << RID_SHORT)) && explicit_char)
		{
			error ("long or short specified with char for `%s'", name);
		}
		elif ((specbits & 1 << RID_SIGNED) && (specbits & 1 << RID_UNSIGNED))
		{
			error ("signed and unsigned given together for `%s'", name);
		}
		else
		{
			if (!explicit_int && !explicit_char && pedantic)
				warning ("long, short, signed or unsigned used invalidly for `%s'", name);

			if (specbits & 1 << RID_UNSIGNED)
			{
				if (longlong)
					type = long_long_unsigned_type_node;
				elif (specbits & 1 << RID_LONG)
					type = long_unsigned_type_node;
				elif (specbits & 1 << RID_SHORT)
					type = short_unsigned_type_node;
				elif (type == char_type_node)
					type = unsigned_char_type_node;
				else
					type = unsigned_type_node;
			}
			elif ((specbits & 1 << RID_SIGNED) && type == char_type_node)
			{
				type = signed_char_type_node;
			}
			elif (longlong)
			{
				type = long_long_integer_type_node;
			}
			elif (specbits & 1 << RID_LONG)
			{
				type = long_integer_type_node;
			}
			elif (specbits & 1 << RID_SHORT)
			{
				type = short_integer_type_node;
			}
		}
	}

	/*
	 *	Set CONSTP if this declaration is `const', whether
	 *	by explicit specification or via a typedef.
	 *	Likewise for VOLATILEP
	 */
	constp = !! (specbits & 1 << RID_CONST) + TREE_READONLY (type);
	volatilep = !! (specbits & 1 << RID_VOLATILE) + TREE_VOLATILE (type);
	inlinep = !! (specbits & (1 << RID_INLINE));

	if (constp > 1)
		warning ("duplicate `const'");

	if (volatilep > 1)
		warning ("duplicate `volatile'");

	type = TYPE_MAIN_VARIANT (type);

	/*
	 *	Warn if two storage classes are given. Default to `auto'
	 */
	{
		int	nclasses = 0;

		if (specbits & 1 << RID_AUTO)
			nclasses++;

		if (specbits & 1 << RID_STATIC)
			nclasses++;

		if (specbits & 1 << RID_EXTERN)
			nclasses++;

		if (specbits & 1 << RID_REGISTER)
			nclasses++;

		if (specbits & 1 << RID_TYPEDEF)
			nclasses++;
#define	ENTRY
#ifdef	ENTRY
		if (specbits & 1 << RID_ENTRY)
			nclasses++;
#endif	ENTRY

		/*
		 *	Warn about storage classes that are invalid
		 *	for certain kinds of declarations
		 *	(parameters, typenames, etc.)
		 */
		if   (nclasses > 1)
		{
			error ("multiple storage classes in declaration of `%s'", name);
		}
		elif (funcdef_flag && (specbits & ((1 << RID_REGISTER) | (1 << RID_AUTO) | (1 << RID_TYPEDEF))))
		{
			if (specbits & 1 << RID_AUTO)
				error ("function definition declared `auto'");

			if (specbits & 1 << RID_REGISTER)
				error ("function definition declared `auto'");

			if (specbits & 1 << RID_TYPEDEF)
				error ("function definition declared `typedef'");

			specbits &= ~ ((1 << RID_TYPEDEF) | (1 << RID_REGISTER) | (1 << RID_AUTO));
		}
		elif (decl_context != NORMAL && nclasses > 0)
		{
			if (decl_context == PARM && specbits & 1 << RID_REGISTER)
			{
				/* vazio */;
			}
			else
			{
				error
				(	(decl_context == FIELD ? "storage class specified for structure field `%s'" :
					(decl_context == PARM  ? "storage class specified for parameter `%s'" :
					 "storage class specified for typename")),
					 name
				);

				specbits &=
					~ (
						  (1 << RID_TYPEDEF)
						| (1 << RID_REGISTER)
						| (1 << RID_AUTO)
						| (1 << RID_STATIC)
						| (1 << RID_EXTERN)
#ifdef	ENTRY
						| (1 << RID_ENTRY)
#endif	ENTRY
					);
			}
		}
		elif (specbits & 1 << RID_EXTERN && initialized && ! funcdef_flag)
		{
			warning ("`%s' initialized and declared `extern'", name);
		}
#ifdef	ENTRY
		elif (current_binding_level == global_binding_level)
		{
			if (specbits & (1 << RID_AUTO))
				error ("top-level declaration of `%s' specifies `auto'", name);
		}
		else 	/* current_binding_level != global_binding_level */
		{
			if (specbits & (1 << RID_ENTRY))
				error ("non top-level declaration of `%s' specifies `entry'", name);
		}
#else
		elif (current_binding_level == global_binding_level && specbits & (1 << RID_AUTO))
		{
			error ("top-level declaration of `%s' specifies `auto'", name);
		}
#endif	ENTRY
	}

	/*
	 *	Now figure out the structure of the declarator proper.
	 *	Descend through it, creating more complex types,
	 *	until we reach the declared identifier (or NULL_TREE,
	 *	in an absolute declarator)
	 */
	while (declarator && TREE_CODE (declarator) != IDENTIFIER_NODE)
	{
		if (type == error_mark_node)
		{
			declarator = TREE_OPERAND (declarator, 0);
			continue;
		}

		/*
		 *	Each level of DECLARATOR is either an ARRAY_REF
		 *	(for ...[..]), an INDIRECT_REF (for *...),
	 	 *	a CALL_EXPR (for ...(...)), an identifier (for
		 *	the name being declared) or a null pointer
		 *	(for the place in an absolute declarator where
		 *	the name was omitted). For the last two cases,
		 *	we have just exited the loop.
		 *
	 	 *	At this point, TYPE is the type of elements of
		 *	an array, or for a function to return, or for
		 *	a pointer to point to. After this sequence of
		 *	ifs, TYPE is the type of the array or function
		 *	or pointer, and DECLARATOR has had its outermost
		 *	layer removed
		 */
		if (TREE_CODE (declarator) == ARRAY_REF)
		{
			register tree	itype = NULL_TREE;
			register tree	size = TREE_OPERAND (declarator, 1);

			declarator = TREE_OPERAND (declarator, 0);

			/*
			 *	Check for some types that there
			 *	cannot be arrays of
			 */
			if (type == void_type_node)
			{
				error ("declaration of `%s' as array of voids", name);
				type = error_mark_node;
			}

			if (TREE_CODE (type) == FUNCTION_TYPE)
			{
				error ("declaration of `%s' as array of functions", name);
				type = error_mark_node;
			}

			if (size == error_mark_node)
				type = error_mark_node;

			if (type == error_mark_node)
				continue;

			/*
			 *	If size was specified, set ITYPE to a
			 *	range-type for that size. Otherwise,
			 *	ITYPE remains null. finish_decl may
			 *	figure it out from an initial value
			 */
			if (size)
			{
				/*
				 *	might be a cast
				 */
				if
				(	TREE_CODE (size) == NOP_EXPR &&
					TREE_TYPE (size) == TREE_TYPE (TREE_OPERAND (size, 0))
				)
					size = TREE_OPERAND (size, 0);

				if
				(	TREE_CODE (TREE_TYPE (size)) != INTEGER_TYPE &&
					TREE_CODE (TREE_TYPE (size)) != ENUMERAL_TYPE
				)
				{
					error ("size of array `%s' has non-integer type", name);
					size = integer_one_node;
				}

				if (pedantic && integer_zerop (size))
					warning ("ANSI C forbids zero-size array `%s'", name);

				if (TREE_CODE (size) == INTEGER_CST)
				{
					if (INT_CST_LT (size, integer_zero_node))
					{
						error ("size of array `%s' is negative", name);
						size = integer_one_node;
					}

					itype = build_index_type (build_int_2 (TREE_INT_CST_LOW (size) - 1, 0));
				}
				else
				{
					if (pedantic)
						warning ("ANSI C forbids variable-size array `%s'", name);

					itype = build_binary_op (MINUS_EXPR, size, integer_one_node);
					itype = build_index_type (itype);
				}
			}

#if 0	/* --------------------------------------------------- */
			/*
			 *	This loses for union incomplete (*foo)[4];
			 *
			 *	Complain about arrays of incomplete types,
			 *	except in typedefs
			 */
			if (TYPE_SIZE (type) == 0 && !(specbits & (1 << RID_TYPEDEF)))
				warning ("array type has incomplete element type");
#endif	/* --------------------------------------------------- */

			/*	
			 *	Build the array type itself. Merge any
			 *	constancy or volatility into the target type
			 */
			if (constp || volatilep)
				type = c_build_type_variant (type, constp, volatilep);

#if 0	/* --------------------------------------------------- */
			/*
			 *	don't clear these; leave them set so
			 *	that the array type or the variable is
			 *	itself const or volatile
			 */
			constp = 0; volatilep = 0;
#endif	/* --------------------------------------------------- */

			type = build_array_type (type, itype);
		}
		elif (TREE_CODE (declarator) == CALL_EXPR)
		{
			tree arg_types;

			/*
			 *	Declaring a function type. Make sure
			 *	we have a valid type for the function
			 *	to return
			 */
			if (type == error_mark_node)
				continue;

			if (pedantic && (constp || volatilep))
				warning ("function declared to return const or volatile result");

			/*
			 *	Warn about some types functions can't return
			 */
			if (TREE_CODE (type) == FUNCTION_TYPE)
			{
				error ("`%s' declared as function returning a function", name);
				type = integer_type_node;
			}

			if (TREE_CODE (type) == ARRAY_TYPE)
			{
				error ("`%s' declared as function returning an array", name);
				type = integer_type_node;
			}

			/*
			 *	Traditionally, declaring return type float
			 *	means double
			 */
			if (flag_traditional && type == float_type_node)
				type = double_type_node;

			/*
			 *	Construct the function type and go to
			 *	the next inner layer of declarator
			 *
			 *	Say it's a definition only for the CALL_EXPR
			 *	closest to the identifier
			 */
			arg_types = grokparms
			(	TREE_OPERAND (declarator, 1),
				funcdef_flag && TREE_CODE (TREE_OPERAND (declarator, 0)) == IDENTIFIER_NODE
			);

#if 0	/* --------------------------------------------------- */
			/*
			 *	This seems to be false. We turn off
			 *	temporary allocation above in this
			 *	function if -traditional. And this
			 *	code caused inconsistent results with
			 *	prototypes: callers would ignore them,
			 *	and pass arguments wrong
			 *
			 *	Omit the arg types if -traditional,
			 *	since the arg types and the list links
			 *	might not be permanent
			 */
			type = build_function_type (type, flag_traditional ? 0 : arg_types);
#endif	/* --------------------------------------------------- */

			type = build_function_type (type, arg_types);
			declarator = TREE_OPERAND (declarator, 0);
		}
		elif (TREE_CODE (declarator) == INDIRECT_REF)
		{
			/*
			 *	Merge any constancy or volatility into
			 *	the target type for the pointer
			 */
			if (constp || volatilep)
				type = c_build_type_variant (type, constp, volatilep);

			constp = 0; volatilep = 0;

			type = build_pointer_type (type);

			/*
			 *	Process a list of type modifier keywords
			 *	(such as const or volatile) that were given
			 *	inside the `*'
			 */
			if (TREE_TYPE (declarator))
			{
				register tree	typemodlist;
				int		erred = 0;

				for
				(	typemodlist = TREE_TYPE (declarator);
					typemodlist;
					typemodlist = TREE_CHAIN (typemodlist)
				)
				{
					if   (TREE_VALUE (typemodlist) == ridpointers[RID_CONST])
					{
						constp++;
					}
					elif (TREE_VALUE (typemodlist) == ridpointers[RID_VOLATILE])
					{
						volatilep++;
					}
					elif (!erred)
					{
						erred = 1;
						error ("invalid type modifier within pointer declarator");
					}
				}

				if (constp > 1)
					warning ("duplicate `const'");

				if (volatilep > 1)
					warning ("duplicate `volatile'");
			}

			declarator = TREE_OPERAND (declarator, 0);
		}
		else
		{
			abort ();
		}

#if 0	/* --------------------------------------------------- */
		 layout_type (type);
#endif	/* --------------------------------------------------- */

		/*
		 *	Should perhaps replace the following code
		 *	by changes in stor_layout.c
		 */
		if (TREE_CODE (type) == FUNCTION_DECL)
		{
			/*
			 *	A function variable in C should be Pmode
			 *	rather than EPmode because it has just
			 *	the address of a function, no static chain
			 */
			TYPE_MODE (type) = Pmode;
		}

	}	/* end while (declarator ...) */

	/*
	 *	Now TYPE has the actual type
	 *
	 *	If this is declaring a typedef name,
	 *	return a TYPE_DECL
	 */
	if (specbits & (1 << RID_TYPEDEF))
	{
		/*
		 *	Note that the grammar rejects storage classes
	 	 *	in typenames, fields or parameters
		 */
		if (constp || volatilep)
			type = c_build_type_variant (type, constp, volatilep);

		if (resume_temporary)
			resume_temporary_allocation ();

		return (build_decl (TYPE_DECL, declarator, type));
	}

	/*
	 *	Detect the case of an array type of unspecified size
	 *	which came, as such, direct from a typedef name.
	 *	We must copy the type, so that each identifier gets
	 *	a distinct type, so that each identifier's size can be
	 *	controlled separately by its own initializer
	 */
	if
	(	type != NULL_TREE && typedef_type != NULL_TREE &&
		TYPE_MAIN_VARIANT (type) == TYPE_MAIN_VARIANT (typedef_type) &&
		TREE_CODE (type) == ARRAY_TYPE &&
		TYPE_DOMAIN (type) == NULL_TREE
	)
		type = build_array_type (TREE_TYPE (type), NULL_TREE);

	/*
	 *	If this is a type name (such as, in a cast or sizeof),
	 *	compute the type and return it now
	 */
	if (decl_context == TYPENAME)
	{
		/*
		 *	Note that the grammar rejects storage classes
		 *	in typenames, fields or parameters
		 */
		if (constp || volatilep)
			type = c_build_type_variant (type, constp, volatilep);

		if (resume_temporary)
			resume_temporary_allocation ();

		return (type);
	}

	/*
	 *	`void' at top level (not within pointer) is allowed
	 *	only in typedefs or type names. We don't complain
	 *	about parms either, but that is because a better
	 *	error message can be made later
	 */
	if (type == void_type_node && decl_context != PARM)
	{
		error
		(	"variable or field `%s' declared void",
			IDENTIFIER_POINTER (declarator)
		);

		type = integer_type_node;
	}

	/*
	 *	Now create the decl, which may be a VAR_DECL, a PARM_DECL
	 *	or a FUNCTION_DECL, depending on DECL_CONTEXT and TYPE
	 */
	{
		register tree	decl;

		if (decl_context == PARM)
		{
			/*
			 *	A parameter declared as an array of T
			 *	is really a pointer to T. One declared
			 *	as a function is really a pointer to
			 *	a function
			 */
			if (TREE_CODE (type) == ARRAY_TYPE)
			{
				/*
				 *	Transfer const-ness of array into
				 *	that of type pointed to
				 */
				type = build_pointer_type
				(	c_build_type_variant
					(	TREE_TYPE (type),
						constp,
						volatilep
					)
				);

				volatilep = constp = 0;
			}
			elif (TREE_CODE (type) == FUNCTION_TYPE)
			{
				if (pedantic && (constp || volatilep))
					warning ("ANSI C forbids const or volatile function types");

				type = build_pointer_type (c_build_type_variant (type, constp, volatilep));
				volatilep = constp = 0;
			}

			if (initialized)
				error ("parameter `%s' is initialized", name);

			decl = build_decl (PARM_DECL, declarator, type);

			/*
			 *	Compute the type actually passed in the
			 *	parmlist, for the case where there is no
			 *	prototype. (For example, shorts and chars
			 *	are passed as ints.) When there is a
			 *	prototype, this is overridden later
			 */
			DECL_ARG_TYPE (decl) = type;

			if (type == float_type_node)
			{
				DECL_ARG_TYPE (decl) = double_type_node;
			}
			elif
			(	TREE_CODE (type) == INTEGER_TYPE &&
				(TYPE_PRECISION (type) < TYPE_PRECISION (integer_type_node) ||
					type == short_integer_type_node ||
					type == short_unsigned_type_node)
			)
			{
				/*
				 *	ANSI C says short and char are
				 *	promoted to int or unsigned int,
				 *	even if that is not wider
				 */
				if (TYPE_PRECISION (type) == TYPE_PRECISION (integer_type_node) && TREE_UNSIGNED (type))
					DECL_ARG_TYPE (decl) = unsigned_type_node;
				else
					DECL_ARG_TYPE (decl) = integer_type_node;
			}
		}
		elif (decl_context == FIELD)
		{
			/*
			 *	Structure field. It may not be a function
			 */
			if (TREE_CODE (type) == FUNCTION_TYPE)
			{
				error ("field `%s' declared as a function", IDENTIFIER_POINTER (declarator));
				type = build_pointer_type (type);
			}
			elif (TREE_CODE (type) != ERROR_MARK && TYPE_SIZE (type) == NULL_TREE)
			{
				error ("field `%s' has incomplete type", IDENTIFIER_POINTER (declarator));
				type = error_mark_node;
			}

			if (TREE_CODE (type) == ARRAY_TYPE && (constp || volatilep))
			{
				/*
				 *	Move type qualifiers down to
				 *	element of an array
				 */
				type = c_build_type_variant (type, constp, volatilep);
				constp = volatilep = 0;
			}

			/*
			 *	Note that the grammar rejects storage
			 *	classes in typenames, fields or parameters
			 */
			decl = build_decl (FIELD_DECL, declarator, type);
		}
		elif (TREE_CODE (type) == FUNCTION_TYPE)
		{
			if (specbits & ((1 << RID_AUTO) | (1 << RID_REGISTER)))
				error ("invalid storage class for function `%s'", IDENTIFIER_POINTER (declarator));

			/*
			 *	Function declaration not at top level.
			 *	Storage classes other than `extern' are
			 *	not allowed and `extern' makes no difference
			 */
			if
			(	current_binding_level != global_binding_level &&
				(specbits & ((1 << RID_STATIC) | (1 << RID_INLINE))) &&
				pedantic
			)
				warning ("invalid storage class for function `%s'", IDENTIFIER_POINTER (declarator));

			decl = build_decl (FUNCTION_DECL, declarator, type);

			TREE_EXTERNAL (decl) = 1;

			/*
			 *	Record presence of `static'
			 */
			TREE_PUBLIC (decl) = !(specbits & (1 << RID_STATIC));

			/*
			 *	Record presence of `inline',
			 *	if it is reasonable
			 */
			if (inlinep)
			{
				tree	last = tree_last (TYPE_ARG_TYPES (type));

				if (! strcmp (IDENTIFIER_POINTER (declarator), "main"))
				{
					warning ("cannot inline function `main'");
				}
				elif (last && TREE_VALUE (last) != void_type_node)
				{
					warning ("inline declaration ignored for function with `...'");
				}
				else
				{
					/*
					 *	Assume that otherwise the
					 *	function can be inlined
					 */
					TREE_INLINE (decl) = 1;
				}

				if (specbits & (1 << RID_EXTERN))
					current_extern_inline = 1;
			}
		}
		else	/* Variable */
		{
			/*
			 *	It's a variable
			 *
			 *	Move type qualifiers down to
			 *	element of an array
			 */
			if (TREE_CODE (type) == ARRAY_TYPE && (constp || volatilep))
			{
				type = c_build_type_variant (type, constp, volatilep);

#if 0	/* --------------------------------------------------- */
				/*
				 *	but a variable whose type is
				 *	const should still have TREE_READONLY
				 */
				constp = volatilep = 0;
#endif	/* --------------------------------------------------- */
			}

			decl = build_decl (VAR_DECL, declarator, type);

			if (inlinep)
				warning_with_decl (decl, "variable `%s' declared `inline'");

			/*
			 *	An uninitialized decl with `extern'
			 *	is a reference
			 */
			TREE_EXTERNAL (decl) = !initialized && (specbits & (1 << RID_EXTERN));

			/*
			 *	At top level, either `static'
			 *	or no s.c. makes a definition
		 	 *	(perhaps tentative), and absence
			 *	of `static' makes it public
			 */
			if (current_binding_level == global_binding_level)
			{
#ifdef	ENTRY
				if (specbits & (1 << RID_ENTRY))
					TREE_ENTRY (decl) = 1;
#endif	ENTRY
				TREE_PUBLIC (decl) = !(specbits & (1 << RID_STATIC));
				TREE_STATIC (decl) = ! TREE_EXTERNAL (decl);
			}
			else
			{
				/*
				 *	Not at top level, only `static'
				 *	makes a static definition
				 */
				TREE_STATIC (decl) = (specbits & (1 << RID_STATIC)) != 0;
				TREE_PUBLIC (decl) = TREE_EXTERNAL (decl);

				/*
				 *	`extern' with initialization is
				 *	invalid if not at top level
				 */
				if ((specbits & (1 << RID_EXTERN)) && initialized)
					error ("`%s' has both `extern' and initializer", name);
			}
		}

		/*
		 *	Record `register' declaration for warnings on &
		 *	and in case doing stupid register allocation
		 */
		if (specbits & (1 << RID_REGISTER))
			TREE_REGDECL (decl) = 1;

		/*
		 *	Record constancy and volatility
		 */
		if (constp)
			TREE_READONLY (decl) = 1;

		if (volatilep)
		{
			TREE_VOLATILE (decl) = 1;
			TREE_THIS_VOLATILE (decl) = 1;
		}

		if (resume_temporary)
			resume_temporary_allocation ();

		return (decl);
	}

}	/* end grokdeclarator */
