#include <stdio.h>
#include <stdlib.h>
#include <string.h>

main (int argc, register char *argv[])
{
	char	 	s[256], t[256];
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

		ps = memcpy (t, s, size);

		printf ("memcpy (%P, %P) = \"%s\"\n", t, ps, ps);
	}
}
