/*
 ****************************************************************
 *								*
 *			icmp.c					*
 *								*
 *	Funções relacionadas com o protocolo ICMP		*
 *								*
 *	Versão	3.0.0, de 28.12.92				*
 *		4.0.0, de 16.08.00				*
 *								*
 *	Funções:						*
 *		send_icmp_datagram,      receive_icmp_datagram	*
 *		send_icmp_error_message  print_dst_unreach	*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 * 		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
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

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */

/*
 ****** Protótipos de funções ***********************************
 */
void		print_dst_unreach (int);

/*
 ****************************************************************
 *	Envia uma mensagem de erro ICMP				*
 ****************************************************************
 */
void
send_icmp_error_message (IPADDR addr, int type, int code, void *area, int count)
{
	register ITBLOCK	*bp;
	register ICMP_H		*icmph;

	/*
	 *	Obtém um bloco para a mensagem de erro ICMP
	 */
	if ((bp = get_it_block (IT_OUT_CTL)) == NOITBLOCK)
	{
#ifdef	MSG
		printf ("%g: NÃO obtive bloco\n");
#endif	MSG
		return;
	}

   /***	bp->it_src_addr = 0;	***/
	bp->it_dst_addr = addr;

	/*
	 *	Prepara o cabeçalho ICMP
	 */
	icmph = (ICMP_H *)(bp->it_frame + RESSZ); /* >= ETH_H + IP_H */

	icmph->ih_type	   = type;
	icmph->ih_code	   = code;
   /***	icmph->ih_checksum = 0; ***/
   	icmph->u.ih_addr   = 0;

	/*
	 *	Copia o trecho do datagrama para identificação posterior
	 */
	memmove ((void *)icmph + sizeof (ICMP_H), area, count);

	bp->it_u_area  = icmph;
	bp->it_u_count = sizeof (ICMP_H) + count;

	send_icmp_datagram (bp);

}	/* end send_icmp_error_message */

/*
 ****************************************************************
 *	Envia um datagrama ICMP					*
 ****************************************************************
 */
void
send_icmp_datagram (register ITBLOCK *bp)
{
	register ICMP_H	*icmph;
	register ROUTE	*rp;

	/*
	 *	Repare que o conteúdo já está no bloco "bp"
	 *	(inclusive cabeçalho ICMP e endereço destino)
	 *
	 *	Obtém o dispositivo destino e endereço fonte
	 */
	if ((rp = get_route_entry (bp->it_dst_addr)) == NOROUTE)
	   	{ put_it_block (bp); u.u_error = TNOROUTE; return; }

	if (bp->it_src_addr == 0)
		bp->it_src_addr	= rp->r_my_addr;

	bp->it_route = rp;

	/*
	 *	Calcula o "checksum"
	 */
	icmph = bp->it_u_area;

	icmph->ih_checksum = 0;
	icmph->ih_checksum = compute_checksum (icmph, bp->it_u_count);
	
	/*
	 *	Envia o datagrama IP
	 */
	bp->it_free_after_IO = 1;
	bp->it_wait_for_arp = 0;

   /***	bp->it_u_area  = icmph; ***/
   /***	bp->it_u_count = sizeof (ICMP_H) + count; ***/

	send_ip_datagram (ICMP_PROTO, bp);

}	/* end send_icmp_datagram */

/*
 ****************************************************************
 *	Recebe um datagrama ICMP				*
 ****************************************************************
 */
void
receive_icmp_datagram (register ITBLOCK *bp, void *area, int count)
{
	register ITSCB	*ip = &itscb;
	register ICMP_H	*icmph;
	register ITBLOCK *cbp;

	/*
	 *	Verifica a correção da mensagem
	 *
	 *	Repare que "receive_ip_datagram" é que dá o "put_it_block"
	 */
	icmph = (ICMP_H *)area;

	if (compute_checksum (icmph, count) != 0)
	{
#ifdef	MSG
		if (ip->it_report_error)
		{
			printf ("%g: Recebendo datagrama com checksum incorreto\n");
		}
#endif	MSG
	   /***	put_it_block (bp); ***/ 	return;
	}

	/*
	 *	Analisa o tipo da mensagem ICMP
	 */
	switch (icmph->ih_type)
	{
		/*
		 *	Pedido de ECHO
		 */
	    case ICMP_ECHO_REQ:
		if ((cbp = get_it_block (IT_OUT_CTL)) == NOITBLOCK)
		{
#ifdef	MSG
			printf ("%g: NÃO obtive bloco\n");
#endif	MSG
		   /***	put_it_block (bp); ***/
			return;
		}

		cbp->it_u_area = cbp->it_frame + RESSZ; /* >= ETH_H + IP_H */

		if (count > DGSZ - MIN_IP_H_SZ)
			count = DGSZ - MIN_IP_H_SZ;

		memmove (cbp->it_u_area, icmph, count);
		cbp->it_dst_addr = bp->it_src_addr;
		cbp->it_src_addr = bp->it_dst_addr;

		icmph = (ICMP_H *)cbp->it_u_area;
		icmph->ih_type = ICMP_ECHO_REPLY;

	   /***	cbp->it_u_area  = icmph; ***/
		cbp->it_u_count = count;

		send_icmp_datagram (cbp);

	   /***	put_it_block (bp); ***/

#ifdef	MSG
		if (ip->it_list_info)
			{ printf ("%g: Respondendo PING de "); pr_itn_addr (cbp->it_dst_addr); printf ("\n"); }
#endif	MSG
		return;		

		/*
		 *	Resposta de ECHO
		 */
	    case ICMP_ECHO_REPLY:
	   /***	put_it_block (bp); ***/
		return;

		/*
		 *	Por enquanto, ignorando as demais
		 */
	    default:
#ifdef	MSG
		if (ip->it_report_error)
		{
			printf ("%g: ");

			if (icmph->ih_type == 3 /* dst unreach */)
				print_dst_unreach (icmph->ih_code);
			else
				printf ("Ignorando datagrama com tipo %d\n", icmph->ih_type);
		}
#endif	MSG
	   /***	put_it_block (bp); ***/
		return;

	}	/* end switch */

}	/* end receive_icmp_datagram */

/*
 ****************************************************************
 *   Decodifica a mensagem ICMP de "destination unreachable"	*
 ****************************************************************
 */
void
print_dst_unreach (int code)
{
	printf ("Ignorando \"destination unreachable\" (");

	switch (code)
	{
	    case 0:
		printf ("net");
		break;

	    case 1:
		printf ("host");
		break;

	    case 2:
		printf ("protocol");
		break;

	    case 3:
		printf ("port");
		break;

	    case 4:
		printf ("fragmentation");
		break;

	    case 5:
		printf ("source route failed");
		break;

	    default:
		printf ("%d", code);
		break;
	}

	printf (")\n");

}	/* end print_dst_unreach */
