#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

main (int argc, register const char *argv[])
{
	register fd, fda, fdb, fdc;

	fd = open ("../t.c", 0);

	printf ("fd = %d\n", fd);

	if (fd < 0)
		printf ("Erro no open %s\n", strerror (errno));

	fda = dup (fd);
	printf ("fda = %d\n", fda);

	fdb = dup2 (fd, 15);
	printf ("fdb = %d\n", fdb);

	fdc = dup2 (fd, 25);
	printf ("fdc = %d\n", fdc);
	if (fdc < 0)
		printf ("Erro no dup2 %s\n", strerror (errno));
}
