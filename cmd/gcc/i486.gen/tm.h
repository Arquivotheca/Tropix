/* Definitions for Intel 386 running system V with gnu tools
   Copyright (C) 1988 Free Software Foundation, Inc.

This file is part of GNU CC.

GNU CC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 1, or (at your option)
any later version.

GNU CC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU CC; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */


#include "tm-i486.h"

/* Use the bsd assembler syntax.  */
/* we need to do this because gas is really a bsd style assembler,
 * and so doesn't work well this these att-isms:
 *
 *  ASM_OUTPUT_SKIP is .set .,.+N, which isn't implemented in gas
 *  ASM_OUTPUT_LOCAL is done with .set .,.+N, but that can't be
 *   used to define bss static space
 *
 * Next is the question of whether to uses underscores.  RMS didn't
 * like this idea at first, but since it is now obvious that we
 * need this separate tm file for use with gas, at least to get
 * dbx debugging info, I think we should also switch to underscores.
 * We can keep tm-i386v for real att style output, and the few
 * people who want both form will have to compile twice.
 */

#if (0)	/*************************************/
#include "tm-bsd486.h"
#endif	/*************************************/

/* these come from tm-bsd486.h, but are specific to sequent */

#undef DBX_NO_XREFS
#undef DBX_CONTIN_LENGTH

/*
 ******	Definições da seção ".const" ****************************
 */
#define	EXTRA_SECTIONS	in_const, in_bss

#define	EXTRA_SECTION_FUNCTIONS				\
void							\
const_section (void)					\
{							\
	if (in_section != in_const)			\
	{						\
		fprintf (asm_out_file, "\t.const\n");	\
		in_section = in_const;			\
	}						\
							\
}	/* end const_section */				\
							\
void							\
bss_section (void)					\
{							\
	if (in_section != in_bss)			\
	{						\
		fprintf (asm_out_file, "\t.bss\n");	\
		in_section = in_bss;			\
	}						\
							\
}	/* end bss_section */

#define	SELECT_VAR_SECTION(decl)				\
	if (TREE_READONLY (decl) && ! TREE_VOLATILE (decl))	\
		const_section ();				\
	else							\
		data_section ();

#define	SELECT_LABEL_SECTION(exp)					\
	if ((TREE_CODE (exp) == STRING_CST) && flag_writable_strings)	\
		data_section ();					\
	else								\
		const_section ();

#define	SELECT_RTX_SECTION(mode, x)					\
		const_section ();

/*
 ******	TARGET_DEFAULTs *****************************************
 */
/* By default, target has a 80387.  */

#define TARGET_DEFAULT 1

/* Specify predefined symbols in preprocessor.  */

#define CPP_PREDEFINES "-Dunix -Di386"

/* Allow #sccs in preprocessor.  */

#define SCCS_DIRECTIVE

/* Output #ident as a .ident.  */

#define ASM_OUTPUT_IDENT(FILE, NAME) fprintf (FILE, "\t.ident	\"%s\"\n", NAME);

/* We do not want to output SDB debugging information.  */

#undef SDB_DEBUGGING_INFO

/* We want to output DBX debugging information.  */

#define DBX_DEBUGGING_INFO

/* Implicit library calls should use memcpy, not bcopy, etc.  */

#define TARGET_MEM_FUNCTIONS

#if (0)	/*************************************/
#if 0  /* People say gas uses the log as the arg to .align.  */
/* When using gas, .align N aligns to an N-byte boundary.  */

#undef ASM_OUTPUT_ALIGN
#define ASM_OUTPUT_ALIGN(FILE,LOG)	\
     if ((LOG)!=0) fprintf ((FILE), "\t.align\t%d\n", 1<<(LOG))
#endif
#endif	/*************************************/

#define ASM_OUTPUT_ALIGN(FILE,LOG)			\
	if ((LOG) == 1)					\
	{						\
		fprintf (FILE, "\t.align\t2\n");	\
	}						\
	else if ((LOG) == 2)				\
	{						\
		fprintf (FILE, "\t.align\t4\n");	\
	}						\
	else if ((LOG) != 0)				\
	{						\
		fprintf					\
		(	stderr,				\
			"ERRO: align (%d)\n", LOG	\
		);					\
		abort ();				\
	}

#if (0)	/*************************************/
/*
 *	Align labels, etc. at 4-byte boundaries
 */
#define ASM_OUTPUT_ALIGN_CODE(FILE) \
     fprintf ((FILE), "\t.align\t4\n");
#endif	/*************************************/


/* Machines that use the AT&T assembler syntax
   also return floating point values in an FP register.  */
/* Define how to find the value returned by a function.
   VALTYPE is the data type of the value (as a tree).
   If the precise function being called is known, FUNC is its FUNCTION_DECL;
   otherwise, FUNC is 0.  */

#define VALUE_REGNO(MODE) \
  (((MODE)==SFmode || (MODE)==DFmode) ? FIRST_FLOAT_REG : 0)

/* 1 if N is a possible register number for a function value. */

#define FUNCTION_VALUE_REGNO_P(N) ((N) == 0 || (N)== FIRST_FLOAT_REG)

#undef ASM_FILE_START
#define ASM_FILE_START(FILE)

#if (0)	/*************************************/
#undef ASM_FILE_START
#define ASM_FILE_START(FILE) \
  fprintf (FILE, "\t.file\t\"%s\"\n", dump_base_name);
#endif	/*************************************/

/*
 *	Definitions for BSD assembler syntax for Intel 386
 * (actually AT&T syntax for insns and operands,
 * adapted to BSD conventions for symbol names and debugging.)
 */

#define TARGET_VERSION fprintf (stderr, " (80386, TROPIX syntax)");

/*
 *	Define some concatenation macros to concatenate an opcode
 *	and one, two or three operands.  In other assembler syntaxes
 *	they may alter the order of ther operands
 */
#define AS1(a,b)	#a "\t" #b
#define AS2(a,b,c)	#a "\t" #b "," #c
#define AS3(a,b,c,d)	#a "\t" #b "," #c "," #d

/*
 *	Output the size-letter for an opcode. CODE is the letter
 *	used in an operand spec (L, B, W, S or Q). CH is the
 *	corresponding lower case letter (except if CODE is L then
 *	CH is `l')
 */
#define PUT_OP_SIZE(CODE,CH,FILE)	putc (CH,(FILE))

/*
 *	Opcode suffix for fullword insn
 */
#define L_SIZE "l"

/*
 *	Prefix for register names in this syntax
 */
#define RP	""

/*
 *	Prefix for immediate operands in this syntax
 */
#define IP	"#"

/*
 *	Prefix for internally generated assembler labels
 */
#define LPREFIX ".L"

/*
 *	Output the prefix for an immediate operand,
 *	or for an offset operand
 */
#define PRINT_IMMED_PREFIX(FILE)	fputs ("#", (FILE))
#define PRINT_OFFSET_PREFIX(FILE)	fputs ("#", (FILE))

#if (0)	/*************************************/
/*
 *	Indirect call instructions should use `*'
 */
#define USE_STAR	1
#endif	/*************************************/

/*
 *	Prefix for a memory-operand X
 */
#define PRINT_PTR(X, FILE)

/*
 *	Delimiters that surround base reg and index reg
 */
#define ADDR_BEG(FILE)	putc('(', (FILE))
#define ADDR_END(FILE)	putc(')', (FILE))

#if (0)	/*************************************/
/*
 *	Print an index register (whose rtx is IREG)
 */
#define PRINT_IREG(FILE,IREG) \
  do								\
  { fputs (",", (FILE)); PRINT_REG ((IREG), 0, (FILE)); }	\
  while (0)
  
/*
 *	Print an index scale factor SCALE
 */
#define PRINT_SCALE(FILE,SCALE) \
  if ((SCALE) != 1) fprintf ((FILE), ",%d", (SCALE))

/*
 *	Print a base/index combination. BREG is the base reg rtx,
 *	IREG is the index reg rtx, and SCALE is the index scale
 *	factor (an integer)
 */
#define PRINT_B_I_S(BREG, IREG, SCALE, FILE) 			\
	{							\
		putc ('(', (FILE));				\
								\
		if (BREG)					\
			PRINT_REG ((BREG), 0, (FILE));		\
		else						\
			putc ('*', (FILE));			\
								\
		if ((IREG) != 0)				\
		{						\
			putc (',', (FILE));			\
								\
			PRINT_REG ((IREG), 0, (FILE)); 		\
								\
			if ((SCALE) != 'b')			\
				fprintf ((FILE), "@%c", (SCALE));\
		}						\
								\
		putc (')', (FILE));				\
	}
#endif	/*************************************/


/*
 *	Define the syntax of pseudo-ops, labels and comments
 */

/*
 *	Assembler pseudos to introduce constants of various size
 */
#define ASM_BYTE	"\t.byte\t"
#define ASM_SHORT	"\t.word\t"
#define ASM_LONG	"\t.long\t"
#define ASM_DOUBLE	"\t.double\t"

/*
 *	String containing the assembler's comment-starter
 */
#define COMMENT_BEGIN "|*"

/* Output at beginning of assembler file.
   ??? I am skeptical of this -- RMS.  */

#define ASM_FILE_START(FILE)

#if (0)	/*************************************/
#define ASM_FILE_START(FILE) \
  fprintf (FILE, "\t.file\t\"%s\"\n", dump_base_name);
#endif	/*************************************/

#if (0)	/*************************************/
/*
 *	This was suggested, but it shouldn't be right
 *	for DBX output. -- RMS
 */
#define ASM_OUTPUT_SOURCE_FILENAME(FILE, NAME)
#endif	/*************************************/

/*
 *	Output to assembler file text saying following lines
 *	may contain character constants, extra white space,
 *	comments, etc.
 */
#define ASM_APP_ON ""

/*
 *	Output to assembler file text saying following lines
 *	no longer contain unusual constructs
 */
#define ASM_APP_OFF ""


/*
 *	Define the syntax of labels and symbol definitions/declarations
 */

/*
 *	This is how to output an assembler line that says to
 *	advance the location counter by SIZE bytes
 */
#define ASM_OUTPUT_SKIP(FILE,SIZE)  \
		  fprintf (FILE, "\t.dsb\t%u\n", (SIZE))

/*
 *	Output before read-only data
 */
#define TEXT_SECTION_ASM_OP "\t.text"

/*
 *	Output before writable data
 */
#define DATA_SECTION_ASM_OP "\t.data"



/*
 *	Define the syntax of labels and symbol definitions/declarations
 */

/*
 *	This says how to output an assembler line to
 *	define a global common symbol
 */
#define ASM_OUTPUT_COMMON(FILE, NAME, SIZE, ROUNDED)	\
(	fputs ("\t.common\t", (FILE)),			\
	assemble_name ((FILE), (NAME)),			\
	fprintf ((FILE), ",%u\n", (ROUNDED)))


/* This says how to output an assembler line
   to define a entry symbol.  */

#define ASM_OUTPUT_ENTRY(FILE, NAME, SIZE, ROUNDED)	\
asm_output_local_or_entry (FILE, NAME, SIZE, ROUNDED, 0 /* entry */)

/* This says how to output an assembler line
   to define a local common symbol.  */

#define ASM_OUTPUT_LOCAL(FILE, NAME, SIZE, ROUNDED)	\
asm_output_local_or_entry (FILE, NAME, SIZE, ROUNDED, 1 /* local */)

#if (0)	/*************************************/
/* This is how to output an assembler line
   that says to advance the location counter
   to a multiple of 2**LOG bytes.  */

#define ASM_OUTPUT_ALIGN(FILE,LOG)	\
     if ((LOG)!=0) fprintf ((FILE), "\t.align %d\n", (LOG))
#endif	/*************************************/

/* This is how to store into the string BUF
   the symbol_ref name of an internal numbered label where
   PREFIX is the class of label and NUM is the number within the class.
   This is suitable for output with `assemble_name'.  */

#define ASM_GENERATE_INTERNAL_LABEL(BUF,PREFIX,NUMBER)	\
    sprintf ((BUF), "*%s%d", (PREFIX), (NUMBER))

/* This is how to output an internal numbered label where
   PREFIX is the class of label and NUM is the number within the class.  */

#define ASM_OUTPUT_INTERNAL_LABEL(FILE,PREFIX,NUM)	\
  fprintf (FILE, "%s%d:\n", PREFIX, NUM)

/* This is how to output a reference to a user-level label named NAME.  */

#define ASM_OUTPUT_LABELREF(FILE,NAME)	\
  fprintf (FILE, "_%s", NAME)

/* Sequent has some changes in the format of DBX symbols.  */
#define DBX_NO_XREFS 1

/* Don't split DBX symbols into continuations.  */
#define DBX_CONTIN_LENGTH 0
 
/* This is how to output a command to make the user-level label named NAME
   defined for reference from other files.  */

#define ASM_GLOBALIZE_LABEL(FILE,NAME)	\
  (fputs ("\t.global\t", FILE), assemble_name (FILE, NAME), fputs ("\n", FILE))

#if (0)	/*******************************************************/
/* This is how to output an assembler line defining a `double' constant.  */

#undef ASM_OUTPUT_DOUBLE
#define ASM_OUTPUT_DOUBLE(FILE,VALUE)  \
  fprintf (FILE, "\t.double\t%.20e\n", (VALUE))
#endif	/*******************************************************/
