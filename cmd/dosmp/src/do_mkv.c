/*
 ****************************************************************
 *								*
 *			do_mkv.c				*
 *								*
 *	Cria um nome de volume					*
 *								*
 *	Versão	3.0.0, de 15.11.93				*
 *		3.8.0, de 07.07.05				*
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

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <errno.h>

#include "../h/common.h"
#include "../h/dos.h"
#include "../h/proto.h"

/*
 ******	Protótipos de funções ***********************************
 */
void	mk_one_vol (const char *);
void	do_mkv_help (void);

/*
 ****************************************************************
 *	Cria um volume						*
 ****************************************************************
 */
void
do_mkv (int argc, const char *argv[])
{
	int		opt;

	/*
	 *	Analisa as opções
	 */
	while ((opt = cmd_getopt (argv, "H")) >= 0)
	{
		switch (opt)
		{
		    case 'H':			/* Help */
			do_mkv_help ();
			return;

		    default:			/* Erro */
			fprintf (stderr, "\n");
			do_mkv_help ();
			return;
		}
	}

	/*
	 *	Pequena consistência
	 */
	argc -= cmd_optind;
	argv += cmd_optind;

	if (argc != 1)
		{ do_mkv_help (); return; }

	/*
	 *	Cria o volume
	 */
	mk_one_vol (*argv);

}	/* end do_mkv */

/*
 ****************************************************************
 *	Cria um diretório					*
 ****************************************************************
 */
void
mk_one_vol (const char *path)
{
	const char	*cp, *end_bp, *son_nm;
	char		c, *bp;
	int		par_clusno;
	DOSSTAT		z;
	char		can_nm[sizeof (z.z_name) + 1 + sizeof (z.z_ext) + 1];
	char		par_nm[BLSZ];

	/*
	 *	Obtém o nome do diretório pai
	 */
	if   ((cp = strrchr (path, '/')) == NOSTR)
		{ strcpy (par_nm, "."); son_nm = path; }
	elif (cp == path)
		{ strcpy (par_nm, "/"); son_nm = cp + 1; }
	else
		{ strcpy (par_nm, path); par_nm[cp - path] = '\0'; son_nm = cp + 1; }

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
	 *	Converte para 8.3
	 */
	for (cp = son_nm, bp = can_nm, end_bp = bp + sizeof (z.z_name); (c = *cp) != '\0'; cp++)
	{
		if   (c == '.')
			continue;
		elif (islower (c))
			c = toupper (c);

		if (bp >= end_bp)
		{
			if (bp > can_nm + sizeof (z.z_name))
				break;

			*bp++ = '.';

			end_bp += 1 + sizeof (z.z_ext);
		}

		*bp++ = c;
	}

	*bp++ = '\0';

#ifdef	MSG
	if (!streq (can_nm, son_nm))
		printf ("Identificador canonizado para \"%s\"\n", can_nm);
#endif	MSG

	/*
	 *	Verifica se por acaso o arquivo filho já existe
	 */
	if (dos_fstat (can_nm, par_clusno, &z) >= 0)
	{
		printf
		(	"%s: O arquivo \"%s\" já existe\n",
			cmd_nm, can_nm
		);
		return;
	}

	/*
	 *	Prepara a entrada do volume
	 */
	z.z_mode = Z_VOL;
	put_dos_file_time (time ((time_t *)NULL), &z.z_d);
   	PUT_CLUSTER (0, &z);
	PUT_LONG (0, z.z_size);

	z.z_lfn_clusno = par_clusno;
	z.z_lfn_nm = (char *)can_nm;

	dos_lfn_dir_put (&z);

	/*
	 *	Verifica se alterou o nome do volume principal
	 */
	vol_nm[0] = '\0';

	dir_walk (vol_search, uni.u_root_cluster); 

}	/* end mk_one_vol */

/*
 ****************************************************************
 *	Resumo de utilização do programa			*
 ****************************************************************
 */
void
do_mkv_help (void)
{
	fprintf
	(	stderr,
		"%s - cria um nome de volume do sistema de arquivos DOS\n"
		"\nSintaxe:\n"
		"\t%s <nome_do_volume>\n",
		cmd_nm, cmd_nm
	);

}	/* end do_mkv_help */
