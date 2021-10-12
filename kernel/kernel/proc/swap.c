/*
 ****************************************************************
 *								*
 *			swap.c					*
 *								*
 *	Gerência de SWAP					*
 *								*
 *	Versão	3.0.0, de 01.12.94				*
 *		4.6.0, de 06.10.04				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2004 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include "../h/common.h"
#include "../h/sync.h"
#include "../h/scb.h"
#include "../h/region.h"

#include "../h/map.h"
#include "../h/signal.h"
#include "../h/mon.h"
#include "../h/inode.h"
#include "../h/iotab.h"
#include "../h/devhead.h"
#include "../h/bhead.h"
#include "../h/uproc.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Variáveis globais e definições externas			*
 ****************************************************************
 */
#define	NOAGE		(-30000)
#define	NOPRI		30000
#define	NSWAPBUF	2

#define M64KBSZ		(64 * KBSZ)
#define M64KBMASK	(M64KBSZ - 1)
#define M16MBLIMIT	(16 * MBSZ)

entry BHEAD		swapbuf[NSWAPBUF];	/* Bheads para o SWAP */
entry SEMA		swapsema;		/* Semaforo do SWAP */
entry LOCK		swaplock;		/* Lock do SWAP */

/*
 ****************************************************************
 *	Gerência de processos					*
 ****************************************************************
 */
void
sched (void)
{
	const UPROCV	*uvp;
	UPROC		*up;
	int		block_update_time = 0;
	int		sync_update_time  = 0;
	int		usb_explore_time  = 0;

	/*
	 *	O Escalador está em ação
	 */
	for (EVER)
	{
		/*
		 *	Espera um segundo
		 */
		EVENTWAIT  (&every_second_event, PSCHED);

		/*
		 *	Atualiza os tempos e dá o SIGALRM
		 */
		for (uvp = &scb.y_uproc[scb.y_initpid]; uvp <= scb.y_lastuproc; uvp++)
		{
			if ((up = uvp->u_uproc) == NOUPROC)
				continue;

			if (up->u_state == SNULL || up->u_state == SZOMBIE)
				continue;

			/* SWAPRDY, CORERDY, SWAPSLP, CORESLP & RUN */

			up->u_time++;			

			if (up->u_alarm && --up->u_alarm == 0)
				sigproc (up, SIGALRM);

			if (up->u_nfs.nfs_transmitted)
			{
				if (time - up->u_nfs.nfs_last_snd_time > up->u_nfs.nfs_timeout + 1)
					nfs_retransmit_client_datagram (up);
			}
		}

		/*
		 *	Outras funções
		 */
		idle_inc_color ();	/* Troca a cor do "idle" */

		screen_saver_every_second_work ();

		dns_resolver_every_second_work ();

		if (sync_update_time++ >= scb.y_sync_time)
		{
			update (NODEV, 0 /* NOT blocks */);
			sync_update_time = 0;
		}

		if   (mon.y_block_dirty_cnt * 100 > scb.y_nbuf * 60)
		{
			block_update ();
		}
		elif (block_update_time++ >= scb.y_block_time)
		{
			block_update ();
			block_update_time = 0;
		}

#if (0)	/*******************************************************/
		if (usb_active > 0 && usb_explore_time++ >= usb_active)
			usb_explore_time = 0;
#endif	/*******************************************************/
		if (usb_active > 0)
		{
			EVENTDONE (&usb_explore_event);
		}

	}	/* end if (reschedtime) */

}	/* end sched */

/*
 ****************************************************************
 *	Realiza um SWAP						*
 ****************************************************************
 */
void
swap (pg_t daddr, pg_t pg_caddr, pg_t pg_count, int rw)
{
	register BHEAD		*bp;
	unsigned long		ph_area;
	unsigned long		cp_area = 0, cp_count = 0, area;
	pg_t			malloc_area = 0, malloc_size;
	long			u_count, count;
	dev_t			swapdev;
	int			i;
	char			must_copy = 0;

	/*
	 *	Teste importante
	 */
	if ((swapdev = scb.y_swapdev) == NODEV)
		panic ("Swap: NÃO tenho dispositivo de SWAP");

	/*
	 *	Obtem um BHEAD para SWAP
	 */
	SEMALOCK (&swapsema, PSCHED);

	SPINLOCK (&swaplock);

	for (i = 0; /* abaixo */; i++)
	{
		if (i >= NSWAPBUF)
			panic ("Swap: Fila de buffers de SWAP surpreendentemente vazia");

		bp = &swapbuf[i];

		if (TAS (&bp->b_lock) == 0)
			break;

	}	/* end for (fila de buffers) */

	SPINFREE (&swaplock);

	/*
	 *	Prepara os valores em bytes
	 */
	ph_area = VIRT_TO_PHYS_ADDR (PGTOBY (pg_caddr));
	u_count = PGTOBY (pg_count);

	area    = ph_area;		/* Em princípio */
	daddr  -= SWAP_OFFSET;
	daddr  += scb.y_swaplow;

	/*
	 *	Verifica se precisa de realizar cópias (em virtude de DMA)
	 */
	if (biotab[MAJOR (swapdev)].w_tab->v_flags & V_DMA_24)
	{
		if (ph_area + u_count > M16MBLIMIT)
			must_copy++;

		if ((ph_area & M64KBMASK) + u_count > M64KBSZ)
			must_copy++;
	}

	/*
	 *	Se for o caso, aloca uma área de cópia
	 */
	if (must_copy)
	{
		if ((malloc_area = malloc_dma_24 (&malloc_size)) == NOPG)
			panic ("Swap: Não consegui obter memória");

		area	 =
		cp_area  = VIRT_TO_PHYS_ADDR (PGTOBY (malloc_area));
		cp_count = PGTOBY (malloc_size);
#ifdef	MSG
		if (CSWT (5))
			printf ("Swap (malloc_dma_24): %P, %d\n", cp_area, cp_count);
#endif	MSG
	}

	/*
	 *	Inicia o ciclo de operações
	 */
    more:
	count = u_count;

	if (must_copy)
	{
		if (count > cp_count)
			count = cp_count;

		if (rw == B_WRITE)
		{
			memmove
			(	(void *)PHYS_TO_VIRT_ADDR (cp_area),
				(void *)PHYS_TO_VIRT_ADDR (ph_area),
				count
			);
		}
	}

	/*
	 *	Inicia a operação
	 */
	EVENTCLEAR (&bp->b_done);

	bp->b_flags	= B_SWAP|rw;
	bp->b_dev	=
	bp->b_phdev	= swapdev;

	bp->b_addr	=
	bp->b_base_addr = (void *)PHYS_TO_VIRT_ADDR (area);
	bp->b_blkno	=
	bp->b_phblkno	= daddr;
	bp->b_base_count = count;
	bp->b_error	= 0;

#ifdef	MSG
	if (CSWT (5))
	{
		printf
		(	"%g: dev = %v, area = %P, blkno = %d, count = %d, %s\n",
			bp->b_dev, area, bp->b_blkno, bp->b_base_count,
			(bp->b_flags & B_READ) ? "R" : "W"
		);
	}
#endif	MSG

	(*biotab[MAJOR (bp->b_dev)].w_strategy) (bp);

	/*
	 *	Ao fim da operação, testa se houve erro
	 */
	EVENTWAIT (&bp->b_done, PSCHED);

	if (bp->b_flags & B_ERROR)
		panic ("Swap: Erro de E/S");

	if (bp->b_residual != 0)
		panic ("Swap: Residual em E/S");

	if (must_copy && rw == B_READ)
	{
		memmove
		(	(void *)PHYS_TO_VIRT_ADDR (ph_area),
			(void *)PHYS_TO_VIRT_ADDR (cp_area),
			count
		);
	}

	if (!must_copy)
		area += count;

	ph_area  += count;

   /***	u_area   += count; ***/
	u_count  -= count;
   /***	u_offset += count; ***/
	daddr    += count >> BLSHIFT;

	if (u_count > 0)
		goto more;

	/*
	 *	Epílogo
	 */
	CLEAR (&bp->b_lock); SEMAFREE (&swapsema);

	if (must_copy)
		mrelease (M_CORE, malloc_size, malloc_area);

}	/* end swap */

/*
 ****************************************************************
 *	Inicializa o Semaforo do Swap				*
 ****************************************************************
 */
void
swapinit (void)
{
	SEMAINIT (&swapsema, NSWAPBUF, 1 /* com histórico */);

}	/* end swapinit */
