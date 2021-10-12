#include <sys/syscall.h>

#include <stdio.h>

main ()
{
	setgid (35);

	printf ("getgid = %d\n", getgid () );
}
