/*
 ****************************************************************
 *								*
 *			mkfifo.c				*
 *								*
 *	Cria FIFOs 						*
 *								*
 *	Versão	1.0.0, de 05.02.86				*
 *		3.0.0, de 31.10.90				*
 *								*
 *	Módulo: mkfifo						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Multiprocessado		*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <stat.h>
#include <errno.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão:	3.0.0, de 23.05.97";

#define	elif	else if

entry int	exit_code = 0;	/* Código de retorno */

entry int	vflag;		/* Verbose */

/*
 ****** Protótipos de funções ***********************************
 */
void		help (void);

/*
 ****************************************************************
 *	Cria FIFOs 						*
 ****************************************************************
 */
int
main (int argc, register const char *argv[])
{
	register int	opt;

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
	 *	Cria os FIFOs
	 */
	for (/* vazio */; *argv; argv++)
	{
		if (mknod (*argv, S_IFIFO|S_IWRITE|S_IREAD, 0) < 0)
		{
			fprintf
			(	stderr,
				"%s: Não consegui criar \"%s\" (%s)\n",
				pgname, *argv, strerror (errno)
			);
			exit_code++;
		}

	}	/* end for */

	return (exit_code);

}	/* end mkfifo */

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
		"%s - cria FIFOs\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s <fifo> ...\n",
		pgname, pgversion, pgname
	);

	exit (2);

}	/* end help */
