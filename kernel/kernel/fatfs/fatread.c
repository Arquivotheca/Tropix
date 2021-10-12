/*
 ****************************************************************
 *								*
 *			fatread.c				*
 *								*
 *	Leitura e escrita de arquivos da FAT			*
 *								*
 *	Versão	4.2.0, de 14.12.01				*
 *		4.3.0, de 16.09.02				*
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

#include "../h/mntent.h"
#include "../h/sb.h"
#include "../h/fat.h"
#include "../h/inode.h"
#include "../h/stat.h"
#include "../h/bhead.h"
#include "../h/uerror.h"
#include "../h/signal.h"
#include "../h/uproc.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ******	Protótipos de funções ***********************************
 */
void		fat_read_root (IOREQ *iop);
void		fat_write_root (IOREQ *iop);

/*
 ****************************************************************
 *	Leitura de um INODE FAT					*
 ****************************************************************
 */
void
fat_read (IOREQ *iop)
{
	INODE		*ip = iop->io_ip;
	FATSB		*sp = ip->i_sb->sb_ptr;
	dev_t		dev = ip->i_dev;
	BHEAD		*bp;
	daddr_t		blkno, end_blkno;
	int		range;
	clusno_t	log_clusno, ph_clusno;
	off_t		offset, off;
	int		count, 	cnt;

	/*
	 *	Prepara algumas variáveis
	 */
	if (ip->i_sb->sb_mntent.me_flags & SB_ATIME)
		{ ip->i_atime = time; inode_dirty_inc (ip); }

	/*
	 *	Caso muito especial
	 */
	if (S_ISDIR (ip->i_mode) && ip->i_first_clusno == 0)
		return (fat_read_root (iop));

	/*
	 *	Le os diversos "clusters"
	 */
	do
	{
		if (S_ISDIR (ip->i_mode))
			range = 0x7FFFFFFF;	/* Termina pelo "cluster" */
		else
			range = ip->i_size - iop->io_offset_low;

		if (range <= 0)
			return;

		log_clusno = iop->io_offset_low >> sp->s_CLUSTER_SHIFT;
		offset	   = iop->io_offset_low &  sp->s_CLUSTER_MASK;
		count	   = MIN (sp->s_CLUSTER_SZ - offset, iop->io_count);

		if (count > range)
			count = range;

		/*
		 *	Processa os blocos de um "cluster"
		 */
		if (C_ISBAD_OR_EOF (ph_clusno = fat_cluster_read_map (ip, log_clusno)))
			return;

		FIRST_AND_LAST_BLK (ph_clusno, blkno, end_blkno);

		blkno += (offset >> BLSHIFT);

		do
		{
			if (blkno + 1 < end_blkno)
				bp = breada (dev, blkno, blkno + 1, 0);
			else
				bp = bread (dev, blkno, 0);

			off = offset & BLMASK;

			if ((cnt = bp->b_count - off) > count)
				cnt = count;

			if (unimove (iop->io_area, bp->b_addr + off, cnt, iop->io_cpflag) < 0)
				{ u.u_error = EFAULT; block_put (bp); return; }

			iop->io_area	   += cnt;
			iop->io_offset_low += cnt;
			iop->io_count	   -= cnt;

			count  -= cnt;
			offset += cnt;

			blkno += (bp->b_count >> BLSHIFT);

			block_put (bp);
		}	
		while (count > 0);
	}
	while (u.u_error == NOERR && iop->io_count > 0);

}	/* end fat_read */

/*
 ****************************************************************
 *	Leitura de um DIRETÓRIO RAIZ FAT 12/16			*
 ****************************************************************
 */
void
fat_read_root (IOREQ *iop)
{
	INODE		*ip = iop->io_ip;
	FATSB		*sp = ip->i_sb->sb_ptr;
	dev_t		dev = ip->i_dev;
	BHEAD		*bp;
	daddr_t		blkno, end_blkno;
	int		range;
	off_t		off;
	int		count, cnt;

	/*
	 *	Obtém os blocos
	 */
	ROOT_FIRST_AND_LAST_BLK (blkno, end_blkno);

	if ((range = ((end_blkno - blkno) << BLSHIFT) - iop->io_offset_low) <= 0)
		return;

	blkno += (iop->io_offset_low >> BLSHIFT);

	if ((count = iop->io_count) > range)
		count = range;

	/*
	 *	Le os diversos blocos
	 */
	do
	{
		if (blkno + 1 < end_blkno)
			bp = breada (dev, blkno, blkno + 1, 0);
		else
			bp = bread (dev, blkno, 0);

		off = iop->io_offset_low & BLMASK;

		if ((cnt = bp->b_count - off) > count)
			cnt = count;

		if (unimove (iop->io_area, bp->b_addr + off, cnt, iop->io_cpflag) < 0)
			{ u.u_error = EFAULT; block_put (bp); return; }

		iop->io_area	   += cnt;
		iop->io_offset_low += cnt;
		iop->io_count	   -= cnt;

		count -= cnt;

		blkno += (bp->b_count >> BLSHIFT);

		block_put (bp);
	}
	while (u.u_error == NOERR && count > 0);

}	/* end fat_read_root */

/*
 ****************************************************************
 *	Escrita de um INODE FAT					*
 ****************************************************************
 */
void
fat_write (IOREQ *iop)
{
	INODE		*ip = iop->io_ip;
	FATSB		*sp = ip->i_sb->sb_ptr;
	dev_t		dev = ip->i_dev;
	BHEAD		*bp;
	daddr_t		blkno, end_blkno;
	clusno_t	log_clusno, ph_clusno;
	off_t		offset, off;
	int		count, 	cnt;

	/*
	 *	Atualiza o tempo de modificação do INODE
	 */
	ip->i_mtime = time; inode_dirty_inc (ip);

	/*
	 *	Caso muito especial
	 */
	if (S_ISDIR (ip->i_mode) && ip->i_first_clusno == 0)
		return (fat_write_root (iop));

	/*
	 *	Malha de escrita
	 */
	do
	{
		log_clusno = iop->io_offset_low >> sp->s_CLUSTER_SHIFT;
		offset	   = iop->io_offset_low &  sp->s_CLUSTER_MASK;
		count	   = MIN (sp->s_CLUSTER_SZ - offset, iop->io_count);

		/*
		 *	Calcula o bloco físico
		 */
		if ((ph_clusno = fat_cluster_write_map (ip, log_clusno)) < 0)
			return;

		/*
		 *	Processa os blocos de um "cluster"
		 */
		FIRST_AND_LAST_BLK (ph_clusno, blkno, end_blkno);

		blkno += (offset >> BLSHIFT);

		do
		{
			bp = bread (dev, blkno, 0);

			if (u.u_error != NOERR)
				{ block_put (bp); return; }

			off = offset & BLMASK;

			if ((cnt = bp->b_count - off) > count)
				cnt = count;

			if (unimove (bp->b_addr + off, iop->io_area, cnt, iop->io_cpflag) < 0)
				{ u.u_error = EFAULT; block_put (bp); return; }

			iop->io_area	   += cnt;
			iop->io_offset_low += cnt;
			iop->io_count	   -= cnt;

			count  -= cnt;
			offset += cnt;

			blkno += (bp->b_count >> BLSHIFT);

			bdwrite (bp);

			/* Atualiza o tamanho do arquivo */

			if (iop->io_offset_low > ip->i_size)
				ip->i_size = iop->io_offset_low;
		}	
		while (count > 0);
	}
	while (u.u_error == NOERR && iop->io_count > 0);

}	/* end fat_write */

/*
 ****************************************************************
 *	Escrita de um DIRETÓRIO RAIZ FAT 12/16			*
 ****************************************************************
 */
void
fat_write_root (IOREQ *iop)
{
	INODE		*ip = iop->io_ip;
	FATSB		*sp = ip->i_sb->sb_ptr;
	dev_t		dev = ip->i_dev;
	BHEAD		*bp;
	daddr_t		blkno, end_blkno;
	int		range;
	off_t		off;
	int		cnt;

	/*
	 *	Obtém os blocos
	 */
	ROOT_FIRST_AND_LAST_BLK (blkno, end_blkno);

	if ((range = ((end_blkno - blkno) << BLSHIFT) - iop->io_offset_low) <= 0)
		{ u.u_error = ENOSPC; return; }

	blkno += (iop->io_offset_low >> BLSHIFT);

	if (iop->io_count > range)
		{ u.u_error = ENOSPC; return; }

	/*
	 *	Escreve os diversos blocos
	 */
	do
	{
		bp = bread (dev, blkno, 0);

		off = iop->io_offset_low & BLMASK;

		if ((cnt = bp->b_count - off) > iop->io_count)
			cnt = iop->io_count;

		if (unimove (bp->b_addr + off, iop->io_area, cnt, iop->io_cpflag) < 0)
			{ u.u_error = EFAULT; block_put (bp); return; }

		iop->io_area	   += cnt;
		iop->io_offset_low += cnt;
		iop->io_count	   -= cnt;

		blkno += (bp->b_count >> BLSHIFT);

		bdwrite (bp);
	}
	while (u.u_error == NOERR && iop->io_count > 0);

}	/* end fat_write_root */

/*
 ****************************************************************
 *	Rotina de "write" de um "link" simbólico 		*
 ****************************************************************
 */
INODE *
fat_write_symlink (const char *old_path, int old_count, IOREQ *iop, DIRENT *dep)
{
	u.u_error = EINVAL;

	iput (iop->io_ip);

	return (NOINODE);

}	/* end fat_write_symlink */
