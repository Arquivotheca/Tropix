/*
 ****************************************************************
 *								*
 *			if.c					*
 *								*
 *	Processa as pseudos ".if", ".endif" e correlatas	*
 *								*
 *	Versão	3.0.7, de 15.09.94				*
 *		3.0.8, de 24.09.94				*
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
#include "../h/expr.h"
#include "../h/msg.h"
#include "../h/as.h"
#include "../h/proto.h"

/*
 ******	Variáveis Utilizadas ************************************
 *
 *	Convenção da PILHA:
 *		'U': é indefinido, e nunca foi definido
 *		'N': é indefinido, mas já foi definido
 *		'D': é definido
 */
entry char	if_stack[100];		/* Pilha de ".IFs", etc ... */

entry char	*if_stackp = if_stack;	/* Ponteiro do Stack */

entry int	if_false_level;	/* No. de Condicionais FALSE aninhadas */

/*
 ****************************************************************
 *	Analisa as pseudos ".if" (== ".ifnz") e ".ifz"		*
 ****************************************************************
 */
void
pseudo_if (int nz)
{
	EXP		exp;

	/*
	 *	Código de "nz":
	 *
	 *		0 => ".ifz"
	 *		1 => ".ifnz"
	 */
	if (if_false_level == 0)
	{
		gettoken (); 	expression (&exp);

		if (exp.ex_type != ABS)
			nonabsexp (&exp);

		if (nz ? exp.ex_value : !exp.ex_value)
			{ *if_stackp++ = 'D'; }
		else
			{ *if_stackp++ = 'U'; if_false_level++; }
	}
	else
	{
		{ *if_stackp++ = 'U'; if_false_level++; }
	}

#if (0)	/*************************************/
	gettoken (); 	expression (&exp);

	if (exp.ex_type != ABS)
		nonabsexp (&exp);

	if (if_false_level == 0 && (nz ? exp.ex_value : !exp.ex_value))
		{ *if_stackp++ = 'D'; }
	else
		{ *if_stackp++ = 'U'; if_false_level++; }
#endif	/*************************************/

}	/* end pseudo_if */

/*
 ****************************************************************
 *	Analisa as pseudos ".ifdef" e ".ifndef"			*
 ****************************************************************
 */
void
pseudo_ifdef (int nz)
{
	EXP		exp;
	int		defined;

	/*
	 *	Código de "nz":
	 *
	 *		1 => ".ifdef"
	 *		0 => ".ifndef
	 */
	if (if_false_level == 0)
	{
		gettoken (); 	expression (&exp);

		defined = (exp.ex_type != UNDEF);

		if (nz ? defined : !defined)
			{ *if_stackp++ = 'D'; }
		else
			{ *if_stackp++ = 'U'; if_false_level++; }
	}
	else
	{
		{ *if_stackp++ = 'U'; if_false_level++; }
	}

#if (0)	/*************************************/
	gettoken (); 	expression (&exp);

	defined = (exp.ex_type != UNDEF);

	if (if_false_level == 0 && (nz ? defined : !defined))
		{ *if_stackp++ = 'D'; }
	else
		{ *if_stackp++ = 'U'; if_false_level++; }
#endif	/*************************************/

}	/* end pseudo_ifdef */

/*
 ****************************************************************
 *	Analisa as pseudos ".elif" (== ".elifnz") e ".elifz"	*
 ****************************************************************
 */
void
pseudo_elif (int nz)
{
	EXP		exp;

	/*
	 *	Código de "nz":
	 *
	 *		0 => ".elifz"
	 *		1 => ".elifnz"
	 */
	if (if_stackp <= if_stack)
	{
		msg
		(	ERRO, LONGMARK,
			"\".elif\" sem prévio \".if...\""
		);
		return;
	}

	switch (if_stackp[-1])
	{
	    case 'U':
		if (if_false_level == 1)
		{
			gettoken (); 	expression (&exp);

			if (exp.ex_type != ABS)
				nonabsexp (&exp);

			if (nz ? exp.ex_value : !exp.ex_value)
				{ if_stackp[-1] = 'D'; if_false_level--; }
		}
		break;

	    case 'N': 		/* Nada faz */
		break;

	    case 'D':
		if_stackp[-1] = 'N';
		if_false_level++;
		break;

	    default:
		msg
		(	ERRO, LONGMARK,
			"Estado inválido da PILHA: '%c'",
			if_stackp[-1]
		);

	}	/* end switch */

}	/* end pseudo_elif */

/*
 ****************************************************************
 *	Analisa as pseudos ".eldef" e ".elndef"			*
 ****************************************************************
 */
void
pseudo_eldef (int nz)
{
	EXP		exp;
	int		defined;

	/*
	 *	Código de "nz":
	 *
	 *		1 => ".eldef"
	 *		0 => ".elndef
	 */
	if (if_stackp <= if_stack)
	{
		msg
		(	ERRO, LONGMARK,
			"\".elif\" sem prévio \".if...\""
		);
		return;
	}

	switch (if_stackp[-1])
	{
	    case 'U':
		if (if_false_level == 1)
		{
			gettoken (); 	expression (&exp);

			defined = (exp.ex_type != UNDEF);

			if (nz ? defined : !defined)
				{ if_stackp[-1] = 'D'; if_false_level--; }
		}
		break;

	    case 'N': 		/* Nada faz */
		break;

	    case 'D':
		if_stackp[-1] = 'N';
		if_false_level++;
		break;

	    default:
		msg
		(	ERRO, LONGMARK,
			"Estado inválido da PILHA: '%c'",
			if_stackp[-1]
		);

	}	/* end switch */

}	/* end pseudo_eldef */



#if (0)	/*************************************/
	gettoken (); 	expression (&exp);

	if (exp.ex_type != ABS)
		nonabsexp (&exp);

	switch (if_stackp[-1])
	{
	    case 'U':
		if (if_false_level == 1 && (nz ? exp.ex_value : !exp.ex_value))
			{ if_stackp[-1] = 'D'; if_false_level--; }
		break;

	    case 'N': 		/* Nada faz */
		break;

	    case 'D':
		if_stackp[-1] = 'N';
		if_false_level++;
		break;

	    default:
		msg
		(	ERRO, LONGMARK,
			"Estado inválido da PILHA: '%c'",
			if_stackp[-1]
		);
	}
#endif	/*************************************/

/*
 ****************************************************************
 *	Analisa a pseudo ".else"				*
 ****************************************************************
 */
void
pseudo_else (void)
{
	if (if_stackp <= if_stack)
	{
		msg
		(	ERRO, LONGMARK,
			"\".else\" sem prévio \".if...\""
		);
		return;
	}

	switch (if_stackp[-1])
	{
	    case 'U':
		if_stackp[-1] = 'D';
		if_false_level--;
		break;

	    case 'N': 		/* Nada faz */
		break;

	    case 'D':
		if_stackp[-1] = 'U';
		if_false_level++;
		break;

	    default:
		msg
		(	ERRO, LONGMARK,
			"Estado inválido da PILHA: '%c'",
			if_stackp[-1]
		);
	}

}	/* end pseudo_else */

/*
 ****************************************************************
 *	Analisa a pseudo ".endif"				*
 ****************************************************************
 */
void
pseudo_endif (void)
{
	if (if_stackp <= if_stack)
	{
		msg
		(	ERRO, LONGMARK,
			"\".endif\" sem prévio \".if...\""
		);
		return;
	}

	switch (*--if_stackp)
	{
	    case 'U':
	    case 'N':
		if_false_level--;
		break;

	    case 'D':
		break;

	    default:
		msg
		(	ERRO, LONGMARK,
			"Estado inválido da PILHA: '%c'",
			if_stackp[0]
		);
	}

}	/* end pseudo_endif */

/*
 ****************************************************************
 *	Verifica o estado final da PILHA			*
 ****************************************************************
 */
void
if_stack_check (void)
{
	if (if_stackp != if_stack)
	{
		msg (ERRO, NOMARK, "Faltaram \".endif's\"");
	}
	else
	{
		if (if_false_level)
		{
			msg
			(	COMP, NOMARK,
				"stack_check: if_false_level não zerou: %d",
				if_false_level
			);
		}
	}

}	/* end if_stack_check */
