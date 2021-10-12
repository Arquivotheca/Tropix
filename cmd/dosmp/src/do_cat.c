/*
 ****************************************************************
 *								*
 *			do_cat.c				*
 *								*
 *	Escreve um arquivo na saída padrão			*
 *								*
 *	Versão	3.0.0, de 28.10.93				*
 *		3.0.0, de 10.06.95				*
 *								*
 *	Módulo: DOSMP						*
 *		Utilitários Sofisticados			*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 		Inspirado no PLDOS de Norival Ribeiro Figueira	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "../h/common.h"
#include "../h/dos.h"
#include "../h/proto.h"

/*
 ******	Protótipos de funções ***********************************
 */
void	do_cat_help (void);

/*
 ****************************************************************
 *	Escreve um arquivo na saída padrão			*
 ****************************************************************
 */
void
do_cat (int argc, const char *argv[])
{
	int		opt;
	int		n;
	const char	*nm;
	char		area[128];
	DOSSTAT		z;
	DOSFILE		f;

	/*
	 *	Analisa as opções
	 */
	while ((opt = cmd_getopt (argv, "H")) >= 0)
	{
		switch (opt)
		{
		    case 'H':			/* Help */
			do_cat_help ();
			return;

		    default:			/* Erro */
			fprintf (stderr, "\n");
			do_cat_help ();
			return;
		}
	}

	/*
	 *	Pequena consistência
	 */
	argc -= cmd_optind;
	argv += cmd_optind;

	if (argc != 1)
		{ do_cat_help (); return; }

	nm = argv[0];

	/*
	 *	Abre o arquivo
	 */
	if (dos_stat (nm, &z) < 0)
	{
		printf
		(	"%s: Não achei o arquivo \"%s\" (%s)\n",
			cmd_nm, nm, strerror (errno)
		);
		return;
	}

	if (!Z_ISREG (z.z_mode))
	{
		printf
		(	"%s: O arquivo \"%s\" não é regular\n",
			cmd_nm, nm
		);
		return;
	}

	if (dos_open (&f, &z) < 0)
		return;

	/*
	 *	Percorre os blocos do arquivo
	 */
	while ((n = dos_read (&f, area, sizeof (area))) > 0)
	{
		write (0, area, n);

		if (received_intr)
		{
			if (area[n-1] != '\n')
				printf ("\n");
			break;
		}
	}

}	/* end do_cat */

/*
 ****************************************************************
 *	Resumo de utilização do programa			*
 ****************************************************************
 */
void
do_cat_help (void)
{
	fprintf
	(	stderr,
		"%s - imprime um arquivo DOS na saída padrão\n"
		"\nSintaxe:\n"
		"\t%s <arquivo>\n",
		cmd_nm, cmd_nm
	);

}	/* end do_cat_help */
