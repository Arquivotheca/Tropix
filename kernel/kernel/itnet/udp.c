/*
 ****************************************************************
 *								*
 *			udp.c					*
 *								*
 *	Funções relacionadas com o protocolo UDP		*
 *								*
 *	Versão	3.0.0, de 07.08.91				*
 *		4.9.0, de 11.04.06				*
 *								*
 *	Funções:						*
 *		send_udp_datagram,    receive_udp_datagram	*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 * 		Copyright © 2005 NCE/UFRJ - tecle "man licença"	*
 * 								*
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
#include "../h/nfs.h"

#undef	UDP_DEBUG
#ifdef	UDP_DEBUG
#include "../h/inode.h"
#include "../h/sysdirent.h"
#endif	UDP_DEBUG

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ******	Protótipos de funções ***********************************
 */
void		write_udp_datagram (int port, void *area, int count);

/*
 ****************************************************************
 *	Envia um datagrama de usuário UDP			*
 ****************************************************************
 */
void
send_udp_datagram (register UDP_EP *up, register ITBLOCK *bp)
{
	register UT_H	*uhp;
	register ROUTE	*rp;

	/*
	 *	Obtém o dispositivo destino e endereço fonte
	 */
	if ((rp = get_route_entry (up->up_snd_addr)) == NOROUTE)
	   	{ put_it_block (bp); u.u_error = TNOROUTE; return; }

	if (up->up_my_addr == 0)
		up->up_my_addr = rp->r_my_addr;

	bp->it_src_addr	= up->up_my_addr;
	bp->it_dst_addr	= up->up_snd_addr;
	bp->it_route	= rp;

	/*
	 *	Prepara o pseudo-cabeçalho UDP
	 */
	uhp = (UT_H *)(bp->it_u_area - sizeof (UT_H));

	uhp->ph.ph_src_addr = ENDIAN_LONG (rp->r_my_addr);
	uhp->ph.ph_dst_addr = ENDIAN_LONG (up->up_snd_addr);
	uhp->ph.ph_proto    = ENDIAN_SHORT (UDP_PROTO);
	uhp->ph.ph_size     = ENDIAN_SHORT (sizeof (UDP_H) + bp->it_u_count);

	/*
	 *	Prepara o cabeçalho UDP
	 */
	uhp->uh.uh_src_port = ENDIAN_SHORT (up->up_my_port);
	uhp->uh.uh_dst_port = ENDIAN_SHORT (up->up_snd_port);
	uhp->uh.uh_size     = ENDIAN_SHORT (sizeof (UDP_H) + bp->it_u_count);
	uhp->uh.uh_checksum = 0;

	/*
	 *	Calcula o "checksum"
	 */
	uhp->uh.uh_checksum = compute_checksum (uhp, sizeof (UT_H) + bp->it_u_count);

	/*
	 *	Envia o datagrama IP
	 */
	bp->it_u_area  -= sizeof (UDP_H);
	bp->it_u_count += sizeof (UDP_H);

	bp->it_free_after_IO = 1;
	bp->it_wait_for_arp = 1;

	send_ip_datagram (UDP_PROTO, bp);

}	/* end send_udp_datagram */

/*
 ****************************************************************
 *	Recebe um datagrama de usuário UDP			*
 ****************************************************************
 */
void
receive_udp_datagram (ITBLOCK *bp, void *area, int count)
{
	register ITSCB	*ip = &itscb;
	register UT_H	*uhp = (UT_H *)(area - sizeof (PSD_H));
	register UDP_EP	*up;
	register ushort	dst_port;

	/*
	 *	Prepara o pseudo-cabeçalho
	 */
	uhp->ph.ph_src_addr = ENDIAN_LONG (bp->it_src_addr);
	uhp->ph.ph_dst_addr = ENDIAN_LONG (bp->it_dst_addr);
	uhp->ph.ph_proto    = ENDIAN_SHORT (UDP_PROTO);
	uhp->ph.ph_size     = ENDIAN_SHORT (count);

	dst_port = ENDIAN_SHORT (uhp->uh.uh_dst_port);

	/*
	 *	Grava o datagrama em "udp.debug"
	 */
#ifdef	UDP_DEBUG
	bp->it_u_area = uhp;

	bp->it_area  = area  + sizeof (UDP_H);
	bp->it_count = count - sizeof (UDP_H);

	write_udp_datagram (dst_port, bp->it_area, bp->it_count);
#endif	UDP_DEBUG

	/*
	 *	Verifica a correção da mensagem.
	 */
	if (compute_checksum (uhp, sizeof (PSD_H) + count) != 0)
	{
		if (uhp->uh.uh_checksum != 0)	/* É opcional! */
		{
#ifdef	MSG
			if (ip->it_report_error)
				printf ("%g: Recebendo datagrama com checksum incorreto\n");
#endif	MSG
			put_it_block (bp); 	return;
		}
	}

	/*
	 *	Verifica se é uma porta tratada pelo núcleo (RPC/NFS/DHCP)
	 */
	switch (dst_port)
	{
	    case PMAP_PORT:
	    case RPC_PORT:
	    case NFS_CLIENT_PORT:
		bp->it_u_area = uhp;

		bp->it_area   = area  + sizeof (UDP_H);
		bp->it_count  = count - sizeof (UDP_H);

		wake_up_nfs_daemon (bp);
		return;

	    case DHCP_CLIENT_PORT:
		bp->it_u_area = uhp;

		bp->it_area   = area  + sizeof (UDP_H);
		bp->it_count  = count - sizeof (UDP_H);

		dhcp_wake_up (bp);
	   /***	put_it_block (bp); ***/
		return;
	}

	/*
	 *	Coloca o datagrama UDP na fila de entrada do EP correspondente
	 */
    again:
	SPINLOCK (&ip->it_uep_lock);

	for (up = ip->it_uep_busy; /* abaixo */; up = up->up_next)
	{
		if (up == NO_UDP_EP)
		{
			SPINFREE (&ip->it_uep_lock);
#ifdef	MSG
			if (ip->it_report_error)
				printf ("%g: Recebendo datagrama para port %d desconhecido\n", dst_port);
#endif	MSG
			/*
			 *	Envia um ICMP de port desconhecido
			 */
			send_icmp_error_message
			(
				bp->it_src_addr,
				ICMP_DST_UNREACH,
				ICMP_PORT_UNREACH,
				bp->it_u_area,
				area - bp->it_u_area + 8
			);

			put_it_block (bp); 	return;
		}

		if (up->up_my_port == dst_port)
			break;
	}

	SPINFREE (&ip->it_uep_lock);

	/*
	 *	Prepara parâmetros sobre os dados
	 */
	bp->it_u_area = uhp;

	bp->it_area   = area  + sizeof (UDP_H);
	bp->it_count  = count - sizeof (UDP_H);

#if (0)	/*******************************************************/
	bp->it_area = (char *)uhp + sizeof (UT_H);
	bp->it_count = ENDIAN_SHORT (uhp->ph.ph_size) - sizeof (UDP_H);
#endif	/*******************************************************/

	/*
	 *	Põe o bloco na fila de entrada
	 */
	SPINLOCK (&up->up_inq_lock);

	if (up->up_my_port != dst_port)
		{ SPINFREE (&up->up_inq_lock); goto again; }

	if (up->up_inq_first == NOITBLOCK)
		up->up_inq_first = bp;
	else
		up->up_inq_last->it_inq_forw.inq_ptr = bp;

	up->up_inq_last = bp;
	bp->it_inq_forw.inq_ptr = NOITBLOCK;

	EVENTDONE (&up->up_inq_nempty);

	if (up->up_attention_set && up->up_uproc->u_attention_index < 0)
	{
		up->up_attention_set = 0;
		up->up_uproc->u_attention_index = up->up_fd_index;
		EVENTDONE (&up->up_uproc->u_attention_event);
	}

	SPINFREE (&up->up_inq_lock);

}	/* end receive_udp_data */

#ifdef	UDP_DEBUG
/*
 ****************************************************************
 *	Grava o datagrama UDP					*
 ****************************************************************
 */
void
write_udp_datagram (int dst_port, void *area, int count)
{
	INODE		*ip;
	static INODE	*udp_debug_ip;
	DIRENT		de;
	IOREQ		io;

	/*
	 *	Verifica se deve escrever o pacote no arquivo "udp.debug"
	 */
	printf ("%g: (porta %d) Escrevendo %d bytes\n", dst_port, count);

	ip = udp_debug_ip; u.u_error = NOERR;

	if (ip == NOINODE)
	{
		if ((ip = iname ("udp.debug", getschar, ICREATE, &io, &de)) == NOINODE)
		{
			if (u.u_error != NOERR)
				{ printf ("%g: NÃO consegui abrir \"udp.debug\" (%r)\n", u.u_error); return ; }

			if ((ip = (*io.io_ip->i_fsp->fs_make_inode) (&io, &de, 0666)) == NOINODE)
				{ printf ("%g: Erro na escrita de \"udp.debug\" (%r)\n", u.u_error); return ; }
		}

		udp_debug_ip = ip;
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
	   /***	io.io_fd	   = ...;	***/
	   /***	io.io_fp	   = ...;	***/
		io.io_ip	   = ip;
	   /***	io.io_dev	   = ...;	***/
		io.io_area	   = &count;
		io.io_count	   = sizeof (count);
		io.io_offset_low   = ip->i_size;
		io.io_cpflag	   = SS;
	   /***	io.io_rablock	   = ...;	***/

		(*ip->i_fsp->fs_write) (&io);

	   /***	io.io_fd	   = ...;	***/
	   /***	io.io_fp	   = ...;	***/
	   /***	io.io_ip	   = ip;	***/
	   /***	io.io_dev	   = ...;	***/
		io.io_area	   = area;
		io.io_count	   = count;
		io.io_offset_low   = ip->i_size; 
	   /***	io.io_cpflag	   = SS;	***/
	   /***	io.io_rablock	   = ...;	***/

		(*ip->i_fsp->fs_write) (&io);
	}

	SLEEPFREE (&ip->i_lock);

}	/* end write_udp_datagram */
#endif	UDP_DEBUG
