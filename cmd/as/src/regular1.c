/*
 ****************************************************************
 *								*
 *			regular1.c				*
 *								*
 *	Processamento das operações regulares do passo 1	*
 *								*
 *	Versão	3.0.0, de 23.09.94				*
 *		4.4.0, de 17.03.03				*
 *								*
 *	Módulo: AS-80386/486/PENTIUM				*
 *		Tradutores					*
 *		Categoria D					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2003 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
 ******	Os vetores de indefinidos no passo 1 ********************
 */
#define	NEEDED_SHIFT	10			/* 1024 Bytes */
#define	NEEDED_SZ	(1 << NEEDED_SHIFT)
#define	NEEDED_MASK	(NEEDED_SZ - 1)

entry char	*needed_immed_in_pass1_vec;	/* Imediatos */
entry int	needed_immed_in_pass1_codes;	

entry char	*needed_disp_in_pass1_vec;	/* Deslocamentos */
entry int	needed_disp_in_pass1_codes;	

/*
 ****************************************************************
 *	Trata as Instruções que têm Sintaxe Regular		*
 ****************************************************************
 */
int
regular1 (OPCODE *op)
{
	OPADDR		*ap, *end_ap;
	OPND		opnd1, opnd2;
	char		*begin, *End;

	/*
	 *	Prólogo
	 */
	opnd1.op_type = 0; 	opnd2.op_type = 0;

	/*
	 *	Retira o primeiro e segundo argumento (se forem dados)
	 */
	gettoken (); 	begin = token->l_begin;	End = token->l_end;

	if (token->l_major != EOL)
	{
		if (getoperand (&opnd1) == INVAL)
			return (0);

		End = opnd1.op_end;

		if (token->l_major == CM)
		{
			if (latoken->l_major == EOL)
			{
				msg (ADVR, LONGMARK, "Vírgula ignorada");
				gettoken ();
			}
			else
			{
				gettoken ();

				if (getoperand (&opnd2) == INVAL)
					return (0);

				End = opnd2.op_end;
			}
		}
	}

	/*
	 *	Analisa se tem referencia a expressões indefinidas
	 */
	if
	(	op->o_flags & IQ && opnd1.op_type & IMMED &&
		opnd1.op_expr.ex_type == UNDEF
	)
	{
		needed_immed_set (opcodeno);
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
				"Operando(s) inválido(s)", NOSTR,
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

	/*
	 *	Verifica se a linha acabou
	 */
	if (token->l_major != EOL && (ap->o_flags & getOPND3) == 0)
	{
		begin = token->l_begin;

		while (latoken->l_major != EOL)
			gettoken ();

		msg
		(	ADVR, MYMARK,
			"Operando(s) supérfluo(s): serão ignorados",
			NOSTR, begin, token->l_end
		);
	}

	return (get_code_sz (ap, &opnd1, &opnd2));

}	/* end regular1 */



#if (0)	/*************************************/
/*
 ****************************************************************
 *	Trata as Instruções que têm Sintaxe Regular		*
 ****************************************************************
 */
int
regular1 (OPCODE *op)
{
	OPADDR		*ap, *end_ap;
	int		second_operand_present = 0;
	OPND		*opnderr, opnd1, opnd2;
	char		*begin;

	ap     = op->o_begin;
	end_ap = op->o_end;

	/*
	 *	Verifica se a operação espera operandos. Repare que não
	 *	é possível ter operandos opcionais!
	 */
	if (ap->o_opnd1 != 0)
	{
		/*
		 *	A instrução tem operandos: analisa o primeiro deles
		 */
		gettoken (); 		begin = token->l_begin;

		if (getoperand (&opnd1) == INVAL)
			return (0);

		opnderr = &opnd1;

		/*
		 *	Analisa se tem referencia a expressões indefinidas
		 */
		if
		(	op->o_flags & IQ && opnd1.op_type & IMMED &&
			opnd1.op_expr.ex_type == UNDEF
		)
		{
			needed_immed_set (opcodeno);
		}

		/*
		 *	Procura, na tabela de enderecamentos, um padrão que
		 *	seja satisfeito pelo tipo do primeiro operando
		 */
	    again:
		for (/* acima */; /* abaixo */; ap++)
		{
			if (ap >= end_ap)
			{
				msg
				(	ERRO, OPNDMARK,
					"Operando inválido", NOSTR, opnderr
				);
				return (0);
			}

			if (ap->o_opnd1 & opnd1.op_type)
				break;
		}

		/*
		 *	Encontrou: verifica se espera mais um operando
		 */
		if (ap->o_opnd2 != 0)
		{
			if (!second_operand_present)
			{
				if (token->l_major != CM)
				{
					msg
					(	ERRO, MYMARK,
						"Número errado de operandos",
						NOSTR, begin, token->l_end
					);
					return (0);
				}

				gettoken ();

				if (getoperand (&opnd2) == INVAL)
					return (0);

				second_operand_present++;
			}

			/*
			 *	Analisa o segundo operando
			 */
			if ((ap->o_opnd2 & opnd2.op_type) == 0)
			{
				opnderr = &opnd2;

				ap++; 	goto again;
			}
		}

		/*
		 *	Verifica se a linha acabou
		 */
		if (token->l_major != EOL && (ap->o_flags & getOPND3) == 0)
		{
			begin = token->l_begin;

		    ignore_the_rest:
			while (latoken->l_major != EOL)
				gettoken ();

			msg
			(	ADVR, MYMARK,
				"Operando(s) supérfluo(s): serão ignorados",
				NOSTR, begin, token->l_end
			);
		}

	}
	else	/* A instrução não espera operandos */
	{
		if (latoken->l_major != EOL)
		{
			begin = latoken->l_begin;
			goto ignore_the_rest;
		}
	}

	return (get_code_sz (ap, &opnd1, &opnd2));

}	/* end regular1 */
#endif	/*************************************/

/*
 ****************************************************************
 *	Obtém o terceiro operando				*
 ****************************************************************
 */
int
regular1_opnd3 (long type, OPND *opnd3)
{
	char		*begin;

	/*
	 *	Retorno:
	 *		-1: Erro
	 *		 0: Não há terceiro argumento
	 *		+1: Há terceiro argumento
	 */
	if (token->l_major == EOL)
		return (0);

	if (token->l_major != CM)
	{
		msg
		(	ADVR, LONGMARK,
			"Procurando por um operando opcional"
		);
		return (0);
	}

	gettoken ();

	if (getoperand (opnd3) == INVAL)
		return (-1);

	if ((opnd3->op_type & type) == 0)
	{
		msg
		(	ERRO, OPNDMARK,
			"Operando inválido", NOSTR, opnd3
		);
		return (-1);
	}

	if (token->l_major != EOL)
	{
		begin = token->l_begin;

		while (latoken->l_major != EOL)
			gettoken ();

		msg
		(	ADVR, MYMARK,
			"Operando(s) supérfluo(s): serão ignorados",
			NOSTR, begin, token->l_end
		);
	}

	return (+1);

}	/* end regular1_opnd3 */

/*
 ****************************************************************
 *	Calcula o tamanho de uma instrução			*
 ****************************************************************
 */
int
get_code_sz (OPADDR *ap, OPND *opnd1, OPND *opnd2)
{
	int 		size;
	int		i;
	OPND		opnd3;

	/*
	 *	Processa o prefixo de troca de tamanho de operando
	 */
	if (seg16)	/* Normal == 16 bits */
	{
		if (ap->o_flags & LP)
			size = 1;
		else
			size = 0;
	}
	else		/* Normal == 32 bits */
	{
		if (ap->o_flags & WP)
			size = 1;
		else
			size = 0;
	}

	/*
	 *	Verifica o prefixo flutuante
	 */
	if (ap->o_flags & WAITP)
		size += 1;

	/*
	 *	Processa os grupos de operações
	 */
	switch (ap->o_group)
	{
		/*
		 *	0 operandos: 	OP
		 */
	    case 0:
		size += 1;
		break;

		/*
		 *	0 operandos: 	OP, EXT
		 */
	    case 1:
		size += 2;
		break;

		/*
		 *	1 operando: 	OP, #IMMED1
		 */
	    case 5:
		size += 1;
		size += get_opnd_sz (opnd1, ap->o_opnd1);
		break;

		/*
		 *	1 operando: 	OP|reg1
		 */
	    case 6:
		size += 1;
		break;

		/*
		 *	1 operando: 	OP|reg2
		 */
	    case 7:
		size += 1;
		break;

		/*
		 *	1 operando: 	OP, RM1|ext
		 */
	    case 8:
		size += 1;
		size += get_opnd_sz (opnd1, 0);
		break;

		/*
		 *	1 operando: 	0xF0, OP, RM1|ext
		 */
	    case 9:
		size += 2;
		size += get_opnd_sz (opnd1, 0);
		break;

		/*
		 *	1 operando: 	OP|sreg1   ou	0xF0, EXT|sreg1 
		 */
	    case 10:
		if (opnd1->op_reg < 4)
			size += 1;
		else
			size += 2;
		break;

		/*
		 *	1 operando: 	OP|sreg1  ou	0xF0, EXT|sreg1
		 */
	    case 11:
		if (opnd1->op_reg == 1)		/* CS */
			goto bad_opnd_1;
		if (opnd1->op_reg < 4)
			size += 1;
		else
			size += 2;
		break;

		/*
		 *	1 operando: 	OP, RM2|ext (para flutuante)
		 */
	    case 12:
		size += 1;
		size += get_opnd_sz (opnd2, 0);
		break;

		/*
		 *	1 operando: 	0xF0, OP|reg1
		 */
	    case 13:
		size += 2;
		break;

		/*
		 *	2 operandos:	OP, offset1
		 *	2 operandos:	OP, offset2
		 */
	    case 15:
	    case 16:
		if (seg16)
			size += 1 + 2;
		else
			size += 1 + 4;
		break;

		/*
		 *	2 operandos:	OP, RM2|reg1
		 */
	    case 17:
		size += 1;
		size += get_opnd_sz (opnd2, 0);
		break;

		/*
		 *	2 operandos:	0x0F, OP, RM2|reg1
		 */
	    case 18:
		size += 2;
		size += get_opnd_sz (opnd2, 0);
		break;

		/*
		 *	2 operandos:	OP, RM1|reg2
		 */
	    case 19:
	    case_19:
		size += 1;
		size += get_opnd_sz (opnd1, 0);
		break;

		/*
		 *	2 operandos:	0x0F, OP, RM1|reg2
		 */
	    case 20:
		size += 2;
		size += get_opnd_sz (opnd1, 0);
		break;

		/*
		 *	2 operandos:	OP|reg2, #IMMED
		 */
	    case 21:
		size += 1;
		size += get_opnd_sz (opnd1, ap->o_opnd1);
		break;

		/*
		 *	2 operandos:	OP, #IMMED1
		 */
	    case 22:
		size += 1;
		size += get_opnd_sz (opnd1, ap->o_opnd1);
		break;

		/*
		 *	2 operandos:	OP, #IMMED2
		 */
	    case 23:
		size += 1;
		size += get_opnd_sz (opnd2, ap->o_opnd2);
		break;

		/*
		 *	2 operandos:	OP, RM2, #IMMED1
		 */
	    case 24:
		size += 1;
		size += get_opnd_sz (opnd2, 0);
		size += get_opnd_sz (opnd1, ap->o_opnd1);
		break;

		/*
		 *	2 operandos:	OP, RM2|ext, #IMMED1
		 */
	    case 25:
		size += 1;
		size += get_opnd_sz (opnd2, 0);
		size += get_opnd_sz (opnd1, ap->o_opnd1);
		break;

		/*
		 *	2 operandos:	0x0F, OP, RM2|ext, #IMMED1
		 */
	    case 26:
		size += 2;
		size += get_opnd_sz (opnd2, 0);
		size += get_opnd_sz (opnd1, ap->o_opnd1);
		break;

		/*
		 *	2 operandos:	OP, #IMMED1, #IMMED2
		 */
	    case 27:
		size += 1;
		size += get_opnd_sz (opnd1, ap->o_opnd1);
		size += get_opnd_sz (opnd2, ap->o_opnd2);
		break;

		/*
		 *	2 operandos (r2, r0): OP
		 */
	    case 28:
		size += 1;
		if (opnd1->op_reg != 2 /* RD */)
		{
		    bad_opnd_1:
			msg (ERRO, OPNDMARK, "Operando inválido", NOSTR, opnd1);
		}
		break;

		/*
		 *	2 operandos (r0, r2): OP
		 */
	    case 29:
		size += 1;
		if (opnd2->op_reg != 2 /* RD */)
		{
		    bad_opnd_2:
			msg (ERRO, OPNDMARK, "Operando inválido", NOSTR, opnd2);
		}
		break;

		/*
		 *	Shifts & rotates: Registro
		 *	2 operandos:	OP, RM2|ext
		 */
	    case 30:
		size += 1;
		if (opnd1->op_reg != 1 /* RC */)
			goto bad_opnd_1;
		size += get_opnd_sz (opnd2, 0);
		break;

		/*
		 *	Shifts & rotates: Imediato
		 *	2 operandos:	OP|0xD0, RM2|ext ou
		 *			OP, RM2|ext, #IMMED1
		 */
	    case 31:
		size += 1;
		if   (opnd1->op_expr.ex_type == ABS)
		{
			size += get_opnd_sz (opnd2, 0);
			if (opnd1->op_expr.ex_value != 1)
				size += 1;
		}
		elif (opnd1->op_expr.ex_type == UNDEF)
		{
			needed_immed_set (opcodeno);
			size += get_opnd_sz (opnd2, 0) + 1;
		}
		else
		{
			goto bad_opnd_1;
		}
		break;

#if (0)	/*************************************/
		size += 1;
		if (opnd1->op_expr.ex_type != ABS)
			goto bad_opnd_1;
		size += get_opnd_sz (opnd2, 0);
		if (opnd1->op_expr.ex_value != 1)
			size += get_opnd_sz (opnd1, ap->o_opnd1);
		break;
#endif	/*************************************/

		/*
		 *	3 operandos:	OP, RM2/reg3, #IMMED1
		 */
	    case 32:
		size += 1;
		if (regular1_opnd3 (GREG32 /* == 16 */, &opnd3) == 0)
		{
			if (opnd2->op_type & MEM)
				goto bad_opnd_2;
		}
		size += get_opnd_sz (opnd2, 0);
		size += get_opnd_sz (opnd1, ap->o_opnd1);
		break;

		/*
		 *	3 operandos:	0x0F, OP, RM3/reg2
		 */
	    case 33:
		size += 2;
		if ((i = regular1_opnd3 (RM32 /* == 16 */, &opnd3)) <= 0)
		{
			if (i == 0)
				msg (ERRO, LONGMARK, "Número errado de operandos");
			break;
		}
		size += get_opnd_sz (&opnd3, 0);
		if (opnd1->op_reg != 1 /* RC */)
			goto bad_opnd_1;
		break;

		/*
		 *	3 operandos:	0x0F, OP, RM3/reg2, #IMMED1
		 */
	    case 34:
		size += 2;
		if ((i = regular1_opnd3 (RM32 /* == 16 */, &opnd3)) <= 0)
		{
			if (i == 0)
				msg (ERRO, LONGMARK, "Número errado de operandos");
			break;
		}
		size += get_opnd_sz (&opnd3, 0);
		size += get_opnd_sz (opnd1, ap->o_opnd1);
		break;

		/*
		 *	Desvios Curtos (deslocamento de 1 byte)
		 */
	    case 50:
		size += 2;
		break;

		/*
		 *	Desvios Médios/longos (deslocamento de 2/4 bytes)
		 */
	    case 52:
		if (seg16)
			size += 2 + 2;
		else
			size += 2 + 4;
		break;

		/*
		 *	Desvios incondicionais médio/longo ("bral", "call")
		 */
	    case 53:
		if (seg16)
			size += 1 + 2;
		else
			size += 1 + 4;
		break;

		/*
		 *	Desvios condicionais SDI 32 bits (tamanhos 2, 6)
		 *	Desvios condicionais SDI 16 bits (tamanhos 2, 4)
		 */
	    case 55:
		if (seg16)
			return (analyse_sdi (&opnd1->op_expr, 2, 4, 4));
		else
			return (analyse_sdi (&opnd1->op_expr, 2, 6, 6));

		/*
		 *	Desvios incondicionais SDI 32 bits (tamanhos 2, 5)
		 *	Desvios incondicionais SDI 16 bits (tamanhos 2, 3)
		 */
	    case 56:
		if (seg16)
			return (analyse_sdi (&opnd1->op_expr, 2, 3, 3));
		else
			return (analyse_sdi (&opnd1->op_expr, 2, 5, 5));

		/*
		 *	Desvios intersegmento médio ("jmpi") expressão
		 *
		 *	OP, EXPR2, #IMMED1
		 */
	    case 58:
		size += 1 + 2 + 2;
		break;

		/*
		 *	Desvios intersegmento médio ("jmpil") expressão
		 *
		 *	OP, EXPR2, #IMMED1
		 */
	    case 59:
		size += 1 + 4 + 2;
		break;

		/*
		 *	Prefixos de segmentos
		 */
	    case 60:
		size += 1;
		break;

		/*
		 *	Instrução CLR	reg  => EOR	reg,reg
		 *	Instrução TST	reg  => TEST	reg,reg
		 */
	    case 61:
	   /***	opnd2 = opnd1; ***/
		goto  case_19;

		/*
		 *	Instrução sintetizada LINK
		 */
	    case 62:
		size += 3;

		i = opnd1->op_expr.ex_value;

#if (0)	/*************************************/
		if   (opnd1->op_expr.ex_type != ABS || (unsigned)i >= 128)
#endif	/*************************************/
		if   (opnd1->op_expr.ex_type != ABS || opnd1->op_type != IMM_8)
		{
			if (seg16)
			{
				size += 2 + 2;

				if ((unsigned)i >= 64 * 1024)
					goto bad_opnd_1;
			}
			else
			{
				size += 2 + 4;
			}
		}
		elif (i != 0)
		{
			size += 3;
		}

		break;

		/*
		 *	Instrução CLR	rm => MOV	#0,rm
		 */
	    case 63:
		size += 1 + get_opnd_sz (opnd1, 0);

		if   (ap->o_flags & LP)
			size += 4;
		elif (ap->o_flags & WP)
			size += 2;
		else
			size += 1;

		break;

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

}	/* end get_code_sz */

/*
 ****************************************************************
 *	Nomes dos registros (para diagnósticos)			*
 ****************************************************************
 */
const char	reg_names[8][16] =
{
	"r0, eax", 	"r1, ecx", 	"r2, edx", 	"r3, ebx",
	"sp, esp", 	"fp, ebp", 	"r4, esi", 	"r5, edi"
};

/*
 ****************************************************************
 *	Calcula o tamanho de um operando			*
 ****************************************************************
 */
int
get_opnd_sz (OPND *opnd, int val)
{
	EXP		*exp;
	long		value;

	exp = &opnd->op_expr;

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
			return (1);

		    case IMMED16:
		    case IMMED16U:
			return (2);

		    case IMMED32:
			return (4);

		    default:
			msg
			(	ERRO, NOMARK,
				"Tipo imprevisto de tamanho de imediato: %06X",
				val
			);
			break;
		}

		break;

		/*
		 *	Os registros inteiros
		 */
	    case RA:
	    case R3:
	    case R4:
	    case HA:
	    case H3:
		return (1);

		/*
		 *	Os registros flutuantes
		 */
	    case F0:
	    case F1:
	    case F7:
		return (1);

		/*
		 *	Indireto: (base)
		 */
	    case INDIR:
		if (seg16)	/* 16 bits */
		{
			switch (opnd->op_reg)
			{
			    case 6:	/* r4 (esi) */
			    case 7:	/* r5 (edi) */
			    case 3:	/* r3 (ebx) */
				break;

			    case 5:	/* fp (ebp) */
			   /***	exp->ex_type = ABS; ***/
			   /***	exp->ex_value = 0; ***/
				goto case_BDISP;

			    default:
			    bad_base:
				msg
				(	ERRO, NOMARK,
					"Registro base (%s) inválido",
					reg_names[opnd->op_reg]
				);
				break;
			}
		}
		else		/* INDIR 32 bits */
		{
			switch (opnd->op_reg)
			{
			    case 4:	/* sp (esp) */
			   /***	opnd->op_reg = 4; ***/	/* sp */
				opnd->op_index = -1;	/* Indexado sem índice */
			   /***	opnd->op_scale = 0; ***/ /* N_BYTE */
				goto case_INDEX;

			    case 5:	/* fp (ebp) */
			   /***	exp->ex_type = ABS; ***/
			   /***	exp->ex_value = 0; ***/
				goto case_BDISP;
			}
		}

		return (1);

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
			    case 7:	/* r5 (edi) */
			    case 5:	/* fp (ebp) */
			    case 3:	/* r3 (ebx) */
				break;

			    default:
				goto bad_base;
			}

			if   (exp->ex_type == UNDEF)
			{
				needed_disp_set (opcodeno);
				return (1 + 2);
			}
			elif (exp->ex_type != ABS)
			{
				return (1 + 2);
			}
			elif ((value = exp->ex_value) >= -128 && value <= 127)
			{
				return (1 + 1);
			}
			elif (value >= -32768 && value <= 32767)
			{
				return (1 + 2);
			}
			else	/* value de 32 bits */
			{
			    bad_offset:
				msg
				(	ERRO, NOMARK,
					"Deslocamento (%d) inválido",
					value
				);
			}
		}
		else		/* BDISP 32 bits */
		{
			switch (opnd->op_reg)
			{
			    case 4:	/* sp (esp) */
			   /***	opnd->op_reg = 4; ***/	/* sp */
				opnd->op_index = -1;	/* Indexado sem índice */
			   /***	opnd->op_scale = 0; ***/ /* N_BYTE */
				goto case_INDEX;
			}

			if   (exp->ex_type == UNDEF)
			{
				needed_disp_set (opcodeno);
				return (1 + 4);
			}
			elif (exp->ex_type != ABS)
			{
				return (1 + 4);
			}
			elif ((value = exp->ex_value) >= -128 && value <= 127)
			{
				return (1 + 1);
			}
			else	/* value de 32 bits */
			{
				return (1 + 4);
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
			    case 3:	/* r3 (ebx) */
				break;

			    default:
				goto bad_base;
			}

			switch (opnd->op_index)
			{
			    case 6:	/* r4 (esi) */
			    case 7:	/* r5 (edi) */
				break;

			    default:
			    bad_index:
				msg
				(	ERRO, NOMARK,
					"Registro índice (%s) inválido",
					reg_names[opnd->op_index]
				);
				break;
			}

			if (opnd->op_scale != 0)	/* != N_BYTE */
			{
				msg
				(	ERRO, NOMARK,
					"Escalonamento do índice inválido"
				);
			}

			if   (exp->ex_type == UNDEF)
			{
				needed_disp_set (opcodeno);
				return (1 + 2);
			}
			elif (exp->ex_type != ABS)
			{
				return (1 + 2);
			}
			elif ((value = exp->ex_value) == 0)
			{
				return (1);
			}
			elif (value >= -128 && value <= 127)
			{
				return (1 + 1);
			}
			elif (value >= -32768 && value <= 32767)
			{
				return (1 + 2);
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
			   /***	opnd->op_index = 4; **/	/* sp */
				break;

			    case 4:	/* sp (esp) */
			   	goto bad_index;
			}

			if   (exp->ex_type == UNDEF)
			{
				needed_disp_set (opcodeno);
				return (2 + 4);
			}
			elif (exp->ex_type != ABS)
			{
				return (2 + 4);
			}
			elif ((value = exp->ex_value) == 0 && opnd->op_reg != 5)/* fp */
			{
				return (2);
			}
			elif (value >= -128 && value <= 127)
			{
				return (2 + 1);
			}
			else	/* value de 32 bits */
			{
				return (2 + 4);
			}
		}

	   /***	break; ***/

		/*
		 *	Indexado sem base: expr(*,index[@scale])
		 */
	    case NOBASE:
		if (seg16)	/* 16 bits */
		{
			msg
			(	ERRO, NOMARK,
				"Supressão da base inválido no modo de 16 bits"
			);
		}
		else		/* NOBASE 32 bits */
		{
			switch (opnd->op_index)
			{
			    case 4:	/* sp (esp) */
			   	goto bad_index;
			}

			return (2 + 4);
		}

		break;

		/*
		 *	Somente uma expressão
		 */
	    case EXPR:
	    case ASTER:
		if (seg16)	/* 16 bits */
		{
			return (1 + 2);
		}
		else		/* 32 bits */
		{
			return (1 + 4);
		}

	    default:
		msg
		(	ERRO, NOMARK,
			"Tipo imprevisto de operando: %06X",
			opnd->op_type
		);
		break;

	}	/* end switch (opnd->op_type) */

	return (0);

}	/* end get_opnd_sz */

/*
 ****************************************************************
 *	Processa um SDI						*
 ****************************************************************
 */
int
analyse_sdi (EXP *exp, int min_sz, int med_sz, int max_sz)
{
	if (section != TEXT)
	{
		msg
		(	ERRO, NOMARK,
			"SDIs só podem ser usadas na seção TEXT"
		);
		return (min_sz);
	}

	if (exp->ex_type == TEXT || exp->ex_type == UNDEF)
	{
		if (exp->ex_ref != NOSYMTB && exp->ex_nopnd == 1)
		{
			put_jmp_sdi
			(	*lc + min_sz,
				exp->ex_ref,
				med_sz - min_sz,
				max_sz - min_sz
			);
			return (min_sz);
		}
	}

	if (exp->ex_type == TEXT && exp->ex_ref == NOSYMTB)
	{
		long		span;

		span = exp->ex_value - *lc;

		if   (span - min_sz >= -128   && span - min_sz <= 127)
			return (min_sz);
		elif (span - med_sz >= -32768 && span - med_sz <= 32767)
			return (med_sz);
		else
			return (max_sz);
	}

	msg
	(	ERRO, EXPMARK,
		"Operando inválido para SDIs",
		NOSTR, exp
	);

	return (min_sz);

}	/* end analyse_sdi */

/*
 ****************************************************************
 *	Liga o membro do vetor de imediatos indefinidos		*
 ****************************************************************
 */
int
needed_immed_set (int op_code_no)
{
	int		old_bytes, bytes;

	if (op_code_no >= needed_immed_in_pass1_codes)
	{
		old_bytes = needed_immed_in_pass1_codes >> 3;

		bytes = ((op_code_no >> 3) & ~NEEDED_MASK) + NEEDED_SZ;

		if ((needed_immed_in_pass1_vec = realloc (needed_immed_in_pass1_vec, bytes)) == NOSTR)
			error (NOSTR);

		memsetl (needed_immed_in_pass1_vec + old_bytes, 0, (bytes - old_bytes) >> 2);

		needed_immed_in_pass1_codes = (bytes << 3);
	}

	return (needed_immed_in_pass1_vec[op_code_no >> 3] |= (0x80 >> (op_code_no & 7)));

}	/* end needed_immed_set */

/*
 ****************************************************************
 *	Testa o membro do vetor de imediatos indefinidos	*
 ****************************************************************
 */
int
needed_immed_tst (int op_code_no)
{
	if (op_code_no >= needed_immed_in_pass1_codes)
		return (0);

	return (needed_immed_in_pass1_vec[op_code_no >> 3] & (0x80 >> (op_code_no & 7)));

}	/* end needed_immed_set */

/*
 ****************************************************************
 *	Liga o membro do vetor de deslocamentos indefinidos	*
 ****************************************************************
 */
int
needed_disp_set (int op_code_no)
{
	int		old_bytes, bytes;

	if (op_code_no >= needed_disp_in_pass1_codes)
	{
		old_bytes = needed_disp_in_pass1_codes >> 3;

		bytes = ((op_code_no >> 3) & ~NEEDED_MASK) + NEEDED_SZ;

		if ((needed_disp_in_pass1_vec = realloc (needed_disp_in_pass1_vec, bytes)) == NOSTR)
			error (NOSTR);

		memsetl (needed_disp_in_pass1_vec + old_bytes, 0, (bytes - old_bytes) >> 2);

		needed_disp_in_pass1_codes = (bytes << 3);
	}

	return (needed_disp_in_pass1_vec[op_code_no >> 3] |= (0x80 >> (op_code_no & 7)));

}	/* end needed_disp_set */

/*
 ****************************************************************
 *	Testa o membro do vetor de imediatos indefinidos	*
 ****************************************************************
 */
int
needed_disp_tst (int op_code_no)
{
	if (op_code_no >= needed_disp_in_pass1_codes)
		return (0);

	return (needed_disp_in_pass1_vec[op_code_no >> 3] & (0x80 >> (op_code_no & 7)));

}	/* end needed_disp_set */
