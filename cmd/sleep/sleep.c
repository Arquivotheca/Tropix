/*
 ****************************************************************
 *								*
 *			sleep.c					*
 *								*
 *	Suspende a execução por um intervalo de tempo		*
 *								*
 *	Versão	1.1.0, de 20.01.87				*
 *		3.0.1, de 05.07.97				*
 *								*
 *	Módulo: sleep						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Multiprocessado		*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <stdio.h>
#include <stdlib.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão:	3.0.1, de 05.07.97";

entry int	vflag;		/* Verbose */

/*
 ****** Protótipos de funções ***********************************
 */
void		help (void);

/*
 ****************************************************************
 *	Suspende a execução por um intervalo de tempo		*
 ****************************************************************
 */
int
main (int argc, register const char *argv[])
{
	register int	opt;
	const char	*str;
	unsigned	time;

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "vNH")) != EOF)
	{
		switch (opt)
		{
		    case 'v':			/* Verbose */
			vflag++;
			break;

		    case 'H':			/* Help */
			help ();

		    default:			/* Erro */
			putc ('\n', stderr);
			help ();

		}	/* end switch */

	}	/* end while */

	argv += optind;
	argc -= optind;

	/*
	 *	Analisa o tempo dado
	 */
	if (argv[0] == NOSTR)
		help ();

	if ((time = strtol (argv[0], &str, 0)) == 0 || *str != '\0')
	{
		fprintf (stderr, "Tempo inválido: \"%s\"\n", argv[0]);
		exit (1);
	}

	/*
	 *	Dorme o tempo pedido
	 */
	sleep (time);

	return (0);

}	/* end sleep */

/*
 ****************************************************************
 *	Resumo de utilização do programa			*
 ****************************************************************
 */
void
help (void)
{
	fprintf
	(	stderr,
		"%s - suspende a execução por um intervalo de tempo\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s <tempo>\n",
		pgname, pgversion, pgname
	);
	fprintf
	(	stderr,
		"\nObs.:\tO <tempo> é dado em segundos\n"
	);

	exit (2);

}	/* end help */
