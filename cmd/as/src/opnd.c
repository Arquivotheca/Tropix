/*
 ****************************************************************
 *								*
 *			opnd.c					*
 *								*
 *	Análise de operandos					*
 *								*
 *	Versão	3.0.0, de 13.09.94				*
 *		4.1.0, de 24.08.01				*
 *								*
 *	Rotinas:						*
 *		getoperand	getindex	reglist		*
 *								*
 *	Módulo: AS-80386/486/PENTIUM				*
 *		Tradutores					*
 *		Categoria D					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2001 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include <sys/types.h>

#include <stdio.h>
#include <a.out.h>

#include "../h/common.h"
#include "../h/as.h"
#include "../h/tokens.h"
#include "../h/scan.h"
#include "../h/lex.h"
#include "../h/id_sym_tb.h"
#include "../h/expr.h"
#include "../h/opnd.h"
#include "../h/msg.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Definições globais					*
 ****************************************************************
 */
#define	RETURN(x)	return (opnd->op_type = (x))

/*
 ****************************************************************
 *	Extrai um operando					*
 ****************************************************************
 */
long
getoperand (OPND *opnd)
{
	const SYMTB	*sp;
	SYMTB		**link_place;
	MAJOR		major;
	long		size;
	EXP		*ep;
	char		mod_seen = 0, mul_seen = 0;

	opnd->op_begin	= token->l_begin;
	major		= token->l_major;

	/*
	 *	#expr
	 */
	if (major == NUMBER)
	{
		ep = &opnd->op_expr;

		gettoken ();

		expression (ep);
		opnd->op_end = ep->ex_end;

		if   (ep->ex_type == ERR)
		{
			RETURN (INVAL);
		}
		elif (ep->ex_type == REAL)
		{
			/*
			 *	Como os campos op_immed e op_rimmed
			 *	pertencem à mesma "union", não
			 *	precisamos fazer a atribuição
			 */
			RETURN (FIMMED);
		}
		elif (ep->ex_type != ABS)	/* Repare que Inclui UNDEF */
		{
			if (seg16)
				RETURN (IMM_16);
			else
				RETURN (IMM_32);
		}
		elif ((size = ep->ex_value) < -32768 || size > 65535)
		{
			RETURN (IMM_32);
		}
		elif (size > 32767)
		{
			RETURN (IMM_16U);
		}
		elif (size < -128 || size > 255)
		{
			RETURN (IMM_16);
		}
		elif (size > 127)
		{
			RETURN (IMM_8U);
		}
		else
		{
			RETURN (IMM_8);
		}

	}	/* end if (major == NUMBER) */

	/*
	 *	%reg
	 */
	if (major == OP && token->l_minor == N_MOD)
	{
		if (CATEG (token->l_begin + 1) == LETTER)
		{
			gettoken (); 	 		token->l_begin--;
			major = token->l_major;		mod_seen++;
		}
	}

	/*
	 *	reg
	 */
	if (major == ID && (sp = id_hash (token->l_name, token->l_len, &link_place)) != NOSYMTB)
	{
		opnd->op_end = token->l_end;

		if (sp->s.s_type == REG)
		{
			opnd->op_reg = reg_value_vec[sp->s.s_value];

			switch (sp->s.s_value)
			{
				/*
				 *	ra
				 */
			    case 0:
				gettoken ();
				RETURN (RA);

				/*
				 *	ha
				 */
			    case 8:
				gettoken ();
				RETURN (HA);

				/*
				 *	rb, rc, rd
				 */
			    case 1:
			    case 2:
			    case 3:
				gettoken ();
				RETURN (R3);

				/*
				 *	hb, hc, hd
				 */
			    case 9:
			    case 10:
			    case 11:
				gettoken ();
				RETURN (H3);

				/*
				 *	rsi, rdi, fp e sp
				 */
			    case 4:
			    case 5:
			    case 6:
			    case 7:
				gettoken ();
				RETURN (R4);

				/*
				 *	f0
				 */
			    case 12:
				gettoken ();
				RETURN (F0);

				/*
				 *	f1
				 */
			    case 13:
				gettoken ();
				RETURN (F1);

				/*
				 *	f2 a f7
				 */
			    case 14:
			    case 15:
			    case 16:
			    case 17:
			    case 18:
			    case 19:
				gettoken ();
				RETURN (F7);

				/*
				 *	cr0, cr2 e cr3
				 */
			    case 20:
			    case 21:
			    case 22:
				gettoken ();
				RETURN (CREG);

				/*
				 *	dr0-3, dr6, dr7
				 */
			    case 23:
			    case 24:
			    case 25:
			    case 26:
			    case 27:
			    case 28:
				gettoken ();
				RETURN (DREG);

				/*
				 *	tr6, tr7
				 */
			    case 29:
			    case 30:
				gettoken ();
				RETURN (TREG);

				/*
				 *	Segmentação
				 */
			    case 31:
			    case 32:
			    case 33:
			    case 34:
			    case 35:
			    case 36:
				gettoken ();
				RETURN (SREG);

			    default:
				msg
				(	COMP, LONGMARK,
					"Registrador desconhecido: %d",
					sp->s.s_value
				);

			}	/* end switch (reg) */

		}	/* end (sp->s.s_type == REG) */

	}	/* end começando por REG */

	/*
	 *	Verifica o que veio depois do "%"
	 */
	if (mod_seen)
	{
		msg (ERRO, LONGMARK, "Registro inválido");
		RETURN (INVAL);
	}

	/*
	 *	(base), (base, index) ou (base, index@scale)
	 */
	if (major == LP)
	{
		opnd->op_expr.ex_type = ABS;
		opnd->op_expr.ex_value = 0;

		RETURN (get_base_index (opnd));
	}

	/*
	 *	Verifica se começa por "*"
	 */
	if (major == OP && token->l_minor == N_MUL)
	{
		gettoken (); 	 	   /***	token->l_begin--; ***/
	   /***	major = token->l_major; ***/	mul_seen++;
	}

	/*
	 *	O operando comeca com uma expressão.
	 */
	ep = &opnd->op_expr;

	expression (ep);

	opnd->op_end = ep->ex_end;

	if (ep->ex_type == ERR)
		RETURN (INVAL);

	if (ep->ex_type == REAL)
	{
		msg
		(	ERRO, EXPMARK,
			"Expressão real não permitida neste caso", 0,
			ep
		);

		RETURN (INVAL);
	}

	/*
	 *	Analisa o que vem após a expressão
	 */
	if (token->l_major == LP)
	{
		/* expr(base), expr(base,index) ou expr(base,index@scale) */

		if (!mul_seen)
			RETURN (get_base_index (opnd));

		msg (ERRO, LONGMARK, "Sintaxe não permitida juntamente com \"*\"");
	   /***	opnd->op_end = opnd->op_begin; ***/
		RETURN (INVAL);
	}
	elif (mul_seen)
	{
		RETURN (ASTER);	/*	*expr	*/
	}
	else
	{
		RETURN (EXPR);	/*	expr	*/
	}

}	/* end getoperand */

/*
 ****************************************************************
 *	Analisa um operando do tipo base-indice			*
 ****************************************************************
 */
ulong
get_base_index (OPND *opnd)
{
	MAJOR		major;
	SYMTB		*sp, **link_place;
	int		nobase = 0;

	/* Acabou de ver um LP */

	/*
	 *	%reg ou omissão da base
	 */
	if   ((major = gettoken ()) == OP)
	{
		if   (token->l_minor == N_MOD)
		{
			if (CATEG (token->l_begin + 1) == LETTER)
			{
				gettoken ();
				token->l_begin--;
				major = token->l_major;
			}
		}
		elif (token->l_minor == N_MUL)
		{
			nobase++;
			gettoken ();
			goto get_index;
		}
	}
	elif (major == CM)
	{
		nobase++;
		goto get_index;
	}

	if (major != ID)
	{
	    opmsg:
		msg (ERRO, LONGMARK, "Erro em operando");
		return (INVAL);
	}

	/*
	 *	expr(reg) ou (reg)
	 */
	sp = id_hash (token->l_name, token->l_len, &link_place);

	if (sp == NOSYMTB || !ISAREG (sp))
		goto opmsg;

	if ((unsigned)sp->s.s_value >= 8)
		goto regmsg;

	opnd->op_reg = reg_value_vec[sp->s.s_value];

	if (gettoken () == RP)
	{
		opnd->op_end = token->l_end; 	gettoken ();

#if (0)	/*************************************/
		if   (sp->s.s_value == 4)		/* sp */
		{
		   /***	opnd->op_reg = ... (sp) ***/
			opnd->op_index = opnd->op_reg; /* == (sp) => none */
			opnd->op_scale = 0;	/* N_BYTE */
			return (INDEX);
		}
		elif (sp->s.s_value == 5)		/* fp */
		{
			return (BDISP);		/*  expr(reg) */
		}
#endif	/*************************************/

		if (opnd->op_expr.ex_type == ABS && opnd->op_expr.ex_value == 0)
			return (INDIR);		/*  (reg) */
		else
			return (BDISP);		/*  expr(reg) */
	}

	/*
	 *	expr(reg,index[@scale]) ou (reg,index[@scale])
	 */
    get_index:
	if (token->l_major != CM)
		goto opmsg;

	if ((major = gettoken ()) == OP && token->l_minor == N_MOD)
	{
		if (CATEG (token->l_begin + 1) == LETTER)
			{ gettoken (); token->l_begin--; major = token->l_major; }
	}

	if (major != ID)
		goto opmsg;

	sp = id_hash (token->l_name, token->l_len, &link_place);

	if (sp == NOSYMTB || !ISAREG (sp))
		goto opmsg;

	if ((unsigned)sp->s.s_value >= 8)
		goto regmsg;

	opnd->op_index = reg_value_vec[sp->s.s_value];

	if (gettoken () == SCALE)
	{
		opnd->op_scale = token->l_minor - N_BYTE;
		gettoken ();
	}
	else
	{
		opnd->op_scale = 0;	/* BYTE */
	}

	if (token->l_major != RP)
		goto opmsg;

	opnd->op_end = token->l_end; 	gettoken ();

	/*
	 *	Transforma
	 *
	 *		offset(*,reg@b)
	 *	em 
	 *		offset(reg)
	 */
	if (nobase)
	{
		if (opnd->op_scale != 0)	/* != BYTE */
			return (NOBASE);

		opnd->op_reg = opnd->op_index;

		if (opnd->op_expr.ex_type == ABS && opnd->op_expr.ex_value == 0)
			return (INDIR);		/*  (reg) */
		else
			return (BDISP);		/*  expr(reg) */
	}
	else
	{
		return (INDEX);
	}

	/*
	 *	Registro inválido
	 */
    regmsg:
	msg (ERRO, LONGMARK, "Registro não permitido neste endereçamento");
	return (INVAL);

}	/* end get_base_index */
