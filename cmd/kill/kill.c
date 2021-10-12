/*
 ****************************************************************
 *								*
 *			kill.c					*
 *								*
 *	Envia sinais a processos				*
 *								*
 *	Versão	1.0.0, de 07.01.87				*
 *		3.0.1, de 08.05.97				*
 *								*
 *	Módulo: kill						*
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
#include <ctype.h>
#include <string.h>
#include <errno.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão:	3.0.1, de 08.05.97";

entry int	exit_code = 0;	/* Código de retorno */

entry int	vflag;		/* Verbose */

/*
 ****** Protótipos de funções ***********************************
 */
int		get_signo (const char *);
void		help (void);

/*
 ****************************************************************
 *	Envia sinais a processos				*
 ****************************************************************
 */
int
main (int argc, register const char *argv[])
{
	register int	opt;
	int		signo = 0;
	const char	*str, *signm = "???";
	long		pid;

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "s:vH")) != EOF)
	{
		switch (opt)
		{
		    case 's':			/* Número do sinal */
			if (signo != 0)
				help ();

			if ((signo = get_signo (signm = optarg)) < 0)
			{
				fprintf
				(	stderr,
					"%s: Nome de sinal inválido: \"%s\"\n",
					pgname, optarg
				);
				exit (1);
			}
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
	 *	Se não foi dado o sinal ...
	 */
	if (signo == 0)
		{ signo = SIGTERM; signm = "SIGTERM"; }

	signal (signo, SIG_IGN);

	if (vflag)
		printf ("sinal = %d\n", signo);

	/*
	 *	Envia o sinal para os processos
	 */
	for (/* vazio */; *argv != NOSTR; argv++)
	{
		if ((pid = strtol (*argv, &str, 0)) < 0  ||  *str != '\0')
		{
			fprintf
			(	stderr,
				"%s: Número de processo inválido: \"%s\"\n",
				pgname, *argv
			);
			exit_code++;
			continue;
		}

		if (kill (pid, signo) < 0)
		{
			fprintf
			(	stderr,
				"%s: Não consegui enviar o sinal \"%s\" "
				"para o processo \"%s\" (%s)\n",
				pgname, signm, *argv, strerror (errno)
			);
			exit_code++;
		}

	}	/* end for */

	return (exit_code);

}	/* end kill */

/*
 ****************************************************************
 *	Retira o número do sinal				*
 ****************************************************************
 */
int
get_signo (const char *signm)
{
	register char		*cp;
	register int		signo, sz;
	char			*signm_cpy;
	const char		*str;

	/*
	 *	Examina se é dado na forma numérica
	 */
	if ((signo = strtol (signm, &str, 0)) > 0 && signo <= NSIG && *str == '\0')
		return (signo);

	/*
	 *	Examina se é dado na forma simbólica
	 */
	if ((signm_cpy = strdup (signm)) == NOSTR)
	{
		fprintf (stderr, "%s: Não obtive memória\n", pgname);
		exit (1);
	}

	for (cp = signm_cpy; *cp != '\0'; cp++)
		*cp = toupper (*cp);

	if (strncmp (signm_cpy, "SIG", 3) == 0)
		signm_cpy += 3;

	sz = strlen (signm_cpy);

	/*
	 *	Repare que é bem dependente da tabela de "sigtostr"
	 */
	for (signo = 1; signo <= NSIG; signo++)
	{
		str = sigtostr (signo) + 3;

		if (strncmp (str, signm_cpy, sz) == 0  &&  str[sz] == ':')
				return (signo);
	}

	return (-1);

}	/* end get_signo */

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
		"%s - envia sinais a processos\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-s <sinal>] <pid> ...\n",
		pgname, pgversion, pgname
	);
	fprintf
	(	stderr,
		"\nOpções:"
		"\t-s: Nome do <sinal> (SIGTERM (15) por omissão)\n"
		"\nObs.:\tO sinal pode ser dado na forma numérica "
			"(por ex. \"22\", \"0x16\", \"026\")\n"
		"\tou simbólica (por ex. \"SIGABRT\", \"ABRT\", "
			"\"sigabrt\" ou \"abrt\")\n"
	);

	exit (2);

}	/* end help */
