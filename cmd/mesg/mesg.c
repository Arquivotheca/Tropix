/*
 ****************************************************************
 *								*
 *			mesg.c					*
 *								*
 *	Habilita ou desabilita a recepção de mensagens		*
 *								*
 *	Versão	1.0.0, de 30.05.86				*
 *		3.0.0, de 23.05.97				*
 *								*
 *	Módulo: mesg						*
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
entry int	nflag;		/* Desabilita */
entry int	sflag;		/* Habilita */

/*
 ****** Protótipos de funções ***********************************
 */
void		help (void);

/*
 ****************************************************************
 *	Habilita ou desabilita a recepção de mensagens		*
 ****************************************************************
 */
int
main (int argc, register const char *argv[])
{
	register int	opt;
	register char	*tty_nm;
	STAT		s;

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "nNsSyYvH")) != EOF)
	{
		switch (opt)
		{
		    case 'n':			/* Desabilita */
		    case 'N':
			if (sflag)
				help ();
			nflag++;
			break;

		    case 's':			/* Habilita */
		    case 'S':
		    case 'y':
		    case 'Y':
			if (nflag)
				help ();
			sflag++;
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
	 *	Obtém o estado do terminal
	 */
	if ((tty_nm = ttyname (2 /* == STDERR */)) == NOSTR)
	{
		fprintf
		(	stderr,
			"%s: Não consegui obter o nome do terminal (%s)\n",
			pgname, strerror (errno)
		);
		exit (2);
	}

	if (stat (tty_nm, &s) < 0)
	{
		fprintf
		(	stderr,
			"%s: Não consegui obter o estado do terminal \"%s\" (%s)\n",
			pgname, tty_nm, strerror (errno)
		);
		exit (2);
	}

	/*
	 *	Imprime o estado atual
	 */
	if (nflag + sflag == 0)
	{
		if (s.st_mode & S_IWOTH)
		{
			fprintf (stderr, "Recepção habilitada\n");
		   /***	exit_code = 0; ***/
		}
		else
		{
			fprintf (stderr, "Recepção desabilitada\n");
			exit_code = 1;
		}
	}

	/*
	 *	Altera o estado
	 */
	if   (nflag)
	{
		exit_code = 1;

		if (chmod (tty_nm, s.st_mode & ~(S_IWGRP|S_IWOTH)) < 0)
		{
			fprintf
			(	stderr,
				"%s: Não consegui mudar o estado do terminal \"%s\" (%s)\n",
				pgname, tty_nm, strerror (errno)
			);
			exit_code = 2;
		}
	}
	elif (sflag)
	{
	   /***	exit_code = 0; ***/

		if (chmod (tty_nm, s.st_mode|S_IWGRP|S_IWOTH) < 0)
		{
			fprintf
			(	stderr,
				"%s: Não consegui mudar o estado do terminal \"%s\" (%s)\n",
				pgname, tty_nm, strerror (errno)
			);
			exit_code = 2;
		}
	}

	return (exit_code);

}	/* end mesg */

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
		"%s - habilita ou desabilita a recepção de mensagens\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-nNsSyY]\n",
		pgname, pgversion, pgname
	);
	fprintf
	(	stderr,
		"\nOpções:"
		"\t-n (N)      : Desabilita a recepção\n"
		"\t-s (S, y, Y): Habilita a recepção\n"
		"\nObs.:\tSem opções, imprime o estado atual\n"
	);

	exit (2);

}	/* end help */
