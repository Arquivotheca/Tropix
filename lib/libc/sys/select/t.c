#include <sys/types.h>
#include <sys/syscall.h>
#include <sys/times.h>
#include <sys/select.h>

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <termio.h>

int
main (int argc, const char *argv[])
{
	int		r, fd, nfd, nfound;
	const char	*fnm1 = "/dev/console";
	const char	*fnm2 = "/dev/video2";
	int		fd_vector[20];
	char		area[80];
	int		req_mask, ret_mask;

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

	req_mask = (1 << fd_vector[0]) | (1 << fd_vector[1]);

	nfd = fd_vector[0];

	if (nfd < fd_vector[1])
		nfd = fd_vector[1];

	nfd++;

	printf ("nfd = %d, req_mask = %P\n", nfd, req_mask);

	for (;;)
	{
		ret_mask = req_mask;

		if ((nfound = select (nfd, &ret_mask, 0, 0, 0)) < 0)
			error ("$*Erro no select");

		fd = ffbs (ret_mask);

		printf
		(	"nfound = %d, máscara = %P, fd = %d\n",
			nfound, ret_mask, fd
		);

		if ((r = read (fd, area, 80)) < 0)
			error ("$*Erro no read");

		if (r == 0)
			return (0);

		write (1, area, r);

	}	/* end for EVER */

}
