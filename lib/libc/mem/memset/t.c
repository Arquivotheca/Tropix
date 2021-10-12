#include <stdlib.h>
#include <stdio.h>
#include <string.h>

main (int argc, register char *argv[])
{
	char	 	s[256];
	register char	*ps;
	int		size;

	if (argv[1] != NOSTR)
		size = strtol (argv[1], 0, 0);
	else
		size = 5;

	printf ("size = %d\n", size);

	for (;;)
	{
		if (gets (s) != s)
			exit (0);

		ps = memset (s, '+', size);
		printf ("memset = \"%s\"\n", ps);
	}
}
