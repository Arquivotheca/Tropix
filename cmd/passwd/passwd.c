/*
 ****************************************************************
 *								*
 *			passwd.c				*
 *								*
 *	Altera a senha do usuário				*
 *								*
 *	Versão	1.0.0, de 09.02.87				*
 *		3.0.1, de 03.06.97				*
 *								*
 *	Módulo: passwd						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Multiprocessado		*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#include <ctype.h>
#include <time.h>
#include <errno.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão:	3.0.1, de 03.06.97";

#define	elif	else if

#define SUPERUSER	0
#define	PWSZ		13

entry int	vflag;		/* Verbose */

const int	pwsz[8] = { 11, 11, 8, 7, 8, 7, 6, 6 };

const char	passwd[]  = "/etc/passwd";
const char	nopwd[]   = "-------------";	/* Senha Nula */

/*
 ****** Protótipos de funções ***********************************
 */
char		*getshortpass (const char *);
void		help (void);

/*
 ****************************************************************
 *	Altera a senha do usuário				*
 ****************************************************************
 */
int
main (int argc, register const char *argv[])
{
	register int		opt, i;
	register const char	*pw;
	register FILE		*fp;
	register PASSWD		*pwd;
	const char		*uname = NOSTR;
	int			c, ruid, flags, pwlen;
	int			vezes = 0;
	off_t			curreg;
	const char		*cp;
	time_t			salt;
	char			saltc[2];
	char			pwarea[80+1];
	char			regarea[120];

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "vH")) != EOF)
	{
		switch (opt)
		{
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
	argc -= optind;

	/*
	 *	Verifica se o UID é de superusuário
	 */
	if (geteuid () != 0)
	{
		fprintf
		(	stderr,
			"%s: O usuário efetivo não é SUPERUSUÁRIO!\n",
			pgname
		);
		exit (1);
	}

	/*
	 *	Obtem o nome do usuário
	 */
	if   (argc == 0)
	{
		if ((uname = getlogin ()) == NOSTR)
		{
			fprintf
			(	stderr,
				"%s: Não consegui obter o nome do usuário\n",
				pgname
			);
			exit (1);
		}
	}
	elif (argc == 1)
	{
		uname = argv[0];
	}
	else
	{
		help ();
	}

	printf ("Trocando a senha do usuário \"%s\"\n", uname);

	/*
	 *	Procura o usuario no arquivo "/etc/passwd"
	 */
	curreg = 0;

	while ((pwd = getpwent ()) != NOPASSWD)
	{
		if (streq (pwd->pw_name, uname))
			break;

		curreg = getpwpos ();
	}

	/*
	 *	Verifica se a conta existe
	 */
	if (pwd == NOPASSWD)
	{
		printf ("Conta inexistente\n");
		exit (1);
	}

	/*
	 *	Verifica a Permissão
	 */
	ruid = getuid ();

	if (ruid != SUPERUSER && ruid != pwd->pw_uid)
	{
		printf ("Permissão negada\n");
		exit (1);
	}

	endpwent ();

	/*
	 *	Verifica se a senha antiga tem PWSZ caracteres
	 */
	if (strlen (pwd->pw_passwd) != PWSZ)
	{
		fprintf
		(	stderr,
			"%s: Senha encriptada inválida: \"%s\"\n",
			pgname,	pwd->pw_passwd
		);
		exit (2);
	}

	/*
	 *	Confere a senha antiga
	 */
	if (!streq (pwd->pw_passwd, nopwd) && (ruid != SUPERUSER || pwd->pw_uid == SUPERUSER))
	{
		strcpy (pwarea, getshortpass ("Senha antiga: "));

		pw = crypt (pwarea, pwd->pw_passwd);

		if (!streq (pw, pwd->pw_passwd))
		{
			printf ("Não conferiu - Senha não modificada\n");
			exit (1);
		}
	}

	/*
	 *	Le a senha nova
	 */
    again:
	if (vezes++ > 3)
	{
		printf ("A senha não foi modificada\n");
		exit (1);
	}

	strcpy (pwarea, getshortpass ("Senha nova:   "));

	/*
	 *	Caso de senha nula
	 */
	if ((pwlen = strlen (pwarea)) == 0)
	{
		strcpy (pwarea, getshortpass ("Repita a senha:   "));

		if ((pwlen = strlen (pwarea)) != 0)
		{
			printf ("Não conferiu\n");
			goto again;
		}

		if (pwd->pw_uid != SUPERUSER)
		{
			pw = nopwd;
			goto pwok;
		}

		printf ("Superusuário não pode ter senha nula\n");
		goto again;
	}

	/*
	 *	Analisa a senha dada
	 */
	flags = 0; 	pw = pwarea;

	while (c = *pw++)
	{
		if   (isdigit (c))
			flags |= 1;
		elif (isalpha (c))
			flags |= 2;
		else
			flags |= 4;
	}

	if (pwlen < pwsz[flags])
	{
		printf ("Senha muito curta\n");
		goto again;
	}
		

	/*
	 *	A senha foi aprovada. Repete para conferir
	 */
	if (!streq (pwarea, getshortpass ("Repita a senha: ")))
	{
		printf ("Não conferiu\n");
		goto again;
	}

	/*
	 *	Prepara a senha encriptada
	 */
	time (&salt);

	saltc[0] = salt & 0x3F;
	saltc[1] = (salt >> 6) & 0x3F;

	for (i = 0; i < 2; i++)
	{
		c = saltc[i] + '.';

		if (c > '9')
			c += 7;

		if (c > 'Z')
			c += 6;

		saltc[i] = c;
	}

	pw = crypt (pwarea, saltc);

	/*
	 *	A senha já está encriptada ou é nula
	 */
    pwok:
	endpwent ();

	if ((fp = fopen (passwd,"r+")) == NOFILE)
	{
		fprintf
		(	stderr,
			"%s: Não consegui abrir o arquivo de contas/senhas (%s)\n",
			pgname, strerror (errno)
		);
		goto bad;
	}

	fseek (fp, curreg, SEEK_SET);

	if (fngets (regarea, sizeof (regarea), fp) != regarea)
	{
		fprintf
		(	stderr,
			"%s: Não consegui ler o arquivo de contas/senhas (%s)\n",
			pgname, strerror (errno)
		);
		goto bad;
	}

	cp = strchr (regarea, ':');

	if (strncmp (++cp, pwd->pw_passwd, PWSZ))
	{
		fprintf
		(	stderr,
			"%s: A senha antiga do arquivo de contas/senhas não conferiu\n",
			pgname
		);
		goto bad;
	}
                          
	fseek (fp, curreg + (cp - regarea), SEEK_SET);

	if (fwrite (pw, PWSZ, 1, fp) != 1)
	{
		fprintf
		(	stderr,
			"%s: Não consegui atualizar o arquivo de contas/senhas (%s)\n",
			pgname, strerror (errno)
		);
		goto bad;
	}

	fclose (fp);

	printf ("Senha nova em uso\n");
	
	exit (0);


	/*
	 *	Erros
	 */
    bad:
	fclose (fp);
	printf ("A senha não foi modificada\n");
	return (1);

}	/* end passwd */

/*
 ****************************************************************
 *	Encurta a senha (de 80 para 14 caracteres)		*
 ****************************************************************
 */
char *
getshortpass (const char *prompt)
{
	char		*cp;

	if ((cp = getpass (prompt)) == NOSTR)
	{
		fprintf
		(	stderr,
			"%s: Não consegui obter uma senha (%s)\n",
			pgname, strerror (errno)
		);
		exit (1);
	}

	cp[IDSZ] = '\0';

	return (cp);

}	/* end getshortpass */

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
		"%s - altera a senha do usuário\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [<conta>]\n",
		pgname, pgversion, pgname
	);

	exit (2);

}	/* end help */
