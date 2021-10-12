/*
 ****************************************************************
 *								*
 *			src/error.c				*
 *								*
 *	Impressão de mensagens de erro				*
 *								*
 *	Versão	2.0.0, de 17.08.88				*
 *		3.0.0, de 23.03.95				*
 *								*
 *	Módulo: EX/VI						*
 *		Editores de texto				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <stdio.h>
#include <curses.h>

#include "../h/global.h"
#include "../h/scan.h"
#include "../h/extern.h"
#include "../h/proto.h"

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
scn_error (const char *mens, ERROR marca)
{
	/*
	 *	Coloca a marca indicativa do erro
	 */
	putmark (marca);

	/*
	 *	Escreve a Mensagem
	 */
	waddstr (Win, "ERRO: ");
	waddstr (Win, mens);

	error_msg = NOSTR;

}	/* end scn_error */

/*
 ****************************************************************
 *	Coloca a marca indicativa do erro			*
 ****************************************************************
 */
void
putmark (ERROR marca)
{
	char		*lp, *bp, *ep;
	int		col, c;

	switch (marca)
	{
	    case NOMARK:
	    default:
		return;

	    case LONGMARK:
		bp = token.t_begin;
		ep = token.t_end;

		if (token.t_end < token.t_begin)
			ep = bp;
		break;

	    case SHORTMARK:
		ep = bp = nextp;
		break;
	}

	/*
	 *	Calcula espaco inicial
	 */
	col = 2;

	for (lp = Line; lp < bp; lp++)
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
		waddch (Win, '\t');

	while (c-- > 0)
		waddch (Win, ' ');

	/*
	 *	Marca pequena: escreve apenas '^'
	 */
	if (ep == bp)
	{
		waddch (Win, '^');
		waddch (Win, '\n');
		return;
	}

	/*
	 *	Marca grande: escreve o início
	 */
	waddch (Win, '|'); col++;

	/*
	 *	Marca grande: escreve meio
	 */
	for (lp = bp + 1; lp < ep; lp++)
	{
		if (*lp == '\t')
		{
			for (c = TAB_STOP(col); col < c; col++)
				waddch (Win, '/');
		}
		else
		{
			col++;
			waddch (Win, '/');
		}
	}

	/*
	 *	Marca grande: escreve fim
	 */
	waddch (Win, '|');
	waddch (Win, '\n');

}	/* end putmark */
