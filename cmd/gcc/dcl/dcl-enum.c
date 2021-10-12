/*
 ****************************************************************
 *								*
 *			dcl-enum.c				*
 *								*
 *	Process declarations and symbol lookup for C front end.	*
 *	Also constructs types; the standard scalar types at	*
 *	initialization, and structure, union, array and enum	*
 *	types when they are declared.				*
 *								*
 *	Versão	3.0.0, de 31.12.93				*
 *		3.0.0, de 31.12.93				*
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

#include "../h/tree.h"
#include "../h/flags.h"

#include "../tree/tree.h"
#include "../tchk/tchk.h"
#include "../top/top.h"

#include "dcl.h"

/*
 ****************************************************************
 *	Lay out the type T, and its element type, and so on	*
 ****************************************************************
 */
void
layout_array_type (tree t)
{
	if (TREE_CODE (TREE_TYPE (t)) == ARRAY_TYPE)
		layout_array_type (TREE_TYPE (t));

	layout_type (t);

}	/* end layout_array_type */

/*
 ****************************************************************
 *	Begin compiling the definition of an enumeration type	*
 ****************************************************************
 */
/*
 *	Begin compiling the definition of an enumeration
 *	type. NAME is its name (or null if anonymous).
 *	Returns the type object, as yet incomplete. Also
 *	records info about it so that build_enumerator may
 *	be used to declare the individual values as they
 *	are read
 */
tree
start_enum (tree name)
{
	register tree	enumtype = NULL_TREE;

	/*
	 *	If this is the real definition for a previous
	 *	forward reference, fill in the contents in the
	 *	same object that used to be the forward reference
	 */
	if (name != NULL_TREE)
		enumtype = lookup_tag (ENUMERAL_TYPE, name, current_binding_level, 1);

	if (enumtype == NULL_TREE || TREE_CODE (enumtype) != ENUMERAL_TYPE)
	{
		enumtype = make_node (ENUMERAL_TYPE);
		pushtag (name, enumtype);
	}

	/*
	 *	This enum is a named one that has been declared already
	 *	Completely replace its old definition. The old
	 *	enumerators remain defined, however
	 */
	if (TYPE_VALUES (enumtype) != NULL_TREE)
	{
		error ("redeclaration of `enum %s'", IDENTIFIER_POINTER (name));
		TYPE_VALUES (enumtype) = NULL_TREE;
	}

	/*
	 *	Initially, set up this enum as like `int' so that
	 *	we can create the enumerators' declarations and
	 *	values. Later on, the precision of the type may
	 *	be changed and it may be laid out again
	 */
	TYPE_PRECISION (enumtype) = TYPE_PRECISION (integer_type_node);
	TYPE_SIZE (enumtype) = NULL_TREE;
	fixup_unsigned_type (enumtype);

	enum_next_value = integer_zero_node;

	return (enumtype);

}	/* end start_enum */

/*
 ****************************************************************
 *	Install the decls in the enumeration type		*
 ****************************************************************
 */
/*
 *	After processing and defining all the values of an
 *	enumeration type, install their decls in the enumeration
 *	type and finish it off. ENUMTYPE is the type object and
 *	VALUES a list of name-value pairs. Returns ENUMTYPE
 */
tree
finish_enum (register tree enumtype, register tree values)
{
	register tree	pair;
	tree		maximum = NULL_TREE, minimum = NULL_TREE;
	register long	maxvalue = 0;
	register long	minvalue = 0;

	if (in_parm_level_p ())
		warning ("enum defined inside parms");

	TYPE_VALUES (enumtype) = values;

	/*
	 *	Calculate the maximum and minimum values
	 *	of any enumerator in this type
	 */
	for (pair = values; pair; pair = TREE_CHAIN (pair))
	{
		tree value = TREE_VALUE (pair);

		if (pair == values)
		{
			maximum = minimum = value;
		}
		else
		{
			if (tree_int_cst_lt (maximum, value))
				maximum = value;

			if (tree_int_cst_lt (value, minimum))
				minimum = value;
		}
	}

	TYPE_MIN_VALUE (enumtype) = minimum;
	TYPE_MAX_VALUE (enumtype) = maximum;

	/*
	 *	An enum can have some negative values;
	 *	then it is signed
	 */
	if (tree_int_cst_lt (minimum, integer_zero_node))
		TREE_UNSIGNED (enumtype) = 0;

	if (flag_short_enums)
	{
		/*
		 *	Determine the precision this type needs,
		 *	lay it out, and define it
		 */
		int		maxvalue = TREE_INT_CST_LOW (maximum);
		int		minvalue = TREE_INT_CST_LOW (minimum);
		int		prec = floor_log2 (maxvalue) + 1;

		if (! tree_int_cst_lt (minimum, integer_zero_node))
		{
			/*
			 *	All values are nonnegatives
			 */
			if (prec == 0)
				prec = 1;

			TYPE_PRECISION (enumtype) = prec;
		}
		else
		{
			int		negprec;

			negprec = floor_log2 (-1 - minvalue) + 1;

			if (prec < negprec)
				prec = negprec;

			TYPE_PRECISION (enumtype) = prec + 1;
		}

		/*
		 *	Increase the size till it becomes
		 *	the size of some mode
		 */
		TYPE_PRECISION (enumtype) = round_size (TYPE_PRECISION (enumtype));

		/*
		 *	Cancel the laying out previously done for
		 *	the enum type, so that fixup_unsigned_type
		 *	will do it over
		 */
		TYPE_SIZE (enumtype) = NULL_TREE;

		layout_type (enumtype);
	}

	return (enumtype);

}	/* end finish_enum */

/*
 ****************************************************************
 *	Build and install a CONST_DECL				*
 ****************************************************************
 */
/*
 *	Build and install a CONST_DECL for one value of the
 *	current enumeration type (one that was begun with
 *	start_enum). Return a tree-list containing the name
 *	and its value. Assignment of sequential values by
 *	default is handled here
 */
tree
build_enumerator (tree name, tree value)
{
	register tree	decl;

	/*
	 *	Validate and default VALUE
	 *	Remove no-op casts from the value
	 */
	while (value != NULL_TREE && TREE_CODE (value) == NOP_EXPR)
		value = TREE_OPERAND (value, 0);

	if (value != NULL_TREE && TREE_CODE (value) != INTEGER_CST)
	{
		error
		(	"enumerator value for `%s' not integer constant",
			IDENTIFIER_POINTER (name)
		);

		value = NULL_TREE;
	}

	/*
	 *	Default based on previous value
	 */
	if (value == NULL_TREE)
		value = enum_next_value;

	/*
	 *	Might as well enforce the ANSI restriction, since
	 *	values outside this range don't work in version 1
	 */
	if (! int_fits_type_p (value, integer_type_node))
	{
		error ("enumerator value outside range of `int'");
		value = integer_zero_node;
	}

	/*
	 *	Set basis for default for next value
	 */
	enum_next_value = build_binary_op_nodefault (PLUS_EXPR, value, integer_one_node, PLUS_EXPR);

	/*
	 *	Now create a declaration for the enum value name
	 */
	decl = build_decl (CONST_DECL, name, integer_type_node);
	DECL_INITIAL (decl) = value;
	TREE_TYPE (value) = integer_type_node;
	pushdecl (decl);

	return (saveable_tree_cons (name, value, NULL_TREE));

}	/* end build_enumerator */

/*
 ****************************************************************
 *	Create the FUNCTION_DECL for a function definition	*
 ****************************************************************
 */
/*
 *	Create the FUNCTION_DECL for a function definition.
 *	DECLSPECS and DECLARATOR are the parts of the
 *	declaration; they describe the function's name and
 *	the type it returns, but twisted together in a
 *	fashion that parallels the syntax of C.
 *
 *	This function creates a binding context for the
 *	function body as well as setting up the FUNCTION_DECL
 *	in current_function_decl.
 *
 *	Returns 1 on success. If the DECLARATOR is not
 *	suitable for a function (it defines a datum instead),
 *	we return 0, which tells yyparse to report a parse error
 */
int
start_function (tree declspecs, tree declarator)
{
	tree		decl1, old_decl, restype;

	/*
	 *	Assume, until we see it does
	 */
	current_function_returns_value = 0;
	current_function_returns_null = 0;
	warn_about_return_type = 0;
	current_extern_inline = 0;

	decl1 = grokdeclarator (declarator, declspecs, FUNCDEF, 1);

	/*
	 *	If the declarator is not suitable for a
	 *	function definition, cause a syntax error
	 */
	if (decl1 == NULL_TREE)
		return (0);

	current_function_decl = decl1;

	announce_function (current_function_decl);

	if (TYPE_SIZE (TREE_TYPE (TREE_TYPE (decl1))) == NULL_TREE)
	{
		error ("return-type is an incomplete type");

		/*
		 *	Make it return void instead
		 */
		TREE_TYPE (decl1) = build_function_type
		(	void_type_node,
			TYPE_ARG_TYPES (TREE_TYPE (decl1))
		);
	}

	if (warn_about_return_type)
		warning ("return-type defaults to `int'");

	/*
	 *	Save the parm names or decls from this function's
	 *	declarator where store_parm_decls will find them
	 */
	current_function_parms = last_function_parms;
	current_function_parm_tags = last_function_parm_tags;

	/*
	 *	Make the init_value nonzero so pushdecl knows
	 *	this is not tentative. error_mark_node is
	 *	replaced below (in poplevel) with the LET_STMT
	 */
	DECL_INITIAL (current_function_decl) = error_mark_node;

	/*
	 *	If this definition isn't a prototype and we had
	 *	a prototype declaration before, copy the arg type
	 *	info from that prototype
	 */
	old_decl = lookup_name_current_level (DECL_NAME (current_function_decl));

	if
	(	old_decl != NULL_TREE &&
		TREE_TYPE (TREE_TYPE (current_function_decl)) == TREE_TYPE (TREE_TYPE (old_decl)) &&
		TYPE_ARG_TYPES (TREE_TYPE (current_function_decl)) == NULL_TREE
	)
		TREE_TYPE (current_function_decl) = TREE_TYPE (old_decl);

	/*
	 *	This is a definition, not a reference. So normally
	 *	clear TREE_EXTERNAL. However, `extern inline' acts
	 *	like a declaration except for defining how to inline.
	 *	So set TREE_EXTERNAL in that case
	 */
	TREE_EXTERNAL (current_function_decl) = current_extern_inline;

	/*
	 *	This function exists in static storage.
	 *	(This does not mean `static' in the C sense!)
	 */
	TREE_STATIC (current_function_decl) = 1;

	/*
	 *	Record the decl so that the function name is defined.
	 *	If we already have a decl for this name, and it is a
	 *	FUNCTION_DECL, use the old decl
	 */
	current_function_decl = pushdecl (current_function_decl);

	pushlevel (0);
	declare_parm_level ();

	make_function_rtl (current_function_decl);

	restype = TREE_TYPE (TREE_TYPE (current_function_decl));

	/*
	 *	Promote the value to int before returning it
	 */
	if
	(	TREE_CODE (restype) == INTEGER_TYPE &&
		TYPE_PRECISION (restype) < TYPE_PRECISION (integer_type_node)
	)
		restype = integer_type_node;

	DECL_RESULT_TYPE (current_function_decl) = restype;
	DECL_RESULT (current_function_decl) = build_decl (RESULT_DECL, value_identifier, restype);

	/*
	 *	Allocate further tree nodes temporarily
	 *	during compilation of this function only
	 */
	temporary_allocation ();

	/*
	 *	If this fcn was already referenced via a block-scope
	 *	`extern' decl (or an implicit decl), propagate
	 *	certain information about the usage
	 */
	if (TREE_ADDRESSABLE (DECL_NAME (current_function_decl)))
		TREE_ADDRESSABLE (current_function_decl) = 1;

	return (1);

}	/* end start_function */

/*
 ****************************************************************
 *	Store the parameter into the current function		*
 ****************************************************************
 */
/*
 *	Store the parameter declarations into the current
 *	function declaration. This is called after parsing
 *	the parameter declarations, before digesting the
 *	body of the function
 */
void
store_parm_decls (void)
{
	register tree	fndecl = current_function_decl;
	register tree	parm;

	/*
	 *	This is either a chain of PARM_DECLs
	 *	(if a prototype was used) or a list of
	 *	IDENTIFIER_NODEs (for an old-fashioned C definition)
	 */
	tree		specparms = current_function_parms;

	/*
	 *	This is a list of types declared
	 *	among parms in a prototype
	 */
	tree		parmtags = current_function_parm_tags;

	/*
	 *	This is a chain of PARM_DECLs
	 *	from old-style parm declarations
	 */
	register tree	parmdecls = getdecls ();

	/*
	 *	This is a chain of any other decls that
	 *	came in among the parm declarations.
	 *	If a parm is declared with	enum {foo, bar} x;
	 *	then CONST_DECLs for foo and bar are put here
	 */
	tree		nonparms = NULL_TREE;

	/*
	 *	This case is when the function was defined with an
	 *	ANSI prototype. The parms already have decls, so we
	 *	need not do anything here except record them as in
	 *	effect and complain if any redundant old-style parm
	 *	decls were written
	 */
	if (specparms != NULL_TREE && TREE_CODE (specparms) != TREE_LIST)
	{
		register tree	next;
		tree		others = NULL_TREE;

		if (parmdecls != NULL_TREE)
		{
			error_with_decl
			(	fndecl,
				"parm types given both in parmlist and separately"
			);
		}

		specparms = nreverse (specparms);

		for (parm = specparms; parm; parm = next)
		{
			next = TREE_CHAIN (parm);

			if (DECL_NAME (parm) == NULL_TREE)
			{
				error_with_decl (parm, "parameter name omitted");
			}
			elif (TREE_TYPE (parm) == void_type_node)
			{
				error_with_decl (parm, "parameter `%s' declared void");
			}
			elif (TREE_CODE (parm) == PARM_DECL)
			{
				pushdecl (parm);
			}
			else
			{
				/*
				 *	If we find an enum constant,
				 *	put it aside for the moment
				 */
				TREE_CHAIN (parm) = NULL_TREE;
				others = chainon (others, parm);
			}
		}

		/*
		 *	Get the decls in their original chain order
		 *	and record in the function
		 */
		DECL_ARGUMENTS (fndecl) = getdecls ();

		/*
		 *	Now pushdecl the enum constants
		 */
		for (parm = others; parm; parm = next)
		{
			next = TREE_CHAIN (parm);

			if (DECL_NAME (parm) == NULL_TREE)
				/* vazio */;
			elif (TREE_TYPE (parm) == void_type_node)
				/* vazio */;
			elif (TREE_CODE (parm) != PARM_DECL)
				pushdecl (parm);
		}

		storetags (chainon (parmtags, gettags ()));
	}
	else
	{
		/*
		 *	SPECPARMS is an identifier list--a chain
		 *	of TREE_LIST nodes each with a parm name
		 *	as the TREE_VALUE.
		 *
	 	 *	PARMDECLS is a list of declarations for parameters.
	 	 *	Warning! It can also contain CONST_DECLs which
		 *	are not parameters but are names of enumerators
		 *	of any enum types declared among the parameters.
		 *
	 	 *	First match each formal parameter name with its
		 *	declaration. Associate decls with the names and
		 *	store the decls into the TREE_PURPOSE slots
		 */
		for (parm = specparms; parm; parm = TREE_CHAIN (parm))
		{
			register tree	tail;
			tree		found = NULL_TREE;

			if (TREE_VALUE (parm) == NULL_TREE)
			{
				error_with_decl (fndecl, "parameter name missing from parameter list");
				TREE_PURPOSE (parm) = NULL_TREE;
				continue;
			}

			/*
			 *	See if any of the parmdecls specifies
			 *	this parm by name. Ignore any enumerator
			 *	decls
			 */
			for (tail = parmdecls; tail; tail = TREE_CHAIN (tail))
			{
				if
				(	DECL_NAME (tail) == TREE_VALUE (parm) &&
					TREE_CODE (tail) == PARM_DECL
				)
				{
					found = tail;
					break;
				}
			}

			/*
			 *	If declaration already marked, we have
			 *	a duplicate name. Complain, and don't
			 *	use this decl twice
			 */
			if (found && DECL_CONTEXT (found) != NULL_TREE)
			{
				error_with_decl (found, "multiple parameters named `%s'");
				found = NULL_TREE;
			}

			/*
			 *	If the declaration says "void",
			 *	complain and ignore it
			 */
			if (found && TREE_TYPE (found) == void_type_node)
			{
				error_with_decl (found, "parameter `%s' declared void");
				TREE_TYPE (found) = integer_type_node;
				DECL_ARG_TYPE (found) = integer_type_node;
				layout_decl (found, 0);
			}

			/*
			 *	If no declaration found, default to int
			 */
			if (!found)
			{
				found = build_decl
				(	PARM_DECL,
					TREE_VALUE (parm),
					integer_type_node
				);

				DECL_ARG_TYPE (found) = TREE_TYPE (found);
				DECL_SOURCE_LINE (found) = DECL_SOURCE_LINE (fndecl);
				DECL_SOURCE_FILE (found) = DECL_SOURCE_FILE (fndecl);

				if (extra_warnings)
					warning_with_decl (found, "type of `%s' defaults to `int'");

				pushdecl (found);
			}

			TREE_PURPOSE (parm) = found;

			/*
			 *	Mark this decl as "already found" --
			 *	see test, above. It is safe to clobber
			 *	DECL_CONTEXT temporarily because the
			 *	final values are not stored until the
			 *	`poplevel' in `finish_function'
			 */
			DECL_CONTEXT (found) = error_mark_node;

		}	/* end for (parm = ...) */

		/*
		 *	Complain about declarations not matched
		 *	with any names. Put any enumerator constants
		 *	onto the list NONPARMS
		 */
		nonparms = NULL_TREE;

		for (parm = parmdecls; parm; /* vazio */ )
		{
			tree		next = TREE_CHAIN (parm);

			TREE_CHAIN (parm) = NULL_TREE;

			/*
			 *	Complain about args with incomplete types
			 */
			if (TYPE_SIZE (TREE_TYPE (parm)) == NULL_TREE)
			{
				error_with_decl (parm, "parameter `%s' has incomplete type");
				TREE_TYPE (parm) = error_mark_node;
			}

			if (TREE_CODE (parm) != PARM_DECL)
			{
				nonparms = chainon (nonparms, parm);
			}
			elif (DECL_CONTEXT (parm) == NULL_TREE)
			{
				error_with_decl
				(	parm,
					 "declaration for parameter `%s' but no such parameter"
				);

				/*
				 *	Pretend the parameter was not missing.
				 *	This gets us to a standard state
				 *	and minimizes further error messages
				 */
				specparms = chainon
				(	specparms,
					tree_cons (parm, NULL_TREE, NULL_TREE)
				);
			}

			parm = next;

		}	/* end for (parm = ...) */

		/*
		 *	Chain the declarations together in the order
		 *	of the list of names. Store that chain in the
		 *	function decl, replacing the list of names
		 */
		parm = specparms;

		DECL_ARGUMENTS (fndecl) = NULL_TREE;

		{
			register tree	last;

			for (last = NULL_TREE; parm; parm = TREE_CHAIN (parm))
			{
				if (TREE_PURPOSE (parm))
				{
					if (last == NULL_TREE)
						DECL_ARGUMENTS (fndecl) = TREE_PURPOSE (parm);
					else
						TREE_CHAIN (last) = TREE_PURPOSE (parm);

					last = TREE_PURPOSE (parm);
					TREE_CHAIN (last) = NULL_TREE;
					DECL_CONTEXT (last) = NULL_TREE;
				}
			}
		}

		/*
		 *	If there was a previous prototype, set the
		 *	DECL_ARG_TYPE of each argument according to
		 *	the type previously specified, and report
		 *	any mismatches
		 */
		if (TYPE_ARG_TYPES (TREE_TYPE (fndecl)))
		{
			register tree	type;

			for
			(	parm = DECL_ARGUMENTS (fndecl),
				type = TYPE_ARG_TYPES (TREE_TYPE (fndecl));
				parm || (type && TREE_VALUE (type) != void_type_node);
				parm = TREE_CHAIN (parm), type = TREE_CHAIN (type)
			)
			{
				if
				(	parm == NULL_TREE || type == NULL_TREE ||
					TREE_VALUE (type) == void_type_node
				)
				{
					error ("number of arguments doesn't match prototype");
					break;
				}

				/*
				 *	Type for passing arg must be consistent
		 		 *	with that declared for the arg.
				 *	If -traditional, allow `unsigned int'
				 *	instead of `int' in the prototype
				 */
				if
				(	! comptypes (DECL_ARG_TYPE (parm), TREE_VALUE (type)) &&
					(! (flag_traditional &&
					DECL_ARG_TYPE (parm) == integer_type_node &&
					TREE_VALUE (type) == unsigned_type_node))
				)
				{
					error
					(	"argument `%s' doesn't match function prototype",
						IDENTIFIER_POINTER (DECL_NAME (parm))
					);

					if
					(	DECL_ARG_TYPE (parm) == integer_type_node &&
						TREE_VALUE (type) == TREE_TYPE (parm)
					)
					{
						error ("a formal parameter type that promotes to `int'");
						error ("can match only `int' in the prototype");
					}

					if
					(	DECL_ARG_TYPE (parm) == double_type_node &&
						TREE_VALUE (type) == TREE_TYPE (parm)
					)
					{
						error ("a formal parameter type that promotes to `double'");
						error ("can match only `double' in the prototype");
					}
				}
			}
		}

		/*
		 *	Now store the final chain of decls for the
		 *	arguments as the decl-chain of the current
		 *	lexical scope. Put the enumerators in as well,
		 *	at the front so that DECL_ARGUMENTS is not
		 *	modified
		 */
		storedecls (chainon (nonparms, DECL_ARGUMENTS (fndecl)));
	}

	/*
	 *	Make sure the binding level for the top of the
	 *	function body gets a LET_STMT if there are any
	 *	in the function. Otherwise, the dbx output is wrong
	 */
	keep_next_if_subblocks = 1;

	/*
	 *	Initialize the RTL code for the function
	 */
	init_function_start (fndecl, input_filename, lineno);

	/*
	 *	Set up parameters and prepare for return, for the function
	 */
	expand_function_start (fndecl, 0);

}	/* end store_parm_decls */

/*
 ****************************************************************
 *	Finish up a function declaration			*
 ****************************************************************
 */
/*
 *	Finish up a function declaration and compile
 *	that function all the way to assembler language output.
 *	The free the storage for the function definition.
 *
 *	This is called after parsing the body of the function
 *	definition. LINENO is the current line number
 */
void
finish_function (int lineno)
{
	register tree	fndecl = current_function_decl;

#if (0)	/* --------------------------------------------------- */
	/*
	 *	This caused &foo to be of type ptr-to-const-function
	 *	which then got a warning when stored in a
	 *	ptr-to-function variable
	 */
	TREE_READONLY (fndecl) = 1;
#endif 	/* --------------------------------------------------- */

	poplevel (1, 0, 1);

	/*
	 *	Must mark the RESULT_DECL as being in this function
	 */
	DECL_CONTEXT (DECL_RESULT (fndecl)) = DECL_INITIAL (fndecl);

	/*
	 *	Obey `register' declarations if `setjmp' is called
	 *	in this ftn
	 */
	if (flag_traditional && current_function_calls_setjmp)
		setjmp_protect (DECL_INITIAL (fndecl));

	/*
	 *	Generate rtl for function exit
	 */
	expand_function_end (input_filename, lineno);

	/*
	 *	So we can tell if jump_optimize sets it to 1
	 */
	current_function_returns_null = 0;

	/*
	 *	Run the optimizers and output the assembler
	 *	code for this function
	 */
	rest_of_compilation (fndecl);

	if   (TREE_THIS_VOLATILE (fndecl) && current_function_returns_null)
	{
		warning ("`volatile' function does return");
	}
	elif
	(	warn_return_type && current_function_returns_null &&
		TREE_TYPE (TREE_TYPE (fndecl)) != void_type_node
	)
	{
		/*
		 *	If this function returns non-void and
		 *	control can drop through, complain
		 */
		warning ("control reaches end of non-void function");

	}
	elif
	(	extra_warnings &&
		current_function_returns_value && current_function_returns_null
	)
	{
		/*
		 *	With just -W, complain only if function returns
		 *	both with and without a value
		 */
		warning ("this function may return with or without a value");

	}

	/*
	 *	Free all the tree nodes making up this function.
	 *	Switch back to allocating nodes permanently until
	 *	we start another function
	 */
	permanent_allocation ();

	if (DECL_SAVED_INSNS (fndecl) == (struct rtx_def *)NULL)
	{
		/*
		 *	Stop pointing to the local nodes about to
		 *	be freed. But DECL_INITIAL must remain
		 *	nonzero so we know this was an actual
		 *	function definition
		 */
		DECL_INITIAL (fndecl) = error_mark_node;
		DECL_ARGUMENTS (fndecl) = NULL_TREE;
	}

	/*
	 *	Let the error reporting routines know that
	 *	we're outside a function
	 */
	current_function_decl = NULL_TREE;

}	/* end finish_function */
