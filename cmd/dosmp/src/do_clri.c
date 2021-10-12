/*
 ****************************************************************
 *								*
 *			do_clri.c				*
 *								*
 *	Remove entradas de diretório (modo de pânico)		*
 *								*
 *	Versão	3.0.0, de 30.11.93				*
 *		3.0.0, de 03.03.95				*
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "../h/common.h"
#include "../h/dos.h"
#include "../h/proto.h"

/*
 ******	Protótipos de funções ***********************************
 */
void		simple_clri (const char *);
void		do_clri_help (void);

/*
 ****************************************************************
 *	Remove entradas de diretório (modo de pânico)		*
 ****************************************************************
 */
void
do_clri (int argc, const char *argv[])
{
	int		opt;

	/*
	 *	Analisa as opções
	 */
	while ((opt = cmd_getopt (argv, "vH")) >= 0)
	{
		switch (opt)
		{
		    case 'v':			/* Verboso */
			cmd_vflag++;
			break;

		    case 'H':			/* Help */
			do_clri_help ();
			return;

		    default:			/* Erro */
			fprintf (stderr, "\n");
			do_clri_help ();
			return;
		}
	}

	/*
	 *	Pequena consistência
	 */
	argc -= cmd_optind;
	argv += cmd_optind;

	if (argc == 0)
		{ do_clri_help (); return; }

	/*
	 *	Processa os arquivos
	 */
	for (/* vazio */; *argv; argv++)
	{
		if (received_intr)
			break;

		simple_clri (*argv);
	}

}	/* end do_clri */

/*
 ****************************************************************
 *	Move um arquivo						*
 ****************************************************************
 */
void
simple_clri (const char *path)
{
	DOSSTAT		z;

	/*
	 *	Obtém o estado do arquivo
	 */
	if (dos_stat (path, &z) < 0)
	{
		printf
		(	"%s: Não consegui obter o estado de \"%s\" (%s)\n",
			cmd_nm, path, strerror (errno)
		);
		return;
	}

	/*
	 *	Pede confirmação do usuario
	 */
	fprintf (stderr, "%s (cluster %d)? (n) : ", path, z.z_cluster);

	if (askyesno () <= 0)
		return;

	/*
	 *	Zera a entrada
	 */
	dos_lfn_dir_clr (&z);

}	/* end simple_clri */

/*
 ****************************************************************
 *	Resumo de utilização do programa			*
 ****************************************************************
 */
void
do_clri_help (void)
{
	fprintf
	(	stderr,
		"%s - remove entradas de diretório DOS (modo de PÂNICO)\n"
		"\nSintaxe:\n"
		"\t%s <arquivo> ...\n",
		cmd_nm, cmd_nm
	);

}	/* end do_clri_help */
