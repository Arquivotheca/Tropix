/*
 ****************************************************************
 *								*
 *			putprtb.c				*
 *								*
 *	Modifica as tabelas de conv. dos terminais/impressoras	*
 *								*
 *	Versao 1.0, de 6.2.86					*
 *								*
 *	Modulo: putprtb						*
 *		Utilitarios Especiais				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Multiprocessado		*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/kcntl.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <termio.h>

/*
 ****************************************************************
 *	Variaveis e Constantes					*
 ****************************************************************
 */
#define	NOSTR	(char *)NULL

char		*tbaddr;	/* Tabela escolhida */

int		flag12;		/* Tabela do nucleo */	

extern char	isotoisotb[];	/* Tabela sem conversao */

extern char	isotoatb[];	/* ISO para ASCII */

extern char	isotoabitb[];	/* ISO para ABICOMP */

extern char	cmpfoldtb[];	/* Tira letras Maiusculas */

/*
 ******	Protótipos de funções ***********************************
 */
void	help (void);

/*
 ****************************************************************
 *	Ponto de Entrada do Programa				*
 ****************************************************************
 */
int
main (int argc, register const char *argv[])
{
	register opt;

	/*
	 *	Analisa as Opcoes.
	 */
	while ((opt = getopt (argc, argv, "12iabmH")) != EOF)
	{
		switch (opt)
		{
		    case '1':			/* USER1 */
			if (flag12)
				help ();
			flag12 = USER1;
			break;

		    case '2':			/* USER2 */
			if (flag12)
				help ();
			flag12 = USER2;
			break;

		    case 'i':			/* ISO */
			if (tbaddr)
				help ();
			tbaddr = isotoisotb;
			break;

		    case 'a':			/* ASCII */
			if (tbaddr)
				help ();
			tbaddr = isotoatb;
			break;

		    case 'b':			/* ABICOMP */
			if (tbaddr)
				help ();
			tbaddr = isotoabitb;
			break;

		    case 'm':			/* MINUSCULO */
			if (tbaddr)
				help ();
			tbaddr = cmpfoldtb;
			break;

		    case 'H':			/* help */
			help ();

		    default:			/* erro */
			putc ('\n', stderr);
			help ();
		}

	}

	argv += optind;

	if (*argv != NOSTR)
		help ();

	if (flag12 == 0)
		flag12 = USER1;

	if (tbaddr == NOSTR)
		tbaddr = isotoisotb;

	if (kcntl (K_PUTCVTB, flag12, tbaddr) < 0)
	{
		fprintf (stderr, "Erro no K_PUTCVTB\n");
		perror (pgname);
		exit (1);
	}

	return (0);

}	/* end putprtb */

/*
 ****************************************************************
 *	Resumo de Utilizacao do Programa			*
 ****************************************************************
 */
void
help (void)
{
	fprintf (stderr, "%s - Modifica as tabelas de conversao ", pgname);
	fprintf (stderr, "dos terminais/impressoras.\n", pgname);
	fprintf (stderr, "\tVersao: 1.0 de 6.2.87.\n");
	fprintf (stderr, "\nSintaxe:\n");
	fprintf (stderr, "\t%s [-12] [-iabm]\n", pgname);
	fprintf (stderr, "\nOpcoes:");
	fprintf (stderr, "\t-1: altera a tabela USER1 (valor por omissao).\n");
	fprintf (stderr, "\t-2: altera a tabela USER2.\n\n");
	fprintf (stderr, "\t-i: sem conversao (valor por omissao).\n");
	fprintf (stderr, "\t-a: conversao para ASCII.\n");
	fprintf (stderr, "\t-b: conversao para ABICOMP.\n");
	fprintf (stderr, "\t-m: conversao para minusculas.\n");
	exit (2);

}	/* end help */
