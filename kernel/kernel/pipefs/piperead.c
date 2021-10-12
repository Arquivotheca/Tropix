/*
 ****************************************************************
 *								*
 *			piperead.c				*
 *								*
 *	Leitura e escrita de arquivos				*
 *								*
 *	Versão	4.3.0, de 04.09.02				*
 *		4.3.0, de 04.09.02				*
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

#include "../h/map.h"
#include "../h/frame.h"
#include "../h/kfile.h"
#include "../h/inode.h"
#include "../h/uerror.h"
#include "../h/signal.h"
#include "../h/uproc.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Definições globais					*
 ****************************************************************
 *
 */
#define	PIPESZ	(8 * KBSZ)	/* Tamanho máximo de um PIPE */

/*
 ****************************************************************
 *	Criação de um PIPE					*
 ****************************************************************
 */
int
pipe (void)
{
	INODE		*ip;
	KFILE		*rfp, *wfp;
	int		rfd;
	pg_t		pgno;

	/*
	 *	Obtém um INODE para o PIPE
	 */
	if ((ip = pipe_alloc_inode ()) == NOINODE)
		return (-1);

	/*
	 *	Aloca uma área no final da memoria para conter o PIPE
	 */
	if ((pgno = malloce (M_CORE, BYUPPG (PIPESZ))) == NOPG)
		{ iput (ip); u.u_error = ENOMEM; return (-1); }

	/*
	 *	Obtém um KFILE
	 */
	if ((rfp = feget ()) == NOKFILE)
		{ iput (ip); goto bad; }

	rfd = u.u_fd;

	if ((wfp = feget ()) == NOKFILE)
	{
		rfp->f_count = 0;
		u.u_ofile[rfd] = NOKFILE;
		iput (ip);
		goto bad;
	}

	u.u_frame->s.sf_r1 = u.u_fd;

	wfp->f_flags = O_WRITE|O_PIPE;
	wfp->f_inode = ip;

	rfp->f_flags = O_READ|O_PIPE;
	rfp->f_inode = ip;

	iinc (ip);

	ip->i_pipeaddr = pgno;

	SLEEPFREE (&ip->i_lock);

	return (rfd);

	/*
	 *	Em caso de erro, ...
	 */
    bad:
	mrelease (M_CORE, BYUPPG (PIPESZ), pgno);
	return (-1);

}	/* end pipe */

/*
 ****************************************************************
 *	Close PIPE						*
 ****************************************************************
 */
void
pipe_kfile_close (KFILE *fp)
{
	INODE		*ip;
	UPROC		*up;

	/*
	 *	"ip" vem e volta LOCKED
	 */
	ip = fp->f_inode;

	if (ip->i_union != IN_PIPE)
		printf ("%g: Campo \"union\" não PIPE\n");

	if (fp->f_flags & O_READ)
	{
		EVENTDONE (&ip->i_notfull);
	}
	else
	{
		EVENTDONE (&ip->i_notempty);

		if (ip->i_attention_set && (up = ip->i_uproc)->u_attention_index < 0)
		{
			ip->i_attention_set = 0;
			up->u_attention_index = ip->i_fd_index;
			EVENTDONE (&up->u_attention_event);
		}
	}

	if (ip->i_count == 1)
	{
		mrelease (M_CORE, BYUPPG (PIPESZ), ip->i_pipeaddr);

		ip->i_union = IN_NULL;
		memclr (&ip->u, sizeof (ip->u));

		/* O "close" final do INODE */

		if ((ip->i_mode & IFMT) == IFIFO)
			ip->i_mode |= IREAD|IWRITE;

		ip->i_flags &= ~ILOCK;
	}

	iput (ip);

}	/* end pipe_close */

/*
 ****************************************************************
 *	Open PIPE						*
 ****************************************************************
 */
void
pipe_open (INODE *ip, int flag)
{
}	/* end pipe_open */

/*
 ****************************************************************
 *	Close PIPE						*
 ****************************************************************
 */
void
pipe_close (INODE *ip)
{
}	/* end pipe_close */

/*
 ****************************************************************
 *	Leitura de um PIPE					*
 ****************************************************************
 */
void
pipe_read (IOREQ *iop)
{
	KFILE		*fp = iop->io_fp;
	INODE		*ip;
	off_t		count;
	void		*vp;

	/*
	 *	Verifica se o pipe contem algo
	 */
	ip = fp->f_inode;
    loop:
	SLEEPLOCK (&ip->i_lock, PINODE);

	if (fp->f_offset_low == ip->i_size)
	{
		/*
		 *	O pipe está vazio; verifica
		 *	se tem um processo escrevendo nele
		 */
		SLEEPFREE (&ip->i_lock);

		if (ip->i_count < 2)
			return;

		if (fp->f_flags & O_NDELAY)
			return;

		EVENTWAIT (&ip->i_notempty, PPIPE);
		goto loop;
	}

	/*
	 *	Le o Pipe
	 */
	count = MIN (iop->io_count, ip->i_size - fp->f_offset_low);

	vp = (void *)(PGTOBY (ip->i_pipeaddr) + fp->f_offset_low);

	if (memmove (iop->io_area, vp, count) < 0)
	{
		u.u_error = EFAULT;
		return;
	}

   /***	iop->io_area += count; ***/
	fp->f_offset_low += count;
	iop->io_count -= count;

	/*
	 *	Se o leitor alcancou o escritor
	 *	reinicializa tudo
	 */
	if (fp->f_offset_low == ip->i_size)
	{
		fp->f_offset_low = 0;
		ip->i_size = 0;
		EVENTCLEAR (&ip->i_notempty);
		EVENTDONE (&ip->i_notfull);
	}

	SLEEPFREE (&ip->i_lock);

}	/* end pipe_read */

/*
 ****************************************************************
 *	Escrita em um PIPE					*
 ****************************************************************
 */
void
pipe_write (IOREQ *iop)
{
	KFILE		*fp = iop->io_fp;
	off_t		count;
	INODE		*ip;
	UPROC		*up;
	void		*vp;

	ip = fp->f_inode;

	/*
	 *	Verifica se terminou a escrita
	 */
    loop:
	if (iop->io_count == 0)
		return;

	SLEEPLOCK (&ip->i_lock, PINODE);

	/*
	 *	Verifica se tem leitor e escritor
	 */
	if ((ip->i_mode & IFMT) != IFIFO && ip->i_count < 2)
	{
		SLEEPFREE (&ip->i_lock);
		u.u_error = EPIPE;
		sigproc (u.u_myself, SIGPIPE);
		return;
	}

	/*
	 *	Se o pipe está cheio, espera o leitor ler um trecho
	 */
	if (ip->i_size >= PIPESZ)
	{
#if (0)	/*******************************************************/
		if (((ip->i_mode & IFMT) == IFIFO) && (fp->f_flags & O_NDELAY))
#endif	/*******************************************************/
		if (fp->f_flags & O_NDELAY)
			{ SLEEPFREE (&ip->i_lock); u.u_error = EPFULL; return; }

		SLEEPFREE (&ip->i_lock);
		EVENTWAIT (&ip->i_notfull, PPIPE);
		goto loop;
	}

	/*
	 *	Escreve o que é possivel
	 */
	count = MIN (iop->io_count, PIPESZ - ip->i_size);

	vp = (void *)(PGTOBY (ip->i_pipeaddr) + ip->i_size);

	if (memmove (vp, iop->io_area, count) < 0)
		{ u.u_error = EFAULT; return; }

   	iop->io_area += count;
	iop->io_count -= count;
	ip->i_size += count;

	if (ip->i_size == PIPESZ)
		EVENTCLEAR (&ip->i_notfull);

	SLEEPFREE (&ip->i_lock);
	EVENTDONE (&ip->i_notempty);

	if (ip->i_attention_set && (up = ip->i_uproc)->u_attention_index < 0)
	{
		ip->i_attention_set = 0;
		up->u_attention_index = ip->i_fd_index;
		EVENTDONE (&up->u_attention_event);
	}

	goto loop;

}	/* end pipe_write */

/*
 ****************************************************************
 *	Open de um FIFO 					*
 ****************************************************************
 */
void
pfifo (KFILE *fp)
{
	INODE		*ip;
	pg_t		pgno;

	/*
	 *	Recebe e devolve o INODE locked
	 */
	ip = fp->f_inode;

	ip->i_fsp = &fstab[FS_PIPE];

	/*
	 *	Se for o primeiro parceiro, aloca os recursos	
	 */
	if   (ip->i_count == 1)
	{
		if (ip->i_union != IN_NULL)
			printf ("%g: Campo \"union\" não NULO\n");

		if ((pgno = malloce (M_CORE, BYUPPG (PIPESZ))) == NOPG)
			{ u.u_error = ENOMEM; return; }

		/*
		 *	Depois de alterar a parte "text", não deve ter êrro
		 */
		ip->i_pipeaddr = pgno;
		ip->i_union = IN_PIPE;

		EVENTSET   (&ip->i_notfull);
		EVENTCLEAR (&ip->i_notempty);
		EVENTCLEAR (&ip->i_fifopar);
	}
	elif (ip->i_count == 2)
	{
		if (ip->i_union != IN_PIPE)
			printf ("%g: Campo \"union\" não PIPE\n");

		/*
		 *	Notifica o parceiro
		 */
		EVENTDONE (&ip->i_fifopar);
	}
	else
	{
		/*
		 *	Não podemos ter FIFOs com mais de 2 parceiros
		 */
		u.u_error = EPIPE;
	}

	/*
	 *	Epílogo	
	 */
	fp->f_flags |= O_PIPE;

}	/* end pfifo */

/*
 ****************************************************************
 *	Espera pelo parceiro do FIFO				*
 ****************************************************************
 */
void
pfifowait (KFILE *fp)
{
	INODE		 *ip;

	if (fp->f_flags & O_NDELAY)
		return;

	ip = fp->f_inode;

	if (ip->i_count == 1)
		EVENTWAIT (&ip->i_fifopar, PPIPE);

}	/* end pfifowait */

/*
 ****************************************************************
 *	Prepara o "attention" do PIPE				*
 ****************************************************************
 */
int
pipe_attention (int fd_index, char **attention_set)
{
	const KFILE	*fp;
	INODE		*ip;

	fp = u.u_fileptr; ip = fp->f_inode;

	if ((fp->f_flags & (O_READ|O_PIPE)) != (O_READ|O_PIPE))
		{ u.u_error = EBADF; return (-1); }

	if (EVENTTEST (&ip->i_notempty) >= 0)
		return (1);

	ip->i_uproc	= u.u_myself;
	ip->i_fd_index	= fd_index;

	ip->i_attention_set = 1;
	*attention_set = &ip->i_attention_set;

	return (0);

}	/* end pipe_attention */
