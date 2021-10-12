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
		if (gets (t) != t)
			exit (0);

		printf ("memeq = %d\n", memeq (s, t, size) );
	}
}
