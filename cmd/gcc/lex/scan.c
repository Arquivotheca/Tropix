/*
 ****************************************************************
 *								*
 *			scan.c					*
 *								*
 *	Analisador léxico					*
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
 ******	Inclusões de arquivos ***********************************
 */
#pragma	getenv (GEN)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include ../$GEN.gen/config.h
#include "../h/tree.h"
#include "../h/input.h"
#include "../h/c-parse.h"
#include "../h/c-tree.h"
#include "../src/c-parse.tab.h"
#include "../h/flags.h"

#include "../top/top.h"
#include "../tree/tree.h"

/*
 ******	Definição de macros *************************************
 */
#define	elif	else if
#define	EVER	;;
#define	NOSTR	(char *)NULL

#define isalnum(char)	((char >= 'a' && char <= 'z') ||	\
			(char >= 'A' && char <= 'Z') ||		\
			(char >= '0' && char <= '9'))

#define isdigit(char)	(char >= '0' && char <= '9')

#define ENDFILE -1	/* token that represents end-of-file */

/*
 ******	Variáveis globais ***************************************
 */
FILE		*finput;	/* input file */

static int	maxtoken;	/* Current nominal length of token buffer. */
static char	*token_buffer;	/* Pointer to token buffer.
				   Actual allocated length is maxtoken + 2. */
static int	max_wide;	/* Current nominal length of wide_buffer. */
static int	*wide_buffer;	/* Pointer to wide-string buffer.
				   Actual allocated length is max_wide + 1. */

extern tree	lastiddecl;

static int	end_of_file;	/* Nonzero if end-of-file has been seen on input. */

void yyerror ();

static int	nextchar = -1; /* Buffered character to reread.
				  I'm not sure why ungetc is not used.  */

/*
 *	Data type that represents the GNU C reserved words.
 */
struct resword	{ char *name; short token; enum rid rid; };

#define MIN_WORD_LENGTH     2      /* minimum size for C keyword */
#define MAX_WORD_LENGTH     13     /* maximum size for C keyword */
#define MIN_HASH_VALUE      7      /* range of the hash keys values  */
#define MAX_HASH_VALUE      91     /* for the perfect hash generator */
#define NORID		    RID_UNUSED

/*
 ****************************************************************
 *	Função de "hash" para identificadores			*
 ****************************************************************
 */

/*
 *	This function performs the minimum-perfect hash mapping from input
 *	string to reswords table index.  It only looks at the first and
 *	last characters in the string, thus assuring the O(1) lookup time
 *	(this keeps our constant down to an insignificant amount!).
 *	Compiling the following 2 functions as inline removes all
 *	overhead of the function calls.
 */

#ifdef __GNUC__
__inline
#endif
int
hash (register char *str, register int len)
{
/*
 *	This table is used to build the hash table index that recognizes
 *	reserved words in 0(1) steps.  It is larger than strictly necessary,
 *	but I'm trading off the space for the time-saving luxury of avoiding
 *	subtraction of an offset.  All those ``91's'' (actually just a
 *	short-hand for MAX_HASH_VALUE #defined above) are used to speed up
 *	the search when the string found on the input stream doesn't have a
 *	first or last character that is part of the set of alphabetic
 *	characters that comprise the first or last characters in C
 *	reserved words.
 */
	static int hash_table[] =
	{
	     91, 91, 91, 91, 91, 91, 91, 91, 91, 91,
	     91, 91, 91, 91, 91, 91, 91, 91, 91, 91,
	     91, 91, 91, 91, 91, 91, 91, 91, 91, 91,
	     91, 91, 91, 91, 91, 91, 91, 91, 91, 91,
	     91, 91, 91, 91, 91, 91, 91, 91, 91, 91,
	     91, 91, 91, 91, 91, 91, 91, 91, 91, 91,
	     91, 91, 91, 91, 91, 91, 91, 91, 91, 91,
	     91, 91, 91, 91, 91, 91, 91, 91, 91, 91,
	     91, 91, 91, 91, 91, 91, 91, 91, 91, 91,
	     91, 91, 91, 91, 91,  1, 91,  2,  1, 32,
	      7,  5, 18, 20,  1, 17, 91,  1, 18,  1,
	     28,  1, 23, 91, 12, 20,  1, 41,  7, 15,
	     91, 91, 10, 91, 91, 91, 91, 91
	};

	register int hval = len ;

	switch (hval)
	{
	    default:
	    case 3:
		hval += hash_table[str[2]];
	    case 2:
	    case 1:
		return hval + hash_table[str[0]] + hash_table[str[len - 1]];

	}	/* end switch */

}	/* end hash */

/*
 ****************************************************************
 *	Verifica se um identificador é uma palavra reservada	*
 ****************************************************************
 */
/*
 *	This routine attempts to match the string found in the reswords table
 *	with the one from the input stream.  If all the relevant details
 *	match then an actual strcmp comparison is performed and the address of
 *	correct struct resword entry is returned.  Otherwise, a NULL
 *	pointer is returned.
 */
#ifdef __GNUC__
__inline
#endif
struct resword *
is_reserved_word (register char *str, register int len)
{
	/*
	 *	This is the hash table of keywords.
	 *	The order of keywords has been chosen for perfect hashing.
	 *	Therefore, this table cannot be updated by hand.
	 *	Use the program ``gperf,'' available with the latest libg++
	 *	distribution, to generate an updated table.  A file called
	 *	c-parse.gperf, distributed with GNU C,
	 *	contains the keyword file.
	 */
	static struct resword reswords[] =
	{
		{ "" }, { "" }, { "" }, { "" }, { "" }, { "" }, { "" },
		{"asm",  ASM, NORID },
		{"auto",  SCSPEC, RID_AUTO },
		{"__asm",  ASM, NORID },
		{"do",  DO, NORID },
		{"__asm__",  ASM, NORID },
		{"break",  BREAK, NORID },
		{"__typeof__",  TYPEOF, NORID },
		{ "" },
		{"__alignof__",  ALIGNOF, NORID },
		{ "" },
		{"__attribute__",  ATTRIBUTE, NORID },
		{ "" },
		{"__attribute",  ATTRIBUTE, NORID },
		{ "" },
		{"__volatile__",  TYPE_QUAL, RID_VOLATILE },
		{"int",  TYPESPEC, RID_INT },
		{"__volatile",  TYPE_QUAL, RID_VOLATILE },
		{ "" },
		{"float",  TYPESPEC, RID_FLOAT },
		{"goto",  GOTO, NORID },
		{"short",  TYPESPEC, RID_SHORT },
		{"__typeof",  TYPEOF, NORID },
		{"__inline__",  SCSPEC, RID_INLINE },
		{"__alignof",  ALIGNOF, NORID },
		{"__inline",  SCSPEC, RID_INLINE },
		{"__signed__",  TYPESPEC, RID_SIGNED },
		{"default",  DEFAULT, NORID },
		{"else",  ELSE, NORID },
		{"void",  TYPESPEC, RID_VOID },
		{"__signed",  TYPESPEC, RID_SIGNED },
		{"if",  IF, NORID },
		{"volatile",  TYPE_QUAL, RID_VOLATILE },
		{"struct",  STRUCT, NORID },
		{"extern",  SCSPEC, RID_EXTERN },
		{"__const",  TYPE_QUAL, RID_CONST },
		{"while",  WHILE, NORID },
		{"__const__",  TYPE_QUAL, RID_CONST },
		{"switch",  SWITCH, NORID },
		{"for",  FOR, NORID },
		{"inline",  SCSPEC, RID_INLINE },
		{"return",  RETURN, NORID },
		{"typeof",  TYPEOF, NORID },
		{"typedef",  SCSPEC, RID_TYPEDEF },
		{"char",  TYPESPEC, RID_CHAR },
		{"enum",  ENUM, NORID },
		{"register",  SCSPEC, RID_REGISTER },
		{"signed",  TYPESPEC, RID_SIGNED },
		{"sizeof",  SIZEOF, NORID },
		{ "" }, { "" }, { "" }, { "" },
		{"double",  TYPESPEC, RID_DOUBLE },
		{"static",  SCSPEC, RID_STATIC },
		{"case",  CASE, NORID },
		{ "" }, { "" }, { "" }, { "" },
		{"const",  TYPE_QUAL, RID_CONST },
		{ "" }, { "" }, { "" },
		{"long",  TYPESPEC, RID_LONG },
		{ "" }, { "" },
		{"continue",  CONTINUE, NORID },
		{ "" }, { "" },
		{"unsigned",  TYPESPEC, RID_UNSIGNED },
		{ "" }, { "" }, { "" }, { "" }, { "" },
		{ "" }, { "" }, { "" }, { "" },
		{ "" }, { "" }, { "" }, { "" }, { "" },
		{"union",  UNION, NORID }
	};

#define	ENTRY
#ifdef	ENTRY
	static struct resword entry_resword =
		{"entry",  SCSPEC, RID_ENTRY };
#endif	ENTRY

	if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
	{
		register int key = hash (str, len);

		if (key <= MAX_HASH_VALUE)
		{
			register char *s = reswords[key].name;

			if (*s == *str && !strcmp (str + 1, s + 1))
				return (&reswords[key]);
		}

#ifdef	ENTRY
		/*
		 *	PROVISÓRIO: Processa "entry"
		 */
		if (str[0] == 'e' && !strcmp (str, "entry") && flag_tropix)
			return (&entry_resword);
#endif	ENTRY

	}

	return ((struct resword *)NULL);

}	/* end is_reserved_word */

/*
 *	The elements of `ridpointers' are identifier nodes
 *	for the reserved type names and storage classes.
 *	It is indexed by a RID_... value.
 */
tree		ridpointers[RID_MAX];

int		check_newline ();

/*
 ****************************************************************
 *	Inicializa a análise léxica				*
 ****************************************************************
 */
void
init_lex (void)
{
	/*
	 *	Start it at 0, because check_newline is called at
	 *	the very beginning and will increment it to 1.
	 */
	lineno = 0;

	maxtoken = 40;
	token_buffer = (char *)xmalloc (maxtoken + 2);
	max_wide = 40;
	wide_buffer = (int *)xmalloc ((max_wide + 1) * UNITS_PER_WORD);

	ridpointers[RID_INT] = get_identifier ("int");
	ridpointers[RID_CHAR] = get_identifier ("char");
	ridpointers[RID_VOID] = get_identifier ("void");
	ridpointers[RID_FLOAT] = get_identifier ("float");
	ridpointers[RID_DOUBLE] = get_identifier ("double");
	ridpointers[RID_SHORT] = get_identifier ("short");
	ridpointers[RID_LONG] = get_identifier ("long");
	ridpointers[RID_UNSIGNED] = get_identifier ("unsigned");
	ridpointers[RID_SIGNED] = get_identifier ("signed");
	ridpointers[RID_INLINE] = get_identifier ("inline");
	ridpointers[RID_CONST] = get_identifier ("const");
	ridpointers[RID_VOLATILE] = get_identifier ("volatile");
	ridpointers[RID_AUTO] = get_identifier ("auto");
	ridpointers[RID_STATIC] = get_identifier ("static");
	ridpointers[RID_EXTERN] = get_identifier ("extern");
	ridpointers[RID_TYPEDEF] = get_identifier ("typedef");
	ridpointers[RID_REGISTER] = get_identifier ("register");
#ifdef	ENTRY
	ridpointers[RID_ENTRY] = get_identifier ("entry");
#endif	ENTRY

}	/* end init_lex */

/*
 ****************************************************************
 *	Reinicializa a análise léxica				*
 ****************************************************************
 */
void
reinit_parse_for_function ()
{

}	/* end reinit_parse_for_function */

/*
 ****************************************************************
 *	Pula caracteres brancos					*
 ****************************************************************
 */
/*
 *	If C is not whitespace, return C.
 *	Otherwise skip whitespace and return first nonwhite char read.
 */
int
skip_white_space (register int c)
{
	register int inside;

	for (EVER)
	{
		switch (c)
		{
		    case '/':
			c = getc (finput);

			if (c != '*')
			{
				ungetc (c, finput);
				return '/';
			}

			c = getc (finput);

			inside = 1;

			while (inside)
			{
				if (c == '*')
				{
					while (c == '*')
						c = getc (finput);

					if (c == '/')
					{
						inside = 0;
						c = getc (finput);
					}
				}
				elif (c == '\n')
				{
					lineno++;
					c = getc (finput);
				}
				elif (c == EOF)
				{
					error ("unterminated comment");
					break;
				}
				else
				{
					c = getc (finput);
				}
			}
			break;

		    case '\n':
			c = check_newline ();
			break;

		    case ' ':
		    case '\t':
		    case '\f':
		    case '\r':
		    case '\v':
		    case '\b':
			c = getc (finput);
			break;

		    case '\\':
			c = getc (finput);

			if (c == '\n')
				lineno++;
			else
				error ("stray '\\' in program");

			c = getc (finput);
			break;

		    default:
			return (c);

		}	/* end switch */

	}	/* end for (EVER) */

}	/* end skip_white_space */

/*
 ****************************************************************
 *	Estende a área de entrada				*
 ****************************************************************
 */
/*
 *	Make the token buffer longer, preserving the data in it.
 *	P should point to just beyond the last valid character
 *	in the old buffer. The value we return is a pointer to
 *	the new buffer at a place corresponding to P.
 */
char *
extend_token_buffer (char *p)
{
	int offset = p - token_buffer;

	maxtoken = maxtoken * 2 + 10;
	token_buffer = (char *)xrealloc (token_buffer, maxtoken + 2);

	return (token_buffer + offset);

}	/* end extend_token_buffer */

/*
 ****************************************************************
 *	Processa o começo de uma linha				*
 ****************************************************************
 */

/*
 *	At the beginning of a line, increment the line number
 *	and process any #-directive on this line.
 *	If the line is a #-directive, read the entire line and
 *	return a newline. Otherwise, return the line's first
 *	non-whitespace character.
 */
int
check_newline (void)
{
	register int	c;
	register int	token;

	lineno++;

	/*
	 *	Read first nonwhite char on the line.
	 */
	c = getc (finput);

	while (c == ' ' || c == '\t')
		c = getc (finput);

	/*
	 *	If not #, return it so caller will use it.
	 */
	if (c != '#')
		return c;

	/*
	 *	Read first nonwhite char after the `#'.
	 */
	c = getc (finput);

	while (c == ' ' || c == '\t')
		c = getc (finput);

	/*
	 *	If a letter follows, then if the word here is `line', skip
 	 *	it and ignore it; otherwise, ignore the line, with an error
 	 *	if the word isn't `pragma'.
	 */
	if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
	{
		if (c == 'p')
		{
			if
			(	getc (finput) == 'r' && getc (finput) == 'a' &&
				getc (finput) == 'g' && getc (finput) == 'm' &&
				getc (finput) == 'a' && ((c = getc (finput)) ==
						' ' || c == '\t' || c == '\n')
			)
				goto skipline;
		}
		elif (c == 'l')
		{
			if
			(	getc (finput) == 'i' && getc (finput) == 'n' &&
				getc (finput) == 'e' && ((c = getc (finput)) ==
							' ' || c == '\t')
			)
				goto linenum;
		}
		elif (c == 'i')
		{
			if
			(	getc (finput) == 'd' && getc (finput) == 'e' &&
				getc (finput) == 'n' && getc (finput) == 't' &&
				((c = getc (finput)) == ' ' || c == '\t')
			)
			{
				extern FILE *asm_out_file;

				if (pedantic)
					error ("ANSI C does not allow #ident");

				/*
				 *	Here we have just seen `#ident '.
				 *	A string constant should follow.
				 */
				while (c == ' ' || c == '\t')
					c = getc (finput);

				/*
				 *	If no argument, ignore the line.
				 */
				if (c == '\n')
					return (c);

				ungetc (c, finput);
				token = yylex ();

				if (nextchar >= 0)
					ungetc (nextchar, finput), nextchar = -1;

				if
				(	token != STRING ||
					TREE_CODE (yylval.ttype) != STRING_CST
				)
				{
					error ("invalid #ident");
					goto skipline;
				}

#ifdef ASM_OUTPUT_IDENT
	ASM_OUTPUT_IDENT (asm_out_file, TREE_STRING_POINTER (yylval.ttype));
#endif ASM_OUTPUT_IDENT

				/*
				 *	Skip the rest of this line.
				 */
				goto skipline;
			}
		}

		error ("undefined or invalid # directive");
		goto skipline;
	}

	/*
	 *	Here we have either `#line' or `# <nonletter>'.
 	 *	In either case, it should be a line number;
	 *	a digit should follow.
	 */
linenum:
	while (c == ' ' || c == '\t')
		c = getc (finput);

	/*
	 *	If the # is the only nonwhite char on the line,
	 *	just ignore it. Check the new newline.
	 */
	if (c == '\n')
		return c;

	/*
	 *	Something follows the #; read a token.
	 */
	ungetc (c, finput);
	token = yylex ();

	if (nextchar >= 0)
		ungetc (nextchar, finput), nextchar = -1;

	if (token == CONSTANT && TREE_CODE (yylval.ttype) == INTEGER_CST)
	{
		int old_lineno = lineno;

		/*
		 *	subtract one, because it is the following line that
		 *	gets the specified number
		 */
		int l = TREE_INT_CST_LOW (yylval.ttype) - 1;

		/*
		 *	Is this the last nonwhite stuff on the line?
		 */
		c = getc (finput);

		while (c == ' ' || c == '\t')
			c = getc (finput);

		/*
		 *	No more: store the line number and check following line.
		 */
		if (c == '\n')
		{
			lineno = l;
			return c;
		}

		ungetc (c, finput);

		/*
		 *	More follows: it must be a string constant (filename).
		 */
		token = yylex ();

		if (nextchar >= 0)
			ungetc (nextchar, finput), nextchar = -1;

		if (token != STRING || TREE_CODE (yylval.ttype) != STRING_CST)
		{
			error ("invalid #line");
			goto skipline;
		}

		input_filename = (char *)permalloc
					(TREE_STRING_LENGTH (yylval.ttype) + 1);
		strcpy (input_filename, TREE_STRING_POINTER (yylval.ttype));
		lineno = l;

		if (main_input_filename == NOSTR)
			main_input_filename = input_filename;

		/*
		 *	Is this the last nonwhite stuff on the line?
		 */
		c = getc (finput);

		while (c == ' ' || c == '\t')
			c = getc (finput);

		if (c == '\n')
			return c;

		ungetc (c, finput);

		token = yylex ();

		if (nextchar >= 0)
			ungetc (nextchar, finput), nextchar = -1;

		/*
		 *	`1' after file name means entering new file.
		 *	`2' after file name means just left a file.
		 */
		if (token == CONSTANT && TREE_CODE (yylval.ttype) == INTEGER_CST)
		{
			if (TREE_INT_CST_LOW (yylval.ttype) == 1)
			{
				struct file_stack *p = (struct file_stack *)
					xmalloc (sizeof (struct file_stack));

				input_file_stack->line = old_lineno;
				p->next = input_file_stack;
				p->name = input_filename;
				input_file_stack = p;
				input_file_stack_tick++;
			}
			elif (input_file_stack->next)
			{
				struct file_stack *p = input_file_stack;

				input_file_stack = p->next;
				free (p);
				input_file_stack_tick++;
			}
			else
			{
				error ("#-lines for entering and leaving files don't match");
			}
		}
	}
	else
	{
		error ("invalid #-line");
	}

	/*
	 *	skip the rest of this line.
	 */
skipline:
	if (c == '\n')
		return c;

	while ((c = getc (finput)) != EOF && c != '\n')
		/* vazio */;

	return c;

}	/* end check_newline */

/*
 ****************************************************************
 *	Processa os "escapes"					*
 ****************************************************************
 */
int
readescape (void)
{
	register int	c = getc (finput);
	register int	count, code;
	int		firstdig;

	switch (c)
	{
	    case 'x':
		code = 0;
		count = 0;

		for (EVER)
		{
			c = getc (finput);

			if
			(	!(c >= 'a' && c <= 'f') &&
				!(c >= 'A' && c <= 'F') &&
				!(c >= '0' && c <= '9')
			)
			{
				  ungetc (c, finput);
				  break;
			}

			code *= 16;

			if (c >= 'a' && c <= 'f')
				code += c - 'a' + 10;

			if (c >= 'A' && c <= 'F')
				code += c - 'A' + 10;

			if (c >= '0' && c <= '9')
				code += c - '0';

			if (count == 0)
				  firstdig = code;
			count++;
		}

		if   (count == 0)
		{
			error ("\\x used with no following hex digits");
		}
		elif
		(	(count - 1) * 4 >= TYPE_PRECISION (integer_type_node) ||
		   (count > 1 &&
		   ((1 << (TYPE_PRECISION (integer_type_node) - (count - 1) * 4))
					   <= firstdig))
		)
		{
			warning ("hex escape out of range");
		}

		return code;

	    case '0':  case '1':  case '2':  case '3':  case '4':
	    case '5':  case '6':  case '7':
		code = 0;
		count = 0;

		while ((c <= '7') && (c >= '0') && (count++ < 3))
		{
			code = (code * 8) + (c - '0');
			c = getc (finput);
		}

		ungetc (c, finput);
		return code;

	    case '\\': case '\'': case '"':
		return c;

	    case '\n':
		lineno++;
		return -1;

	    case 'n':
		return TARGET_NEWLINE;

	    case 't':
		return TARGET_TAB;

	    case 'r':
		return TARGET_CR;

	    case 'f':
		return TARGET_FF;

	    case 'b':
		return TARGET_BS;

	    case 'a':
		return TARGET_BELL;

	    case 'v':
		return TARGET_VT;

	    case 'e':
		if (flag_tropix)
			return 033;
		else
			break;

	    case 'E':
		return 033;

		/*
		 *	`\(', etc, are used at beginning of line
		 *	to avoid confusing Emacs.
		 */
	    case '?':
	    case '(':
	    case '{':
	    case '[':
		return c;
	}

	if (c >= 040 && c <= 0177)
	{
		warning ("unknown escape sequence `\\%c'", c);
	}
	else
	{
		warning
		(	"unknown escape sequence: `\\' followed by "
			"char code 0x%x", c
		);
	}

	return c;

}	/* end readescape */

/*
 ****************************************************************
 *	Imprime uma mensagem de erro				*
 ****************************************************************
 */
void
yyerror (const char *string)
{
	char buf[200];

	strcpy (buf, string);

	/*
	 *	We can't print string and character constants well
	 *	because the token_buffer contains the result of
	 *	processing escapes.
	 */
	if (end_of_file)
		strcat (buf, " at end of input");
	elif (token_buffer[0] == 0)
		strcat (buf, " at null character");
	elif (token_buffer[0] == '"')
		strcat (buf, " before string constant");
	elif (token_buffer[0] == '\'')
		strcat (buf, " before character constant");
	elif (token_buffer[0] < 040 || token_buffer[0] >= 0177)
		sprintf (buf + strlen (buf), " before character 0%o",
							token_buffer[0]);
	else
		strcat (buf, " before `%s'");

	error (buf, token_buffer);

}	/* end yyerror */

/*
 ****************************************************************
 *	Função de análise léxica				*
 ****************************************************************
 */
int
yylex (void)
{
	register int	c;
	register char	*p;
	register int	value;
	int		wide_flag = 0;

	if (nextchar >= 0)
		c = nextchar, nextchar = -1;
	else
		c = getc (finput);

	/*
	 *	Effectively do c = skip_white_space (c)
	 *	but do it faster in the usual cases.
	 */
	for (EVER) switch (c)
	{
	    case ' ':
	    case '\t':
	    case '\f':
	    case '\r':
	    case '\v':
	    case '\b':
		c = getc (finput);
		break;

	    case '\n':
	    case '/':
	    case '\\':
		c = skip_white_space (c);

	default:
		goto found_nonwhite;
	}

    found_nonwhite:
	token_buffer[0] = c;
	token_buffer[1] = 0;

	/*
	 *	yylloc.first_line = lineno;
	 */
	switch (c)
	{
	    case EOF:
		end_of_file = 1;
		token_buffer[0] = 0;
		value = ENDFILE;
		break;

	    case '$':
		if (dollars_in_ident)
			goto letter;
		return '$';

		/*
		 *	Capital L may start a wide-string or
		 *	wide-character constant.
		 */
	    case 'L':
	    {
		register int c = getc (finput);

		if (c == '\'')
			{ wide_flag = 1; goto char_constant; }

		if (c == '"')
			{ wide_flag = 1; goto string_constant; }

		ungetc (c, finput);
	    }

	    case 'A':  case 'B':  case 'C':  case 'D':  case 'E':
	    case 'F':  case 'G':  case 'H':  case 'I':  case 'J':
	    case 'K':		  case 'M':  case 'N':  case 'O':
	    case 'P':  case 'Q':  case 'R':  case 'S':  case 'T':
	    case 'U':  case 'V':  case 'W':  case 'X':  case 'Y':
	    case 'Z':
	    case 'a':  case 'b':  case 'c':  case 'd':  case 'e':
	    case 'f':  case 'g':  case 'h':  case 'i':  case 'j':
	    case 'k':  case 'l':  case 'm':  case 'n':  case 'o':
	    case 'p':  case 'q':  case 'r':  case 's':  case 't':
	    case 'u':  case 'v':  case 'w':  case 'x':  case 'y':
	    case 'z':
	    case '_':
	    letter:
		p = token_buffer;

		while (isalnum (c) || c == '_' || c == '$')
		{
			if (p >= token_buffer + maxtoken)
			p = extend_token_buffer (p);

			if (c == '$' && ! dollars_in_ident)
				break;

			*p++ = c;
			c = getc (finput);
		}

		*p = 0;
		nextchar = c;

		value = IDENTIFIER;
		yylval.itype = 0;

		/*
		 *	Try to recognize a keyword.
		 *	Uses minimum-perfect hash function
		 */
	    {
		register struct resword *ptr;

		if (ptr = is_reserved_word (token_buffer, p - token_buffer))
		{
			if (ptr->rid)
				yylval.ttype = ridpointers[ptr->rid];

			/*
			 *	-fno-asm means don't recognize
			 *	the non-ANSI keywords.
			 */
			if
			(	(!flag_no_asm || (ptr->token != ASM &&
				ptr->token != TYPEOF &&
				 ptr->rid != RID_INLINE)
			 /* Recognize __asm and __inline despite -fno-asm.  */
				 || token_buffer[0] == '_')
				/*
				 *	-ftraditional means don't recognize
				 *	nontraditional keywords typeof, const,
				 *	volatile, signed or inline.
				 */
				&& (! flag_traditional
				|| (ptr->token != TYPE_QUAL
				&& ptr->token != TYPEOF
				&& ptr->rid != RID_SIGNED
				&& ptr->rid != RID_INLINE)
				/*
				 *	Recognize __inline, etc.
				 *	despite -ftraditional.
				 */
				|| token_buffer[0] == '_')
			)
			{
				value = ptr->token;
				/*
				 *	Even if we decided to recognize asm,
				 *	still perhaps warn.
				 */
				if
				(	pedantic && (value == ASM ||
						value == TYPEOF ||
					ptr->rid == RID_INLINE) &&
					token_buffer[0] != '_'
				)
				{
					warning
					(	"ANSI does not permit "
						"the keyword `%s'",
						 token_buffer
					);
				}
			}
		}
	    }

		/*
		 *	If we did not find a keyword, look for an identifier
		 *	(or a typename).
		 */
		if (value == IDENTIFIER)
		{
			yylval.ttype = get_identifier (token_buffer);
			lastiddecl = lookup_name (yylval.ttype);

			if
			(	lastiddecl != NULL_TREE &&
					TREE_CODE (lastiddecl) == TYPE_DECL
			)
				value = TYPENAME;
		}
		break;

	    case '0':  case '1':  case '2':  case '3':  case '4':
	    case '5':  case '6':  case '7':  case '8':  case '9':
	    case '.':
	    {
		int		base = 10;
		int		count = 0;
		int		largest_digit = 0;
		int		numdigits = 0;

		/*
		 *	for multi-precision arithmetic,
	 	 *	we store only 8 live bits in each short,
	 	 *	giving us 64 bits of reliable precision
		 */
		short		shorts[8];
		int		overflow = 0;

		enum anon1 { NOT_FLOAT, AFTER_POINT, TOO_MANY_POINTS }
				floatflag = NOT_FLOAT;

		for (count = 0; count < 8; count++)
			shorts[count] = 0;

		p = token_buffer; 	*p++ = c;

		if (c == '0')
		{
			*p++ = (c = getc (finput));

			if ((c == 'x') || (c == 'X'))
			{
				base = 16;
				*p++ = (c = getc (finput));
			}
			else
			{
				base = 8;
				numdigits++;
			}
		}

		/*
		 *	Read all the digits-and-decimal-points.
		 */
		while
		(	c == '.'  || (isalnum (c) && (c != 'l') && (c != 'L')
			 && (c != 'u') && (c != 'U')
			 && (floatflag == NOT_FLOAT ||
						((c != 'f') && (c != 'F'))))
		)
		{
			if (c == '.')
			{
				if (base == 16)
				{
					error
					(	"floating constant may not "
						"be in radix 16"
					);
				}

				if (floatflag == AFTER_POINT)
				{
					error ("malformed floating constant");
					floatflag = TOO_MANY_POINTS;
				}
				else
				{
					floatflag = AFTER_POINT;
				}

				base = 10;
				*p++ = c = getc (finput);

				/*
				 *	Accept '.' as the start of a floating-
				 *	point number only when it is followed
				 *	by a digit. Otherwise, unread the
				 *	following non-digit and use the '.'
				 *	as a structural token.
				 */
				if (p == token_buffer + 2 && !isdigit (c))
				{
					if (c == '.')
					{
						c = getc (finput);

						if (c == '.')
						{
							*p++ = c;
							*p = 0;
							return ELLIPSIS;
						}
						error ("parse error at `..'");
					}

					ungetc (c, finput);
					token_buffer[1] = 0;
					value = '.';
					goto done;
				}
			}
			else
			{
			/*
			 *	It is not a decimal point.
		 	 *	It should be a digit (perhaps a hex digit).
			 */
				if (isdigit (c))
				{
					c = c - '0';
				}
				elif (base <= 10)
				{
					if ((c&~040) == 'E')
					{
						base = 10;
						floatflag = AFTER_POINT;
						break; /* start of exponent */
					}
						error
						(	"nondigits in number "
							"and not hexadecimal"
						);

					c = 0;
				}
				elif (c >= 'a')
				{
					c = c - 'a' + 10;
				}
				else
				{
					c = c - 'A' + 10;
				}

				if (c >= largest_digit)
					largest_digit = c;

				numdigits++;

				for (count = 0; count < 8; count++)
				{
					shorts[count] *= base;

					if (count)
					{
						shorts[count] +=
							(shorts[count-1] >> 8);
						shorts[count-1] &= (1<<8)-1;
					}
					else
					{
						shorts[0] += c;
					}
				}

				if
				(	shorts[7] >= 1<<8
					|| shorts[7] < - (1 << 8)
				)
					overflow = TRUE;

				if (p >= token_buffer + maxtoken - 3)
					p = extend_token_buffer (p);

				*p++ = (c = getc (finput));
			}
		}

		if (numdigits == 0)
			error ("numeric constant with no digits");

		if (largest_digit >= base)
		{
			error
			(	"numeric constant contains digits "
				"beyond the radix"
			);
		}

		/*
		 *	Remove terminating char from the token
		 *	buffer and delimit the string
		 */
		*--p = 0;

		if (floatflag != NOT_FLOAT)
		{
			tree		type = double_type_node;
			char		f_seen = 0;
			char		l_seen = 0;
			REAL_VALUE_TYPE	value;

			/*
			 *	Read explicit exponent if any,
			 *	and put it in tokenbuf.
			 */
			if ((c == 'e') || (c == 'E'))
			{
				if (p >= token_buffer + maxtoken - 3)
					p = extend_token_buffer (p);

				*p++ = c;
				c = getc (finput);

				if ((c == '+') || (c == '-'))
				{
					*p++ = c;
					c = getc (finput);
				}

				if (! isdigit (c))
				{
					error
					(	"floating constant exponent "
						"has no digits"
					);
				}

				while (isdigit (c))
				{
					if (p >= token_buffer + maxtoken - 3)
						p = extend_token_buffer (p);
					*p++ = c;
					c = getc (finput);
				}
			}

			*p = 0;
			errno = 0;
			value = REAL_VALUE_ATOF (token_buffer);
#ifdef ERANGE
			if (errno == ERANGE && !flag_traditional)
			{
				char	*p1 = token_buffer;

				/*
				 *	Check for "0.0" and variants;
				 *	Sunos 4 spuriously returns
				 *	ERANGE for them.
				 */
				while (*p1 == '0')
					p1++;

				if (*p1 == '.')
				{
					p1++;
					while (*p1 == '0')
						p1++;
				}

				if (*p1 == 'e' || *p1 == 'E')
				{
					/*
					 *	with significand==0,
					 *	ignore the exponent
					 */
					p1++;
					while (*p1 != 0)
						p1++;
				}

				/*
				 *	ERANGE is also reported for underflow,
		 		 *	so test the value to distinguish
				 *	overflow from that.
				 */
				if (*p1 != 0 && (value > 1.0 || value < -1.0))
				{
					warning
					(	"floating point number exceeds "
						"range of `double'"
					);
				}
			}
#endif

			/*
			 *	Read the suffixes to choose a data type.
			 */
			for (EVER)
			{
				if (c == 'f' || c == 'F')
				{
					float floater;

					if (f_seen)
					{
						error
						(	"two `f's in floating "
							"constant"
						);
					}

					f_seen = 1;
					type = float_type_node;
					floater = value;
					value = floater;
				}
				elif (c == 'l' || c == 'L')
				{
					if (l_seen)
					{
						error
						(	"two `l's in floating "
							"constant"
						);
					}
					l_seen = 1;
					type = long_double_type_node;
				}
				else
				{
				    if (isalnum (c))
				    {
					error ("garbage at end of number");

					while (isalnum (c))
					{
					    if (p >= token_buffer + maxtoken - 3)
							p = extend_token_buffer (p);
					    *p++ = c;
					    c = getc (finput);
					}
				    }
				    break;
				}

				if (p >= token_buffer + maxtoken - 3)
					p = extend_token_buffer (p);

				*p++ = c;
				c = getc (finput);
			}

			/*
			 *	Create a node with determined type and value.
			 */
			yylval.ttype = build_real (type, value);

			ungetc (c, finput);
			*p = 0;
		}
		else
		{
			tree	type;
			int	spec_unsigned = 0;
			int	spec_long = 0;
			int	spec_long_long = 0;

			for (EVER)
			{
				if (c == 'u' || c == 'U')
				{
					if (spec_unsigned)
					{
						error
						(	"two `u's in integer "
							"constant"
						);
					}

					spec_unsigned = 1;
				}
				elif (c == 'l' || c == 'L')
				{
					if (spec_long)
					{
						if   (spec_long_long)
						{
							error
							(	"three `l's in "
								"integer constant"
							);
						}
						elif (pedantic)
						{
							warning
							(	"ANSI C forbids "
								"long long "
								"integer constants"
							);
						}

						spec_long_long = 1;
					}
					spec_long = 1;
				}
				else
				{
				    if (isalnum (c))
				    {
					error ("garbage at end of number");

					while (isalnum (c))
					{
						if (p >= token_buffer + maxtoken - 3)
							p = extend_token_buffer (p);
						*p++ = c;
						c = getc (finput);
					}
				    }
				    break;
				}

				if (p >= token_buffer + maxtoken - 3)
					p = extend_token_buffer (p);

				*p++ = c;
				c = getc (finput);
			}

			ungetc (c, finput);

			if
			(	(overflow || shorts[7] || shorts[6] ||
				shorts[5] || shorts[4])
				&& !spec_long_long
			)
				warning ("integer constant out of range");

			/*
			 * 	If it won't fit in a signed long long,
			 *	make it unsigned. We can't distinguish based
			 *	on the tree node because any integer constant
			 *	fits any long long type.
			 */
			if (shorts[7] >= (1<<8))
				spec_unsigned = 1;

			/*
			 *	This is simplified by the fact that our
			 *	constant is always positive.
			 *
			 *	The casts in the following statement should
			 *	not be needed, but they get around bugs in
			 *	some C compilers.
			 */
			yylval.ttype =
			(	build_int_2
				 ((((long)shorts[3]<<24) + ((long)shorts[2]<<16)
				 + ((long)shorts[1]<<8) + (long)shorts[0]),
				(spec_long_long
				? (((long)shorts[7]<<24) + ((long)shorts[6]<<16)
				+ ((long)shorts[5]<<8) + (long)shorts[4])
				 : 0))
			);

			if
			(	!spec_long && !spec_unsigned && int_fits_type_p
					(yylval.ttype, integer_type_node)
			)
			{
				type = integer_type_node;
			}
			elif
			(	!spec_long && (base != 10 || spec_unsigned) &&
				int_fits_type_p 
					(yylval.ttype, unsigned_type_node)
			)
			{
				type = unsigned_type_node;
			}
			elif
			(	!spec_unsigned && !spec_long_long &&
				int_fits_type_p
					(yylval.ttype, long_integer_type_node)
			)
			{
				type = long_integer_type_node;
			}
			elif
			(	!spec_long_long && int_fits_type_p
					(yylval.ttype, long_unsigned_type_node)
			)
			{
				type = long_unsigned_type_node;
			}
			elif
			(	!spec_unsigned && int_fits_type_p
				(yylval.ttype, long_long_integer_type_node)
			)
			{
				type = long_long_integer_type_node;
			}
			elif
			(	int_fits_type_p (yylval.ttype,
					long_long_unsigned_type_node)
			)
			{
				type = long_long_unsigned_type_node;
			}
			else
			{
				type = long_long_integer_type_node;
				warning ("integer constant out of range");
			}

			TREE_TYPE (yylval.ttype) = type;
			*p = 0;
		}

		value = CONSTANT; break;
	    }

	    case '\'':
	char_constant:
	    {
		register int	result = 0;
		register	num_chars = 0;
		int		width = TYPE_PRECISION (char_type_node);
		int		max_chars;

		if (wide_flag)
			width = TYPE_PRECISION (integer_type_node);

		max_chars = TYPE_PRECISION (integer_type_node) / width;

		for (EVER)
		{
		    tryagain:
			c = getc (finput);

			if (c == '\'' || c == EOF)
				break;

			if (c == '\\')
			{
				c = readescape ();

				if (c < 0)
					goto tryagain;

				if
				(	width < HOST_BITS_PER_INT &&
					(unsigned) c >= (1 << width)
				)
				{
					warning
					(	"escape sequence out of range "
						"for character"
					);
				}
			}
			elif (c == '\n')
			{
				if (pedantic)
				{
					warning
					(	"ANSI C forbids newline in "
						"character constant"
					);
				}

				lineno++;
			}

			num_chars++;

			if (num_chars > maxtoken - 4)
				extend_token_buffer (token_buffer);

			token_buffer[num_chars] = c;

			/*
			 *	Merge character into result;
			 *	ignore excess chars.
			 */
			if (num_chars < max_chars + 1)
			{
				if (width < HOST_BITS_PER_INT)
					result = (result << width) |
						(c & ((1 << width) - 1));
				else
					result = c;
			}
		}

		token_buffer[num_chars + 1] = '\'';
		token_buffer[num_chars + 2] = 0;

		if   (c != '\'')
		{
			error ("malformatted character constant");
		}
		elif (num_chars == 0)
		{
			error ("empty character constant");
		}
		elif (num_chars > max_chars)
		{
			num_chars = max_chars;
			error ("character constant too long");
		}
		elif (num_chars != 1 && !flag_traditional && extra_warnings)
		{
			warning ("multi-character character constant");
		}

		/*
		 *	If char type is signed, sign-extend the constant.
		 */
		if (!wide_flag)
		{
			int num_bits = num_chars * width;

			if
			(	TREE_UNSIGNED (char_type_node) ||
				((result >> (num_bits - 1)) & 1) == 0
			)
			{
				yylval.ttype = build_int_2
				(	result & ((unsigned) ~0
					 >> (HOST_BITS_PER_INT - num_bits)),
					 0
				);
			}
			else
			{
				yylval.ttype = build_int_2
				(	result | ~((unsigned) ~0
					>> (HOST_BITS_PER_INT - num_bits)),
					-1
				);
			}
		}
		else
		{
			yylval.ttype = build_int_2 (result, 0);
		}

		TREE_TYPE (yylval.ttype) = integer_type_node;
		value = CONSTANT; break;
	    }

	    case '"':
	    string_constant:
	    {
		int	*widep;

		c = getc (finput);
		p = token_buffer + 1;

		if (wide_flag)
			widep = wide_buffer;

		while (c != '"' && c >= 0)
		{
			if (c == '\\')
			{
				c = readescape ();

				if (c < 0)
					goto skipnewline;

				if
				(   !wide_flag &&
				    c >= (1 << TYPE_PRECISION (char_type_node))
				)
				{
					warning
					(	"escape sequence out of "
						"range for character"
					);
				}
			}
			elif (c == '\n')
			{
				if (pedantic)
				{
					warning
					(	"ANSI C forbids newline "
						"in string constant"
					);
				}

				lineno++;
			}

			/*
			 *	Store the char in C into the appropriate buffer
			 */
			if (wide_flag)
			{
				if (widep == wide_buffer + max_wide)
				{
					int	n = widep - wide_buffer;

					max_wide *= 2;

					wide_buffer = (int *)xrealloc
						(wide_buffer,
						(max_wide + 1) * UNITS_PER_WORD);

					widep = wide_buffer + n;
				}

				*widep++ = c;
			}
			else
			{
				if (p == token_buffer + maxtoken)
				p = extend_token_buffer (p);
				*p++ = c;
			}

			skipnewline:
			c = getc (finput);
		}

		/*
		 *	We have read the entire constant.
		 *	Construct a STRING_CST for the result.
		 */
		if (wide_flag)
		{
			/*
			 *	If this is a L"..." wide-string, make a vector
			 *	of the ints in wide_buffer.
			 */
			*widep = 0;

			/*
			 *	We have not implemented the case where `int'
			 *	on the target and on the execution machine
			 *	differ in size.
			 */
			if (TYPE_PRECISION (integer_type_node)
					!= sizeof (int) * BITS_PER_UNIT)
				abort ();

			yylval.ttype = build_string
				((widep - wide_buffer + 1) * sizeof (int),
				wide_buffer);

			TREE_TYPE (yylval.ttype) = int_array_type_node;
		}
		else
		{
			*p = 0;

			yylval.ttype = build_string
					(p - token_buffer, token_buffer + 1);

			TREE_TYPE (yylval.ttype) = char_array_type_node;
		}

		*p++ = '"';
		*p = 0;

		value = STRING;
		break;
	    }

	    case '+':
	    case '-':
	    case '&':
	    case '|':
	    case '<':
	    case '>':
	    case '*':
	    case '/':
	    case '%':
	    case '^':
	    case '!':
	    case '=':
	    {
		register int c1;

	    combine:
		switch (c)
		{
		    case '+':
			yylval.code = PLUS_EXPR; break;
		    case '-':
			yylval.code = MINUS_EXPR; break;
		    case '&':
			yylval.code = BIT_AND_EXPR; break;
		    case '|':
			yylval.code = BIT_IOR_EXPR; break;
		    case '*':
			yylval.code = MULT_EXPR; break;
		    case '/':
			yylval.code = TRUNC_DIV_EXPR; break;
		    case '%':
			yylval.code = TRUNC_MOD_EXPR; break;
		    case '^':
			yylval.code = BIT_XOR_EXPR; break;
		    case LSHIFT:
			yylval.code = LSHIFT_EXPR; break;
		    case RSHIFT:
			yylval.code = RSHIFT_EXPR; break;
		    case '<':
			yylval.code = LT_EXPR; break;
		    case '>':
			yylval.code = GT_EXPR; break;
		}

		token_buffer[1] = c1 = getc (finput);
		token_buffer[2] = 0;

		if (c1 == '=')
		{
			switch (c)
			{
			    case '<':
				value = ARITHCOMPARE;
				yylval.code = LE_EXPR;
				goto done;
			    case '>':
				value = ARITHCOMPARE;
				yylval.code = GE_EXPR;
				goto done;
			    case '!':
				value = EQCOMPARE;
				yylval.code = NE_EXPR;
				goto done;
			    case '=':
				value = EQCOMPARE;
				yylval.code = EQ_EXPR;
				goto done;
			  }

			value = ASSIGN; goto done;
		}
		elif (c == c1) switch (c)
		{
		    case '+':
			value = PLUSPLUS; goto done;
		    case '-':
			value = MINUSMINUS; goto done;
		    case '&':
			value = ANDAND; goto done;
		    case '|':
			value = OROR; goto done;
		    case '<':
			c = LSHIFT;
			goto combine;
		    case '>':
			c = RSHIFT;
			goto combine;
		}
		elif ((c == '-') && (c1 == '>'))
			{ value = POINTSAT; goto done; }

		ungetc (c1, finput);
		token_buffer[1] = 0;

		if ((c == '<') || (c == '>'))
			value = ARITHCOMPARE;
		else value = c;
			goto done;
	    }

		/*
		 *	Don't make yyparse think this is eof.
		 */
	    case 0:
		value = 1;
		break;

	    default:
		value = c;

	}	/* end switch (c) */

    done:
	/*
	 *	yylloc.last_line = lineno;
	 */
	return value;

}	/* end yylex */
