/*
 ****************************************************************
 *								*
 *			c-parse.y				*
 *								*
 *	YACC parser for C syntax				*
 *								*
 *	Versão	3.0.0, de 24.09.93				*
 *		3.0.0, de 27.09.93				*
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
 ******	Conflitos ***********************************************
 */
%expect 8

/*
 *	These are the 8 conflicts you should get in parse.output;
 *	the state numbers may vary if minor changes in the grammar are made.
 *
 *	S.  41 contains 1 shift/reduce conflict.  (2 ways to recover from error.)
 *	S.  92 contains 1 shift/reduce conflict.  (2 ways to recover from error.)
 *	S.  99 contains 1 shift/reduce conflict.  (2 ways to recover from error.)
 *	S. 103 contains 1 shift/reduce conflict.  (2 ways to recover from error.)
 *	S. 119 contains 1 shift/reduce conflict.  (See comment at component_decl.)
 *	S. 183 contains 1 shift/reduce conflict.  (2 ways to recover from error.)
 *	S. 193 contains 1 shift/reduce conflict.  (2 ways to recover from error.)
 *	S. 199 contains 1 shift/reduce conflict.  (2 ways to recover from error.)
 */

%{
/*
 ******	Inclusões de arquivos ***********************************
 */
#pragma	getenv (GEN)

#include ../$GEN.gen/config.h
#include "../h/tree.h"
#include "../h/input.h"
#include "../h/c-parse.h"
#include "../h/c-tree.h"

#define	elif	else if

/*
 *	Cause the `yydebug' variable to be defined.
 */
#define YYDEBUG 1
%}

/*
 ******	Início da gramática *************************************
 */
%start program

%union { long itype; tree ttype; enum tree_code code; }

/*
 *	All identifiers that are not reserved words
 *	and are not declared typedefs in the current block
 */
%token IDENTIFIER

/*
 *	All identifiers that are declared typedefs in the current block.
 *	In some contexts, they are treated just like IDENTIFIER,
 *	but they can also serve as typespecs in declarations.
 */
%token TYPENAME

/*
 *	Reserved words that specify storage class.
 *	yylval contains an IDENTIFIER_NODE which indicates which one.
 */
%token SCSPEC

/*
 *	Reserved words that specify type.
 *	yylval contains an IDENTIFIER_NODE which indicates which one.
 */
%token TYPESPEC

/*
 *	Reserved words that qualify type: "const" or "volatile".
 *	yylval contains an IDENTIFIER_NODE which indicates which one.
 */
%token TYPE_QUAL

/*
 *	Character or numeric constants.
 *	yylval is the node for the constant.
 */
%token CONSTANT

/*
 *	String constants in raw form.
 *	yylval is a STRING_CST node.
 */
%token STRING

/*
 *	"...", used for functions with variable arglists.
 */
%token ELLIPSIS

/*
 *	the reserved words
 */
%token SIZEOF ENUM STRUCT UNION IF ELSE WHILE DO FOR SWITCH CASE DEFAULT
%token BREAK CONTINUE RETURN GOTO ASM TYPEOF ALIGNOF
%token ATTRIBUTE

/*
 *	Add precedence rules to solve dangling else s/r conflict
 */
%nonassoc IF
%nonassoc ELSE

/*
 *	Define the operator tokens and their precedences.
 *	The value is an integer because, if used, it is the tree code
 *	to use in the expression made from the operator.
 */
%right <code>	ASSIGN '='
%right <code>	'?' ':'
%left  <code>	OROR
%left  <code>	ANDAND
%left  <code>	'|'
%left  <code>	'^'
%left  <code>	'&'
%left  <code>	EQCOMPARE
%left  <code>	ARITHCOMPARE
%left  <code>	LSHIFT RSHIFT
%left  <code>	'+' '-'
%left  <code>	'*' '/' '%'
%right <code>	UNARY PLUSPLUS MINUSMINUS
%left		HYPERUNARY
%left  <code>	POINTSAT '.' '(' '['

/*
 ******	Tipo dos não terminais **********************************
 */
%type <code>	unop
%type <ttype>	identifier IDENTIFIER TYPENAME CONSTANT
%type <ttype>	expr nonnull_exprlist exprlist
%type <ttype>	expr_no_commas cast_expr unary_expr primary string STRING
%type <ttype>	typed_declspecs reserved_declspecs
%type <ttype>	typed_typespecs reserved_typespecquals
%type <ttype>	declmods typespec typespecqual_reserved
%type <ttype>	SCSPEC TYPESPEC TYPE_QUAL nonempty_type_quals maybe_type_qual
%type <ttype>	initdecls notype_initdecls initdcl notype_initdcl
%type <ttype>	init initlist maybeasm
%type <ttype>	asm_operands nonnull_asm_operands asm_operand asm_clobbers
%type <ttype>	maybe_attribute attribute_list attrib

%type <ttype>	compstmt

%type <ttype>	declarator
%type <ttype>	notype_declarator after_type_declarator
%type <ttype>	parm_declarator

%type <ttype>	structsp component_decl_list component_decl_list2
%type <ttype>	component_decl components component_declarator
%type <ttype>	enumlist enumerator
%type <ttype>	typename absdcl absdcl1 type_quals
%type <ttype>	xexpr parms parm identifiers

%type <ttype>	parmlist parmlist_1 parmlist_2
%type <ttype>	parmlist_or_identifiers parmlist_or_identifiers_1

%type <itype>	setspecs

/*
 ******	Declarações *********************************************
 */
%{
/*
 *	the declaration found for the last IDENTIFIER token read in.
 *	yylex must look this up to detect typedefs, which get token
 *	type TYPENAME, so it is left around in case the identifier
 *	is not a typedef but is used in a context which makes it
 *	a reference to a variable.
 */
entry tree	lastiddecl;

extern tree	make_pointer_declarator ();
extern tree	combine_strings ();
extern void	reinit_parse_for_function ();

/*
 *	List of types and structure classes of the current declaration.
 */
tree		current_declspecs;

/*
 *	Stack of saved values of current_declspecs.
 */
tree		 declspec_stack;

/*
 *	1 if we explained undeclared var errors.
 */
int		undeclared_variable_notice;

extern int	yylex ();
%}

/*
 ******	Regras sintáticas ***************************************
 */
%%
program:   /* empty */
	|  extdefs
	;

/*
 *	the reason for the strange actions in this rule
 *	is so that notype_initdecls when reached via datadef
 *	can find a valid list of type and sc specs in $0.
 */
extdefs:  	{ $<ttype>$ = NULL_TREE; }
	   extdef
	|  extdefs
		{$<ttype>$ = NULL_TREE; }
	   extdef
	;

extdef:    fndef
	|  datadef
	|  ASM '(' string ')' ';'
		{ if (TREE_CHAIN ($3))
			$3 = combine_strings ($3);
		  assemble_asm ($3);
		}
	;

datadef:   setspecs notype_initdecls ';'
		{ if (pedantic)
		  {
			error
			(	"ANSI C forbids data definition lacking "
				"type or storage class"
			);
		  }
		  elif (!flag_traditional)
		  {
			warning ("data definition lacks type or storage class");
		  }
		}
        |  declmods setspecs notype_initdecls ';'
		  {}
	|  typed_declspecs setspecs initdecls ';'
		  {}
        |  declmods ';'
		  { error ("empty declaration"); }
	|  typed_declspecs ';'
		  { shadow_tag ($1); }
	|  error ';'
	|  error '}'
	|  ';'
		{ if (pedantic)
			warning
			(	"ANSI C does not allow extra `;' outside "
				"of a function"
			);
		}
	;

fndef:	   typed_declspecs setspecs declarator
		{ if (! start_function ($1, $3))
		    YYFAIL;
		  reinit_parse_for_function ();
		}
	   xdecls
		{ store_parm_decls (); }
	   compstmt_or_error
		{ finish_function (lineno); }
	|  typed_declspecs setspecs declarator error
		{ }
	|  declmods setspecs notype_declarator
		{ if (! start_function ($1, $3))
		    YYFAIL;
		  reinit_parse_for_function ();
		}
	   xdecls
		{ store_parm_decls (); }
	   compstmt_or_error
		{ finish_function (lineno); }
	|  declmods setspecs notype_declarator error
		{ }
	|  setspecs notype_declarator
		{ if (! start_function (0, $2))
		    YYFAIL;
		  reinit_parse_for_function ();
		}
	   xdecls
		{ store_parm_decls (); }
	   compstmt_or_error
		{ finish_function (lineno); }
	|  setspecs notype_declarator error
		{ }
	;

identifier:
	   IDENTIFIER
	|  TYPENAME
	;

unop:      '&'
		{ $$ = ADDR_EXPR; }
	|  '-'
		{ $$ = NEGATE_EXPR; }
	|  '+'
		{ $$ = CONVERT_EXPR; }
	|  PLUSPLUS
		{ $$ = PREINCREMENT_EXPR; }
	|  MINUSMINUS
		{ $$ = PREDECREMENT_EXPR; }
	|  '~'
		{ $$ = BIT_NOT_EXPR; }
	|  '!'
		{ $$ = TRUTH_NOT_EXPR; }
	;

expr:	   nonnull_exprlist
		{ $$ = build_compound_expr ($1); }
	;

exprlist:  /* empty */
		{ $$ = NULL_TREE; }
	|  nonnull_exprlist
	;

nonnull_exprlist:
	   expr_no_commas
		{ $$ = build_tree_list (NULL_TREE, $1); }
	|  nonnull_exprlist ',' expr_no_commas
		{ chainon ($1, build_tree_list (NULL_TREE, $3)); }
	;

unary_expr:
	   primary
	|  '*' cast_expr   %prec UNARY
		{ $$ = build_indirect_ref ($2, "unary *"); }
	|  unop cast_expr  %prec UNARY
		{ $$ = build_unary_op ($1, $2, 0); }
	|  SIZEOF unary_expr  %prec UNARY
		{ if
		  (	TREE_CODE ($2) == COMPONENT_REF
		  	&& TREE_PACKED (TREE_OPERAND ($2, 1))
		  )
		  	error ("`sizeof' applied to a bit-field");

		  /*
		   *	ANSI says arrays and functions are converted inside
		   *	comma. But we can't really convert them in
		   *	build_compound_expr because that would break commas
		   *	in lvalues.
		   *	So do the conversion here if operand was a comma.
		   */
		  if
		  (	TREE_CODE ($2) == COMPOUND_EXPR
		        && (TREE_CODE (TREE_TYPE ($2)) == ARRAY_TYPE
			|| TREE_CODE (TREE_TYPE ($2)) == FUNCTION_TYPE)
		  )
		  	$2 = default_conversion ($2);

		  $$ = c_sizeof (TREE_TYPE ($2));
		}
	|  SIZEOF '(' typename ')'  %prec HYPERUNARY
		{ $$ = c_sizeof (groktypename ($3)); }
	|  ALIGNOF unary_expr  %prec UNARY
		{ if
		  (	TREE_CODE ($2) == COMPONENT_REF &&
		        TREE_PACKED (TREE_OPERAND ($2, 1))
		  )
		  	error ("`__alignof' applied to a bit-field");

		  if (TREE_CODE ($2) == INDIRECT_REF)
		  {
		      tree t = TREE_OPERAND ($2, 0);
		      tree best = t;
		      int bestalign = TYPE_ALIGN (TREE_TYPE (TREE_TYPE (t)));

		      while
		      (	TREE_CODE (t) == NOP_EXPR &&
			TREE_CODE (TREE_TYPE (TREE_OPERAND (t, 0))) ==
								POINTER_TYPE
		      )
		      {
			  int thisalign;

			  t = TREE_OPERAND (t, 0);
			  thisalign = TYPE_ALIGN (TREE_TYPE (TREE_TYPE (t)));
			  if (thisalign > bestalign)
				best = t, bestalign = thisalign;
		      }

		      $$ = c_alignof (TREE_TYPE (TREE_TYPE (best)));
		    }
		    else
		    {
			/*
			 *	ANSI says arrays and fns are converted
			 *	inside comma. But we can't convert them
			 *	in build_compound_expr because that would
			 *	break commas in lvalues. So do the conversion
			 *	here if operand was a comma.
			 */
			if
			(	TREE_CODE ($2) == COMPOUND_EXPR &&
				(TREE_CODE (TREE_TYPE ($2)) == ARRAY_TYPE ||
				TREE_CODE (TREE_TYPE ($2)) == FUNCTION_TYPE)
			)
			$2 = default_conversion ($2);

			$$ = c_alignof (TREE_TYPE ($2));
		    }
		}
	|  ALIGNOF '(' typename ')'  %prec HYPERUNARY
		{ $$ = c_alignof (groktypename ($3)); }
	;

cast_expr:
	   unary_expr
	|  '(' typename ')' cast_expr  %prec UNARY
		{ tree type = groktypename ($2);
		  $$ = build_c_cast (type, $4);
		}
	|  '(' typename ')' '{' initlist maybecomma '}'  %prec UNARY
		{ tree type = groktypename ($2);
		  if (pedantic)
		    warning ("ANSI C forbids constructor expressions");
		  $$ = digest_init (type,
			build_nt (CONSTRUCTOR, NULL_TREE, nreverse ($5)), 0);
		  if (TREE_CODE (type) == ARRAY_TYPE && TYPE_SIZE (type) == 0)
		  {
		      int failure = complete_array_type (type, $$, 1);

		      if (failure)
				abort ();
		  }
		}
	;

expr_no_commas:
	   cast_expr
	|  expr_no_commas '+' expr_no_commas
		{ $$ = build_binary_op ($2, $1, $3); }
	|  expr_no_commas '-' expr_no_commas
		{ $$ = build_binary_op ($2, $1, $3); }
	|  expr_no_commas '*' expr_no_commas
		{ $$ = build_binary_op ($2, $1, $3); }
	|  expr_no_commas '/' expr_no_commas
		{ $$ = build_binary_op ($2, $1, $3); }
	|  expr_no_commas '%' expr_no_commas
		{ $$ = build_binary_op ($2, $1, $3); }
	|  expr_no_commas LSHIFT expr_no_commas
		{ $$ = build_binary_op ($2, $1, $3); }
	|  expr_no_commas RSHIFT expr_no_commas
		{ $$ = build_binary_op ($2, $1, $3); }
	|  expr_no_commas ARITHCOMPARE expr_no_commas
		{ $$ = build_binary_op ($2, $1, $3); }
	|  expr_no_commas EQCOMPARE expr_no_commas
		{ $$ = build_binary_op ($2, $1, $3); }
	|  expr_no_commas '&' expr_no_commas
		{ $$ = build_binary_op ($2, $1, $3); }
	|  expr_no_commas '|' expr_no_commas
		{ $$ = build_binary_op ($2, $1, $3); }
	|  expr_no_commas '^' expr_no_commas
		{ $$ = build_binary_op ($2, $1, $3); }
	|  expr_no_commas ANDAND expr_no_commas
		{ $$ = build_binary_op (TRUTH_ANDIF_EXPR, $1, $3); }
	|  expr_no_commas OROR expr_no_commas
		{ $$ = build_binary_op (TRUTH_ORIF_EXPR, $1, $3); }
	|  expr_no_commas '?' xexpr ':' expr_no_commas
		{ $$ = build_conditional_expr ($1, $3, $5); }
	|  expr_no_commas '=' expr_no_commas
		{ $$ = build_modify_expr ($1, NOP_EXPR, $3); }
	|  expr_no_commas ASSIGN expr_no_commas
		{ $$ = build_modify_expr ($1, $2, $3); }
	;

primary:
	   IDENTIFIER
		{ $$ = lastiddecl;

		  if (!$$ || $$ == error_mark_node)
		  {	
			if (yychar == YYEMPTY)
				yychar = YYLEX;

			if   (yychar == '(')
			{
				$$ = implicitly_declare ($1);
				assemble_external ($$);
				TREE_USED ($$) = 1;
			}
			elif (current_function_decl == 0)
			{
				error
				(	"`%s' undeclared, outside of functions",
					 IDENTIFIER_POINTER ($1)
				);
				$$ = error_mark_node;
			}
			else
			{
				if
				(	IDENTIFIER_GLOBAL_VALUE ($1) !=
								error_mark_node
					|| IDENTIFIER_ERROR_LOCUS ($1) !=
							current_function_decl
				)
				{
					error
					(	"`%s' undeclared",
#if (0)	/*******************************************************/
						" (first use this function)",
#endif	/*******************************************************/
						IDENTIFIER_POINTER ($1)
					);

					if (! undeclared_variable_notice)
					{
#if (0)	/*******************************************************/
						error
						(	"(Each undeclared "
							"identifier is reported "
							"only once"
						);
						error
						(	"for each function "
							"it appears in.)"
						);
#endif	/*******************************************************/
						undeclared_variable_notice = 1;
					}
				}

				$$ = error_mark_node;

				/* Prevent repeated error messages. */

				IDENTIFIER_GLOBAL_VALUE ($1) = error_mark_node;
				IDENTIFIER_ERROR_LOCUS ($1) = current_function_decl;
			}
		  }
		  elif (! TREE_USED ($$))
		  {
			if (TREE_EXTERNAL ($$))
				assemble_external ($$);
			TREE_USED ($$) = 1;
		  }

		  if (TREE_CODE ($$) == CONST_DECL)
			$$ = DECL_INITIAL ($$);
		}
	|  CONSTANT
	|  string
		{ $$ = combine_strings ($1); }
	|  '(' expr ')'
		{ $$ = $2; }
	|  '(' error ')'
		{ $$ = error_mark_node; }
	|  '('
		{ if (current_function_decl == 0)
		  {
			error
			(	"braced-group within expression allowed "
				"only inside a function"
			);
			YYFAIL;
		  }

		  keep_next_level ();
		  $<ttype>$ = expand_start_stmt_expr ();
		}
	   compstmt ')'
		{ tree rtl_exp;
		  if (pedantic)
			warning ("ANSI C forbids braced-groups within expressions");
		  rtl_exp = expand_end_stmt_expr ($<ttype>2);
		  $$ = $3;
		  TREE_USED ($$) = 0;

		  /*
		   *	Since the statements have side effects,
		   *	consider this volatile.
		   */
		  TREE_VOLATILE ($$) = 1;
		  TREE_TYPE ($$) = TREE_TYPE (rtl_exp);
		  STMT_BODY ($$) = rtl_exp;
		}
	|  primary '(' exprlist ')'   %prec '.'
		{ $$ = build_function_call ($1, $3); }
	|  primary '[' expr ']'   %prec '.'
		{ $$ = build_array_ref ($1, $3); }
	|  primary '.' identifier
		{ $$ = build_component_ref ($1, $3); }
	|  primary POINTSAT identifier
		{ $$ = build_component_ref (build_indirect_ref ($1, "->"), $3); }
	|  primary PLUSPLUS
		{ $$ = build_unary_op (POSTINCREMENT_EXPR, $1, 0); }
	|  primary MINUSMINUS
		{ $$ = build_unary_op (POSTDECREMENT_EXPR, $1, 0); }
	;

/*
 *	Produces a STRING_CST with perhaps more STRING_CSTs chained onto it.
 */
string:
	   STRING
	|  string STRING
		{ $$ = chainon ($1, $2); }
	;

xdecls:
	   /* empty */
	|  decls
	;

decls:
	   decl
	|  errstmt
	|  decls decl
	|  decl errstmt
	;

/*
 *	Records the type and storage class specs to use for processing
 *	the declarators that follow.
 *	Maintains a stack of outer-level values of current_declspecs,
 *	for the sake of parm declarations nested in function declarators.
 */
setspecs:  /* empty */
		{ $$ = suspend_momentary ();
		  declspec_stack = tree_cons (0, current_declspecs,
						      declspec_stack);
		  current_declspecs = $<ttype>0;
		}
	;

decl:
	   typed_declspecs setspecs initdecls ';'
		{ current_declspecs = TREE_VALUE (declspec_stack);
		  declspec_stack = TREE_CHAIN (declspec_stack);
		  resume_momentary ($2);
		}
	|  declmods setspecs notype_initdecls ';'
		{ current_declspecs = TREE_VALUE (declspec_stack);
		  declspec_stack = TREE_CHAIN (declspec_stack);
		  resume_momentary ($2);
		}
	|  typed_declspecs ';'
		{ shadow_tag ($1); }
	|  declmods ';'
		{ warning ("empty declaration"); }
	;

/*
 *	Declspecs which contain at least one type specifier or typedef name.
 *	(Just `const' or `volatile' is not enough.)
 *	A typedef'd name following these is taken as a name to be declared.
 */
typed_declspecs:
	   typespec reserved_declspecs
		{ $$ = tree_cons (NULL_TREE, $1, $2); }
	|  declmods typespec reserved_declspecs
		{ $$ = chainon ($3, tree_cons (NULL_TREE, $2, $1)); }
	;

reserved_declspecs:
	   /* empty */
		{ $$ = NULL_TREE; }
	|  reserved_declspecs typespecqual_reserved
		{ $$ = tree_cons (NULL_TREE, $2, $1); }
	|  reserved_declspecs SCSPEC
		{ $$ = tree_cons (NULL_TREE, $2, $1); }
	;

/*
 *	List of just storage classes and type modifiers.
 *	A declaration can start with just this, but then it cannot be used
 *	to redeclare a typedef-name.
 */
declmods:
	   TYPE_QUAL
		{ $$ = tree_cons (NULL_TREE, $1, NULL_TREE); }
	|  SCSPEC
		{ $$ = tree_cons (NULL_TREE, $1, NULL_TREE); }
	|  declmods TYPE_QUAL
		{ $$ = tree_cons (NULL_TREE, $2, $1); }
	|  declmods SCSPEC
		{ $$ = tree_cons (NULL_TREE, $2, $1); }
	;


/*
 *	Used instead of declspecs where storage classes are not allowed
 *	(that is, for typenames and structure components).
 *	Don't accept a typedef-name if anything but a modifier precedes it.
 */
typed_typespecs:
	   typespec reserved_typespecquals
		{ $$ = tree_cons (NULL_TREE, $1, $2); }
	|  nonempty_type_quals typespec reserved_typespecquals
		{ $$ = chainon ($3, tree_cons (NULL_TREE, $2, $1)); }
	;

reserved_typespecquals:  /* empty */
		{ $$ = NULL_TREE; }
	|  reserved_typespecquals typespecqual_reserved
		{ $$ = tree_cons (NULL_TREE, $2, $1); }
	;

/*
 *	A typespec (but not a type qualifier).
 *	Once we have seen one of these in a declaration,
 *	if a typedef name appears then it is being redeclared.
 */
typespec:
	   TYPESPEC
	|  structsp
	|  TYPENAME
	|  TYPEOF '(' expr ')'
		{ $$ = TREE_TYPE ($3); }
	|  TYPEOF '(' typename ')'
		{ $$ = groktypename ($3); }
	;

/*
 *	A typespec that is a reserved word, or a type qualifier.
 */
typespecqual_reserved:
	   TYPESPEC
	|  TYPE_QUAL
	|  structsp
	;

initdecls:
	   initdcl
	|  initdecls ',' initdcl
	;

notype_initdecls:
	   notype_initdcl
	|  notype_initdecls ',' initdcl
	;

maybeasm:
	  /* empty */
		{ $$ = NULL_TREE; }
	|  ASM '(' string ')'
		{ if (TREE_CHAIN ($3))
			$3 = combine_strings ($3);
		  $$ = $3;
		}
	;

/*
 *	Note how the declaration (in "init") of the variable is
 *	in effect while its init is parsed!
 */
initdcl:
	   declarator maybeasm maybe_attribute '='
		{ $<ttype>$ = start_decl ($1, current_declspecs, 1); }
	   init
		{ finish_decl ($<ttype>5, $6, $2); }
	|  declarator maybeasm maybe_attribute
		{ tree d = start_decl ($1, current_declspecs, 0);
		  finish_decl (d, NULL_TREE, $2);
		}
	;

notype_initdcl:
	   notype_declarator maybeasm maybe_attribute '='
		{ $<ttype>$ = start_decl ($1, current_declspecs, 1); }
	   init
		{ finish_decl ($<ttype>5, $6, $2); }
	|  notype_declarator maybeasm maybe_attribute
		{ tree d = start_decl ($1, current_declspecs, 0);
		  finish_decl (d, NULL_TREE, $2);
		}
	;

/*
 *	the * rules are dummies to accept the Apollo extended syntax
 *	so that the header files compile.
 */
maybe_attribute:
	   /* empty */
		{ $$ = NULL_TREE; }
	|  ATTRIBUTE '(' '(' attribute_list ')' ')'
	        { $$ = $4; }
	;

attribute_list:
	   attrib
	|  attribute_list ',' attrib
	;

attrib:
	   IDENTIFIER
		{ warning ("`%s' attribute directive ignored",
						IDENTIFIER_POINTER ($1));
		  $$ = $1;
		}
	|  IDENTIFIER '(' CONSTANT ')'
		{
		  /*
		   *	if not "aligned (1)", then issue warning
		   */
		  if
		  (	strcmp (IDENTIFIER_POINTER ($1), "aligned") != 0 ||
			TREE_CODE ($3) != INTEGER_CST ||
			TREE_INT_CST_LOW ($3) != 1
		  )
			warning ("`%s' attribute directive ignored",
						IDENTIFIER_POINTER ($1));
		  $$ = $1;
		}
	|  IDENTIFIER '(' identifiers ')'
		{ warning ("`%s' attribute directive ignored",
						IDENTIFIER_POINTER ($1));
		  $$ = $1;
		}
	;

init:
	   expr_no_commas
	|  '{' '}'
		{ $$ = build_nt (CONSTRUCTOR, NULL_TREE, NULL_TREE);
		  if (pedantic)
		    warning ("ANSI C forbids empty initializer braces");
		}
	|  '{' initlist '}'
		{ $$ = build_nt (CONSTRUCTOR, NULL_TREE, nreverse ($2)); }
	|  '{' initlist ',' '}'
		{ $$ = build_nt (CONSTRUCTOR, NULL_TREE, nreverse ($2)); }
	|  error
		{ $$ = NULL_TREE; }
	;

/*
 *	This chain is built in reverse order,
 *	and put in forward order where initlist is used.
 */
initlist:
	   init
		{ $$ = build_tree_list (NULL_TREE, $1); }
	|  initlist ',' init
		{ $$ = tree_cons (NULL_TREE, $3, $1); }
	;

/*
 *	Any kind of declarator (thus, all declarators allowed
 *	after an explicit typespec).
 */
declarator:
	   after_type_declarator
	|  notype_declarator
	;

/*
 *	A declarator that is allowed only after an explicit typespec.
 */
after_type_declarator:
	   '(' after_type_declarator ')'
		{ $$ = $2; }
	|  after_type_declarator '(' parmlist_or_identifiers  %prec '.'
		{ $$ = build_nt (CALL_EXPR, $1, $3, NULL_TREE); }
/***	|  after_type_declarator '(' error ')'  %prec '.'
		{ $$ = build_nt (CALL_EXPR, $1, NULL_TREE, NULL_TREE);
		  poplevel (0, 0, 0); }  ***/
	|  after_type_declarator '[' expr ']'  %prec '.'
		{ $$ = build_nt (ARRAY_REF, $1, $3); }
	|  after_type_declarator '[' ']'  %prec '.'
		{ $$ = build_nt (ARRAY_REF, $1, NULL_TREE); }
	|  '*' type_quals after_type_declarator  %prec UNARY
		{ $$ = make_pointer_declarator ($2, $3); }
	|  TYPENAME
	;

/*
 *	Kinds of declarator that can appear in a parameter list
 *	in addition to notype_declarator.  This is like after_type_declarator
 *	but does not allow a typedef name in parentheses as an identifier
 *	(because it would conflict with a function with that typedef as arg).
 */
parm_declarator:
	   parm_declarator '(' parmlist_or_identifiers  %prec '.'
		{ $$ = build_nt (CALL_EXPR, $1, $3, NULL_TREE); }
/***	|  parm_declarator '(' error ')'  %prec '.'
		{ $$ = build_nt (CALL_EXPR, $1, NULL_TREE, NULL_TREE);
		  poplevel (0, 0, 0); }  ***/
	|  parm_declarator '[' expr ']'  %prec '.'
		{ $$ = build_nt (ARRAY_REF, $1, $3); }
	|  parm_declarator '[' ']'  %prec '.'
		{ $$ = build_nt (ARRAY_REF, $1, NULL_TREE); }
	|  '*' type_quals parm_declarator  %prec UNARY
		{ $$ = make_pointer_declarator ($2, $3); }
	|  TYPENAME
	;

/*
 *	A declarator allowed whether or not there has been
 *	an explicit typespec.  These cannot redeclare a typedef-name.
 */
notype_declarator:
	   notype_declarator '(' parmlist_or_identifiers  %prec '.'
		{ $$ = build_nt (CALL_EXPR, $1, $3, NULL_TREE); }
/***	|  notype_declarator '(' error ')'  %prec '.'
		{ $$ = build_nt (CALL_EXPR, $1, NULL_TREE, NULL_TREE);
		  poplevel (0, 0, 0); }  ***/
	|  '(' notype_declarator ')'
		{ $$ = $2; }
	|  '*' type_quals notype_declarator  %prec UNARY
		{ $$ = make_pointer_declarator ($2, $3); }
	|  notype_declarator '[' expr ']'  %prec '.'
		{ $$ = build_nt (ARRAY_REF, $1, $3); }
	|  notype_declarator '[' ']'  %prec '.'
		{ $$ = build_nt (ARRAY_REF, $1, NULL_TREE); }
	|  IDENTIFIER
	;

structsp:
	   STRUCT identifier '{'
		{ $$ = start_struct (RECORD_TYPE, $2);
		  /* Start scope of tag before parsing components.  */
		}
	   component_decl_list '}'
		{ $$ = finish_struct ($<ttype>4, $5);
		  /* Really define the structure.  */
		}
	|  STRUCT '{' component_decl_list '}'
		{ $$ = finish_struct (start_struct (RECORD_TYPE, NULL_TREE),
				      $3);
		}
	|  STRUCT identifier
		{ $$ = xref_tag (RECORD_TYPE, $2); }
	|  UNION identifier '{'
		{ $$ = start_struct (UNION_TYPE, $2); }
	   component_decl_list '}'
		{ $$ = finish_struct ($<ttype>4, $5); }
	|  UNION '{' component_decl_list '}'
		{ $$ = finish_struct (start_struct (UNION_TYPE, NULL_TREE),
				      $3);
		}
	|  UNION identifier
		{ $$ = xref_tag (UNION_TYPE, $2); }
	|  ENUM identifier '{'
		{ $<itype>3 = suspend_momentary ();
		  $$ = start_enum ($2);
		}
	   enumlist maybecomma_warn '}'
		{ $$ = finish_enum ($<ttype>4, nreverse ($5));
		  resume_momentary ($<itype>3);
		}
	|  ENUM '{'
		{ $<itype>2 = suspend_momentary ();
		  $$ = start_enum (NULL_TREE);
		}
	   enumlist maybecomma_warn '}'
		{ $$ = finish_enum ($<ttype>3, nreverse ($4));
		  resume_momentary ($<itype>2);
		}
	|  ENUM identifier
		{ $$ = xref_tag (ENUMERAL_TYPE, $2); }
	;

maybecomma:
	   /* empty */
	|  ','
	;

maybecomma_warn:
	   /* empty */
	|  ','
		{ if (pedantic)
			warning ("comma at end of enumerator list");
		}
	;

component_decl_list:
	   component_decl_list2
		{ $$ = $1; }
	|  component_decl_list2 component_decl
		{ $$ = chainon ($1, $2);
		  warning ("no semicolon at end of struct or union");
		}
	;

component_decl_list2:
	   /* empty */
		{ $$ = NULL_TREE; }
	|  component_decl_list2 component_decl ';'
		{ $$ = chainon ($1, $2); }
	|  component_decl_list2 ';'
		{ if (pedantic)
		    warning ("extra semicolon in struct or union specified");
		}
	;

/*
 *	There is a shift-reduce conflict here, because `components' may
 *	start with a `typename'.  It happens that shifting
 *	(the default resolution) does the right thing, because it
 *	treats the `typename' as part of a `typed_typespecs'.
 *
 *	It is possible that this same technique would allow the distinction
 *	between `notype_initdecls' and `initdecls' to be eliminated.
 *	But I am being cautious and not trying it.
 */
component_decl:
	   typed_typespecs setspecs components
		{ $$ = $3;
		  current_declspecs = TREE_VALUE (declspec_stack);
		  declspec_stack = TREE_CHAIN (declspec_stack);
		  resume_momentary ($2);
		}
	|  nonempty_type_quals setspecs components
		{ $$ = $3;
		  current_declspecs = TREE_VALUE (declspec_stack);
		  declspec_stack = TREE_CHAIN (declspec_stack);
		  resume_momentary ($2);
		}
	|  error
		{ $$ = NULL_TREE; }
	;

components:
	   /* empty */
		{ if (pedantic)
		    warning ("ANSI C forbids member declarations with no members");
		  $$ = NULL_TREE;
		}
	|  component_declarator
	|  components ',' component_declarator
		{ $$ = chainon ($1, $3); }
	;

component_declarator:
	   declarator maybe_attribute
		{ $$ = grokfield (input_filename, lineno, $1,
						current_declspecs, NULL_TREE);
		}
	|  declarator ':' expr_no_commas maybe_attribute
		{ $$ = grokfield (input_filename, lineno, $1,
						current_declspecs, $3);
		}
	|  ':' expr_no_commas
		{ $$ = grokfield (input_filename, lineno, NULL_TREE,
						current_declspecs, $2);
		}
	;

/*
 *	We chain the enumerators in reverse order.
 *	They are put in forward order where enumlist is used.
 *	(The order used to be significant, but no longer is so.
 *	However, we still maintain the order, just to be clean.)
 */
enumlist:
	   enumerator
	|  enumlist ',' enumerator
		{ $$ = chainon ($3, $1); }
	;

enumerator:
	   identifier
		{ $$ = build_enumerator ($1, NULL_TREE); }
	|  identifier '=' expr_no_commas
		{ $$ = build_enumerator ($1, $3); }
	;

typename:
	   typed_typespecs absdcl
		{ $$ = build_tree_list ($1, $2); }
	|  nonempty_type_quals absdcl
		{ $$ = build_tree_list ($1, $2); }
	;

/*
 *	an absolute declarator
 */
absdcl:
	   /* empty */
		{ $$ = NULL_TREE; }
	|  absdcl1
	;

nonempty_type_quals:
	   TYPE_QUAL
		{ $$ = tree_cons (NULL_TREE, $1, NULL_TREE); }
	|  nonempty_type_quals TYPE_QUAL
		{ $$ = tree_cons (NULL_TREE, $2, $1); }
	;

type_quals:
	   /* empty */
		{ $$ = NULL_TREE; }
	|  type_quals TYPE_QUAL
		{ $$ = tree_cons (NULL_TREE, $2, $1); }
	;

/*
 *	a nonempty absolute declarator
 */
absdcl1:  
	   '(' absdcl1 ')'
		{ $$ = $2; }
	   /* `(typedef)1' is `int'.  */
	|  '*' type_quals absdcl1  %prec UNARY
		{ $$ = make_pointer_declarator ($2, $3); }
	|  '*' type_quals  %prec UNARY
		{ $$ = make_pointer_declarator ($2, NULL_TREE); }
	|  absdcl1 '(' parmlist  %prec '.'
		{ $$ = build_nt (CALL_EXPR, $1, $3, NULL_TREE); }
	|  absdcl1 '[' expr ']'  %prec '.'
		{ $$ = build_nt (ARRAY_REF, $1, $3); }
	|  absdcl1 '[' ']'  %prec '.'
		{ $$ = build_nt (ARRAY_REF, $1, NULL_TREE); }
	|  '(' parmlist  %prec '.'
		{ $$ = build_nt (CALL_EXPR, NULL_TREE, $2, NULL_TREE); }
	|  '[' expr ']'  %prec '.'
		{ $$ = build_nt (ARRAY_REF, NULL_TREE, $2); }
	|  '[' ']'  %prec '.'
		{ $$ = build_nt (ARRAY_REF, NULL_TREE, NULL_TREE); }
	;

/*
 *	At least one statement, the first of which parses without error.
 *	stmts is used only after decls, so an invalid first statement
 *	is actually regarded as an invalid decl and part of the decls.
 */
stmts:
	   stmt
	|  stmts stmt
	|  stmts errstmt
	;

xstmts:
	   /* empty */
	|  stmts
	;

errstmt:
	   error ';'
	;

pushlevel:
	   /* empty */
		{ pushlevel (0);
		  clear_last_expr ();
		  push_momentary ();
		  expand_start_bindings (0);
		}
	;

/*
 *	This is the body of a function definition.
 *	It causes syntax errors to ignore to the next openbrace.
 */
compstmt_or_error:
	   compstmt
		{}
	|  error compstmt
	;

compstmt: 
	   '{' '}'
		{ $$ = 0; }
	|  '{' pushlevel decls xstmts '}'
		{ expand_end_bindings (getdecls (), 1, 0);
		  $$ = poplevel (1, 1, 0);
		  pop_momentary ();
		}
	|  '{' pushlevel error '}'
		{ expand_end_bindings (getdecls (), kept_level_p (), 0);
		  $$ = poplevel (kept_level_p (), 0, 0);
		  pop_momentary ();
		}
	|  '{' pushlevel stmts '}'
		{ expand_end_bindings (getdecls (), kept_level_p (), 0);
		  $$ = poplevel (kept_level_p (), 0, 0);
		  pop_momentary ();
		}
	;

simple_if:
	   IF '(' expr ')'
		{ emit_line_note (input_filename, lineno);
		  expand_start_cond (truthvalue_conversion ($3), 0);
		}
	   stmt
	;

stmt:
	   compstmt
		{}
	|  expr ';'
		{ emit_line_note (input_filename, lineno);
		  /*
		   *	Do default conversion if safe and possibly important,
		   *	in case within ({...}).
		   */
		  if
		  (	(TREE_CODE (TREE_TYPE ($1)) == ARRAY_TYPE &&
								lvalue_p ($1))
		      || TREE_CODE (TREE_TYPE ($1)) == FUNCTION_TYPE
		  )
			$1 = default_conversion ($1);

		  expand_expr_stmt ($1);
		  clear_momentary ();
		}
	|  simple_if ELSE
		{ expand_start_else (); }
	   stmt
		{ expand_end_else (); }
	|  simple_if %prec IF
		{ expand_end_cond (); }
	|  WHILE
		{ emit_nop ();
		  emit_line_note (input_filename, lineno);
		  expand_start_loop (1);
		}
	   '(' expr ')'
		{ emit_line_note (input_filename, lineno);
		  expand_exit_loop_if_false (truthvalue_conversion ($4));
		}
	   stmt
		{ expand_end_loop (); }
	|  DO
		{ emit_nop ();
		  emit_line_note (input_filename, lineno);
		  expand_start_loop_continue_elsewhere (1);
		}
	   stmt WHILE
		{ expand_loop_continue_here (); }
	   '(' expr ')' ';'
		{ emit_line_note (input_filename, lineno);
		  expand_exit_loop_if_false (truthvalue_conversion ($7));
		  expand_end_loop ();
		  clear_momentary ();
		}
	|  FOR
	   '(' xexpr ';'
		{ emit_nop ();
		  emit_line_note (input_filename, lineno);
		  if ($3)
			expand_expr_stmt ($3);
		  expand_start_loop_continue_elsewhere (1);
		}
	   xexpr ';'
		{ emit_line_note (input_filename, lineno);
		  if ($6)
		    expand_exit_loop_if_false (truthvalue_conversion ($6));
		}
	   xexpr ')'
		/*
		 *	Don't let the tree nodes for $9 be discarded
		 *	by clear_momentary during the parsing of the next stmt.
		 */
		{ push_momentary ();
		  $<itype>10 = lineno;
		}
	   stmt
		{ emit_line_note (input_filename, $<itype>10);
		  expand_loop_continue_here ();
		  if ($9)
		    expand_expr_stmt ($9);
		  pop_momentary ();
		  expand_end_loop ();
		}
	|  SWITCH '(' expr ')'
		{ emit_line_note (input_filename, lineno);
		  c_expand_start_case ($3);
		  /*
		   *	Don't let the tree nodes for $3 be discarded by
		   *	clear_momentary during the parsing of the next stmt.
		   */
		  push_momentary ();
		}
	   stmt
		{ expand_end_case ($3);
		  pop_momentary ();
		}
	|  CASE expr ':'
		{ register tree value = fold ($2);
		  register tree label =
			    build_decl (LABEL_DECL, NULL_TREE, NULL_TREE);

		  /*
		   *	build_c_cast puts on a NOP_EXPR to make a non-lvalue.
		   *	Strip such NOP_EXPRs.
		   */
		  if
		  (	TREE_CODE (value) == NOP_EXPR &&
			TREE_TYPE (value) == TREE_TYPE (TREE_OPERAND
								(value, 0))
		  )
			    value = TREE_OPERAND (value, 0);

		  if
		  (	TREE_CODE (value) != INTEGER_CST &&
			value != error_mark_node
		  )
		  {
		      error ("case label does not reduce to an integer constant");
		      value = error_mark_node;
		  }
		  else
		  {
			/*
			 *	Promote char or short to int.
			 */
			value = default_conversion (value);
		  }

		  if (value != error_mark_node)
		  {
			  int success = pushcase (value, label);

			  if   (success == 1)
				error ("case label not within a switch statement");
			  elif (success == 2)
				error ("duplicate case value");
			  elif (success == 3)
				warning ("case value out of range");
		  }
		}
	   stmt
	|  DEFAULT ':'
		{
		  register tree label =
			    build_decl (LABEL_DECL, NULL_TREE, NULL_TREE);
		  int success = pushcase (NULL_TREE, label);

		  if   (success == 1)
		    error ("default label not within a switch statement");
		  elif (success == 2)
		    error ("multiple default labels in one switch");
		}
	   stmt
	|  BREAK ';'
		{ emit_line_note (input_filename, lineno);
		  if ( ! expand_exit_something ())
		    error ("break statement not within loop or switch");
		}
	|  CONTINUE ';'
		{ emit_line_note (input_filename, lineno);
		  if (! expand_continue_loop ())
			    error ("continue statement not within a loop");
		}
	|  RETURN ';'
		{ emit_line_note (input_filename, lineno);
		  c_expand_return (NULL_TREE);
		}
	|  RETURN expr ';'
		{ emit_line_note (input_filename, lineno);
		  c_expand_return ($2);
		}
	|  ASM maybe_type_qual '(' string ')' ';'
		{ if (TREE_CHAIN ($4)) $4 = combine_strings ($4);
		  emit_line_note (input_filename, lineno);
		  expand_asm ($4);
		}
	/* This is the case with just output operands.  */
	|  ASM maybe_type_qual '(' string ':' asm_operands ')' ';'
		{ if (TREE_CHAIN ($4))
			$4 = combine_strings ($4);
		  emit_line_note (input_filename, lineno);
		  c_expand_asm_operands ($4, $6, NULL_TREE, NULL_TREE,
					 $2 == ridpointers[(int)RID_VOLATILE],
					 input_filename, lineno);
		}
	/* This is the case with input operands as well.  */
	|  ASM maybe_type_qual '(' string ':' asm_operands ':' asm_operands ')' ';'
		{ if (TREE_CHAIN ($4))
			$4 = combine_strings ($4);
		  emit_line_note (input_filename, lineno);
		  c_expand_asm_operands ($4, $6, $8, NULL_TREE,
					 $2 == ridpointers[(int)RID_VOLATILE],
					 input_filename, lineno);
		}
	/* This is the case with clobbered registers as well.  */
	|  ASM maybe_type_qual '(' string ':' asm_operands ':'
  	   asm_operands ':' asm_clobbers ')' ';'
		{ if (TREE_CHAIN ($4))
			$4 = combine_strings ($4);
		  emit_line_note (input_filename, lineno);
		  c_expand_asm_operands ($4, $6, $8, $10,
					 $2 == ridpointers[(int)RID_VOLATILE],
					 input_filename, lineno);
		}
	|  GOTO identifier ';'
		{ tree decl;
		  emit_line_note (input_filename, lineno);
		  decl = lookup_label ($2);
		  TREE_USED (decl) = 1;
		  expand_goto (decl);
		}
	|  identifier ':'
		{ tree label = define_label (input_filename, lineno, $1);
		  emit_nop ();
		  if (label)
		    expand_label (label);
		}
	   stmt
	|  ';'
	;

/*
 *	Either a type-qualifier or nothing.
 *	First thing in an `asm' statement.
 */
maybe_type_qual:
	   /* empty */
		{ emit_line_note (input_filename, lineno); }
	|  TYPE_QUAL
		{ emit_line_note (input_filename, lineno); }
	;

xexpr:
	   /* empty */
		{ $$ = NULL_TREE; }
	|  expr
	;

/*
 *	These are the operands other than the first string and colon
 *	in  asm ("addextend %2,%1": "=dm" (x), "0" (y), "g" (*x))
 */
asm_operands:
	   /* empty */
		{ $$ = NULL_TREE; }
	|  nonnull_asm_operands
	;

nonnull_asm_operands:
	   asm_operand
	|  nonnull_asm_operands ',' asm_operand
		{ $$ = chainon ($1, $3); }
	;

asm_operand:
	   STRING '(' expr ')'
		{ $$ = build_tree_list ($1, $3); }
	;

asm_clobbers:
	   string
		{ $$ = tree_cons (NULL_TREE, combine_strings ($1), NULL_TREE); }
	|  asm_clobbers ',' string
		{ $$ = tree_cons (NULL_TREE, combine_strings ($3), $1); }
	;

/*
 *	This is what appears inside the parens in a function declarator.
 *	Its value is a list of ..._TYPE nodes.
 */
parmlist:
		{ pushlevel (0);
		  declare_parm_level ();
		}
	   parmlist_1
		{ $$ = $2;
		  parmlist_tags_warning ();
		  poplevel (0, 0, 0);
		}
	;

/*
 *	This is referred to where either a parmlist or an identifier list is ok.
 *	Its value is a list of ..._TYPE nodes or a list of identifiers.
 */
parmlist_or_identifiers:
		{ pushlevel (0);
		  declare_parm_level ();
		}
	   parmlist_or_identifiers_1
		{ $$ = $2;
		  parmlist_tags_warning ();
		  poplevel (0, 0, 0);
		}
	;

parmlist_or_identifiers_1:
	   parmlist_2 ')'
	|  identifiers ')'
		{ $$ = tree_cons (NULL_TREE, NULL_TREE, $1); }
	|  error ')'
		{ $$ = tree_cons (NULL_TREE, NULL_TREE, NULL_TREE); }
	;

parmlist_1:
	   parmlist_2 ')'
	|  error ')'
		{ $$ = tree_cons (NULL_TREE, NULL_TREE, NULL_TREE); }
	;

/*
 *	This is what appears inside the parens in a function declarator.
 *	Is value is represented in the format that grokdeclarator expects.
 */
parmlist_2:
	   /* empty */
		{ $$ = get_parm_info (0); }
	|  parms
		{ $$ = get_parm_info (1); }
	|  parms ',' ELLIPSIS
		{ $$ = get_parm_info (0); }
	;

parms:
	   parm
		{ push_parm_decl ($1); }
	|  parms ',' parm
		{ push_parm_decl ($3); }
	;

/*
 *	A single parameter declaration or parameter type name,
 *	as found in a parmlist.
 */
parm:
	   typed_declspecs parm_declarator
		{ $$ = build_tree_list ($1, $2)	; }
	|  typed_declspecs notype_declarator
		{ $$ = build_tree_list ($1, $2)	; }
	|  typed_declspecs absdcl
		{ $$ = build_tree_list ($1, $2); }
	|  declmods notype_declarator
		{ $$ = build_tree_list ($1, $2)	; }
	|  declmods absdcl
		{ $$ = build_tree_list ($1, $2); }
	;

/*
 *	A nonempty list of identifiers.
 */
identifiers:
	   IDENTIFIER
		{ $$ = build_tree_list (NULL_TREE, $1); }
	|  identifiers ',' IDENTIFIER
		{ $$ = chainon ($1, build_tree_list (NULL_TREE, $3)); }
	;
%%
