#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>

main ()
{
	register fd;
	char	area[10];

	fd = open ("../t.c", O_RDONLY);

	read (fd, area, 10);
	write (1, area, 10);

	if (read (456, area, 10) < 0)
		printf ("Erro no read: %s\n", strerror (errno) ); 
}
