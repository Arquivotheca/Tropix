/*
 ****************************************************************
 *								*
 *			dma.c					*
 *								*
 *	Controla o DMA do PC					*
 *								*
 *	Versão	3.0.0, de 19.01.95				*
 *		3.2.3, de 18.04.00				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include "../h/common.h"
#include "../h/scb.h"
#include "../h/sync.h"
#include "../h/bhead.h"
#include "../h/region.h"
#include "../h/signal.h"
#include "../h/uerror.h"
#include "../h/uproc.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****** Lista dos registros do DMA ******************************
 */
#define	IO_DMA1		0x00	/* 8237A DMA Controller #1 */
#define	IO_ICU1		0x20	/* 8259A Interrupt Controller #1 */
#define	IO_DMA2		0xC0	/* 8237A DMA Controller #2 */

#define	DMA37MD_SINGLE	0x40	/* Single pass mode */
#define	DMA37MD_CASCADE	0xC0	/* Cascade mode */
#define	DMA37MD_AUTO	0x50	/* Autoinitialise single pass mode */
#define	DMA37MD_WRITE	0x04	/* Read the device, write memory operation */
#define	DMA37MD_READ	0x08	/* Write the device, read memory operation */

#define	DMA1_CHN(c)	(IO_DMA1 + 1*(2*(c)))	/* Addr reg for channel c */
#define	DMA1_SMSK	(IO_DMA1 + 1*10)	/* Single mask register */
#define	DMA1_MODE	(IO_DMA1 + 1*11)	/* Mode register */
#define	DMA1_FFC	(IO_DMA1 + 1*12)	/* Clear first/last FF */

#define	DMA2_CHN(c)	(IO_DMA2 + 2*(2*(c)))	/* Addr reg for channel c */
#define	DMA2_SMSK	(IO_DMA2 + 2*10)	/* Single mask register */
#define	DMA2_MODE	(IO_DMA2 + 2*11)	/* Mode register */
#define	DMA2_FFC	(IO_DMA2 + 2*12)	/* Clear first/last FF */

/*
 ******	Vetor de endereços **************************************
 */
const int	uni_dma_page_port[8] = { 0x87, 0x83, 0x81, 0x82, 0x8F, 0x8B, 0x89, 0x8A };

/*
 ****************************************************************
 *	Programa o DMA						*
 ****************************************************************
 */
int
uni_dma_setup (int rw, void *area, int count, int channel)
{
	int		value, port;

	if ((unsigned)channel > 7)
	{
		printf ("uni_dma_setup: Canal %d inválido\n", channel);
		u.u_error = EINVAL;
		return (-1);
	}

	/*
	 *	Finalmente, programa o controlador de DMA
	 */
	if (channel < 4)
	{
		/* Canais 0 .. 3 */

		value = channel;

		if (rw & B_AUTO)
			value |= DMA37MD_AUTO;
		else
			value |= DMA37MD_SINGLE;

		if (rw & B_READ)
			value |= DMA37MD_WRITE;
		else
			value |= DMA37MD_READ;

		write_port (value, DMA1_MODE);
		write_port (0, DMA1_FFC);

		/* send start address */

		port =  DMA1_CHN (channel);

		write_port ((unsigned)area, port);
		write_port ((unsigned)area >> 8, port);
		write_port ((unsigned)area >> 16, uni_dma_page_port[channel]);

		/* send count */

		write_port (--count, port + 1);
		write_port (count >> 8, port + 1);

		/* unmask channel */

		write_port (channel, DMA1_SMSK);
	}
	else
	{
		/* Canais 4 .. 7 */

		value = (channel & 3);

		if (rw & B_AUTO)
			value |= DMA37MD_AUTO;
		else
			value |= DMA37MD_SINGLE;

		if (rw & B_READ)
			value |= DMA37MD_WRITE;
		else
			value |= DMA37MD_READ;

		write_port (value, DMA2_MODE);
		write_port (0, DMA2_FFC);

		/* send start address */

		port = DMA2_CHN (channel - 4);

		write_port ((unsigned)area >> 1, port);
		write_port ((unsigned)area >> 9, port);
		write_port ((unsigned)area >> 16, uni_dma_page_port[channel]);

		/* send count */

		count >>= 1;
		write_port (--count, port + 2);
		write_port (count >> 8, port + 2);

		/* unmask channel */

		write_port (channel & 3, DMA2_SMSK);
	}

	return (0);

}	/* uni_dma_setup */

/*
 ****************************************************************
 *	Programa DMA externo (controlado por uma placa)		*
 ****************************************************************
 */
void
dmaexternal (int channel)
{
	int	dma_mode_port;

	if ((unsigned)channel > 7)
	{
		printf ("dmaexternal: No. de canal (%d) inválido\n", channel);
		return;
	}

	if ((channel & 4) == 0)
		dma_mode_port = IO_DMA1 + 0x0B;
	else
		dma_mode_port = IO_DMA2 + 0x16;

	write_port (0xC0 | (channel & 3), dma_mode_port);

	if ((channel & 4) == 0)
		write_port (channel & 3, dma_mode_port - 1);
	else
		write_port (channel & 3, dma_mode_port - 2);

}	/* end dmaexternal */
