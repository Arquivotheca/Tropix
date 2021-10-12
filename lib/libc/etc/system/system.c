/*
 ****************************************************************
 *								*
 *			system.c				*
 *								*
 *	Executa um comando atraves do "shell"			*
 *								*
 *	Versão	1.0.0, de 20.11.86				*
 *		3.0.0, de 26.06.95				*
 *								*
 *	Módulo: system						*
 *		libc/etc					*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/syscall.h>

#include <stdlib.h>
#include <signal.h>

/*
 ****************************************************************
 *	Executa um Comando					*
 ****************************************************************
 */
int
system (const char *str)
{
	int		status, pid, w;
	register void	(*istat) (int, ...), (*qstat) (int, ...);

	if (str == NOSTR)	/* Temos interpretador de comandos */
		return (1);

	if ((pid = fork ()) == 0)
	{
		execl ("/bin/sh", "sh", "-c", str, 0);
		return (-1);
	}
 
	istat = signal (SIGINT,  SIG_IGN);
	qstat = signal (SIGQUIT, SIG_IGN);

	while ((w = wait (&status)) != pid && w != -1)
		/* vazio */;

	if (w == -1)
		status = -1;

	signal (SIGINT,  istat);
	signal (SIGQUIT, qstat);

	return (status);

}	/* end system */
