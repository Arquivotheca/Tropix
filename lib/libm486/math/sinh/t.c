#include <math.h>
#include <stdio.h>

main ()
{
	double		a, b, c, d;

	for (;;)
	{
		if (scanf ("%Lg", &a) < 0)
			exit (0);

		b = sinh (a);
		c = cosh (a);
		d = tanh (a);

		printf ("a = %25.20G     ", a);
		printf ("tanh = %25.20G\n", d);
		printf ("sinh= %25.20G     ", b);
		printf ("cosh = %25.20G\n", c);
	}
}
