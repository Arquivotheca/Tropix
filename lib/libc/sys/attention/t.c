
#include <sys/types.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <signal.h>
#include <fcntl.h>
#include <termio.h>

const char *pgname;

main (int argc, register const char *argv[])
{
	register int	r, fd, index = -1;
	const char	*fnm1 = "/dev/console";
	const char	*fnm2 = "/dev/video2";
	int		fd_vector[20];
	char		area[80];

	pgname = argv[0];

	if (argv[1])
	{
		fnm1 = argv[1];

		if (argv[2])
			fnm2 = argv[2];
	}

	if ((fd_vector[0] = open (fnm1, O_RDONLY)) < 0)
	{
		printf ("Erro no open 1\n");
		perror (argv[0]);
		exit (1);
	}

	if ((fd_vector[1] = open (fnm2, O_RDONLY)) < 0)
	{
		printf ("Erro no open 2\n");
		perror (argv[0]);
		exit (1);
	}

	printf ("fnm1 = \"%s\", fd[0] = %d\n", fnm1, fd_vector[0]);
	printf ("fnm2 = \"%s\", fd[1] = %d\n", fnm2, fd_vector[1]);

	for (;;)
	{
		if ((index = attention (2, fd_vector, index, 0)) < 0)
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
