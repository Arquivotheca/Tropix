/*
 ****************************************************************
 *								*
 *			v7read.c				*
 *								*
 *	Leitura e escrita de arquivos				*
 *								*
 *	Versão	3.0.0, de 22.11.94				*
 *		4.2.0, de 16.04.02				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include "../h/common.h"
#include "../h/sync.h"
#include "../h/scb.h"
#include "../h/region.h"

#include "../h/iotab.h"
#include "../h/kfile.h"
#include "../h/inode.h"
#include "../h/bhead.h"
#include "../h/mntent.h"
#include "../h/sb.h"
#include "../h/v7.h"
#include "../h/sysdirent.h"
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
v7_open (INODE *ip, int oflag)
{
	/*
	 *	Recebe e devolve o INODE locked
	 */
	if (ip->i_flags & ILOCK || (oflag & O_LOCK) && ip->i_count != 1)
		{ u.u_error = EBUSY; return; }

	if (oflag & O_LOCK)
		ip->i_flags |= ILOCK;

}	/* end v7_open */

/*
 ****************************************************************
 *	Close de INODEs						*
 ****************************************************************
 */
void
v7_close (INODE *ip)
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

}	/* end v7_close */

/*
 ****************************************************************
 *	Leitura de um INODE					*
 ****************************************************************
 */
void
v7_read (IOREQ *iop)
{
	INODE		*ip = iop->io_ip;
	BHEAD		*bp;
	dev_t		dev;
	daddr_t		lbn, pbn, rabn;
	off_t		range;
	int		offset, count;

	/*
	 *	Prepara algumas variáveis
	 */
	if (ip->i_sb->sb_mntent.me_flags & SB_ATIME)
		{ ip->i_atime = time; inode_dirty_inc (ip); }

	/*
	 *	Trata-se de arquivo regular ou diretório
	 */
	dev = ip->i_dev;

	do
	{
		lbn	= pbn = iop->io_offset_low >> BLSHIFT;
		offset	= iop->io_offset_low & BLMASK;
		count	= MIN (BLSZ - offset, iop->io_count);

		/*
		 *	Calcula os blocos físicos e "readahead"
		 */
		if ((range = ip->i_size - iop->io_offset_low) <= 0)
			return;

		if (range < count)
			count = range;

		pbn = v7_block_map (ip, lbn, B_READ, &rabn);

		if (u.u_error)
			return;

		/*
		 *	Le um bloco
		 */
		if (pbn < 0)
		{
			/*
			 *	BLOCO dentro do arquivo, mas nunca foi escrito
			 */
			bp = block_free_get (BLSZ);
			memclr (bp->b_addr, BLSZ);
			bp->b_residual = 0;
		}
		elif (ip->i_lastr + 1 == lbn)
		{
			/*
			 *	A leitura está sendo feita sequencialmente
			 */
			bp = breada (dev, pbn, rabn, 0);
#ifdef	MSG
			if (CSWT (4))
				printf ("%g: Readahead: dev = %v, bn = %d, ra = %d\n", dev, pbn, rabn);
#endif	MSG
		}
		else
		{
			/*
			 *	Leitura normal
			 */
			bp = bread (dev, pbn, 0);
		}

#ifdef	MSG
		if (CSWT (13) && bp->b_residual != 0)
			printf ("%g: Residual = %d\n", bp->b_residual);
#endif	MSG

		/*
		 *	Move a Informação para a area desejada
		 */
		ip->i_lastr = lbn;

		count = MIN (count, BLSZ - bp->b_residual);

		if (unimove (iop->io_area, bp->b_addr + offset, count, iop->io_cpflag) < 0)
			{ u.u_error = EFAULT; return; }

		iop->io_area       += count;
		iop->io_offset_low += count;
		iop->io_count      -= count;

		block_put (bp);
	}
	while (u.u_error == NOERR && iop->io_count > 0 && count > 0);

}	/* end v7_read */

/*
 ****************************************************************
 *	Escrita de um INODE					*
 ****************************************************************
 */
void
v7_write (IOREQ *iop)
{
	INODE		*ip = iop->io_ip;
	BHEAD		*bp;
	dev_t		dev;
	daddr_t		bn, rabn;
	int		count, offset;

	/*
	 *	Atualiza o tempo de modificação do INODE
	 */
	dev = ip->i_dev;

	ip->i_mtime = time; inode_dirty_inc (ip);

	/*
	 *	Malha de escrita
	 */
	do
	{
		bn	= iop->io_offset_low >> BLSHIFT;
		offset	= iop->io_offset_low & BLMASK;
		count	= MIN (BLSZ - offset, iop->io_count);

		/*
		 *	Calcula o Endereço Físico
		 *	Retorna em caso de BLOCO inválido
		 */
		if ((bn = v7_block_map (ip, bn, B_WRITE, &rabn)) <= NODADDR)
			return;

		/*
		 *	Obtém o Bloco para escrever
		 */
#if (0)	/*******************************************************/
		if (count == BLSZ) /* && offset == 0 */
			bp = block_get (dev, bn, 0);
		else
#endif	/*******************************************************/
			bp = bread (dev, bn, 0);

		/*
		 *	Coloca a informação no buffer e o escreve
		 */
		if (unimove (bp->b_addr + offset, iop->io_area, count, iop->io_cpflag) < 0)
			{ u.u_error = EFAULT; block_put (bp); return; }

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
		if (iop->io_offset_low > ip->i_size)
			ip->i_size = iop->io_offset_low;

	}
	while (u.u_error == NOERR && iop->io_count > 0);

}	/* end v7_write */

/*
 ****************************************************************
 *	Rotina de "read" de um "link" simbólico 		*
 ****************************************************************
 */
int
v7_read_symlink (IOREQ *iop)
{
	INODE		*ip = iop->io_ip;
	BHEAD		*bp;
	int		count;

	/*
	 *	Prepara algumas variáveis
	 */
	if (ip->i_sb->sb_mntent.me_flags & SB_ATIME)
		{ ip->i_atime = time; inode_dirty_inc (ip); }

	/*
	 *	Lê o bloco correspondente
	 */
	bp = bread (ip->i_dev, ip->i_addr[0], 0);

	if (bp->b_flags & B_ERROR)
		{ block_put (bp); return (-1); }

	count = MIN (iop->io_count, ip->i_size + 1);

	if (unimove (iop->io_area, bp->b_addr, count, iop->io_cpflag) < 0)
		u.u_error = EFAULT;

	block_put (bp);

	return (count);

}	/* end v7_read_symlink */

/*
 ****************************************************************
 *	Rotina de "write" de um "link" simbólico 		*
 ****************************************************************
 */
INODE *
v7_write_symlink (const char *old_path, int old_count, IOREQ *iop, DIRENT *dep)
{
	INODE		*ip;

	/*
	 *	Verifica o tamanho do conteúdo: o NULO está incluído
	 */
	if (old_count > BLSZ)
		{ iput (iop->io_ip); u.u_error = E2BIG; return (NOINODE); }

	/*
	 *	Cria o arquivo
	 */
	if ((ip = v7_make_inode (iop, dep, IFLNK)) == NOINODE)
		return (NOINODE);

	ip->i_mode |= 0777;	/* Tôdas as permissões */

	/*
	 *	Grava o conteúdo
	 */
	iop->io_ip	   = ip;
	iop->io_area	   = (char *)old_path;
	iop->io_count	   = old_count;
	iop->io_offset_low = 0;
	iop->io_cpflag	   = US;

	v7_write (iop);

	ip->i_size -= 1;	/* No tamanho, não inclui o NULO */

	return (ip);

}	/* end v7_write_symlink */
