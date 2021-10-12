/*
 ****************************************************************
 *								*
 *			phase2.c				*
 *								*
 *	Fase 2: Procura arquivos com blocos duplicados		*
 *								*
 *	Versão	4.3.0, de 16.07.02				*
 *		4.3.0, de 26.07.02				*
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
 *	Fase 2: Procura arquivos com blocos duplicados		*
 ****************************************************************
 */
int
phase2 (void)
{
	INO_HASH	*ip, **hp;

	/*
	 *	Identifica os arquivos com os blocos duplicados
	 */
	printf ("*** Fase 2: Procura arquivos com blocos duplicados\n");

	if (dup_block_sz <= 0)
		return (CONTINUE);

	/*
	 *	Examina cada INODE presente na tabela HASH
	 */
	for (hp = &inode_hash_tb[0]; hp < &inode_hash_tb[INO_HASH_SZ]; hp++)
	{
		for (ip = *hp; ip != NO_INO_HASH; ip = ip->i_next)
		{
			if (ip->i_state == CLR_STATE)
				continue;

			fs_n_dup_blk = 0;

			file_block_scan (inode_get (ip->i_ino), ip, search_dup_blocks);
		}
	}

	return (CONTINUE);

}	/* end phase2 */

/*
 ****************************************************************
 *	Função caminhante de análise de blocos duplicados	*
 ****************************************************************
 */
int
search_dup_blocks (T1DINO *np, INO_HASH *ip, daddr_t bno, int type)
{
	DUPBLK		*dup_p;
	int		i;

	if (bno <= T1_BMNO || bno >= ((T1SB *)sb.b_area)->s_fs_sz)
		return (CONTINUE);

	for (dup_p = dup_block_tb, i = 0; i < dup_block_sz; dup_p++, i++)
	{
		if (dup_p->dup_bno == bno)
		{
			if (fs_n_dup_blk++ == 0)
			{
				print_inode (ip->i_ino, np);
				printf ("Arquivo com bloco(s) duplicado(s)\n\n");
			}

			ip->i_state = CLR_STATE;
		}
	}

	return (CONTINUE);

}	/* end search_dup_blocks */
