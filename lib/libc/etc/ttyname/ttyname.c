/*
 ****************************************************************
 *								*
 *			ttyname.c				*
 *								*
 *	Obtem o nome do terminal				*
 *								*
 *	Versão	1.0.0, de 08.09.86				*
 *		3.0.0, de 27.06.95				*
 *								*
 *	Módulo: ttyname						*
 *		libc/stdio					*
 *		Categoria D					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/syscall.h>

#include <stdlib.h>
#include <string.h>
#include <termio.h>

/*
 ****************************************************************
 *	Rotina "ttyname"					*
 ****************************************************************
 */
char *
ttyname (int fd)
{
	static char	name[20];
	TERMIO		termio;

	if (ioctl (fd, TCGETS, &termio) < 0)
		return (NOSTR);

	strcpy (name, "/dev/");
	strcat (name, termio.c_name);

	return (name);

}	/* end ttyname */
