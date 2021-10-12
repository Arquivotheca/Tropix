/*
 ****************************************************************
 *								*
 *			kinfo.c					*
 *								*
 *	Funções auxiliares do INTERNET				*
 *								*
 *	Versão	3.0.0, de 12.11.91				*
 *		4.9.0, de 22.08.06				*
 *								*
 *	Funções:						*
 *		k_getinfo,	k_getstate,  	k_look,		*
 *		k_strevent, 	k_optmgmt,	k_rcvuderr,	*
 *		k_sync,		k_push,		k_nread,	*
 *		k_attention,	k_unattention,	k_getaddr	*
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
#include "../h/inode.h"
#include "../h/uerror.h"
#include "../h/signal.h"
#include "../h/uproc.h"
#include "../h/xti.h"
#include "../h/itnet.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */

/*
 ******	Conversão do estado interno para o estado XTI ***********
 */
const char	state_map[] =
{
	/* S_NULO  */		T_UNINIT,
	/* S_CLOSED  */		T_UNINIT,
	/* S_UNBOUND */		T_UNBND,
	/* S_BOUND */		T_IDLE,
	/* S_LISTEN */		T_INCON,
	/* S_SYN_SENT */	T_OUTCON,
	/* S_SYN_RECEIVED */	T_INCON,
	/* S_ESTABLISHED */	T_DATAXFER,
	/* S_FIN_WAIT_1 */	T_OUTREL,
	/* S_FIN_WAIT_2 */	T_OUTREL,
	/* S_CLOSE_WAIT */	T_INREL,
	/* S_CLOSING */		T_OUTREL,
	/* S_LAST_ACK */	T_INREL,
	/* S_TIME_WAIT */	T_OUTREL
};

/*
 ******	Nomes dos estados ***************************************
 */
const char	state_nm[][16] =
{
	/* S_NULO  */		"NULL",
	/* S_CLOSED  */		"CLOSED",
	/* S_UNBOUND */		"UNBOUND",
	/* S_BOUND */		"BOUND",
	/* S_LISTEN */		"LISTEN",
	/* S_SYN_SENT */	"SYN_SENT",
	/* S_SYN_RECEIVED */	"SYN_RECEIVED",
	/* S_ESTABLISHED */	"ESTABLISHED",
	/* S_FIN_WAIT_1 */	"FIN_WAIT_1",
	/* S_FIN_WAIT_2 */	"FIN_WAIT_2",
	/* S_CLOSE_WAIT */	"CLOSE_WAIT",
	/* S_CLOSING */		"CLOSING",
	/* S_LAST_ACK */	"LAST_ACK",
	/* S_TIME_WAIT */	"TIME_WAIT"
};

/*
 ******	Tabela de eventos ***************************************
 */
typedef struct
{
	int	e_code;		/* Código numérico do evento */
	char	*e_str;		/* Descrição do evento */

}	EVENTTB;

const EVENTTB	eventtb[] =
{
   	0,
	"Não ocorreu nenhum evento",

   	T_LISTEN,	/*** "T_LISTEN: " ***/
	"Foi recebido um pedido de conexão",

	T_CONNECT,	/*** "T_CONNECT: " ***/
	"Foi recebida a confirmação de um pedido de conexão",

	T_DATA,		/*** "T_DATA: " ***/
	"Foram recebidos dados normais",

	T_EXDATA,	/*** "T_EXDATA: " ***/
	"Foram recebidos dados urgentes",

	T_DISCONNECT,	/*** "T_DISCONNECT: " ***/
	"Foi recebida uma indicação de desconexão abortiva",

	T_UDERR,	/*** "T_UDERR: " ***/
	"Ocorreu um erro no envio de um datagrama anterior",

	T_ORDREL,	/*** "T_ORDREL: " ***/
	"Foi recebida uma indicação de desconexão ordenada",

	T_GODATA,	/*** "T_GODATA: " ***/
	"É possível enviar dados normais novamente",

	T_GOEXDATA,	/*** "T_GOEXDATA: " ***/
	"É possível enviar dados urgentes novamente",

	T_TIMEOUT,	/*** "T_TIMEOUT: " ***/
	"Tempo de espera esgotado",

	0
};

/*
 ****************************************************************
 *	Obtém informações dependentes do protocolo		*
 ****************************************************************
 */
void
k_getinfo (int minor, T_INFO *infop)
{
	T_INFO		info;

	if (infop == (T_INFO *)NULL)
		{ u.u_error = EFAULT; return; }

	switch (minor)
	{
		/*
		 *	RAW
		 */
	    case RAW:
		info.addr	= sizeof (INADDR);
		info.options	= -2;
		info.tsdu	= DGSZ - MIN_IP_H_SZ;
		info.etsdu	= -2;
		info.connect	= -2;
		info.discon	= -2;
		info.servtype	= 0;

		break;

		/*
		 *	UDP
		 */
	    case UDP:
		info.addr	= sizeof (INADDR);
		info.options	= -2;
		info.tsdu	= DGSZ - MIN_IP_H_SZ - sizeof (UDP_H);
		info.etsdu	= -2;
		info.connect	= -2;
		info.discon	= -2;
		info.servtype	= T_CLTS;

		break;

		/*
		 *	TCP
		 */
	    case TCP:
		info.addr	= sizeof (INADDR);
		info.options	= -2;
		info.tsdu	= 0;
		info.etsdu	= -1;
		info.connect	= -2;
		info.discon	= -2;
		info.servtype	= T_COTS_ORD;

		break;

		/*
		 *	Protocolo desconhecido
		 */
	    default:
		u.u_error = ENXIO;
		return;

	}	/* end switch */

	/*
	 *	Copia a estrutura para o usuário
	 */
	if (unimove (infop, &info, sizeof (T_INFO), SU) < 0)
		{ u.u_error = EFAULT; return; }

}	/* end k_getinfo */

/*
 ****************************************************************
 *	Obtém o estado do EP					*
 ****************************************************************
 */
int
k_getstate (int minor)
{
	KFILE		*fp = u.u_fileptr;

	switch (minor)
	{
		/*
		 *	RAW
		 */
	    case RAW:
	    {
		RAW_EP		*rp;

		if (fp->f_union != KF_ITNET)
			{ u.u_error = EBADF; return (-1); }

		rp = fp->f_endpoint;

		return (state_map[rp->rp_state]);
	    }

		/*
		 *	UDP
		 */
	    case UDP:
	    {
		UDP_EP		*up;

		if (fp->f_union != KF_ITNET)
			{ u.u_error = EBADF; return (-1); }

		up = fp->f_endpoint;

		return (state_map[up->up_state]);
	    }

		/*
		 *	TCP
		 */
	    case TCP:
	    {
		TCP_EP		*tp;

		if (fp->f_union != KF_ITNET)
			{ u.u_error = EBADF; return (-1); }

		tp = fp->f_endpoint;

		return (state_map[tp->tp_state]);
	    }

		/*
		 *	Protocolos desconhecidos
		 */
	    default:
		{ u.u_error = ENXIO; return (-1); }

	}	/* end switch */

}	/* end k_getstate */

/*
 ****************************************************************
 *	Obtém o nome simbólico de um estado			*
 ****************************************************************
 */
const char *
get_state_nm (S_STATE state)
{
	if (state < 0 || state > S_TIME_WAIT)
		return ("NOSTATE");
	else
		return (state_nm[state]);

}	/* end get_state_nm */

/*
 ****************************************************************
 *	Obtém os eventos ocorridos				*
 ****************************************************************
 */
int
k_look (int minor)
{
	ITBLOCK		*bp;
	KFILE		*fp = u.u_fileptr;
	int		event = 0;

	switch (minor)
	{
		/*
		 *	Protocolo RAW
		 */
	    case RAW:
	    {
		RAW_EP		*rp;

		if (fp->f_union != KF_ITNET)
			{ u.u_error = EBADF; return (-1); }

		rp = fp->f_endpoint;

		if ((bp = rp->rp_inq_first) != NOITBLOCK)
			return (T_DATA);

		return (0);
	    }

		/*
		 *	Protocolo UDP
		 */
	    case UDP:
	    {
		UDP_EP		*up;

		if (fp->f_union != KF_ITNET)
			{ u.u_error = EBADF; return (-1); }

		up = fp->f_endpoint;

		if ((bp = up->up_inq_first) != NOITBLOCK)
			return (T_DATA);

		return (0);
	     }

		/*
		 *	Protocolo TCP
		 */
	    case TCP:
	    {
		TCP_EP		*tp;
		const LISTENQ	*lp, *end_listen_q;
		int		n;

		if (fp->f_union != KF_ITNET)
			{ u.u_error = EBADF; return (-1); }

		tp = fp->f_endpoint;

		if (tp->tp_timeout)	/* Foi excedido o tempo */
			return (T_TIMEOUT);
	
		if (tp->tp_rst)		/* Chegou um segmento com RST */
			return (T_DISCONNECT);
	
		/*
		 *	Eventos associados à fila de entrada
		 */
		SPINLOCK (&tp->tp_inq_lock);

		switch (tp->tp_state)
		{
		    case S_BOUND:
		    case S_LISTEN:
		    case S_SYN_SENT:
			for (lp = tp->tp_listen_q, end_listen_q = lp + LISTENQ_MAXSZ; lp < end_listen_q; lp++)
			{
				if (lp->tp_listen_seq > 0)
				{
					if (tp->tp_state == S_SYN_SENT)
						{ event = T_CONNECT; break; }
					else
						{ event = T_LISTEN; break; }
				}

			}	/* end for (lista de listen) */

			if (event)
				break;

			/* continua */

		    default:
			if (tp->tp_rnd_in.tp_rnd_count > 0)
			{
				if (TCP_GE (tp->tp_rcv_up, tp->tp_rcv_usr))
					{ event =  T_EXDATA; break; }
				else
					{ event =  T_DATA; break; }
			}

			if (tp->tp_rnd_in.tp_rnd_fin)
				{ event =  T_ORDREL; break; }

		}	/* end switch (state) */

		SPINFREE (&tp->tp_inq_lock);
	
		if (event)
			return (event);

		/*
		 *	Eventos de saída
		 */
		if (tp->tp_godata)	/* Possível enviar dados */
			{ tp->tp_godata = 0; return (T_GODATA); }
	
		if (tp->tp_gourgdata)	/* Possível enviar dados urgentes */
			{ tp->tp_gourgdata = 0; return (T_GOEXDATA); }

		/*
		 *	Não há evento algum
		 */
		return (0);
    	    }

		/*
		 *	Protocolos desconhecidos
		 */
	    default:
		{ u.u_error = ENXIO; return (-1); }

	}	/* end switch */

}	/* end k_look */

/*
 ****************************************************************
 *	Fornece o texto de um evento				*
 ****************************************************************
 */
int
k_strevent (int event, char *area)
{
	const EVENTTB	*ep;
	const char	*cp;

	for (ep = eventtb; /* abaixo */; ep++)
	{
		if (ep->e_str == NOSTR)
			{ cp = "Evento desconhecido"; break; }

		if (ep->e_code == event)
			{ cp = ep->e_str; break; }
	}

	if (unimove (area, cp, 80, SU) < 0)
		{ u.u_error = EFAULT; return (-1); }

	return (0);

}	/* end k_strevent */

/*
 ****************************************************************
 *	Gerencia as opções					*
 ****************************************************************
 */
void
k_optmgmt (int minor, const T_OPTMGMT *req, T_OPTMGMT *ret)
{
	KFILE		*fp = u.u_fileptr;
	TCP_EP		*tp;
	int		i;
	const ITSCB	*ip = &itscb;
	T_OPTMGMT	optmgmt;
	TCP_OPTIONS	tcp_options;
	long		req_flags, ret_flags = T_SUCCESS;

	/*
	 *	Prólogo: analisa o EP
	 */
	if (minor != TCP)
		{ u.u_error = ENXIO; return; }

	if (fp->f_union != KF_ITNET)
		{ u.u_error = EBADF; return; }

	tp = fp->f_endpoint;

	if (tp->tp_state != S_BOUND)
		{ u.u_error = TOUTSTATE; return; }

	/*
	 *	Le a estrutura T_OPTMGMT
	 */
	if (unimove (&optmgmt, req, sizeof (T_OPTMGMT), US) < 0)
		{ u.u_error = EFAULT; return; }

	/*
	 *	Le a estrutura TCP_OPTIONS do usuário
	 */
	switch (req_flags = optmgmt.flags)
	{
	    case T_NEGOTIATE:
	    case T_CHECK:
		if (optmgmt.opt.len != sizeof (TCP_OPTIONS))
			{ u.u_error = TBADOPT; return; }

		if (unimove (&tcp_options, optmgmt.opt.buf, sizeof (TCP_OPTIONS), US) < 0)
			{ u.u_error = EFAULT; return; }
		break;

	    case T_DEFAULT:
#ifdef	TST_UDATA_OPT
		if (optmgmt.opt.len != 0)
			{ u.u_error = TBADOPT; return; }
#endif	TST_UDATA_OPT
		memclr (&tcp_options, sizeof (TCP_OPTIONS));
		break;

	    default:
		{ u.u_error = TBADFLAG; return; }

	}	/* end switch (optmgmt.flags) */

	/*
	 *	Processa "max_seg_size"
	 */
	i = tcp_options.max_seg_size;

	switch (req_flags)
	{
	    case T_NEGOTIATE:
		if   (i <= 0)
			i = 1;
		elif (i > ip->it_MAX_SGSZ)
			i = ip->it_MAX_SGSZ;

		tp->tp_max_seg_sz	 = i;

		if (tp->tp_good_wnd > i)
			tp->tp_good_wnd	 = i;

		tcp_options.max_seg_size = i;
		break;

	    case T_CHECK:
		if (i <= 0 || i > ip->it_MAX_SGSZ)
			ret_flags = T_FAILURE;

		tcp_options.max_seg_size = tp->tp_max_seg_sz;
		break;

	    case T_DEFAULT:
		tcp_options.max_seg_size = ip->it_MAX_SGSZ;
		break;

	}	/* end switch (optmgmt.flags) */

	/*
	 *	Processa "max_wait"
	 */
	i = tcp_options.max_wait;

	switch (req_flags)
	{
	    case T_NEGOTIATE:
		if (i < 0)
			i = 0;

		tp->tp_max_wait	= i;
		tcp_options.max_wait = i;
		break;

	    case T_CHECK:
		if (i < 0)
			ret_flags = T_FAILURE;

		tcp_options.max_wait = tp->tp_max_wait;
		break;

	    case T_DEFAULT:
		tcp_options.max_wait = ip->it_WAIT;
		break;

	}	/* end switch (optmgmt.flags) */

	/*
	 *	Processa "max_silence"
	 */
	i = tcp_options.max_silence;

	switch (req_flags)
	{
	    case T_NEGOTIATE:
		if (i < 0)
			i = 0;

		tp->tp_max_silence	= i;
		tcp_options.max_silence = i;
		break;

	    case T_CHECK:
		if (i < 0)
			ret_flags = T_FAILURE;

		tcp_options.max_silence = tp->tp_max_silence;
		break;

	    case T_DEFAULT:
		tcp_options.max_silence = ip->it_SILENCE;
		break;

	}	/* end switch (optmgmt.flags) */

	/*
	 *	Processa "window_size"
	 */
	i = tcp_options.window_size;

	switch (req_flags)
	{
	    case T_NEGOTIATE:
		if (i > 64 * KBSZ)
			i = 64 * KBSZ;

		if (i < ip->it_MAX_SGSZ)
			i = ip->it_MAX_SGSZ;

		if (i < PGSZ / 2)
			i = PGSZ / 2;

		tp->tp_rnd_in.tp_rnd_sz  = i;
		tp->tp_rnd_out.tp_rnd_sz = i;

		tcp_options.window_size = i;
		break;

	    case T_CHECK:
		if (i > 64 * KBSZ || i < ip->it_MAX_SGSZ || i < PGSZ / 2)
			ret_flags = T_FAILURE;

		tcp_options.window_size = tp->tp_rnd_in.tp_rnd_sz;
		break;

	    case T_DEFAULT:
		tcp_options.window_size = ip->it_WND_SZ;
		break;

	}	/* end switch (optmgmt.flags) */

	/*
	 *	Processa "tp_max_client_conn"
	 */
	i = tcp_options.max_client_conn;

	switch (req_flags)
	{
	    case T_NEGOTIATE:
		if (i < 0)
			i = 0;

		tp->tp_max_client_conn = tcp_options.max_client_conn = i;
		break;

	    case T_CHECK:
		if (i < 0)
			ret_flags = T_FAILURE;

		tcp_options.max_client_conn = tp->tp_max_client_conn;
		break;

	    case T_DEFAULT:
		tcp_options.max_client_conn = 0;
		break;

	}	/* end switch (optmgmt.flags) */

	/*
	 *	Analisa a estrutura T_OPTMGMT de ret
	 */
	if (unimove (&optmgmt, ret, sizeof (T_OPTMGMT), US) < 0)
		{ u.u_error = EFAULT; return; }

	/*
	 *	Escreve de volta a estrutura TCP_OPTIONS
	 */
	if (req_flags == T_CHECK)
	{
		optmgmt.flags = ret_flags;

		if (optmgmt.opt.maxlen >= sizeof (TCP_OPTIONS))
			optmgmt.opt.len = sizeof (TCP_OPTIONS);
		else
			optmgmt.opt.len = 0;
	}
	else 	/* == T_NEGOTIATE || T_DEFAULT */
	{
		optmgmt.flags = 0;

		if (optmgmt.opt.maxlen < sizeof (TCP_OPTIONS))
			{ u.u_error = TBUFOVFLW; return; }

		optmgmt.opt.len = sizeof (TCP_OPTIONS);
	}

	if (optmgmt.opt.len == sizeof (TCP_OPTIONS))
	{
		if (unimove (optmgmt.opt.buf, &tcp_options, sizeof (TCP_OPTIONS), SU) < 0)
			{ u.u_error = EFAULT; return; }
	}

	/*
	 *	Escreve a estrutura T_OPTMGMT de volta
	 */
	if (unimove (ret, &optmgmt, sizeof (T_OPTMGMT), SU) < 0)
		{ u.u_error = EFAULT; return; }

}	/* end k_optmgmt */

/*
 ****************************************************************
 *	Obtém informações sobre uma operação UDP		*
 ****************************************************************
 */
void
k_rcvuderr (T_UDERROR *p)
{
	{ u.u_error = TNOUDERR; return; }

}	/* end k_rcvuderr */

/*
 ****************************************************************
 *	Sincroniza as estruturas de dados			*
 ****************************************************************
 */
int
k_sync (int minor)
{
	return (k_getstate (minor));

}	/* end k_sync */

/*
 ****************************************************************
 *	Descarrega blocos parciais				*
 ****************************************************************
 */
void
k_push (IOREQ *iop)
{
	KFILE		*fp = iop->io_fp;
	TCP_EP		*tp;

	if (fp->f_union != KF_ITNET)
		{ u.u_error = EBADF; return; }

	tp = fp->f_endpoint;

	if (tp->tp_rst)
		{ u.u_error = TLOOK; return; }

	iop->io_count = 0;

	if (tp->tp_pipe_mode)
		pipe_tcp_data_packet (iop, C_PSH, tp);
	else
		queue_tcp_data_packet (iop, C_PSH, tp);

}	/* end k_push */

/*
 ****************************************************************
 *	Obtém o número de bytes prontos para serem lidos	*
 ****************************************************************
 */
int
k_nread (int minor)
{
	KFILE		*fp = u.u_fileptr;
	ITBLOCK 	*bp;
	int		count = 0;

	switch (minor)
	{
		/*
		 *	RAW
		 */
	    case RAW:
	    {
		RAW_EP		*rp;

		if (fp->f_union != KF_ITNET)
			{ u.u_error = EBADF; return (-1); }

		rp = fp->f_endpoint;

		SPINLOCK (&rp->rp_inq_lock);

		for (bp = rp->rp_inq_first; bp != NOITBLOCK; bp = bp->it_inq_forw.inq_ptr)
			count += bp->it_count;

		SPINFREE (&rp->rp_inq_lock);

		return (count);
	    }

		/*
		 *	UDP
		 */
	    case UDP:
	    {
		UDP_EP		*up;

		if (fp->f_union != KF_ITNET)
			{ u.u_error = EBADF; return (-1); }

		up = fp->f_endpoint;

		SPINLOCK (&up->up_inq_lock);

		for (bp = up->up_inq_first; bp != NOITBLOCK; bp = bp->it_inq_forw.inq_ptr)
			count += bp->it_count;

		SPINFREE (&up->up_inq_lock);

		return (count);
	    }

		/*
		 *	TCP
		 */
	    case TCP:
	    {
		TCP_EP		*tp;

		if (fp->f_union != KF_ITNET)
			{ u.u_error = EBADF; return (-1); }

		tp = fp->f_endpoint;

		if (tp->tp_rst)
			{ u.u_error = TLOOK; return (-1); }

	   /***	SPINLOCK (&tp->tp_inq_lock); ***/

		if ((count = tp->tp_rnd_in.tp_rnd_count) == 0)
		{
			if (tp->tp_rnd_in.tp_rnd_fin)
			{
			   /***	SPINFREE (&tp->tp_inq_lock); ***/
				u.u_error = TLOOK; return (-1);
			}
		}

	   /***	SPINFREE (&tp->tp_inq_lock); ***/

		return (count);
	    }

		/*
		 *	Protocolos desconhecidos
		 */
	    default:
		{ u.u_error = ENXIO; return (-1); }

	}	/* end switch */

}	/* end k_nread */

/*
 ****************************************************************
 *	Processa o ATTENTION					*
 ****************************************************************
 */
int
k_attention (int minor, int fd_index, char **attention_set)
{
	KFILE		*fp = u.u_fileptr;
	ITSCB		*ip = &itscb;

	switch (minor)
	{
		/*
		 *	RAW
		 */
	    case RAW:
	    {
		RAW_EP		*rp;

		if (fp->f_union != KF_ITNET)
			{ u.u_error = EBADF; return (-1); }

		rp = fp->f_endpoint;

		SPINLOCK (&rp->rp_inq_lock);

		if (rp->rp_inq_first != NOITBLOCK)
			{ SPINFREE (&rp->rp_inq_lock); return (1); }

		rp->rp_uproc	= u.u_myself;
		rp->rp_fd_index	= fd_index;

		rp->rp_attention_set = 1;
		*attention_set	= &rp->rp_attention_set;

		SPINFREE (&rp->rp_inq_lock);

		return (0);
	    }

		/*
		 *	UDP
		 */
	    case UDP:
	    {
		UDP_EP		*up;

		if (fp->f_union != KF_ITNET)
			{ u.u_error = EBADF; return (-1); }

		up = fp->f_endpoint;

		SPINLOCK (&up->up_inq_lock);

		if (up->up_inq_first != NOITBLOCK)
			{ SPINFREE (&up->up_inq_lock); return (1); }

		up->up_uproc	= u.u_myself;
		up->up_fd_index	= fd_index;

		up->up_attention_set = 1;
		*attention_set	= &up->up_attention_set;

		SPINFREE (&up->up_inq_lock);

		return (0);
	    }

		/*
		 *	TCP
		 */
	    case TCP:
	    {
		TCP_EP		*tp;
		const LISTENQ	*lp, *end_listen_q;

		if (fp->f_union != KF_ITNET)
			{ u.u_error = EBADF; return (-1); }

		tp = fp->f_endpoint;

		if (tp->tp_rst)
			return (1);
#if (0)	/*******************************************************/
			{ u.u_error = TLOOK; return (-1); }
#endif	/*******************************************************/

		SPINLOCK (&tp->tp_inq_lock);

		switch (tp->tp_state)
		{
		    case S_BOUND:
		    case S_LISTEN:
		    case S_SYN_SENT:
			for (lp = tp->tp_listen_q, end_listen_q = lp + LISTENQ_MAXSZ; lp < end_listen_q; lp++)
			{
				if (lp->tp_listen_seq > 0)
					{ SPINFREE (&tp->tp_inq_lock); return (1); }

			}	/* end for (lista de listen) */

			break;

		    case S_ESTABLISHED:
			if (tp->tp_rnd_in.tp_rnd_count > 0 || tp->tp_rnd_in.tp_rnd_fin)
			{
				SPINFREE (&tp->tp_inq_lock);
#ifdef	MSG
				if (ip->it_list_input)
				{
					printf
					(	"%g: Porta %d tem % bytes\n",
						tp->tp_my_port, tp->tp_rnd_in.tp_rnd_count
					);
				}
#endif	MSG
				return (1);
			}

			break;

		    case S_FIN_WAIT_2:
			if (tp->tp_rnd_in.tp_rnd_fin)
				{ SPINFREE (&tp->tp_inq_lock); return (1); }

			break;

		    default:
			{ SPINFREE (&tp->tp_inq_lock); u.u_error = TOUTSTATE; return (-1); }

		}	/* end switch */

		/* Ainda não chegou o evento */

		tp->tp_uproc	= u.u_myself;
		tp->tp_fd_index	= fd_index;

		tp->tp_attention_set = 1;
		*attention_set	= &tp->tp_attention_set;

		SPINFREE (&tp->tp_inq_lock);
#ifdef	MSG
		if (ip->it_list_input)
			printf ("%g: Porta %d NÃO tem dados\n", tp->tp_my_port);
#endif	MSG
		return (0);
	    }

		/*
		 *	Protocolos desconhecidos
		 */
	    default:
		{ u.u_error = ENXIO; return (-1); }

	}	/* end switch */

}	/* end k_attention */

/*
 ****************************************************************
 *	Obtém os endereços local e remoto			*
 ****************************************************************
 */
void
k_getaddr (int minor, INADDR *addr_ptr)
{
	KFILE		*fp = u.u_fileptr;
	INADDR		addr[4];

	/*
	 *	O endereço deve ser dado
	 */
	if (addr_ptr == (INADDR *)NULL)
		{ u.u_error = EFAULT; return; }

	/*
	 *	Examina o protocolo
	 */
	switch (minor)
	{
		/*
		 *	RAW
		 */
	    case RAW:
	    {
		RAW_EP		*rp;

		if (fp->f_union != KF_ITNET)
			{ u.u_error = EBADF; return; }

		rp = fp->f_endpoint;

		addr[0].a_family = 0;
	   	addr[0].a_addr	 = rp->rp_my_addr;
		addr[0].a_proto	 = 0;

		addr[1].a_family = 0;
	   	addr[1].a_addr	 = rp->rp_bind_addr;
		addr[1].a_proto	 = rp->rp_bind_proto;

		addr[2].a_family = 0;
		addr[2].a_addr	 = rp->rp_snd_addr;
		addr[2].a_proto	 = rp->rp_snd_proto;

		addr[3].a_family = 0;
		addr[3].a_addr	 = rp->rp_rcv_addr;
		addr[3].a_proto	 = rp->rp_rcv_proto;

		if (unimove (addr_ptr, addr, 4 * sizeof (INADDR), SU) < 0)
			{ u.u_error = EFAULT; return; }

		break;
	    }

		/*
		 *	UDP
		 */
	    case UDP:
	    {
		UDP_EP		*up;

		if (fp->f_union != KF_ITNET)
			{ u.u_error = EBADF; return; }

		up = fp->f_endpoint;

		addr[0].a_family = 0;
	   	addr[0].a_addr	 = up->up_my_addr;
		addr[0].a_port	 = up->up_my_port;

		addr[1].a_family = 0;
		addr[1].a_addr	 = up->up_bind_addr;
		addr[1].a_port	 = up->up_bind_port;

		addr[2].a_family = 0;
		addr[2].a_addr	 = up->up_snd_addr;
		addr[2].a_port	 = up->up_snd_port;

		addr[3].a_family = 0;
		addr[3].a_addr	 = up->up_rcv_addr;
		addr[3].a_port	 = up->up_rcv_port;

		if (unimove (addr_ptr, addr, 4 * sizeof (INADDR), SU) < 0)
			{ u.u_error = EFAULT; return; }

		break;
	    }

		/*
		 *	TCP
		 */
	    case TCP:
	    {
		TCP_EP		*tp;

		if (fp->f_union != KF_ITNET)
			{ u.u_error = EBADF; return; }

		tp = fp->f_endpoint;

		addr[0].a_family = 0;
		addr[0].a_addr	 = tp->tp_my_addr;
		addr[0].a_port	 = tp->tp_my_port;

		addr[1].a_family = 0;
		addr[1].a_addr	 = tp->tp_dst_addr;
		addr[1].a_port	 = tp->tp_dst_port;

		if (unimove (addr_ptr, addr, 2 * sizeof (INADDR), SU) < 0)
			{ u.u_error = EFAULT; return; }

		break;
	    }

		/*
		 *	Protocolos desconhecidos
		 */
	    default:
		{ u.u_error = ENXIO; return; }

	}	/* end switch */

}	/* end k_getaddr */
