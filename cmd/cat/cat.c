/*
 ****************************************************************
 *								*
 *			cat.c					*
 *								*
 *	Concatena e copia arquivos				*
 *								*
 *	Versão	1.0.1, de 06.03.86				*
 *		4.8.0, de 24.08.05				*
 *								*
 *	Módulo: cat						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Multiprocessado		*
 *		Copyright © 2005 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/syscall.h>
#include <sys/stat.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <a.out.h>
#include <errno.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão:	4.8.0, de 24.08.05";

#define		FDOUT	1	/* File descriptor da saída */

#define	NOSTR	(char *)NULL

entry int	vflag;		/* Verboso */
entry int	nflag;		/* Numeração */
entry int	Nflag;		/* Nomes pelo <stdin> */

entry dev_t	stdout_dev = -1;	/* posição fisica no disco */
entry ino_t	stdout_ino = -1;

entry int	offset;		/* Deslocamento */

/*
 ******	Protótipos de funções ***********************************
 */
int		copy (FILE *, const char *);
int		ncopy (FILE *, const char *);
int		scopy (FILE *, const char *);
void		help (void);

/*
 ****************************************************************
 *	Concatena e copia arquivos				*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	int		opt, status = 0;
	const char	*offset_str = NOSTR, *str;
	FILE		*fp;
	STAT		s;

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "no:NH")) != EOF)
	{
		switch (opt)
		{
		    case 'n':			/* Numera as linhas */
			nflag++;
			break;

		    case 'o':			/* Deslocamento */
			offset_str = optarg;
			break;

		    case 'N':			/* Nomes do <stdin> */
			Nflag++;
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
   /***	argc -= optind;	***/

	/*
	 *	Analisa o deslocamento
	 */
	if (offset_str != NOSTR)
	{
		if ((offset = strtol (offset_str, &str, 0)) < 0 || *str)
			error ("$Deslocamento inválido: \"%s\"", offset_str);
	}

	/*
	 *	Determina quem é o "stdout"
	 */
	fstat (FDOUT, &s);

	if (!S_ISCHR (s.st_mode) && !S_ISBLK (s.st_mode))
		{ stdout_dev = s.st_dev; stdout_ino = s.st_ino; }
	
	/*
	 *	Se não foram dados arquivos de entrada, lê a entrada padrão
	 */
	if (*argv == NOSTR && !Nflag)
		exit (copy (stdin, "stdin"));

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
	if (*argv == NOSTR) 		 /* Não foram dados argumentos: lê os nomes do <stdin> */
	{
		char		*area = alloca (1024);

		while (fngets (area, 1024, stdin) != NOSTR)
		{
			if ((fp = fopen (area, "r")) == NOFILE)
			{
				error ("*Não consegui abrir \"%s\"", area);
				status++;
			}
			else
			{
				status += copy (fp, area);
				fclose (fp);
			}

		}	/* end while */
	}
	else				/* Foram dados argumentos */
	{
		for (/* vazio */; *argv; argv++)
		{
			/*
			 *	'-' indica entrada padrão
			 */
			if (streq (*argv, "-"))
			{
				status += copy (stdin, "stdin");
				continue;
			}

			/*
			 *	Entrada não padrão
			 *	Tenta acessar o arquivo
			 */
			if ((fp = fopen (*argv, "r")) == NOFILE)
			{
				error ("*Não consegui abrir \"%s\"", *argv);
				status++;
			}
			else
			{
				status += copy (fp, *argv);
				fclose (fp);
			}
	
		}	/* end for */

	}	/* end 	if (*argv == NOSTR) */

	return (status);

}	/* end main */
				
/*
 ****************************************************************
 *	Prepara a cópia						*
 ****************************************************************
 */
int
copy (FILE *fp, const char *file_nm)
{
	HEADER		h;
	STAT		s;

	/*
	 *	Verifica se a entrada == saída
	 */
	if (fstat (fileno (fp), &s) < 0)
	{
		error ("*Não consegui obter o estado de \"%s\"", file_nm);
		return (1);
	}

	if (s.st_dev == stdout_dev && s.st_ino == stdout_ino)
	{
		error ("\"%s\" é tambem o arquivo de saída", file_nm);
		return (1);
	}

	/*
	 *	Testa se é um programa objeto
	 *	Apenas se não for "stdin" nem FIFO,
	 *	pois não podemos dar "rewind" em um pipe
	 */
	if (fp != stdin && !S_ISFIFO (s.st_mode))
	{
		if ((fread (&h, sizeof (HEADER), 1, fp)) == 1)
		{
			if (h.h_magic == FMAGIC || h.h_magic == NMAGIC || h.h_magic == SMAGIC)
			{
				error ("\"%s\" é um programa objeto", file_nm);
				return (1);
			}
		}

		rewind (fp);

	}	/* end 	if (fp != stdin) */

	return ((nflag || offset > 0) ? ncopy (fp, file_nm) : scopy (fp, file_nm));

}	/* end copy */

/*
 ****************************************************************
 *	Copia numerando ou com deslocamento			*
 ****************************************************************
 */
int
ncopy (FILE *fp, const char *file_nm)
{
	int		i, c, lineno = 1;

	/*
	 *	Copia o arquivo
	 */
	while ((c = getc (fp)) != EOF)
	{
		for (i = offset; i > 0; i--)
			putchar (' ');

		if (nflag)
			fprintf (stdout, "%6ld  ", lineno++);

		/*
		 *	Le e escreve uma linha
		 */
		while (c != EOF && c != '\n')
			{ putchar (c); c = getc (fp); }

		if (c == EOF)
			break;

		putchar ('\n');
	}

	if (ferror (fp))
	{
		error ("*Erro de leitura em \"%s\"", file_nm);
		return (1);
	}

	return (0);

}	/* end ncopy */

/*
 ****************************************************************
 *	Copia sem numerar					*
 ****************************************************************
 */
int
scopy (FILE *fp, const char *file_nm)
{
	int		n, fdin;
	char		area[BL4SZ];

	fdin = fileno (fp);

	lseek (fdin, 0, SEEK_SET);

	while ((n = read (fdin, area, sizeof (area))) > 0)
	{
		if (write (FDOUT, area, n) != n)
		{
			error ("*Erro na escrita em \"stdout\" ao copiar \"%s\"", file_nm);
			return (1);
		}
	}

	if (n < 0)
	{	
		error ("*Erro na leitura de \"%s\"", file_nm);
		return (1);
	}

	return (0);

}	/* end scopy */	

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
		"%s - Concatena e copia arquivos\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-nN] [-o <o>] [<arquivo> ...]\n",
		pgname, pgversion, pgname
	);
	fprintf
	(	stderr,
		"\nOpções:"
		"\t-n: Numera as linhas de saída\n"
		"\t-o: Desloca cada linha <o> caracteres para a direita\n"
		"\t-N: Lê os nomes dos arquivos de \"stdin\"\n"
		"\nObs.:\tUm <arquivo> com nome \"-\" representa a entrada padrão\n"
	);

	exit (2);

}	/* end help */
