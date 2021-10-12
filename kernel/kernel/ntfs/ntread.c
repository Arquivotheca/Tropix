/*
 ****************************************************************
 *								*
 *			ntread.c				*
 *								*
 *	Leitura e escrita de arquivos				*
 *								*
 *	Versão	4.6.0, de 29.06.04				*
 *		4.6.0, de 29.06.04				*
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
#include "../h/region.h"
#include "../h/stat.h"

#include "../h/iotab.h"
#include "../h/kfile.h"
#include "../h/inode.h"
#include "../h/bhead.h"
#include "../h/mntent.h"
#include "../h/sb.h"
#include "../h/nt.h"
#include "../h/uerror.h"
#include "../h/signal.h"
#include "../h/uproc.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Open de INODEs						*
 ****************************************************************
 */
void
nt_open (INODE *ip, int oflag)
{
	/*
	 *	Recebe e devolve o INODE locked
	 */
	if (ip->i_flags & ILOCK || (oflag & O_LOCK) && ip->i_count != 1)
		{ u.u_error = EBUSY; return; }

	if (oflag & O_LOCK)
		ip->i_flags |= ILOCK;

}	/* end nt_open */

/*
 ****************************************************************
 *	Close de INODEs						*
 ****************************************************************
 */
void
nt_close (INODE *ip)
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

}	/* end nt_close */

/*
 ****************************************************************
 *	Leitura de um INODE					*
 ****************************************************************
 */
void
nt_read (IOREQ *iop)
{
	INODE		*ip = iop->io_ip;
	const NTSB	*ntsp = ip->i_sb->sb_ptr;
	int		range;
	int		old_count, count;

	/*
	 *	Somente para arquivos regulares
	 */
	if (!S_ISREG (ip->i_mode))
		{ u.u_error = EINVAL; return; }

	/*
	 *	Prepara algumas variáveis
	 */
	if (ip->i_sb->sb_mntent.me_flags & SB_ATIME)
		{ ip->i_atime = time; /*** inode_dirty_inc (ip); ***/ }

	/*
	 *	Lê o atributo
	 */
	if ((range = ip->i_size - iop->io_offset_low) <= 0)
		return;

	if ((count = old_count = iop->io_count) > range)
		count = range;

	iop->io_count = count;

	if ((count = ntfs_readattr (ntsp, NTFS_A_DATA, NOSTR, iop)) < 0)
		return;

	iop->io_count = old_count - count;

}	/* end nt_read */
