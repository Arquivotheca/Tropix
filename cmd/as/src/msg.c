/*
 ****************************************************************
 *								*
 *			msg.c					*
 *								*
 *	Impressão das mensagens de erro				*
 *								*
 *	Versão	1.0.0, de 21.01.86				*
 *		3.0.0, de 26.02.94				*
 *								*
 *	Rotinas:						*
 *		msg	putmark					*
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
#include <stdarg.h>
#include <a.out.h>

#include "../h/common.h"
#include "../h/tokens.h"
#include "../h/scan.h"
#include "../h/id_sym_tb.h"
#include "../h/expr.h"
#include "../h/opnd.h"
#include "../h/msg.h"
#include "../h/as.h"
#include "../h/proto.h"

/*
 ********************************************************
 *	Rotina de Tratamento de Erros			*
 ********************************************************
 */
void
msg (int tipo, int marca, char *fmt, ...)
{
	va_list		args;
	va_list		printf_args;
	int		var;
	char		*begin, *end;

	/*
	 *	Retira os argumentos
	 */
	va_start (args, fmt);
	printf_args = args;

	var	= va_arg (args, int);
	begin	= va_arg (args, char *);
	end	= va_arg (args, char *);

	/*
	 *	Imprime a linha
	 */
	if (!lineprinted)
		printline ();

	/*
	 *	Coloca a Marca indicativa do Erro
	 */
	putmark (marca, begin, end);

	/*
	 *	Coloca o Tipo do Erro
	 */
	fprintf (stderr, "******** ");

	switch (tipo)
	{
	    case ADVR:
		fprintf (stderr, "ADVR: ");
		break;

	    case ERRO:
		fprintf (stderr, "ERRO: ");
		break;

	    case COMP:
		fprintf (stderr, "ERRO DO MONTADOR: ");

	}	/* end switch */

	vfprintf (stderr, fmt, printf_args);

	fputc ('\n', stderr);

	if (tipo == COMP)
		quit (1);

	if (tipo == ERRO)
		nmsgs++;

	va_end (args);

}	/* end msg */

/*
 ********************************************************
 *	Coloca a Marca Indicativa do Erro		*
 ********************************************************
 */
void
putmark (int marca, char *begin, char *end)
{
	char		*lp, *bp, *ep;
	short		col;
	EXP		*expp;
	OPND		*opndp;

	switch (marca)
	{
	    case NOMARK:
		return;

	    case SHORTMARK:
		bp = ep = token->l_begin;
		break;

	    default:
	    case LONGMARK:
		bp = token->l_begin;
		ep = token->l_end;
		break;

	    case LASHORTMARK:
		bp = ep = latoken->l_begin;
		break;

	    case LALONGMARK:
		bp = latoken->l_begin;
		ep = latoken->l_end;
		break;

	    case MYMARK:
		bp = begin;
		ep = end;
		marca = LONGMARK;
		break;

	    case EXPMARK:
		expp = (EXP *)begin;
		bp = expp->ex_begin;
		ep = expp->ex_end;
		marca = LONGMARK;
		break;

	    case OPNDMARK:
		opndp = (OPND *)begin;
		bp = opndp->op_begin;
		ep = opndp->op_end;
		marca = LONGMARK;
		break;
	}

	fputc ('\t', stderr);

	col = 8;

	for (lp = line; lp < bp; lp++)
	{
		if (*lp == '\t')
		{
			fputc ('\t', stderr);
			col = (col + 8) & ~7;
		}
		else
		{
			fputc (' ',  stderr);
			col++;
		}
	}

	if (ep == bp)
	{
		fprintf (stderr, "|\n");
		return;
	}

	fputc ('|', stderr);

	col++;

	for (lp = bp + 1; lp < ep; lp++)
	{
		if (*lp == '\t')
		{
			do
			{
				fputc ('/', stderr);
			}
			while (++col & 7);
		}
		else
		{
			col++;
			fputc ('/', stderr);
		}
	}

	fprintf (stderr, "|\n");

}	/* end putmark */

/*
 ********************************************************
 *	Imprime a Linha					*
 ********************************************************
 */
void
printline (void)
{
	fprintf (stderr, "%-8d%s\n", token->l_line, line);
	lineprinted++;

}	/* end printline */
