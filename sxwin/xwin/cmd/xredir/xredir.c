/*
 ****************************************************************
 *								*
 *			xredir.c				*
 *								*
 *	Redireciona mensagens do kernel para um arquivo		*
 *								*
 *	Versão	1.0.0, de 26.02.98				*
 *		4.4.0, de 19.03.03				*
 *								*
 *	Módulo: cmd/xredir					*
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
#include <string.h>
#include <fcntl.h>
#include <termio.h>
#include <signal.h>

entry	const char	initab[] =	"/etc/initab";

/*
 ****************************************************************
 *	Variáveis globais					*
 ****************************************************************
 */
entry	int	sig_received;

entry	char	ptyc_name[20],
		ptys_name[20];

entry	int	ptyc_fd,
		ptys_fd;

entry	char	buf[512];

/*
 ****************************************************************
 *	Protótipos						*
 ****************************************************************
 */
void	catch_signals (int, ...);
int	search_available_pty (char *, int *, char *, int *);

/*
 ****************************************************************
 *	Ponto de Entrada					*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	int		log, n;
	TERMIO		old_termio, new_termio, *tp;

	if (fork ())
		return (0);

	search_available_pty (ptyc_name, &ptyc_fd, ptys_name, &ptys_fd);

	log = open ("xredir.log", O_WRONLY|O_TRUNC|O_CREAT, 0777);

	signal (SIGTERM, catch_signals);
	signal (SIGHUP,  catch_signals);
	signal (SIGQUIT, catch_signals);
	signal (SIGINT,  catch_signals);

	/*
	 *	Põe a entrada do terminal em RAW
	 */
	ioctl (ptyc_fd, TCGETS, tp = &old_termio);

	tp = &new_termio; *tp = old_termio;

	tp->c_oflag &= ~OPOST;
	tp->c_lflag &= ~(ISIG|ICANON|ECHO);
	tp->t_min = 1;

	ioctl (ptyc_fd, TCSETNR, tp);

	close (ptyc_fd);
	ptyc_fd = -1;

	/*
	 *	Desvia as mensagens endereçadas à tela corrente.
	 */
	ioctl (ptys_fd, TC_KERNEL_PTY_ON);

	for (sig_received = 0; sig_received == 0; )
	{
		n = read (ptys_fd, buf, sizeof (buf));
		write (log, buf, n);
	}

	close (log);

	ioctl (ptys_fd, TC_KERNEL_PTY_OFF);
	close (ptys_fd);

	return (0);

}	/* end main */

/*
 ****************************************************************
 *	Captura os sinais					*
 ****************************************************************
 */
void
catch_signals (int signo, ...)
{
	sig_received = 1;

}	/* end catch_signals */

/*
 ****************************************************************
 *	Le o arquivo "initab" procurando um pseudo terminal 	*
 ****************************************************************
 */
int
search_available_pty (char *ptyc_nm, int *pptyc_fd,
		      char *ptys_nm, int *pptys_fd)
{
	FILE	*fp;
	char	*line_nm, state, buf[120];
	int	fdc, fds, slot;

	if ((fp = fopen (initab, "r")) == NOFILE)
		return (-1);

	for (slot = 0; fgets (buf, sizeof (buf), fp) == buf; slot++)
	{
		state   = *strfield (buf,   ':');
		line_nm =  strfield (NOSTR, ':');

		if (state != '+' || strncmp (line_nm, "pty", 3) != 0)
			continue;

		strcpy (ptyc_nm, "/dev/");
		strcat (ptyc_nm, line_nm);
		strcpy (ptys_nm, ptyc_nm);
		ptys_nm[8] = 's';

		if ((fds = open (ptys_nm, O_RDWR|O_LOCK)) >= 0)
		{
			if ((fdc = open (ptyc_nm, O_RDWR|O_LOCK)) >= 0)
			{
				*pptys_fd = fds;
				*pptyc_fd = fdc;

				fclose (fp);
				return (slot);
			}

			close (fds);
		}
	}

	fclose (fp);
	return (-1);

}	/* end search_available_pty */
