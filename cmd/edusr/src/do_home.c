/*
 ****************************************************************
 *								*
 *			home.c					*
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
void	do_home_help (void);

/*
 ****************************************************************
 *	Modifica o diretório contendo os arquivos dos usuários	*
 ****************************************************************
 */
void
do_home (int argc, const char *argv[])
{
	int		opt;
	const char	*nm;

	/*
	 *	Analisa as opções
	 */
	while ((opt = cmd_getopt (argv, "H")) >= 0)
	{
		switch (opt)
		{
		    case 'H':			/* Help */
			do_home_help ();
			return;

		    default:			/* Erro */
			fprintf (stderr, "\n");
			do_home_help ();
			return;
		}
	}

	/*
	 *	Pequena consistência
	 */
	argc -= cmd_optind;
	argv += cmd_optind;

	if (argc > 1)
		{ do_home_help (); return; }

	/*
	 *	Imprime o "home" corrente
	 */
	if (argc == 0)
		{ do_home_current (); return ; }

	/*
	 *	Processo o novo "home"
	 */
	nm = argv[0];

	if (*nm != '/')
	{
		error ("O nome do diretório \"%s\" NÃO começa por \"/\"", nm);
		return;
	}

	if (chdir (nm) < 0 )
	{
		error ("*Não consegui mudar o diretório corrente para \"%s\"", nm);
		return;
	}

	strcpy (current_home, nm);

	do_home_current ();

}	/* end eddir */

/*
 ****************************************************************
 *	Imprime o valor corrente				*
 ****************************************************************
 */
void
do_home_current (void)
{
	printf
	(	"O diretório corrente é \"%s\"\n",
		current_home
	);

}	/* end do_home_current */

/*
 ****************************************************************
 *	Resumo de utilização do programa			*
 ****************************************************************
 */
void
do_home_help (void)
{
	fprintf
	(	stderr,
		"%s - altera o diretório \"home\"\n"
		"\nSintaxe:\n"
		"\t%s [<diretório>]\n"
		"\nObs.:\tSe o <diretório> não for dado, é impresso o \"home\" corrente\n",
		cmd_nm, cmd_nm
	);

}	/* end do_home_help */
