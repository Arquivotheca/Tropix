/*
 ****************************************************************
 *								*
 *			nfs2c_read.c				*
 *								*
 *	Leitura e escrita de arquivos NFS			*
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
#include "../h/nfs.h"
#include "../h/kfile.h"
#include "../h/inode.h"
#include "../h/sysdirent.h"
#include "../h/devhead.h"
#include "../h/bhead.h"
#include "../h/signal.h"
#include "../h/uproc.h"
#include "../h/uerror.h"

#include <stddef.h>

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Variáveis e áreas globais				*
 ****************************************************************
 */
#define BL4toBL(x)   ((x) << (BL4SHIFT - BLSHIFT))

#define b_nfssp		b_retry_cnt	/* Para "strategy" */
#define b_inode		b_ptr		/* Para "strategy" */
#define b_quantum	b_residual	/* Para "strategy" */

entry DEVHEAD		nfs_tab;

/*
 ******	Protótipos de funções ***********************************
 */
void		nfs2_strategy (BHEAD *bp);
void		nfs2_read_strategy (BHEAD *bp);
void		nfs2_write_strategy (BHEAD *bp);

void		nfs2_do_write (BHEAD *bp);

/*
 ****************************************************************
 *	Open de INODEs						*
 ****************************************************************
 */
void
nfs2_open (INODE *ip, int oflag)
{
	/*
	 *	Recebe e devolve o INODE locked
	 */
	if (ip->i_flags & ILOCK || (oflag & O_LOCK) && ip->i_count != 1)
		{ u.u_error = EBUSY; return; }

	if (oflag & O_LOCK)
		ip->i_flags |= ILOCK;

}	/* end nfs2_open */

/*
 ****************************************************************
 *	Close de INODEs						*
 ****************************************************************
 */
void
nfs2_close (INODE *ip)
{
	/*
	 *	Recebe o INODE locked e devolve free
	 */
	if (ip->i_count == 1)
	{
		/* O "close" final do INODE */

		if ((ip->i_mode & IFMT) == IFIFO)
			ip->i_mode |= IREAD|IWRITE;

		ip->i_flags &= ~ILOCK;
	}

	iput (ip);

}	/* end nfs2_close */

/*
 ****************************************************************
 *	Leitura de um INODE					*
 ****************************************************************
 */
void
nfs2_read (IOREQ *iop)
{
	INODE		*ip = iop->io_ip;
	dev_t		dev = ip->i_dev;
	BHEAD		*bp;
	daddr_t		lbn;
	int		count;
	off_t		range, offset;

	/*
	 *	Prepara algumas variáveis
	 */
	if (ip->i_sb->sb_mntent.me_flags & SB_ATIME)
		{ ip->i_atime = time; /*** inode_dirty_inc (ip); ***/ }

	/*
	 *	Trata-se de arquivo regular ou diretório
	 */
	do
	{
		lbn	= iop->io_offset_low >> BL4SHIFT;
		offset	= iop->io_offset_low &  BL4MASK;
		count	= MIN (BL4SZ - offset, iop->io_count);

		if ((range = ip->i_size - iop->io_offset_low) <= 0)
			return;

		if (count > range)
			count = range;

		/*
		 *	Lê um bloco: Repare o "endereço físico"
		 */
		bp = block_get (dev, BL4toBL (lbn), ip->i_ino);

		if (bp->b_quantum == 0 && offset != 0)
			memclr (bp->b_base_addr, BL4SZ);

		if (EVENTTEST (&bp->b_done) < 0)
		{
			bp->b_flags	|= B_READ;
			bp->b_inode	 = ip;
			bp->b_nfssp	 = (int)ip->i_sb->sb_ptr;

			nfs2_read_strategy (bp);

		   /***	EVENTWAIT (&bp->b_done, PBLKIO); ***/
		}
	
		geterror (bp);

		/*
		 *	Move a Informação para a area desejada
		 */
		if (unimove (iop->io_area, bp->b_base_addr + offset, count, iop->io_cpflag) < 0)
			{ u.u_error = EFAULT; block_put (bp); return; }

if (count != BL4SZ && count > bp->b_quantum)
{
NFSDATA *dp = nfs2_get_nfsdata (ip->i_sb->sb_ptr, bp->b_ino);

printf ("%g: count = %d, quantum = %d, offset = %d (%d)\n", count, bp->b_quantum, iop->io_offset_low, offset);
printf ("	i_size = %d, d_size = %d\n", ip->i_size, dp->d_size);
}

		if (count > bp->b_quantum - offset)
			count = bp->b_quantum - offset;

		iop->io_area       += count;
		iop->io_offset_low += count;
		iop->io_count      -= count;

		block_put (bp);
	}
	while (u.u_error == NOERR && iop->io_count > 0 && count > 0);

}	/* end nfs2_read */

/*
 ****************************************************************
 *	Escrita de um INODE					*
 ****************************************************************
 */
void
nfs2_write (IOREQ *iop)
{
	INODE		*ip = iop->io_ip;
	dev_t		dev = ip->i_dev;
	BHEAD		*bp;
	daddr_t		lbn;
	int		count;
	off_t		offset;

#ifdef	DEBUG
	printf ("%g: count = %d, offset = %d\n", iop->io_count, iop->io_offset_low);
#endif	DEBUG

	/*
	 *	Prepara algumas variáveis
	 */
	ip->i_mtime = time; inode_dirty_inc (ip);

	/*
	 *	Trata-se de arquivo regular ou diretório
	 */
	do
	{
		lbn	= iop->io_offset_low >> BL4SHIFT;
		offset	= iop->io_offset_low &  BL4MASK;
		count	= MIN (BL4SZ - offset, iop->io_count);

		/*
		 *	Obtém o Bloco para escrever: Repare o "endereço físico"
		 */
		bp = block_get (dev, BL4toBL (lbn), ip->i_ino);

		if (count != BL4SZ && bp->b_quantum == 0)
			memclr (bp->b_base_addr, BL4SZ);

		bp->b_nfssp = (int)ip->i_sb->sb_ptr;

		/*
		 *	Coloca a informação no buffer e o escreve
		 */
		if (unimove (bp->b_addr + offset, iop->io_area, count, iop->io_cpflag) < 0)
			{ u.u_error = EFAULT; block_put (bp); return; }

		if (bp->b_quantum < offset + count)
			bp->b_quantum = offset + count;

		iop->io_area       += count;
		iop->io_offset_low += count;
		iop->io_count      -= count;

		if (u.u_error != NOERR)
			block_put (bp);
		else
			bdwrite (bp);

		/*
		 *	Atualiza o tamanho do arquivo
		 */
		if (ip->i_size < iop->io_offset_low)
			ip->i_size = iop->io_offset_low;
	}
	while (u.u_error == NOERR && iop->io_count > 0);

}	/* end nfs2_write */

/*
 ****************************************************************
 *	Executa a leitura ou escrita				*
 ****************************************************************
 */
void
nfs2_strategy (BHEAD *bp)
{
	if (bp->b_flags	& B_READ)
		{ nfs2_read_strategy (bp); printf ("%g: Chamando \"nfs2_read_strategy\"\n"); }
	else
		nfs2_write_strategy (bp);

}	/* end  nfs2_strategy */

/*
 ****************************************************************
 *	Envia o comando de leitura				*
 ****************************************************************
 */
void
nfs2_read_strategy (BHEAD *bp)
{
	NFSSB		*nfssp = (NFSSB *)bp->b_nfssp;
	const SB	*sp = nfssp->sb_sp;
	INODE		*ip = bp->b_inode;
	struct arg { FHANDLE handle; int offset; int count; int totalcount; }		*rp;
	const struct reply { int status; FATTR fattr; int count; char data[1]; }	*reply = NULL;
	struct u_nfs	*up = &u.u_myself->u_nfs;
	int		count, client_cnt, server_cnt;
	int		index = u.u_uprocv - scb.y_uproc;
	off_t		offset;
	ITBLOCK		*itp;
	UDP_EP		uep;

	/*
	 *	Malha principal
	 */
	offset		= BLTOBY (bp->b_blkno);
	bp->b_quantum	= 0;
	count		= bp->b_base_count;
	up->nfs_bp	= NOBHEAD;

	while (count > 0)
	{
		if ((client_cnt = count) > sp->sb_mntent.me_rsize)
			client_cnt = sp->sb_mntent.me_rsize;

		up->nfs_xid = (nfs_req_source++ << XID_PROC_SHIFT) | index;

		up->nfs_timeout	= sp->sb_mntent.me_timeout;
		up->nfs_transmitted = 0;

		if (up->nfs_bp != NOBHEAD)
			put_it_block (up->nfs_bp);

#ifdef	DEBUG
printf ("%g: Usando READ, dev = %v, count = %d, offset = %d\n", bp->b_dev, client_cnt, offset);
#endif	DEBUG
	    get_read_again:
		rp = nfs_setup_client_datagram_prolog (nfssp, &uep, NFSPROC_READ, &itp);

		memmove (&rp->handle, &ip->i_handle, sizeof (FHANDLE));

		rp->offset	= ENDIAN_LONG (offset);
		rp->count	= ENDIAN_LONG (client_cnt);
		rp->totalcount	= 0;		/* Não usado */

	   /***	itp->it_u_area   = ...;	***/
		itp->it_u_count  = (int)&rp[1] - (int)itp->it_u_area;

		EVENTCLEAR (&up->nfs_event);

		up->nfs_sp = nfssp;
		CLEAR (&up->nfs_lock);
		up->nfs_transmitted++;
		up->nfs_last_snd_time = time;

		send_udp_datagram (&uep, itp);

		EVENTWAIT (&up->nfs_event, PITNETIN);

		if   (up->nfs_status == 0)
			goto get_read_again;
		elif (up->nfs_status < 0)
			{ bp->b_error = up->nfs_error; goto bad1; }

		/* Analisa a resposta */

		reply = up->nfs_area;

		if ((bp->b_error = ENDIAN_LONG (reply->status)) != NFS_OK)
			goto bad3;

	   /***	nfs2_put_fattr_attribute (&reply->fattr, (INODE *)bp->b_inode);	***/

		if ((server_cnt = ENDIAN_LONG (reply->count)) == 0)
			break;

		if ((void *)reply + offsetof (struct reply, data) + server_cnt > up->nfs_end_area)
			printf ("%g: Tamanho do RPC insuficiente\n");

#ifdef	DEBUG
printf ("%g: Recebi %d bytes\n", server_cnt);
#endif	DEBUG

		if (server_cnt > client_cnt)
			server_cnt = client_cnt;

		memmove (bp->b_base_addr + (offset & BL4MASK), reply->data, server_cnt);

		offset		+= server_cnt;
		count		-= server_cnt;
		bp->b_quantum	+= server_cnt;

		/* Prevê um EOF prematuro */

		if (server_cnt < client_cnt)
			break;

	}	/* end for (blocos) */

	/*
	 *	Utiliza o último FATTR
	 */
	if (up->nfs_bp != NOBHEAD)
		{ nfs2_put_fattr_attribute (&reply->fattr, ip); put_it_block (up->nfs_bp); }

	EVENTSET (&bp->b_done);

	return;

	/*
	 *	Em caso de erro, ...
	 */
    bad3:
	put_it_block (up->nfs_bp);
    bad1:
	bp->b_flags |= B_ERROR;

	EVENTSET (&bp->b_done);

}	/* end  nfs2_read_strategy */

/*
 ****************************************************************
 *	Coloca o pedido de escrita na fila			*
 ****************************************************************
 */
void
nfs2_write_strategy (BHEAD *bp)
{
	NFSSB		*nfssp = (NFSSB *)bp->b_nfssp;

	/*
	 *	Insere na fila do processo de atualização, e acorda-o
	 */
	SPINLOCK (&nfssp->sb_update_lock);

	if (nfssp->sb_update_bhead_queue == NOBHEAD)
		nfssp->sb_update_bhead_queue = bp;
	else
		nfssp->sb_update_bhead_last->b_forw = bp;

	nfssp->sb_update_bhead_last = bp; bp->b_forw = NOBHEAD;

	EVENTDONE (&nfssp->sb_update_nempty);

	SPINFREE (&nfssp->sb_update_lock);

}	/* end  nfs2_write_strategy */

/*
 ****************************************************************
 *	Código do processo de atualização			*
 ****************************************************************
 */
void
nfs2_update_daemon (NFSSB *nfssp)
{
	BHEAD		*bp;

	for (EVER)
	{
		/*
		 *	É possível, em certos casos, vir um alarme falso!
		 */
		for (EVER)
		{
			EVENTWAIT (&nfssp->sb_update_nempty, PBLKIO);

			SPINLOCK (&nfssp->sb_update_lock);

			if ((bp = nfssp->sb_update_bhead_queue) != NOBHEAD)
				break;

			EVENTCLEAR (&nfssp->sb_update_nempty);

			SPINFREE (&nfssp->sb_update_lock);
		}

		/*
		 *	Achou um BHEAD a atualizar; retira-o da fila
		 */
		if ((nfssp->sb_update_bhead_queue = bp->b_forw) == NOBHEAD)
		{
			nfssp->sb_update_bhead_last = NOBHEAD;

			if (nfssp->sb_update_inode_queue == NOINODE)
				EVENTCLEAR (&nfssp->sb_update_nempty);
		}

		SPINFREE (&nfssp->sb_update_lock);

		nfs2_do_write (bp);

	}	/* end for (EVER) */

}	/* end nfs2_update_daemon */

/*
 ****************************************************************
 *	Envia o comando NFS de escrita				*
 ****************************************************************
 */
void
nfs2_do_write (BHEAD *bp)
{
	NFSSB		*nfssp = (NFSSB *)bp->b_nfssp;
	SB		*sp = nfssp->sb_sp;
	const NFSDATA	*dp;
	struct arg { FHANDLE handle; int beginoffset, offset, totalcount, count; } *rp;
	const struct reply { int status; FATTR fattr; }	*reply;
	struct u_nfs	*up = &u.u_myself->u_nfs;
	int		count, client_cnt;
	int		index = u.u_uprocv - scb.y_uproc;
	off_t		offset;
	ITBLOCK		*itp;
	UDP_EP		uep;

	/*
	 *	Se o arquivo foi removido, ignora os dados
	 */
	if ((dp = nfs2_get_nfsdata (nfssp, bp->b_ino)) == NONFSDATA)
		{ EVENTDONE (&bp->b_done); return; }

	/*
	 *	Malha principal
	 */
	offset = BLTOBY (bp->b_blkno);

	count = bp->b_quantum;

	while (count > 0)
	{
		if ((client_cnt = count) > sp->sb_mntent.me_rsize)
			client_cnt = sp->sb_mntent.me_rsize;

		up->nfs_xid = (nfs_req_source++ << XID_PROC_SHIFT) | index;

		up->nfs_timeout	= sp->sb_mntent.me_timeout;
		up->nfs_transmitted = 0;

#ifdef	DEBUG
printf ("%g: Usando WRITE, dev = %v, count = %d, offset = %d\n", bp->b_dev, client_cnt, offset);
#endif	DEBUG

	    get_write_again:
		rp = nfs_setup_client_datagram_prolog (nfssp, &uep, NFSPROC_WRITE, &itp);

		memmove (&rp->handle, &dp->d_handle, sizeof (FHANDLE));

		rp->beginoffset	= 0;
		rp->offset	= ENDIAN_LONG (offset);
		rp->totalcount	= 0;
		rp->count	= ENDIAN_LONG (client_cnt);

		memmove (&rp[1], bp->b_base_addr + (offset & BL4MASK), client_cnt);

	   /***	itp->it_u_area   = ...;	***/
		itp->it_u_count  = (int)&rp[1] + ROUND_XDR (client_cnt) - (int)itp->it_u_area;

		EVENTCLEAR (&up->nfs_event);

		up->nfs_sp = nfssp;
		CLEAR (&up->nfs_lock);
		up->nfs_transmitted++;
		up->nfs_last_snd_time = time;

		send_udp_datagram (&uep, itp);

		EVENTWAIT (&up->nfs_event, PITNETIN);

		if   (up->nfs_status == 0)
			goto get_write_again;
		elif (up->nfs_status < 0)
			{ bp->b_error = up->nfs_error; goto bad1; }

		/* Analisa a resposta */

		reply = up->nfs_area;

		if ((bp->b_error = ENDIAN_LONG (reply->status)) != NFS_OK)
		{
			if (bp->b_error == EROFS)
				nfs_server_is_rofs (sp);

			goto bad3;
		}

		if ((void *)reply + sizeof (struct reply) > up->nfs_end_area)
			printf ("%g: Tamanho do RPC insuficiente\n");

		offset	+= client_cnt;
		count	-= client_cnt;

		/* NÃO vale a pena usar o FATTR */
 
		put_it_block (up->nfs_bp);

	}	/* end for (blocos) */

	EVENTDONE (&bp->b_done);

#ifdef	DEBUG
printf ("%g: Concluindo o WRITE\n");
#endif	DEBUG

	return;

	/*
	 *	Em caso de erro, ...
	 */
    bad3:
	put_it_block (up->nfs_bp);
    bad1:
	bp->b_flags |= B_ERROR;

	EVENTDONE (&bp->b_done);

}	/* end  nfs2_do_write */

/*
 ****************************************************************
 *	Rotina de leitura de um elo simbólico 			*
 ****************************************************************
 */
int
nfs2_read_symlink (IOREQ *iop)
{
	INODE		*ip = iop->io_ip;
	const SB	*sp = ip->i_sb;
	NFSSB		*nfssp = sp->sb_ptr;
	struct arg { FHANDLE handle; }				*rp;
	struct reply { int status; int count; char path[1]; }	*reply;
	struct u_nfs	*up = &u.u_myself->u_nfs;
	int		count, server_cnt;
	ITBLOCK		*itp;
	UDP_EP		uep;

	/*
	 *	Prepara algumas variáveis
	 *
	 *	Repare que "ip->i_size" NÃO inclui o NULO
	 */
	if (ip->i_sb->sb_mntent.me_flags & SB_ATIME)
		{ ip->i_atime = time; inode_dirty_inc (ip); }

	up->nfs_xid = (nfs_req_source++ << XID_PROC_SHIFT) | (u.u_uprocv - scb.y_uproc);

	up->nfs_timeout	= sp->sb_mntent.me_timeout;
	up->nfs_transmitted = 0;

#ifdef	DEBUG
printf ("%g: Usando READLINK\n");
#endif	DEBUG

    get_readlink_again:
	rp = nfs_setup_client_datagram_prolog (nfssp, &uep, NFSPROC_READLINK, &itp);

	memmove (&rp->handle, &ip->i_handle, sizeof (FHANDLE));

   /***	itp->it_u_area   = ...;	***/
	itp->it_u_count  = (int)&rp[1] - (int)itp->it_u_area;

	EVENTCLEAR (&up->nfs_event);

	up->nfs_sp = nfssp;
	CLEAR (&up->nfs_lock);
	up->nfs_transmitted++;
	up->nfs_last_snd_time = time;

	send_udp_datagram (&uep, itp);

	EVENTWAIT (&up->nfs_event, PITNETIN);

	if   (up->nfs_status == 0)
		goto get_readlink_again;
	elif (up->nfs_status < 0)
		{ u.u_error = up->nfs_error; return (-1); }

	/* Analisa a resposta */

	reply = (struct reply *)up->nfs_area;

	if ((u.u_error = ENDIAN_LONG (reply->status)) != NFS_OK)
		{ put_it_block (up->nfs_bp); return (-1); }

	server_cnt = ENDIAN_LONG (reply->count);

	if ((void *)reply + offsetof (struct reply, path) + server_cnt > up->nfs_end_area)
		printf ("%g: Tamanho do RPC insuficiente\n");

	reply->path[server_cnt] = '\0';

	count = MIN (iop->io_count, server_cnt + 1);

	if (unimove (iop->io_area, reply->path, count, iop->io_cpflag) < 0)
		u.u_error = EFAULT;

	put_it_block (up->nfs_bp); return (count);

}	/* end nfs2_read_symlink */

/*
 ****************************************************************
 *	Rotina de escrita de um elo simbólico 			*
 ****************************************************************
 */
INODE *
nfs2_write_symlink (const char *old_path, int old_count, IOREQ *iop, DIRENT *dep)
{
	INODE		*ip = iop->io_ip;
	SB		*sp = ip->i_sb;
	NFSSB		*nfssp = sp->sb_ptr;
	struct arg { FHANDLE handle; int name[1]; /* NAME, SATTR */ }	*rp;
	const struct reply { int status; }	*reply;
	struct u_nfs	*up = &u.u_myself->u_nfs;
	int		*ptr;
	SATTR		*sap;
	ITBLOCK		*bp;
	FHANDLE		handle;
	UDP_EP		uep;

	/*
	 *	Prepara algumas variáveis
	 *
	 *	Repare que "ip->i_size" NÃO inclui o NULO
	 */
	if (ip->i_sb->sb_mntent.me_flags & SB_ATIME)
		{ ip->i_atime = time; /*** inode_dirty_inc (ip); ***/ }

	ip->i_mtime = time; inode_dirty_inc (ip);

	memmove (&handle, &ip->i_handle, sizeof (FHANDLE)); iput (ip);

	up->nfs_xid = (nfs_req_source++ << XID_PROC_SHIFT) | (u.u_uprocv - scb.y_uproc);

	up->nfs_timeout	= sp->sb_mntent.me_timeout;
	up->nfs_transmitted = 0;

#ifdef	DEBUG
printf ("%g: Usando WRITELINK\n");
#endif	DEBUG

    get_symlink_again:
	rp = nfs_setup_client_datagram_prolog (nfssp, &uep, NFSPROC_SYMLINK, &bp);

	memmove (&rp->handle, &handle, sizeof (FHANDLE));

	ptr = nfs_put_xdr_name (rp->name, dep->d_name, dep->d_namlen);

	sap = nfs_put_xdr_name (ptr, old_path, old_count - 1);

	sap->sa_mode		= ENDIAN_LONG (IFLNK | 0777);
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
		goto get_symlink_again;
	elif (up->nfs_status < 0)
		{ u.u_error = up->nfs_error; return (NOINODE); }

	/* Analisa a resposta */

	if ((void *)(reply = up->nfs_area) + sizeof (struct reply) > up->nfs_end_area)
		printf ("%g: Tamanho do RPC insuficiente\n");

	u.u_error = ENDIAN_LONG (reply->status);

	if (u.u_error == EROFS)
		nfs_server_is_rofs (sp);

	put_it_block (up->nfs_bp); return (NOINODE);

}	/* end nfs2_write_symlink */
