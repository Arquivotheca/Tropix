/*
 ****************************************************************
 *								*
 *			getenv.c				*
 *								*
 *	Obtem o valor de uma variável do ambiente		*
 *								*
 *	Versão	1.0.0, de 27.10.86				*
 *		3.0.0, de 01.06.95				*
 *								*
 *	Módulo: getenv						*
 *		libc/etc					*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <stdlib.h>
#include <string.h>

/*
 ****************************************************************
 *	Obtem o valor de uma variável do ambiente		*
 ****************************************************************
 */
const char *
getenv (const char *var)
{
	register int		size;
	register const char	**ep;
	register const char	*cp;

	/*
	 *	Verifica se há ambiente
	 */
	if ((ep = environ) == (const char **)NULL)
		return (NOSTR);

	/*
	 *	Procura a variável no ambiente
	 */
	size = strlen (var);

	while (cp = *ep++)
	{
		if (cp[size] == '=' && strncmp (cp, var, size) == 0)
			return (cp + size + 1);
	}

	return (NOSTR);

}	/* end getenv */
