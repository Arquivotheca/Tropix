/*
 ****************************************************************
 *								*
 *			t.c					*
 *								*
 *	Programa de teste do mkfifo				*
 *								*
 *	Versão	2.3.0, de 31.10.90				*
 *		3.0.0, de 23.05.97				*
 *								*
 *	Modulo: MKFIFO						*
 *		Utilitarios Basicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Multiprocessado		*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>

/*
 ****************************************************************
 *	Definições Gerais					*
 ****************************************************************
 */
#define		elif	else if

const char	pgversion[] =  "Versão: 3.0.0, de 23.05.97";

entry const char *pgname;
entry char	area[512];

/*
 ******	Protótipos de funções ***********************************
 */
void		help (void);

/*
 ****************************************************************
 *	Programa Principal					*
 ****************************************************************
 */
int
main (int argc, register const char *argv[])
{
	register int	n;
	STAT		s;
	int		oflag, opt, fdin, fdout;

	pgname = argv[0];

	oflag = O_RDONLY;	/* == 0 */

	/*
	 *	Analisa as Opções
	 */
	while ((opt = getopt (argc, argv, "drwH")) != EOF)
	{
		switch (opt)
		{
		    case 'd':			/* No delay */
			oflag |= O_NDELAY;
			break;

		    case 'r':			/* leitura do fifo */
			oflag |= O_RDONLY;
			break;

		    case 'w':			/* escrita no fifo */
			oflag |= O_WRONLY;
			break;

		    case 'H':			/* help */
			help ();

		    default:			/* erro */
			putc ('\n', stderr);
			help ();
		}

	}	/* end while */

	argv += optind;

	if (*argv == NOSTR)
		help ();

	if (stat (*argv, &s) < 0)
	{
		fprintf
		(	stderr,
			"%s: Não consegui obter o estado de \"%s\" (%s)\n",
			pgname, *argv, strerror (errno)
		);
		exit (1);
	}

	if ((s.st_mode & S_IFMT) != S_IFIFO)
	{
		fprintf (stderr, "%s: %s não é fifo\n", pgname, *argv);
		exit (2);
	}

	fdin = 0;

	if ((fdout = open (*argv, oflag)) == 0)
	{
		fprintf
		(	stderr,
			"%s: Não consegui abrir \"%s\" (%s)\n",
			pgname, *argv, strerror (errno)
		);
		exit (1);
	}

	if (oflag & O_WRONLY)
	{
		fprintf (stderr, "Escrevendo no fifo\n");
	}
	else
	{
		fprintf (stderr, "Lendo do fifo\n");
		fdin = fdout;
		fdout = 1;
	}

	while ((n = read (fdin, area, 512)) > 0)
	{
		if (write (fdout, area, n) != n)
		{
			fprintf
			(	stderr,
				"%s: Não consegui escrever em stdout (%s)\n",
				pgname, strerror (errno)
			);
		}
	}

	if (fdout == 1)
		close (fdin);
	else
		close (fdout);

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
		"%s - programa de teste do mkfifo\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-rw] <fifo>\n",
		pgname, pgversion, pgname
	);

	fprintf
	(	stderr,
		"\nOpções:"
		"\t-r: Le de um fifo e escreve em stdout\n"
		"\t-w: Le de stdin e escreve em um fifo\n"
	);

	exit (2);

}	/* end help */
