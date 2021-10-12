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

	n = fcntl (fd, F_GETFD, 0);
	printf ("n = %d\n", n);

	n = fcntl (fd, F_SETFD, 121);
	printf ("n = %d\n", n);

	n = fcntl (fd, F_GETFD, 0);
	printf ("n = %d\n", n);
}
