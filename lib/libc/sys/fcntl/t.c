
#include <sys/syscall.h>

#include <fcntl.h>
#include <stdio.h>
#include <errno.h>

main (int argc, const char *argv[])
{
	register int	fd, fda;
	char		buf[80];

	fd = open ("../t.c", 0);

	printf ("fd = %d\n", fd);

	if (fd < 0)
		printf ("Erro no open %s\n", strerror (errno));

	fda = fcntl (fd, F_DUPFD, 13);
	printf ("fda = %d\n", fda);

	if (fda < 0)
	{
		printf ("Erro no fcntl %s\n", strerror (errno));
		exit (1);
	}

	read (fda, buf, 80);
	write (1, buf, 80);
	putchar ('\n');

}
