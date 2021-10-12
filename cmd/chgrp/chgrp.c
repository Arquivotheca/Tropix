/*
 ****************************************************************
 *								*
 *			chgrp.c					*
 *								*
 *	Troca o grupo de arquivos				*
 *								*
 *	Versão	1.0.1, de 23.07.87				*
 *		4.2.0, de 08.05.02				*
 *								*
 *	Módulo: chgrp						*
 *		Utilitários básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Multiprocessado		*
 *		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <grp.h>
#include <errno.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão:	4.2.0, de 08.05.02";

entry int	Nflag;		/* Nomes dos pgms no <stdin> */
entry int	vflag;		/* Verbose */

/*
 ******	Protótipos de funções ***********************************
 */
int		get_gid (const char *);
int		change_gid (const char *, int);
void		help (void);

/*
 ****************************************************************
 *	Programa principal					*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	int		opt;
	int		ret = 0, gid;

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
	 *	Obtém o group ID
	 */
	if (*argv == NOSTR)
		help ();

	gid = get_gid (*argv++);

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
	if (*argv == NOSTR)			/* Não foram dados argumentos: lê do "stdin" */
	{
		char		*area = alloca (1024);

		while (fngets (area, 1024, stdin) != NOSTR)
		{
			ret += change_gid (area, gid);
		}
	}
	else					/* Foram dados argumentos */
	{
		for (/* vazio */; *argv; argv++)
		{
			ret += change_gid (*argv, gid);
		}
	}

	return (ret);

}	/* end main */

/*
 ****************************************************************
 *	Obtém o group ID					*
 ****************************************************************
 */
int
get_gid (const char *gname)
{
	GROUP		*gp;
	const char	*ptr;
	int		gid;

	if (*gname >= '0' && *gname <= '9')
	{
		gid = strtol (gname, &ptr, 0);

		if (ptr != strend (gname))
		{
			error ("Grupo inválido: \"%s\"", gname);
			exit (4);
		}

		if ((gp = getgrgid (gid)) == NOGROUP)
		{
			error ("Advertência - grupo desconhecido: \"%s\"", gname);
			return (gid);
		}
	}
	else
	{
		if ((gp = getgrnam (gname)) == NOGROUP)
		{
			error ("Grupo desconhecido: \"%s\"", gname);
			exit (4);
		}
	}

	return (gp->gr_gid);

}	/* end get_gid */

/*
 ****************************************************************
 *	Troca o grupo do arquivo				*
 ****************************************************************
 */
int
change_gid (const char *file_nm, int gid)
{
	STAT		statb;

	if (stat (file_nm, &statb) < 0)
	{
		error ("*Não consegui obter o estado de \"%s\"", file_nm);
		return (1);
	}
	else
	{
		if (chown (file_nm, statb.st_uid, gid) < 0)
		{
			error ("*Não consegui trocar o grupo de \"%s\"", file_nm);
			return (1);
		}
	}

	return (0);

}	/* end change_gid */

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
		"%s - troca a identificação do grupo de arquivos\n",
		pgname
	);

	fprintf (stderr, "\n%s\n", pgversion);
	fprintf (stderr, "\nSintaxe:\n");
	fprintf (stderr, "\t%s [-N] <grupo> [<arquivo> ...]\n", pgname);

	fprintf
	(	stderr,
		"\nOpções:\t-N: Lê os nomes dos arquivos de \"stdin\"\n"
	);

	fprintf
	(	stderr,
		"\t    Esta opção é implícita se não forem dados "
		"nomes de arquivos\n"
	);

	exit (2);

}	/* end help */
