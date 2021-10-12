/*
 ****************************************************************
 *								*
 *			nfs2s_mnt.c				*
 *								*
 *	Funções relacionadas com o NFS MOUNT PROGRAM		*
 *								*
 *	Versão	4.8.0, de 12.08.05				*
 *		4.8.0, de 13.09.05				*
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
#include "../h/scb.h"
#include "../h/sync.h"
#include "../h/region.h"

#include "../h/mntent.h"
#include "../h/inode.h"
#include "../h/sysdirent.h"
#include "../h/itnet.h"
#include "../h/nfs.h"
#include "../h/signal.h"
#include "../h/uproc.h"
#include "../h/uerror.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ******	Variáveis da tabela de montagem NFS *********************
 */
enum { NFS_MOUNT_INC = 16 };

entry EXPORT	*nfs_mount;	/* Endereço da tabela de montagem NFS */

entry int	nfs_mount_sz;	/* Tamanho total da tabela NFS */
entry int	nfs_mount_len;	/* No. de entradas usadas */

/*
 ******	Protótipos de funções ***********************************
 */
void		nfs2_send_mntpgm_datagram (const RPCINFO *rp);
void		nfs2_send_umntpgm_datagram (const RPCINFO *rp);
void		print_called_proc (int proc);

/*
 ****************************************************************
 *	Recebe um pedido de porta (MOUNT PROGRAM)		*
 ****************************************************************
 */
void
nfs2_receive_mntpgm_datagram (const RPCINFO *info)
{
	/*
	 *	Verifica se a versão do MOUNT PROGRAM é a correta
	 */
	if (info->f_vers != MOUNTVERS)
	{
		printf ("%g: Recebi um pedido MNTPGM versão %d\n", info->f_vers);
		nfs_send_var_rpc_datagram
		(
			info, 7, RPC_REPLY, RPC_MSG_ACCEPTED, AUTH_NULL, 0,
			PROG_MISMATCH, MOUNTVERS, MOUNTVERS
		);
		return;
	}

	/*
	 *	Analisa a função pedida ao MOUNT PROGRAM
	 */
	switch (info->f_proc)
	{
 	    case MOUNTPROG_MNT:
		nfs2_send_mntpgm_datagram (info);
		return;

 	    case MOUNTPROG_UMNT:
		nfs2_send_umntpgm_datagram (info);
		return;

	    case MOUNTPROG_NULL:
 	    case MOUNTPROG_DUMP:
 	    case MOUNTPROG_UMNTALL:
	    default:
		printf ("%g: pedido MNTPGM %d AINDA NÃO IMPLEMENTADO\n", info->f_proc);
		nfs_send_var_rpc_datagram
		(
			info, 5, RPC_REPLY, RPC_MSG_ACCEPTED, AUTH_NULL, 0, PROG_UNAVAIL
		);
		return;
	}

}	/* end nfs2_receive_mntpgm_datagram */

/*
 ****************************************************************
 *	Envia uma resposta do MOUNT PROGRAM			*
 ****************************************************************
 */
void
nfs2_send_mntpgm_datagram (const RPCINFO *info)
{
	const struct arg { int dir_path[1]; }		*rp = info->f_area;
	struct reply { RPCPLG prolog; FHANDLE handle; }	*reply;
	const char	*dir_path = nfs_can_xdr_dirpath (rp->dir_path);
	int		len;
	char		*cp;
	ITBLOCK		*bp;
	INODE		*ip;
	EXPORT		*ep, *mp;
	const EXPORT	*end_mp;
	IOREQ		io;
	UDP_EP		uep;
	char		client_nm[NETNM_SZ];
	DIRENT		de;

	/*
	 *	Garante que sempre haja uma entrada vaga
	 */
	if (nfs_mount_len >= nfs_mount_sz)
	{
		nfs_mount_sz += NFS_MOUNT_INC;

		if ((nfs_mount = realloc_byte (nfs_mount, nfs_mount_sz * sizeof (EXPORT))) == NOEXPORT)
			{ nfs_send_status_datagram (info, ENOMEM); return; }

		memclr (nfs_mount + nfs_mount_sz - NFS_MOUNT_INC, NFS_MOUNT_INC * sizeof (EXPORT));
	}

	/*
	 *	Percorre a tabela NFS_MOUNT para procurar o endereço do servidor
	 */
	for (client_nm[0] = '\0', mp = nfs_mount, end_mp = mp + nfs_mount_len; mp < end_mp; mp++)
	{
		if (mp->e_client_addr != info->f_peer_addr)
			continue;

		strcpy (client_nm, mp->e_client_nm);

		if (streq (mp->e_dir_path, dir_path))
			break;

#if (0)	/*******************************************************/
		if (!streq (mp->e_dir_path, dir_path))
			continue;

		if (!mp->e_mounted)
			break;

		printf ("%g: O cliente \"%s\" já está montado em \"%s\"\n", client_nm, dir_path);
		nfs_send_status_datagram (info, EPERM);
		return;
#endif	/*******************************************************/
	}

	/*
	 *	O ponteiro "mp" está apontando para a entrada adequada de NFS_MOUNT
	 *
	 *	Obtém o nome do cliente, se for o caso
	 */
	if (client_nm[0] == '\0')
	{
		if (k_addr_to_node (info->f_peer_addr, client_nm, 1 /* do reverse */) < 0)
			{ nfs_send_status_datagram (info, EPERM); return; }
	}

	/*
	 *	Agora procura uma entrada adequada na tabela EXPORT
	 */
	for (ep = scb.y_export; /* abaixo */; ep++)
	{
		if (ep->e_client_nm[0] == '\0') 			/* A tabela acabou */
		{
			printf
			(	"NFS: Recusada a montagem de \"%s\" por \"%s\" (%s)\n",
				dir_path, info->f_client_nm, edit_ipaddr (info->f_peer_addr)
			);

			nfs_send_status_datagram (info, NFSERR_PERM);
			return;
		}

		/* Acrescenta o nome do "domain", se for o caso (e não tiver "?" ou "*") */

		for (cp = ep->e_client_nm; /* abaixo */; cp++)
		{
			if (*cp == '.' || *cp == '*' || *cp == '?')
				break;

			if (*cp != '\0')
				continue;

			*cp++ = '.'; strscpy (cp, domain_name, NETNM_SZ - (cp - ep->e_client_nm));
			break;
		}

		if (patmatch (client_nm, ep->e_client_nm))
		{
			if (streq (dir_path, ep->e_dir_path))
				break;
		}
	}

	/*
	 *	Prepara o resto da entrada
	 */
	memmove (mp, ep, sizeof (EXPORT));
	strcpy  (mp->e_client_nm, client_nm);
	mp->e_client_addr = info->f_peer_addr;

	if ((len = mp - nfs_mount + 1) > nfs_mount_len)
		nfs_mount_len = len;

	/*
	 *	Procura o diretório dado
	 */
	if ((ip = iname (dir_path, getschar, ISEARCH, &io, &de)) == NOINODE)
	{
		printf ("%g: NÃO consegui achar o diretório \"%s\"\n", dir_path);
		nfs_send_status_datagram (info, NFS_ERRNO);
		return;
	}

	/*
	 *	Obtém um ITBLOCK para compor a resposta
	 */
	bp = nfs_setup_server_datagram_prolog (info, &uep); reply = bp->it_u_area;

	/* O handle: NÃO precisa converter o ENDIAN */

	reply->handle.h_magic	= HANDLE_MAGIC;
	reply->handle.h_export	= ep - scb.y_export;
	reply->handle.h_index	= mp - nfs_mount;
	reply->handle.h_inode	= ip;
	reply->handle.h_dev	= ip->i_dev;
	reply->handle.h_ino	= ip->i_ino;

	iput (ip);

	/*
	 *	Envia a mensagem através do protocolo UDP
	 */
   /***	bp->it_u_area  = reply;	***/
	bp->it_u_count  = sizeof (struct reply);

	send_udp_datagram (&uep, bp);

	/*
	 *	Está montado
	 */
	mp->e_mounted = 1;

}	/* end nfs2_send_mntpgm_datagram */

/*
 ****************************************************************
 *	Envia uma resposta do UMOUNT PROGRAM			*
 ****************************************************************
 */
void
nfs2_send_umntpgm_datagram (const RPCINFO *info)
{
	const struct arg { int dir_path[1]; }		*rp = info->f_area;
	const char	*dir_path = nfs_can_xdr_dirpath (rp->dir_path);
	EXPORT		*mp;
	const EXPORT	*end_mp;

	/*
	 *	Procura a entrada correspondente na tabela NFS_MOUNT
	 */
	for (mp = nfs_mount, end_mp = mp + nfs_mount_len; /* abaixo */; mp++)
	{
		if (mp >= end_mp) 			/* A tabela acabou */
		{
			printf
			(	"%g: NÃO consegui achar o cliente \"%s\" (%s), dir \"%s\" na tabela\n",
				info->f_client_nm, edit_ipaddr (info->f_peer_addr), dir_path
			);

			nfs_send_status_datagram (info, ENOENT);
			return;
		}

		/* Compara o endereço IP e diretório */

		if (mp->e_client_addr == info->f_peer_addr && streq (mp->e_dir_path, dir_path))
			break;

	}	/* end for (entradas do vetor export) */

	/*
	 *	Achou
	 */
	mp->e_mounted = 0;

	nfs_send_status_datagram (info, NFS_OK);

}	/* end nfs2_send_umntpgm_datagram */

/*
 ****************************************************************
 *	Recebe um pedido de NFS					*
 ****************************************************************
 */
void
nfs2_receive_nfs_datagram (RPCINFO *info)
{
	const struct arg { FHANDLE name; }	*rp = info->f_area;
	EXPORT		*ep, *mp;
	char		*cp;
	int		old_nfs_mount_sz;

	/*
	 *	Verifica se a versão do NFS é a correta
	 */
	if (info->f_vers != NFS_VERS)
	{
		printf ("%g: Recebi um pedido para a Versão %d do NFS\n", info->f_vers);
		nfs_send_var_rpc_datagram
		(
			info, 7, RPC_REPLY, RPC_MSG_ACCEPTED, AUTH_NULL, 0,
			PROG_MISMATCH, NFS_VERS, NFS_VERS
		);
	}

	/*
	 *	Para este caso, nada verifica
	 */
	if (info->f_proc == NFSPROC_NULL)
	{
		nfs_send_status_datagram (info, NFS_OK);
		return;
	}

	/*
	 *	Obtém o índice na tabela EXPORT
	 *
	 *	Baseado no fato de que o primeiro argumento é sempre FHANDLE
	 */
	if (rp->name.h_magic != HANDLE_MAGIC)
	{
		printf ("%g: Número mágico de HANDLE inválido\n");
		nfs_send_status_datagram (info, NFSERR_STALE);
		return;
	}

	if ((unsigned)(info->f_export = rp->name.h_export) >= scb.y_n_export)
	{
		printf ("%g: Índice da tabela EXPORT inválido\n");
		nfs_send_status_datagram (info, NFSERR_STALE);
		return;
	}

	if ((unsigned)(info->f_index  = rp->name.h_index) >= 256)	/* Valor razoável */
	{
		printf ("%g: Índice da tabela NFS_MOUNT inválido\n");
		nfs_send_status_datagram (info, NFSERR_STALE);
		return;
	}

	/*
	 *	Lembrar o caso de "reboot": Verifica o tamanho da tabela NFS_MOUNT
	 */
	if (info->f_index >= nfs_mount_sz)
	{
		old_nfs_mount_sz = nfs_mount_sz; nfs_mount_sz = info->f_index + NFS_MOUNT_INC;

		if ((nfs_mount = realloc_byte (nfs_mount, nfs_mount_sz * sizeof (EXPORT))) == NOEXPORT)
			{ nfs_send_status_datagram (info, ENOMEM); return; }

		memclr
		(	&nfs_mount[old_nfs_mount_sz],
			(nfs_mount_sz - old_nfs_mount_sz) * sizeof (EXPORT)
		);
	}

	mp = &nfs_mount[info->f_index];

	if (mp->e_client_addr == info->f_peer_addr)		/* Lembrar o caso do "reboot" */
		goto found;

	if (mp->e_client_addr != 0)
	{
		printf ("%g: Endereço do cliente inválido (%s :", edit_ipaddr (info->f_peer_addr));
		printf (": %s)\n", edit_ipaddr (mp->e_client_addr));
		nfs_send_status_datagram (info, NFSERR_STALE);
		return;
	}

	/*
	 *	Processa o "reboot"
	 */
	ep = &scb.y_export[info->f_export];

	memmove (mp, ep, sizeof (EXPORT));
	mp->e_client_addr = info->f_peer_addr;

	if (k_addr_to_node (info->f_peer_addr, mp->e_client_nm, 1 /* do reverse */) < 0)
		{ nfs_send_status_datagram (info, EPERM); return; }

	for (cp = ep->e_client_nm; /* abaixo */; cp++)
	{
		if (*cp == '.' || *cp == '*' || *cp == '?')
			break;

		if (*cp != '\0')
			continue;

		*cp++ = '.'; strscpy (cp, domain_name, NETNM_SZ - (cp - ep->e_client_nm));
		break;
	}

	if (!patmatch (mp->e_client_nm, ep->e_client_nm))
	{
		printf ("%g: Nome do cliente inválido (%s :: %s)\n", mp->e_client_nm, ep->e_client_nm);
		nfs_send_status_datagram (info, NFSERR_STALE);
		return;
	}

	if (info->f_index + 1 > nfs_mount_len)
		nfs_mount_len = info->f_index + 1;

	mp->e_mounted = 1;

	/*
	 *	Prepara o UID e GID
	 */
    found:
	info->f_ronly = mp->e_ronly;

	if   (mp->e_use_anon_ids)
	{
		info->f_uid = mp->e_anon_uid;
		info->f_gid = mp->e_anon_gid;
	}
	elif (mp->e_all_squash)
	{
		info->f_uid = SQUASH_ID;
		info->f_gid = SQUASH_ID;
	}
	elif (!mp->e_no_root_squash)
	{
		if (info->f_uid == 0)
			info->f_uid = SQUASH_ID;

		if (info->f_gid == 0)
			info->f_gid = SQUASH_ID;
	}

#undef	DEBUG
#ifdef	DEBUG
	/*
	 *	Imprime o pedido recebido
	 */
	print_called_proc (info->f_proc);
#endif	DEBUG

	/*
	 *	Analisa a função pedida ao MOUNT PROGRAM
	 */
	switch (info->f_proc)
	{
 	    case NFSPROC_GETATTR:
		nfs2_server_getattr (info);
		break;

 	    case NFSPROC_SETATTR:
		nfs2_server_setattr (info);
		break;

 	    case NFSPROC_LOOKUP:
		nfs2_server_lookup (info);
		break;

 	    case NFSPROC_READLINK:
		nfs2_server_readlink (info);
		break;

 	    case NFSPROC_READ:
		nfs2_server_read (info);
		break;

 	    case NFSPROC_WRITE:
		nfs2_server_write (info);
		break;

 	    case NFSPROC_CREATE:
		nfs2_server_create (info);
		break;

 	    case NFSPROC_REMOVE:
		nfs2_server_remove (info);
		break;

 	    case NFSPROC_RENAME:
		nfs2_server_rename (info);
		break;

 	    case NFSPROC_LINK:
		nfs2_server_link (info);
		break;

 	    case NFSPROC_SYMLINK:
		nfs2_server_symlink (info);
		break;

 	    case NFSPROC_MKDIR:
		nfs2_server_mkdir (info);
		break;

 	    case NFSPROC_RMDIR:
		nfs2_server_rmdir (info);
		break;

 	    case NFSPROC_READDIR:
		nfs2_server_readdir (info);
		break;

 	    case NFSPROC_STATFS:
		nfs2_server_statfs (info);
		break;

	    default:
		printf ("%g: pedido NFS %d AINDA NÃO IMPLEMENTADO\n", info->f_proc);
		nfs_send_status_datagram (info, NFSERR_NXIO);
		break;
	}

}	/* end nfs2_receive_nfs_datagram */

#ifdef	DEBUG
/*
 ****************************************************************
 *	Imprime o pedido recebido				*
 ****************************************************************
 */
void
print_called_proc (int proc)
{
	printf (" ");

	switch (proc)
	{
 	    case NFSPROC_GETATTR:
		printf ("GETATTR");
		break;

 	    case NFSPROC_SETATTR:
		printf ("SETATTR");
		break;

 	    case NFSPROC_LOOKUP:
		printf ("LOOKUP");
		break;

 	    case NFSPROC_READLINK:
		printf ("READLINK");
		break;

 	    case NFSPROC_READ:
		printf ("READ");
		break;

 	    case NFSPROC_WRITE:
		printf ("WRITE");
		break;

 	    case NFSPROC_CREATE:
		printf ("CREATE");
		break;

 	    case NFSPROC_REMOVE:
		printf ("REMOVE");
		break;

 	    case NFSPROC_RENAME:
		printf ("RENAME");
		break;

 	    case NFSPROC_LINK:
		printf ("LINK");
		break;

 	    case NFSPROC_SYMLINK:
		printf ("SYMLINK");
		break;

 	    case NFSPROC_MKDIR:
		printf ("MKDIR");
		break;

 	    case NFSPROC_RMDIR:
		printf ("RMDIR");
		break;

 	    case NFSPROC_READDIR:
		printf ("READDIR");
		break;

 	    case NFSPROC_STATFS:
		printf ("STATFS");
		break;

	    default:
		printf ("%d", proc);
		break;
	}

}	/* end print_called_proc */
#endif	DEBUG
