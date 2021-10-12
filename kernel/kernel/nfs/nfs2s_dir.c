/*
 ****************************************************************
 *								*
 *			nfs2s_dir.c				*
 *								*
 *	Servidor NFS Versão 2: Manipulação de diretórios	*
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

#include "../h/mntent.h"
#include "../h/sb.h"
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
 ******	Definições globais **************************************
 */
enum {	NDIRENT = 15 };		/* Em torno de 4 KB */

#define	ENTRYSZ(nmsz)	(sizeof (ino_t) + sizeof (int) + ((nmsz + 4) & ~3) + sizeof (off_t))

/*
 ****************************************************************
 *	Servidor NFS Versão 2: Processa o READDIR		*
 ****************************************************************
 */
void
nfs2_server_readdir (const RPCINFO *info)
{
	const struct arg { FHANDLE dir; off_t offset; int count; }	*rp = info->f_area;
	struct reply { RPCPLG prolog; }					*reply;
	INODE		*ip;
	ITBLOCK		*bp;
	const DIRENT	*dp, *end_dp;
	int		count, cnt, sz, *ptr, eof = 0;
	UDP_EP		uep;
	IOREQ		io;
	DIRENT		*dirent;

	/*
	 *	Em primeiro lugar, localiza o INODE
	 */
	if ((void *)rp + sizeof (struct arg) > info->f_end_area)
		{ nfs_send_status_datagram (info, EINVAL); return; }	/* Erro PROVISÓRIO */

	if ((ip = nfs2_get_handle_inode (&rp->dir)) == NOINODE)
		{ nfs_send_status_datagram (info, NFS_ERRNO); return; }

	if (nfs_access (ip, IREAD, info) < 0)
		{ iput (ip); nfs_send_status_datagram (info, NFS_ERRNO); return; }

	/*
	 *	Obtém um ITBLOCK para já ter um espaço para preparar as entradas do diretório
	 */
	if ((count = ENDIAN_LONG (rp->count)) > MAX_NFS2_DATA)
		count = MAX_NFS2_DATA;

	bp = nfs_setup_server_datagram_prolog (info, &uep); reply = bp->it_u_area;

	/* As entradas DIRENT */

	ptr = (int *)&reply[1];

	io.io_offset = ENDIAN_LONG (rp->offset);

	if ((dirent = malloc_byte (NDIRENT * sizeof (DIRENT))) == NODIRENT)
		{ iput (ip); nfs_send_status_datagram (info, ENOMEM); return; }

	/*
	 *	Começa a preparar as entradas
	 */
	for (dp = end_dp = NODIRENT; count > 0; dp++)
	{
		if (dp >= end_dp)
		{
		   /***	io.io_fd	= ...;	***/
		   /***	io.io_fp	= ...;	***/
			io.io_ip	= ip;
		   /***	io.io_dev	= ...;	***/
			io.io_area	= dirent;
			io.io_count	= NDIRENT * sizeof (DIRENT);
		   /***	io.io_offset	= ...;	(Veja acima) ***/
			io.io_cpflag	= SS;
		   /***	io.io_rablock	= ...;	***/

			if (ip->i_sb->sb_mntent.me_flags & SB_ATIME)
				{ ip->i_atime = time; inode_dirty_inc (ip); }

			(*ip->i_fsp->fs_read_dir) (&io);

			if ((cnt = NDIRENT * sizeof (DIRENT) - io.io_count) <= 0) /* No. de bytes fornecidos */
				{ eof = ENDIAN_IMMED (1); break; }

			dp = dirent; end_dp = (DIRENT *)((int)dp + cnt);
		}

		if ((sz = ENTRYSZ (dp->d_namlen)) > count)
			break;

		/* Compõe uma entrada */

		*ptr++ = ENDIAN_IMMED (1);		/* TRUE */

		*ptr++ = ENDIAN_LONG  (dp->d_ino);

		ptr    = nfs_put_xdr_name (ptr, dp->d_name, dp->d_namlen);

		if (&dp[1] >= end_dp)
			*ptr++ = ENDIAN_LONG (io.io_offset_low);
		else
			*ptr++ = ENDIAN_LONG (dp[1].d_offset);	/* Da entrada seguinte! */

		count -= sz;

	}	/* end for (dp = ...) */

	iput (ip);

	free_byte (dirent);

	*ptr++ = ENDIAN_IMMED (0);		/* FALSE */

	*ptr++ = eof;

	/*
	 *	Envia a mensagem através do protocolo UDP
	 */
   /***	bp->it_u_area   = reply;	***/
	bp->it_u_count  = (int)ptr - (int)reply;

	send_udp_datagram (&uep, bp);

}	/* end nfs2_server_readdir */

/*
 ****************************************************************
 *	Servidor NFS Versão 2: Processa o LOOKUP		*
 ****************************************************************
 */
void
nfs2_server_lookup (const RPCINFO *info)
{
	const struct arg { FHANDLE dir; int name[1]; }			*rp = info->f_area;
	struct reply { RPCPLG prolog; FHANDLE name; FATTR attr; }	*reply;
	INODE		*ip;
	ITBLOCK		*bp;
	UDP_EP		uep;
	IOREQ		io;
	DIRENT		de;

	/*
	 *	Em primeiro lugar, localiza o INODE
	 */
	if ((ip = nfs2_get_handle_inode (&rp->dir)) == NOINODE)
		{ nfs_send_status_datagram (info, NFS_ERRNO); return; }

	if (nfs_access (ip, IEXEC, info) < 0)
		{ iput (ip); nfs_send_status_datagram (info, NFS_ERRNO); return; }

	/*
	 *	Procura o identificador
	 */
   /***	io.io_fd	= ...;	***/
   /***	io.io_fp	= ...;	***/
	io.io_ip	= ip;
   /***	io.io_dev	= ...;	***/
   /***	io.io_area	= ...;	***/
   /***	io.io_count	= ...;	***/
   /***	io.io_offset	= ...;	***/
   /***	io.io_cpflag	= ...;	***/
   /***	io.io_rablock	= ...;	***/

	nfs_get_xdr_name (rp->name, &de);

	if ((*ip->i_fsp->fs_search_dir) (&io, &de, ISEARCH) <= 0)
		{ iput (ip); nfs_send_status_datagram (info, ENOENT); return; }

	iput (ip);

	/*
	 *	Agora, tenta obter o INODE do filho
	 */
	if ((ip = iget (rp->dir.h_dev, de.d_ino, INOMNT)) == NOINODE)
		{ nfs_send_status_datagram (info, NFS_ERRNO); return; }

#if (0)	/*******************************************************/
	if (ip->i_dev != rp->dir.h_dev)
		{ iput (ip); nfs_send_status_datagram (info, ENOTMNT); return; }
#endif	/*******************************************************/

	/*
	 *	Obtém um ITBLOCK para compor a resposta
	 */
	bp = nfs_setup_server_datagram_prolog (info, &uep); reply = bp->it_u_area;

	/* Os dados da resposta: o HANDLE e atributos */ 

	reply->name.h_magic	= HANDLE_MAGIC;
	reply->name.h_export	= info->f_export;
	reply->name.h_index	= info->f_index;
	reply->name.h_inode	= ip;
	reply->name.h_dev	= ip->i_dev;
	reply->name.h_ino	= ip->i_ino;

	nfs2_get_fattr_attribute (&reply->attr, ip);

	iput (ip);

	/*
	 *	Envia a mensagem através do protocolo UDP
	 */
   /***	bp->it_u_area  = reply;	***/
	bp->it_u_count = sizeof (struct reply);

	send_udp_datagram (&uep, bp);

}	/* end nfs2_server_lookup */

/*
 ****************************************************************
 *	Servidor NFS Versão 2: Processa o MKDIR			*
 ****************************************************************
 */
void
nfs2_server_mkdir (const RPCINFO *info)
{
	const struct arg { FHANDLE dir; int name[1]; /*** SATTR ***/ }	*rp = info->f_area;
	struct reply { RPCPLG prolog; FHANDLE file; FATTR attr; }	*reply;
	INODE		*pp, *ip;
	ITBLOCK		*bp;
	int		mode;
	const SATTR	*ap;
	UDP_EP		uep;
	IOREQ		io;
	DIRENT		de;

	/*
	 *	Em primeiro lugar, localiza o INODE do diretório PAI
	 */
	if ((io.io_ip = pp = nfs2_get_handle_inode (&rp->dir)) == NOINODE)
		{ nfs_send_status_datagram (info, NFS_ERRNO); return; }

	if (nfs_access (pp, IWRITE, info) < 0)
		{ iput (pp); nfs_send_status_datagram (info, NFS_ERRNO); return; }

	/*
	 *	Procura o identificador
	 */
	ap = nfs_get_xdr_name (rp->name, &de);

	if ((*pp->i_fsp->fs_search_dir) (&io, &de, ICREATE) > 0)
		{ iput (pp); nfs_send_status_datagram (info, EEXIST); return; }

	if (u.u_error)
		{ iput (pp); nfs_send_status_datagram (info, NFS_ERRNO); return; }

	/*
	 *	Tenta criar o diretório
	 */
	mode = IFDIR | (ENDIAN_LONG (ap->sa_mode) & S_IRWXA);

   /***	io.io_dev = pp->i_dev;	***/

	if ((ip = (*pp->i_fsp->fs_make_dir) (&io, &de, mode)) == NOINODE)
		{ nfs_send_status_datagram (info, NFS_ERRNO); return; }

	ip->i_uid = info->f_uid;
	ip->i_gid = info->f_gid;

	nfs2_put_sattr_attribute (ap, ip);

	/*
	 *	Obtém um ITBLOCK para compor a resposta
	 */
	bp = nfs_setup_server_datagram_prolog (info, &uep); reply = bp->it_u_area;

	/* Os dados da resposta */ 

	reply->file.h_magic	= HANDLE_MAGIC;
	reply->file.h_export	= info->f_export;
	reply->file.h_index	= info->f_index;
	reply->file.h_inode	= ip;
	reply->file.h_dev	= ip->i_dev;
	reply->file.h_ino	= ip->i_ino;

	nfs2_get_fattr_attribute (&reply->attr, ip);

	iput (ip);

	/*
	 *	Envia a mensagem através do protocolo UDP
	 */
   /***	bp->it_u_area  = reply;	***/
	bp->it_u_count = sizeof (struct reply);

	send_udp_datagram (&uep, bp);

}	/* end nfs2_server_mkdir */

/*
 ****************************************************************
 *	Servidor NFS Versão 2: Processa o RMDIR			*
 ****************************************************************
 */
void
nfs2_server_rmdir (const RPCINFO *info)
{
	const struct arg { FHANDLE dir; int name[1]; }	*rp = info->f_area;
	struct reply { RPCPLG prolog; }	/***		*reply ***/;
	int		code;
	INODE		*pp, *ip;
	IOREQ		io;
	DIRENT		de;

	/*
	 *	Em primeiro lugar, localiza o INODE do diretório PAI
	 */
	if ((pp = nfs2_get_handle_inode (&rp->dir)) == NOINODE)
		{ code = NFS_ERRNO; goto out0; }

	if (nfs_access (pp, IWRITE, info) < 0)
		{ iput (pp); nfs_send_status_datagram (info, NFS_ERRNO); return; }

	io.io_ip = pp;

	nfs_get_xdr_name (rp->name, &de);

	if ((*pp->i_fsp->fs_search_dir) (&io, &de, IDELETE) <= 0)
		{ code = ENOENT; goto out1; }

	/*
	 *	Verifica se por acaso, não está tentado remover "."
	 */
	if (pp->i_ino == de.d_ino)
		{ code = EINVAL; goto out1; }

	/*
	 *	Obtem o filho (que deve ser apagado).
	 */
	if ((ip = iget (pp->i_dev, de.d_ino, 0)) == NOINODE)
		{ code = ENOENT; goto out1; }

	if (!S_ISDIR (ip->i_mode))
		{ code = ENOTDIR; goto out2; }

	/*
	 *	Verifica se é o diretório corrente
	 */
	if (ip == u.u_cdir)
		{ code = EINVAL; goto out2; }

	/*
	 *	Verifica se é um arquivo de um FS montado
	 */
	if (ip->i_dev != pp->i_dev)
		{ code = EBUSY; goto out2; }

	/*
	 *	Verifica se o diretório está vazio
	 */
	if ((*ip->i_fsp->fs_empty_dir) (ip) <= 0)
		{ code = NFSERR_NOTEMPTY; goto out2; }

	/*
	 *	Apaga a entrada do diretório
	 */
	(*ip->i_fsp->fs_clr_dir) (&io, &de, ip);

	pp->i_nlink--;
	ip->i_nlink -= 2;

	/*
	 *	Libera os INODEs
	 */
	iput (ip); iput (pp);

	/*
	 *	Envia um datagrama com NFS_OK
	 */
	nfs_send_status_datagram (info, NFS_OK);
	return;

	/*
	 *	Em caso e erro, ...
	 */
    out2:
	iput (ip);
    out1:
	iput (pp);
    out0:
	nfs_send_status_datagram (info, code);

}	/* end nfs2_server_rmdir */
