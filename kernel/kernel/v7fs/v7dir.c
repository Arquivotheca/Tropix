/*
 ****************************************************************
 *								*
 *			v7dir.c					*
 *								*
 *	Tratamento de diretórios do V7FS			*
 *								*
 *	Versão	4.2.0, de 20.12.01				*
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
#include "../h/v7.h"
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
 *	Procura um nome dado no diretório			*
 ****************************************************************
 */
int
v7_search_dir (IOREQ *iop, DIRENT *dep, int mission)
{
	INODE		*ip = iop->io_ip;
	BHEAD		*bp = NOBHEAD;
	const V7DIR	*dp, *end_dp;
	dev_t		dev = ip->i_dev;
	daddr_t		lbn = 0, pbn, rabn;
	off_t		offset;
	int		n, range, len;

	/*
	 *	Recebe e devolve o "ip" trancado
	 *
	 *	Se encontrou a entrada, devolve:
	 *		"dep->d_ino"			     o INO  (caso normal)
	 *		"dep->d_namlen" e "dep->d_name"	     o nome (caso INUMBER)
	 *		"iop->io_offset_low" o início da entrada
	 *
	 *	Se NÃO encontrou a entrada, devolve:
	 *		"iop->io_offset_low" o local recomendado para a nova entrada
	 */

	if (ip->i_sb->sb_mntent.me_flags & SB_ATIME)
		{ ip->i_atime = time; inode_dirty_inc (ip); }

	iop->io_offset_low = ip->i_size;	/* Ainda não achou uma entrada vaga */

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
	for (dp = end_dp = NOV7DIR, offset = 0; /* abaixo */; dp++, offset += sizeof (V7DIR))
	{
		if (dp >= end_dp)
		{
			/* Lê um bloco de diretório */

			if (bp != NOBHEAD)
				{ block_put (bp); bp = NOBHEAD; }

			if ((range = ip->i_size - offset) <= 0)
				return (0);

			pbn = v7_block_map (ip, lbn, B_READ, &rabn);

			if (u.u_error)
				return (-1);

			if (pbn < 0)
			{
				/* BLOCO dentro do arquivo, mas nunca foi escrito */

				bp = block_free_get (BLSZ);
				memclr (bp->b_addr, BLSZ);
				bp->b_residual = 0;
			}
			elif (ip->i_lastr + 1 == lbn)
			{
				bp = breada (dev, pbn, rabn, 0);
				ip->i_lastr = lbn;
			}
			else
			{
				bp = bread (dev, pbn, 0);
				ip->i_lastr = lbn;
			}

			if ((n = BLSZ) > range)
				n = range;

			dp = bp->b_addr; end_dp = (V7DIR *)((int)dp + n);

			lbn++;
		}

		/*
		 *	Usa as entradas deste bloco
		 */
		if (dp->d_ino == NOINO)
		{
			if (iop->io_offset_low == ip->i_size)
				iop->io_offset_low = offset;

			continue;
		}

		if (mission & INUMBER)		/* Busca dado o INO */
		{
			if (ENDIAN_SHORT (dp->d_ino) == dep->d_ino)
			{
				if (dp->d_name[IDSZ-1] == '\0')
					dep->d_namlen = strlen (dp->d_name);
				else
					dep->d_namlen = IDSZ;

				memmove (dep->d_name, dp->d_name, IDSZ); dep->d_name[IDSZ] = '\0';

				iop->io_offset_low = offset; block_put (bp); return (1);
			}
		}
		else				/* Busca dado o nome */
		{
			/* Compara o nome (somente 14 caracteres) */

			if ((len = dep->d_namlen) >= IDSZ)
				len = IDSZ;
			else
				len += 1;

			if (memeq (dp->d_name, dep->d_name, len))
			{
				dep->d_ino = ENDIAN_SHORT (dp->d_ino);

				iop->io_offset_low = offset; block_put (bp); return (1);
			}
		}

	}	/* end for (EVER) */

}	/* end v7_search_dir */

/*
 ****************************************************************
 *	Fornece entradas do diretório para o usuário 		*
 ****************************************************************
 */
void
v7_read_dir (IOREQ *iop)
{
	INODE		*ip = iop->io_ip;
	BHEAD		*bp = NOBHEAD;
	const V7DIR	*dp, *end_dp;
	daddr_t		lbn, pbn, rabn;
	off_t		offset;
	int		n, range;
	DIRENT		dirent;

	/*
	 *	Praeludium
	 */
	memclr (&dirent, sizeof (DIRENT));

	/*
	 *	Malha principal
	 */
	for (dp = end_dp = NOV7DIR; iop->io_count >= sizeof (DIRENT); dp++, iop->io_offset_low += sizeof (V7DIR))
	{
		if (dp >= end_dp)
		{
			/* Lê um bloco de diretório */

			if (bp != NOBHEAD)
				{ block_put (bp); bp = NOBHEAD; }

			if ((range = ip->i_size - iop->io_offset_low) <= 0)
				{ iop->io_eof = 1; break; }

			lbn    = iop->io_offset_low >> BLSHIFT;
			offset = iop->io_offset_low &  BLMASK;

			pbn = v7_block_map (ip, lbn, B_READ, &rabn);

			if (u.u_error)
				break;

			if (pbn < 0)
			{
				/* BLOCO dentro do arquivo, mas nunca foi escrito */

				bp = block_free_get (BLSZ);
				memclr (bp->b_addr, BLSZ);
				bp->b_residual = 0;
			}
			elif (ip->i_lastr + 1 == lbn)
			{
				bp = breada (ip->i_dev, pbn, rabn, 0);
				ip->i_lastr = lbn;
			}
			else
			{
				bp = bread (ip->i_dev, pbn, 0);
				ip->i_lastr = lbn;
			}

			if ((n = BLSZ - offset) > range)
				n = range;

			dp = bp->b_addr + offset; end_dp = (V7DIR *)((int)dp + n);
		}

		/*
		 *	Usa as entradas deste bloco
		 */
		if (dp->d_ino == 0)
			continue;

		dirent.d_offset = iop->io_offset_low;

		dirent.d_ino = ENDIAN_SHORT (dp->d_ino);

		if (dp->d_name[IDSZ-1] == '\0')
			dirent.d_namlen = strlen (dp->d_name);
		else
			dirent.d_namlen = IDSZ;

		memmove (dirent.d_name, dp->d_name, IDSZ); dirent.d_name[IDSZ] = '\0';

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

}	/* end v7_read_dir */

/*
 ****************************************************************
 *	Escreve a entrada no diretório				*
 ****************************************************************
 */
void
v7_write_dir (IOREQ *iop, const DIRENT *dep, const INODE *son_ip, int rename)
{
	V7DIR		*dp;
	BHEAD		*bp;
	int		len;
	INODE		*ip = iop->io_ip;
	ino_t		par_ino, old_ino;
	V7DIR		d;

	/*
	 *	Recebe "iop->io_ip" trancado e devolve destrancado
	 *
	 * 	"iop->io_offset_low" contém o deslocamento no diretório pai
	 *
	 *	Prepara a entrada do diretório
	 */
	memclr (&d, sizeof (V7DIR));

	if ((len = dep->d_namlen + 1) > IDSZ)
		len = IDSZ;

	d.d_ino = ENDIAN_SHORT (dep->d_ino);
	memmove (d.d_name, dep->d_name, len);

	/*
	 *	Escreve a entrada
	 */
   /***	iop->io_fd	    = ...;	***/
   /***	iop->io_fp	    = ...;	***/
   /***	iop->io_ip	    = ...;	***/
   /***	iop->io_dev	    = ...;	***/
	iop->io_area	    = &d;
	iop->io_count	    = sizeof (V7DIR);
   /***	iop->io_offset_low  = ...;	***/
	iop->io_cpflag	    = SS;
   /***	iop->io_rablock	    = ...;	***/

	v7_write (iop);

	/*
	 ******	Se for um diretório, ajusta a entrada ".." *********
	 */
	if (!rename || !S_ISDIR (son_ip->i_mode))
		{ iput (ip); return; }

	ip->i_nlink++; par_ino = ip->i_ino; inode_dirty_inc (ip); iput (ip);

	bp = bread (son_ip->i_dev, son_ip->i_addr[0], 0);

	if (u.u_error != NOERR)
		{ block_put (bp); return; }

	dp = bp->b_addr + sizeof (V7DIR);	/* Deslocamento do ".." */

	old_ino = ENDIAN_SHORT (dp->d_ino); dp->d_ino = ENDIAN_SHORT (par_ino);

	bdwrite (bp);

	if ((ip = iget (son_ip->i_dev, old_ino, 0)) == NOINODE)
		{ printf ("%g: NÃO consegui obter o INODE do pai\n"); return; }

	ip->i_nlink--; inode_dirty_inc (ip); iput (ip);

}	/* end v7_write_dir */

/*
 ****************************************************************
 *	Certifica-se de que um diretório está vazio		*
 ****************************************************************
 */
int
v7_empty_dir (INODE *ip)
{
	BHEAD		*bp = NOBHEAD;
	const V7DIR	*dp, *end_dp;
	dev_t		dev = ip->i_dev;
	daddr_t		lbn = 0, pbn, rabn;
	off_t		offset;
	int		n, range;

	/*
	 *	Recebe e devolve o "ip" trancado
	 *
	 *	Se estiver vazio, devolve 1
	 *
	 */

	/*
	 *	Malha principal
	 */
	for (dp = end_dp = NOV7DIR, offset = 0; /* abaixo */; dp++, offset += sizeof (V7DIR))
	{
		if (dp >= end_dp)
		{
			/* Lê um bloco de diretório */

			if (bp != NOBHEAD)
				{ block_put (bp); bp = NOBHEAD; }

			if ((range = ip->i_size - offset) <= 0)
				return (1);

			pbn = v7_block_map (ip, lbn, B_READ, &rabn);

			if (u.u_error)
				return (-1);

			if (pbn < 0)
			{
				/* BLOCO dentro do arquivo, mas nunca foi escrito */

				bp = block_free_get (BLSZ);
				memclr (bp->b_addr, BLSZ);
				bp->b_residual = 0;
			}
			elif (ip->i_lastr + 1 == lbn)
			{
				bp = breada (dev, pbn, rabn, 0);
				ip->i_lastr = lbn;
			}
			else
			{
				bp = bread (dev, pbn, 0);
				ip->i_lastr = lbn;
			}

			if ((n = BLSZ) > range)
				n = range;

			dp = bp->b_addr; end_dp = (V7DIR *)((int)dp + n);

			lbn++;
		}

		/*
		 *	Usa as entradas deste bloco
		 */
		if (offset < 2 * sizeof (V7DIR))		/* Pula "." e ".." */
			continue;

		if (dp->d_ino == NOINO)			/* Entrada vazia */
			continue;

		/* Achou algo */

		if (bp != NOBHEAD)
			block_put (bp);

		return (0);

	}	/* end for (EVER) */

}	/* end v7_empty_dir */

/*
 ****************************************************************
 *	Cria um diretório da V7FS				*
 ****************************************************************
 */
INODE *
v7_make_dir (IOREQ *iop, DIRENT *dep, int mode)
{
	INODE		*pp, *ip;
	dev_t		par_dev;
	ino_t		par_ino;
	V7DIR		d;

	pp = iop->io_ip;

	par_dev = pp->i_dev; par_ino = pp->i_ino;

	pp->i_nlink++;		/* Já levando em conta o ".." */

	if ((ip = v7_make_inode (iop, dep, mode)) == NOINODE)
		goto bad;

	/*
	 *	Prepara o "."
	 */
	memclr (&d, sizeof (V7DIR));
	d.d_ino = ENDIAN_SHORT (ip->i_ino);
	d.d_name[0] = '.';

   /***	iop->io_fd	    = ...;	***/
   /***	iop->io_fp	    = ...;	***/
	iop->io_ip	    = ip;
   /***	iop->io_dev	    = ...;	***/
	iop->io_area	    = &d;
	iop->io_count	    = sizeof (V7DIR);
	iop->io_offset_low  = 0;
	iop->io_cpflag	    = SS;
   /***	iop->io_rablock	    = ...;	***/

	v7_write (iop);

	/*
	 *	Prepara o ".."
	 */
   /***	memclr (&d, sizeof (V7DIR)); ***/
	d.d_ino = ENDIAN_SHORT (par_ino);
   /***	d.d_name[0] = '.'; ***/
	d.d_name[1] = '.';

   /***	iop->io_fd	    = ...;	***/
   /***	iop->io_fp	    = ...;	***/
	iop->io_ip	    = ip;
   /***	iop->io_dev	    = ...;	***/
	iop->io_area	    = &d;
	iop->io_count	    = sizeof (V7DIR);
	iop->io_offset_low  = sizeof (V7DIR);
   /***	iop->io_cpflag	    = SS;	***/
   /***	iop->io_rablock	    = ...;	***/

	v7_write (iop);

	ip->i_nlink = 2;

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

}	/* end v7_make_dir */

/*
 ****************************************************************
 *	Apaga uma entrada de diretório				*
 ****************************************************************
 */
void
v7_clr_dir (IOREQ *iop, const DIRENT *dep, const INODE *son_ip)
{
	V7DIR		d;

	/*
	 *	Apaga a entrada do diretório
	 */
	memclr (&d, sizeof (V7DIR) );

   /***	iop->io_fd	    = ...;	***/
   /***	iop->io_fp	    = ...;	***/
   /***	iop->io_ip	    = pp;	***/
   /***	iop->io_dev	    = ...;	***/
	iop->io_area	    = &d;
	iop->io_count	    = sizeof (V7DIR);
   /***	iop->io_offset_low  = ...;			/* De "iname" */
	iop->io_cpflag	    = SS;
   /***	iop->io_rablock	    = ...;	***/

	v7_write (iop);

}	/* end v7_clr_dir */
