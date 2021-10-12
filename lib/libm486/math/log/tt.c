#include <stdio.h>
#include <math.h>

#if (0)	/*******************************************************/
double	log (), log2 (), log10 (), logb ();
#endif	/*******************************************************/

main ()
{
	double		a;
	double	 	b, c, d;
	double	 	e, f, g;

	for (;;)
	{
		a = 4.0;
		c = log2 (a);

		if (c != 2)
		{
			printf ("a =             %g\n", a);
			printf ("log2 (exp) =    %g\n", c);
			exit (1);
		}
	}
}
