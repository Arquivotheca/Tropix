/*
 ****************************************************************
 *								*
 *			ntinode.c				*
 *								*
 *	Alocação e desalocação de INODEs no disco		*
 *								*
 *	Versão	4.3.0, de 29.06.04				*
 *		4.3.0, de 14.07.04				*
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
#include "../h/stat.h"
#include "../h/region.h"

#include "../h/inode.h"
#include "../h/bhead.h"
#include "../h/mntent.h"
#include "../h/sb.h"
#include "../h/nt.h"
#include "../h/sysdirent.h"
#include "../h/uerror.h"
#include "../h/signal.h"
#include "../h/uproc.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ******	Protótipos de funções ***********************************
 */
time_t		ntfs_nttimetounix (long long nt);
void		ntfs_free_ntvattr (struct ntvattr *vap);

/*
 ****************************************************************
 *	Lê e converte um INODE NTFS do disco para a memória	*
 ****************************************************************
 */
int
nt_read_inode (INODE *ip)
{
	const SB		*sp = ip->i_sb;
	NTSB			*ntsp = sp->sb_ptr;
	int			isdir;
	struct filerec		*mfrp;
	struct attr		*ap;
	struct ntvattr		*vap, *last_vap;
	const struct ntvattr	*data_vap = NULL, *name_vap = NULL;
	IOREQ			io;

	/*
	 *	Prólogo
	 */
#ifdef	DEBUG
	printf ("nt_read_inode: Lendo os atributos do INODE %d\n", ip->i_ino);
#endif	DEBUG

	mfrp = malloc_byte (ntsp->s_MFT_record_size);

	io.io_dev	= ip->i_dev;
	io.io_ip	= ntsp->s_mft_inode;
	io.io_area	= mfrp;
	io.io_count	= ntsp->s_MFT_record_size;
   /***	io.io_offset	= ...;	***/
	io.io_cpflag	= SS;

	/*
	 *	Verifica se é um INODE do sistema ou do usuário 
	 */
	if (ip->i_ino < NTFS_SYSNODESNUM)	/* Do Sistema */
	{
		ino_t		ino;

		/* Deselegância para evitar INO == 0 */

		if ((ino = ip->i_ino) == NTFS_PSEUDO_MFTINO)
			ino = NTFS_MFTINO;

		io.io_offset = ((long long)ntsp->s_MFT_cluster_no << ntsp->s_CLUSTER_SHIFT)
			     + ino * ntsp->s_MFT_record_size;

		if (ntfs_bread (&io) < 0)
			goto bad;
	}
	else					/* Do Usuário */
	{
		io.io_offset_low = ip->i_ino * ntsp->s_MFT_record_size;

		if (ntfs_readattr (ntsp, NTFS_A_DATA, NULL, &io) < 0)
			goto bad;
	}

	/*
	 *	Confere os números mágicos e os ajustes
	 */
	if (ntfs_procfixups (ntsp, NTFS_FILEMAGIC, mfrp, ntsp->s_MFT_record_size) < 0)
		goto bad;

	/*
	 *	Cria a lista de atributos
	 */
	ip->i_attr_list = NULL; last_vap = NULL;

	for
	(	ap = (struct attr *)((char *)mfrp + mfrp->fr_attroff);
		ap->a_hdr.a_type != -1;
		ap = (struct attr *)((char *)ap + ap->a_hdr.a_reclen)
	)
	{
		if ((vap = ntfs_attrtontvattr (ntsp, ap)) == NULL)
			goto bad;

		switch (vap->va_type)
		{
	       /*** case NTFS_A_STD:	***/
		    case NTFS_A_ATTRLIST:
		    case NTFS_A_VOLUMENAME:
		    case NTFS_A_INDXROOT:
		    case NTFS_A_INDX:
		    case NTFS_A_INDXBITMAP:
			break;

		    case NTFS_A_NAME:
			name_vap = vap;
			break;

		    case NTFS_A_DATA:
			data_vap = vap;
			break;

		    default:
#ifdef	DEBUG
			printf
			(	"nt_read_inode: Ignorando atributo %s (%d bytes), ino = %d\n",
				ntfs_edit_attrib_type (vap->va_type), ntfs_get_ntvattr_size (vap), ip->i_ino
			);
#endif	DEBUG
			ntfs_free_ntvattr (vap);
			continue;

		}	/* end switch */
#ifdef	DEBUG
		printf
		(	"nt_read_inode: Acrescentando atributo %s, ino = %d\n",
			ntfs_edit_attrib_type (vap->va_type), ip->i_ino
		);
#endif	DEBUG
		/* Insere na lista */

		if (last_vap == NULL)
			ip->i_attr_list = (int)vap;
		else
			last_vap->va_next = vap;

		last_vap = vap; vap->va_next = NULL;

	}	/* end for (lista de atributos) */

	/*
	 *	Processo o modo
	 */
	isdir = (mfrp->fr_flags & 2);

	if (isdir)
		ip->i_mode = S_IFDIR | S_IEXEC | (S_IEXEC >> IPSHIFT) | (S_IEXEC >> (2 * IPSHIFT));
	else
		ip->i_mode = S_IFREG | S_IEXEC;

	ip->i_mode |= S_IREAD | (S_IREAD >> IPSHIFT) | (S_IREAD >> (2 * IPSHIFT));

	/*
	 *	Copia diversos campos em comum
	 */
	ip->i_uid	= sp->sb_uid;
	ip->i_gid	= sp->sb_gid;

#if (0)	/*******************************************************/
	if ((ip->i_nlink = mfrp->fr_nlink) <= 0)
		ip->i_nlink = 1;
#endif	/*******************************************************/

	if (name_vap != NULL)
	{
		ip->i_atime	= ntfs_nttimetounix (name_vap->va_a_name->n_times.t_access);
		ip->i_mtime	= ntfs_nttimetounix (name_vap->va_a_name->n_times.t_write);
		ip->i_ctime	= ntfs_nttimetounix (name_vap->va_a_name->n_times.t_create);
	}

	/*
	 *	Inicializa a parte do diretório
	 */
	if (isdir)
	{
		ip->i_last_entryno	= 0;
		ip->i_last_type		= 0;
		ip->i_last_offset	= 0;
		ip->i_last_dir_blkno	= 0;
		ip->i_total_entrynos	= 0;
	}

	ip->i_fsp	= &fstab[FS_NT];
	ip->i_rdev	= 0;

	free_byte (mfrp);

	/*
	 *	Procura o atributo DATA, se for o caso ... para obter o tamanho
	 */
	ip->i_size = 0;		/* A princípio */

	if (!isdir)
	{
		if (data_vap == NULL)
		{
			if ((data_vap = ntfs_ntvattrget (ntsp, ip, NTFS_A_DATA, NOSTR, 0, 0)) == NULL)
			{
				printf ("%g: NÃO consegui achar os dados do INODE %d\n", ip->i_ino);
				return (0);
			}
		}

		ip->i_size  = data_vap->va_datalen;
		ip->i_nlink = 1;
	}
	else
	{
		nt_get_nlink_dir (ip);
	}

	/*
	 *	Epílogo
	 */
	return (0);

	/*
	 *	Em caso de erro, ...
	 */
   bad:
	printf ("%g: Erro na leitura do INODE (%v, %d)\n", ip->i_dev, ip->i_ino);

	free_byte (mfrp);

	return (-1);

}	/* end nt_read_inode */

/*
 ****************************************************************
 *	Conversão e Escrita do INODE do NTFS			*
 ****************************************************************
 */
void
nt_write_inode (INODE *ip)
{
	/*
	 *	Verifica se não houve modificação no INODE
	 *	ou entao ele está montado "Read only"
	 */
	if ((ip->i_flags & ICHG) == 0)
		return;

	inode_dirty_dec (ip);

	if (ip->i_sb->sb_mntent.me_flags & SB_RONLY)
		return;

}	/* end nt_write_inode */

/*
 ****************************************************************
 *	Libera todos os blocos de um arquivo			*
 ****************************************************************
 */
void
nt_trunc_inode (INODE *ip)
{

}	/* end nt_trunc_inode */

/*
 ****************************************************************
 *	Libera um INODE do Disco				*
 ****************************************************************
 */
void
nt_free_inode (const INODE *ip)
{
}	/* end nt_free_inode */

/*
 ****************************************************************
 *	Converte o formato do tempo de NTFS para UNIX		*
 ****************************************************************
 */
time_t
ntfs_nttimetounix (long long nt)
{
	time_t		t;

	/* WindowNT times are in 100 ns and from 1601 Jan 1 */

	t = nt / (1000 * 1000 * 10) -
		369LL * 365LL * 24LL * 60LL * 60LL -
		89LL * 1LL * 24LL * 60LL * 60LL;

	return (t);

}	/* end ntfs_nttimetounix */

/*
 ****************************************************************
 *	Libera a memória de todos atributos de um INODE		*
 ****************************************************************
 */
void
ntfs_free_all_ntvattr (INODE *ip)
{
	struct ntvattr	*vap, *next_vap;

#ifdef	DEBUG
	printf ("%g: Liberando atributos do INODE (%v, %d)\n", ip->i_sb->sb_dev, ip->i_ino);
#endif	DEBUG

	if (ip->i_sb->sb_code != FS_NT)
	{
		printf ("%g: INODE (%v, %d) de dispositivo NÃO NTFS\n", ip->i_sb->sb_dev, ip->i_ino);
		return;
	}

	for (vap = (struct ntvattr *)ip->i_attr_list; vap != NULL; vap = next_vap)
		{ next_vap = vap->va_next; ntfs_free_ntvattr (vap); }

	ip->i_attr_list = NULL;

}	/* end ntfs_free_all_ntvattr */

/*
 ****************************************************************
 *	Libera a memória de uma	"struct ntvattr"		*
 ****************************************************************
 */
void
ntfs_free_ntvattr (struct ntvattr *vap)
{
	if (vap->va_flag & NTFS_AF_INRUN)
	{
		free_byte (vap->va_vruncn);
		free_byte (vap->va_vruncl);
	}
	else
	{
		free_byte (vap->va_datap);
	}

	free_byte (vap);

}	/* end ntfs_free_ntvattr */
