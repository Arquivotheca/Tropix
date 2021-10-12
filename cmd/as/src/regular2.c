/*
 ****************************************************************
 *								*
 *			regular2.c				*
 *								*
 *	Processamento das operações regulares do passo 2	*
 *								*
 *	Versão	3.0.0, de 23.09.94				*
 *		4.0.0, de 16.11.00				*
 *								*
 *	Rotinas:						*
 *		pass2	regular2    testphase			*
 *								*
 *	Módulo: AS-80386/486/PENTIUM				*
 *		Tradutores					*
 *		Categoria D					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>

#include <stdio.h>
#include <a.out.h>

#include "../h/common.h"
#include "../h/tokens.h"
#include "../h/scan.h"
#include "../h/id_sym_tb.h"
#include "../h/expr.h"
#include "../h/opnd.h"
#include "../h/msg.h"
#include "../h/as.h"
#include "../h/op_code.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Gera Código para as Instruções de Sintaxe Regular	*
 ****************************************************************
 */
int
regular2 (OPCODE *op)
{
	OPADDR		*ap, *end_ap;
	OPND		opnd1, opnd2;
	char		*begin, *end;

	/*
	 *	Prólogo
	 */
	opnd1.op_type = 0; 	opnd2.op_type = 0;

	/*
	 *	Retira o primeiro e segundo argumento (se forem dados)
	 */
	gettoken (); 	begin = token->l_begin;	end = token->l_end;

	if (token->l_major != EOL)
	{
		if (getoperand (&opnd1) == INVAL)
			return (0);

		end = opnd1.op_end;

		if (token->l_major == CM)
		{
			if (gettoken () != EOL)
			{
				if (getoperand (&opnd2) == INVAL)
					return (0);

				end = opnd2.op_end;
			}
		}
	}

	/*
	 *	Analisa se teve referencias a expressões indefinidas
	 */
	if
	(	op->o_flags & IQ && opnd1.op_type & IMMED &&
		needed_immed_tst (opcodeno)
	)
	{
		if (seg16)
			opnd1.op_type =	IMM_16;
		else
			opnd1.op_type =	IMM_32;
	}

	/*
	 *	Em seguida, procura uma entrada adequada da tabela
	 */
	for (ap = op->o_begin, end_ap = op->o_end; /* abaixo */; ap++)
	{
		if (ap >= end_ap)
		{
			msg
			(	ERRO, MYMARK,
				"*** COMP (Regular2): Perdi a entrada!", NOSTR,
				begin, end
			);
			return (0);
		}

		if   (opnd1.op_type == 0)
		{
			if (ap->o_opnd1 == 0)
				break;
		}
		elif (opnd1.op_type & ap->o_opnd1)
		{
			if   (opnd2.op_type == 0)
			{
				if (ap->o_opnd2 == 0)
					break;
			}
			elif (opnd2.op_type & ap->o_opnd2)
			{
				break;
			}
		}

	}	/* end for (tabela) */

	return (putcode (ap, &opnd1, &opnd2));

}	/* end regular2 */




#if (0)	/*************************************/
/*
 ****************************************************************
 *	Gera Código para as Instruções de Sintaxe Regular	*
 ****************************************************************
 */
int
regular2 (OPCODE *op)
{
	OPADDR		*ap = op->o_begin;
	long		opnd;
	OPND		opnd1, opnd2;

	opnd2.op_type = -1;	/* Segundo operando ainda não computado */

	if ((opnd = ap->o_opnd1) != 0)
	{
		GETOPND2 (&opnd1, 0);

		/*
		 *	Analisa se teve referencias a expressões indefinidas
		 */
		if
		(	op->o_flags & IQ && opnd1.op_type & IMMED &&
			needed_immed_tst (opcodeno)
		)
		{
			if (seg16)
				opnd1.op_type =	IMM_16;
			else
				opnd1.op_type =	IMM_32;
		}

		/*
		 *	Procura o operando 1
		 */
	    again:
		while ((opnd &= opnd1.op_type) == 0)
		{
			if (++ap >= op->o_end)
			{
				msg (COMP, LONGMARK, "regular2: erro 1");
				return (0);
			}

			opnd = ap->o_opnd1;
		}

		/*
		 *	Procura o operando 2
		 */
		if ((opnd = ap->o_opnd2) != 0)
		{
			if (opnd2.op_type == -1)
			{
				if (token->l_major != CM)
				{
					msg (COMP, LONGMARK, "regular2: erro 2");
					return (0);
				}

				GETOPND2 (&opnd2, 0);
			}

			if ((opnd &= opnd2.op_type) == 0)
				goto again;
		}

	}	/* end if (examinando operandos) */

	/*
	 *	Gera o Código Correspondente
	 */
	return (putcode (ap, &opnd1, &opnd2));

}	/* end regular2 */
#endif	/*************************************/

/*
 ****************************************************************
 *	Obtém o terceiro operando				*
 ****************************************************************
 */
int
regular2_opnd3 (long type, OPND *opnd3)
{
	/*
	 *	Retorno:
	 *		-1: Erro
	 *		 0: Não há terceiro argumento
	 *		+1: Há terceiro argumento
	 */
	if (token->l_major == EOL)
		return (0);

	if (token->l_major != CM)
		return (0);

	GETOPND2 (opnd3, -1);

	if ((opnd3->op_type & type) == 0)
	{
		msg (COMP, LONGMARK, "regular2: erro 3");
		return (-1);
	}

	return (+1);

}	/* end regular2_opnd3 */

/*
 ****************************************************************
 *	Gera Codigo para uma Instrução				*
 ****************************************************************
 */
int
putcode (OPADDR *ap, OPND *opnd1, OPND *opnd2)
{
	int	 	size, i;
	OPND		opnd3;

	/*
	 *	Processa o prefixo de troca de tamanho de operando
	 */
	if (seg16)	/* Normal == 16 bits */
	{
		if (ap->o_flags & LP)
			size = putB (0x66);
		else
			size = 0;
	}
	else		/* Normal == 32 bits */
	{
		if (ap->o_flags & WP)
			size = putB (0x66);
		else
			size = 0;
	}

	/*
	 *	Verifica o prefixo flutuante
	 */
	if (ap->o_flags & WAITP)
		size += putB (0x9B);

	/*
	 *	Processa os grupos de operações
	 */
	switch (ap->o_group)
	{
		/*
		 *	0 operandos: 	OP
		 */
	    case 0:
		size += putB (ap->o_opcode);
		break;

		/*
		 *	0 operandos: 	OP, EXT
		 */
	    case 1:
		size += putbigW (ap->o_opcode << 8 | ap->o_opexten);
		break;

		/*
		 *	1 operando: 	OP, #IMMED1
		 */
	    case 5:
		size += putB (ap->o_opcode);
		size += putopnd (opnd1, ap->o_opnd1);
		break;

		/*
		 *	1 operando: 	OP|reg1
		 */
	    case 6:
		size += putB (ap->o_opcode | opnd1->op_reg);
		break;

		/*
		 *	1 operando: 	OP|reg2
		 */
	    case 7:
		size += putB (ap->o_opcode | opnd2->op_reg);
		break;

		/*
		 *	1 operando: 	OP, RM1|ext
		 */
	    case 8:
		size += putB (ap->o_opcode);
		size += putopnd (opnd1, ap->o_opexten);
		break;

		/*
		 *	1 operando: 	0xF0, OP, RM1|ext
		 */
	    case 9:
		size += putbigW (0x0F00 | ap->o_opcode);
		size += putopnd (opnd1, ap->o_opexten);
		break;

		/*
		 *	1 operando: 	OP|sreg1   ou	0xF0, EXT|sreg1
		 */
	    case 10:
	    case 11:
		if (opnd1->op_reg < 4)
			size += putB (ap->o_opcode | (opnd1->op_reg << 3));
		else
			size += putbigW (0x0F00 | ap->o_opexten | (opnd1->op_reg << 3));
		break;

		/*
		 *	1 operando: 	OP, RM2|ext (para flutuante)
		 */
	    case 12:
		size += putB (ap->o_opcode);
		size += putopnd (opnd2, ap->o_opexten);
		break;

		/*
		 *	1 operando: 	0xF0, OP|reg1
		 */
	    case 13:
		size += putbigW (0x0F00 | ap->o_opcode | opnd1->op_reg);
		break;

		/*
		 *	2 operandos:	OP, offset1
		 */
	    case 15:
		size += putB (ap->o_opcode);

		if (seg16)
			size += reloc (&opnd1->op_expr, RWORD);
		else
			size += reloc (&opnd1->op_expr, RLONG);
		break;

		/*
		 *	2 operandos:	OP, offset2
		 */
	    case 16:
		size += putB (ap->o_opcode);

		if (seg16)
			size += reloc (&opnd2->op_expr, RWORD);
		else
			size += reloc (&opnd2->op_expr, RLONG);
		break;

		/*
		 *	2 operandos:	OP, RM2|reg1
		 */
	    case 17:
		size += putB (ap->o_opcode);
		size += putopnd (opnd2, opnd1->op_reg);
		break;

		/*
		 *	2 operandos:	0x0F, OP, RM2|reg1
		 */
	    case 18:
		size += putbigW (0x0F00 | ap->o_opcode);
		size += putopnd (opnd2, opnd1->op_reg);
		break;

		/*
		 *	2 operandos:	OP, RM1|reg2
		 */
	    case 19:
	    case_19:
		size += putB (ap->o_opcode);
		size += putopnd (opnd1, opnd2->op_reg);
		break;

		/*
		 *	2 operandos:	0x0F, OP, RM1|reg2
		 */
	    case 20:
		size += putbigW (0x0F00 | ap->o_opcode);
		size += putopnd (opnd1, opnd2->op_reg);
		break;

		/*
		 *	2 operandos:	OP|reg2, #IMMED1
		 */
	    case 21:
		size += putB (ap->o_opcode | opnd2->op_reg);
		size += putopnd (opnd1, ap->o_opnd1);
		break;

		/*
		 *	2 operandos:	OP, #IMMED1
		 */
	    case 22:
		size += putB (ap->o_opcode);
		size += putopnd (opnd1, ap->o_opnd1);
		break;

		/*
		 *	2 operandos:	OP, #IMMED2
		 */
	    case 23:
		size += putB (ap->o_opcode);
		size += putopnd (opnd2, ap->o_opnd2);
		break;

		/*
		 *	2 operandos:	OP, RM2, #IMMED1
		 */
	    case 24:
		size += putB (ap->o_opcode);
		size += putopnd (opnd2, 0);
		size += putopnd (opnd1, ap->o_opnd1);
		break;

		/*
		 *	2 operandos:	OP, RM2|ext, #IMMED1
		 */
	    case 25:
		size += putB (ap->o_opcode);
		size += putopnd (opnd2, ap->o_opexten);
		size += putopnd (opnd1, ap->o_opnd1);
		break;

		/*
		 *	2 operandos:	0x0F, OP, RM2|ext, #IMMED1
		 */
	    case 26:
		size += putbigW (0x0F00 | ap->o_opcode);
		size += putopnd (opnd2, ap->o_opexten);
		size += putopnd (opnd1, ap->o_opnd1);
		break;

		/*
		 *	2 operandos:	OP, #IMMED1, #IMMED2
		 */
	    case 27:
		size += putB (ap->o_opcode);
		size += putopnd (opnd1, ap->o_opnd1);
		size += putopnd (opnd2, ap->o_opnd2);
		break;

		/*
		 *	2 operandos (r2, r0): OP
		 */
	    case 28:
		size += putB (ap->o_opcode);
		break;

		/*
		 *	2 operandos (r0, r2): OP
		 */
	    case 29:
		size += putB (ap->o_opcode);
		break;

		/*
		 *	Shifts & rotates: Registro
		 *	2 operandos:	OP, RM2|ext
		 */
	    case 30:
		size += putB (ap->o_opcode);
		size += putopnd (opnd2, ap->o_opexten);
		break;

		/*
		 *	Shifts & rotates: Imediato
		 *	2 operandos:	OP|0xD0, RM2|ext ou
		 *			OP, RM2|ext, #IMMED1
		 */
	    case 31:
		if
		(	opnd1->op_expr.ex_type == ABS &&
			opnd1->op_expr.ex_value == 1 &&
			!needed_immed_tst (opcodeno)
		)
		{
			size += putB (ap->o_opcode | 0xD0);
			size += putopnd (opnd2, ap->o_opexten);
		}
		else
		{
			size += putB (ap->o_opcode);
			size += putopnd (opnd2, ap->o_opexten);
			size += reloc (&opnd1->op_expr, RBYTE);
		}
		break;

#if (0)	/*************************************/
		if (opnd1->op_expr.ex_value == 1)
		{
			size += putB (ap->o_opcode | 0xD0);
			size += putopnd (opnd2, ap->o_opexten);
		}
		else
		{
			size += putB (ap->o_opcode);
			size += putopnd (opnd2, ap->o_opexten);
			size += putopnd (opnd1, ap->o_opnd1);
		}
		break;
#endif	/*************************************/

		/*
		 *	3 operandos:	OP, RM2/reg3, #IMMED1
		 */
	    case 32:
		size += putB (ap->o_opcode);
		if (regular2_opnd3 (GREG32 /* == 16 */, &opnd3) == 0)
			opnd3.op_reg = opnd2->op_reg;
		size += putopnd (opnd2, opnd3.op_reg);
		size += putopnd (opnd1, ap->o_opnd1);
		break;

		/*
		 *	3 operandos:	0x0F, OP, RM3/reg2
		 */
	    case 33:
		size += putbigW (0x0F00 | ap->o_opcode);
		regular2_opnd3 (RM32 /* == 16 */, &opnd3);
		size += putopnd (&opnd3, opnd2->op_reg);
		break;

		/*
		 *	3 operandos:	0x0F, OP, RM3/reg2, #IMMED1
		 */
	    case 34:
		size += putbigW (0x0F00 | ap->o_opcode);
		regular2_opnd3 (RM32 /* == 16 */, &opnd3);
		size += putopnd (&opnd3, opnd2->op_reg);
		size += putopnd (opnd1, ap->o_opnd1);
		break;

		/*
		 *	Desvios condicionais curtos (deslocamento de 1 byte)
		 */
	    case 50:
		i = checkrexp (&opnd1->op_expr, 2, RBYTE);

		size += putbigW (ap->o_opcode << 8 | i);

		break;

		/*
		 *	Desvios condicionais médios/longos (desl. de 2/4 bytes)
		 */
	    case 52:
		size += putbigW	(0x0F00 | ap->o_opcode);

		if (seg16)
		{
			if  (opnd1->op_expr.ex_type == UNDEF)
				size += reloc_rel (&opnd1->op_expr, 4, RWORD);
			else
				size += putW (checkrexp (&opnd1->op_expr, 4, RWORD));
		}
		else
		{
			if  (opnd1->op_expr.ex_type == UNDEF)
				size += reloc_rel (&opnd1->op_expr, 6, RLONG);
			else
				size += putL (checkrexp (&opnd1->op_expr, 6, RLONG));
		}

		break;

		/*
		 *	Desvios incondicionais médio/longo ("bral", "call")
		 */
	    case 53:
		size += putB (ap->o_opcode);

		if (seg16)
		{
			if  (opnd1->op_expr.ex_type == UNDEF)
				size += reloc_rel (&opnd1->op_expr, 3, RWORD);
			else
				size += putW (checkrexp (&opnd1->op_expr, 3, RWORD));
		}
		else
		{
			if  (opnd1->op_expr.ex_type == UNDEF)
				size += reloc_rel (&opnd1->op_expr, 5, RLONG);
			else
				size += putL (checkrexp (&opnd1->op_expr, 5, RLONG));
		}

		break;

		/*
		 *	Desvios condicionais SDI 32 bits (tamanhos 2, 6)
		 */
	    case 55:
		if (seg16)
			goto case_55_w;

		if   (opnd1->op_expr.ex_type == UNDEF)
		{
			size += putbigW	(0x0F80 | ap->o_opcode);
			size += reloc_rel (&opnd1->op_expr, 6, RLONG);
			break;
		}
		elif (opnd1->op_expr.ex_type != TEXT)
		{
		    should_be_text:
			msg
			(	ERRO, EXPMARK,
				"Destino do SDI não é na seção TEXT",
				NOSTR, &opnd1->op_expr
			);
			break;
		}

		i = opnd1->op_expr.ex_value - *lc - 2;

		if   (i >= -128 && i <= 127)
		{
			size += putbigW (0x7000 | ap->o_opcode << 8 | i & 0xFF);
		}
		else
		{
			i -= (6 - 2);
			size += putbigW	(0x0F80 | ap->o_opcode);
			size += putL	(i);
		}

		break;

		/*
		 *	Desvios condicionais SDI 16 bits (tamanhos 2, 4)
		 */
	    case_55_w:
		if   (opnd1->op_expr.ex_type == UNDEF)
		{
			size += putbigW	(0x0F80 | ap->o_opcode);
			size += reloc_rel (&opnd1->op_expr, 4, RWORD);
			break;
		}
		elif (opnd1->op_expr.ex_type != TEXT)
		{
			goto should_be_text;
		}

		i = opnd1->op_expr.ex_value - *lc - 2;

		if   (i >= -128 && i <= 127)
		{
			size += putbigW (0x7000 | ap->o_opcode << 8 | i & 0xFF);
		}
		elif ((i -= (4 - 2)) >= -32768 && i <= 32767)
		{
			size += putbigW	(0x0F80 | ap->o_opcode);
			size += putW	(i);
		}
		else
		{
			size += 4;

		    out_of_range:
			msg
			(	ERRO, EXPMARK,
				"Valor/deslocamento (%d) fora dos limites",
				i, &opnd1->op_expr
			);
		}

		break;

		/*
		 *	Desvios incondicionais SDI 32 bits (tamanhos 2, 5)
		 */
	    case 56:
		if (seg16)
			goto case_56_w;

		if   (opnd1->op_expr.ex_type == UNDEF)
		{
			size += putB (0xE9);
			size += reloc_rel (&opnd1->op_expr, 5, RLONG);
			break;
		}
		elif (opnd1->op_expr.ex_type != TEXT)
		{
			goto should_be_text;
		}

		i = opnd1->op_expr.ex_value - *lc - 2;

		if   (i >= -128 && i <= 127)
		{
			size += putbigW (0xEB00 | i & 0xFF);
		}
		else
		{
			i -= (5 - 2);
			size += putB (0xE9);
			size += putL (i);
		}

		break;

		/*
		 *	Desvios incondicionais SDI 16 bits (tamanhos 2, 3)
		 */
	    case_56_w:
		if   (opnd1->op_expr.ex_type == UNDEF)
		{
			size += putB (0xE9);
			size += reloc_rel (&opnd1->op_expr, 3, RWORD);
			break;
		}
		elif (opnd1->op_expr.ex_type != TEXT)
		{
			goto should_be_text;
		}

		i = opnd1->op_expr.ex_value - *lc - 2;

		if   (i >= -128 && i <= 127)
		{
			size += putbigW (0xEB00 | i & 0xFF);
		}
		elif ((i -= (3 - 2)) >= -32768 && i <= 32767)
		{
			size += putB (0xE9);
			size += putW (i);
		}
		else
		{
			size += 3;
			goto out_of_range;
		}

		break;

		/*
		 *	Desvios intersegmento médio ("jmpi") expressão
		 *
		 *	OP, EXPR2, #IMMED1
		 */
	    case 58:
		size += putB (ap->o_opcode);
		size += reloc (&opnd2->op_expr, RWORD);
		size += putopnd (opnd1, ap->o_opnd1);

		break;

		/*
		 *	Desvios intersegmento longo ("jmpil") expressão
		 *
		 *	OP, EXPR2, #IMMED1
		 */
	    case 59:
		size += putB (ap->o_opcode);
		size += reloc (&opnd2->op_expr, RLONG);
		size += putopnd (opnd1, ap->o_opnd1);

		break;

		/*
		 *	Prefixos de segmentos
		 */
	    case 60:
	    {
		static const char	seg_vec[6] =
			{ 0x26, 0x2E, 0x36, 0x3E, 0x64, 0x65 };

		size += putB (seg_vec[opnd1->op_reg]);

		break;
	    }

		/*
		 *	Instrução CLR	reg  => EOR	reg,reg
		 *	Instrução TST	reg  => TEST	reg,reg
		 */
	    case 61:
		opnd2 = opnd1;
		goto  case_19;

		/*
		 *	Instrução sintetizada LINK
		 */
	    case 62:
		size += putB	(0x55);		/* pushl	fp */
		size += putbigW (0x89E5);	/* movl		sp,fp */

		i = opnd1->op_expr.ex_value;

#if (0)	/*************************************/
		if   (opnd1->op_expr.ex_type != ABS || (unsigned)i >= 128)
#endif	/*************************************/
		if   (opnd1->op_expr.ex_type != ABS || opnd1->op_type != IMM_8)
		{
			size += putbigW (0x81EC);	/* subl		#i,sp */

			if (seg16)
				size += reloc (&opnd1->op_expr, RWORD);
			else
				size += reloc (&opnd1->op_expr, RLONG);
		}
		elif (i != 0)
		{
			size += putbigW (0x83EC);	/* subl		#i,sp */
			size += putB (i);
		}

		break;

		/*
		 *	Instrução CLR	rm => MOV	#0,rm
		 */
	    case 63:
		size += putB (ap->o_opcode);
		size += putopnd (opnd1, 0);

		if   (ap->o_flags & LP)
			size += putL (0);
		elif (ap->o_flags & WP)
			size += putW (0);
		else
			size += putB (0);

		break;

#if (0)	/*************************************/
		/*
		 *	1 operando: 	OP, RM1|ext
		 */
	    case 70:
		size += putB (ap->o_opcode);
		size += putopnd (opnd1, ap->o_opexten);
		break;
#endif	/*************************************/

		/*
		/*
		 *	Desconhecido
		 */
	    default:
		msg
		(	ERRO, NOMARK,
			"Tipo imprevisto de grupo regular: %d",
			ap->o_group
		);

		break;

	}	/* end switch (group) */

	return (size);

}	/* end putcode */

/*
 ****************************************************************
 *	Escreve um Operando					*
 ****************************************************************
 */
int
putopnd (OPND *opnd, int val)
{
	EXP		*exp;
	int		reg, size = 0;
	long		value;
	ulong		mod_rm_sib;

	exp = &opnd->op_expr; 	reg = val << 3;

	switch (opnd->op_type)
	{
		/*
		 *	Modo Imediato
		 */
	    case IMM_8:
	    case IMM_8U:
	    case IMM_16:
	    case IMM_16U:
	    case IMM_32:
		switch (val)
		{
		    case IMMED8:
		    case IMMED8U:
			return (reloc (exp, RBYTE));

		    case IMMED16:
		    case IMMED16U:
			return (reloc (exp, RWORD));

		    case IMMED32:
			return (reloc (exp, RLONG));

		    default:
			msg
			(	ERRO, NOMARK,
				"Tipo imprevisto de tamanho de imediato: %06X",
				val
			);
			return (0);
		}

		/*
		 *	Os registros inteiros: Igual para 16/32 bits!
		 */
	    case RA:
	    case R3:
	    case R4:
	    case HA:
	    case H3:
		size += putB (0xC0 | reg | opnd->op_reg);
		break;

		/*
		 *	Os registros flutuantes
		 */
	    case F0:
	    case F1:
	    case F7:
		size += putB (val | opnd->op_reg);
		break;

		/*
		 *	Indireto: (base)
		 */
	    case INDIR:
		if (seg16)	/* 16 bits */
		{
			switch (opnd->op_reg)
			{
			    case 6:	/* r4 (esi) */
				mod_rm_sib = 4;
				break;

			    case 7:	/* r5 (edi) */
				mod_rm_sib = 5;
				break;

			    case 3:	/* r3 (ebx) */
				mod_rm_sib = 7;
				break;

			    case 5:	/* fp (ebp) */
			   /***	exp->ex_type = ABS; ***/
			   /***	exp->ex_value = 0; ***/
				goto case_BDISP;

			    default:
			    bad_base:
				msg (ERRO, NOMARK, "putopnd: erro 4");
				break;
			}

			size += putB (reg | mod_rm_sib);
		}
		else		/* INDIR 32 bits */
		{
			switch (opnd->op_reg)
			{
			    case 4:	/* sp (esp) */
			   /***	opnd->op_reg = 4; ***/	/* sp */
				opnd->op_index = -1;	/* Indexado sem índice */
				opnd->op_scale = 0;	/* N_BYTE */
				goto case_INDEX;

			    case 5:	/* fp (ebp) */
			   /***	exp->ex_type = ABS; ***/
			   /***	exp->ex_value = 0; ***/
				goto case_BDISP;
			}

			size += putB (reg | opnd->op_reg);
		}

		break;

		/*
		 *	Base + deslocamento:	expr(base)
		 */
	    case BDISP:
	    case_BDISP:
		if (seg16)	/* 16 bits */
		{
			switch (opnd->op_reg)
			{
			    case 6:	/* r4 (esi) */
				mod_rm_sib = 4;
				break;

			    case 7:	/* r5 (edi) */
				mod_rm_sib = 5;
				break;

			    case 5:	/* fp (ebp) */
				mod_rm_sib = 6;
				break;

			    case 3:	/* r3 (ebx) */
				mod_rm_sib = 7;
				break;

			    default:
				goto bad_base;
			}

			if   (exp->ex_type != ABS || needed_disp_tst (opcodeno))
			{
				size += putB (0x80 | reg | mod_rm_sib);
				size += reloc (exp, RWORD);
			}
			elif ((value = exp->ex_value) >= -128 && value <= 127)
			{
				size += putB (0x40 | reg | mod_rm_sib);
				size += putB (value);
			}
			elif (value >= -32768 && value <= 32767)
			{
				size += putB (0x80 | reg | mod_rm_sib);
				size += putW (value);
			}
			else	/* value de 32 bits */
			{
			    bad_offset:
				msg (ERRO, NOMARK, "putopnd: erro 5");
			}
		}
		else		/* BDISP 32 bits */
		{
			switch (opnd->op_reg)
			{
			    case 4:	/* sp (esp) */
			   /***	opnd->op_reg = 4; ***/	/* sp */
				opnd->op_index = -1;	/* Indexado sem índice */
				opnd->op_scale = 0;	/* N_BYTE */
				goto case_INDEX;
			}

			if   (exp->ex_type != ABS || needed_disp_tst (opcodeno))
			{
				size += putB (0x80 | reg | opnd->op_reg);
				size += reloc (exp, RLONG);
			}
			elif ((value = exp->ex_value) >= -128 && value <= 127)
			{
				size += putB (0x40 | reg | opnd->op_reg);
				size += putB (value);
			}
			else	/* value de 32 bits */
			{
				size += putB (0x80 | reg | opnd->op_reg);
				size += putL (value);
			}
		}

		break;

		/*
		 *	Indexado: expr(base,index[@scale])
		 */
	    case INDEX:
	    case_INDEX:
		if (seg16)	/* 16 bits */
		{
			switch (opnd->op_reg)
			{
			    case 5:	/* fp (ebp) */
				mod_rm_sib = reg | 2;
				break;

			    case 3:	/* r3 (ebx) */
				mod_rm_sib = reg /* | 0 */;
				break;

			    default:
				goto bad_base;
			}

			switch (opnd->op_index)
			{
			    case 6:	/* r4 (esi) */
			   /***	mod_rm_sib |= 0; ***/
				break;

			    case 7:	/* r5 (edi) */
				mod_rm_sib |= 1;
				break;

			    default:
			    bad_index:
				msg (ERRO, NOMARK, "putopnd: erro 6");
				break;
			}

			if (opnd->op_scale != 0)	/* N_BYTE */
			{
			    bad_scale:
				msg (ERRO, NOMARK, "putopnd: erro 7");
			}

			if   (exp->ex_type != ABS || needed_disp_tst (opcodeno))
			{
				size += putB (0x80 | mod_rm_sib);
				size += reloc (exp, RWORD);
			}
			elif ((value = exp->ex_value) == 0)
			{
				size += putB (/* 0x00 | */ mod_rm_sib);
			}
			elif (value >= -128 && value <= 127)
			{
				size += putB (0x40 | mod_rm_sib);
				size += putB (value);
			}
			elif (value >= -32768 && value <= 32767)
			{
				size += putB (0x80 | mod_rm_sib);
				size += putW (value);
			}
			else	/* value de 32 bits */
			{
				goto bad_offset;
			}
		}
		else		/* INDEX 32 bits */
		{
			switch (opnd->op_index)
			{
			    case -1:	/* Indexado sem índice */
				opnd->op_index = 4;	/* sp */
				break;

			    case 4:	/* sp (esp) */
			   	goto bad_index;
			}

			mod_rm_sib  =	(reg << 8) |
					(opnd->op_scale << 6) |
					(opnd->op_index << 3) |
					 opnd->op_reg;
	
			if   (exp->ex_type != ABS || needed_disp_tst (opcodeno))
			{
				size += putbigW (mod_rm_sib | 0x8400);
				size += reloc (exp, RLONG);
			}
			elif ((value = exp->ex_value) == 0 && opnd->op_reg != 5)/* fp */
			{
				size += putbigW (mod_rm_sib | 0x0400);
			}
			elif (value >= -128 && value <= 127)
			{
				size += putbigW (mod_rm_sib | 0x4400);
				size += putB (value);
			}
			else	/* value de 32 bits */
			{
				size += putbigW (mod_rm_sib | 0x8400);
				size += putL (value);
			}
		}
		break;

		/*
		 *	Indexado sem base: expr(*,index[@scale])
		 */
	    case NOBASE:
	    case_NOBASE:
		if (seg16)	/* 16 bits */
		{
			msg (ERRO, NOMARK, "putopnd: erro 8");
		}
		else		/* NOBASE 32 bits */
		{
			switch (opnd->op_index)
			{
			    case 4:	/* sp (esp) */
			   	goto bad_index;
			}

			mod_rm_sib  =	(reg << 8) |
					(opnd->op_scale << 6) |
					(opnd->op_index << 3) |
					 5;	/* fp */
	
			size += putbigW (mod_rm_sib | 0x0400);
			size += reloc (exp, RLONG);
		}

		break;

		/*
		 *	Somente uma expressão
		 */
	    case EXPR:
	    case ASTER:
		if (seg16)	/* 16 bits */
		{
			size += putB (reg | 0x06);
			size += reloc (exp, RWORD);
		}
		else		/* 32 bits */
		{
			size += putB (reg | 0x05);
			size += reloc (exp, RLONG);
		}

		break;

	    default:
		msg
		(	ERRO, NOMARK,
			"Tipo imprevisto de operando: %06X",
			opnd->op_type
		);

	}	/* end switch (opnd->op_type) */

	return (size);

}	/* end putopnd */
