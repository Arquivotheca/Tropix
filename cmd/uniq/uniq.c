/*
 ****************************************************************
 *								*
 *			uniq.c					*
 *								*
 *	Indica linhas repetidas de um arquivo			*
 *								*
 *	Versão	2.0.0, de 03.09.85				*
 *		3.0.0, de 13.07.97				*
 *								*
 *	Módulo: uniq						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Multiprocessado		*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão:	3.0.0, de 13.07.97";

#define	LINE_SZ	1024

#define	NOVOID	(void *)NULL
#define	elif	else if
#define	EVER	;;
#define STR(x)	# x
#define XSTR(x)	STR (x)

entry int	nflag;		/* Apenas um exemplar de cada linha repetida */
entry int	uflag;		/* Escreve apenas as linhas não repetidas */
entry int	dflag;		/* Um exemplar somente das linhas repetidas */
entry int	cflag;		/* Contagem de freqüência */
entry int	vflag;		/* Verbose */

/*
 ****** Protótipos de funções ***********************************
 */
void		help (void);
const char	*field_char_skip (const char *, int, int);

/*
 ****************************************************************
 *	Indica linhas repetidas de um arquivo			*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	register int	opt, repetition;
	register int	field_cnt = 0, char_cnt = 0;
	char		line_buf_1[LINE_SZ], line_buf_2[LINE_SZ];
	char		*line_1 = line_buf_1, *line_2 = line_buf_2;
	const char	*field_1, *field_2;
	const char	*field_str = NOSTR, *char_str = NOSTR;
	int		flag_total;
	const char	*str;
	char		*t;

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "nudcf:b:vH")) != EOF)
	{
		switch (opt)
		{
		    case 'n':	/* Apenas um exemplar de cada linha repetida */
			nflag++;
			break;

		    case 'u':	/* Escreve apenas as linhas não repetidas */
			uflag++;
			break;

		    case 'd':	/* Um exemplar somente das linhas repetidas */
			dflag++;
			break;

		    case 'c':	/* Contagem de freqüência */
			cflag++;
			break;

		    case 'f':			/* Campos a pular */
			field_str = optarg;
			break;

		    case 'b':			/* Bytes a pular */
			char_str = optarg;
			break;

		    case 'v':			
			vflag++;
			break;

		    case 'H':			/* Help */
			help ();

		    default:			/* Erro */
			putc ('\n', stderr);
			help ();

		}	/* end switch */

	}	/* end while */

	/*
	 *	Consistência das opções/argumentos
	 */
	argv += optind;
	argc -= optind;

	if (argc > 2)
		help ();

	if ((flag_total = nflag + uflag + dflag) > 1)
		help ();

	if (flag_total == 0) 
		nflag++;

	if (nflag)
		{ uflag++; dflag++; }

	/*
	 *	Analisa o número de campos a pular
	 */
	if (field_str != NOSTR)
	{
		if ((field_cnt = strtol (field_str, &str, 0)) < 0 || *str != '\0')
		{
			fprintf
			(	stderr,
				"%s: Número de campos inválido: \"%s\"\n",
				pgname, field_str
			);
			exit (1);
		}
	}

	/*
	 *	Analisa o número de caracteres a pular
	 */
	if (char_str != NOSTR)
	{
		if ((char_cnt = strtol (char_str, &str, 0)) < 0 || *str != '\0')
		{
			fprintf
			(	stderr,
				"%s: Número de caracteres inválido: \"%s\"\n",
				pgname, char_str
			);
			exit (1);
		}
	}

	/*
	 *	Abre o arquivo de entrada
	 */
	if (argc > 0  &&  !streq (argv[0], "-"))
	{
		if (freopen (argv[0], "r", stdin) == NOFILE)
		{
			fprintf
			(	stderr,
				"%s: Não consegui abrir \"%s\" (%s)\n",
				pgname, argv[0], strerror (errno)
			);
			exit (1);
		}
	}

	/*
	 *	Abre o arquivo de saída
	 */
	if (argc > 1)
	{
		if (freopen (argv[1], "w", stdout) == NOFILE)
		{
			fprintf
			(	stderr,
				"%s: Não consegui abrir \"%s\" (%s)\n",
				pgname, argv[1], strerror (errno)
			);
			exit (1);
		}
	}

	/*
	 *	Lê a primeira linha
	 */
	repetition = 0;

	if (fngets (line_1, LINE_SZ, stdin) == NOSTR)
	{
		if (ferror (stdin))
		{
			fprintf
			(	stderr,
				"%s: Erro de leitura da <entrada> (%s)\n",
				pgname, strerror (errno)
			);
			exit (1);
		}

		exit (0);
	}

	field_1 = field_char_skip (line_1, field_cnt, char_cnt);

	/*
	 *	Vai lendo as linhas restantes, comparando sempre com a anterior
	 */
	while (fngets (line_2, LINE_SZ, stdin) != NOSTR)
	{
		field_2 = field_char_skip (line_2, field_cnt, char_cnt);

		if (streq (field_2, field_1))
			{ repetition++; continue; }

		if (dflag && repetition || uflag && !repetition)
		{
			if (cflag)
				printf ("%6d  ", repetition + 1);

			puts (line_1);
		}

		repetition = 0;

		field_1  = field_2;

		t = line_2; line_2 = line_1; line_1 = t;

	}	/* end while */

	/*
	 *	Escreve a última linha
	 */
	if (dflag && repetition || uflag && !repetition)
	{
		if (cflag)
			printf ("%6d  ", repetition + 1);

		puts (line_1);
	}

	/*
	 *	Encerra a operação
	 */
	if (ferror (stdin))
	{
		fprintf
		(	stderr,
			"%s: Erro de leitura da <entrada> (%s)\n",
			pgname, strerror (errno)
		);
		exit (1);
	}

	fflush (stdout);

	if (ferror (stdout))
	{
		fprintf
		(	stderr,
			"%s: Erro de escrita na <saída> (%s)\n",
			pgname, strerror (errno)
		);
		exit (1);
	}

	return (0);

}	/* end uniq */

/*
 ****************************************************************
 *	Pula os campos/bytes					*
 ****************************************************************
 */
const char *
field_char_skip (const char *cp, int field_cnt, int char_cnt)
{
	if (field_cnt)
	{
		while (*cp == ' ' || *cp == '\t')
			cp++;

		while (field_cnt-- > 0)
		{
			while (*cp != ' ' && *cp != '\t' && *cp != '\0')
				cp++;

			while (*cp == ' ' || *cp == '\t')
				cp++;
		}
	}

	while (char_cnt-- > 0 && *cp != '\0')
		cp++;

	return (cp);

}	/* field_char_skip */

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
		"%s - indica linhas repetidas de um arquivo\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-nudc] [-f <f>] [-b <b>] [<entrada> [<saída>]]\n",
		pgname, pgversion, pgname
	);
	fprintf
	(	stderr,
		"\nOpções:"
		"\t-n: Mantém apenas um exemplar de cada linha repetida (modo normal)\n"
		"\t-u: Escreve apenas as linhas não repetidas\n"
		"\t-d: Escreve apenas um exemplar somente das linhas repetidas\n"
		"\t-c: Adiciona uma contagem de freqüência como prefixo\n"
		"\t-f: Na comparação, ignora os <f> campos iniciais\n"
		"\t-b: Na comparação, ignora os <b> caracteres iniciais\n"
		"\nObs.:\tSe <entrada> for \"-\", será lida a entrada padrão\n"
	);

	exit (2);

}	/* end help */
