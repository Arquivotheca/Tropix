#include <math.h>
#include <stdio.h>

main ()
{
	double		a;
	double		b, c;

	for (;;)
	{
		if (scanf ("%Lg", &a) < 0)
			exit (0);

		b = sqrt (a);
		c = b * b;
		printf ("a = %25.20G\n", a);
		printf ("sqrt = %25.20G     ", b);
		printf ("sqrt * sqrt = %25.20G\n", c);
	}
}
