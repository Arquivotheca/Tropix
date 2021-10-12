/*
 ****************************************************************
 *								*
 *			CatString3.c				*
 *								*
 *	Concatena 3 cadeias					*
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

#include <stdio.h>
#include <string.h>

#define	MAXLEN	255

char *
CatString3 (char *a, char *b, char *c)
{
	static char	s[MAXLEN + 1];
	int		len = 0;

	if (a != NOSTR)
		len += strlen (a);

	if (b != NOSTR)
		len += strlen (b);

	if (c != NOSTR)
		len += strlen (c);

	if (len > MAXLEN)
		return (NOSTR);

	if (a != NOSTR)
		strcpy (s, a);
	else
		s[0] = '\0';

	if (b != NOSTR)
		strcat (s, b);

	if (c != NOSTR)
		strcat (s, c);

	return (s);

}	/* end CatString3 */
