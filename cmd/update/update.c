/*
 ****************************************************************
 *								*
 *			update.c				*
 *								*
 *	Atualiza os sistemas de arquivos periodicamente		*
 *								*
 *	Versão	1.0.0, de 27.06.86				*
 *		3.0.1, de 14.07.97				*
 *								*
 *	Módulo: update						*
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
#include <signal.h>
#include <fcntl.h>
#include <errno.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão:	3.0.1, de 14.07.97";

#define PERIOD	30		/* Período Default */

#define	NUFILE	20		/* No. max. de arquivos abertos */

#define	NOVOID	(void *)NULL
#define	elif	else if
#define	EVER	;;
#define STR(x)	# x
#define XSTR(x)	STR (x)

entry int	vflag;		/* Verbose */

entry int	period = PERIOD; /* Período efetivo */

/*
 ******	Lista de Diretórios Mantidos sempre na Tabela INODE *****
 */
const char	*direc_tb[] =
{
	"/bin",
	"/usr",
	"/usr/bin",
	""
};

/*
 ****** Protótipos de funções ***********************************
 */
void		help (void);
void		period_sync (int, ...);

/*
 ****************************************************************
 *	Atualiza os sistemas de arquivos periodicamente		*
 ****************************************************************
 */
int
main (int argc, register const char *argv[])
{
	register int		opt, fd;
	register const char	**dpp;
	const char		*str;

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
	 *	Verifica se o UID é de superusuário
	 */
	if (geteuid () != 0)
	{
		fprintf
		(	stderr,
			"%s: O usuário efetivo não é SUPERUSUÁRIO!\n",
			pgname
		);
		exit (1);
	}

	/*
	 *	Retira o Período (em Segundos)
	 */
	if (argc > 1)
		help ();

	if (argv[0] != NOSTR)
	{
		if ((period = strtol (argv[0], &str, 0)) <= 0 || *str)
		{
			fprintf
			(	stderr,
				"%s: Período inválido: \"%s\" "
				"(será usado um período de " XSTR (PERIOD)
				" segundos)\n",
				pgname, argv[0]
			);
			period = PERIOD;
		}
	}

	/*
	 *	Cria um filho para realizar a atualização
	 */
	if (fork ())
		exit (0);

	/*
	 *	Abre os diretórios permanentes
	 */
#undef	DEBUG
#ifdef	DEBUG
	for (fd = 3; fd < NUFILE; fd++)
		close (fd);
#else
	for (fd = 0; fd < NUFILE; fd++)
		close (fd);
#endif	DEBUG

	for (dpp = direc_tb; *dpp[0] != '\0'; dpp++)
	{
		if (open (*dpp, O_RDONLY) < 0)
		{
			fprintf
			(	stderr,
				"%s: Não consegui abrir \"%s\" (%s)\n",
				pgname, *dpp, strerror (errno)
			);
		}
	}

	/*
	 *	Chama a Rotina para a atualização
	 */
	period_sync (0);

	for (EVER)
		pause ();

	/* Nunca dá exit */

}	/* end update */

/*
 ****************************************************************
 *	Atualiza						*
 ****************************************************************
 */
void
period_sync (int signo, ...)
{
#ifdef	DEBUG
	fprintf (stderr, "%s: Fazendo SYNC\n", pgname);
#else
	if (sync () < 0)
	{
		fprintf
		(	stderr,
			"%s: Erro na chamada ao sistema SYNC (%s)\n",
			pgname, strerror (errno)
		);
	}
#endif	DEBUG

	signal (SIGALRM, period_sync);

	alarm (period);

}	/* end period_sync */

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
		"%s - atualiza os sistemas de arquivos periodicamente\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [<n>]\n",
		pgname, pgversion, pgname
	);
	fprintf
	(	stderr,
		"\nObs.:\tSe o período <n> (em segundos) de atualização\n"
		"\tnão for dado, será usado o valor de "
		XSTR (PERIOD) " segundos\n"
	);

	exit (2);

}	/* end help */
