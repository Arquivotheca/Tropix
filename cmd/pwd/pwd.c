/*
 ****************************************************************
 *								*
 *			pwd.c					*
 *								*
 *	Imprime o diretório corrente				*
 *								*
 *	Versão	1.0.0, de 04.04.86				*
 *		3.0.1, de 28.07.97				*
 *								*
 *	Módulo: pwd						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão:	3.0.1, de 28.06.97";

#define	elif	else if

#define		PWDNMSZ		512

entry int	vflag;		/* Verbose */

/*
 ****** Protótipos de funções ***********************************
 */
void		help (void);

/*
 ****************************************************************
 *	Imprime o diretório corrente				*
 ****************************************************************
 */
int
main (int argc, register const char *argv[])
{
	register int	opt;
	char		*pwd;

	/*
	 *	Analisa as opções
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

		}	/* end switch */

	}	/* end while */

	argv += optind;
	argc -= optind;

	/*
	 *	Obtém o diretório corrente
	 */
	if ((pwd = getcwd (NOSTR, PWDNMSZ)) == NOSTR)
	{
		fprintf
		(	stderr,
			"%s: Não consegui obter o nome do diretório corrente (%s)\n",
			pgname, strerror (errno)
		);
		exit (1);
	}

	/*
	 *	Imprime o diretório corrente
	 */
	printf ("%s\n", pwd);

	return (0);

}	/* end pwd */

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
		"%s - imprime o diretório corrente\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s\n",
		pgname, pgversion, pgname
	);

	exit (2);

}	/* end help */
