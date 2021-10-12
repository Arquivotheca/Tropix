/*
 ****************************************************************
 *								*
 *			do_mkfile.c				*
 *								*
 *	Cria um arquivo regular de tamanho dado			*
 *								*
 *	Versão	3.0.0, de 05.04.96				*
 *		3.0.0, de 05.04.96				*
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
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>

#include "../h/common.h"
#include "../h/dos.h"
#include "../h/proto.h"

/*
 ******	Protótipos de funções ***********************************
 */
void	do_mkfile_help (void);

/*
 ****************************************************************
 *	Cria um arquivo regular de tamanho dado			*
 ****************************************************************
 */
void
do_mkfile (int argc, const char *argv[])
{
	const UNI	*up = &uni;
	const char	*cp;
	int		opt, mb;
	ulong		n_cluster, d_size;
	int		d_cluster;
	int		par_clusno;
	const char	*str;
	const char	*son_nm, *sz_str, *nm;
	DOSSTAT		z;
	char		par_nm[BLSZ];

	/*
	 *	Analisa as opções
	 */
	while ((opt = cmd_getopt (argv, "H")) >= 0)
	{
		switch (opt)
		{
		    case 'H':			/* Help */
			do_mkfile_help ();
			return;

		    default:			/* Erro */
			fprintf (stderr, "\n");
			do_mkfile_help ();
			return;
		}
	}

	/*
	 *	Pequena consistência
	 */
	argc -= cmd_optind;
	argv += cmd_optind;

	if (argc != 2)
		{ do_mkfile_help (); return; }

	nm = argv[0]; sz_str = argv[1];

	/*
	 *	Converte o tamanho dado de MB para CLUSTERs
	 */
	if ((mb = strtol (sz_str, &str, 0)) <= 0 || *str || mb >= 4 * KBSZ)
	{
		printf
		(	"%s: Tamanho inválido: \"%s\"\n",
			cmd_nm, sz_str
		);
		return;
	}

	d_size = (mb << MBSHIFT); n_cluster = d_size / CLUSZ;

	/*
	 *	Obtém o nome do diretório pai
	 */
	if   ((cp = strrchr (nm, '/')) == NOSTR)
		{ strcpy (par_nm, "."); son_nm = nm; }
	elif (cp == nm)
		{ strcpy (par_nm, "/"); son_nm = cp + 1; }
	else
		{ strcpy (par_nm, nm); par_nm[cp - nm] = '\0'; son_nm = cp + 1; }

	/*
	 *	Analisa o diretório pai
	 */
	if (dos_stat (par_nm, &z) < 0)
	{
		printf
		(	"%s: O diretório pai \"%s\" já deve existir (%s)\n",
			cmd_nm, par_nm, strerror (errno)
		);
		return;
	}

	if (!Z_ISDIR (z.z_mode))
	{
		printf
		(	"%s: O arquivo \"%s\" não é um diretório\n",
			cmd_nm, par_nm
		);
		return;
	}

	par_clusno = z.z_cluster;

	/*
	 *	Verifica se por acaso o diretório filho já existe
	 */
	if (dos_fstat (son_nm, par_clusno, &z) >= 0)
	{
		printf
		(	"%s: O arquivo \"%s\" já existe\n",
			cmd_nm, son_nm
		);
		return;
	}

	/*
	 *	Tenta alocar os CLUSTERs necessários
	 */
	if ((d_cluster = alloc_file_clusters (n_cluster)) < 0)
	{
		printf
		(	"%s: O Sistema de arquivos não possui %d MB "
			"contíguos livres\n",
			cmd_nm, mb
		);
		return;
	}

	/*
	 *	Prepara a entrada do pai para o filho
	 */
   /***	z.z_name = ...; ***/
   /***	z.z_ext = ...; ***/
	z.z_mode = Z_REG|Z_MODIF;
	put_dos_file_time (time ((time_t *)NULL), &z.z_d);
	PUT_CLUSTER (d_cluster, &z);
	PUT_LONG (d_size, z.z_size);

	z.z_lfn_clusno = par_clusno;
	z.z_lfn_nm = (char *)son_nm;

	dos_lfn_dir_put (&z);

}	/* end do_mkfile */

/*
 ****************************************************************
 *	Resumo de utilização do programa			*
 ****************************************************************
 */
void
do_mkfile_help (void)
{
	fprintf
	(	stderr,
		"%s - cria um arquivo regular de tamanho dado\n"
		"\nSintaxe:\n"
		"\t%s <arquivo> <MB sz>\n",
		cmd_nm, cmd_nm
	);

}	/* end do_mkfile_help */
