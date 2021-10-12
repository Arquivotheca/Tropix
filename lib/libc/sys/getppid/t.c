#include <sys/syscall.h>

#include <stdio.h>

main ()
{
	printf ("ppid = %d\n", getppid () );
}
