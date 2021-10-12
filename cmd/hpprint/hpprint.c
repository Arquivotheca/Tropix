/*
 ****************************************************************
 *								*
 *			hpprint.c				*
 *								*
 *	Utilitário para impressão na HP DeskJet (PCL)		*
 *								*
 *	Versão	3.0.0, de 27.12.96				*
 *		4.7.0, de 30.12.04				*
 *								*
 *	Módulo: hpprint						*
 *		Utilitários Supérfluos				*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2004 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */


#include <sys/types.h>
#include <sys/syscall.h>

#include <termio.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

/*
 ****************************************************************
 *	Nome e versão do programa				*
 ****************************************************************
 */
const char	pgversion[] = "Versão 4.7.0, de 30.12.04";

/*
 ****************************************************************
 *	Comandos PCL						*
 ****************************************************************
 */
#define	PCL_RESET	"\033E"			/* Reset */
#define	PCL_QUALITY	"\033(s%dQ"		/* Qualidade de impressão */
#define	PCL_LATIN1	"\033(0N"		/* Latin 1 */
#define	PCL_A4		"\033&l%sA"		/* A4 */
#define	PCL_UPMARGIN	"\033&l0E"		/* Margem superior = 0 linhas */
#define	PCL_VSTEP	"\033&l%dC"		/* Passo vertical */
#define	PCL_PITCH	"\033&k%dS"		/* Pitch */
#define	PCL_EJECT	"\033&l0H"		/* Pula página */

/*
 ****************************************************************
 *	Variáveis globais					*
 ****************************************************************
 */
const char	*printername	= "/dev/lp";	/* Dispositivo de impressão */
FILE		*printer;

/*
 *****	Parâmetros de impressão  *******************************
 */
int	linesperpage	=	75;		/* Linhas / página */
int	quality		=	1;		/* Qualidade de impressão */
int	pitch		=	0;		/* Pitch normal */
int	tab		=	8;		/* Tabulador */
int	margin		=	0;		/* Margem esquerda */

int	vflag;					/* Verboso */
int	Nflag;					/* Lê os nomes do "stdin" */

char	fname[1024];				/* Buffer para ler os nomes */
char	blanks[] = "                    ";	/* Para fazer margem esquerda */

/*
 *****	Estatística da impressão  *******************************
 */
int	totalnpages;				/* Número total de páginas */
int	nfiles;					/* Número de arquivos */

/*
 *****	Características dos tipos de papel  *********************
 */
typedef	enum
{
	A4,
	LETTER

}	PAPERTYPE;

typedef	struct
{
	char	p_name[8];			/* Nome do papel */

	int	p_horizontal_printable_area,	/* Porção útil para impressão */
		p_vertical_printable_area;	/* na DeskJet 660C (* 100) */

	char	p_init_string[4];		/* Cadeia de inicialização */

}	PAPERCHAR;

PAPERCHAR	paper_char [] =
{
	{ "A4",    800, 1107, "26" },
	{ "a4",    800, 1107, "26" },
	{ "CARTA", 800, 1037, "2"  },
	{ "carta", 800, 1037, "2"  },
	{ "" }
};

PAPERCHAR	*paper	=	paper_char;	/* Default = A4 */

/*
 ****************************************************************
 *	Protótipos de funções					*
 ****************************************************************
 */
int	check_integer_argument (char *, int, int);
void	help (int);
int	initialize_printer (void);
int	doprint (const char *);
int	printfile (const char *, FILE *);
void	set_paper_type (const char *);

/*
 ****************************************************************
 *	Programa principal					*
 ****************************************************************
 */
void
main (int argc, const char *argv[])
{
	int	opt, exitval;

	while ((opt = getopt (argc, argv, "CMNHqcvp:P:l:t:o:")) != EOF)
	{
		switch (opt)
		{
		    case 'C':
			quality = 0;
			linesperpage = 75;
			margin = 16;
			pitch = 2;
			break;

		    case 'M':
			quality = 0;
			linesperpage = 75;
			margin = 16;
			pitch = 2;
			break;

		    case 'N':
			Nflag++;
			break;

		    case 'H':
			help (0);

		    case 'q':
			quality = 2;
			break;

		    case 'c':
			pitch = 2;
			break;

		    case 'v':
			vflag++;
			break;

		    case 'p':
			set_paper_type (optarg);
			break;

		    case 'P':
			printername = optarg;
			break;

		    case 'l':
			linesperpage =	check_integer_argument
					(	"o número de linhas por página",
						20, 90
					);
			break;

		    case 't':
			tab =		check_integer_argument
					(	"o tabulador",
						2, 8
					);
			break;

		    case 'o':
			margin =	check_integer_argument
					(	"o tamanho da margem esquerda",
						0, 16
					);
			break;

		    default:
			help (1);
		}
	}

	argc -= optind;
	argv += optind;

	if (initialize_printer ())
		error ("$impressora %s não disponível", printername);

	blanks[margin] = '\0';

	exitval = 0;

	if (Nflag)
	{
		if (*argv != NULL)
			error ("$com a opção -N não podem ser dados arquivos");

		while (fngets (fname, sizeof (fname), stdin) != NULL)
			exitval += doprint (fname);
	}
	else if (*argv == NULL)
	{
		exitval = printfile ("stdin", stdin);
	}
	else
	{
		for (/* sem inicialização */; *argv != NULL; argv++)
			exitval += doprint (*argv);
	}

	if (vflag && nfiles > 1)
	{
		printf
		(	"\n***** Total: %d arquivos, %d páginas\n",
			nfiles,
			totalnpages
		);
	}

	exit (exitval);

}	/* end main */

/*
 ****************************************************************
 *	Analisa um argumento inteiro				*
 ****************************************************************
 */
int
check_integer_argument (char *purpose, int lower, int upper)
{
	int		n;

	n = atoi (optarg);

	if (n < lower || n > upper)
	{
		error
		(	"$valor %d inválido para %s",
			n, purpose
		);
	}

	return (n);

}	/* end check_integer_argument */

/*
 ****************************************************************
 *	Inicializa a impressora					*
 ****************************************************************
 */
int
initialize_printer (void)
{
	int		fd;
	TERMIO		term;

	/*
	 *	Abre a impressora e coloca em modo RAW.
	 */
	if ((printer = fopen (printername, "w")) == NULL)
		return (1);

	fd = fileno (printer);

	if (isatty (fd))
	{
		if (ioctl (fd, TCGETS, &term) < 0)
			return (1);

		term.c_oflag &= ~OPOST;

		if (ioctl (fd, TCSETS, &term) < 0)
			return (1);
	}

	/*
	 *	Envia as cadeias de inicialização.
	 */
	fprintf
	(	printer,
		PCL_RESET
		PCL_QUALITY
		PCL_LATIN1
		PCL_A4
		PCL_UPMARGIN
		PCL_VSTEP
		PCL_PITCH,
		quality,
		paper->p_init_string,
		(paper->p_vertical_printable_area * 48) / (linesperpage * 100),
		pitch
	);

	fflush (printer);

	return (0);

}	/* end initialize_printer */

/*
 ****************************************************************
 *	Abre o arquivo e imprime				*
 ****************************************************************
 */
int
doprint (const char *name)
{
	FILE	*fp;
	int	val;

	if ((fp = fopen (name, "r")) == NULL)
		return (1);

	val = printfile (name, fp);
	fclose (fp);

	return (val);

}	/* end doprint */

/*
 ****************************************************************
 *	Imprime um arquivo					*
 ****************************************************************
 */
int
printfile (const char *name, FILE *fp)
{
	int		c, nchar, nlines, npages;

	if (vflag)
	{
		printf ("%s:\t", name);
		fflush (stdout);
	}

	nchar = nlines = 0;
	npages = 1;

	while ((c = getc (fp)) != EOF)
	{
		if (ferror (printer))
			return (1);

		switch (c)
		{
		    case '\f':
			fputs ("\r\033&l0H", printer);
			fflush (printer);

			npages++;
			nlines = nchar = 0;
			break;

		    case '\n':
			if (++nlines >= linesperpage)
			{
				fputs ("\r\033&l0H", printer);
				fflush (printer);

				npages++;
				nlines = 0;
			}
			else
				fputs ("\r\n", printer);

			nchar = 0;
			break;

		    case '\r':
			putc (c, printer);
			nchar = 0;
			break;

		    case '\t':
			if (nchar == 0)
				fputs (blanks, printer);

			for (c = tab - (nchar % tab); c > 0; c--, nchar++)
				putc (' ', printer);
			break;

		    default:
			if (nchar++ == 0)
				fputs (blanks, printer);

			putc (c, printer);
			break;
		}
	}

	if (nlines != 0)
	{
		fputs ("\r\033&l0H", printer);
		fflush (printer);
	}

	if (vflag)
	{
		printf ("%d páginas\n", npages);
		totalnpages += npages;
		nfiles++;
	}

	return (0);

}	/* end printfile */

/*
 ****************************************************************
 *	Obtém o tipo de papel a ser usado na impressão		*
 ****************************************************************
 */
void
set_paper_type (const char *paper_name)
{
	PAPERCHAR	*pp;

	for (pp = paper_char; pp->p_name[0] != '\0'; pp++)
	{
		if (strcmp (pp->p_name, paper_name) == 0)
		{
			paper = pp;
			return;
		}
	}

	error ("$tipo de papel desconhecido: %s", paper_name);

}	/* end set_paper_type */

#if (0)	/*******************************************************/
/*
 ****************************************************************
 *	Imprime mensagens de erro				*
 ****************************************************************
 */
void
error (char *msg, ...)
{
	int	doexit;
	va_list	vp;

	va_start (vp, msg);

	doexit = 0;
	if (*msg == '$')
		doexit++, msg++;

	fprintf (stderr, "%s: ", pgname);
	vfprintf (stderr, msg, vp);
	putc ('\n', stderr);

	va_end (vp);

	if (doexit)
		exit (1);

}	/* end error */
#endif	/*******************************************************/

/*
 ****************************************************************
 *	Resumo de Utilizacao do Programa			*
 ****************************************************************
 */
void
help (int exitval)
{
	fprintf
	(	stderr,
		"%s - impressor para HP DeskJet (PCL)\n\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-qvcN] [-P <dev>] [-p <papel>] [-l <nl>]\n"
		"\t                [-t <tab>] [-o <nc>] [-CM] "
		"[<arquivo> ...]\n",
		pgname, pgversion, pgname
	);

	fprintf
	(	stderr,
		"\nOpções:"
		"\t-q: impressão com boa qualidade (default = rascunho)\n"
		"\t-v: verboso (imprime estatísticas na saída padrão)\n"
		"\t-c: utiliza caracteres comprimidos\n"
		"\t-N: obtém os nomes dos arquivos a imprimir da entrada padrão\n"
		"\n"
		"\t-P: imprime no dispositivo <dev>            (default = /dev/lp)\n"
		"\t-p: <papel> = A4 ou CARTA                   (default = A4)\n"
		"\t-l: utiliza <nl> linhas/página              (default = 75)\n"
		"\t-t: utiliza <tab> como tamanho do tabulador (default = 8)\n"
		"\t-o: utiliza margem esquerda de <nc> colunas (default = 0)\n"
		"\n"
		"\t-C: impressão de programas em C (-c -l 75 -o 16)\n"
		"\t-M: impressão de manuais        (-c -l 75 -o 16)\n"
	);

	fprintf
	(	stderr,
		"\nSe não forem dados arquivos nem a opção -N, é impressa a entrada padrão\n"
	);

	exit (exitval);

}	/* end help */
