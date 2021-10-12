/*
 ****************************************************************
 *								*
 *			nfs2s_read.c				*
 *								*
 *	Servidor NFS Versão 2: Leitura e escrita de arquivos	*
 *								*
 *	Versão	4.8.0, de 27.08.05				*
 *		4.8.0, de 27.08.05				*
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
#include "../h/region.h"

#include "../h/inode.h"
#include "../h/stat.h"
#include "../h/sysdirent.h"
#include "../h/itnet.h"
#include "../h/nfs.h"
#include "../h/signal.h"
#include "../h/uerror.h"
#include "../h/uproc.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Servidor NFS Versão 2: Processa o READ			*
 ****************************************************************
 */
void
nfs2_server_read (const RPCINFO *info)
{
	const struct arg { FHANDLE file; off_t offset; int count; }		*rp = info->f_area;
	struct reply { RPCPLG prolog; FATTR attr; int count; /* dados */ }	*reply;
	INODE		*ip;
	ITBLOCK		*bp;
	int		count;
	UDP_EP		uep;
	IOREQ		io;

	/*
	 *	Em primeiro lugar, verifica o tamanho e localiza o INODE
	 */
	if ((count = ENDIAN_LONG (rp->count)) > MAX_NFS2_DATA)
	{
		printf ("%g: O nó \"%s\" pediu uma leitura de %d bytes\n", info->f_client_nm, count);
		nfs_send_status_datagram (info, EFBIG);
		return;
	}

	if ((ip = nfs2_get_handle_inode (&rp->file)) == NOINODE)
		{ nfs_send_status_datagram (info, NFS_ERRNO); return; }

	if (!S_ISREG (ip->i_mode) && !S_ISDIR (ip->i_mode))
		{ iput (ip); nfs_send_status_datagram (info, EIO); return; }

	if (nfs_access (ip, IREAD, info) < 0)
		{ iput (ip); nfs_send_status_datagram (info, NFS_ERRNO); return; }

	/*
	 *	Obtém um ITBLOCK para compor a resposta
	 */
	bp = nfs_setup_server_datagram_prolog (info, &uep); reply = bp->it_u_area;

	/* Efetua a leitura */

   /***	io.io_fd	 = ...;		***/
   /***	io.io_fp	 = ...;		***/
	io.io_ip	 = ip;
   /***	io.io_dev	 = ...;		***/
	io.io_area	 = &reply[1];
	io.io_count	 = count;
	io.io_offset_low = ENDIAN_LONG (rp->offset);
	io.io_cpflag	 = SS;
   /***	io.io_rablock	 = ...;		***/

	(*ip->i_fsp->fs_read) (&io);

	if (u.u_error != NOERR)
		{ iput (ip); nfs_send_status_datagram (info, NFS_ERRNO); return; }

	/* Coloca os atributos */

	nfs2_get_fattr_attribute (&reply->attr, ip);

	iput (ip);

	count	       -= io.io_count;

	reply->count	= ENDIAN_LONG (count);

   /***	bp->it_u_area   = reply;	***/
	bp->it_u_count  = sizeof (struct reply) + ROUND_XDR (count);

	send_udp_datagram (&uep, bp);

}	/* end nfs2_server_read */

/*
 ****************************************************************
 *	Servidor NFS Versão 2: Processa o WRITE			*
 ****************************************************************
 */
void
nfs2_server_write (const RPCINFO *info)
{
	const struct arg { FHANDLE file; int d1; off_t offset; int d2; int count; int data[1]; }
										*rp = info->f_area;
	struct reply { RPCPLG prolog; FATTR attr; }		*reply;
	INODE		*ip;
	ITBLOCK		*bp;
	int		count;
	UDP_EP		uep;
	IOREQ		io;

	/*
	 *	Em primeiro lugar, verifica o tamanho e localiza o INODE
	 */
	if ((count = ENDIAN_LONG (rp->count)) > MAX_NFS2_DATA)
	{
		printf ("%g: O nó \"%s\" pediu uma leitura de %d bytes\n", info->f_client_nm, count);
		nfs_send_status_datagram (info, EFBIG);
		return;
	}

	if ((ip = nfs2_get_handle_inode (&rp->file)) == NOINODE)
		{ nfs_send_status_datagram (info, NFS_ERRNO); return; }

	if (!S_ISREG (ip->i_mode))
		{ iput (ip); nfs_send_status_datagram (info, EISDIR); return; }

	if (nfs_access (ip, IWRITE, info) < 0)
		{ iput (ip); nfs_send_status_datagram (info, NFS_ERRNO); return; }

	/* Efetua a escrita */

   /***	io.io_fd	 = ...;		***/
   /***	io.io_fp	 = ...;		***/
	io.io_ip	 = ip;
   /***	io.io_dev	 = ...;		***/
	io.io_area	 = (void *)rp->data;
	io.io_count	 = count;
	io.io_offset_low = ENDIAN_LONG (rp->offset);
	io.io_cpflag	 = SS;
   /***	io.io_rablock	 = ...;		***/

	(*ip->i_fsp->fs_write) (&io);

	if (u.u_error != NOERR)
		{ iput (ip); nfs_send_status_datagram (info, NFS_ERRNO); return; }

	/*
	 *	Obtém um ITBLOCK para compor a resposta
	 */
	bp = nfs_setup_server_datagram_prolog (info, &uep); reply = bp->it_u_area;

	/* Os dados da resposta */ 

	nfs2_get_fattr_attribute (&reply->attr, ip);

	iput (ip);

	/*
	 *	Envia a mensagem através do protocolo UDP
	 */
   /***	bp->it_u_area   = reply;	***/
	bp->it_u_count  = sizeof (struct reply);

	send_udp_datagram (&uep, bp);

}	/* end nfs2_server_write */

/*
 ****************************************************************
 *	Servidor NFS Versão 2: Processa o READLINK		*
 ****************************************************************
 */
void
nfs2_server_readlink (const RPCINFO *info)
{
	const struct arg { FHANDLE file; }			*rp = info->f_area;
	struct reply { RPCPLG prolog; int count; /* data */ }	*reply;
	INODE		*ip;
	ITBLOCK		*bp;
	int		count;
	UDP_EP		uep;
	IOREQ		io;

	/*
	 *	Em primeiro lugar, localiza o INODE
	 */
	if ((ip = nfs2_get_handle_inode (&rp->file)) == NOINODE)
		{ nfs_send_status_datagram (info, NFS_ERRNO); return; }

	if (!S_ISLNK (ip->i_mode))
		{ iput (ip); nfs_send_status_datagram (info, EIO); return; }

#if (0)	/*******************************************************/
	if (nfs_access (ip, IREAD, info) < 0)
		{ iput (ip); nfs_send_status_datagram (info, NFS_ERRNO); return; }
#endif	/*******************************************************/

	/*
	 *	Obtém um ITBLOCK para compor a resposta
	 */
	bp = nfs_setup_server_datagram_prolog (info, &uep); reply = bp->it_u_area;

	/* Efetua a leitura */

   /***	io.io_fd	 = ...;		***/
   /***	io.io_fp	 = ...;		***/
	io.io_ip	 = ip;
   /***	io.io_dev	 = ...;		***/
	io.io_area	 = &reply[1];
	io.io_count	 = MAX_NFS2_DATA;
   /***	io.io_offset_low = ...;		***/
	io.io_cpflag	 = SS;
   /***	io.io_rablock	 = ...;		***/

	if ((count = (*ip->i_fsp->fs_read_symlink) (&io)) <= 0)
	{
		/* ..............; */
	}

	iput (ip);

	reply->count	= ENDIAN_LONG (count - 1);

	/*
	 *	Envia a mensagem através do protocolo UDP
	 */
   /***	bp->it_u_area   = reply;	***/
	bp->it_u_count  = sizeof (struct reply) + ROUND_XDR (count - 1);

	send_udp_datagram (&uep, bp);

}	/* end nfs2_server_readlink */

/*
 ****************************************************************
 *	Servidor NFS Versão 2: Processa o SYMLINK		*
 ****************************************************************
 */
void
nfs2_server_symlink (const RPCINFO *info)
{
	const struct arg { FHANDLE dir; int name[1]; /* int path[1], sattr */ }	*rp = info->f_area;
	struct reply { RPCPLG prolog; }	/*** *reply ***/;
	const int	*path;
	int		len;
	INODE		*pp, *ip;
	const SATTR	*ap;
	IOREQ		io;
	DIRENT		de;

	/*
	 *	Em primeiro lugar, localiza o INODE do diretório PAI
	 */
	if ((io.io_ip = pp = nfs2_get_handle_inode (&rp->dir)) == NOINODE)
		{ nfs_send_status_datagram (info, NFS_ERRNO); return; }

	/*
	 *	Verifica se já existe o arquivo
	 */
	path = nfs_get_xdr_name (rp->name, &de);

	if ((*pp->i_fsp->fs_search_dir) (&io, &de, ICREATE) > 0)
		{ iput (pp); nfs_send_status_datagram (info, EEXIST); return; }

	if (u.u_error != NOERR)
		{ iput (pp); nfs_send_status_datagram (info, NFS_ERRNO); return; }

	if (nfs_access (pp, IWRITE, info) < 0)
		{ iput (pp); nfs_send_status_datagram (info, NFS_ERRNO); return; }

	/*
	 *	Cria o elo simbólico
	 */
	len = ENDIAN_LONG (*path++); ((char *)path)[len] = '\0';

	ap = (SATTR *)(path + ((len + 3) >> 2));

	if ((ip = (*pp->i_fsp->fs_write_symlink) ((char *)path, len + 1, &io, &de)) == NOINODE)
		{ nfs_send_status_datagram (info, NFS_ERRNO); return; }

	/* "fs_write_symlink" dá iput (pp); */

	ip->i_uid = info->f_uid;
	ip->i_gid = info->f_gid;

	nfs2_put_sattr_attribute (ap, ip);

	iput (ip);

	/*
	 *	Envia um datagrama com NFS_OK
	 */
	nfs_send_status_datagram (info, NFS_OK);

}	/* end nfs2_server_symlink */
