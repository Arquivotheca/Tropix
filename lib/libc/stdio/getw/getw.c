/*
 ****************************************************************
 *								*
 *			getw.c					*
 *								*
 *	Le uma palavra da entrada padrão (stdin)		*
 *								*
 *	Versão	1.0.0, de 18.11.86				*
 *		3.0.0, de 10.07.95				*
 *								*
 *	Módulo: getw 						*
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
 *	Le uma palavra de "stdin"				*
 ****************************************************************
 */
int
getw (register FILE *fp)
{
	register int	i;
	register char	*s;
	int		w;

	s = (char *)&w;

	for (i = sizeof (int); i > 0; i--)
		*s++ = getc (fp);

	if (fp->_flag & (_IOERR|_IOEOF))
		return (EOF);

	return (w);

}	/* end getw */
