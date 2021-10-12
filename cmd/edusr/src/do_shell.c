/*
 ****************************************************************
 *								*
 *			do_shell.c				*
 *								*
 *	Gerenciador de contas					*
 *								*
 *	Versão	1.5.0, de 04.06.84				*
 *		3.0.0, de 29.07.97				*
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
#include <sys/types.h>
#include <sys/stat.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "../h/edusr.h"

/*
 ******	Protótipos de funções ***********************************
 */
void	do_shell_help (void);

/*
 ****************************************************************
 *	Modifica o "shell" corrente				*
 ****************************************************************
 */
void
do_shell (int argc, const char *argv[])
{
	int		opt;
	const char	*nm;
	STAT		s;

	/*
	 *	Analisa as opções
	 */
	while ((opt = cmd_getopt (argv, "H")) >= 0)
	{
		switch (opt)
		{
		    case 'H':			/* Help */
			do_shell_help ();
			return;

		    default:			/* Erro */
			fprintf (stderr, "\n");
			do_shell_help ();
			return;
		}
	}

	/*
	 *	Pequena consistência
	 */
	argc -= cmd_optind;
	argv += cmd_optind;

	if (argc > 1)
		{ do_shell_help (); return; }

	/*
	 *	Verifica se é para imprimir o valor corrente
	 */
	if (argc == 0)
		{ do_shell_current (); return; }

	/*
	 *	Altera o valor
	 */
	nm = argv[0];

	if (*nm != '/')
	{
		error ("O nome do \"shell\" deve começar por \"/\"", nm);
		return;
	}

	if (stat (nm, &s) < 0 )
	{
		error ("*Não consegui obter o estado de \"%s\"", nm);
		return;
	}

	if (!S_ISREG (s.st_mode))
	{
		error ("O arquivo \"%s\" NÃO é regular", nm);
		return;
	}

	if ((s.st_mode & (S_IXUSR|S_IXGRP|S_IXOTH)) == 0)
	{
		error ("O arquivo \"%s\" NÃO é executável", nm);
		return;
	}

	strcpy (current_shell, nm);

	do_shell_current ();

}	/* end shell */
/*
 ****************************************************************
 *	Imprime o "shell" corrente				*
 ****************************************************************
 */
void
do_shell_current (void)
{
	printf ("O \"shell\" corrente é \"%s\"\n", current_shell);

}	/* end do_shell_current */

/*
 ****************************************************************
 *	Resumo de utilização do programa			*
 ****************************************************************
 */
void
do_shell_help (void)
{
	fprintf
	(	stderr,
		"%s - altera o interpretador de comandos (\"shell\")\n"
		"\nSintaxe:\n"
		"\t%s [<shell>]\n"
		"\nObs.\tSe <shell> não for dado, imprime o \"shell\" corrente\n",
		cmd_nm, cmd_nm
	);

}	/* end do_shell_help */
