/*
 ****************************************************************
 *								*
 *			dma.c					*
 *								*
 *	Controla o DMA do PC					*
 *								*
 *	Versão	3.0.0, de 19.01.95				*
 *		3.0.0, de 03.04.95				*
 *								*
 *	Módulo: Boot2						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 1997 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include <common.h>

#include "../h/common.h"
#include "../h/extern.h"
#include "../h/proto.h"

/*
 ******	Definição de portas *************************************
 */
#define IO_DMA_1		0x00		/* Controlador 8237A #1 */
#define IO_DMA_2		0xC0		/* Controlador 8237A #2 */

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
		dma_mode_port = IO_DMA_1 + 0x0B;
	else
		dma_mode_port = IO_DMA_2 + 0x16;

	write_port (0xC0 | (channel & 3), dma_mode_port);

	if ((channel & 4) == 0)
		write_port (channel & 3, dma_mode_port - 1);
	else
		write_port (channel & 3, dma_mode_port - 2);

}	/* end dmaexternal */
