#include <sys/syscall.h>
#include <stdio.h>
#include <stdlib.h>

main (int argc, register const char *argv[])
{
	register	n, ret;
	char		s[30];

	printf ("end = %P, _cbreak = %P\n", &end, _cbreak);

	while (gets (s) == s)
	{
		n = strtol (s, 0, 0);

		ret = brk ((void *)n);

		printf ("n = %P, brk = %d, _cbreak = %P\n", n, ret, _cbreak);
	}

	while (gets (s) == s)
	{
		n = strtol (s, 0, 0);

		ret = (int)sbrk (n);

		printf ("n = %d, sbrk = %P, _cbreak = %P\n", n, ret, _cbreak);
	}

} 
