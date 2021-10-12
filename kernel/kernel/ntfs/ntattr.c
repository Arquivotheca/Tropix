/*
 ****************************************************************
 *								*
 *			ntattr.c				*
 *								*
 *	Leitura e manipulação dos atributos			*
 *								*
 *	Versão	4.6.0, de 28.06.04				*
 *		4.6.0, de 09.07.04				*
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
 ******	Definições globais **************************************
 */
#define GET_UINT16(addr)	(*((ushort *)(addr)))

/*
 ******	Protótipos de funções ***********************************
 */
int		ntfs_readattr_plain (const NTSB *ntsp, int type, const char *nm, IOREQ *iop);
struct ntvattr	*ntfs_findvattr (const NTSB *ntsp, INODE *ip, struct ntvattr **lvapp,
			int type, const char *name, int nm_len, clusno_t vcn);
struct ntvattr	*ntfs_copy_ntvattr (const struct ntvattr *vap);
int		ntfs_get_ntvattr_size (const struct ntvattr *vap);
int		ntfs_runtovrun (struct ntvattr *vap, const char *run);
int		ntfs_uncompblock (char *dst, const char *src);
const char	*ntfs_edit_attrib_type (int type);

/*
 ****************************************************************
 *	Lê um atributo, de descomprime-o, se for o caso		*
 ****************************************************************
 */
int
ntfs_readattr (const NTSB *ntsp, int type, const char *nm, IOREQ *iop)
{
	INODE		*ip = iop->io_ip;
	struct ntvattr	*vap;
	char		*cup = NOSTR, *uup = NOSTR;
	off_t		offset, base;
	int		left, copied, count;
	int		total_count = 0;
	IOREQ		io;

#undef	DEBUG
#ifdef	DEBUG
	printf
	(	"ntfs_readattr: reading INO %d: %s, from %d size %d bytes\n",
		ip->i_ino, ntfs_edit_attrib_type (type), iop->io_offset_low, iop->io_count
	);
#endif	DEBUG

	/*
	 *	Retorna o número total de bytes copiados
	 *
	 *	Lê o atributo e converte-o para a forma interna
	 */
	if ((vap = ntfs_ntvattrget (ntsp, ip, type, nm, 0, 0)) == NULL)
		return (-1);

	if (iop->io_offset_low + iop->io_count > vap->va_datalen)
	{
		printf
		(	"%g: Deslocamento além do atributo (%d :: %d)\n",
			iop->io_offset_low + iop->io_count, vap->va_datalen
		);
		u.u_error = EBADFMT; return (-1);
	}

	/*
	 *	Se NÃO estiver comprimido, ...
	 */
	if (vap->va_compression == 0 || vap->va_compressalg == 0)
		return (ntfs_readattr_plain (ntsp, type, nm, iop));

	/*
	 *	Está comprimido
	 */
	if ((cup = malloc_byte (ntsp->s_COMPRESS_BLOCK_SIZE)) == NULL)
		{ u.u_error = ENOMEM; return (-1); }

	if ((uup = malloc_byte (ntsp->s_COMPRESS_BLOCK_SIZE)) == NULL)
		{ u.u_error = ENOMEM; goto bad; }

	base	= iop->io_offset_low & (~NTFS_COMPRESS_CLUSTER_MASK << ntsp->s_CLUSTER_SHIFT);
	offset	= iop->io_offset_low - base;

	for (left = iop->io_count; left > 0; /* abaixo */)
	{
		io.io_ip	 = ip;
		io.io_area	 = cup;
		io.io_count	 = ntsp->s_COMPRESS_BLOCK_SIZE;
		io.io_offset_low = base;
		io.io_cpflag	 = SS;

		if ((copied = ntfs_readattr_plain (ntsp, type, nm, &io)) < 0)
			goto bad;

		if ((count = ntsp->s_COMPRESS_BLOCK_SIZE - offset) > left)
			count = left;

		if   (copied == ntsp->s_COMPRESS_BLOCK_SIZE)
		{
			if (unimove (iop->io_area, cup + offset, count, iop->io_cpflag) < 0)
				{ u.u_error = EFAULT; goto bad; }
		}
		elif (copied == 0)
		{
			memclr (uup, count);

			if (unimove (iop->io_area, uup, count, iop->io_cpflag) < 0)
				{ u.u_error = EFAULT; goto bad; }
		}
		else
		{
			int		i, coff;

			ip->i_sb->sb_sub_code = 1;		/* Usando compressão */

			for (i = 0, coff = 0; i * NTFS_COMPBLOCK_SIZE < ntsp->s_COMPRESS_BLOCK_SIZE; i++)
				coff += ntfs_uncompblock (uup + i * NTFS_COMPBLOCK_SIZE, cup + coff);

			if (unimove (iop->io_area, uup + offset, count, iop->io_cpflag) < 0)
				{ u.u_error = EFAULT; goto bad; }
		}

		left		-= count;
		iop->io_area	+= count;
		total_count	+= count;

		offset	+= count - ntsp->s_COMPRESS_BLOCK_SIZE;
		base	+= ntsp->s_COMPRESS_BLOCK_SIZE;
	}

	free_byte (cup);
	free_byte (uup);

	return (total_count);

	/*
	 *	Em caso de erro, ...
	 */
    bad:
	if (cup != NOSTR)
		free_byte (cup);

	if (uup != NOSTR)
		free_byte (uup);

	return (-1);

}	/* end ntfs_readattr */

/*
 ****************************************************************
 *  Procura um atributo na lista, ou usa o elo ATTR_A_ATTRLIST	*
 ****************************************************************
 */
struct ntvattr *
ntfs_ntvattrget (const NTSB *ntsp, INODE *ip, int type, const char *name, clusno_t vcn, int optional_attr)
{
	struct ntvattr		*vap, *lvap = NULL, *new_vap;
	int			nm_len, len;
	struct attr_attrlist	*alpool = NULL;
	struct attr_attrlist	*nextaalp, *aalp;
	INODE			*newip;
	IOREQ			io;
	char			nm[16];		/* Supondo nomes de atributos curtos */

	/*
	 *	Prólogo
	 */
	if (name == NOSTR)
		name = "";

	nm_len = strlen (name);

	if (nm_len > sizeof (nm))
	{
		printf ("ntfs_ntvattrget: Nome do atributo \"%s\" muito longo\n", name);
		u.u_error = EBADFMT; return (NULL);
	}

#ifdef	DEBUG
	printf
	(	"ntfs_ntvattrget: Início; INODE = %d, type = %s, name = \"%s\", vcn = %d\n",
		ip->i_ino, ntfs_edit_attrib_type (type), name, vcn
	);
#endif	DEBUG

	/*
	 *	Tenta achar o atributo no INODE dado
	 */
	if ((vap = ntfs_findvattr (ntsp, ip, &lvap, type, name, nm_len, vcn)) != NULL)
		return (vap);

	if (lvap == NULL)
		goto bad;

	/*
	 *	Percorre $ATTRIBUTE_LIST procurando o atributo pedido
	 */
	if ((alpool = malloc_byte (lvap->va_datalen)) == NULL)
		{ u.u_error = ENOMEM; return (NULL); }

	io.io_dev	 = ip->i_dev;
	io.io_area	 = alpool;
	io.io_count	 = lvap->va_datalen;
	io.io_offset_low = 0;
	io.io_cpflag	 = SS;

	if ((len = ntfs_readntvattr_plain (ntsp, lvap, &io)) < 0)
		goto bad;

	/*
	 *	Procura o atributo desejado na lista de atributos de outros INODEs
	 */
	for (aalp = alpool; aalp != NULL; aalp = nextaalp)
	{
#ifdef	DEBUG
		printf
		(	"ntfs_ntvattrget: attrlist: ino: %d, attr: 0x%x, vcn: %d\n",
			aalp->al_inumber, aalp->al_type, aalp->al_vcnstart
		);
#endif	DEBUG
		if (len > aalp->reclen)
			nextaalp = (struct attr_attrlist *)((int)aalp + aalp->reclen);
		else
			nextaalp = NULL;

		len -= aalp->reclen;

		/*
		 *	Compara o nome atual
		 */
		if (aalp->al_type != type || aalp->al_nm_len != nm_len)
			continue;

		little_unicode2iso (nm, aalp->al_name, nm_len);

		if (!memeq (nm, name, nm_len))
			continue;

		/*
		 *	Compara o nome seguinte
		 */
		if (nextaalp != NULL  &&  nextaalp->al_vcnstart <= vcn)
		{
			if (nextaalp->al_type == type && nextaalp->al_nm_len == nm_len)
			{
				little_unicode2iso (nm, nextaalp->al_name, nm_len);

				if (memeq (nm, name, nm_len))
					continue;
			}
		}

		/*
		 *	Consulta o novo INODE
		 */
#ifdef	DEBUG
		printf
		(	"ntfs_ntvattrget: Procurando o atributo %s do INODE %d em %d\n",
			ntfs_edit_attrib_type (type), ip->i_ino, aalp->al_inumber
		);
#endif	DEBUG
		if ((newip = iget (ip->i_dev, aalp->al_inumber, 0)) == NOINODE)
			goto bad;

		if ((vap = ntfs_findvattr (ntsp, newip, &lvap, type, name, nm_len, vcn)) == NULL)
			{ iput (newip); goto bad; }
#ifdef	DEBUG
		printf
		(	"ntfs_ntvattrget: Achei o atributo %s do INODE %d em %d\n",
			ntfs_edit_attrib_type (type), ip->i_ino, aalp->al_inumber
		);
#endif	DEBUG
		free_byte (alpool);

		/* Insere uma cópia no INODE dado */

		if ((new_vap = ntfs_copy_ntvattr (vap)) == NULL)
			{ iput (newip); goto bad; }

		iput (newip);

		new_vap->va_next = (struct ntvattr *)ip->i_attr_list;
		ip->i_attr_list	 = (int)new_vap;

		return (new_vap);		/* Devolve a cópia */
	}

	/*
	 *	Em caso de erro, ...
	 */
    bad:
	if (alpool != NULL)
		free_byte (alpool);

	/* Se for um atributo não indispensável, NÃO considera erro */

	if (optional_attr)
		return (NULL);

	printf
	(	"%g: NÃO encontrei o atributo: ino: %d, type: %s, name: %s, vcn: %d\n",
		ip->i_ino, ntfs_edit_attrib_type (type), name, vcn
	);

	u.u_error = EBADFMT; return (NULL);

}	/* end ntfs_ntvattrget */

/*
 ****************************************************************
 *	Procura um atributo em um INODE				*
 ****************************************************************
 */
struct ntvattr *
ntfs_findvattr (const NTSB *ntsp, INODE *ip, struct ntvattr **lvapp, int type, const char *name,
		int nm_len, clusno_t vcn)
{
	struct ntvattr	*vap;

	/*
	 *	Procura o atributo desejado na lista
	 */
	for (*lvapp = NULL, vap = (struct ntvattr *)ip->i_attr_list; vap != NULL; vap = vap->va_next)
	{
#ifdef	DEBUG
		printf
		(	"ntfs_findvattr: type: %s, vcn: %d :: %d\n",
			ntfs_edit_attrib_type (vap->va_type),
			vap->va_vcnstart, vap->va_vcnend
		);
#endif	DEBUG

		if
		(	vap->va_type == type &&
			vap->va_vcnstart <= vcn && vap->va_vcnend >= vcn &&
			vap->va_nm_len == nm_len &&
			memeq (name, vap->va_name, nm_len)
		)
		{
			return (vap);
		}

		if (vap->va_type == NTFS_A_ATTRLIST)
			*lvapp = vap;
	}

	return (NULL);

}	/* end ntfs_findvattr */

/*
 ****************************************************************
 *	Lê o atributo						*
 ****************************************************************
 */
int
ntfs_readattr_plain (const NTSB *ntsp, int type, const char *nm, IOREQ *iop)
{
	INODE		*ip = iop->io_ip;
	int		count, read, total_count = 0;
	struct ntvattr	*vap;
	off_t		offset_begin, offset_end;
	clusno_t	cn;
	IOREQ		io;

	/*
	 *	Retorna o número total de bytes copiados
	 */
	while (iop->io_count > 0)
	{
		cn = iop->io_offset_low >> ntsp->s_CLUSTER_SHIFT;

		if ((vap = ntfs_ntvattrget (ntsp, ip, type, nm, cn, 0)) == NULL)
			return (-1);

		offset_begin =  (vap->va_vcnstart    << ntsp->s_CLUSTER_SHIFT);
		offset_end   = ((vap->va_vcnend + 1) << ntsp->s_CLUSTER_SHIFT);

		if ((count = offset_end - iop->io_offset_low) > iop->io_count)
			count = iop->io_count;
#ifdef	DEBUG
		printf
		(	"ntfs_readattr_plain: o: %d, s: %d (%d - %d)\n",
			iop->io_offset_low, count, vap->va_vcnstart, vap->va_vcnend
		);
#endif	DEBUG
		io.io_dev	 = ip->i_dev;
		io.io_area	 = iop->io_area;
		io.io_count	 = count;
		io.io_offset_low = iop->io_offset_low - offset_begin;
		io.io_cpflag	 = iop->io_cpflag;

		if ((read = ntfs_readntvattr_plain (ntsp, vap, &io)) < 0)
			return (-1);

		iop->io_area	   += count;
		iop->io_offset_low += count;
		iop->io_count	   -= count;

		total_count	   += read;
	}

	return (total_count);

}	/* end ntfs_readattr_plain */

/*
 ****************************************************************
 *	Copia um atributo do MFT ou de um RUN (do disco)	*
 ****************************************************************
 */
int
ntfs_readntvattr_plain (const NTSB *ntsp, const struct ntvattr *vap, IOREQ *iop)
{
	int		run_index;
	off_t		run_offset;
	int		run_count;
	daddr_t		run_blkno;
	int		total_count = 0, count, range;

	/*
	 *	Retorna o número total de bytes copiados
	 *
	 *	Se o atributo já está na memória, ...
	 */
	if ((vap->va_flag & NTFS_AF_INRUN) == 0)
	{
		if (unimove (iop->io_area, vap->va_datap + iop->io_offset_low, iop->io_count, iop->io_cpflag) < 0)
			{ u.u_error = EFAULT; return (-1); }

		total_count	    = iop->io_count;

	   /***	iop->io_area	   += iop->io_count;	***/
		iop->io_count	    = 0;
	   /***	iop->io_offset_low += iop->io_count;	***/

		return (total_count);
	}

	/*
	 *	Le o atributo do disco, percorrendo os vários RUNs
	 */
	for
	(	run_index = 0,		run_offset = 0;
		/* Veja abaixo */	iop->io_count > 0;
		run_index++,		run_offset += run_count
	)
	{
		if (run_index >= vap->va_vruncnt)
		{
			printf ("ntfs_readntvattr_plain: Tentando ler após o final do atributo do disco\n");
			u.u_error = EBADFMT; return (-1);
		}

	   /***	run_offset = ...;	***/
		run_count  = vap->va_vruncl[run_index] <<  ntsp->s_CLUSTER_SHIFT;
		run_blkno  = vap->va_vruncn[run_index] << (ntsp->s_CLUSTER_SHIFT - BLSHIFT);

		/*
		 *	Pula este RUN, se NÃO tem informação desejada
		 */
		if ((range = run_offset + run_count - iop->io_offset_low) <= 0)
			continue;

		if ((count = iop->io_count) > range)
			count =  range;

		/*
		 *	Percorre os vários blocos do CACHE relativos a este RUN
		 */
		if (run_blkno)
		{
			IOREQ		io;

			io.io_dev	= iop->io_dev;
			io.io_area	= iop->io_area;
			io.io_count	= count;
			io.io_offset	= ((long long)run_blkno << BLSHIFT) + (iop->io_offset_low - run_offset);
			io.io_cpflag	= iop->io_cpflag;

			if (ntfs_bread (&io) < 0)
				return (-1);

			total_count	+= count;
		}
		else	/* Este RUN não tem CLUSTERs no disco (um buraco) */
		{
			if (iop->io_cpflag != SS)
			{
				if (pubyte (iop->io_area, 0) < 0 || pubyte (iop->io_area + count - 1, 0) < 0)
					{ u.u_error = EFAULT; return (-1); }
			}

			memclr (iop->io_area, count);

		   /***	total_count	+= count;	***/		/* Áreas NULAs NÃO contam! */
		}

		iop->io_area	   += count;
		iop->io_offset_low += count;
		iop->io_count	   -= count;

	}	/* end while (Percorrendo os RUNs) */

	return (total_count);

}	/* end ntfs_readntvattr_plain */

/*
 ****************************************************************
 *	Converte um atributo do disco para a memória		*
 ****************************************************************
 */
struct ntvattr *
ntfs_attrtontvattr (const NTSB *ntsp, const struct attr *rap)
{
	struct ntvattr	*vap;
#ifdef	DEBUG
	int		i;
#endif	DEBUG

	/*
	 *	Campos comuns
	 */
	if ((vap = malloc_byte (sizeof (struct ntvattr))) == NULL)
		{ u.u_error = ENOMEM; return (NULL); }

	vap->va_flag		= rap->a_hdr.a_flag;
	vap->va_type		= rap->a_hdr.a_type;
	vap->va_compression	= rap->a_hdr.a_compression;
	vap->va_index		= rap->a_hdr.a_index;
	vap->va_nm_len		= rap->a_hdr.a_nm_len;

	/*
	 *	Converte o nome
	 */
	if (rap->a_hdr.a_nm_len)
		little_unicode2iso (vap->va_name, (char *)rap + rap->a_hdr.a_nameoff, vap->va_nm_len);

	/*
	 *	Verifica se o atributo é residente ou não
	 */
	if (vap->va_flag & NTFS_AF_INRUN)
	{
		vap->va_datalen		= rap->a_nr.a_datalen;
		vap->va_allocated	= rap->a_nr.a_allocated;
		vap->va_vcnstart	= rap->a_nr.a_vcnstart;
		vap->va_vcnend		= rap->a_nr.a_vcnend;
		vap->va_compressalg	= rap->a_nr.a_compressalg;

		if (ntfs_runtovrun (vap, (char *)rap + rap->a_nr.a_dataoff) < 0)
			{ free_byte (vap); return (NULL); }
#ifdef	DEBUG
		printf ("RUN (%s): cnt = %d", ntfs_edit_attrib_type (vap->va_type), vap->va_vruncnt);

		for (i = 0; i < vap->va_vruncnt; i++)
			printf (" (%d, %d)", vap->va_vruncn[i], vap->va_vruncl[i]);

		printf ("\n");
#endif	DEBUG
	}
	else
	{
		vap->va_compressalg	= 0;
		vap->va_datalen		=
		vap->va_allocated	= rap->a_r.a_datalen_long;
		vap->va_vcnstart	= 0;
		vap->va_vcnend		= vap->va_allocated >> ntsp->s_CLUSTER_SHIFT;
		vap->va_datap		= malloc_byte (vap->va_datalen);

		if (vap->va_datap == NULL)
			{ free_byte (vap); u.u_error = ENOMEM; return (NULL); }

		memmove (vap->va_datap, (char *)rap + rap->a_r.a_dataoff, rap->a_r.a_datalen_long);
	}

	return (vap);

}	/* end ntfs_attrtontvattr */

/*
 ****************************************************************
 *	Copia um atributo no formato interno			*
 ****************************************************************
 */
struct ntvattr *
ntfs_copy_ntvattr (const struct ntvattr *vap)
{
	struct ntvattr	*new_vap;
	int		size;

	if ((new_vap = malloc_byte (sizeof (struct ntvattr))) == NULL)
		{ u.u_error = ENOMEM; return (NULL); }

	memmove (new_vap, vap, sizeof (struct ntvattr));

	if (vap->va_flag & NTFS_AF_INRUN)
	{
		size = vap->va_vruncnt * sizeof (clusno_t);

		if ((new_vap->va_vruncn = malloc_byte (size)) == NULL)
			{ free_byte (new_vap); u.u_error = ENOMEM; return (NULL); }

		if ((new_vap->va_vruncl = malloc_byte (size)) == NULL)
			{ free_byte (new_vap->va_vruncn); free_byte (new_vap); u.u_error = ENOMEM; return (NULL); }

		memmove (new_vap->va_vruncn, vap->va_vruncn, size);
		memmove (new_vap->va_vruncl, vap->va_vruncl, size);
	}
	else
	{
		if ((new_vap->va_datap = malloc_byte (vap->va_datalen)) == NULL)
			{ free_byte (new_vap); u.u_error = ENOMEM; return (NULL); }

		memmove (new_vap->va_datap, vap->va_datap, vap->va_datalen);
	}

	return (new_vap);

}	/* end ntfs_copy_ntvattr */

#ifdef	DEBUG
/*
 ****************************************************************
 *	Calcula o tamanho de um atributo interno		*
 ****************************************************************
 */
int
ntfs_get_ntvattr_size (const struct ntvattr *vap)
{
	int		size;

	size = sizeof (struct ntvattr);

	if (vap->va_flag & NTFS_AF_INRUN)
		size += 2 * vap->va_vruncnt * sizeof (clusno_t);
	else
		size += vap->va_datalen;

	return (size);

}	/* ntfs_get_ntvattr_size */
#endif	DEBUG

/*
 ****************************************************************
 *	Descomprime os RUNs					*
 ****************************************************************
 */
int
ntfs_runtovrun (struct ntvattr *vap, const char *run)
{
	off_t		offset;
	int		i, sz, cnt;
	clusno_t	*cn, *cl;
	clusno_t	prev, tmp;

	/*
	 *	Conta o tamanho do RUN e aloca memória
	 */
	for (offset = cnt = 0; (i = run[offset]) != 0; cnt++)
		offset += (i & 0x0F) + (i >> 4) + 1;

	if ((vap->va_vruncn = cn = malloc_byte (cnt * sizeof (clusno_t))) == NULL)
		{ u.u_error = ENOMEM; return (-1); }

	if ((vap->va_vruncl = cl = malloc_byte (cnt * sizeof (clusno_t))) == NULL)
		{ free_byte (vap->va_vruncn); u.u_error = ENOMEM; return (-1); }

	vap->va_vruncnt	= cnt;

	/*
	 *	Restaura os valores dos endereços e tamanhos
	 */
	for (offset = cnt = prev = 0; (sz = run[offset++]) != 0; cnt++)
	{
		cl[cnt] = 0;

		for (i = 0; i < (sz & 0x0F); i++)
			cl[cnt] |= run[offset++] << (i << 3);

		sz >>= 4;

		if (run[offset + sz - 1] & 0x80)
			tmp = -1 << (sz << 3);
		else
			tmp = 0;

		for (i = 0; i < sz; i++)
			tmp |= run[offset++] << (i << 3);

		if (tmp)
			prev = cn[cnt] = prev + tmp;
		else
			cn[cnt] = tmp;
	}

	return (0);

}	/* end ntfs_runtovrun */

/*
 ****************************************************************
 *	Confere os números mágicos e efetua os ajustes		*
 ****************************************************************
 *
 *	All important structures in NTFS use 2 consistency checks:
 *	. a magic structure identifier (FILE, INDX, RSTR, RCRD...)
 *	. a fixup technique : the last word of each sector (called
 *	  a fixup) of a structure's record should end with the word
 *	  at offset <n> of the first sector, and if it is the case,
 *	  must be replaced with the words following <n>. The value of
 *	  <n> and the number of fixups is taken from the fields at
 *	  the offsets 4 and 6. Note that the sector size is defined
 *	  as NTFS_SECTOR_SIZE and not as the hardware sector size
 *	  (this is concordant with what the Windows NTFS driver does). 
 */
int
ntfs_procfixups (const NTSB *ntsp, ulong magic, void *area, int count)
{
	struct fixuphdr	*fhp = area;
	int	 	i;
	ushort		fixup, *fxp, *cfxp;

	/*
	 *	Confere a mágica
	 */
	if (fhp->fh_magic != magic)
	{
		printf ("ntfs_procfixups: Número mágico NÃO confere  (%P :: %P)\n", fhp->fh_magic, magic);
		u.u_error = EBADFMT; return (-1);
	}

	/*
	 *	Confere o tamanho
	 */
	if ((fhp->fh_fnum - 1) * ntsp->s_bytes_per_sector != count)
	{
		printf
		(	"ntfs_procfixups: "
	 		"Tamanho %d NÃO confere para bloco de tamanho %d\n", count, fhp->fh_fnum
		);
		u.u_error = EBADFMT; return (-1);
	}

	/*
	 *	Confere o deslocamento
	 */
	if (fhp->fh_foff >= ntsp->s_MFT_record_size)
	{
		printf
		(	"ntfs_procfixups: Deslocamento excessivo (%d :: %d)\n",
			fhp->fh_foff, ntsp->s_MFT_record_size
		);
		u.u_error = EBADFMT; return (-1);
	}

	/*
	 *	Executa o ajustes
	 */
	fxp	= (ushort *)(area + fhp->fh_foff);
	cfxp	= (ushort *)(area + ntsp->s_bytes_per_sector - 2);
	fixup	= *fxp++;

	for (i = 1; i < fhp->fh_fnum; i++, fxp++)
	{
		if (*cfxp != fixup)
		{
			printf ("ntfs_procfixups: Ajuste %d NÃO confere\n", i);
			u.u_error = EBADFMT; return (-1);
		}

		*cfxp = *fxp; ((char *)cfxp) += ntsp->s_bytes_per_sector;
	}

	return (0);

}	/* end ntfs_procfixups */

/*
 ****************************************************************
 *	Descomprime um bloco compactado				*
 ****************************************************************
 */
int
ntfs_uncompblock (char *dst, const char *src)
{
	int		head, tag, len, shift, mask;
	int		code, blen, delta, i, j, pos, cpos;

	/*
	 *	Verifica se o bloco está efetivamente comprimido
	 */
	len = ((head = GET_UINT16 (src)) & 0x0FFF) + 3;

	if ((head & 0x8000) == 0)
	{
		memmove (dst, src + 2, len - 2);
		memclr (dst + len - 2, NTFS_COMPBLOCK_SIZE + 2 - len);
		return (len);
	}

	/*
	 *	Está efetivamente comprimido; agora examina cada 8 bytes
	 */
	cpos = 2; pos = 0;

	while ((cpos < len) && (pos < NTFS_COMPBLOCK_SIZE))
	{
		tag = src[cpos++];

		for (i = 0; i < 8  &&  pos < NTFS_COMPBLOCK_SIZE; i++)
		{
			if (tag & 1)
			{
				for (j = pos - 1, mask = 0x0FFF, shift = 12; j >= 0x10; j >>= 1)
					{ shift--; mask >>= 1; }

				code  = GET_UINT16 (src + cpos);
				delta = (code >> shift) + 1;
				blen  = (code &  mask)  + 3;

				for (j = 0; j < blen && pos < NTFS_COMPBLOCK_SIZE; j++)
					{ dst[pos] = dst[pos - delta]; pos++; }

				cpos += 2;
			}
			else
			{
				dst[pos++] = src[cpos++];
			}

			tag >>= 1;
		}
	}

	return (len);

}	/* end ntfs_uncompblock */

/*
 ****************************************************************
 *	Edita um tipo de atributo				*
 ****************************************************************
 */
const char	* const attrtb[] = 
{
	"?", 		/* 0x00 */
	"STD", 		/* 0x10 */
	"LIST", 	/* 0x20 */
	"NM", 		/* 0x30 */
	"?", 		/* 0x40 */
	"?", 		/* 0x50 */
	"VOL", 		/* 0x60 */
	"?", 		/* 0x70 */
	"DATA", 	/* 0x80 */
	"INDEXROOT",	/* 0x90 */
	"INDEX", 	/* 0xA0 */
	"INDEXBITMAP", 	/* 0xB0 */
	"?", 		/* 0xC0 */
	"?", 		/* 0xD0 */
	"?", 		/* 0xE0 */
	"?", 		/* 0xF0 */
};

const char *
ntfs_edit_attrib_type (int type)
{
	static char	other[16];
	const char	*str;

	if (type & ~0xF0)
		{ sprintf (other, "%P", type); return (other); }

	str = attrtb[type >> 4];

	if (str[0] == '?')
		{ sprintf (other, "0x%02X", type); return (other); }

	return (attrtb[type >> 4]);

}	/* end ntfs_edit_atrrib_typ */
