/*
 ****************************************************************
 *								*
 *			nfs2c_inode.c				*
 *								*
 *	Alocação e desalocação de INODEs NFS			*
 *								*
 *	Versão	4.8.0, de 05.10.05				*
 *		4.8.0, de 05.10.05				*
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
#include "../h/bhead.h"
#include "../h/sysdirent.h"
#include "../h/itnet.h"
#include "../h/nfs.h"
#include "../h/inode.h"
#include "../h/signal.h"
#include "../h/uproc.h"
#include "../h/uerror.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Lê um INODE NFS do servidor				*
 ****************************************************************
 */
int
nfs2_read_inode (INODE *ip)
{
	const SB	*sp = ip->i_sb;
	NFSSB		*nfssp = sp->sb_ptr;
	struct u_nfs    *up = &u.u_myself->u_nfs;
	const FHANDLE	*hp;
	int		index = u.u_uprocv - scb.y_uproc;
	int		*ptr;
	NFSDATA		*dp, *par_dp;
	ITBLOCK		*bp;
	const FATTR	*fp;
	UDP_EP		uep;

	/*
	 *	Procura o INO na lista de NFSDATA e analisa as informações disponíveis
	 */
	if ((dp = nfs2_get_nfsdata (nfssp, ip->i_ino)) == NONFSDATA)
	    { printf ("%g: NÃO encontrei o INO %d no CACHE\n", ip->i_ino); u.u_error = EINVAL; return (-1); }

	/*
	 *	Obtém os dados do INODE
	 */
	if (!dp->d_handle_pres)			/*********** Obtém o HANDLE **************/
	{
		struct arg { FHANDLE handle; int name[1]; } 				*rp;
		const struct reply { int status; FHANDLE fhandle; FATTR fattr; }	*reply;

#ifdef	CASE_DEBUG
		printf ("%g: Usando caso 2: LOOKUP (INO = %d)\n", ip->i_ino);
#endif	CASE_DEBUG

		if (dp->d_nm_len == 0)
		{
			printf ("%g: NÃO encontrei o NOME do INO %d no CACHE\n", ip->i_ino);
			u.u_error = EINVAL; return (-1);
		}

		if ((par_dp = nfs2_get_nfsdata (nfssp, dp->d_par_ino)) == NONFSDATA)
		{
			printf ("%g: NÃO encontrei o pai do INO %d no CACHE\n", ip->i_ino);
			u.u_error = EINVAL; return (-1);
		}

		if (!par_dp->d_handle_pres)
		{
			printf ("%g: O pai do INO %d NÃO tem HANDLE\n", ip->i_ino);
			u.u_error = EINVAL; return (-1);
		}

		up->nfs_xid = (nfs_req_source++ << XID_PROC_SHIFT) | index;

		up->nfs_timeout = sp->sb_mntent.me_timeout;
		up->nfs_transmitted = 0;

	    get_handle_again:
		rp = nfs_setup_client_datagram_prolog (nfssp, &uep, NFSPROC_LOOKUP, &bp);

		memmove (&rp->handle, &par_dp->d_handle, sizeof (FHANDLE));

		ptr = nfs_put_xdr_name (rp->name, dp->d_name, dp->d_nm_len);

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
			goto get_handle_again;
		elif (up->nfs_status < 0)
			{ u.u_error = up->nfs_error; return (-1); }

		/* Analisa a resposta */

		reply = up->nfs_area;

		if ((u.u_error = ENDIAN_LONG (reply->status)) != NFS_OK)
			{  put_it_block (up->nfs_bp); return (-1); }

		if ((void *)reply + sizeof (struct reply) > up->nfs_end_area)
			printf ("%g: Tamanho do RPC insuficiente\n");

		hp = &reply->fhandle; fp = &reply->fattr;

		nfs2_put_nfsdata (nfssp, ip->i_ino, hp, NODIRENT);	/* Já tem o nome */
#ifdef	DEBUG
		printf
		(	"%g: type = %d, mode = %o, size = %d, blksz = %d\n",
			ENDIAN_LONG (fp->fa_type), ENDIAN_LONG (fp->fa_mode),
			ENDIAN_LONG (fp->fa_size), ENDIAN_LONG (fp->fa_blksz)
		);
#endif	DEBUG
	}
	else					/*********** Obtém os ATRIBUTOS **********/
	{
		struct arg { FHANDLE handle; } 			*rp;
		const struct reply { int status; FATTR fattr; }	*reply;

#undef	CASE_DEBUG
#ifdef	CASE_DEBUG
		printf ("%g: Usando caso 1: GETATTR (INO = %d)\n", ip->i_ino);
#endif	CASE_DEBUG

		up->nfs_xid = (nfs_req_source++ << XID_PROC_SHIFT) | index;

		up->nfs_timeout = sp->sb_mntent.me_timeout;
		up->nfs_transmitted = 0;

	    get_fattr_again:
		rp = nfs_setup_client_datagram_prolog (nfssp, &uep, NFSPROC_GETATTR, &bp);

		memmove (&rp->handle, &dp->d_handle, sizeof (FHANDLE));

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
			goto get_fattr_again;
		elif (up->nfs_status < 0)
			{ u.u_error = up->nfs_error; return (-1); }

		/* Analisa a resposta */

		reply = up->nfs_area;

		if ((u.u_error = ENDIAN_LONG (reply->status)) != NFS_OK)
			{  put_it_block (up->nfs_bp); return (-1); }

		if ((void *)reply + sizeof (struct reply) > up->nfs_end_area)
			printf ("%g: Tamanho do RPC insuficiente\n");

		hp = &dp->d_handle; fp = &reply->fattr;
	}

	/*
	 *	Agora, processa as informações
	 */
	memmove (ip->i_handle, hp, sizeof (FHANDLE));

	ip->i_size = dp->d_size;	/* Se for ZERO, será corrigido abaixo */

	nfs2_put_fattr_attribute (fp, ip);

	ip->i_mtime = 0;

	nfs2_put_time_attribute (fp, dp, ip);

   /***	inode_dirty_inc (ip); 	***/

	if (up->nfs_bp != NOITBLOCK)
		put_it_block (up->nfs_bp);

if (ip->i_mtime == 0)
printf ("%g: \"%s\": Tempo ZERADO\n", ip->i_nm);

	/*
	 ****** Completa o INODE ****************************************
	 */
	ip->i_fsp = &fstab[FS_NFS2];

	return (0);

}	/* end nfs2_read_inode */

/*
 ****************************************************************
 *	Atualiza um INODE NFS no servidor			*
 ****************************************************************
 */
void
nfs2_write_inode (INODE *ip)
{
	NFSSB		*nfssp = ip->i_sb->sb_ptr;
	NFSDATA		*dp;

	/*
	 *	Verifica se não houve modificação no INODE
	 */
	if ((ip->i_flags & ICHG) == 0)
		return;

	inode_dirty_dec (ip);

#ifdef	DEBUG
	printf ("%g: Atualizando INODE (%v, %d, %s)\n", ip->i_dev, ip->i_ino, ip->i_nm);
#endif	DEBUG

	/*
	 *	Atualiza o tamanho no cache
	 */
	if ((dp = nfs2_get_nfsdata (nfssp, ip->i_ino)) != NONFSDATA)
		{ dp->d_size = ip->i_size; dp->d_mtime = ip->i_mtime; }

	/*
	 *	Verifica se ele está montado somente para leituras
	 */
	if (ip->i_sb->sb_mntent.me_flags & SB_RONLY)
		return;

	if (ip->i_nlink <= 0)
		return;

	/*
	 *	Envia os dados para o servidor
	 */
	nfs2_do_setattr (ip, -1 /* NÃO altera o tamanho */);

}	/* end nfs2_write_inode */

/*
 ****************************************************************
 *	Cria um arquivo novo					*
 ****************************************************************
 */
INODE *
nfs2_make_inode (IOREQ *iop, DIRENT *dep, int mode)
{
	INODE		*pp = iop->io_ip, *ip;
	dev_t		par_dev = pp->i_dev;
	ino_t		ino;
	SB		*sp = pp->i_sb;
	NFSSB		*nfssp = sp->sb_ptr;
	struct arg { FHANDLE handle; int name[1]; /* SATTR */ }			*rp;
	const struct reply { int status; FHANDLE fhandle; FATTR fattr; }	*reply;
	struct u_nfs    *up = &u.u_myself->u_nfs;
	int		index = u.u_uprocv - scb.y_uproc;
	NFSDATA		*dp;
	ITBLOCK		*bp;
	SATTR		*sap;
	FHANDLE		pp_handle;
	FATTR		fattr;
	UDP_EP		uep;

	/*
	 *	Cria o INODE
	 */
	memmove (&pp_handle, pp->i_handle, sizeof (FHANDLE));

	iput (pp);

#ifdef	DEBUG
	printf ("%g: Usando CREATE\n");
#endif	DEBUG

	up->nfs_xid = (nfs_req_source++ << XID_PROC_SHIFT) | index;

	up->nfs_timeout = sp->sb_mntent.me_timeout;
	up->nfs_transmitted = 0;

    get_create_again:
	rp = nfs_setup_client_datagram_prolog (nfssp, &uep, NFSPROC_CREATE, &bp);

	memmove (&rp->handle, &pp_handle, sizeof (FHANDLE));

	sap = nfs_put_xdr_name (rp->name, dep->d_name, dep->d_namlen);

	if ((mode & IFMT) == 0)
		mode |= IFREG;

	sap->sa_mode		= ENDIAN_LONG (mode & ~u.u_cmask);
	sap->sa_uid		= ENDIAN_LONG (u.u_euid);
	sap->sa_gid		= ENDIAN_LONG (u.u_egid);
	sap->sa_size		= ENDIAN_IMMED (-1);
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
		goto get_create_again;
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
		return (NOINODE);

	memmove (ip->i_handle, &dp->d_handle, sizeof (FHANDLE));

	nfs2_put_fattr_attribute (&fattr, ip);

ip->i_size  = 0;
dp->d_size  = 0;

ip->i_mtime = time;
ip->i_atime = time;
ip->i_ctime = time;

   /***	inode_dirty_inc (ip);	***/

	/*
	 *	Obtém o código do sistema de arquivos
	 */
	ip->i_fsp = &fstab[FS_NFS2];

	return (ip);

}	/* end nfs2_make_inode */

/*
 ****************************************************************
 *	Trunca um INODE NFS					*
 ****************************************************************
 */
void
nfs2_trunc_inode (INODE *ip)
{
	NFSDATA		*dp;

	if (ip->i_nlink <= 0)
		return;

	/*
	 *	Em primeiro lugar, retira o conteúdo dos blocos do CACHE
	 */
	block_free (ip->i_dev, ip->i_ino);

	if ((dp = nfs2_get_nfsdata (ip->i_sb->sb_ptr, ip->i_ino)) != NONFSDATA)
		dp->d_size = 0;

	ip->i_size = 0;

	nfs2_do_setattr (ip, 0 /* Trunca o tamanho */);

}	/* end nfs2_trunc_inode */

/*
 ****************************************************************
 *	Libera um INODE do Disco				*
 ****************************************************************
 */
void
nfs2_free_inode (const INODE *ip)
{
	const SB	*sp = ip->i_sb;
	NFSSB		*nfssp = sp->sb_ptr;

	block_free (ip->i_dev, ip->i_ino);

	nfs2_remove_nfsdata (nfssp, ip->i_ino);

}	/* end nfs2_free_inode */

/*
 ****************************************************************
 *	Executa um SETATTR					*
 ****************************************************************
 */
void
nfs2_do_setattr (INODE *ip, off_t size)
{
	SB		*sp = ip->i_sb;
	NFSSB		*nfssp = sp->sb_ptr;
	struct arg { FHANDLE handle; SATTR sattr; }	*rp;
	const struct reply { int status; FATTR fattr; }	*reply;
	struct u_nfs    *up = &u.u_myself->u_nfs;
	int		index = u.u_uprocv - scb.y_uproc;
	ITBLOCK		*bp;
	UDP_EP		uep;

	/*
	 *	Envia o estado do INODE
	 */
#ifdef	CASE_DEBUG
	printf ("%g: Usando SETATTR (INO = %s, %d), size = %d\n", ip->i_nm, ip->i_ino, size);
#endif	CASE_DEBUG

	up->nfs_xid = (nfs_req_source++ << XID_PROC_SHIFT) | index;

	up->nfs_timeout = sp->sb_mntent.me_timeout;
	up->nfs_transmitted = 0;

    get_sattr_again:
	rp = nfs_setup_client_datagram_prolog (nfssp, &uep, NFSPROC_SETATTR, &bp);

	memmove (&rp->handle, &ip->i_handle, sizeof (FHANDLE));

	rp->sattr.sa_mode	= ENDIAN_LONG (ip->i_mode);
	rp->sattr.sa_uid	= ENDIAN_LONG (ip->i_uid);
	rp->sattr.sa_gid	= ENDIAN_LONG (ip->i_gid);
	rp->sattr.sa_size	= ENDIAN_LONG (size);
	rp->sattr.sa_atime.sec	= ENDIAN_LONG (ip->i_atime);
	rp->sattr.sa_atime.usec	= 0;
	rp->sattr.sa_mtime.sec	= ENDIAN_LONG (ip->i_mtime);
	rp->sattr.sa_mtime.usec	= 0;

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
		goto get_sattr_again;
	elif (up->nfs_status < 0)
		{ u.u_error = up->nfs_error; return; }

	/* Analisa a resposta */

	reply = up->nfs_area;

	if ((u.u_error = ENDIAN_LONG (reply->status)) != NFS_OK)
	{
		if (u.u_error == EROFS)
			nfs_server_is_rofs (sp);

		put_it_block (up->nfs_bp); return;
	}

	if ((void *)reply + sizeof (struct reply) > up->nfs_end_area)
		printf ("%g: Tamanho do RPC insuficiente\n");

   /***	nfs2_put_fattr_attribute (&reply->fattr, ip, 0);	***/

	inode_dirty_dec (ip);		/* O INODE foi atualizado no servidor */

	put_it_block (up->nfs_bp);

}	/* end nfs2_do_setattr */
