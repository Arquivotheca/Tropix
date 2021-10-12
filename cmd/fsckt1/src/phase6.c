/*
 ****************************************************************
 *								*
 *			phase6.c				*
 *								*
 *	Fase 6: Verifica a lista de blocos livres		*
 *								*
 *	Versão	4.3.0, de 23.07.02				*
 *		4.3.0, de 16.08.02				*
 *								*
 *	Módulo: fsckt1						*
 *		Utilitários Especiais				*
 *		Categoria D					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/common.h>
#include <sys/sync.h>
#include <sys/t1.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>

#include "../h/fsckt1.h"

/*
 ****************************************************************
 *	Fase 6: Verifica a lista de blocos livres		*
 ****************************************************************
 */
int
phase6 (void)
{
	T1SB		*sp = (T1SB *)sb.b_area;
	daddr_t		size, zone_bno;
	char		*bp, *mp;
	int		i, mtype, btype;

	printf ("*** Fase 6: Verifica os mapas de bits\n");

	/*
	 *	Marca os blocos fora do sistema de arquivos
	 */
	if ((size = sp->s_fs_sz & T1_ZONE_MASK) > 0)
	{
		mp = mem_block_map + (sp->s_fs_sz / T1_ZONE_SZ) * BL4SZ;

		for (i = size; i < T1_ZONE_SZ; i++)
		{
			T1_PUT_MAP (mp, i, T1_MAP_OUT);
		}
	}

	/*
	 *	Compara os dois mapas de bits
	 */
	for
	(	zone_bno = 0, bp = disk_block_map, mp = mem_block_map;
		zone_bno < sp->s_fs_sz;
		zone_bno += T1_ZONE_SZ, bp += BL4SZ, mp += BL4SZ
	)
	{
		int		bad;

		if (memeq (mp, bp, BL4SZ))
			continue;

		for (bad = 0, i = 0; i < T1_ZONE_SZ; i++)
		{
			if ((mtype = T1_GET_MAP (mp, zone_bno + i)) != (btype = T1_GET_MAP (bp, zone_bno + i)))
			{
				bad++;

				if (vflag) printf
				(	"O Código do bloco %d está incorreto (%s :: %s)\n",
					zone_bno + i,
					edit_block_type (mtype), edit_block_type (btype)
				);
			}
		}

		printf ("\nO Mapa de Bits do bloco %d contém %d incorreções. ", zone_bno + T1_BMNO, bad);

		if (faskyesno ("Ajusta") > 0)
			bwrite (mp, zone_bno + T1_BMNO, BL4SZ);
	}

	return (CONTINUE);

}	/* end phase6 */
