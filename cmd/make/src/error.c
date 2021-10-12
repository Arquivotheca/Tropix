/*
 ****************************************************************
 *								*
 *			error.c					*
 *								*
 *	Impressão de mensagens de erro				*
 *								*
 *	Versão	3.0.0, de 29.03.96				*
 *		3.2.3, de 09.01.00				*
 *								*
 *	Módulo: Make: Construtor de Programas			*
 *		Interpretador de Comandos			*
 *		Categoria D					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 * 								*
 ****************************************************************
 */

#include <sys/syscall.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

#include "../h/common.h"
#include "../h/error.h"
#include "../h/sym.h"
#include "../h/scan.h"
#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Variáveis Exportadas					*
 ****************************************************************
 */
entry	int	nwarnings;	/* Número de advertências */
entry	int	nerrors;	/* Número de erros */

/*
 ****************************************************************
 *	Definições Locais					*
 ****************************************************************
 */
#define	TAB_STOP(c)	(((c) & (~7)) + 8)
#define TABSZ		8

/*
 ****************************************************************
 *	Variáveis locais					*
 ****************************************************************
 */
static	RANGE	given_mark;	/* Marca fornecida explicitamente */

/*
 ****************************************************************
 *	Protótipos de funções					*
 ****************************************************************
 */
void	print_line (char *, int);
void	put_mark (MARKTYPE);

/*
 ****************************************************************
 *	Imprime a Linha						*
 ****************************************************************
 */
void
print_line (char *line, int nline)
{
	char		*cp, c;
	static int	last_line_printed = 0;

	if (last_line_printed == nline)
		return;

	fprintf	(stderr, "%-7d ", nline);

	for (cp = line; (c = *cp++) != '\n' && c != NOCHR; putc (c, stderr))
		/* vazio */;

	putc ('\n', stderr);

	last_line_printed = nline;

}	/* end print_line */

/*
 ****************************************************************
 *	Coloca a Marca Indicativa do Erro			*
 ****************************************************************
 */
void
put_mark (MARKTYPE mark)
{
	char	*lp, *bp, *ep;
	int	col, c, nl;

	bp = ep = lp = NOSTR;		/* Para fazer o "lint" feliz */
	nl = 0;

	switch (mark)
	{
	    case NOMARK:
		return;

	    case TOKMARK:
		bp = token.t_range.r_begin;
		ep = token.t_range.r_end;
		nl = token.t_range.r_nline;
		break;

	    case GIVENMARK:
		bp = given_mark.r_begin;
		ep = given_mark.r_end;
		nl = given_mark.r_nline;
		break;

	    case SHORTMARK:
		bp = ep = token.t_range.r_begin;
		nl = token.t_range.r_nline;
		break;
	}

	for (lp = bp; lp >= makefile_text && *lp != NOCHR; lp--)
		/* vazio */;

	print_line (lp + 1, nl);

	/*
	 *	Calcula espaço inicial
	 */
	col = 8;

	for (/* sem inicialização */; lp < bp; lp++)
	{
		if (*lp == '\t')
			col = TAB_STOP (col);
		else
			col++;
	}

	/*
	 *	Escreve espaço inicial
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
		fputs ("^\n", stderr);
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
		    for (c = TAB_STOP (col); col < c; col++)
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
	fputs ("|\n", stderr);

}	/* end put_mark */

/*
 ****************************************************************
 *	Imprime uma mensagem de erro				*
 ****************************************************************
 */
entry void
err_msg (ERRTYPE type, MARKTYPE mark, char *mens, ...)
{
	va_list		ap;

	/*
	 *	Coloca a Marca indicativa do Erro
	 */
	put_mark (mark);

	/*
	 *	Coloca o Tipo do Erro
	 */
	fputs ("******** ", stderr);

	switch (type)
	{
	    case WARNING:
		fputs ("ADVR: ", stderr);
		nwarnings++;
		break;

	    case ERROR:
		fputs ("ERRO: ", stderr);
		nerrors++;
		break;

	    case INTERNAL_ERROR:
		fputs ("ERRO DO MAKE: ", stderr);

	}	/* end switch (tipo) */

	va_start (ap, mens);
	vfprintf (stderr, mens, ap);
	va_end (ap);

	putc ('\n', stderr);

	if (type == INTERNAL_ERROR)
		exit (1);

}	/* end err_msg */

/*
 ****************************************************************
 *	Emite uma mensagem de erro				*
 ****************************************************************
 */
entry void
msg (char *mens, ...)
{
	va_list	ap;

	fprintf (stderr, "%s: ", pgname);
	va_start (ap, mens);
	vfprintf (stderr, mens, ap);
	va_end (ap);

	putc ('\n', stderr);

}	/* end msg */

/*
 ****************************************************************
 *	Define marcas não relacionadas com o token corrente	*
 ****************************************************************
 */
entry void
set_mark (RANGE *rp)
{
	(void)memmove (&given_mark, rp, sizeof (RANGE));

}	/* end set_mark */

/*
 ****************************************************************
 *	Tenta alocar e aborta se não conseguir			*
 ****************************************************************
 */
entry void *
emalloc (int size)
{
	void	*vp;

	if ((vp = malloc (size)) == NOVOID)
	{
		msg ("Memória insuficiente");
		exit (2);
	}

	return (vp);

}	/* end emalloc */
