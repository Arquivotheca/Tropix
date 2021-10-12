/*
 ****************************************************************
 *								*
 *			dir_walk.c				*
 *								*
 *   Executa um função para todas as entradas de um diretório	*
 *								*
 *	Versão	3.0.0, de 28.10.93				*
 *		3.0.0, de 26.02.96				*
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
#include <ctype.h>

#include "../h/common.h"
#include "../h/dos.h"
#include "../h/proto.h"

/*
 ******	Protótipos de funções ***********************************
 */

/*
 ****************************************************************
 *   Executa um função para todas as entradas de um diretório	*
 ****************************************************************
 */
int
dir_walk (int (*function) (const DOSSTAT *), int clusno)
{
	int		n;
	DOSFILE		f;
	DOSSTAT		z;

	/*
	 *	Pequena inicialização
	 */
	if (dos_open_by_clus (&f, clusno, BIG_SZ) < 0)
		return (-1);

	dos_lfn_init (&z);

	/*
	 *	Percorre o diretório
	 */
	while (dos_read (&f, &z.z_d, sizeof (DOSDIR)) > 0)
	{
		if (z.z_name[0] == '\0')
			break;

		if (z.z_name[0] == Z_EMPTY)
			{ dos_lfn_reset (&z); continue; }

		if (Z_ISLFN (z.z_mode))
			{ dos_lfn_add (&z); continue; }

		dos_lfn_get_nm (&z);
		GET_CLUSTER (&z);

		if ((n = (*function) (&z)) < 0)
			return (n);

		dos_lfn_reset (&z);

	}	/* end lendo o diretório */

	return (0);

}	/* end dir_walk */
