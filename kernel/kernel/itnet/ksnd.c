/*
 ****************************************************************
 *								*
 *			ksnd.c					*
 *								*
 *	Funções de transferência do INTERNET			*
 *								*
 *	Versão	3.0.0, de 19.08.91				*
 *		4.7.0, de 02.12.04				*
 *								*
 *	Funções:						*
 *		k_sndraw,	k_rcvraw,			*
 *		k_sndudata,	k_rcvudata,			*
 *		k_snd,		k_rcv,				*
 *		k_sndrel,	k_rcvrel,			*
 *		k_snddis,	k_rcvdis,			*
 *		wait_tcp_segment				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 * 		Copyright © 2004 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include "../h/common.h"
#include "../h/scb.h"
#include "../h/sync.h"
#include "../h/region.h"

#include "../h/kfile.h"
#include "../h/inode.h"
#include "../h/uerror.h"
#include "../h/signal.h"
#include "../h/uproc.h"
#include "../h/cpu.h"
#include "../h/xti.h"
#include "../h/itnet.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ******	Protótipos de funções ***********************************
 */
void		tcp_fin_wait_2_time_out (TCP_EP *);

/*
 ****************************************************************
 *	Escreve um datagrama RAW				*
 ****************************************************************
 */
void
k_sndraw (IOREQ *iop, INADDR *addr_ptr, int addr_size)
{
	KFILE		*fp = u.u_fileptr;
	RAW_EP		*rp;
	ITBLOCK		*bp;
	INADDR		addr[2];
	void		*area;

	if (fp->f_union != KF_ITNET)
		{ u.u_error = EBADF; return; }

	rp = fp->f_endpoint;

	if (rp->rp_state != S_BOUND)
		{ u.u_error = TOUTSTATE; return; }

	if ((unsigned)iop->io_count > DGSZ - MIN_IP_H_SZ)
		{ u.u_error = TBADDATA; return; }

	/*
	 *	Analisa o endereço
	 */
	switch (addr_size)
	{
	    case 0:
		rp->rp_snd_addr  = rp->rp_bind_addr;
		rp->rp_snd_proto = rp->rp_bind_proto;

		rp->rp_my_addr  = 0;			/* Calcula o endereço local */
		break;

	    default:
		if (unimove (addr, addr_ptr, addr_size, US) < 0)
			{ u.u_error = EFAULT; return; }

		rp->rp_snd_addr  = addr[0].a_addr;
		rp->rp_snd_proto = addr[0].a_proto;

		if (addr_size == 2 * sizeof (INADDR))
			rp->rp_my_addr = addr[1].a_addr; /* Endereço local dado */
		else
			rp->rp_my_addr = 0;		/* Calcula o endereço local */
		break;

	}	/* end switch */

	/*
	 *	Obtém um ITBLOCK e copia os dados
	 */
	bp = get_it_block (IT_OUT_DATA);

	area = bp->it_frame + RESSZ;

	if (unimove (area, iop->io_area, iop->io_count, US) < 0)
		{ u.u_error = EFAULT; put_it_block (bp); return; }

	/*
	 *	Envia a mensagem através do protocolo RAW
	 */
	bp->it_u_area  = area;
	bp->it_u_count = iop->io_count;

	send_raw_datagram (rp, bp);

	iop->io_count = 0;

}	/* end k_sndraw */

/*
 ****************************************************************
 *	Le um datagrama RAW					*
 ****************************************************************
 */
void
k_rcvraw (IOREQ *iop, INADDR *addr_ptr, int addr_size, int *flag_ptr)
{
	KFILE		*fp = u.u_fileptr;
	RAW_EP		*rp;
	ITBLOCK		*bp;
	INADDR		addr[2];

	if (fp->f_union != KF_ITNET)
		{ u.u_error = EBADF; return; }

	rp = fp->f_endpoint;

	if (rp->rp_state != S_BOUND)
		{ u.u_error = TOUTSTATE; return; }

	if (iop->io_count < 0)
		{ u.u_error = TBADDATA; return; }

	/*
	 *	Espera ter dados disponíveis
	 */
	for (EVER)
	{
		SPINLOCK (&rp->rp_inq_lock);

		if ((bp = rp->rp_inq_first) != NOITBLOCK)
			break;

		EVENTCLEAR (&rp->rp_inq_nempty);

		SPINFREE (&rp->rp_inq_lock);

		if (u.u_oflag & O_NDELAY)
			{ u.u_error = TNODATA; return; }

		EVENTWAIT (&rp->rp_inq_nempty, PITNETIN);
	}

	SPINFREE (&rp->rp_inq_lock);

	rp->rp_rcv_addr	 = bp->it_src_addr;
	rp->rp_rcv_proto = bp->it_proto;

	rp->rp_my_addr	 = bp->it_dst_addr;

	/*
	 *	Copia o endereço para o usuário
	 */
	switch (addr_size)
	{
	    case 2 * sizeof (INADDR):
		addr[1].a_addr	 = rp->rp_my_addr;
	   /***	addr[1].a_proto	 = rp->rp_my_proto; ***/
		addr[1].a_family = 0;		/* PROVISÓRIO */

		/* continua */

	    case sizeof (INADDR):

		addr[0].a_addr	 = rp->rp_rcv_addr;
		addr[0].a_proto	 = rp->rp_rcv_proto;
		addr[0].a_family = 0;		/* PROVISÓRIO */

		if (unimove (addr_ptr, addr, addr_size, SU) < 0)
			{ u.u_error = EFAULT; return; }

		break;

	}	/* end switch */

	/*
	 *	Copia os dados
	 */
	if (iop->io_count < bp->it_count)
		{ u.u_error = TBUFOVFLW; return; }

	if (unimove (iop->io_area, bp->it_area, bp->it_count, SU) < 0)
		{ u.u_error = EFAULT; return; }

   /***	iop->io_area  += bp->it_count; ***/
	iop->io_count -= bp->it_count;

	/*
	 *	Remove o bloco da fila
	 */
	SPINLOCK (&rp->rp_inq_lock);

	if ((rp->rp_inq_first = bp->it_inq_forw.inq_ptr) == NOITBLOCK)
	{
		rp->rp_inq_last = NOITBLOCK;
		EVENTCLEAR (&rp->rp_inq_nempty);
	}

	SPINFREE (&rp->rp_inq_lock);

	put_it_block (bp);

}	/* end k_rcvraw */

/*
 ****************************************************************
 *	Escreve um datagrama UDP				*
 ****************************************************************
 */
void
k_sndudata (IOREQ *iop, INADDR *addr_ptr, int addr_size)
{
	KFILE		*fp = u.u_fileptr;
	UDP_EP		*up;
	ITBLOCK		*bp;
	INADDR		addr[2];
	void		*area;

	if (fp->f_union != KF_ITNET)
		{ u.u_error = EBADF; return; }

	up = fp->f_endpoint;

	if (up->up_state != S_BOUND)
		{ u.u_error = TOUTSTATE; return; }

	if ((unsigned)iop->io_count > DGSZ - MIN_IP_H_SZ - sizeof (UDP_H))
		{ u.u_error = TBADDATA; return; }

	/*
	 *	Analisa o endereço
	 */
	switch (addr_size)
	{
	    case 0:
		up->up_snd_addr = up->up_bind_addr;
		up->up_snd_port = up->up_bind_port;

		up->up_my_addr  = 0;			/* Calcula o endereço local */
		break;

	    default:
		if (unimove (addr, addr_ptr, addr_size, US) < 0)
			{ u.u_error = EFAULT; return; }

		up->up_snd_addr = addr[0].a_addr;
		up->up_snd_port = addr[0].a_port;

		if (addr_size == 2 * sizeof (INADDR))
			up->up_my_addr = addr[1].a_addr; /* Endereço local dado */
		else
			up->up_my_addr = 0;		/* Calcula o endereço local */
		break;

	}	/* end switch */

	/*
	 *	Obtém um ITBLOCK e copia os dados
	 */
	bp = get_it_block (IT_OUT_DATA);

	area = bp->it_frame + RESSZ;	/* >= ETH_H + IP_H + UDP_H */

	if (unimove (area, iop->io_area, iop->io_count, US) < 0)
		{ u.u_error = EFAULT; put_it_block (bp); return; }

	/*
	 *	Envia a mensagem através do protocolo UDP
	 */
	bp->it_u_area  = area;
	bp->it_u_count = iop->io_count;

	send_udp_datagram (up, bp);

	iop->io_count = 0;

}	/* end k_sndudata */

/*
 ****************************************************************
 *	Le um datagrama UDP					*
 ****************************************************************
 */
void
k_rcvudata (IOREQ *iop, INADDR *addr_ptr, int addr_size, int *flag_ptr)
{
	KFILE		*fp = u.u_fileptr;
	UDP_EP		*up;
	UT_H		*uhp;
	ITBLOCK		*bp;
	int		count;
	INADDR		addr[2];
	int		flags = 0;

	if (fp->f_union != KF_ITNET)
		{ u.u_error = EBADF; return; }

	up = fp->f_endpoint;

	if (up->up_state != S_BOUND)
		{ u.u_error = TOUTSTATE; return; }

	if (iop->io_count < 0)
		{ u.u_error = TBADDATA; return; }

	/*
	 *	Espera ter dados disponíveis
	 */
	for (EVER)
	{
		SPINLOCK (&up->up_inq_lock);

		if ((bp = up->up_inq_first) != NOITBLOCK)
			break;

		EVENTCLEAR (&up->up_inq_nempty);

		SPINFREE (&up->up_inq_lock);

		if (u.u_oflag & O_NDELAY)
			{ u.u_error = TNODATA; return; }

		EVENTWAIT (&up->up_inq_nempty, PITNETIN);
	}

	SPINFREE (&up->up_inq_lock);

	/*
	 *	Copia o endereço para o usuário
	 */
	uhp = bp->it_u_area;

	up->up_rcv_addr = ENDIAN_LONG (uhp->ph.ph_src_addr);
	up->up_rcv_port = ENDIAN_SHORT (uhp->uh.uh_src_port);

	up->up_my_addr = ENDIAN_LONG (uhp->ph.ph_dst_addr);
	up->up_my_port = ENDIAN_SHORT (uhp->uh.uh_dst_port);

	switch (addr_size)
	{
	    case 2 * sizeof (INADDR):
		addr[1].a_addr	 = up->up_my_addr;
		addr[1].a_port	 = up->up_my_port;
		addr[1].a_family = 0;		/* PROVISÓRIO */

		/* continua */

	    case sizeof (INADDR):

		addr[0].a_addr	 = up->up_rcv_addr;
		addr[0].a_port	 = up->up_rcv_port;
		addr[0].a_family = 0;		/* PROVISÓRIO */

		if (unimove (addr_ptr, addr, addr_size, SU) < 0)
			{ u.u_error = EFAULT; return; }

		break;

	}	/* end switch */

	/*
	 *	Copia os dados
	 */
	count = MIN (iop->io_count, bp->it_count);

	if (unimove (iop->io_area, bp->it_area, count, SU) < 0)
		{ u.u_error = EFAULT; return; }

	bp->it_area  += count;
	bp->it_count -= count;

   /***	iop->io_area  += count; ***/
	iop->io_count -= count;

	/*
	 *	Se os dados do bloco acabaram, remove-o da fila
	 */
	if (bp->it_count <= 0)
	{
		SPINLOCK (&up->up_inq_lock);

		if ((up->up_inq_first = bp->it_inq_forw.inq_ptr) == NOITBLOCK)
		{
			up->up_inq_last = NOITBLOCK;
			EVENTCLEAR (&up->up_inq_nempty);
		}

		SPINFREE (&up->up_inq_lock);

		put_it_block (bp);
	}
	else
	{
		flags |= T_MORE;
	}

	/*
	 *	Atualiza "flags"
	 */
	if (uhp->uh.uh_checksum == 0)
		flags |= T_NO_CHECKSUM;

	if (flag_ptr != (int *)NULL)
	{
		if (pulong (flag_ptr, flags) < 0)
			{ u.u_error = EFAULT; return; }
	}

}	/* end k_rcvudata */

/*
 ****************************************************************
 *	Envia um segmento TCP					*
 ****************************************************************
 */
void
k_snd (IOREQ *iop, int flags)
{
	KFILE		*fp = iop->io_fp;
	TCP_EP		*tp;
	int		type = C_DATA;

	if (fp->f_union != KF_ITNET)
		{ u.u_error = EBADF; return; }

	tp = fp->f_endpoint;

	if (tp->tp_rst)
		{ u.u_error = TLOOK; return; }

	if (tp->tp_state != S_ESTABLISHED && tp->tp_state != S_CLOSE_WAIT)
		{ u.u_error = TOUTSTATE; return; }

	if (iop->io_count <= 0)
		{ u.u_error = TBADDATA; return; }

	if (flags & T_PUSH)
		type |= C_PSH;
	if (flags & T_URGENT)
		type |= C_URG;

	SLEEPLOCK (&tp->tp_lock, PITNETOUT);

	if (tp->tp_pipe_mode)
		pipe_tcp_data_packet (iop, type, tp);
	else
		queue_tcp_data_packet (iop, type, tp);

	SLEEPFREE (&tp->tp_lock);

}	/* end k_snd */

/*
 ****************************************************************
 *	Recebe um segmento TCP					*
 ****************************************************************
 */
void
k_rcv (IOREQ *iop, int *flag_ptr)
{
	KFILE		*fp = iop->io_fp;
	TCP_EP		*tp;
	int		n, count, total = 0, flags = 0;
	char		enough = 0;

	if (fp->f_union != KF_ITNET)
		{ u.u_error = EBADF; return; }

	tp = fp->f_endpoint;

	if (tp->tp_rst)
		{ u.u_error = TLOOK; return; }

	if (tp->tp_state != S_ESTABLISHED && tp->tp_state != S_FIN_WAIT_1 && tp->tp_state != S_FIN_WAIT_2)
		{ u.u_error = TOUTSTATE; return; }

	if (iop->io_count <= 0)
		{ u.u_error = TBADDATA; return; }

	/*
	 *	Espera chegarem dados
	 */
	SPINLOCK (&tp->tp_inq_lock);

    again:
	for (EVER)
	{
		if (tp->tp_rnd_in.tp_rnd_count > 0 || tp->tp_rnd_in.tp_rnd_fin)
			break;

		EVENTCLEAR (&tp->tp_inq_nempty);

		SPINFREE (&tp->tp_inq_lock);

		if (fp->f_flags & O_NDELAY)
			{ u.u_error = TNODATA; return; }

		EVENTWAIT (&tp->tp_inq_nempty, PITNETIN);

		if (tp->tp_rst)
			{ u.u_error = TLOOK; return; }

		SPINLOCK (&tp->tp_inq_lock);

	}	/* end for (EVER) */

	/*
	 *	Verifica se o bloco tem dados ou apenas o FIN
	 */
	if (tp->tp_rnd_in.tp_rnd_count > 0)
	{
		count = iop->io_count;

		if (count > tp->tp_rnd_in.tp_rnd_count)
			count = tp->tp_rnd_in.tp_rnd_count;

		if ((n = tp->tp_rcv_up - tp->tp_rcv_usr) > 0 && count >= n)
			{ count = n; enough++; }

		if ((n = tp->tp_rcv_psh - tp->tp_rcv_usr) > 0 && count >= n)
			{ count = n; enough++; }
#ifdef	MSG
		if (itscb.it_list_input)
		{
			char	area[9];

			n = circular_area_read (&tp->tp_rnd_in, area, 8, 0);
			area[8] = '\0';

			if (n > 0)
			{
				printf ("%g: count = %d, enough = %d, \"%s\"\n", count, enough, area);
				printf ("psh = %P, usr = %P\n", tp->tp_rcv_psh, tp->tp_rcv_usr);
			}
		}
#endif	MSG
		if (circular_area_get (&tp->tp_rnd_in, iop->io_area, count) != count)
			{ SPINFREE (&tp->tp_inq_lock); return; }

		iop->io_area  += count;
		iop->io_count -= count;

		tp->tp_rcv_usr += count;
		tp->tp_rcv_wnd += count;

		total += count;

		/* A Janela de recepção abriu */

		if (tp->tp_rcv_wnd - tp->tp_rcv_nxt >= tp->tp_good_wnd)
		{
			if (tp->tp_pipe_mode)
				EVENTDONE (&tp->tp_rnd_in.tp_rnd_nfull);
			else
				wake_up_in_daemon (IN_EP, tp);
#ifdef	MSG
			if (itscb.it_list_window)
			{
				printf
				(	"%g: (porta %d): Mandei ACK (%d)\n",
					tp->tp_my_port, tp->tp_rcv_wnd - tp->tp_rcv_nxt
				);
			}
#endif	MSG
		}

		if (iop->io_count > 0  &&  !enough)
			goto again;
	}

	SPINFREE (&tp->tp_inq_lock);

	/*
	 *	Veio o "FIN" ou o bloco ainda tem mais dados:
	 *
	 *	Atualiza "flags"
	 */
	if (TCP_GE (tp->tp_rcv_up, tp->tp_rcv_usr))
		flags |= T_URGENT;

	if (TCP_GE (tp->tp_rcv_psh, tp->tp_rcv_usr))
		flags |= T_PUSH;

	if (flag_ptr != (int *)NULL)
	{
		if (pulong (flag_ptr, flags) < 0)
			{ u.u_error = EFAULT; return; }
	}

}	/* end k_rcv */

/*
 ****************************************************************
 *	Inicia o término da conexão TCP				*
 ****************************************************************
 */
void
k_sndrel (IOREQ *iop)
{
	KFILE		*fp = iop->io_fp;
	TCP_EP		*tp, *dst_tp;
	int		n, count;

	if (fp->f_union != KF_ITNET)
		{ u.u_error = EBADF; return; }

	tp = fp->f_endpoint;

	if (tp->tp_rst)
		{ u.u_error = TLOOK; return; }

	switch (tp->tp_state)
	{
	    case S_ESTABLISHED:
	    case S_CLOSE_WAIT:
		break;

	    default:
		{ u.u_error = TOUTSTATE; return; }

	}	/* end switch */

	/*
	 *	Dá o FLUSH dos dados de entrada
	 */
#define	IN_FLUSH
#ifdef	IN_FLUSH
	SPINLOCK (&tp->tp_inq_lock);

	if ((count = tp->tp_rnd_in.tp_rnd_count) > 0)
	{
		circular_area_del (&tp->tp_rnd_in, count);

	   /***	iop->io_area  += count; ***/
	   /***	iop->io_count -= count; ***/

	   /***	tp->tp_rcv_usr += count; ***/
		tp->tp_rcv_wnd += count;

		if (tp->tp_pipe_mode)
			EVENTDONE (&tp->tp_rnd_in.tp_rnd_nfull);
		else
			wake_up_in_daemon (IN_EP, tp);
#ifdef	MSG
		if (itscb.it_list_window)
		{
			printf
			(	"%g: (porta %d): Mandei ACK (%d)\n",
				tp->tp_my_port, tp->tp_rcv_wnd - tp->tp_rcv_nxt
			);
		}
#endif	MSG
	}

	SPINFREE (&tp->tp_inq_lock);
#endif	IN_FLUSH

	/*
	 *	Dá o "flush" dos dados de saída
	 */
	SLEEPLOCK (&tp->tp_lock, PITNETOUT);

	iop->io_count = 0;

	if (tp->tp_pipe_mode)
		pipe_tcp_data_packet  (iop, C_PSH, tp);
	else
		queue_tcp_data_packet (iop, C_PSH, tp);

	SLEEPFREE (&tp->tp_lock);

	/*
	 *	Espera a janela de transmissão ter espaço para o FIN
	 */
	dst_tp = tp->tp_pipe_tcp_ep;

	if (!tp->tp_pipe_mode) for (EVER)
	{
		if (tp->tp_rst)
			{ u.u_error = TLOOK; return; }

		SPINLOCK (&tp->tp_rnd_out.tp_rnd_lock);

		if (TCP_LE (tp->tp_snd_nxt + 1, tp->tp_snd_wnd))
			{ SPINFREE (&tp->tp_rnd_out.tp_rnd_lock); break; }

		EVENTCLEAR (&tp->tp_good_snd_wnd);

		SPINFREE (&tp->tp_rnd_out.tp_rnd_lock);

		EVENTWAIT (&tp->tp_good_snd_wnd, PITNETOUT);
	}
	else for (EVER)
	{
		if (tp->tp_rst)
			{ u.u_error = TLOOK; return; }

		SPINLOCK (&dst_tp->tp_inq_lock);

		if (dst_tp->tp_rnd_in.tp_rnd_count + 1 <= dst_tp->tp_rnd_in.tp_rnd_sz)
			{ SPINFREE (&dst_tp->tp_inq_lock); break; }

		EVENTCLEAR (&dst_tp->tp_rnd_in.tp_rnd_nfull);

		SPINFREE (&dst_tp->tp_inq_lock);

		EVENTWAIT (&dst_tp->tp_rnd_in.tp_rnd_nfull, PITNETOUT);
	}

	/*
	 *	Envia o "FIN"
	 */
	switch (tp->tp_state)
	{
	    case S_ESTABLISHED:
		{ tp->tp_state = S_FIN_WAIT_1; break; }

	    case S_CLOSE_WAIT:
		{ tp->tp_state = S_LAST_ACK; break; }

	    default:
		{ u.u_error = TOUTSTATE; return; }

	}	/* end switch */

	SLEEPLOCK (&tp->tp_lock, PITNETOUT);

	tp->tp_rnd_out.tp_rnd_fin = 1;
	tp->tp_snd_fin = tp->tp_snd_nxt + 1;

	send_tcp_ctl_packet (C_FIN|C_ACK, tp);

	SLEEPFREE (&tp->tp_lock);

	tp->tp_last_snd_time = time;

	/*
	 *	Agora, espera pelo ACK do FIN
	 */
	for (EVER)
	{
		if (tp->tp_rst)
			{ u.u_error = TLOOK; return; }

		SPINLOCK (&tp->tp_inq_lock);

		if (tp->tp_snd_una == tp->tp_snd_fin)
			{ SPINFREE (&tp->tp_inq_lock); break; }

		EVENTCLEAR (&tp->tp_inq_nempty);

		SPINFREE (&tp->tp_inq_lock);

		EVENTWAIT (&tp->tp_inq_nempty, PITNETIN);

	}	/* end for (EVER) */

	/*
	 *	A mudança de estado é realizada por "rcv"
	 */
   /***	tp->tp_state = ...; ***/

}	/* end k_sndrel */

/*
 ****************************************************************
 *	Espera o sinal de término da conexão TCP		*
 ****************************************************************
 */
void
k_rcvrel (void)
{
	KFILE		*fp = u.u_fileptr;
	TCP_EP		*tp;
	int		count;

	if (fp->f_union != KF_ITNET)
		{ u.u_error = EBADF; return; }

	tp = fp->f_endpoint;

	if (tp->tp_rst)
		{ u.u_error = TLOOK; return; }

	switch (tp->tp_state)
	{
	    case S_ESTABLISHED:
	    case S_FIN_WAIT_2:
		break;

	    default:
		{ u.u_error = TOUTSTATE; return; }

	}	/* end switch */

	/*
	 *	Espera chegar o "FIN"
	 */
	for (EVER)
	{
		if (tp->tp_rst)
			{ u.u_error = TLOOK; return; }

		SPINLOCK (&tp->tp_inq_lock);

		if ((count = tp->tp_rnd_in.tp_rnd_count) > 0)
		{
			/* Dá o FLUSH dos dados de entrada */

			circular_area_del (&tp->tp_rnd_in, count);

		   /***	iop->io_area  += count; ***/
		   /***	iop->io_count -= count; ***/

		   /***	tp->tp_rcv_usr += count; ***/
			tp->tp_rcv_wnd += count;

			if (tp->tp_pipe_mode)
				EVENTDONE (&tp->tp_rnd_in.tp_rnd_nfull);
			else
				wake_up_in_daemon (IN_EP, tp);
#ifdef	MSG
			if (itscb.it_list_window)
			{
				printf
				(	"%g: (porta %d): Mandei ACK (%d)\n",
					tp->tp_my_port, tp->tp_rcv_wnd - tp->tp_rcv_nxt
				);
			}
#endif	MSG
		}

		if (tp->tp_rnd_in.tp_rnd_fin)
			{ SPINFREE (&tp->tp_inq_lock); break; }

		EVENTCLEAR (&tp->tp_inq_nempty);

		SPINFREE (&tp->tp_inq_lock);

		EVENTWAIT (&tp->tp_inq_nempty, PITNETIN);

	}	/* end for (EVER) */

#ifdef	MSG
	if (itscb.it_list_window)
		printf ("%g: (%d)\n", tp->tp_rcv_wnd - tp->tp_rcv_nxt);
#endif	MSG

	switch (tp->tp_state)
	{
	    case S_ESTABLISHED:
		{ tp->tp_state = S_CLOSE_WAIT; break; }

	    case S_FIN_WAIT_2:
#undef	REAL_TIME_WAIT			/* Lembrar em "itn.c" */
#ifdef	REAL_TIME_WAIT
		toutset
		(	2 * (tp->tp_SRTT >> 10) * scb.y_hz,
			tcp_fin_wait_2_time_out,
			(int)tp
		);
		tp->tp_state = S_TIME_WAIT;
#else
		tp->tp_state = S_BOUND;		/* Deveria esperar 2 MSL */
#endif	REAL_TIME_WAIT
		break;

	    default:
		{ u.u_error = TOUTSTATE; return; }

	}	/* end switch */

}	/* end k_rcvrel */

#ifdef	REAL_TIME_WAIT
/*
 ****************************************************************
 *	Retorna o estado do "endpoint" para BOUND, após 2 MSL	*
 ****************************************************************
 */
void
tcp_fin_wait_2_time_out (TCP_EP *tp)
{
	tp->tp_state = S_BOUND;

}	/* end tcp_fin_wait_2_time_out */
#endif	REAL_TIME_WAIT

/*
 ****************************************************************
 *	Inicia o término abortivo da conexão TCP		*
 ****************************************************************
 */
void
k_snddis (T_CALL *callp)
{
	KFILE		*fp = u.u_fileptr;
	TCP_EP		*tp;
	LISTENQ		*lp;
	const LISTENQ	*end_listen_q;
	T_CALL		call;
	IPADDR		listen_addr;
	int		listen_port;
	seq_t		listen_irs;

	/*
	 *	Obtém a estrutura "call"
	 */
	if (callp != (T_CALL *)NULL)
	{
		if (unimove (&call, callp, sizeof (T_CALL), US) < 0)
			{ u.u_error = EFAULT; return; }
#ifdef	TST_UDATA_OPT
		if (call.opt.len != 0)
			{ u.u_error = TBADOPT; return; }

		if (call.udata.len != 0)
			{ u.u_error = TBADDATA; return; }
#endif	TST_UDATA_OPT
	}

	/*
	 *	Analisa o "endpoint"
	 */
	if (fp->f_union != KF_ITNET)
		{ u.u_error = EBADF; return; }

	tp = fp->f_endpoint;

	/*
	 *	Analisa o estado do "endpoint"
	 */
	switch (tp->tp_state)
	{
	    case S_LISTEN:
		break;

	    case S_SYN_RECEIVED:
	    case S_ESTABLISHED:
	    case S_FIN_WAIT_1:
	    case S_FIN_WAIT_2:
	    case S_CLOSE_WAIT:
		goto abort;

	    default:
		{ u.u_error = TOUTSTATE; return; }

	}	/* end switch */

	/*
	 *	Primeiro caso: Rejeição de um pedido de conexão
	 */
	if (callp == (T_CALL *)NULL)
		{ u.u_error = TBADSEQ; return; }

	SPINLOCK (&tp->tp_inq_lock);

	for (lp = tp->tp_listen_q, end_listen_q = lp + LISTENQ_MAXSZ; /* abaixo */; lp++)
	{
		if (lp >= end_listen_q)
			{ SPINFREE (&tp->tp_inq_lock); u.u_error = TBADSEQ; return; }

		if (lp->tp_listen_seq == call.sequence)
			break;
	}

	listen_addr = lp->tp_listen_addr;
	listen_port = lp->tp_listen_port;
	listen_irs  = lp->tp_listen_irs;

	lp->tp_listen_seq = 0;	/* Remove */

	tp->tp_listen_qlen--;

	SPINFREE (&tp->tp_inq_lock);

	/*
	 *	Envia o segmento RST
	 */
	send_tcp_rst_packet
	(
		C_RST|C_ACK,
		listen_addr,		/* dst_addr */
		listen_port,		/* dst_port */
		tp->tp_my_addr,		/* my_addr */
		tp->tp_my_port,		/* my_port */
		0,			/* seq */
		listen_irs + 1		/* ack */
	);

	return;

	/*
	 *	Demais casos: Aborto de uma conexão existente
	 *
	 *	Remove todos os blocos da fila de saída
	 */
    abort:
	reset_the_connection (tp, 1 /* envia RST */);

}	/* end k_snddis */

/*
 ****************************************************************
 *	Espera o sinal de término abortivo da conexão TCP	*
 ****************************************************************
 */
void
k_rcvdis (T_DISCON *disconp)
{
	KFILE		*fp = u.u_fileptr;
	TCP_EP		*tp;
	T_DISCON	discon;

	if (fp->f_union != KF_ITNET)
		{ u.u_error = EBADF; return; }

	tp = fp->f_endpoint;

	if (!tp->tp_rst)
		{ u.u_error = TNODIS; return; }

	/*
	 *	Analisa a estrutura T_DISCON dada
	 */
	if (disconp != (T_DISCON *)NULL)
	{
		if (unimove (&discon, disconp, sizeof (T_DISCON), US) < 0)
			{ u.u_error = EFAULT; return; }
	}

	/*
	 *	Analisa o estado
	 */
	switch (tp->tp_state)
	{
	    case S_BOUND:
		break;

	    case S_SYN_SENT:
		tp->tp_state = S_BOUND;
		break;

	    case S_ESTABLISHED:
	    case S_FIN_WAIT_2:
#if (0)	/*******************************************************/
		delete_retrans_queue (tp);
#endif	/*******************************************************/

		tp->tp_state = S_BOUND;		/* PROVISÓRIO */
		break;

	    default:
		{ u.u_error = TOUTSTATE; return; }

	}	/* end switch */

	/*
	 *	Apaga o evento
	 */
	tp->tp_rst = 0;

	/*
	 *	Copia as estrutura T_DISCON de volta
	 */
	if (disconp != (T_DISCON *)NULL)
	{
		discon.udata.len = 0;
		discon.reason	 = 0;
		discon.sequence  = 0;

		if (unimove (disconp, &discon, sizeof (T_DISCON), SU) < 0)
			{ u.u_error = EFAULT; return; }
	}

}	/* end k_rcvdis */
