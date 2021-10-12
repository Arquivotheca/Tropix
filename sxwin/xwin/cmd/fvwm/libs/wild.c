/*
 ****************************************************************
 *								*
 *			wild.c					*
 *								*
 *	Verifica se uma cadeia satisfaz um padrão		*
 *								*
 *	Versão	1.0.0, de 03.08.97				*
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

#include <stdlib.h>
#include <string.h>

int
matchWildcards (char *pattern, char *string)
{
	if (string == NOSTR)
	{
		if (pattern == NOSTR || streq (pattern, "*"))
			return (1);

		return (0);
	}

	if (pattern == NOSTR)
		return (1);

	return (patmatch (string, pattern));

}	/* end matchWildcards */
