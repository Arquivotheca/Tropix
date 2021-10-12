#include <stdio.h>
#include <string.h>

main (int argc, register char *argv[])
{
	char	 	s[256], t[256];
	register char	*ps;
	int		size = argv[1] ? strtol (argv[1], 0, 0) : 3;
	register char	*a3, *a2 = (char *)-13;
	register int	d7, d6, d5, d4, d3, d2 = -54;

	printf ("size = %d\n", size);

	for (;;)
	{
		if (gets (s) != s)
			exit (0);
		if (gets (t) != t)
			exit (0);

		printf ("strntcmp = %d\n", strntcmp (s, t, size, cmpisotb) );

		if (a2 != (char *)-13)
			printf ("***** Não conservou a2!\n"); 

		if (d2 != -54)
			printf ("***** Não conservou d2!\n"); 
	}
}
