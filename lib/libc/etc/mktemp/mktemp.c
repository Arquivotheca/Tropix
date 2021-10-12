/*
 ****************************************************************
 *								*
 *			mktemp.c				*
 *								*
 *	Gera nomes para arquivos temporários			*
 *								*
 *	Versão	1.0.0, de 30.10.86				*
 *		3.0.0, de 03.01.95				*
 *								*
 *	Módulo: mktemp						*
 *		libc/etc					*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/*
 ****************************************************************
 *	Gera nomes de arquivos temporários			*
 ****************************************************************
 */
char *
mktemp (const char *user_pattern)
{
	register char	*pattern;
	register char	*Xbegin, *cp;
	register char	c;

	/*
	 *	Tira uma cópia, pois o original pode estar no TEXT
	 */
	if ((pattern = strdup (user_pattern)) == NOSTR)
		return (NOSTR);

	/*
	 *	Verifica se o padrão dado é da forma ".....XXXXXX"
	 */
	if ((Xbegin = strchr (pattern, 'X')) == NOSTR)
		return (NOSTR);

	if ((cp = strrchr (pattern, 'X')) == NOSTR || cp - Xbegin != 5)
		return (NOSTR);

	sprintf (Xbegin + 1, "%05d", getpid () % 100000 ); 

	for (c = 'a'; c <= 'z'; c++)
	{
		*Xbegin = c;

		if (access (pattern, F_OK) < 0)
			return (pattern);
	}

	return (NOSTR);

}	/* end mktemp */
