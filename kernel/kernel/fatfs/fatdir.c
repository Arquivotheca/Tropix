/*
 ****************************************************************
 *								*
 *			fatdir.c				*
 *								*
 *	Tratamento de diretórios do FATFS			*
 *								*
 *	Versão	4.2.0, de 26.12.01				*
 *		4.8.0, de 02.11.05				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2005 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include "../h/common.h"
#include "../h/sync.h"
#include "../h/scb.h"
#include "../h/region.h"

#include "../h/mntent.h"
#include "../h/sb.h"
#include "../h/kfile.h"
#include "../h/fat.h"
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
 ******	Protótipos de funções ***********************************
 */
void	fat_get_par_ino (INODE *ip);
ino_t	fat_search_clusno (INODE *, clusno_t, clusno_t, clusno_t *);

/*
 ****************************************************************
 *	Procura um nome dado no diretório			*
 ****************************************************************
 */
int
fat_search_dir (IOREQ *iop, DIRENT *dep, int mission)
{
	INODE		*ip = iop->io_ip;
	FATSB		*sp = ip->i_sb->sb_ptr;
	BHEAD		*bp = NOBHEAD;
	const FATDIR	*dp, *end_dp;
	dev_t		dev = ip->i_dev;
	daddr_t		blkno, end_blkno;
	clusno_t	log_clusno, clusno;
	int		code;
	off_t		offset;
	FATLFN		z;

	/*
	 *	Recebe e devolve o "ip" trancado
	 *
	 *	Se encontrou a entrada, devolve:
	 *		"dep->d_ino"			     o INO  (caso normal)
	 *		"dep->d_namlen" e "dep->d_name"	     o nome (caso INUMBER)
	 *		"iop->io_offset_low" o início da entrada
	 *		"iop->io_count"	     o tamanho da entrada
	 *
	 *	(exceto para "." e "..")
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
			if (ip->i_par_ino == NOINO)
				fat_get_par_ino (ip);

			dep->d_ino = ip->i_par_ino; return (1);
		}
	}

	/*
	 *	Obtém os blocos iniciais
	 */
	log_clusno = 0; clusno = ip->i_first_clusno;

	if (clusno == 0)
		ROOT_FIRST_AND_LAST_BLK (blkno, end_blkno);
	else
		FIRST_AND_LAST_BLK (clusno, blkno, end_blkno);

	blkno -= 1;	/* É incrementado antes da primeira leitura */

	FAT_LFN_INIT (&z);

	/*
	 *	Malha principal
	 */
	for (dp = end_dp = NOFATDIR, offset = 0; /* abaixo */; dp++, offset += sizeof (FATDIR))
	{
		if (dp >= end_dp)
		{
			/* Lê um bloco de diretório */

			if (bp != NOBHEAD)
				{ block_put (bp); bp = NOBHEAD; }

			if (++blkno >= end_blkno)
			{
				if (clusno == 0)		/* RAIZ */
					return (0);

				clusno = fat_cluster_read_map (ip, ++log_clusno);

				if (C_ISBAD_OR_EOF (clusno))
					return (0);

				FIRST_AND_LAST_BLK (clusno, blkno, end_blkno);
			}

			if (blkno < end_blkno - 1)
				bp = breada (dev, blkno, blkno + 1, 0);
			else
				bp = bread (dev, blkno, 0);

			dp = bp->b_addr; end_dp = (FATDIR *)((int)dp + BLSZ);
		}

		/*
		 *	Usa as entradas deste bloco
		 */
		if (dp->d_name[0] == '\0')			/* Final do diretório */
			{ block_put (bp); return (0); }

		if (dp->d_name[0] == FAT_EMPTY)			/* Entrada vazia */
			{ FAT_LFN_RESET (&z); continue; }

		if (FAT_LFN_EMPTY (&z))				/* Guarda o "offset" do início da entrada */
			iop->io_offset_low = offset;

		if (FAT_ISLFN (dp->d_mode))
			{ fat_lfn_add (dp, &z); continue; }

		/* Terminou um nome: compara */

		fat_lfn_get_nm (ip->i_sb, dp, &z);

		if (mission & INUMBER)		/* Busca dado o INO */
		{
			if (FAT_MAKE_INO (blkno, offset) == dep->d_ino)
			{
				dep->d_namlen = strlen (z.z_lfn_nm);
				memmove (dep->d_name, z.z_lfn_nm, dep->d_namlen + 1);

				block_put (bp); return (1);
			}
		}
		else				/* Busca dado o nome */
		{
			if ((ip->i_sb->sb_mntent.me_flags & SB_CASE) == 0 || (mission & ICREATE))
				code = strnocaseeq (z.z_lfn_nm, dep->d_name);
			else
				code = streq (z.z_lfn_nm, dep->d_name);

			if (code != 0)
			{
				dep->d_ino = FAT_MAKE_INO (blkno, offset);

				strcpy (dep->d_name, z.z_lfn_nm);	/* Coloca o nome original */

				iop->io_count = offset - iop->io_offset_low + sizeof (FATDIR);

				block_put (bp); return (1);
			}
		}

		FAT_LFN_RESET (&z);

	}	/* end for (dp = ...) */

}	/* end fat_search_dir */

/*
 ****************************************************************
 *	Fornece entradas do diretório para o usuário		*
 ****************************************************************
 */
void
fat_read_dir (IOREQ *iop)
{
	INODE		*ip = iop->io_ip;
	FATSB		*sp = ip->i_sb->sb_ptr;
	BHEAD		*bp = NOBHEAD;
	const FATDIR	*dp, *end_dp;
	clusno_t	log_clusno, ph_clusno;
	int		offset, file_offset;
	daddr_t		blkno = 0, end_blkno = 0;
	DIRENT		dirent;
	FATLFN		z;

	/*
	 *	Verifica se foi pedida a entrada "."
	 */
	if (iop->io_offset_low == 0)
	{
		dirent.d_offset	= 0;
		dirent.d_ino	= ip->i_ino;
		dirent.d_namlen	= 1;
		strcpy (dirent.d_name, ".");

		if (unimove (iop->io_area, &dirent, sizeof (DIRENT), iop->io_cpflag) < 0)
			{ u.u_error = EFAULT; return; }

		/* Avança os ponteiros */

		iop->io_area	   += sizeof (DIRENT);
		iop->io_count	   -= sizeof (DIRENT);
		iop->io_offset_low += sizeof (FATDIR);

		if (iop->io_count < sizeof (DIRENT))
			return;
	}

	/*
	 *	Verifica se foi pedida a entrada ".."
	 */
	if (iop->io_offset_low == sizeof (FATDIR))
	{
		if (ip->i_par_ino == NOINO)
			fat_get_par_ino (ip);

		dirent.d_offset	= sizeof (FATDIR);
		dirent.d_ino	= ip->i_par_ino;
		dirent.d_namlen	= 2;
		strcpy (dirent.d_name, "..");

		if (unimove (iop->io_area, &dirent, sizeof (DIRENT), iop->io_cpflag) < 0)
			{ u.u_error = EFAULT; return; }

		/* Avança os ponteiros */

		iop->io_area	   += sizeof (DIRENT);
		iop->io_count	   -= sizeof (DIRENT);
		iop->io_offset_low += sizeof (FATDIR);

		if (iop->io_count < sizeof (DIRENT))
			return;
	}

	/*
	 *	Compensa os diretórios RAIZ, pois não têm "." e ".."
	 */
	file_offset = iop->io_offset_low;

	if (ip->i_ino == FAT_ROOT_INO)
		file_offset -= 2 * sizeof (FATDIR);

	FAT_LFN_INIT (&z);

	/*
	 *	Malha principal
	 */
	for
	(	dp = end_dp = NOFATDIR;
		iop->io_count >= sizeof (DIRENT);
		dp++, iop->io_offset_low += sizeof (FATDIR), file_offset += sizeof (FATDIR)
	)
	{
		if (dp >= end_dp)
		{
			/* Lê um bloco de diretório */

			if (bp != NOBHEAD)
				{ block_put (bp); bp = NOBHEAD; }

			if (++blkno >= end_blkno)
			{
				if (ip->i_first_clusno)
				{
					log_clusno = file_offset >> sp->s_CLUSTER_SHIFT;
					offset	   = file_offset &  sp->s_CLUSTER_MASK;

					ph_clusno = fat_cluster_read_map (ip, log_clusno);

					if (C_ISBAD_OR_EOF (ph_clusno))
						{ iop->io_eof = 1; break; }
				
					FIRST_AND_LAST_BLK (ph_clusno, blkno, end_blkno);

					blkno += offset >> BLSHIFT;
				}
				else
				{
					ROOT_FIRST_AND_LAST_BLK (blkno, end_blkno);

					blkno += file_offset >> BLSHIFT;

					if (blkno >= end_blkno)
						{ iop->io_eof = 1; break; }
				}
			}

			if (blkno < end_blkno - 1)
				bp = breada (ip->i_dev, blkno, blkno + 1, 0);
			else
				bp = bread (ip->i_dev, blkno, 0);

			dp = bp->b_addr + (file_offset & BLMASK); end_dp = bp->b_addr + BLSZ;

		}	/* end if (dp >= end_dp) */

		/*
		 *	Usa as entradas deste bloco
		 */
		if (dp->d_name[0] == '\0')			/* Final do diretório */
			break;

		if (dp->d_name[0] == FAT_EMPTY)			/* Entrada vazia */
			{ FAT_LFN_RESET (&z); continue; }

		if (FAT_LFN_EMPTY (&z))				/* Guarda o início do nome */
			dirent.d_offset	= iop->io_offset_low;

		if (FAT_ISLFN (dp->d_mode))
			{ fat_lfn_add (dp, &z); continue; }

		fat_lfn_get_nm (ip->i_sb, dp, &z);

		if (FAT_ISVOL (dp->d_mode))			/* Por enquanto, ignorando VOL */
			{ FAT_LFN_RESET (&z); continue; }

		if (streq (z.z_lfn_nm, ".") || streq (z.z_lfn_nm, ".."))
		{
			printf
			(	"fat_read_dir: Li a entrada inesperada \"%s\" do INO %d\n",
				z.z_lfn_nm, ip->i_ino
			);
		}

		/* Achou um nome útil */

	   /***	dirent.d_offset	= iop->io_offset_low; ***/
		dirent.d_ino	= FAT_MAKE_INO (blkno, file_offset);
		dirent.d_namlen	= strlen (z.z_lfn_nm);
		memmove (dirent.d_name, z.z_lfn_nm, dirent.d_namlen + 1);

		if (unimove (iop->io_area, &dirent, sizeof (DIRENT), iop->io_cpflag) < 0)
			{ u.u_error = EFAULT; break; }

		/* Avança os ponteiros */

		iop->io_area  += sizeof (DIRENT);
		iop->io_count -= sizeof (DIRENT);

		FAT_LFN_RESET (&z);

	}	/* end for (dp = ...) */
	
	/*
	 *	Postludium
	 */
	if (bp != NOBHEAD)
		block_put (bp);

}	/* end fat_read_dir */

/*
 ****************************************************************
 *	Escreve a entrada no diretório				*
 ****************************************************************
 */
void
fat_write_dir (IOREQ *iop, const DIRENT *dep, const INODE *son_ip, int rename)
{
	INODE		*ip = iop->io_ip, *new_ip;
	ino_t		ino;
	int		son_is_dir;

	/*
	 *	Se não sabe que a entrada anterior do diretório será removida, ...
	 */
	if (!rename)
		{ u.u_error = EMLINK; iput (ip); return; }

	/*
	 *	Escreve a entrada no diretório pai, e obtém um INODE 
	 */
	if ((ino = fat_lfn_dir_put (ip, dep)) == -1)
		{ iput (ip); return; }

	if ((new_ip = iget (ip->i_dev, ino, 0)) == NOINODE)
		{ iput (ip); return; }

	if (son_is_dir = S_ISDIR (son_ip->i_mode))
		{ ip->i_nlink++; inode_dirty_inc (ip); }

	/*
	 *	Prepara o INODE
	 */
	new_ip->i_fsp		= &fstab[FS_FAT];

	new_ip->i_mode		= son_ip->i_mode;
	new_ip->i_uid		= son_ip->i_uid;
	new_ip->i_gid		= son_ip->i_gid;
	new_ip->i_nlink		= son_ip->i_nlink;
	new_ip->i_size		= son_ip->i_size;

	new_ip->i_atime		= son_ip->i_atime;
	new_ip->i_mtime		= son_ip->i_mtime;
	new_ip->i_ctime		= son_ip->i_ctime;

	new_ip->i_first_clusno	= son_ip->i_first_clusno;
	new_ip->i_par_ino	= ip->i_ino;

	new_ip->i_clusvec_sz	= son_ip->i_clusvec_sz;
	new_ip->i_clusvec_len	= son_ip->i_clusvec_len;
	new_ip->i_clusvec	= son_ip->i_clusvec; ((INODE *)son_ip)->i_clusvec = NULL;

	inode_dirty_inc (new_ip); iput (new_ip); iput (ip);

	/*
	 ****** Se for um diretório, ajusta a entrada ".." *********
	 */
	if (!son_is_dir)
		return;

	if (son_ip->i_par_ino == NOINO)
		fat_get_par_ino ((INODE *)son_ip);

	if ((ip = iget (son_ip->i_dev, son_ip->i_par_ino, 0)) == NOINODE)
		{ printf ("%g: NÃO consegui obter o INODE do pai\n"); return; }

	ip->i_nlink--; inode_dirty_inc (ip); iput (ip);

}	/* end fat_write_dir */

/*
 ****************************************************************
 *	Certifica-se de que um diretório está vazio		*
 ****************************************************************
 */
int
fat_empty_dir (INODE *ip)
{
	BHEAD		*bp = NOBHEAD;
	const FATDIR	*dp, *end_dp;
	dev_t		dev = ip->i_dev;
	off_t		offset;
	FATSB		*sp = ip->i_sb->sb_ptr;
	daddr_t		blkno, end_blkno;
	clusno_t	log_clusno, clusno;

	/*
	 *	Recebe e devolve o "ip" trancado
	 *
	 *	Se estiver vazio, devolve 1
	 *
	 */

	/*
	 *	Obtém os blocos iniciais
	 */
	log_clusno = 0; clusno = ip->i_first_clusno;

	if (clusno == 0)			/* Não queremos remover a RAIZ */
		return (0);

	FIRST_AND_LAST_BLK (clusno, blkno, end_blkno);

	blkno -= 1;	/* É incrementado antes da primeira leitura */

	/*
	 *	Malha principal
	 */
	for (dp = end_dp = NOFATDIR, offset = 0; /* abaixo */; dp++, offset += sizeof (FATDIR))
	{
		if (dp >= end_dp)
		{
			/* Lê um bloco de diretório */

			if (bp != NOBHEAD)
				{ block_put (bp); bp = NOBHEAD; }

			if (++blkno >= end_blkno)
			{
				clusno = fat_cluster_read_map (ip, ++log_clusno);

				if (C_ISBAD_OR_EOF (clusno))
					return (1);

				FIRST_AND_LAST_BLK (clusno, blkno, end_blkno);
			}

			if (blkno < end_blkno - 1)
				bp = breada (dev, blkno, blkno + 1, 0);
			else
				bp = bread (dev, blkno, 0);

			dp = bp->b_addr; end_dp = (FATDIR *)((int)dp + BLSZ);
		}

		/*
		 *	Usa as entradas deste bloco
		 */
		if (offset < 2 * sizeof (FATDIR))	/* Pula "." e ".." */
			continue;

		if (dp->d_name[0] == FAT_EMPTY)		/* Entrada vazia */
			continue;

		if (dp->d_name[0] == '\0')		/* Final do diretório */
		{
			if (bp != NOBHEAD)
				block_put (bp);

			return (1);
		}

		/* Achou algo */

		if (bp != NOBHEAD)
			block_put (bp);

		return (0);

	}	/* end for (dp = ...) */

}	/* end fat_empty_dir */

/*
 ****************************************************************
 *	Prepara as entradas de um diretório			*
 ****************************************************************
 */
INODE *
fat_make_dir (IOREQ *iop, DIRENT *dep, int mode)
{
	const FATSB	*sp;
	INODE		*pp, *ip;
	FATDIR		*dp;
	BHEAD		*bp;
	daddr_t		blkno;
#define	PUT_TIME_IN_DOTs
#ifdef	PUT_TIME_IN_DOTs
	ushort		dummy_time;
#endif	PUT_TIME_IN_DOTs
	ino_t		par_ino;
	dev_t		par_dev;
   /***	clusno_t	par_clusno;	***/

	pp = iop->io_ip;

	par_dev = pp->i_dev; par_ino = pp->i_ino;

	pp->i_nlink++;		/* Já levando em conta o ".." */

   /***	par_clusno = pp->i_first_clusno; ***/

	if ((ip = fat_make_inode (iop, dep, mode)) == NOINODE)
		goto bad1;

	/*
	 *	Obtém um "cluster" para o diretório
	 */
	sp = ip->i_sb->sb_ptr;

	if ((ip->i_first_clusno = fat_cluster_write_map (ip, 0)) < 0)
		{ u.u_error = ENOSPC; goto bad0; }

	blkno = FIRST_BLK (ip->i_first_clusno);

	bp = bread (ip->i_dev, blkno, 0);

	if (u.u_error != NOERR)
		{ block_put (bp); goto bad0; }

	/*
	 *	Prepara o "."
	 */
	dp = bp->b_addr;

	memmove (dp->d_name, ".          ", sizeof (dp->d_name) + sizeof (dp->d_ext));
	dp->d_mode	= FAT_DIR;
#ifdef	PUT_TIME_IN_DOTs
	fat_put_time (time, &dp->d_ctime, &dp->d_cdate);
	fat_put_time (time, &dummy_time,  &dp->d_adate);
#endif	PUT_TIME_IN_DOTs
	fat_put_time (time, &dp->d_mtime, &dp->d_mdate);
   /***	dp->d_ccenti	= 0; ***/
	FAT_PUT_CLUSTER (ip->i_first_clusno, dp);
   /***	dp->d_size	= 0; ***/

	/*
	 *	Prepara o ".."
	 */
	dp++;

	memmove (dp->d_name, "..         ", sizeof (dp->d_name) + sizeof (dp->d_ext));
	dp->d_mode	= FAT_DIR;
#ifdef	PUT_TIME_IN_DOTs
	fat_put_time (time, &dp->d_ctime, &dp->d_cdate);
	fat_put_time (time, &dummy_time,  &dp->d_adate);
#endif	PUT_TIME_IN_DOTs
	fat_put_time (time, &dp->d_mtime, &dp->d_mdate);
   /***	dp->d_ccenti	= 0; ***/
	if (pp->i_first_clusno != sp->s_root_cluster)
		FAT_PUT_CLUSTER (pp->i_first_clusno, dp);
   /***	dp->d_size	= 0; ***/

	/*
	 *	Escreve o diretório
	 */
	bdwrite (bp);

	ip->i_nlink = 2;

	if (u.u_error == NOERR)
		return (ip);

	/*
	 *	Se houve erro, ...
	 */
    bad0:
	iput (ip);
    bad1:
	if ((pp = iget (par_dev, par_ino, 0)) != NOINODE)
		{ pp->i_nlink--; iput (pp); }

	return (NOINODE);

}	/* end fat_make_dir */

/*
 ****************************************************************
 *	Remove uma entrada de diretório				*
 ****************************************************************
 */
void
fat_clr_dir (IOREQ *iop, const DIRENT *dep, const INODE *son_ip)
{
	void		*area;
	FATDIR		*dp;
	const FATDIR	*end_dp;

	/*
	 *	Na estrutura IOREQ "*iop" devem estar definidos:
	 *
	 *	io_ip:		 INODE
	 *	io_offset_low:	 Começo da entrada
	 *	io_count:	 Tamanho da entrada
	 *
	 */
	if ((area = malloc_byte (iop->io_count)) == NOVOID)
		{ u.u_error = ENOMEM; return; }

	memclr (area, iop->io_count);

	for (dp = area, end_dp = (FATDIR *)((int)dp + iop->io_count);  dp < end_dp; dp++)
		dp->d_name[0] = FAT_EMPTY;

   /***	iop->io_fd	   = ...;	***/
   /***	iop->io_fp	   = ...;	***/
   /***	iop->io_ip	   = pp;	***/
   /***	iop->io_dev	   = ...;	***/
	iop->io_area	   = area;
   /***	iop->io_count	   = ...;	***/		/* De "iname" */
   /***	iop->io_offset_low = ...;			/* De "iname" */
	iop->io_cpflag	   = SS;
   /***	iop->io_rablock	   = ...;	***/

	fat_write (iop);

	iop->io_ip->i_size = 0;		/* É um diretório */

	free_byte (area);

}	/* end fat_clr_dir */

/*
 ****************************************************************
 *	Obtém o número de referências a este diretório		*
 ****************************************************************
 */
void
fat_get_nlink_dir (INODE *ip)
{
	BHEAD		*bp = NOBHEAD;
	const FATDIR	*dp, *end_dp;
	dev_t		dev = ip->i_dev;
	off_t		offset;
	FATSB		*sp = ip->i_sb->sb_ptr;
	daddr_t		blkno, end_blkno;
	clusno_t	log_clusno, clusno;
	FATLFN		z;

	/*
	 *	Recebe e devolve o "ip" trancado
	 *
	 *	Se estiver vazio, devolve 1
	 *
	 */

	/*
	 *	Obtém os blocos iniciais
	 */
	log_clusno = 0; clusno = ip->i_first_clusno;

	if (clusno == 0)
		ROOT_FIRST_AND_LAST_BLK (blkno, end_blkno);
	else
		FIRST_AND_LAST_BLK (clusno, blkno, end_blkno);

	blkno -= 1;	/* É incrementado antes da primeira leitura */

	ip->i_nlink = 2;

	FAT_LFN_INIT (&z);

	/*
	 *	Malha principal
	 */
	for (dp = end_dp = NOFATDIR, offset = 0; /* abaixo */; dp++, offset += sizeof (FATDIR))
	{
		if (dp >= end_dp)
		{
			/* Lê um bloco de diretório */

			if (bp != NOBHEAD)
				{ block_put (bp); bp = NOBHEAD; }

			if (++blkno >= end_blkno)
			{
				if (clusno == 0)		/* RAIZ */
					return;

				if (C_ISBAD_OR_EOF (clusno = fat_cluster_read_map (ip, ++log_clusno)))
					return;

#if (0)	/*******************************************************/
				clusno = fat_cluster_read_map (ip, ++log_clusno);

				if (C_ISBAD_OR_EOF (clusno))
					return;
#endif	/*******************************************************/

				FIRST_AND_LAST_BLK (clusno, blkno, end_blkno);
			}

			if (blkno < end_blkno - 1)
				bp = breada (dev, blkno, blkno + 1, 0);
			else
				bp = bread (dev, blkno, 0);

			dp = bp->b_addr; end_dp = (FATDIR *)((int)dp + BLSZ);
		}

		/*
		 *	Usa as entradas deste bloco
		 */
		if (dp->d_name[0] == '\0')		/* Final do diretório */
		{
			if (bp != NOBHEAD)
				block_put (bp);

			return;
		}

		if (dp->d_name[0] == FAT_EMPTY)		/* Entrada vazia */
			{ FAT_LFN_RESET (&z); continue; }

		if (FAT_ISLFN (dp->d_mode))
			{ fat_lfn_add (dp, &z); continue; }

		/* Terminou um nome: examina */

		fat_lfn_get_nm (ip->i_sb, dp, &z);

		if (z.z_lfn_nm[0] == '.')		/* Pula "." e ".." */
		{
			if (z.z_lfn_nm[1] == '\0' || z.z_lfn_nm[1] == '.' && z.z_lfn_nm[2] == '\0')
				{ FAT_LFN_RESET (&z); continue; }
		}

		if (FAT_ISVOL (dp->d_mode) && ip->i_ino == FAT_ROOT_INO)
			strscpy (ip->i_sb->sb_fname, z.z_lfn_nm, sizeof (ip->i_sb->sb_fname));

		if (FAT_ISDIR (dp->d_mode))
			ip->i_nlink++;

		FAT_LFN_RESET (&z);

	}	/* end for (dp = ...) */

}	/* end fat_get_nlink_dir */

/*
 ****************************************************************
 *	Obtém o INO do diretório pai				*
 ****************************************************************
 */
void
fat_get_par_ino (INODE *ip)
{
	FATSB		*sp = ip->i_sb->sb_ptr;
	const FATDIR	*dp;
	daddr_t		blkno;
	BHEAD		*bp;
	clusno_t	par_clusno, par_par_clusno = -1;

	/*
	 *	Se for a RAIZ, ...
	 */
	if (ip->i_ino == FAT_ROOT_INO)
		{ ip->i_par_ino = FAT_ROOT_INO; return; }

	/*
	 *	Lê o primeiro bloco do diretório
	 */
	blkno = FIRST_BLK (ip->i_first_clusno);

	bp = bread (ip->i_dev, blkno, 0);

	dp = bp->b_addr + sizeof (FATDIR);	/* Segunda entrada */

	if (!memeq (dp->d_name, "..      ", 8))
	{
		printf ("fat_read_dir: A segunda entrada do INO %d NÃO é \"..\"\n", ip->i_ino);
		block_put (bp);
		ip->i_par_ino = FAT_ROOT_INO;
		return;
	}

	par_clusno = fat_get_cluster (sp, dp);

	block_put (bp);

	/*
	 *	Obtém o CLUSTER do avô
	 */
	fat_search_clusno (ip, par_clusno, NOINO, &par_par_clusno);

	if (par_par_clusno == -1)			/* Se o pai não tem "..", é a RAIZ */
		{ ip->i_par_ino = FAT_ROOT_INO; return; }

	/*
	 *	Procura a entrada do pai no diretório avô
	 */
	ip->i_par_ino = fat_search_clusno (ip, par_par_clusno, par_clusno, &par_par_clusno);

}	/* end fat_get_par_ino */

/*
 ****************************************************************
 *	Procura a entrada do diretório, dado o CLUSNO		*
 ****************************************************************
 */
ino_t
fat_search_clusno (INODE *ip, clusno_t clusno, clusno_t search_clusno, clusno_t *par_clusno)
{
	FATSB		*sp = ip->i_sb->sb_ptr;
	BHEAD		*bp = NOBHEAD;
	const FATDIR	*dp, *end_dp;
	daddr_t		blkno, end_blkno;
	int		offset;

	/*
	 *	Recebe:
	 *		em "clusno" o diretório no qual buscar
	 *		o "search_clusno" e o ".."
	 *
	 *	Retorna:
	 *		o INO do diretório começando por "search_clusno"
	 *		o CLUSNO do ".." e em "*par_clusno"
	 */

	/*
	 *	Obtém os blocos iniciais
	 */
	if (clusno == 0)
		ROOT_FIRST_AND_LAST_BLK (blkno, end_blkno);
	else
		FIRST_AND_LAST_BLK (clusno, blkno, end_blkno);

	blkno -= 1;	/* É incrementado antes da primeira leitura */

	/*
	 *	Malha principal
	 */
	for (dp = end_dp = NOFATDIR, offset = 0; /* abaixo */; dp++, offset += sizeof (FATDIR))
	{
		if (dp >= end_dp)
		{
			/* Lê um bloco de diretório */

			if (bp != NOBHEAD)
				{ block_put (bp); bp = NOBHEAD; }

			if (++blkno >= end_blkno)
			{
				if (clusno == 0)		/* RAIZ */
					return (NOINO);

				clusno = fat_get_fat_value (ip->i_sb, clusno);

				if (C_ISBAD_OR_EOF (clusno))
					return (NOINO);

				FIRST_AND_LAST_BLK (clusno, blkno, end_blkno);
			}

			bp = bread (ip->i_dev, blkno, 0);

			dp = bp->b_addr; end_dp = (FATDIR *)((int)dp + BLSZ);
		}

		/*
		 *	Usa as entradas deste bloco
		 */
		if (dp->d_name[0] == '\0')		/* Final do diretório */
		{
			if (bp != NOBHEAD)
				block_put (bp);

			return (NOINO);
		}

		if (dp->d_name[0] == FAT_EMPTY)		/* Entrada vazia */
			continue;

		if (memeq (dp->d_name, "..      ", 8))	/* Supõe ".." no início */
		{
			*par_clusno = fat_get_cluster (sp, dp);

			if (search_clusno != NOINO)
				continue;

			if (bp != NOBHEAD)
				block_put (bp);

			return (NOINO);
		}

		if (fat_get_cluster (sp, dp) != search_clusno)
			continue;

		/* Achou */

		if (bp != NOBHEAD)
			block_put (bp);

		return (FAT_MAKE_INO (blkno, offset));

	}	/* end for (EVER) */

}	/* end fat_search_clusno */

/*
 ****************************************************************
 *	Obtém o número do "cluster"				*
 ****************************************************************
 */
int
fat_get_cluster (const FATSB *sp, const FATDIR *dp)
{
	int		cluster;

	cluster = FAT_GET_SHORT (dp->d_low_cluster);

	if (sp->s_fat_bits == 32)
	{
		cluster |= (FAT_GET_SHORT (dp->d_high_cluster) << 16);

		if (cluster == 0)
			cluster = sp->s_root_cluster;
	}

	return (cluster);

}	/* end fat_get_cluster */
