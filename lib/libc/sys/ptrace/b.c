/*
 ****************************************************************
 *								*
 *			tr1.c					*
 *								*
 *	Teste do TRACE -- FILHO					*
 *								*
 *	Versão	1.0.0, de 11.02.88				*
 *		3.0.0, de 18.08.95				*
 *								*
 *	Módulo: KERNEL						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	*pgname;	/* Nome do programa */

int		vflag;		/* Verbose */

/*
 ******	Protótipos de funções ***********************************
 */
int	help (void);

/*
 ****************************************************************
 *	Programa de teste do TRACE				*
 ****************************************************************
 */
main (int argc, register const char *argv[])
{
	register int	opt;

	pgname = argv[0];

	/*
	 *	Analisa as opções.
	 */
	while ((opt = getopt (argc, argv, "vH")) != EOF)
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
		}

	}

	argv += optind;
	argc -= optind;

	/*
	 *	Aqui executa o filho
	 */
	printf ("FILHO: iniciando\n");

	printf ("FILHO: terminando\n");
	
	exit (0);

}	/* end tr0 */

/*
 ****************************************************************
 *	Resumo de utilização do programa			*
 ****************************************************************
 */
int
help (void)
{
	fprintf (stderr, "%s - programa.\n", pgname);
	fprintf (stderr, "\tVersão: 1.0 de x.88.\n");
	fprintf (stderr, "\nSintaxe:\n");
	fprintf (stderr, "\t%s [-opções] arg\n", pgname);
	fprintf (stderr, "\nOpções:\t-x: opção.\n");
	fprintf (stderr, "\t-y: opção.\n");
	fprintf (stderr, "\nObs.:\tobservação.\n");
	exit (2);

}	/* end help */
