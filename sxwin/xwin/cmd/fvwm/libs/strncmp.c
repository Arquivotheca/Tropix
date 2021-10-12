/*
 ****************************************************************
 *								*
 *			strncmp.c				*
 *								*
 *	Comparação especial de cadeias				*
 *								*
 *	Versão	1.0.0, de 27.08.97				*
 *		4.4.0, de 20.03.03				*
 *								*
 *	Módulo: cmd/fvwm/libs					*
 *		X Windows - Clientes				*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright (c) 2003 TROPIX Technologies Ltd.	*
 * 								*
 ****************************************************************
 */

int
strncasecmp (char *s1, char *s2, int n)
{
	int	c1, c2;
  
	for (/* vazio */; n > 0; n--)
	{
		c1 = *s1++; c2 = *s2++;

		if (c1 == '\0' || c2 == '\0')
			return (c1 - c2);

		if (c1 >= 'A' && c1 <= 'Z')
			c1 += ('a' - 'A');

		if (c2 >= 'A' && c2 <= 'Z')
			c2 += ('a' - 'A');

		if (c1 != c2)
			return (c1 - c2);
	}

	return (0);

}	/* end strncasecmp */
