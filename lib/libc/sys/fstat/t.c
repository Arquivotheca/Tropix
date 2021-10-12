#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <errno.h>

main ()
{
	register	fd;
	STAT		statb;

	if ((fd = open ("../t.c", 0)) < 0)
		printf ("Erro no open: %s\n", strerror (errno));

	if (fstat (fd, &statb) < 0)
		printf ("Erro no fstat: %s\n", strerror (errno));

	printf ("size = %d\n", statb.st_size);

	if (fstat (fd, (STAT *)0x305555) < 0)
		printf ("Erro no fstat: %s\n", strerror (errno));
}
