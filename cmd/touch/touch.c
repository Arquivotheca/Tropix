/*
 ****************************************************************
 *								*
 *			touch.c					*
 *								*
 *	Programa para modificar os tempos dos arquivos		*
 *								*
 *	Versão	1.0.0, de 22.01.87				*
 *		4.2.0, de 08.05.02				*
 *								*
 *	Módulo: touch						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <ctype.h>
#include <errno.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão:	4.2.0, de 08.05.02";

entry int	exitcode = 0;	/* Código de retorno */

entry int	vflag;		/* Verbose */
entry int	cflag;		/* Criação */
entry int	Nflag;		/* Nomes dos pgms no <stdin> */

const int	ndiasmes[] =
{
	31, 28, 31, 30, 31, 30,	31, 31, 30, 31, 30, 31
};

entry time_t	TEMPO;		/* Tempo do arquivo */

entry int	tempo_dado;	/* O tempo foi dado */

/*
 ******	Protótipos de funções ***********************************
 */
void		help (void);
void		proc_file (const char *);
time_t 		get_time (const char *);

/*
 ****************************************************************
 *	Programa para modificar os tempos dos arquivos		*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	int		opt, n;
	const char	*str;
	STAT		s;

	time (&TEMPO);

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "vcd:r:f:NH")) != EOF)
	{
		switch (opt)
		{
		    case 'v':			/* Verbose */
			vflag++;
			break;

		    case 'c':			/* Criação */
			cflag++;
			break;

		    case 'd':			/* Comando data */
			if (tempo_dado++)
				help ();

			TEMPO = get_time (optarg);
			break;

		    case 'r':			/* Dias relativos */
			if (tempo_dado++)
				help ();

			if ((n = strtol (optarg, &str, 0)) <= 0 || *str != '\0')
				error ("$Número de <dias> inválido: \"%s\"", optarg);

			TEMPO -= n * 60 * 60 * 24;
			break;

		    case 'f':			/* Arquivo <base> */
			if (tempo_dado++)
				help ();

			if (stat (optarg, &s) < 0)
				error ("$*Não consegui obter o estado de \"%s\"", optarg);

			TEMPO = s.st_mtime;
			break;

		    case 'N':			/* Nomes do <stdin> */
			Nflag++;
			break;

		    case 'H':			/* Help */
			help ();

		    default:			/* Erro */
			fputc ('\n', stderr);
			help ();

		}	/* end switch */

	}	/* end while */

	argv += optind;

	if (vflag)
		printf ("%s", btime (&TEMPO));

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
	if (*argv == NOSTR) 		/* Não foram dados argumentos: lê do "stdin" */
	{
		char		*area = alloca (1024);

		while (fngets (area, 1024, stdin) == area)
			proc_file (area);
	}
	else				/* Foram dados argumentos */
	{
		for (/* vazio */; *argv != NOSTR; argv++)
			proc_file (*argv);
	}

	return (exitcode);

}	/* end touch */

/*
 ****************************************************************
 *	Processa um arquivo					*
 ****************************************************************
 */
void
proc_file (const char *file_nm)
{
	STAT		s;

	if (vflag)
		printf ("%s:\n", file_nm);

	/*
	 *	Se o arquivo ainda não existe, cria
	 */
	if (access (file_nm, F_OK) < 0 && creat (file_nm, 0666) < 0)
	{
		error ("*Não consegui criar o arquivo \"%s\"", file_nm);
		exitcode++;
		return;
	}

	/*
	 *	Examina se é simplesmente para atualiza a data
	 */
	if (tempo_dado == 0 && cflag == 0)
	{
		if (utime (file_nm, (time_t *)NULL) == 0)
			return;

		goto bad;
	}

	if (stat (file_nm, &s) < 0)
	{
		error ("*Não consegui obter o estado de \"%s\"", file_nm);
		exitcode++;
		return;
	}

	s.st_atime = TEMPO;
	s.st_mtime = TEMPO;

	if (cflag)
		s.st_ctime = TEMPO;

	if (utime (file_nm, (time_t *)&s.st_atime) == 0)
		return;

    bad:
	error ("*Erro na troca de tempos de \"%s\"", file_nm);
	exitcode++;

}	/* end proc_file */

/*
 ****************************************************************
 *	Rotina para obter o tempo				*
 ****************************************************************
 */
time_t
get_time (const char *cp)
{
	int		i,j;
	TM 		*t;
	const char	*p;
	char		*str;
	char		strloc[2];
	long		numanos, bissexto, n29fev;
	long		fuso;
	time_t		tempo;

	t = localtime (&TEMPO);

	fuso = gettzmin () * 60;

	t->tm_sec = 0;

	for (i = 0, p = cp; *p != '\0'; i++, p++)
	{
		if (isdigit (*p))
			continue;

		if (*p == ':' || *p == '.')
		{
			t->tm_sec = atoi (++p);

			if (t->tm_sec > 59)
				help ();
			p--;
			break;
		}

		help ();
	}

	if ((i % 2) != 0)
		help ();

	p--;
	strloc[2] = '\0';

	j = 0;
	while (i >= 2)
	{
		str = &strloc[1];
		*str-- = *p--;
		*str   = *p--;

		switch (j)
		{
		    case 0:
			t->tm_min = atoi (str);
			if (t->tm_min > 59)
				help ();
			break;

		    case 1:
			t->tm_hour = atoi (str);
			if (t->tm_hour > 23)
				help ();
			break;

		    case 2:
			t->tm_mday = atoi (str);
			if (t->tm_mday > 31)
				help ();
			break;

		    case 3:
			t->tm_mon = atoi (str) - 1;
			if (t->tm_mon > 11)
				help ();
			break;

		    case 4:
			t->tm_year = atoi (str);
			if (t->tm_year > 37 && t->tm_year < 70)
				help ();
			break;

		    default:
			help ();

		}	/* end switch (j) */

		i -= 2;
		j++;

	}	/* end while */

	numanos = t->tm_year - 70;

	if (numanos < 0)
		numanos += 100;

	if (((numanos + 2) % 4) == 0)
		bissexto = 1;
	else
		bissexto = 0;

	if (t->tm_mday > ndiasmes[t->tm_mon])
	{
		if (t->tm_mon != 1 || t->tm_mday != 29 || bissexto != 1)
				help ();
	}

	n29fev = (numanos + 1) / 4;

	tempo = (numanos * 365 + n29fev);

	t->tm_yday = tempo;

	for (i = 0; i < t->tm_mon; i++)
	{
		tempo += ndiasmes[i];

		if (i == 1)
			tempo += bissexto;
	}

	tempo += t->tm_mday - 1;

	t->tm_yday = tempo - t->tm_yday;

	t->tm_wday = (tempo + 4) % 7;

	tempo = ((tempo * 24) + t->tm_hour) * 60;
	tempo = (tempo + t->tm_min) * 60 + t->tm_sec;
	tempo -= fuso;

	return (tempo);

}	/* end get_time */

/*
 ****************************************************************
 *	Resumo de Utilização do Programa			*
 ****************************************************************
 */
void
help (void)
{
	fprintf
	(	stderr,
		"%s - Modifica os tempos de arquivos\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-vcN] [-d <data>] [-r <dias>] "
			"[-f <base>] [<arquivo> ...]\n",
		pgname, pgversion, pgname
	);
	fprintf
	(	stderr,
		"\nOpções:"
		"\t-v: Imprime os nomes dos <arquivo>s\n"
		"\t-c: Modifica também o tempo de criação\n"
		"\t-N: Lê os nomes dos <arquivo>s de \"stdin\"\n"
		"\t    Esta opção é implícita se não forem dados <arquivo>s\n"
		"\t--: Data atual\n"
		"\t-d: Data no formato do comando \"data\"\n"
		"\t-r: Data relativa (<dias> no passado)\n"
		"\t-f: Utiliza a data de modificação do arquivo <base>\n"
	);

	exit (2);

}	/* end help */
