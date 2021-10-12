#include <sys/syscall.h>

#include <stdio.h>

main ()
{
	printf ("pid = %d\n", getpid () );
}
