#include <sys/syscall.h>

#include <stdio.h>
#include <errno.h>

int
main (int argc, register const char *argv[])
{
	const char	*pgname = argv[0];

	printf ("TZMIN = %d\n", gettzmin ());
}
