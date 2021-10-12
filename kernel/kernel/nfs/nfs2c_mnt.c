/*
 ****************************************************************
 *								*
 *			nfs2c_mnt.c				*
 *								*
 *	Funções relacionadas com o NFS MOUNT PROGRAM (cliente)	*
 *								*
 *	Versão	4.8.0, de 01.10.05				*
 *		4.8.0, de 01.10.05				*
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

#include "../h/itnet.h"
#include "../h/sysdirent.h"
#include "../h/nfs.h"
#include "../h/mntent.h"
#include "../h/sb.h"
#include "../h/ustat.h"
#include "../h/signal.h"
#include "../h/uproc.h"
#include "../h/uerror.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Variáveis e áreas globais				*
 ****************************************************************
 */
entry int		nfs_req_source;

/*
 ****************************************************************
 *	Envia um pedido de montagem ao servidor			*
 ****************************************************************
 */
NFSSB *
nfs2c_send_mntpgm_datagram (const char *user_dev_nm, const struct mntent *mntent_ptr, SB *sp)
{
	char		*dir_nm;
	ITBLOCK		*bp;
	struct arg { FHANDLE handle; }				  *rp;
	const struct port_reply	  { int port; }			  *port_reply;
	const struct handle_reply { int status; FHANDLE handle; } *handle_reply;
	const struct fattr_reply  { int status; FATTR fattr; }	  *fattr_reply;
	struct u_nfs	*up = &u.u_myself->u_nfs;
	int		*msg, *ptr;
	int		index;
	NFSSB		*nfssp;
	UDP_EP		uep;
	FHANDLE		root_handle;
	char		dev_nm[64];

	/*
	 *	Obtém o diretório remoto a ser montado
	 */
	if (get_user_str (dev_nm, user_dev_nm, sizeof (dev_nm)) < 0)
		{ u.u_error = EFAULT; return (NONFSSB); }

	if ((dir_nm = strchr (dev_nm, ':')) == NOSTR)
		{ u.u_error = EINVAL; return (NONFSSB); }

	*dir_nm++ = '\0';

	/*
	 *	Consegue um superbloco para a montagem
	 */
	if ((nfssp = malloc_byte (sizeof (NFSSB))) == NONFSSB)
		{ u.u_error = ENOMEM; return (NONFSSB); }

	memclr (nfssp, sizeof (NFSSB));

	nfssp->sb_sp = sp;

	nfssp->sb_server_addr = mntent_ptr->me_server_addr;
	strscpy (nfssp->sb_server_nm, dev_nm, sizeof (nfssp->sb_server_nm));

	index = u.u_uprocv - scb.y_uproc;

	/*
	 ******	Primeira parte: Obter a porta MOUNT *********************
	 */
	up->nfs_xid = (nfs_req_source++ << XID_PROC_SHIFT) | index;

	up->nfs_timeout	= mntent_ptr->me_timeout;
	up->nfs_transmitted = 0;

    get_mount_port_again:
	bp = get_it_block (IT_OUT_DATA);

	ptr = msg = (int *)(bp->it_frame + RESSZ);

	ptr = nfs_store_rpc_values
	(
		ptr, 11, PMAP_PROG, PMAP_VERS, PMAPPROC_GETPORT, AUTH_NULL, 0, AUTH_NULL, 0,
		MOUNTPROG, MOUNTVERS, UDP_PROTO, 0
	);

	uep.up_snd_addr = nfssp->sb_server_addr;
	uep.up_snd_port = PMAP_PORT;

	uep.up_my_addr  = 0;		/* Calcula o endereço local */
	uep.up_my_port	= NFS_CLIENT_PORT;

	bp->it_u_area   = msg;
	bp->it_u_count  = (int)ptr - (int)msg;

	EVENTCLEAR (&up->nfs_event);

	up->nfs_sp = nfssp;
	CLEAR (&up->nfs_lock);
	up->nfs_transmitted++;
	up->nfs_last_snd_time = time;

	send_udp_datagram (&uep, bp);

	EVENTWAIT (&up->nfs_event, PITNETIN);

	if   (up->nfs_status == 0)
		goto get_mount_port_again;
	elif (up->nfs_status < 0)
		goto bad_with_error;

	/* Analisa a resposta */

	if ((void *)(port_reply = up->nfs_area) + sizeof (struct port_reply) > up->nfs_end_area)
		printf ("%g: Tamanho do RPC insuficiente\n");

	nfssp->sb_mount_port = ENDIAN_LONG (port_reply->port);

#ifdef	DEBUG
	printf ("%g: mount_port = %d\n", nfssp->sb_mount_port);
#endif	DEBUG

	put_it_block (up->nfs_bp); 

	/*
	 ******	Segunda parte: Montar o diretório ***********************
	 */
	up->nfs_xid = (nfs_req_source++ << XID_PROC_SHIFT) | index;

   /***	up->nfs_timeout = mntent_ptr->me_timeout;	***/
	up->nfs_transmitted = 0;

    get_handle_again:
	bp = get_it_block (IT_OUT_DATA);

	ptr	= msg = (int *)(bp->it_frame + RESSZ);

	ptr	= nfs_store_rpc_values (ptr, 3, MOUNTPROG, MOUNTVERS, MOUNTPROG_MNT);

	ptr	= nfs_put_auth_unix (ptr, 0, 0);	/* Autenticação UNIX */

	*ptr++	= ENDIAN_IMMED (AUTH_NULL);		/* Autenticação */
	*ptr++	= ENDIAN_IMMED (0);			/* Tamanho NULO */

	ptr	= nfs_put_xdr_name (ptr, dir_nm, strlen (dir_nm));

	uep.up_snd_addr = nfssp->sb_server_addr;
	uep.up_snd_port = nfssp->sb_mount_port;

	uep.up_my_addr  = 0;		/* Calcula o endereço local */
	uep.up_my_port	= NFS_CLIENT_PORT;

	bp->it_u_area   = msg;
	bp->it_u_count  = (int)ptr - (int)msg;

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
		goto bad_with_error;

	/* Analisa a resposta */

	handle_reply = up->nfs_area;

	if ((u.u_error = ENDIAN_LONG (handle_reply->status)) != NFS_OK)
		goto bad_with_put;

	if ((void *)handle_reply + sizeof (struct handle_reply) > up->nfs_end_area)
		printf ("%g: Tamanho do RPC insuficiente\n");

	memmove (&root_handle, &handle_reply->handle, sizeof (FHANDLE));

	put_it_block (up->nfs_bp);

	/*
	 ******	Terceira parte: Obter a porta NFS ***********************
	 */
	up->nfs_xid = (nfs_req_source++ << XID_PROC_SHIFT) | index;

   /***	up->nfs_timeout = mntent_ptr->me_timeout;	***/
	up->nfs_transmitted = 0;

    get_nfs_port_again:
	bp = get_it_block (IT_OUT_DATA);

	ptr = msg = (int *)(bp->it_frame + RESSZ);

	ptr = nfs_store_rpc_values
	(
		ptr, 11, PMAP_PROG, PMAP_VERS, PMAPPROC_GETPORT, AUTH_NULL, 0, AUTH_NULL, 0,
		NFS_PROG, NFS_VERS, UDP_PROTO, 0
	);

	uep.up_snd_addr = nfssp->sb_server_addr;
	uep.up_snd_port = PMAP_PORT;

	uep.up_my_addr  = 0;		/* Calcula o endereço local */
	uep.up_my_port	= NFS_CLIENT_PORT;

	bp->it_u_area   = msg;
	bp->it_u_count  = (int)ptr - (int)msg;

	EVENTCLEAR (&up->nfs_event);

	up->nfs_sp = nfssp;
	CLEAR (&up->nfs_lock);
	up->nfs_transmitted++;
	up->nfs_last_snd_time = time;

	send_udp_datagram (&uep, bp);

	EVENTWAIT (&up->nfs_event, PITNETIN);

	if   (up->nfs_status == 0)
		goto get_nfs_port_again;
	elif (up->nfs_status < 0)
		goto bad_with_error;

	/* Analisa a resposta */

	if ((void *)(port_reply = up->nfs_area) + sizeof (struct port_reply) > up->nfs_end_area)
		printf ("%g: Tamanho do RPC insuficiente\n");

	nfssp->sb_nfs_port = ENDIAN_LONG (port_reply->port);

#ifdef	DEBUG
	printf ("%g: nfs_port = %d\n", nfssp->sb_nfs_port);
#endif	DEBUG

	put_it_block (up->nfs_bp);

	/*
	 ****** Quarta parte: Obtém os atributos da RAIZ ****************
	 */
	up->nfs_xid = (nfs_req_source++ << XID_PROC_SHIFT) | index;

   /***	up->nfs_timeout = mntent_ptr->me_timeout;	***/
	up->nfs_transmitted = 0;

    get_stat_again:
	rp = nfs_setup_client_datagram_prolog (nfssp, &uep, NFSPROC_GETATTR, &bp);

	memmove (&rp->handle, &root_handle, sizeof (FHANDLE));

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
		goto get_stat_again;
	elif (up->nfs_status < 0)
		goto bad_with_error;

	/* Analisa a resposta */

	fattr_reply = up->nfs_area;

	if ((u.u_error = ENDIAN_LONG (fattr_reply->status)) != NFS_OK)
                goto bad_with_put;

	if ((void *)fattr_reply + sizeof (struct fattr_reply) > up->nfs_end_area)
		printf ("%g: Tamanho do RPC insuficiente\n");

	nfssp->sb_root_ino = ENDIAN_LONG (fattr_reply->fattr.fa_ino);

	if (nfs2_put_nfsdata (nfssp, nfssp->sb_root_ino, &root_handle, NODIRENT) == NONFSDATA)
		return (NONFSSB);

#ifdef	DEBUG
	printf
	(	"%g: count = %d, root_ino = %d\n",
		up->nfs_end_area - up->nfs_area, nfssp->sb_root_ino
	);
#endif	DEBUG

	put_it_block (up->nfs_bp);

	return (nfssp);

	/*
	 *	Em caso de erro, ...
	 */
    bad_with_error:
	u.u_error = up->nfs_error;
	free_byte (nfssp);
	return (NONFSSB);

    bad_with_put:
	free_byte (nfssp);
	put_it_block (up->nfs_bp);
	return (NONFSSB);

}	/* end nfs2_send_mntpgm_datagram */

/*
 ****************************************************************
 *	Envia um pedido de desmontagem ao servidor		*
 ****************************************************************
 */
void
nfs2c_send_umntpgm_datagram (const NFSSB *nfssp, const char *dir_nm)
{
	ITBLOCK		*bp;
	struct u_nfs	*up = &u.u_myself->u_nfs;
	int		*msg, *ptr;
   /***	const int	*roptr;	***/
	UDP_EP		uep;

	/*
	 *	Desmonta o sistema de arquivos
	 */
	up->nfs_xid = (nfs_req_source++ << XID_PROC_SHIFT) | (u.u_uprocv - scb.y_uproc);

	up->nfs_timeout	= nfssp->sb_sp->sb_mntent.me_timeout;
	up->nfs_transmitted = 0;

    get_umnt_again:
	bp = get_it_block (IT_OUT_DATA);

	ptr	= msg = (int *)(bp->it_frame + RESSZ);

	ptr	= nfs_store_rpc_values (ptr, 3, MOUNTPROG, MOUNTVERS, MOUNTPROG_UMNT);

	ptr	= nfs_put_auth_unix (ptr, 0, 0);	/* Autenticação UNIX */

	*ptr++	= ENDIAN_IMMED (AUTH_NULL);		/* Autenticação */
	*ptr++	= ENDIAN_IMMED (0);			/* Tamanho NULO */

	ptr	= nfs_put_xdr_name (ptr, dir_nm, strlen (dir_nm));

	uep.up_snd_addr = nfssp->sb_server_addr;
	uep.up_snd_port = nfssp->sb_mount_port;

	uep.up_my_addr  = 0;		/* Calcula o endereço local */
	uep.up_my_port	= NFS_CLIENT_PORT;

	bp->it_u_area   = msg;
	bp->it_u_count  = (int)ptr - (int)msg;

	EVENTCLEAR (&up->nfs_event);

	up->nfs_sp = nfssp;
	CLEAR (&up->nfs_lock);
	up->nfs_transmitted++;
	up->nfs_last_snd_time = time;

	send_udp_datagram (&uep, bp);

	EVENTWAIT (&up->nfs_event, PITNETIN);

	if   (up->nfs_status == 0)
		goto get_umnt_again;
	elif (up->nfs_status < 0)
		{ u.u_error = up->nfs_error; return; }

	/* Analisa a resposta: NÃO há código de retorno */

   /***	roptr = up->nfs_area; ***/

   /***	u.u_error = ENDIAN_LONG (*roptr); ***/

	put_it_block (up->nfs_bp);

}	/* end nfs2c_send_umntpgm_datagram */

/*
 ****************************************************************
 *	Obtém a estatística do sistema de arquivos NFS		*
 ****************************************************************
 */
void
nfs2_get_ustat (const SB *sp, USTAT *ustatp)
{
	NFSSB		*nfssp = sp->sb_ptr;
	struct arg { FHANDLE handle; }							*rp;
	const struct reply { int status; ulong tsize, bsize, blocks, bfree, bavail; }	*reply;
	struct u_nfs	*up = &u.u_myself->u_nfs;
	const NFSDATA	*dp;
	ITBLOCK		*bp;
	UDP_EP		uep;

	/*
	 *	Obtém o HANDLE da raiz
	 */
	if ((dp = nfs2_get_nfsdata (nfssp, nfssp->sb_root_ino)) == NONFSDATA)
		{ u.u_error = ENOTMNT; return; }

	/*
	 *	Envia o pedido para o servidor
	 */
	up->nfs_xid = (nfs_req_source++ << XID_PROC_SHIFT) | (u.u_uprocv - scb.y_uproc);

	up->nfs_timeout	= sp->sb_mntent.me_timeout;
	up->nfs_transmitted = 0;

    get_ustat_again:
	rp = nfs_setup_client_datagram_prolog (nfssp, &uep, NFSPROC_STATFS, &bp);

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
		goto get_ustat_again;
	elif (up->nfs_status < 0)
		{ u.u_error = up->nfs_error; return; }

	/* Analisa a resposta */

	reply = up->nfs_area;

	if ((u.u_error = ENDIAN_LONG (reply->status)) != NFS_OK)
		{ put_it_block (up->nfs_bp); return; }

	if ((void *)reply + sizeof (struct reply) > up->nfs_end_area)
		printf ("%g: Tamanho do RPC insuficiente\n");

	/*
	 *	Preenche a estrutura USTAT
	 */
	ustatp->f_type		= FS_NFS2;
	ustatp->f_sub_type	= 0;
	ustatp->f_bsize		= ENDIAN_LONG (reply->bsize);
	ustatp->f_nmsize	= 0;
	ustatp->f_fsize		= ENDIAN_LONG (reply->blocks);
	ustatp->f_tfree		= ENDIAN_LONG (reply->bfree);
	ustatp->f_isize		= 0;
	ustatp->f_tinode	= 0;
	ustatp->f_fname[0]	= '\0';
	ustatp->f_fpack[0]	= '\0';
	ustatp->f_m		= 1;
	ustatp->f_n		= 1;
	ustatp->f_symlink_sz	= 0;

	put_it_block (up->nfs_bp);

}	/* end nfs2_get_ustat */
