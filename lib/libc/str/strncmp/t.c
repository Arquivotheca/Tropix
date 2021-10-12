#include <stdio.h>
#include <string.h>

main (int argc, register char *argv[])
{
	char	 	s[256], t[256];
	int		size = argv[1] ? strtol (argv[1], 0, 0) : 3;

	printf ("size = %d\n", size);

	for (;;)
	{
		if (gets (s) != s)
			return (0);
		if (gets (t) != t)
			return (0);

		printf ("\"%s\" :: ", s );
		printf ("\"%s\" => %d\n", t, strncmp (s, t, size) );
	}
}
