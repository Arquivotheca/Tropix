/*
 ****************************************************************
 *								*
 *			do_cd.c					*
 *								*
 *	Muda de diretório corrente				*
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
#include <string.h>
#include <errno.h>

#include "../h/common.h"
#include "../h/dos.h"
#include "../h/proto.h"

/*
 ******	Protótipos de funções ***********************************
 */
void	do_cd_help (void);

/*
 ******	Definições globais **************************************
 */
entry int	cwd_cluster;		/* Diretório corrente */

/*
 ****************************************************************
 *	Muda de diretório corrente				*
 ****************************************************************
 */
void
do_cd (int argc, const char *argv[])
{
	int		opt;
	const char	*dir_nm;
	DOSSTAT		z;

	/*
	 *	Analisa as opções
	 */
	while ((opt = cmd_getopt (argv, "H")) >= 0)
	{
		switch (opt)
		{
		    case 'H':			/* Help */
			do_cd_help ();
			return;

		    default:			/* Erro */
			fprintf (stderr, "\n");
			do_cd_help ();
			return;
		}
	}

	/*
	 *	Pequena consistência
	 */
	argc -= cmd_optind;
	argv += cmd_optind;

	if (argc != 1)
		{ do_cd_help (); return; }

	dir_nm = argv[0];

	/*
	 *	Procura o diretório
	 */
	if (dos_stat (dir_nm, &z) < 0)
	{
		printf
		(	"%s: Não achei o diretório \"%s\" (%s)\n",
			cmd_nm, dir_nm, strerror (errno)
		);
		return;
	}

	if (!Z_ISDIR (z.z_mode))
	{
		printf
		(	"O arquivo \"%s\" não é um diretório \n",
			dir_nm
		);
		return;
	}

	cwd_cluster = z.z_cluster;

}	/* end do_cd */

/*
 ****************************************************************
 *	Resumo de utilização do programa			*
 ****************************************************************
 */
void
do_cd_help (void)
{
	fprintf
	(	stderr,
		"%s - troca de diretório corrente DOS\n"
		"\nSintaxe:\n"
		"\t%s <novo_diretório>\n",
		cmd_nm, cmd_nm
	);

}	/* end help */
