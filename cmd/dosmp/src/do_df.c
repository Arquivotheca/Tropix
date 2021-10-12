/*
 ****************************************************************
 *								*
 *			do_df.c					*
 *								*
 *	Imprime o espaço total e livre do sistema de arquivos	*
 *								*
 *	Versão	3.0.0, de 28.10.93				*
 *		3.0.0, de 09.06.96				*
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

#include <sys/common.h>

#include <stdio.h>

#include "../h/common.h"
#include "../h/dos.h"
#include "../h/proto.h"

/*
 ******	Protótipos de funções ***********************************
 */
void	do_df_help (void);

/*
 ****************************************************************
 *	Imprime o diretório corrente				*
 ****************************************************************
 */
void
do_df (int argc, const char *argv[])
{
	const UNI	*up = &uni;
	int		livres, total, usados, ruins, index;
	int		end_index;
	short		clusno;
	int		opt;

	/*
	 *	Analisa as opções
	 */
	while ((opt = cmd_getopt (argv, "H")) >= 0)
	{
		switch (opt)
		{
		    case 'H':			/* Help */
			do_df_help ();
			return;

		    default:			/* Erro */
			fprintf (stderr, "\n");
			do_df_help ();
			return;
		}
	}

	/*
	 *	Pequena consistência
	 */
	argc -= cmd_optind;
	argv += cmd_optind;

	if (argc != 0)
		{ do_df_help (); return; }

	/*
	 *	Verifica quantos "clusters" estão livres
	 */
	get_whole_fat ();

	for
	(	livres = 0, ruins = 0,
		index = 2, end_index = 2 + up->u_n_clusters;
		index < end_index;
		index++
	) 
	{
		if   (C_ISFREE (clusno = get_fat_value (index)))
			livres++;
		elif (C_ISBAD (clusno))
			ruins++;
	}

	total  = up->u_n_clusters;
	usados = total - livres - ruins;

	total  = BLTOKB (up->u_sectors_per_cluster * total);
	usados = BLTOKB (up->u_sectors_per_cluster * usados);
	livres = BLTOKB (up->u_sectors_per_cluster * livres);
	ruins  = BLTOKB (up->u_sectors_per_cluster * ruins);

	/*
	 *	Imprime o cabeçalho
	 */
	printf
	(	"RW DISPOSITIVO         SISTEMA    "
		" TOTAL  ALOCADOS  LIVRES  RUINS (KB)   %%\n"
	);

	/*
	 *	Imprime o resultado
	 */
	printf ("%s ", fs_status == FS_MOUNT_RO ? "RO" : "  ");
	printf ("%-20.20s", dev_nm);
	printf ("%-8.8s  ", sb.s_id);
	printf ("%7d   %7d %7d %6d      ", total, usados, livres, ruins);
	printf
	(	"%s %%\n",
		total == 0 ? "?" : editpercent (getpercent (usados, total - ruins))
	);

}	/* end do_df */

/*
 ****************************************************************
 *	Resumo de utilização do programa			*
 ****************************************************************
 */
void
do_df_help (void)
{
	fprintf
	(	stderr,
		"%s - imprime o espaço total e livre do sistema de arquivos\n"
		"\nSintaxe:\n"
		"\t%s\n",
		cmd_nm, cmd_nm
	);

}	/* end do_df_help */
