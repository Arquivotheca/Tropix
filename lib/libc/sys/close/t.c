#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

main ()
{
	register fd, ret;

	fd = open ("t.c", 0);

	ret = close (fd);
	printf ("fd = %d, ret = %d\n", fd, ret);

	ret = close (fd);
	printf ("fd = %d, ret = %d\n", fd, ret);
}
