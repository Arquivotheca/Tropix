/*
 ****************************************************************
 *								*
 *			ext2dir.c				*
 *								*
 *	Tratamento de diretórios do EXT2FS			*
 *								*
 *	Versão	4.4.0, de 23.10.02				*
 *		4.8.0, de 02.11.05				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 * 		Copyright © 2005 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include "../h/common.h"
#include "../h/sync.h"
#include "../h/scb.h"
#include "../h/region.h"

#include "../h/kfile.h"
#include "../h/mntent.h"
#include "../h/sb.h"
#include "../h/ext2.h"
#include "../h/sysdirent.h"
#include "../h/inode.h"
#include "../h/stat.h"
#include "../h/bhead.h"
#include "../h/uerror.h"
#include "../h/signal.h"
#include "../h/uproc.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ******	Vetor de conversão do tipo do arquivo no diretório ******
 */
enum
{
	EXT2_FT_UNKNOWN,
	EXT2_FT_REG_FILE,
	EXT2_FT_DIR,
	EXT2_FT_CHRDEV,
	EXT2_FT_BLKDEV,
	EXT2_FT_FIFO,
	EXT2_FT_SOCK,
	EXT2_FT_SYMLINK,
	EXT2_FT_MAX
};

const entry char	ext2_mode_conv[] =
{
	EXT2_FT_UNKNOWN,	/*  0 */
	EXT2_FT_FIFO,		/*  1 */
	EXT2_FT_CHRDEV,		/*  2 */
	EXT2_FT_UNKNOWN,	/*  3 */
	EXT2_FT_DIR,		/*  4 */
	EXT2_FT_UNKNOWN,	/*  5 */
	EXT2_FT_BLKDEV,		/*  6 */
	EXT2_FT_UNKNOWN,	/*  7 */
	EXT2_FT_REG_FILE,	/*  8 */
	EXT2_FT_UNKNOWN,	/*  9 */
	EXT2_FT_SYMLINK,	/* 10 */
	EXT2_FT_UNKNOWN,	/* 11 */
	EXT2_FT_UNKNOWN,	/* 12 */
	EXT2_FT_UNKNOWN,	/* 13 */
	EXT2_FT_UNKNOWN,	/* 14 */
	EXT2_FT_UNKNOWN		/* 15 */
};

/*
 ****************************************************************
 *	Procura um nome dado no diretório			*
 ****************************************************************
 */
int
ext2_search_dir (IOREQ *iop, DIRENT *dep, int mission)
{
	INODE		*ip = iop->io_ip;
	EXT2SB		*ext2sp = ip->i_sb->sb_ptr;
	BHEAD		*bp = NOBHEAD;
	const EXT2DIR	*dp, *end_dp;
	dev_t		dev = ip->i_dev;
	daddr_t		lbn = 0, pbn, rabn;
	off_t		offset;

	/*
	 *	Recebe e devolve o "ip" trancado
	 *
	 *	Se encontrou a entrada, devolve:
	 *		"dep->d_ino"			     o INO  (caso normal)
	 *		"dep->d_namlen" e "dep->d_name"	     o nome (caso INUMBER)
	 *		"iop->io_offset_low" o início da entrada
	 *
	 *	Se NÃO encontrou a entrada, devolve:
	 *		NADA
	 */

	if (ip->i_sb->sb_mntent.me_flags & SB_ATIME)
		{ ip->i_atime = time; inode_dirty_inc (ip); }

	/*
	 *	Verifica se está procurando por "." ou ".."
	 */
	if ((mission & INUMBER) == 0 && dep->d_name[0] == '.')
	{
		if   (dep->d_name[1] == '\0')
		{
			dep->d_ino = ip->i_ino; return (1);
		}
		elif (dep->d_name[1] == '.' && dep->d_name[2] == '\0')
		{
			if (ip->i_par_ino != NOINO)
				{ dep->d_ino = ip->i_par_ino; return (1); }
		}
	}

	/*
	 *	Malha principal
	 */
	for
	(	offset = 0, dp = end_dp = NOEXT2DIR;
		/* abaixo */;
		offset += dp->d_ent_sz, dp = EXT2DIR_SZ_PTR (dp)
	)
	{
		if (dp >= end_dp)
		{
			/* Lê um bloco de diretório */

			if (bp != NOBHEAD)
				{ block_put (bp); bp = NOBHEAD; }

			if (offset >= ip->i_size)
				return (0);

			pbn = ext2_block_map (ip, lbn, B_READ, &rabn);

			if (u.u_error)
				return (-1);

			if   (ip->i_lastr + 1 == lbn)
				bp = breada (dev, EXT2_BLOCKtoBL (pbn), EXT2_BLOCKtoBL (rabn), 0);
			else
				bp = bread (dev, EXT2_BLOCKtoBL (pbn), 0);

			ip->i_lastr = lbn;

			dp = bp->b_addr; end_dp = bp->b_addr + ext2sp->s_BLOCK_SZ;

			lbn++;
		}

		/*
		 *	Usa as entradas deste bloco
		 */
		if (dp->d_ino == NOINO)
			continue;

		if (mission & INUMBER)		/* Busca dado o INO */
		{
			if (dp->d_ino == dep->d_ino)
			{
				dep->d_namlen = dp->d_nm_len; strcpy (dep->d_name, dp->d_name);

				iop->io_offset_low = offset; block_put (bp); return (1);
			}
		}
		else				/* Busca dado o nome */
		{
			if (dp->d_nm_len == dep->d_namlen && memeq (dp->d_name, dep->d_name, dp->d_nm_len))
			{
				dep->d_ino = dp->d_ino;

				iop->io_offset_low = offset; block_put (bp); return (1);
			}
		}

	}	/* end for (EVER) */

}	/* end ext2_search_dir */

/*
 ****************************************************************
 *	Fornece entradas do diretório para o usuário 		*
 ****************************************************************
 */
void
ext2_read_dir (IOREQ *iop)
{
	INODE		*ip = iop->io_ip;
	EXT2SB		*ext2sp = ip->i_sb->sb_ptr;
	BHEAD		*bp = NOBHEAD;
	const EXT2DIR	*dp, *end_dp;
	daddr_t		lbn, pbn, rabn;
	off_t		offset;
	int		count, range;
	DIRENT		dirent;

	/*
	 *	Malha principal
	 */
	for
	(	dp = end_dp = NOEXT2DIR;
		iop->io_count >= sizeof (DIRENT);
		iop->io_offset_low += dp->d_ent_sz, dp = EXT2DIR_SZ_PTR (dp)
	)
	{
		if (dp >= end_dp)
		{
			/* Lê um bloco de diretório */

			if (bp != NOBHEAD)
				{ block_put (bp); bp = NOBHEAD; }

			if ((range = ip->i_size - iop->io_offset_low) <= 0)
				{ iop->io_eof = 1; break; } 

			lbn    = iop->io_offset_low >> ext2sp->s_BLOCK_SHIFT;
			offset = iop->io_offset_low &  ext2sp->s_BLOCK_MASK;

			pbn = ext2_block_map (ip, lbn, B_READ, &rabn);

			if (u.u_error)
				break;

			if   (ip->i_lastr + 1 == lbn)
				bp = breada (ip->i_dev, EXT2_BLOCKtoBL (pbn), EXT2_BLOCKtoBL (rabn), 0);
			else
				bp = bread (ip->i_dev, EXT2_BLOCKtoBL (pbn), 0);

			ip->i_lastr = lbn;

			if ((count = ext2sp->s_BLOCK_SZ - offset) > range)
				count = range;

			dp = bp->b_addr + offset; end_dp = bp->b_addr + offset + count;
		}

		/*
		 *	Usa as entradas deste bloco
		 */
		if (dp->d_ino == NOINO)
			continue;

		dirent.d_offset	= iop->io_offset_low;
		dirent.d_ino	= dp->d_ino;
		dirent.d_namlen = dp->d_nm_len;
		memmove (dirent.d_name, dp->d_name, dirent.d_namlen);
		dirent.d_name[dirent.d_namlen] = '\0';

		if (unimove (iop->io_area, &dirent, sizeof (DIRENT), iop->io_cpflag) < 0)
			{ u.u_error = EFAULT; break; }

		/* Avança os ponteiros */

		iop->io_area  += sizeof (DIRENT);
		iop->io_count -= sizeof (DIRENT);

	}	/* end for () */

	/*
	 *	Postludium
	 */
	if (bp != NOBHEAD)
		block_put (bp);

}	/* end ext2_read_dir */

/*
 ****************************************************************
 *	Escreve a entrada no diretório				*
 ****************************************************************
 */
void
ext2_write_dir (IOREQ *iop, const DIRENT *dep, const INODE *son_ip, int rename)
{
	INODE		*ip = iop->io_ip;
	SB		*sp = ip->i_sb;
	EXT2SB		*ext2sp = sp->sb_ptr;
	int		mode = son_ip->i_mode;
	BHEAD		*bp = NOBHEAD;
	EXT2DIR		*dp, *end_dp;
	daddr_t		lbn, pbn, rabn;
	ino_t		par_ino, old_ino;
	off_t		offset;
	int		size, avail_size, group_index;

	/*
	 *	Calcula o tamanho necessário
	 */
	size = EXT2DIR_SIZEOF (dep->d_namlen);

	/*
	 *	Malha principal
	 */
	for
	(	offset = 0, dp = end_dp = NOEXT2DIR;
		/* abaixo */;
		offset += dp->d_ent_sz, dp = EXT2DIR_SZ_PTR (dp)
	)
	{
		if (dp >= end_dp)
		{
			/* Lê um bloco de diretório */

			if (bp != NOBHEAD)
				{ block_put (bp); bp = NOBHEAD; }

			if (offset >= ip->i_size)
				break;

			lbn = offset >> ext2sp->s_BLOCK_SHIFT;

			pbn = ext2_block_map (ip, lbn, B_READ, &rabn);

			if (u.u_error)
				{ iput (ip); return; }

			if   (ip->i_lastr + 1 == lbn)
				bp = breada (ip->i_dev, EXT2_BLOCKtoBL (pbn), EXT2_BLOCKtoBL (rabn), 0);
			else
				bp = bread (ip->i_dev, EXT2_BLOCKtoBL (pbn), 0);

			ip->i_lastr = lbn;

			dp = bp->b_addr; end_dp = bp->b_addr + ext2sp->s_BLOCK_SZ;
		}

		/*
		 *	Verifica se esta entrada tem espaço
		 */
		avail_size = dp->d_ent_sz;

		if (dp->d_ino != NOINO)
			avail_size -= EXT2DIR_SIZEOF (dp->d_nm_len);

		if (avail_size < size)
			continue;

		/*
		 *	Prepara a entrada
		 */
		if (dp->d_ino != 0)
		{
			dp->d_ent_sz = EXT2DIR_SIZEOF (dp->d_nm_len);

			dp = EXT2DIR_SZ_PTR (dp);

			dp->d_ent_sz = avail_size;
		}

		dp->d_ino	= dep->d_ino;
	   /***	dp->d_ent_sz	= ...;		/* Acima ***/
		dp->d_nm_len	= dep->d_namlen;
		dp->d_type	= ext2_mode_conv[mode >> 12];
		memmove (dp->d_name, dep->d_name, dp->d_nm_len);

		bdwrite (bp);

		ip->i_mtime = time; inode_dirty_inc (ip);

		goto incr_dirs_count;

	}	/* end for () */

	/*
	 *	É necessário adicionar um bloco novo
	 */
	dp = alloca (size);

	dp->d_ino	= dep->d_ino;
	dp->d_ent_sz	= ext2sp->s_BLOCK_SZ;
	dp->d_nm_len	= dep->d_namlen;
	dp->d_type	= ext2_mode_conv[mode >> 12];
	memmove (dp->d_name, dep->d_name, dp->d_nm_len);

   /***	iop->io_fd	    = ...;	***/
   /***	iop->io_fp	    = ...;	***/
   /***	iop->io_ip	    = ip;	***/
   /***	iop->io_dev	    = ...;	***/
	iop->io_area	    = dp;
	iop->io_count	    = size;
	iop->io_offset_low  = offset;
	iop->io_cpflag	    = SS;
   /***	iop->io_rablock	    = ...;	***/

	ext2_write (iop);

	/*
	 *	Não esquece de arredondar o tamanho do diretório
	 */
	ip->i_size = (ip->i_size + ext2sp->s_BLOCK_MASK) & ~ext2sp->s_BLOCK_MASK;

	/*
	 ******	Incrementa o uso de diretórios no grupo *****************
	 */
    incr_dirs_count:
	if (S_ISDIR (mode))
	{
		group_index = (dep->d_ino - 1) / ext2sp->s_inodes_per_group;

		SPINLOCK (&sp->sb_lock);

		ext2sp->s_bg[group_index].bg_used_dirs_count++;
		sp->sb_sbmod = 1;

		SPINFREE (&sp->sb_lock);
	}

	/*
	 ******	Se for um diretório, ajusta a entrada ".." *********
	 */
	if (!rename || !S_ISDIR (son_ip->i_mode))
		{ iput (ip); return; }

	ip->i_nlink++; par_ino = ip->i_ino; /*** inode_dirty_inc (ip); ***/ iput (ip);

	bp = bread (son_ip->i_dev, EXT2_BLOCKtoBL (son_ip->i_addr[0]), 0);

	if (u.u_error != NOERR)
		{ block_put (bp); return; }

	dp = bp->b_addr + EXT2DIR_SIZEOF (1);   /* Deslocamento do ".." */

	old_ino = dp->d_ino; dp->d_ino = par_ino;

	bdwrite (bp);

	if ((ip = iget (son_ip->i_dev, old_ino, 0)) == NOINODE)
		{ printf ("%g: NÃO consegui obter o INODE do pai\n"); return; }

	ip->i_nlink--; inode_dirty_inc (ip); iput (ip);

}	/* end ext2_write_dir */

/*
 ****************************************************************
 *	Certifica-se de que um diretório está vazio		*
 ****************************************************************
 */
int
ext2_empty_dir (INODE *ip)
{
	EXT2SB		*ext2sp = ip->i_sb->sb_ptr;
	BHEAD		*bp = NOBHEAD;
	const EXT2DIR	*dp, *end_dp;
	dev_t		dev = ip->i_dev;
	daddr_t		lbn = 0, pbn, rabn;
	off_t		offset;

	/*
	 *	Recebe e devolve o "ip" trancado
	 *
	 *	Se estiver vazio, devolve 1
	 *
	 */

	/*
	 *	Malha principal
	 */
	for
	(	offset = 0, dp = end_dp = NOEXT2DIR;
		/* abaixo */;
		offset += dp->d_ent_sz, dp = EXT2DIR_SZ_PTR (dp)
	)
	{
		if (dp >= end_dp)
		{
			/* Lê um bloco de diretório */

			if (bp != NOBHEAD)
				{ block_put (bp); bp = NOBHEAD; }

			if (offset >= ip->i_size)
				return (1);

			pbn = ext2_block_map (ip, lbn, B_READ, &rabn);

			if (u.u_error)
				return (-1);

			if   (ip->i_lastr + 1 == lbn)
				bp = breada (dev, EXT2_BLOCKtoBL (pbn), EXT2_BLOCKtoBL (rabn), 0);
			else
				bp = bread (dev, EXT2_BLOCKtoBL (pbn), 0);

			ip->i_lastr = lbn;

			dp = bp->b_addr; end_dp = bp->b_addr + ext2sp->s_BLOCK_SZ;

			lbn++;
		}

		/*
		 *	Usa as entradas deste bloco
		 */
		if (offset < 2 * EXT2DIR_SIZEOF (1))		/* Pula "." e ".." */
			continue;

		if (dp->d_ino == NOINO)				/* Entrada vazia */
			continue;

		/* Achou algo */

		block_put (bp);

		return (0);

	}	/* end for (EVER) */

}	/* end ext2_empty_dir */

/*
 ****************************************************************
 *	Cria um diretório da EXT2FS				*
 ****************************************************************
 */
INODE *
ext2_make_dir (IOREQ *iop, DIRENT *dep, int mode)
{
	SB		*sp;
	EXT2SB		*ext2sp;
	INODE		*pp, *ip;
	dev_t		par_dev;
	ino_t		par_ino;
	EXT2DIR		d[2];

	pp = iop->io_ip; sp = pp->i_sb; ext2sp = sp->sb_ptr;

	par_dev = pp->i_dev; par_ino = pp->i_ino;

	pp->i_nlink++; inode_dirty_inc (pp); 	/* Já levando em conta o ".." */

	if ((ip = ext2_make_inode (iop, dep, mode)) == NOINODE)
		goto bad;

	/*
	 *	Prepara o "." e o ".."
	 */
	memclr (d, sizeof (d));

	d[0].d_ino	= dep->d_ino;
	d[0].d_ent_sz	= EXT2DIR_SIZEOF (1);
	d[0].d_nm_len	= 1;
	d[0].d_type	= EXT2_FT_DIR;
	d[0].d_name[0]	= '.';

	d[1].d_ino	= par_ino;
	d[1].d_ent_sz	= ext2sp->s_BLOCK_SZ - EXT2DIR_SIZEOF (1);
	d[1].d_nm_len	= 2;
	d[1].d_type	= EXT2_FT_DIR;
	d[1].d_name[0]	= '.';
	d[1].d_name[1]	= '.';

   /***	iop->io_fd	    = ...;	***/
   /***	iop->io_fp	    = ...;	***/
	iop->io_ip	    = ip;
   /***	iop->io_dev	    = ...;	***/
	iop->io_area	    = d;
	iop->io_count	    = sizeof (d);
	iop->io_offset_low  = 0;
	iop->io_cpflag	    = SS;
   /***	iop->io_rablock	    = ...;	***/

	ext2_write (iop);

	ip->i_nlink = 2;
	ip->i_size  = ext2sp->s_BLOCK_SZ;

	if (u.u_error == NOERR)
		return (ip);

	/*
	 *	Se houve erro, ...
	 */
	iput (ip);

    bad:
	if ((pp = iget (par_dev, par_ino, 0)) != NOINODE)
		{ pp->i_nlink--; iput (pp); }

	return (NOINODE);

}	/* end ext2_make_dir */

/*
 ****************************************************************
 *	Apaga uma entrada de diretório				*
 ****************************************************************
 */
void
ext2_clr_dir (IOREQ *iop, const DIRENT *dep, const INODE *son_ip)
{
	INODE		*ip = iop->io_ip;
	SB		*sp = ip->i_sb;
	EXT2SB		*ext2sp = sp->sb_ptr;
	dev_t		dev = ip->i_dev;
	daddr_t		lbn, pbn, rabn;
	BHEAD		*bp;
	EXT2DIR		*adp, *dp, *next_dp, *end_dp, *clr_dp;
	int		group_index;
	char		type;

	/*
	 *	Lê o bloco relevante do diretório
	 */
	lbn = iop->io_offset_low >> ext2sp->s_BLOCK_SHIFT;

	if ((pbn = ext2_block_map (ip, lbn, B_READ, &rabn)) <= NODADDR)
		{ printf ("%g: O bloco lógico %d do diretório não existe\n", lbn); return; }

	bp = bread (dev, EXT2_BLOCKtoBL (pbn), 0);

	/*
	 *	Procura a entrada anterior
	 */
	dp = bp->b_addr; end_dp = bp->b_addr + ext2sp->s_BLOCK_SZ;
	
	clr_dp = (EXT2DIR *)((int)dp + (iop->io_offset_low & ext2sp->s_BLOCK_MASK));

	for (adp = NOEXT2DIR; /* abaixo */; adp = dp, dp = next_dp)
	{
		if (dp >= end_dp)
			{ printf ("%g: Não encontrei a entrada desejada\n"); block_put (bp); return; }

		next_dp = EXT2DIR_SZ_PTR (dp);

		if (dp == clr_dp)
			break;
	}

	/*
	 *	Encontrou a entrada
	 */
	group_index = (dp->d_ino - 1) / ext2sp->s_inodes_per_group;
	type	    = dp->d_type;

	if (adp != NOEXT2DIR)		/* Tem entrada anterior */
	{
		adp->d_ent_sz += dp->d_ent_sz;
		memclr (dp, dp->d_ent_sz);
	}
	else				/* Era a primeira entrada */
	{
		memclr (dp, dp->d_ent_sz);
		dp->d_ent_sz = (int)next_dp - (int)dp;
	}

	bdwrite (bp);

	/*
	 *	Decrementa o contador de diretórios do grupo
	 */
	if (type == EXT2_FT_DIR)
	{
		SPINLOCK (&sp->sb_lock);

		ext2sp->s_bg[group_index].bg_used_dirs_count--;
		sp->sb_sbmod = 1;

		SPINFREE (&sp->sb_lock);
	}

	/*
	 *	Atualiza o INODE
	 */
	ip->i_mtime = time; inode_dirty_inc (ip);

}	/* end ext2_clr_dir */
