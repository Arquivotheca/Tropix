/*
 ****************************************************************
 *								*
 *			rmusr.c					*
 *								*
 *	Gerenciador de contas					*
 *								*
 *	Versão	1.5.0, de 04.06.84				*
 *		3.1.0, de 17.10.98				*
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
#include <pwd.h>
#include <grp.h>
#include <signal.h>
#include <errno.h>

#include "../h/edusr.h"

/*
 ******	Protótipos de funções ***********************************
 */
void	do_rmusr_help (void);

/*
 ****************************************************************
 *	Remove a conta de um usuário				*
 ****************************************************************
 */
void
do_rmusr (int argc, const char *argv[])
{
	int		opt;
	FILE		*tfp;
	const PASSWD	*pw;
	const char	*user_nm;
	int		found = 0;
	char		command_str[128];

	/*
	 *	Analisa as opções
	 */
	while ((opt = cmd_getopt (argv, "H")) >= 0)
	{
		switch (opt)
		{
		    case 'H':			/* Help */
			do_rmusr_help ();
			return;

		    default:			/* Erro */
			fprintf (stderr, "\n");
			do_rmusr_help ();
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

	/*
	 *	Examina os argumentos
	 */
	if (user_nm == NOSTR)
	{
		error ("Faltando o nome do <usuário>");
		return;
	}

	if (isdigit (user_nm[0]))
	{
		error ("É necessário dar o nome do <usuário>");
		return;
	}

	/*
	 *	Remove a entrada do usuário
	 */
	signal (SIGINT, SIG_IGN);
	tfp = tmpfile ();
	setpwent ();

	while ((pw = getpwent ()) != NOPASSWD)
	{
		if (!streq (pw->pw_name, user_nm))
			{ putpwent (pw, tfp); continue; }

		found++;

		/* Remove o HOME */

		fprintf
		(	stderr,
			"Remove o usuário \"%s\" (juntamente com seus arquivos)? (n): ",
			user_nm
		);

		if (askyesno () <= 0)
			goto abort;

#if (0)	/*******************************************************/
		strcpy (command_str, "rmtree -f ");
#endif	/*******************************************************/
		strcpy (command_str, "rmtree ");
		strcat (command_str, pw->pw_dir);

		if ((system (command_str) >> 8) != 0)
			error ("Não consegui remover os arquivos do usuário \"%s\"", user_nm);

		/* Remove a caixa postal */

		strcpy (mbox_nm_ptr, user_nm);

		printf ("\n");
		strcpy (command_str, "rmtree ");
		strcat (command_str, mbox_nm);

		if ((system (command_str) >> 8) != 0)
			error ("Não consegui remover a caixa postal do usuário \"%s\"", user_nm);
	}

	endpwent ();
	copyfile (tfp, pwd_file_nm);
	signal (SIGINT, on_intr);

	if (!found)
		error ("Não encontrei o usuário \"%s\" no arquivo de contas/senhas", user_nm);

	return;

	/*
	 *	Em caso de desistência, ...
	 */
    abort:
	endpwent ();
	fclose (tfp);
	signal (SIGINT, on_intr);

}	/* end do_rmusr */

/*
 ****************************************************************
 *	Resumo de utilização do programa			*
 ****************************************************************
 */
void
do_rmusr_help (void)
{
	fprintf
	(	stderr,
		"%s - remove um usuário (juntamente com seus arquivos)\n"
		"\nSintaxe:\n"
		"\t%s <usuário>\n",
		cmd_nm, cmd_nm
	);

}	/* end do_rmusr_help */
