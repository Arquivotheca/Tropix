/*
 ****************************************************************
 *								*
 *			dsort.c					*
 *								*
 *	Função geral de ordenação das filas de E/S		*
 *								*
 *	Versão	3.0.0, de 08.10.94				*
 *		3.0.0, de 08.10.94				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 1999 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include "../h/common.h"
#include "../h/sync.h"
#include "../h/scb.h"
#include "../h/region.h"

#include "../h/bhead.h"
#include "../h/devhead.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Variáveis e definições globais 				*
 ****************************************************************
 */
#define	B_DIREC		B_UP
#define	B_DOWN		0

#define	V_DIREC		V_UP
#define	V_DOWN		0

/*
 ****************************************************************
 *	Inserção de um "bp" na fila de um "dp"			*
 ****************************************************************
 */
void
insdsort (register DEVHEAD *dp, register BHEAD	*bp)
{
	register BHEAD	*ap, *tp;
	register int	bpcyl;

 	/*
	 *	A fila de pedidos tem 3 seções:
	 *
	 *	  1. Seção de SWAP: Fifo
	 *	  2. Seção de READ: Dividido em duas Seções:
	 *	     Atual e seguinte (Elevador)
	 *	  3. Seção de WRITE: Fifo
	 */

	SPINLOCK (&dp->v_lock);

	/*
	 *	Verifica se a Fila está Vazia
	 */
	if ((ap = dp->v_first) == NOBHEAD)
	{
		dp->v_first = bp;
		dp->v_last = bp;

		bp->b_forw = NOBHEAD;
		bp->b_back = NOBHEAD;

		dp->v_flags |=  V_UP;
		bp->b_flags |=  B_UP;

		SPINFREE (&dp->v_lock);

		return;
	}
	
	/*
	 *	A Fila não está vazia. Guarda o local de inserção provisorio
	 *	(depois do primeiro pedido). Repare que não podemos inserir
	 *	antes do primeiro da fila
	 */
	tp = ap;
	
	/*
	 *	Verifica se o Pedido é SWAP
	 */
	if (bp->b_flags & B_SWAP)
	{
		/*
		 *   O pedido é SWAP; acha o final da fila de SWAP, que é FIFO
		 */
		for (/* vazio */; ap != NOBHEAD; ap = ap->b_forw)
		{
			if ((ap->b_flags & B_SWAP) == 0)
				break;
			tp = ap;
		}
		goto insert;
	}
	
	/*
	 *	Verifica se o pedido é WRITE; a seção de WRITE é a ultima, FIFO
	 */
	if ((bp->b_flags & B_READ) == 0)
		{ tp = dp->v_last; goto insert;	}

	/*
	 *	O Pedido é READ Normal. Procura a segunda seção do elevador,
	 *	varrendo do final para o comeco. Repare que "ap" aponta
	 *	para o 1. bhead da fila
	 */
	bpcyl = bp->b_cylin;

	for (tp = dp->v_last; tp != ap; tp = tp->b_back)
	{
		if (tp->b_flags & B_SWAP)
		{
			/*
			 *	A seção READ está vazia
			 */
			dp->v_flags |=  V_UP;
			bp->b_flags |=  B_UP;

			goto insert;
		}

		if (tp->b_flags & B_READ)
		{
			/*
			 *	Achou o final da seção de READ
			 */
			break;
		}
	}

	/*
	 *	Verifica se Chegou no comeco da fila
	 */
	if (tp == ap)
	{
		/*
		 *	A Fila só tem seção de WRITE;
		 *	Inicia uma seção de READ
		 */
		dp->v_flags |=  V_UP;
		bp->b_flags |=  B_UP;

	   /***	tp = dp->v_first; ***/

		goto insert;
	}

	/*
	 *	Achou o final da seção READ; Verifica se tem 1 ou 2 Filas
	 */
	if ((dp->v_flags & V_DIREC) == (tp->b_flags & B_DIREC))
	{
		/*
		 *	Só temos uma FILA de READ. Verifica se podemos inserir
		 *	no final desta ou temos de iniciar uma SEGUNDA FILA
		 */
		if ((dp->v_flags & V_DIREC) == V_DOWN)
		{
			if (bpcyl <= tp->b_cylin)
			{
				bp->b_flags &= ~B_UP;
				goto insert;
			}
		}
		else	/* Subindo */
		{
			if (bpcyl >= tp->b_cylin)
			{
				bp->b_flags |=  B_UP;
				goto insert;
			}
		}

		/*
		 *	Temos de Criar a SEGUNDA FILA
		 */
		if (dp->v_flags & V_UP)
			bp->b_flags &= ~B_UP;
		else
			bp->b_flags |=  B_UP;
		goto insert;
	}

	/*
	 *	Achou o final da SEGUNDA FILA do READ
	 */
	if ((tp->b_flags & B_DIREC) == B_UP)
	{
		/*
		 *	A SEGUNDA FILA está subindo
		 */
		bp->b_flags |= B_UP;

		for (/* vazio */; tp != ap; tp = tp->b_back)
		{
			if ((tp->b_flags & (B_SWAP|B_READ)) != B_READ)
			{
				/*
				 *	Saiu fora da fila de READ
				 */
				goto insert;
			}

			if ((tp->b_flags & B_DIREC) == B_DOWN)
			{
				/*
				 *	Chegou no final 1a. fila de READ
				 */
				goto insert;
			}
	
			if (tp->b_cylin <= bpcyl)
				goto insert;
		}
	}
	else
	{
		/*
		 *	A SEGUNDA FILA está descendo
		 */
		bp->b_flags &= ~B_UP;

		for (/* vazio */; tp != ap; tp = tp->b_back)
		{
			if ((tp->b_flags & (B_SWAP|B_READ)) != B_READ)
			{
				/*
				 *	Saiu fora da fila de READ
				 */
				goto insert;
			}

			if ((tp->b_flags & B_DIREC) == B_UP)
			{
				/*
				 *	Chegou no final 1a. fila de READ
				 */
				goto insert;
			}
	
			if (tp->b_cylin >= bpcyl)
				goto insert;
		}
	}

	/*
	 *	Algo Errado!
	 */
	printf ("%g: Inconsistencia na fila READ\n");

	printf ("dp = %X, fila =", dp->v_flags);

	for (tp = dp->v_first; tp != NOBHEAD; tp = tp->b_forw)
	{
		printf (" (%x, %d)", tp->b_flags, tp->b_cylin);  
	}

	printf (" bp = (%x, %d)\n", bp->b_flags, bp->b_cylin);  

	tp = dp->v_last;

	/*
	 *	Seção de Inserção no local escolhido
	 */
     insert:
	bp->b_forw = tp->b_forw;
	bp->b_back = tp;

	tp->b_forw = bp;

	if (tp == dp->v_last)
		dp->v_last = bp;
	else
		bp->b_forw->b_back = bp;

	SPINFREE (&dp->v_lock);

}	/* end dsort */

/*
 ****************************************************************
 *	Remoção da Fila 					*
 ****************************************************************
 */
BHEAD *
remdsort (register DEVHEAD *dp)
{
	register BHEAD	*obp, *bp;	

	/*
	 *	A FILA vem e volta LOCKED
	 */
	obp = dp->v_first;

	/*
	 *	Retira o pedido da fila
	 */
	if ((dp->v_first = bp = obp->b_forw) == NOBHEAD)
	{
		dp->v_last = NOBHEAD;
	}
	else
	{
		bp->b_back = NOBHEAD;

		if
		(	(bp = bp->b_forw) != NOBHEAD &&
			(bp->b_flags & (B_READ|B_SWAP)) == B_READ
		)
		{
			dp->v_flags &= ~V_DIREC;
			dp->v_flags |= bp->b_flags & B_DIREC;
		}
	}

	return (obp);

}	/* end remdsort */
