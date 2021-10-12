#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

main (int argc, register const char *argv[])
{
	register	n, ret;
	char		s[30];

	while (gets (s) == s)
	{
		n = atoi (s);

		ret = nice (n);

		printf ("n = %d, ret = %d\n", n, ret);
	}

} 
