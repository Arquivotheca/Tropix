/*
 ****************************************************************
 *								*
 *			grep.c					*
 *								*
 *	Procura por Expressões Regulares em Arquivos		*
 *								*
 *	Versão	1.0.0, de 21.01.87				*
 *		4.2.0, de 08.05.02				*
 *								*
 *	Módulo: grep						*
 *		Utilitarios Basicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Multiprocessado		*
 *		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <a.out.h>
#include <errno.h>

/*
 ****************************************************************
 *	Variáveis e definições globais				*
 ****************************************************************
 */
#define	LINESZ	1024	/* Tamanho de uma linha nos arquivos */
#define	MAXEXP	20	/* No. max. de expr. regulares */

#define	elif	else if

typedef	enum { NORMAL, COUNT, NUMBER, SILENT, LIST }	PRINT;

/*
 ******	Variáveis ***********************************************
 */
entry const char	pgversion[] =  "Versão:	4.2.0, de 08.05.02";

entry char		mflag;		/* Maíusculas e minúsculas iguais */
entry char		rflag;		/* Selecione as linhas SEM padrões */
entry char		Nflag;		/* Nomes dos pgms no <stdin> */

entry PRINT		print = NORMAL;	/* Modo de mostrar a ocorrência */

entry int		print_name;	/* Imprime ou não o nome do arquivo */

entry const char	*exp_list[MAXEXP+1]; /* Forma compilada das  expr. reg. */
entry int		nexpr = 0;	/* Número de expr. regulares */

entry int		exit_code = 1;	/* Valor para exit */

/*
 ******	Protótipos de funções ***********************************
 */
void			grep (const char *, FILE *);
int			isobj (FILE *);
char			*lower_conv (char *, const char *);
void			erro (void);
void			help (void);

/*
 ****************************************************************
 *	Procura por Expressões Regulares em Arquivos		*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	const char	*op;
	char		*p;
	int		opt;
	FILE		*fp;

	/*
	 *	Leitura das Opções e Argumentos
	 */
	while ((opt = getopt (argc, argv, "clnsmrNH")) != EOF)
	{
		switch (opt)
		{
		    case 'c':
			if (print == NORMAL)
				print = COUNT;
			else
				erro ();
			break;

		    case 'l':
			if (print == NORMAL)
				print = LIST;
			else
				erro ();
			break;

		    case 'n':
			if (print == NORMAL)
				print = NUMBER;
			else
				erro ();
			break;

		    case 's':
			if (print == NORMAL)
				print = SILENT;
			else
				erro ();
			break;

		    case 'm':			/* Maiúsculas e minúsculas iguais */
			mflag++;
			break;

		    case 'r':			/* Selecione linhas SEM os padrões */
			rflag++;
			break;

		    case 'N':			/* Nomes do <stdin> */
			Nflag++;
			break;

		    case 'H':
			help ();
			break;

		    case '?':
			putc ('\n', stderr);
			help ();
			break;

		}	/* end switch (opt) */

	}	/* end while getopt */

	argv += optind;

	/*
	 *	Separação das expressões regulares
	 */
	if ((op = *argv++) == NOSTR)
		help ();

	if (mflag)
		lower_conv ((char *)op, op);

	while ((p = strchr (op, '|')) != NOSTR  &&  nexpr < MAXEXP)
	{
		/*
		 *	Verifica se ocorrência de "\|", sem ser de "\\|"
		 */
	    testfalseor:
		if (p > op + 1  &&  *(p - 1) == '\\')
		{
			if (*(p -2) != '\\')
			{
				if ((p = strchr (p + 1, '|')) != NOSTR)
					goto testfalseor;
				else
					break;
			}
		}

		*p = '\0';

		/*
		 *	Compilação da Expressão Regular
		 */
		if ((exp_list[nexpr++] = regcmp (op)) == NOSTR)
			error ("$Não consigo compilar \"%s\"", op);

		*p++ = '|'; op = p;
	}

	/*
	 *	Compilação da Expressão Regular restante
	 */
	if ((exp_list[nexpr++] = regcmp (op)) == NOSTR)
			error ("$Não consigo compilar \"%s\"", op);

	exp_list[nexpr] = NOSTR;

	/*
	 *	Analisa o Nflag
	 */
	if (Nflag == 0)
	{
		if (*argv == NOSTR)
		{
		   /***	print_name = 0; ***/

			grep ("stdin", stdin);
			exit (exit_code);
		}
	}
	else
	{
		if (*argv != NOSTR)
		{
			error ("Os argumentos supérfluos serão ignorados");
			*argv = NOSTR;
		}
	}

	/*
	 *	Procura os argumentos
	 */
	if (*argv == NOSTR) 		 /* Não foram dados argumentos: lê os nomes do "stdin" */
	{
		char		*area = alloca (1024);

		print_name++;

		while (fngets (area, 1024, stdin) != NOSTR)
		{
			if ((fp = fopen (area, "r")) == NOFILE)
			{
				error ("*Não consigo abrir \"%s\"", area);
				exit_code = 2;
			}
			else
			{
				grep (area, fp);
				fclose (fp);
			}
		}
	}
	else 				 /* Foram dados argumentos */
	{
		if (argv[1])
			print_name++;

		for (/* vazio */; *argv; argv++)
		{
			if ((fp = fopen (*argv, "r")) == NOFILE)
			{
				error ("*Não consigo abrir \"%s\"", *argv);
				exit_code = 2;
			}
			else
			{
				grep (*argv, fp);
				fclose (fp);
			}
		}
	}

	return (exit_code);

}	/* end main */

/*
 ****************************************************************
 *	Procura a Expressão Regular em um Arquivo		*
 ****************************************************************
 */
void
grep (const char *file_nm, FILE *fp)
{
	const char	**exp;
	int		number = 0;	/* Ordem da linha atual no arquivo atual */
	int		count = 0;	/* No. de linhas onde a expr. (não) ocorreu */
	int		ocount;
	char		*line_ptr;
	char		read_line[LINESZ], cmp_line[LINESZ], match[LINESZ];
	STAT		s;

	/*
	 *	Se o arquivo não for a entrada padrão, ...
	 */
	if (fp != stdin)
	{
		if (fstat (fileno (fp), &s) < 0)
		{
			error ("*Não consigo obter o estado de \"%s\"", file_nm);
			exit_code = 2;
			return;
		}	

		/*
		 *	Verifica se o arquivo é regular
		 */
		if ((s.st_mode & S_IFMT) != S_IFREG)
		{
			error ("\"%s\" não é um arquivo regular", file_nm);
			exit_code = 2;
			return;
		}	

		/*
		 *	Testa se é um programa objeto apenas se não
		 *	for "stdin", pois não podemos dar "rewind" em um pipe
		 */
		if (isobj (fp))
		{
			error ("\"%s\" é um programa objeto", file_nm);
			exit_code = 2;
			return;
		}
	}

	/*
	 *	Lê as linhas do arquivo
	 */
	while (fngets (read_line, LINESZ, fp) != NOSTR)
	{
		number++; ocount = count;

		if (mflag)
			line_ptr = lower_conv (cmp_line, read_line);
		else
			line_ptr = read_line;

		for (exp = exp_list; *exp != NOSTR; exp++)
		{
			if (regex (*exp, line_ptr, match) != NOSTR)
			{
				if (!rflag)
					count++;
			}
			else
			{
				if (rflag)
					count++;
			}
		}

		if (count > ocount)
		{
			switch (print)
			{
			    case NORMAL:
				if (print_name)
					printf ("%s:\t%s\n", file_nm, read_line);
				else
					printf ("%s\n", read_line);
				break;

			    case NUMBER:
				printf
				(	"%s (%d):\t%s\n", file_nm,
					number,
					read_line
				);
				break;

			    case LIST:
				puts (file_nm);
				return;

			    default:
				break;
			}
		}

	}	/* end while fngets */

	if (print == COUNT)
		printf ("%s:\t%d\n", file_nm, count);

	if (exit_code == 1)
	{
		if   (!rflag && count > 0)
			exit_code = 0;
		elif (rflag && count < number)
			exit_code = 0;
	}

}	/* end grep */

/*
 ****************************************************************
 *	Testa se o arquivo é objeto				*
 ****************************************************************
 */
int
isobj (FILE *fp)
{
	HEADER		h;
	int		magic;

	if ((fread (&h, sizeof (HEADER), 1, fp)) == 1)
	{
		magic = h.h_magic;

		if (magic == FMAGIC || magic == NMAGIC || magic == SMAGIC)
			return (1);
	}

	rewind (fp);

	return (0);

}	/* end isobj */

/*
 ****************************************************************
 *	Converte uma cadeia para minúscula			*
 ****************************************************************
 */
char *
lower_conv (char *cmp_line, const char *read_line)
{
	int		c;
	const char	*bp;
	char		*cp;

	for (bp = read_line, cp = cmp_line; (c = *bp) != '\0'; bp++, cp++)
		*cp = tolower (c);

	*cp = '\0';

	return (cmp_line);

}	/* lower_conv */

/*
 ****************************************************************
 *	Aborta quando as opções são mutuamente exclusivas	*
 ****************************************************************
 */
void
erro (void)
{
	error ("$Opções mutuamente exclusivas");

}	/* end erro */

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
		"%s - busca de padrões em arquivos\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-clnsmrN] <padrão>[|<padrão> ...] [<arquivo> ...]\n",
		pgname, pgversion, pgname
	);

	fprintf
	(	stderr,
		"\nOpções:"
		"\t--: Imprime as linhas selecionadas\n"
		"\t-c: Conta as linhas selecionadas\n"
		"\t-l: Lista os nomes dos arquivos contendo linhas selecionadas\n"
		"\t-n: Precede à linha, o seu número\n"
		"\t-s: Modo silencioso, apenas o código de retôrno\n"
		"\t-m: Trata maísculas e minúsculas como iguais\n"
		"\t-r: Seleciona as linhas que não contém "
			"nenhum dos padrões\n"
		"\t-N: Lê os nomes dos arquivos de \"stdin\".\n"
		"\t    Se não forem dados arquivos, nem a opção \"-N\",\n"
		"\t    o conteúdo do arquivo será lido de \"stdin\"\n"
		"\nObs.:\tAs opções \"c\", \"l\", \"n\" e \"s\" "
		"são mutuamente exclusivas\n"
	);

	exit (2);

}	/* end help */
