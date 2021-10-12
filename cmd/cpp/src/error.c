/*
 ****************************************************************
 *								*
 *			error.c					*
 *								*
 *	Impressão das mensagens de erro				*
 *								*
 *	Versão	1.0.0, de 05.08.88				*
 *		4.6.0, de 27.08.04				*
 *								*
 *	Módulo:	CPP						*
 *		Tradutores					*
 *		Categoria D					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2004 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "../h/scan.h"
#include "../h/error.h"
#include "../h/cpp.h"

/*
 ****************************************************************
 *	Definições globais					*
 ****************************************************************
 */
entry int	nerrors;

/*
 ****************************************************************
 *	Imprime Mensagens de Erro				*
 ****************************************************************
 */
void
err_msg (ERRTYPE tipo, MARKTYPE marca, const char *msg, ...)
{
	va_list		args;
	int		var;
	const char	*begin_col, *end_col;

	/*
	 *	Retira os argumentos
	 */
	va_start (args, msg);

	var	  = va_arg (args, int);
	begin_col = va_arg (args, char *);
	end_col	  = va_arg (args, char *);

	/*
	 *	Coloca a Marca indicativa do Erro
	 */
	putmark (marca, begin_col, end_col);

	/*
	 *	Coloca o Tipo do Erro
	 */
	if (marca != NOMARK)
		fprintf (stderr, "******** ");

	switch (tipo)
	{
	    case ADVR:
		fprintf (stderr, "%sADVR%s: ", color_vector[MC_WARN], color_vector[MC_DEF]);
		break;

	    case ERRO:
		nerrors++;
		fprintf (stderr, "%sERRO%s: ", color_vector[MC_ERR], color_vector[MC_DEF]);
		break;

	    case COMP:
		fprintf (stderr, "ERRO DO PREPROCESSADOR: ");

	}	/* end switch */

	fprintf (stderr, msg, var);
	fprintf (stderr, "\n");

	if (tipo == COMP)
		quit (1);

	va_end (args);

}	/* end err_msg */

/*
 ********************************************************
 *	Coloca a Marca Indicativa do Erro		*
 ********************************************************
 */
void
putmark (MARKTYPE marca, const char *begin_col, const char *end_col)
{
	const char	*lp, *bp = NOSTR, *ep = NOSTR;
	int		col;
	int		lno = 0;

	switch (marca)
	{
	    case NOMARK:
	    default:
		fprintf
		(
			stderr,
			"%s: Arquivo \"%s\", Linha %d: ",
			pgname, basename (srcname), lineno
		);
		return;

	    case TOKMARK:
		bp  = token.l_begin;
		ep  = token.l_end;
		lno = token.l_line;
		break;

	    case MYMARK:
		bp  = begin_col;
		ep  = end_col;
		lno = lineno;
		break;
	}

	lp = printline (bp, lno);

	fprintf (stderr, "\t\t\t");

	for (col = 8; lp < bp; lp++)
	{
		if (*lp == '\t')
		{
			putc ('\t', stderr);
			col = (col + 8) & ~7;
		}
		else
		{
			putc (' ',  stderr);
			col++;
		}
	}

	if (ep == bp)
	{
		fprintf (stderr, "|\n");
		return;
	}

	putc ('|', stderr);

	col++;

	for (lp = bp + 1; lp < ep; lp++)
	{
		if (*lp == '\t')
		{
			do
			{
				putc ('/', stderr);
			}
			while (++col & 7);
		}
		else
		{
			col++;
			putc ('/', stderr);
		}
	}

	fprintf (stderr, "|\n");

}	/* end putmark */

/*
 ****************************************************************
 *	Imprime a Linha						*
 ****************************************************************
 */
const char *
printline (const char *bp, int lno)
{
	const char		*cp, *lp;
	char			c;
	static const char	*lastp;
	static int		lastlineno;

	for (cp = bp - 1; *cp != '\n'; cp--)
		/* vazio */;

	if (cp == lastp && lastlineno == lno)
		return (cp + 1);

	lastp = cp; lastlineno = lno;

	fprintf	(stderr, "%-14.14s  %6d  ", basename (srcname), lineno);

	lp = ++cp;

	do { c = *cp++;	putc (c, stderr); } while (c != '\n');

	return (lp);

}	/* end printline */

/*
 ****************************************************************
 *	Determina a Última Componente				*
 ****************************************************************
 */
const char *
basename (const char *name)
{
	const char	*cp;

	if ((cp = strrchr (name, '/')) == NOSTR)
		return (name);
	else
		return (++cp);

}	/* end basename */
