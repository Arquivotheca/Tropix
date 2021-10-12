/*
 ****************************************************************
 *								*
 *			l3tol.c					*
 *								*
 *	Converte endereços de disco entre os formatos 3/4 bytes	*
 *								*
 *	Versão	2.1.0, de 18.08.88				*
 *		3.0.0, de 07.01.95				*
 *								*
 *	Módulo: l3tol						*
 *		libc/etc 					*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <stdio.h>
#include <stdlib.h>

/*
 ****************************************************************
 *	Conversão 3 => 4					*
 ****************************************************************
 */
void
l3tol (long *dp, register const char *s, register int n)
{
	register char *d;

	d = (char *)dp;

#ifdef	LITTLE_ENDIAN
	while (n-- > 0)
	{
		d[0] = s[2];
		d[1] = s[1];
		d[2] = s[0];
		d[3] = 0;

		d += 4;
		s += 3;
	}
#else
	while (n-- > 0)
	{
		*d++ = 0;
		*d++ = *s++;
		*d++ = *s++;
		*d++ = *s++;
	}
#endif	LITTLE_ENDIAN

}	/* end l3tol */

/*
 ****************************************************************
 *	Conversão 4 => 3					*
 ****************************************************************
 */
void
ltol3 (register char *d, const long *dp, register int n)
{
	register const char	*s;

	s = (char *)dp;

#ifdef	LITTLE_ENDIAN
	while (n-- > 0)
	{
		if (s[3] != 0)
			fprintf (stderr, "ltol3: Endereço inválido (> 2^24)\n");

		d[0] = s[2];
		d[1] = s[1];
		d[2] = s[0];

		s += 4;
		d += 3;
	}
#else
	while (n-- > 0)
	{
		if (*s++ != 0)
			fprintf (stderr, "ltol3: Endereço inválido (> 2^24)\n");

		*d++ = *s++;
		*d++ = *s++;
		*d++ = *s++;
	}
#endif	LITTLE_ENDIAN

}	/* end ltol3 */
