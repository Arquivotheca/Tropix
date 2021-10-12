/*
 ****************************************************************
 *								*
 *			do_walk.c				*
 *								*
 *	Percorre uma árvore DOS					*
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
int	walk_function (const char *, DOSSTAT *, int);
void	do_walk_help (void);

/*
 ****************************************************************
 *	Percorre uma árvore DOS					*
 ****************************************************************
 */
void
do_walk (int argc, const char *argv[])
{
	int		opt;

	/*
	 *	Analisa as opções
	 */
	while ((opt = cmd_getopt (argv, "lH")) >= 0)
	{
		switch (opt)
		{
		    case 'l':			/* Longo */
			cmd_lflag++;
			break;

		    case 'H':			/* Help */
			do_walk_help ();
			return;

		    default:			/* Erro */
			fprintf (stderr, "\n");
			do_walk_help ();
			return;
		}
	}

	/*
	 *	Pequena consistência
	 */
	argc -= cmd_optind;
	argv += cmd_optind;

	/*
	 *	Percorre as árvores dadas
	 */
	if (argc == 0)
	{
		dos_ftw (".", walk_function);
	}
	else for (/* vazio */; *argv; argv++)
	{
		if (received_intr)
			break;

		dos_ftw (*argv, walk_function);
	}

}	/* end do_walk */

/*
 ****************************************************************
 *	Função chamada por "ftw"				*
 ****************************************************************
 */
int
walk_function (const char *nm, DOSSTAT *zp, int status)
{
	if (status == DOS_FTW_NS)
	{
		printf
		(	"%s: Não encontrei o arquivo \"%s\" (%s)\n",
			cmd_nm, nm, strerror (errno)
		);
		return (-1);
	}

	if (cmd_lflag > 1)
		printf ("%6d,%6d  ", zp->z_blkno, zp->z_offset);

	printf ("%c  ", file_type_edit (zp->z_mode));

	if (Z_ISREG (zp->z_mode))
		printf ("%9d  ", GET_LONG (zp->z_size));
	else
		printf ("           ");

	if (cmd_lflag)
	{

		printf ("%4d  ", zp->z_cluster);

		printf ("(%d)\t", status); 
	}

	printf ("%s", color_vector[file_color_convert (zp->z_mode)]);

	if (zp->z_mode & Z_HIDDEN)
		printf ("(%s)\n", nm);
	else
		printf (" %s\n", nm);

	printf ("%s", color_vector[MC_DEF]);

	return (0);

}	/* end walk_function */

/*
 ****************************************************************
 *	Resumo de utilização do programa			*
 ****************************************************************
 */
void
do_walk_help (void)
{
	fprintf
	(	stderr,
		"%s - percorre árvores DOS\n"
		"\nSintaxe:\n"
		"\t%s [-l] [<árvore> ...]\n",
		cmd_nm, cmd_nm
	);

	fprintf
	(	stderr,
		"\nOpções:"
		"\t-l: Listagem longa\n"
		"\nObs.:\tSe não for dada nenhuma <árvore>, é percorrido "
			"o diretório corrente\n"
	);

}	/* end do_walk_help */
