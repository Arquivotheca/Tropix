/*
 ****************************************************************
 *								*
 *			mkcrcstb.c				*
 *								*
 *	Gera a Tabela de CRC					*
 *								*
 *	Versão	1.0.0, de 28.08.86				*
 *		3.0.0, de 18.06.95				*
 *								*
 *	Módulo: crc16s						*
 *		libc/etc					*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <stdlib.h>
#include <mkasmtb.h>

/*
 ******	Define a entrada ****************************************
 */
const OBJENT	ent_desc[] =
{
	sizeof (short), 0, 1, NOSTR, (void *)NULL,
	0
};

/*
 ****************************************************************
 *	Tabelas		 					*
 ****************************************************************
 */
				/*	      16    12    5	*/
#define	POL	0x11021		/* Polinômio x   + x   + x  + 1 */
#define	MASK	0x10000		/* Padrão CRC-CCITT */

entry long	x[] =
{
	POL << 3, 	POL << 2, 	POL << 1, 	POL << 0
};

entry long	m[] =
{
	MASK << 3, 	MASK << 2, 	MASK << 1, 	MASK << 0
};

entry short	crcstb[16];

/*
 ****************************************************************
 *	Entrada do Programa					*
 ****************************************************************
 */
main ()
{
	register short	i, j;
	register long	crc, k;

	crc = 0; k = 0;

	for (i = 0; i < 16; i++)
	{
		for (j = 0; j < 4; j++)
		{
			if (crc & m[j])
				crc ^= x[j];
		}

		crcstb[i] = crc;

		k += 0x10000;
		crc = k;
	}

	mkasmtb
	(	".crcstb.s",
		"crcstb",
		crcstb,
		sizeof (crcstb),
		ROL,
		sizeof (short),
		ent_desc
	);

}	/* end mkcrcstb */
