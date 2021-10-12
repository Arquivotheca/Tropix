/*
 ****************************************************************
 *								*
 *			ichr.c					*
 *								*
 *	Leitura e escrita de dispositivos não estruturados	*
 *								*
 *	Versão	4.2.0, de 26.11.01				*
 *		4.2.0, de 26.11.01				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2001 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include "../h/common.h"
#include "../h/sync.h"
#include "../h/scb.h"
#include "../h/region.h"

#include "../h/mntent.h"
#include "../h/sb.h"
#include "../h/iotab.h"
#include "../h/kfile.h"
#include "../h/inode.h"
#include "../h/bhead.h"
#include "../h/uerror.h"
#include "../h/signal.h"
#include "../h/uproc.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Open de um INODE (CHR)					*
 ****************************************************************
 */
void
chr_open (INODE *ip, int oflag)
{
	dev_t		dev;
	int		major;

	/*
	 *	Recebe e devolve o INODE locked
	 */
	dev = ip->i_rdev; major = MAJOR (dev);

	if ((unsigned)major >= scb.y_nchrdev)
		{ u.u_error = ENXIO; return; }

	(*ciotab[major].w_open) (dev, oflag);

}	/* end chr_open */

/*
 ****************************************************************
 *	Close de um INODE (CHR)					*
 ****************************************************************
 */
void
chr_close (INODE *ip)
{
	/*
	 *	Recebe o INODE locked e devolve free
	 */

	/*
	 *	Se não era o ultimo usuário de um dispositivo, ...
	 */
	if (ip->i_count > 1)
	{
		/* O "close" antes do close do INODE */

		(*ciotab[MAJOR(ip->i_rdev)].w_close) (ip->i_rdev, 1);

		/* Simplesmente decrementa o uso */

		iput (ip); return;
	}

	/*
	 *	O "close" final do INODE
	 */
	(*ciotab[MAJOR(ip->i_rdev)].w_close) (ip->i_rdev, 0);

	iput (ip);

}	/* end chr_close */

/*
 ****************************************************************
 *	Leitura de um INODE (CHR)				*
 ****************************************************************
 */
void
chr_read (IOREQ *iop)
{
	INODE		*ip = iop->io_ip;

	/*
	 *	Prepara algumas variáveis
	 */
	if (ip->i_sb->sb_mntent.me_flags & SB_ATIME)
		{ ip->i_atime = time; inode_dirty_inc (ip); }

	/*
 	 *	Trata diretamente no driver
	 */
	iop->io_dev = ip->i_rdev;

	(*ciotab[MAJOR (ip->i_rdev)].w_read) (iop);

}	/* end chr_read */

/*
 ****************************************************************
 *	Escrita de um INODE (CHR)				*
 ****************************************************************
 */
void
chr_write (IOREQ *iop)
{
	INODE		*ip = iop->io_ip;

	/*
	 *	Não atualiza o tempo de modificação ?
	 */

	/*
	 *	Chama diretamente o driver
	 */
	iop->io_dev = ip->i_rdev;

	(*ciotab[MAJOR (ip->i_rdev)].w_write) (iop);

}	/* end chr_write */
