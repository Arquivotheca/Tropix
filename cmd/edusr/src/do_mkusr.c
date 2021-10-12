/*
 ****************************************************************
 *								*
 *			mkusr.c					*
 *								*
 *	Gerenciador de contas					*
 *								*
 *	Versão	1.5.0, de 04.06.84				*
 *		3.1.0, de 17.17.98				*
 *								*
 *	Módulo: edusr						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
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
#include <signal.h>
#include <errno.h>

#include "../h/edusr.h"

/*
 ****************************************************************
 *	Definições globais					*
 ****************************************************************
 */
#define	SUPERUSER	0

const char	profile_text_file_nm[] = "/usr/pub/lib/profile.sample";

#define	MAIL_PREFIX	"/var/mail/"

entry char	mbox_nm[48] = MAIL_PREFIX;
entry char	*mbox_nm_ptr = mbox_nm + sizeof (MAIL_PREFIX) - 1;


#if (0)	/*******************************************************/
const char	profile_text[] =
{
	"GAR=deflate;			export GAR\n"
	"TNET=rdir=.,reduce;		export TNET\n"
	"PATH=.:/bin:/usr/bin:/etc:/usr/etc:/usr/pub/bin:/usr/xwin/bin\n"
	"PS1='$USER@$% [$:]: '\n"
	"umask 027\n"
	"HIST=20\n"
	"stty switch '^A'\n"
	"set -r\n"
	"fortune\n"
};
#endif	/*******************************************************/

/*
 ******	Protótipos de funções ***********************************
 */
int	do_mkusr_write_entry (FILE *, const char *, const char *, int, const char *);
void	do_mkusr_help (void);

/*
 ****************************************************************
 *	Cria a conta de um usuário				*
 ****************************************************************
 */
void
do_mkusr (int argc, const char *argv[])
{
	int		opt;
	FILE		*tfp;
	const PASSWD	*pw;
	const char	*user_nm, *shell_nm;
	int		uid = 0, last_user_id = -1, colocou = 0;
	char		comment_str[128];
	int		comment_cnt = 0;

	/*
	 *	Analisa as opções
	 */
	while ((opt = cmd_getopt (argv, "sH")) >= 0)
	{
		switch (opt)
		{
		    case 's':			/* Superusuário */
			cmd_sflag++;
			break;

		    case 'H':			/* Help */
			do_mkusr_help ();
			return;

		    default:			/* Erro */
			fprintf (stderr, "\n");
			do_mkusr_help ();
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

	comment_str[0] = '\0';

	while (*argv != NOSTR)
	{
		if (comment_cnt++)
			strcat (comment_str, " ");

		strcat (comment_str, *argv++);
	}

	/*
	 *	Examina os argumentos
	 */
	if (user_nm == NOSTR)
	{
		error ("Faltando o nome do <usuário>");
		return;
	}

	if (!isalpha (user_nm[0]))
	{
		error ("O nome do <usuário> deve começar por uma letra");
		return;
	}

#if (0)	/*******************************************************/
	if (strlen (user_nm) > IDSZ)
	{
		error ("O nome do <usuário> não deve ter mais de %d caracteres", IDSZ);
		return;
	}
#endif	/*******************************************************/

	/*
	 *	Certifica-se de que o usuário ainda não existe
	 */
	if ((pw = getpwnam (user_nm)) != NOPASSWD)
	{
		error ("O usuário \"%s\" já consta do arquivo de contas/senhas", user_nm);
		return;
	}

	/*
	 *	Examina o "home" e "shell"
	 */
	if (chdir (current_home) < 0 )
	{
		error ("*Não consegui mudar o diretório corrente para \"%s\"", current_home);
		return;
	}

	if (streq (current_shell, std_shell))
		shell_nm = "";
	else
		shell_nm = current_shell;

	/*
	 *	Cria a entrada do novo usuário
	 */
	signal (SIGINT, SIG_IGN);
	tfp = tmpfile ();
	setpwent ();

	while ((pw = getpwent ()) != NOPASSWD)
	{
		if (!colocou)
		{
			if   (cmd_sflag && pw->pw_uid > 0)
			{
				if (do_mkusr_write_entry (tfp, user_nm, comment_str, SUPERUSER, shell_nm) < 0)
					goto abort;

				colocou++;
			}
			elif ((uid = pw->pw_uid) > NIG && uid > last_user_id + 1)
			{
				uid = (last_user_id < NIG ? NIG : last_user_id + 1);

				if (do_mkusr_write_entry (tfp, user_nm, comment_str, uid, shell_nm) < 0)
					goto abort;

				colocou++;
			}
		}
	
		putpwent (pw, tfp); last_user_id = pw->pw_uid;
	
	}	/* loop copiando o passwd file */

	if (!colocou)
	{
		if (cmd_sflag)
		{
			if (do_mkusr_write_entry (tfp, user_nm, comment_str, SUPERUSER, shell_nm) < 0)
				goto abort;
		}
		else
		{
			uid = (last_user_id < NIG ? NIG : last_user_id + 1);

			if (do_mkusr_write_entry (tfp, user_nm, comment_str, uid, shell_nm) < 0)
				goto abort;
		}
	}
	
	endpwent ();
	copyfile (tfp, pwd_file_nm);

	/*
	 *	Cria o diretório HOME do usuário
	 */
	if (mkdir (user_nm, 0750) < 0 )
	{
		error ("*Não consegui criar o diretório HOME \"%s/%s\"", current_home, user_nm);

		if (errno != EEXIST)
			{ signal (SIGINT, on_intr); return; }
	}

	if (chown (user_nm, uid, current_group_id) < 0)
		error ("*Não consegui alterar o dono do diretório HOME \"%s/%s\"", current_home, user_nm);

	/*
	 *	Cria o arquivo ".profile" do usuário
	 */
	if (chdir (user_nm) < 0 )
	{
		error ("*Não consegui mudar o diretório corrente para \"%s/%s\"", current_home, user_nm);
		signal (SIGINT, on_intr); return;
	}

	if ((tfp = fopen (profile_text_file_nm, "r")) == NOFILE)
	{
		error ("*Não consegui abrir o arquivo protótipo de \".profile\"");
		signal (SIGINT, on_intr); return;
	}

	copyfile (tfp, ".profile");

#if (0)	/*******************************************************/
	if ((fd = creat (".profile", 0644)) < 0)
	{
		error ("*Não consegui criar o arquivo \".profile\" do usuário \"%s\"", user_nm);
		signal (SIGINT, on_intr); return;
	}

	if (write (fd, profile_text, sizeof (profile_text) - 1) != sizeof (profile_text) - 1)
		error ("*Não consegui escrever o conteúdo do arquivo \".profile\" do usuário \"%s\"", user_nm);

	close (fd);
#endif	/*******************************************************/

	if (chown (".profile", uid, current_group_id) < 0)
		error ("*Não consegui alterar o dono do arquivo \".profile\" do usuário \"%s\"", user_nm);

	if (chdir ("..") < 0 )
		error ("*Não consegui mudar o diretório corrente para \"%s\"", current_home);

	/*
	 *	Cria a caixa postal do usuário
	 */
	strcpy (mbox_nm_ptr, user_nm);

	if (mkdir (mbox_nm, 0700) < 0 )
	{
		error ("*Não consegui criar a caixa postal \"%s\"", mbox_nm);

		if (errno != EEXIST)
			{ signal (SIGINT, on_intr); return; }
	}

	if (chown (mbox_nm, uid, current_group_id) < 0)
		error ("*Não consegui alterar o dono da caixa postal do usuário \"%s\"", user_nm);

	signal (SIGINT, on_intr);
	return;

	/*
	 *	Em caso de desistência ou erro, ...
	 */
    abort:
	endpwent ();
	fclose (tfp);
	signal (SIGINT, on_intr);

}	/* end do_mkusr */

/*
 ****************************************************************
 *	Cria a entrada do usuário				*
 ****************************************************************
 */
int
do_mkusr_write_entry
(
	FILE		*tfp,
	const char	*user_nm,
	const char	*comment_str,
	int		uid,
	const char	*shell_nm
)
{
	fprintf
	(	stderr,
		"\nUsuário\t\t  = %s (%d)\n"
		"Grupo\t\t  = %s (%d)\n"
		"Comentário\t  = %s\n"
		"Cota\t\t  = %d\n"
		"Data de expiração = %s\n"
		"Diretório\t  = %s/%s\n"
		"Shell\t\t  = %s\n",
		user_nm, uid,
		current_group_nm, current_group_id,
		comment_str, current_quota,
		current_expir_date ? time_to_external_cv (current_expir_date) : "0",
		current_home, user_nm,
		current_shell
	);

	fprintf (stderr, "\nDeseja criar o usuário? (n): ");

	if (askyesno () <= 0)
		return (-1);

	fprintf
	(	tfp,
		"%s:%s:%s:%d:%d:%d:%d::%s/%s:%s\n",
		user_nm,
		uid == 0 ? root_pwd : no_pwd,
		comment_str,
		uid,
		current_group_id,
		current_quota,
		current_expir_date,
		current_home,
		user_nm,
		shell_nm
	);

	return (0);

}	/* end do_mkusr_write_entry */

/*
 ****************************************************************
 *	Resumo de utilização do programa			*
 ****************************************************************
 */
void
do_mkusr_help (void)
{
	fprintf
	(	stderr,
		"%s - cria um novo usuário\n"
		"\nSintaxe:\n"
		"\t%s [-s] <usuário> [<comentário>]\n",
		cmd_nm, cmd_nm
	);
	fprintf
	(	stderr,
		"\nOpções:"
		"\t-s: O novo <usuário> será um SUPERUSUÁRIO\n"
		"\nObs.:\tO <comentário> engloba todos os argumentos após o <usuário>\n"
	);

}	/* end do_mkusr_help */
