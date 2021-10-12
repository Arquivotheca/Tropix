/*
 ****************************************************************
 *								*
 *			mkcrctb.c				*
 *								*
 *	Gera a Tabela de CRC					*
 *								*
 *	Versão	1.0.0, de 28.08.86				*
 *		3.0.0, de 18.06.95				*
 *								*
 *	Módulo: crc16						*
 *		libc/etc					*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <mkasmtb.h>

/*
 ******	Definições **********************************************
 */
#define	NULL	0
#define	NOSTR	(char *)NULL

				/*	      16    12    5	*/
#define	POL	0x11021		/* Polinômio x   + x   + x  + 1 */
#define	MASK	0x10000		/* Padrão CRC-CCITT */

const long	x[] =
{
	POL << 7, 	POL << 6, 	POL << 5, 	POL << 4,
	POL << 3, 	POL << 2, 	POL << 1, 	POL << 0
};

const long	m[] =
{
	MASK << 7, 	MASK << 6, 	MASK << 5, 	MASK << 4,
	MASK << 3, 	MASK << 2, 	MASK << 1, 	MASK << 0
};

short		crctb[256];

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
 *	Entrada do Programa					*
 ****************************************************************
 */
main ()
{
	register short	i, j;
	register long	crc, k;


	crc = 0; k = 0;

	for (i = 0; i < 256; i++)
	{
		for (j = 0; j < 8; j++)
		{
			if (crc & m[j])
				crc = crc ^ x[j];
		}

		crctb[i] = crc;

		k += 0x10000; crc = k;
	}

	mkasmtb
	(	".crctb.s",
		"crctb",
		crctb,
		sizeof (crctb),
		ROL,
		sizeof (short),
		ent_desc
	);

}	/* end mkcrctb */
