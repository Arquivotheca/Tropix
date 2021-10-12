/*
 ****************************************************************
 *								*
 *			ntmount.c				*
 *								*
 *	Diversas funções da montagem de sistemas de arquivos	*
 *								*
 *	Versão	4.6.0, de 28.06.04				*
 *		4.6.0, de 12.07.04				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2004 NCE/UFRJ - tecle "man licença"	*
 *		Baseado no FreeBSD 5.2				*
 *								*
 ****************************************************************
 */

#include "../h/common.h"
#include "../h/sync.h"
#include "../h/scb.h"
#include "../h/region.h"

#include "../h/disktb.h"
#include "../h/mntent.h"
#include "../h/sb.h"
#include "../h/nt.h"
#include "../h/ustat.h"
#include "../h/iotab.h"
#include "../h/inode.h"
#include "../h/bhead.h"
#include "../h/kfile.h"
#include "../h/uerror.h"
#include "../h/signal.h"
#include "../h/uproc.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ******	Protótipos de funções ***********************************
 */
int		nt_get_free_clusters (const SB *unisp);

/*
 ****************************************************************
 *	Certifica-se de que o sistema de arquivos é NT		*
 ****************************************************************
 */
int
nt_authen (dev_t dev)
{
	BHEAD			*bp;
	const struct ntboot	*ntp;
	int			status = -1;
	DISKTB			*up;

	/*
	 *	Confere a assinatura mágica
	 */
	u.u_error = NOERR;

	bp = bread (dev, NT_SBNO, 0);

	ntp = (struct ntboot *)bp->b_addr;

	if   (u.u_error)
		/* vazio */;
	elif (!memeq (ntp->s_sysid, "NTFS    ", 8))
		u.u_error = ENOTFS;
	elif (ntp->s_media != 0xF8)
		u.u_error = ENOTFS;
	elif (status = 0, (up = disktb_get_entry (dev)) != NODISKTB && up->p_type == 0)
		up->p_type = PAR_NTFS;

	block_put (bp);

	return (status);

}	/* end nt_authen */

/*
 ****************************************************************
 *	Operação de MOUNT					*
 ****************************************************************
 */
int
nt_mount (SB *sp)
{
	BHEAD			*bp;
	const struct ntboot	*ntp;
	NTSB			*ntsp;

	/*
	 *	Aloca a parte específica do NTSB
	 */
	if ((ntsp = malloc_byte (sizeof (NTSB))) == NONTSB)
		{ u.u_error = ENOMEM; return (-1); }

	memclr (ntsp, sizeof (NTSB));

	bp = bread (sp->sb_dev, NT_SBNO, 0);

	if (u.u_error)
		goto out_1;

	ntp = (struct ntboot *)bp->b_addr;

	if (ntp->s_spv_high != 0)
		{ u.u_error = EFBIG; goto out_1; }

	/*
	 *	Prepara o SB
	 */
	ntsp->s_bytes_per_sector	= *(ushort *)&ntp->s_bps;
	ntsp->s_sectors_per_cluster	= ntp->s_spc;

	ntsp->s_CLUSTER_SZ		= ntsp->s_bytes_per_sector * ntsp->s_sectors_per_cluster;
	ntsp->s_CLUSTER_SHIFT		= log2 (ntsp->s_CLUSTER_SZ);
	ntsp->s_CLUSTER_MASK		= ntsp->s_CLUSTER_SZ - 1;

	if (ntsp->s_CLUSTER_SHIFT < 0)
		{ u.u_error = ENOTFS; goto out_1; }

	ntsp->s_total_sectors		= ntp->s_spv_low;
	ntsp->s_MFT_cluster_no		= ntp->s_mftcn_low;
   /***	ntsp->s_MFT_mirror_cluster_no	= ntp->s_mftmirrcn_low;	***/

	if (ntp->s_mftrecsz > 0)
		ntsp->s_MFT_record_size	= ntp->s_mftrecsz << ntsp->s_CLUSTER_SHIFT;
	else
		ntsp->s_MFT_record_size	= (1 << -ntp->s_mftrecsz);

	block_put (bp);

	ntsp->s_COMPRESS_BLOCK_SIZE	= NTFS_COMPRESS_CLUSTER_SIZE << ntsp->s_CLUSTER_SHIFT;

   /***	idclr (sp->sb_fname);	***/
   /***	idclr (sp->sb_fpack);	***/

	sp->sb_code	= FS_NT;
	sp->sb_sub_code	= 0;		/* Por enquanto, sem o uso de compressão */
	sp->sb_ptr	= ntsp;
	sp->sb_mntent.me_flags |= SB_RONLY;

	sp->sb_root_ino	= NTFS_ROOTINO;

   /***	ntsp->s_mft_inode = NOINODE;	***/

	return (0);

	/*
	 *	Em caso de ERRO
	 */
    out_1:
	block_put (bp);
	free_byte (ntsp);

	return (-1);

}	/* end nt_mount */

/*
 ****************************************************************
 *	Algumas operações que devem ser feitas após a montagem	*
 ****************************************************************
 */
void
ntfs_mount_epilogue (SB *sp)
{
	NTSB		*ntsp = sp->sb_ptr;
	INODE		*vol_ip;
	int		size;
	struct ntvattr	*vap;

	/*
	 *	Obtém o INODE do MFT e o nome do VOLUME
	 */
	if ((ntsp->s_mft_inode = iget (sp->sb_dev, NTFS_PSEUDO_MFTINO, 0)) == NOINODE)
		{ printf ("%g: NÃO consegui obter o INODE do MFT de \"%v\"\n", sp->sb_dev); return; }

	iinc (ntsp->s_mft_inode);

	iput (ntsp->s_mft_inode);

	/*
	 *	Obtém o nome do VOLUME
	 */
	if ((vol_ip = iget (sp->sb_dev, NTFS_VOLUMEINO, 0)) == NOINODE)
		{ printf ("%g: NÃO consegui obter o INODE do VOLUME de \"%v\"\n", sp->sb_dev); return; }

	if ((vap = ntfs_ntvattrget (ntsp, vol_ip, NTFS_A_VOLUMENAME, NOSTR, 0, 1)) == NULL)
	{
		iput (vol_ip);
		printf ("%g: NÃO consegui obter o atributo do nome do VOLUME de \"%v\"\n", sp->sb_dev);
		return;
	}

	if ((size = vap->va_datalen >> 1) > IDSZ)
		size = IDSZ;

	little_unicode2iso ((char *)sp->sb_fname, vap->va_datap, size);

	iput (vol_ip);

}	/* end ntfs_mount_epilogue */

/*
 ****************************************************************
 *	Obtém a estatística do sistema de arquivos NT		*
 ****************************************************************
 */
void
nt_get_ustat (const SB *unisp, USTAT *up)
{
	NTSB		*ntsp = unisp->sb_ptr;

	/*
	 *	Obtém o número de CLUSTERs livres
	 */
	if (ntsp->s_free_clusters == 0)
		ntsp->s_free_clusters = nt_get_free_clusters (unisp);

	/*
	 *	Preenche a área do usuário
	 */
	up->f_type	 = FS_NT;
	up->f_sub_type	 = unisp->sb_sub_code;		/* Indica uso de compressão */
	up->f_bsize	 = ntsp->s_bytes_per_sector;
	up->f_nmsize	 = NTFS_MAXFILENAME;
	up->f_fsize	 = ntsp->s_total_sectors;
	up->f_tfree	 = ntsp->s_free_clusters * ntsp->s_sectors_per_cluster;
	up->f_isize	 = 0;
	up->f_tinode	 = 0;
	strcpy (up->f_fname, unisp->sb_fname);
	strcpy (up->f_fpack, unisp->sb_fpack);
	up->f_m		 = 1;
	up->f_n		 = 1;
	up->f_symlink_sz = 0;

}	/* end nt_get_ustat */

/*
 ****************************************************************
 *	Calcula o número de CLUSTERs livres			*
 ****************************************************************
 */
int
nt_get_free_clusters (const SB *unisp)
{
	NTSB		*ntsp = unisp->sb_ptr;
	INODE		*ip;
	struct ntvattr	*vap;
	char		*bitmap;
	int		bitmap_sz, i, j, free_clusters;
	IOREQ		io;

	/*
	 *	Conta o número de CLUSTERs livres do mapa de BITs
	 */
	if ((ip = iget (unisp->sb_dev, NTFS_BITMAPINO, 0)) == NOINODE)
		goto bad0;

	if ((vap = ntfs_ntvattrget (ntsp, ip, NTFS_A_DATA, NOSTR, 0, 1)) == NULL)
		goto bad1;

	bitmap_sz = vap->va_datalen;

	if ((bitmap = malloc_byte (bitmap_sz)) == NOSTR)
		{ u.u_error = ENOMEM; goto bad1; }

	io.io_ip	 = ip;
	io.io_area	 = bitmap;
	io.io_count	 = bitmap_sz;
	io.io_offset_low = 0;
	io.io_cpflag	 = SS;

	if (ntfs_readattr (ntsp, NTFS_A_DATA, NOSTR, &io) < 0)
		goto bad2;

	for (free_clusters = 0, i = 0; i < bitmap_sz; i++)
	{
                for (j = 0; j < 8; j++)
		{
                        if (~bitmap[i] & (1 << j))
				free_clusters++;
		}
	}

	free_byte (bitmap);
	iput (ip);

	return (free_clusters);

	/*
	 *	Em caso de erro, ...
	 */
    bad2:
	free_byte (bitmap);
    bad1:
	iput (ip);
    bad0:
	printf ("%g: NÃO consegui obter o número de CLUSTERs livres de \"%v\"\n", unisp->sb_dev);

	return (0);

}	/* end nt_get_free_clusters */

/*
 ****************************************************************
 *	Lê uma seqüência de bytes através de vários blocos	*
 ****************************************************************
 */
int
ntfs_bread (IOREQ *iop)
{
	BHEAD		*bp;
	daddr_t		blkno;
	off_t		offset;
	int		count, range;

#ifdef	DEBUG
	printf
	(	"ntfs_bread: dev = %P, cpflag = %d, area = %P, offset = %d, count = %d\n",
		iop->io_dev, iop->io_cpflag, iop->io_area, iop->io_offset_low, iop->io_count
	);
#endif	DEBUG

	/*
	 *	Lê os vários blocos
	 */
	while (iop->io_count > 0)
	{ 
		blkno  = iop->io_offset >> BLSHIFT;
		offset = iop->io_offset_low & BLMASK;

		bp = bread (iop->io_dev, blkno, 0);

		if (u.u_error)
			{ block_put (bp); return (-1); }

		range = bp->b_count - offset;

		if ((count = iop->io_count) > range)
			count = range;

		if (unimove (iop->io_area, bp->b_addr, count, iop->io_cpflag) < 0)
			{ u.u_error = EFAULT; return (-1); }

		block_put (bp);

		iop->io_area	+= count;
		iop->io_count	-= count;
		iop->io_offset	+= count;
	}

	return (0);

}	/* end ntfs_bread */
