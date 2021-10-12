/*
 ****************************************************************
 *								*
 *			kbind.c					*
 *								*
 *	Funções principais do INTERNET				*
 *								*
 *	Versão	3.0.0, de 05.08.91				*
 *		4.9.0, de 16.08.06				*
 *								*
 *	Funções:						*
 *		k_bind,		k_unbind,	k_connect,	*
 *		k_listen,	k_accept,	k_rcvconnect	*
 *		sort_tcp_ep					*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2006 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include "../h/common.h"
#include "../h/scb.h"
#include "../h/sync.h"
#include "../h/region.h"

#include "../h/kfile.h"
#include "../h/uerror.h"
#include "../h/signal.h"
#include "../h/uproc.h"
#include "../h/cpu.h"
#include "../h/xti.h"
#include "../h/itnet.h"
#include "../h/nfs.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */

/*
 ****** Definição de parâmetros do PORT *************************
 */
#define	MIN_UDP_PORT	4096
#define	MAX_UDP_PORT	65535

#define	MIN_TCP_PORT	4096
#define	MAX_TCP_PORT	65535

/*
 ****** Protótipos de funções ***********************************
 */
void		k_rcvconnect (T_CALL *);
void		sort_tcp_ep (TCP_EP *);
void		check_for_pipe_mode (TCP_EP *);

/*
 ****************************************************************
 *	Associa um "port" a um "endpoint"			*
 ****************************************************************
 */
void
k_bind (int minor, const T_BIND *req, T_BIND *ret)
{
	ITSCB		*ip = &itscb;
	KFILE		*fp = u.u_fileptr;
	T_BIND		bind;
	INADDR		addr[2];	/* Local, remoto */
	int		port_given = 0, negotiate_qlen = 0;
	static long	udp_port_source = MIN_UDP_PORT;
	static long	tcp_port_source = MIN_TCP_PORT;

	/*
	 *	Analisa o endereço dado
	 */
	addr[1].a_addr = 0;	addr[1].a_port = 0;	/* Remoto */

	if (req != (T_BIND *)NULL)
	{
		if (unimove (&bind, req, sizeof (T_BIND), US) < 0)
			{ u.u_error = EFAULT; return; }

		if (bind.qlen < 0)
			bind.qlen = 0;
	}
	else
	{
		bind.addr.len = 0;
		bind.qlen = 0;
	}

	if (bind.addr.len != 0)
	{
		if (bind.addr.len != sizeof (INADDR) && bind.addr.len != 2 * sizeof (INADDR))
			{ u.u_error = TBADADDR; return; }

		if (unimove (&addr[0], bind.addr.buf, bind.addr.len, US) < 0)
			{ u.u_error = EFAULT; return; }

		if (addr[0].a_port != 0)
			port_given++;

		if (addr[1].a_addr == 0 && addr[1].a_port != 0)
			{ u.u_error = TBADADDR; return; }
	}

	/*
	 *	Verifica qual o protocolo a utilizar
	 */
	switch (minor)
	{
		/*
		 *	Protocolo RAW
		 */
	    case RAW:
	    {
		RAW_EP		*rp;

		if (fp->f_union != KF_ITNET)
			{ u.u_error = EBADF; return; }

		rp = fp->f_endpoint;

		SLEEPLOCK (&rp->rp_lock, PITNETOUT);

		if (rp->rp_state != S_UNBOUND)
			{ SLEEPFREE (&rp->rp_lock); u.u_error = TOUTSTATE; return; }

		rp->rp_bind_proto = addr[0].a_proto;
		rp->rp_bind_addr  = addr[0].a_addr;

		rp->rp_state	= S_BOUND;

		SLEEPFREE (&rp->rp_lock);

		break;
	    }

		/*
		 *	Protocolo UDP
		 */
	    case UDP:
	    {
		UDP_EP		*up;

		SPINLOCK (&ip->it_uep_lock);

		if (!port_given)
		{
		    udp_again:
			if (udp_port_source > MAX_UDP_PORT)
				udp_port_source = MIN_UDP_PORT;

			addr[0].a_port = udp_port_source++;
		}
		else switch (addr[0].a_port)
		{
		    case PMAP_PORT:
		    case RPC_PORT:
		    case NFS_CLIENT_PORT:
			SPINFREE (&ip->it_uep_lock);
			u.u_error = TADDRBUSY;
			return;
		}

		/*
		 *	Verifica se este "port" já está sendo utilizado
		 */
		for (up = ip->it_uep_busy; up != NO_UDP_EP; up = up->up_next)
		{
			if (up->up_my_port != addr[0].a_port)
				continue;

			if (!port_given)
				goto udp_again;

			SPINFREE (&ip->it_uep_lock);
			u.u_error = TADDRBUSY;
			return;
		}

		SPINFREE (&ip->it_uep_lock);

		/*
		 *	Associa o "port" ao "endpoint"
		 */
		if (fp->f_union != KF_ITNET)
			{ u.u_error = EBADF; return; }

		up = fp->f_endpoint;

		SLEEPLOCK (&up->up_lock, PITNETOUT);

		if (up->up_state != S_UNBOUND)
			{ SLEEPFREE (&up->up_lock); u.u_error = TOUTSTATE; return; }

		up->up_my_port   = addr[0].a_port;

		up->up_bind_addr = addr[1].a_addr;
		up->up_bind_port = addr[1].a_port;

		up->up_state	 = S_BOUND;

		SLEEPFREE (&up->up_lock);

		break;
	    }

		/*
		 *	Protocolo TCP
		 */
	    case TCP:
	    {
		TCP_EP		*tp;

		SPINLOCK (&ip->it_tep_lock);

		if (!port_given)
		{
		    tcp_again:
			if (tcp_port_source > MAX_TCP_PORT)
				tcp_port_source = MIN_TCP_PORT;

			addr[0].a_port = tcp_port_source++;
		}

		/*
		 *	Verifica se este "port" já está sendo utilizado
		 */
		for (tp = ip->it_tep_busy; tp != NO_TCP_EP; tp = tp->tp_next)
		{
			if (tp->tp_my_port != addr[0].a_port)
				continue;

			if (!port_given)
				goto tcp_again;

			if (bind.qlen > 0 && tp->tp_listen_maxqlen > 0)
				goto busy;

			if (tp->tp_dst_addr != addr[1].a_addr)
			{
				if (addr[1].a_addr != 0)
					continue;
			}

			if (tp->tp_dst_port != addr[1].a_port)
			{
				if (addr[1].a_port != 0)
					continue;
			}

		    busy:
			SPINFREE (&ip->it_tep_lock);
			u.u_error = TADDRBUSY;
			return;
		}

		SPINFREE (&ip->it_tep_lock);

		/*
		 *	Associa o "port" ao "endpoint"
		 */
		if (fp->f_union != KF_ITNET)
			{ u.u_error = EBADF; return; }

		tp = fp->f_endpoint;

		SLEEPLOCK (&tp->tp_lock, PITNETOUT);

		if (tp->tp_state != S_UNBOUND)
			{ SLEEPFREE (&tp->tp_lock); u.u_error = TOUTSTATE; return; }

	   /***	SPINLOCK (&tp->tp_inq_lock); ***/

		negotiate_qlen = MIN (bind.qlen, LISTENQ_MAXSZ);

		tp->tp_listen_maxqlen = negotiate_qlen;
		tp->tp_listen_qlen    = 0;
		tp->tp_listen_source  = 0;
		memclr (tp->tp_listen_q, sizeof (tp->tp_listen_q));

	   /***	SPINFREE (&tp->tp_inq_lock); ***/

		tp->tp_my_port	= addr[0].a_port;

		tp->tp_dst_addr	= addr[1].a_addr;
		tp->tp_dst_port	= addr[1].a_port;

		tp->tp_state	= S_BOUND;

		sort_tcp_ep (tp);

		SLEEPFREE (&tp->tp_lock);

		break;
	    }

		/*
		 *	Protocolos INEXISTENTES
		 */
	    default:
		u.u_error = ENXIO;
		return;

	}	/* end switch */

	/*
	 *	Copia as estrutura T_BIND e INADDR de volta
	 */
	if (ret != (T_BIND *)NULL)
	{
		if (unimove (&bind, ret, sizeof (T_BIND), US) < 0)
			{ u.u_error = EFAULT; return; }

		if (bind.addr.maxlen < sizeof (INADDR))
			{ u.u_error = TBUFOVFLW; return; }

		bind.addr.len = bind.addr.maxlen;

		if (bind.addr.len > 2 * sizeof (INADDR))
			bind.addr.len = 2 * sizeof (INADDR);

		addr[0].a_addr = 0;	addr[0].a_family = 0;

		if (unimove (bind.addr.buf, &addr[0], bind.addr.len, SU) < 0)
			{ u.u_error = EFAULT; return; }

		if (minor == TCP)
			bind.qlen = negotiate_qlen;

		if (unimove (ret, &bind, sizeof (T_BIND), SU) < 0)
			{ u.u_error = EFAULT; return; }
	}

}	/* end k_bind */

/*
 ****************************************************************
 *	Desassocia um "port" de um "endpoint"			*
 ****************************************************************
 */
void
k_unbind (int minor)
{
	ITSCB		*ip = &itscb;
	KFILE		*fp = u.u_fileptr;

	/*
	 *	Verifica qual o protocolo a utilizar
	 */
	switch (minor)
	{
		/*
		 *	Protocolo RAW
		 */
	    case RAW:
	    {
		RAW_EP		*rp, *rp_next;
		LOCK		rp_lock;

		if (fp->f_union != KF_ITNET)
			{ u.u_error = EBADF; return; }

		rp = fp->f_endpoint;

		SLEEPLOCK (&rp->rp_lock, PITNETOUT);

		if (rp->rp_state != S_BOUND)
			{ SLEEPFREE (&rp->rp_lock); u.u_error = TOUTSTATE; return; }

		delete_raw_queues (rp);

		SPINLOCK (&ip->it_rep_lock);

		rp_next = rp->rp_next;
		rp_lock = rp->rp_lock;

		memclr (rp, sizeof (RAW_EP));

		rp->rp_next  = rp_next;
		rp->rp_lock  = rp_lock;
		rp->rp_state = S_UNBOUND;

		SPINFREE (&ip->it_rep_lock);

		SLEEPFREE (&rp->rp_lock);

		break;
	    }

		/*
		 *	Protocolo UDP
		 */
	    case UDP:
	    {
		UDP_EP		*up, *up_next;
		LOCK		up_lock;

		if (fp->f_union != KF_ITNET)
			{ u.u_error = EBADF; return; }

		up = fp->f_endpoint;

		SLEEPLOCK (&up->up_lock, PITNETOUT);

		if (up->up_state != S_BOUND)
			{ SLEEPFREE (&up->up_lock); u.u_error = TOUTSTATE; return; }

		delete_udp_queues (up);

		SPINLOCK (&ip->it_uep_lock);

		up_next = up->up_next;
		up_lock = up->up_lock;

		memclr (up, sizeof (UDP_EP));

		up->up_next  = up_next;
		up->up_lock  = up_lock;
		up->up_state = S_UNBOUND;

		SPINFREE (&ip->it_uep_lock);

		SLEEPFREE (&up->up_lock);

		break;
	    }

		/*
		 *	Protocolo TCP
		 */
	    case TCP:
	    {
		TCP_EP		*tp, *tp_next;
		LOCK		tp_lock;

		if (fp->f_union != KF_ITNET)
			{ u.u_error = EBADF; return; }

		tp = fp->f_endpoint;

		SLEEPLOCK (&tp->tp_lock, PITNETOUT);

		if (tp->tp_state != S_BOUND)
			{ SLEEPFREE (&tp->tp_lock); u.u_error = TOUTSTATE; return; }

		circular_area_mrelease (tp);

		SPINLOCK (&ip->it_tep_lock);

		tp_next = tp->tp_next;
		tp_lock = tp->tp_lock;

		memclr (tp, sizeof (TCP_EP));

		tp->tp_next		 = tp_next;
		tp->tp_lock		 = tp_lock;
		tp->tp_state		 = S_UNBOUND;
		tp->tp_SRTT		 = ip->it_SRTT;
		tp->tp_max_seg_sz 	 = ip->it_MAX_SGSZ;
		tp->tp_good_wnd 	 = ip->it_GOOD_WND;
		tp->tp_max_wait		 = ip->it_WAIT;
		tp->tp_max_silence	 = ip->it_SILENCE;
		tp->tp_rnd_in.tp_rnd_sz  = ip->it_WND_SZ;
		tp->tp_rnd_out.tp_rnd_sz = ip->it_WND_SZ;
		tp->tp_last_rcv_time	 = time;

		SPINFREE (&ip->it_tep_lock);

		SLEEPFREE (&tp->tp_lock);

		break;
	    }

		/*
		 *	Protocolos INEXISTENTES
		 */
	    default:
		u.u_error = ENXIO;
		return;

	}	/* end switch */

}	/* end k_unbind */

/*
 ****************************************************************
 *	Envia um pedido de conexão				*
 ****************************************************************
 */
void
k_connect (const T_CALL *sndcall, T_CALL *rcvcall)
{
	TCP_EP		*tp;
	ROUTE		*rp;
	KFILE		*fp = u.u_fileptr;
	T_CALL		call;
	INADDR		addr;
	seq_t		ISS;

	/*
	 *	Analisa o EP
	 */
	if (fp->f_union != KF_ITNET)
		{ u.u_error = EBADF; return; }

	tp = fp->f_endpoint;

	if (tp->tp_state != S_BOUND)
		{ u.u_error = TOUTSTATE; return; }

	tp->tp_listen_maxqlen = 0;

	/*
	 *	Le a estrutura T_CALL do usuário
	 */
	if (unimove (&call, sndcall, sizeof (T_CALL), US) < 0)
		{ u.u_error = EFAULT; return; }

#ifdef	TST_UDATA_OPT
	if (call.opt.len != 0)			/* Não pode ter opções */
		{ u.u_error = TBADOPT; return; }

	if (call.udata.len != 0)		/* Não pode ter dados */
		{ u.u_error = TBADDATA; return; }
#endif	TST_UDATA_OPT

	/*
	 *	Le o endereço do destino (o endereço do servidor)
	 */
	if (call.addr.len != sizeof (INADDR))
		{ u.u_error = TBADADDR; return; }

	if (unimove (&addr, call.addr.buf, sizeof (INADDR), US) < 0)
		{ u.u_error = EFAULT; return; }

	/*
	 *	Obtém o dispositivo destino e endereço fonte
	 */
	if ((rp = get_route_entry (addr.a_addr)) == NOROUTE)
		{ u.u_error = TNOROUTE; return; }

	if (tp->tp_dst_addr != 0  &&  tp->tp_dst_addr != addr.a_addr)
		{ u.u_error = TBADADDR; return; }

	if (tp->tp_dst_port != 0  &&  tp->tp_dst_port != addr.a_port)
		{ u.u_error = TBADADDR; return; }

	SLEEPLOCK (&tp->tp_lock, PITNETOUT);

	tp->tp_dst_addr = addr.a_addr;
	tp->tp_dst_port = addr.a_port;

	tp->tp_my_addr	= rp->r_my_addr;
	tp->tp_route	= rp;

	sort_tcp_ep (tp);

	/*
	 *	Aloca a área circular para a E/S
	 */
	if (circular_area_alloc (tp) < 0)
		return;

	/*
	 *	Inicializa as variáveis do TCP
	 */
	ISS = (time << 7) | hz;		/* Gera ISSs de 0 a 2 ** 32 */
	ISS <<= 8;			/* Com ciclo de +- 12 horas */
	ISS = ISS + ISS + ISS;

	tp->tp_snd_iss = ISS;
	tp->tp_snd_una = ISS;
	tp->tp_snd_nxt = ISS;
	tp->tp_snd_wnd = ISS + 1;	/* Será decidido no ACK_of_SYN */
	tp->tp_snd_up  = ISS;
	tp->tp_snd_psh = ISS;

   /***	tp->tp_rcv_irs = ...; Será decidido na recepção do SYN|ACK ***/
   /***	tp->tp_rcv_usr = ...; ***/
   	tp->tp_rcv_nxt = 0;
	tp->tp_rcv_wnd = tp->tp_rcv_nxt + itscb.it_WND_SZ;
   /***	tp->tp_old_wnd = ...; ***/
   /***	tp->tp_rcv_up  = ...; ***/
   /***	tp->tp_snd_up  = ...; ***/

	/*
	 *	Coloca o SYN na fila circular de saída
	 */
   /***	SPINLOCK (&tp->tp_rnd_out.tp_rnd_lock); ***/

	tp->tp_rnd_out.tp_rnd_syn = C_SYN;

   /***	SPINFREE (&tp->tp_rnd_out.tp_rnd_lock); ***/

	/*
	 *	Envia o pedido de conexão
	 */
	tp->tp_state = S_SYN_SENT;

	send_tcp_ctl_packet (C_SYN, tp);

	SLEEPFREE (&tp->tp_lock);

	tp->tp_last_snd_time = time;
	tp->tp_last_ack_time = time;

	if (u.u_oflag & O_NDELAY)
		{ u.u_error = TNODATA; return; }

	k_rcvconnect (rcvcall);

}	/* end k_connect */

/*
 ****************************************************************
 *	Espera por um pedido de conexão				*
 ****************************************************************
 */
void
k_listen (T_CALL *callp)
{
	KFILE		*fp = u.u_fileptr;
	TCP_EP		*tp;
	LISTENQ		*lp;
	const LISTENQ	*end_listen_q;
	T_CALL		call;
	INADDR		addr;

	/*
	 *	Copia a estrutura "call" do usuário
	 */
	if (fp->f_union != KF_ITNET)
		{ u.u_error = EBADF; return; }

	tp = fp->f_endpoint;

	if (unimove (&call, callp, sizeof (T_CALL), US) < 0)
		{ u.u_error = EFAULT; return; }

	if (call.addr.maxlen < sizeof (INADDR))
		{ u.u_error = TBUFOVFLW; return; }

	/*
	 *	Verifica o estado do "endpoint"
	 */
	if (tp->tp_state == S_BOUND)
		tp->tp_state = S_LISTEN;

	if (tp->tp_state != S_LISTEN)
		{ u.u_error = TOUTSTATE; return; }

	if (tp->tp_listen_maxqlen <= 0)
		{ u.u_error = TBADQLEN; return; }

	/*
	 *	Espera chegar um pedido de conexão (um SYN)
	 */
	for (EVER)
	{
		if (tp->tp_rst)
			{ u.u_error = TLOOK; return; }

		SPINLOCK (&tp->tp_inq_lock);

		for (lp = tp->tp_listen_q, end_listen_q = lp + LISTENQ_MAXSZ; lp < end_listen_q; lp++)
		{
			if (lp->tp_listen_seq > 0)
				goto found;

		}	/* end for (lista de listen) */

		EVENTCLEAR (&tp->tp_inq_nempty);

		SPINFREE (&tp->tp_inq_lock);

		if (u.u_oflag & O_NDELAY)
			{ u.u_error = TNODATA; return; }

		EVENTWAIT (&tp->tp_inq_nempty, PITNETIN);

	}	/* end for (EVER) */

	/*
	 *	O servidor recebe o endereço do cliente e o no. de seqüência
	 */
    found:
	addr.a_addr = lp->tp_listen_addr;
	addr.a_port = lp->tp_listen_port;

	call.addr.len	= sizeof (INADDR);
	call.sequence	= lp->tp_listen_seq;

	lp->tp_listen_seq = -lp->tp_listen_seq;		/* "t_listen" já forneceu */

	SPINFREE (&tp->tp_inq_lock);

	call.opt.len	= 0;
	call.udata.len	= 0;

	if (unimove (call.addr.buf, &addr, sizeof (INADDR), SU) < 0)
		{ u.u_error = EFAULT; return; }

	if (unimove (callp, &call, sizeof (T_CALL), SU) < 0)
		{ u.u_error = EFAULT; return; }

}	/* end k_listen */

/*
 ****************************************************************
 *	Aceita uma conexão					*
 ****************************************************************
 */
void
k_accept (int conn_fd, const T_CALL *callp)
{
	TCP_EP		*tpl, *tpc;
	KFILE		*fp = u.u_fileptr;
	ROUTE		*rp;
	LISTENQ		*lp;
	const LISTENQ	*end_listen_q;
	T_CALL		call;
	INADDR		addr;
	seq_t		ISS;

	/*
	 *	Obtém a estrutura "call"
	 */
	if (unimove (&call, callp, sizeof (T_CALL), US) < 0)
		{ u.u_error = EFAULT; return; }

#ifdef	TST_UDATA_OPT
	if (call.opt.len != 0)
		{ u.u_error = TBADOPT; return; }

	if (call.udata.len != 0)
		{ u.u_error = TBADDATA; return; }
#endif	TST_UDATA_OPT

	/*
	 *	Obtém o endereço opcional dado pelo cliente
	 */
	if (call.addr.len != 0)
	{
		if (call.addr.len != sizeof (INADDR))
			{ u.u_error = TBADADDR; return; }

		if (unimove (&addr, call.addr.buf, sizeof (INADDR), US) < 0)
			{ u.u_error = EFAULT; return; }
	}

	/*
	 *	Analisa o "endpoint" do "listen"
	 */
	if (fp->f_union != KF_ITNET)
		{ u.u_error = EBADF; return; }

	tpl = fp->f_endpoint;

	if (tpl->tp_state != S_LISTEN)
		{ u.u_error = TOUTSTATE; return; }

	/*
	 *	Analisa o "endpoint" da conexão
	 */
	if ((fp = fget (conn_fd)) == NOKFILE)
		return;

	if ((tpc = fp->f_endpoint) == NO_TCP_EP)
		{ u.u_error = EBADF; return; }

	if (tpc == tpl)
	{
		if (tpl->tp_listen_qlen > 1)
			{ u.u_error = EBADF; return; }
	}
	else /* tpc != tpl */
	{
		if (tpc->tp_state != S_BOUND)
			{ u.u_error = TOUTSTATE; return; }
	}

	/*
	 *	Procura a seqüência na fila de LISTEN (e remove)
	 */
	SPINLOCK (&tpl->tp_inq_lock);

	call.sequence = -call.sequence;		/* Está negativa */

	for (lp = tpl->tp_listen_q, end_listen_q = lp + LISTENQ_MAXSZ; /* abaixo */; lp++)
	{
		if (lp >= end_listen_q)
			{ SPINFREE (&tpl->tp_inq_lock); u.u_error = TBADSEQ; return; }

		if (lp->tp_listen_seq == call.sequence)
			break;
	}

	/*
	 *	Utiliza as informações do LISTEN
	 */
	tpc->tp_dst_addr = lp->tp_listen_addr;
	tpc->tp_dst_port = lp->tp_listen_port;
	tpc->tp_my_addr	 = lp->tp_listen_my_addr;
	tpc->tp_rcv_irs  = lp->tp_listen_irs;

	if (lp->tp_max_seg_sz > 0)
	{
		if (tpc->tp_max_seg_sz > lp->tp_max_seg_sz)
			tpc->tp_max_seg_sz = lp->tp_max_seg_sz;

		if (tpc->tp_good_wnd > lp->tp_max_seg_sz)
			tpc->tp_good_wnd   = lp->tp_max_seg_sz;
	}

#ifdef	MSG
	if (itscb.it_list_info)
	{
		printf ("%g: my_addr = ");
		pr_itn_addr (tpc->tp_my_addr);
		printf (", dst_addr = ");
		pr_itn_addr (tpc->tp_dst_addr);
		printf (", recebi max_seg_sz = %d\n", lp->tp_max_seg_sz);
		printf
		(	"\tUsando tp_max_seg_sz = %d, tp_good_wnd = %d\n",
			tpc->tp_max_seg_sz, tpc->tp_good_wnd
		);
	}
#endif	MSG

	lp->tp_listen_seq = 0;	/* Remove */

	tpl->tp_listen_qlen--;

	SPINFREE (&tpl->tp_inq_lock);

	/*
	 *	Se o usuário forneceu o endereço, confere
	 */
	if (call.addr.len != 0)
	{
		if (tpc->tp_dst_addr != addr.a_addr || tpc->tp_dst_port != addr.a_port)
			{ u.u_error = TBADADDR; return; }
	}

	/*
	 *	Aloca a área circular para a E/S
	 */
	if (circular_area_alloc (tpc) < 0)
		return;

	/*
	 *	Inicializa as seqüências
	 */
	SLEEPLOCK (&tpc->tp_lock, PITNETOUT);

	ISS = (time << 7) | hz;		/* Gera ISS de 0 a 2 ** 32 */
	ISS <<= 8;			/* Com ciclo de +- 12 horas */
	ISS = ISS + ISS + ISS;

	tpc->tp_snd_iss = ISS;
	tpc->tp_snd_una = ISS;
	tpc->tp_snd_nxt = ISS;
	tpc->tp_snd_wnd = ISS + 1;	/* Será decidido no ACK_of_SYN */
	tpc->tp_snd_up  = ISS;
	tpc->tp_snd_psh = ISS;

   /***	tpc->tp_rcv_irs = ...; /* acima ***/
	tpc->tp_rcv_usr = tpc->tp_rcv_irs + 1;
	tpc->tp_rcv_nxt = tpc->tp_rcv_irs + 1;
	tpc->tp_rcv_wnd = tpc->tp_rcv_nxt + itscb.it_WND_SZ;
	tpc->tp_old_wnd = tpc->tp_rcv_wnd;
	tpc->tp_rcv_up  = tpc->tp_rcv_nxt;
	tpc->tp_rcv_psh = tpc->tp_rcv_nxt;

	/*
	 *	Obtém o dispositivo destino e endereço fonte
	 */
	if ((rp = get_route_entry (tpc->tp_dst_addr)) == NOROUTE)
		{ SLEEPFREE (&tpc->tp_lock); u.u_error = TNOROUTE; return; }

   /***	tpc->tp_my_addr	= rp->r_my_addr; ***/
	tpc->tp_route	= rp;

	sort_tcp_ep (tpc);

	/*
	 *	Confirma a conexão
	 */
   /***	SPINLOCK (&tpc->tp_rnd_out.tp_rnd_lock); ***/

	tpc->tp_rnd_out.tp_rnd_syn = C_SYN|C_ACK;

   /***	SPINFREE (&tpc->tp_rnd_out.tp_rnd_lock); ***/

	tpc->tp_listen_maxqlen = 0;	/* Pode ser igual a "tpl" */

	tpc->tp_state = S_SYN_RECEIVED;

	send_tcp_ctl_packet (C_SYN|C_ACK, tpc);

	SLEEPFREE (&tpc->tp_lock);

	tpc->tp_last_snd_time = time;
	tpc->tp_last_ack_time = time;

	/*
	 *	Espera chegar o ACK da confirmação de conexão
	 */
	for (EVER)
	{
		if (tpc->tp_rst)
			{ u.u_error = TLOOK; return; }

		SPINLOCK (&tpc->tp_inq_lock);

		if (tpc->tp_state == S_ESTABLISHED)
			{ SPINFREE (&tpc->tp_inq_lock); break; }

		EVENTCLEAR (&tpc->tp_inq_nempty);

		SPINFREE (&tpc->tp_inq_lock);

		EVENTWAIT (&tpc->tp_inq_nempty, PITNETIN);

	}	/* end for (EVER) */

	/*
	 *	A mudança de estado é realizada por "rcv"
	 */
   /***	tpc->tp_state = S_ESTABLISHED; ***/

	check_for_pipe_mode (tpc);

}	/* end k_accept */

/*
 ****************************************************************
 *	Espera a resposta de um pedido de conexão		*
 ****************************************************************
 */
void
k_rcvconnect (T_CALL *rcvcall)
{
	KFILE		*fp = u.u_fileptr;
	TCP_EP		*tp;
	const LISTENQ	*lp;
	ROUTE		*rp;
	T_CALL		call;
	INADDR		addr;

	/*
	 *	Analisa o EP
	 */
	if (fp->f_union != KF_ITNET)
		{ u.u_error = EBADF; return; }

	tp = fp->f_endpoint;

#if (0)	/*******************************************************/
	if (tp->tp_state != S_SYN_SENT && tp->tp_state != S_ESTABLISHED)
		{ u.u_error = TOUTSTATE; return; }
#endif	/*******************************************************/

	if (tp->tp_state != S_SYN_SENT)
	{
		if (tp->tp_rst)
			{ u.u_error = TLOOK; return; }

		if (tp->tp_state != S_ESTABLISHED)
			{ u.u_error = TOUTSTATE; return; }
	}

	/*
	 *	Espera chegar a confirmação do pedido de conexão
	 */
	for (EVER)
	{
		if (tp->tp_rst)
			{ u.u_error = TLOOK; return; }

		SPINLOCK (&tp->tp_inq_lock);

		if (tp->tp_state == S_ESTABLISHED)
			{ SPINFREE (&tp->tp_inq_lock); break; }

		EVENTCLEAR (&tp->tp_inq_nempty);

		SPINFREE (&tp->tp_inq_lock);

		EVENTWAIT (&tp->tp_inq_nempty, PITNETIN);

	}	/* end for (EVER) */

	/*
	 *	A mudança de estado é realizada por "rcv"
	 */
   /***	tp->tp_state = S_ESTABLISHED; ***/

	/*
	 *	Recebe o novo endereço (alternativo) do servidor
	 */
	lp = tp->tp_listen_q;

	addr.a_addr = lp->tp_listen_addr;
	addr.a_port = lp->tp_listen_port;

	/*
	 *	Obtém o dispositivo destino e endereço fonte
	 */
	if ((rp = get_route_entry (addr.a_addr)) == NOROUTE)
		{ u.u_error = TNOROUTE; return; }

#if (0)	/*************************************/
	if (tp->tp_dst_addr != 0  &&  tp->tp_dst_addr != addr.a_addr)
		{ u.u_error = TBADADDR; return; }

	if (tp->tp_dst_port != 0  &&  tp->tp_dst_port != addr.a_port)
		{ u.u_error = TBADADDR; return; }
#endif	/*************************************/

	SLEEPLOCK (&tp->tp_lock, PITNETOUT);

	tp->tp_dst_addr = addr.a_addr;
	tp->tp_dst_port = addr.a_port;

   /***	tp->tp_my_addr	= rp->r_my_addr; ***/
	tp->tp_my_addr  = lp->tp_listen_my_addr;
	tp->tp_route	= rp;

	/*
	 *	Trata o "max_seg_sz" do bloco contendo o SYN
	 */
	if (lp->tp_max_seg_sz > 0)
	{
		if (tp->tp_max_seg_sz > lp->tp_max_seg_sz)
			tp->tp_max_seg_sz = lp->tp_max_seg_sz;

		if (tp->tp_good_wnd > lp->tp_max_seg_sz)
			tp->tp_good_wnd   = lp->tp_max_seg_sz;
	}

#ifdef	MSG
	if (itscb.it_list_info)
	{
		printf ("%g: recebi max_seg_sz = %d\n", lp->tp_max_seg_sz);
		printf
		(	"\tUsando tp_max_seg_sz = %d, tp_good_wnd = %d\n",
			tp->tp_max_seg_sz, tp->tp_good_wnd
		);
	}
#endif	MSG

	SLEEPFREE (&tp->tp_lock);

	/*
	 *	Envia o ACK da confirmação de conexão
	 */
	send_tcp_ctl_packet (C_ACK, tp);

	/*
	 *	Copia as informações para o usuário
	 */
	if (rcvcall)
	{
		if (unimove (&call, rcvcall, sizeof (T_CALL), US) < 0)
			{ u.u_error = EFAULT; return; }

		if (call.addr.maxlen < sizeof (INADDR))
			{ u.u_error = TBUFOVFLW; return; }

		if (unimove (call.addr.buf, &addr, sizeof (INADDR), SU) < 0)
			{ u.u_error = EFAULT; return; }

		call.addr.len	= sizeof (INADDR);

		call.opt.len	= 0;
		call.udata.len	= 0;
		call.sequence	= 0;

		if (unimove (rcvcall, &call, sizeof (T_CALL), SU) < 0)
			{ u.u_error = EFAULT; return; }
	}

	check_for_pipe_mode (tp);

}	/* end k_rcvconnect */

/*
 ****************************************************************
 *	Coloca o EP no local correto da fila de TCP EPs		*
 ****************************************************************
 */
void
sort_tcp_ep (TCP_EP *tp)
{
	ITSCB		*ip = &itscb;
	TCP_EP		*atp, *btp;

	SPINLOCK (&ip->it_tep_lock);

	/*
	 *	Em primeiro lugar, procura o antecessor
	 */
	for
	(	atp = NO_TCP_EP, btp = ip->it_tep_busy;
		/* abaixo */;
		atp = btp,  btp = btp->tp_next
	)
	{
		if (btp == NO_TCP_EP)
		{
#ifdef	MSG
			printf ("%g: Nao encontrei o TCP EP na fila, port = %d\n", tp->tp_my_port);
#endif	MSG
			atp = NO_TCP_EP;
			tp->tp_next = ip->it_tep_busy;
			ip->it_tep_busy = tp;

			break;
		}

		if (btp == tp)
			break;
	}

	/*
	 *	Primeiro caso: Dado endereço e porta
	 */
	if (tp->tp_dst_port != 0)
	{
#ifdef	MSG
		if (tp->tp_dst_addr == 0)
			printf ("%g: Faltando endereco, port = %d\n", tp->tp_my_port);
#endif	MSG
		/*
		 *	Primeiro caso, variante "a", conexão remota: Fácil, Coloca no início 
		 */
		if (tp->tp_dst_addr != MY_IP_ADDR)
		{
			if (ip->it_tep_busy != tp)	/* Implica atp != NO_TCP_EP */
			{
				atp->tp_next = tp->tp_next;

				tp->tp_next = ip->it_tep_busy;
				ip->it_tep_busy = tp;
			}

			SPINFREE (&ip->it_tep_lock);
			return;
		}

		/*
		 *	Primeiro caso, variante "b", conexão local: Coloca depois das remotas
		 */
		if (atp == NO_TCP_EP)
			ip->it_tep_busy = tp->tp_next;
		else
			atp->tp_next = tp->tp_next;

		for
		(	atp = NO_TCP_EP, btp = ip->it_tep_busy;
			/* abaixo */;
			atp = btp,  btp = btp->tp_next
		)
		{
			if (btp == NO_TCP_EP)
				break;

			if (btp->tp_dst_addr == 0 || btp->tp_dst_port == 0)
				break;

			if (btp->tp_dst_addr == MY_IP_ADDR)
				break;
		}

		if (atp == NO_TCP_EP)
			ip->it_tep_busy = tp;
		else
			atp->tp_next = tp;

		tp->tp_next = btp;

		SPINFREE (&ip->it_tep_lock);

		return;
	}

	/*
	 *	Segundo caso: Não dado nem endereço nem porta
	 *
	 *	Fácil: Coloca no final
	 */
	if (tp->tp_dst_addr == 0)
	{
		if (tp->tp_next != NO_TCP_EP)
		{
			if (atp == NO_TCP_EP)
				ip->it_tep_busy = tp->tp_next;
			else
				atp->tp_next = tp->tp_next;

			for (btp = tp; btp->tp_next != NO_TCP_EP; btp = btp->tp_next)
				/* vazio */;

			btp->tp_next = tp;
			tp->tp_next  = NO_TCP_EP;
		}

		SPINFREE (&ip->it_tep_lock);
		return;
	}

	/*
	 *	Terceiro caso: Dado apenas o endereço
	 *
	 *	Coloca antes do que não tem endereço nem porta
	 */
	if (atp == NO_TCP_EP)
		ip->it_tep_busy = tp->tp_next;
	else
		atp->tp_next = tp->tp_next;

	for
	(	atp = NO_TCP_EP, btp = ip->it_tep_busy;
		/* abaixo */;
		atp = btp,  btp = btp->tp_next
	)
	{
		if (btp == NO_TCP_EP)
			break;

		if (btp->tp_my_port != tp->tp_my_port)
			continue;

		if (btp->tp_dst_addr == 0)
			break;
	}

	if (atp == NO_TCP_EP)
		ip->it_tep_busy = tp;
	else
		atp->tp_next = tp;

	tp->tp_next = btp;

	SPINFREE (&ip->it_tep_lock);

}	/* end sort_tcp_ep */

/*
 ****************************************************************
 *	Verifica se o destino é o próprio computador		*
 ****************************************************************
 */
void
check_for_pipe_mode (TCP_EP *tp)
{
	ITSCB		*ip = &itscb;
	TCP_EP		*dst_tp;

	/*
	 *	Se o modo não foi pedido, nada faz
	 */
	if (!ip->it_pipe_mode)
		return;

	if (tp->tp_dst_addr != MY_IP_ADDR)
		return;

	/*
	 *	Examina os "endpoints"
	 */
	SPINLOCK (&ip->it_tep_lock);

	for (dst_tp = ip->it_tep_busy; /* abaixo */; dst_tp = dst_tp->tp_next)
	{
		if (dst_tp == NO_TCP_EP)
			{ SPINFREE (&ip->it_tep_lock); return; }

		if (dst_tp->tp_my_addr != MY_IP_ADDR)
			continue;

		if (dst_tp->tp_my_port == tp->tp_dst_port)
			break;

	}	/* end procurando o EP correspondente ao "port" */

	/*
	 *	Altera o modo
	 */
	tp->tp_pipe_tcp_ep = dst_tp; 
	tp->tp_pipe_mode   = 1; 

	dst_tp->tp_pipe_tcp_ep = tp; 
	dst_tp->tp_pipe_mode   = 1; 

	SPINFREE (&ip->it_tep_lock);

	EVENTDONE (&tp->tp_pipe_event);
	EVENTWAIT (&dst_tp->tp_pipe_event, PITNETIN);

} 	/* end check_for_pipe_mode */
