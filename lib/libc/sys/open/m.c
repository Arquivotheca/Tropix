#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>

main ()
{
	int		fd, i;

	for (i = 0; i < 100; i++)
	{
		if ((fd = open ("/", O_RDONLY)) < 0)
			break;
 
		printf (" %d", fd);
	}

	printf ("\nTotal = %d\n", i);
}
