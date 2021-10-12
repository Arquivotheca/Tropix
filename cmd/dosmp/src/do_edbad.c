/*
 ****************************************************************
 *								*
 *			do_edbad.c				*
 *								*
 *	Examina CLUTERs ruins					*
 *								*
 *	Versão	4.8.0, de 22.09.05				*
 *		4.8.0, de 22.09.05				*
 *								*
 *	Módulo: DOSMP						*
 *		Utilitários Sofisticados			*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2005 NCE/UFRJ - tecle "man licença"	*
 * 		Inspirado no PLDOS de Norival Ribeiro Figueira	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/stat.h>
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
void		do_edbad_help (void);

/*
 ****************************************************************
 *	Examina CLUTERs ruins					*
 ****************************************************************
 */
void
do_edbad (int argc, const char *argv[])
{
	const UNI	*up = &uni;
	int		opt;
	int		clusno, value;
	int		title = 0, total = 0;

	/*
	 *	Analisa as opções
	 */
	while ((opt = cmd_getopt (argv, "ifvH")) >= 0)
	{
		switch (opt)
		{
		    case 'i':			/* Interativo */
			cmd_iflag++;
			break;

		    case 'f':			/* Força */
			cmd_fflag++;
			break;

		    case 'v':			/* Verboso */
			cmd_vflag++;
			break;

		    case 'H':			/* Help */
			do_edbad_help ();
			return;

		    default:			/* Erro */
			fprintf (stderr, "\n");
			do_edbad_help ();
			return;
		}
	}

	/*
	 *	Pequena consistência
	 */
	argc -= cmd_optind;
	argv += cmd_optind;

	/*
	 *	Lista os CLUSTERs ruins
	 */
	for (clusno = 0; clusno < up->u_n_clusters; clusno++)
	{
		value =  get_fat_value (clusno);

		if (C_ISBAD (value))
		{
			if (!title)
				{ printf ("---cluster-- ----bloco---\n\n"); title++; }

			printf ("%12d %12d\n", clusno, FIRST_BLK (clusno));

			total++;
		}
	}

	if (total) printf
	(	"\nTotal: %d clusters (%s)\n",
		total, cv_blocks_to_KB_MB_or_GB (total * up->u_sectors_per_cluster)
	);

}	/* end do_edbad */

/*
 ****************************************************************
 *	Resumo de utilização do programa			*
 ****************************************************************
 */
void
do_edbad_help (void)
{
	fprintf
	(	stderr,
		"%s - lista os CLUTERs ruins\n"
		"\nSintaxe:\n"
		"\t%s\n",
		cmd_nm, cmd_nm
	);
#if (0)	/*******************************************************/
	fprintf
	(	stderr,
		"\nOpções:"
		"\t-s: Somente o total de cada <árvore>\n"
		"\t-a: Informação para cada arquivo\n"
		"\nObs.:\tAs opções \"-a\" e \"-s\" são mutuamente exclusivas.\n"
		"\tUma <árvore> pode também ser um arquivo normal.\n"
	);
#endif	/*******************************************************/

}	/* end help */
