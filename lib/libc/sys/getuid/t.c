#include <sys/syscall.h>

#include <stdio.h>

main ()
{
	setuid (35);

	printf ("getuid = %d\n", getuid () );
}
