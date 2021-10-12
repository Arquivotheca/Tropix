/*
 ****************************************************************
 *								*
 *			daemon.c				*
 *								*
 *  IN_DAEMON - Processa datagramas dos dispositivos de entrada	*
 * OUT_DAEMON - Processa a fila de saída TCP 			*
 *								*
 *	Versão	3.0.0, de 10.08.91				*
 *		4.9.0, de 22.08.06				*
 *								*
 *	Funções:						*
 *		wake_up_in_daemon,  execute_in_daemon		*
 *		execute_out_daemon				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 * 		Copyright © 2006 NCE/UFRJ - tecle "man licença"	*
 * 								*
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
#include "../h/itnet.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
#define	FAR_FUTURE 0x7FFFFFFF		/* Tempo bem no futuro */

#define	RAW_TIME	30		/* Tempo entre cada exame da fila RAW */

entry char	*domain_name;		/* Nome do domain */

/*
 ****** Protótipos de funções ***********************************
 */
void		examine_raw_queues (void);
void		examine_and_retransmit_outq (TCP_EP *tp, int listen_queue_examine);

/*
 ****************************************************************
 *	Põe um datagrama/TCP_EP na fila de entrada da INTERNET	*
 ****************************************************************
 */
void
wake_up_in_daemon (IN_ENUM inq_enum, void *inq_ptr)
{
	ITSCB		*ip = &itscb;
	ITBLOCK		*bp;
	TCP_EP		*tp;

	SPINLOCK (&ip->it_inq_lock);

	if (inq_enum == IN_EP)
	{
		tp = inq_ptr;

		if (tp->tp_inq_present)
		{
			SPINFREE (&ip->it_inq_lock);
			return;
		}
	}

	if   (ip->it_inq_first.inq_ptr == NOVOID)
	{
		ip->it_inq_first.inq_enum = inq_enum;
		ip->it_inq_first.inq_ptr  = inq_ptr;
	}
	elif (ip->it_inq_last.inq_enum == IN_BP)
	{
		bp = ip->it_inq_last.inq_ptr;

		bp->it_inq_forw.inq_enum = inq_enum;
		bp->it_inq_forw.inq_ptr  = inq_ptr;
	}
	else	/* IN_EP */
	{
		tp = ip->it_inq_last.inq_ptr;

		tp->tp_inq_forw.inq_enum = inq_enum;
		tp->tp_inq_forw.inq_ptr  = inq_ptr;
	}

	if (inq_enum == IN_BP)
	{
		bp = inq_ptr;

		bp->it_inq_forw.inq_enum = IN_EOF;
		bp->it_inq_forw.inq_ptr  = NOVOID;
	}
	else	/* IN_EP */
	{
		tp = inq_ptr;

		tp->tp_inq_forw.inq_enum = IN_EOF;
		tp->tp_inq_forw.inq_ptr  = NOVOID;

		tp->tp_inq_present = 1;
	}

	ip->it_inq_last.inq_enum = inq_enum;
	ip->it_inq_last.inq_ptr  = inq_ptr;

	EVENTDONE (&ip->it_inq_nempty);

	SPINFREE (&ip->it_inq_lock);

}	/* end wake_up_in_daemon */

/*
 ****************************************************************
 * IN_DAEMON - Processa datagramas dos dispositivos de entrada	*
 ****************************************************************
 */
void
execute_in_daemon (void *ptr)
{
	ITSCB		*ip = &itscb;
	KFILE		*fp;
	DNS		*dp;
	ROUTE		*rp;
	ITBLOCK		*bp;
	TCP_EP		*tp;
	char		*cp;
	void		*p;
	int		n;

	/*
	 *	Lê a tabela DNS do usuário
	 */
	strcpy (u.u_pgname, "in_daemon");

	if ((int)(p = (void *)gulong (ptr)) < 0)
		{ u.u_error = EFAULT; return; }

	if (unimove (scb.y_dns, p, scb.y_n_dns * sizeof (DNS), US) < 0)
		{ u.u_error = EFAULT; return; }

	ptr += sizeof (DNS *);

	/*
	 *	Lê a tabela ROUTE do usuário
	 */
	if ((int)(p = (void *)gulong (ptr)) < 0)
		{ u.u_error = EFAULT; return; }

	if (unimove (scb.y_route, p, scb.y_n_route * sizeof (ROUTE), US) < 0)
		{ u.u_error = EFAULT; return; }

	ptr += sizeof (ROUTE *);

	/*
	 *	Lê a tabela EXPORT do usuário
	 */
	if ((int)(p = (void *)gulong (ptr)) < 0)
		{ u.u_error = EFAULT; return; }

	if (unimove (scb.y_export, p, scb.y_n_export * sizeof (EXPORT), US) < 0)
		{ u.u_error = EFAULT; return; }

	/*
	 *	Analisa a tabela DNS dada
	 */
	for (dp = scb.y_dns; dp->d_host_nm[0] != '\0'; dp++)
	{
	   /***	strcpy (dp->d_host_nm, ...); ***/
	   /***	dp->d_host_addr = ...; ***/

		dp->d_entry_code = 'C';
	   	EVENTSET (&dp->d_resolver_done);
	   	dp->d_error_code = DNS_NO_ERROR;
	   	dp->d_server_index = 0;

	   	dp->d_expir_time = FAR_FUTURE;
	   /***	dp->d_last_use_time = 0; ***/

	   /***	dp->d_can_ptr = NODNS; ***/

		/*
		 *	Obtém o nome do domínio do primeiro nome com "."
		 */
		if (domain_name == NOSTR) for (cp = dp->d_host_nm; *cp != '\0'; cp++)
		{
			if (*cp != '.')
				continue;

			*cp = '\0';
			strscpy (scb.y_uts.uts_nodename, dp->d_host_nm, sizeof (scb.y_uts.uts_nodename));
			*cp = '.';

			domain_name = cp + 1;
			break;
		}
	}

	/*
	 *	O nome do domínio tem de ser conhecido
	 */
	if (domain_name == NOSTR)
		u.u_error = EINVAL;

	/*
	 *	Analisa a tabela ROUTE dada
	 *
	 *	Repare que o "inode" fornecido é na realidade o "fd"
	 *	Repare que a primeira entrada é do próprio nó
	 */
	for (rp = scb.y_route + 1; rp->r_dev_nm[0] != '\0'; rp++)
	{
	   /***	rp->r_mask = ...; ***/
	   /***	rp->r_net_addr = ...; ***/
	   /***	rp->r_my_addr = ...; ***/
	   /***	rp->r_gateway_addr = ...; ***/

		if ((fp = fget ((int)rp->r_inode)) == NOKFILE)
			continue;

		if ((fp->f_flags & O_RW) != O_RW)
			{ u.u_error = EBADF; continue; }

		rp->r_inode = fp->f_inode;
	   /***	rp->r_dev_nm[16] = ...; ***/

	   /***	rp->r_my_ether_addr = ...; ***/
	   /***	rp->r_ether_dev = ...; ***/

		/* Verifica se é necessário obter um endereço via DHCP */

		if (rp->r_dhcp == 0)
			continue;

		if (newproc (THREAD) == 0)
		{
			snprintf (u.u_pgname, sizeof (u.u_pgname), "dhcp_daemon_%d", rp - scb.y_route);

			dhcp_daemon (rp);

			/****** sem retorno *****/
		}

	}	/* end for (rotas) */

	/*
	 *	Verifica se não houve algum erro
	 */
	if (u.u_error != 0)
		return;

	/*
	 *	A partir daqui, não usa mais endereços virtuais de usuário
	 */
	u.u_no_user_mmu = 1;

	/*
	 ******	Trabalho principal do DAEMON: esperar a chegada de datagramas *****
	 */
	for (EVER)
	{
		/*
		 *	É possível, em certos casos, vir um alarme falso!
		 */
		for (EVER)
		{
			EVENTWAIT (&ip->it_inq_nempty, PITNETIN);

			SPINLOCK (&ip->it_inq_lock);

			if (ip->it_inq_first.inq_ptr != NOVOID)
				break;

			EVENTCLEAR (&ip->it_inq_nempty);

			SPINFREE (&ip->it_inq_lock);
		}

		/*
		 *	Achou um bloco com datagrama/tcp_ep
		 */
		if (ip->it_inq_first.inq_enum == IN_BP)
		{
			bp = ip->it_inq_first.inq_ptr;

			ip->it_inq_first.inq_enum = bp->it_inq_forw.inq_enum;

			if ((ip->it_inq_first.inq_ptr = bp->it_inq_forw.inq_ptr) == NOITBLOCK)
			{
			   /***	ip->it_inq_last.inq_enum = IN_EOF; ***/
				ip->it_inq_last.inq_ptr = NOITBLOCK;
				EVENTCLEAR (&ip->it_inq_nempty);
			}

			SPINFREE (&ip->it_inq_lock);

			receive_frame (bp);
		}
		else	/* tcp_ep */
		{
			tp = ip->it_inq_first.inq_ptr;

			ip->it_inq_first.inq_enum = tp->tp_inq_forw.inq_enum;

			if ((ip->it_inq_first.inq_ptr = tp->tp_inq_forw.inq_ptr) == NOITBLOCK)
			{
			   /***	ip->it_inq_last.inq_enum = IN_EOF; ***/
				ip->it_inq_last.inq_ptr = NO_TCP_EP;
				EVENTCLEAR (&ip->it_inq_nempty);
			}

			tp->tp_inq_present = 0;	/* Não está mais na fila de ACKs */

			SPINFREE (&ip->it_inq_lock);

			if (TCP_LT (tp->tp_old_wnd, tp->tp_rcv_wnd))
				send_tcp_ctl_packet (C_ACK, tp);
		}

	}	/* end for (EVER) */

}	/* end execute_in_daemon */

/*
 ****************************************************************
 *	OUT_DAEMON - Processa a fila de saída TCP 		*
 ****************************************************************
 */
void
execute_out_daemon (void)
{
	ITSCB		*ip = &itscb;
	TCP_EP		*tp;
	time_t		update_time;
	int		raw_time = 0, listen_queue_examine;
	int		listen_time_count = 0;

	/*
	 *	Não usa endereços virtuais de usuário
	 */
	u.u_no_user_mmu = 1; strcpy (u.u_pgname, "out_daemon");

	/*
	 *	Percorre com cuidado a lista de TCP_EPs
	 */
	for (EVER)
	{
		EVENTWAIT (&every_second_event, PITNETOUT);

		update_time = time;

		/* De minuto em minuto, examina a lista LISTEN */

		if (++listen_time_count >= 60)
			{ listen_queue_examine = 1; listen_time_count = 0; }
		else
			listen_queue_examine = 0;

	    again:
		SPINLOCK (&ip->it_tep_lock);

		for (tp = ip->it_tep_busy; tp != NO_TCP_EP; tp = tp->tp_next)
		{
			if (tp->tp_outq_time == update_time)
				continue;

			SPINFREE (&ip->it_tep_lock);

			examine_and_retransmit_outq (tp, listen_queue_examine);

			tp->tp_outq_time = update_time;	goto again;
		}

		SPINFREE (&ip->it_tep_lock);

		/*
		 *	A cada RAW_TIME segundos, examina as filas RAW
		 */
		if (++raw_time >= RAW_TIME)
			{ examine_raw_queues (); raw_time = 0; }

	}	/* end for (EVER) */

}	/* end execute_out_daemon */

/*
 ****************************************************************
 *	Examina a lista de blocos de saída de um TCP_EP		*
 ****************************************************************
 */
void
examine_and_retransmit_outq (TCP_EP *tp, int listen_queue_examine)
{
	ITSCB		*ip = &itscb;
	int		SRTT, delta_time;
	seq_t		seq;
	int		total, total_count, count;

	/*
	 *	Verifica se passou o tempo máximo de silêncio
	 */
	delta_time = time - tp->tp_last_rcv_time;

	switch (tp->tp_state)
	{
	    case S_LISTEN:
	    {
		LISTENQ		*lp;
		const LISTENQ	*end_listen_q;

		tp->tp_last_rcv_time = time;

		/* De minuto em minuto, examina a lista LISTEN */

		if (listen_queue_examine == 0)
			break;

		SPINLOCK (&tp->tp_inq_lock);

		for (lp = tp->tp_listen_q, end_listen_q = lp + LISTENQ_MAXSZ; lp < end_listen_q; lp++)
		{
			if (lp->tp_listen_seq == 0 || time - lp->tp_listen_time < tp->tp_max_wait)
				continue;

			if (tp->tp_listen_qlen > 0)
				tp->tp_listen_qlen--;

			memclr (lp, sizeof (LISTENQ));
		}

		SPINFREE (&tp->tp_inq_lock);

		break;
	    }

	    case S_ESTABLISHED:
		if (tp->tp_max_silence == 0 || delta_time <= tp->tp_max_silence)
			break;

		goto do_reset;

	    default:
		if (tp->tp_max_wait == 0 || delta_time <= tp->tp_max_wait)
			break;
	    do_reset:
		tp->tp_timeout = 1;
		reset_the_connection (tp, 1 /* Envia o RST */);
		return;

	}	/* end switch */

	/*
	 *	Verifica se há operação de saída em andamento
	 */
   /***	if (SLEEPTEST (&tp->tp_lock) < 0) ***/
	   /***	return; ***/

	/*
	 *	Verifica se a janela já está fechada há muito tempo
	 */
#define	TST_BYTE_TIME	15	/* Tempo em segundos */

	if (tp->tp_snd_una == tp->tp_snd_nxt)
	{
		if
		(	tp->tp_closed_wnd_time != 0 &&
			time - tp->tp_closed_wnd_time > TST_BYTE_TIME
		)
		{
			send_tcp_data_packet (tp, tp->tp_snd_nxt - 1, 1, C_RETRANS|C_TST_BYTE);

			tp->tp_closed_wnd_time = time;
#ifdef	MSG
			if (ip->it_retransmission)
				printf ("%g: Enviei segmento de 1 byte\n");
#endif	MSG
		}

	   /***	SLEEPFREE (&tp->tp_lock); ***/ return;
	}

	/*
	 *	Analisa o tempo
	 */
	if ((SRTT = tp->tp_SRTT) == 0)
		SRTT = 10 * 1024;	/* Default: 10   segundos */

	if ((SRTT *= ip->it_BETA) > 60 * 1024)
		SRTT = 60 * 1024;	/* Máximo:  60   segundos */

	if (SRTT < 2 * 1024 + 512)
		SRTT = 2 * 1024 + 512;	/* Mínimo:   2.5 segundos */

	if ((delta_time = 1024 * (time - tp->tp_last_ack_time) + 512) < SRTT)
		{ /*** SLEEPFREE (&tp->tp_lock); ***/ return; }

	/*
	 *	Se já retransmitiu demais, dá um RESET na conexão
	 */
	if (tp->tp_retransmitted >= ip->it_N_TRANS)
	{
		tp->tp_timeout = 1;
	   /***	tp->tp_rst = 1; ***/

		reset_the_connection (tp, 1 /* Envia o RST */);

	   /***	SLEEPFREE (&tp->tp_lock); ***/
		return;
	}

	/*
	 *	Retransmite
	 */
	SPINLOCK (&tp->tp_rnd_out.tp_rnd_lock);

	seq   = tp->tp_snd_una;

	if (tp->tp_rnd_out.tp_rnd_syn)
		seq++;

	total = tp->tp_snd_nxt - seq;

	if (tp->tp_rnd_out.tp_rnd_fin)
		total--;

	total_count = total;

	SPINFREE (&tp->tp_rnd_out.tp_rnd_lock);


	if (tp->tp_rnd_out.tp_rnd_syn)
		send_tcp_ctl_packet (tp->tp_rnd_out.tp_rnd_syn|C_RETRANS, tp);

	while (total_count > 0)
	{
		if ((count = total_count) > tp->tp_max_seg_sz)
			count = tp->tp_max_seg_sz;

		send_tcp_data_packet (tp, seq, count, C_RETRANS);

		seq	    += count;
		total_count -= count;
	}

	if (tp->tp_rnd_out.tp_rnd_fin)
		send_tcp_ctl_packet (C_FIN|C_ACK|C_RETRANS, tp);

	tp->tp_last_ack_time  = time;
	tp->tp_retransmitted += 1;

#ifdef	MSG
	if (ip->it_retransmission)
	{
		printf
		(	"%g: Retransmiti (%d :: %d): %s%s%d bytes%s\n",
			delta_time, SRTT,
			tp->tp_rnd_out.tp_rnd_syn & C_SYN ? "SYN, " : "",
			tp->tp_rnd_out.tp_rnd_syn & C_ACK ? "ACK, " : "",
			total,
			tp->tp_rnd_out.tp_rnd_fin ? ", FIN" : ""
		);
	}
#endif	MSG

   /***	SLEEPFREE (&tp->tp_lock); ***/

}	/* end examine_and_retransmit_outq */

/*
 ****************************************************************
 *	A cada N segundos devolve os blocos "raw" antigos	*
 ****************************************************************
 */
void
examine_raw_queues (void)
{
	ITSCB		*ip = &itscb;
	RAW_EP		*rp;
	ITBLOCK		*bp, *abp;
	ITBLOCK		*it_forw;

#define	N	RAW_TIME	/* Tempo em segundos */

    again:
	SPINLOCK (&ip->it_rep_lock);

	for (rp = ip->it_rep_busy; rp != NO_RAW_EP; rp = rp->rp_next)
	{
	   	SPINLOCK (&rp->rp_inq_lock);

		for
		(	abp = NOITBLOCK, bp = rp->rp_inq_first;
			bp != NOITBLOCK;
			abp = bp, bp = it_forw
		)
		{
			it_forw = bp->it_inq_forw.inq_ptr;

			if (bp->it_time + N >= time)
				continue;

			/*
			 *	Achou um bloco para remover
			 */
			if (abp == NOITBLOCK)
				rp->rp_inq_first = it_forw;
			else
				abp->it_inq_forw.inq_ptr = it_forw;

			if (rp->rp_inq_first == NOITBLOCK)
				EVENTCLEAR (&rp->rp_inq_nempty);

			if (rp->rp_inq_last == bp)
				rp->rp_inq_last = abp;

		   	SPINFREE (&rp->rp_inq_lock);

			SPINFREE (&ip->it_rep_lock);

			put_it_block (bp);

			goto again;

		}	/* end (percorrendo a fila) */

	   	SPINFREE (&rp->rp_inq_lock);

	}	/* end percorrendo os endpoints RAW */

	SPINFREE (&ip->it_rep_lock);

}	/* end examine_raw_queues */

/*
 ****************************************************************
 *	Acrescenta uma nova entrada na tabela de Roteamento	*
 ****************************************************************
 */
int
add_route_entry (IPADDR my_addr, const char *dev_nm, INODE *ip)
{
	ROUTE		*rp, *lp;

	/*
	 *	Procura a última entrada da tabela
	 */
	for (lp = scb.y_route; /* abaixo */; lp++)
	{
		if (lp->r_dev_nm[0] == '\0')
			break;
	}

	if (lp + 1 >= scb.y_end_route)
		{ u.u_error = EBUSY; return (-1); }

	/*
	 *	Verifica se a última é "default"
	 */
	rp = lp - 1;	/* A última entrada ocupada */

	lp++;		/* O novo marcador */

	if (rp >= scb.y_route && rp->r_net_addr == 0)
		memmove (rp + 1, rp, sizeof (ROUTE));
	else
		rp++;

	/*
	 *	Prepara a nova entrada
	 */
	rp->r_mask = 0;
	rp->r_net_addr = 0;

	rp->r_my_addr = my_addr;
	rp->r_gateway_addr = 0;

	rp->r_inode = ip;
	strcpy (rp->r_dev_nm, dev_nm);

   /***	rp->r_my_ether_addr = ... ***/
	rp->r_ether_dev = 0;

	/*
	 *	Não esquece do marcador de final
	 */
	lp->r_dev_nm[0] = '\0';

	return (0);

}	/* end add_route_entry */

/*
 ****************************************************************
 *	Remove uma entrada da tabela de Roteamento		*
 ****************************************************************
 */
int
del_route_entry (IPADDR my_addr)
{
	ROUTE		*rp, *empty_rp;

	/*
	 *	Procura a entrada da tabela
	 */
	for (rp = scb.y_route; /* abaixo */; rp++)
	{
		if (rp->r_dev_nm[0] == '\0')
			{ u.u_error = EINVAL; return (-1); }

		if (rp->r_my_addr == my_addr)
			break;
	}

	/*
	 *	Procura a última entrada da tabela
	 */
	for (empty_rp = rp + 1; /* abaixo */; empty_rp++)
	{
		if (empty_rp->r_dev_nm[0] == '\0')
			break;
	}

	memmove (rp, rp + 1, (char *)empty_rp - (char *)rp);

	return (0);

}	/* end del_route_entry */
