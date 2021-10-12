/*
 ****************************************************************
 *								*
 *			pty.c					*
 *								*
 *	Obtém um pseudo-terminal livre				*
 *								*
 *	Versão	1.0.0, de 08.06.97				*
 *								*
 *	Módulo: xc/lib/aux					*
 *		X Windows - Bibliotecas				*
 *		Baseado no software homônimo do XFree86		*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright (c) 1997 TROPIX Technologies Ltd.	*
 * 								*
 ****************************************************************
 */

#include <sys/syscall.h>

#include <stdio.h>
#include <string.h>
#include <fcntl.h>

static const char	initab[] =	"/etc/initab";

/*
 ****************************************************************
 *	Le o arquivo "initab" procurando um pseudo terminal 	*
 ****************************************************************
 */
int
search_available_pty (char *ptyc_name, int *ptyc_fd,
		      char *ptys_name, int *ptys_fd)
{
	FILE	*fp;
	char	*line_name, state, buf[120];
	int	fdc, fds, slot;

	if ((fp = fopen (initab, "r")) == NOFILE)
		return (-1);

	for (slot = 0; fgets (buf, sizeof (buf), fp) == buf; slot++)
	{
		state     = *strfield (buf,   ':');
		line_name =  strfield (NOSTR, ':');

		if (state != '+' || strncmp (line_name, "pty", 3) != 0)
			continue;

		strcpy (ptyc_name, "/dev/");
		strcat (ptyc_name, line_name);
		strcpy (ptys_name, ptyc_name);
		ptys_name[8] = 's';

		if ((fds = open (ptys_name, O_RDWR|O_LOCK)) >= 0)
		{
			if ((fdc = open (ptyc_name, O_RDWR|O_LOCK)) >= 0)
			{
				*ptys_fd = fds;
				*ptyc_fd = fdc;

				fclose (fp);
				return (slot);
			}

			close (fds);
		}
	}

	fclose (fp);
	return (-1);

}	/* end search_available_pty */
