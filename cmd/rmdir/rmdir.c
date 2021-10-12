/*
 ****************************************************************
 *								*
 *			rmdir.c					*
 *								*
 *	Remove diretórios					*
 *								*
 *	Versão	1.0.0, de 21.01.87				*
 *		4.2.0, de 08.05.02				*
 *								*
 *	Módulo: rmdir						*
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
#include <errno.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão:	4.2.0, de 08.05.02";

#define	elif	else if

entry int	vflag;		/* Verbose */
entry int	Nflag;		/* Nomes dos pgms no <stdin> */

/*
 ******	Protótipos de funções ***********************************
 */
void		help (void);

/*
 ****************************************************************
 *	Remove diretórios					*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	int		opt, exit_code = 0;

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

		    case 'N':			/* Nomes do <stdin> */
			Nflag++;
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
	 *	Analisa o Nflag
	 */
	if (Nflag && *argv != NOSTR)
	{
		error ("Os argumentos supérfluos serão ignorados");
		*argv = NOSTR;
	}

	/*
	 *	Procura os argumentos
	 */
	if (*argv == NOSTR)		/* Não foram dados argumentos: lê do "stdin" */
	{
		char		*area = alloca (1024);

		while (fngets (area, 1024, stdin) != NOSTR)
		{
			if (vflag)
				printf ("%s\n", area);

			if (rmdir (area) < 0)
			{
				error ("*Não consegui remover o diretório \"%s\"", area);
				exit_code++;
			}
		}
	}
	else				/* Foram dados argumentos */
	{
		for (/* vazio */; *argv; argv++)
		{
			if (vflag)
				printf ("%s\n", *argv);

			if (rmdir (*argv) < 0)
			{
				error ("*Não consegui remover o diretório \"%s\"", *argv);
				exit_code++;
			}
		}
	}

	return (exit_code);

}	/* end main */

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
		"%s - remove diretórios\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-vN] <diretório> ...\n",
		pgname, pgversion, pgname
	);

	fprintf
	(	stderr,
		"\nOpções:"
		"\t-v: Lista os nomes dos diretórios\n"
		"\t-N: Lê os nomes dos <arquivo>s de \"stdin\"\n"
		"\t    Esta opção é implícita se não forem dados argumentos\n"
	);

	exit (2);

}	/* end help */
