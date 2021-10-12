/*
 ****************************************************************
 *								*
 *			error.c					*
 *								*
 *	Impressão de mensagens de erro				*
 *								*
 *	Versão	3.2.2, de 16.09.99				*
 *		4.4.0, de 05.12.02				*
 *								*
 *	Módulo: SHELL						*
 *		Interpretador de Comandos			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/common.h>
#include <sys/scb.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>

#include "../h/common.h"
#include "../h/error.h"
#include "../h/sym.h"
#include "../h/scan.h"
#include "../h/categ.h"

#include "../h/extern.h"

/*
 ****************************************************************
 *	Definições Locais					*
 ****************************************************************
 */
#define	TAB_STOP(c)	(((c) & (~7)) + 8)
#define TABSZ	8

/*
 ****************************************************************
 *	Protótipos de Funções					*
 ****************************************************************
 */
extern	void	printline (void);
extern	void	putmark (MARKTYPE);
extern	void	printline (void);

/*
 ****************************************************************
 *	Imprime a Linha						*
 ****************************************************************
 */
void
printline (void)
{
	fprintf (stderr, "%3d ", nline);
	fprintf (stderr, "%s\n", linebuf);

	lineprinted++;

}	/* end printline */

/*
 ****************************************************************
 *	Coloca a Marca Indicativa do Erro			*
 ****************************************************************
 */
void
putmark (MARKTYPE marca)
{
	char		*lp, *bp, *ep;
	int		col, c;

	if (!lineprinted)
		printline ();

	bp = token.t_begin;
	ep = token.t_end;

	if (marca == SHORTMARK)
		bp = ep;

	/*
	 *	Calcula espaço inicial
	 */
	col = 4;
	for (lp = linebuf; lp < bp; lp++)
	{
		if (*lp == '\t')
			col = TAB_STOP(col);
		else
			col++;
	}

	/*
	 *	Escreve espaco inicial
	 */
	for (c = col; c >= TABSZ; c -= TABSZ)
		putc ('\t', stderr);

	while (c-- > 0)
		putc (' ', stderr);

	/*
	 *	Marca pequena: escreve apenas ^
	 */
	if (ep == bp)
	{
		fprintf (stderr, "^\n");
		return;
	}

	/*
	 *	Marca grande: escreve o início
	 */
	putc ('|', stderr);
	col++;

	/*
	 *	Marca grande: escreve meio
	 */
	for (lp = bp + 1; lp < ep; lp++)
	{
		if (*lp == '\t')
		{
		    for (c = TAB_STOP(col); col < c; col++)
			putc ('/', stderr);
		}
		else
		{
			col++;
			putc ('/', stderr);
		}
	}

	/*
	 *	Marca grande: escreve fim
	 */
	fprintf (stderr, "|\n");

}	/* end putmark */

/*
 ****************************************************************
 *	Imprime uma mensagem de erro				*
 ****************************************************************
 */
void
sherror (MARKTYPE marca, const char *format, ...)
{
	va_list		args;

	/*
	 *	Prepara os argumentos
	 */
	va_start (args, format);

	/*
	 *	Coloca a marca indicativa do erro.
	 */
	putmark (marca);

	/*
	 *	Escreve a mensagem
	 */
	fprintf  (stderr, "%s: ", pgname);
	vfprintf (stderr, format, args);

	putc ('\n', stderr);

	va_end (args);

}	/* end sherror */
