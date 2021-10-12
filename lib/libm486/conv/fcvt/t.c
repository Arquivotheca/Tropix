#include <stdio.h>
#include <stdlib.h>

main ()
{
	double		n;
	char		buf[128];

	while (gets (buf) == buf)
	{
		n = strtod (buf, 0);

		printf ("Número \"%s\": 0x%08X 0x%08X\n", buf, n);

		printf ("%%.15E:	\"%.15E\"\n", n);
		printf ("%%.15G:	\"%.15G\"\n", n);
		printf ("%%.15F:	\"%.15F\"\n", n);
		printf ("%%.15e:	\"%.15e\"\n", n);
		printf ("%%.15g:	\"%.15g\"\n", n);
		printf ("%%.15f:	\"%.15f\"\n", n);
		printf ("%%e:	\"%e\"\n", n);
		printf ("%%g:	\"%g\"\n", n);
		printf ("%%f:	\"%f\"\n", n);
	}
}
