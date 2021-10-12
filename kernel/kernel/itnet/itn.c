/*
 ****************************************************************
 *								*
 *			itn.c					*
 *								*
 *	Driver dos dispositivos "/dev/itn..."			*
 *								*
 *	Versão	3.0.0, de 26.07.91				*
 *		4.9.0, de 30.08.06				*
 *								*
 *	Funções:						*
 *		itnopen,  itnclose,   itnctl,			*
 *		itnread,  itnwrite				*
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
#include "../h/sync.h"
#include "../h/scb.h"
#include "../h/region.h"

#include "../h/kfile.h"
#include "../h/inode.h"
#include "../h/uerror.h"
#include "../h/signal.h"
#include "../h/uproc.h"
#include "../h/ioctl.h"
#include "../h/xti.h"
#include "../h/itnet.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****** Definições globais **************************************
 */
#define	WND_SZ		(6 * 1024)	/* Default, para RND de 3 páginas */

#define	GOOD_WND	MAX_SGSZ	/* Default */

#define	DEF_ALPHA	820		/* Default de ALPHA (80 %) */

#define	DEF_BETA	2		/* Default BETA para SRTT */

#define	INIT_SRTT	(5 * 1024)	/* Valor inicial de SRTT */

#define	DEF_N_TRANS	12		/* No. de transmissões de TCP */

#define DEF_WAIT	(2  * 60)	/* Tempo máximo sem conexão */
#define	DEF_SILENCE	(30 * 60)	/* Tempo máximo com conexão */

/*
 ******	Variáveis globais ***************************************
 */
entry IPADDR	nm_serv_tb[DNS_SERVER_CNT + 1];

entry EVENT	dns_server_table_not_empty_event;

extern INODE	*itnet_debug_ip;	/* Arquivo de depuração */

/*
 ****************************************************************
 *	Inicializa a rede ou obtém um EP			*
 ****************************************************************
 */
void
itnopen (dev_t dev, int oflag)
{
	ITSCB		*ip = &itscb;
	KFILE		*fp = u.u_fileptr;
	IT_MINOR	minor = MINOR (dev);

	/*
	 *	Verifica se a a rede está inicializada
	 */
	if (TAS (&ip->it_init_lock) >= 0)
	{
		if (minor == DAEMON)
		{
			if (superuser () < 0)
				return;

			init_it_block ();

			ether_init ();

			raw_ep_free_init ();

			udp_ep_free_init ();

			tcp_ep_free_init ();

			ip->it_N_BLOCK = scb.y_n_itblock;

			if (ip->it_WND_SZ == 0)
				ip->it_WND_SZ = WND_SZ;

			if (ip->it_GOOD_WND == 0)
				ip->it_GOOD_WND = GOOD_WND;

			if (ip->it_ALPHA == 0)
				ip->it_ALPHA = DEF_ALPHA;

			if (ip->it_BETA == 0)
				ip->it_BETA = DEF_BETA;

			if (ip->it_SRTT == 0)
				ip->it_SRTT = INIT_SRTT;

			if (ip->it_N_TRANS == 0)
				ip->it_N_TRANS = DEF_N_TRANS;

			if (ip->it_WAIT == 0)
				ip->it_WAIT = DEF_WAIT;

			if (ip->it_SILENCE == 0)
				ip->it_SILENCE = DEF_SILENCE;

			if (ip->it_MAX_SGSZ == 0)
				ip->it_MAX_SGSZ = MAX_SGSZ;

			SEMAINIT (&ip->it_block_sema, ip->it_N_BLOCK, 0 /* sem histórico */);

		   /***	ip->it_gateway = 1; ***/
		   	ip->it_pipe_mode = 1;
		}
		else
		{
			CLEAR (&ip->it_init_lock);

			u.u_error = TBADNET;
		}

		return;
	}

	/*
	 *	Pequena consistência
	 */
	if ((oflag & O_RW) != O_RW)
		{ u.u_error = EINVAL; return; }

	/*
	 *	Obtém e inicializa um "endpoint"
	 */
	switch (minor)
	{
		/*
		 *	O "in_daemon": "open"s seguintes não fazem efeito
		 */
	    case DAEMON:
		return;

		/*
		 *	Protocolo RAW
		 */
	    case RAW:
	    {
		RAW_EP		*rp;

		if (fp->f_union != KF_NULL)
			{ u.u_error = EBADF; return; }

		if ((rp = get_raw_ep ()) == NO_RAW_EP)
			{ u.u_error = EAGAIN; return; }

	   /***	SPINLOCK (&rp->rp_inq_lock); ***/

		rp->rp_state	= S_UNBOUND;

		fp->f_union	= KF_ITNET;
		fp->f_endpoint	= rp;

	   /***	SPINFREE (&rp->rp_inq_lock); ***/

		return;
	    }

		/*
		 *	Protocolo UDP
		 */
	    case UDP:
	    {
		UDP_EP		*up;

		if (fp->f_union != KF_NULL)
			{ u.u_error = EBADF; return; }

		if ((up = get_udp_ep ()) == NO_UDP_EP)
			{ u.u_error = EAGAIN; return; }

	   /***	SPINLOCK (&up->up_inq_lock); ***/

		up->up_state	= S_UNBOUND;

		fp->f_union	= KF_ITNET;
		fp->f_endpoint	= up;

	   /***	SPINFREE (&up->up_inq_lock); ***/

		return;
	    }

		/*
		 *	Protocolo TCP
		 */
	    case TCP:
	    {
		TCP_EP		*tp;

		if (fp->f_union != KF_NULL)
			{ u.u_error = EBADF; return; }

		if ((tp = get_tcp_ep ()) == NO_TCP_EP)
			{ u.u_error = EAGAIN; return; }

	   /***	SLEEPLOCK (&tp->tp_lock, PITNETOUT); ***/

		tp->tp_state		 = S_UNBOUND;
		tp->tp_SRTT		 = ip->it_SRTT;
		tp->tp_max_seg_sz 	 = ip->it_MAX_SGSZ;
		tp->tp_good_wnd 	 = ip->it_GOOD_WND;
		tp->tp_max_wait		 = ip->it_WAIT;
		tp->tp_max_silence	 = ip->it_SILENCE;
		tp->tp_rnd_in.tp_rnd_sz  = ip->it_WND_SZ;
		tp->tp_rnd_out.tp_rnd_sz = ip->it_WND_SZ;
		tp->tp_last_rcv_time	 = time;

		fp->f_union	= KF_ITNET;
		fp->f_endpoint	= tp;

	   /***	SLEEPFREE (&tp->tp_lock); ***/

		return;
	    }

	}	/* end switch */

	u.u_error = ENXIO;
	return;

}	/* end itnopen */

/*
 ****************************************************************
 *	Fecha um "endpoint"					*
 ****************************************************************
 */
void
itnclose (dev_t dev, int flag)
{
	KFILE		*fp = u.u_fileptr;
	IT_MINOR	minor = MINOR (dev);

	switch (minor)
	{
		/*
		 *	O "in_daemon"
		 */
	    case DAEMON:
		if (flag != 0)	/* Só processa quando o "inode_cnt" zerar */
			return;
#ifdef	MSG
		if (itscb.it_uep_count)
			{ printf ("%g: Impedi a ITNET terminar ainda com EPs ativos\n"); return; }
#endif	MSG
		if (itnet_debug_ip)
		{
			SLEEPLOCK (&itnet_debug_ip->i_lock, PINODE);
			iput (itnet_debug_ip);
			itnet_debug_ip = NOINODE;
		}

		put_all_it_block ();	/* Devolve os ITBLOCKs sobrando */

		CLEAR (&itscb.it_init_lock);

		return;

		/*
		 *	Protocolo RAW
		 */
	    case RAW:
	    {
		if (fp->f_union != KF_ITNET)
			{ u.u_error = EBADF; return; }

		put_raw_ep (fp->f_endpoint);

		fp->f_union	= KF_NULL;
	   /***	fp->f_endpoint	= (RAW_EP *)NULL); ***/

		return;
	    }

		/*
		 *	Protocolo UDP
		 */
	    case UDP:
	    {
		if (fp->f_union != KF_ITNET)
			{ u.u_error = EBADF; return; }

		put_udp_ep (fp->f_endpoint);

		fp->f_union	= KF_NULL;
	   /***	fp->f_endpoint	= (UDP_EP *)NULL); ***/

		return;
	    }

		/*
		 *	Protocolo TCP
		 */
	    case TCP:
	    {
		TCP_EP		*tp;

		if (fp->f_union != KF_ITNET)
			{ u.u_error = EBADF; return; }

		tp = fp->f_endpoint;

		switch (tp->tp_state)
		{
		    case S_SYN_RECEIVED:
		    case S_ESTABLISHED:
		    case S_FIN_WAIT_1:
		    case S_FIN_WAIT_2:
		    case S_CLOSE_WAIT:
		    case S_CLOSING:
		    case S_LAST_ACK:
#undef	REAL_TIME_WAIT				/* Lembrar em "ksnd.c" */
#ifndef	REAL_TIME_WAIT
		    case S_TIME_WAIT:
#endif	REAL_TIME_WAIT
			send_tcp_rst_packet
			(
				C_RST,
				tp->tp_dst_addr,	/* dst_addr */
				tp->tp_dst_port,	/* dst_port */
				tp->tp_my_addr,		/* my_addr */
				tp->tp_my_port,		/* my_port */
				tp->tp_snd_nxt,		/* seq */
				tp->tp_rcv_nxt		/* ack */
			);
			break;

#ifdef	REAL_TIME_WAIT
		    case S_TIME_WAIT:
			for (EVER)
			{
				EVENTWAIT (&every_second_event, PITNETOUT);

				if (tp->tp_state == S_BOUND)
					break;
			}
			break;
#endif	REAL_TIME_WAIT

		    default:
			break;

		}	/* end switch */

		put_tcp_ep (tp);

		fp->f_union	= KF_NULL;
	   /***	fp->f_endpoint	= (TCP_EP *)NULL); ***/

		return;
	    }

	}	/* end switch */

}	/* end itnclose */

/*
 ****************************************************************
 *	Função de IOCTL						*
 ****************************************************************
 */
int
itnctl (dev_t dev, int cmd, int arg, int flag)
{
	ITSCB		*ip = &itscb;
	int		minor = MINOR (dev);
	ITSCB		uitscb;
	int		i = 0;

	/*
	 *	Executa o comando
	 */
	switch (cmd)
	{
		/*
		 *	O DAEMON de entrada
		 */
	    case I_IN_DAEMON:
		if (superuser () < 0)
			return (-1);

		execute_in_daemon ((void *)arg);
		return (UNDEF);

		/*
		 *	O DAEMON de saída
		 */
	    case I_OUT_DAEMON:
		if (superuser () < 0)
			return (-1);

		execute_out_daemon ();
		return (UNDEF);

		/*
		 *	O DAEMON do NFS
		 */
	    case I_NFS_DAEMON:
		if (superuser () < 0)
			return (-1);

		execute_nfs_daemon ();
		return (UNDEF);

		/*
		 *	Get "itscb"
		 */
	    case I_GET_ITSCB:
		if (unimove ((ITSCB *)arg, ip, sizeof (ITSCB), SU) < 0)
			u.u_error = EFAULT;

		return (UNDEF);

		/*
		 *	Put "itscb"
		 */
	    case I_PUT_ITSCB:
		if (superuser () < 0)
			return (-1);

		if (unimove (&uitscb, (ITSCB *)arg, sizeof (ITSCB), US) < 0)
			{ u.u_error = EFAULT; return (-1); }

	   /***	ip->it_N_BLOCK	= uitscb.it_N_BLOCK; ***/
		ip->it_WND_SZ	= uitscb.it_WND_SZ;
		ip->it_GOOD_WND	= uitscb.it_GOOD_WND;
		ip->it_ALPHA	= uitscb.it_ALPHA;
		ip->it_BETA	= uitscb.it_BETA;
		ip->it_SRTT	= uitscb.it_SRTT;
		ip->it_N_TRANS  = uitscb.it_N_TRANS;
		ip->it_WAIT 	= uitscb.it_WAIT;
		ip->it_SILENCE  = uitscb.it_SILENCE;
		ip->it_MAX_SGSZ = uitscb.it_MAX_SGSZ;

		if (ip->it_GOOD_WND > ip->it_MAX_SGSZ)
			ip->it_GOOD_WND = ip->it_MAX_SGSZ;

		if (ip->it_WND_SZ < ip->it_MAX_SGSZ)
			ip->it_WND_SZ = ip->it_MAX_SGSZ;

		if (ip->it_WND_SZ < PGSZ / 2)
			ip->it_WND_SZ = PGSZ / 2;

		memmove (&ip->it_write_input, &uitscb.it_write_input, ITSCB_FLAG_SZ);

		return (0);

		/*
		 *	Get DNS (Tabela ou tamanho)
		 */
	    case I_GET_DNS:
		if (arg == 0)
			return (scb.y_n_dns);

		if (unimove ((DNS *)arg, scb.y_dns, scb.y_n_dns * sizeof (DNS), SU) < 0)
			u.u_error = EFAULT;

		return (UNDEF);

		/*
		 *	Get ROUTE (Tabela ou tamanho)
		 */
	    case I_GET_ROUTE:
		if (arg == 0)
			return (scb.y_n_route);

		if (unimove ((ROUTE *)arg, scb.y_route, scb.y_n_route * sizeof (ROUTE), SU) < 0)
			u.u_error = EFAULT;

		return (UNDEF);

		/*
		 *	Get ETHER (Tabela ou tamanho)
		 */
	    case I_GET_ETHER:
		if (arg == 0)
			return (scb.y_n_ether);

		if (unimove ((ETHER *)arg, scb.y_ether, scb.y_n_ether * sizeof (ETHER), SU) < 0)
			u.u_error = EFAULT;

		return (UNDEF);

		/*
		 *	Get EXPORT (Tabela ou tamanho)
		 */
	    case I_GET_EXPORT:
		if (arg == 0)
			return (scb.y_n_export);

		if (unimove ((EXPORT *)arg, scb.y_export, scb.y_n_export * sizeof (EXPORT), SU) < 0)
			u.u_error = EFAULT;

		return (UNDEF);

		/*
		 *	Put EXPORT
		 */
	    case I_PUT_EXPORT:
		if (superuser () < 0)
			return (-1);

		if (unimove (scb.y_export, (EXPORT *)arg, scb.y_n_export * sizeof (EXPORT), US) < 0)
			u.u_error = EFAULT;

		return (UNDEF);

		/*
		 *	Get NFS_MOUNT (Tabela ou tamanho)
		 */
	    case I_GET_NFS_MOUNT:
	    {
		extern EXPORT	*nfs_mount;
		extern int	nfs_mount_sz;

		if (arg == 0)
			return (nfs_mount_sz);

		if (unimove ((EXPORT *)arg, nfs_mount, nfs_mount_sz * sizeof (EXPORT), SU) < 0)
			u.u_error = EFAULT;

		return (UNDEF);
	    }

		/*
		 *	Get TCP_EPs ativos
		 */
	    case I_GET_TCP_EP:
	    {
		TCP_EP		*tp;

		for (tp = ip->it_tep_busy; tp != NO_TCP_EP; tp = tp->tp_next)
		{
			if (unimove ((TCP_EP *)arg, tp, sizeof (TCP_EP), SU) < 0)
				{ u.u_error = EFAULT; return (-1); }

			arg += sizeof (TCP_EP);		i++;
		}

		return (i);
	    }

		/*
		 *	k_domain_resolver: Domain name resolver
		 */
	    case I_DNS_WAIT_REQ:
		return (k_dns_wait_req ((void *)arg));

	    case I_DNS_PUT_INFO:
		return (k_dns_put_info ((void *)arg));

	    case I_GET_DNS_SERVER:
		if (arg == 0)
			return (DNS_SERVER_CNT);

		EVENTWAIT (&dns_server_table_not_empty_event, PITNETOUT);

		if (unimove ((IPADDR *)arg, nm_serv_tb, sizeof (nm_serv_tb), SU) < 0)
			u.u_error = EFAULT;

		return (UNDEF);

	    case I_PUT_DNS_SERVER:
		if (unimove (nm_serv_tb, (IPADDR *)arg, sizeof (nm_serv_tb), US) < 0)
			u.u_error = EFAULT;

		if (nm_serv_tb[0] != 0)
			EVENTDONE (&dns_server_table_not_empty_event);

		return (UNDEF);

		/*
		 *	Processa/prepara o ATTENTION
		 */
	     case U_ATTENTION:
		return (k_attention (minor, arg, (char **)flag));

	}	/* end switch */

	/*
	 *	Erro
	 */
	u.u_error = EINVAL;
	return (-1);

}	/* end itnctl */

/*
 ****************************************************************
 *	A coletânea de funções					*
 ****************************************************************
 */
int
itnet (int fd, int cmd, int arg1, int arg2)
{
	int		minor;
	KFILE		*fp;
	dev_t		dev;
	T_UNITDATA	unitdata;
	long		count;
	IOREQ		io;

	/*
	 *	Prólogo
	 */
	if ((fp = fget (fd)) == NOKFILE)
		return (-1);

	u.u_oflag = fp->f_flags;

	dev = fp->f_inode->i_rdev;

	minor = MINOR (dev);

	/*
	 *	Prepara o bloco de E/S
	 */
   /***	io.io_fd	 = ...;	***/
	io.io_fp	 = fp;
   /***	io.io_ip	 = ...;	***/
   /***	io.io_dev	 = ...;	***/
   /***	io.io_area	 = ...;	***/
   /***	io.io_count	 = ...;	***/
   /***	io.io_offset_low = ...;	***/
   /***	io.io_cpflag	 = ...;	***/
   /***	io.io_rablock	 = ...;	***/

	/*
	 *	Executa o comando
	 */
	switch (cmd)
	{
		/*
		 *	k_accept
		 */
	    case I_ACCEPT:
		if (minor != TCP)
			break;
		k_accept (arg1, (const T_CALL *)arg2);
		return (UNDEF);

		/*
		 *	k_bind
		 */
	    case I_BIND:
		k_bind (minor, (const T_BIND *)arg1, (T_BIND *)arg2);
		return (UNDEF);

		/*
		 *	k_connect
		 */
	    case I_CONNECT:
		if (minor != TCP)
			break;
		k_connect ((const T_CALL *)arg1, (T_CALL *)arg2);
		return (UNDEF);

		/*
		 *	k_getinfo
		 */
	    case I_GETINFO:
		k_getinfo (minor, (T_INFO *)arg1);
		return (UNDEF);

		/*
		 *	k_getstate
		 */
	    case I_GETSTATE:
		return (k_getstate (minor));

		/*
		 *	k_listen
		 */
	    case I_LISTEN:
		if (minor != TCP)
			break;
		k_listen ((T_CALL *)arg1);
		return (UNDEF);

		/*
		 *	k_look
		 */
	    case I_LOOK:
		return (k_look (minor));

		/*
		 *	k_optmgmt
		 */
	    case I_OPTMGMT:
		k_optmgmt (minor, (const T_OPTMGMT *)arg1, (T_OPTMGMT *)arg2);
		return (UNDEF);

		/*
		 *	k_rcvconnect
		 */
	    case I_RCVCONNECT:
		if (minor != TCP)
			break;
		k_rcvconnect ((T_CALL *)arg1);
		return (UNDEF);

		/*
		 *	k_rcvdis
		 */
	    case I_RCVDIS:
		if (minor != TCP)
			break;
		k_rcvdis ((T_DISCON *)arg1);
		return (UNDEF);

		/*
		 *	k_rcvrel
		 */
	    case I_RCVREL:
		if (minor != TCP)
			break;
		k_rcvrel ();
		return (UNDEF);

		/*
		 *	k_rcvudata
		 */
	    case I_RCVUDATA:
		if (minor != UDP)
			break;

		if (unimove (&unitdata, (T_UNITDATA *)arg1, sizeof (T_UNITDATA), US) < 0)
			{ u.u_error = EFAULT; return (-1); }
#ifdef	TST_UDATA_OPT
		if (unitdata.opt.len != 0)
			{ u.u_error = TBADOPT; return (-1); }
#endif	TST_UDATA_OPT

		count	  = unitdata.udata.maxlen;

		io.io_area	= unitdata.udata.buf;
		io.io_count	= unitdata.udata.maxlen;

		switch (unitdata.addr.maxlen)
		{
		    case 0:
		    case sizeof (INADDR):
		    case 2 * sizeof (INADDR):
			k_rcvudata (&io, unitdata.addr.buf, unitdata.addr.maxlen, (int *)arg2);

			break;

		    default:
			{ u.u_error = TBADADDR; return (-1); }

		}	/* end switch */

		unitdata.addr.len = unitdata.addr.maxlen;

		if (u.u_error)
			return (-1);

		unitdata.opt.len = 0;

		unitdata.udata.len = count - io.io_count;

		if (unimove ((T_UNITDATA *)arg1, &unitdata, sizeof (T_UNITDATA), SU) < 0)
			{ u.u_error = EFAULT; return (-1); }

		return (UNDEF);

		/*
		 *	k_rcvuderr
		 */
	    case I_RCVUDERR:
		if (minor != UDP)
			break;
		k_rcvuderr ((T_UDERROR *)arg1);
		return (UNDEF);

		/*
		 *	k_snddis
		 */
	    case I_SNDDIS:
		if (minor != TCP)
			break;
		k_snddis ((T_CALL *)arg1);
		return (UNDEF);

		/*
		 *	k_sndrel
		 */
	    case I_SNDREL:
		if (minor != TCP)
			break;
		k_sndrel (&io);
		return (UNDEF);

		/*
		 *	k_sndudata
		 */
	    case I_SNDUDATA:
		if (minor != UDP)
			break;

		if (unimove (&unitdata, (T_UNITDATA *)arg1, sizeof (T_UNITDATA), US) < 0)
			{ u.u_error = EFAULT; return (-1); }
#ifdef	TST_UDATA_OPT
		if (unitdata.opt.len != 0)
			{ u.u_error = TBADOPT; return (-1); }
#endif	TST_UDATA_OPT

		io.io_area	= unitdata.udata.buf;
		io.io_count	= unitdata.udata.len;

		switch (unitdata.addr.len)
		{
		    case 0:
		    case sizeof (INADDR):
		    case 2 * sizeof (INADDR):
			k_sndudata (&io, unitdata.addr.buf, unitdata.addr.len);

			break;

		    default:
			{ u.u_error = TBADADDR; return (-1); }

		}	/* end switch */

		return (UNDEF);

		/*
		 *	k_sync
		 */
	    case I_SYNC:
		return (k_sync (minor));

		/*
		 *	k_unbind
		 */
	    case I_UNBIND:
		k_unbind (minor);
		return (UNDEF);

		/*
		 *	k_push
		 */
	    case I_PUSH:
		if (minor != TCP)
			break;
		k_push (&io);
		return (UNDEF);

		/*
		 *	k_nread
		 */
	    case I_NREAD:
		return (k_nread (minor));

		/*
		 *	k_strevent
		 */
	    case I_GET_EVENT_STR:
		return (k_strevent (arg1, (char *)arg2));

		/*
		 *	k_getaddr
		 */
	    case I_GETADDR:
		k_getaddr (minor, (INADDR *)arg1);
		return (UNDEF);

		/*
		 *	k_rcvraw
		 */
	    case I_RCVRAW:
		if (minor != RAW)
			break;

		if (unimove (&unitdata, (T_UNITDATA *)arg1, sizeof (T_UNITDATA), US) < 0)
			{ u.u_error = EFAULT; return (-1); }

#ifdef	TST_UDATA_OPT
		if (unitdata.opt.len != 0)
			{ u.u_error = TBADOPT; return (-1); }
#endif	TST_UDATA_OPT

		count	  = unitdata.udata.maxlen;

		io.io_area	= unitdata.udata.buf;
		io.io_count	= unitdata.udata.maxlen;

		switch (unitdata.addr.maxlen)
		{
		    case 0:
		    case sizeof (INADDR):
		    case 2 * sizeof (INADDR):
			k_rcvraw (&io, unitdata.addr.buf, unitdata.addr.maxlen, (int *)arg2);

			break;

		    default:
			{ u.u_error = TBADADDR; return (-1); }

		}	/* end switch */

		unitdata.addr.len = unitdata.addr.maxlen;

		if (u.u_error)
			return (-1);

		unitdata.opt.len = 0;

		unitdata.udata.len = count - io.io_count;

		if (unimove ((T_UNITDATA *)arg1, &unitdata, sizeof (T_UNITDATA), SU) < 0)
			{ u.u_error = EFAULT; return (-1); }

		return (UNDEF);

		/*
		 *	k_sndraw
		 */
	    case I_SNDRAW:
		if (minor != RAW)
			break;

		if (unimove (&unitdata, (T_UNITDATA *)arg1, sizeof (T_UNITDATA), US) < 0)
			{ u.u_error = EFAULT; return (-1); }
#ifdef	TST_UDATA_OPT
		if (unitdata.opt.len != 0)
			{ u.u_error = TBADOPT; return (-1); }
#endif	TST_UDATA_OPT

		io.io_area	= unitdata.udata.buf;
		io.io_count	= unitdata.udata.len;

		switch (unitdata.addr.len)
		{
		    case 0:
		    case sizeof (INADDR):
		    case 2 * sizeof (INADDR):
			k_sndraw (&io, unitdata.addr.buf, unitdata.addr.len);

			break;

		    default:
			{ u.u_error = TBADADDR; return (-1); }

		}	/* end switch */

		return (UNDEF);

		/*
		 *	k_node_to_addr
		 */
	    case I_NODE_TO_ADDR:
		return (k_node_to_addr ((void *)arg1));

		/*
		 *	k_addr_to_node
		 */
	    case I_ADDR_TO_NODE:
		return (k_addr_to_node (arg1, (char *)arg2, 0 /* sem busca */));

		/*
		 *	k_mail_to_node
		 */
	    case I_MAIL_TO_NODE:
		return (k_mail_to_node ((DNS *)arg1));

		/*
		 *	k_addr_to_name
		 */
	    case I_ADDR_TO_NAME:
		return (k_addr_to_node (arg1, (char *)arg2, 1 /* com busca */));


	}	/* end switch */

	/*
	 *	Erro
	 */
	u.u_error = EINVAL;
	return (-1);

}	/* end itnet */

/*
 ****************************************************************
 *	Função "t_rcv"						*
 ****************************************************************
 */
int
itnetrcv (int fd, void *area, int count, int *flags)
{
	KFILE		*fp;
	dev_t		dev;
	IOREQ		io;

	/*
	 *	Prólogo
	 */
	if ((fp = fget (fd)) == NOKFILE)
		return (-1);

	u.u_oflag = fp->f_flags;

	dev = fp->f_inode->i_rdev;

	if (MINOR (dev) != TCP)
		{ u.u_error = EINVAL; return (-1); }

	/*
	 *	Prepara o bloco de E/S
	 */
   /***	io.io_fd	 = ...;	***/
	io.io_fp	 = fp;
   /***	io.io_ip	 = ...;	***/
   /***	io.io_dev	 = ...;	***/
	io.io_area	 = area;
	io.io_count	 = count;
   /***	io.io_offset_low = ...;	***/
   /***	io.io_cpflag	 = ...;	***/
   /***	io.io_rablock	 = ...;	***/

	/*
	 *	Executa o "t_rcv"
	 */
	k_rcv (&io, flags);

	return (count - io.io_count);

}	/* end itnetrcv */

/*
 ****************************************************************
 *	Função "t_snd"						*
 ****************************************************************
 */
int
itnetsnd (int fd, const void *area, int count, int flags)
{
	KFILE		*fp;
	IOREQ		io;

	/*
	 *	Prólogo
	 */
	if ((fp = fget (fd)) == NOKFILE)
		return (-1);

	u.u_oflag = fp->f_flags;

	if (MINOR (fp->f_inode->i_rdev) != TCP)
		{ u.u_error = EINVAL; return (-1); }

	/*
	 *	Prepara o bloco de E/S
	 */
   /***	io.io_fd	 = ...;	***/
	io.io_fp	 = fp;
   /***	io.io_ip	 = ...;	***/
   /***	io.io_dev	 = ...;	***/
	io.io_area	 = (void *)area;
	io.io_count	 = count;
   /***	io.io_offset_low = ...;	***/
   /***	io.io_cpflag	 = ...;	***/
   /***	io.io_rablock	 = ...;	***/

	/*
	 *	Executa o "t_snd"
	 */
	k_snd (&io, flags);

	return (count - io.io_count);

}	/* end itnetsnd */

/*
 ****************************************************************
 *	Função de read						*
 ****************************************************************
 */
void
itnread (IOREQ *iop)
{
	switch (MINOR (iop->io_dev))
	{
	    case UDP:
		k_rcvudata (iop, (INADDR *)NULL, 0, (int *)NULL);
		break;

	    case TCP:
		k_rcv (iop, (int *)0);
		break;

	    default:
		u.u_error = ENXIO;
		break;

	}	/* end switch */

}	/* end itnread */

/*
 ****************************************************************
 *	Função de escrita					*
 ****************************************************************
 */
void
itnwrite (IOREQ *iop)
{
	switch (MINOR (iop->io_dev))
	{
	    case UDP:
		k_sndudata (iop, (INADDR *)NULL, 0);
		break;

	    case TCP:
		k_snd (iop, 0);
		break;

	    default:
		u.u_error = ENXIO;
		break;

	}	/* end switch */

}	/* end itnwrite */
