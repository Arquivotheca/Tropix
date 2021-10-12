/*
 ****************************************************************
 *								*
 *			expr.c					*
 *								*
 *	Analise e avaliação de expressões aritméticas		*
 *								*
 *	Versão	1.0.0, de 26.02.86				*
 *		4.1.0, de 24.08.01				*
 *								*
 *	Rotinas:						*
 *		expression	operate				*
 *								*
 *	Módulo: AS-80386/486/PENTIUM				*
 *		Tradutores					*
 *		Categoria D					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2001 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>

#include <stdio.h>
#include <a.out.h>

#include "../h/common.h"
#include "../h/tokens.h"
#include "../h/scan.h"
#include "../h/msg.h"
#include "../h/id_sym_tb.h"
#include "../h/expr.h"
#include "../h/as.h"
#include "../h/proto.h"

/*
 ******	Protótipos de funções ***********************************
 */
void		operate (ESTACK *, EXP *);

/*
 ****************************************************************
 *	Tabela de prioridades (baseada em "C")			*
 ****************************************************************
 */
static	ESTACK	stack[ESTACKSZ];	/* Stack para Avaliação da Expressão */

#define	INC		15		/* Incremento da Prioridade */
#define	UPREC		12		/* Prioridade de unários */

static const char	prec[]	=
{
	3,		/* N_OR,	|	*/
	4,		/* N_EOR,	^	*/
	5,		/* N_AND,	&	*/
	8,		/* N_LSHIFT,	<<	*/
	8,		/* N_RSHIFT,	>>	*/
	9,		/* N_PLUS,	+	*/
	9,		/* N_MINUS,	-	*/
	10,		/* N_MUL,	*	*/
	10,		/* N_DIV,	/	*/
	10,		/* N_MOD,	%	*/
	UPREC,		/* N_NOT,	~	*/

	0,		/* N_BYTE,	:b ou @b */
	0,		/* N_WORD,	:w ou @w */
	0,		/* N_LONG,	:l ou @l */
	0,		/* N_QUAD,	@q	*/

	6,		/* N_EQ,	==	*/
	6,		/* N_NE,	!=	*/
	7,		/* N_GT,	>	*/
	7,		/* N_GE,	>=	*/
	7,		/* N_LT,	<	*/
	7,		/* N_LE,	<=	*/

	1,		/* N_OROR,	||	*/
	2,		/* N_ANDAND,	&&	*/
	UPREC		/* N_NOTNOT	!	*/
};

static	int	incp;			/* Incremento da Precedencia */

/*
 ****************************************************************
 *	Avalia uma Expressão					*
 ****************************************************************
 */
long
expression (EXP *exp)
{
	ESTACK		*top;
	int		n, syntax;
	MINOR		minor;
	SYMTB		*sp;
	TOKEN		*tp;

	syntax        = E_LC|E_OPND|E_UNOPR;
	incp          = 0;

	top           = &stack[0];
	top->e_opr    = N_NULL;
	top->e_prec   = 0;

	exp->ex_nopnd = 0;
	exp->ex_begin = exp->ex_end = token->l_begin;
	exp->ex_type  = UNDEF;

	for (EVER)
	{
		tp = token;

		switch (tp->l_major)
		{
			/*
			 *	Constante ou símbolo: Empilha o valor
			 */
		    case CTE:
			if ((syntax & E_OPND) == 0)
				goto smsg;

			syntax = E_END|E_RC|E_OPR;

			top->e_value	= tp->l_ival;
			top->e_type	= ABS;
			top->e_ref	= NOSYMTB;

			exp->ex_nopnd++;

			break;

		    case LID:
			if ((syntax & E_OPND) == 0)
				goto smsg;

			syntax = E_END|E_RC|E_OPR;

			sp = putsym (tp->l_name, tp->l_len, S_SEARCH);

			if (pass == 2 && sp->s.s_type == UNDEF)
			{
				msg
				(	ERRO, LONGMARK,
					"Símbolo local indefinido"
				);
			}

			sp->s.s_flags |= S_LOCAL;

			top->e_value	= sp->s.s_value;
			top->e_type	= sp->s.s_type;
			top->e_ref	= sp;

			exp->ex_nopnd++;

			break;

		    case LOCCNT:
			if ((syntax & E_OPND) == 0)
				goto smsg;

			syntax = E_END|E_RC|E_OPR;

			top->e_value	= *lc;
			top->e_type	= section;
			top->e_ref	= NOSYMTB;

			exp->ex_nopnd++;

			break;
			
		    case ID:
			if ((syntax & E_OPND) == 0)
				goto smsg;

			syntax = E_END|E_RC|E_OPR;

			sp = putsym (tp->l_name, tp->l_len, S_SEARCH);

			switch (sp->s.s_type)
			{
			    case ABS:
				top->e_value	= sp->s.s_value;
				top->e_type	= ABS;
				top->e_ref	= sp;
				break;

			    case UNDEF:
				top->e_value	= 0;
				top->e_type	= UNDEF;
				top->e_ref	= sp;
				break;

			    case REAL:
				if (top->e_opr == N_MINUS)
					exp->ex_rval = -sp->s_rval;
				else
					exp->ex_rval = sp->s_rval;

				exp->ex_type	= REAL;

				top->e_value	= 0;
				top->e_type	= ABS;
				top->e_ref	= sp;
				break;

			    default:
				top->e_value	= sp->s.s_value;
				top->e_type	= sp->s.s_type;
				top->e_ref	= sp;
				break;
			}

			exp->ex_nopnd++;
			sp->s.s_flags |= S_REF;
			break;

		    case RCTE:
			if ((syntax & E_OPND) == 0)
				goto smsg;

			syntax = E_END|E_RC|E_OPR;

			top->e_value	= 0;
			top->e_type	= ABS;
			top->e_ref	= NOSYMTB;

			exp->ex_nopnd++;
			exp->ex_type = REAL;

			if (top->e_opr == N_MINUS)
				exp->ex_rval = -token->l_rval;
			else
				exp->ex_rval = token->l_rval;

			break;

			/*
			 *	Incrementa a Prioridade de INC
			 */
		    case LC:
		    case LB:
			if ((syntax & E_LC) == 0)
				goto smsg;

			syntax = E_LC|E_OPND|E_UNOPR;
			incp += INC;
			break;

			/*
			 *	Decrementa a Prioridade de INC
			 */
		    case RC:
		    case RB:
			if ((syntax & E_RC) == 0)
				goto smsg;

			if ((incp -= INC) < 0)
				{ syntax = 0; goto smsg; }

			syntax = E_END|E_RC|E_OPR;
			break;

			/*
			 *	Operador: Se tiver menor prioridade que o
			 *	operador do topo da pilha, realiza as  o-
			 *	perações pendentes. No final, empilha   o
			 *	operador corrente.
			 *	Ocorrendo erro de sintaxe, verifica se não
			 *	pode ser considerado operador unario
			 */
		    case OP:
			minor = tp->l_minor;

			if ((syntax & E_OPR) == 0)
			{
				if ((syntax & E_UNOPR) == 0)
					goto smsg;

				switch (minor)
				{
				    case N_MINUS:
					top->e_value	= 0;
					top->e_type	= ABS;
					top->e_ref	= NOSYMTB;

					top++;

					top->e_opr	= N_MINUS;
					top->e_prec	= UPREC + incp;
					break;

				    case N_PLUS:
					break;

				    case N_NOT:		/* ~ */
					top->e_value	= 0;	/* Não usado */
					top->e_type	= ABS;
					top->e_ref	= NOSYMTB;

					top++;

					top->e_opr	= N_NOT;
					top->e_prec	= UPREC + incp;
					break;

				    case N_NOTNOT:	/* ! */
					top->e_value	= 0;	/* Não usado */
					top->e_type	= ABS;
					top->e_ref	= NOSYMTB;

					top++;

					top->e_opr	= N_NOTNOT;
					top->e_prec	= UPREC + incp;
					break;

#if (0)	/*************************************/
				    case N_NOT:
					top->e_value	= ~NULL;
					top->e_type	= ABS;
					top->e_ref	= NOSYMTB;

					top++;

					top->e_opr	= N_EOR;
					top->e_prec	= UPREC + incp;
					break;
#endif	/*************************************/

				    default:
					goto smsg;

				}	/* end switch */
			}
			else
			{
				n = prec[minor] + incp;

				while (n <= top->e_prec)
					{ operate (top, exp); top--; }

				top++;

				top->e_opr	= minor;
				top->e_prec	= n;
			}

			syntax = E_LC|E_OPND|E_UNOPR;
			break;

			/*
			 *	Fim de Expressão: Testa  Balanceamento   de
			 *	Parenteses e realiza as operações pendentes
			 */
		    default:
			if ((syntax & E_END) == 0)
				goto smsg;

			if (incp != 0)
				{ syntax = 0; goto smsg; }

			if (exp->ex_type == REAL)
			{
				if (exp->ex_nopnd != 1)
				{
					msg
					(	ERRO, EXPMARK,
						"Expressão envolvendo reais só"
						" pode ter um único operando",
						0, exp
					);

					exp->ex_type = ERR;
				}
				return (0);
			}

			while (top->e_opr != N_NULL)
				{ operate (top, exp); top--; }

			exp->ex_type = top->e_type;

#if (0)	/*************************************/
			if ((exp->ex_type = top->e_type) == ERR)
			{
				msg
				(	ERRO, EXPMARK,
					"Erro em expressão",
					NOSTR, exp
				);
			}
#endif	/*************************************/

			exp->ex_ref = top->e_ref;

			return (exp->ex_value = top->e_value);

		}	/* end switch */

		/*
		 *	Testa estouro da pilha.
		 */
		if (top >= &stack[ESTACKSZ])
		{
			msg
			(	COMP, NOMARK,
				"BUM!!! - Estouro na Pilha de Operações"
			);
		}

		/*
		 *	Le o Proximo Token
		 */
		exp->ex_end = tp->l_end; 	gettoken ();

	}	/* end for (EVER) */

	/*
	 *	Escreve a Mensagem de Erro
	 */
    smsg:
	switch (syntax)
	{
#if (0)	/*************************************/
	    case E_LC|E_OPND:
		msg (ERRO, SHORTMARK, "Esperava operando, \"[\" ou \"{\"");
		break;
#endif	/*************************************/

	    case E_LC|E_OPND|E_UNOPR:
		msg
		(	ERRO, SHORTMARK,
			"Esperava operando, \"[\", \"{\" ou operador unário"
		);
		break;

	    case E_END|E_RC|E_OPR:
		msg
		(	ERRO, SHORTMARK,
			"Esperava operador, \"]\", \"}\" ou fim de expressão"
		);
		break;

	    default:
		msg (ERRO, SHORTMARK, "Número de  \"[\" (ou \"{\") não confere com o de \"]\" (ou \"}\")");
	}

	exp->ex_type = ERR; 	return (exp->ex_value = 0);

}	/* end expression */

/*
 ****************************************************************
 *	Realiza a Operação do Topo da Stack			*
 ****************************************************************
 */
void
operate (ESTACK *top, EXP *exp)
{
	long		v1, v2;
	char		t1, t2;
	char		*op_str = "";

	/*
	 *	Prepara os valores para a operação
	 */
	v2 = top->e_value; 	t2 = top->e_type; 	top--;
	v1 = top->e_value; 	t1 = top->e_type;

	/*
	 *	Se já houve erro, ...
	 */
	if (t1 == ERR || t2 == ERR)
		{ top->e_type = ERR; top->e_value = 0; return; }

	/*
	 *	No passo 1 podemos ter expressões indefinidas
	 */
	if (pass == 1 && (t1 == UNDEF || t2 == UNDEF))
		{ top->e_type = UNDEF; top->e_value = 0; return; }

	/*
	 *	Executa a operação (lista em prioridade crescente)
	 */
	switch ((top + 1)->e_opr)
	{
	    case N_OROR:
		if (t1 != ABS || t2 != ABS)
			{ op_str = "||"; goto err_need_abs; }

		top->e_value = (v1 || v2);	return;

	    case N_ANDAND:
		if (t1 != ABS || t2 != ABS)
			{ op_str = "&&"; goto err_need_abs; }

		top->e_value = (v1 && v2);	return;

	    case N_OR:
		if (t1 == ABS && t2 == ABS)
			{ top->e_value = v1 | v2; return; }

		op_str = "|";	goto err_need_abs;

	    case N_EOR:
		if (t1 == ABS && t2 == ABS)
			{ top->e_value = v1 ^ v2; return; }

		op_str = "^";	goto err_need_abs;

	    case N_AND:
		if (t1 == ABS && t2 == ABS)
			{ top->e_value = v1 & v2; return; }

		op_str = "&";	goto err_need_abs;

	    case N_EQ:
		if (t1 != ABS || t2 != ABS)
			{ op_str = "=="; goto err_need_abs; }

		top->e_value = (v1 == v2);	return;

	    case N_NE:
		if (t1 != ABS || t2 != ABS)
			{ op_str = "!="; goto err_need_abs; }

		top->e_value = (v1 != v2);	return;

	    case N_GT:
		if (t1 != ABS || t2 != ABS)
			{ op_str = ">"; goto err_need_abs; }

		top->e_value = (v1 > v2);	return;

	    case N_GE:
		if (t1 != ABS || t2 != ABS)
			{ op_str = ">="; goto err_need_abs; }

		top->e_value = (v1 >= v2);	return;

	    case N_LT:
		if (t1 != ABS || t2 != ABS)
			{ op_str = "<"; goto err_need_abs; }

		top->e_value = (v1 < v2);	return;

	    case N_LE:
		if (t1 != ABS || t2 != ABS)
			{ op_str = "<="; goto err_need_abs; }

		top->e_value = (v1 <= v2);	return;

	    case N_LSHIFT:
		if (t1 == ABS && t2 == ABS)
			{ top->e_value = (unsigned)v1 << v2; return; }

		op_str = "<<";	goto err_need_abs;

	    case N_RSHIFT:
		if (t1 == ABS && t2 == ABS)
			{ top->e_value = (unsigned)v1 >> v2; return; }

		op_str = ">>";	goto err_need_abs;

	    case N_PLUS:
		if (t1 == REG || t2 == REG || t1 != ABS && t2 != ABS)
			{ op_str = "+"; goto err_inval; }

		if (t2 != ABS)
			{ top->e_type = t2; top->e_ref = (top+1)->e_ref; }

		top->e_value = v1 + v2;		return;

	    case N_MINUS:
		if (t1 == REG || t2 == REG || t2 == UNDEF || t1 == ABS && t2 != ABS)
			{ op_str = "-"; goto err_inval; }

		if (t2 == ABS)
			{ top->e_value = v1 - v2; return; }

		if (t1 == t2)
			{ top->e_type = ABS; top->e_value = v1 - v2; return; }

		op_str = "-";	goto err_inval;

	    case N_MUL:
		if (t1 == ABS && t2 == ABS)
			{ top->e_value = v1 * v2; return; }

		op_str = "*";	goto err_need_abs;

	    case N_DIV:
		if (t1 != ABS || t2 != ABS)
			{ op_str = "/"; goto err_need_abs; }

		if (v2 == 0)
		{
			msg
			(	ERRO, EXPMARK,
				"Divisão por ZERO",
				NOSTR, exp
			);
		}

		top->e_value = v1 / v2;		return;

	    case N_MOD:
		if (t1 != ABS || t2 != ABS)
			{ op_str = "%"; goto err_need_abs; }

		if (v2 == 0)
		{
			msg
			(	ERRO, EXPMARK,
				"Módulo de ZERO",
				NOSTR, exp
			);
		}

		top->e_value = v1 % v2;		return;

	    case N_NOT:		/* ~ Unário */
		if (t2 != ABS)
			{ op_str = "~"; goto err_need_abs; }

		top->e_value = ~v2;		return;

	    case N_NOTNOT:	/* ! Unário */
		if (t2 != ABS)
			{ op_str = "!"; goto err_need_abs; }

		top->e_value = !v2;		return;

	    default:
		msg
		(	ERRO, EXPMARK,
			"*** COMP: Operador desconhecido (minor %d)",
			(top + 1)->e_opr, exp
		);

	}	/* end switch */

	/*
	 *	Emite mensagem de erro
	 */
    err_need_abs:
	if (t1 == UNDEF || t2 == UNDEF)
	{
		msg
		(	ERRO, EXPMARK,
			"Operando(s) indefinido(s) ainda no 2. passo "
			"(operador \"%s\")",
			op_str, exp
		);
	}
	else
	{
		msg
		(	ERRO, EXPMARK,
			"O operador \"%s\" necessita de 2 operandos absolutos",
			op_str, exp
		);
	}

	top->e_type = ERR;	top->e_value = 0;	return;

    err_inval:
	msg
	(	ERRO, EXPMARK,
		"Operando(s) inválido(s) para o operador \"%s\"",
		op_str, exp
	);

	top->e_type = ERR;	top->e_value = 0;	return;

}	/* end operate */
