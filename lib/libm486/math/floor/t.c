#include <math.h>
#include <stdio.h>
#include <stdlib.h>

main ()
{
	double		a, b, c;

	printf ("Dê os valores de a e b\n");

	for (;;)
	{
		scanf ("%Lg %Lg", &a, &b);

		printf ("a = %25.20G\t", a);
		printf ("b = %25.20G\n", b);

		c = floor (a);
		printf ("floor (a) = %25.20G\n", c);

		c = ceil (a);
		printf ("ceil (a) = %25.20G\n", c);

		c = fabs (a);
		printf ("abs (a) = %25.20G\n", c);

		c = fmod (a, b);
		printf ("fmod (a, b) = %25.20G\n", c);

	}
}
