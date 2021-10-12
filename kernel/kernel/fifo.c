#include <sys/types.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <termio.h>

int
main (int argc, const char *argv[])
{
	int	fd;

	if (argc == 1)
		return (0);

	if ((fd = open (argv[1], 0)) < 0)
	{
		printf ("Erro no open\n");
		exit (1);
	}

	printf ("fifo = %d\n", ioctl (fd, TCFIFOSZ, 0));

	close (fd);

	return (0);
}
