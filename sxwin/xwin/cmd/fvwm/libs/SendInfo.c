/*
 ****************************************************************
 *								*
 *			SendInfo.c				*
 *								*
 *	Envia uma mensagem ao Fvwm				*
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

#include <sys/types.h>
#include <sys/syscall.h>
#include <stdio.h>

void
SendInfo (int *fdv, char *message, ulong window)
{
	int	len, fd;

	if (message == NOSTR)
		return;

	fd = fdv[0];

	write (fd, &window, sizeof (ulong));
      
	len = strlen (message);
	write (fd, &len, sizeof (len));
	write (fd, message, len);
      
	/* keep going */
	len = 1;
	write (fd, &len, sizeof (len));

}	/* end SendInfo */
