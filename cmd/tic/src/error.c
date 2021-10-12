/*
 ****************************************************************
 *								*
 *			error.c					*
 *								*
 *	Impressão de mensagens de erro				*
 *								*
 *	Versão 2.0.0, de 21.04.87				*
 *	       3.0.0, de 12.07.97				*
 *								*
 *	Modulo: tic/tac						*
 *		Utilitarios Basicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include	<sys/types.h>

#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>

#include	"../h/error.h"
#include	"../h/scan.h"

/*
 ****************************************************************
 *	Variáveis Externas					*
 ****************************************************************
 */
extern	TOKEN	token;		/* Estrutura de saida do scanner */
extern	char	*line;		/* Aponta Buffer com a linha */
extern	int	nline;		/* No. da linha atual */
extern	bool	lineprinted;	/* A linha já foi impressa */
extern	char	*source;	/* Nome do Arquivo de Entrada */

/*
 ****************************************************************
 *	Variáveis Exportadas:					*
 ****************************************************************
 */
entry int	nadvr;		/* Numero de advertencias */
entry int	nerro;		/* Numero de erros */
entry int	shift = 7;	/* Deslocamento inicial para a Marcação */

/*
 ****************************************************************
 *	Funções Locais 						*
 ****************************************************************
 */
void	putmark ();		/* Coloca marca indicativa do erro */

/*
 ****************************************************************
 *	Definições Locais					*
 ****************************************************************
 */
#define TABSPACE		8
#define	TAB_STOP(c)	(((c) & (~7)) + TABSPACE)

/*
 ******	Protótipos de funções ***********************************
 */
void		putmark (MARKTYPE);
void		quit (int);
void		printline (void);

/*
 ****************************************************************
 *	Rotina de Tratamento de Erros				*
 ****************************************************************
 */
void
tic_error (ERRTYPE tipo, MARKTYPE marca, const char *mens, const char *var)
{
	/*
	 *	Coloca a Marca indicativa do Erro
	 */
	putmark (marca);

	/*
	 *	Coloca o Tipo do Erro
	 */
	fputs ("******** ", stderr);

	switch (tipo)
	{
	    case ADVR:
		fputs ("ADVR: ", stderr);
		nadvr++;
		break;

	    case ERRO:
		fputs ("ERRO: ", stderr);
		nerro++;
		break;

	    case FATAL:
		fputs ("ERRO FATAL: ", stderr);
		break;

	    case COMP:
		fputs ("ERRO INTERNO: ", stderr);

	}	/* end switch (tipo) */

	fprintf (stderr, mens, var);
	putc ('\n', stderr);
	putc ('\n', stderr);

	if (tipo == COMP)
		quit (1);

}	/* end tic_error */

/*
 ****************************************************************
 *	Coloca a Marca Indicativa do Erro			*
 ****************************************************************
 */
void
putmark (MARKTYPE marca)
{
	register char	*lp, *bp, *ep;
	register int	col, c;

	printline ();

	if (marca == NOMARK)
		return;

	bp = token.t_begin;
	ep = token.t_end;

	if (marca == SMARK)
		ep = bp;

	/*
	 *	Calcula espaco inicial
	 */
	col = shift;

	for (lp = line; lp < bp; lp++)
	{
		if (*lp == '\t')
			col = TAB_STOP(col);
		else
			col++;
	}

	/*
	 *	Escreve espaco inicial
	 */
	for (c = col; c >= TABSPACE; c -= TABSPACE)
		putc ('\t', stderr);

	while (c-- > 0)
		putc (' ', stderr);

	/*
	 *	Marca pequena: escreve apenas |
	 */
	if (ep == bp)
	{
		fputs ("|\n", stderr);
		return;
	}

	/*
	 *	Marca grande: escreve o inicio
	 */
	putc ('|', stderr);
	col++;

	/*
	 *	Marca grande: escreve meio
	 */
	for (lp = bp + 1 ; lp < ep ; lp++)
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
	if (*lp == '\t')
	{
	    for (c = TAB_STOP(col) - 1 ; col < c ; col++)
		putc ('/', stderr);
	}
	fputs ("|\n", stderr);

}	/* end putmark */

/*
 ****************************************************************
 *	Termino do Scanner devido a Erro Irrecuperável		*
 ****************************************************************
 */
void
quit (int n)
{
	exit (n);

}	/* end quit */

/*
 ****************************************************************
 *	Escreve a Linha Atual.					*
 ****************************************************************
 */
void
printline (void)
{
	char	buf[50];

	sprintf (buf, "%s (%d): ", source, nline);

	shift = strlen (buf);

	fprintf (stderr, "%s%s\n", buf, line);

	lineprinted = true;

}	/* end printline */
