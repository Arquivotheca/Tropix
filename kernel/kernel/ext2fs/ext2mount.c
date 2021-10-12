/*
 ****************************************************************
 *								*
 *			ext2mount.c				*
 *								*
 *	Diversas funções da montagem de sistemas de arquivos	*
 *								*
 *	Versão	4.4.0, de 24.10.02				*
 *		4.7.0, de 09.12.04				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2004 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include <stddef.h>

#include "../h/common.h"
#include "../h/sync.h"
#include "../h/scb.h"
#include "../h/region.h"

#include "../h/disktb.h"
#include "../h/mntent.h"
#include "../h/sb.h"
#include "../h/ext2.h"
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

#undef	DEBUG

/*
 ******	Protótipos de funções ***********************************
 */
void		ext2_print_sb (const EXT2SB *ext2sp);

/*
 ****************************************************************
 *	Certifica-se de que o sistema de arquivos é EXT2	*
 ****************************************************************
 */
int
ext2_authen (dev_t dev)
{
	BHEAD		*bp;
	int		status = -1;
	DISKTB		*up;

	/*
	 *	Confere a assinatura mágica
	 */
	u.u_error = NOERR;

	bp = bread (dev, EXT2_SBNO, 0);

	if   (u.u_error)
		/* vazio */;
	elif ((((EXT2SB *)bp->b_addr)->s_magic) != EXT2_SBMAGIC)
		u.u_error = ENOTFS;
	elif (status = 0, (up = disktb_get_entry (dev)) != NODISKTB && up->p_type == 0)
		up->p_type = PAR_LINUX_EXT2;

	block_put (bp);

	return (status);

}	/* end ext2_authen */

/*
 ****************************************************************
 *	Operação de MOUNT					*
 ****************************************************************
 */
int
ext2_mount (SB *sp)
{
	BHEAD		*bp;
	EXT2SB		*ext2sp;
	const EXT2BG	*rbgp, *end_rbgp;
	EXT2BG		*wbgp;
	int		group_cnt, group;
	daddr_t		bg_bno;

	/*
	 *	Lê o superbloco e verifica quantos são os grupos
	 */
	bp = bread (sp->sb_dev, EXT2_SBNO, 0);

	if (u.u_error)
		{ block_put (bp); return (-1); }

	ext2sp = (EXT2SB *)bp->b_addr;

	if (ext2sp->s_magic != EXT2_SBMAGIC)
		{ u.u_error = ENOTFS; block_put (bp); return (-1); }

	group_cnt = (ext2sp->s_blocks_count - ext2sp->s_first_data_block + ext2sp->s_blocks_per_group - 1)
						/ ext2sp->s_blocks_per_group;

	/*
	 *	Aloca a parte específica do EXT2SB
	 */
	if ((ext2sp = malloc_byte (sizeof (EXT2SB) + (group_cnt - 1) * sizeof (EXT2BG))) == NOEXT2SB)
		{ u.u_error = ENOMEM; block_put (bp); return (-1); }

	memmove (ext2sp, bp->b_addr, sizeof (EXT2SB));

   /***	block_put (bp); bp = NOBHEAD; ***/

	ext2sp->s_group_cnt = group_cnt;

	/*
	 *	Lê os descritores de grupos
	 */
	end_rbgp = rbgp = NOEXT2BG; bg_bno = EXT2_BGNO (ext2sp->s_log_block_size);

	for (wbgp = ext2sp->s_bg, group = 0; group < group_cnt; group++, wbgp++, rbgp++)
	{
		if (rbgp >= end_rbgp)
		{
		   /***	if (bp != NOBHEAD) ***/
				block_put (bp);

			bp = bread (sp->sb_dev, bg_bno, 0);

			if (u.u_error)
				{ block_put (bp); return (-1); }

			rbgp = bp->b_addr; end_rbgp = bp->b_addr + bp->b_count;

			bg_bno += (BYTOBL (bp->b_count));
		}

		if (rbgp->bg_block_bitmap >= ext2sp->s_blocks_count)
		{
			printf ("%g: Grupo com endereço inválido, dev = %v\n", sp->sb_dev);
			u.u_error = ENOTFS; block_put (bp); return (-1);
		}

		memmove (wbgp, rbgp, sizeof (EXT2BG));
	}

	block_put (bp);

	/*
	 *	Prepara uma série de valores úteis
	 */
	if (ext2sp->s_log_block_size > 2)				/* Só até blocos de 4 KB */
		{ u.u_error = ENOTFS; return (-1); }

	ext2sp->s_BLOCKtoBL_SHIFT = 1 + ext2sp->s_log_block_size;	/* Blocos para blocos de 512 B */

	ext2sp->s_BLOCK_SHIFT = KBSHIFT + ext2sp->s_log_block_size;	/* Blocos para bytes */
	ext2sp->s_BLOCK_SZ    = (1 <<  ext2sp->s_BLOCK_SHIFT);
	ext2sp->s_BLOCK_MASK  = ext2sp->s_BLOCK_SZ - 1;

	ext2sp->s_INDIR_SHIFT = KBSHIFT + ext2sp->s_log_block_size - 2;	/* No. de blocos indiretos */
	ext2sp->s_INDIR_SZ    = (1 <<  ext2sp->s_INDIR_SHIFT);
	ext2sp->s_INDIR_MASK  = ext2sp->s_INDIR_SZ - 1;

	ext2sp->s_INOperBLOCK_SZ    = ext2sp->s_BLOCK_SZ / sizeof (EXT2DINO);	/* No. de INODEs/bloco */
	ext2sp->s_INOperBLOCK_MASK  = ext2sp->s_INOperBLOCK_SZ - 1;
	ext2sp->s_INOperBLOCK_SHIFT = log2 (ext2sp->s_INOperBLOCK_SZ);

	if (ext2sp->s_INOperBLOCK_SHIFT < 0)
		{ u.u_error = ENOTFS; return (-1); }

	strcpy (sp->sb_fname, ext2sp->s_volume_name);

	sp->sb_code	= FS_EXT2;
	sp->sb_sub_code = ext2sp->s_log_block_size;
	sp->sb_ptr	= ext2sp;

	sp->sb_root_ino	= EXT2_ROOT_INO;

#ifdef	DEBUG
	ext2_print_sb (ext2sp);
#endif	DEBUG

	return (0);

}	/* end ext2_mount */

/*
 ****************************************************************
 *	Obtém a estatística do sistema de arquivos EXT2		*
 ****************************************************************
 */
void
ext2_get_ustat (const SB *unisp, USTAT *up)
{
	EXT2SB	*sp = unisp->sb_ptr;

	up->f_type	 = FS_EXT2;
	up->f_sub_type	 = 0;
	up->f_bsize	 = (KBSZ << sp->s_log_block_size);
	up->f_nmsize	 = MAXNAMLEN;
	up->f_fsize	 = sp->s_blocks_count;
	up->f_tfree	 = sp->s_free_blocks_count;
	up->f_isize	 = sp->s_inodes_count;
	up->f_tinode	 = sp->s_free_inodes_count;
	strcpy (up->f_fname, unisp->sb_fname);
	strcpy (up->f_fpack, unisp->sb_fpack);
	up->f_m		 = 1;
	up->f_n		 = 1;
	up->f_symlink_sz = EXT2_NADDR * sizeof (daddr_t);

}	/* end ext2_get_ustat */

/*
 ****************************************************************
 *	Atualiza um Super Bloco EXT2				*
 ****************************************************************
 */
void
ext2_update (const SB *sp)
{
	EXT2SB		*ext2sp = sp->sb_ptr;
	const EXT2BG	*rbgp;
	EXT2BG		*wbgp, *end_wbgp;
	BHEAD		*bp = NOBHEAD;
	int		group;
	daddr_t		bg_bno;

	/*
	 *	Escreve os descritores de grupos
	 */
	end_wbgp = wbgp = NOEXT2BG; bg_bno = EXT2_BGNO (ext2sp->s_log_block_size);

	for (rbgp = ext2sp->s_bg, group = 0; group < ext2sp->s_group_cnt; group++, wbgp++, rbgp++)
	{
		if (wbgp >= end_wbgp)
		{
			if (bp != NOBHEAD)
				bwrite (bp);

			bp = bread (sp->sb_dev, bg_bno, 0);

			if (u.u_error)
				break;

			wbgp = bp->b_addr; end_wbgp = bp->b_addr + bp->b_count;

			bg_bno += (BYTOBL (bp->b_count));
		}

		memmove (wbgp, rbgp, sizeof (EXT2BG));
	}

	bwrite (bp);

	/*
	 *	Escreve o SB no Disco
	 */
	bp = bread (sp->sb_dev, EXT2_SBNO, 0);

	memmove (bp->b_addr, sp->sb_ptr, offsetof (EXT2SB, s_end_member));

	bwrite (bp);

#ifdef	MSG
	if (CSWT (6))
		printf ("%g: SB, dev = %v\n", sp->sb_dev);
#endif	MSG

}	/* end ext2_update */

#ifdef	DEBUG
/*
 ****************************************************************
 *	Imprime algumas informações do SB			*
 ****************************************************************
 */
void
ext2_print_sb (const EXT2SB *ext2sp)
{
	const EXT2BG	*rbgp;
	int		group;

	printf ("Tamanho do sistema	= %d\n", ext2sp->s_blocks_count);
	printf ("Número de grupos	= %d\n", ext2sp->s_group_cnt);
	printf ("Blocos por groupo	= %d\n", ext2sp->s_blocks_per_group);
	printf ("Inodes por groupo	= %d\n", ext2sp->s_inodes_per_group);

	for (rbgp = ext2sp->s_bg, group = 0; group < ext2sp->s_group_cnt; group++, rbgp++)
	{
		printf
		(	"%d: b_bitmap = %d, i_bitmap = %d, i_table = %d, "
			"free_b = %d, free_i = %d, used_dir = %d\n",
			group, rbgp->bg_block_bitmap, rbgp->bg_inode_bitmap, rbgp->bg_inode_table,
			rbgp->bg_free_blocks_count, rbgp->bg_free_inodes_count, rbgp->bg_used_dirs_count
		);
	}

	printf
	(	"log_block_size = %d, BLOCKtoBL_SHIFT = %d\n",
		ext2sp->s_log_block_size, ext2sp->s_BLOCKtoBL_SHIFT
	);

	printf
	(	"BLOCK_SHIFT = %d, BLOCK_SZ = %d, BLOCK_MASK = %d\n",
		ext2sp->s_BLOCK_SHIFT, ext2sp->s_BLOCK_SZ, ext2sp->s_BLOCK_MASK
	);

	printf
	(	"INDIR_SHIFT = %d, INDIR_SZ = %d, INDIR_MASK = %d\n",
		ext2sp->s_INDIR_SHIFT, ext2sp->s_INDIR_SZ, ext2sp->s_INDIR_MASK
	);

	printf
	(	"INOperBLOCK_SHIFT = %d, INOperBLOCK_SZ = %d, INOperBLOCK_MASK = %d\n",
		ext2sp->s_INOperBLOCK_SHIFT, ext2sp->s_INOperBLOCK_SZ, ext2sp->s_INOperBLOCK_MASK
	);

}	/* end ext2_print_sb */
#endif	DEBUG
