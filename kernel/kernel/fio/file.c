/*
 ****************************************************************
 *								*
 *			file.c					*
 *								*
 *	Algoritmos de manipulação de KFILESs			*
 *								*
 *	Versão	3.0.0, de 22.11.94				*
 *		4.6.0, de 16.06.04				*
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
#include "../h/mon.h"
#include "../h/inode.h"
#include "../h/stat.h"
#include "../h/kfile.h"
#include "../h/iotab.h"
#include "../h/sysdirent.h"
#include "../h/ipc.h"
#include "../h/uerror.h"
#include "../h/signal.h"
#include "../h/uproc.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Variáveis e definições globais				*
 ****************************************************************
 */
#define	MAXSZ	0x7FFFFFFF	/* Max. tamanho de um off_t */

entry LOCK	filelock;	/* Lock da tabela KFILE */

/*
 ****************************************************************
 *	Open de um arquivo					*
 ****************************************************************
 */
void
fopen (INODE *ip, int oflag, int foflag)
{
	KFILE		*fp;

	/*
	 *	Recebe o "ip" locked, e devolve free. Verifica as permissões
	 *	para Arquivos antigos e criação com truncamento.
	 */
	if (foflag != FCNEW)
	{
		if (oflag & O_READ)
			kaccess (ip, IREAD);

		if (oflag & O_WRITE)
		{
			kaccess (ip, IWRITE);

			if ((ip->i_mode & IFMT) == IFDIR)
				u.u_error = EISDIR;
		}
	}

	if (u.u_error)
		goto bad;

	/*
	 *	Se for "criação com truncamento", trunca o arquivo.
	 */
	if (oflag & O_TRUNC || foflag == FCTRUNC)
	{
		/*
		 *	Antes de truncar o arquivo, verifica se tem LOCKs
		 */
		if (ip->i_lockf && lfaccess (ip, 0, MAXSZ, 'T') < 0)
				goto bad;

		(*fstab[ip->i_sb->sb_code].fs_trunc_inode) (ip);
	}

	/*
	 *	Obtem um KFILE para o arquivo.
	 */
	if ((fp = feget ()) == NOKFILE)
		goto bad;

	/*
	 *	Prepara a entrada KFILE.
	 *	Repare que os Bits de Read/Write coincidem.
	 */
	fp->f_flags = oflag & (O_RW|O_NDELAY|O_APPEND);

	fp->f_inode = ip;

	/*
	 *	O FIFO tem tratamento especial	
	 */
	if ((ip->i_mode & IFMT) == IFIFO)
		pfifo (fp);

	/*
	 *	Processa o Open do INODE.
	 */
	(*ip->i_fsp->fs_open) (ip, oflag);

	/*
	 *	Verifica se houve algum erro.
	 */
	if (u.u_error == NOERR)
	{
		SLEEPFREE (&ip->i_lock);

		if ((ip->i_mode & IFMT) == IFIFO)
			pfifowait (fp);

		return;
	}

	/*
	 *	Houve erro: desaloca as entradas KFILE e UFILE.
	 *	Repare que em "u_fd" está o fd.
	 */
	u.u_ofile[u.u_fd] = NOKFILE;
	fclear (fp);

	/*
	 *	Caso de erro
	 */
    bad:
	iput (ip);

}	/* end fopen */

/*
 ****************************************************************
 *	Close de um Arquivo					*
 ****************************************************************
 */
void
fclose (KFILE *fp)
{
	INODE		*ip;

	if (fp == NOKFILE)
		return;

	u.u_fileptr = fp;

	/*
	 *	Obtem o INODE assocido ao KFILE.
	 */
	ip = fp->f_inode;

	SLEEPLOCK (&ip->i_lock, PINODE);

	/*
	 *	Libera os LOCKs
	 */
	if (ip->i_lockf)
		lfclosefree (ip);

	/*
	 *	Se não era o ultimo usuario, simplesmente decrementa o uso.
	 */
	if (fp->f_count > 1)
		{ fp->f_count--; SLEEPFREE (&ip->i_lock); return; }

	/*
	 *	Era o ultimo usuario. Libera a entrada.
	 */
	if (fp->f_flags & O_PIPE)
		pipe_kfile_close (fp);

	/*
	 *	Libera as entradas dos eventos, semáforos e regiões locais
	 */
	switch (fp->f_union)
	{
	    case KF_EVENT:
		ueventlfcrelease (fp);
		break;

	    case KF_SEMA:
		usemalfcrelease (fp);
		break;

	    case KF_SHMEM:
		shmem_release (fp);
		break;

	}	/* end switch */

	(*ip->i_fsp->fs_close) (ip);

   /***	fp->f_union = KF_NULL; ***/
	fclear (fp);

}	/* end fclose */

/*
 ****************************************************************
 *	Obtem o Ponteiro para a estrutura KFILE			*
 ****************************************************************
 */
KFILE *
fget (int fd)
{
	KFILE		*fp;

	if ((unsigned)fd < NUFILE && (fp = u.u_ofile[fd]) != NOKFILE)
		{ u.u_fileptr = fp; return (fp); }

	u.u_error = EBADF;
	return (NOKFILE);

}	/* end fget */

/*
 ****************************************************************
 *	Aloca uma entrada da tabela UFILE e da KFILE		*
 ****************************************************************
 */
KFILE *
feget (void)
{
	KFILE		*fp;
	int		fd;

	if ((fd = ufeget ()) < 0)
		return (NOKFILE);

	/*
	 *	Conseguiu uma entrada da tabela UFILE.
	 *	Tenta obter uma da KFILE.
	 */
	SPINLOCK (&filelock);

	for (fp = scb.y_kfile; fp < scb.y_endkfile; fp++)
	{
		if (fp->f_count == 0)
		{
			u.u_ofile[fd] = fp;

			fp->f_count++;
		   /*** fp->f_offset = 0; ***/
		   /*** fp->f_flags  = 0; ***/

			mon.y_kfile_in_use_cnt++;

			SPINFREE (&filelock);

			u.u_fileptr = fp;

			return (fp);
		}
	}

	/*
	 *	A Tabela KFILE está cheia.
	 */
	SPINFREE (&filelock);

	printf ("Feget: Tabela KFILE cheia\n");

	u.u_error = ENFILE;

	return (NOKFILE);

}	/* end feget */

/*
 ****************************************************************
 *	Desaloca uma entrada KFILE				*
 ****************************************************************
 */
void
fclear (KFILE *fp)
{
	SPINLOCK (&filelock);

	memclr (fp, sizeof (KFILE) );

	mon.y_kfile_in_use_cnt--;

	SPINFREE (&filelock);

}	/* end fclear */

/*
 ****************************************************************
 *	Aloca uma entrada UFILE					*
 ****************************************************************
 */
int
ufeget (void)
{
	int		fd;

	for (fd = 0; fd < NUFILE; fd++)
	{
		if (u.u_ofile[fd] == NOKFILE)
		{
			u.u_fd = fd;
			u.u_ofilef[fd] = 0;
			return (fd);
		}
	}

	u.u_error = EMFILE;
	return (-1);

}	/* end ufeget */

/*
 ****************************************************************
 *	Verifica se o acesso é válido				*
 ****************************************************************
 */
int
kaccess (INODE *ip, long mode)
{
	long		tst_mode;

	/*
	 *	O argumento "mode" deve conter apenas um dos bits IREAD, IWRITE ou IEXEC
	 */
	if (mode == IWRITE)
	{
		if (ip->i_sb->sb_mntent.me_flags & SB_RONLY)
			{ u.u_error = EROFS; return (-1); }

		if (xuntext (ip, 0 /* no lock */) < 0)
			{ u.u_error = ETXTBSY; return (-1); }
	}

	/*
	 *	O caso especial do superusuário
	 */
	if (u.u_euid == 0)
	{
		if (mode == IEXEC && ip->i_mode & (S_IXUSR|S_IXGRP|S_IXOTH) == 0)
			{ u.u_error = EACCES; return (-1); }

		return (0);
	}

	/*
	 *	Verifica as Permissões
	 */
	tst_mode = mode >> (2 * IPSHIFT);

	if (u.u_egid == ip->i_gid)
		tst_mode |= mode >> IPSHIFT;

	if (u.u_euid == ip->i_uid)
		tst_mode |= mode;

	if ((ip->i_mode & tst_mode) == 0)
		{ u.u_error = EACCES; return (-1); }

	return (0);

}	/* end kaccess */


#if (0)	/*******************************************************/
	/*
	 *	Verifica as Permissões.
	 */
	if (u.u_euid == 0)
		return (0);

	if (u.u_euid != ip->i_uid)
	{
		mode >>= IPSHIFT;

		if (u.u_egid != ip->i_gid)
			mode >>= IPSHIFT;
	}

	if ((ip->i_mode & mode) != 0)
		return (0);

	u.u_error = EACCES;

	return (-1);
#endif	/*******************************************************/

/*
 ****************************************************************
 *	Verifica se é o dono ou superusuário			*
 ****************************************************************
 */
INODE *
owner (const char *path)
{
	INODE		*ip;
	IOREQ		io;
	DIRENT		de;

	if ((ip = iname (path, getuchar, ISEARCH, &io, &de)) == NOINODE)
		return (NOINODE);

	if (u.u_euid == ip->i_uid || superuser () == 0)
		return (ip);

	iput (ip); return (NOINODE);

}	/* end owner */

/*
 ****************************************************************
 *	Verifica se é o superusuário				*
 ****************************************************************
 */
int
superuser (void)
{
	if (u.u_euid == 0)
		return (0);

	u.u_error = EPERM; return (-1);

}	/* end superuser */
