/*
 ****************************************************************
 *								*
 *			isatty.c				*
 *								*
 *	Verifica se o arquivo é um terminal			*
 *								*
 *	Versão	1.0.0, de 05.09.86				*
 *		3.0.0, de 19.12.94				*
 *								*
 *	Módulo:	isatty 						*
 *		libc/etc					*
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
#include <termio.h>

/*
 ****************************************************************
 *	Verifica se é um terminal				*
 ****************************************************************
 */
int
isatty (int fd)
{
	if (ioctl (fd, TCISATTY, 0) < 0)
		return (0);

	return (1);

}	/* isatty */
