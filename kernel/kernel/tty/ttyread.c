/*
 ****************************************************************
 *								*
 *			ttyread.c				*
 *								*
 *	Interface geral para terminais (parte I)		*
 *								*
 *	Versão	3.0.0, de 15.12.94				*
 *		3.1.1, de 22.07.98				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 1999 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include "../h/common.h"
#include "../h/sync.h"
#include "../h/scb.h"
#include "../h/region.h"

#include "../h/tty.h"
#include "../h/video.h"
#include "../h/kfile.h"
#include "../h/inode.h"
#include "../h/uerror.h"
#include "../h/signal.h"
#include "../h/uproc.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	"Open" de um terminal					*
 ****************************************************************
 */
void
ttyopen (dev_t dev, TTY *tp, int oflag)
{
	/*
	 *	Chamado a cada OPEN do INODE
	 */
	if ((tp->t_state & ISOPEN) == 0)
	{
		EVENTCLEAR (&tp->t_inqnempty);
		EVENTSET   (&tp->t_outqnfull);
		EVENTSET   (&tp->t_outqempty);
		CLEAR	   (&tp->t_obusy);	/* Ainda em fase de teste */

		tp->t_dev  = dev;
		tp->t_col  = 0;
		tp->t_wcol = 0;
		tp->t_lno  = 0;

		if ((tp->t_state & ONEINIT) == 0)
		{
			ttychars (tp);
			tp->t_lflag |= ISO;
			tp->t_cvtb   = isotoisotb;
		}

		strcpy (tp->t_termio.c_name, u.u_name);

		tp->t_state |= ISOPEN;
	}
#ifdef	ONEREADER
	else
	{
		/*
		 *	Terminais não podem ter mais de 1 Leitor
		 */
		if (oflag & O_READ)
			{ u.u_error = EBUSY; return; }
	}
#endif	ONEREADER

	/*
	 *	Processa o "tty Group"
	 */
	if (u.u_tgrp == 0)
	{
		u.u_tty = tp;
		u.u_ttydev = dev;

		if (tp->t_tgrp == 0)
			tp->t_tgrp = u.u_pid;

		u.u_tgrp = tp->t_tgrp;
	}

}	/* end ttyopen */

/*
 ****************************************************************
 *	"Close" de um terminal					*
 ****************************************************************
 */
void
ttyclose (TTY *tp)
{
	/*
	 *	Chamado apenas no ultimo CLOSE do INODE
	 */
   /***	SLEEPLOCK (&tp->t_olock, PTTYOUT); ***/

	EVENTWAIT (&tp->t_outqempty, PTTYOUT);

	ttyiflush (tp);
	ttyoflush (tp);

	tp->t_tgrp = 0;
	tp->t_state &= ONEINIT;

   /***	SLEEPFREE (&tp->t_olock); ***/

}	/* end ttyclose */

/*
 ****************************************************************
 *	Leitura de um TTY					*
 ****************************************************************
 */
void
ttyread (IOREQ *iop, TTY *tp)
{
	int		c = 0, count;

	/*
	 *	Em modo COOKED permite ler apenas para a área do USUARIO!
	 */

	/*
	 *	Verifica se o TTY está aberto
	 */
	if ((tp->t_state & ISOPEN) == 0)
		{ u.u_error = EBADF; return; }

	/*
	 *	Analisa a opção NO-DELAY	
	 */
    again:
	if   ((u.u_oflag & O_NDELAY) == 0)
		EVENTWAIT (&tp->t_inqnempty, PTTYIN);
	elif (EVENTTEST (&tp->t_inqnempty) < 0)
		return;

	/*
	 *	Verifica o tipo da entrada
	 */
	if ((tp->t_lflag & (ICANON|ICOMM)) != ICANON)
	{
		/*
		 *	Entrada COMM ou RAW
		 */
		if ((count = cread (&tp->t_inq, iop->io_area, iop->io_count, iop->io_cpflag)) < 0)
			return;

	   /*** iop->io_area	    += count; ***/
		iop->io_count	    -= count;
	   /*** iop->io_offset_low  += count; ***/

		/*
		 *	Verifica se a fila ficou vazia
		 */
	   	SPINLOCK (&tp->t_inq.c_lock);

		if (tp->t_inq.c_count < tp->t_min)
			EVENTCLEAR (&tp->t_inqnempty);

	   	SPINFREE (&tp->t_inq.c_lock);

		/*
		 *	Verifica se foi alarme falso
		 */
		if (count == 0)
			goto again;
	}
	else
	{
		/*
		 *	Entrada COOKED
		 */
		count = iop->io_count;

		while (iop->io_count > 0)
		{
			SPINLOCK (&tp->t_inq.c_lock);

			if ((c = cget (&tp->t_inq)) == DELIM)
				{ SPINFREE (&tp->t_inq.c_lock); break; }

			/* Verifica se foi um alarme falso ou lista vazia */

			if (c < 0)
			{
				if (count == iop->io_count)
				{ 
					if (tp->t_inq.c_count == 0)
						EVENTCLEAR (&tp->t_inqnempty);

					SPINFREE (&tp->t_inq.c_lock);
					goto again;
				}

				SPINFREE (&tp->t_inq.c_lock); return;
			}

			SPINFREE (&tp->t_inq.c_lock);

			if (pubyte (iop->io_area, c) < 0)
				{ u.u_error = EFAULT; break; }

			iop->io_area++;
			iop->io_count--;
		   /*** iop->io_offset_low++; ***/

		}	/* while caracteres disponíveis */

		/*
		 *	Processa o Delimitador
		 */
		SPINLOCK (&tp->t_inq.c_lock);

		if (c != DELIM)
		{
			if (tp->t_inq.c_count > 0)
				c = tp->t_inq.c_cf[0];

			if (c == DELIM)
				c = cget (&tp->t_inq);
		}

		if (c == DELIM)
		{
			if (--tp->t_delimcnt <= 0)
				EVENTCLEAR (&tp->t_inqnempty);
		}

		SPINFREE (&tp->t_inq.c_lock);

	}	/* end cooked */

}	/* end ttyread */

/*
 ****************************************************************
 *	Escrita em um TTY					*
 ****************************************************************
 */
void
ttywrite (IOREQ *iop, TTY *tp)
{
	int		c, count;

	/*
	 *	Em modo COOKED permite apenas escrever da area do USUARIO!
	 */
   /***	SLEEPLOCK (&tp->t_olock, PTTYOUT); ***/

	/*
	 *	Verifica se o TTY está aberto
	 */
	if ((tp->t_state & ISOPEN) == 0)
		{ u.u_error = EBADF; /*** SLEEPFREE (&tp->t_olock); ***/ return; }

	/*
	 *	Verifica o tipo da Saída
	 */
	if ((tp->t_oflag & OPOST) == 0)
	{
		/*
		 *	Saída RAW
		 */
		while (iop->io_count > 0)
		{
			if (EVENTTEST (&tp->t_outqnfull) < 0)
			{
				(*tp->t_oproc) (tp, 0);
				EVENTWAIT (&tp->t_outqnfull, PTTYOUT);
			}

			/*
			 *	A CLIST tem no maximo TTYMAXQ caracteres,
			 *	logo ainda cabem mais TTYMAXQ, pois
			 *	TTYMAXQ + TTYMAXQ < TTYPANIC
			 */
			count = MIN (iop->io_count, TTYMAXQ);

			if (cwrite (&tp->t_outq, iop->io_area, count, iop->io_cpflag) < 0)
				break;

			EVENTCLEAR (&tp->t_outqempty);

			iop->io_area	    += count;
			iop->io_count	    -= count;
		   /*** iop->io_offset_low  += count; ***/

			/*
			 *	Verifica se a fila está cheia
			 */
			SPINLOCK (&tp->t_outq.c_lock);

			if (tp->t_outq.c_count >= TTYMAXQ)
				EVENTCLEAR (&tp->t_outqnfull);

			SPINFREE (&tp->t_outq.c_lock);

		}	/* end while */
	}
	else
	{
		/*
		 *	Saída COOKED
		 */
		while (iop->io_count > 0)
		{
			if (EVENTTEST (&tp->t_outqnfull) < 0)
			{
				(*tp->t_oproc) (tp, 0);
				EVENTWAIT (&tp->t_outqnfull, PTTYOUT);
			}

			if ((c = gubyte (iop->io_area)) < 0)
				{ u.u_error = EFAULT; break; }

			iop->io_area++;
			iop->io_count--;
		   /*** iop->io_offset_low++; ***/

			ttyout (c, tp);

			/*
			 *	Verifica se a fila ficou cheia
			 */
			SPINLOCK (&tp->t_outq.c_lock);

			if (tp->t_outq.c_count >= TTYMAXQ)
				EVENTCLEAR (&tp->t_outqnfull);

			SPINFREE (&tp->t_outq.c_lock);

		}	/* end while */

		tp->t_wcol = tp->t_col;

	}	/* end if (saída RAW) */

	(*tp->t_oproc) (tp, 0);

   /***	SLEEPFREE (&tp->t_olock); ***/

}	/* end ttywrite */

/*
 ****************************************************************
 *	Define os caracteres de controle default		*
 ****************************************************************
 */
void
ttychars (TTY *tp)
{
	tp->t_intr   = CINTR;
	tp->t_quit   = CQUIT;
	tp->t_erase  = CERASE;
	tp->t_kill   = CKILL;
	tp->t_eof    = CEOT;
	tp->t_min    = 1;
	tp->t_eol    = CESC;
	tp->t_time   = 0;
	tp->t_res    = 0;
	tp->t_swtch  = 0x01;		/* <^A> */
   /***	tp->t_page   = ...; ***/	/* Abaixo */
	tp->t_aerase = CAERASE;
	tp->t_retype = CRTYPE;
	tp->t_word   = CWORD;
   /***	tp->t_color  = ...; ***/

	tp->t_nline  = video_data->video_LINE;
	tp->t_page   = tp->t_nline - 1;
	tp->t_ncol   = video_data->video_COL;

}	/* end ttychars */

/*
 ****************************************************************
 *	Esvazia a fila de entrada				*
 ****************************************************************
 */
void
ttyiflush (TTY *tp)
{
	EVENTCLEAR (&tp->t_inqnempty);
	tp->t_delimcnt = 0;

	SPINLOCK (&tp->t_inq.c_lock);

	while  (cget (&tp->t_inq) >= 0)
		/* vazio */;

	SPINFREE (&tp->t_inq.c_lock);

}	/* end ttyiflush */

/*
 ****************************************************************
 *	Esvazia a Fila de Saída					*
 ****************************************************************
 */
void
ttyoflush (TTY *tp)
{
	tp->t_state &= ~TTYSTOP;
	tp->t_lno = 0;

#ifdef	T_STOP
	(*ciotab[MAJOR(tp->t_dev)].w_stop) (tp);
#endif	T_STOP

	SPINLOCK (&tp->t_outq.c_lock);

	while  (cget (&tp->t_outq) >= 0)
		/* vazio */;

	SPINFREE (&tp->t_outq.c_lock);

	EVENTDONE  (&tp->t_outqnfull);
	EVENTDONE  (&tp->t_outqempty);

	CLEAR (&tp->t_obusy);	/* Ainda em fase de teste */

}	/* end ttyoflush */
