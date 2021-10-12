#include <stdio.h>
#include <string.h>

main (int argc, register const char *argv[])
{
	char 	s[256];
	char	*ps;
	char	c = ':';

	if (argv[1] != 0)
		c = '\0';

	printf ("s = %P, c = '%c' == 0x%02X\n", s, c, c);

	for (;;)
	{
		if (gets (s) != s)
			exit (0);

		printf
		(	"s = \"%s\", strchr = \"%P\", strrchr = \"%P\"\n",
			s,
			strchr  (s, c),
			strrchr (s, c)
		);

		printf
		(	"s = \"%s\", strchr = \"%s\", strrchr = \"%s\"\n",
			s,
			strchr  (s, c),
			strrchr (s, c)
		);
	}
}
