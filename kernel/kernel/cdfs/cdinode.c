/*
 ****************************************************************
 *								*
 *			cdinode.c				*
 *								*
 *	Tratamento de INODES no FS ISO9660			*
 *								*
 *	Versão	4.2.0, de 19.12.01				*
 *		4.2.0, de 31.01.02				*
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
#include "../h/inode.h"
#include "../h/bhead.h"
#include "../h/sysdirent.h"
#include "../h/cdfs.h"
#include "../h/stat.h"
#include "../h/uerror.h"
#include "../h/signal.h"
#include "../h/uproc.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Leitura e Conversão do INODE do CDROM			*
 ****************************************************************
 */
int
cd_read_inode (INODE *ip)
{
	const CDDIR	*dp;
	BHEAD		*bp;
	const SB	*sp = ip->i_sb;
	const CDSB	*cdsp = sp->sb_ptr;
	daddr_t		block;

	/*
	 *	Lê o bloco correspondente.
	 */
	bp = bread (ip->i_dev, ip->i_ino >> BLSHIFT, 0);

	if (bp->b_flags & B_ERROR)
		{ block_put (bp); return (-1); }

	dp = (CDDIR *)(bp->b_addr + (ip->i_ino & BLMASK));

	/*
	 *	Processo o modo e "nlink"
	 */
    again:
	switch (dp->d_flags & ISO_IFMT)
	{
	    case ISO_REG:
		ip->i_mode = S_IFREG | S_IEXEC;
		ip->i_nlink = 1;
		break;

	    case ISO_DIR:
		ip->i_mode = S_IFDIR | S_IEXEC | (S_IREAD >> IPSHIFT) | (S_IREAD >> (2 * IPSHIFT));
	   	ip->i_nlink = 0;	/* Será calculado abaixo */
		break;

	    default:
		printf ("%g: Lendo tipo 0x%02X inválido do INODE %d da CDROM\n", dp->d_flags , ip->i_ino);
		block_put (bp);
		return (-1);
	}

	ip->i_mode |= S_IREAD | (S_IREAD >> IPSHIFT) | (S_IREAD >> (2 * IPSHIFT));

	/*
	 *	Retira outros campos
	 */
	ip->i_uid = sp->sb_uid;
	ip->i_gid = sp->sb_gid;

#if (0)	/*******************************************************/
	if (sp->sb_mntent.me_flags & SB_USER)
	{
		ip->i_uid = sp->sb_uid;
		ip->i_gid = sp->sb_gid;
	}
	else
	{
		ip->i_uid = 0;
		ip->i_gid = 0;
	}
#endif	/*******************************************************/

	ip->i_rdev	= NODEV;

	ip->i_size	= ISO_GET_LONG (dp->d_size);

	/*
	 *	Obtém o número do primeiro bloco.
	 */
	ip->i_first_block = ISO_GET_LONG (dp->d_first_block) + dp->d_ext_attr_len;

	/*
	 *	Retira os tempos.
	 */
	ip->i_atime	= 
	ip->i_mtime	= 
	ip->i_ctime	= cd_get_time (dp->d_date);

	/*
	 *	Se for "Rock Ridge", ...
	 */
	if (cdsp->s_rock_ridge)
	{
		if ((block = cd_rr_fields_analysis (dp, ip->i_sb, RR_STAT_MISSION, ip)) != 0)
		{
			/* É um diretório relocado */

			ip->i_first_block = block;

			block_put (bp);

			bp = bread (ip->i_dev, ISOBL_TO_BL (ip->i_first_block), 0);

			if (bp->b_flags & B_ERROR)
				{ block_put (bp); return (-1); }

			dp = (CDDIR *)bp->b_addr;

			goto again;
		}

	}	/* se Rock Ridge */

	/*
	 *	Epílogo
	 */
	ip->i_fsp	= &fstab[FS_CD];

	block_put (bp);

	if (ip->i_nlink == 0)		/* É um diretório e não RR */
		cd_get_nlink_dir (ip);

	return (0);

}	/* end cd_read_inode */

/*
 ****************************************************************
 *	Rotina de "read" de um "link" simbólico 		*
 ****************************************************************
 */
int
cd_read_symlink (IOREQ *iop)
{
	const CDDIR	*dp;
	INODE		*ip = iop->io_ip;
	BHEAD		*bp;
	int		result;

	/*
	 *	Lê o bloco correspondente.
	 */
	bp = bread (ip->i_dev, ip->i_ino >> BLSHIFT, 0);

	if (bp->b_flags & B_ERROR)
		{ block_put (bp); return (-1); }

	dp = (CDDIR *)(bp->b_addr + (ip->i_ino & BLMASK));

	result = cd_rr_fields_analysis (dp, ip->i_sb, RR_SYMLINK_MISSION, iop);

	block_put (bp);

	return (result);

}	/* end cd_read_symlink */

/*
 ****************************************************************
 *	Escrita de um INODE					*
 ****************************************************************
 */
void
cd_write_inode (INODE *ip)
{
	ip->i_flags &= ~ICHG;

}	/* end cd_write_inode */
