/*
 ****************************************************************
 *								*
 *		    m68k.gen/tropix.final.c			*
 *								*
 *	Funções de saída da geração de código			*
 *								*
 *	Versão	3.0.0, de 09.01.94				*
 *		3.0.0, de 12.01.94				*
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
 ******	Inclusões ***********************************************
 */
#pragma	getenv (GEN)

#include <stdio.h>

#include ../$GEN.gen/config.h
#include "../h/rtl.h"
#include "../h/regs.h"
#include ../$GEN.gen/insn-config.h
#include "../h/recog.h"
#include "../h/conditions.h"
#include "../h/gdbfiles.h"
#include "../h/flags.h"
#include "../h/real.h"
#include "../h/output.h"
#include "../h/stab.h"

/*
 ******	Definições úteis  ***************************************
 */
#define	elif	else if

/*
 ******	Definições externas  ************************************
 */
extern FILE	*asm_out_file; 	/* File in which code is being written */

/*
 ******	Protótipos de funções ***********************************
 */
void		output_address (rtx);

extern rtx	alter_subreg ();

/*
 ****************************************************************
 *	Print operand using machine-dependent assembler syntax	*
 ****************************************************************
 */
/*
 *	Print operand X using machine-dependent assembler syntax.
 *	The macro PRINT_OPERAND is defined just to control this
 *	function. CODE is a non-digit that preceded the
 *	operand-number in the % spec, such as 'z' if the spec
 *	was `%z3'.	CODE is 0 if there was no char between the
 *	% and the digits. When CODE is a non-letter, X is 0.
 *
 *	The meanings of the letters are machine-dependent and
 *	controlled by PRINT_OPERAND
 */
void
output_operand (register rtx x, register int code)
{
	register FILE	*fp = asm_out_file;
	int		i;

	if (x && GET_CODE (x) == SUBREG)
		x = alter_subreg (x);

	PRINT_OPERAND (fp, x, code);

}	/* end output_operand */

/*
 ****************************************************************
 *	Print a memory reference operand for address ADDR	*
 ****************************************************************
 */
/*
 *	Print a memory reference operand for address ADDR using
 *	machine-dependent assembler syntax. The macro
 *	PRINT_OPERAND_ADDRESS exists just to control this function
 *
 *	Note that this contains a kludge that knows that the only
 *	reason we have an address (plus (label_ref...) (reg...))
 *	is in the insn before a tablejump, and we know that m68k.md
 *	generates a label LInnn: on such an insn
 */
void
output_address (register rtx addr)
{
	register FILE	*fp = asm_out_file;
	register rtx	reg1, reg2;
	rtx		breg, ireg;
	rtx		offset;
	char		*scale_vec;

	walk_alter_subreg (addr);

	PRINT_OPERAND_ADDRESS (fp, addr);

}	/* output_address */

/*
 ****************************************************************
 *	Emite a declaração de um "entry" ou "local"		*
 ****************************************************************
 */
void
asm_output_local_or_entry (register FILE *fp, register char *name,
				int size, int rounded, int local)
{
	bss_section ();

	if (!local)
	{
		fputs ("\t.global\t", fp);
		assemble_name (fp, name);
		fputs ("\n", fp);
	}

	switch (size)
	{
	    case 1:
		assemble_name (fp, name);
		fputs (":\n\t.dsb\t1\n", fp);
		return;

	    case 2:
		assemble_name (fp, name);
		fputs (":\n\t.dsw\t1\n", fp);
		return;

	    case 4:
		assemble_name (fp, name);
		fputs (":\n\t.dsl\t1\n", fp);
		return;

	    default:
		fputs ("\t.align\t4\n", fp);

		assemble_name (fp, name);
		fprintf (fp, ":\n\t.dsb\t%u\n", rounded);

	}	/* end switch */

}	/* end asm_output_entry */
