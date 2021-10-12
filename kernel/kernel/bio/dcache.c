/*
 ****************************************************************
 *								*
 *			dcache.c				*
 *								*
 *	Gerencia do cache de blocos dos dispositivos		*
 *								*
 *	Versão	4.2.0, de 24.10.01				*
 *		4.8.0, de 25.01.06				*
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

#include "../h/bhead.h"
#include "../h/devmajor.h"
#include "../h/mon.h"

#include "../h/region.h"
#include "../h/signal.h"
#include "../h/uproc.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Variáveis e definições globais				*
 ****************************************************************
 */
entry BHEAD	block_lru;	/* Cabeça da lista LRU */

entry LOCK	block_lru_lock;	/* Lock da lista LRU */

entry BHEAD	*clock_block;	/* Para a atualização dos blocos */

#define BHASH(dev, blkno, ino)	(scb.y_bhash + ((unsigned int)(dev ^ blkno ^ ino)) % scb.y_nbhash)

/*
 ******	Protótipos de funções ***********************************
 */
int		block_insert_hash (BHEAD *bp, BHASHTB *hp);
void		block_remove_hash (BHEAD *bp, BHASHTB *hp);

/*
 ****************************************************************
 *	Obtem um certo BLOCO (nem sempre com a informação)	*
 ****************************************************************
 */
BHEAD *
block_get (dev_t dev, daddr_t blkno, ino_t ino)
{
	BHEAD		*bp;
	BHASHTB 	*hp;
	int		cycle;

	mon.y_block_lru_total++;

	/*
	 *	Espera desocupar um BLOCO
	 */
	hp = BHASH (dev, blkno, ino);

	/*
	 *	Inicia a busca na lista HASH
	 */
    again:
	SPINLOCK (&hp->h_lock);

	for (bp = hp->h_bhead; bp != NOBHEAD; bp = bp->b_next)
	{
		if (bp->b_blkno != blkno || bp->b_dev != dev || bp->b_ino != ino)
			continue;

		if (bp->b_pid == u.u_pid)		/*** PROVISÓRIO ***/
		{
			printf
			(	"Processo %d: tentando dar LOCK no mesmo BHEAD (%v, %d)\n",
				bp->b_pid, bp->b_dev, blkno
			);

			print_calls ();
		}

		SPINFREE (&hp->h_lock);

		SLEEPLOCK (&bp->b_lock, PBLKIO);

		/* Verifica se foi Alarme Falso */

		if (bp->b_blkno != blkno || bp->b_dev != dev || bp->b_ino != ino)
			{ SLEEPFREE (&bp->b_lock); goto again; }

		/* Verifica se Tem Operação em Andamento */

		if (bp->b_flags & B_ASYNC)
			{ EVENTWAIT (&bp->b_done, PBLKIO); bp->b_flags &= ~B_ASYNC; }

		mon.y_block_lru_hit++;

		goto put_end_lru;

	}	/* end busca na lista HASH */

	SPINFREE (&hp->h_lock);

	/*
	 *	O BLOCO desejado não está na lista HASH
	 *
	 *	Obtem um bloco livre, a partir da lista LRU
	 *
	 */
	cycle = 0;
    again_lru:
	SPINLOCK (&block_lru_lock);

	for (bp = block_lru.b_lru_forw; /* abaixo */; bp = bp->b_lru_forw)
	{
		if (bp == &block_lru)
		{
			if (cycle++ > 0)
				printf ("%g: Já é a volta # %d da lista LRU\n", cycle);

			continue;
		}

		if (SLEEPTEST (&bp->b_lock) < 0)
			continue;

		/*
		 *	Obteve um candidato trancado pela lista LRU
		 *
		 *	Verifica se não é um BLOCO que tem informação ainda não atualizada
		 */
		if (bp->b_flags & B_DIRTY)
		{
			if (cycle <= 0)						/* Primeira volta */
			{
				SLEEPFREE (&bp->b_lock); continue;
			}
			else							/* Voltas seguintes */
			{
				SPINFREE (&block_lru_lock); bp->b_flags &= ~B_ASYNC; bwrite (bp);
				goto again_lru;
			}
		}		

		/*
		 *	Verifica se tem Operação em Andamento
		 */
		if (bp->b_flags & B_ASYNC)
		{
			if (cycle <= 0 || MAJOR (bp->b_dev) == NFS_MAJOR)	/* Primeira volta ou NFS */
			{
				if (EVENTTEST (&bp->b_done) < 0)
					{ SLEEPFREE (&bp->b_lock); continue; }		

				SPINFREE (&block_lru_lock);
			}
			else							/* Voltas seguintes */
			{
				SPINFREE (&block_lru_lock);

				EVENTWAIT (&bp->b_done, PBLKIO);
			}

			bp->b_flags &= ~B_ASYNC;
		}
		else
		{
			SPINFREE (&block_lru_lock);
		}

		break;

	}	/* end for (lista LRU) */

	/*
	 *	Verifica se está em alguma lista HASH
	 *
	 *	Neste ponto, a lista LRU já está liberada
	 */
	if (bp->b_dev == NODEV)
		mon.y_block_full_cnt++;
	else
		block_remove_hash (bp, BHASH (bp->b_dev, bp->b_blkno, bp->b_ino));

	/*
	 *	Preenche os campos do BHEAD
	 */
	bp->b_dev	= dev;
	bp->b_phdev	= dev;
	bp->b_blkno	= blkno;
	bp->b_phblkno	= blkno;
	bp->b_ino	= ino;
	bp->b_flags	= 0;
	bp->b_residual	= 0;

	EVENTCLEAR (&bp->b_done);

	mon.y_block_lru_miss++;

	/*
	 *	Agora, insere na fila HASH nova
	 */
	if (block_insert_hash (bp, hp) < 0)
{
printf ("%g: Evitada a duplicação da alocação do bloco\n");
		{ SLEEPFREE (&bp->b_lock); goto again; }
}

	/*
	 *	Coloca no final da lista LRU
	 */
   put_end_lru:
	SPINLOCK (&block_lru_lock);

	if (bp != block_lru.b_lru_back)
	{
		bp->b_lru_back->b_lru_forw = bp->b_lru_forw;
		bp->b_lru_forw->b_lru_back = bp->b_lru_back;

		block_lru.b_lru_back->b_lru_forw = bp;
		bp->b_lru_back = block_lru.b_lru_back;

		block_lru.b_lru_back = bp;
		bp->b_lru_forw = &block_lru;
	}

	SPINFREE (&block_lru_lock);

	bp->b_pid = u.u_pid;	/* PROVISÓRIO */

	bp->b_addr  = bp->b_base_addr;
	bp->b_count = BL4SZ;

	/*
	 *	Verifica se é um bloco de um PSEUDO-disco
	 */
	if (MAJOR (bp->b_phdev) == PSEUDO_MAJOR)
		pseudo_alloc_bhead (bp);

	/*
	 *	Tenta manter alguns blocos livres
	 */
    dirty_again:
	while (scb.y_nbuf - mon.y_block_dirty_cnt < 8)
	{
		BHEAD		*dirty_bp;

		SPINLOCK (&block_lru_lock);

		for (dirty_bp = block_lru.b_lru_forw; dirty_bp != &block_lru; dirty_bp = dirty_bp->b_lru_forw)
		{
			if ((dirty_bp->b_flags & B_DIRTY) == 0)
				continue;

			if (SLEEPTEST (&dirty_bp->b_lock) < 0)
				continue;

			if ((dirty_bp->b_flags & B_DIRTY) == 0)
				{ SLEEPFREE (&dirty_bp->b_lock); continue; }

			SPINFREE (&block_lru_lock);

			dirty_bp->b_flags |= B_ASYNC; bwrite (dirty_bp);

			goto dirty_again;
		}

		SPINFREE (&block_lru_lock); break;
	}

	return (bp);	

}	/* end block_get */

/*
 ****************************************************************
 *	Obtém um BLOCO livre					*
 ****************************************************************
 */
BHEAD *
block_free_get (int size)
{
	BHEAD		*bp;

	if ((bp = malloc_byte (sizeof (BHEAD))) == NOBHEAD)
		return (NOBHEAD);

	if ((bp->b_base_addr = malloc_byte (size)) == NOVOID)
		{ free_byte (bp); return (NOBHEAD); }

	bp->b_addr    = bp->b_base_addr;
#if (0)	/*******************************************************/
	bp->b_phaddr  = (void *)VIRT_TO_PHYS_ADDR (bp->b_addr);
#endif	/*******************************************************/
	bp->b_dev     = NODEV;
	bp->b_phdev   = NODEV;
	bp->b_blkno   = 0;
	bp->b_phblkno = 0;
	bp->b_ino     = NOINO;
	bp->b_flags   = 0;

	/* Já devolve o bloco com o comando SCSI zerado */

   	memclr (bp->b_cmd_txt, sizeof (bp->b_cmd_txt) + sizeof (bp->b_cmd_len));

	EVENTCLEAR (&bp->b_done);

	SLEEPCLEAR (&bp->b_lock);

	SLEEPTEST (&bp->b_lock);

	bp->b_pid = u.u_pid;	/* PROVISÓRIO */

	return (bp);	

}	/* end block_free_get */

/*
 ****************************************************************
 *	Libera o BLOCO						*
 ****************************************************************
 */
void
block_put (BHEAD *bp)
{
	/*
	 *	Verifica se houve erro
	 */
	if ((bp->b_flags & (B_ERROR|B_READ|B_ASYNC)) == (B_ERROR|B_READ))
		EVENTCLEAR (&bp->b_done);

	bp->b_addr = NOVOID;
	bp->b_pid = -1;			/* PROVISÓRIO */

	SLEEPFREE (&bp->b_lock);

	/*
	 *	Se foi alocado por "block_free_get", ...
	 */
	if (bp->b_dev == NODEV)
	{
		free_byte (bp->b_base_addr);
		free_byte (bp);
	}

}	/* end block_put */

/*
 ****************************************************************
 *	Verifica se um bloco está associado a um BHEAD		*
 ****************************************************************
 */
int
block_in_core (dev_t dev, daddr_t blkno, ino_t ino)
{
	BHEAD		*bp;
	BHASHTB		*hp;

	hp = BHASH (dev, blkno, ino);

	SPINLOCK (&hp->h_lock);

	for (bp = hp->h_bhead; bp != NOBHEAD; bp = bp->b_next)
	{
		if (bp->b_blkno != blkno || bp->b_dev != dev || bp->b_ino != ino)
			{ SPINFREE (&hp->h_lock); return (0); }
	}

	SPINFREE (&hp->h_lock); return (-1);

}	/* end block_in_core */

/*
 ****************************************************************
 *	Insere um BLOCO da lista hash				*
 ****************************************************************
 */
int
block_insert_hash (BHEAD *bp, BHASHTB *hp)
{
	BHEAD		*tst_bp;

	/*
	 *	Devolve:
	 *		 0 => Inseriu o bloco na fila HASH
	 *		-1 => O bloco já se encontra na fila HASH
	 *
	 */
	SPINLOCK (&hp->h_lock);

	for (tst_bp = hp->h_bhead; /* abaixo */; tst_bp = tst_bp->b_next)
	{
		if (tst_bp == NOBHEAD)
			{ bp->b_next = hp->h_bhead; hp->h_bhead = bp; SPINFREE (&hp->h_lock); return (0); }

		if (tst_bp->b_dev == bp->b_dev && tst_bp->b_blkno == bp->b_blkno && tst_bp->b_ino == bp->b_ino)
			{ SPINFREE (&hp->h_lock); return (-1); }
	}

}	/* end block_insert_hash */

/*
 ****************************************************************
 *	Remove um BLOCO da lista hash				*
 ****************************************************************
 */
void
block_remove_hash (BHEAD *bp, BHASHTB *hp)
{
	BHEAD		*abp;

	SPINLOCK (&hp->h_lock);

	if (hp->h_bhead == bp)
		{ hp->h_bhead = bp->b_next; SPINFREE (&hp->h_lock); return; }

	for (abp = hp->h_bhead; abp != NOBHEAD; abp = abp->b_next)
	{
		if (abp->b_next == bp)
			{ abp->b_next = bp->b_next; SPINFREE (&hp->h_lock); return; }
	}

	SPINFREE (&hp->h_lock);

	printf ("%g: BHEAD não encontrado na lista HASH\n");

}	/* end block_remove_hash */

/*
 ****************************************************************
 *	Atualiza os BLOCOS no Disco				*
 ****************************************************************
 */
void
block_update (void)
{
	int		blocks_written = 0, blocks_examined;
	BHEAD		*bp = clock_block;

	/*
	 *	Escreve os blocos modificados há algum tempo
	 */
	for (blocks_examined = scb.y_nbuf; blocks_examined > 0; blocks_examined--, bp++)
	{
		if (bp >= scb.y_endbhead)
			bp = scb.y_bhead;

		if ((bp->b_flags & B_DIRTY) == 0)
			continue;

		if (time - bp->b_dirty_time <= scb.y_dirty_time)
			continue;

		if (SLEEPTEST (&bp->b_lock) < 0)
			continue;

		/*
		 *	Escreve assincronamente
		 */
		bp->b_flags |= B_ASYNC; bwrite (bp);
#ifdef	MSG
		if (CSWT (6)) printf
		(	"%g: bl = (%v, %d), n = %d, time = %d\n",
			bp->b_dev, bp->b_blkno, blocks_written + 1, time - bp->b_dirty_time
		);
#endif	MSG
		if (++blocks_written >= scb.y_max_flush)
			break;

	}	/* end percorrendo os BHEADs */

	/*
	 *	Epílogo
	 */
	clock_block = bp;

}	/* block_update */

/*
 ****************************************************************
 *	Atualiza os BLOCOS no Disco				*
 ****************************************************************
 */
void
block_flush (dev_t dev)
{
	BHEAD		*bp;

	/*
	 *	Escreve os blocos modificados
	 */
	for (bp = scb.y_bhead; bp < scb.y_endbhead; bp++)
	{
		if (dev != NODEV && dev != bp->b_dev)
			continue;

		if ((bp->b_flags & B_DIRTY) == 0)
			continue;

		SLEEPLOCK (&bp->b_lock, PBLKIO);

		/* Escreve assincronamente */

		bp->b_flags |= B_ASYNC; bwrite (bp);
#ifdef	MSG
		if (CSWT (6))
			printf ("%g: dev = %v, blno = %d\n", bp->b_dev, bp->b_blkno);
#endif	MSG

	}	/* end percorrendo os BHEADs */

	/*
	 *	Espera terminar as operações
	 */
	for (bp = scb.y_bhead; bp < scb.y_endbhead; bp++)
	{
		if (dev != NODEV && dev != bp->b_dev)
			continue;

		if ((bp->b_flags & B_ASYNC) == 0)
			continue;

		SLEEPLOCK (&bp->b_lock, PBLKIO);

		if (bp->b_flags & B_ASYNC)
			{ EVENTWAIT (&bp->b_done, PBLKIO); bp->b_flags &= ~B_ASYNC; }

		SLEEPFREE (&bp->b_lock);
	}

}	/* end block_flush */

/*
 ****************************************************************
 *	Apaga os blocos (para umount e close)			*
 ****************************************************************
 */
void
block_free (dev_t dev, ino_t ino /* Para NFS */)
{
	BHEAD		*bp;

	for (bp = scb.y_bhead; bp < scb.y_endbhead; bp++)
	{
		if (bp->b_dev != dev)
			continue;

		/* Para NFS */

		if (ino != NOINO && bp->b_ino != ino)
			continue;

		SLEEPLOCK (&bp->b_lock, PBLKIO);

		/* Verifica se há operação em andamento */

		if (bp->b_flags & B_ASYNC)
			{ EVENTWAIT (&bp->b_done, PBLKIO); bp->b_flags &= ~B_ASYNC; }

		/* Remove da lista HASH */

		block_remove_hash (bp, BHASH (bp->b_dev, bp->b_blkno, bp->b_ino));

		/* Verifica se ainda não foi escrito */

		if (bp->b_flags & B_DIRTY)
		{
			if (ino == NOINO)
				printf ("%g: BHEAD DIRTY: (%v, %d)\n", dev, bp->b_blkno);

			mon.y_block_dirty_cnt--;
		}

		/*
		 *	Zera alguns campos
		 */
		bp->b_dev	= NODEV;
		bp->b_phdev	= NODEV;
		bp->b_blkno	= 0;
		bp->b_phblkno	= 0;
		bp->b_ino	= NOINO;
		bp->b_flags	= 0;
		bp->b_next	= NOBHEAD;

		mon.y_block_full_cnt--;

		/*
		 *	Insere no início da lista LRU
		 */
		SPINLOCK (&block_lru_lock);

		bp->b_lru_back->b_lru_forw = bp->b_lru_forw;
		bp->b_lru_forw->b_lru_back = bp->b_lru_back;

		block_lru.b_lru_forw->b_lru_back = bp;
		bp->b_lru_forw = block_lru.b_lru_forw;

		block_lru.b_lru_forw = bp;
		bp->b_lru_back = &block_lru;

		SLEEPFREE (&bp->b_lock);

		SPINFREE (&block_lru_lock);
	}

}	/* end block_free */

/*
 ****************************************************************
 *	Inicialização						*
 ****************************************************************
 */
void
block_lru_init (void)
{
	BHEAD		*bp, *abp;
	char		*vp;
	int		nbuf = 0;

	/*
	 *	Examina os endereços dos buffers
	 */
	if ((int)scb.y_buf0 & BLMASK  ||  (int)scb.y_buf1 & BLMASK)
		panic ("binit: BUFFERs com endereços não alinhados");

	/*
	 ******	Coloca todos os BHEADS na lista LRU *************
	 */
	abp = &block_lru;
	bp  = scb.y_bhead;

   /***	SPINLOCK (&block_lru_lock); ***/

	/*
	 *	Primeiro grupo: abaixo de 640 Kb
	 */
	for (vp = scb.y_buf0; vp < scb.y_endbuf0; vp += BL4SZ, bp++)
	{
		bp->b_dev	= NODEV;
		bp->b_phdev	= NODEV;
		bp->b_flags	= 0;

		bp->b_base_addr	= vp;
		bp->b_addr	= vp;
		bp->b_base_count = BL4SZ;
#if (0)	/*******************************************************/
		bp->b_phaddr = (void *)VIRT_TO_PHYS_ADDR (vp);
#endif	/*******************************************************/

		nbuf++;

		abp->b_lru_forw = bp;
		bp->b_lru_back  = abp;

		abp = bp; 
	}

	/*
	 *	Segundo grupo: acima de 1 MB
	 */
	for (vp = scb.y_buf1; vp < scb.y_endbuf1; vp += BL4SZ, bp++)
	{
		bp->b_dev	= NODEV;
		bp->b_phdev	= NODEV;
		bp->b_flags	= 0;

		bp->b_base_addr	= vp;
		bp->b_addr	= vp;
		bp->b_base_count = BL4SZ;
#if (0)	/*******************************************************/
		bp->b_phaddr = (void *)VIRT_TO_PHYS_ADDR (vp);
#endif	/*******************************************************/

		nbuf++;

		abp->b_lru_forw = bp;
		bp->b_lru_back  = abp;

		abp = bp; 
	}

	/*
	 *	A lista LRU é circular
	 */
	abp->b_lru_forw      = &block_lru;
	block_lru.b_lru_back = abp;

   /***	SPINFREE (&block_lru_lock); ***/

	/*
	 *	Pequeno teste de consistência
	 */
	if (bp != scb.y_endbhead)
		panic ("binit: Inconsistência de \"endbhead\"");

	if (nbuf != scb.y_nbuf)
		panic ("binit: Inconsistência de \"nbuf\"\n");

	/*
	 *	Para a função de "update"
	 */
	clock_block = scb.y_bhead;

}	/* end block_lru_init */
