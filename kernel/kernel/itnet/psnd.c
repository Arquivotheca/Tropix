/*
 ****************************************************************
 *								*
 *			psnd.c					*
 *								*
 *	Funções relacionadas com a saída do protocolo TCP	*
 *								*
 *	Versão	3.1.00, de 14.07.98				*
 *		3.1.00, de 14.07.98				*
 *								*
 *	Funções:						*
 *		send_tcp_ctl_packet,  send_tcp_rst_packet	*
 *		send_tcp_data_packet				*
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
#include "../h/region.h"

#include "../h/kfile.h"
#include "../h/inode.h"
#include "../h/uerror.h"
#include "../h/signal.h"
#include "../h/uproc.h"
#include "../h/itnet.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Envia um pacote de dados TCP				*
 ****************************************************************
 */
void
pipe_tcp_data_packet (IOREQ *iop, int type, register TCP_EP *tp)
{
	register TCP_EP		*dst_tp = tp->tp_pipe_tcp_ep;
	int			n, count;
	char			push;
	seq_t			end_seq;

	/*
	 *	Atualiza o URGENT e PUSH
	 */
	SPINLOCK (&tp->tp_rnd_out.tp_rnd_lock);

	end_seq = tp->tp_snd_nxt + iop->io_count;

	if (type & C_URG)
		tp->tp_snd_up  = end_seq;

	if (type & C_PSH || iop->io_count == 0)
		tp->tp_snd_psh = end_seq;

	SPINFREE (&tp->tp_rnd_out.tp_rnd_lock);

	/*
	 ******	Espera até haver espaço na fila circular *********
	 */
    again:
	SPINLOCK (&dst_tp->tp_inq_lock);

	if ((count = iop->io_count) > tp->tp_max_seg_sz)
		count = tp->tp_max_seg_sz;

	for (EVER)
	{
		if (dst_tp->tp_rnd_in.tp_rnd_count + count <= dst_tp->tp_rnd_in.tp_rnd_sz)
			break;

		EVENTCLEAR (&dst_tp->tp_rnd_in.tp_rnd_nfull);

		SPINFREE (&dst_tp->tp_inq_lock);

		EVENTWAIT (&dst_tp->tp_rnd_in.tp_rnd_nfull, PITNETOUT);

		if (tp->tp_rst)
			{ u.u_error = TLOOK; return; }

		SPINLOCK (&dst_tp->tp_inq_lock);
	}

	if ((count = dst_tp->tp_rnd_in.tp_rnd_sz - dst_tp->tp_rnd_in.tp_rnd_count) > iop->io_count)
		count = iop->io_count;

	/*
	 *	Insere os dados diretamente na fila circular do destino
	 */
	if (circular_area_put (&dst_tp->tp_rnd_in, iop->io_area, count, US) != count)
		{ SPINFREE (&dst_tp->tp_inq_lock); return; }

	tp->tp_snd_nxt += count;
	tp->tp_snd_una += count;
	tp->tp_snd_wnd += count;

	dst_tp->tp_rcv_nxt += count;

	tp->tp_last_snd_time	 = time;
	tp->tp_last_rcv_time	 = time;
	dst_tp->tp_last_rcv_time = time;

	iop->io_area  += count;
	iop->io_count -= count;

	/*
	 *	Processa o URG e PSH
	 */
	if (TCP_LT (dst_tp->tp_rcv_up, tp->tp_snd_up))
		dst_tp->tp_rcv_up = tp->tp_snd_up;

	if (TCP_LT (dst_tp->tp_rcv_psh, tp->tp_snd_psh))
		dst_tp->tp_rcv_psh = tp->tp_snd_psh;

	count = dst_tp->tp_rnd_in.tp_rnd_count;

#if (0)	/*******************************************************/
	if (count >= dst_tp->tp_rnd_in.tp_rnd_sz)
#endif	/*******************************************************/
		dst_tp->tp_rcv_psh = dst_tp->tp_rcv_nxt;

	if (TCP_GT (dst_tp->tp_rcv_psh, dst_tp->tp_rcv_usr))
		push = 1;
	else
		push = 0;

	if (count > 0 && push)
		do_eventdone_on_inqueue (dst_tp);

	SPINFREE (&dst_tp->tp_inq_lock);

	/*
	 *	Verifica se acabou
	 */
	if (iop->io_count > 0)
		goto again;

}	/* end pipe_tcp_data_packet */
