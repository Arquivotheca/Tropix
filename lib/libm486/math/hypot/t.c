#include <math.h>
#include <stdio.h>

main ()
{
	double		a, b, c;

	printf ("Dê os dois catetos\n");

	for (;;)
	{
		if (scanf ("%Lg %Lg", &b, &c) < 0)
			exit (0);

		a = hypot (b, c);

		printf ("hip = %25.20G\n", a);
		printf ("catb = %25.20G     ", b);
		printf ("catc = %25.20G\n", c);
	}
}
