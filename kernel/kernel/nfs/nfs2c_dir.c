/*
 ****************************************************************
 *								*
 *			nfs2c_dir.c				*
 *								*
 *	Tratamento de diretórios do NFS				*
 *								*
 *	Versão	4.8.0, de 07.10.05				*
 *		4.8.0, de 07.10.05				*
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
#include "../h/scb.h"
#include "../h/sync.h"
#include "../h/region.h"

#include "../h/mntent.h"
#include "../h/sb.h"
#include "../h/itnet.h"
#include "../h/inode.h"
#include "../h/sysdirent.h"
#include "../h/nfs.h"
#include "../h/signal.h"
#include "../h/uproc.h"
#include "../h/uerror.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ******	Protótipos de funções ***********************************
 */
int		nfs2_search_dir_by_ino (IOREQ *iop, DIRENT *dep);

/*
 ****************************************************************
 *	Procura um nome dado no diretório			*
 ****************************************************************
 */
int
nfs2_search_dir (IOREQ *iop, DIRENT *dep, int mission)
{
	INODE		*ip = iop->io_ip, *son_ip;
	const SB	*sp = ip->i_sb;
	NFSSB		*nfssp = sp->sb_ptr;
	struct arg { FHANDLE handle; int name[1]; }				*rp;
	const struct reply { int status; FHANDLE fhandle; FATTR fattr; }	*reply;
	struct u_nfs	*up = &u.u_myself->u_nfs;
	NFSDATA		*dp;
	ITBLOCK		*bp;
	int		*ptr;
	UDP_EP		uep;
	FATTR		fattr;

	/*
	 *	Recebe e devolve o "ip" trancado
	 *
	 *	Se encontrou a entrada, devolve "1":
	 *		"dep->d_ino"			     o INO  (caso normal)
	 *		"dep->d_namlen" e "dep->d_name"	     o nome (caso INUMBER)
	 *
	 *	Se NÃO encontrou a entrada, devolve "0":
	 *		NADA
	 */
	if (sp->sb_mntent.me_flags & SB_ATIME)
		{ ip->i_atime = time; inode_dirty_inc (ip); }

	/*
	 *	Verifica se é INUMBER
	 */
	if (mission & INUMBER)
		return (nfs2_search_dir_by_ino (iop, dep));

	/*
	 *	Verifica se está procurando por "." ou ".."
	 */
	if (dep->d_name[0] == '.')
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
	 *	procurando por outros nomes
	 */
#ifdef	DEBUG
printf ("%g: Procurando entrada \"%s\" no INO %d, usando LOOKUP\n", dep->d_name, ip->i_ino);
#endif	DEBUG

	up->nfs_xid = (nfs_req_source++ << XID_PROC_SHIFT) | (u.u_uprocv - scb.y_uproc);

	up->nfs_timeout	= sp->sb_mntent.me_timeout;
	up->nfs_transmitted = 0;

    get_lookup_again:
	rp = nfs_setup_client_datagram_prolog (nfssp, &uep, NFSPROC_LOOKUP, &bp);

	memmove (&rp->handle, ip->i_handle, sizeof (FHANDLE));

	ptr = nfs_put_xdr_name (rp->name, dep->d_name, dep->d_namlen);

   /***	bp->it_u_area   = ...;	***/
	bp->it_u_count  = (int)ptr - (int)bp->it_u_area;

	EVENTCLEAR (&up->nfs_event);

	up->nfs_sp = nfssp;
	CLEAR (&up->nfs_lock);
	up->nfs_transmitted++;
	up->nfs_last_snd_time = time;

	send_udp_datagram (&uep, bp);

	EVENTWAIT (&up->nfs_event, PITNETIN);

	if   (up->nfs_status == 0)
		goto get_lookup_again;
	elif (up->nfs_status < 0)
		{ u.u_error = up->nfs_error; return (-1); }

	/*
	 *	Analisa a resposta
	 */
	reply = up->nfs_area;

	if ((u.u_error = ENDIAN_LONG (reply->status)) != NFS_OK)
	{
		put_it_block (up->nfs_bp);

		if (u.u_error == ENOENT)
			{ u.u_error = NOERR; return (0); }
		else
			return (-1);
	}

	if ((void *)reply + sizeof (struct reply) > up->nfs_end_area)
		printf ("%g: Tamanho do RPC insuficiente\n");

	dep->d_ino = ENDIAN_LONG (reply->fattr.fa_ino);

	if ((dp = nfs2_put_nfsdata (nfssp, dep->d_ino, &reply->fhandle, dep)) == NONFSDATA)
		return (-1);

	memmove (&fattr, &reply->fattr, sizeof (FATTR));

	put_it_block (up->nfs_bp);

	/*
	 *	Cria/atualiza o INODE
	 */
	if ((son_ip = iget (ip->i_dev, dep->d_ino, INOREAD)) == NOINODE)
		return (-1);

	memmove (son_ip->i_handle, &dp->d_handle, sizeof (FHANDLE));

	if (son_ip->i_size == 0)
		son_ip->i_size = dp->d_size;

	nfs2_put_fattr_attribute (&fattr, son_ip);

	nfs2_put_time_attribute (&fattr, dp, son_ip);

	dp->d_size = son_ip->i_size;

   /*** inode_dirty_inc (son_ip); ***/

if (son_ip->i_mtime == 0)
printf ("%g: \"%s\": Tempo ZERADO\n", dep->d_name);

	son_ip->i_fsp = &fstab[FS_NFS2];

	iput (son_ip);

	return (1);

}	/* end nfs2_search_dir */

/*
 ****************************************************************
 *	Procura um ino dado no diretório			*
 ****************************************************************
 */
int
nfs2_search_dir_by_ino (IOREQ *iop, DIRENT *dep)
{
	INODE		*ip = iop->io_ip;
	const SB	*sp = ip->i_sb;
	NFSSB		*nfssp = sp->sb_ptr;
	struct u_nfs	*up = &u.u_myself->u_nfs;
	struct arg { FHANDLE handle; int offset; int count; }	*rp;
	ITBLOCK		*bp;
	int		index = u.u_uprocv - scb.y_uproc;
	const int	*roptr;
	off_t		offset = 0;
	ino_t		ino;
	UDP_EP		uep;

	/*
	 *	Malha principal
	 */
	for (EVER)
	{
		up->nfs_xid = (nfs_req_source++ << XID_PROC_SHIFT) | index;

		up->nfs_timeout	= sp->sb_mntent.me_timeout;
		up->nfs_transmitted = 0;

	    get_dirent_again:
		rp = nfs_setup_client_datagram_prolog (nfssp, &uep, NFSPROC_READDIR, &bp);

		memmove (&rp->handle, ip->i_handle, sizeof (FHANDLE));
#ifdef	DEBUG
printf ("%g: Usando READDIR, ino = %d, offset = %d, magic = %P\n", ip->i_ino, offset, ip->i_handle[0]);
#endif	DEBUG
		rp->offset = ENDIAN_LONG (offset);
		rp->count  = ENDIAN_IMMED (MAX_NFS2_DATA);

	   /***	bp->it_u_area   = ...;	***/
		bp->it_u_count  = (int)&rp[1] - (int)bp->it_u_area;

		EVENTCLEAR (&up->nfs_event);

		up->nfs_sp = nfssp;
		CLEAR (&up->nfs_lock);
		up->nfs_transmitted++;
		up->nfs_last_snd_time = time;

		send_udp_datagram (&uep, bp);

		EVENTWAIT (&up->nfs_event, PITNETIN);

		if   (up->nfs_status == 0)
			goto get_dirent_again;
		elif (up->nfs_status < 0)
			{ u.u_error = up->nfs_error; return (-1); }

		/*
		 *	Analisa a resposta
		 */
		roptr = up->nfs_area;

		if ((u.u_error = ENDIAN_LONG (*roptr++)) != NFS_OK)
			{ put_it_block (up->nfs_bp); return (-1); }
#undef	DEBUG
#ifdef	DEBUG
printf ("%g: Recebi %d bytes\n", up->nfs_end_area - (void *)roptr);
#endif	DEBUG
		/*
		 *	Usa as entradas deste bloco
		 */
		while ((void *)roptr < up->nfs_end_area)
		{
			if (*roptr++ == 0)	/* FALSE */
			{
				if (*roptr)	/* EOF */
					{ put_it_block (up->nfs_bp); u.u_error = ENOENT; return (-1); }

				break;
			}

			ino	= ENDIAN_LONG (*roptr++);
			roptr	= nfs_get_xdr_name (roptr, dep);
			offset	= ENDIAN_LONG (*roptr++);

			if (ip->i_par_ino == NOINO && streq (dep->d_name, ".."))
				ip->i_par_ino = ino;

			if (ino == dep->d_ino)		/* Achou */
				{ put_it_block (up->nfs_bp); return (1); }
		}

		put_it_block (up->nfs_bp);

	}	/* end for () */

}	/* end nfs2_search_dir_by_ino */

/*
 ****************************************************************
 *	Fornece entradas do diretório para o usuário 		*
 ****************************************************************
 */
void
nfs2_read_dir (IOREQ *iop)
{
	INODE		*ip = iop->io_ip;
	const SB	*sp = ip->i_sb;
	NFSSB		*nfssp = sp->sb_ptr;
	struct arg { FHANDLE handle; int offset; int count; }	*rp;
	struct u_nfs	*up = &u.u_myself->u_nfs;
	ITBLOCK		*bp;
	NFSDATA		*dp;
	int		index = u.u_uprocv - scb.y_uproc;
	const int	*roptr;
	off_t		offset;
	UDP_EP		uep;
	DIRENT		dirent;

	/*
	 *	Malha principal
	 */
	while (iop->io_count >= sizeof (DIRENT))
	{
		up->nfs_xid = (nfs_req_source++ << XID_PROC_SHIFT) | index;

		up->nfs_timeout	= sp->sb_mntent.me_timeout;
		up->nfs_transmitted = 0;

	    get_dirent_again:
		rp = nfs_setup_client_datagram_prolog (nfssp, &uep, NFSPROC_READDIR, &bp);

		memmove (&rp->handle, ip->i_handle, sizeof (FHANDLE));
#ifdef	DEBUG
printf ("%g: Usando READDIR, ino = %d, offset = %d, magic = %P\n", ip->i_ino, iop->io_offset_low, ip->i_handle[0]);
#endif	DEBUG
		rp->offset = ENDIAN_LONG (iop->io_offset_low);
		rp->count  = ENDIAN_IMMED (MAX_NFS2_DATA);

	   /***	bp->it_u_area   = ...;	***/
		bp->it_u_count  = (int)&rp[1] - (int)bp->it_u_area;

		EVENTCLEAR (&up->nfs_event);

		up->nfs_sp = nfssp;
		CLEAR (&up->nfs_lock);
		up->nfs_transmitted++;
		up->nfs_last_snd_time = time;

		send_udp_datagram (&uep, bp);

		EVENTWAIT (&up->nfs_event, PITNETIN);

		if   (up->nfs_status == 0)
			goto get_dirent_again;
		elif (up->nfs_status < 0)
			{ u.u_error = up->nfs_error; return; }

		/*
		 *	Analisa a resposta
		 */
		roptr = up->nfs_area;

		if ((u.u_error = ENDIAN_LONG (*roptr++)) != NFS_OK)
			{ put_it_block (up->nfs_bp); return; }
#undef	DEBUG
#ifdef	DEBUG
printf ("%g: Recebi %d bytes\n", up->nfs_end_area - (void *)roptr);
#endif	DEBUG
		/*
		 *	Usa as entradas deste bloco
		 */
		offset = iop->io_offset_low;

		while ((void *)roptr < up->nfs_end_area)
		{
			if (iop->io_count < sizeof (DIRENT))
				{ put_it_block (up->nfs_bp); return; }

			if (*roptr++ == 0)	/* FALSE */
			{
				if (*roptr)	/* EOF */
{
#ifdef	DEBUG
printf ("%g: Recebi EOF\n");
#endif	DEBUG
					{ iop->io_eof = 1; put_it_block (up->nfs_bp); return; }
}

				break;
			}

			dirent.d_ino	= ENDIAN_LONG (*roptr++);
			roptr = nfs_get_xdr_name (roptr, &dirent);
			dirent.d_offset	= offset;

			if (ip->i_par_ino == NOINO && streq (dirent.d_name, ".."))
				ip->i_par_ino = dirent.d_ino;

#ifdef	DEBUG
printf ("%g: Recebi ino = %d, nm = \"%s\", offset = %d\n", dirent.d_ino, dirent.d_name, dirent.d_offset);
#endif	DEBUG

			offset = ENDIAN_LONG (*roptr++);

			if (unimove (iop->io_area, &dirent, sizeof (DIRENT), iop->io_cpflag) < 0)
				{ u.u_error = EFAULT; break; }

			if ((dp = nfs2_put_nfsdata (nfssp, dirent.d_ino, NOFHANDLE, &dirent)) == NONFSDATA)
				break;

			dp->d_par_ino = ip->i_ino;

			/* Avança os ponteiros */

			iop->io_area  += sizeof (DIRENT);
			iop->io_count -= sizeof (DIRENT);

			iop->io_offset_low = offset;
		}

		put_it_block (up->nfs_bp);

	}	/* end for () */

}	/* end nfs2_read_dir */

/*
 ****************************************************************
 *	Escreve a entrada no diretório				*
 ****************************************************************
 */
void
nfs2_write_dir (IOREQ *iop, const DIRENT *dep, const INODE *son_ip, int rename)
{
	INODE		*pp = iop->io_ip;
	SB		*sp = pp->i_sb;
	NFSSB		*nfssp = sp->sb_ptr;
	struct arg { FHANDLE son_handle, par_handle; int name[1]; }	*rp;
	const struct reply { int status; }	*reply;
	struct u_nfs    *up = &u.u_myself->u_nfs;
	int		*ptr;
	ITBLOCK		*bp;
	FHANDLE		pp_handle;
	UDP_EP		uep;

	/*
	 *	Escreve o diretório
	 */
#ifdef	DEBUG
printf ("%g: Usando LINK\n");
#endif	DEBUG

	memmove (&pp_handle, pp->i_handle, sizeof (FHANDLE)); iput (pp);

	up->nfs_xid = (nfs_req_source++ << XID_PROC_SHIFT) | (u.u_uprocv - scb.y_uproc);

	up->nfs_timeout	= sp->sb_mntent.me_timeout;
	up->nfs_transmitted = 0;

    get_link_again:
	rp = nfs_setup_client_datagram_prolog (nfssp, &uep, NFSPROC_LINK, &bp);

	memmove (&rp->son_handle, son_ip->i_handle, sizeof (FHANDLE));

	memmove (&rp->par_handle, &pp_handle, sizeof (FHANDLE));

	ptr = nfs_put_xdr_name (rp->name, dep->d_name, dep->d_namlen);

   /***	bp->it_u_area   = ...;	***/
	bp->it_u_count  = (int)ptr - (int)bp->it_u_area;

	EVENTCLEAR (&up->nfs_event);

	up->nfs_sp = nfssp;
	CLEAR (&up->nfs_lock);
	up->nfs_transmitted++;
	up->nfs_last_snd_time = time;

	send_udp_datagram (&uep, bp);

	EVENTWAIT (&up->nfs_event, PITNETIN);

	if   (up->nfs_status == 0)
		goto get_link_again;
	elif (up->nfs_status < 0)
		{ u.u_error = up->nfs_error; return; }

	/* Analisa a resposta */

	if ((void *)(reply = up->nfs_area) + sizeof (struct reply) > up->nfs_end_area)
		printf ("%g: Tamanho do RPC insuficiente\n");

	u.u_error = ENDIAN_LONG (reply->status);

	if (u.u_error == EROFS)
		nfs_server_is_rofs (sp);

	put_it_block (up->nfs_bp);

}	/* end nfs2_write_dir */

/*
 ****************************************************************
 *	Certifica-se de que um diretório está vazio		*
 ****************************************************************
 */
int
nfs2_empty_dir (INODE *ip)
{
	const SB	*sp = ip->i_sb;
	NFSSB		*nfssp = sp->sb_ptr;
	struct arg { FHANDLE handle; int offset; int count; }	*rp;
	struct u_nfs	*up = &u.u_myself->u_nfs;
	ITBLOCK		*bp;
	int		index = u.u_uprocv - scb.y_uproc;
	const int	*roptr;
	off_t		offset = 0;
	UDP_EP		uep;
	DIRENT		dirent;

	/*
	 *	Recebe e devolve o "ip" trancado
	 *
	 *	Se estiver vazio, devolve 1
	 *
	 */

	/*
	 *	Malha principal
	 */
	for (EVER)
	{
		up->nfs_xid = (nfs_req_source++ << XID_PROC_SHIFT) | index;

		up->nfs_timeout	= sp->sb_mntent.me_timeout;
		up->nfs_transmitted = 0;

	    get_dirent_again:
		rp = nfs_setup_client_datagram_prolog (nfssp, &uep, NFSPROC_READDIR, &bp);

		memmove (&rp->handle, ip->i_handle, sizeof (FHANDLE));
#ifdef	DEBUG
printf ("%g: Usando READDIR, ino = %d, magic = %P\n", ip->i_ino, ip->i_handle[0]);
#endif	DEBUG
		rp->offset = ENDIAN_LONG (offset);
		rp->count  = ENDIAN_IMMED (MAX_NFS2_DATA);

	   /***	bp->it_u_area   = ...;	***/
		bp->it_u_count  = (int)&rp[1] - (int)bp->it_u_area;

		EVENTCLEAR (&up->nfs_event);

		up->nfs_sp = nfssp;
		CLEAR (&up->nfs_lock);
		up->nfs_transmitted++;
		up->nfs_last_snd_time = time;

		send_udp_datagram (&uep, bp);

		EVENTWAIT (&up->nfs_event, PITNETIN);

		if   (up->nfs_status == 0)
			goto get_dirent_again;
		elif (up->nfs_status < 0)
			{ u.u_error = up->nfs_error; return (-1); }

		/*
		 *	Analisa a resposta
		 */
		roptr = up->nfs_area;

		if ((u.u_error = ENDIAN_LONG (*roptr++)) != NFS_OK)
			{ put_it_block (up->nfs_bp); return (-1); }
#ifdef	DEBUG
printf ("%g: Recebi %d bytes\n", up->nfs_end_area - (void *)roptr);
#endif	DEBUG
		/*
		 *	Usa as entradas deste bloco
		 */
		while ((void *)roptr < up->nfs_end_area)
		{
			if (*roptr++ == 0)	/* FALSE */
			{
				if (*roptr)	/* EOF */
{
#ifdef	DEBUG
printf ("%g: Recebi EOF\n");
#endif	DEBUG
					{ put_it_block (up->nfs_bp); return (1); }
}

				break;
			}

			roptr = nfs_get_xdr_name (++roptr, &dirent);		/* Pula "ino" */

			if (!IS_DOT_or_DOTDOT (dirent.d_name))
				{ put_it_block (up->nfs_bp); return (0); }

#ifdef	DEBUG
printf ("%g: Recebi ino = %d, nm = \"%s\", offset = %d\n", dirent.d_ino, dirent.d_name, dirent.d_offset);
#endif	DEBUG

			offset = ENDIAN_LONG (*roptr++);
		}

		put_it_block (up->nfs_bp);

	}	/* end for (EVER) */

}	/* end nfs2_empty_dir */

/*
 ****************************************************************
 *	Cria um diretório					*
 ****************************************************************
 */
INODE *
nfs2_make_dir (IOREQ *iop, DIRENT *dep, int mode)
{
	INODE		*pp = iop->io_ip, *ip;
	dev_t		par_dev = pp->i_dev;
	ino_t		par_ino = pp->i_ino, ino;
	SB		*sp = pp->i_sb;
	NFSSB		*nfssp = sp->sb_ptr;
	struct arg { FHANDLE handle; int name[1]; /* SATTR */ }		 *rp;
	const struct reply { int status; FHANDLE fhandle; FATTR fattr; } *reply;
	struct u_nfs    *up = &u.u_myself->u_nfs;
	int		index = u.u_uprocv - scb.y_uproc;
	NFSDATA		*dp;
	ITBLOCK		*bp;
	SATTR		*sap;
	FHANDLE		pp_handle;
	FATTR		fattr;
	UDP_EP		uep;

	/*
	 *	Atualiza o pai
	 */
	pp->i_nlink++; inode_dirty_inc (pp);

	memmove (&pp_handle, pp->i_handle, sizeof (FHANDLE));

	iput (pp);

	/*
	 *	Cria o diretório
	 */
#ifdef	DEBUG
	printf ("%g: Usando MKDIR\n");
#endif	DEBUG

	up->nfs_xid = (nfs_req_source++ << XID_PROC_SHIFT) | index;

	up->nfs_timeout = sp->sb_mntent.me_timeout;
	up->nfs_transmitted = 0;

    get_mkdir_again:
	rp = nfs_setup_client_datagram_prolog (nfssp, &uep, NFSPROC_MKDIR, &bp);

	memmove (&rp->handle, &pp_handle, sizeof (FHANDLE));

	sap = nfs_put_xdr_name (rp->name, dep->d_name, dep->d_namlen);

	sap->sa_mode		= ENDIAN_LONG (mode & ~u.u_cmask);
	sap->sa_uid		= ENDIAN_LONG (u.u_euid);
	sap->sa_gid		= ENDIAN_LONG (u.u_egid);
	sap->sa_size		= -1;
	sap->sa_atime.sec	= ENDIAN_LONG (time);
	sap->sa_atime.usec	= 0;
	sap->sa_mtime.sec	= ENDIAN_LONG (time);
	sap->sa_mtime.usec	= 0;

   /***	bp->it_u_area   = ...;	***/
	bp->it_u_count  = (int)&sap[1] - (int)bp->it_u_area;

	EVENTCLEAR (&up->nfs_event);

	up->nfs_sp = nfssp;
	CLEAR (&up->nfs_lock);
	up->nfs_transmitted++;
	up->nfs_last_snd_time = time;

	send_udp_datagram (&uep, bp);

	EVENTWAIT (&up->nfs_event, PITNETIN);

	if   (up->nfs_status == 0)
		goto get_mkdir_again;
	elif (up->nfs_status < 0)
		{ u.u_error = up->nfs_error; return (NOINODE); }

	/* Analisa a resposta */

	reply = up->nfs_area;

	if ((u.u_error = ENDIAN_LONG (reply->status)) != NFS_OK)
	{
		if (u.u_error == EROFS)
			nfs_server_is_rofs (sp);

		put_it_block (up->nfs_bp); return (NOINODE);
	}

	if ((void *)reply + sizeof (struct reply) > up->nfs_end_area)
		printf ("%g: Tamanho do RPC insuficiente\n");

	/*
	 *	Põe o HANDLE no CACHE
	 */
	ino = ENDIAN_LONG (reply->fattr.fa_ino);

	dp = nfs2_put_nfsdata (nfssp, ino, &reply->fhandle, dep);

	memmove (&fattr, &reply->fattr, sizeof (FATTR));

	put_it_block (up->nfs_bp);

	if (dp == NONFSDATA)
		return (NOINODE);

	/*
	 *	Constrói o INODE local
	 */
	if ((ip = iget (par_dev, ino, 0)) == NOINODE)
	{
		if ((pp = iget (par_dev, par_ino, 0)) != NOINODE)
			{ pp->i_nlink--; inode_dirty_inc (pp); iput (pp); }

		return (NOINODE);
	}

	memmove (ip->i_handle, &dp->d_handle, sizeof (FHANDLE));

	nfs2_put_fattr_attribute (&fattr, ip);

   /*** inode_dirty_inc (ip);	***/

	/*
	 *	Obtém o código do sistema de arquivos
	 */
	ip->i_fsp = &fstab[FS_NFS2];

	return (ip);

}	/* end nfs2_make_dir */

/*
 ****************************************************************
 *	Apaga uma entrada de diretório				*
 ****************************************************************
 */
void
nfs2_clr_dir (IOREQ *iop, const DIRENT *dep, const INODE *ip)
{
	INODE		*pp = iop->io_ip;
	SB		*sp = pp->i_sb;
	NFSSB		*nfssp = sp->sb_ptr;
	struct arg { FHANDLE handle; int name[1]; }	*rp;
	const struct reply { int status; }		*reply;
	struct u_nfs	*up = &u.u_myself->u_nfs;
	ITBLOCK		*bp;
	int		code, *ptr;
	UDP_EP		uep;

	/*
	 *	Verifica se é um diretório ou não
	 */
	if ((ip->i_mode & IFMT) == IFDIR)
		code = NFSPROC_RMDIR;
	else
		code = NFSPROC_REMOVE;

	/*
	 *	Apaga a entrada do diretório
	 */
#ifdef	DEBUG
printf ("%g: usando REMOVE\n");
#endif	DEBUG

	up->nfs_xid = (nfs_req_source++ << XID_PROC_SHIFT) | (u.u_uprocv - scb.y_uproc);

	up->nfs_timeout	= sp->sb_mntent.me_timeout;
	up->nfs_transmitted = 0;

    get_remove_again:
	rp = nfs_setup_client_datagram_prolog (nfssp, &uep, code, &bp);

	memmove (&rp->handle, pp->i_handle, sizeof (FHANDLE));

	ptr = nfs_put_xdr_name (rp->name, dep->d_name, dep->d_namlen);

   /***	bp->it_u_area   = ...;	***/
	bp->it_u_count  = (int)ptr - (int)bp->it_u_area;

	EVENTCLEAR (&up->nfs_event);

	up->nfs_sp = nfssp;
	CLEAR (&up->nfs_lock);
	up->nfs_transmitted++;
	up->nfs_last_snd_time = time;

	send_udp_datagram (&uep, bp);

	EVENTWAIT (&up->nfs_event, PITNETIN);

	if   (up->nfs_status == 0)
		goto get_remove_again;
	elif (up->nfs_status < 0)
		{ u.u_error = up->nfs_error; return; }

	/* Analisa a resposta */

	if ((void *)(reply = up->nfs_area) + sizeof (struct reply) > up->nfs_end_area)
		printf ("%g: Tamanho do RPC insuficiente\n");

	u.u_error = ENDIAN_LONG (reply->status);

	if (u.u_error == EROFS)
		nfs_server_is_rofs (sp);

	put_it_block (up->nfs_bp);

}	/* end nfs2_clr_dir */

/*
 ****************************************************************
 *	Troca o nome de um arquivo				*
 ****************************************************************
 */
int
nfs2_rename (INODE *old_ip, IOREQ *iop, const DIRENT *dep, const char *linkname)
{
	INODE		*new_ip;
	SB		*sp = old_ip->i_sb;
	NFSSB		*nfssp = sp->sb_ptr;
	struct arg { FHANDLE handle; int name[1]; /* FHANDLE, NAME */ }	*rp;
	const struct reply { int status; }	*reply;
	struct u_nfs    *up = &u.u_myself->u_nfs;
	ino_t		old_pp_ino;
	int		*ptr;
	const NFSDATA	*old_dp, *new_dp;
	ITBLOCK		*bp;
	DIRENT		de;
	UDP_EP		uep;

	/*
	 *	Obtém o diretório antigo
	 */
	if ((old_pp_ino = old_ip->i_par_ino) == NOINO)
		{ printf ("%g: NÃO consegui obter o INO\n"); u.u_error = ENOTMNT; iput (old_ip); return (-1); }

	if (old_ip->i_nm != NOSTR)
		{ free_byte (old_ip->i_nm); old_ip->i_nm = NOSTR; }

	iput (old_ip);

	if ((old_dp = nfs2_get_nfsdata (nfssp, old_pp_ino)) == NONFSDATA)
		{ u.u_error = ENOTMNT; return (-1); }

	/*
	 *	Examina o Nome novo
	 */
	if ((new_ip = iname (linkname, getuchar, ICREATE, iop, &de)) != NOINODE)
		{ u.u_error = EEXIST; iput (new_ip); return (-1); }

	if (u.u_error != NOERR)
		return (-1);

	new_dp = nfs2_get_nfsdata (nfssp, iop->io_ip->i_ino);

	iput (iop->io_ip);

	/*
	 *	
	 */
#ifdef	DEBUG
printf ("%g: Usando RENAME\n");
#endif	DEBUG

	up->nfs_xid = (nfs_req_source++ << XID_PROC_SHIFT) | (u.u_uprocv - scb.y_uproc);

	up->nfs_timeout	= sp->sb_mntent.me_timeout;
	up->nfs_transmitted = 0;

    get_rename_again:
	rp = nfs_setup_client_datagram_prolog (nfssp, &uep, NFSPROC_RENAME, &bp);

	memmove (&rp->handle, &old_dp->d_handle, sizeof (FHANDLE));

	rp = nfs_put_xdr_name (rp->name, dep->d_name, dep->d_namlen);

	memmove (&rp->handle, &new_dp->d_handle, sizeof (FHANDLE));

	ptr = nfs_put_xdr_name (rp->name, de.d_name, de.d_namlen);

   /***	bp->it_u_area   = ...;	***/
	bp->it_u_count  = (int)ptr - (int)bp->it_u_area;

	EVENTCLEAR (&up->nfs_event);

	up->nfs_sp = nfssp;
	CLEAR (&up->nfs_lock);
	up->nfs_transmitted++;
	up->nfs_last_snd_time = time;

	send_udp_datagram (&uep, bp);

	EVENTWAIT (&up->nfs_event, PITNETIN);

	if   (up->nfs_status == 0)
		goto get_rename_again;
	elif (up->nfs_status < 0)
		{ u.u_error = up->nfs_error; return (-1); }

	/* Analisa a resposta */

	if ((void *)(reply = up->nfs_area) + sizeof (struct reply) > up->nfs_end_area)
		printf ("%g: Tamanho do RPC insuficiente\n");

	u.u_error = ENDIAN_LONG (reply->status);

	if (u.u_error == EROFS)
		nfs_server_is_rofs (sp);

	put_it_block (up->nfs_bp);

	return (u.u_error == NOERR ? 1 : -1);	/* Para informar "rename" */

}	/* end nfs2_rename */
