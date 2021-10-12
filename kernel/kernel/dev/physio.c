/*
 ****************************************************************
 *								*
 *			physio.c				*
 *								*
 *	Entrada/saída NÃO estruturada				*
 *								*
 *	Versão	1.0.0, de 28.08.86				*
 *		4.6.0, de 03.08.04				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include "../h/common.h"
#include "../h/sync.h"
#include "../h/scb.h"
#include "../h/region.h"

#include "../h/inode.h"
#include "../h/bhead.h"
#include "../h/map.h"
#include "../h/signal.h"
#include "../h/uproc.h"
#include "../h/uerror.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Definições globais					*
 ****************************************************************
 */
#define M64KBSZ		(64 * KBSZ)
#define M64KBMASK	(M64KBSZ - 1)
#define M16MBLIMIT	(16 * MBSZ)

/*
 ****************************************************************
 *	Entrada/saída NÃO estruturada				*
 ****************************************************************
 */
void
physio (IOREQ *iop, int (*strategy) (BHEAD *), BHEAD *bp, int rw, int dma)
{
	ulong		ph_area;
	ulong		cp_area = 0, cp_count = 0, area;
	pg_t		malloc_area = 0, malloc_size;
	int		count = iop->io_count;
	const char	*str;
	char		must_copy = 0;

	/*
	 *	Obtém o endereço físico inicial da área
	 */
	if ((ph_area = (unsigned)user_area_to_phys_area (iop->io_area, count)) == 0)
		{ u.u_error = EINVAL; return; }

	area = ph_area;

	/*
	 *	Verifica se precisa de realizar cópias (em virtude de DMA)
	 */
	if (dma)
	{
		if (ph_area + count > M16MBLIMIT)
			must_copy++;

		if ((ph_area & M64KBMASK) + count > M64KBSZ)
			must_copy++;
	}

	/*
	 *	Se for o caso, aloca uma área de cópia
	 */
	if (must_copy)
	{
		if ((malloc_area = malloc_dma_24 (&malloc_size)) == NOPG)
			{ str = "Não consegui obter memória"; goto bad; }

		area	 =
		cp_area  = VIRT_TO_PHYS_ADDR (PGTOBY (malloc_area));
		cp_count = PGTOBY (malloc_size);

#ifdef	MSG
		if (CSWT (31))
			printf ("physio (malloc_dma_24): %P, %d\n", cp_area, cp_count);
#endif	MSG

	}

	/*
	 *	Inicia o ciclo de operações
	 */
	SLEEPLOCK (&bp->b_lock, PBLKIO-1);

   /***	u.u_flags |= SLOCK; ***/

    more:
	count = iop->io_count;

	if (must_copy)
	{
		if (count > cp_count)
			count = cp_count;

		if (rw == B_WRITE)
		{
			memmove
			(	(void *)PHYS_TO_VIRT_ADDR (cp_area),
				(void *)PHYS_TO_VIRT_ADDR (ph_area),
				count
			);
		}
	}

	/*
	 *	Inicia a operação
	 */
	EVENTCLEAR (&bp->b_done);

	bp->b_flags = rw;
	bp->b_dev   =
	bp->b_phdev = iop->io_dev;
 
	bp->b_addr	 =
	bp->b_base_addr	 = (void *)PHYS_TO_VIRT_ADDR (area);
	bp->b_blkno	 =
	bp->b_phblkno    = iop->io_offset >> BLSHIFT;
	bp->b_base_count = count;
	bp->b_error	 = 0;

	u.u_error = NOERR;

#ifdef	MSG
	if (CSWT (31) && count > BLSZ)
		printf ("physio: %P, %d\n", area, count);
#endif	MSG

	(*strategy) (bp);
 
	/*
	 *	Ao fim do I/O testa erro
	 */
	EVENTWAIT (&bp->b_done, PBLKIO);
	count -= bp->b_residual;
	geterror (bp);

	if (must_copy && rw == B_READ && u.u_error == NOERR)
	{
		memmove
		(	(void *)PHYS_TO_VIRT_ADDR (ph_area),
			(void *)PHYS_TO_VIRT_ADDR (cp_area),
			count
		);
	}

	if (!must_copy)
		area += count;
	ph_area += count;

	iop->io_area   += count;
	iop->io_count  -= count;
	iop->io_offset += count;

	if (u.u_error == NOERR && iop->io_count > 0 && bp->b_residual == 0)
		goto more;

	/*
	 *	Epílogo
	 */
   /***	u.u_flags &= ~SLOCK; ***/

	SLEEPFREE (&bp->b_lock);

	if (must_copy)
		mrelease (M_CORE, malloc_size, malloc_area);

	return;
 
	/*
	 *	Erro
	 */
    bad:
	printf
	(	"%g: %s (area = %P, count = %d, offset = (%d, %d)\n",
		str, iop->io_area, iop->io_count, iop->io_offset_high, iop->io_offset_low
	);

	u.u_error = EINVAL;

}	/* end physio */
