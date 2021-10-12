/*
 ****************************************************************
 *								*
 *			ip.c					*
 *								*
 *	Funções relacionadas com o protocolo IP			*
 *								*
 *	Versão	3.0.0, de 07.08.91				*
 *		4.9.0, de 09.08.06				*
 *								*
 *	Funções:						*
 *		send_ip_datagram,    route_ip_datagram,		*
 *		receive_ip_datagram, do_gateway_service		*
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

#include "../h/inode.h"
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
#define	IS_TIME	((hz & 0x1E) == 0x10)	/* Macro para os ticks do relógio */

/*
 ****** Protótipos de funções ***********************************
 */
int		do_gateway_service (ITBLOCK *);

/*
 ****************************************************************
 *	Envia um datagrama IP					*
 ****************************************************************
 */
void
send_ip_datagram (PROTO_ENUM proto, ITBLOCK *bp)
{
	IP_H		*iph;
	int		ip_h_size;
	static int	source_ip_id;

	/*
	 *		Prepara o cabeçalho IP
	 *
	 *	Neste ponto, deve-se decidir o tamanho do cabeçalho IP;
	 *	no momento estamos usando o tamanho mínimo
	 *	(sem o "ip_extension")
	 *
 	 */
	ip_h_size = sizeof (IP_H) - sizeof (iph->ip_extension);

	/*
	 *	Preenche o cabeçalho IP
	 */
	bp->it_ip_header = iph = (IP_H *)(bp->it_u_area - ip_h_size);

	memclr (iph, ip_h_size);		/* Zera toda a entrada */

	iph->ip_version = IP_VERSION;
	iph->ip_h_size = ip_h_size >> 2;	/* Tamanho em longs */
   /***	iph->ip_precedence = 0; ***/
   /***	iph->ip_low_delay = 0; ***/
   /***	iph->ip_high_th_put = 0; ***/
   /***	iph->ip_high_rely = 0; ***/
   /***	iph->ip_reser0 = 0; ***/
	iph->ip_size = ENDIAN_SHORT (bp->it_u_count + ip_h_size);

	iph->ip_id = ENDIAN_SHORT (++source_ip_id);
   /***	iph->ip_reser1 = 0; ***/
   /***	iph->ip_do_not_fragment = 0; ***/
   /***	iph->ip_last_fragment = 0; ***/
   /***	iph->ip_fragment_off = 0; ***/

	iph->ip_time_to_live = 60;
	iph->ip_proto = proto;
   /***	iph->ip_h_checksum = 0; ***/

	iph->ip_src_addr = ENDIAN_LONG (bp->it_src_addr);
	iph->ip_dst_addr = ENDIAN_LONG (bp->it_dst_addr);

   	iph->ip_h_checksum = compute_checksum (iph, ip_h_size);

	/*
	 *	Envia a mensagem
	 */
	bp->it_ether_header_ready = 0;
   /***	bp->it_wait_for_arp = ...; /* Definido pelo usuário desta função ***/

	bp->it_u_area   = iph;
	bp->it_u_count += ip_h_size;

	send_frame (bp);

}	/* end send_ip_datagram */

/*
 ****************************************************************
 *	Recebe um datagrama IP					*
 ****************************************************************
 */
void
receive_ip_datagram (ITBLOCK *bp)
{
	ITSCB		*ip = &itscb;
	IP_H		*iph;
	ROUTE		*rp;
	void		*area;
	int		ip_h_size, count;
	IPADDR		dst_addr;

	/*
	 *	Confere a versão do IP
	 */
	bp->it_ip_header = iph = bp->it_u_area;

	ip_h_size = iph->ip_h_size << 2;	/* Tamanho longs => bytes */

	if (iph->ip_version != IP_VERSION)
	{
#ifdef	MSG
		if (ip->it_report_error)
			printf ("%g: Recebendo datagrama com versão %d incorreta\n", iph->ip_version);
#endif	MSG
		put_it_block (bp); return;
	}

	/*
	 *	Verifica se deve "achar" que o checksum está errado
	 */
	if (ip->it_gen_chk_err  &&  IS_TIME)
	   	iph->ip_h_checksum++;

	/*
	 *	Confere o "checksum" do cabeçalho do datagrama
	 */
   	if (compute_checksum (iph, ip_h_size) != 0)
	{
#ifdef	MSG
		if (ip->it_report_error)
			printf ("%g: Recebendo datagrama com checksum incorreto\n");
#endif	MSG
		put_it_block (bp); return;
	}

	/*
	 *	Verifica se o tamanho é suficiente
	 */
	if (bp->it_u_count < ENDIAN_SHORT (iph->ip_size))
	{
#ifdef	MSG
		if (ip->it_report_error)
		{
			printf
			(	"%g: Tamanho do datagrama insuficiente: %d :: %d\n",
				bp->it_u_count, ENDIAN_SHORT (iph->ip_size)
			);
		}
#endif	MSG
		put_it_block (bp); return;
	}

	/*
	 *	Verifica se é um fragmento
	 */
	if (iph->ip_last_fragment != 0)
	{
#ifdef	MSG
		if (ip->it_report_error)
			printf ("%g: Recebendo datagrama fragmentado\n");
#endif	MSG
		put_it_block (bp); return;
	}

	/*
	 *	Verifica e ignora MULTICAST
	 */
	dst_addr = ENDIAN_LONG (iph->ip_dst_addr);

	if ((dst_addr & 0xF0000000) == 0xE0000000)
	{
#ifdef	MSG
		if (ip->it_report_error)
			printf ("%g: ignorando pacote MULTICAST\n");
#endif	MSG
		put_it_block (bp); return;
	}

	/*
	 *	Verifica se o destino do datagrama está correto
	 */
	for (rp = scb.y_route; /* abaixo */; rp++)
	{
		IPADDR		not_mask;

		if (rp->r_dev_nm[0] == '\0')
		{
			if (ip->it_gateway && do_gateway_service (bp) >= 0)
				return;
#ifdef	MSG
			if (ip->it_report_error)
			{
				printf ("%g: Recebendo datagrama para endereço ");
				pr_itn_addr (dst_addr);
				printf (" desconhecido\n");
			}
#endif	MSG
			/*
			 *	Envia um ICMP de nó desconhecido
			 */
			send_icmp_error_message
			(
				ENDIAN_LONG (iph->ip_src_addr),
				ICMP_DST_UNREACH,
				ICMP_HOST_UNREACH,
				bp->it_u_area,
				ip_h_size + 8	/* 8 bytes => 64 bits */
			);

			put_it_block (bp); return;

		}	/* end if (final da tabela) */

		if (dst_addr == rp->r_my_addr)			/* Achou */
			break;

		if ((not_mask = ~rp->r_mask) == 0)		/* Verifica se é BROADCAST */
			continue;

		if ((dst_addr & not_mask) == not_mask)
		{
#ifdef	MSG
			if (CSWT (36))
			{
				printf ("%g: Recebendo BROADCAST de ");
				pr_itn_addr (ENDIAN_LONG (iph->ip_src_addr));
				printf (", proto = %d\n", iph->ip_proto);
			}
#endif	MSG
#if (0)	/*******************************************************/
			if (iph->ip_proto == UDP_PROTO)
			{
				UDP_H   *uhp = (UDP_H *)((int)iph + ip_h_size);
				int	port = ENDIAN_SHORT (uhp->uh_dst_port);

				if (port == DHCP_CLIENT_PORT)
					break;
			}
#endif	/*******************************************************/

			put_it_block (bp); return;		/* Por enquanto, ignora */
		}

	}	/* end percorrendo a ROUTE table procurando o destino */

	bp->it_src_addr = ENDIAN_LONG (iph->ip_src_addr);
	bp->it_dst_addr = dst_addr;
	bp->it_route	= rp;

	/*
	 *	Localiza a área de dados
	 */
	area = (char *)iph + ip_h_size;
	count = ENDIAN_SHORT (iph->ip_size) - ip_h_size;

	/*
	 *	Identifica o protocolo
	 */
	switch (iph->ip_proto)
	{
	    case UDP_PROTO:
		receive_udp_datagram (bp, area, count);
		return;

	    case TCP_PROTO:
		receive_tcp_packet (bp, area, count);
		return;

	    case ICMP_PROTO:
		receive_icmp_datagram (bp, area, count);
		break;

	}	/* end switch */

	/*
	 *	Para protocolos desconhecidos (talvez experimentais)
	 */
	if (ip->it_rep_busy != NO_RAW_EP)
	{
		if (receive_raw_datagram (bp, iph->ip_proto, area, count) > 0)
			{ put_it_block (bp); return; }
	}

	if (iph->ip_proto == ICMP_PROTO)
		{ put_it_block (bp); return; }

	/*
	 *	Protocolo desconhecido
	 */
#ifdef	MSG
	if (ip->it_report_error)
		printf ("%g: Recebendo datagrama com protocolo %d desconhecido\n", iph->ip_proto);
#endif	MSG

	/*
	 *	Envia um ICMP de protocolo desconhecido
	 */
	send_icmp_error_message
	(
		bp->it_src_addr,
		ICMP_DST_UNREACH,
		ICMP_PROTOCOL_UNREACH,
		bp->it_u_area,
		ip_h_size + 8	/* 8 bytes => 64 bits */
	);

	put_it_block (bp);

}	/* end receive_ip_datagram */

/*
 ****************************************************************
 *	Executa o papel de "gateway"				*
 ****************************************************************
 */
int
do_gateway_service (ITBLOCK *bp)
{
	ROUTE		*rp;
	IPADDR		dst_addr;
	IP_H		*iph = (IP_H *)bp->it_u_area;
	int		time_to_live_and_proto;

	/*
	 *	Verifica se o destino do datagrama é outro nó da rede
	 */
	bp->it_dst_addr = dst_addr = ENDIAN_LONG (iph->ip_dst_addr);

	if ((rp = get_route_entry (dst_addr)) == NOROUTE)
		return (-1);

	/*
	 *	Verifica se o TTL foi esgotado
	 */
	if (iph->ip_time_to_live <= 1)
	{
		/* Envia um ICMP de TTL esgotado */

		send_icmp_error_message
		(
			ENDIAN_LONG (iph->ip_src_addr),
			ICMP_TIME_EXCEEDED,
			0,				/* In Transit */
			bp->it_u_area,
			(iph->ip_h_size << 2) + 8	/* 8 bytes => 64 bits */
		);

		put_it_block (bp); return (0);
	}

	/*
	 *	Recalcula o "checksum"
	 *
	 *	Repare que o cálculo abaixo leva em conta a posição
	 *	do TTL na estrutura e o fato de ser "little-endian".
	 */
	time_to_live_and_proto = (iph->ip_time_to_live | (iph->ip_proto << 8));

	iph->ip_h_checksum = incremental_checksum
	(
		iph->ip_h_checksum,
		time_to_live_and_proto, 
		time_to_live_and_proto - 1
	);

	iph->ip_time_to_live--;

#define	DEBUG
#ifdef	DEBUG
	{
		ushort	calculated_checksum = iph->ip_h_checksum, correct_checksum;

		iph->ip_h_checksum = 0;

	   	correct_checksum = compute_checksum (iph, iph->ip_h_size << 2);

		if (calculated_checksum != correct_checksum)
		{
			printf
			(	"%g: Recálculo incorreto do \"checksum\": %04X :: %04X\n",
				calculated_checksum, correct_checksum
			);
		}

		iph->ip_h_checksum = correct_checksum;
	}
#endif	DEBUG

#ifdef	MSG
	if (CSWT (35))
		{ printf ("%g: Redirecionando para "); pr_itn_addr (dst_addr); printf ("\n"); }
#endif	MSG

	/*
	 *	Encontrou o nó seguinte
	 */
   	bp->it_n_transmitted = 0;
	bp->it_free_after_IO = 1;
	bp->it_ether_header_ready = 0;
	bp->it_wait_for_arp = 0;

	bp->it_route = rp;

   /***	bp->it_u_area  = bp->it_u_area; ***/
   /***	bp->it_u_count = bp->it_u_count; ***/

	send_frame (bp);

	return (0);

}	/* end do_gateway_service */

/*
 ****************************************************************
 *	Roteamento de datagramas IP				*
 ****************************************************************
 */
ROUTE *
get_route_entry (IPADDR dst_addr)
{
	ROUTE		*rp;

	for (rp = scb.y_route; /* abaixo */; rp++)
	{
		if (rp->r_dev_nm[0] == '\0')
			return (NOROUTE);

		if ((dst_addr & rp->r_mask) == rp->r_net_addr)
			return (rp);
	}

}	/* end get_route_entry */
