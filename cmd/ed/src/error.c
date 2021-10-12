/*
 ****************************************************************
 *								*
 *			src/error.c				*
 *								*
 *	Impressão de Mensagens de Erro				*
 *								*
 *	Versão	1.0.0, de 04.11.86				*
 *		3.0.0, de 27.05.97				*
 *								*
 *	Modulo: ED						*
 *		Editores de Texto				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Multiprocessado		*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <stdio.h>
#include <stdlib.h>

#include "../h/ed.h"
#include "../h/scan.h"
#include "../h/proto.h"
#include "../h/extern.h"

/*
 ****************************************************************
 *	Definições Locais					*
 ****************************************************************
 */
#define  TABSZ		(8)	/* Tem que ser potencia de 2 */

#define  TAB_STOP(c)	(((c) & ~(TABSZ-1)) + TABSZ)

/*
 ****************************************************************
 *	Rotina de Tratamento de Erros				*
 ****************************************************************
 */
void
ed_error (const char *mens, ERR marca)
{
	/*
	 *	Escreve a linha de comando se executando em Batch
	 */
	if (!Interactive && Line[0] != '\0')
	{
		putchar (':');
		putchar (' ');
		puts (Line);
	}

	/*
	 *	Coloca a Marca indicativa do Erro
	 */
	putmark (marca);

	/*
	 *	Escreve a Mensagem
	 */
	if (!Interactive)
		printf ("%s: ERRO: %s\n", pgname, mens);
	else
		printf ("ERRO: %s\n", mens);

	error_msg = NULL;

}	/* end error */

/*
 ****************************************************************
 *	Coloca a Marca Indicativa do Erro			*
 ****************************************************************
 */
void
putmark (ERR marca)
{
	char		*lp, *bp, *ep;
	int		col, c;

	switch (marca)
	{
	    case NOMARK:
		return;

	    case LONGMARK:
		bp = token.t_begin;
		ep = token.t_end;

		if (token.t_end < token.t_begin)
			ep = bp;
		break;

	    case SHORTMARK:
	    default:
		ep = bp = nextp;
		break;
	}

	/*
	 *	Calcula o espaco inicial
	 */
	for (col = 2, lp = Line; lp < bp; lp++)
	{
		if (*lp == '\t')
			col = TAB_STOP(col);
		else
			col++;
	}

	/*
	 *	Escreve o espaco inicial
	 */
	for (c = col; c >= TABSZ; c -= TABSZ)
		putchar ('\t');

	while (c-- > 0)
		putchar (' ');

	/*
	 *	Marca pequena: escreve apenas '^'
	 */
	if (ep == bp)
		{ putchar ('^'); putchar ('\n'); return; }

	/*
	 *	Marca grande: escreve o início
	 */
	putchar ('|'); col++;

	/*
	 *	Marca grande: escreve meio
	 */
	for (lp = bp + 1; lp < ep; lp++)
	{
		if (*lp == '\t')
		{
			for (c = TAB_STOP(col); col < c; col++)
				putchar ('/');
		}
		else
		{
			col++;
			putchar ('/');
		}
	}

	/*
	 *	Marca grande: escreve fim
	 */
	putchar ('|'); putchar ('\n');

}	/* end putmark */
