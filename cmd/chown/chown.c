/*
 ****************************************************************
 *								*
 *			chown.c					*
 *								*
 *	Troca o dono de arquivos				*
 *								*
 *	Versão	1.0.1, de 23.07.87				*
 *		4.2.0, de 08.05.02				*
 *								*
 *	Módulo: chown						*
 *		Utilitários básicos				*
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
#include <pwd.h>
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
int		get_uid (const char *);
int		change_uid (const char *, int);
void		help (void);

/*
 ****************************************************************
 *	Programa principal					*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	int		opt, uid, ret = 0;

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
	 *	Obtém o user ID
	 */
	if (*argv == NOSTR)
		help ();

	uid = get_uid (*argv++);

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
	if (*argv == NOSTR)		/* Não foram dados argumentos: lê do "stdin" */
	{
		char		*area = alloca (1024);

		while (fngets (area, 1024, stdin) != NOSTR)
		{
			ret += change_uid (area, uid);
		}
	}
	else				/* Foram dados argumentos */
	{
		for (/* vazio */; *argv; argv++)
		{
			ret += change_uid (*argv, uid);
		}
	}

	return (ret);

}	/* end main */

/*
 ****************************************************************
 *	Obtém o user ID						*
 ****************************************************************
 */
int
get_uid (const char *uname)
{
	PASSWD		*pwdp;
	const char	*ptr;
	int		uid;

	if (*uname >= '0' && *uname <= '9')
	{
		uid = strtol (uname, &ptr, 0);

		if (ptr != strend (uname))
			error ("$Usuário inválido: \"%s\"", uname);

		if ((pwdp = getpwuid (uid)) == NOPASSWD)
		{
			error ("Advertência - usuário desconhecido: \"%s\"", uname);
			return (uid);
		}
	}
	else
	{
		if ((pwdp = getpwnam (uname)) == NOPASSWD)
			error ("$Advertência - usuário desconhecido: \"%s\"", uname);
	}

	return (pwdp->pw_uid);

}	/* end get_uid */

/*
 ****************************************************************
 *	Troca o usuário do arquivo				*
 ****************************************************************
 */
int
change_uid (const char *filenm, int uid)
{
	STAT		statb;

	if (stat (filenm, &statb) < 0)
	{
		error ("*Não consegui obter o estado de \"%s\"", filenm);
		return (1);
	}
	else
	{
		if (chown (filenm, uid, statb.st_gid) < 0)
		{
			error ("*Não consegui trocar o dono de \"%s\"", filenm);
			return (1);
		}
	}

	return (0);

}	/* end change_uid */

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
		"%s - troca a identificação do dono (UID) de arquivos\n",
		pgname
	);
	fprintf (stderr, "\n%s\n", pgversion);
	fprintf (stderr, "\nSintaxe:\n");
	fprintf (stderr, "\t%s [-N] <usuário> [<arquivo> ...]\n", pgname);
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
