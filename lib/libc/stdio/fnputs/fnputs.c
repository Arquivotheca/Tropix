/*
 ****************************************************************
 *								*
 *			fnputs.c				*
 *								*
 *	Escreve uma cadeia de carac. em um arquivo (com <nl>)	*
 *								*
 *	Versão	1.0.0, de 18.11.86				*
 *		3.0.0, de 08.07.95				*
 *								*
 *	Módulo: fnputs						*
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
 *	Escreve uma cadeia de carac. em um arquivo (com <nl>)	*
 ****************************************************************
 */
int
fnputs (const char *str, FILE *fp)
{
	int	c;

	while ((c = *str++) != '\0')
		putc (c, fp);

	putc ('\n', fp);

	if (ferror (fp))
		return (EOF);

	return (0);

}	/* end fnputs */
