#include <id.h>

main (argc, argv)
register char	**argv;
{
	lid_t		a, b, c, d;
	register int	i;

	lidcan (a, argv[1]);
	lidcan (b, argv[2]);

	i =  lidcmp (a, b);

	printf ("(%d, %d)\n", strlen (argv[1]), strlen (argv[2]));

	if      (i < 0)
		printf ("\"%-31.31s\" <  \"%-31.31s\"\n", a, b);
	else if (i > 0)
		printf ("\"%-31.31s\" >  \"%-31.31s\"\n", a, b);
	else
		printf ("\"%-31.31s\" == \"%-31.31s\"\n", a, b);

}
