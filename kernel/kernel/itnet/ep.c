/*
 ****************************************************************
 *								*
 *			ep.c					*
 *								*
 *	Funções de inicialização do INTERNET			*
 *								*
 *	Versão	3.0.00, de 26.07.91				*
 *		3.1.00, de 14.07.98				*
 *								*
 *	Funções:						*
 *		raw_ep_free_init,	get_raw_ep,		*
 *		put_raw_ep,		delete_raw_queues	*
 *		udp_ep_free_init,	get_udp_ep,		*
 *		put_udp_ep,		delete_udp_queues	*
 *		tcp_ep_free_init,	get_tcp_ep		*
 *		put_tcp_ep,		delete_udp_queues	*
 *		delete_retrans_queue				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 * 		Copyright © 1999 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include "../h/common.h"
#include "../h/scb.h"
#include "../h/sync.h"

#include "../h/itnet.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Inicializa a lista livre de RAW EPs			*
 ****************************************************************
 */
void
raw_ep_free_init (void)
{
	register ITSCB	*ip = &itscb;
	register RAW_EP	*rp;

   /***	SPINLOCK (&ip->it_rep_lock); ***/

	ip->it_rep_free = NO_RAW_EP;

	for (rp = scb.y_end_raw_ep - 1; rp >= scb.y_raw_ep; rp--)
	{
		rp->rp_state = -1;

		rp->rp_next = ip->it_rep_free;
		ip->it_rep_free = rp;
	}

	ip->it_rep_total = scb.y_n_raw_ep;
   	ip->it_rep_count = 0;
   	ip->it_rep_busy = NO_RAW_EP;

   /*** SPINFREE (&ip->it_rep_lock); ***/

}	/* end raw_ep_free_init */

/*
 ****************************************************************
 *	Obtém um RAW EP da lista livre				*
 ****************************************************************
 */
RAW_EP	*
get_raw_ep (void)
{
	register ITSCB	*ip = &itscb;
	register RAW_EP	*rp;

	SPINLOCK (&ip->it_rep_lock);

	if ((rp = ip->it_rep_free) == NO_RAW_EP)
		{ SPINFREE (&ip->it_rep_lock); return (NO_RAW_EP); }

	ip->it_rep_free = rp->rp_next;

	memclr (rp, sizeof (RAW_EP));

	rp->rp_next = ip->it_rep_busy;
	ip->it_rep_busy = rp;

	ip->it_rep_count++;

	SPINFREE (&ip->it_rep_lock);

	return (rp);

}	/* end get_raw_ep */

/*
 ****************************************************************
 *	Devolve um RAW EP para a lista livre			*
 ****************************************************************
 */
void
put_raw_ep (register RAW_EP *rp)
{
	register ITSCB	*ip = &itscb;
	register RAW_EP *arp;

#ifdef	MSG
	if (rp == NO_RAW_EP)
	    { printf ("%g: Devolvendo EP NULO\n"); return; }
#endif	MSG

	rp->rp_state = -1;

	/*
	 *	Em primeiro lugar, tira da fila "busy"
	 */
	SPINLOCK (&ip->it_rep_lock);

	if (ip->it_rep_busy == rp)
	{
		ip->it_rep_busy = rp->rp_next;
	}
	else for (arp = ip->it_rep_busy; /* abaixo */; arp = arp->rp_next)
	{
		if (arp == NO_RAW_EP)
		{
			SPINFREE (&ip->it_rep_lock);
#ifdef	MSG
			printf ("%g: Não achei o EP na fila BUSY\n");
#endif	MSG
			return;
		}

		if (arp->rp_next == rp)
		{
			arp->rp_next = rp->rp_next;
			break;
		}
	}

	SPINFREE (&ip->it_rep_lock);

	/*
	 *	Remove as filas
	 */
	delete_raw_queues (rp);

	/*
	 *	Devolve o EP para a lista livre
	 */
	SPINLOCK (&ip->it_rep_lock);

	rp->rp_next = ip->it_rep_free;
	ip->it_rep_free = rp;

	ip->it_rep_count--;

	SPINFREE (&ip->it_rep_lock);

}	/* end put_raw_ep */

/*
 ****************************************************************
 *	Libera as filas de um RAW EP				*
 ****************************************************************
 */
void
delete_raw_queues (register RAW_EP *rp)
{
	register ITBLOCK *bp;
	register int	i;
	ITBLOCK	 	*it_forw;

	/*
	 *	Devolve os blocos da fila de entrada
	 */
   	SPINLOCK (&rp->rp_inq_lock);

	bp = rp->rp_inq_first;
	rp->rp_inq_first = NOITBLOCK;

	EVENTCLEAR (&rp->rp_inq_nempty);

   	SPINFREE (&rp->rp_inq_lock);

	for (i = 0; bp != NOITBLOCK; bp = it_forw)
		{ it_forw = bp->it_inq_forw.inq_ptr; put_it_block (bp); i++; }

#ifdef	MSG
	if (i > 0 && itscb.it_report_error)
		printf ("%g: %d bloco(s) residual(is) na entrada\n", i);
#endif	MSG

}	/* end delete_raw_queues */

/*
 ****************************************************************
 *	Inicializa a lista livre de UDP EPs			*
 ****************************************************************
 */
void
udp_ep_free_init (void)
{
	register ITSCB	*ip = &itscb;
	register UDP_EP	*up;

   /***	SPINLOCK (&ip->it_uep_lock); ***/

	ip->it_uep_free = NO_UDP_EP;

	for (up = scb.y_end_udp_ep - 1; up >= scb.y_udp_ep; up--)
	{
		up->up_state = -1;

		up->up_next = ip->it_uep_free;
		ip->it_uep_free = up;
	}

	ip->it_uep_total = scb.y_n_udp_ep;
   	ip->it_uep_count = 0;
   	ip->it_uep_busy = NO_UDP_EP;

   /*** SPINFREE (&ip->it_uep_lock); ***/

}	/* end udp_ep_free_init */

/*
 ****************************************************************
 *	Obtém um UDP EP da lista livre				*
 ****************************************************************
 */
UDP_EP	*
get_udp_ep (void)
{
	register ITSCB	*ip = &itscb;
	register UDP_EP	*up;

	SPINLOCK (&ip->it_uep_lock);

	if ((up = ip->it_uep_free) == NO_UDP_EP)
		{ SPINFREE (&ip->it_uep_lock); return (NO_UDP_EP); }

	ip->it_uep_free = up->up_next;

	memclr (up, sizeof (UDP_EP));

	up->up_next = ip->it_uep_busy;
	ip->it_uep_busy = up;

	ip->it_uep_count++;

	SPINFREE (&ip->it_uep_lock);

	return (up);

}	/* end get_udp_ep */

/*
 ****************************************************************
 *	Devolve um UDP EP para a lista livre			*
 ****************************************************************
 */
void
put_udp_ep (register UDP_EP *up)
{
	register ITSCB	*ip = &itscb;
	register UDP_EP *aup;

#ifdef	MSG
	if (up == NO_UDP_EP)
	    { printf ("%g: Devolvendo EP NULO\n"); return; }
#endif	MSG

	up->up_state = -1;

	/*
	 *	Em primeiro lugar, tira da fila "busy"
	 */
	SPINLOCK (&ip->it_uep_lock);

	if (ip->it_uep_busy == up)
	{
		ip->it_uep_busy = up->up_next;
	}
	else for (aup = ip->it_uep_busy; /* abaixo */; aup = aup->up_next)
	{
		if (aup == NO_UDP_EP)
		{
			SPINFREE (&ip->it_uep_lock);
#ifdef	MSG
			printf ("%g: Não achei o EP na fila BUSY\n");
#endif	MSG
			return;
		}

		if (aup->up_next == up)
		{
			aup->up_next = up->up_next;
			break;
		}
	}

	SPINFREE (&ip->it_uep_lock);

	/*
	 *	Remove as filas
	 */
	delete_udp_queues (up);

	/*
	 *	Devolve o EP para a lista livre
	 */
	SPINLOCK (&ip->it_uep_lock);

	up->up_next = ip->it_uep_free;
	ip->it_uep_free = up;

	ip->it_uep_count--;

	SPINFREE (&ip->it_uep_lock);

}	/* end put_udp_ep */

/*
 ****************************************************************
 *	Libera as filas de um UDP EP				*
 ****************************************************************
 */
void
delete_udp_queues (register UDP_EP *up)
{
	register ITBLOCK *bp;
	register int	i;
	ITBLOCK	 	*it_forw;

	/*
	 *	Devolve os blocos da fila de entrada
	 */
   	SPINLOCK (&up->up_inq_lock);

	bp = up->up_inq_first;
	up->up_inq_first = NOITBLOCK;

	EVENTCLEAR (&up->up_inq_nempty);

   	SPINFREE (&up->up_inq_lock);

	for (i = 0; bp != NOITBLOCK; bp = it_forw)
		{ it_forw = bp->it_inq_forw.inq_ptr; put_it_block (bp); i++; }

#ifdef	MSG
	if (i > 0 && itscb.it_report_error)
		printf ("%g: %d bloco(s) residual(is) na entrada\n", i);
#endif	MSG

}	/* end delete_udp_queues */

/*
 ****************************************************************
 *	Inicializa a lista livre de TCP EPs			*
 ****************************************************************
 */
void
tcp_ep_free_init (void)
{
	register ITSCB	*ip = &itscb;
	register TCP_EP	*tp;

   /***	SPINLOCK (&ip->it_tep_lock); ***/

	ip->it_tep_free = NO_TCP_EP;

	for (tp = scb.y_end_tcp_ep - 1; tp >= scb.y_tcp_ep; tp--)
	{
		tp->tp_state = -1;

		tp->tp_next = ip->it_tep_free;
		ip->it_tep_free = tp;
	}

	ip->it_tep_total = scb.y_n_tcp_ep;
   	ip->it_tep_count = 0;
   	ip->it_tep_busy = NO_TCP_EP;

   /*** SPINFREE (&ip->it_tep_lock); ***/

}	/* end tcp_ep_free_init */

/*
 ****************************************************************
 *	Obtém um TCP EP da lista livre				*
 ****************************************************************
 */
TCP_EP	*
get_tcp_ep (void)
{
	register ITSCB	*ip = &itscb;
	register TCP_EP	*tp;

	SPINLOCK (&ip->it_tep_lock);

	if ((tp = ip->it_tep_free) == NO_TCP_EP)
		{ SPINFREE (&ip->it_tep_lock); return (NO_TCP_EP); }

	ip->it_tep_free = tp->tp_next;

	memclr (tp, sizeof (TCP_EP));

	tp->tp_next = ip->it_tep_busy;
	ip->it_tep_busy = tp;

	ip->it_tep_count++;

	SPINFREE (&ip->it_tep_lock);

	return (tp);

}	/* end get_tcp_ep */

/*
 ****************************************************************
 *	Devolve um TCP EP para a lista livre			*
 ****************************************************************
 */
void
put_tcp_ep (register TCP_EP *tp)
{
	register ITSCB	*ip = &itscb;
	register TCP_EP *atp;

#ifdef	MSG
	if (tp == NO_TCP_EP)
	    { printf ("%g: Devolvendo EP NULO\n"); return; }
#endif	MSG

	tp->tp_state = -1;

	/*
	 *	Em primeiro lugar, tira da fila "busy"
	 */
	SPINLOCK (&ip->it_tep_lock);

	if (ip->it_tep_busy == tp)
	{
		ip->it_tep_busy = tp->tp_next;
	}
	else for (atp = ip->it_tep_busy; /* abaixo */; atp = atp->tp_next)
	{
		if (atp == NO_TCP_EP)
		{
			SPINFREE (&ip->it_tep_lock);
#ifdef	MSG
			printf ("%g: Não achei o EP na fila BUSY\n");
#endif	MSG
			return;
		}

		if (atp->tp_next == tp)
		{
			atp->tp_next = tp->tp_next;
			break;
		}
	}

	SPINFREE (&ip->it_tep_lock);

	/*
	 *	Libera a área circular
	 */
	circular_area_mrelease (tp);

	/*
	 *	Imprime o SRTT
	 */
#ifdef	MSG
	if (ip->it_list_info)
		printf ("%g: port = %d, SRTT = %d ms\n", tp->tp_my_port, tp->tp_SRTT);
#endif	MSG

	/*
	 *	Devolve o EP para a lista livre
	 */
	SPINLOCK (&ip->it_tep_lock);

	tp->tp_next = ip->it_tep_free;
	ip->it_tep_free = tp;

	ip->it_tep_count--;

	SPINFREE (&ip->it_tep_lock);

}	/* end put_tcp_ep */
