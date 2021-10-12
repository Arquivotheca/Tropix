/*
 ****************************************************************
 *								*
 *			cdread.c				*
 *								*
 *	Leitura no Sistema de Arquivos ISO9660			*
 *								*
 *	Versão	4.0.0, de 01.02.92				*
 *		4.0.0, de 23.04.92				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include "../h/common.h"
#include "../h/sync.h"
#include "../h/scb.h"
#include "../h/region.h"

#include "../h/iotab.h"
#include "../h/kfile.h"
#include "../h/cdfs.h"
#include "../h/inode.h"
#include "../h/bhead.h"
#include "../h/uerror.h"
#include "../h/signal.h"
#include "../h/uproc.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Rotina de "read"					*
 ****************************************************************
 */
void
cd_read (IOREQ *iop)
{
	INODE		*ip = iop->io_ip;
	BHEAD		*bp;
	dev_t		dev = ip->i_dev;
	daddr_t		blkno;
	off_t		offset;
	int		range, count;

	/*
	 *	Lê os diversos blocos
	 */
	do
	{
		if ((range = ip->i_size - iop->io_offset_low) <= 0)
			break;

		blkno	= ip->i_first_block + (iop->io_offset_low >> ISO_BLSHIFT);
		offset	= iop->io_offset_low & ISO_BLMASK;

		if (ip->i_lastr + 1 == blkno)
			bp = breada (dev, ISOBL_TO_BL (blkno), ISOBL_TO_BL (blkno + 1), 0);
		else
			bp = bread (dev, ISOBL_TO_BL (blkno), 0);

		ip->i_lastr = blkno;

		count = bp->b_count - offset;

		if (count > iop->io_count)
			count = iop->io_count;

		if (count > range)
			count = range;

		/*
		 *	Move a Informação para a area desejada
		 */
		if (unimove (iop->io_area, bp->b_addr + offset, count, iop->io_cpflag) < 0)
			{ u.u_error = EFAULT; block_put (bp); break; }

		iop->io_area	   += count;
		iop->io_offset_low += count;
		iop->io_count	   -= count;

		block_put (bp);
	}
	while (u.u_error == NOERR && iop->io_count > 0 && count > 0);

}	/* end cd_read */
