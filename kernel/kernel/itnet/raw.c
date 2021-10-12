/*
 ****************************************************************
 *								*
 *			raw.c					*
 *								*
 *	Funções relacionadas com o protocolo RAW		*
 *								*
 *	Versão	3.0.0, de 27.12.92				*
 *		4.0.0, de 17.08.00				*
 *								*
 *	Funções:						*
 *		send_raw_datagram,    receive_raw_datagram	*
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

/*
 ****************************************************************
 *	Envia um datagrama RAW					*
 ****************************************************************
 */
void
send_raw_datagram (register RAW_EP *rp, register ITBLOCK *bp)
{
	register ROUTE		*dp;

	/*
	 *	Repare que o conteúdo já está no bloco "bp"
	 *
	 *	Obtém o dispositivo destino e endereço fonte
	 */
	if ((dp = get_route_entry (rp->rp_snd_addr)) == NOROUTE)
	   	{ put_it_block (bp); u.u_error = TNOROUTE; return; }

	if (rp->rp_my_addr == 0)
		rp->rp_my_addr = dp->r_my_addr;

	bp->it_src_addr	= rp->rp_my_addr;
	bp->it_dst_addr	= rp->rp_snd_addr;
	bp->it_route	= dp;

	/*
	 *	Envia o datagrama IP
	 */
	bp->it_free_after_IO = 1;
	bp->it_wait_for_arp = 1;

   /***	bp->it_u_area  = ...; ***/
   /***	bp->it_u_count = ...; ***/

	send_ip_datagram (rp->rp_snd_proto, bp);

}	/* end send_raw_datagram */

/*
 ****************************************************************
 *	Recebe um datagrama RAW					*
 ****************************************************************
 */
int
receive_raw_datagram (register ITBLOCK *bp, int proto, void *area, int count)
{
	register ITBLOCK *nbp;
	register ITSCB	*ip = &itscb;
	register RAW_EP	*rp;
	int		found = 0;

	/*
	 *   Coloca o datagrama RAW na fila de entrada dos EPs correspondentes
	 */
	SPINLOCK (&ip->it_rep_lock);

	for (rp = ip->it_rep_busy; rp != NO_RAW_EP; rp = rp->rp_next)
	{
		if (rp->rp_bind_proto != 0 && rp->rp_bind_proto != proto)
			continue;

		if (rp->rp_bind_addr != 0 && rp->rp_bind_addr != bp->it_src_addr)
			continue;

		found++;

		/*
		 *	Tira uma cópia do bloco
		 */
		if ((nbp = get_it_block (IT_IN)) == NOITBLOCK)
		{
#ifdef	MSG
			printf ("%g: NÃO obtive bloco\n");
#endif	MSG
			SPINFREE (&ip->it_rep_lock);

			return (found);		/* ou continue? */
		}

		unimove (nbp->it_frame, area, count, SS);

		nbp->it_src_addr = bp->it_src_addr;
		nbp->it_dst_addr = bp->it_dst_addr;

		nbp->it_area  = nbp->it_frame;
		nbp->it_count = count;

		nbp->it_proto = proto;

		/*
		 *	Põe o bloco na fila de entrada
		 */
		SPINLOCK (&rp->rp_inq_lock);

		if (rp->rp_inq_first == NOITBLOCK)
			rp->rp_inq_first = nbp;
		else
			rp->rp_inq_last->it_inq_forw.inq_ptr = nbp;

		rp->rp_inq_last = nbp;
		nbp->it_inq_forw.inq_ptr = NOITBLOCK;

		nbp->it_time = time;

		EVENTDONE (&rp->rp_inq_nempty);

		if (rp->rp_attention_set && rp->rp_uproc->u_attention_index < 0)
		{
			rp->rp_attention_set = 0;
			rp->rp_uproc->u_attention_index = rp->rp_fd_index;
			EVENTDONE (&rp->rp_uproc->u_attention_event);
		}

		SPINFREE (&rp->rp_inq_lock);

	}	/* end percorrendo os RAW EPs */

	SPINFREE (&ip->it_rep_lock);

	return (found);

}	/* end receive_raw_data */
