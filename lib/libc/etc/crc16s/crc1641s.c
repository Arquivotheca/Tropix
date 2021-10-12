/*
 ****************************************************************
 *								*
 *			crc1641s.c				*
 *								*
 *	Calcula o CRC de 16 bits, baseado no CCITT norma V.41	*
 *								*
 *	Versão	2.3.0, de 05.12.91				*
 *		3.0.0, de 18.06.95				*
 *								*
 *	Módulo: crc1641s					*
 *		libc/etc					*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

/*
 ****************************************************************
 * 								*
 *	Calcula o CRC de 16 bits:				*
 * 								*
 *	O CRC (Cyclic Redundancy Check) é definido pela		*
 *	norma V.41 do CCITT como o resto da			*
 * 	divisão (módulo 2) da mensagem em buf 			*
 * 	acrescida de 16 bits zero				*
 * 	pelo polinômio x**16 + x**15 + x**2 + 1			*
 * 								*
 ****************************************************************
 */
int
crc1641s (register unsigned char *buf, register int nbytes)
{
	register unsigned short	crc;
	register unsigned char	b, c;
	extern const short	crcstb[];

	crc = 0;

	while (nbytes-- > 0)
	{
		b = *buf++;

		c = b >> 4;
		crc = crcstb[(crc >> 12) ^ c] ^ (crc << 4);

		c = b & 0x0F;
		crc = crcstb[(crc >> 12) ^ c] ^ (crc << 4);
	}

	return (crc);

}	/* end crc1641s */
