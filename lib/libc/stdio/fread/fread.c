/*
 ****************************************************************
 *								*
 *			fread.c					*
 *								*
 *	Le um registro						*
 *								*
 *	Versão	1.0.0, de 07.11.86				*
 *		3.0.0, de 03.01.95				*
 *								*
 *	Módulo: fread						*
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
 *	Lê um registro						*
 ****************************************************************
 */
size_t
fread (void *buf, register size_t size, register size_t nregs, register FILE *fp)
{
	register int	i, j, c;
	register char	*cp = buf;

	if (size <= 0 || nregs <= 0)
		return (0);

	for (i = 0; i < nregs; i++)
	{
		for (j = size; j > 0; j--)
		{
			if ((c = getc (fp)) >= 0)
				*cp++ = c;
			else
				return (i);
		}
	}

	return (i);

}	/* end fread */
