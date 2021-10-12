#include <stdio.h>
#include <stdlib.h>
#include <string.h>

main (int argc, register char *argv[])
{
	char	 	s[256], t[256];
	register char	*ps;
	int		c = ':';
	int		size;

	if (argv[1] != NOSTR)
		size = strtol (argv[1], 0, 0);
	else
		size = 5;

	printf ("src = %P, dst = %P\n", s, t);
	printf ("size = %d\n", size);

	for (;;)
	{
		if (gets (s) != s)
			return (0);

		memset (t, 0, sizeof t);

		ps = memccpy (t, s, c, size);

		if (ps)
		{
			printf ("memccpy = %P: \"%s\"\n", ps, ps);

			printf ("dst = \"%s\"\n", t);
			memmove (ps, "<cat>", 5);
			printf ("catdst = \"%s\"\n", t);
		}
		else
		{
			printf ("memccpy = %P\n", ps);

			printf ("dst = \"%s\"\n", t);
		}


	}
}
