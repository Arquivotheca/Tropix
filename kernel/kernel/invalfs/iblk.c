/*
 ****************************************************************
 *								*
 *			iblk.c					*
 *								*
 *	Leitura e escrita de dispositivos estruturados		*
 *								*
 *	Versão	4.2.0, de 26.11.01				*
 *		4.2.0, de 26.11.01				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2001 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include "../h/common.h"
#include "../h/sync.h"
#include "../h/scb.h"
#include "../h/region.h"

#include "../h/mntent.h"
#include "../h/sb.h"
#include "../h/iotab.h"
#include "../h/kfile.h"
#include "../h/inode.h"
#include "../h/bhead.h"
#include "../h/uerror.h"
#include "../h/signal.h"
#include "../h/uproc.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Open de INODE (BLK)					*
 ****************************************************************
 */
void
blk_open (INODE *ip, int oflag)
{
	dev_t		dev;
	int		major;

	/*
	 *	Recebe e devolve o INODE locked
	 */
	dev = ip->i_rdev; major = MAJOR (dev);

	if ((unsigned)major >= scb.y_nblkdev)
		{ u.u_error = ENXIO; return; }

	(*biotab[major].w_open) (dev, oflag);

}	/* end blk_open */

/*
 ****************************************************************
 *	Close de INODE (BLK)					*
 ****************************************************************
 */
void
blk_close (INODE *ip)
{
	/*
	 *	Se não era o ultimo usuário de um dispositivo, ...
	 */
	if (ip->i_count > 1)
	{
		/* O "close" antes do close do INODE */

		(*biotab[MAJOR(ip->i_rdev)].w_close) (ip->i_rdev, 1);

		/* Simplesmente decrementa o uso */

		iput (ip); return;
	}

	/*
	 *	O "close" final do INODE
	 */
	block_flush (ip->i_rdev); block_free (ip->i_rdev, NOINO);

	(*biotab[MAJOR(ip->i_rdev)].w_close) (ip->i_rdev, 0);

	iput (ip);

}	/* end blk_close */

/*
 ****************************************************************
 *	Leitura de um INODE (BLK)				*
 ****************************************************************
 */
void
blk_read (IOREQ *iop)
{
	INODE		*ip = iop->io_ip;
	BHEAD		*bp;
	dev_t		dev;
	daddr_t		pbn;
	int		offset, count;

	/*
	 *	Prepara algumas variáveis
	 */
	if (ip->i_sb->sb_mntent.me_flags & SB_ATIME)
		{ ip->i_atime = time; inode_dirty_inc (ip); }

	dev = ip->i_rdev;

	/*
	 *	Lê os blocos
	 */
	do
	{
		pbn	= BL4BASEBLK ((long)(iop->io_offset >> BLSHIFT));
		offset	= iop->io_offset & BL4MASK;
		count	= MIN (BL4SZ - offset, iop->io_count);

		/*
		 *	Le um bloco
		 */
		if (ip->i_lastr + (BL4SZ/BLSZ) == pbn)
			bp = breada (dev, pbn, pbn + (BL4SZ/BLSZ), 0);
		else
			bp = bread (dev, pbn, 0);

		ip->i_lastr = pbn;

		/*
		 *	Move a Informação para a area desejada
		 */
		count = MIN (count, BL4SZ - bp->b_residual);

		if (unimove (iop->io_area, bp->b_base_addr + offset, count, iop->io_cpflag) < 0)
			{ u.u_error = EFAULT; return; }

		iop->io_area   += count;
		iop->io_offset += count;
		iop->io_count  -= count;

		block_put (bp);
	}
	while (u.u_error == NOERR && iop->io_count > 0 && count > 0);

}	/* end blk_read */

/*
 ****************************************************************
 *	Escrita de um INODE (BLK)				*
 ****************************************************************
 */
void
blk_write (IOREQ *iop)
{
	INODE		*ip = iop->io_ip;
	BHEAD		*bp;
	dev_t		dev;
	daddr_t		pbn;
	int		count, offset;

	/*
	 *	Calcula alguns valores
	 */
	dev = ip->i_rdev;

	ip->i_mtime = time; inode_dirty_inc (ip);

	/*
	 *	Malha de escrita
	 */
	do
	{
		pbn	= BL4BASEBLK ((long)(iop->io_offset >> BLSHIFT));
		offset	= iop->io_offset & BL4MASK;
		count	= MIN (BL4SZ - offset, iop->io_count);

		/*
		 *	Obtem o Bloco para escrever
		 */
		if (count == BL4SZ) /* && offset == 0 */
			bp = block_get (dev, pbn, 0);
		else
			bp = bread (dev, pbn, 0);

		/*
		 *	Coloca a informação no buffer e o escreve
		 */
		if (unimove (bp->b_base_addr + offset, iop->io_area, count, iop->io_cpflag) < 0)
			{ u.u_error = EFAULT; block_put (bp); return; }

		iop->io_area   += count;
		iop->io_offset += count;
		iop->io_count  -= count;

		if (u.u_error != NOERR)
			block_put (bp);
		else
			bdwrite (bp);
	}
	while (u.u_error == NOERR && iop->io_count > 0);

}	/* end blk_write */
