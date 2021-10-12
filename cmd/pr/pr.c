/*
 ****************************************************************
 *								*
 *			pr.c					*
 *								*
 *	Imprime arquivos					*
 *								*
 *	Versão	1.0.0, de 17.04.86				*
 *		4.7.0, de 30.12.04				*
 *								*
 *	Módulo: pr						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/stat.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <a.out.h>
#include <time.h>
#include <errno.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão:	4.7.0, de 30.12.04";

#define	elif	else if

#define STR(x)	# x
#define XSTR(x)	STR (x)

#define	STDOUT	1
#define	STDIN	0

#define	PAGESZ	75		/* Tamanho total da página */
#define	UNUSEDSZ 10		/* Tamanho não usado para texto */

entry int	exit_code = 0;	/* Código de retorno */

entry int	hflag;		/* Cabeçalho dado */
entry int	vflag;		/* Verbose */
entry int	Nflag;		/* Nomes dos pgms no <stdin> */

/*
 ****** Variáveis diversas **************************************
 */
entry const char *header;	/* Header */

entry int	init_pageno = 1; /* Número da página inicial */

entry int	pagesz = PAGESZ - UNUSEDSZ;	/* Linhas por página */ 

entry int	pagecnt;	/* Contador de páginas */

entry int	lastpage = 0;	/* Última página */

entry int	linecnt;	/* Contador de linhas */

entry int	offset;		/* Deslocamento */

entry dev_t	stdout_dev;	/* Dispositivo da saída padrão */
entry ino_t	stdout_ino;

/*
 ****** Protótipos de funções ***********************************
 */
void		help (void);
int		proc_file (const char *, FILE *, STAT *);

/*
 ****************************************************************
 *	Imprime arquivos					*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	int			opt;
	FILE			*fp;
	const char		*pageno_str = NOSTR;
	const char		*lastpage_str = NOSTR;
	const char		*pagesz_str = NOSTR;
	const char		*offset_str = NOSTR;
	const char		*str;
	STAT			s;

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "o:p:e:h:l:vNH")) != EOF)
	{
		switch (opt)
		{
		    case 'o':			/* Deslocamento */
			offset_str = optarg;
			break;

		    case 'p':			/* Página de início  */
			pageno_str = optarg;
			break;

		    case 'e':			/* Página de final  */
			lastpage_str = optarg;
			break;

		    case 'h':			/* Cabeçalho */
			header = optarg;
			hflag++;
			break;

		    case 'l':			/* Comprimento da página */
			pagesz_str = optarg;
			break;

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
	 *	Analisa o deslocamento
	 */
	if (offset_str != NOSTR)
	{
		if ((offset = strtol (offset_str, &str, 0)) < 0 || *str)
			error ("$Deslocamento inválido: \"%s\"", offset_str);
	}

	/*
	 *	Analisa o número de página dado
	 */
	if (pageno_str != NOSTR)
	{
		if ((init_pageno = strtol (pageno_str, &str, 0)) <= 0 || *str)
			error ("$Número de página inicial inválido: \"%s\"", pageno_str);
	}

	/*
	 *	Analisa o número da última página
	 */
	if (lastpage_str != NOSTR)
	{
		if ((lastpage = strtol (lastpage_str, &str, 0)) < init_pageno || *str)
			error ("$Número de página final inválido: \"%s\"", lastpage_str);
	}

	/*
	 *	Analisa o tamanho de página dado
	 */
	if (pagesz_str != NOSTR)
	{
		if ((pagesz = strtol (pagesz_str, &str, 0) - UNUSEDSZ) <= 0 || *str)
			error ("$Tamanho de página inválido: \"%s\"", pagesz_str);
	}

	/*
	 *	Determina quem é o "stdout"
	 */
	fstat (STDOUT, &s);

	stdout_dev = s.st_dev;
	stdout_ino = s.st_ino;

	/*
	 *	Analisa o Nflag
	 */
	if   (Nflag)
	{
		if (*argv != NOSTR)
		{
			error ("Os argumentos supérfluos serão ignorados");
			*argv = NOSTR;
		}
	}
	elif (*argv == NOSTR)
	{
		/*
		 *	Não foram dados argumentos, nem a opção "-N"
		 */
		fstat (STDIN, &s);	time (&s.st_mtime);

		exit (proc_file ("stdin", stdin, &s));
	}

	/*
	 *	Procura os argumentos
	 */
	if (*argv == NOSTR)		/* Não foram dados argumentos: lê do "stdin" */
	{
		char		*area = alloca (1024);

		while (fngets (area, 1024, stdin) != NOSTR)
		{
			if ((fp = fopen (area, "r")) == NOFILE)
			{
				error ("*Não consegui abrir \"%s\"", area);
				continue;
			}
	
			fstat (fileno (fp), &s);

			exit_code += proc_file (area, fp, &s);

			fclose (fp);
		}
	}
	else				/* Foram dados argumentos */
	{
		/*
		 *	Foram dados argumentos
		 */
		for (/* vazio */; *argv; argv++)
		{
			if (streq (*argv, "-"))
			{
				fstat (STDIN, &s);	time (&s.st_mtime);

				exit_code += (proc_file ("stdin", stdin, &s));

				continue;
			}

			if ((fp = fopen (*argv, "r")) == NOFILE)
			{
				error ("*Não consegui abrir \"%s\"", *argv);
				continue;
			}
	
			fstat (fileno (fp), &s);

			exit_code += proc_file (*argv, fp, &s);

			fclose (fp);
		}
	}

	return (exit_code);

}	/* end pr */

/*
 ****************************************************************
 *	Processa uma arquivo					*
 ****************************************************************
 */
int
proc_file (const char *filenm, FILE *fp, STAT *sp)
{
	int			i, c;
	HEADER			h;
	const char		*filetime;

	/*
	 *	Testa se é um diretório
	 */
	if (S_ISDIR (sp->st_mode))
	{
		error ("\"%s\" é um diretório", filenm);
		return (1);
	}

	/*
	 *	Verifica se não está se tentanto imprimir a própria saída!
	 */
	if (fp != stdin)
	{
		if (sp->st_dev == stdout_dev  &&  sp->st_ino == stdout_ino)
		{
			error ("\"%s\" é o próprio arquivo de saída", filenm);
			return (1);
		}
	}

	/*
	 *	Testa se é um programa objeto apenas se não for
	 *	"stdin"   (não podemos dar "rewind" em um pipe)
	 */
	if (fp != stdin && !S_ISFIFO (sp->st_mode))
	{
		if ((fread (&h, sizeof (h), 1, fp)) == 1)
		{
			if (h.h_magic == FMAGIC || h.h_magic == NMAGIC)
			{
				error ("\"%s\" é um programa objeto", filenm);
				return (1);
			}
		}

		rewind (fp);
	}

	/*
	 *	Obtém a data do arquivo
	 */
	filetime = btime (&sp->st_mtime);

	/*
	 *	Se não foi dado "header", é usado o nome do arquivo
	 */
	if (!hflag)
		header = filenm; 

	/*
	 *	Le até a página onde deve comecar a impressão
	 */
	pagecnt = 1; 	c = ' ';

	while (init_pageno > pagecnt  &&  c >= 0 /* != EOF */)
	{
		/*
		 *	Lê uma página
		 */
		linecnt = 1;

		while (linecnt <= pagesz  &&  c >= 0 /* != EOF */)
		{
			/*
			 *	Le uma linha
			 */
			while ((c = getc (fp)) >= 0 /* != EOF */  &&  c != '\n')
			{
				if (c == '\f')
				{
					if ((c = getc (fp)) != '\n')
						ungetc (c, fp);

					linecnt = pagesz - 1;
					break;
				}
			}

			linecnt++;
		}

		pagecnt++;
	}

	/*
	 *	Verifica se ainda não acabou o arquivo
	 */
	if (c < 0 /* == EOF */)
		return (0);

	/*
	 *	Imprime até o fim do arquivo
	 */
	for (c = getc (fp); c >= 0 /* != EOF */; pagecnt++)
	{
		/*
		 *	Imprime o cabecalho
		 */
		printf ("\n\n");

		for (i = offset; i > 0; i--)
			putchar (' ');

		printf
		(	"%-24.24s     %s     pag. %d\n\n\n",
			filetime, header, pagecnt
		);

		/*
		 *	Imprime uma página
		 */
		linecnt = 1;

		while (linecnt <= pagesz)
		{
			/*
			 *	Coloca o "offset"
			 */
			for (i = offset; i > 0; i--)
				putchar (' ');

			/*
			 *	Imprime uma linha
			 */
			while (c >= 0 /* != EOF */  &&  c != '\n'  &&  c != '\f')
			{
				putchar (c);
				c = getc (fp);
			}

			/*
			 *	Examina os caracteres EOF, <nl> e <np>
			 */
			switch (c)
			{
			    case '\n':
				putchar ('\n');
				c = getc (fp);
				linecnt++;
				break;

			    case EOF:
				while (linecnt++ <= pagesz)
					putchar ('\n');
				break;

			    case '\f':
				while (linecnt++ <= pagesz)
					putchar ('\n');
				if ((c = getc (fp)) == '\n')
					c = getc (fp);
				break;
			}
		
		}

		printf ("\n\n\n\n\n");

		/*
		 *	Verifica se chegou à última página
		 */
		if (lastpage > 0  &&  pagecnt >= lastpage)
			return (0);

	} 	/* end imprimindo as páginas */

	return (0);

}	/* end proc_file */

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
		"%s - imprime arquivos\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-o <o>] [-p <p>] [-e <e>] [-h \"<h>\"] "
			"[-l <l>] [-N] [<arquivo> ...]\n",
		pgname, pgversion, pgname
	);
	fprintf
	(	stderr,
		"\nOpções:"
		"\t-o: Desloca cada linha <o> caracteres para a direita\n"
		"\t-p: Começa a imprimir a partir da página <p>\n"
		"\t-e: Imprime somente até a página <e>\n"
		"\t-h: Utiliza <h> como cabeçalho das páginas\n"
		"\t    (ao invés dos nomes dos respectivos <arquivo>s)\n"
		"\t-l: Usa a página com <l> linhas "
			"(ao invés de " XSTR (PAGESZ) ")\n"
		"\t-N: Lê os nomes dos arquivos de \"stdin\"\n"
		"\nObs.:\tUm <arquivo> com nome \"-\" " 
			"representa a entrada padrão\n"
	);

	exit (2);

}	/* end help */
