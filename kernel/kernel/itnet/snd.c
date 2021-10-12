/*
 ****************************************************************
 *								*
 *			snd.c					*
 *								*
 *	Funções relacionadas com a saída do protocolo TCP	*
 *								*
 *	Versão	3.0.0, de 17.08.91				*
 *		4.4.0, de 30.12.02				*
 *								*
 *	Funções:						*
 *		send_tcp_ctl_packet,  send_tcp_rst_packet	*
 *		send_tcp_data_packet				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 * 		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include "../h/common.h"
#include "../h/scb.h"
#include "../h/sync.h"
#include "../h/region.h"

#include "../h/inode.h"
#include "../h/uerror.h"
#include "../h/signal.h"
#include "../h/uproc.h"
#include "../h/itnet.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Envia um pacote de contrôle TCP				*
 ****************************************************************
 */
void
send_tcp_ctl_packet (int type, register TCP_EP *tp)
{
	register const ITSCB	*ip = &itscb;
	register ITBLOCK	*bp;
	register TT_H		*thp;
	int			tcp_h_size;

	/*
	 *	Obtém o dispositivo destino e endereço fonte
	 */
	if   (type & (C_SYN|C_FIN))
	{
		bp = get_it_block (IT_OUT_DATA);
	}
	elif ((bp = get_it_block (IT_OUT_CTL)) == NOITBLOCK)
	{
#ifdef	MSG
		printf ("%g: NÃO obtive bloco\n");
#endif	MSG
		return;
	}

	bp->it_src_addr	= tp->tp_my_addr;
	bp->it_dst_addr	= tp->tp_dst_addr;
	bp->it_route	= tp->tp_route;

	/*
	 *	Prepara o ponteiro para os cabeçalhos reunidos
	 */
	thp = (TT_H *)(bp->it_frame + RESSZ);

	/*
	 *	Decide o tamanho do cabeçalho TCP
	 */
	if (type & C_SYN)
		tcp_h_size = sizeof (TCP_H);
	else
		tcp_h_size = sizeof (TCP_H) - sizeof (thp->th.opt);

	/*
	 *	Prepara o pseudo-cabeçalho TCP
	 */
	thp->ph.ph_src_addr = ENDIAN_LONG (tp->tp_my_addr);
	thp->ph.ph_dst_addr = ENDIAN_LONG (tp->tp_dst_addr);
	thp->ph.ph_proto    = ENDIAN_SHORT (TCP_PROTO);
	thp->ph.ph_size     = ENDIAN_SHORT (tcp_h_size);

	/*
	 *	Começa a preencher o cabeçalho TCP
	 */
	memclr (&thp->th, tcp_h_size);		/* Zera toda a entrada */

	thp->th.th_src_port = ENDIAN_SHORT (tp->tp_my_port);
	thp->th.th_dst_port = ENDIAN_SHORT (tp->tp_dst_port);

	thp->th.th_ctl = type;

	/*
	 *	Coloca o no. de seqüência
	 */
	if (type & C_RETRANS && type & (C_SYN|C_FIN))
	{
		if   (type & C_SYN)
			thp->th.th_seq_no = ENDIAN_LONG  (tp->tp_snd_iss);
		else	/* C_FIN */
			thp->th.th_seq_no = ENDIAN_LONG  (tp->tp_snd_nxt - 1);
	}
	else
	{
		thp->th.th_seq_no = ENDIAN_LONG (tp->tp_snd_nxt);
	}

	/*
	 *	Se tiver SYN, coloca a opção do tamanho máximo de segmento
	 */
	if (type & C_SYN)
	{
		thp->th.opt.th_opt_kind = 2;
		thp->th.opt.th_opt_size = 4;
		thp->th.opt.th_max_seg_sz = ENDIAN_SHORT (ip->it_MAX_SGSZ);
#ifdef	MSG
		if (ip->it_list_info)
			printf ("%g: enviando max_seg_sz = %d\n", ip->it_MAX_SGSZ);
#endif	MSG
	}

	/*
	 *	Se tiver ACK, guarda o último ACK enviado
	 */
	SPINLOCK (&tp->tp_inq_lock);

	if (type & C_ACK)
		tp->tp_old_wnd = tp->tp_rcv_wnd;

	thp->th.th_window = ENDIAN_SHORT (tp->tp_rcv_wnd - tp->tp_rcv_nxt);
   	thp->th.th_ack_no = ENDIAN_LONG  (tp->tp_rcv_nxt);

	SPINFREE (&tp->tp_inq_lock);

	/*
	 *	Coloca o "checksum"
	 */
	thp->th.th_h_size = tcp_h_size >> 2;
   /***	thp->th.th_checksum = 0; ***/
	thp->th.th_checksum = compute_checksum (thp, sizeof (PSD_H) + tcp_h_size);

	/*
	 *	Transmite
	 */
   /***	bp->it_ctl = thp->th.th_ctl; ***/
	bp->it_n_transmitted = 0;
	bp->it_free_after_IO = 1;
	bp->it_wait_for_arp = 1;

	bp->it_u_area  = &thp->th;
	bp->it_u_count = tcp_h_size;

	if (type & (C_SYN|C_FIN) && (type & C_RETRANS) == 0)
		tp->tp_snd_nxt += 1;

	send_ip_datagram (TCP_PROTO, bp);

}	/* end send_tcp_ctl_packet */

/*
 ****************************************************************
 *	Envia um pacote de contrôle RST TCP			*
 ****************************************************************
 */
void
send_tcp_rst_packet (int type, IPADDR dst_addr, int dst_port, IPADDR my_addr, int my_port, seq_t seq, seq_t ack)
{
	register ROUTE		*rp;
	register ITBLOCK 	*bp;
	register TT_H		*thp;

	/*
	 *	Obtém o dispositivo destino e endereço fonte
	 */
	if ((rp = get_route_entry (dst_addr)) == NOROUTE)
		{ u.u_error = TNOROUTE; return; }

	if ((bp = get_it_block (IT_OUT_CTL)) == NOITBLOCK)
	{
#ifdef	MSG
		printf ("%g: NÃO obtive bloco\n");
#endif	MSG
		return;
	}

	if ((bp->it_src_addr = my_addr) == 0)
		bp->it_src_addr = rp->r_my_addr;

	bp->it_dst_addr	= dst_addr;
	bp->it_route	= rp;

	/*
	 *	Prepara o ponteiro para os cabeçalhos reunidos
	 */
	thp = (TT_H *)(bp->it_frame + RESSZ);

	/*
	 *	Prepara o pseudo-cabeçalho TCP
	 */
	thp->ph.ph_src_addr = ENDIAN_LONG (rp->r_my_addr);
	thp->ph.ph_dst_addr = ENDIAN_LONG (dst_addr);
	thp->ph.ph_proto    = ENDIAN_SHORT (TCP_PROTO);
	thp->ph.ph_size     = ENDIAN_SHORT (MIN_TCP_H_SZ);

	/*
	 *	Preenche o cabeçalho TCP
	 */
	memclr (&thp->th, MIN_TCP_H_SZ);		/* Zera toda a entrada */

	thp->th.th_src_port = ENDIAN_SHORT (my_port);
	thp->th.th_dst_port = ENDIAN_SHORT (dst_port);

	thp->th.th_ctl = type | C_RST;

	thp->th.th_seq_no = ENDIAN_LONG (seq);
   	thp->th.th_ack_no = ENDIAN_LONG (ack);

	/*
	 *	Envia a mensagem
	 */
	thp->th.th_h_size = MIN_TCP_H_SZ >> 2;
   /***	thp->th.th_checksum = 0; ***/
	thp->th.th_checksum = compute_checksum (thp, sizeof (PSD_H) + MIN_TCP_H_SZ);

	/*
	 *    Transmite
	 */
   /***	bp->it_ctl = thp->th.th_ctl; ***/
	bp->it_n_transmitted = 0;
	bp->it_free_after_IO = 1;
	bp->it_wait_for_arp = 1;

	bp->it_u_area  = &thp->th;
	bp->it_u_count = MIN_TCP_H_SZ;

	send_ip_datagram (TCP_PROTO, bp);

}	/* end send_tcp_rst_packet */

/*
 ****************************************************************
 *	Envia um pacote de dados TCP				*
 ****************************************************************
 */
void
queue_tcp_data_packet (IOREQ *iop, int type, register TCP_EP *tp)
{
	int		n, wnd_count, count;
	char		flush = 0;
	seq_t		end_seq;

	/*
	 *	Atualiza o URGENT e PUSH
	 */
	SPINLOCK (&tp->tp_rnd_out.tp_rnd_lock);

	end_seq = tp->tp_snd_una + tp->tp_rnd_out.tp_rnd_count + iop->io_count;

	if (type & C_URG)
		tp->tp_snd_up = end_seq;

	if (type & C_PSH || iop->io_count == 0)
		{ tp->tp_snd_psh = end_seq; flush++; }

	/*
	 *	Espera até haver espaço na fila circular
	 */
    again:
	if ((count = iop->io_count) > tp->tp_max_seg_sz)
		count = tp->tp_max_seg_sz;

	for (EVER)
	{
		if (tp->tp_rnd_out.tp_rnd_count + count <= tp->tp_rnd_out.tp_rnd_sz)
			break;

		EVENTCLEAR (&tp->tp_rnd_out.tp_rnd_nfull);

		SPINFREE (&tp->tp_rnd_out.tp_rnd_lock);

		EVENTWAIT (&tp->tp_rnd_out.tp_rnd_nfull, PITNETOUT);

		if (tp->tp_rst)
			{ u.u_error = TLOOK; return; }

		SPINLOCK (&tp->tp_rnd_out.tp_rnd_lock);
	}

	if ((count = tp->tp_rnd_out.tp_rnd_sz - tp->tp_rnd_out.tp_rnd_count) > iop->io_count)
		count = iop->io_count;

	/*
	 *	Insere os dados na fila circular
	 */
	if (circular_area_put (&tp->tp_rnd_out, iop->io_area, count, US) != count)
		{ SPINFREE (&tp->tp_rnd_out.tp_rnd_lock); return; }

	iop->io_area  += count;
	iop->io_count -= count;

	/*
	 *	Verifica se já tem uma quantidade boa para transmitir
	 */
    tst_rnd:
	count = tp->tp_snd_una + tp->tp_rnd_out.tp_rnd_count - tp->tp_snd_nxt;

	if   (count < tp->tp_max_seg_sz)
	{
		if (iop->io_count > 0)
			goto again;

		if (count <= 0 || !flush)
			{ SPINFREE (&tp->tp_rnd_out.tp_rnd_lock); return; }
	}
	elif (count == tp->tp_max_seg_sz)	/* Para o caso de coincidir */
	{
		if (iop->io_count <= 0)
			tp->tp_snd_psh = end_seq;
	}
	else
	{
		count = tp->tp_max_seg_sz;
	}

	/*
	 *	Espera a janela de transmissão ter espaço
	 */
	if ((wnd_count = tp->tp_good_wnd) < count)
		wnd_count = count;

	for (EVER)
	{
		if (TCP_LE (tp->tp_snd_nxt + wnd_count, tp->tp_snd_wnd))
			break;

		tp->tp_closed_wnd_time = time;	/* A janela fechou */

		EVENTCLEAR (&tp->tp_good_snd_wnd);

		SPINFREE (&tp->tp_rnd_out.tp_rnd_lock);
#ifdef	MSG
		if (itscb.it_list_window)
		{
			printf
			(	"%g: Vou esperar GOOD_WND (no momento %d)\n",
				tp->tp_snd_wnd - tp->tp_snd_nxt
			);
		}
#endif	MSG
		EVENTWAIT (&tp->tp_good_snd_wnd, PITNETOUT);

		if (tp->tp_rst)
			{ u.u_error = TLOOK; return; }

		SPINLOCK (&tp->tp_rnd_out.tp_rnd_lock);
	}

	tp->tp_closed_wnd_time = 0;	/* A janela abriu */

	/*
	 *	Envia esta parte dos dados
	 */

	SPINFREE (&tp->tp_rnd_out.tp_rnd_lock);

	send_tcp_data_packet (tp, tp->tp_snd_nxt, count, 0 /* 1a. trans */);

	tp->tp_last_snd_time = time;

	SPINLOCK (&tp->tp_rnd_out.tp_rnd_lock);

	goto tst_rnd;

}	/* end queue_tcp_data_packet */

/*
 ****************************************************************
 *	Envia um pacote de dados TCP				*
 ****************************************************************
 */
void
send_tcp_data_packet (register TCP_EP *tp, seq_t seq, int count, int flags)
{
	register ITBLOCK	*bp;
	register TT_H		*thp;
	char			*area;
	int			offset, i, total_size;

	/*
	 *	Cuidado: Não pode ser chamada com count == 0
	 *
	 *	Obtém um bloco para a saída e insere os dados
	 */
	bp = get_it_block (IT_OUT_DATA); area = bp->it_frame + RESSZ;

	SPINLOCK (&tp->tp_rnd_out.tp_rnd_lock);

	if   (flags & C_TST_BYTE)
	{
		area[0] = 0;
	}
	else
	{
		if ((offset = seq - tp->tp_snd_una) < 0)
		{
#ifdef	MSG
			if (!(flags & C_RETRANS))
				printf ("%g: Área inválida, offset = %d\n", offset);
#endif	MSG
			if ((count += offset) <= 0)
			{
				SPINFREE (&tp->tp_rnd_out.tp_rnd_lock);
				put_it_block (bp);
				return;
			}

			offset = 0;
		}

		if ((i = circular_area_read (&tp->tp_rnd_out, area, count, offset)) != count)
		{
#ifdef	MSG
			printf ("%g: Cópia insuficiente de caracteres: %d :: %d (%2X)\n", i, count, flags);
#endif	MSG
		}
	}

	SPINFREE (&tp->tp_rnd_out.tp_rnd_lock);

	/*
	 *	Prepara o pseudo-cabeçalho TCP
	 */
	thp = (TT_H *)(area - MIN_TCP_H_SZ - sizeof (PSD_H));

	thp->ph.ph_src_addr = ENDIAN_LONG (tp->tp_my_addr);
	thp->ph.ph_dst_addr = ENDIAN_LONG (tp->tp_dst_addr);
	thp->ph.ph_proto    = ENDIAN_SHORT (TCP_PROTO);
	thp->ph.ph_size     = ENDIAN_SHORT (total_size = MIN_TCP_H_SZ + count);

	/*
	 *	Preenche o cabeçalho TCP
	 */
	memclr (&thp->th, MIN_TCP_H_SZ);		/* Zera toda a entrada */

	thp->th.th_src_port = ENDIAN_SHORT (tp->tp_my_port);
	thp->th.th_dst_port = ENDIAN_SHORT (tp->tp_dst_port);

	thp->th.th_h_size = MIN_TCP_H_SZ >> 2;

	thp->th.th_ctl = C_ACK;

	/*
	 *	Não esquece de guardar o último ACK enviado
	 */
	SPINLOCK (&tp->tp_rnd_in.tp_rnd_lock);

	tp->tp_old_wnd = tp->tp_rcv_wnd;

	thp->th.th_seq_no = ENDIAN_LONG  (seq);
	thp->th.th_window = ENDIAN_SHORT (tp->tp_rcv_wnd - tp->tp_rcv_nxt);
   	thp->th.th_ack_no = ENDIAN_LONG  (tp->tp_rcv_nxt);

	SPINFREE (&tp->tp_rnd_in.tp_rnd_lock);

	if ((i = tp->tp_snd_psh - seq) > 0)
		thp->th.th_ctl |= C_PSH;

	if ((i = tp->tp_snd_up - seq) > 0)
	{
		if (i > count)
			i = count;

		thp->th.th_ctl |= C_URG;
		thp->th.th_urg_ptr = ENDIAN_SHORT (i);
	}

	/*
	 *	Calcula o "checksum"
	 */
   /***	thp->th.th_checksum = 0; ***/
	thp->th.th_checksum = compute_checksum (thp, sizeof (PSD_H) + total_size);

	/*
	 *	Envia a mensagem
	 */
	bp->it_free_after_IO = 1;
	bp->it_wait_for_arp = 1;

	bp->it_src_addr	= tp->tp_my_addr;
	bp->it_dst_addr	= tp->tp_dst_addr;
	bp->it_route	= tp->tp_route;

	bp->it_u_area  = &thp->th;
	bp->it_u_count = total_size;

	if ((flags & C_RETRANS) == 0)
		tp->tp_snd_nxt += count;
#ifdef	MSG
	if (itscb.it_list_output)
	{
		printf
		(	"%g: count = %d, %s, snd_psh = %P, seq = %P\n",
			count, thp->th.th_ctl & C_PSH ? "PSH" : "",
			tp->tp_snd_psh, seq
		);
	}
#endif	MSG

	send_ip_datagram (TCP_PROTO, bp);

}	/* end send_tcp_data_packet */
