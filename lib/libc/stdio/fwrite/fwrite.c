/*
 ****************************************************************
 *								*
 *			fwrite.c				*
 *								*
 *	Escreve registros em um arquivo				*
 *								*
 *	Versão	1.0.0, de 18.11.86				*
 *		3.0.0, de 09.07.95				*
 *								*
 *	Módulo: fwrite						*
 *		libc/stdio					*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <stdio.h>

/*
 ****************************************************************
 *	Escreve registros					*
 ****************************************************************
 */
size_t
fwrite (const void *str, register size_t size, register size_t nregs, register FILE *fp)
{
	register const char	*cp = str;
	register int		i, j, c;

	if (nregs <= 0 || size <= 0)
		return (0);

	for (i = 0; i < nregs; i++)
	{
		for (j = size; j > 0; j--)
			{ c = *cp++; putc (c, fp); }

		if (ferror (fp))
			return (i);
	}

	return (i);

}	/* end fwrite */
