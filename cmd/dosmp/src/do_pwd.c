/*
 ****************************************************************
 *								*
 *			do_pwd.c				*
 *								*
 *	Imprime o diretório corrente				*
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

#include "../h/common.h"
#include "../h/dos.h"
#include "../h/proto.h"

/*
 ******	Protótipos de funções ***********************************
 */
void	do_pwd_help (void);

/*
 ****************************************************************
 *	Imprime o diretório corrente				*
 ****************************************************************
 */
void
do_pwd (int argc, const char *argv[])
{
	char		*pp;
	const UNI	*up = &uni;
	int		opt, len;
	int		son_clusno, par_clusno;
	int		clusno;
	DOSSTAT		z;
	DOSFILE		f;
	char		path[BLSZ];

	/*
	 *	Analisa as opções
	 */
	while ((opt = cmd_getopt (argv, "H")) >= 0)
	{
		switch (opt)
		{
		    case 'H':			/* Help */
			do_pwd_help ();
			return;

		    default:			/* Erro */
			fprintf (stderr, "\n");
			do_pwd_help ();
			return;
		}
	}

	/*
	 *	Pequena consistência
	 */
	argc -= cmd_optind;
	argv += cmd_optind;

	if (argc != 0)
		{ do_pwd_help (); return; }

	/*
	 *	Verifica o caso particular da raiz
	 */
	if (cwd_cluster == up->u_root_cluster)
		{ printf ("/\n"); return; }

	/*
	 *	Prepara a área do caminho
	 */
	pp = path + sizeof (path) - 1; *pp = '\0';

	/*
	 *	Caminha a árvore de baixo para cima
	 *
	 *	Lembrar que a RAIZ NÃO tem ".." !
	 */
	for
	(	son_clusno = -1, clusno = cwd_cluster;
		son_clusno != up->u_root_cluster;
		son_clusno = clusno, clusno = par_clusno
	)
	{
		par_clusno = -1; len = -1;

		if (dos_open_by_clus (&f, clusno, BIG_SZ) < 0)
			return;

		dos_lfn_init (&z);

		while (dos_read (&f, &z.z_d, sizeof (DOSDIR)) > 0)
		{
			if (received_intr)
				return;

			if (z.z_name[0] == '\0')
				break;

			if (z.z_name[0] == Z_EMPTY)
				{ dos_lfn_reset (&z); continue; }

			if (Z_ISLFN (z.z_mode))
				{ dos_lfn_add (&z); continue; }

			dos_lfn_get_nm (&z); GET_CLUSTER (&z);

			if (streq (z.z_lfn_nm, ".."))
			{
				par_clusno = z.z_cluster;

				if (son_clusno < 0 || len >= 0)
					goto next_level;
			}

			if (z.z_cluster == son_clusno)
			{
				len = strlen (z.z_lfn_nm); pp -= len;

				strncpy (pp, z.z_lfn_nm, len); *--pp = '/';

				if (par_clusno >= 0 || clusno == up->u_root_cluster)
					goto next_level;
			}

			dos_lfn_reset (&z);

		}	/* end lendo o diretório */

		error ("pwd: Não achei o pai ou o filho (clusno = %d)", clusno);
		return;

	    next_level:
		/* vazio */;

	}	/* end for (pelos diretórios) */

	/*
	 *	Terminou: imprime
	 */
	printf ("%s\n", pp);

}	/* end do_pwd */

/*
 ****************************************************************
 *	Resumo de utilização do programa			*
 ****************************************************************
 */
void
do_pwd_help (void)
{
	fprintf
	(	stderr,
		"%s - imprime o diretório corrente DOS\n"
		"\nSintaxe:\n"
		"\t%s\n",
		cmd_nm, cmd_nm
	);

}	/* end do_pwd_help */
