/*
 ****************************************************************
 *								*
 *			findIconFile.c				*
 *								*
 *	Localiza um arquivo contendo um ícone			*
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

#include <sys/syscall.h>
#include <string.h>
#include <stdlib.h>

#include "fvwmlib.h"

char *
findIconFile (char *icon, char *pathlist, int type)
{
	char	*path, *dir_end;
	int	l1, l2;

	l1 = (icon     == NOSTR) ? 0 : strlen (icon);
	l2 = (pathlist == NOSTR) ? 0 : strlen (pathlist);

	path = safemalloc (l1 + l2 + 10);
	*path = '\0';
	if (*icon == '/') 
		return (strcpy (path, icon));
     
	if (pathlist == NOSTR || *pathlist == '\0')
		return (strcpy (path, icon));
 
	while (pathlist && *pathlist)
	{ 
		if ((dir_end = strchr (pathlist, ':')) == NOSTR)
		{
			strcpy (path, pathlist);
		}
		else
		{
			strncpy (path, pathlist, dir_end - pathlist);
			path[dir_end - pathlist] = 0;
		}

		strcat (path, "/");
		strcat (path, icon);

		if (access (path, type) == 0) 
			return (path);

		strcat (path, ".gz");
		if (access (path, type) == 0) 
			return (path);

		if (dir_end == NOSTR)
			pathlist = NOSTR;
		else
			pathlist = dir_end + 1;
	}

	free (path);
	return (NOSTR);

}	/* end findIconFile */
