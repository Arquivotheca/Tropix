/*
 ****************************************************************
 *								*
 *			strcasecmp.c				*
 *								*
 *	Compara cadeias, desprezando maiúsculas/minúsculas	*
 *								*
 *	Versão	1.0.0, de 16.06.97				*
 *		1.0.1, de 28.03.98				*
 *								*
 *	Módulo: xc/lib/aux					*
 *		X Windows - Bibiotecas				*
 *		Baseado no software homônimo do XFree86		*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright (c) 1998 TROPIX Technologies Ltd.	*
 * 								*
 ****************************************************************
 */

#include <string.h>

int
strcasecmp (char *s1, char *s2)
{
	int	c1, c2, n, n2;

	n  = strlen (s1);
	n2 = strlen (s2);

	if (n != n2)
		return (1);	/* muito estranho */

	for (;;)
	{
		c1 = *s1; 
		c2 = *s2;

		if (!c1 || !c2) 
			return (c1 - c2);

		if (c1 >= 'A' && c1 <= 'Z')
			c1 += 'a' - 'A';

		if (c2 >= 'A' && c2 <= 'Z')
			c2 += 'a' - 'A';

		if (c1 != c2) 
			return (c1 - c2);

		n--;
		s1++;
		s2++;
	}

}	/* end strcasecmp */
