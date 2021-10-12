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
 *  This macro generates the assembly code for function entry	*
 ****************************************************************
 */
/*
 *	This macro generates the assembly code for function
 *	entry. FILE is a stdio stream to output the code to.
 *	SIZE is an int: how many units of temporary storage
 *	to allocate. Refer to the array `regs_ever_live' to
 *	determine which registers to save; `regs_ever_live[I]'
 *	is nonzero if register number I is ever used in the
 *	function. This macro is responsible for knowing which
 *	registers should not be saved even if used
 *
 *	Note that the order of the bit mask for fmovem is the
 *	opposite of the order for movem!
 */
void
function_prologue (register FILE *fp, int SIZE)
{
	register int	regno;
	register int	mask = 0;
	extern char	call_used_regs[];
	int		fsize = ((SIZE) + 3) & -4;

	if (frame_pointer_needed)
	{
		if (TARGET_68020 || fsize < 0x8000)
			fprintf (fp, "\tlink\tfp,#%d\n", -fsize);
		else
			fprintf (fp, "\tlink\tfp,#0\n\tsubl\t#%d,sp\n", fsize);
	}

	for (regno = 24; regno < 56; regno++)
	{
		if (regs_ever_live[regno] && ! call_used_regs[regno])
			fprintf (fp, "\tfpmoved\t%s,-(sp)\n", reg_names[regno]);
	}

	for (regno = 16; regno < 24; regno++)
	{
		if (regs_ever_live[regno] && ! call_used_regs[regno])
			mask |= 1 << (regno - 16);
	}

	if ((mask & 0xFF) != 0)
		fprintf (fp, "\tfmovem\t#0x%X,-(sp)\n", mask & 0xFF);

	mask = 0;

	for (regno = 0; regno < 16; regno++)
	{
		if (regs_ever_live[regno] && ! call_used_regs[regno])
			mask |= 1 << (15 - regno);
	}

	if (frame_pointer_needed)
		mask &= ~ (1 << (15-FRAME_POINTER_REGNUM));

	if   (exact_log2 (mask) >= 0)
		fprintf (fp, "\tmovl\t%s,-(sp)\n", reg_names[15 - exact_log2 (mask)]);
	elif (mask)
		fprintf (fp, "\tmoveml\t#0x%X,-(sp)\n", mask);

}	/* end funtion_prologue */

/*
 ****************************************************************
 *   This macro generates the assembly code for function exit	*
 ****************************************************************
 */
/*
 *	This macro generates the assembly code for function exit,
 *	on machines that need it. If FUNCTION_EPILOGUE is not
 *	defined then individual return instructions are generated
 *	for each return statement. Args are same as for
 *	FUNCTION_PROLOGUE.
 *
 *	The function epilogue should not depend on the current
 *	stack pointer! It should use the frame pointer only.
 *	This is mandatory because of alloca; we also take advantage
 *	of it to omit stack adjustments before returning
 */
void
function_epilogue (register FILE *fp, int SIZE)
{
	register int	regno;
	register int	mask, fmask;
	register int	nregs;
	int		offset, foffset, fpoffset;
	extern char	call_used_regs[];
	extern int	current_function_pops_args;
	extern int	current_function_args_size;
	int		fsize = ((SIZE) + 3) & -4;
	int		big = 0;

	FUNCTION_EXTRA_EPILOGUE (fp, SIZE);

	nregs = 0;	fmask = 0; fpoffset = 0;

	for (regno = 24 ; regno < 56 ; regno++)
	{
		if (regs_ever_live[regno] && ! call_used_regs[regno])
			nregs++;
	}

	fpoffset = nregs * 8;
	nregs = 0;

	for (regno = 16; regno < 24; regno++)
	{
		if (regs_ever_live[regno] && ! call_used_regs[regno])
			{ nregs++; fmask |= 1 << (23 - regno); }
	}

	foffset = fpoffset + nregs * 12;
	nregs = 0;	mask = 0;

	if (frame_pointer_needed)
		regs_ever_live[FRAME_POINTER_REGNUM] = 0;

	for (regno = 0; regno < 16; regno++)
	{
		if (regs_ever_live[regno] && ! call_used_regs[regno])
			{ nregs++; mask |= 1 << regno; }
	}

	offset = foffset + nregs * 4;

	if
	(	offset + fsize >= 0x8000 && frame_pointer_needed &&
		(mask || fmask || fpoffset)
	)
	{
		fprintf (fp, "\tmovl\t#%d,a0\n", -fsize);
		fsize = 0, big = 1;
	}

	if (exact_log2 (mask) >= 0)
	{
		if   (big)
		{
			fprintf
			(	fp, "\tmovl\t-%d(fp,a0),%s\n",
#if (0)	/*************************************/
			(	fp, "\tmovl\tfp@(-%d,a0:l),%s\n",
#endif	/*************************************/
				offset + fsize, reg_names[exact_log2 (mask)]
			);
		}
		elif (! frame_pointer_needed)
		{
			fprintf
			(	fp, "\tmovl\t(sp)+,%s\n",
				reg_names[exact_log2 (mask)]
			);
		}
		else
		{
			fprintf
			(	fp, "\tmovl\t-%d(fp),%s\n",
#if (0)	/*************************************/
			(	fp, "\tmovl\tfp@(-%d),%s\n",
#endif	/*************************************/
				offset + fsize, reg_names[exact_log2 (mask)]
			);
		}
	}
	elif (mask)
	{
		if (big)
		{
			fprintf
			(	fp, "\tmoveml\t-%d(fp,a0),#0x%X\n",
#if (0)	/*************************************/
			(	fp, "\tmoveml\tfp@(-%d,a0:l),#0x%X\n",
#endif	/*************************************/
				offset + fsize, mask
			);
		}
		elif (! frame_pointer_needed)
		{
			fprintf (fp, "\tmoveml\t(sp)+,#0x%X\n", mask);
		}
		else
		{
			fprintf
			(	fp, "\tmoveml\t-%d(fp),#0x%X\n",
#if (0)	/*************************************/
			(	fp, "\tmoveml\tfp@(-%d),#0x%X\n",
#endif	/*************************************/
				offset + fsize, mask
			);
		}
	}

	if (fmask)
	{
		if (big)
		{
			fprintf
			(	fp, "\tfmovem\t-%d(fp,a0),#0x%X\n",
#if (0)	/*************************************/
			(	fp, "\tfmovem\tfp@(-%d,a0:l),#0x%X\n",
#endif	/*************************************/
				foffset + fsize, fmask
			);
		}
		elif (! frame_pointer_needed)
		{
			fprintf (fp, "\tfmovem\t(sp)+,#0x%X\n", fmask);
		}
		else
		{
			fprintf
			(	fp, "\tfmovem\t-%d(fp),#0x%X\n",
#if (0)	/*************************************/
			(	fp, "\tfmovem\tfp@(-%d),#0x%X\n",
#endif	/*************************************/
				foffset + fsize, fmask
			);
		}
	}

	if (fpoffset != 0) for (regno = 55; regno >= 24; regno--)
	{
		if (regs_ever_live[regno] && ! call_used_regs[regno])
		{
			if (big)
			{
				fprintf
				(	fp, "\tfpmoved\t-%d(fp,a0),%s\n",
#if (0)	/*************************************/
				(	fp, "\tfpmoved\tfp@(-%d,a0:l),%s\n",
#endif	/*************************************/
					fpoffset + fsize, reg_names[regno]
				);
			}
			elif (! frame_pointer_needed)
			{
				fprintf
				(	fp, "\tfpmoved\t(sp)+,%s\n",
					reg_names[regno]
				);
			}
			else
			{
				fprintf
				(	fp, "\tfpmoved\t-%d(fp),%s\n",
#if (0)	/*************************************/
				(	fp, "\tfpmoved\tfp@(-%d),%s\n",
#endif	/*************************************/
					fpoffset + fsize, reg_names[regno]
				);

				fpoffset -= 8;
			}
		}
	}

	if (frame_pointer_needed)
		fprintf (fp, "\tunlk\tfp\n");

	if (current_function_pops_args && current_function_args_size)
		fprintf (fp, "\trtd\t#%d\n", current_function_args_size);
	else
		fprintf (fp, "\trts\n");

}	/* end function_epilogue */

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

	if   (code == '.')
	{
		/* vazio */;
	}
	elif (code == '#')
	{
		fprintf (fp, "#");
	}
	elif (code == '-')
	{
		fprintf (fp, "-(sp)");
	}
	elif (code == '+')
	{
		fprintf (fp, "(sp)+");
	}
	elif (code == '@')
	{
		fprintf (fp, "(sp)");
	}
	elif (code == '!')
	{
		fprintf (fp, "cc");
	}
	elif (GET_CODE (x) == REG)
	{
		if (REGNO (x) < 16 && (code == 'y' || code == 'x') && GET_MODE (x) == DFmode)
			fprintf (fp, "%s:%s", reg_names[REGNO (x)], reg_names[REGNO (x)+1]);
		else
			fprintf (fp, "%s", reg_names[REGNO (x)]);
	}
	elif (GET_CODE (x) == MEM)
	{
		output_address (XEXP (x, 0));

		if
		(	code == 'd' && ! TARGET_68020 &&
			CONSTANT_ADDRESS_P (XEXP (x, 0)) &&
			!(GET_CODE (XEXP (x, 0)) == CONST_INT &&
			INTVAL (XEXP (x, 0)) < 0x8000 &&
			INTVAL (XEXP (x, 0)) >= -0x8000)
		)
			fprintf (fp, ":l");
	}
	elif
	(	(code == 'y' || code == 'w') &&
		GET_CODE(x) == CONST_DOUBLE &&
		(i = standard_sun_fpa_constant_p (x))
	)
	{
		fprintf (fp, "%%%d", i & 0x1FF);
	}
	elif (GET_CODE (x) == CONST_DOUBLE && GET_MODE (x) == SFmode)
	{
		union { double d; int i[2]; } u;
		union { float f; int i; } u1;

		PRINT_OPERAND_EXTRACT_FLOAT (x);
		u1.f = u.d;
		PRINT_OPERAND_PRINT_FLOAT (code, fp);
	}
	elif (GET_CODE (x) == CONST_DOUBLE && GET_MODE (x) != DImode)
	{
		union { double d; int i[2]; } u;

		PRINT_OPERAND_EXTRACT_FLOAT (x);
		ASM_OUTPUT_DOUBLE_OPERAND (fp, u.d);
	}
	else
	{
		putc ('#', fp);
		output_addr_const (fp, x);
	}

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

	scale_vec = "bwlq";

	switch (GET_CODE (addr))
	{
	    case REG:
		fprintf (fp, "(%s)", reg_names[REGNO (addr)]);
		break;

	    case PRE_DEC:
		fprintf (fp, "-(%s)", reg_names[REGNO (XEXP (addr, 0))]);
		break;

	    case POST_INC:
		fprintf (fp, "(%s)+", reg_names[REGNO (XEXP (addr, 0))]);
		break;

	    case PLUS:
		reg1 = NULL_RTX;	reg2 = NULL_RTX;
		ireg = NULL_RTX;	breg = NULL_RTX;
		offset = NULL_RTX;

		if (CONSTANT_ADDRESS_P (XEXP (addr, 0)))
		{
			offset = XEXP (addr, 0); addr = XEXP (addr, 1);
		}
		elif (CONSTANT_ADDRESS_P (XEXP (addr, 1)))
		{
			offset = XEXP (addr, 1); addr = XEXP (addr, 0);
		}

		if (GET_CODE (addr) != PLUS)
		{
			/* vazio */;
		}
		elif (GET_CODE (XEXP (addr, 0)) == SIGN_EXTEND)
		{
			reg1 = XEXP (addr, 0); addr = XEXP (addr, 1);
		}
		elif (GET_CODE (XEXP (addr, 1)) == SIGN_EXTEND)
		{
			reg1 = XEXP (addr, 1); addr = XEXP (addr, 0);
		}
		elif (GET_CODE (XEXP (addr, 0)) == MULT)
		{
			reg1 = XEXP (addr, 0); addr = XEXP (addr, 1);
		}
		elif (GET_CODE (XEXP (addr, 1)) == MULT)
		{
			reg1 = XEXP (addr, 1); addr = XEXP (addr, 0);
		}
		elif (GET_CODE (XEXP (addr, 0)) == REG)
		{
			reg1 = XEXP (addr, 0); addr = XEXP (addr, 1);
		}
		elif (GET_CODE (XEXP (addr, 1)) == REG)
		{
			reg1 = XEXP (addr, 1); addr = XEXP (addr, 0);
		}

		if
		(	GET_CODE (addr) == REG || GET_CODE (addr) == MULT ||
			GET_CODE (addr) == SIGN_EXTEND
		)
		{
			if (reg1 == NULL_RTX)
				reg1 = addr;
			else
				reg2 = addr;

			addr = NULL_RTX;
		}

		if (offset != NULL_RTX)
		{	
			if (addr != NULL_RTX)
				abort ();

			addr = offset;
		}

		if
		(	(reg1 && (GET_CODE (reg1) == SIGN_EXTEND ||
			GET_CODE (reg1) == MULT)) ||
			(reg2 != NULL_RTX && REGNO_OK_FOR_BASE_P (REGNO (reg2)))
		)
		{
			breg = reg2; ireg = reg1;
		}
		elif (reg1 != NULL_RTX && REGNO_OK_FOR_BASE_P (REGNO (reg1)))
		{
			breg = reg1; ireg = reg2;
		}

		if (ireg != NULL_RTX && breg == NULL_RTX && GET_CODE (addr) == LABEL_REF)
		{
			int	 scale = 1;

			if (GET_CODE (ireg) == MULT)
			{
				scale = INTVAL (XEXP (ireg, 1));
				ireg = XEXP (ireg, 0);
			}

			if (GET_CODE (ireg) == SIGN_EXTEND)
			{
				fprintf
				(	fp, "L%d-LI%d-2(pc,%s:w",
#if (0)	/*************************************/
				(	fp, "pc@(L%d-LI%d-2,%s:w",
#endif	/*************************************/
					CODE_LABEL_NUMBER (XEXP (addr, 0)),
					CODE_LABEL_NUMBER (XEXP (addr, 0)),
					reg_names[REGNO (XEXP (ireg, 0))]
				);
			}
			else
			{
				fprintf
				(	fp, "L%d-LI%d-2(pc,%s",
#if (0)	/*************************************/
				(	fp, "pc@(L%d-LI%d-2,%s:l",
#endif	/*************************************/
					CODE_LABEL_NUMBER (XEXP (addr, 0)),
					CODE_LABEL_NUMBER (XEXP (addr, 0)),
					reg_names[REGNO (ireg)]
				);
			}

			if (scale != 1)
				fprintf (fp, "@%c", scale_vec[scale]);

			putc (')', fp);
			break;
		}

		if (breg != NULL_RTX && ireg == NULL_RTX && GET_CODE (addr) == LABEL_REF)
		{
			fprintf
			(	fp, "L%d-LI%d-2(pc,%s",
#if (0)	/*************************************/
			(	fp, "pc@(L%d-LI%d-2,%s:l",
#endif	/*************************************/
				CODE_LABEL_NUMBER (XEXP (addr, 0)),
				CODE_LABEL_NUMBER (XEXP (addr, 0)),
				reg_names[REGNO (breg)]
			);

			putc (')', fp);
			break;
		}

		if   (ireg != NULL_RTX || breg != NULL_RTX)
		{
			int	 scale = 1;

			if (breg == NULL_RTX)
				abort ();

			if (addr && GET_CODE (addr) == LABEL_REF)
				abort ();

#if (0)	/*************************************/
			fprintf (fp, "%s@(", reg_names[REGNO (breg)]);
#endif	/*************************************/

			if   (addr != NULL_RTX)
				output_addr_const (fp, addr);
			elif (ireg != NULL_RTX)
				putc ('0', fp);

	/** ===> **/	fprintf (fp, "(%s", reg_names[REGNO (breg)]);

			if (ireg != NULL_RTX)
				putc (',', fp);

#if (0)	/*************************************/
			if (addr != NULL_RTX && ireg != NULL_RTX)
				putc (',', fp);
#endif	/*************************************/

			if (ireg != NULL_RTX && GET_CODE (ireg) == MULT)
			{
				scale = INTVAL (XEXP (ireg, 1));
				ireg = XEXP (ireg, 0);
			}

			if   (ireg != NULL_RTX && GET_CODE (ireg) == SIGN_EXTEND)
				fprintf (fp, "%s:w", reg_names[REGNO (XEXP (ireg, 0))]);
			elif (ireg != NULL_RTX)
				fprintf (fp, "%s", reg_names[REGNO (ireg)]);
#if (0)	/*************************************/
				fprintf (fp, "%s:l", reg_names[REGNO (ireg)]);
#endif	/*************************************/

			if (scale != 1)
				fprintf (fp, "@%c", scale_vec[scale]);

			putc (')', fp);
			break;
		}
		elif (reg1 != NULL_RTX && GET_CODE (addr) == LABEL_REF)
		{
			fprintf
			(	fp, "L%d-LI%d-2(pc,%s)",
#if (0)	/*************************************/
			(	fp, "pc@(L%d-LI%d-2,%s:l)",
#endif	/*************************************/
				CODE_LABEL_NUMBER (XEXP (addr, 0)),
				CODE_LABEL_NUMBER (XEXP (addr, 0)),
				reg_names[REGNO (reg1)]
			);
			break;
		}

	    default:
		if
		(	GET_CODE (addr) == CONST_INT &&
			INTVAL (addr) < 0x8000 && INTVAL (addr) >= -0x8000
		)
		{
			fprintf (fp, "%d:w", INTVAL (addr));
		}
		else
		{
			output_addr_const (fp, addr);
		}
	}

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
