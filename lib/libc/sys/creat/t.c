#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

main (int argc, register const char *argv[])
{
	register fd, ret;

	fd = creat (argv[1], strtol (argv[2], (const char **)0, 0));

	printf ("fd = %d\n", fd);

	if (fd < 0)
		printf ("Erro no creat: %s\n", strerror (errno));

	write (fd, "TEXTO\n", 6);

	ret = close (fd);
	printf ("fd = %d, ret = %d\n", fd, ret);

}
