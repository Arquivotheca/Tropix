/*
 ****************************************************************
 *								*
 *			map.c					*
 *								*
 *	Gera com muita ênfase as estruturas do sistema		*
 *								*
 *	Versão	4.3.0, de 04.07.02				*
 *		4.3.0, de 04.07.02				*
 *								*
 *	Módulo: mkfst1						*
 *		Utilitários especiais				*
 *		Categoria D					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/common.h>
#include <sys/sync.h>
#include <sys/syscall.h>
#include <sys/t1.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../h/mkfst1.h"

/*
 ****************************************************************
 *	Analisa o dispositivo					*
 ****************************************************************
 */
void
make_empty_bitmaps (void)
{
	daddr_t		size;
	daddr_t		bm_bno;
	char		*bp = bitmap;
	int		i;

	/*
	 *	Prepara a imagem do Bitmap para o caso usual
	 */
	memset (bp, ((T1_MAP_FREE << 4) | T1_MAP_FREE), sizeof (bitmap));

	T1_PUT_MAP (bp, T1_BMNO, T1_MAP_MAP);	/* Marca a entrada do próprio mapa */

	/*
	 *	Escreve os vários blocos de Bitmap
	 */
	for (bm_bno = T1_BMNO; bm_bno < sb.s_fs_sz; bm_bno += T1_ZONE_SZ)
	{
		if ((size = sb.s_fs_sz - (bm_bno - T1_BMNO)) < T1_ZONE_SZ)
		{
			for (i = size; i < T1_ZONE_SZ; i++)
			{
				T1_PUT_MAP (bp, i, T1_MAP_OUT);
			}
		}

		fs_write (bm_bno, bp);

		sb.s_busy_bl++;
	}

	/*
	 *	Prepara a imagem do Bitmap para o primeiro mapa de bits
	 *	Refaz a imagem do primeiro mapa de bits
	 */
	if (sb.s_fs_sz > T1_ZONE_SZ)
		memset (bp, ((T1_MAP_FREE << 4) | T1_MAP_FREE), sizeof (bitmap));

	T1_PUT_MAP (bp, T1_BOOTNO, T1_MAP_OUT);	/* Bloco de BOOT */
	T1_PUT_MAP (bp, T1_SBNO,   T1_MAP_OUT);	/* Bloco do Super-Bloco */
	T1_PUT_MAP (bp, T1_BMNO,   T1_MAP_MAP);	/* Marca a entrada do próprio mapa */

}	/* end make_empty_bitmaps */

/*
 ****************************************************************
 *	Procura um bloco livre no mapa				*
 ****************************************************************
 */
daddr_t
get_empty_block (int type)
{
	daddr_t		bno;
	char		*bp = bitmap;

	/*
	 *	Incialmente, procura o bloco livre
	 */
	for (bno = 0; /* abaixo */; bno++)
	{
		if (bno >= T1_ZONE_SZ)
			error ("$Disco cheio");

		if (T1_GET_MAP (bp, bno) == T1_MAP_FREE)
			break;
	}

	/*
	 *	Atribui o novo tipo
	 */
	T1_PUT_MAP (bp, bno, type);

	sb.s_busy_bl++;

	return (bno);

}	/* end get_empty_block */
