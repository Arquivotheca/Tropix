/*
 ****************************************************************
 *								*
 *			crc16.c					*
 *								*
 *	Calcula o CRC de 16 bits, baseado no CCITT		*
 *								*
 *	Versão	1.0.0, de 28.08.86				*
 *		3.0.0, de 09.06.95				*
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

#include <stdlib.h>

/*
 ****************************************************************
 * 								*
 *	Calcula o CRC de 16 bits:				*
 * 								*
 *	O CRC (Cyclic Redundancy Check) é definido como o	*
 * 	resto da divisão (módulo 2) da mensagem em "area"	*
 * 	pelo polinômio x**16 + x**15 + x**2 + 1			*
 *	recomendado pelo CCITT					*
 * 								*
 ****************************************************************
 */
int
crc16 (const void *area, register int nbytes)
{
	register unsigned short	crc;
	register const char	*cp = area;
	extern const short	crctb[];

	crc = 0;

	while (nbytes-- > 0)
		crc = crctb[crc >> 8] ^ ((crc << 8) | *cp++);

	return (crc);

}	/* end crc16 */
