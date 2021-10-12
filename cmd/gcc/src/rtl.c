/*
 ****************************************************************
 *								*
 *			rtl.c					*
 *								*
 *	Allocate, read and print RTL for C-Compiler		*
 *								*
 *	Versão	3.0.0, de 26.08.93				*
 *		3.0.0, de 05.01.94				*
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
#include <ctype.h>

#include ../$GEN.gen/config.h

#include "../h/rtl.h"
#include "../h/obstack.h"

/*
 ******	Definições externas *************************************
 */
#define	obstack_chunk_alloc	xmalloc
#define	obstack_chunk_free	free

#define	NOSTR	(char *)NULL
#define	elif	else if

extern void	*xmalloc (int);
extern void	*xrealloc (void *, int);

/*
 *	Obstack used for allocating RTL objects. Between functions,
 *	this is the permanent_obstack. While parsing and expanding
 *	a function, this is maybepermanent_obstack so we can save
 *	it if it is an inline function. During optimization and
 *	output, this is temporary_obstack
 */
extern struct obstack	*rtl_obstack;

#define MIN(x,y) ((x < y) ? x : y)

/*
 *	Indexed by rtx code, gives number of operands for an rtx with
 *	that code. Does NOT include rtx header data (code and links).
 *	This array is initialized in init_rtl
 */
entry int	rtx_length[NUM_RTX_CODE + 1];

/*
 *	Indexed by rtx code, gives the name of that kind of rtx, as a C string
 */
#define DEF_RTL_EXPR(ENUM, NAME, FORMAT)	NAME ,

const char	*rtx_name[] =
{
#include "../h/rtl.def.v"		/* rtl expressions are documented here */
};

#undef DEF_RTL_EXPR

/*
 *	Indexed by machine mode, gives the name of that machine mode.
 *	This name does not include the letters "mode"
 */
#define DEF_MACHMODE(SYM, NAME, CLASS, SIZE, UNIT, WIDER)	NAME,

const char	*mode_name[] =
{
#include "../h/machmode.def.v"
};

#undef DEF_MACHMODE

/*
 *	Indexed by machine mode, gives the length of the mode, in bytes.
 *	GET_MODE_CLASS uses this
 */
#define DEF_MACHMODE(SYM, NAME, CLASS, SIZE, UNIT, WIDER)	CLASS,

const enum mode_class	mode_class[] =
{
#include "../h/machmode.def.v"
};

#undef DEF_MACHMODE

/*
 *	Indexed by machine mode, gives the length of the mode, in bytes.
 *	GET_MODE_SIZE uses this
 */
#define DEF_MACHMODE(SYM, NAME, CLASS, SIZE, UNIT, WIDER)	\
	(SIZE * UNITS_PER_WORD + 3) / 4,

const int	mode_size[] =
{
#include "../h/machmode.def.v"
};

#undef DEF_MACHMODE

/*
 *	Indexed by machine mode, gives the length of the mode's subunit.
 *	GET_MODE_UNIT_SIZE uses this
 */
#define DEF_MACHMODE(SYM, NAME, CLASS, SIZE, UNIT, WIDER)	\
	(UNIT * UNITS_PER_WORD + 3) / 4,

const int	mode_unit_size[] =
{
#include "../h/machmode.def.v"		/* machine modes are documented here */
};

#undef DEF_MACHMODE

/*
 *	Indexed by machine mode, gives next wider natural mode
 *	(QI -> HI -> SI -> DI, etc.)	Widening multiply instructions use this
 */
#define DEF_MACHMODE(SYM, NAME, CLASS, SIZE, UNIT, WIDER)	\
	(enum machine_mode) WIDER,

const enum machine_mode	mode_wider_mode[] =
{
#include "../h/machmode.def.v"		/* machine modes are documented here */
};

#undef DEF_MACHMODE

/*
 *	Indexed by rtx code, gives a sequence of operand-types for rtx's of
 *	that code. The sequence is a C string in which each charcter
 *	describes one operand
 *
 *	"*" undefined. can cause a warning message
 *	"0" field is unused (or used in a phase-dependent manner)
 *	    prints nothing
 *	"i" an integer. prints the integer
 *	"s" a pointer to a string. prints the string
 *	"S" like "s", but optional: the containing rtx may end
 *	    before this operand
 *	"e" a pointer to an rtl expression. prints the expression
 *	"E" a pointer to a vector that points to a number of rtl
 *	    expressions. prints a list of the rtl expressions
 *	"u" a pointer to another insn. prints the uid of the insn
 */
const char	*rtx_format[] =
{
#define DEF_RTL_EXPR(ENUM, NAME, FORMAT)	FORMAT ,

#include "../h/rtl.def.v"		/* rtl expressions are defined here */

#undef DEF_RTL_EXPR
};

/*
 *	Names for kinds of NOTEs and REG_NOTEs
 */
const char	*note_insn_name[] =
{
	"NOTE_INSN_FUNCTION_BEG",	"NOTE_INSN_DELETED",
	"NOTE_INSN_BLOCK_BEG",		"NOTE_INSN_BLOCK_END",
	"NOTE_INSN_LOOP_BEG",		"NOTE_INSN_LOOP_END",
	"NOTE_INSN_FUNCTION_END",	"NOTE_INSN_SETJMP",
	"NOTE_INSN_LOOP_CONT" 
};

const char	 *reg_note_name[] =
{
	"",		"REG_DEAD",	"REG_INC",
	"REG_EQUIV",	"REG_WAS_0", 	"REG_EQUAL",
	"REG_RETVAL",	"REG_LIBCALL", 	"REG_NONNEG",
	"REG_UNSET"
};

/*
 ****************************************************************
 *	Allocate an rtx vector of N elements			*
 ****************************************************************
 */
/*
 *	Allocate an rtx vector of N elements. Store the length,
 *	and initialize all elements to zero
 */
rtvec
rtvec_alloc (int n)
{
	rtvec		rt;
	int		i;

	rt = (rtvec)obstack_alloc (rtl_obstack, sizeof (struct rtvec_def) + ((n - 1) * sizeof (rtunion)));

	/*
 	 *	clear out the vector
	 */
	PUT_NUM_ELEM(rt, n);

	/*
	 *	Not portable due to rtunion
	 */
	for (i = 0; i < n; i++)
		rt->elem[i].rtvec = NULL_RTVEC;

	return (rt);

}	/* end rtvec_alloc */

/*
 ****************************************************************
 *	Allocate an rtx of code CODE				*
 ****************************************************************
 */
/*
 *	Allocate an rtx of code CODE. The CODE is stored
 *	in the rtx; all the rest is initialized to zero
 */
rtx
rtx_alloc (RTX_CODE code)
{
	rtx		rt;
	register int	nelts, length;

	nelts = GET_RTX_LENGTH (code);
	length = sizeof (struct rtx_def) + (nelts - 1) * sizeof (rtunion);

	rt = (rtx)obstack_alloc (rtl_obstack, length);

	*(int *)rt = 0;
	PUT_CODE (rt, code);

	return (rt);

}	/* end rtx_alloc */

/*
 ****************************************************************
 *	Create a new copy of an rtx				*
 ****************************************************************
 */
/*
 *	Create a new copy of an rtx. Recursively copies the
 *	operands of the rtx, except for those few rtx codes
 *	that are sharable
 */
rtx
copy_rtx (register rtx orig)
{
	register rtx		copy;
	register int		i, j;
	register RTX_CODE	code;
	register char		*format_ptr;

	code = GET_CODE (orig);

	switch (code)
	{
	    case REG:
	    case QUEUED:
	    case CONST_INT:
	    case CONST_DOUBLE:
	    case SYMBOL_REF:
	    case CODE_LABEL:
	    case PC:
	    case CC0:
		return (orig);
	}

	copy = rtx_alloc (code);
	PUT_MODE (copy, GET_MODE (orig));

	copy->in_struct = orig->in_struct;
	copy->volatil = orig->volatil;
	copy->unchanging = orig->unchanging;
	copy->integrated = orig->integrated;
	
	format_ptr = GET_RTX_FORMAT (GET_CODE (copy));

	for (i = 0; i < GET_RTX_LENGTH (GET_CODE (copy)); i++)
	{
		switch (*format_ptr++)
		{
		    case 'e':
			XEXP (copy, i) = XEXP (orig, i);

			if (XEXP (orig, i) != NULL_RTX)
				XEXP (copy, i) = copy_rtx (XEXP (orig, i));
			break;

		    case 'E':
			XVEC (copy, i) = XVEC (orig, i);

			if (XVEC (orig, i) != NULL_RTVEC)
			{
				XVEC (copy, i) = rtvec_alloc (XVECLEN (orig, i));

				for (j = 0; j < XVECLEN (copy, i); j++)
					XVECEXP (copy, i, j) = copy_rtx (XVECEXP (orig, i, j));
			}
			break;

		default:
			XINT (copy, i) = XINT (orig, i);
			break;

		}	/* end switch (*format_ptr++) */

	}	/* end for (i = ...) */

	return (copy);

}	/* end copy_rtx */

/*
 ****************************************************************
 *	Printing rtl for debugging dumps			*
 ****************************************************************
 */
static FILE *outfile;

char spaces[] = "                                                                                                                                                                ";

static int sawclose = 0;

/*
 ****************************************************************
 *	Print IN_RTX onto OUTFILE.				*
 ****************************************************************
 */
/*
 *	Print IN_RTX onto OUTFILE.
 *	This is the recursive part of printing
 */
void
print_rtx (register rtx in_rtx)
{
	static int	indent;
	register int	i, j;
	register char	*format_ptr;

	if (sawclose)
	{
		fprintf (outfile, "\n%s", (spaces + (sizeof spaces - indent * 2)));
		sawclose = 0;
	}

	if (in_rtx == NULL_RTX)
	{
		fprintf (outfile, "(nil)");
		sawclose = 1;
		return;
	}

	/*
	 *	print name of expression code
	 */
	fprintf (outfile, "(%s", GET_RTX_NAME (GET_CODE (in_rtx)));

	if (in_rtx->in_struct)
		fprintf (outfile, "/s");

	if (in_rtx->volatil)
		fprintf (outfile, "/v");

	if (in_rtx->unchanging)
		fprintf (outfile, "/u");

	if (in_rtx->integrated)
		fprintf (outfile, "/i");

	/*
	 *	Print REG_NOTE names for EXPR_LIST and INSN_LIST
	 */
	if (GET_MODE (in_rtx) != VOIDmode)
	{
		if (GET_CODE (in_rtx) == EXPR_LIST || GET_CODE (in_rtx) == INSN_LIST)
			fprintf (outfile, ":%s", GET_REG_NOTE_NAME (GET_MODE (in_rtx)));
		else
			fprintf (outfile, ":%s", GET_MODE_NAME (GET_MODE (in_rtx)));
	}

	format_ptr = GET_RTX_FORMAT (GET_CODE (in_rtx));

	for (i = 0; i < GET_RTX_LENGTH (GET_CODE (in_rtx)); i++)
	{
		switch (*format_ptr++)
		{
		    case 'S':
		    case 's':
			if (XSTR (in_rtx, i) == NOSTR)
				fprintf (outfile, " \"\"");
			else
				fprintf (outfile, " (\"%s\")", XSTR (in_rtx, i));
			sawclose = 1;
			break;

			/*
			 *	0 indicates a field for internal
			 *	use that should not be printed
			 */
		    case '0':
			break;

		    case 'e':
			indent += 2;

			if (!sawclose)
				fprintf (outfile, " ");

			print_rtx (XEXP (in_rtx, i));
			indent -= 2;
			break;

		    case 'E':
			indent += 2;

			if (sawclose)
			{
				fprintf (outfile, "\n%s", (spaces + (sizeof spaces - indent * 2)));
				sawclose = 0;
			}

			fprintf (outfile, "[ ");

			if (NULL_RTVEC != XVEC (in_rtx, i))
			{
				indent += 2;

				if (XVECLEN (in_rtx, i))
					sawclose = 1;

				for (j = 0; j < XVECLEN (in_rtx, i); j++)
					print_rtx (XVECEXP (in_rtx, i, j));

				indent -= 2;
			}

			if (sawclose)
				fprintf (outfile, "\n%s", (spaces + (sizeof spaces - indent * 2)));

			fprintf (outfile, "] ");
			sawclose = 1;
			indent -= 2;
			break;

		    case 'i':
			fprintf (outfile, " %d", XINT (in_rtx, i));
			sawclose = 0;
			break;

			/*
			 *	Print NOTE_INSN names rather than integer codes
			 */
		    case 'n':
			if (XINT (in_rtx, i) <= 0)
				fprintf (outfile, " %s", GET_NOTE_INSN_NAME (XINT (in_rtx, i)));
			else
				fprintf (outfile, " %d", XINT (in_rtx, i));

			sawclose = 0;
			break;

		    case 'u':
			if (XEXP (in_rtx, i) != NULL_RTX)
				fprintf(outfile, " %d", INSN_UID (XEXP (in_rtx, i)));
			else
				fprintf(outfile, " 0");
			sawclose = 0;
			break;

		    default:
			fprintf
			(	stderr,
				"switch format wrong in rtl.print_rtx(). "
				"format was: %c.\n",
				format_ptr[-1]
			);
			abort ();

		}	/* end switch (*format_ptr++) */

	}	/* end for (i = ...) */

	fprintf (outfile, ")");

	sawclose = 1;

}	/* end print_rtx */

/*
 ****************************************************************
 * Call this function from the debugger to see what X looks like*
 ****************************************************************
 */
void
debug_rtx (rtx x)
{
	outfile = stderr;

	print_rtx (x);

	fprintf (stderr, "\n");

}	/* end debug_rtx */

/*
 ****************************************************************
 *	External entry point for printing a chain of insns	*
 ****************************************************************
 */
/*
 *	External entry point for printing a chain of insns
 *	starting with RTX_FIRST onto file OUTF.
 *	A blank line separates insns.
 *
 *	If RTX_FIRST is not an insn, then it alone is printed,
 *	with no newline
 */
void
print_rtl (FILE *outf, rtx rtx_first)
{
	register rtx	tmp_rtx;

	outfile = outf;
	sawclose = 0;

	switch (GET_CODE (rtx_first))
	{
	    case INSN:
	    case JUMP_INSN:
	    case CALL_INSN:
	    case NOTE:
	    case CODE_LABEL:
	    case BARRIER:
		for (tmp_rtx = rtx_first; NULL_RTX != tmp_rtx; tmp_rtx = NEXT_INSN (tmp_rtx))
		{
			print_rtx (tmp_rtx);
			fprintf (outfile, "\n");
		}
		break;

	    default:
		print_rtx (rtx_first);
	}

}	/* end print_rtl */

/*
 ****************************************************************
 *	Dump code after printing a message			*
 ****************************************************************
 */
/*
 *	Dump code after printing a message. Used when
 *	read_rtx finds invalid data
 */
void
dump_and_abort (int expected_c, int actual_c, FILE *infile)
{
	int		c, i;

	if (expected_c >= 0)
	{
		fprintf
		(	stderr,
			"Expected character %c.  Found character %c.",
			expected_c, actual_c
		);
	}

	fprintf (stderr, "  At file position: %ld\n", ftell (infile));
	fprintf (stderr, "Following characters are:\n\t");

	for (i = 0; i < 200; i++)
	{
		c = getc (infile);

		if (EOF == c)
			break;

		putc (c, stderr);
	}

	fprintf (stderr, "Aborting.\n");

	abort ();

}	/* end dump_and_abort */

/*
 ****************************************************************
 *	Read chars from INFILE until a non-whitespace char	*
 ****************************************************************
 */
/*
 *	Read chars from INFILE until a non-whitespace char
 *	and return that. Comments, both Lisp style and C style,
 *	are treated as whitespace. Tools such as genflags use
 *	this function
 */
int
read_skip_spaces (FILE *infile)
{
	register	int c;

	while (c = getc (infile))
	{
		if (c == ' ' || c == '\n' || c == '\t' || c == '\f')
		{
			/* vazio */;
		}
		elif (c == ';')
		{
			while ((c = getc (infile)) && c != '\n')
				/* vazio */;
		}
		elif (c == '/')
		{
			register int	prevc;

			c = getc (infile);

			if (c != '*')
				dump_and_abort ('*', c, infile);
		
			prevc = 0;

			while (c = getc (infile))
			{
				if (prevc == '*' && c == '/')
					break;

				prevc = c;
			}
		}
		else
		{
			break;
		}
	}

	return (c);

}	/* end read_skip_spaces */

/*
 ****************************************************************
 *	Read an rtx code name into the buffer STR[]		*
 ****************************************************************
 */
/*
 *	Read an rtx code name into the buffer STR[]. It is
 *	terminated by any of the punctuation chars of rtx
 *	printed syntax
 */
void
read_name (char *str, FILE *infile)
{
	register char	*p;
	register int	c;

	c = read_skip_spaces (infile);

	p = str;

	while (1)
	{
		if (c == ' ' || c == '\n' || c == '\t' || c == '\f')
			break;

		if
		(	c == ':' || c == ')' || c == ']' || c == '"' ||
			c == '/' || c == '(' || c == '['
		)
		{
			ungetc (c, infile);
			break;
		}

		*p++ = c;
		c = getc (infile);
	}

	if (p == str)
	{
		fprintf (stderr, "missing name or number");
		dump_and_abort (-1, -1, infile);
	}

	*p = 0;

}	/* end read_name */

/*
 ****************************************************************
 *	Read an rtx in printed representation from INFILE	*
 ****************************************************************
 */
/*
 *	Read an rtx in printed representation from INFILE
 *	and return an actual rtx in core constructed accordingly.
 *	read_rtx is not used in the compiler proper, but rather in
 *	the utilities gen*.c that construct C code from machine
 *	descriptions
 */
rtx
read_rtx (register FILE *infile)
{
	register int	i, j, list_counter;
	RTX_CODE	tmp_code;
	register char	*format_ptr;

	/*
 	 *	tmp_char is a buffer used for reading decimal integers
	 *	and names of rtx types and machine modes. Therefore,
	 *	256 must be enough
	 */
	char		tmp_char[256];
	rtx		return_rtx;
	register int	c;
	int		tmp_int;

	/*
	 *	Linked list structure for making RTXs:
	 */
	struct rtx_list
	{
		struct rtx_list *next;
		rtx value;		/* Value of this node...		*/
	};

	/*
	 *	Should be open paren
	 */
	c = read_skip_spaces (infile);

	if (c != '(')
		dump_and_abort ('(', c, infile);

	read_name (tmp_char, infile);

	tmp_code = UNKNOWN;

	/*
	 *	might speed this search up
	 */
	for (i = 0; i < NUM_RTX_CODE; i++)
	{
		if (!(strcmp (tmp_char, GET_RTX_NAME (i))))
		{
			tmp_code = (RTX_CODE) i;	/* get value for name */
			break;
		}
	}

	if (tmp_code == UNKNOWN)
	{
		fprintf
		(	stderr,
			"Unknown rtx read in rtl.read_rtx(). Code name was %s .",
			tmp_char
		);
	}

	/*
	 *	(NIL) stands for an expression that isn't there
	 */
	if (tmp_code == NIL)
	{
		/*
		 *	Discard the closeparen
		 */
		while ((c = getc (infile)) && c != ')')
			/* vazio */;

		return (NULL_RTX);
	}

	/*
	 *	if we end up with an insn expression then we free
	 *	this space below
	 */
	return_rtx = rtx_alloc (tmp_code);

	format_ptr = GET_RTX_FORMAT (GET_CODE (return_rtx));

	/*
	 *	If what follows is `: mode ', read it and store the
	 *	mode in the rtx
	 */
	i = read_skip_spaces (infile);

	if (i == ':')
	{
		register int	k;

		read_name (tmp_char, infile);

		for (k = 0; k < NUM_MACHINE_MODES; k++)
		{
			if (!strcmp (GET_MODE_NAME (k), tmp_char))
				break;
		}

		PUT_MODE (return_rtx, (enum machine_mode) k );
	}
	else
	{
		ungetc (i, infile);
	}

	for (i = 0; i < GET_RTX_LENGTH (GET_CODE (return_rtx)); i++)
	{
		switch (*format_ptr++)
		{
			/*
			 *	0 means a field for internal use only.
			 *	Don't expect it to be present in the input
			 */
		    case '0':
			break;

		    case 'e':
		    case 'u':
			XEXP (return_rtx, i) = read_rtx (infile);
			break;

		    case 'E':
		    {
			register struct	rtx_list *next_rtx, *rtx_list_link;
			struct rtx_list		*list_rtx;

			c = read_skip_spaces (infile);

			if (c != '[')
				dump_and_abort ('[', c, infile);

			/*
			 *	add expressions to a list, while keeping
			 *	a count
			 */
			next_rtx = (struct rtx_list *)NULL;
			list_counter = 0;

			while ((c = read_skip_spaces (infile)) && c != ']')
			{
				ungetc (c, infile);
				list_counter++;

				rtx_list_link = (struct rtx_list *)alloca (sizeof (struct rtx_list));
				rtx_list_link->value = read_rtx (infile);

				if (next_rtx == (struct rtx_list *)NULL)
					list_rtx = rtx_list_link;
				else
					next_rtx->next = rtx_list_link;

				next_rtx = rtx_list_link;
				rtx_list_link->next = (struct rtx_list *)NULL;
			}

			/*
			 *	get vector length and allocate it
			 */
			XVEC (return_rtx, i) = (list_counter ? rtvec_alloc (list_counter) : (struct rtvec_def *)NULL);

			if (list_counter > 0) 
			{
				next_rtx = list_rtx;

				for (j = 0; j < list_counter; j++,
					next_rtx = next_rtx->next)

				XVECEXP (return_rtx, i, j) = next_rtx->value;
			}

			/* close bracket gotten */
			break;
		    }

			/*
			 *	'S' is an optional string: if a closeparen
			 *	follows, just store NULL for this element
			 */
		    case 'S':
			c = read_skip_spaces (infile);
			ungetc (c, infile);

			if (c == ')')
			{
				XSTR (return_rtx, i) = NOSTR;
				break;
			}

		    case 's':
		    {
			int		saw_paren = 0;
			register char	*stringbuf;
			int		stringbufsize;

			c = read_skip_spaces (infile);

			if (c == '(')
			{
				saw_paren = 1;
				c = read_skip_spaces (infile);
			}

			if (c != '"')
				dump_and_abort ('"', c, infile);

			j = 0;
			stringbufsize = 10;
			stringbuf = xmalloc (stringbufsize + 1);

			while (1)
			{
				if (j >= stringbufsize - 4)
				{
					stringbufsize *= 2;
					stringbuf = (char *) xrealloc (stringbuf, stringbufsize + 1);
				}

				/*
				 *	Read the string
				 */
				stringbuf[j] = getc (infile);

				if   (stringbuf[j] == '\\')
				{
					/*
					 *	Read the string
					 *
					 *	\; makes stuff for a C string
					 *	constant containing newline and
					 *	tab
					 */
					stringbuf[j] = getc (infile);

					if (stringbuf[j] == ';')
					{
						strcpy (&stringbuf[j], "\\n\\t");
						j += 3;
					}
				}
				elif (stringbuf[j] == '"')
				{
					break;
				}

				j++;
			}

			/*
			 *	NULL terminate the string
			 */
			stringbuf[j] = 0;
			stringbuf = (char *) xrealloc (stringbuf, j + 1);

			if (saw_paren)
			{
				c = read_skip_spaces (infile);

				if (c != ')')
					dump_and_abort (')', c, infile);
			}

			XSTR (return_rtx, i) = stringbuf;
			break;
		    }

		    case 'i':
		    case 'n':
			read_name (tmp_char, infile);
			tmp_int = atoi (tmp_char);
			XINT (return_rtx, i) = tmp_int;
			break;

		    default:
			fprintf
			(	stderr,
				"switch format wrong in "
				"rtl.read_rtx(). format was: %c.\n",
				format_ptr[-1]
			);

			fprintf
			(	stderr,
				"\tfile position: %ld\n",
				ftell (infile)
			);

			abort ();
		}
	}

	c = read_skip_spaces (infile);

	if (c != ')')
		dump_and_abort (')', c, infile);

	return (return_rtx);

}	/* end read_rtx */

/*
 ****************************************************************
 *	Initializes the vector `rtx_length'			*
 ****************************************************************
 */
/*
 *	This is called once per compilation, before any rtx's
 *	are constructed. It initializes the vector `rtx_length'
 */
void
init_rtl (void)
{
	int		i;

	for (i = 0; i < NUM_RTX_CODE; i++)
		rtx_length[i] = strlen (rtx_format[i]);

	/*
	 *	Make CONST_DOUBLE bigger, if real values are bigger than
	 *	it normally expects to have room for. Note that
	 *	REAL_VALUE_TYPE is not defined by default, since tree.h
	 *	is not included. But the default dfn as `double' would
	 *	do no harm
	 */
#ifdef	REAL_VALUE_TYPE
	i = sizeof (REAL_VALUE_TYPE) / sizeof (rtunion) + 2;

	if (rtx_length[(int) CONST_DOUBLE] < i)
	{
		char		*s = malloc (i + 1);

		rtx_length[(int) CONST_DOUBLE] = i;
		rtx_format[(int) CONST_DOUBLE] = s;

		*s++ = 'e';
		*s++ = '0';

		/*
		 *	Set the GET_RTX_FORMAT of CONST_DOUBLE to a string
		 *	of as many `i's as we now have elements
		 */
		for (i = 0; i < rtx_length[(int) CONST_DOUBLE]; i++)
			*s++ = 'i';

		*s++ = 0;
	}
#endif	REAL_VALUE_TYPE

}	/* end init_rtl */
