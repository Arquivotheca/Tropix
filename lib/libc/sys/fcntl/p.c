#include <sys/syscall.h>

#include <fcntl.h>
#include <stdio.h>
#include <errno.h>

main (int argc, const char *argv[])
{
	register int	fd, n;

	fd = open ("../t.c", 0);
	if (fd < 0)
		printf ("Erro no open %s\n", strerror (errno));

	n = fcntl (fd, F_GETFL, 0);
	printf ("n = %x\n", n);

	n = fcntl (fd, F_SETFL, O_NDELAY);
	printf ("n = %x\n", n);

	n = fcntl (fd, F_GETFL, 0);
	printf ("n = %x\n", n);

	n = fcntl (fd, F_SETFL, O_NDELAY|O_TRUNC);
	printf ("n = %x, err = %s\n", n, strerror (errno));

	n = fcntl (fd, F_GETFL, 0);
	printf ("n = %x\n", n);
}
