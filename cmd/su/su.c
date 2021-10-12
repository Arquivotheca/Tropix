/*
 ****************************************************************
 *								*
 *			su.c					*
 *								*
 *	Muda de conta temporariamente				*
 *								*
 *	Versão	1.0.0, de 20.07.87				*
 *		3.0.2, de 11.07.97				*
 *								*
 *	Módulo: su						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pwd.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão:	3.0.2, de 11.07.97";

#define	PWSZ	13		/* Tamanho da senha encriptada */

#define	NOVOID	(void *)NULL
#define	elif	else if
#define	EVER	;;
#define STR(x)	# x
#define XSTR(x)	STR (x)

entry int	eflag;		/* Ambiente */
entry int	vflag;		/* Verbose */

/*
 ******	Definições **********************************************
 */
const char	root_nm[] =	"root";
const char	stdshell[] =	"/bin/sh";
const char	nopwd[] =	"-------------";

#undef	RETYPE
#ifdef	RETYPE
const char	superuser[] =	"<superuser>";
entry char	retype[14];	/* Senha do Retype */
entry char	*superuser_pwd;	/* Senha do superusuário */
#endif	RETYPE

/*
 ******	Ambiente ************************************************
 */
entry char	homedir[64]	= "HOME=";
entry char	shell[64]	= "SHELL=";
entry char	user[24]	= "USER=";

/*
 ****** Protótipos de funções ***********************************
 */
void		help (void);

/*
 ****************************************************************
 *	Muda de conta temporariamente				*
 ****************************************************************
 */
int
main (int argc, register const char *argv[])
{
	register int		opt;
	register PASSWD		*pwd;
	register char		*cp;
	const char		*new_user;
	int			ruid;

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "evH")) != EOF)
	{
		switch (opt)
		{
		    case 'e':			/* Ambiente */
			eflag++;
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
   /***	argc -= optind; ***/

	/*
	 *	Verifica se o programa foi instalado corretamente
	 */
	if (geteuid () != 0)
	{
		fprintf
		(	stderr,
			"%s: Não sou SUPERUSUÁRIO!\n",
			pgname
		);
		exit (1);
	}

	ruid = getuid ();

	/*
	 *	Verifica o novo usuário
	 */
	if (argv[0] != NOSTR)
		new_user = argv[0];
	else
		new_user = root_nm;

#ifdef	RETYPE
	/*
	 *	Obtem a senha do "retype"
	 */
	if ((pwd = getpwnam (superuser)) == NOPASSWD)
	{
		fprintf
		(	stderr,
			"%s: Não encontrei a senha de \"superuser\"\n",
			pgname
		);
		exit (2);
	}

	strcpy (retype, pwd->pw_passwd);
#endif	RETYPE

	/*
	 *	Le e confere a senha
	 */
	if ((pwd = getpwnam (new_user)) == NOPASSWD)
	{
		fprintf
		(	stderr,
			"%s: Conta desconhecida: \"%s\"\n",
			pgname, new_user
		);
		exit (3);
	}

	if (strlen (pwd->pw_passwd) != PWSZ)
	{
		fprintf
		(	stderr,
			"%s: Senha encriptada inválida: \"%s\"\n",
			pgname, pwd->pw_passwd
		);
		exit (4);
	}

#ifdef	RETYPE
	/*
	 *	Se for <superusuário> e não tem a primeira senha ...
	 */
	if (streq (pwd->pw_passwd, nopwd) && pwd->pw_uid == 0)
	{
		pwd->pw_passwd = retype;
		superuser_pwd = nopwd;
	}
	else
	{
		superuser_pwd = retype;
	}
#endif	RETYPE

	/*
	 *	Confere a primeira senha
	 */
	if (ruid != 0 && !streq (pwd->pw_passwd, nopwd))
	{
		if ((cp = getpass ("Senha: ")) == NOSTR)
		{
			fprintf
			(	stderr,
				"Erro em \"getpass\"\n"
			);
			exit (5);
		}

		if (!streq (crypt (cp, pwd->pw_passwd), pwd->pw_passwd))
			{ printf ("Senha incorreta\n"); exit (6); }
	}

#ifdef	RETYPE
	/*
	 *	Se for SUPERUSUÁRIO, pede a segunda SENHA
	 */
	if (ruid != 0 && pwd->pw_uid == 0 && !streq (superuser_pwd, nopwd))
	{
		if ((cp = getpass ("Repita a Senha: ")) == NOSTR)
		{
			fprintf
			(	stderr,
				"Erro em \"getpass\"\n"
			);
			exit (7);
		}

		if (!streq (crypt (cp, superuser_pwd), superuser_pwd))
			{ printf ("Senha incorreta\n"); exit (8); }
	}
#endif	RETYPE

	/*
	 *	Passou pelas SENHAS
	 */
	endpwent ();

	setgid (pwd->pw_gid);
	setuid (pwd->pw_uid);

	if (pwd->pw_shell == NOSTR || pwd->pw_shell[0] == '\0')
		pwd->pw_shell = (char *)stdshell;

	/*
	 *	Se foi pedido a mudança de ambiente, ...
	 */
	if (eflag)
	{
		putenv (strcat (homedir, pwd->pw_dir));
		putenv (strcat (shell, pwd->pw_shell));
		putenv (strcat (user, pwd->pw_name));

		if (chdir (pwd->pw_dir) < 0)
		{
			fprintf
			(	stderr,
				"%s: Diretório inválido: %s (%s)\n",
				pgname, pwd->pw_dir, strerror (errno)
			);
		}

		execl (pwd->pw_shell, "-su", 0);
	}
	else
	{
		execl (pwd->pw_shell, "su", 0);
	}

	fprintf
	(	stderr,
		"%s: Não achei o shell \"%s\" (%s)\n",
		pgname, pwd->pw_shell, strerror (errno)
	);

	return (9);

}	/* end su */

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
		"%s - muda de conta temporariamente\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-e] [<conta nova>]\n",
		pgname, pgversion, pgname
	);
	fprintf
	(	stderr,
		"\nOpções:"
		"\t-e: Utiliza o ambiente da <conta nova>\n"
		"\nObs.:\tO valor por omissão da <conta nova> é \"root\" (superusuário)\n"
	);

	exit (2);

}	/* end help */
