/*
 ****************************************************************
 *								*
 *			rcv.c					*
 *								*
 *	Funções relacionadas com a entrada do protocolo TCP	*
 *								*
 *	Versão	3.0.0, de 17.08.91				*
 *		4.9.0, de 21.08.06				*
 *								*
 *	Funções:						*
 *	  receive_tcp_packet, insert_tcp_segment_in_input_queue *
 *	  reset_the_connection,  remove_acked_segments		*
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
#define	ALPHA(i)	((i * ip->it_ALPHA) >> 10)

#define	ALPHA_C(i)	((i * (1024 - ip->it_ALPHA)) >> 10)

#define MIN(a, b)	((a) < (b) ? (a) : (b))

/*
 ****** Protótipos de funções ***********************************
 */
int		trim_tcp_segment (ITBLOCK *, int);
void		insert_tcp_segment_in_input_queue (TCP_EP *, ITBLOCK *);

/*
 ****************************************************************
 *	Recebe um segmento TCP					*
 ****************************************************************
 */
void
receive_tcp_packet (ITBLOCK *bp, void *area, int count)
{
	ITSCB		*ip = &itscb;
	TT_H		*thp = (TT_H *)(area - sizeof (PSD_H));
	TCP_EP		*tp;
	char		OK = 0;
	ulong		seg_seq;
	ulong		rcv_wnd;
	ulong		rcv_nxt;
	int		n;
	int		h_size, max_seg_sz;
	ushort		th_src_port, th_dst_port;
	char		inq_event = 0, outq_event = 0, wnd_event = 0;
	char		inq_locked = 0, send_ack = 0;

	/*
	 *	Prepara o pseudo-cabeçalho
	 */
	thp->ph.ph_src_addr = ENDIAN_LONG (bp->it_src_addr);
	thp->ph.ph_dst_addr = ENDIAN_LONG (bp->it_dst_addr);
	thp->ph.ph_proto = ENDIAN_SHORT (TCP_PROTO);
	thp->ph.ph_size  = ENDIAN_SHORT (count);

	th_src_port = ENDIAN_SHORT (thp->th.th_src_port);
	th_dst_port = ENDIAN_SHORT (thp->th.th_dst_port);

	/*
	 *	A "it_u_area" aponta para o PSD_H
	 */
	bp->it_u_area = thp; h_size = thp->th.th_h_size << 2;

	/*
	 ******	Verifica a correção da mensagem *****************
	 */
	if ((bp->it_count = count - h_size) < 0 || compute_checksum (thp, sizeof (PSD_H) + count) != 0)
	{
#ifdef	MSG
		if (ip->it_report_error)
			printf ("%g: Recebendo segmento com checksum incorreto\n");
#endif	MSG
		goto drop_without_reset;
	}

	/*
	 ******	Prepara os dados sobre o segmento TCP ***********
	 */
	bp->it_ctl = thp->th.th_ctl;
	bp->it_area = (char *)&thp->th + h_size;
   /***	bp->it_count = ...; /* Acima ***/

	bp->it_seg_seq = ENDIAN_LONG (thp->th.th_seq_no);
	bp->it_seg_ack = ENDIAN_LONG (thp->th.th_ack_no);

	bp->it_seg_len = bp->it_count;

	if (bp->it_ctl & C_SYN)
		bp->it_seg_len++;

	if (bp->it_ctl & C_FIN)
		bp->it_seg_len++;

	bp->it_seg_wnd = bp->it_seg_ack + ENDIAN_SHORT (thp->th.th_window);
	bp->it_seg_nxt = bp->it_seg_seq + bp->it_seg_len;

	if (bp->it_ctl & C_URG)
	{
		bp->it_seg_up = bp->it_seg_seq + ENDIAN_SHORT (thp->th.th_urg_ptr);

		if (bp->it_seg_up > bp->it_seg_nxt)
			bp->it_seg_up = bp->it_seg_nxt;
	}

   /***	bp->it_seg_prc = ...; ***/

#ifdef	MSG
	if (ip->it_list_input)
	{
		printf ("%g: Recebendo segmento de porta %d, para porta %d (", th_src_port, th_dst_port);
		if (bp->it_ctl & C_SYN)
			printf ("SYN");
		if (bp->it_ctl & C_FIN)
			printf (" FIN");
		if (bp->it_ctl & C_ACK)
			printf (" ACK");
		if (bp->it_ctl & C_PSH)
			printf (" PSH");
		if (bp->it_count)
			printf (" data = %d", bp->it_count);
		printf (")\n");
	}
#endif	MSG

	/*
	 ******	Procura o EP correspondente ao "port" ***********
	 */
	SPINLOCK (&ip->it_tep_lock);

	for (tp = ip->it_tep_busy; /* abaixo */; tp = tp->tp_next)
	{
		if (tp == NO_TCP_EP)
		{
#ifdef	MSG
			if (ip->it_report_error)
				printf ("%g: Recebendo segmento para port %d desconhecido\n", th_dst_port);
#endif	MSG
			SPINFREE (&ip->it_tep_lock);

			goto drop_perhaps_with_reset;

		}	/* if (não achou o endpoint) */

		/*
		 *	Critério para encontrar o endpoint. Leva em conta o
		 *	fato de poder ter vários com o mesmo port local
		 */
		if (tp->tp_my_port != th_dst_port)
			continue;

		if (tp->tp_dst_addr != bp->it_src_addr && tp->tp_dst_addr != 0)
			continue;

		if (tp->tp_dst_port != th_src_port && tp->tp_dst_port != 0)
		{
			if (tp->tp_state != S_SYN_SENT)
				continue;
		}

		break;

	}	/* end procurando o EP correspondente ao "port" */

	SPINFREE (&ip->it_tep_lock);

	tp->tp_last_rcv_time = time;

	/*
	 ******	Analisa o estado da conexão correspondente ******
	 */
	switch (tp->tp_state)
	{
		/*
		 ******	Fechado *********************************
		 */
	    case S_CLOSED:
	    case S_UNBOUND:
#ifdef	MSG
		if (ip->it_report_error)
		{
			printf
			(	"%g: (porta %d) Recebendo segmento em estado \"%s\" inválido\n",
				tp->tp_my_port,	get_state_nm (tp->tp_state)
			);
		}
#endif	MSG
		goto drop_perhaps_with_reset;

		/*
		 ******	Abertura passiva ************************
		 */
	    case S_BOUND:
	    case S_LISTEN:
	    {
		LISTENQ		*lp;
		const LISTENQ	*end_listen_q;

		if ((bp->it_ctl & (C_ACK|C_SYN|C_RST)) != C_SYN)
		{
			SPINLOCK (&tp->tp_inq_lock);

			for (lp = tp->tp_listen_q, end_listen_q = lp + LISTENQ_MAXSZ; lp < end_listen_q; lp++)
			{
				if (lp->tp_listen_addr == bp->it_src_addr && lp->tp_listen_port == th_src_port)
				{
					if (lp->tp_listen_seq != 0)
					{
						if (tp->tp_listen_qlen > 0)
							tp->tp_listen_qlen--;

						memclr (lp, sizeof (LISTENQ)); break;
					}
				}
			}

			SPINFREE (&tp->tp_inq_lock);

			if (bp->it_ctl & C_RST)
				goto drop_without_reset;
			else
				goto drop_with_reset;
		}

		/* Verifica se já há muitas conexões deste cliente */

		if (tp->tp_max_client_conn != 0)
		{
			TCP_EP		*tqp;
			int		conn_count = 0;

			SPINLOCK (&ip->it_tep_lock);

			for (tqp = ip->it_tep_busy; tqp != NO_TCP_EP; tqp = tqp->tp_next)
			{
				if (tqp->tp_state != S_ESTABLISHED)
					continue;

				if (tqp->tp_my_port != tp->tp_my_port)
					continue;

				if (tqp->tp_dst_addr != bp->it_src_addr)
					continue;

				if (++conn_count >= tp->tp_max_client_conn)
					{ SPINFREE (&ip->it_tep_lock); goto drop_with_reset; }
			}

			SPINFREE (&ip->it_tep_lock);
		}

		tp->tp_state = S_LISTEN;	/* Passa de BOUND para LISTEN */

		/* Verifica se o bloco tem a opção de tamanho máximo de segmento */

		max_seg_sz = 0;

		if (h_size >= sizeof (TCP_H) && thp->th.opt.th_opt_kind == 2 && thp->th.opt.th_opt_size == 4)
			max_seg_sz = ENDIAN_SHORT (thp->th.opt.th_max_seg_sz);
#ifdef	MSG
		if (ip->it_list_info)
		{
			printf
			(
				"%g: O SYN contém max_seg_sz = %d, janela = %d (de porta %d para porta %d)\n",
				max_seg_sz, ENDIAN_SHORT (thp->th.th_window), th_src_port, tp->tp_my_port
			);
		}
#endif	MSG
		/* Insere os dados na fila de LISTEN */

		SPINLOCK (&tp->tp_inq_lock);

		if (tp->tp_listen_qlen >= tp->tp_listen_maxqlen)
			goto listen_q_full;

		for (lp = tp->tp_listen_q, end_listen_q = lp + LISTENQ_MAXSZ; /* abaixo */; lp++)
		{
			if (lp >= end_listen_q)
			{
			    listen_q_full:
				SPINFREE (&tp->tp_inq_lock);
#ifdef	MSG
				if (ip->it_report_error)
					printf ("%g: Fila de LISTEN cheia (porta %d)\n", th_dst_port);
#endif	MSG
				goto drop_with_reset;
			}

			if (lp->tp_listen_seq == 0)
				break;
		}

		if (++tp->tp_listen_source <= 0)
			tp->tp_listen_source = 1;

		lp->tp_listen_seq     = tp->tp_listen_source;
		lp->tp_listen_addr    = bp->it_src_addr;
		lp->tp_listen_port    = th_src_port;
		lp->tp_listen_my_addr = bp->it_dst_addr;
		lp->tp_listen_irs     = bp->it_seg_seq;
		lp->tp_max_seg_sz     = max_seg_sz;
		lp->tp_window_size    = ENDIAN_SHORT (thp->th.th_window);
		lp->tp_listen_time    = time;

		tp->tp_listen_qlen++;

		do_eventdone_on_inqueue (tp);

		SPINFREE (&tp->tp_inq_lock);

		goto drop_without_reset;
	    }

		/*
		 ******	Abertura ativa *************************
		 */
	    case S_SYN_SENT:
	    {
		LISTENQ		*lp;

		if (!(bp->it_ctl & C_ACK))
			goto drop_perhaps_with_reset;

		if (bp->it_seg_ack != tp->tp_snd_iss + 1)
			goto drop_perhaps_with_reset;

		if (bp->it_ctl & C_RST)
		{
			reset_the_connection (tp, 0);
			goto drop_without_reset;
		}

		if ((bp->it_ctl & C_SYN) == 0)
			goto drop_with_reset;

	   /***	tp->tp_snd_iss = ...; ***/
		tp->tp_snd_una = bp->it_seg_ack;
	   /***	tp->tp_snd_nxt = ...; ***/
		tp->tp_snd_wnd = bp->it_seg_wnd;
	   /***	tp->tp_snd_up  = ...; ***/
	   /***	tp->tp_snd_psh = ...; ***/

		SPINLOCK (&tp->tp_inq_lock);

		tp->tp_rcv_irs = bp->it_seg_seq;
		tp->tp_rcv_usr = bp->it_seg_seq + 1;
		tp->tp_rcv_nxt = bp->it_seg_nxt;
	   	tp->tp_rcv_wnd = tp->tp_rcv_nxt + ip->it_WND_SZ;
	   	tp->tp_old_wnd = tp->tp_rcv_wnd;
		tp->tp_rcv_up  = bp->it_seg_seq;
		tp->tp_rcv_psh = bp->it_seg_seq;

		if (bp->it_ctl & C_URG)
			tp->tp_rcv_up  = bp->it_seg_up;

		tp->tp_rnd_out.tp_rnd_syn = 0;
		tp->tp_rnd_in.tp_rnd_syn  = C_SYN;

		if (bp->it_count > 0)
			circular_area_put (&tp->tp_rnd_in, bp->it_area, bp->it_count, SS);

		/* Verifica se o bloco tem a opção de tamanho máximo de segmento */

		max_seg_sz = 0;

		if (h_size >= sizeof (TCP_H) && thp->th.opt.th_opt_kind == 2 && thp->th.opt.th_opt_size == 4)
			max_seg_sz = ENDIAN_SHORT (thp->th.opt.th_max_seg_sz);
#ifdef	MSG
		if (ip->it_list_info) printf
		(
			"%g: O SYN|ACK contém max_seg_sz = %d, janela = %d (de porta %d para porta %d)\n",
			max_seg_sz, ENDIAN_SHORT (thp->th.th_window), th_src_port, tp->tp_my_port
		);
#endif	MSG
		lp = tp->tp_listen_q;

	   /***	lp->tp_listen_seq     = ...; ***/
		lp->tp_listen_addr    = bp->it_src_addr;
		lp->tp_listen_port    = th_src_port;
		lp->tp_listen_my_addr = bp->it_dst_addr;
	   /***	lp->tp_listen_irs     = ...; /* acima ***/
		lp->tp_max_seg_sz     = max_seg_sz;
		lp->tp_window_size    = ENDIAN_SHORT (thp->th.th_window);

		tp->tp_state = S_ESTABLISHED;

		do_eventdone_on_inqueue (tp);

		SPINFREE (&tp->tp_inq_lock);

		goto drop_without_reset;
	    }

		/*
		 ******	Em regime ***************************
		 */
	    case S_SYN_RECEIVED:
	    case S_ESTABLISHED:
	    case S_FIN_WAIT_1:
	    case S_FIN_WAIT_2:
	    case S_CLOSE_WAIT:
	    case S_CLOSING:
	    case S_LAST_ACK:
	    case S_TIME_WAIT:
		seg_seq = bp->it_seg_seq;
		rcv_wnd = tp->tp_rcv_wnd;
		rcv_nxt = tp->tp_rcv_nxt;

		if (bp->it_seg_len == 0)	/* Ou seja, somente um ACK */
		{
			if (rcv_nxt == rcv_wnd)	/* Janela fechada */
			{
				if (seg_seq == rcv_nxt)
					OK++;
			}
			else			/* Janela aberta */
			{
				if (TCP_GE (seg_seq, rcv_nxt) && TCP_LT (seg_seq, rcv_wnd))
					OK++;
			}
		}
		else	/* bp->it_seg_len > 0 */
		{
			if (rcv_nxt != rcv_wnd)	/* Janela aberta */
			{
				if (TCP_GE (seg_seq, rcv_nxt) && TCP_LT (seg_seq, rcv_wnd))
				{
					OK++;
				}
				else
				{
					seg_seq += bp->it_seg_len - 1;

					if (TCP_GE (seg_seq, rcv_nxt) && TCP_LT (seg_seq, rcv_wnd))
						OK++;
				}
			}
		}

		if (OK == 0)
		{
#ifdef	MSG
			if (ip->it_list_bad_seg) printf
			(
				"%g: Segmento não aceito (porta %d): %02X, seg_len = %d, "
				"seg_seq = %u, seg_ack = %u, rcv_nxt = %u, rcv_wnd = (%u, %d)\n",
				tp->tp_my_port,	bp->it_ctl, bp->it_seg_len,
				bp->it_seg_seq, bp->it_seg_ack, rcv_nxt, rcv_wnd, rcv_wnd - rcv_nxt
			);
#endif	MSG
			if ((bp->it_ctl & C_RST) == 0 && bp->it_seg_len > 0)
				send_tcp_ctl_packet (C_ACK, tp);

			goto drop_without_reset;
		}

		/*
		 *	Verifica os bits RST e SYN
		 */
		if (bp->it_ctl & (C_RST|C_SYN))
		{
			reset_the_connection (tp, 0);
			goto drop_perhaps_with_reset;
		}

		/*
		 *	Verifica o campo ACK
		 */
		if ((bp->it_ctl & C_ACK) == 0)
			goto drop_without_reset;

		/*
		 *	Verifica se recebeu o ACK do SYN
		 */
		if (tp->tp_state == S_SYN_RECEIVED)
		{
			if (TCP_LT (bp->it_seg_ack, tp->tp_snd_una) || TCP_GT (bp->it_seg_ack, tp->tp_snd_nxt))
				goto drop_with_reset;

			tp->tp_state = S_ESTABLISHED;

			inq_event++;
		}

		/*
		 *	Continua o processamento do segmento
		 */
		if (TCP_GT (bp->it_seg_ack, tp->tp_snd_nxt)) /* Ainda não enviado */
		{
#ifdef	MSG
			if (ip->it_list_bad_seg) printf
			(
				"%g: ACK não aceito (porta %d): seg_ack = %u, snd_nxt = %u, seg_wnd = %d\n",
				tp->tp_my_port, bp->it_seg_ack, tp->tp_snd_nxt, bp->it_seg_wnd - bp->it_seg_ack
			);
#endif	MSG
			if (bp->it_seg_len > 0)
				send_tcp_ctl_packet (C_ACK, tp);

			goto do_events;
		}

		/*
		 *	Remove os segmentos já recebidos
		 */
		if (TCP_GTR (bp->it_seg_ack, tp->tp_snd_una, n))
		{
			SPINLOCK (&tp->tp_rnd_out.tp_rnd_lock);

			if (tp->tp_rnd_out.tp_rnd_syn)
				{ tp->tp_rnd_out.tp_rnd_syn = 0; n--; }

			if (n > 0)
				n -= circular_area_del (&tp->tp_rnd_out, n);

			if (n > 0 && tp->tp_rnd_out.tp_rnd_fin)
				{ tp->tp_rnd_out.tp_rnd_fin = 0; n--; }

			tp->tp_snd_una = bp->it_seg_ack;

			SPINFREE (&tp->tp_rnd_out.tp_rnd_lock);

			if (n != 0)
				printf ("%g: Erro na remoção (sobraram %d bytes)\n", n);

			/* Reavalia o SRTT */

			if (tp->tp_retransmitted == 0 && tp->tp_snd_una == tp->tp_snd_nxt)
			{
				int	delta_time;

				delta_time = 1024 * (time - tp->tp_last_snd_time) + 512;

				if (tp->tp_SRTT == 0)
					tp->tp_SRTT = delta_time;
				else
					tp->tp_SRTT = ALPHA (tp->tp_SRTT) + ALPHA_C (delta_time);
#ifdef	MSG
				if (ip->it_list_SRTT)
					printf ("%g: time = %d => SRTT = %d\n", delta_time, tp->tp_SRTT);
#endif	MSG
			}

			tp->tp_last_ack_time = time;
			tp->tp_retransmitted = 0;

			outq_event++;
		}

		/*
		 *	Atualiza a janela de transmissão
		 */
		if (TCP_GT (bp->it_seg_wnd, tp->tp_snd_wnd))
		{
		   /***	SPINLOCK (&tp->tp_inq_lock); ***/

			tp->tp_snd_wnd = bp->it_seg_wnd;

		   /***	SPINFREE (&tp->tp_inq_lock); ***/

			wnd_event++;
#ifdef	MSG
			if (ip->it_list_window) printf
			(
				"%g: Alterado snd_wnd (port %d): %d\n",
				tp->tp_my_port, tp->tp_snd_wnd - tp->tp_snd_una
			);
#endif	MSG
		}
#ifdef	MSG
		elif (ip->it_list_bad_seg && bp->it_seg_wnd != tp->tp_snd_wnd)
		{
			printf
			(	"%g: ACK retrógado (port %d): tp_wnd = %u, seg_wnd = %u\n",
				tp->tp_my_port, tp->tp_snd_wnd - tp->tp_snd_una, bp->it_seg_wnd - bp->it_seg_ack
			);
		}
#endif	MSG
		/*
		 *	Verifica se chegou o ACK do FIN
		 */
		if (tp->tp_snd_una == tp->tp_snd_fin)
		{
			if   (tp->tp_state == S_FIN_WAIT_1)
			{
				tp->tp_state = S_FIN_WAIT_2;
				inq_event++;
			}
			elif (tp->tp_state == S_CLOSING)
			{
				tp->tp_state = S_TIME_WAIT;
				inq_event++;
			}
			elif (tp->tp_state == S_LAST_ACK)
			{
				inq_event++;
			}
		}

		/*
		 *	Verifica se o segmento tem alguma informação
		 */
		if (bp->it_seg_len == 0)
			goto do_events;

		/*
		 *	Verifica se os dados são contíguos
		 */
		if (TCP_LT (rcv_nxt, bp->it_seg_seq))
		{
			send_tcp_ctl_packet (C_ACK, tp);
			goto do_events;
		}

		if (n > 0)
		{
			if (n > bp->it_seg_len)
			{
#ifdef	MSG
				if (ip->it_list_bad_seg)
				{
					printf
					(	"%g: Segmento nada tem de útil (porta %d): (%u :: %u)\n",
						tp->tp_my_port, n, bp->it_seg_len
					);
				}
#endif	MSG
				send_tcp_ctl_packet (C_ACK, tp);

				goto do_events;
			}

			bp->it_area    += n;
			bp->it_count   -= n;
			bp->it_seg_len -= n;
		}

		/*
		 *	Coloca os dados na fila de entrada
		 */
		SPINLOCK (&tp->tp_inq_lock); inq_locked++;

		if (circular_area_put (&tp->tp_rnd_in, bp->it_area, bp->it_count, SS) != bp->it_count)
		{
#ifdef	MSG
			printf ("%g: Erro na inserção do segmento (porta %d)\n", tp->tp_my_port);
#endif	MSG
		}

		if (bp->it_ctl & C_FIN)
			tp->tp_rnd_in.tp_rnd_fin = 1;

		tp->tp_rcv_nxt += bp->it_seg_len;

		if (bp->it_ctl & C_URG)
			tp->tp_rcv_up  = bp->it_seg_up;

		if (bp->it_ctl & C_PSH)
			tp->tp_rcv_psh = bp->it_seg_nxt;

		if (bp->it_seg_len > 0)
			send_ack++;

		inq_event++;

		/*
		 *	Executa os eventos pendentes
		 */
	    do_events:
		if (outq_event)
			EVENTDONE (&tp->tp_rnd_out.tp_rnd_nfull);

		if (wnd_event)
			EVENTDONE (&tp->tp_good_snd_wnd);

		if (inq_event)
			do_eventdone_on_inqueue (tp);

		if (inq_locked)
			SPINFREE (&tp->tp_inq_lock);

		/*
		 *	Manda um ACK imediatamente (com janela pequena)
		 */
		if (send_ack)
		{
			send_tcp_ctl_packet (C_ACK, tp);
#ifdef	MSG
			if (ip->it_list_window)
			{
				printf
				(	"%g: (porta %d): Mandei ACK com rcv_wnd = %d\n",
					tp->tp_my_port, tp->tp_rcv_wnd - tp->tp_rcv_nxt
				);
			}
#endif	MSG
		}

		goto drop_without_reset;

		/*
		 *	Estado desconhecido
		 */
	    default:
#ifdef	MSG
		printf ("%g: Estado inválido (porta %d): %d\n", tp->tp_my_port, tp->tp_state);
#endif	MSG
		break;

	}	/* end switch */

	/*
	 ******	Descarta o segmento, mas enviando ou não um RESET **********
	 */
    drop_perhaps_with_reset:
	if (bp->it_ctl & C_RST)
		goto drop_without_reset;

	/*
	 *	Descarta o segmento, mas envia um RESET
	 */
    drop_with_reset:
	if (bp->it_ctl & C_ACK)
	{
		send_tcp_rst_packet
		(
			C_RST,
			bp->it_src_addr,			/* dst_addr */
			th_src_port,				/* dst_port */
			bp->it_dst_addr,			/* my_addr */
			th_dst_port,				/* my_port */
			bp->it_seg_ack,				/* seq */
			0					/* ack */
		);
	}
	else	/* C_ACK off */
	{
		send_tcp_rst_packet
		(
			C_RST|C_ACK,
			bp->it_src_addr,			/* dst_addr */
			th_src_port,				/* dst_port */
			bp->it_dst_addr,			/* my_addr */
			th_dst_port,				/* my_port */
			0,					/* seq */
			bp->it_seg_nxt				/* ack */
		);
	}

	/*
	 *	Descarta o segmento
	 */
    drop_without_reset:
	put_it_block (bp);

}	/* end receive_tcp_packet */

/*
 ****************************************************************
 *	Acorda o processo esperando por entrada			*
 ****************************************************************
 */
void
do_eventdone_on_inqueue (TCP_EP *tp)
{
#ifdef	MSG
	if (itscb.it_list_input)
	{
		printf
		(	"%g: Dando eventdone na porta %d com %d bytes%s\n",
			tp->tp_my_port, tp->tp_rnd_in.tp_rnd_count,
			tp->tp_rnd_in.tp_rnd_fin ? " e FIN" : ""
		);
	}
#endif	MSG

	EVENTDONE (&tp->tp_inq_nempty);

	if (tp->tp_attention_set && tp->tp_uproc->u_attention_index < 0)
	{
		tp->tp_attention_set = 0;
		tp->tp_uproc->u_attention_index = tp->tp_fd_index;
		EVENTDONE (&tp->tp_uproc->u_attention_event);
	}

}	/* end do_eventdone_on_inqueue */

/*
 ****************************************************************
 *	Fecha a conexão						*
 ****************************************************************
 */
void
reset_the_connection (TCP_EP *tp, int send_reset)
{
	/*
	 *	Envia o "RST"
	 */
	if (send_reset) send_tcp_rst_packet
	(
		C_RST,
		tp->tp_dst_addr,	/* dst_addr */
		tp->tp_dst_port,	/* dst_port */
		tp->tp_my_addr,		/* my_addr */
		tp->tp_my_port,		/* my_port */
		tp->tp_snd_nxt,		/* seq */
		tp->tp_rcv_nxt		/* ack */
	);

	tp->tp_rst = 1;

	tp->tp_state   = S_BOUND;
	tp->tp_my_port = 0;

	EVENTDONE (&tp->tp_rnd_out.tp_rnd_nfull);

	EVENTDONE (&tp->tp_good_snd_wnd);

	do_eventdone_on_inqueue (tp);

}	/* end reset_the_connection */
