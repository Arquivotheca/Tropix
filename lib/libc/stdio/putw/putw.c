/*
 ****************************************************************
 *								*
 *			putw.c					*
 *								*
 *	Escreve uma palavra em um arquivo			*
 *								*
 *	Versão	1.0.0, de 30.12.86				*
 *		3.0.0, de 10.07.95				*
 *								*
 *	Módulo: putw 						*
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
 *	Escreve uma palava em um arquivo			*
 ****************************************************************
 */
int
putw (int w, register FILE *fp)
{
	register short	i;
	register int 	c;
	register char	*s;

	s = (char *)&w;

	for (i = sizeof (int); i > 0; i--)
		{ c = *s++; putc (c, fp); }

	return (ferror (fp));

}	/* end putw */
