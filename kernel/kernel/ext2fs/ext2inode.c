/*
 ****************************************************************
 *								*
 *			ext2inode.c				*
 *								*
 *	Alocação e desalocação de INODEs no disco		*
 *								*
 *	Versão	4.4.0, de 23.10.02				*
 *		4.7.0, de 11.12.04				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2004 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include "../h/common.h"
#include "../h/sync.h"
#include "../h/scb.h"
#include "../h/region.h"

#include "../h/inode.h"
#include "../h/bhead.h"
#include "../h/mntent.h"
#include "../h/sb.h"
#include "../h/ext2.h"
#include "../h/sysdirent.h"
#include "../h/uerror.h"
#include "../h/signal.h"
#include "../h/uproc.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ******	Protótipos de funções ***********************************
 */
INODE		*ext2_alloc_inode (SB *sp);
ino_t		ext2_alloc_inode_in_group (SB *sp, int group_index);

/*
 ****************************************************************
 *	Lê e converte um INODE EXT2FS do disco para a memória	*
 ****************************************************************
 */
int
ext2_read_inode (INODE *ip)
{
	const EXT2SB	*ext2sp = ip->i_sb->sb_ptr;
	const EXT2BG	*bgp;
	BHEAD		*bp;
	daddr_t		bno;
	const EXT2DINO	*dp;
	int		group_index, ino_group_index, ino_block_index;
	ulong		ifmt;

	/*
	 *	Le o bloco correspondente
	 */
	group_index	= (ip->i_ino - 1) / ext2sp->s_inodes_per_group;
	ino_group_index = (ip->i_ino - 1) % ext2sp->s_inodes_per_group;

	if (group_index >= ext2sp->s_group_cnt)
		{ u.u_error = ENOTFS; return (-1); }

	bgp = &ext2sp->s_bg[group_index];

	bno = bgp->bg_inode_table + (ino_group_index >> ext2sp->s_INOperBLOCK_SHIFT);
	ino_block_index = ino_group_index & ext2sp->s_INOperBLOCK_MASK;

#ifdef	DEBUG
	printf
	(	"ino = %d, group_index = %d, ino_group_index = %d, inode_table = %d, bno = %d, ino_block_index = %d\n",
		ip->i_ino, group_index, ino_group_index, bgp->bg_inode_table, bno, ino_block_index
	);
#endif	DEBUG

	/*
	 *	Lê o bloco do INODE
	 */
	bp = bread (ip->i_dev, EXT2_BLOCKtoBL (bno), 0);

	if (bp->b_flags & B_ERROR)
	{
		printf ("%g: Erro na leitura do INODE (%v, %d)\n", ip->i_dev, ip->i_ino);
		block_put (bp); return (-1);
	}

	dp = (EXT2DINO *)bp->b_addr + ino_block_index;

	/*
	 *	Copia diversos campos em comum
	 */
	ip->i_mode	= dp->n_mode;
	ip->i_uid	= dp->n_uid;
	ip->i_gid	= dp->n_gid;
	ip->i_nlink	= dp->n_nlink;
	ip->i_blocks	= dp->n_blocks;
	ip->i_size	= dp->n_size;
   /***	ip->i_high_size	= dp->n_high_size; ***/

	ip->i_atime	= dp->n_atime;
	ip->i_mtime	= dp->n_mtime;
	ip->i_ctime	= dp->n_ctime;

	memmove (ip->i_addr, dp->n_addr, sizeof (dp->n_addr));

	/*
	 *	Obtém o código do sistema de arquivos
	 */
	if   ((ifmt = ip->i_mode & IFMT) == IFCHR)
	{
		ip->i_fsp  = &fstab[FS_CHR];
		ip->i_rdev = (dev_t)ip->i_addr[0];
	}
	elif (ifmt == IFBLK)
	{
		ip->i_fsp  = &fstab[FS_BLK];
		ip->i_rdev = (dev_t)ip->i_addr[0];
	}
	else
	{
		ip->i_fsp  = &fstab[FS_EXT2];
		ip->i_rdev = 0;
	}

	block_put (bp);

	return (0);

}	/* end ext2_read_inode */

/*
 ****************************************************************
 *	Converte o formato do INODE: memória para disco		*
 ****************************************************************
 */
void
ext2_write_inode (INODE *ip)
{
	const EXT2SB	*ext2sp = ip->i_sb->sb_ptr;
	const EXT2BG	*bgp;
	BHEAD		*bp;
	int		group_index, ino_group_index, ino_block_index;
	daddr_t		bno;
	EXT2DINO	*dp;
	ulong		ifmt;

	/*
	 *	Verifica se não houve modificação no INODE
	 *	ou entao ele está montado "Read only"
	 */
	if ((ip->i_flags & ICHG) == 0)
		return;

	inode_dirty_dec (ip);

	if (ip->i_sb->sb_mntent.me_flags & SB_RONLY)
		return;

	/*
	 *	Obtém o bloco correspondente
	 */
	group_index	= (ip->i_ino - 1) / ext2sp->s_inodes_per_group;
	ino_group_index = (ip->i_ino - 1) % ext2sp->s_inodes_per_group;

	if (group_index >= ext2sp->s_group_cnt)
		{ u.u_error = ENOTFS; return; }

	bgp = &ext2sp->s_bg[group_index];

	bno = bgp->bg_inode_table + (ino_group_index >> ext2sp->s_INOperBLOCK_SHIFT);
	ino_block_index = ino_group_index & ext2sp->s_INOperBLOCK_MASK;

	/*
	 *	Lê o bloco do INODE
	 */
	bp = bread (ip->i_dev, EXT2_BLOCKtoBL (bno), 0);

	if (bp->b_flags & B_ERROR)
	{
		printf ("%g: Erro na leitura do INODE (%v, %d)\n", ip->i_dev, ip->i_ino);
		block_put (bp); return;
	}

	/*
	 *	Copia diversos campos em comum
	 */
	dp = (EXT2DINO *)bp->b_addr + ino_block_index;

	memclr (dp, sizeof (EXT2DINO));

	dp->n_mode	= ip->i_mode;
	dp->n_uid	= ip->i_uid;
	dp->n_gid	= ip->i_gid;
	dp->n_nlink	= ip->i_nlink;
	dp->n_blocks	= ip->i_blocks;
	dp->n_size	= ip->i_size;
   /***	dp->n_high_size	= ip->i_high_size; ***/

	dp->n_atime	= ip->i_atime;
	dp->n_mtime	= ip->i_mtime;
	dp->n_ctime	= ip->i_ctime;

	if ((ifmt = ip->i_mode & IFMT) == IFBLK || ifmt == IFCHR)
		ip->i_addr[0] = (daddr_t)ip->i_rdev;

	memmove (dp->n_addr, ip->i_addr, sizeof (dp->n_addr));

	bdwrite (bp);

#ifdef	MSG
	if (CSWT (6))
		printf ("%g: Atualizando o INODE (%v, %d)\n", ip->i_dev, ip->i_ino);
#endif	MSG

}	/* end ext2_write_inode */

/*
 ****************************************************************
 *	Cria um arquivo novo					*
 ****************************************************************
 */
INODE *
ext2_make_inode (IOREQ *iop, DIRENT *dep, int mode)
{
	INODE		*ip;
	int		ifmt;

	/*
	 *	Tenta obter um INODE no Dispositivo do pai
	 */
	if ((ip = ext2_alloc_inode (iop->io_ip->i_sb)) == NOINODE)
		{ iput (iop->io_ip); return (NOINODE); }

	/*
	 *	Prepara o INODE
	 */
	if ((mode & IFMT) == 0)
		mode |= IFREG;

	ip->i_mode	= mode & ~u.u_cmask;
	ip->i_nlink	= 1;
	ip->i_uid	= u.u_euid;
	ip->i_gid	= u.u_egid;

	ip->i_rdev	= 0;
   /***	ip->i_blocks	= 0;	Veja o "memclr", abaixo ***/
	ip->i_size	= 0;

	memclr (ip->i_addr, sizeof (ip->i_addr));

	ip->i_atime	= time;
	ip->i_mtime	= time;
	ip->i_ctime	= time;

	inode_dirty_inc (ip);

	/*
	 *	Obtém o código do sistema de arquivos
	 */
	if   ((ifmt = mode & IFMT) == IFCHR)
		ip->i_fsp  = &fstab[FS_CHR];
	elif (ifmt == IFBLK)
		ip->i_fsp  = &fstab[FS_BLK];
	else
		ip->i_fsp  = &fstab[FS_EXT2];

	/*
	 *	Escreve o nome do novo arquivo no Diretorio pai
	 */
	dep->d_ino = ip->i_ino;

	ext2_write_dir (iop, dep, ip, 0 /* NO rename */);

	return (ip);

}	/* end ext2_make_inode */

/*
 ****************************************************************
 *	Libera todos os blocos de um arquivo			*
 ****************************************************************
 */
void
ext2_trunc_inode (INODE *ip)
{
	int		index;
	daddr_t		bno;

	/*
	 *	Examina o tipo do arquivo
	 */
	switch (ip->i_mode & IFMT)
	{
	    case IFLNK:
		if (ip->i_size <= EXT2_NADDR * sizeof (daddr_t))
		{
			memclr (ip->i_addr, sizeof (ip->i_addr));
			break;
		}
	    case IFREG:
	    case IFDIR:
		for (index = EXT2_NADDR - 1; index >= 0; index--)
		{
			/* Percorre os diversos niveis de indireção */

			if ((bno = ip->i_addr[index]) == NODADDR)
				continue;
	
			ip->i_addr[index] = NODADDR;
	
			/*
			 *	Verifica qual o nivel de indireção
			 */
			if (index < EXT2_NADDR - 3)
				ext2_block_release (ip->i_sb, bno);
			else
				ext2_indir_block_release (ip->i_sb, bno, index - (EXT2_NADDR - 4));
		}

		break;

	    default:
		return;

	}	/* end switch (mode) */

	/*
	 *	Epilogo
	 */
	ip->i_blocks = 0;
	ip->i_size   = 0;

	ip->i_mtime = time;
	inode_dirty_inc (ip);

}	/* end ext2_trunc_inode */

/*
 ****************************************************************
 *	Libera um INODE do Disco				*
 ****************************************************************
 */
void
ext2_free_inode (const INODE *ip)
{
	SB		*sp = ip->i_sb;
	EXT2SB		*ext2sp = sp->sb_ptr;
	EXT2BG		*bgp;
	BHEAD		*map_bp;
	int		group_index, ino_group_index;

	/*
	 *	Repare que NÃO está zerando o INODE no disco!
	 *
	 *	Obtém a localização do bit correspondente
	 */
	group_index	= (ip->i_ino - 1) / ext2sp->s_inodes_per_group;
	ino_group_index = (ip->i_ino - 1) % ext2sp->s_inodes_per_group;

	if (group_index >= ext2sp->s_group_cnt)
		{ u.u_error = ENOTFS; return; }

	bgp = &ext2sp->s_bg[group_index];

	/*
	 *	Obtém o mapa de bits
	 */
	map_bp = bread (sp->sb_dev, EXT2_BLOCKtoBL (bgp->bg_inode_bitmap), 0);

	if (map_bp->b_flags & B_ERROR)
		{ block_put (map_bp); return; }

	/*
	 *	Desaloca o INODE
	 */
	EXT2_CLR_MAP (map_bp->b_addr, ino_group_index);

	bdwrite (map_bp);

	SPINLOCK (&sp->sb_lock);

	bgp->bg_free_inodes_count++;

	ext2sp->s_free_inodes_count++; sp->sb_sbmod = 1;

	SPINFREE (&sp->sb_lock);

}	/* end ext2_free_inode */

/*
 ****************************************************************
 *	Aloca um INODE no disco (e na memória)			*
 ****************************************************************
 */
INODE *
ext2_alloc_inode (SB *sp)
{
	EXT2SB		*ext2sp = sp->sb_ptr;
	const EXT2BG	*bgp, *end_bgp;
	INODE		*ip;
	ino_t		ino;
	int		group_index = 0;

	/*
	 *	Procura nos vários grupos
	 */
	for (bgp = ext2sp->s_bg, end_bgp = bgp + ext2sp->s_group_cnt; /* abaixo */; bgp++, group_index++)
	{
		if (bgp >= end_bgp)
		{
			printf ("%g: O sistema de arquivos \"%v\" não tem mais INODEs livres\n", sp->sb_dev);

			u.u_error = ENOSPC; return (NOINODE);
		}

		if (bgp->bg_free_blocks_count == 0 || bgp->bg_free_inodes_count == 0)
			continue;

		if ((ino = ext2_alloc_inode_in_group (sp, group_index)) != NOINO)
			break;
	}

	/*
	 *	Achou um INODE livre
	 */
	if ((ip = iget (sp->sb_dev, ino, 0)) == NOINODE)
		{ printf ("%g: Não obtive o INODE (%v, %d)\n", sp->sb_dev, ino); return (NOINODE); }

	return (ip);

}	/* end ext2_alloc_inode */

/*
 ****************************************************************
 *	Procura um bloco vago para INODEs em um mapa de bits 	*
 ****************************************************************
 */
ino_t
ext2_alloc_inode_in_group (SB *sp, int group_index)
{
	BHEAD		*map_bp;
	EXT2SB		*ext2sp = sp->sb_ptr;
	EXT2BG		*bgp;
	int		offset, index;
	char		*cp;
	const char	*end_cp;
	ino_t		ino;

	/*
	 *	Obtém as informações do grupo
	 */
	bgp = &ext2sp->s_bg[group_index];

	/*
	 *	Obtém o mapa de bits dado
	 */
	map_bp = bread (sp->sb_dev, EXT2_BLOCKtoBL (bgp->bg_inode_bitmap), 0);

	if (map_bp->b_flags & B_ERROR)
		{ block_put (map_bp); return (NOINO); }

	/*
	 *	Procura um INODE livre
	 */
	cp = map_bp->b_addr; end_cp = cp + ext2sp->s_inodes_per_group / 8;

	for (/* acima */; /* abaixo */; cp++)
	{
		if (cp >= end_cp)
			{ block_put (map_bp); return (NOINO); }

		if (*cp != 0xFF)
			break;
	}

	offset = cp - (char *)map_bp->b_addr; index = ffbs (~*cp);

	/*
	 *	Aloca o INODE
	 */
	*cp |= (1 << index);		/* Alocamos este INODE */

	bdwrite (map_bp);

	SPINLOCK (&sp->sb_lock);

	bgp->bg_free_inodes_count--;

	ext2sp->s_free_inodes_count--; sp->sb_sbmod = 1;

	SPINFREE (&sp->sb_lock);

	/*
	 *	Calcula o número do INODE
	 */
	ino = group_index * ext2sp->s_inodes_per_group + offset * 8 + index + 1;

#ifdef	DEBUG
	printf ("group_index = %d, offset = %d, index = %d, ino = %d\n", group_index, offset, index, ino);
#endif	DEBUG

	return (ino);

}	/* end ext2_alloc_inode_in_group */
