/*
 ****************************************************************
 *								*
 *			gram.h					*
 *								*
 *   Data definitions for internal represen. of bison's input	*
 *								*
 *	Versão	3.0.0, de 06.07.93				*
 *		3.0.0, de 06.07.93				*
 *								*
 *	Módulo: GBISON						*
 *		Gerador de analisadores sintáticos		*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 		Baseado em software homônimo do GNU		*
 * 								*
 ****************************************************************
 */

/* representation of the grammar rules:

ntokens is the number of tokens, and nvars is the number of variables
(nonterminals).  nsyms is the total number, ntokens + nvars.

Each symbol (either token or variable) receives a symbol number.
Numbers 0 to ntokens-1 are for tokens, and ntokens to nsyms-1 are for
variables.  Symbol number zero is the end-of-input token.  This token
is counted in ntokens.

The rules receive rule numbers 1 to nrules in the order they are written.
Actions and guards are accessed via the rule number.

The rules themselves are described by three arrays: rrhs, rlhs and
ritem.  rlhs[R] is the symbol number of the left hand side of rule R.
The right hand side is stored as symbol numbers in a portion of
ritem.  rrhs[R] contains the index in ritem of the beginning of the
portion for rule R.

If rlhs[R] is -1, the rule has been thrown out by reduce.c
and should be ignored.

The length of the portion is one greater
 than the number of symbols in the rule's right hand side.
The last element in the portion contains minus R, which
identifies it as the end of a portion and says which rule it is for.

The portions of ritem come in order of increasing rule number and are
followed by an element which is zero to mark the end.  nitems is the
total length of ritem, not counting the final zero.  Each element of
ritem is called an "item" and its index in ritem is an item number.

Item numbers are used in the finite state machine to represent
places that parsing can get to.

Precedence levels are recorded in the vectors sprec and rprec.
sprec records the precedence level of each symbol,
rprec the precedence level of each rule.
rprecsym is the symbol-number of the symbol in %prec for this rule (if any).

Precedence levels are assigned in increasing order starting with 1 so
that numerically higher precedence values mean tighter binding as they
ought to.  Zero as a symbol or rule's precedence means none is
assigned.

Associativities are recorded similarly in rassoc and sassoc.  */


#define	ISTOKEN(s)	((s) < ntokens)
#define	ISVAR(s)	((s) >= ntokens)


extern int nitems;
extern int nrules;
extern int nsyms;
extern int ntokens;
extern int nvars;

extern short *ritem;
extern short *rlhs;
extern short *rrhs;
extern short *rprec;
extern short *rprecsym;
extern short *sprec;
extern short *rassoc;
extern short *sassoc;
extern short *rline;		/* Source line number of each rule */

extern int start_symbol;


/* associativity values in elements of rassoc, sassoc.  */

#define RIGHT_ASSOC 1
#define LEFT_ASSOC 2
#define NON_ASSOC 3

/* token translation table:
indexed by a token number as returned by the user's yylex routine,
it yields the internal token number used by the parser and throughout bison.
If translations is zero, the translation table is not used because
the two kinds of token numbers are the same.  */

extern short *token_translations;
extern int translations;
extern int max_user_token_number;

/* semantic_parser is nonzero if the input file says to use the hairy parser
that provides for semantic error recovery.  If it is zero, the yacc-compatible
simplified parser is used.  */

extern int semantic_parser;

/* pure_parser is nonzero if should generate a parser that is all pure and reentrant. */

extern int pure_parser;

/* error_token_number is the token number of the error token.  */

extern int error_token_number;
