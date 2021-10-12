/*
 ****************************************************************
 *								*
 *			expr.c					*
 *								*
 *	Análise e avaliação de expressões aritméticas		*
 *								*
 *	Versão	1.0.0, de 19.09.88				*
 *		4.1.0, de 23.08.01				*
 *								*
 *	Módulo: CPP						*
 *		Tradutores					*
 *		Categoria D					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2001 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <stdio.h>

#include "../h/scan.h"
#include "../h/error.h"
#include "../h/symtb.h"
#include "../h/cpp.h"

/*
 ****************************************************************
 *	Constantes e definições globais				*
 ****************************************************************
 */
#define	STACKSZ		20	/* Profudidade Máxima da Pilha */
#define	INC		20	/* Incremento da Prioridade */

extern int	definedstate;	/* Estado do defined */

typedef	struct
{
	long	e_opnd;			/* Operando */
	MINOR	e_opr;			/* Operador */
	int	e_prec;			/* Precedência do Operador */

}	STACK;

/*
 ****************************************************************
 *	Máscara Sintática					*
 ****************************************************************
 */
#define	E_END		0x01		/* Fim da Expressão */
#define	E_LP		0x02		/* '(' */
#define	E_RP		0x04		/* ')' */
#define	E_OPND		0x08		/* Operando */
#define	E_OPR		0x10		/* Operador */
#define	E_UNOPR		0x20		/* Operador Unário */

/*
 ****************************************************************
 *	Variáveis & Definições Locais				*
 ****************************************************************
 */
#define	UNARY	11		/* Operadores Unários */

static	STACK	stack[STACKSZ],	/* Pilha para Avaliação da Expressão */
		*top;		/* Topo da Pilha */

static	char	prec[]	=
{
	1,				/* || N_OROR */
	2,				/* && N_ANDAND */
	3,				/* |  N_OR */
	4,				/* ^  N_XOR */
	5,				/* &  N_AND */
	6,				/* == N_EQ */
	6,				/* != N_NE */
	7,				/* <  N_LT */
	7,				/* <= N_LE */
	7,				/* >  N_GT */
	7,				/* >= N_GE */
	8,				/* >> N_RIGHT */
	8,				/* << N_LEFT */
	9,				/* +  N_PLUS */
	9,				/* -  N_MINUS */
	10,				/* *  N_MUL */
	10,				/* /  N_DIV */
	10,				/* %  N_MOD */
	11,				/* ~  N_NOT */
	11				/* !  N_NEG */

};					/* Vetor de Precedências */

static	int	incp;			/* Incremento da Precedência */

/*
 ****************************************************************
 *	Avalia uma Expressão					*
 ****************************************************************
 */
int
expression (void)
{
	int		n, syntax;
	TOKEN		*tp;
	const char	*expbegin;
	MINOR		minor = 0;
	char		area[MACSZ];
	const char	*nextpsave;
	const char	*lcp = NOSTR, *lep;

	syntax        = E_LP|E_OPND|E_UNOPR;
	incp          = 0;
	top           = stack;
	top->e_opr    = N_NULL;
	top->e_prec   = 0;

	/*
	 *	Expande a linha
	 */
	definedstate = 1;

	expbegin = expand (nextp, area);

	definedstate = 0;

	if (dflag)
		printexpline ("Expression: ", expbegin);

	nextpsave = nextp;

	tp = &token;

	/*
	 *	Trata os casos especiais:
	 *
	 *		"str" == "str"
	 *		"str" != "str"
	 *
	 */
	nextp = expbegin;
	eof_state = 0;

	if
	(
		scan () == STR &&
		(lcp = token.l_begin), (lep = token.l_end), scan () == OP &&
		((minor = token.l_minor) == N_EQ || minor == N_NE) &&
		scan () == STR
	)
	{
		n =  maccmp (lcp, lep, minor);

		nextp = nextpsave;
		lineno++;

		return (n);
	}

	nextp = expbegin;

	scan ();

	for (EVER)
	{
		switch (tp->l_major)
		{
			/*
			 *	Constante: Empilha o valor.
			 */
		    case ICTE:
			if ((syntax & E_OPND) == 0)
				goto serror;

			syntax = E_END|E_RP|E_OPR;

			top->e_opnd = tp->l_ival;

			break;

			/*
			 *	Identificador: será considerado == 0
			 */
		    case ID:
			if (Tflag) err_msg
			(	ADVR, TOKMARK,
				"Macro não definida; "
				"será considerada igual a zero"
			);

			if ((syntax & E_OPND) == 0)
				goto serror;

			syntax = E_END|E_RP|E_OPR;

			top->e_opnd = 0;

			break;

			/*
			 *	Incrementa a Prioridade de INC
			 */
		    case LP:
			if ((syntax & E_LP) == 0)
				goto serror;

			syntax = E_LP|E_OPND|E_UNOPR;
			incp += INC;
			break;

			/*
			 *	Decrementa a Prioridade de INC
			 */
		    case RP:
			if ((syntax & E_RP) == 0)
				goto serror;

			if ((incp -= INC) < 0)
			{
				syntax = 0;
				goto serror;
			}

			syntax = E_END|E_RP|E_OPR;
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
				switch (minor)
				{
				    case N_MINUS:
					top->e_opnd = 0;
					top++;
					top->e_opr = N_MINUS;
					top->e_prec = UNARY + incp;
					break;

				    case N_PLUS:
					break;

				    case N_NOT:
					top->e_opnd = ~NULL;
					top++;
					top->e_opr = N_XOR;
					top->e_prec = UNARY + incp;
					break;

				    case N_NEG:
					top->e_opnd = 0;
					top++;
					top->e_opr = N_EQ;
					top->e_prec = UNARY + incp;
					break;

				    default:
					goto serror;

				}	/* end switch */

				syntax = E_LP|E_OPND;
			}
			else
			{
				syntax = E_LP|E_OPND;
				n = prec[minor] + incp;
				while (n <= top->e_prec)
					operate ();
				top++;
				top->e_opr = minor;
				top->e_prec = n;
			}
			break;

			/*
			 *	Fim de Expressão: Testa  Balanceamento   de
			 *	Parentêses e realiza as operações pendentes
			 */
		    case EOL:
			if ((syntax & E_END) == 0)
				goto serror;

			if (incp != 0)
			{
				syntax = 0;
				goto serror;
			}

			while (top->e_opr != N_NULL)
				operate ();

			nextp = nextpsave;
			lineno++;

			return (top->e_opnd);

			/*
			 *	Token inválido
			 */
		    default:
			err_msg
			(	ERRO, TOKMARK,
				"Operador/operando inválido"
			);

			nextp = nextpsave;
			lineno++;

			return (0);

		}	/* end switch */

		/*
		 *	Testa Estouro da Pilha.
		 */
		if (top >= &stack[STACKSZ])
		{
			err_msg
			(
				COMP,  NOMARK,
				"BUM!!! - Estouro na Pilha de Operações"
			);
		}

		scan ();

	}	/* end for (EVER) */

	/*
	 *	Escreve a Mensagem de Erro
	 */
    serror:
	switch (syntax)
	{
	    case E_LP|E_OPND:
		err_msg
		(	ERRO, TOKMARK,
			"Esperava um operando ou '('"
		);
		break;

	    case E_LP|E_OPND|E_UNOPR:
		err_msg
		(	ERRO, TOKMARK,
			"Esperava um operando, '(' ou operador unário"
		);
		break;

	    case E_END|E_RP|E_OPR:
		err_msg
		(	ERRO, TOKMARK,
			"Esperava um operador, ')' ou fim da expressão"
		);
		break;

	    default:
		err_msg
		(	ERRO, TOKMARK,
			"O número de '(' não confere com ')'"
		);
	}

	nextp = nextpsave;
	lineno++;

	return (0);

}	/* end expression */

/*
 ****************************************************************
 *	Realiza a Operação do Topo da Pilha			*
 ****************************************************************
 */
void
operate (void)
{
	long		v1, v2;

	v2 = top->e_opnd;
	top--;
	v1 = top->e_opnd;

	switch ((top + 1)->e_opr)
	{
	    case N_OROR:
		top->e_opnd = (v1 || v2);
		break;

	    case N_ANDAND:
		top->e_opnd = (v1 && v2);
		break;

	    case N_OR:
		top->e_opnd = (v1 | v2);
		break;

	    case N_XOR:
		top->e_opnd = (v1 ^ v2);
		break;

	    case N_AND:
		top->e_opnd = (v1 & v2);
		break;

	    case N_EQ:
		top->e_opnd = (v1 == v2);
		break;

	    case N_NE:
		top->e_opnd = (v1 != v2);
		break;

	    case N_LT:
		top->e_opnd = (v1 < v2);
		break;

	    case N_LE:
		top->e_opnd = (v1 <= v2);
		break;

	    case N_GT:
		top->e_opnd = (v1 > v2);
		break;

	    case N_GE:
		top->e_opnd = (v1 >= v2);
		break;

	    case N_LEFT:
		top->e_opnd = (v1 << v2);
		break;

	    case N_RIGHT:
		top->e_opnd = (v1 >> v2);
		break;

	    case N_PLUS:
		top->e_opnd = (v1 + v2);
		break;

	    case N_MINUS:
		top->e_opnd = (v1 - v2);
		break;

	    case N_MUL:
		top->e_opnd = (v1 * v2);
		break;

	    case N_DIV:
		if (v2 == 0)
		{
			err_msg (ERRO, TOKMARK, "Divisão por Zero");
			top->e_opnd = 0;
		}
		else
		{
			top->e_opnd = (v1 / v2);
		}
		break;

	    case N_MOD:
		if (v2 == 0)
		{
			err_msg (ERRO, TOKMARK, "Divisão por Zero");
			top->e_opnd = 0;
		}
		else
		{
			top->e_opnd = (v1 % v2);
		}
		break;

	    default:
		err_msg
		(	COMP, TOKMARK,
			"Operador inválido: %d",
			(top + 1)->e_opr
		);

	}	/* end switch */

}	/* end operate */

/*
 ****************************************************************
 *	Compara cadeias						*
 ****************************************************************
 */
int
maccmp (const char *lcp, const char *lep, MINOR op)
{
	const char	*rcp;
	const char	*rep;

	rcp  = token.l_begin;
	rep  = token.l_end;

	if (lep - lcp != rep - rcp)
		return (op == N_NE);

	while (lcp <= lep)
	{
		if (*lcp++ != *rcp++)
			return (op == N_NE);
	}

	return (op == N_EQ);

}	/* end maccmp */
