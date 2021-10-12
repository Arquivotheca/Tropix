/*
 ****************************************************************
 *								*
 *			pty.c					*
 *								*
 *	Driver para os pseudo-terminais				*
 *								*
 *	Versão	2.3.0, de 09.02.91				*
 *		4.6.0, de 23.08.04				*
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

#include "../h/devmajor.h"
#include "../h/tty.h"
#include "../h/ioctl.h"
#include "../h/kfile.h"
#include "../h/inode.h"
#include "../h/sysdirent.h"
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
entry TTY	*kernel_pty_tp;		/* Para enviar mensagens do KERNEL */

/*
 ******	Redefinições ********************************************
 */
#define	t_ptydata t_addr		/* Usa o "t_addr" para apontador da continuação */

/*
 ******	Protótipos **********************************************
 */
void		ptyopen (dev_t, OFLAG, char *, LOCK *);
void		ptyclose (dev_t, int, char *, LOCK *);
void		ptystart (TTY *, int); 
int		pty_convert_and_copy (IOREQ *, TTY *);

/*
 ****************************************************************
 *	Procura e abre um par de pseudo terminais		*
 ****************************************************************
 */
int
getpty (PTYIO *pp)
{
	int		unit;
	PTYIO		ptyio;
	INODE		*cip, *sip;
	IOREQ		io;
	DIRENT		de;

	/*
	 *	Procura uma entrada vaga
	 */
	for (unit = 0; /* abaixo */; unit++)
	{
		if (unit >= scb.y_npty)
			{ u.u_error = EBUSY; return (-1); }

		if (TAS (&scb.y_pty[unit].ptys_lock) >= 0)
			break;
	}

	/*
	 *	Servidor
	 */
	sprintf (ptyio.t_nm_server, "/dev/ptys%02d", unit);

	if ((sip = iname (ptyio.t_nm_server, getschar, ISEARCH, &io, &de)) == NOINODE)
		return (-1);

	fopen (sip, O_RW, FOPEN);

	if (u.u_error != NOERR)
		return (-1);

	ptyio.t_fd_server = u.u_fd;

	/*
	 *	Cliente
	 */
	strcpy (ptyio.t_nm_client, ptyio.t_nm_server); ptyio.t_nm_client[8] = 'c';

	if ((cip = iname (ptyio.t_nm_client, getschar, ISEARCH, &io, &de)) == NOINODE)
		goto bad_0;

	fopen (cip, O_RW, FOPEN);

	if (u.u_error != NOERR)
		goto bad_0;

	ptyio.t_fd_client = u.u_fd;

	/*
	 *	Copia para o usuário
	 */
	if (unimove (pp, &ptyio, sizeof (PTYIO), SU) < 0)
		{ u.u_error = EFAULT; goto bad_1; }

	return (0);

	/*
	 *	Em caso de erro, ...
	 */
    bad_1:
	SLEEPLOCK (&cip->i_lock, PINODE); iput (cip);
    bad_0:
	SLEEPLOCK (&sip->i_lock, PINODE); iput (sip);

	return (-1);

}	/* end getpty */

/*
 ****************************************************************
 *	 Open do cliente					*
 ****************************************************************
 */
void
ptycopen (dev_t dev, OFLAG oflag)
{
	PTY		*pp;

	pp = &scb.y_pty[MINOR (dev)];

	ptyopen (dev, oflag, &pp->ptyc_nopen, &pp->ptyc_lock);

}	/* end ptycopen */

/*
 ****************************************************************
 *	 Open do servidor					*
 ****************************************************************
 */
void
ptysopen (dev_t dev, OFLAG oflag)
{
	PTY		*pp;

	pp = &scb.y_pty[MINOR (dev)];

	ptyopen (dev, oflag, &pp->ptys_nopen, &pp->ptys_lock);

}	/* end ptysopen */

/*
 ****************************************************************
 *	 Código comum do "open"					*
 ****************************************************************
 */
void
ptyopen (dev_t dev, OFLAG oflag, char *pty_nopen, LOCK *pty_lock)
{
	int		unit;
	TTY		*tp;

	/*
	 *	Prólogo
	 */
	if ((unsigned)(unit = MINOR (dev)) >= scb.y_npty)
		{ u.u_error = ENXIO; return; }

	if   (TAS (pty_lock) >= 0)
	{
		if ((oflag & O_LOCK) == 0)
			CLEAR (pty_lock);
	}
	elif ((oflag & O_LOCK) && *pty_nopen)
	{
		u.u_error = EBUSY; return;
	}

	tp =  &scb.y_pty[unit].pty;

	if (*pty_nopen > 0)
		{ ttyopen (dev, tp, oflag); (*pty_nopen)++; return; }

	/*
	 *	Abre este pseudo-terminal
	 */
	if ((tp->t_state & ISOPEN) == 0)
	{
		/*
		 *	Prepara diversos parâmetros
		 */
		tp->t_oproc = (void (*) ())ptystart;

		tp->t_ptydata = (void *)&scb.y_pty[unit];

	   /***	tp->t_irq   = ...; ***/

		tp->t_iflag = ICRNL|IXON;
		tp->t_oflag = OPOST|ONLCR;
		tp->t_cflag = B9600|CS8|CLOCAL;
		tp->t_lflag = ISIG|ICANON|ECHO|ECHOE|ECHOK|CNTRLX|ISO|VIDEO;

		EVENTCLEAR (&tp->t_outqnempty);
	}

	ttyopen (dev, tp, oflag);

	((char *)tp->t_termio.c_name)[3] = 'c';	/* O nome sempre do cliente */

	/*
	 *	Epílogo
	 */
	(*pty_nopen)++;

}	/* end ptyopen */

/*
 ****************************************************************
 *	Close do cliente					*
 ****************************************************************
 */
void
ptycclose (dev_t dev, int flag)
{
	PTY		*pp;

	pp = &scb.y_pty[MINOR (dev)];

	ptyclose (dev, flag, &pp->ptyc_nopen, &pp->ptyc_lock);

}	/* end ptycclose */

/*
 ****************************************************************
 *	Close do servidor					*
 ****************************************************************
 */
void
ptysclose (dev_t dev, int flag)
{
	PTY		*pp;

	pp = &scb.y_pty[MINOR (dev)];

	ptyclose (dev, flag, &pp->ptys_nopen, &pp->ptys_lock);

	if (pp->ptys_nopen > 0 || pp->ptyc_nopen > 0)
		return;

	if (&pp->pty == kernel_pty_tp)
		kernel_pty_tp = NOTTY;

}	/* end ptysclose */

/*
 ****************************************************************
 *	Close do cliente e servidor				*
 ****************************************************************
 */
void
ptyclose (dev_t dev, int flag, char *pty_nopen, LOCK *pty_lock)
{
	PTY		*pp;
	TTY		*tp;

	/*
	 *	Prólogo
	 */
	if (--(*pty_nopen) > 0)
		return;

	CLEAR (pty_lock);

	pp = &scb.y_pty[MINOR (dev)];

	if (pp->ptys_nopen > 0 || pp->ptyc_nopen > 0)
		return;

	/*
	 *	Esvazia a fila de saída, pois ninguém mais vai lê-la
	 */
	tp = &pp->pty;

   /***	SLEEPLOCK (&tp->t_olock, PTTYOUT); ***/

	ttyoflush (tp);

   /***	SLEEPFREE (&tp->t_olock); ***/

	ttyclose (tp);

   /***	EVENTCLEAR (&tp->t_outqnempty); ***/

#if (0)	/*******************************************************/
	if (tp == kernel_pty_tp)
		kernel_pty_tp = NOTTY;
#endif	/*******************************************************/

}	/* end ptyclose */

/*
 ****************************************************************
 *	Ioctl do cliente					*
 ****************************************************************
 */
int
ptycctl (dev_t dev, int cmd, void *arg, int flag)
{
	TTY		*tp;
	int		ret;

	tp = &scb.y_pty[MINOR (dev)].pty;

	/*
	 *	Processa/prepara o ATTENTION
	 */
	switch (cmd)
	{
		/*
		 *	Processa/prepara o ATTENTION
		 */
	    case U_ATTENTION:
	   /***	spl... (); ***/

		if (EVENTTEST (&tp->t_inqnempty) == 0)
			{ /*** spl0 (); ***/ return (1); }

		tp->t_uproc	= u.u_myself;
		tp->t_index	= (int)arg;

		tp->t_attention_set = 1;
		*(char **)flag	= &tp->t_attention_set;

	   /*** spl0 (); ***/

		return (0);

#ifdef	UNATTENTION
		/*
		 *	Desarma o ATTENTION
		 */
	    case U_UNATTENTION:

		tp->t_state &= ~ATTENTION;

		return (0);
#endif	UNATTENTION

	}	/* end switch */

	/*
	 *	Demais IOCTLs
	 */
   /***	SLEEPLOCK (&tp->t_olock, PTTYOUT); ***/

	ret = ttyctl (tp, cmd, arg);

   /***	SLEEPFREE (&tp->t_olock); ***/

	return (ret);

}	/* end ptycctl */

/*
 ****************************************************************
 *	Ioctl do servidor					*
 ****************************************************************
 */
int
ptysctl (dev_t dev, int cmd, void *arg, int flag)
{
	TTY		*tp;
	PTY		*pp;
	int		ret;

	tp = &scb.y_pty[MINOR (dev)].pty; pp = (PTY *)tp->t_ptydata;

	/*
	 *	IOCTLs especiais
	 */
	switch (cmd)
	{
		/*
		 *	Processa/prepara o ATTENTION
		 */
	    case U_ATTENTION:

	   /***	spl... (); ***/

		if (tp->t_state & TTYSTOP)
			EVENTCLEAR (&tp->t_outqnempty);

		if (EVENTTEST (&tp->t_outqnempty) == 0)
			{ /*** spl0 (); ***/ return (1); }

		pp->uproc	= u.u_myself;
		pp->index	= (int)arg;

		pp->attention_set	= 1;
		*(char **)flag	= &pp->attention_set;

	   /*** spl0 (); ***/

		return (0);

#ifdef	UNATTENTION
		/*
		 *	Desarma o ATTENTION
		 */
	    case U_UNATTENTION:

		tp->t_state &= ~ATTENTIONS;

		return (0);
#endif	UNATTENTION

		/*
		 *	Desvia o "printf" do kernel para um PTY
		 */
	    case TC_KERNEL_PTY_ON:
		if (superuser () < 0)
			return (-1);

		if (kernel_pty_tp != NOTTY)
			{ u.u_error = EINVAL; return (-1); }

		kernel_pty_tp = tp;
		return (0);

		/*
		 *	Restaura o "printf" do kernel para a console
		 */
	    case TC_KERNEL_PTY_OFF:

		if (superuser () < 0)
			return (-1);

		if (kernel_pty_tp != tp)
			{ u.u_error = EINVAL; return (-1); }

		kernel_pty_tp = NOTTY;
		return (0);

	}	/* end switch */

	/*
	 *	Demais IOCTLs
	 */
	if (cmd == TCNREAD)
		return (tp->t_outq.c_count);

   /***	SLEEPLOCK (&tp->t_olock, PTTYOUT); ***/

	ret = ttyctl (tp, cmd, arg);

   /***	SLEEPFREE (&tp->t_olock); ***/

	return (ret);

}	/* end ptysctl */

/*
 ****************************************************************
 *	Leitura do cliente					*
 ****************************************************************
 */
void
ptycread (IOREQ *iop)
{
	ttyread (iop, &scb.y_pty[MINOR (iop->io_dev)].pty);

}	/* end ptycread */

/*
 ****************************************************************
 *	Escrita do cliente					*
 ****************************************************************
 */
void
ptycwrite (IOREQ *iop)
{
	ttywrite (iop, &scb.y_pty[MINOR (iop->io_dev)].pty);

}	/* end ptycwrite */

/*
 ****************************************************************
 *	Leitura do servidor					*
 ****************************************************************
 */
void
ptysread (IOREQ *iop)
{
	TTY		*tp;
	int		c, count = 0;

	/*
	 *	Verifica se este pseudo-terminal está aberto
	 */
	tp = &scb.y_pty[MINOR (iop->io_dev)].pty;

	if ((tp->t_state & ISOPEN) == 0)
		{ u.u_error = EBADF; return; }

	/*
	 *	Testa o estado STOP
	 */
    again:
	if (tp->t_state & TTYSTOP)
		EVENTCLEAR (&tp->t_outqnempty);

	/*
	 *	Analisa a opção NO-DELAY	
	 */
	if   ((u.u_oflag & O_NDELAY) == 0)
		EVENTWAIT (&tp->t_outqnempty, PTTYOUT);
	elif (EVENTTEST (&tp->t_outqnempty) < 0)
		return;

	/*
	 *	Verifica se é COOKED ou não
	 */
	if ((tp->t_oflag & OPOST) == 0)
	{
		/*
		 *	Copia os caracteres da CLIST para a área do usuário
		 */
		if ((tp->t_lflag & CODE) == ISO)
			count = cread (&tp->t_outq, iop->io_area, iop->io_count, SU);
		else
			count = pty_convert_and_copy (iop, tp);

		if (count > 0)
		{
		   /*** iop->io_area	   += count; ***/
			iop->io_count	   -= count;
		   /*** iop->io_offset_low += count; ***/
		}
	}
	else	/* COOKED */
	{
		for (EVER)
		{
			SPINLOCK (&tp->t_outq.c_lock); c = cget (&tp->t_outq); SPINFREE (&tp->t_outq.c_lock);

			if (c < 0)
				break;

			if (pubyte (iop->io_area, tp->t_cvtb[c]) < 0)
				{ u.u_error = EFAULT; break; }

			count++;

			iop->io_area++;
			iop->io_count--;
		   /*** iop->io_offset_low++; ***/

			if (c == '\n')
			{
				tp->t_lno++;

				if (tp->t_page != 0 && tp->t_lno >= tp->t_page)
					tp->t_state |= TTYSTOP;
			}

			if (tp->t_state & TTYSTOP)
				break;

			if (iop->io_count <= 0)
				break;
		}
	}

	/*
	 *	Verifica se a fila ficou vazia
	 */
	SPINLOCK (&tp->t_outq.c_lock);

	if (tp->t_outq.c_count <= TTYMINQ)
		EVENTDONE (&tp->t_outqnfull);

	if (tp->t_outq.c_count <= 0)
	{
		EVENTCLEAR (&tp->t_outqnempty);
		EVENTDONE  (&tp->t_outqempty);
	}

	SPINFREE (&tp->t_outq.c_lock);

	/*
	 *	Testa novamente o CSTOP
	 */
	if (tp->t_state & TTYSTOP)
		EVENTCLEAR (&tp->t_outqnempty);

	/*
	 *	Verifica se foi um alarme falso
	 */
	if (count == 0 && u.u_error == 0)
		goto again;

}	/* end ptysread */

/*
 ****************************************************************
 * Copia caracteres da CLIST para o usário, convertendo código	*
 ****************************************************************
 */
int
pty_convert_and_copy (IOREQ *iop, TTY *tp)
{
	int		cnt, count = iop->io_count;
	char		*cp, *ep, *cvtb = tp->t_cvtb;
	char		*area = iop->io_area;
	char		conv_buf[80];

	for (EVER)
	{
		if ((cnt = MIN (sizeof (conv_buf), count)) == 0)
			return (iop->io_count - count);

		if ((cnt = cread (&tp->t_outq, conv_buf, cnt, SS)) < 0)
			return (-1);

		if (cnt == 0)
			return (iop->io_count - count);

		for (cp = conv_buf, ep = cp + cnt; cp < ep; cp++) 
			*cp = cvtb[*cp];

		if (unimove (area, conv_buf, cnt, SU) < 0)
			{ u.u_error = EFAULT; return (-1); }

		area  += cnt;
		count -= cnt;
	}

}	/* end pty_convert_and_copy */

/*
 ****************************************************************
 *	Escrita do servidor					*
 ****************************************************************
 */
void
ptyswrite (IOREQ *iop)
{
	TTY		*tp;
	int		c;

	/*
	 *	Verifica se este pseudo-terminal está aberto
	 */
	tp = &scb.y_pty[MINOR (iop->io_dev)].pty;

	if ((tp->t_state & ISOPEN) == 0)
		{ u.u_error = EBADF; return; }

	/*
	 *	Retira da área do usuário de põe na CLIST
	 */
	while (iop->io_count > 0)
	{
		if ((c = gubyte (iop->io_area)) < 0)
			{ u.u_error = EFAULT; break; }

		ttyin (c, tp);

		iop->io_area++;
		iop->io_count--;
	   /*** iop->io_offset_low++; ***/

	}	/* end while */

}	/* end ptyswrite */

/*
 ****************************************************************
 *	Rotina para iniciar a transmissão			*
 ****************************************************************
 */
void
ptystart (TTY *tp, int flag)
{
	PTY		*pp = (PTY *)tp->t_ptydata;

	if (tp->t_state & TTYSTOP)
		return;

	if (tp->t_outq.c_count <= 0)
		return;

	EVENTDONE (&tp->t_outqnempty);

	if (pp->attention_set)
	{
		pp->attention_set = 0;

		if (((PTY *)tp->t_ptydata)->uproc->u_attention_index < 0)
		{
			pp->uproc->u_attention_index = pp->index; 
			EVENTDONE (&pp->uproc->u_attention_event);
		}
	}

}	/* end ptystart */
