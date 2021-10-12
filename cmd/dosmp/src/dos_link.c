/*
 ****************************************************************
 *								*
 *			dos_link.c				*
 *								*
 *	Cria/remove entradas de diretório			*
 *								*
 *	Versão	3.0.0, de 20.10.93				*
 *		3.0.0, de 09.06.95				*
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

/*
 ****************************************************************
 *	Cria uma entrada de diretório				*
 ****************************************************************
 */
int
dos_link (const DOSSTAT *old_zp, const char *new_path)
{
	const char	*cp;
	char		par_nm[BLSZ];
	const char	*son_nm;
	int		par_clusno;
	DOSSTAT		z;

	/*
	 *	Obtém o nome do diretório pai
	 */
	if   ((cp = strrchr (new_path, '/')) == NOSTR)
		{ strcpy (par_nm, "."); son_nm = new_path; }
	elif (cp == new_path)
		{ strcpy (par_nm, "/"); son_nm = cp + 1; }
	else
		{ strcpy (par_nm, new_path); par_nm[cp - new_path] = '\0'; son_nm = cp + 1; }

	/*
	 *	Analisa o diretório pai
	 */
	if (dos_stat (par_nm, &z) < 0)
	{
		printf
		(	"%s: O diretório pai \"%s\" já deve existir (%s)\n",
			cmd_nm, par_nm, strerror (errno)
		);
		return (-1);
	}

	if (!Z_ISDIR (z.z_mode))
	{
		printf
		(	"%s: O arquivo \"%s\" não é um diretório\n",
			cmd_nm, par_nm
		);
		return (-1);
	}

	par_clusno = z.z_cluster;

	/*
	 *	Verifica se por acaso o arquivo filho já existe
	 */
	if (dos_fstat (son_nm, par_clusno, &z) >= 0)
	{
		printf
		(	"%s: O arquivo \"%s\" já existe\n",
			cmd_nm, son_nm
		);
		return (-1);
	}

	memmove (&z, old_zp, sizeof (DOSSTAT));

	z.z_lfn_clusno = par_clusno;
	z.z_lfn_nm = (char *)son_nm;

	dos_lfn_dir_put (&z);

	return (0);

}	/* end dos_link */

/*
 ****************************************************************
 *	Remove uma entrada de diretório dado o STAT		*
 ****************************************************************
 */
int
dos_unlink (DOSSTAT *zp, int truncate)
{
	if (truncate)
	{
		if (Z_ISREG (zp->z_mode) || Z_ISDIR (zp->z_mode))
			put_cluster_list (zp->z_cluster);
	}

	dos_lfn_dir_clr (zp);

	return (0);

}	/* end dos_unlink */
