/*
 ****************************************************************
 *								*
 *			emptydir.c				*
 *								*
 * 	Gera os nomes dos diretórios vazios (para remoção)	*
 *								*
 *	Versão	3.1.0, de 14.01.99				*
 *		4.2.0, de 08.05.02				*
 *								*
 *	Módulo: emptydir					*
 *		Utilitários experimentais			*
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
#include <dirent.h>
#include <ftw.h>
#include <fcntl.h>
#include <errno.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão:	4.2.0, de 08.05.02";

#define	EVER	;;
#define	elif	else if
#define STR(x)	# x
#define XSTR(x)	STR (x)

/*
 ******	Variáveis globais ***************************************
 */
entry int	gflag;		/* Gera o garfield */
entry int	vflag;		/* Verbose */
entry int	Nflag;		/* Nomes dos pgms no <stdin> */

entry int	n_dirs;		/* No. total de diretórios vazios */

/*
 ****** Garfield ************************************************
 */
const char	garfield[] = "garfield";
entry FILE	*gfp;

/*
 ******	Protótipos de funções ***********************************
 */
int		accept (const char *, STAT *, int);
void		dir_read (const char *, STAT *);
void		help (void);

/*
 ****************************************************************
 * 	Gera os nomes dos diretórios vazios (para remoção)	*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	int		opt;

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "gvNH")) != EOF)
	{
		switch (opt)
		{
		    case 'g':			/* garfield */
			gflag++;
			continue;

		    case 'v':			/* verbose flag */
			vflag++;
			continue;

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

	/*
	 *	Ajustes
	 */
   /***	argc -= optind; ***/
	argv += optind;

	/*
	 *	Prepara o "garfield"
	 */
	if (gflag && (gfp = fopen (garfield, "w")) == NOFILE)
		error ("$*Não consegui abrir \"%s\"", garfield);

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
	if (*argv == NOSTR) 		 /* Não foram dados argumentos: lê do "stdin" */
	{
		char		*area = alloca (1024);

		while (fngets (area, 1024, stdin) != NOSTR)
		{
			ftw (area, accept);
		}
	}
	else 				 /* Foram dados argumentos */
	{
		for (/* vazio */; *argv; argv++)
		{
			ftw (*argv, accept);
		}
	}

	/*
	 *	Imprime o total
	 */
	if (!vflag)
		return (0);

	if (n_dirs == 0)
		error ("Não foram encontrados diretórios vazios");
	else
		error ("Foram encontrados %d diretórios vazios", n_dirs);

	return (0);

}	/* end emptydir */

/*
 ****************************************************************
 *	Processa um nome 					*
 ****************************************************************
 */
int
accept (const char *file_nm, STAT *sp, int status)
{
	if   (status == FTW_NS)
	{
		error ("*Não consegui obter o estado de \"%s\"", file_nm);
		return (0);
	}
	elif (status == FTW_DNR)
	{
		error ("*Não consegui percorrer \"%s\"", file_nm);
	}

	/*
	 *	Só interessam diretórios
	 */
	if ((sp->st_mode & S_IFMT) == S_IFDIR)
		dir_read (file_nm, sp);

	return (0);

}	/* end accept */

/*
 ****************************************************************
 *	Le um diretório						*
 ****************************************************************
 */
void
dir_read (const char *dir_nm, STAT *sp)
{
	DIR		*dir_fp;
	const DIRENT	*dp;

	/*
	 *	Abre o diretório
	 */
	if ((dir_fp = opendir (dir_nm)) == NODIR)
	{
		error ("*Não consegui abrir \"%s\"", dir_nm);
		return;
	}

	/*
	 *	Le o diretório
	 */
	while ((dp = readdir (dir_fp)) != NODIRENT)
	{
		if (streq (dp->d_name, ".") || streq (dp->d_name, ".."))
			continue;

		/* Não está vazio */

		closedir (dir_fp); return;

	}	/* end while (fread) */

	/*
	 *	Está vazio
	 */
	closedir (dir_fp);

	n_dirs++;

	if (gflag)
		fprintf (gfp, "%s\n", dir_nm);
	else
		printf ("%s\n", dir_nm);

}	/* end dir_read */

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
		"%s - gera os nomes dos diretórios vazios\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-gvN] [<árvore> ...]\n\n",
		pgname, pgversion, pgname
	);
	fprintf
	(	stderr,
		"\nOpções:"
		"\t-g: Gera o arquivo \"garfield\"\n"
		"\t-v: Verboso\n"
		"\t-N: Lê os nomes das <árvore>s de \"stdin\"\n"
	);
	exit (2);

}	/* end help */
