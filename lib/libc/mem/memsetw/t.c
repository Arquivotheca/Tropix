#include <stdlib.h>
#include <string.h>

main (int argc, register char *argv[])
{
	short	 	s[256];
	register int	i;
	short		w;
	int		size;

	if (argv[1] != NOSTR)
		size = strtol (argv[1], 0, 0);
	else
		size = 5;

	if (memsetw (s, 12123, size) != s)
		printf ("Não retornou a área\n");

	for (i = -1 ; i < 12 ; i++)
		printf ("s[%d] = %d\n", i, s[i]);
}
