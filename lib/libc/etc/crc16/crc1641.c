/*
 ****************************************************************
 *								*
 *			crc1641.c				*
 *								*
 *	Calcula o CRC de 16 bits, baseado no CCITT norma V.41	*
 *								*
 *	Versão	2.3.0, de 05.12.91				*
 *		3.0.0, de 09.06.95				*
 *								*
 *	Módulo: crc1641						*
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
 *	O CRC (Cyclic Redundancy Check) é definido pela		*
 *	norma V.41 do CCITT como o resto da			*
 * 	divisão (módulo 2) da mensagem em "area"		*
 * 	acrescida de 16 bits zero				*
 * 	pelo polinômio x**16 + x**15 + x**2 + 1			*
 * 								*
 ****************************************************************
 */
int
crc1641 (const void *area, register int nbytes)
{
	register unsigned short	bcc;
	register const char	*cp = area;
	extern const short	crctb[];

	bcc = 0;

	while (nbytes-- > 0)
		bcc = crctb[(bcc >> 8) ^ *cp++] ^ (bcc << 8);

	return (bcc);

}	/* end crc1641 */
