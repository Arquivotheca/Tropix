#include <math.h>
#include <stdio.h>

main ()
{
	double		a;
	double		b, c, d, e;
	extern	int	errno;

	for (;;)
	{
		if (scanf ("%Lg", &a) < 0)
			exit (0);

		printf ("a = %25.20G     \n", a);

		errno = 0;
		d = exp (a);
		b = expb (5.0, a);
/*
*/
		c = exp10 (a);
		e = exp2 (a);
		if (errno)
			perror ("t");

		printf ("a = %25.20G     ", a);
		printf ("exp5 = %25.20G\n", b);
		printf ("expe = %25.20G     ", d);
		printf ("exp10 = %25.20G\n", c);
		printf ("exp2 = %25.20G\n", e);
	}
}
