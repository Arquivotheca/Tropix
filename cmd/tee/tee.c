/*
 ****************************************************************
 *								*
 *			tee.c					*
 *								*
 *	Copia a entrada padrão para diversos arquivos e		*
 *	para a saida padrão					*
 *								*
 *	Versão	1.0.0, de 10.08.89				*
 *		3.0.0, de 12.07.97				*
 *								*
 *	Módulo: tee						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Multiprocessado		*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>	
#include <fcntl.h>	
#include <errno.h>	

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão:	3.0.0, de 12.07.97";

#define	O_MODE(f) (f ? O_WRONLY|O_CREAT|O_APPEND : O_WRONLY|O_CREAT|O_TRUNC)

entry int	aflag;		/* Append */
entry int	vflag;		/* Verbose */

/*
 ******	Protótipos de funções ***********************************
 */
void		help (void);

/*
 ****************************************************************
 *	Programa de duplicar a entrada padrão			*
 ****************************************************************
 */
int
main (int argc, register const char *argv[])
{
	register int		opt, n, i;
	register int		*fd;
	int			nfiles;
	int			ret = 0;
	char			buf[BUFSIZ];

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "aivH")) != EOF)
	{
		switch (opt)
		{
		    case 'a':			/* Append */
			aflag++;
			break;

		    case 'i':			/* Ignore interrupts */
			signal (SIGHUP,  SIG_IGN);
			signal (SIGINT,  SIG_IGN);
			signal (SIGQUIT, SIG_IGN);
			break;

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
	 *	Aloca área para os "file descriptors"
	 */
	nfiles = argc + 1;

	fd = alloca (nfiles * sizeof (int));
	
	argv--;		/* argv[0] == stdout */
	fd[0] = 1;

	/*
	 *	Abre todos os arquivos dados
	 */
	for (i = 1; argv[i]; i++)
	{
		if ((fd[i] = open (argv[i], O_MODE (aflag), 0666)) < 0)
		{
			fprintf 
			(	stderr,
				"%s: Não consegui abrir \"%s\" (%s)\n",
				pgname, argv[i], strerror (errno)
			);
			ret++;
		}
	}

	if (ret)
		exit (ret);

	/*
	 *	Lê da entrada padrão e copia em todas as saídas
	 */
	while ((n = read (0, buf, BUFSIZ)) > 0)
	{
		for (i = 0; i < nfiles; i++)
		{
			if (write (fd[i], buf, n) != n)
			{
				fprintf 
				(	stderr,
					"%s: Erro de escrita no arquivo \"%s\" (%s)\n",
					pgname, i == 0 ? "stdout" : argv[i], strerror (errno)
				);
				exit (1);
			}
		}
	}

	if (n < 0)
	{
		fprintf 
		(	stderr,
			"%s: Erro na leitura da entrada padrão (%s)\n",
			pgname, strerror (errno)
		);
		ret++;
	}

	/*
	 *	Fecha todos os arquivos
	 */
	for (i = 1; argv[i]; i++)
		close (fd[i]);

	return (ret);

}	/* end tee */

/*
 ****************************************************************
 *	Resumo de utilização do programa			*
 ****************************************************************
 */
void
help ()
{
	fprintf
	(	stderr,
		"%s - copia a entrada padrão para diversos arquivos e "
		 "a saida padrão\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-ai] [<arquivo> ...]\n",
		pgname, pgversion, pgname
	);
	fprintf
	(	stderr,
		"\nOpções:"
		"\t-a: adiciona ao final dos arquivos ao invés de "
			"reescreve-los\n"
		"\t-i: Ignora interrupções\n"
	);

	exit (2);

}	/* end help */
