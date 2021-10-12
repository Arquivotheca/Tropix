/*
 ****************************************************************
 *								*
 *			ramd.c					*
 *								*
 *	Driver de disco simulado na memória interna		*
 *								*
 *	Versão	1.0.0, de 10.03.87				*
 *		4.6.0, de 03.08.04				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2001 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include "../h/common.h"
#include "../h/sync.h"
#include "../h/scb.h"
#include "../h/region.h"

#include "../h/disktb.h"
#include "../h/devhead.h"
#include "../h/devmajor.h"
#include "../h/bhead.h"
#include "../h/inode.h"
#include "../h/ioctl.h"
#include "../h/uerror.h"
#include "../h/signal.h"
#include "../h/uproc.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Variáveis e definições globais 				*
 ****************************************************************
 */
entry DEVHEAD	ramdtab;	/* Cabeca da lista de dp's e do major */
entry BHEAD	rramdbuf;	/* para as operações RAW */

/*
 ****************************************************************
 *	Prepara a utilização da unidade				*
 ****************************************************************
 */
int
ramdattach (int major)
{
	return (0);

}	/* end ramdattach */

/*
 ****************************************************************
 *	Executa um pedido de entrada/saída			*
 ****************************************************************
 */
int
ramdstrategy (BHEAD *bp)
{
	const DISKTB	*up;
	void		*addr, *endaddr;

	/*
	 *	Verifica a validade do pedido
	 */
	up = &disktb[MINOR (bp->b_phdev)];

	switch (up->p_target)
	{
	    case 0:
		addr	= scb.y_ramd0 + BLTOBY (bp->b_phblkno);
		endaddr = scb.y_endramd0;
		break;

	    case 1:
		addr	= scb.y_ramd1 + BLTOBY (bp->b_phblkno);
		endaddr = scb.y_endramd1;
		break;

	    case 2:		/* PIPE */
		printf ("%g: Chamada para PIPE\n");
		print_calls ();
		return (-1);

	    default:
	    case_bad:
		bp->b_flags |= B_ERROR;
		bp->b_error = ENXIO;
		EVENTDONE (&bp->b_done);
		return (-1);

	}	/* end switch (minor) */

	if (bp->b_phblkno < 0 || addr + bp->b_base_count > endaddr)
		goto case_bad;

	/*
	 *	Executa o  comando
	 */
	switch (bp->b_flags & (B_READ|B_WRITE))
	{
	    case B_READ:
		memmove (bp->b_base_addr, addr, bp->b_base_count);
		break;

	    case B_WRITE:
		memmove (addr, bp->b_base_addr, bp->b_base_count);
		break;

	}	/* end switch */

	bp->b_residual = 0;

	EVENTDONE (&bp->b_done);

	/*
	 *	Para não segurar muito o sistema
	 */
	if (u.u_pri >= PTTYOUT)
		u.u_pri = PTTYOUT-1;

	return (0);

}	/* end ramdstrategy */

/*
 ****************************************************************
 *	Leitura de modo não estruturado				*
 ****************************************************************
 */
void
ramdread (IOREQ *iop)
{
	physio (iop, ramdstrategy, &rramdbuf, B_READ, 0 /* dma */);

}	/* end ramdread */

/*
 ****************************************************************
 *	Escrita de modo não estruturado				*
 ****************************************************************
 */
void
ramdwrite (IOREQ *iop)
{
	physio (iop, ramdstrategy, &rramdbuf, B_WRITE, 0 /* dma */);

}	/* end ramdwrite */

/*
 ****************************************************************
 *	Função de controle					*
 ****************************************************************
 */
int
ramdctl (dev_t dev, int cmd, int arg, int flag)
{
	switch (cmd)
	{
	    case DKISADISK:
		return (0);
	}

	return (-1);

}	/* end ramdioctl */
