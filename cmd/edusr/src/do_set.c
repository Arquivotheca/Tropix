/*
 ****************************************************************
 *								*
 *			do_set.c				*
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
 * 								*
 ****************************************************************
 */

#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "../h/edusr.h"

/*
 ******	Protótipos de funções ***********************************
 */
void	do_set_help (void);

/*
 ****************************************************************
 *	Modifica o diretório atual				*
 ****************************************************************
 */
void
do_set (int argc, const char *argv[])
{
	int		opt;

	/*
	 *	Analisa as opções
	 */
	while ((opt = cmd_getopt (argv, "H")) >= 0)
	{
		switch (opt)
		{
		    case 'H':			/* Help */
			do_set_help ();
			return;

		    default:			/* Erro */
			fprintf (stderr, "\n");
			do_set_help ();
			return;
		}
	}

	/*
	 *	Pequena consistência
	 */
	argc -= cmd_optind;
	argv += cmd_optind;

	if (argc != 0)
		{ do_set_help (); return; }

	/*
	 *	Imprime os valores
	 */
	do_mkgrp_current ();
	do_cota_current ();
	do_expir_current ();
	do_home_current ();
	do_shell_current ();

}	/* end do_set */

/*
 ****************************************************************
 *	Resumo de utilização do programa			*
 ****************************************************************
 */
void
do_set_help (void)
{
	fprintf
	(	stderr,
		"%s - imprime os valores correntes das variáveis\n"
		"\nSintaxe:\n"
		"\t%s\n",
		cmd_nm, cmd_nm
	);

}	/* end do_set_help */
