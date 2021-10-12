/*
 ****************************************************************
 *								*
 *			nfs2s_tree.c				*
 *								*
 *	Servidor NFS Versão 2: Manipulação de árvores		*
 *								*
 *	Versão	4.8.0, de 27.08.05				*
 *		4.8.0, de 23.09.05				*
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
 ****************************************************************
 *	Servidor NFS Versão 2: Processa o CREATE		*
 ****************************************************************
 */
void
nfs2_server_create (const RPCINFO *info)
{
	const struct arg { FHANDLE dir; int name[1]; /* SATTR */ }	*rp = info->f_area;
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
	 *	Por enquanto só aceitando arquivos regulares
	 */
	ap = nfs_get_xdr_name (rp->name, &de);

	mode = ENDIAN_LONG (ap->sa_mode);

	if (!S_ISREG (mode))
		{ iput (pp); nfs_send_status_datagram (info, EISDIR); return; }

	/*
	 *	Verifica se já existe o arquivo
	 */
	if   ((*pp->i_fsp->fs_search_dir) (&io, &de, ICREATE) > 0)
	{
		if ((ip = iget (pp->i_dev, de.d_ino, 0)) == NOINODE)
			{ iput (pp); nfs_send_status_datagram (info, NFS_ERRNO); return; }

		iput (pp);

		if (!S_ISREG (ip->i_mode))
			{ iput (ip); nfs_send_status_datagram (info, EISDIR); return; }

		(*fstab[ip->i_sb->sb_code].fs_trunc_inode) (ip);
	}
	elif (u.u_error)
	{
		iput (pp); nfs_send_status_datagram (info, NFS_ERRNO); return;
	}
	elif ((ip = (*pp->i_fsp->fs_make_inode) (&io, &de, mode)) == NOINODE)
	{
		/* "fs_make_inode" dá iput (pp); */

		nfs_send_status_datagram (info, NFS_ERRNO); return;
	}

	ip->i_uid = info->f_uid;
	ip->i_gid = info->f_gid;

	nfs2_put_sattr_attribute (ap, ip);

	/*
	 *	Obtém um ITBLOCK para compor a resposta
	 */
	bp = nfs_setup_server_datagram_prolog (info, &uep); reply = bp->it_u_area;

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
   /***	bp->it_u_area   = reply;	***/
	bp->it_u_count  = sizeof (struct reply);

	send_udp_datagram (&uep, bp);

}	/* end nfs2_server_create */

/*
 ****************************************************************
 *	Servidor NFS Versão 2: Processa o LINK			*
 ****************************************************************
 */
void
nfs2_server_link (const RPCINFO *info)
{
	const struct arg { FHANDLE old_h; FHANDLE new_h; int new_nm[1]; } *rp = info->f_area;
	INODE		*old_ip, *new_pp;
	IOREQ		io;
	DIRENT		de;

	/*
	 *	Em primeiro lugar, localiza o arquivo "old"
	 */
	if ((old_ip = nfs2_get_handle_inode (&rp->old_h)) == NOINODE)
		{ nfs_send_status_datagram (info, NFS_ERRNO); return; }

	/*
	 *	NÃO é permitido criar um elo físico para um diretório
	 */
	if (S_ISDIR (old_ip->i_mode))
		{ iput (old_ip); nfs_send_status_datagram (info, EPERM); return; }

	SLEEPFREE (&old_ip->i_lock);

	/*
	 *	Examina o Diretório novo
	 */
	if ((new_pp = nfs2_get_handle_inode (&rp->new_h)) == NOINODE)
		{ nfs_send_status_datagram (info, NFS_ERRNO); goto bad; }

	if (nfs_access (new_pp, IWRITE, info) < 0)
		{ iput (new_pp); nfs_send_status_datagram (info, NFS_ERRNO); goto bad; }

	/*
	 *	Os 2 Nomes devem estar no mesmo FS
	 */
	if (new_pp->i_dev != old_ip->i_dev)
		{ iput (new_pp); nfs_send_status_datagram (info, EXDEV); goto bad; }

	/*
	 *	Prepara a escrita do novo elo físico
	 */
	io.io_ip	= new_pp;
   /***	io.io_dev	= new_pp->i_dev;	***/

	nfs_get_xdr_name (rp->new_nm, &de);

	if ((*new_pp->i_fsp->fs_search_dir) (&io, &de, ICREATE) > 0)
		{ iput (new_pp); nfs_send_status_datagram (info, EEXIST); goto bad; }

	/*
	 *	Escreve o novo elo físico no diretório
 	 */
	de.d_ino = old_ip->i_ino;

	(*io.io_ip->i_fsp->fs_write_dir) (&io, &de, old_ip, 0 /* NO rename */);

	/* "fs_write_dir" dá "iput (new_pp)" */

	if (u.u_error != NOERR)
		{ nfs_send_status_datagram (info, NFS_ERRNO); goto bad; }

	/*
	 *	Sucesso
	 */
	SLEEPLOCK (&old_ip->i_lock, PINODE);
	old_ip->i_nlink++; inode_dirty_inc (old_ip);
	iput (old_ip);

	/*
	 *	Envia um datagrama com NFS_OK
	 */
	nfs_send_status_datagram (info, NFS_OK);
	return;

	/*
	 *	Em caso de erro
	 */
    bad:
	SLEEPLOCK (&old_ip->i_lock, PINODE);
	iput (old_ip);

}	/* end nfs2_server_link */

/*
 ****************************************************************
 *	Servidor NFS Versão 2: Processa o REMOVE		*
 ****************************************************************
 */
void
nfs2_server_remove (const RPCINFO *info)
{
	const struct arg { FHANDLE file; int name[1]; }	*rp = info->f_area;
	INODE			*pp, *ip;
	IOREQ			io;
	DIRENT			de;

	/*
	 *	Em primeiro lugar, localiza o INODE (do pai)
	 */
	if ((pp = nfs2_get_handle_inode (&rp->file)) == NOINODE)
		{ nfs_send_status_datagram (info, NFS_ERRNO); return; }

	if (nfs_access (pp, IWRITE, info) < 0)
		{ iput (pp); nfs_send_status_datagram (info, NFS_ERRNO); return; }

	/*
	 *	Procura o identificador
	 */
   /***	io.io_fd	= ...;	***/
   /***	io.io_fp	= ...;	***/
	io.io_ip	= pp;
   /***	io.io_dev	= ...;	***/
   /***	io.io_area	= ...;	***/
   /***	io.io_count	= ...;	***/
   /***	io.io_offset	= ...;	***/
   /***	io.io_cpflag	= ...;	***/
   /***	io.io_rablock	= ...;	***/

	nfs_get_xdr_name (rp->name, &de);

	if ((*pp->i_fsp->fs_search_dir) (&io, &de, IDELETE) <= 0)
		{ iput (pp); nfs_send_status_datagram (info, ENOENT); return; }

	/*
	 *	Agora, tenta obter o INODE do filho
	 */
	if ((ip = iget (rp->file.h_dev, de.d_ino, 0)) == NOINODE)
		{ iput (pp); nfs_send_status_datagram (info, NFS_ERRNO); return; }

	/*
	 *	Não podemos mais remover diretórios ...
	 */
	if (S_ISDIR (ip->i_mode))
		{ iput (ip); iput (pp); nfs_send_status_datagram (info, EISDIR); return; }

	/*
	 *	Verifica se é um arquivo de um FS montado
	 */
	if (ip->i_dev != pp->i_dev ||  pseudo_search_dev (ip) != NODEV)
		{ iput (ip); iput (pp); nfs_send_status_datagram (info, EBUSY); return; }

	xuntext (ip, 0 /* no lock */);

	if ((ip->i_flags & ITEXT) && ip->i_nlink == 1)
		{ iput (ip); iput (pp); nfs_send_status_datagram (info, ETXTBSY); return; }

   /***	io.io_ip = pp;	***/

	(*pp->i_fsp->fs_clr_dir) (&io, &de, ip);

	ip->i_nlink--;
	inode_dirty_inc (ip);

	/*
	 *	Libera os INODEs
	 */
	iput (ip); iput (pp);

	/*
	 *	Envia um datagrama com NFS_OK
	 */
	nfs_send_status_datagram (info, NFS_OK);

}	/* end nfs2_server_remove */

/*
 ****************************************************************
 *	Servidor NFS Versão 2: Processa o RENAME		*
 ****************************************************************
 */
void
nfs2_server_rename (const RPCINFO *info)
{
	const struct arg { FHANDLE old_h; int old_nm[1]; /* FHANDLE new_h; int new_nm[1]; */ } *rp = info->f_area;
	const FHANDLE	*new_h;
	off_t		old_offset;
	int		old_count;
	INODE		*old_pp, *old_ip, *new_pp;
	IOREQ		io;
	DIRENT		de;

	/*
	 *	Em primeiro lugar, localiza o arquivo "old"
	 */
	if ((old_pp = nfs2_get_handle_inode (&rp->old_h)) == NOINODE)
		{ nfs_send_status_datagram (info, NFS_ERRNO); return; }

	if (nfs_access (old_pp, IWRITE, info) < 0)
		{ iput (old_pp); nfs_send_status_datagram (info, NFS_ERRNO); return; }

	SLEEPFREE (&old_pp->i_lock);

	io.io_ip  = old_pp;
   /***	io.io_dev = old_pp->i_dev;	***/

	new_h = nfs_get_xdr_name (rp->old_nm, &de);

	if ((*old_pp->i_fsp->fs_search_dir) (&io, &de, LSEARCH) <= 0)
		{ nfs_send_status_datagram (info, ENOENT); goto bad; }

	old_offset = io.io_offset_low;
	old_count  = io.io_count;

	if ((old_ip = iget (old_pp->i_dev, de.d_ino, 0)) == NOINODE)
		{ nfs_send_status_datagram (info, NFS_ERRNO); goto bad; }

#if (0)	/*******************************************************/
	/* Por enquanto, NÃO diretórios */

	if (S_ISDIR (old_ip->i_mode))
		{ iput (old_ip); nfs_send_status_datagram (info, EISDIR); goto bad; }
#endif	/*******************************************************/

	/*
	 *	Examina o Diretório novo
	 */
	if ((new_pp = nfs2_get_handle_inode (new_h)) == NOINODE)
		{ iput (old_ip); nfs_send_status_datagram (info, NFS_ERRNO); goto bad; }

	if (nfs_access (new_pp, IWRITE, info) < 0)
		{ iput (new_pp); iput (old_ip); nfs_send_status_datagram (info, NFS_ERRNO); goto bad; }

	if (new_pp->i_dev != old_pp->i_dev)
		{ iput (new_pp); iput (old_ip); nfs_send_status_datagram (info, EXDEV); goto bad; }

	/*
	 *	Prepara a escrita do novo elo físico
	 */
	io.io_ip  = new_pp;
   /***	io.io_dev = new_pp->i_dev;	***/

	nfs_get_xdr_name ((int *)&new_h[1], &de);

	if ((*new_pp->i_fsp->fs_search_dir) (&io, &de, ICREATE) > 0)
		{ iput (new_pp); iput (old_ip); nfs_send_status_datagram (info, EEXIST); goto bad; }

	/*
	 *	Escreve o novo elo físico no diretório
 	 */
	de.d_ino = old_ip->i_ino;

	(*io.io_ip->i_fsp->fs_write_dir) (&io, &de, old_ip, 1 /* YES, rename */);

	/* "fs_write_dir" dá "iput (new_pp)" */

	if (u.u_error != NOERR)
		{ iput (old_ip); nfs_send_status_datagram (info, NFS_ERRNO); goto bad; }

	/*
	 *	Remove o nome antigo
	 */
	io.io_ip	 = old_pp;
   /***	io.io_dev	 = old_pp->i_dev;	***/
	io.io_offset_low = old_offset;
	io.io_count	 = old_count;

	(*old_pp->i_fsp->fs_clr_dir) (&io, &de, old_ip);

	inode_dirty_inc (old_ip);

	iput (old_ip);

	/*
	 *	Sucesso
	 */
	SLEEPLOCK (&old_pp->i_lock, PINODE);
	iput (old_pp);

	/*
	 *	Envia um datagrama com NFS_OK
	 */
	nfs_send_status_datagram (info, NFS_OK);
	return;

	/*
	 *	Em caso de erro
	 */
    bad:
	SLEEPLOCK (&old_pp->i_lock, PINODE);
	iput (old_pp);

}	/* end nfs2_server_rename */
