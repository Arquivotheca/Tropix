#include <sys/types.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <termio.h>

const char	*pgname;

void	readfunction ();

main (int argc, register const char *argv[])
{
	register int	r, fd, index = -1;
	char		*fnm1 = "/dev/tty";
	int		fd_vector[20];
	char		area[80];
	int		timeout = 0;

	pgname = argv[0];

	if (argv[1])
		timeout = atoi (argv[1]);

	printf ("TIMEOUT = %d\n", timeout);

	if ((fd_vector[0] = open (fnm1, O_RDONLY)) < 0)
	{
		printf ("Erro no open 1\n");
		perror (pgname);
		exit (1);
	}

	for (;;)
	{
		sleep (2);

		if ((index = attention (1, fd_vector, index, timeout)) < 0)
		{
			printf ("Erro no attention\n");
			perror (argv[0]);
			exit (1);
		}

		fd = fd_vector[index];

		printf ("attention retornou index = %d, fd = %d\n", index, fd);

		if ((r = read (fd, area, 80)) < 0)
		{
			printf ("Erro no read\n");
			perror (argv[0]);
			exit (1);
		}

		if (r == 0)
			exit (0);

		write (1, area, r);

	}	/* end for EVER */

}
