/*
 ****************************************************************
 *								*
 *			fatmount.c				*
 *								*
 *	Montagem de sistemas de arquivos FAT			*
 *								*
 *	Versão	4.2.0, de 12.12.01				*
 *		4.9.0, de 27.09.06				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2006 NCE/UFRJ - tecle "man licença"	*
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
#include "../h/ustat.h"
#include "../h/fat.h"
#include "../h/bhead.h"
#include "../h/uerror.h"
#include "../h/signal.h"
#include "../h/uproc.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Certifica-se de que o sistema de arquivos é FAT		*
 ****************************************************************
 */
int
fat_authen (dev_t dev)
{
	BHEAD		*bp;
	const FATSB0	*fp;
	DISKTB		*up;
	int		code;

	u.u_error = NOERR;

	bp = bread (dev, 0, 0); fp = bp->b_addr;

	if (u.u_error)
		goto out_1;

	if (fp->f_boot_valid[0] != 0x55 || fp->f_boot_valid[1] != 0xAA)
		goto out_2;

	if (fp->f_sectors_per_cluster <= 0)
		goto out_2;

	if (FAT_GET_SHORT (fp->f_bytes_per_sector) != BLSZ)
		goto out_2;

	if ((unsigned)fp->f_n_fats > 2)
		goto out_2;

	/*
	 *	Analisa detalhes diferentes para várias FATs
	 */
	if   (memeq (fp->f_32_fat_msg, "FAT32", 5))
	{
		code = PAR_DOS_FAT32_L;

		if
		(	FAT_GET_SHORT (fp->f_root_dir_entries) != 0 ||
			FAT_GET_SHORT (fp->f_little_size) != 0 ||
			FAT_GET_SHORT (fp->f_little_sectors_per_fat) != 0 ||
			FAT_GET_SHORT (fp->f_version) != 0	||
			fp->f_32_signature != 0x29
		)
			goto out_2;
	}
	elif (memeq (fp->f_16_fat_msg, "FAT16", 5))
	{
		code = PAR_DOS_FAT16;

		if
		(	fp->f_16_signature != 0x29	||
			FAT_GET_SHORT (fp->f_root_dir_entries) == 0 ||
			FAT_GET_SHORT (fp->f_little_sectors_per_fat) == 0
		)
			goto out_2;
	}
	elif (memeq (fp->f_16_fat_msg, "FAT12", 5))
	{
		code = PAR_DOS_FAT12;

		if
		(	FAT_GET_SHORT (fp->f_root_dir_entries) == 0 ||
			FAT_GET_SHORT (fp->f_little_sectors_per_fat) == 0
		)
			goto out_2;
	}
	else
	{
		goto out_2;
	}

	block_put (bp);

	/*
	 *	Atualiza a entrada DISKTB, se for o caso ...
	 */
	if ((up = disktb_get_entry (dev)) != NODISKTB && up->p_type == 0)
		up->p_type = code;

	return (0);

	/*
	 *	Em caso de erro, ...
	 */
    out_2:
	u.u_error = ENOTFS;
    out_1:
	block_put (bp);

	return (-1);

}	/* end fat_authen */
/*
 ****************************************************************
 *	Monta um sistema de arquivos FAT			*
 ****************************************************************
 */
int
fat_mount (SB *unisp)
{
	enum	       { BL4BLMASK = BL4SZ/BLSZ - 1 };
	FATSB		*sp;
	const FATSB0	*fp;
	BHEAD		*bp;
	const FATDIR	*dp, *end_dp;
	daddr_t		blkno;
	FATLFN		z;

	/*
	 *	Aloca a parte específica do FATSB
	 */
	if ((sp = malloc_byte (sizeof (FATSB))) == NOFATSB)
		{ u.u_error = ENOMEM; return (-1); }

	memset (sp, 0, sizeof (FATSB));

	bp = bread (unisp->sb_dev, 0, 0);

	if (u.u_error)
		goto out_7;

	fp = bp->b_addr;

	/*
	 *	Inicialmente examina parâmetros básicos
	 */
	if (fp->f_boot_valid[0] != 0x55 || fp->f_boot_valid[1] != 0xAA)
		{ u.u_error = ENOTFS; goto out_7; }

	if ((sp->s_end_fs_blkno = FAT_GET_SHORT (fp->f_little_size)) == 0)
		sp->s_end_fs_blkno = FAT_GET_LONG (fp->f_big_size);

	if   (memeq (fp->f_32_fat_msg, "FAT32", 5))
	{
		sp->s_fat_bits = 32;
	}
	elif (memeq (fp->f_16_fat_msg, "FAT16", 5))
	{
		sp->s_fat_bits = 16;
	}
	elif (memeq (fp->f_16_fat_msg, "FAT12", 5))
	{
		sp->s_fat_bits = 12;
	}
	else
	{
		sp->s_fat_bits = 0;	/* Será decidido pelo número de CLUSTERs */
	}

	if ((sp->s_sectors_per_cluster = fp->f_sectors_per_cluster) <= 0)
		{ u.u_error = ENOTFS; goto out_7; }

	if ((sp->s_bytes_per_sector = FAT_GET_SHORT (fp->f_bytes_per_sector)) != BLSZ)
		{ u.u_error = ENOTFS; goto out_7; }

	sp->s_CLUSTER_SZ	= sp->s_bytes_per_sector * sp->s_sectors_per_cluster;
	sp->s_CLUSTER_MASK	= sp->s_CLUSTER_SZ - 1;
	sp->s_CLUSTER_SHIFT	= log2 (sp->s_CLUSTER_SZ);

	if (sp->s_CLUSTER_SHIFT < 0)
		{ u.u_error = ENOTFS; goto out_7; }

	if ((unsigned)(sp->s_n_fats = fp->f_n_fats) > 2)
		{ u.u_error = ENOTFS; goto out_7; }

	sp->s_fat1_blkno = FAT_GET_SHORT (fp->f_fat_offset);

	sp->s_sectors_per_track = FAT_GET_SHORT (fp->f_sectors_per_track);

	sp->s_n_heads = FAT_GET_SHORT (fp->f_n_heads);

	sp->s_inode_time = time;	/* Tempo simulado para os INODEs */

	/*
	 *	Analisa detalhes diferentes para várias FATs
	 */
	if (sp->s_fat_bits == 32) 	/* FAT de 32 bits */
	{
		if
		(	FAT_GET_SHORT (fp->f_root_dir_entries) != 0 ||
			FAT_GET_SHORT (fp->f_little_size) != 0 ||
			FAT_GET_SHORT (fp->f_little_sectors_per_fat) != 0 ||
			FAT_GET_SHORT (fp->f_version) != 0
		)
			{ u.u_error = ENOTFS; goto out_7; }

		if (fp->f_32_signature != 0x29)
			{ u.u_error = ENOTFS; goto out_7; }

		sp->s_sectors_per_fat	= FAT_GET_LONG (fp->f_big_sectors_per_fat);

	   /***	sp->s_fat1_blkno	= FAT_GET_SHORT (fp->f_fat_offset); ***/
		sp->s_end_fat1_blkno	= sp->s_fat1_blkno + sp->s_sectors_per_fat;

		sp->s_fat2_blkno	= sp->s_end_fat1_blkno;
		sp->s_end_fat2_blkno	= sp->s_fat2_blkno;

		if (sp->s_n_fats > 1)
			sp->s_end_fat2_blkno += sp->s_sectors_per_fat;

		sp->s_root_dir_entries	= 0;
		sp->s_root_dir_blkno	= sp->s_end_fat2_blkno;
		sp->s_end_root_dir_blkno = sp->s_end_fat2_blkno;

		sp->s_root_cluster	= FAT_GET_LONG (fp->f_root_cluster);
		sp->s_cluster_0_blkno	= sp->s_end_fat2_blkno - 2 * sp->s_sectors_per_cluster;
		sp->s_n_clusters	= (sp->s_end_fs_blkno - sp->s_end_fat2_blkno) / sp->s_sectors_per_cluster;
	}
	else				/* FAT de 16/12 bits */
	{
		if (sp->s_fat_bits == 16 && fp->f_16_signature != 0x29)
			{ u.u_error = ENOTFS; goto out_7; }

		if
		(	FAT_GET_SHORT (fp->f_root_dir_entries) == 0 ||
			FAT_GET_SHORT (fp->f_little_sectors_per_fat) == 0
		)
			{ u.u_error = ENOTFS; goto out_7; }

		sp->s_sectors_per_fat	= FAT_GET_SHORT (fp->f_little_sectors_per_fat);

	   /***	sp->s_fat1_blkno	= FAT_GET_SHORT (fp->f_fat_offset); ***/
		sp->s_end_fat1_blkno	= sp->s_fat1_blkno + sp->s_sectors_per_fat;

		sp->s_fat2_blkno	= sp->s_end_fat1_blkno;
		sp->s_end_fat2_blkno	= sp->s_fat2_blkno;

		if (sp->s_n_fats > 1)
			sp->s_end_fat2_blkno += sp->s_sectors_per_fat;

		sp->s_root_dir_entries	= FAT_GET_SHORT (fp->f_root_dir_entries);
		sp->s_root_dir_blkno	= sp->s_end_fat2_blkno;

		sp->s_end_root_dir_blkno = sp->s_root_dir_blkno +
				sp->s_root_dir_entries * sizeof (FATDIR) / sp->s_bytes_per_sector;

		sp->s_root_cluster	= 0;
		sp->s_cluster_0_blkno	= sp->s_end_root_dir_blkno - 2 * sp->s_sectors_per_cluster;
		sp->s_n_clusters	= (sp->s_end_fs_blkno - sp->s_end_root_dir_blkno) / sp->s_sectors_per_cluster;
	}

	/*
	 *	Distingue entre FAT16 e FAT12
	 */
	if (sp->s_fat_bits == 0)
	{
		if (sp->s_n_clusters + 2 <= 0xFF1)
			sp->s_fat_bits = 12;
		else
			sp->s_fat_bits = 16;
	}

	switch (sp->s_fat_bits)
	{
	    case 32: 			/* FAT de 32 bits */
		sp->s_BAD_BEGIN = 0x0FFFFFF1;
		sp->s_EOF_BEGIN = 0x0FFFFFF8;
		sp->s_STD_EOF   = 0x0FFFFFFF;

		sp->s_sb_backup = FAT_GET_SHORT (fp->f_sb_backup);
		sp->s_fs_info	= FAT_GET_SHORT (fp->f_fs_info);
		break;

	    case 16: 			/* FAT de 16 bits */
		sp->s_BAD_BEGIN = 0xFFF1;
		sp->s_EOF_BEGIN = 0xFFF8;
		sp->s_STD_EOF   = 0xFFFF;

		break;

	    case 12: 			/* FAT de 12 bits */
		sp->s_BAD_BEGIN = 0xFF1;
		sp->s_EOF_BEGIN = 0xFF8;
		sp->s_STD_EOF   = 0xFFF;
		break;
	}

	block_put (bp);

	/*
	 *	Verifica se há um bloco de informações da FAT32
	 */
	if (sp->s_fs_info != 0)
	{
		FATSB1		*fsinfo;

		bp = bread (unisp->sb_dev, sp->s_fs_info, 0);

		if (u.u_error)
			goto out_7;

		fsinfo = bp->b_addr;

	   /***	bread (sp->s_fs_info,	  &fsinfo, 0);	***/
	   /***	bread (sp->s_fs_info + 1, (void *)&fsinfo + BLSZ, 0); ***/

		if
		(	memeq (fsinfo->fs_isig1, "RRaA", 4) &&
			memeq (fsinfo->fs_isig2, "rrAa", 4) &&
			memeq (fsinfo->fs_isig3, "\0\0\x55\xAA", 4)
		/*** && memeq (fsinfo->fs_isig4, "\0\0\x55\xAA", 4) ***/
		)
		{
			sp->s_disk_infree   =
			sp->s_infree	    = FAT_GET_LONG (fsinfo->fs_infree);
			sp->s_disk_inxtfree =
			sp->s_inxtfree	    = FAT_GET_LONG (fsinfo->fs_inxtfree);
#ifdef	DEBUG
			printf
			(	"FSINFO: totalfree = %d, nextfree = %d\n",
				sp->s_infree, sp->s_inxtfree
			);
#endif	DEBUG
		}
		else
		{
			sp->s_fs_info = 0;
		}

		block_put (bp);
	}

	/*
	 *	Atualiza as entradas do SB
	 */
   /***	idclr (unisp->sb_fname); ***/
	sprintf ((char *)unisp->sb_fpack, "%v", unisp->sb_dev);

	unisp->sb_code		= FS_FAT;
	unisp->sb_sub_code	= sp->s_fat_bits;
	unisp->sb_ptr		= sp;

	unisp->sb_root_ino	= FAT_ROOT_INO;

	/*
	 *	Lê o primeiro BLOCO da RAIZ para procurar o nome do volume
	 */
	bp = bread (unisp->sb_dev, sp->s_root_dir_blkno, 0);

	FAT_LFN_INIT (&z);

	for (dp = bp->b_addr, end_dp = (FATDIR *)((int)dp + BLSZ); dp < end_dp; dp++)
	{
		if (dp->d_name[0] == '\0')		/* Final do diretório */
			break;

		if (dp->d_name[0] == FAT_EMPTY)		/* Entrada vazia */
			{ FAT_LFN_RESET (&z); continue; }

		if (FAT_ISLFN (dp->d_mode))
			{ fat_lfn_add (dp, &z); continue; }

		/* Terminou um nome: examina */

		fat_lfn_get_nm (unisp, dp, &z);

		if (FAT_ISVOL (dp->d_mode))
			strscpy (unisp->sb_fname, z.z_lfn_nm, sizeof (unisp->sb_fname));

		FAT_LFN_RESET (&z);
	}

	block_put (bp);

	/*
	 *	Verifica se o último bloco do último CLUSTER exige ler após o sistema de arquivos
	 */
	blkno = sp->s_cluster_0_blkno + (sp->s_n_clusters + 2) * sp->s_sectors_per_cluster;

	if ((blkno & BL4BLMASK) && (blkno & ~BL4BLMASK) + BL4BLMASK + 1 > sp->s_end_fs_blkno)
	{
		int	value = fat_get_fat_value (unisp, sp->s_n_clusters + 1);

		if (!C_ISBAD (value))
		    printf ("%g: Último CLUSTER da partição FAT \"%v\" em posição inválida\n", unisp->sb_dev);
	}

	return (0);

	/*
	 *	Em caso de ERRO
	 */
    out_7:
	block_put (bp);

	free_byte (sp);

	return (-1);

}	/* end fat_mount */

/*
 ****************************************************************
 *	Obtém estatísticas sobre o sistema de arquivos FAT	*
 ****************************************************************
 */
void
fat_get_ustat (const SB *unisp, USTAT *up)
{
	const FATSB	*sp = unisp->sb_ptr;
	BHEAD		*bp;
	daddr_t		blkno;
	const char	*cp, *end_cp;
	clusno_t	clusno;
	int		n, free = 0, bad = 0;
	int		count, cnt;
	char		first_block = 1;
	void		*area, *area_ptr;
	ulong		l;

	/*
	 *	Verifica o tipo de FAT
	 */
	n = sp->s_n_clusters;

	blkno = sp->s_fat1_blkno;

	if   (sp->s_fat_bits == 32)		/* FAT 32 */
	{
		free = sp->s_infree;
	}
	elif (sp->s_fat_bits != 12)		/* FAT 16 */
	{
		do
		{
			bp = bread (unisp->sb_dev, blkno, 0);

			cp = bp->b_addr; end_cp = cp + bp->b_count;

			if   (sp->s_fat_bits == 16)
			{
				if (first_block)
					{ cp += 4; first_block = 0; }

				for (/* acima */; cp < end_cp; cp += 2)
				{
					clusno = FAT_GET_SHORT (cp);

					if   (C_ISFREE (clusno))
						free++;
					elif (C_ISBAD (clusno))
						bad++;

					if (--n <= 0)
						break;
				}
			}
			else	/* fat_bits == 32 */
			{
				if (first_block)
					{ cp += 8; first_block = 0; }

				for (/* acima */; cp < end_cp; cp += 4)
				{
					clusno = FAT_GET_LONG (cp);

					if   (C_ISFREE (clusno))
						free++;
					elif (C_ISBAD (clusno))
						bad++;

					if (--n <= 0)
						break;
				}
			}

			block_put (bp);

			blkno += (bp->b_count >> BLSHIFT);
		}
		while (n > 0);
	}
	else				 	/* FAT 12 */
	{
		count = BLTOBY (sp->s_sectors_per_fat);

		if ((area_ptr = area = malloc_byte (count)) == NOVOID)
			{ u.u_error = ENOMEM; return; }

		while (count > 0)
		{
			bp = bread (unisp->sb_dev, blkno, 0);

			if ((cnt = bp->b_count) > count)
				cnt = count;

			memmove (area_ptr, bp->b_addr, cnt);

			block_put (bp);

			blkno    += cnt >> BLSHIFT;

			area_ptr += cnt;
			count	 -= cnt;
		}

		for (cp = area + 3; /* abaixo */; cp += 3)
		{
			l = FAT_GET_LONG (cp);

			clusno =  l	   & 0xFFF;

			if   (C_ISFREE (clusno))
				free++;
			elif (C_ISBAD (clusno))
				bad++;

			if (--n <= 0)
				break;

			clusno = (l >> 12) & 0xFFF;

			if   (C_ISFREE (clusno))
				free++;
			elif (C_ISBAD (clusno))
				bad++;

			if (--n <= 0)
				break;
		}

		free_byte (area);
	}

	/*
	 *	Preenche a estrutura
	 */
	up->f_type	 = FS_FAT;
	up->f_sub_type	 = sp->s_fat_bits;
	up->f_bsize	 = sp->s_sectors_per_cluster << BLSHIFT;
	up->f_nmsize	 = 255;

	up->f_fsize	 = sp->s_n_clusters - bad;
	up->f_tfree	 = free;
	up->f_isize	 = 0;
	up->f_tinode	 = 0;
	strcpy (up->f_fname, unisp->sb_fname);
	strcpy (up->f_fpack, unisp->sb_fpack);
	up->f_m		 = 1;
	up->f_n		 = 1;
	up->f_symlink_sz = 0;

}	/* end fat_get_ustat */

/*
 ****************************************************************
 *	Atualiza o SB da FAT32					*
 ****************************************************************
 */
void
fat_update (const SB *unisp)
{
	FATSB		*sp = unisp->sb_ptr;
	FATSB1		*fsinfo;
	BHEAD		*bp;

	if (sp->s_fat_bits != 32)
		return;

	if (sp->s_fs_info == 0)
		return;

	if (sp->s_disk_infree == sp->s_infree)
		return;

	sp->s_disk_infree = sp->s_infree;

	bp = bread (unisp->sb_dev, sp->s_fs_info, 0);

	if (u.u_error)
		return;

	fsinfo = bp->b_addr;

	FAT_PUT_LONG (sp->s_infree, fsinfo->fs_infree);

	bwrite (bp);

}	/* end fat_update */
