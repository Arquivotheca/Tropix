#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

main ()
{
	int	m;

	m = umask (0123);
	printf ("m = %o\n", m); 

	m = umask (0123);
	printf ("m = %o\n", m); 
}
