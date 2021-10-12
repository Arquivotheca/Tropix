/*
 ****************************************************************
 *								*
 *			block.c					*
 *								*
 *	Gerência de blocos do programa				*
 *								*
 *	Versão	4.3.0, de 09.07.02				*
 *		4.4.0, de 02.11.02				*
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
#include <sys/syscall.h>
#include <sys/t1.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <setjmp.h>

#include "../h/fsckt1.h"

/*
 ****************************************************************
 *	Lê um bloco do dispositivo				*
 ****************************************************************
 */
void
bread (void *area, daddr_t bno, int count)
{
	if (llseek (dev_fd, (loff_t)bno << BL4SHIFT, NULL, L_SET) < 0)
		error ("$*Erro de posicionamento para o bloco %d", bno);

	if (read (dev_fd, area, count) != count)
		error ("$*Erro de leitura do bloco %d", bno);

}	/* end bread */

/*
 ****************************************************************
 *	Escreve um Bloco 					*
 ****************************************************************
 */
void
bwrite (const void *area, daddr_t bno, int count)
{
	if (read_only)
		return;

	if (llseek (dev_fd, (loff_t)bno << BL4SHIFT, NULL, L_SET) < 0)
		error ("$*Erro de posicionamento para o bloco %d", bno);

	if (write (dev_fd, area, count) != count)
		error ("$*Erro de escrita no bloco %d", bno);

	fs_modified++;

}	/* end bwrite */

/*
 ****************************************************************
 *	Aloca um bloco do cache					*
 ****************************************************************
 */
void *
bget (CACHE *bp, daddr_t bno)
{
	if (bp->b_bno == bno)
		return (bp->b_area);

	bflush (bp);

	bread (bp->b_area, bno, BL4SZ);

	bp->b_bno = bno;

	return (bp->b_area);

}	/* end bget */

/*
 ****************************************************************
 *	Descarrega um buffer do cache				*
 ****************************************************************
 */
void
bflush (CACHE *bp)
{
	if (bp->b_dirty)
	{
		bwrite (bp->b_area, bp->b_bno, BL4SZ);

		bp->b_dirty = 0;
	}

}	/* end bflush */
