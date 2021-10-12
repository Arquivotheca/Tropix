/*
 ****************************************************************
 *								*
 *			file.c					*
 *								*
 *	Tenta identificar o conteúdo de um arquivo		*
 *								*
 *	Versão  2.0.0, de 02.12.86				*
 *		4.2.0, de 24.04.02				*
 *								*
 *	Módulo: file						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Multiprocessado		*
 *		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/a.out.h>
#include <sys/syscall.h>

#include <stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão:	4.2.0, de 24.04.02";

entry int	exit_code = 0;	/* Código de retorno */

entry int	vflag;		/* Verbose */
entry int	Nflag;		/* Nomes dos pgms no <stdin> */

#define	elif	else if

/*
 ****** Área de leitura do arquivo ******************************
 */
#define	AREASZ	1024		/* Deve englobar o HEADER do "a.out" */

entry char	area_seq[AREASZ]; /* Área para análise de seqüências */

entry char	area_word[AREASZ]; /* Área para análise de palavras */

/*
 ****** Tabela de palavras chaves *******************************
 */
typedef	struct
{
	char	*c_seq[10];
	char	*c_word[8];

}	CARAC;

const CARAC	SROFF =
{
	"\n.he", "\n.fo", "\n.in", "\n.lp", "\n.pp", "\n.sp", "\n.b", NOSTR, NOSTR, NOSTR,
	NOSTR
};

const CARAC	C =
{
	"++", "!=", "&&", "||", "+=", "-=",  "*=", NOSTR, NOSTR, NOSTR,
	"<stdio.h>", "#define", "#include", "printf", NOSTR
};

const CARAC	SH =
{
	"#1", "&", ">>", "2>", NOSTR, NOSTR, NOSTR, NOSTR, NOSTR, NOSTR,
	"esac", "fi", NOSTR
};

const CARAC	PASCAL =
{
	":=", "(*", NOSTR, NOSTR, NOSTR, NOSTR, NOSTR, NOSTR, NOSTR, NOSTR,
	"var", "writeln", "readln", NOSTR
};

const CARAC	FORTRAN =
{
	NOSTR, NOSTR, NOSTR, NOSTR, NOSTR, NOSTR, NOSTR, NOSTR, NOSTR, NOSTR,
	"format", "subroutine", NOSTR
};

const CARAC	BASIC =
{
	"gosub", NOSTR, NOSTR, NOSTR, NOSTR, NOSTR, NOSTR, NOSTR, NOSTR, NOSTR,
	"rem", NOSTR
};

const CARAC	ASSEMBLER =
{
	"\n|*", "\n\t.", "\n\tmov", "$:", NOSTR, NOSTR, NOSTR, NOSTR, NOSTR, NOSTR,
	NOSTR
};

const CARAC	MAKEFILE =
{
	":\t", NOSTR, NOSTR, NOSTR, NOSTR, NOSTR, NOSTR, NOSTR, NOSTR, NOSTR,
	NOSTR
};

const CARAC	INGLES =
{
	"th", "wh", NOSTR, NOSTR, NOSTR, NOSTR, NOSTR, NOSTR, NOSTR, NOSTR,
	"have", "is", "are", "take", NOSTR
};

const CARAC	PORTUGUES =
{
	"ão", "õe", "lh", "ç", NOSTR, NOSTR, NOSTR, NOSTR, NOSTR, NOSTR,
	"de", "que", "para", NOSTR
};

const CARAC	FRANCES =
{
	NOSTR, NOSTR, NOSTR, NOSTR, NOSTR, NOSTR, NOSTR, NOSTR, NOSTR, NOSTR,
	"est", "et", "un", "il", "suis", "ont", "sont", NOSTR
};

const CARAC	ALEMAO =
{
	"sch", NOSTR, NOSTR, NOSTR, NOSTR, NOSTR, NOSTR, NOSTR, NOSTR, NOSTR,
	"ist", "ich", NOSTR
};

/*
 ****** Protótipos de funções ***********************************
 */
void		analysis_begin (const char *);
int		language_analysis (const CARAC *, int);
void		no_nl_print (char *);
void		help (void);

/*
 ****************************************************************
 *	Tenta identificar o conteúdo de um arquivo		*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	int		opt;

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
		{ error ("Os argumentos supérfluos serão ignorados\n"); *argv = NOSTR; }

	/*
	 *	Procura os argumentos
	 */
	if (*argv == NOSTR)		/* Não foram dados argumentos: lê do "stdin" */
	{
		char		*area = alloca (512);

		while (fngets (area, 512, stdin) != NOSTR)
			analysis_begin (area);
	}
	else
	{
		/*
		 *	Foram dados argumentos
		 */
		for (/* vazio */; *argv; argv++)
			analysis_begin (*argv);
	}

	return (exit_code);

}	/* end file */

/*
 ****************************************************************
 *	Inicia a análise de um arquivo				*
 ****************************************************************
 */
void
analysis_begin (const char *filenm)
{
	char		*cp;
	int		fd, sz;
	STAT		s;

	/*
	 *	Inicialmente, obtém o estado do arquivo
	 */
	printf ("\"%s\": \t", filenm);

	if (stat (filenm, &s) < 0)
	{
		printf ("Não consegui obter o estado (%s)\n", strerror (errno));
		exit_code++;
		return;
	}

	switch (s.st_mode & S_IFMT)
	{
	    case S_IFREG:
		break;

	    case S_IFDIR:
		printf ("É um diretório\n");
		return;

	    case S_IFIFO:
		printf ("É um FIFO\n");
		return;

	    case S_IFBLK:
		printf ("É um arquivo especial de blocos (estruturado)\n");
		return;

	    case S_IFCHR:
		printf ("É um arquivo especial de caracteres (não-estruturado)\n");
		return;

	    default:
		printf ("É um arquivo de TIPO INVÁLIDO\n");
		exit_code++;
		return;

	}	/* end switch */

	/*
	 *	Só sobrou o regular
	 */
	if (s.st_size == 0)
	{
		printf ("É um arquivo regular vazio\n");
		return;
	}

	if ((fd = open (filenm, O_RDONLY)) < 0)
	{
		printf ("Não consegui abrir (%s)\n", strerror (errno));
		exit_code++;
		return;
	}

	if ((sz = read (fd, area_seq, AREASZ)) < 0)
	{
		printf ("Não consegui ler (%s)\n", strerror (errno));
		exit_code++;
		close (fd);
		return;
	}

	close (fd);

	/*
	 *	Verifica se é um módulo objeto
	 */
	if (sz >= sizeof (HEADER))
	{
		int		magic = ((HEADER *)area_seq)->h_magic;

		if   (magic == FMAGIC || magic == NMAGIC)
		{
			printf ("É um módulo objeto\n");
			return;
		}
		elif (magic == SMAGIC)
		{
			printf ("É uma biblioteca compartilhada\n");
			return;
		}
	}

	/*
	 *	Identifica a área de palavras em tokens e separadores
	 */
	memset (area_seq + sz, '\0', AREASZ - sz);

	for (cp = area_seq; cp < area_seq + sz; cp++)
		*cp = tolower (*cp);

	memcpy (area_word, area_seq, AREASZ);

	for (cp = area_word; cp < area_word + sz; cp++)
	{
		switch (*cp)
		{
		    case ' ':
		    case '\n':
		    case '\t':
		    case ')':
		    case '(':
		    case ';':
		    case ',':
		    case ':':
			*cp = '\0';
		}
	}

	/*
	 *	Tenta as várias linguagens
	 */
	if (language_analysis (&SROFF, sz))
	{
		printf ("Aparentemente é um arquivo fonte para o SROFF\n");
		return;
	}

	if (language_analysis (&C, sz))
	{
		printf ("Aparentemente é um programa em \"C\"\n");
		return;
	}

	if (language_analysis (&SH, sz))
	{
		printf ("Aparentemente é um arquivo de comandos para o SH\n");
		return;
	}

	if (language_analysis (&PASCAL, sz))
	{
		printf ("Aparentemente é um programa em PASCAL\n");
		return;
	}

	if (language_analysis (&FORTRAN, sz))
	{
		printf ("Aparentemente é um programa em FORTRAN\n");
		return;
	}

	if (language_analysis (&BASIC, sz))
	{
		printf ("Aparentemente é um programa em BASIC\n");
		return;
	}

	if (language_analysis (&ASSEMBLER, sz))
	{
		printf ("Aparentemente é um programa em ASSEMBLY\n");
		return;
	}

	if (language_analysis (&MAKEFILE, sz))
	{
		printf ("Aparentemente é um MAKEFILE\n");
		return;
	}

	if (language_analysis (&INGLES, sz))
	{
		printf ("Aparentemente é um texto em INGLES\n");
		return;
	}

	if (language_analysis (&PORTUGUES, sz))
	{
		printf ("Aparentemente é um texto em PORTUGUÊS\n");
		return;
	}

	if (language_analysis (&FRANCES, sz))
	{
		printf ("Aparentemente é um texto em FRANCÊS\n");
		return;
	}

	if (language_analysis (&ALEMAO, sz))
	{
		printf ("Aparentemente é um texto em ALEMÃO\n");
		return;
	}

	printf ("Não consegui identificar\n");

}	/* end analysis_begin */

/*
 ****************************************************************
 *	Analisa uma linguagem					*
 ****************************************************************
 */
int
language_analysis (const CARAC *lp, int sz)
{
	char	* const	*cpp;
	char		*cp;
	char		c;
	int		n;

	/*
	 *	Procura palavras características
	 */
	for (cp = area_word; cp < area_word + sz; cp++)
	{
		if (*cp == '\0')
			continue;

		for (cpp = lp->c_word; *cpp != NOSTR; cpp++)
		{
			if (streq (cp, *cpp))
			{
				if (vflag)
					no_nl_print (*cpp);

				return (1);
			}
		}

		cp = strend (cp) - 1;
	}

	/*
	 *	Procura seqüências características
	 */
	for (cpp = lp->c_seq; *cpp != NOSTR; cpp++)
	{
		c = **cpp;	n = strlen (*cpp);

		for (cp = area_seq; cp < area_seq + sz; cp++)
		{
			if (*cp != c)
				continue;

			if (strncmp (cp, *cpp, n) == 0)
			{
				if (vflag)
					no_nl_print (*cpp);
				return (1);
			}
		}
	}

	return (0);

}	/* end language_analysis */

/*
 ****************************************************************
 *	Imprime um texto, sem os <nl>				*
 ****************************************************************
 */
void
no_nl_print (char *cp)
{
	printf ("(\"");

	for (/* vazio */; *cp != '\0'; cp++)
	{
		if (*cp == '\n')
			printf ("\\n");
		else
			putchar (*cp);
	}

	printf ("\") ");

}	/* end no_nl_print */

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
		"%s - tenta identificar o conteúdo de um arquivo\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-vN] [<arquivo> ...]\n",
		pgname, pgversion, pgname
	);
	fprintf
	(	stderr,
		"\nOpções:"
		"\t-v: Indica o elemento léxico que possibilitou a identificação\n"
		"\t-N: Lê os nomes dos <arquivo>s de \"stdin\"\n"
		"\t    Esta opção é implícita se não forem dados argumentos\n"
	);

	exit (2);

}	/* end help */
