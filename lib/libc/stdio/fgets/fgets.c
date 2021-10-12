/*
 ****************************************************************
 *								*
 *			fgets.c					*
 *								*
 *	Le uma linha de um arquivo (mantendo o <nl>)		*
 *								*
 *	Versão	1.0.0, de 17.11.86				*
 *		3.0.0, de 08.07.95				*
 *								*
 *	Módulo: fgets						*
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
 *	Le uma linha de um arquivo (mantendo o <nl>)		*
 ****************************************************************
 */
char *
fgets (char *str, register int n, register FILE *fp)
{
	register char	*cp = str;
	register int	c = 0;

	while (--n > 0 && (c = getc (fp)) >= 0)
	{
		*cp++ = c;

		if (c == '\n')
			break;
	}

	if (cp == str && c < 0)
		return ((char *)NULL);

	*cp = '\0';

	return (str);

}	/* end fgets */
