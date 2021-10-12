/*
 ****************************************************************
 *								*
 *			gets.c					*
 *								*
 *	Lê uma linha da entrada padrão				*
 *								*
 *	Versão	1.0.0, de 17.11.86				*
 *		3.0.0, de 10.07.95				*
 *								*
 *	Módulo: gets						*
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
 *	Le uma linha da entrada padrão				*
 ****************************************************************
 */
char *
gets (char *s)
{
	register char	*cp;
	register int	c;

	cp = s;

	while ((c = getchar ()) != '\n' && c >= 0)
		*cp++ = c;

	if (cp == s && c < 0)
		return ((char *)NULL);

	*cp = '\0';

	return (s);

}	/* end gets */
