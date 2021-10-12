/*
 ****************************************************************
 *								*
 *			cota.c					*
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
void	do_cota_help (void);

/*
 ****************************************************************
 *	Altera o valor da cota de usuário			*
 ****************************************************************
 */
void
do_cota (int argc, const char *argv[])
{
	int		opt;
	const char	*cota_nm, *str;
	int		n;

	/*
	 *	Analisa as opções
	 */
	while ((opt = cmd_getopt (argv, "H")) >= 0)
	{
		switch (opt)
		{
		    case 'H':			/* Help */
			do_cota_help ();
			return;

		    default:			/* Erro */
			fprintf (stderr, "\n");
			do_cota_help ();
			return;
		}
	}

	/*
	 *	Pequena consistência
	 */
	argc -= cmd_optind;
	argv += cmd_optind;

	if (argc > 1)
		{ do_cota_help (); return; }

	/*
	 *	Imprime o "cota" corrente
	 */
	if (argc == 0)
		{ do_cota_current (); return; }

	/*
	 *	Processa a nova "cota"
	 */
	cota_nm = argv[0];

	if ((n = strtol (cota_nm, &str, 0)) < 0 || *str != '\0')
	{
		error ("Valor de cota \"%s\" inválido", cota_nm);
		return;
	}

	current_quota = n;

	do_cota_current ();

}	/* end do_cota */

/*
 ****************************************************************
 *	Imprime a cota corrente					*
 ****************************************************************
 */
void
do_cota_current (void)
{
	if (current_quota == 0)
		printf ("Não há limite de cota corrente\n", current_quota);
	else
		printf ("A cota corrente é de %d blocos\n", current_quota);

}	/* end do_cota_current */

/*
 ****************************************************************
 *	Resumo de utilização do programa			*
 ****************************************************************
 */
void
do_cota_help (void)
{
	fprintf
	(	stderr,
		"%s - altera a cota de blocos por usuário\n"
		"\nSintaxe:\n"
		"\t%s [<cota>]\n"
		"\nObs.:\tSe <cota> não for dada, imprime a \"cota\" corrente.\n"
		"\tUma cota de 0 blocos significa ausência de limite\n",
		cmd_nm, cmd_nm
	);

}	/* end do_cota_help */
