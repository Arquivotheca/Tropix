/*
 ****************************************************************
 *								*
 *			dhcp.c					*
 *								*
 *	Funções relacionadas ao protocolo DHCP			*
 *								*
 *	Versão	4.9.0, de 12.04.06				*
 *		4.9.0, de 02.10.06				*
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
#include "../h/timeout.h"
#include "../h/itnet.h"

#include "../h/extern.h"
#include "../h/proto.h"

#include <stdarg.h>

/*
 ****************************************************************
 *	Definições relativas ao protocolo DHCP			*
 ****************************************************************
 */
enum
{
	DHCP_TIMEOUT		= 5,				/* Tempo para retransmissão */
	DHCP_MIN_LEN		= 300,				/* Tamanho mínimo do pacote */
	DHCP_BROADCAST		= 0x8000,			/* Indicador de difusão */
	DHCP_MAGIC		= 0x63538263,			/* Número mágico */
	DHCP_CLIENT_ID_SZ	= 1 + sizeof (ETHADDR)		/* Tamanho da Identificação do cliente */
};

enum								/* Tipos das mensagens */
{
	DHCP_NO_MSG,
	DHCP_DISCOVER,
	DHCP_OFFER,
	DHCP_REQUEST,
	DHCP_DECLINE,
	DHCP_ACK,
	DHCP_NAK,
	DHCP_RELEASE,
	DHCP_INFORM
};

/*
 ******	Cabeçalho DHCP ******************************************
 */
typedef struct
{
	char		dh_op;					/* Tipo do pacote */
	char		dh_htype;				/* Tipo da ethernet */
	char		dh_hlen;				/* Tamanho do endereço ethernet */
	char		dh_hops;				/* No. de pulos */
	ulong		dh_xid;					/* Identificação do pedido */
	ushort		dh_secs;				/* Duração do pedido (em seg.) */
	ushort		dh_flags;				/* Indicadores */

	ulong		dh_ciaddr;				/* Endereço IP do cliente */
	ulong		dh_yiaddr;				/* Meu Endereço IP */
	ulong		dh_siaddr;				/* Próximo Endereço IP do servidor */
	ulong		dh_giaddr;				/* Outro Endereço IP */

	ETHADDR		dh_chaddr;				/* Endereço ethernet do cliente */
	char		dh_reser1[10];

	char		dh_sname[64];				/* Nome do servidor */
	char		dh_file[128];				/* Nome do arquivo de "boot" */

}	DHCP_H;

/*
 ******	Informações extraídas das mensagens DHCP ****************
 */
typedef struct
{
	ulong		dh_xid;					/* Identificação da mensagem */
	int		dh_msg_type;				/* Tipo da mensagem */

	char		dh_host_nm[16],				/* Nome da máquina */
			dh_domain_nm[32];			/* Domínio */

	int		dh_dns_index;				/* Endereços IP de servidores de nomes */
	IPADDR		dh_dns_server[DNS_SERVER_CNT + 1];

	IPADDR		dh_dhcp_server_ipaddr,			/* Endereço IP do servidor DHCP */
			dh_my_ipaddr,				/* Endereço IP sugerido */
			dh_gateway_ipaddr,			/* Endereço do roteador */
		   	dh_broadcast_addr,			/* Endereço IP para difusão local */
			dh_mask;				/* Máscara da subrede */

	time_t		dh_lease_time,				/* "Lease time" */
			dh_renewal_time,			/* "Renewal time" */
			dh_rebinding_time;			/* "Rebinding time" */

	char		dh_client_id[DHCP_CLIENT_ID_SZ];	/* Identificação */

}	DHCP_INFO;

/*
 ****************************************************************
 *	Definições Globais					*
 ****************************************************************
 */

#ifdef	DEBUG
/*
 ******	Nomes das mensagens DHCP ********************************
 */
const char	* const dhcp_msg_type[] =
{
	"",
	"DISCOVER",
	"OFFER",
	"REQUEST",
	"DECLINE",
	"ACK",
	"NAK",
	"RELEASE",
	"INFORM"
};
#endif	DEBUG

/*
 ******	Estados do Protocolo DHCP *******************************
 */
enum
{
	DHCP_INIT,
	DHCP_SELECTING,
	DHCP_REQUESTING,
	DHCP_BOUND,
	DHCP_REBINDING,
	DHCP_RENEWING,
	DHCP_REBOOTING
};

/*
 ******	Protótipos de funções ***********************************
 */
int		dhcp_send_request (ROUTE *rp, ulong xid, int msg_type, ...);
int		dhcp_wait_for_answer (ROUTE *rp, DHCP_INFO *ip);
int		dhcp_get_info (ITBLOCK *bp, DHCP_INFO *ip);
void		dhcp_print_info (const DHCP_INFO *ip);

extern IPADDR	nm_serv_tb[DNS_SERVER_CNT + 1];

/*
 ****************************************************************
 *	Daemon DHCP						*
 ****************************************************************
 */
void
dhcp_daemon (ROUTE *rp)
{
	int		state = DHCP_INIT;
	ulong		xid;
	IPADDR		server_ipaddr = 0;
	DHCP_INFO	d;

	/*
	 *	Pequeno prelúdio
	 */
	rp->r_my_addr = -1;		/* O Servidor envia este endereço IP */

	EVENTCLEAR (&rp->r_dhcp_done);

	CLEAR (&rp->r_dhcp_lock);

	xid = *(ulong *)&rp->r_my_ether_addr;

	/*
	 *	Máquina de Estados do Protocolo DHCP
	 */
	for (EVER) switch (state)
	{
	    case DHCP_INIT:

		/* Envia DHCP_DISCOVER e espera DHCP_OFFER */

		dhcp_send_request (rp, xid, DHCP_DISCOVER);

		if (dhcp_wait_for_answer (rp, &d) != DHCP_OFFER || d.dh_xid != xid)
			break;

		server_ipaddr = d.dh_dhcp_server_ipaddr;

		state = DHCP_SELECTING;
	   /***	break; ***/

	    case DHCP_SELECTING:

		/* Envia DHCP_REQUEST e espera DHCP_ACK */

		dhcp_send_request (rp, d.dh_xid, DHCP_REQUEST, server_ipaddr, d.dh_my_ipaddr);

		while (dhcp_wait_for_answer (rp, &d) == DHCP_OFFER)
			/* descarta */;

		if (d.dh_msg_type != DHCP_ACK || d.dh_xid != xid)
			{ state = DHCP_INIT; break; }

		state = DHCP_BOUND;
	   /***	break; ***/

	    case DHCP_BOUND:

		/* Preenche a entrada ROUTE principal */

		rp->r_mask		= d.dh_mask;
		rp->r_net_addr		= d.dh_my_ipaddr & d.dh_mask;

		rp->r_my_addr		= d.dh_my_ipaddr;
		rp->r_gateway_addr	= 0;

		/* Preenche a entrada ROUTE "default" */

		if (rp[1].r_default)
		{
			rp[1].r_mask		= 0;
			rp[1].r_net_addr	= 0;

			rp[1].r_my_addr		= d.dh_my_ipaddr;
			rp[1].r_gateway_addr	= d.dh_gateway_ipaddr;
		}

		if (d.dh_domain_nm[0] != '\0' && !streq (domain_name, d.dh_domain_nm))
		{
			printf
			(	"%g: CONFLITO envolvendo o nome do domínio: (\"%s\" :: \"%s\")\n",
				domain_name, d.dh_domain_nm
			);
		}

		if (d.dh_dns_index > 0)
		{
			memmove (nm_serv_tb, d.dh_dns_server, sizeof (nm_serv_tb));

			EVENTDONE (&dns_server_table_not_empty_event);
		}

printf ("*** DHCP Concluído: Endereço IP = %s ***\n", edit_ipaddr (rp->r_my_addr));
pause ();

	}	/* end for (EVER) switch (state) */

}	/* end dhcp_daemon */

/*
 ****************************************************************
 *	Envia um pedido DHCP					*
 ****************************************************************
 */
int
dhcp_send_request (ROUTE *rp, ulong xid, int msg_type, ...)
{
	ITBLOCK		*bp;
	ETH_H		*ep;
	IP_H		*iph;
	UT_H		*uhp;
	DHCP_H		*dhp;
	int		dhp_size, ip_h_size, len;
	char		*cp;
	va_list		ap;
	static int	source_ip_id;

	/*
	 *	Obtém um novo ITBLOCK
	 */
	if ((bp = get_it_block (IT_OUT_CTL)) == NOITBLOCK)
	{
#ifdef	MSG
		printf ("%g: NÃO obtive bloco\n");
#endif	MSG
		return (-1);
	}

	bp->it_route = rp;

	/*
	 *	Determina os endereços iniciais dos cabeçalhos
	 */
	ip_h_size = sizeof (IP_H) - sizeof (iph->ip_extension);

	ep  = (ETH_H *)bp->it_frame;

	iph = (IP_H *)((int)ep + sizeof (ETH_H));

	uhp = (UT_H *)((int)iph + ip_h_size - sizeof (PSD_H));

	/*
	 *	Prepara o pacote DHCP
	 */
	dhp = (DHCP_H *)((int)uhp + sizeof (UT_H));

	memclr (dhp, sizeof (DHCP_H));

	dhp->dh_op	= 1;			/* Tipo do pacote == BOOT REQUEST */
	dhp->dh_htype	= 1;			/* Tipo da ethernet */
	dhp->dh_hlen	= 6;			/* Tamanho do endereço ethernet */
   /***	dhp->dh_hops	= 0;	***/		/* No. de pulos */

	dhp->dh_xid	= xid;			/* Identificação do pedido */

   /***	dhp->dh_secs	= 0;	***/		/* Duração do pedido (em seg.) */
	dhp->dh_flags	= ENDIAN_SHORT (DHCP_BROADCAST);	/* Indicadores */

   /***	dhp->dh_ciaddr	= 0;	***/		/* Endereço IP do cliente */
   /***	dhp->dh_yiaddr	= 0;	***/		/* Meu Endereço IP */
   /***	dhp->dh_siaddr	= 0;	***/		/* Próximo Endereço IP do servidor */
   /***	dhp->dh_giaddr	= 0;	***/		/* Outro Endereço IP */

#if (0)	/********************************************************/
	dhp->dh_chaddr	= rp->r_my_ether_addr;	/* Endereço ethernet do cliente */
#endif	/********************************************************/

   /***	dhp->dh_sname	= ...;	***/		/* Nome do servidor */
   /***	dhp->dh_file	= ...;	***/		/* Nome do arquivo de "boot" */

	/*
	 *	Preenche as opções
	 */
	cp = (char *)(dhp + 1);

	*(ulong *)cp = DHCP_MAGIC;		/* Número mágico */
	cp += sizeof (ulong);

	*cp++ = 53;				/* Opção MSG TYPE ... */
	*cp++ = 1;
	*cp++ = msg_type;			/* ... tipo dado */

	*cp++ = 61;				/* Opção CLIENT IDENTIFIER */
	*cp++ = DHCP_CLIENT_ID_SZ;
	*cp++ = dhp->dh_htype;
	*(ETHADDR *)cp = dhp->dh_chaddr;
	cp += DHCP_CLIENT_ID_SZ - 1;

	*cp++ = 12;				/* Opção HOST NAME */
	*cp++ = len = strlen (scb.y_uts.uts_nodename);
	memmove (cp, scb.y_uts.uts_nodename, len);
	cp += len;

	*cp++ = 60;				/* Opção VENDOR Class ID */
	*cp++ = 6;
	memmove (cp, "TROPIX", 6);
	cp += 6;

	*cp++ = 55;				/* Opção PARAMETER REQUEST List */
	*cp++ = 10;
	*cp++ = 1;				/*	subnet mask		*/
	*cp++ = 3;				/*	gateway address		*/
	*cp++ = 6;				/*	DNS			*/
	*cp++ = 12;				/*	host name		*/
	*cp++ = 15;				/*	domain name		*/
	*cp++ = 28;				/*	broadcast address	*/
	*cp++ = 51;				/*	lease time		*/
	*cp++ = 54;				/*	DHCP server address	*/
	*cp++ = 58;				/*	renewal time		*/
	*cp++ = 59;				/*	rebiding time		*/

	va_start (ap, msg_type);

	switch (msg_type)
	{
	    case DHCP_DISCOVER:
		break;

	    case DHCP_REQUEST:
		*cp++ = 54;			/* Opção SERVER IDENTIFIER */
		*cp++ = 4;
		*(IPADDR *)cp = ENDIAN_LONG (va_arg (ap, IPADDR));
		cp += sizeof (IPADDR);

		*cp++ = 50;			/* Opção REQUESTED IP ADDRESS */
		*cp++ = 4;
		*(IPADDR *)cp = ENDIAN_LONG (va_arg (ap, IPADDR));
		cp += sizeof (IPADDR);
		break;
	}

	va_end (ap);

	*cp++ = 255;				/* Fim das opções */

	if ((int)cp - (int)dhp < DHCP_MIN_LEN)
		cp = (char *)dhp + DHCP_MIN_LEN;

	dhp_size = (int)cp - (int)dhp;

	/*
	 *	Prepara o pseudo-cabeçalho UDP
	 */
	uhp->ph.ph_src_addr = ENDIAN_LONG (0);
	uhp->ph.ph_dst_addr = ENDIAN_LONG (IP_BROADCAST);
	uhp->ph.ph_proto    = ENDIAN_SHORT (UDP_PROTO);
	uhp->ph.ph_size     = ENDIAN_SHORT (sizeof (UDP_H) + dhp_size);

	/*
	 *	Prepara o cabeçalho UDP
	 */
	uhp->uh.uh_src_port = ENDIAN_SHORT (DHCP_CLIENT_PORT);
	uhp->uh.uh_dst_port = ENDIAN_SHORT (DHCP_SERVER_PORT);
	uhp->uh.uh_size     = ENDIAN_SHORT (sizeof (UDP_H) + dhp_size);
	uhp->uh.uh_checksum = 0;

	/*
	 *	Calcula o "checksum"
	 */
	uhp->uh.uh_checksum = compute_checksum (uhp, sizeof (UT_H) + dhp_size);

	/*
	 *	Completa o cabeçalho IP
	 */
   /***	iph = (IP_H *)((int)ep + sizeof (ETH_H));		***/

   /***	ip_h_size = sizeof (IP_H) - sizeof (iph->ip_extension);	***/

	memclr (iph, ip_h_size);		/* Zera a área do cabeçalho */

	iph->ip_version		= IP_VERSION;
	iph->ip_h_size		= ip_h_size >> 2;
   /***	iph->ip_precedence	= 0; ***/
   /***	iph->ip_low_delay	= 0; ***/
   /***	iph->ip_high_th_put	= 0; ***/
   /***	iph->ip_high_rely	= 0; ***/
   /***	iph->ip_reser0		= 0; ***/
	iph->ip_size		= ENDIAN_SHORT ((int)cp - (int)iph);

	iph->ip_id		= ENDIAN_SHORT (++source_ip_id);
   /***	iph->ip_reser1		= 0; ***/
   /***	iph->ip_do_not_fragment = 0; ***/
   /***	iph->ip_last_fragment	= 0; ***/
   /***	iph->ip_fragment_off	= 0; ***/

	iph->ip_time_to_live	= 60;
	iph->ip_proto		= UDP_PROTO;
   /***	iph->ip_h_checksum	= 0; ***/

	iph->ip_src_addr	= ENDIAN_LONG (0);
	iph->ip_dst_addr	= ENDIAN_LONG (IP_BROADCAST);

   	iph->ip_h_checksum = compute_checksum (iph, ip_h_size);

	/*
	 *	Completa o cabeçalho ETHERNET
	 */
	ep->it_ether_dst	= ether_broadcast_addr;
	ep->it_ether_src	= rp->r_my_ether_addr;
	ep->it_type		= ENDIAN_SHORT (ETHERTYPE_IP);

	/*
	 *	Envia o pacote
	 */
	bp->it_free_after_IO = 1;
   /***	bp->it_ether_header_ready = 1;	***/

	bp->it_u_area   = ep;
	bp->it_u_count  = (int)cp - (int)ep;

#ifdef	DEBUG
	printf ("DHCP: Enviei \"%s\", xid = %u\n", dhcp_msg_type[msg_type], xid);
#endif	DEBUG

	route_frame (bp);

	return (0);

}	/* end dhcp_send_request */

/*
 ****************************************************************
 *	Espera uma resposta					*
 ****************************************************************
 */
int
dhcp_wait_for_answer (ROUTE *rp, DHCP_INFO *dp)
{
	TIMEOUT		*tp;
	ITBLOCK		*bp;

	memclr (dp, sizeof (DHCP_INFO));

	/* Tempo máximo de espera: DHCP_TIMEOUT seg. */

	tp = toutset (DHCP_TIMEOUT * scb.y_hz, eventdone, (int)&rp->r_dhcp_done);

	EVENTWAIT (&rp->r_dhcp_done, PITNETIN);

	EVENTCLEAR (&rp->r_dhcp_done);

	bp = rp->r_dhcp_bp;	rp->r_dhcp_bp = NOITBLOCK;

	CLEAR (&rp->r_dhcp_lock);

	/* Verifica se havia mensagem ou se acordou por decurso de tepmo */

	if (bp == NOITBLOCK)		/* Acordou por decurso de tempo */
		return (DHCP_NO_MSG);

	/* Há um ITBLOCK contendo uma mensagem a analisar */

	toutreset (tp, eventdone, (int)&rp->r_dhcp_done);

	dhcp_get_info (bp, dp);
#ifdef	DEBUG
	dhcp_print_info (dp);
#endif	DEBUG

	return (dp->dh_msg_type);

}	/* end dhcp_wait_for_answer */

/*
 ****************************************************************
 *	Extrai as informaçõe de uma mensagem DHCP		*
 ****************************************************************
 */
int
dhcp_get_info (ITBLOCK *bp, DHCP_INFO *dp)
{
	const DHCP_H	*dhp = (DHCP_H *)bp->it_area;
	const char	*cp, *end_cp;

	/*
	 *	Analisa a parte fixa
	 */
	dp->dh_xid	 = dhp->dh_xid;
	dp->dh_my_ipaddr = ENDIAN_LONG (dhp->dh_yiaddr);
	dp->dh_msg_type  = 0;

	/*
	 *	Analisa as opções, guardando as mais importantes
	 */
	cp = (char *)(dhp + 1);

	if (*(ulong *)cp == DHCP_MAGIC)
		cp += sizeof (ulong);

	for (EVER)
	{
		switch (*cp)
		{
		    case 0:
			cp++;
			break;

		    case 1:
			dp->dh_mask = ENDIAN_LONG (*(long *)(cp + 2));
			goto next_opt;

		    case 3:
			dp->dh_gateway_ipaddr = ENDIAN_LONG (*(long *)(cp + 2));
			goto next_opt;

		    case 6:
			for (cp += 2, end_cp = cp + cp[-1]; cp < end_cp; cp += sizeof (IPADDR))
			{
				if (dp->dh_dns_index < DNS_SERVER_CNT)
					dp->dh_dns_server[dp->dh_dns_index++] = ENDIAN_LONG (*(long *)cp);
			}
			break;

		    case 12:
			strscpy (dp->dh_host_nm, cp + 2, cp[1] + 1);
			goto next_opt;

		    case 15:
			strscpy (dp->dh_domain_nm, cp + 2, cp[1] + 1);
			goto next_opt;

		    case 28:
			dp->dh_broadcast_addr = ENDIAN_LONG (*(long *)(cp + 2));
			goto next_opt;

		    case 51:
			dp->dh_lease_time = ENDIAN_LONG (*(long *)(cp + 2));
			goto next_opt;

		    case 52:
			switch (cp[2])
			{
			    case 1:
				cp = dhp->dh_file;
				break;
			    case 2:
			    case 3:
				cp = dhp->dh_sname;
				break;
			    default:
				cp += cp[1] + 2;
			}
			break;

		    case 53:
			dp->dh_msg_type = cp[2];
			goto next_opt;

		    case 54:
			dp->dh_dhcp_server_ipaddr = ENDIAN_LONG (*(long *)(cp + 2));
			goto next_opt;

		    case 58:
			dp->dh_renewal_time = ENDIAN_LONG (*(long *)(cp + 2));
			goto next_opt;

		    case 59:
			dp->dh_rebinding_time = ENDIAN_LONG (*(long *)(cp + 2));
			goto next_opt;

		    case 61:
			if (cp[1] == DHCP_CLIENT_ID_SZ)
				memmove (dp->dh_client_id, cp + 2, DHCP_CLIENT_ID_SZ);
			goto next_opt;

		    case 255:
			goto out;

		    next_opt:
		    default:
			cp += cp[1] + 2;
			break;
		}
	}

	/*
	 *	Saída
	 */
    out:
	put_it_block (bp);

	return (dp->dh_msg_type);

}	/* end dhcp_get_info */

/*
 ****************************************************************
 *	Acorda o "daemon" DHCP por chegada de datagrama		*
 ****************************************************************
 */
void
dhcp_wake_up (ITBLOCK *bp)
{
	ROUTE		*rp;

	if ((rp = bp->it_route) == NOROUTE || rp->r_dhcp == 0 || TAS (&rp->r_dhcp_lock) < 0)
		{ put_it_block (bp); return; }

	/* Conseguiu dar o TAS: guarda o ITBLOCK e acorda o "daemon" */

	rp->r_dhcp_bp = bp;

	EVENTDONE (&rp->r_dhcp_done);

}	/* end dhcp_wake_up */

#ifdef	DEBUG
/*
 ****************************************************************
 *	Imprime as informações extraídas da mensagem DHCP	*
 ****************************************************************
 */
void
dhcp_print_info (const DHCP_INFO *dp)
{
	printf ("DHCP: Recebi \"%s\", xid = %u\n",  dhcp_msg_type[dp->dh_msg_type], dp->dh_xid);

	if (dp->dh_dhcp_server_ipaddr != 0)
		printf ("\tdhcp server = %s\n", edit_ipaddr (dp->dh_dhcp_server_ipaddr));

	if (dp->dh_client_id[0])
	{
		int	i;

		printf ("\tclient ID =");

		for (i = 0; i < DHCP_CLIENT_ID_SZ; i++)
			printf (" %02X", dp->dh_client_id[i]);

		printf ("\n");
	}

	if (dp->dh_my_ipaddr != 0)
		printf ("\tendereço IP sugerido = %s\n", edit_ipaddr (dp->dh_my_ipaddr));

	if (dp->dh_host_nm[0] != '\0')
		printf ("\thost = %s\n", dp->dh_host_nm);

	if (dp->dh_domain_nm[0] != '\0')
		printf ("\tdomain_name = %s\n", dp->dh_domain_nm);

	if (dp->dh_mask != 0)
		printf ("\tmask = %s\n", edit_ipaddr (dp->dh_mask));

	if (dp->dh_gateway_ipaddr != 0)
		printf ("\tgateway = %s\n", edit_ipaddr (dp->dh_gateway_ipaddr));

	if (dp->dh_lease_time != 0)
		printf ("\tlease time = %d\n", dp->dh_lease_time);

	if (dp->dh_renewal_time != 0)
		printf ("\trenewal time = %d\n", dp->dh_renewal_time);

	if (dp->dh_rebinding_time != 0)
		printf ("\trebinding time = %d\n", dp->dh_rebinding_time);

	if (dp->dh_dns_index > 0)
	{
		int		i;

		printf ("\tDNS servers =");

		for (i = 0; i < dp->dh_dns_index; i++)
			printf (" %s", edit_ipaddr (dp->dh_dns_server[i]));

		putchar ('\n');
	}

}	/* end dhcp_print_info */
#endif	DEBUG
