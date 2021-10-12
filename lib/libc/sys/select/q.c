#include <sys/types.h>
#include <sys/syscall.h>
#include <sys/times.h>
#include <sys/select.h>

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <termio.h>

void	readfunction ();

int
main (int argc, register const char *argv[])
{
	int		r, fd;
	int		nfd, nfound;
	char		*fnm1 = "/dev/tty";
	char		area[80];
	fd_set		req_mask, ret_mask;
	MUTM		m, *m_ptr = NULL;

	if (argv[1])
	{
		m_ptr = &m;

		m.mu_time = atoi (argv[1]);

		if (argv[2])
			m.mu_utime = atoi (argv[2]);
	}

	printf ("TIMEOUT = (%d, %d)\n", m.mu_time, m.mu_utime);

	if ((fd = open (fnm1, O_RDONLY)) < 0)
	{
		printf ("Erro no open 1\n");
		perror (pgname);
		exit (1);
	}

#if (0)	/*******************************************************/
	req_mask = (1 << fd_vector[0]);
#endif	/*******************************************************/
	FD_SET (fd, &req_mask);

	nfd = fd + 1;

	printf
	(	"fd = %d, nfd = %d, req_mask = %P\n",
		fd, nfd, req_mask
	);

	for (;;)
	{
#if (0)	/*******************************************************/
		sleep (2);
#endif	/*******************************************************/

		ret_mask = req_mask;

		if ((nfound = select (nfd, &ret_mask, 0, 0, m_ptr)) < 0)
			error ("$*Erro no select");

#if (0)	/*******************************************************/
		fd = ffbs (ret_mask);
#endif	/*******************************************************/

		printf
		(	"nfound = %d, máscara = %P\n",
			nfound, ret_mask
		);

		if (nfound > 0)
		{
			if (FD_ISSET (fd, &ret_mask))
			{
				if ((r = read (fd, area, 80)) < 0)
					error ("$*Erro no read");

				if (r == 0)
					exit (0);

				write (1, area, r);
			}
			else
			{
				printf ("Descritor louco\n"); sleep (5);
			}
		}
		else
		{
			printf ("Nada para ler\n"); sleep (5);
		}

	}	/* end for EVER */

}
