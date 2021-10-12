/*
 ****************************************************************
 *								*
 *			a2ps.c					*
 *								*
 *	Converte arquivos texto para PostScript			*
 *								*
 *	Versão	4.3.0, de 28.05.02				*
 *		4.3.0, de 03.10.02				*
 *								*
 *	Módulo: a2ps						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pwd.h>
#include <time.h>
#include <errno.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
#define	elif		else if
#define	PAGE_ROUND(x,l)	((((x) + (l) - 1) / (l)) * (l))

#define	MAX_LINE_SIZE	95
#define	MAX_PAGE_SIZE	74
#define	MAX_MARGIN_SIZE	20
#define	DEF_TAB_SIZE	8

entry const char	pgversion[] =  "Versão: 4.3.0, de 03.10.02";

entry FILE		*psfile_fp;				/* Arquivo PostScript (saída) */
entry const char	*user_name;				/* Nome do usuário */
entry char		*todays_date;				/* Data de hoje */

entry int		lines_per_page	= MAX_PAGE_SIZE;	/* Nr. de linhas por página */
entry int		tab_size	= DEF_TAB_SIZE;		/* Tamanho do tabulador */
entry int		left_margin;				/* Tamanho da margem esquerda */
entry int		chars_per_line;				/* Nr. de chars. por linha */

entry int		page_number;				/* Nr. de páginas */
entry int		lines_printed;				/* Nr. de linhas impressas */

entry int		Nflag;					/* Nomes dos pgms no <stdin> */
entry int		bflag;					/* Quebra linhas */

/*
 ****** Protótipos de funções ***********************************
 */
void			help (void);
int			proc_file (const char *);
void			write_ps_prologue (void);
void			print_header (const char *);

/*
 ****************************************************************
 *	Programa Principal					*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	int		opt;
	int		exit_code = 0;
	const char	*psfile_name = NOSTR;
	PASSWD		*pwd;
	const char	*p;
	time_t		tm;

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "bc:F:l:o:t:NH")) != EOF)
	{
		switch (opt)
		{
		    case 'b':			/* Quebra linhas */
			bflag++;
			break;

		    case 'c':			/* Nr. de chars por linha */
			chars_per_line = strtol (optarg, &p, 0);

			if (*p != '\0' || chars_per_line < 10)
				error ("$Tamanho de linha inválido");
			break;

		    case 'F':			/* Output */
			psfile_name = optarg;
			break;

		    case 'l':			/* Nr. de linhas por página */
			lines_per_page = strtol (optarg, &p, 0);

			if (*p != '\0' || lines_per_page < 10 || lines_per_page > MAX_PAGE_SIZE)
				error ("$Número de linhas por página inválido");
			break;

		    case 'o':
			left_margin = strtol (optarg, &p, 0);

			if (*p != '\0' || left_margin < 0 || left_margin > MAX_MARGIN_SIZE)
				error ("$Tamanho da margem esquerda inválido");
			break;

		    case 't':
			tab_size = strtol (optarg, &p, 0);

			if (*p != '\0' || tab_size < 0 || tab_size > 16)
				error ("$Tamanho inválido para o tabulador");
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
		error ("Os argumentos supérfluos serão ignorados");

	/*
	 *	Confere o tamanho total das linhas
	 */
	if (chars_per_line == 0)
	{
		chars_per_line = MAX_LINE_SIZE - left_margin;
	}
	elif (chars_per_line + left_margin > MAX_LINE_SIZE)
	{
		error
		(	"$Tamanho excessivo para as linhas: %d + %d > %d!",
			left_margin, chars_per_line, MAX_LINE_SIZE
		);
	}

	/*
	 *	Obtém o nome do usuário
	 */
	if ((user_name = getenv ("USER")) == NOSTR)
		user_name = "Anônimo";
	elif ((pwd = getpwnam (user_name)) != NOPASSWD && pwd->pw_comment[0] != '\0')
		user_name = pwd->pw_comment;

	/*
	 *	Obtém a data
	 */
	time (&tm);
	todays_date = btime (&tm);
	todays_date[strlen (todays_date) - 1] = '\0';

	/*
	 *	Determina a saída
	 */
	if (psfile_name == NOSTR)
	{
		psfile_fp = stdout;
	}
	else
	{
		if ((psfile_fp = fopen (psfile_name, "w")) == NOFILE)
			error ("$*Não pude criar o arquivo \"%s\"", psfile_name);
	}

	/*
	 *	Escreve o prólogo
	 */
	write_ps_prologue ();

	/*
	 *	Processa os arquivos
	 */
	if (Nflag) 		/* Lê os nomes dos arquivos do <stdin> */
	{
		char		*area = alloca (1024);

		while (fngets (area, 1024, stdin) != NOSTR)
			exit_code += proc_file (area);
	}
	elif (*argv == NOSTR)	/* Não foram dados argumentos: usa o <stdin> */
	{
		exit_code += proc_file (NOSTR);
	}
	else			/* Foram dados argumentos */
	{
		for (/* vazio */; *argv; argv++)
			exit_code += proc_file (*argv);
	}

	/*
	 *	Escreve o epílogo
	 */
	fprintf
	(	psfile_fp,
		"sf (%d) 1 endcol\n(%d) endpage\n"
		"%%%%Trailer\n%%%%Pages: %d\n",
		page_number, page_number, page_number
	);

	if (psfile_fp != stdout)
		fclose (psfile_fp);

	return (exit_code);

}	/* end a2ps */

/*
 ****************************************************************
 *	Processa um arquivo					*
 ****************************************************************
 */
int
proc_file (const char *file_path)
{
	FILE		*fp;
	char		line[1024], *cp, c;
	int		col;
	char		*left_margin_blanks;

	/*
	 *	Abre o arquivo
	 */
	if (file_path == NOSTR)
		{ file_path = "<stdin>"; fp = stdin; }
	elif ((fp = fopen (file_path, "r")) == NOFILE)
		{ error ("*Não consegui abrir \"%s\"", file_path); return (-1); }

	/*
	 *	Prepara uma cadeia de brancos com o tamanho da margem esquerda
	 */
	left_margin_blanks = alloca (left_margin + 1);

	memset (left_margin_blanks, ' ', left_margin);	left_margin_blanks[left_margin] = '\0';

	/*
	 *	Começa na próxima página
	 */
	lines_printed = PAGE_ROUND (lines_printed, lines_per_page);

	/*
	 *	Lê o arquivo linha a linha
	 */
	while (fngets (line, sizeof (line), fp) != NOSTR)
	{
		/* Testa a quebra de página */

		if (line[0] == '\f')
		{
			lines_printed = PAGE_ROUND (lines_printed, lines_per_page);

			if (line[1] == '\0')
				continue;
		}

		if ((lines_printed % lines_per_page) == 0)
			print_header (file_path);

		col = 0; fprintf (psfile_fp, "(%s", left_margin_blanks);

		/*
		 *	Processa o conteúdo da linha
		 */
		for (cp = line; (c = *cp) != '\0'; cp++)
		{
			if (col >= chars_per_line)
			{
				/* Linha muito grande */

				if (!bflag)	/* Descarta em vez de quebrar */
					break;

				fprintf (psfile_fp, ") showline\n"); lines_printed++;

				/* Testa a quebra de página */

				if ((lines_printed % lines_per_page) == 0)
					print_header (file_path);

				/* Inicia nova linha */

				col = 0; fprintf (psfile_fp, "(%s", left_margin_blanks);
			}

			/* Processa o caractere */

			if (c >= 0x80)
			{
				fprintf (psfile_fp, "\\%o", c);
				col++;
			}
			else
			{
				switch (c)
				{
				    case '\f':
					lines_printed = PAGE_ROUND (lines_printed, lines_per_page) - 1;
					col = chars_per_line + 1;
					break;

				    case '\t':
					do
					{
						putc (' ', psfile_fp);
					}
					while ((++col % tab_size) != 0);

					break;

				    case '(':
				    case ')':
				    case '\\':
					putc ('\\', psfile_fp);

				    default:
					putc (c, psfile_fp);
					col++;
				}
			}
		}

		fprintf (psfile_fp, ") showline\n"); lines_printed++;
	}

	if (fp != stdin)
		fclose (fp);

	return (0);

}	/* end proc_file */

/*
 ****************************************************************
 *	Escreve o prólogo do arquivo de saída (PostScript)	*
 ****************************************************************
 */
void
write_ps_prologue (void)
{
	const char		**cp;
	extern const char	*ps_prologue[];

	fprintf
	(	psfile_fp,
		"%%!PS-Adobe-1.0\n"
		"/localeprolog 0 def\n"
		"%%!PS-Adobe-1.0\n"
		"%%%%PageLength %d\n"
		"%%%%LineLength %d\n"
		"%%%%DocumentFonts: Courier Helvetica-BoldOblique Times-Bold Times-Roman\n"
		"%%%%Pages: (atend)\n"
		"%%%%EndComments\n"
		"/inch { 72 mul } bind def\n"
		"/fullwidth 8.5 inch def\n"
		"/fullheight 11.7 inch def\n",
		lines_per_page, chars_per_line
	);

	for (cp = ps_prologue; *cp != NOSTR; cp++)
		fprintf (psfile_fp, "%s\n", *cp);

}	/* end write_ps_prologue */

/*
 ****************************************************************
 *	Imprime o cabeçalho					*
 ****************************************************************
 */
void
print_header (const char *file_path)
{
	if (page_number > 0)
	{
		fprintf
		(	psfile_fp,
			"sf (%d) 1 endcol\n"
			"(%d) endpage\n",
			page_number, page_number
		);
	}

	page_number++;

	fprintf
	(	psfile_fp,
		"/MailFor (Arquivo: ) def\n"
		"/User (%s) def\n"
		"/TimeNow (%s) def\n"
		"/Subject () def\n",
		file_path, todays_date
	);


	fprintf
	(	psfile_fp,
		"%%%%Page: ? %d\n"
		"%d newpage\n"
		"sf ",
		page_number, page_number
	);

}	/* end print_header */

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
		"%s - converte arquivos texto em PostScript\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-bN] [-c <nc>] [-l <nl>] [-F <ps>] [-o <mar>]\n"
		"\t     [-t <tab>] [<arquivo> ...]\n",
		pgname, pgversion, pgname
	);

	fprintf
	(	stderr,
		"\nOpções:"
		"\t-b: Quebra em várias as linhas muito grandes\n"
		"\t-c: Produz linhas com <nc> caracteres    (default: %d)\n"
		"\t-F: A saída é produzida em <ps>          (default: \"stdout\")\n"
		"\t-l: Produz páginas com <nl> linhas       (default: %d)\n"
		"\t-o: Margem esquerda de <mar> caracteres  (default: 0)\n"
		"\t-t: Utiliza o tabulador de tamanho <tab> (default: %d)\n"
		"\t-N: Lê os nomes dos <arquivo>s da entrada padrão\n",
		MAX_LINE_SIZE, MAX_PAGE_SIZE, DEF_TAB_SIZE
	);

	fprintf
	(	stderr,
		"\nObs.:\tSe não forem dados <arquivo>s nem a opção -N,\n"
		"\to texto a ser convertido é lido do \"stdin\"\n"
	);

	exit (2);

}	/* end help */
