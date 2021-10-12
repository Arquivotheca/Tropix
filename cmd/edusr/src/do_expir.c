/*
 ****************************************************************
 *								*
 *			expir.c					*
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
#include <time.h>
#include <errno.h>
#include <string.h>

#include "../h/edusr.h"

/*
 ******	Protótipos de funções ***********************************
 */
void	do_expir_help (void);

/*
 ****************************************************************
 *	Altera a data de expiração da conta de usuário		*
 ****************************************************************
 */
void
do_expir (int argc, const char *argv[])
{
	int		opt;
	const char	*expir_nm;
	time_t		n;

	/*
	 *	Analisa as opções
	 */
	while ((opt = cmd_getopt (argv, "H")) >= 0)
	{
		switch (opt)
		{
		    case 'H':			/* Help */
			do_expir_help ();
			return;

		    default:			/* Erro */
			fprintf (stderr, "\n");
			do_expir_help ();
			return;
		}
	}

	/*
	 *	Pequena consistência
	 */
	argc -= cmd_optind;
	argv += cmd_optind;

	if (argc > 1)
		{ do_expir_help (); return; }

	/*
	 *	Imprime o "expir" corrente
	 */
	if (argc == 0)
		{ do_expir_current (); return; }

	/*
	 *	Processa a nova data de expiração
	 */
	expir_nm = argv[0];

	if   (streq (expir_nm, "-") || streq (expir_nm, "0"))
	{
		n = 0;
	}
	elif ((n = time_to_internal_cv (expir_nm)) < 0)
	{
		error ("Data de expiração \"%s\" inválida", expir_nm);
		return;
	}

	current_expir_date = n;

	do_expir_current ();

}	/* end do_expir */

/*
 ****************************************************************
 *	Imprime a data de expiração corrente			*
 ****************************************************************
 */
void
do_expir_current (void)
{
	if (current_expir_date == 0)
		printf ("Não há data de expiração corrente\n");
	else
		printf ("A data de expiração corrente é %s\n", time_to_external_cv (current_expir_date));

}	/* end do_expir_current */

/*
 ****************************************************************
 *	Resumo de utilização do programa			*
 ****************************************************************
 */
void
do_expir_help (void)
{
	fprintf
	(	stderr,
		"%s - altera a data de expiração da conta de usuário\n"
		"\nSintaxe:\n"
		"\t%s [<expir>]\n"
		"\nObs.:\tSe <expir> não for dada, é impressa a data de expiração corrente.\n"
		"\tO formato de <expir> deve ser \"dd.mm.aa\", ou \"0\" significando\n"
		"\ta ausência de data de expiração\n",
		cmd_nm, cmd_nm
	);

}	/* end do_expir_help */
