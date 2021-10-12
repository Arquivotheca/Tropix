/*
 ****************************************************************
 *								*
 *			sysstat.c				*
 *								*
 *	Chamadas ao sistema: estado				*
 *								*
 *	Versão	3.0.0, de 01.01.95				*
 *		4.6.0, de 03.08.04				*
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

#include "../h/mntent.h"
#include "../h/sb.h"
#include "../h/kfile.h"
#include "../h/inode.h"
#include "../h/stat.h"
#include "../h/bhead.h"
#include "../h/sysdirent.h"
#include "../h/uerror.h"
#include "../h/signal.h"
#include "../h/uproc.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ******	Protótipos de funções ***********************************
 */
void		kstat (INODE *, STAT *, off_t);
void		kchdir (char *, INODE **);

/*
 ****************************************************************
 *	Chamada ao sistema "access"				*
 ****************************************************************
 */
int
access (char *path, int mode)
{
	int		svuid, svgid;
	INODE		*ip;
	IOREQ		io;
	DIRENT		de;

	/*
	 *	Coloca o usuário real
	 */
	svuid = u.u_euid;
	svgid = u.u_egid;

	u.u_euid = u.u_ruid;
	u.u_egid = u.u_rgid;

	/*
	 *	Obtem o arquivo
	 */
	if ((ip = iname (path, getuchar, ISEARCH, &io, &de)) != NOINODE)
	{
		if (mode & (IREAD >> 6))
			kaccess (ip, IREAD);

		if (mode & (IWRITE >> 6))
			kaccess (ip, IWRITE);

		if (mode & (IEXEC >> 6))
			kaccess (ip, IEXEC);

		iput (ip);
	}

	/*
	 *	Restaura o usuário efetivo. 
	 */
	u.u_euid = svuid;
	u.u_egid = svgid;

	return (UNDEF);

}	/* end access */

/*
 ****************************************************************
 *	Chamada ao sistema "fstat"				*
 ****************************************************************
 */
int
fstat (int fd, STAT *sp)
{
	KFILE		*fp;

	if ((fp = fget (fd)) == NOKFILE)
		return (-1);

	kstat (fp->f_inode, sp, fp->f_flags & O_PIPE ? fp->f_offset_low : (off_t)0);

	return (UNDEF);

}	/* end fstat */

/*
 ****************************************************************
 *	Chamada ao sistema "stat"				*
 ****************************************************************
 */
int
stat (const char *path, STAT *sp)
{
	INODE		*ip;
	IOREQ		io;
	DIRENT		de;

	if ((ip = iname (path, getuchar, ISEARCH, &io, &de)) == NOINODE)
		return (-1);

	kstat (ip, sp, (off_t)0);

	iput (ip);

	return (UNDEF);

}	/* end stat */

/*
 ****************************************************************
 *	Chamada ao sistema "lstat"				*
 ****************************************************************
 */
int
lstat (const char *path, STAT *sp)
{
	INODE		*ip;
	IOREQ		io;
	DIRENT		de;

	if ((ip = iname (path, getuchar, LSEARCH, &io, &de)) == NOINODE)
		return (-1);

	kstat (ip, sp, (off_t)0);

	iput (ip);

	return (UNDEF);

}	/* end lstat */

/*
 ****************************************************************
 *	Chamada ao sistema "instat"				*
 ****************************************************************
 */
int
instat (int dev, int ino, STAT *sp)
{
	INODE		*ip;

	/*
	 *	Introdução
	 */
	if ((ip = iget (dev, ino, 0)) == NOINODE)
		return (-1);

	kstat (ip, sp, (off_t)0);

	iput (ip);

	return (UNDEF);

}	/* end instat */

/*
 ****************************************************************
 *	Chamada ao sistema "upstat"				*
 ****************************************************************
 */
int
upstat (int dev, STAT *sp)
{
	INODE		*ip, *par_ip;
	ino_t		root_ino;
	SB		*sbp;
	DIRENT		dirent;
	IOREQ		io;

	/*
	 *	Introdução: Certifica-se de que é a raiz de um sistema montado
	 */
	if ((sbp = sbget (dev)) == NOSB)
		{ u.u_error = ENOTMNT; return (-1); }

	root_ino = sbp->sb_root_ino;

	sbput (sbp);

	if ((ip = iget (dev, root_ino, 0)) == NOINODE)
		return (-1);

	if (ip == rootdir || ip == u.u_rdir)
		{ u.u_error = EINVAL; goto bad; }

	/*
	 *	 Obtém o diretório embaixo da montagem
	 */
	par_ip = ip->i_sb->sb_inode;
	iput (ip);	

	SLEEPLOCK (&par_ip->i_lock, PINODE);
	iinc (ip = par_ip);

	/*
	 *	Procura o ".." neste diretório
	 */
	if (ip->i_par_ino != NOINO)
	{
		dirent.d_ino = ip->i_par_ino;
	}
	else
	{
		strcpy (dirent.d_name, ".."); dirent.d_namlen = 2;

		io.io_ip = ip;

		if ((*ip->i_fsp->fs_search_dir) (&io, &dirent, 0) <= 0)
		{
			if (u.u_error == NOERR)
				u.u_error = ENOENT;

			goto bad;
		}

		ip->i_par_ino = dirent.d_ino;
	}

	/*
	 *	Efetua o "stat" pedido
	 */
	kstat (ip, sp, (off_t)0);

    bad:
	iput (ip);

	return (dirent.d_ino);

}	/* end upstat */

/*
 ****************************************************************
 *	Interface interna de "stat"				*
 ****************************************************************
 */
void
kstat (INODE *ip, STAT *sp, off_t offset)
{
	STAT		s;

	/*
	 *	Copia as informações do INODE
	 */
	s.st_dev   = ip->i_dev;
	s.st_ino   = ip->i_ino;
	s.st_mode  = ip->i_mode;
	s.st_nlink = ip->i_nlink;
	s.st_uid   = ip->i_uid;
	s.st_gid   = ip->i_gid;
	s.st_rdev  = ip->i_rdev;
	s.st_size  = ip->i_size - offset;

	s.st_atime = ip->i_atime;
	s.st_mtime = ip->i_mtime;
	s.st_ctime = ip->i_ctime;

	if (unimove (sp, &s, sizeof (STAT), SU) < 0)
		u.u_error = EFAULT;

}	/* end kstat */

/*
 ****************************************************************
 *	Chamada ao sistema "chdir"				*
 ****************************************************************
 */
int
chdir (char *path)
{
	kchdir (path, &u.u_cdir);

	return (UNDEF);

}	/* end chdir */

/*
 ****************************************************************
 *	Chamada ao sistema "chroot"				*
 ****************************************************************
 */
int
chroot (char *path)
{
	if (superuser () == 0)
		kchdir (path, &u.u_rdir);

	return (UNDEF);

}	/* end chroot */

/*
 ****************************************************************
 *	Código comum para "chdir" e "chroot"			*
 ****************************************************************
 */
void
kchdir (char *path, INODE **ipp)
{
	INODE		*ip;
	IOREQ		io;
	DIRENT		de;

	if ((ip = iname (path, getuchar, ISEARCH, &io, &de)) == NOINODE)
		return;

	if ((ip->i_mode & IFMT) != IFDIR)
		{ u.u_error = ENOTDIR; goto bad; }

	if (kaccess (ip, IEXEC) < 0)
		goto bad;

	SLEEPFREE (&ip->i_lock);

	if (*ipp != NOINODE)
		{ SLEEPLOCK (&(*ipp)->i_lock, PINODE); iput (*ipp); }

	*ipp = ip;

	return;

    bad:
	iput (ip);

	return;

}	/* end kchdir */

/*
 ****************************************************************
 *	Chamada ao sistema "chmod"				*
 ****************************************************************
 */
int
chmod (char *path, long mode)
{
	INODE		*ip;

	if ((ip = owner (path)) == NOINODE)
		return (-1);

	if (ip->i_sb->sb_mntent.me_flags & SB_RONLY)
		{ u.u_error = EROFS; goto out; }

	xuntext (ip, 0 /* no lock */);

	if (ip->i_flags & ITEXT)
		{ u.u_error = ETXTBSY; goto out; }

	ip->i_mode &= IFMT;

	if (u.u_euid != 0)
	{
		mode &= ~(ISVTX|IMETX);

		if (u.u_egid != ip->i_gid)
			mode &= ~S_ISGID;
	}

	ip->i_mode  |= mode & ~IFMT;
	inode_dirty_inc (ip);

#if (0)	/*************************************/
	if ((ip->i_flags & ITEXT) && (ip->i_mode & (ISVTX|IMETX)) == 0)
		xuntext (ip, 0 /* no lock */);
#endif	/*************************************/

    out:
	iput (ip);

	return (UNDEF);

}	/* end chmod */

/*
 ****************************************************************
 *	Chamada ao sistema "chown"				*
 ****************************************************************
 */
int
chown (char *path, int uid, int gid)
{
	INODE	 *ip;

	if ((ip = owner (path)) == NOINODE)
		return (-1);

	if (ip->i_sb->sb_mntent.me_flags & SB_RONLY)
		{ u.u_error = EROFS; iput (ip); return (-1); }

	if (u.u_euid != 0)
		ip->i_mode &= ~(ISUID|ISGID);

	ip->i_uid = uid;
	ip->i_gid = gid;

	inode_dirty_inc (ip);

	iput (ip);

	return (0);

}	/* end chown */

/*
 ****************************************************************
 *	Chamada ao sistema "umask"				*
 ****************************************************************
 */
int
umask (int mask)
{
	int		old;

	old = u.u_cmask; 	u.u_cmask = mask & 0777;

	return (old);

}	/* end umask */

/*
 ****************************************************************
 *	Chamada ao sistema "chdirip"				*
 ****************************************************************
 */
int
chdirip (INODE *ip)
{
	if (superuser () < 0)
		return (-1);

	if
	(	ip < scb.y_inode || ip >= scb.y_endinode ||
		((unsigned)ip - (unsigned)scb.y_inode) % sizeof (INODE) != 0
	)
	{
		u.u_error = ENOTDIR;
		return (-1);
	}

	/*
	 *	Incrementa o DIR novo
	 */
	SLEEPLOCK (&ip->i_lock, PINODE);

	if (ip->i_count == 0 || (ip->i_mode & IFMT) != IFDIR)
		{ u.u_error = ENOTDIR; goto bad; }

	iinc (ip);

	SLEEPFREE (&ip->i_lock);

	/*
	 *	Decrementa o DIR antigo
	 */
	if (u.u_cdir != NOINODE)
		{ SLEEPLOCK (&u.u_cdir->i_lock, PINODE); iput (u.u_cdir); }

	u.u_cdir = ip;

	return (0);

    bad:
	SLEEPFREE (&ip->i_lock);

	return (-1);

}	/* end chdirip */

/*
 ****************************************************************
 *	Chamada ao sistema "untext"				*
 ****************************************************************
 */
int
untext (char *path)
{
	INODE		*ip;
	IOREQ		io;
	DIRENT		de;

	if (superuser () < 0)
		return (-1);

	if ((ip = iname (path, getuchar, ISEARCH, &io, &de)) == NOINODE)
		return (-1);

	if ((ip->i_flags & ITEXT) == 0)
		u.u_error = EINVAL;

	if (xuntext (ip, 0 /* no lock */) < 0)
		u.u_error = ETXTBSY;

	iput (ip);

	return (UNDEF);

}	/* end untext */

/*
 ****************************************************************
 *	Chamada ao sistema experimental "getdirpar"		*
 ****************************************************************
 */
ino_t
getdirpar (dev_t dev, ino_t ino, DIRENT *dep)
{
	INODE		*ip, *par_ip;
	ino_t		par_ino;
	IOREQ		io;
	DIRENT		dirent;

	/*
	 *	Verifica se é realmente um diretório
	 */
	if ((ip = iget (dev, ino, 0)) == NOINODE)
		return (-1);

	if ((ip->i_mode & IFMT) != IFDIR)
		{ u.u_error = ENOTDIR; goto bad0; }

	/*
	 *	Verifica se é "/"
	 */
	if (ip == rootdir)
		{ iput (ip); return (0); }

	/*
	 *	Verifica se é a raiz de um sistema montado
	 */
	if (ino == ip->i_sb->sb_root_ino)
	{
		if ((par_ip = ip->i_sb->sb_inode) == NOINODE)
			{ u.u_error = ENOTMNT; goto bad0; }

		iput (ip); ip = par_ip; SLEEPLOCK (&ip->i_lock, PINODE); iinc (ip);
	
		/* Substitui pelo diretório de montagem */

		ino = ip->i_ino; dev = ip->i_dev;
	}

	/*
	 *	Verifica se já tem o INO do pai
	 */
	if (ip->i_par_ino == NOINO)
	{
		strcpy (dirent.d_name, ".."); dirent.d_namlen = 2; io.io_ip = ip;

		if ((*ip->i_fsp->fs_search_dir) (&io, &dirent, 0) <= 0)
		{
			if (u.u_error == NOERR)
				u.u_error = ENOENT;

			goto bad0;
		}
#undef	DEBUG
#ifdef	DEBUG
		printf ("%g: Usei \"search_dir\" (1) %d => %d\n", ino, dirent.d_ino);
#endif	DEBUG
		ip->i_par_ino = dirent.d_ino;
	}

	par_ino = ip->i_par_ino;

	/*
	 *	Verifica se já tem o próprio nome
	 */
	if (ip->i_nm == NOSTR)
	{
		SLEEPFREE (&ip->i_lock);

		if ((par_ip = iget (dev, par_ino, 0)) == NOINODE)
			goto bad1;

		dirent.d_ino = ino; io.io_ip = par_ip;

		if ((*par_ip->i_fsp->fs_search_dir) (&io, &dirent, INUMBER) <= 0)
		{
			if (u.u_error == NOERR)
				u.u_error = ENOENT;

			goto bad2;
		}
#ifdef	DEBUG
		printf ("%g: Usei \"search_dir\" (2) (%d, %v) => \"%s\"\n", par_ino, dev, dirent.d_name);
#endif	DEBUG
		iput (par_ip);

		SLEEPLOCK (&ip->i_lock, PINODE);

		if ((ip->i_nm = malloc_byte (dirent.d_namlen + 2)) == NOSTR)
			{ u.u_error = ENOMEM; goto bad0; }

		ip->i_nm[0] = dirent.d_namlen; strcpy (ip->i_nm + 1, dirent.d_name);
	}

	/*
	 *	Finalmente, preenche a estrutura desejada
	 */
	dirent.d_namlen = ip->i_nm[0]; strcpy (dirent.d_name, ip->i_nm + 1);
	dirent.d_ino	= ino;
	dirent.d_offset = dev;

	iput (ip);

	if (unimove (dep, &dirent, sizeof (DIRENT), SU) < 0)
		u.u_error = EFAULT;

	return (par_ino);

	/*
	 *	Em caso de erro, ...
	 */
    bad2:
	iput (par_ip);
    bad1:
	SLEEPLOCK (&ip->i_lock, PINODE);
    bad0:
	iput (ip); return (-1);

}	/* end getdirpar */
