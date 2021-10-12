#include <stdio.h>
#include <stdlib.h>
#include <string.h>

main (int argc, register char *argv[])
{
	char	 	s[256];
	register char	*ps;
	int		c = ':';
	int		size;

	if (argv[1] != NOSTR)
		size = strtol (argv[1], 0, 0);
	else
		size = 5;

	printf ("size = %d\n", size);

	for (;;)
	{
		if (gets (s) != s)
			return (0);

		ps = memchr (s, c, size);

		if (ps)
		{
			printf ("memchr = \"%s\"\n", ps);
		}
		else
		{
			printf ("memchr = Não achou\n");
		}
	}
}
