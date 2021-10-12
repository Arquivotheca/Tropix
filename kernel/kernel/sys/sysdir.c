/*
 ****************************************************************
 *								*
 *			sysdir.c				*
 *								*
 *	Chamadas ao sistema: Criação e remoção de diretórios	*
 *								*
 *	Versão	3.0.0, de 18.11.92				*
 *		4.8.0, de 05.11.05				*
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
#include "../h/sync.h"
#include "../h/scb.h"
#include "../h/region.h"

#include "../h/mntent.h"
#include "../h/sb.h"
#include "../h/kfile.h"
#include "../h/sysdirent.h"
#include "../h/inode.h"
#include "../h/stat.h"
#include "../h/bhead.h"
#include "../h/uerror.h"
#include "../h/signal.h"
#include "../h/uproc.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	"mkdir" System Call					*
 ****************************************************************
 */
int
mkdir (const char *path, int mode)
{
	INODE		*ip;
	IOREQ		io;
	DIRENT		de;

	if ((ip = iname (path, getuchar, ICREATE, &io, &de)) != NOINODE)
		{ u.u_error = EEXIST; iput (ip); return (-1); }

	if (u.u_error)
		return (-1);

	mode &= ~(IFMT|ISVTX|IMETX); mode |= IFDIR;

	if ((ip = (*io.io_ip->i_fsp->fs_make_dir) (&io, &de, mode)) != NOINODE)
		iput (ip);

	return (UNDEF);

}	/* end mkdir */

/*
 ****************************************************************
 *	"rmdir" System Call					*
 ****************************************************************
 */
int
rmdir (const char *path)
{
	INODE		*ip, *pp;
	IOREQ		io;
	DIRENT		de;

	/*
	 *	Obtem o pai do diretório
	 */
	if ((pp = iname (path, getuchar, IDELETE, &io, &de)) == NOINODE)
		return (-1);

	/*
	 *	Verifica se por acaso, não está tentado remover "."
	 */
	if (pp->i_ino == de.d_ino)
		{ u.u_error = EINVAL; goto outp; }

	/*
	 *	Obtem o filho (que deve ser apagado).
	 */
	if ((ip = iget (pp->i_dev, de.d_ino, 0)) == NOINODE)
		goto outp;

	/*
	 *	Verifica se é realmente um diretório
	 */
	if ((ip->i_mode & IFMT) != IFDIR)
		{ u.u_error = ENOTDIR; goto out; }

	/*
	 *	Verifica se é o diretório corrente
	 */
	if (ip == u.u_cdir)
		{ u.u_error = EINVAL; goto out; }

	/*
	 *	Verifica se é um arquivo de um FS montado
	 */
	if (ip->i_dev != pp->i_dev)
		{ u.u_error = EBUSY; goto out; }

	/*
	 *	Verifica se o diretório está vazio
	 */
	if ((*ip->i_fsp->fs_empty_dir) (ip) <= 0)
		{ u.u_error = ENOTEMPTY; goto out; }

	/*
	 *	Apaga a entrada do diretório
	 */
	(*ip->i_fsp->fs_clr_dir) (&io, &de, ip);

	pp->i_nlink--;
	ip->i_nlink -= 2;

	/*
	 *	Libera os INODEs
	 */
    out:
	iput (ip);
    outp:
	iput (pp);

	return (UNDEF);

}	/* end  rmdir */

/*
 ****************************************************************
 *	"getdirent" System Call					*
 ****************************************************************
 */
int
getdirent (int fd, DIRENT *area, int count, int *eof_ptr)
{
	KFILE		*fp;
	INODE		*ip;
	IOREQ		io;

	/*
	 *	Analisa o arquivo
	 */
	if ((fp = fget (fd)) == NOKFILE)
		return (-1);

	if ((fp->f_flags & O_READ) == 0)
		{ u.u_error = EBADF; return (-1); }

	ip = fp->f_inode;

	if (!S_ISDIR (ip->i_mode))
		{ u.u_error = ENOTDIR; return (-1); }

	if (count < sizeof (DIRENT))
		{ u.u_error = EINVAL; return (-1); }

	/*
	 *	Prepara o pedido de E/S
	 */
	io.io_fd	= fd;
	io.io_fp	= fp;
	io.io_ip	= ip;
   /***	io.io_dev	= ...;	***/
	io.io_area	= area;
	io.io_count	= count;
	io.io_offset	= fp->f_offset;
	io.io_cpflag	= SU;
   /***	io.io_rablock	= ...;	***/
	io.io_eof	= 0;

	/*
	 *	Chama a função específica do sistema de arquivos
	 */
	SLEEPLOCK (&ip->i_lock, PINODE);

	if (ip->i_sb->sb_mntent.me_flags & SB_ATIME)
		{ ip->i_atime = time; inode_dirty_inc (ip); }

	(*ip->i_fsp->fs_read_dir) (&io);

	SLEEPFREE (&ip->i_lock);

	if (io.io_eof && pulong (eof_ptr, 1) < 0)
		u.u_error = EFAULT;

	fp->f_offset = io.io_offset;

	return (count - io.io_count);

}	/* end getdirent */
