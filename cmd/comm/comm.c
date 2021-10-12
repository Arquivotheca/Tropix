/*
 ****************************************************************
 *								*
 *			comm.c					*
 *								*
 *	Seleciona linhas comuns a 2 arquivos ordenados		*
 *								*
 *	Versão	2.3.0, de 19.06.90				*
 *		3.0.0, de 10.04.97				*
 *								*
 *	Módulo: comm						*
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
const char	pgversion[] =  "Versão:	3.0.0, de 10.04.97";

#define	elif	else if

/*
 ******	Indicadores *********************************************
 */
entry int	vflag;		/* Verbose */
entry int	flag1 = 1;	/* Imprime a coluna 1 */
entry int	flag2 = 1;	/* Imprime a coluna 2 */
entry int	flag3 = 1;	/* Imprime a coluna 3 */

/*
 ****** Protótipos de funções ***********************************
 */
void		print_line (const char *, int);
void		help (void);

/*
 ****************************************************************
 *	Seleciona linhas comuns a 2 arquivos ordenados		*
 ****************************************************************
 */
int
main (int argc, register const char *argv[])
{
	register int	opt, i;
	register FILE	*fp0, *fp1;
	register char	*not_eof0, *not_eof1;
	char		buf0[BLSZ], buf1[BLSZ];

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "v123H")) != EOF)
	{
		switch (opt)
		{
		    case 'v':			/* Verbose */
			vflag++;
			break;

		    case '1':			/* Suprime a coluna 1 */
			flag1 = 0;
			break;

		    case '2':			/* Suprime a coluna 2 */
			flag2 = 0;
			break;

		    case '3':			/* Suprime a coluna 3 */
			flag3 = 0;
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
	 *	Abre os arquivos
	 */
	if (argc != 2)
		help ();

	if   (streq (argv[0], "-"))
	{
		fp0 = stdin;
	}
	elif ((fp0 = fopen (argv[0], "r")) == NOFILE)
	{
		fprintf
		(	stderr,
			"%s: Não consegui abrir \"%s\" (%s)\n",
			pgname, argv[0], strerror (errno)
		);
		exit (1);
	}

	if   (streq (argv[1], "-"))
	{
		if   (fp0 == stdin)
		{
			fprintf
			(	stderr,
				"%s: Não pode ser dado \"-\" duas vezes\n",
				pgname
			);
			exit (1);
		}
		fp1 = stdin;
	}
	elif ((fp1 = fopen (argv[1], "r")) == NOFILE)
	{
		fprintf
		(	stderr,
			"%s: Não consegui abrir \"%s\" (%s)\n",
			pgname, argv[1], strerror (errno)
		);
		exit (1);
	}

	/*
	 *	Inicia a análise dos arquivos
	 */
	if (!flag1 && !flag2 && !flag3)
		exit (0);

	not_eof0 = fngets (buf0, BLSZ, fp0);
	not_eof1 = fngets (buf1, BLSZ, fp1);

	while (not_eof0 && not_eof1)
	{
		/*
		 *	Os dois arquivos ainda tem linhas
		 */
		if   ((i = strcmp (buf0, buf1)) == 0)
		{
			/*   
			 *	Linhas  iguais	
			 */
			if (flag3)
				print_line (buf0, 3);

			not_eof0 = fngets (buf0, BLSZ, fp0);
			not_eof1 = fngets (buf1, BLSZ, fp1);
		}
		elif (i > 0)		
		{
			/*
			 *	Linha em buf0 maior que linha em buf1
			 */
			if (flag2)
				print_line (buf1, 2);

			not_eof1 = fngets (buf1, BLSZ, fp1);
		}
		else
		{
			/*	
			 *	Linha em buf1 maior que linha em buf0
			 */
			if (flag1)
				print_line (buf0, 1);

			not_eof0 = fngets (buf0, BLSZ, fp0);
		}

	}	/* end while */

	/*
	 *	Um dos arquivos acabou
	 */
	if   (not_eof0)			
 	{
		/*
		 *	O arquivo 0 não acabou
		 */
		do
		{
			if (flag1)
				print_line (buf0, 1);
		}
		while (fngets (buf0, BLSZ, fp0));

	}
	elif (not_eof1)
 	{
		/*
		 *	O arquivo 1 não acabou
		 */
		do
		{
			if (flag2)
				print_line (buf1, 2);
		}
		while (fngets (buf1, BLSZ, fp1));
	}

	fclose (fp0);
	fclose (fp1);

	return (0);

}	/* end comm */

/*
 ****************************************************************
 *	Imprime a linha						*
 ****************************************************************
 */
void
print_line (const char *area, int place)
{
	switch (place)
	{
	    case 3:
		fputc ('\t', stdout);
	    case 2:
		fputc ('\t', stdout);
	    case 1:
		puts (area);
	}

}	/* end print_line */

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
		"%s - "
		"seleciona ou rejeita linhas comuns a dois arquivos ordenados\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-123] <arquivo1> <arquivo2>\n",
		pgname, pgversion, pgname
	);
	fprintf
	(	stderr,
		"\nOpções:"
		"\t-1: Suprime a coluna 1\n"
		"\t-2: Suprime a coluna 2\n"
		"\t-3: Suprime a coluna 3\n"
		"\nObs.:\tUm dos <arquivo>s pode ser \"-\" "
			"para indicar a entrada padrão\n"
	);

	exit (2);

}	/* end help */
