#include <stdio.h>
#include <string.h>

char	lista[] = ",.;:";

main ()
{
	char 	s[256];
	char	*ps;

	for (;;)
	{
		if (gets (s) != s)
			exit (0);

		ps = strpbrk (s, lista);

		if (ps)
			puts (ps);
		else
			puts ("NULO");

	}
}
