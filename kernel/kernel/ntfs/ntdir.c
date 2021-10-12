/*
 ****************************************************************
 *								*
 *			ntdir.c					*
 *								*
 *	Tratamento de diretórios do FATFS			*
 *								*
 *	Versão	4.6.0, de 29.06.04				*
 *		4.8.0, de 02.08.05				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 * 		Copyright © 2005 NCE/UFRJ - tecle "man licença"	*
 *		Baseado no FreeBSD 5.2				*
 * 								*
 ****************************************************************
 */

#include "../h/common.h"
#include "../h/sync.h"
#include "../h/scb.h"
#include "../h/region.h"

#include "../h/mntent.h"
#include "../h/sb.h"
#include "../h/kfile.h"
#include "../h/nt.h"
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
#define	io_entryno	io_offset_low

/*
 ****************************************************************
 *	Procura um nome dado no diretório			*
 ****************************************************************
 */
int
nt_search_dir (IOREQ *iop, DIRENT *dep, int mission)
{
	INODE			*ip = iop->io_ip;
	const NTSB		*ntsp = ip->i_sb->sb_ptr;
	int			code, status = -1;	/* Em princípio, erro */
	struct ntvattr		*ir_vap;		/* IndexRoot attribute */
	struct ntvattr		*bm_vap  = NULL;	/* BitMap attribute */
	struct ntvattr		*ia_vap  = NULL;	/* IndexAllocation attribute */
	char			*bitmap  = NULL;
	int			clus_per_dirblk = 1;	/* Clusters per directory block */
	int			dir_blkno;
	int			type;			/* Current attribute type */
	int			offset;
	int			size;			/* Length of data to read */
	int			dir_offset;
	struct attr_indexentry	*iep;
	char			*area = NOSTR;
	int			count;
	IOREQ			io;
	DIRENT			dirent;

#undef	DEBUG
#ifdef	DEBUG
	printf ("nt_search_dir: procurando \"%s\"\n", dep->d_name);
#endif	DEBUG

	/*
	 *	Recebe e devolve o "ip" trancado
	 *
	 *	Se encontrou a entrada, devolve:
	 *		"dep->d_ino"			     o INO  (caso normal)
	 *		"dep->d_namlen" e "dep->d_name"	     o nome (caso INUMBER)
	 *
	 *	Se NÃO encontrou a entrada, retorna  == 0
	 */
	if (ip->i_sb->sb_mntent.me_flags & SB_ATIME)
		{ ip->i_atime = time; /*** inode_dirty_inc (ip); ***/ }

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
			struct ntvattr	*vap;

			if ((vap = ntfs_ntvattrget (ntsp, ip, NTFS_A_NAME, NULL, 0, 0)) == NULL)
				return (-1);

			dep->d_ino = vap->va_a_name->n_pnumber; return (1);
		}
	}

	/*
	 *		Prepara a leitura do diretório
	 *
	 * Read ntfs dir like stream of attr_indexentry, not like btree of them.
	 * This is done by scaning $BITMAP:$I30 for busy clusters and reading them.
	 * Of course, $INDEX_ROOT:$I30 is read before. Last read values are stored in
	 * INODE, so we can skip toward record number num almost immediatly.
	 * Anyway this is rather slow routine. The problem is that we don't know
	 * how many records are there in $INDEX_ALLOCATION:$I30 block.
	 */
	if ((ir_vap = ntfs_ntvattrget (ntsp, ip, NTFS_A_INDXROOT, "$I30", 0, 0)) == NULL)
		{ u.u_error = ENOTDIR; return (-1); }
	
	count = ir_vap->va_a_iroot->ir_size;

	area  = malloc_byte (MAX (ir_vap->va_datalen, count));

	/*
	 *	Examina o modo de acesso ao diretório
	 */
	if (ir_vap->va_a_iroot->ir_flag & NTFS_IRFLAG_INDXALLOC)
	{
		if ((bm_vap = ntfs_ntvattrget (ntsp, ip, NTFS_A_INDXBITMAP, "$I30", 0, 0)) == NULL)
			goto out;

		bitmap = malloc_byte (bm_vap->va_datalen);

		io.io_ip	 = ip;
		io.io_area	 = bitmap;
		io.io_count	 = bm_vap->va_datalen;
		io.io_offset_low = 0;
		io.io_cpflag	 = SS;

		if (ntfs_readattr (ntsp, NTFS_A_INDXBITMAP, "$I30", &io) < 0)
			goto out;

		if ((ia_vap = ntfs_ntvattrget (ntsp, ip, NTFS_A_INDX, "$I30", 0, 0)) == NULL)
			goto out;

		clus_per_dirblk = (count + ntsp->s_CLUSTER_MASK) >> ntsp->s_CLUSTER_SHIFT;
	}
	else
	{
		ia_vap = bm_vap = NULL; bitmap = NULL;
	}

	type	  = NTFS_A_INDXROOT;
	offset	  = sizeof (struct attr_indexroot);
	dir_blkno = 0;

	/*
	 *	Percorre os vários blocos do diretório
	 */
	do
	{
		size = (type == NTFS_A_INDXROOT) ? ir_vap->va_datalen : count;

		dir_offset = NTFS_CLUSTER_to_BYTE (dir_blkno * clus_per_dirblk);

		io.io_ip	 = ip;
		io.io_area	 = area;
		io.io_count	 = size;
		io.io_offset_low = dir_offset;
		io.io_cpflag	 = SS;

		if (ntfs_readattr (ntsp, type, "$I30", &io) < 0)
			goto out;

		if (type == NTFS_A_INDX)
		{
			if (ntfs_procfixups (ntsp, NTFS_INDXMAGIC, area, size) < 0)
				goto out;
		}

		if (offset == 0)
		{
			if (type == NTFS_A_INDX)
				offset = (0x18 + ((struct attr_indexalloc *)area)->ia_hdrsize);
			else
				offset = sizeof (struct attr_indexroot);
		}

		/*
		 *	Percorre um bloco do diretório
		 */
		for
		(	iep = (struct attr_indexentry *)(area + offset);
			!(iep->ie_flag & NTFS_IEFLAG_LAST)  &&  offset < size;
			offset += iep->reclen, iep = (struct attr_indexentry *)(area + offset)
		)
		{
			if (iep->ie_fnametype == 2)
				continue;

			if (ip->i_ino == NTFS_ROOTINO)
			{ 
				if (iep->ie_fnm_len == 1 && iep->ie_fname[0] == '.')
					continue;

				if (iep->ie_fname[0] == '$')
					continue;
			}

			if (mission & INUMBER)		/* Busca dado o INO */
			{
				if (iep->ie_number == dep->d_ino)
				{
					dep->d_namlen = iep->ie_fnm_len;

					little_unicode2iso (dep->d_name, iep->ie_fname, iep->ie_fnm_len);

					status = 1; goto out;
				}
			}
			else				/* Busca dado o nome */
			{
				if (iep->ie_fnm_len != dep->d_namlen)
					continue;

				little_unicode2iso (dirent.d_name, iep->ie_fname, iep->ie_fnm_len);
#ifdef	DEBUG
				printf ("nt_search_dir: comparando \"%s\" com \"%s\"\n", dirent.d_name, dep->d_name);
#endif	DEBUG
				if ((ip->i_sb->sb_mntent.me_flags & SB_CASE) == 0 || (mission & ICREATE))
					code = strnocaseeq (dirent.d_name, dep->d_name);
				else
					code = streq (dirent.d_name, dep->d_name);

				if (code != 0) 				/* Achou */
				{
					strcpy (dep->d_name, dirent.d_name);	/* Coloca o nome original */

					dep->d_ino = iep->ie_number; status = 1; goto out;
				}
			}
		}

		if (ia_vap)
		{
			if (type == NTFS_A_INDXROOT)
				dir_blkno = 0;
			else
				dir_blkno++;

			while (NTFS_CLUSTER_to_BYTE (dir_blkno * clus_per_dirblk) < ia_vap->va_datalen)
			{
				if (bitmap[dir_blkno >> 3] & (1 << (dir_blkno & 3)))
					break;

				dir_blkno++;
			}

			type = NTFS_A_INDX; offset = 0;

			if (NTFS_CLUSTER_to_BYTE (dir_blkno * clus_per_dirblk) >= ia_vap->va_datalen)
				break;
		}

	}
	while (ia_vap);

	status = 0;		/* NÃO achou */

	/*
	 *	Epílogo
	 */
    out:
	if (bitmap)
		free_byte (bitmap);

	if (area)
		free_byte (area);

	return (status);

}	/* end nt_search_dir */

/*
 ****************************************************************
 *	Fornece entradas do diretório para o usuário 		*
 ****************************************************************
 */
void
nt_read_dir (IOREQ *iop)
{
	INODE			*ip = iop->io_ip;
	const NTSB		*ntsp = ip->i_sb->sb_ptr;
	struct ntvattr		*ir_vap;		/* IndexRoot attribute */
	struct ntvattr		*bm_vap  = NULL;	/* BitMap attribute */
	struct ntvattr		*ia_vap  = NULL;	/* IndexAllocation attribute */
	char			*bitmap  = NULL;
	int			clus_per_dirblk = 1;	/* Clusters per directory block */
	int			dir_blkno;
	int			type;			/* Current attribute type */
	int			offset, entryno;
	int			size;			/* Length of data to read */
	int			dir_offset;
	struct attr_indexentry	*iep;
	char			*area = NOSTR;
	int			count;
	IOREQ			io;
	DIRENT			dirent;

	/*
	 *	Verifica se está no final do diretório
	 */
	if (ip->i_total_entrynos != 0 && iop->io_entryno >= ip->i_total_entrynos)
	{
#ifdef	DEBUG
		printf ("nt_read_dir: EOF\n");
#endif	DEBUG
		return;
	}

	/*
	 *	Verifica se foi pedida a entrada "."
	 */
	if (iop->io_entryno == 0)
	{
		dirent.d_offset	= 0;
		dirent.d_ino	= ip->i_ino;
		dirent.d_namlen	= 1;
		strcpy (dirent.d_name, ".");

		if (unimove (iop->io_area, &dirent, sizeof (DIRENT), iop->io_cpflag) < 0)
			{ u.u_error = EFAULT; return; }
#ifdef	DEBUG
		printf ("nt_read_dir: forneci \"%s\"\n", dirent.d_name);
#endif	DEBUG
		/* Avança os ponteiros */

		iop->io_area	+= sizeof (DIRENT);
		iop->io_count	-= sizeof (DIRENT);
		iop->io_entryno	+= 1;

		if (iop->io_count < sizeof (DIRENT))
			return;
	}

	/*
	 *	Verifica se foi pedida a entrada ".."
	 */
	if (iop->io_entryno == 1)
	{
		struct ntvattr	*vap;

		if ((vap = ntfs_ntvattrget (ntsp, ip, NTFS_A_NAME, NULL, 0, 0)) == NULL)
			return;

		dirent.d_offset	= 1;
		dirent.d_ino	= vap->va_a_name->n_pnumber;
		dirent.d_namlen	= 2;
		strcpy (dirent.d_name, "..");

		if (unimove (iop->io_area, &dirent, sizeof (DIRENT), iop->io_cpflag) < 0)
			{ u.u_error = EFAULT; return; }
#ifdef	DEBUG
		printf ("nt_read_dir: forneci \"%s\"\n", dirent.d_name);
#endif	DEBUG
		/* Avança os ponteiros */

		iop->io_area	+= sizeof (DIRENT);
		iop->io_count	-= sizeof (DIRENT);
		iop->io_entryno	+= 1;

		if (iop->io_count < sizeof (DIRENT))
			return;
	}

	/*
	 *		Prepara a leitura do diretório
	 *
	 * Read ntfs dir like stream of attr_indexentry, not like btree of them.
	 * This is done by scaning $BITMAP:$I30 for busy clusters and reading them.
	 * Of course, $INDEX_ROOT:$I30 is read before. Last read values are stored in
	 * INODE, so we can skip toward record number num almost immediatly.
	 * Anyway this is rather slow routine. The problem is that we don't know
	 * how many records are there in $INDEX_ALLOCATION:$I30 block.
	 */
	if ((ir_vap = ntfs_ntvattrget (ntsp, ip, NTFS_A_INDXROOT, "$I30", 0, 0)) == NULL)
		{ u.u_error = ENOTDIR; return; }
	
	count = ir_vap->va_a_iroot->ir_size;

	area  = malloc_byte (MAX (ir_vap->va_datalen, count));

	/*
	 *	Examina o modo de acesso ao diretório
	 */
	if (ir_vap->va_a_iroot->ir_flag & NTFS_IRFLAG_INDXALLOC)
	{
		if ((bm_vap = ntfs_ntvattrget (ntsp, ip, NTFS_A_INDXBITMAP, "$I30", 0, 0)) == NULL)
			goto out;

		bitmap = malloc_byte (bm_vap->va_datalen);

		io.io_ip	 = ip;
		io.io_area	 = bitmap;
		io.io_count	 = bm_vap->va_datalen;
		io.io_offset_low = 0;
		io.io_cpflag	 = SS;

		if (ntfs_readattr (ntsp, NTFS_A_INDXBITMAP, "$I30", &io) < 0)
			goto out;

		if ((ia_vap = ntfs_ntvattrget (ntsp, ip, NTFS_A_INDX, "$I30", 0, 0)) == NULL)
			goto out;

		clus_per_dirblk = (count + ntsp->s_CLUSTER_MASK) >> ntsp->s_CLUSTER_SHIFT;
	}
	else
	{
		ia_vap = bm_vap = NULL; bitmap = NULL;
	}

	/*
	 *	Tenta continuar com o final da última operação
	 */
	if (ip->i_last_entryno != 0 && ip->i_last_entryno < iop->io_entryno)
	{
#ifdef	DEBUG
		printf
		(	"nt_read_dir: continuando, i_last_entryno = %d, io_entryno = %d\n",
			ip->i_last_entryno, iop->io_entryno
		);
#endif	DEBUG
		type	  = ip->i_last_type;
		offset	  = ip->i_last_offset;
		dir_blkno = ip->i_last_dir_blkno;
		entryno	  = ip->i_last_entryno;
	}
	else
	{
		type	  = NTFS_A_INDXROOT;
		offset	  = sizeof (struct attr_indexroot);
		dir_blkno = 0;
		entryno	  = 2 - 1;			/* Pré-incrementado */
	}

	/*
	 *	Percorre os vários blocos do diretório
	 */
	do
	{
		if (iop->io_count < sizeof (DIRENT))
			goto count_eof;

		size = (type == NTFS_A_INDXROOT) ? ir_vap->va_datalen : count;

		dir_offset = NTFS_CLUSTER_to_BYTE (dir_blkno * clus_per_dirblk);

		io.io_ip	 = ip;
		io.io_area	 = area;
		io.io_count	 = size;
		io.io_offset_low = dir_offset;
		io.io_cpflag	 = SS;

		if (ntfs_readattr (ntsp, type, "$I30", &io) < 0)
			{ iop->io_eof = 1; goto out; }

		if (type == NTFS_A_INDX)
		{
			if (ntfs_procfixups (ntsp, NTFS_INDXMAGIC, area, size) < 0)
				goto out;
		}

		if (offset == 0)
		{
			if (type == NTFS_A_INDX)
				offset = (0x18 + ((struct attr_indexalloc *)area)->ia_hdrsize);
			else
				offset = sizeof (struct attr_indexroot);
		}

		/*
		 *	Percorre um bloco do diretório
		 */
		for
		(	iep = (struct attr_indexentry *)(area + offset);
			!(iep->ie_flag & NTFS_IEFLAG_LAST)  &&  offset < size;
			offset += iep->reclen, iep = (struct attr_indexentry *)(area + offset)
		)
		{
			if (iop->io_count < sizeof (DIRENT))
			{
			    count_eof:
				ip->i_last_entryno	= entryno;
				ip->i_last_offset	= offset;
				ip->i_last_dir_blkno	= dir_blkno;
				ip->i_last_type		= type;

				goto out;
			}

			if (iep->ie_fnametype == 2)
				continue;

			if (ip->i_ino == NTFS_ROOTINO)
			{ 
				if (iep->ie_fnm_len == 1 && iep->ie_fname[0] == '.')
					continue;

				if (iep->ie_fname[0] == '$')
					continue;
			}

			/* Achou um nome útil: verifica se já está na região pedida */
#ifdef	DEBUG
			printf ("nt_read_dir: Comparando %d com %d\n", entryno + 1, iop->io_entryno);
#endif	DEBUG
			if (++entryno < iop->io_entryno)
				continue;

			dirent.d_offset	= entryno;
			dirent.d_ino	= iep->ie_number;
			dirent.d_namlen	= iep->ie_fnm_len;
			little_unicode2iso (dirent.d_name, iep->ie_fname, iep->ie_fnm_len);

			if (unimove (iop->io_area, &dirent, sizeof (DIRENT), iop->io_cpflag) < 0)
				{ u.u_error = EFAULT; goto out; }
#ifdef	DEBUG
			printf ("nt_read_dir: forneci \"%s\"\n", dirent.d_name);
#endif	DEBUG
			/* Avança os ponteiros */

			iop->io_area	+= sizeof (DIRENT);
			iop->io_count	-= sizeof (DIRENT);
			iop->io_entryno += 1;

		}	/* end (entradas deste bloco */

		if (ia_vap)
		{
			if (type == NTFS_A_INDXROOT)
				dir_blkno = 0;
			else
				dir_blkno++;

			while (NTFS_CLUSTER_to_BYTE (dir_blkno * clus_per_dirblk) < ia_vap->va_datalen)
			{
				if (bitmap[dir_blkno >> 3] & (1 << (dir_blkno & 3)))
					break;

				dir_blkno++;
			}

			type = NTFS_A_INDX; offset = 0;

			if (NTFS_CLUSTER_to_BYTE (dir_blkno * clus_per_dirblk) >= ia_vap->va_datalen)
				{ iop->io_eof = 1; break; }
		}
	}
	while (ia_vap);

	/*
	 *	O diretório acabou
	 */
	ip->i_total_entrynos = entryno + 1;

#ifdef	DEBUG
	printf ("nt_read_dir: total de entradas = %d\n", ip->i_total_entrynos);
#endif	DEBUG

	/*
	 *	Epílogo
	 */
    out:
	if (bitmap)
		free_byte (bitmap);

	if (area)
		free_byte (area);
#ifdef	DEBUG
	printf
	(	"nt_read_dir: retornando com %d bytes (%d entradas)\n",
		old_count - iop->io_count, (old_count - iop->io_count) / sizeof (DIRENT)
	);
#endif	DEBUG

}	/* end nt_read_dir */

/*
 ****************************************************************
 *	Obtém o número de referências a este diretório		*
 ****************************************************************
 */
void
nt_get_nlink_dir (INODE *ip)
{
	const NTSB		*ntsp = ip->i_sb->sb_ptr;
	struct ntvattr		*ir_vap;		/* IndexRoot attribute */
	struct ntvattr		*bm_vap  = NULL;	/* BitMap attribute */
	struct ntvattr		*ia_vap  = NULL;	/* IndexAllocation attribute */
	char			*bitmap  = NULL;
	int			clus_per_dirblk = 1;	/* Clusters per directory block */
	int			dir_blkno;
	int			type;			/* Current attribute type */
	int			offset;
	int			size;			/* Length of data to read */
	int			dir_offset;
	struct attr_indexentry	*iep;
	char			*area = NOSTR;
	int			count;
	IOREQ			io;

	/*
	 *	Recebe e devolve o "ip" trancado
	 */
	ip->i_nlink = 2;

	/*
	 *		Prepara a leitura do diretório
	 *
	 * Read ntfs dir like stream of attr_indexentry, not like btree of them.
	 * This is done by scaning $BITMAP:$I30 for busy clusters and reading them.
	 * Of course, $INDEX_ROOT:$I30 is read before. Last read values are stored in
	 * INODE, so we can skip toward record number num almost immediatly.
	 * Anyway this is rather slow routine. The problem is that we don't know
	 * how many records are there in $INDEX_ALLOCATION:$I30 block.
	 */
	if ((ir_vap = ntfs_ntvattrget (ntsp, ip, NTFS_A_INDXROOT, "$I30", 0, 1)) == NULL)
		return;
	
	count = ir_vap->va_a_iroot->ir_size;

	area  = malloc_byte (MAX (ir_vap->va_datalen, count));

	/*
	 *	Examina o modo de acesso ao diretório
	 */
	if (ir_vap->va_a_iroot->ir_flag & NTFS_IRFLAG_INDXALLOC)
	{
		if ((bm_vap = ntfs_ntvattrget (ntsp, ip, NTFS_A_INDXBITMAP, "$I30", 0, 0)) == NULL)
			goto out;

		bitmap = malloc_byte (bm_vap->va_datalen);

		io.io_ip	 = ip;
		io.io_area	 = bitmap;
		io.io_count	 = bm_vap->va_datalen;
		io.io_offset_low = 0;
		io.io_cpflag	 = SS;

		if (ntfs_readattr (ntsp, NTFS_A_INDXBITMAP, "$I30", &io) < 0)
			goto out;

		if ((ia_vap = ntfs_ntvattrget (ntsp, ip, NTFS_A_INDX, "$I30", 0, 0)) == NULL)
			goto out;

		clus_per_dirblk = (count + ntsp->s_CLUSTER_MASK) >> ntsp->s_CLUSTER_SHIFT;
	}
	else
	{
		ia_vap = bm_vap = NULL; bitmap = NULL;
	}

	type	  = NTFS_A_INDXROOT;
	offset	  = sizeof (struct attr_indexroot);
	dir_blkno = 0;

	/*
	 *	Percorre os vários blocos do diretório
	 */
	do
	{
		size = (type == NTFS_A_INDXROOT) ? ir_vap->va_datalen : count;

		dir_offset = NTFS_CLUSTER_to_BYTE (dir_blkno * clus_per_dirblk);

		io.io_ip	 = ip;
		io.io_area	 = area;
		io.io_count	 = size;
		io.io_offset_low = dir_offset;
		io.io_cpflag	 = SS;

		if (ntfs_readattr (ntsp, type, "$I30", &io) < 0)
			goto out;

		if (type == NTFS_A_INDX)
		{
			if (ntfs_procfixups (ntsp, NTFS_INDXMAGIC, area, size) < 0)
				goto out;
		}

		if (offset == 0)
		{
			if (type == NTFS_A_INDX)
				offset = (0x18 + ((struct attr_indexalloc *)area)->ia_hdrsize);
			else
				offset = sizeof (struct attr_indexroot);
		}

		/*
		 *	Percorre um bloco do diretório
		 */
		for
		(	iep = (struct attr_indexentry *)(area + offset);
			!(iep->ie_flag & NTFS_IEFLAG_LAST)  &&  offset < size;
			offset += iep->reclen, iep = (struct attr_indexentry *)(area + offset)
		)
		{
			if (iep->ie_fnametype == 2)
				continue;

			if (ip->i_ino == NTFS_ROOTINO)
			{ 
				if (iep->ie_fnm_len == 1 && iep->ie_fname[0] == '.')
					continue;

				if (iep->ie_fname[0] == '$')
					continue;
			}

			if (iep->ie_fflag_low & NTFS_FFLAG_DIR)
				ip->i_nlink++;
		}

		if (ia_vap)
		{
			if (type == NTFS_A_INDXROOT)
				dir_blkno = 0;
			else
				dir_blkno++;

			while (NTFS_CLUSTER_to_BYTE (dir_blkno * clus_per_dirblk) < ia_vap->va_datalen)
			{
				if (bitmap[dir_blkno >> 3] & (1 << (dir_blkno & 3)))
					break;

				dir_blkno++;
			}

			type = NTFS_A_INDX; offset = 0;

			if (NTFS_CLUSTER_to_BYTE (dir_blkno * clus_per_dirblk) >= ia_vap->va_datalen)
				break;
		}

	}
	while (ia_vap);

	/*
	 *	Epílogo
	 */
    out:
	if (bitmap)
		free_byte (bitmap);

	if (area)
		free_byte (area);

}	/* end nt_get_nlink_dir */
