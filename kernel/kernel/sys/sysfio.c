/*
 ****************************************************************
 *								*
 *			sysfio.c				*
 *								*
 *	Chamadas ao sistema: Entrada/saída			*
 *								*
 *	Versão	3.0.0, de 13.12.94				*
 *		4.6.0, de 02.09.04				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2004 NCE/UFRJ - tecle "man licença"	*
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
#include "../h/inode.h"
#include "../h/iotab.h"
#include "../h/lockf.h"
#include "../h/sysdirent.h"
#include "../h/uerror.h"
#include "../h/signal.h"
#include "../h/uproc.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ******	Protótipos **********************************************
 */
int	rdwr (IOREQ *, long);

/*
 ****************************************************************
 *	Chamada ao sistema "read"				*
 ****************************************************************
 */
int
read (int fd, void *area, off_t count)
{
	IOREQ		io;

	io.io_fd	= fd;
   /***	io.io_fp	= ...;	***/
   /***	io.io_ip	= ...;	***/
   /***	io.io_dev	= ...;	***/
	io.io_area	= area;
	io.io_count	= count;
   /***	io.io_offset	= ...;	***/
	io.io_cpflag	= SU;
   /***	io.io_rablock	= ...;	***/

	return (rdwr (&io, O_READ));

}	/* end read */

/*
 ****************************************************************
 *	Chamada ao sistema "write"				*
 ****************************************************************
 */
int
write (int fd, void *area, off_t count)
{
	IOREQ		io;

	io.io_fd	= fd;
   /***	io.io_fp	= ...;	***/
   /***	io.io_ip	= ...;	***/
   /***	io.io_dev	= ...;	***/
	io.io_area	= area;
	io.io_count	= count;
   /***	io.io_offset	= ...;	***/
	io.io_cpflag	= US;
   /***	io.io_rablock	= ...;	***/

	return (rdwr (&io, O_WRITE));

}	/* end write */

/*
 ****************************************************************
 *	Código comum para "read/write"				*
 ****************************************************************
 */
int
rdwr (IOREQ *iop, long mode)
{
	KFILE		*fp;
	INODE		*ip;
	int		count = iop->io_count;

#ifdef	MSG
	if (CSWT (18))
	{
		printf
		(	"%g: fd = %d, area = %P, count = %d, mode = %d\n",
			iop->io_fd, iop->io_area, count, mode
		);

		if (count <= 80)
			printf ("\"%s\"\n", iop->io_area);
	}
#endif	MSG

	/*
	 *	Verifica se o descritor é válido
	 */
	if ((fp = fget (iop->io_fd)) == NOKFILE)
		return (-1);

	if ((fp->f_flags & mode) == 0)
		{ u.u_error = EBADF; return (-1); }

	/*
	 *	Prepara os argumentos de E/S
	 */
	if (count /* == iop->io_count ***/ <= 0)
		{ u.u_error = EINVAL; return (-1); }

	u.u_oflag = fp->f_flags;	/* PROVISÓRIO */

	ip = fp->f_inode;

	/*
	 *	Preenche os campos remanescentes
	 */
   /***	iop->io_fd	= ...;	***/
   	iop->io_fp	= fp;
   	iop->io_ip	= ip;
	iop->io_dev	= ip->i_dev;
   /***	iop->io_area	= ...;	***/
   /***	iop->io_count	= ...;	***/
   /***	iop->io_offset	= ...;	Abaixo ***/
   /***	iop->io_cpflag	= ...;	***/
   /***	iop->io_rablock	= ...;	***/

	/*
	 *	Analisa o CÓDIGO do Sistema de Arquivos
	 */
	switch (ip->i_fsp->fs_code)
	{
	    case FS_CHR:
	    case FS_BLK:
		iop->io_offset = fp->f_offset;

		if (mode == O_READ)
			(*ip->i_fsp->fs_read) (iop);
		else
			(*ip->i_fsp->fs_write) (iop);

		fp->f_offset += (count - iop->io_count);
		break;

	    case FS_PIPE:
		if (mode == O_READ)
			pipe_read (iop);
		else
			pipe_write (iop);
		break;

	    case FS_V7:
	    case FS_T1:
	    case FS_FAT:
	    case FS_CD:
	    case FS_EXT2:
	    case FS_NT:
	    case FS_NFS2:
		SLEEPLOCK (&ip->i_lock, PINODE);

		if (mode == O_WRITE && (fp->f_flags & O_APPEND))
			fp->f_offset = ip->i_size;

		iop->io_offset = fp->f_offset;

		/*
		 *	Analisa o "lockf"
		 */
		if (ip->i_lockf)
		{
			if
			(	lfaccess
				(	ip,
					iop->io_offset_low,
					iop->io_offset_low + count,
					fp->f_flags & O_NDELAY ? 'T':'W'
				) < 0
			)
			{
				/*
				 *	Repare que "lockf" só é
				 *	dado em arquivos regulares
				 */
				SLEEPFREE (&ip->i_lock);
				return (-1);
			}
		}

		if (mode == O_READ)
			(*ip->i_fsp->fs_read) (iop);
		else
			(*ip->i_fsp->fs_write) (iop);

		SLEEPFREE (&ip->i_lock);

		fp->f_offset += (count - iop->io_count);
		break;

	    default:
		printf ("%g: Tentando ler/escrever código %f\n", ip->i_fsp->fs_code);
		u.u_error = ENOTFS; return (-1);

	}	/* end switch (ip->i_fsp->fs_code) */

	return (count - iop->io_count);

}	/* end rdwr */

/*
 ****************************************************************
 *	Chamada ao sistema "open"				*
 ****************************************************************
 */
int
open (const char *path, int oflag, int mode)
{
	INODE		*ip;
	IOREQ		io;
	DIRENT		de;

	/*
	 *	Converte o formato externo <fcntl.h> no interno <kfile.h>
	 */
	oflag += 1;

	/*
	 *	Tenta obter o INODE relativo ao arquivo
	 */
	if ((ip = iname (path, getuchar, ISEARCH, &io, &de)) == NOINODE)
	{
		/*
		 *	O arquivo não existe; processa o "creat"
		 */
		if ((oflag & O_CREAT) == 0)
			return (-1);

		u.u_error = NOERR;

		ip = iname (path, getuchar, ICREATE, &io, &de);

		if (u.u_error)
			return (-1);

		mode &= ~(IFMT|ISVTX|IMETX); mode |= IFREG;

		if ((ip = (*io.io_ip->i_fsp->fs_make_inode) (&io, &de, mode)) == NOINODE)
			return (-1);

#if (0)	/*******************************************************/
		if (oflag & O_LOCK)
			ip->i_flags |= ILOCK;
#endif	/*******************************************************/

		fopen (ip, oflag, FCNEW);

		return (u.u_fd);
	}

#if (0)	/*******************************************************/
	/*
	 *	Processa o "LOCK"
	 */
	if (ip->i_flags & ILOCK || oflag & O_LOCK && ip->i_count != 1)
		{ u.u_error = EBUSY; iput (ip); return (-1); }
#endif	/*******************************************************/

	/*
	 *	Verifica se só deve criar se ainda não existia
	 */
	if ((oflag & (O_CREAT|O_EXCL)) == (O_CREAT|O_EXCL))
		{ u.u_error = EEXIST; iput (ip); return (-1); }

#if (0)	/*******************************************************/
	if (oflag & O_LOCK)
		ip->i_flags |= ILOCK;
#endif	/*******************************************************/
	/*
	 *	Verifica se deve interpretar os dispositivos
	 */
	if ((ip->i_sb->sb_mntent.me_flags & SB_DEV) == 0)
	{
		switch (ip->i_mode & IFMT)
		{
		    case IFBLK:
		    case IFCHR:
			{ u.u_error = EINVAL; iput (ip); return (-1); }
		}
	}

	fopen (ip, oflag, FOPEN);

#ifdef	MSG
	if (CSWT (7))
		printf ("%g: %s, %d\n", path, u.u_fd);
#endif	MSG

	return (u.u_fd);

}	/* end open */

/*
 ****************************************************************
 *	Chamada ao sistema "inopen"				*
 ****************************************************************
 */
int
inopen (dev_t dev, ino_t ino)
{
	INODE		*ip;

	/*
	 *	Somente para leituras
	 */
	if ((ip = iget (dev, ino, 0)) == NOINODE)
		return (-1);

	if (kaccess (ip, IREAD) < 0)
		{ iput (ip); return (-1); }

#if (0)	/*******************************************************/
	if (ip->i_flags & ILOCK)
		{ u.u_error = EBUSY; iput (ip); return (-1); }
#endif	/*******************************************************/

	fopen (ip, O_READ, FOPEN);

#ifdef	MSG
	if (CSWT (7))
		printf ("%g: (%d,%d) %d\n", dev, ino, u.u_fd);
#endif	MSG

	return (u.u_fd);

}	/* end inopen */

/*
 ****************************************************************
 *	Chamada ao sistema "creat"				*
 ****************************************************************
 */
int
creat (const char *path, long mode)
{
	INODE		*ip;
	IOREQ		io;
	DIRENT		de;

	if ((ip = iname (path, getuchar, ICREATE, &io, &de)) == NOINODE)
	{
		if (u.u_error)
			return (-1);

		mode &= ~(IFMT|ISVTX|IMETX); mode |= IFREG;

		if ((ip = (*io.io_ip->i_fsp->fs_make_inode) (&io, &de, mode)) == NOINODE)
			return (-1);

		fopen (ip, O_WRITE, FCNEW);
	}
	else
	{
		fopen (ip, O_WRITE, FCTRUNC);
	}

#ifdef	MSG
	if (CSWT (7))
		printf ("%g: %s, %d\n", path, u.u_fd);
#endif	MSG

	return (u.u_fd);

}	/* end creat */

/*
 ****************************************************************
 *	Chamada ao sistema "close"				*
 ****************************************************************
 */
int
close (int fd)
{
	KFILE		*fp;
 
#ifdef	MSG
	if (CSWT (7))
		printf ("%g: %d\n", fd);
#endif	MSG

	if ((fp = fget (fd)) == NOKFILE)
		return (-1);

	u.u_ofile[fd] = NOKFILE;

	fclose (fp);

	return (UNDEF);

}	/* end close */

/*
 ****************************************************************
 *	Chamada ao sistema "lseek"				*
 ****************************************************************
 */
off_t
lseek (int fd, off_t offset_low, int whence)
{
	KFILE		*fp;
	loff_t		offset = offset_low;

	if ((fp = fget (fd)) == NOKFILE)
		return (-1);

	if (fp->f_flags & O_PIPE)
		{ u.u_error = ESPIPE; return (-1); }

	if   (whence == 1)
		offset += fp->f_offset;
	elif (whence == 2)
		offset += fp->f_inode->i_size;

	fp->f_offset = offset;

	return (fp->f_offset_low);

}	/* end lseek */

/*
 ****************************************************************
 *	Chamada ao sistema "llseek"				*
 ****************************************************************
 */
int
llseek (int fd, loff_t offset, loff_t *result, int whence)
{
	KFILE		*fp;

	if ((fp = fget (fd)) == NOKFILE)
		return (-1);

	if (fp->f_flags & O_PIPE)
		{ u.u_error = ESPIPE; return (-1); }

	if   (whence == 1)
		offset += fp->f_offset;
	elif (whence == 2)
		offset += fp->f_inode->i_size;

	fp->f_offset = offset;

	if (result != NULL && unimove (result, &offset, sizeof (offset), SU) < 0)
		u.u_error = EFAULT;

	return (UNDEF);

}	/* end llseek */

/*
 ****************************************************************
 *	Chamada ao sistema "lockf"				*
 ****************************************************************
 */
int
lockf (int fd, long mode, off_t size)
{
	KFILE		*fp;
	INODE		*ip;
	off_t		begin_off, end_off;
	int		r;

#ifdef	MSG
	if (CSWT (18))
		printf ("%g: %d, %d, %d\n", fd, mode, size);
#endif	MSG

	if ((fp = fget (fd)) == NOKFILE)
		return (-1);

	/*
	 *	Permite LOCKs apenas para arquivos regulares
	 */
	ip = fp->f_inode;

	if (fp->f_flags & O_PIPE || (ip->i_mode & IFMT) != IFREG)
		{ u.u_error = EBADF; return (-1); }

	/*
	 *	Para trancar, precisa permissão de escrita
	 */
	if ((fp->f_flags & O_WRITE) == 0)
	{
		if (mode == F_LOCK || mode == F_TLOCK)
			{ u.u_error = EBADF; return (-1); }
	}

	/*
	 *	Analisa a região
	 */
	begin_off = fp->f_offset_low;

	if   (size > 0)
	{
		end_off = begin_off + size;
	}
	elif (size < 0)
	{
		end_off = begin_off;
		begin_off += size;
	}
	else /* size == 0 */
	{
		end_off = 0x7FFFFFFF; /* Tamanho máximo possível de um off_t */
	}

	if (begin_off < 0 || end_off < 0)
		{ u.u_error = EINVAL; return (-1); }

	/*
	 *	Verifica a função pedida
	 */
	SLEEPLOCK (&ip->i_lock, PINODE);

	switch (mode)
	{
	    case F_LOCK:
		r = lflock (ip, begin_off, end_off, 'W');
		break;

	    case F_TLOCK:
		r = lflock (ip, begin_off, end_off, 'T');
		break;

	    case F_TEST:
		r = lfaccess (ip, begin_off, end_off, 'T');
		break;

	    case F_ULOCK:
		r = lffree (ip, begin_off, end_off);
		break;

	    default:
		u.u_error = EINVAL;
		r = -1;

	}	/* end switch */

	SLEEPFREE (&ip->i_lock);

	return (r);

}	/* end lockf */

/*
 ****************************************************************
 *	Chamada ao sistema "mknod"				*
 ****************************************************************
 */
int
mknod (const char *path, long mode, dev_t dev)
{
	INODE		*ip;
	IOREQ		io;
	DIRENT		de;

	if (((mode & IFMT) != IFIFO) && superuser () < 0)
		return (-1);

	if ((ip = iname (path, getuchar, ICREATE, &io, &de)) != NOINODE)
		{ u.u_error = EEXIST; iput (ip); return (-1); }

	if (u.u_error)
		return (-1);

	if ((ip = (*io.io_ip->i_fsp->fs_make_inode) (&io, &de, mode)) == NOINODE)
		return (-1);

	ip->i_rdev = dev;

	iput (ip);

	return (0);

}	/* end mknod */

/*
 ****************************************************************
 *	Função interna "kmkdev" (para atualizar "/dev")		*
 ****************************************************************
 */
int
kmkdev (const char *path, long mode, dev_t dev)
{
	INODE		*ip;
	IOREQ		io;
	DIRENT		de;

	/*
	 *	Verifica se já existe
	 */
	if ((ip = iname (path, getschar, ISEARCH, &io, &de)) != NOINODE)
	{
		if (ip->i_rdev == dev)
			{ iput (ip); return (0); }

		printf
		(	"kmkdev: Atualizei o dispositivo \"%s\" (%d/%d => %d/%d)\n",
			path, MAJOR (ip->i_rdev), MINOR (ip->i_rdev), MAJOR (dev), MINOR (dev)
		);

		ip->i_rdev = dev; inode_dirty_inc (ip); iput (ip); return (0);
	}

	/*
	 *	NÃO existe; tenta criar
	 */
	if ((ip = iname (path, getschar, ICREATE, &io, &de)) != NOINODE)
		{ iput (ip); goto bad; }

	if ((ip = (*io.io_ip->i_fsp->fs_make_inode) (&io, &de, mode)) == NOINODE)
		goto bad;

	ip->i_rdev = dev; iput (ip);

	printf ("kmkdev: Criei o dispositivo \"%s\" (%d/%d)\n", path, MAJOR (dev), MINOR (dev));

	return (0);

	/*
	 *	Em caso de erro, ...
	 */
    bad:
	printf
	(	"kmkdev: Não consegui criar o dispositivo \"%s\" (%s)\n",
		path, sys_errlist[u.u_error]
	);

	return (-1);

}	/* end kmkdev */

/*
 ****************************************************************
 *	Chamada ao sistema "dup"				*
 ****************************************************************
 */
int
dup (int fd1, int fd2)
{
	KFILE		*fp;
	int		fd, dup2;

#ifdef	MSG
	if (CSWT (7))
		printf ("%g: %d, %d, ", fd1, fd2);
#endif	MSG

	dup2 = fd1 & ~0x3F;

	fd1 &= 0x3F;

	if ((fp = fget (fd1)) == NOKFILE)
		return (-1);

	/*
	 *	Verifica qual das formas
	 */
	if ((dup2 & 0x40) == 0)
	{	
		/*
		 *	Dup
		 */
		if ((fd = ufeget ()) < 0)
			return (-1);
	}
	else
	{
		/*
		 *	Dup2
		 */
		fd = fd2;

		if (fd < 0 || fd >= NUFILE)
			{ u.u_error = EBADF; return (-1); }
	}

	/*
	 *	Epílogo
	 */
	if (fd != fd1)
	{
		if (u.u_ofile[fd] != NOKFILE)
			fclose (u.u_ofile[fd]);

		u.u_ofile[fd] = fp;
		fp->f_count++;
	}

#ifdef	MSG
	if (CSWT (7))
		printf ("%g: fd = %d, count = %d\n", fd, fp->f_count);
#endif	MSG

	return (fd);

}	/* end dup */

/*
 ****************************************************************
 *	Chamada ao sistema "sync"				*
 ****************************************************************
 */
int
sync (void)
{
	if (superuser () == 0)
		update (NODEV, 1 /* blocks_also */);

	return (UNDEF);

}	/* end sync */

/*
 ****************************************************************
 *	Atualiza os INODEs e BLOCOs no Disco			*
 ****************************************************************
 */
void
update (dev_t dev, int blocks_also)
{
	INODE		*ip;
	SB		*sp;

	/*
	 *	Espera se já tem esta operação em andamento
	 */
	if (TAS (&updlock) < 0)
		return;

	/*
	 *	Percorre a Lista de SBs, para atualiza-los no disco
	 */
    again:
	SPINLOCK (&sblistlock);

	for (sp = sb_head.sb_forw; sp != &sb_head; sp = sp->sb_forw)
	{
		/*
		 *	Verifica se deve atualizar
		 */
		if (dev != NODEV && dev != sp->sb_dev)
			continue;

		if ((sp->sb_mntent.me_flags & SB_RONLY) || sp->sb_sbmod == 0)
			continue;

		SPINFREE (&sblistlock);

		/*
		 *	Atualiza
		 */
		(*fstab[sp->sb_code].fs_update) (sp);

		sp->sb_sbmod = 0;

		goto again;

	}	/* end for (SBs) */

	SPINFREE (&sblistlock);

	/*
	 *	Atualiza os INODES no disco
	 */
	for (ip = scb.y_inode; ip < scb.y_endinode; ip++)
	{
		if (ip->i_mode == 0)
			continue;

		if (dev != NODEV && dev != ip->i_dev)
			continue;

		if ((sp = ip->i_sb) == NOSB)
			continue;

		if ((sp->sb_mntent.me_flags & SB_RONLY) || (ip->i_flags & ICHG) == 0)
			continue;

		SLEEPLOCK (&ip->i_lock, PINODE);

		if (ip->i_count == 0)
			{ SLEEPFREE (&ip->i_lock); continue; }

		iinc (ip);
#ifdef	MSG
		if (CSWT (6))
			printf ("%g: INODE (%v, %d)\n", ip->i_dev, ip->i_ino);
#endif	MSG
		(*fstab[sp->sb_code].fs_write_inode) (ip);

		iput (ip);

	}	/* end percorrendo a tabela de INODEs */

	/*
	 *	Atualiza os Blocos no Disco
	 */
	if (blocks_also)
		block_flush (dev);

	CLEAR (&updlock);

}	/* end update */
