/*
 ****************************************************************
 *								*
 *			bread.c					*
 *								*
 *	Gerencia da entrada/saída estruturada (blocada)		*
 *								*
 *	Versão	3.0.0, de 06.10.94				*
 *		4.8.0, de 05.12.05				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2005 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include "../h/common.h"
#include "../h/scb.h"
#include "../h/sync.h"
#include "../h/region.h"

#include "../h/devhead.h"
#include "../h/bhead.h"
#include "../h/iotab.h"
#include "../h/signal.h"
#include "../h/uproc.h"
#include "../h/uerror.h"
#include "../h/mon.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Variáveis e definições globais				*
 ****************************************************************
 */
entry LOCK	updlock;	/* LOCK de Operacao de Update */

/*
 ****************************************************************
 *	Obtem um certo BLOCO (com a informação)			*
 ****************************************************************
 */
BHEAD *
bread (dev_t dev, daddr_t blkno, ino_t ino)
{
	BHEAD	 	*bp;
	int		offset;

	/*
	 *	Obtem um buffer para o bloco pedido
	 *	Devolve um buffer "locked"
	 */
	bp = block_get (dev, BL4BASEBLK (blkno), ino);

	offset = BL4BASEOFF (blkno);

	bp->b_addr  = bp->b_base_addr + offset;
	bp->b_count = BL4SZ - offset;

	/*
	 *	Se o bloco já foi lido, devolve-o
	 */
	if (EVENTTEST (&bp->b_done) == 0)
		{ geterror (bp); return (bp); }

	/*
	 *	O Bloco não foi lido; inicia a operação
	 */
	bp->b_flags |= B_READ;
	bp->b_base_count = BL4SZ;

	(*biotab[MAJOR(dev)].w_strategy) (bp);

	/*
	 *	Espera terminar a leitura
	 */
	EVENTWAIT (&bp->b_done, PBLKIO);

	geterror (bp);

	return (bp);

} 	/* end bread */

/*
 ****************************************************************
 *  Obtem um BLOCO (com a informação) e o Lookahead 		*
 ****************************************************************
 */
BHEAD *
breada (dev_t dev, daddr_t blkno, daddr_t rablkno, ino_t ino)
{
	BHEAD		*bp = NOBHEAD, *rabp;
	daddr_t		b4no = BL4BASEBLK (blkno);
	daddr_t		rab4no = BL4BASEBLK (rablkno);
	int		offset;

	/*
	 *	Verifica se o bloco principal já está em um buffer
	 */
	if (block_in_core (dev, b4no, ino) < 0)
	{
		bp = block_get (dev, b4no, ino);

		if (EVENTTEST (&bp->b_done) < 0)
		{
			bp->b_flags |= B_READ;
			bp->b_base_count = BL4SZ;

			(*biotab[MAJOR(dev)].w_strategy) (bp);
		}
	}

	/*
	 *	Verifica se o bloco seguinte já está disponivel
	 */
	if (rablkno && rab4no != b4no && block_in_core (dev, rab4no, ino) < 0)
	{
		rabp = block_get (dev, rab4no, ino);

		if (EVENTTEST (&rabp->b_done) < 0)
		{
			/*
			 *	Não está => Le assincronamente
			 */
			rabp->b_flags |= B_READ|B_ASYNC;
			rabp->b_base_count = BL4SZ;

			(*biotab[MAJOR(dev)].w_strategy) (rabp);
		}

		block_put (rabp);
	}

	/*
	 *	Verifica se o Bloco ainda não está em um buffer
	 */
	if (bp == NOBHEAD)
		return (bread (dev, blkno, ino));

	/*
	 *	Espera terminar a leitura do Bloco principal
	 */
	EVENTWAIT (&bp->b_done, PBLKIO);

	geterror (bp);

	offset = BL4BASEOFF (blkno);

	bp->b_addr  = bp->b_base_addr + offset;
	bp->b_count = BL4SZ - offset;

	return (bp);

}	/* end breada */

/*
 ****************************************************************
 *	Escreve um Bloco					*
 ****************************************************************
 */
void
bwrite (BHEAD *bp)
{
	/*
	 *	Inicia a Operação
	 */
	if (bp->b_flags & B_DIRTY)
		mon.y_block_dirty_cnt--;

	bp->b_flags &= ~(B_READ|B_ERROR|B_DIRTY);
	bp->b_base_count = BL4SZ;
	EVENTCLEAR (&bp->b_done);

	(*biotab[MAJOR(bp->b_dev)].w_strategy) (bp);

	/*
	 *	Se for escrita síncrona, aguarda o término
	 */
	if ((bp->b_flags & B_ASYNC) == 0)
		{ EVENTWAIT (&bp->b_done, PBLKIO); geterror (bp); }

	block_put (bp);

}	/* end bwrite */

/*
 ****************************************************************
 *	Marca um Bloco para ser escrito mais tarde		*
 ****************************************************************
 */
void
bdwrite (BHEAD *bp)
{
	DEVHEAD		*dp;

	dp = biotab[MAJOR(bp->b_dev)].w_tab;

	if (dp->v_flags & V_TAPE)
	{
		/*
		 *	É Fita Magnetica => Escreve (já) na ordem dada
		 */
		bp->b_flags |= B_ASYNC;
		bwrite (bp);
	}
	else
	{
		/*
		 *	Prepara para ser escrito só quando necessario
		 */
		if ((bp->b_flags & B_DIRTY) == 0)
			mon.y_block_dirty_cnt++;

		bp->b_flags |= B_DIRTY;
		bp->b_dirty_time = time;

		EVENTSET (&bp->b_done);

		block_put (bp);
	}

}	/* end bdwrite */

/*
 ****************************************************************
 *	Guarda o no. do erro					*
 ****************************************************************
 */
int
geterror (BHEAD *bp)
{
	if (bp->b_flags & B_ERROR)
	{
		if ((u.u_error = bp->b_error) == NOERR)
			u.u_error = EIO;

		return (-1);
	}

	return (0);

}	/* end geterror */
