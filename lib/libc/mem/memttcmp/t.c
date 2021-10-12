#include <stdio.h>
#include <stdlib.h>
#include <string.h>

main (int argc, register char *argv[])
{
	char	 	s[256], t[256];
	register char	*ps;
	int		size;
	extern char	cmpfoldtb[], cmpafoldtb[], cmpisotb[];
	register char	*a3, *a2 = (char *)-13;
	register int	d7, d6, d5, d4, d3, d2 = -54;

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

		printf
		(	"memttcmp (fold) = %d\n",
			memttcmp (s, t, size, cmpfoldtb, cmpafoldtb)
		);

		printf
		(	"memttcmp (iso)  = %d\n",
			memttcmp (s, t, size, cmpisotb, 0)
		);

		if (a2 != (char *)-13)
			printf ("***** Não conservou a2!\n"); 

		if (d2 != -54)
			printf ("***** Não conservou d2!\n"); 
	}
}
