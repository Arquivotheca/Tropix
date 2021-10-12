#include <stdio.h>
#include <string.h>

main ()
{
	char 	s[256];
	char	*ps;

	for (;;)
	{
		if (gets (s) != s)
			exit (0);

		ps = strfield (s, ':');

		printf ("\"%s\"\n", ps);

		while (ps = strfield (0, ':'))
		{
			printf ("\"%s\"\n", ps);
		}
	}
}
