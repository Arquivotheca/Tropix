/*
 ****************************************************************
 *								*
 *			t_str_to_addr.c				*
 *								*
 *	Decodifica um nome de nó simbólico ("d.d.d.d")		*
 *								*
 *	Versão	2.3.0, de 31.08.91				*
 *		3.0.0, de 10.05.95				*
 *								*
 *	Módulo: t_str_to_addr					*
 *		libxti 						*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 * 		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <xti.h>

/*
 ****************************************************************
 *	Decodifica um nome de nó simbólico ("d.d.d.d")		*
 ****************************************************************
 */
long
t_str_to_addr (const char *node_str)
{
	register long		i, n;
	register char		c = '.';
	register const char	*cp = node_str;
	const char		*str;
	char			node[4];
	register char		*np;

#ifdef	LITTLE_ENDIAN
	np = &node[4];
#else
	np = &node[0];
#endif	LITTLE_ENDIAN

	for (i = 0; i < 4; i++)
	{
		if (i == 3)
			c = '\0';

		if ((n = strtol (cp, &str, 0)) & ~0xFF || *str != c || str == cp)
			return (-1);

		cp = str + 1;

#ifdef	LITTLE_ENDIAN
		*--np = n;
#else
		*np++ = n;
#endif	LITTLE_ENDIAN
	}

	return (*(long *)node);

}	/* end t_str_to_addr */
