/*
 ****************************************************************
 *								*
 *			t1dir.c					*
 *								*
 *	Tratamento de diretórios do T1FS			*
 *								*
 *	Versão	4.3.0, de 29.07.02				*
 *		4.5.0, de 31.08.03				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 * 		Copyright © 2003 NCE/UFRJ - tecle "man licença"	*
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
#include "../h/t1.h"
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
 ****************************************************************
 *	Procura um nome ou INO dado no diretório		*
 ****************************************************************
 */
int
t1_search_dir (IOREQ *iop, DIRENT *dep, int mission)
{
	INODE		*ip = iop->io_ip;
	BHEAD		*bp = NOBHEAD;
	const T1DIR	*dp, *end_dp;
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
	for (offset = 0, dp = end_dp = NOT1DIR; /* abaixo */; offset += dp->d_ent_sz, dp = T1DIR_SZ_PTR (dp))
	{
		if (dp >= end_dp)
		{
			/* Lê um bloco de diretório */

			if (bp != NOBHEAD)
				{ block_put (bp); bp = NOBHEAD; }

			if (offset >= ip->i_size)
				return (0);

			pbn = t1_block_map (ip, lbn, B_READ, &rabn);

			if (u.u_error)
				return (-1);

			if   (ip->i_lastr + 1 == lbn)
				bp = breada (dev, T1_BL4toBL (pbn), T1_BL4toBL (rabn), 0);
			else
				bp = bread (dev, T1_BL4toBL (pbn), 0);

			ip->i_lastr = lbn;

			dp = bp->b_addr; end_dp = (T1DIR *)((int)dp + BL4SZ);

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

}	/* end t1_search_dir */

/*
 ****************************************************************
 *	Fornece entradas do diretório para o usuário 		*
 ****************************************************************
 */
void
t1_read_dir (IOREQ *iop)
{
	INODE		*ip = iop->io_ip;
	BHEAD		*bp = NOBHEAD;
	const T1DIR	*dp, *end_dp;
	daddr_t		lbn, pbn, rabn;
	off_t		offset;
	int		count, range;
	DIRENT		dirent;

	/*
	 *	Malha principal
	 */
	for
	(	dp = end_dp = NOT1DIR;
		iop->io_count >= sizeof (DIRENT);
		iop->io_offset_low += dp->d_ent_sz, dp = T1DIR_SZ_PTR (dp)
	)
	{
		if (dp >= end_dp)
		{
			/* Lê um bloco de diretório */

			if (bp != NOBHEAD)
				{ block_put (bp); bp = NOBHEAD; }

			if ((range = ip->i_size - iop->io_offset_low) <= 0)
				{ iop->io_eof = 1; break; }

			lbn    = iop->io_offset_low >> BL4SHIFT;
			offset = iop->io_offset_low &  BL4MASK;

			pbn = t1_block_map (ip, lbn, B_READ, &rabn);

			if (u.u_error)
				break;

			if   (ip->i_lastr + 1 == lbn)
				bp = breada (ip->i_dev, T1_BL4toBL (pbn), T1_BL4toBL (rabn), 0);
			else
				bp = bread (ip->i_dev, T1_BL4toBL (pbn), 0);

			ip->i_lastr = lbn;

			if ((count = BL4SZ - offset) > range)
				count = range;

			dp = bp->b_addr + offset; end_dp = (T1DIR *)((int)dp + count);
		}

		/*
		 *	Usa as entradas deste bloco
		 */
		if (dp->d_ino == NOINO)
			continue;

		dirent.d_offset	= iop->io_offset_low;
		dirent.d_ino	= dp->d_ino;
		dirent.d_namlen = dp->d_nm_len;
		strcpy (dirent.d_name, dp->d_name);

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

}	/* end t1_read_dir */

/*
 ****************************************************************
 *	Escreve a entrada no diretório				*
 ****************************************************************
 */
void
t1_write_dir (IOREQ *iop, const DIRENT *dep, const INODE *son_ip, int rename)
{
	INODE		*ip = iop->io_ip;
	BHEAD		*bp = NOBHEAD;
	T1DIR		*dp, *end_dp;
	daddr_t		lbn, pbn, rabn;
	off_t		offset;
	ino_t		par_ino, old_ino;
	int		size, avail_size;

	/*
	 *	Calcula o tamanho necessário
	 */
	size = T1DIR_SIZEOF (dep->d_namlen);

	/*
	 *	Malha principal
	 */
	for
	(	offset = 0, dp = end_dp = NOT1DIR;
		/* abaixo */;
		offset += dp->d_ent_sz, dp = T1DIR_SZ_PTR (dp)
	)
	{
		if (dp >= end_dp)
		{
			/* Lê um bloco de diretório */

			if (bp != NOBHEAD)
				{ block_put (bp); bp = NOBHEAD; }

			if (offset >= ip->i_size)
				break;

			lbn = offset >> BL4SHIFT;

			pbn = t1_block_map (ip, lbn, B_READ, &rabn);

			if (u.u_error)
				{ iput (ip); return; }

			if   (ip->i_lastr + 1 == lbn)
				bp = breada (ip->i_dev, T1_BL4toBL (pbn), T1_BL4toBL (rabn), 0);
			else
				bp = bread (ip->i_dev, T1_BL4toBL (pbn), 0);

			ip->i_lastr = lbn;

			dp = bp->b_addr; end_dp = (T1DIR *)((int)dp + BL4SZ);
		}

		/*
		 *	Verifica se esta entrada tem espaço
		 */
		avail_size = dp->d_ent_sz;

		if (dp->d_ino != NOINO)
			avail_size -= T1DIR_SIZEOF (dp->d_nm_len);

		if (avail_size < size)
			continue;

		/*
		 *	Prepara a entrada
		 */
		if (dp->d_ino != 0)
		{
			dp->d_ent_sz = T1DIR_SIZEOF (dp->d_nm_len);

			dp = T1DIR_SZ_PTR (dp);

			dp->d_ent_sz = avail_size;
		}

		dp->d_ino	= dep->d_ino;
	   /***	dp->d_ent_sz	= ...;		/* Acima ***/
		dp->d_nm_len	= dep->d_namlen;
		strcpy (dp->d_name, dep->d_name);

		bdwrite (bp);

		ip->i_mtime = time; inode_dirty_inc (ip);

		goto check_rename;

	}	/* end for () */

	/*
	 *	É necessário adicionar um bloco novo
	 */
	dp = alloca (size);

	dp->d_ino	= dep->d_ino;
	dp->d_ent_sz	= BL4SZ;
	dp->d_nm_len	= dep->d_namlen;
	strcpy (dp->d_name, dep->d_name);

   /***	iop->io_fd	    = ...;	***/
   /***	iop->io_fp	    = ...;	***/
   /***	iop->io_ip	    = ip;	***/
   /***	iop->io_dev	    = ...;	***/
	iop->io_area	    = dp;
	iop->io_count	    = size;
	iop->io_offset_low  = offset;
	iop->io_cpflag	    = SS;
   /***	iop->io_rablock	    = ...;	***/

	t1_write (iop);

	/*
	 *	Não esquece de arredondar o tamanho do diretório
	 */
	ip->i_size = (ip->i_size + BL4MASK) & ~BL4MASK;

	/*
	 ******	Se for um diretório, ajusta a entrada ".." *********
	 */
    check_rename:
	if (!rename || !S_ISDIR (son_ip->i_mode))
		{ iput (ip); return; }

	ip->i_nlink++; par_ino = ip->i_ino; /*** inode_dirty_inc (ip); ***/ iput (ip);

	bp = bread (son_ip->i_dev, T1_BL4toBL (son_ip->i_addr[0]), 0);

	if (u.u_error != NOERR)
		{ block_put (bp); return; }

	dp = bp->b_addr + T1DIR_SIZEOF (1);	/* Deslocamento do ".." */

	old_ino = dp->d_ino; dp->d_ino = par_ino;

	bdwrite (bp);

	if ((ip = iget (son_ip->i_dev, old_ino, 0)) == NOINODE)
		{ printf ("%g: NÃO consegui obter o INODE do pai\n"); return; }

	ip->i_nlink--; inode_dirty_inc (ip); iput (ip);

}	/* end t1_write_dir */

/*
 ****************************************************************
 *	Certifica-se de que um diretório está vazio		*
 ****************************************************************
 */
int
t1_empty_dir (INODE *ip)
{
	BHEAD		*bp = NOBHEAD;
	const T1DIR	*dp, *end_dp;
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
	for (offset = 0, dp = end_dp = NOT1DIR; /* abaixo */; offset += dp->d_ent_sz, dp = T1DIR_SZ_PTR (dp))
	{
		if (dp >= end_dp)
		{
			/* Lê um bloco de diretório */

			if (bp != NOBHEAD)
				{ block_put (bp); bp = NOBHEAD; }

			if (offset >= ip->i_size)
				return (1);

			pbn = t1_block_map (ip, lbn, B_READ, &rabn);

			if (u.u_error)
				return (-1);

			if   (ip->i_lastr + 1 == lbn)
				bp = breada (dev, T1_BL4toBL (pbn), T1_BL4toBL (rabn), 0);
			else
				bp = bread (dev, T1_BL4toBL (pbn), 0);

			ip->i_lastr = lbn;

			dp = bp->b_addr; end_dp = (T1DIR *)((int)dp + BL4SZ);

			lbn++;
		}

		/*
		 *	Usa as entradas deste bloco
		 */
		if (offset < 2 * T1DIR_SIZEOF (1))		/* Pula "." e ".." */
			continue;

		if (dp->d_ino == NOINO)				/* Entrada vazia */
			continue;

		/* Achou algo */

		block_put (bp);

		return (0);

	}	/* end for (EVER) */

}	/* end t1_empty_dir */

/*
 ****************************************************************
 *	Cria um diretório da T1FS				*
 ****************************************************************
 */
INODE *
t1_make_dir (IOREQ *iop, DIRENT *dep, int mode)
{
	INODE		*pp, *ip;
	dev_t		par_dev;
	ino_t		par_ino;
	T1DIR		d[2];

	pp = iop->io_ip;

	par_dev = pp->i_dev; par_ino = pp->i_ino;

	pp->i_nlink++; inode_dirty_inc (pp); 	/* Já levando em conta o ".." */

	if ((ip = t1_make_inode (iop, dep, mode)) == NOINODE)
		goto bad;

	/*
	 *	Prepara o "." e o ".."
	 */
	memclr (d, sizeof (d));

	d[0].d_ino	= dep->d_ino;
	d[0].d_ent_sz	= T1DIR_SIZEOF (1);
	d[0].d_nm_len	= 1;
	d[0].d_name[0]	= '.';

	d[1].d_ino	= par_ino;
	d[1].d_ent_sz	= BL4SZ - T1DIR_SIZEOF (1);
	d[1].d_nm_len	= 2;
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

	t1_write (iop);

	ip->i_nlink = 2;
	ip->i_size  = BL4SZ;

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

}	/* end t1_make_dir */

/*
 ****************************************************************
 *	Apaga uma entrada de diretório				*
 ****************************************************************
 */
void
t1_clr_dir (IOREQ *iop, const DIRENT *dep, const INODE *son_ip)
{
	INODE		*ip = iop->io_ip;
	dev_t		dev = ip->i_dev;
	daddr_t		lbn, pbn, rabn;
	BHEAD		*bp;
	T1DIR		*adp, *dp, *next_dp, *end_dp, *clr_dp;

	/*
	 *	Lê o bloco relevante do diretório
	 */
	lbn = iop->io_offset_low >> BL4SHIFT;

	if ((pbn = t1_block_map (ip, lbn, B_READ, &rabn)) <= NODADDR)
		{ printf ("%g: O bloco lógico %d do diretório não existe\n", lbn); return; }

	bp = bread (dev, T1_BL4toBL (pbn), 0);

	/*
	 *	Procura a entrada anterior
	 */
	dp = bp->b_addr; end_dp = (T1DIR *)((int)dp + BL4SZ);
	
	clr_dp = (T1DIR *)((int)dp + (iop->io_offset_low & BL4MASK));

	for (adp = NOT1DIR; /* abaixo */; adp = dp, dp = next_dp)
	{
		if (dp >= end_dp)
			{ printf ("%g: Não encontrei a entrada desejada\n"); block_put (bp); return; }

		next_dp = T1DIR_SZ_PTR (dp);

		if (dp == clr_dp)
			break;
	}

	/*
	 *	Encontrou a entrada
	 */
	if (adp != NOT1DIR)		/* Tem entrada anterior */
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
	 *	Lembrar de atualizar o INODE
	 */
	ip->i_mtime = time; inode_dirty_inc (ip);

}	/* end t1_clr_dir */
