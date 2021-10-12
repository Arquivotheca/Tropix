/*
 ****************************************************************
 *								*
 *			bio.c					*
 *								*
 *	Entrada/saída de BLOCOs					*
 *								*
 *	Versão	4.3.0, de 14.08.02				*
 *		4.6.0, de 21.10.04				*
 *								*
 *	Módulo: Boot2						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2004 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include <common.h>
#include <sync.h>

#include <stat.h>
#include <disktb.h>
#include <bhead.h>
#include <devmajor.h>

#include "../h/common.h"
#include "../h/extern.h"
#include "../h/proto.h"

/*
 ******	Protótipos de funções ***********************************
 */
void	print_invalid_major (const char *nm, const DISKTB *up);

/*
 ****************************************************************
 *	CACHE de um bloco de 4 KB para leitura			*
 ****************************************************************
 */
char		b_cache_area[BL4SZ];

const DISKTB	*b_cache_disktb = NULL;

int		b_cache_blkno;

/*
 ****************************************************************
 *	Abre o dispositivo					*
 ****************************************************************
 */
int
bopen (const DISKTB *pp)
{
	switch (MAJOR (pp->p_dev))
	{
	    case IDE_MAJOR:
		return (ataopen (pp));

	    case FD_MAJOR:
		return (fd_open (pp->p_target));

	    case SD_MAJOR:
		return (sdopen (pp->p_target));

	    case SC_MAJOR:
		return (ahc_open (pp));

	    case RAMD_MAJOR:
		if (ramd_addr == NOVOID)
			return (-1);
		else
			return (0);

	    default:
		print_invalid_major ("bopen", pp);
		return (-1);
	}

}	/* end bopen */

/*
 ****************************************************************
 *	Le um bloco do dispositivo				*
 ****************************************************************
 */
int
bread (const DISKTB *pp, daddr_t blkno, void *area, int count)
{
	BHEAD		bhead, *bp = &bhead;

	/*
	 *	Caso simples: bloco de 512 bytes
	 */
	if (pp->p_blshift == BLSHIFT)
	{
		bp->b_disktb	 = pp;
		bp->b_flags	 = B_READ;
		bp->b_blkno	 = blkno;
		bp->b_addr	 = area;
		bp->b_base_count = count;

		switch (MAJOR (pp->p_dev))
		{
		    case IDE_MAJOR:
			return (atastrategy (bp));

		    case FD_MAJOR:
			return (fd_strategy (bp));

		    case SD_MAJOR:
			return (sdstrategy (bp));

		    case SC_MAJOR:
			return (ahc_strategy (bp));

		    case RAMD_MAJOR:
			memmove (area, ramd_addr + BLTOBY (blkno), count);
			return (0);

		    default:
			print_invalid_major ("bread", pp);
			return (-1);
		}
	}

	/*
	 *	Caso complexo: bloco NÃO é de 512 bytes => usa o CACHE
	 */
	if (b_cache_disktb != pp || b_cache_blkno != BL4BASEBLK (blkno))
	{
		bp->b_disktb	 = pp;
		bp->b_flags	 = B_READ;
		bp->b_blkno	 = BL4BASEBLK (blkno);
		bp->b_addr	 = b_cache_area;
		bp->b_base_count = BL4SZ;

		switch (MAJOR (pp->p_dev))
		{
		    case IDE_MAJOR:
			atastrategy (bp);
			break;

		    case SD_MAJOR:
			sdstrategy (bp);
			break;

		    case SC_MAJOR:
			ahc_strategy (bp);
			break;

		    default:
			print_invalid_major ("bread", pp);
			return (-1);
		}

		b_cache_disktb = pp; b_cache_blkno = BL4BASEBLK (blkno);
	}

	/*
	 *	Copia o bloco desejado
	 */
	memmove (area, b_cache_area + BL4BASEOFF (blkno), count);

	return (0);

}	/* end bread */

/*
 ****************************************************************
 *	Escreve um Bloco em um dispositivo			*
 ****************************************************************
 */
int
bwrite (const DISKTB *pp, daddr_t blkno, void *area, int count)
{
	BHEAD		bhead, *bp = &bhead;

	/*
	 *	Somente para blocos de 512 bytes, ...
	 */
	if (pp->p_blshift != BLSHIFT)
	{
		printf ("bwrite: Tamanho de bloco (%d) inválido\n", 1 << pp->p_blshift);
		return (-1);
	}

	/*
	 *	Escreve o bloco
	 */
	bp->b_disktb	 = pp;
	bp->b_flags	 = B_WRITE;
	bp->b_blkno	 = blkno;
	bp->b_addr	 = area;
	bp->b_base_count = count;

	switch (MAJOR (pp->p_dev))
	{
	    case IDE_MAJOR:
		return (atastrategy (bp));

	    case FD_MAJOR:
		return (fd_strategy (bp));

	    case SD_MAJOR:
		return (sdstrategy (bp));

	    case SC_MAJOR:
		return (ahc_strategy (bp));

	    case RAMD_MAJOR:
		memmove (ramd_addr + BLTOBY (blkno), area, count);
		return (0);

	    default:
		print_invalid_major ("bwrite", pp);
		return (-1);
	}

}	/* end bwrite */

/*
 ****************************************************************
 *	Executa um comando especial em um dispositivo		*
 ****************************************************************
 */
int
bctl (const DISKTB *up, int cmd, int arg)
{
	/*
	 *	Executa a função
	 */
	switch (MAJOR (up->p_dev))
	{
	    case IDE_MAJOR:
		return (ata_ctl (up, cmd, arg));

	    case SD_MAJOR:
		return (sd_ctl (up, cmd, arg));

	    case SC_MAJOR:
		return (ahc_ctl (up, cmd, arg));

	    default:
		print_invalid_major ("bctl", up);
		return (-1);
	}

}	/* end bctl */

/*
 ****************************************************************
 *	Imprime a mensagem de MAJOR inválido			*
 ****************************************************************
 */
void
print_invalid_major (const char *nm, const DISKTB *up)
{
	printf ("%s: dispositivo \"%s\" com MAJOR inválido: %d\n", nm, up->p_name, MAJOR (up->p_dev));

}	/* end print_invalid_major */
