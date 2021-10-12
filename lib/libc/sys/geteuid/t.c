#include <sys/syscall.h>

#include <stdio.h>

main ()
{
	setuid (35);

	printf ("geteuid = %d\n", geteuid () );
}
