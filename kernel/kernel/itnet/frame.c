/*
 ****************************************************************
 *								*
 *			frame.c					*
 *								*
 *	Funções relacionadas com o pacote (frame)		*
 *								*
 *	Versão	3.0.0, de 17.12.95				*
 *		4.2.0, de 22.12.01				*
 *								*
 *	Funções:						*
 *		send_frame,  route_frame,  receive_frame	*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 * 		Copyright © 2001 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include "../h/common.h"
#include "../h/scb.h"
#include "../h/sync.h"
#include "../h/region.h"

#include "../h/cpu.h"
#include "../h/inode.h"
#include "../h/sysdirent.h"
#include "../h/timeout.h"
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
entry INODE	*itnet_debug_ip;	/* Arquivo de depuração */

/*
 ****************************************************************
 *	Envia um pacote (frame) 				*
 ****************************************************************
 */
void
send_frame (ITBLOCK *bp)
{
	ROUTE		*rp;
	ETH_H		*ep;
	IPADDR		ip_addr;

	/*
	 *	Verifica se o destino é um dispositivo ETHERNET
	 *	e (se for), se o cabeçalho ETHERNET já está pronto
	 */
	rp = bp->it_route;

	if (!rp->r_ether_dev || bp->it_ether_header_ready)
		{ route_frame (bp); return; }

	/*
	 *	Prepara o cabeçalho ETHERNET
	 */
	ep = (ETH_H *)(bp->it_u_area - sizeof (ETH_H));

	if ((ip_addr = rp->r_gateway_addr) == 0)
		ip_addr = bp->it_dst_addr;

	if (ether_get_ether_addr (ip_addr, &ep->it_ether_dst) >= 0)
	{
	    route:
		ep->it_ether_src = rp->r_my_ether_addr;
		ep->it_type = ENDIAN_SHORT (ETHERTYPE_IP);

		bp->it_ether_header_ready = 1;

		bp->it_u_area   = ep;
		bp->it_u_count += sizeof (ETH_H);

		route_frame (bp); return;
	}

	/*
	 *	Ainda não tem o endereço ETHERNET: envia o pedido ARP
	 */
	if (ether_send_arp_request (bp, ip_addr) < 0 || !bp->it_wait_for_arp)
		goto drop;

	/*
	 *	Espera uma fração de segundo e tenta novamente
	 */
	EVENTCLEAR (&bp->it_arp_event);

	toutset (scb.y_hz >> 4, eventdone, (int)&bp->it_arp_event);

	EVENTWAIT (&bp->it_arp_event, PITNETOUT);

	if (ether_get_ether_addr (ip_addr, &ep->it_ether_dst) >= 0)
		goto route;

	/*
	 *	Descarta o datagrama
	 */
    drop:
	if (bp->it_free_after_IO)
		put_it_block (bp);

}	/* send_frame */

/*
 ****************************************************************
 *	Entrega o pacote (frame) ao dispositivo 		*
 ****************************************************************
 */
void
route_frame (ITBLOCK *bp)
{
	INODE		*ip;
	IOREQ		io;

	/*
	 *	Atualiza algumas variáveis do bloco
	 */
	bp->it_in_driver_queue = 1;
   	bp->it_n_transmitted++;

#define	ITNET_DEBUG
#ifdef	ITNET_DEBUG
	/*
	 *	Verifica se deve escrever o pacote no arquivo "itnet.debug"
	 */
	if (itscb.it_write_output)
	{
		int		frame_count;
		IOREQ		*iop;
		DIRENT		*dep;

		ip = itnet_debug_ip;
		u.u_error = NOERR;

		iop = alloca (sizeof (IOREQ));

		if (ip == NOINODE)
		{
			dep = alloca (sizeof (DIRENT));

			if ((ip = iname ("itnet.debug", getschar, ICREATE, iop, dep)) == NOINODE)
			{
				if (u.u_error != NOERR)
					goto send;

				if ((ip = (*iop->io_ip->i_fsp->fs_make_inode) (iop, dep, 0666)) == NOINODE)
					goto send;
			}

			itnet_debug_ip = ip;
		}
		else
		{
			SLEEPLOCK (&ip->i_lock, PINODE);
		}

		if
		(	kaccess (ip, IWRITE) == 0 &&
			(ip->i_mode & IFMT) == IFREG &&
			u.u_euid == u.u_ruid
		)
		{
			frame_count = bp->it_u_count;

		   /***	iop->io_fd	   = ...;	***/
		   /***	iop->io_fp	   = ...;	***/
			iop->io_ip	   = ip;
		   /***	iop->io_dev	   = ...;	***/
			iop->io_area	   = &frame_count;
			iop->io_count	   = sizeof (frame_count);
			iop->io_offset_low = ip->i_size;
			iop->io_cpflag	   = SS;
		   /***	iop->io_rablock	   = ...;	***/

			(*ip->i_fsp->fs_write) (iop);

		   /***	iop->io_fd	   = ...;	***/
		   /***	iop->io_fp	   = ...;	***/
		   /***	iop->io_ip	   = ip;	***/
		   /***	iop->io_dev	   = ...;	***/
			iop->io_area	   = bp->it_u_area;
			iop->io_count	   = frame_count;
			iop->io_offset_low = ip->i_size; 
		   /***	iop->io_cpflag	   = SS;	***/
		   /***	iop->io_rablock	   = ...;	***/

			(*ip->i_fsp->fs_write) (iop);
		}

		SLEEPFREE (&ip->i_lock);
	}

    send:
#endif	ITNET_DEBUG

	/*
	 *	Verifica se o destino é o próprio computador
	 */
	if (bp->it_route->r_inode == NOINODE)
	{
		ITBLOCK	*cbp;

		if ((cbp = get_it_block (IT_IN)) == NOITBLOCK)
		{
#ifdef	MSG
			printf ("%g: NÃO obtive bloco\n");
#endif	MSG
			bp->it_in_driver_queue = 0;
		   	bp->it_time = time;

			if (bp->it_free_after_IO)
				put_it_block (bp);

			return;
		}

		unimove (cbp->it_frame, bp->it_u_area, bp->it_u_count, SS);

		cbp->it_u_count   = bp->it_u_count;
		cbp->it_u_area    = cbp->it_frame;
		cbp->it_ether_dev = 0;
		cbp->it_ppp_dev   = 0;

		bp->it_in_driver_queue = 0;
	   	bp->it_time = time;

		if (bp->it_free_after_IO)
			put_it_block (bp);

		wake_up_in_daemon (IN_BP, cbp);

		return;

	}	/* O destino é o próprio computador */

	/*
	 *	Envia o pacote para o dispositivo de saída
	 */
	ip = bp->it_route->r_inode;

   /***	io.io_fd	 = ...;	***/
   /***	io.io_fp	 = ...;	***/
	io.io_ip	 = ip;
   /***	io.io_dev	 = ...;	***/
	io.io_area	 = bp;
	io.io_count	 = 1;	/* Não utilizado pelo "driver" */
	io.io_offset_low = 0;	/* Não utilizado pelo "driver" */
	io.io_cpflag	 = SS;
   /***	io.io_rablock	 = ...;	***/

	(*ip->i_fsp->fs_write) (&io);

   	bp->it_time = time;

}	/* end route_frame */

/*
 ****************************************************************
 *	Entrada geral de pacotes (ETHERNET ou não)		*
 ****************************************************************
 */
void
receive_frame (ITBLOCK *bp)
{
	ETH_H		*ep;

#ifdef	ITNET_DEBUG
	/*
	 *	Verifica se deve gravar o datagrama no disco
	 */
	if (itscb.it_write_input)
	{
		INODE		*ip = itnet_debug_ip;
		int		frame_count;
		IOREQ		io;
		DIRENT		*dep;

		u.u_error = NOERR;

		if (ip == NOINODE)
		{
			dep = alloca (sizeof (DIRENT));

			if ((ip = iname ("itnet.debug", getschar, ICREATE, &io, dep)) == NOINODE)
			{
				if (u.u_error != NOERR)
					return;

				if ((ip = (*io.io_ip->i_fsp->fs_make_inode) (&io, dep, 0666)) == NOINODE)
					return;
			}

			itnet_debug_ip = ip;
		}
		else
		{
			SLEEPLOCK (&ip->i_lock, PINODE);
		}

		if
		(	kaccess (ip, IWRITE) == 0 &&
			(ip->i_mode & IFMT) == IFREG &&
			u.u_euid == u.u_ruid
		)
		{
			frame_count = bp->it_u_count;

		   /***	io.io_fd	 = ...;	***/
		   /***	io.io_fp	 = ...;	***/
			io.io_ip	 = ip;
		   /***	io.io_dev	 = ...;	***/
			io.io_area	 = (char *)&frame_count;
			io.io_count	 = sizeof (frame_count);
			io.io_offset_low = ip->i_size;
			io.io_cpflag	 = SS;
		   /***	io.io_rablock	 = ...;	***/

			(*ip->i_fsp->fs_write) (&io);


		   /***	io.io_fd	 = ...;	***/
		   /***	io.io_fp	 = ...;	***/
			io.io_ip	 = ip;
		   /***	io.io_dev	 = ...;	***/
			io.io_area	 = bp->it_u_area;
			io.io_count	 = frame_count;
			io.io_offset_low = ip->i_size;
			io.io_cpflag	 = SS;
		   /***	io.io_rablock	 = ...;	***/

			(*ip->i_fsp->fs_write) (&io);
		}

		SLEEPFREE (&ip->i_lock);
	}
#endif	ITNET_DEBUG

	/*
	 *	Se for um pacote PPP, ...
	 */
	if (bp->it_ppp_dev)
		{ receive_ppp_packet (bp); return; }

	/*
	 *	Se não for um pacote ETHERNET, processa logo o datagrama IP
	 */
	if (!bp->it_ether_dev)
		{ receive_ip_datagram (bp); return; }

	/*
	 *	É ETHERNET: Verifica o protocolo do pacote
	 */
	ep = bp->it_u_area;

	switch (ENDIAN_SHORT (ep->it_type))
	{
	    case ETHERTYPE_IP:			/* Datagramas IP */
		bp->it_u_area  += sizeof (ETH_H);
		bp->it_u_count -= sizeof (ETH_H);

		receive_ip_datagram (bp);
		return;

	    case ETHERTYPE_ARP:			/* Protocolo ARP */
		ether_receive_arp_frame (bp);
		return;

	    default:				/* Protocolos desconhecidos */
#ifdef	MSG
#if (0)	/*******************************************************/
		if (itscb.it_report_error)
		{
			printf
			(	"%g: Recebendo pacote com protocolo %04X desconhecido\n",
				ENDIAN_SHORT (ep->it_type)
			);
		}
#endif	/*******************************************************/
#endif	MSG
		put_it_block (bp); return;

	}	/* end switch */

}	/* end receive_frame */
