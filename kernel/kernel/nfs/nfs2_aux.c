/*
 ****************************************************************
 *								*
 *			nfs_aux.c				*
 *								*
 *	Funções auxiliares do protocolo NFS			*
 *								*
 *	Versão	4.8.0, de 21.08.05				*
 *		4.8.0, de 21.08.05				*
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
#include "../h/sb.h"
#include "../h/inode.h"
#include "../h/stat.h"
#include "../h/sysdirent.h"
#include "../h/itnet.h"
#include "../h/nfs.h"
#include "../h/signal.h"
#include "../h/uproc.h"
#include "../h/uerror.h"

#include "../h/extern.h"
#include "../h/proto.h"

#include <stddef.h>
#include <stdarg.h>

/*
 ******	Conversão para o typo do arquivo ************************
 */
const char	nfs2_mode2type[] =
{
	NFNON, NFNON, NFCHR, NFNON, NFDIR, NFNON, NFBLK, NFNON, 
	NFREG, NFNON, NFLNK, NFNON, NFNON, NFNON, NFNON, NFNON, 
};

/*
 ******	Para o final de cadeias *********************************
 */
#ifdef  LITTLE_ENDIAN
const int	nfs_xdr_trim_str[4] = { 0, 0x000000FF, 0x0000FFFF, 0x00FFFFFF };
#else
const int	nfs_xdr_trim_str[4] = { 0, 0xFF000000, 0xFFFF0000, 0xFFFFFF00 };
#endif  LITTLE_ENDIAN

/*
 ****************************************************************
 *	Tenta obter rapidamente o INODE a partir de um HANDLE	*
 ****************************************************************
 */
INODE *
nfs2_get_handle_inode (const FHANDLE *hp)
{
	INODE		*ip;

	/*
	 *	Procura o arquivo dado: Primeiro caso com o ponteiro
	 */
	if
	(	(ip = hp->h_inode) >= scb.y_inode && ip < scb.y_endinode &&
		((int)ip - (int)scb.y_inode) % sizeof (INODE) == 0
	)
	{
		if (ip->i_dev == hp->h_dev && ip->i_ino == hp->h_ino)
		{
			SLEEPLOCK (&ip->i_lock, PINODE);

			if (ip->i_dev == hp->h_dev && ip->i_ino == hp->h_ino)
				{ iinc (ip); return (ip); }

			SLEEPFREE (&ip->i_lock);
		}
	}

	/*
	 *	Procura o arquivo dado: Segundo caso com o par (dev, ino)
	 */
	if ((ip = iget (hp->h_dev, hp->h_ino, INOMNT)) != NOINODE)
		return (ip);

	u.u_error = NFSERR_STALE;

	return (NOINODE);

}	/* end nfs2_get_handle_inode */

/*
 ****************************************************************
 *	Compõe um ATRIBUTO NFS					*
 ****************************************************************
 */
void
nfs2_get_fattr_attribute (FATTR *ap, const INODE *ip)
{
	ap->fa_type	  = ENDIAN_LONG  (nfs2_mode2type[(ip->i_mode >> 12) & 0x0F]);
	ap->fa_mode	  = ENDIAN_LONG  (ip->i_mode & 0xFFFF);
	ap->fa_nlink	  = ENDIAN_LONG  (ip->i_nlink);
	ap->fa_uid	  = ENDIAN_LONG  (ip->i_uid);
	ap->fa_gid	  = ENDIAN_LONG  (ip->i_gid);
	ap->fa_size	  = ENDIAN_LONG  (ip->i_size);
	ap->fa_blksz	  = ENDIAN_IMMED (BL4SZ);				/* É importante? */
	ap->fa_rdev	  = ENDIAN_LONG  (ip->i_rdev);
	ap->fa_blocks	  = ENDIAN_LONG  ((ip->i_size + BL4MASK) >> BL4SHIFT);	/* É importante? */ 
	ap->fa_dev	  = ENDIAN_LONG  (ip->i_dev);
	ap->fa_ino	  = ENDIAN_LONG  (ip->i_ino);

	ap->fa_atime.sec  = ENDIAN_LONG  (ip->i_atime);
	ap->fa_atime.usec = ENDIAN_IMMED (0);
	ap->fa_mtime.sec  = ENDIAN_LONG  (ip->i_mtime);
	ap->fa_mtime.usec = ENDIAN_IMMED (0); 
	ap->fa_ctime.sec  = ENDIAN_LONG  (ip->i_ctime);
	ap->fa_ctime.usec = ENDIAN_IMMED (0);

}	/* end nfs2_get_fattr_attribute */

/*
 ****************************************************************
 *	Atribui o INODE a parttir de um FATTR			*
 ****************************************************************
 */
const void *
nfs2_put_fattr_attribute (const FATTR *ap, INODE *ip)
{
	off_t		size;

	ip->i_mode	= ENDIAN_LONG (ap->fa_mode);
	ip->i_uid	= ENDIAN_LONG (ap->fa_uid);
	ip->i_gid	= ENDIAN_LONG (ap->fa_gid);
	ip->i_nlink	= ENDIAN_LONG (ap->fa_nlink);
	size		= ENDIAN_LONG (ap->fa_size);

	if (ip->i_size < size)
		ip->i_size = size;

   /***	ip->i_atime	= ...; ***/
   /***	ip->i_mtime	= ...; ***/
   /***	ip->i_ctime	= ...; ***/

	ip->i_rdev	= ENDIAN_LONG (ap->fa_rdev);

	inode_dirty_inc (ip);

	return (ap + 1);

}	/* end nfs2_put_fattr_attribute */

/*
 ****************************************************************
 *	Atribui os tempos do INODE				*
 ****************************************************************
 */
void
nfs2_put_time_attribute (const FATTR *ap, const NFSDATA *dp, INODE *ip)
{
	if (ip->i_mtime != 0)
		return;

	ip->i_atime	= ENDIAN_LONG (ap->fa_atime.sec);
	ip->i_mtime	= ENDIAN_LONG (ap->fa_mtime.sec);
	ip->i_ctime	= ENDIAN_LONG (ap->fa_ctime.sec);

	if (dp->d_mtime != 0)
                ip->i_mtime = dp->d_mtime;

	inode_dirty_inc (ip);

}	/* end nfs2_put_time_attribute */

/*
 ****************************************************************
 *	Atribui o INODE a parttir de um SATTR			*
 ****************************************************************
 */
void
nfs2_put_sattr_attribute (const SATTR *ap, INODE *ip)
{
	int		value;

	/*
	 *	Melhor deixar os tempos em último lugar
	 */
	if ((value = ENDIAN_LONG (ap->sa_size)) != -1)
	{
		if   (value == 0)
			(*fstab[ip->i_sb->sb_code].fs_trunc_inode) (ip);
		elif (value != ip->i_size)
			printf ("%g: Tentanto modificar o tamanho do arquivo\n");
	}

	if ((value = ENDIAN_LONG (ap->sa_mode)) != -1)
		ip->i_mode  = (ip->i_mode & ~S_IRWXA) | (value & S_IRWXA);

	if ((value = ENDIAN_LONG (ap->sa_uid)) != -1)
		ip->i_uid   = value;

	if ((value = ENDIAN_LONG (ap->sa_gid)) != -1)
		ip->i_gid   = value;

	if ((value = ENDIAN_LONG (ap->sa_atime.sec)) != -1)
		ip->i_atime = value;

	if ((value = ENDIAN_LONG (ap->sa_mtime.sec)) != -1)
		ip->i_mtime = value;

	inode_dirty_inc (ip);

}	/* end nfs2_put_sattr_attribute */

/*
 ****************************************************************
 *	Verifica se o acesso é válido				*
 ****************************************************************
 */
int
nfs_access (const INODE *ip, long mode, const RPCINFO *info)
{
	long		tst_mode;

	/*
	 *	O argumento "mode" deve conter apenas um dos bits IREAD, IWRITE ou IEXEC
	 */
	if (mode == IWRITE)
	{
		if (info->f_ronly)
			{ u.u_error = EROFS; return (-1); }

		if (ip->i_sb->sb_mntent.me_flags & SB_RONLY)
			{ u.u_error = EROFS; return (-1); }

		if (ip->i_flags & ITEXT)
			{ u.u_error = ETXTBSY; return (-1); }
	}

	/*
	 *	O caso especial do superusuário
	 */
	if (info->f_uid == 0)
	{
		if (mode == IEXEC && ip->i_mode & (S_IXUSR|S_IXGRP|S_IXOTH) == 0)
			{ u.u_error = EACCES; return (-1); }

		return (0);
	}

	/*
	 *	Verifica as Permissões
	 */
	tst_mode = mode >> (2 * IPSHIFT);

	if (info->f_gid == ip->i_gid)
		tst_mode |= mode >> IPSHIFT;

	if (info->f_uid == ip->i_uid)
		tst_mode |= mode;

	if ((ip->i_mode & tst_mode) == 0)
		{ u.u_error = EACCES; return (-1); }

	return (0);

}	/* end nfs_access */

/*
 ****************************************************************
 *	Verifica se é o dono do arquivo ou o superusuário	*
 ****************************************************************
 */
int
nfs_owner (const INODE *ip, const RPCINFO *info)
{
	if (info->f_uid == ip->i_uid)
		return (0);

	if (info->f_uid != 0)
		{ u.u_error = EPERM; return (-1); }

	return (0);

}	/* end nfs_owner */

/*
 ****************************************************************
 *	O servidor NFS está RO					*
 ****************************************************************
 */
void
nfs_server_is_rofs (SB *sp)
{
	if (sp->sb_mntent.me_flags & SB_RONLY)
		return;

	printf ("O Sistema de Arquivos \"%s\" permite apenas leituras\n", sp->sb_dev_nm);

	sp->sb_mntent.me_flags |= SB_RONLY;

}	/* end nfs_server_is_rofs */

/*
 ****************************************************************
 *	Obtém um nome para no formato XDR			*
 ****************************************************************
 */
const void *
nfs_get_xdr_name (const int *ptr, DIRENT *dep)
{
	int		len;

	if ((dep->d_namlen = len = ENDIAN_LONG (*ptr++)) > MAXNAMLEN)
		dep->d_namlen = MAXNAMLEN;

	memmove (dep->d_name, ptr, dep->d_namlen); dep->d_name[dep->d_namlen] = '\0';

	return (ptr + ((len + 3) >> 2));

}	/* end nfs_get_xdr_name */

/*
 ****************************************************************
 *	Converte um nome para o formato XDR			*
 ****************************************************************
 */
void *
nfs_put_xdr_name (int *ptr, const char *nm, int nm_len)
{
	*ptr++ = ENDIAN_LONG (nm_len);

	memmove (ptr, nm, nm_len);

	ptr += ((nm_len - 1) >> 2);

	if (nm_len & 3)
		*ptr &= nfs_xdr_trim_str[nm_len & 3];

	return (ptr + 1);

}	/* end nfs_put_xdr_name */

/*
 ****************************************************************
 *	Canoniza um caminho de montagem				*
 ****************************************************************
 */
const char *
nfs_can_xdr_dirpath (const int *ptr)
{
	int		len;
	char		*path;

	if ((len = ENDIAN_LONG (*ptr++)) > MAXNAMLEN)
		len = MAXNAMLEN;

	path = (char *)ptr; path[len] = '\0';

	return (path);

}	/* end nfs_can_xdr_dirpath */

/*
 ****************************************************************
 *	Coloca a autenticação do tipo UNIX			*
 ****************************************************************
 */
int *
nfs_put_auth_unix (int *ptr, int uid, int gid)
{
	int		len, *sz_place;

	/*
	 *	Completa a estrutura
	 */
	*ptr++ = ENDIAN_IMMED (AUTH_UNIX);	/* O tipo da autenticação */

	sz_place = ptr++;			/* Guarda o local do tamanho */

	*ptr++ = ENDIAN_LONG (time);		/* "stamp" */

	len = strlen (scb.y_uts.uts_nodename);

	ptr = nfs_put_xdr_name (ptr, scb.y_uts.uts_nodename, len);

	*ptr++ = ENDIAN_LONG (uid);		/* "uid"s e "gid"s */
	*ptr++ = ENDIAN_LONG (gid);
	*ptr++ = ENDIAN_IMMED (1);
	*ptr++ = ENDIAN_LONG (gid);

	*sz_place = ENDIAN_LONG ((int)ptr - (int)sz_place - sizeof (int));

	return (ptr);

}	/* end nfs_put_auth_unix */

/*
 ****************************************************************
 *	Envia uma mensagem de ERRO				*
 ****************************************************************
 */
void
nfs_send_status_datagram (const RPCINFO *info, int status)
{
	UDP_EP		uep;
	ITBLOCK		*bp;
	RPCPLG		*reply;

	/*
	 *	Obtém um ITBLOCK para compor a resposta
	 */
	bp = get_it_block (IT_OUT_DATA);

	reply = (RPCPLG *)(bp->it_frame + RESSZ);	/* >= ETH_H + IP_H + UDP_H */

	/* O Prólogo */

	reply->p_xid	 = info->f_xid;
	reply->p_mtype	 = ENDIAN_IMMED (RPC_REPLY); 
	reply->p_rstat	 = ENDIAN_IMMED (RPC_MSG_ACCEPTED);
	reply->p_verf[0] = ENDIAN_IMMED (AUTH_NULL);
	reply->p_verf[1] = ENDIAN_IMMED (0);
	reply->p_rpcstat = ENDIAN_IMMED (SUCCESS);

	/* Os dados da resposta */ 

	if (status < 0 /* NFS_ERRNO */ && (status = u.u_error) == NOERR)
		{ printf ("%g: Código de erro NÃO fornecido\n"); status = EIO; }

	reply->p_nfsstat = ENDIAN_LONG  (status);

	/*
	 *	Envia a mensagem através do protocolo UDP
	 */
	uep.up_snd_addr = info->f_peer_addr;
	uep.up_snd_port = info->f_peer_port;

	uep.up_my_addr  = 0;		/* Calcula o endereço local */
	uep.up_my_port	= RPC_PORT;

	bp->it_u_area   = reply;
	bp->it_u_count  = sizeof (RPCPLG);

	send_udp_datagram (&uep, bp);

}	/* end nfs_send_status_datagram */

/*
 ****************************************************************
 *	Prepara o envio de um datagrama correto			*
 ****************************************************************
 */
ITBLOCK *
nfs_setup_server_datagram_prolog (const RPCINFO *info, UDP_EP *uep)
{
	ITBLOCK		*bp;
	RPCPLG		*reply;

	/*
	 *	Obtém um ITBLOCK para compor a resposta
	 */
	bp = get_it_block (IT_OUT_DATA);

	reply = (RPCPLG *)(bp->it_frame + RESSZ);	/* >= ETH_H + IP_H + UDP_H */

	/* O Prólogo */

	reply->p_xid	 = info->f_xid;
	reply->p_mtype	 = ENDIAN_IMMED (RPC_REPLY); 
	reply->p_rstat	 = ENDIAN_IMMED (RPC_MSG_ACCEPTED);
	reply->p_verf[0] = ENDIAN_IMMED (AUTH_NULL);
	reply->p_verf[1] = ENDIAN_IMMED (0);
	reply->p_rpcstat = ENDIAN_IMMED (SUCCESS);
	reply->p_nfsstat = ENDIAN_IMMED (NFS_OK);

	/*
	 *	Prepara a mensagem para o protocolo UDP
	 */
	uep->up_snd_addr = info->f_peer_addr;
	uep->up_snd_port = info->f_peer_port;

	uep->up_my_addr  = 0;		/* Calcula o endereço local */
	uep->up_my_port	 = RPC_PORT;

	bp->it_u_area    = reply;
   /***	bp->it_u_count   = ...;		***/

	return (bp);

}	/* end nfs_setup_server_datagram_prolog */

/*
 ****************************************************************
 *	Prepara o envio de um datagrama para o servidor		*
 ****************************************************************
 */
void *
nfs_setup_client_datagram_prolog (const NFSSB *nfssp, UDP_EP *uep, int nfs_func, ITBLOCK **bp_ptr)
{
	ITBLOCK		*bp;
	int		*msg, *ptr;

	/*
	 *	Obtém um ITBLOCK para compor a resposta
	 */
	bp = get_it_block (IT_OUT_DATA);

	ptr = msg = (int *)(bp->it_frame + RESSZ);

	/* O Prólogo */

	*ptr++ = u.u_nfs.nfs_xid;			/* xid */
	*ptr++ = ENDIAN_IMMED (RPC_CALL);		/* É um pedido */
	*ptr++ = ENDIAN_IMMED (RPC_VERS);		/* Versão do RPC */
	*ptr++ = ENDIAN_IMMED (NFS_PROG);		/* No. do programa */
	*ptr++ = ENDIAN_IMMED (NFS_VERS);		/* Versão do NFS */
	*ptr++ = ENDIAN_LONG  (nfs_func);		/* Função desejada */

	 ptr   = nfs_put_auth_unix (ptr, u.u_euid, u.u_egid);	/* Autenticação UNIX */

	*ptr++ = ENDIAN_IMMED (AUTH_NULL);		/* Autenticação */
	*ptr++ = ENDIAN_IMMED (0);			/* Tamanho NULO */

	/*
	 *	Prepara a mensagem para o protocolo UDP
	 */
	uep->up_snd_addr = nfssp->sb_server_addr;
	uep->up_snd_port = nfssp->sb_nfs_port;

	uep->up_my_addr  = 0;		/* Calcula o endereço local */
	uep->up_my_port	 = NFS_CLIENT_PORT;

	bp->it_u_area    = msg;
   /***	bp->it_u_count   = ...;		***/

	*bp_ptr = bp;

	return (ptr);

}	/* end nfs_setup_client_datagram_prolog */

/*
 ****************************************************************
 *	Envia um datagrama RPC de erro				*
 ****************************************************************
 */
int *
nfs_store_rpc_values (int *ptr, int count, ...)
{
	va_list		ap;

	/*
	 *	Copia os valores da pilha
	 */
	va_start (ap, count);

	*ptr++ = u.u_nfs.nfs_xid;		/* O "xid" */
	*ptr++ = ENDIAN_IMMED (RPC_CALL);
	*ptr++ = ENDIAN_IMMED (RPC_VERS);

	while (count-- > 0)
		*ptr++ = ENDIAN_LONG (va_arg (ap, int));

	va_end (ap);

	return (ptr);

}	/* end nfs_send_var_rpc_datagram */

/*
 ****************************************************************
 *	Insere o HANDLE e ATTR no CACHE				*
 ****************************************************************
 */
NFSDATA *
nfs2_put_nfsdata (NFSSB *nfssp, ino_t ino, const FHANDLE *hp, const DIRENT *dep)
{
	NFSDATA		*ap, *dp;
	int		sz;

	/*
	 *	Calcula o (novo) tamanho
	 */
	if (dep != NODIRENT)
		sz = sizeof (NFSDATA) + (dep->d_namlen & ~3);
	else
		sz = offsetof (NFSDATA, d_name);

	/*
	 *	Procura a entrada correspondente ao INODE
	 */
	SPINLOCK (&nfssp->sb_data_lock);

	dp = nfssp->sb_nfsdata[ino & NFS_HASH_MASK];

	for (ap = NONFSDATA; /* abaixo */; ap = dp, dp = dp->d_next)
	{
		if (dp == NONFSDATA)			/* É uma entrada nova */
		{
			if ((dp = malloc_byte (sz)) == NONFSDATA)
				{ u.u_error = ENOMEM; SPINFREE (&nfssp->sb_data_lock); return (NONFSDATA); }

			memclr (dp, sz);

			dp->d_ino  = ino;
			dp->d_sz   = sz;

			if (ap == NONFSDATA)
				nfssp->sb_nfsdata[ino & NFS_HASH_MASK] = dp;
			else
				ap->d_next = dp;

			dp->d_next = NONFSDATA;
			break;
		}

		if (dp->d_ino == ino)			/* É uma entrada antiga */
		{
			if (dp->d_sz < sz)
			{
				/* Cuidado, pois "dp" pode mudar de endereço */

				if ((dp = realloc_byte (dp, sz)) == NONFSDATA)
				    { u.u_error = ENOMEM; SPINFREE (&nfssp->sb_data_lock); return (NONFSDATA); }

				if (ap == NONFSDATA)
					nfssp->sb_nfsdata[dep->d_ino & NFS_HASH_MASK] = dp;
				else
					ap->d_next = dp;

				dp->d_sz = sz;
			}

			break;
		}
	}

	/*
	 *	Completa a entrada
	 */
	if (hp != NOFHANDLE)
		{ memmove (&dp->d_handle, hp, sizeof (FHANDLE)); dp->d_handle_pres = 1; }

	if (dep != NODIRENT)
		{ dp->d_nm_len = dep->d_namlen; strcpy (dp->d_name, dep->d_name); }

	SPINFREE (&nfssp->sb_data_lock);
	return (dp);

}	/* end nfs2_put_nfsdata */

/*
 ****************************************************************
 *	Procura uma entrada do CACHE				*
 ****************************************************************
 */
NFSDATA *
nfs2_get_nfsdata (NFSSB *nfssp, ino_t ino)
{
	NFSDATA		*dp;

	/*
	 *	Procura a entrada do diretório
	 */
	SPINLOCK (&nfssp->sb_data_lock);

	dp = nfssp->sb_nfsdata[ino & NFS_HASH_MASK];

	for (/* acima */; dp != NONFSDATA; dp = dp->d_next)
	{
		if (dp->d_ino == ino)
			break;
	}

	SPINFREE (&nfssp->sb_data_lock);
	return (dp);

}	/* end nfs2_get_nfsdata */

/*
 ****************************************************************
 *	Remove uma entrada do CACHE				*
 ****************************************************************
 */
void
nfs2_remove_nfsdata (NFSSB *nfssp, ino_t ino)
{
	NFSDATA		*ap, *dp, *next_dp;

	/*
	 *	Procura e remove a entrada do diretório
	 */
	SPINLOCK (&nfssp->sb_data_lock);

	dp = nfssp->sb_nfsdata[ino & NFS_HASH_MASK];

	for (ap = NONFSDATA; dp != NONFSDATA; ap = dp, dp = next_dp)
	{
		next_dp = dp->d_next;

		if (dp->d_ino != ino)
			continue;

		free_byte (dp);

		if (ap == NONFSDATA)
			nfssp->sb_nfsdata[ino & NFS_HASH_MASK] = next_dp;
		else
			ap->d_next = next_dp;

		break;
	}

	SPINFREE (&nfssp->sb_data_lock);

}	/* end nfs2_remove_nfsdata */

/*
 ****************************************************************
 *	Remove o CACHE NFS de um SB				*
 ****************************************************************
 */
void
nfs2_free_all_nfsdata (NFSSB *nfssp)
{
	NFSDATA		*dp, *next_dp;
	int		index;

   /***	SPINLOCK (&nfssp->sb_data_lock); ***/

	for (index = 0; index < NFS_HASH_SZ; index++)
	{
		for (dp = nfssp->sb_nfsdata[index]; dp != NONFSDATA; dp = next_dp)
			{ next_dp = dp->d_next; free_byte (dp); }

	   /***	nfssp->sb_nfsdata[index] = NONFSDATA; ***/
	}

   /***	SPINFREE (&nfssp->sb_data_lock); ***/

}	/* end nfs2_free_all_nfsdata */
