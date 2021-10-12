/*
 ****************************************************************
 *								*
 *			prusr.c					*
 *								*
 *	Gerenciador de contas					*
 *								*
 *	Versão	1.5.0, de 04.06.84				*
 *		3.0.0, de 30.07.97				*
 *								*
 *	Módulo: edusr						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>

#include "../h/edusr.h"

/*
 ******	Protótipos de funções ***********************************
 */
void	do_prusr_help (void);

/*
 ****************************************************************
 *	Imprime os usuários					*
 ****************************************************************
 */
void
do_prusr (int argc, const char *argv[])
{
	int		opt;
	const PASSWD	*pw;
	const GROUP	*gr;
	const char	*user_nm = NOSTR, *group_nm = NOSTR;
	const char	*str;
	int		user_id = -1, group_id = -1;

	/*
	 *	Analisa as opções
	 */
	while ((opt = cmd_getopt (argv, "lg:H")) >= 0)
	{
		switch (opt)
		{
		    case 'l':			/* Longo */
			cmd_lflag++;
			break;

		    case 'g':			/* Grupo dado */
			group_nm = cmd_optarg;
			break;

		    case 'H':			/* Help */
			do_prusr_help ();
			return;

		    default:			/* Erro */
			fprintf (stderr, "\n");
			do_prusr_help ();
			return;
		}
	}

	/*
	 *	Pequena consistência
	 */
	argc -= cmd_optind;
	argv += cmd_optind;

	if ((user_nm = *argv) != NOSTR) 
	{
		if (streq (user_nm, "-"))
			user_nm = NOSTR;

		argv++;
	}

	if (*argv != NOSTR)
		{ do_prusr_help (); return; }

	/*
	 *	Examina o <usuário> e <grupo>
	 */
	if (user_nm != NOSTR)
	{
		if (group_nm != NOSTR)
		{
			error ("Somente pode ser dado um <grupo> ou um <usuário>", cmd_nm);
			return;
		}

		if (isdigit (user_nm[0]))
		{
			if ((user_id = strtol (user_nm, &str, 0)) < 0 || *str != '\0')
			{
				error ("Sintaxe de usuário inválida: \"%s\"", user_nm);
				return;
			}
		}
	}
	elif (group_nm != NOSTR)
	{
		if   (isdigit (group_nm[0]))
		{
			if ((group_id = strtol (group_nm, &str, 0)) < 0 || *str != '\0')
			{
				error ("Sintaxe de grupo inválida: \"%s\"", group_nm);
				return;
			}
		}
		elif ((gr = getgrnam (group_nm)) == NOGROUP)
		{
			error ("Grupo \"%s\" desconhecido", group_nm);
			return;
		}
		else
		{
			group_id = gr->gr_gid;
		}
	}

	/*
	 *	Imprime o cabeçalho
	 */
	if   (cmd_lflag == 0)
	{
		printf
		(	"USUÁRIO          UID  GRUPO            GID P COMENTÁRIO\n\n"
		);
	}
	elif (cmd_lflag == 1)
	{
		printf
		(	"USUÁRIO          UID  PROGRAMA             DIRETÓRIO\n\n"
		);
	}
	elif (cmd_lflag > 1)
	{
		printf
		(	"USUÁRIO          UID   COTA  L    EXPIR   E\n\n"
		);
	}

	/*
	 *	Percorre o arquivo de contas/senhas
	 */
	setpwent ();

	while ((pw = getpwent ()) != NOPASSWD)
	{
		if   (user_nm != NOSTR)
		{
			if (user_id >= 0)
			{
				if (user_id != pw->pw_uid)
					continue;
			}
			elif (!streq (user_nm, pw->pw_name))
			{
				continue;
			}
		}
		elif (group_id >= 0)
		{
			if (pw->pw_gid != group_id)
				continue;
		}

		group_nm = grcache (pw->pw_gid);

		/*
		 *	Parte sempre presente
		 */
		printf ("%-14.14s %5d", pw->pw_name, pw->pw_uid);

		if   (cmd_lflag == 0)
		{
			if (group_nm == NOSTR)
				printf ("  %-14d", pw->pw_gid);
			else
				printf ("  %-14.14s", group_nm);

			printf ("%6d", pw->pw_gid);

			if   (streq (pw->pw_passwd, no_pwd))
				printf (" *");
			elif (streq (pw->pw_passwd, "locked"))
				printf (" l");
			else
				printf ("  ");

			printf (" %s", pw->pw_comment);
		}
		elif (cmd_lflag == 1)
		{
			if (pw->pw_shell[0] == '\0')
				printf ("  %-20.20s", "/bin/sh");
			else
				printf ("  %-20.20s", pw->pw_shell);

			printf (" %s", pw->pw_dir);
		}
		elif (cmd_lflag > 1)
		{
			if (pw->pw_quota == 0)
				printf ("     .   ");
			else
				printf (" %6d  ", pw->pw_quota);

			printf ("%1.1s  ", pw->pw_lock);

			if (pw->pw_expir == 0)
				printf ("    .     ");
			else
				printf ("%8.8s  ", time_to_external_cv (pw->pw_expir) );

			if (pw->pw_expir > 0 && pw->pw_expir < hoje)
				putchar ('E');
			else
				putchar (' ');
		}

		putchar ('\n');
	}

	putchar ('\n');

}	/* end do_prusr */

/*
 ****************************************************************
 *	Resumo de utilização do programa			*
 ****************************************************************
 */
void
do_prusr_help (void)
{
	fprintf
	(	stderr,
		"%s - imprime uma lista de usuários\n"
		"\nSintaxe:\n"
		"\t%s [-l] [-g <grupo>] [<usuário>]\n",
		cmd_nm, cmd_nm
	);
	fprintf
	(	stderr,
		"\nOpções:"
		"\t-l: Lista longa de informações\n"
		"\t    (se dado duas vezes, lista longa com outras informações)\n"
		"\t-g: Imprime somente usuários do <grupo> dado\n"

		"\nObs.:\tSe for dado o <usuário> são dadas apenas informações sobre ele.\n"
	);

}	/* end do_prusr_help */
