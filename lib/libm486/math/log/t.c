/*
#include <math.h>
*/
#include <stdio.h>

double	log (), log2 (), log10 (), logb ();

main ()
{
	double		a;
	double	 	b, c, d, x;
	double	 	e, f, g, y;

	for (;;)
	{
		if (scanf ("%lg", &a) < 0)
			exit (0);
/*

		e = expb (5.0, a);
		f = exp (a);
		g = exp10 (a);
		y = exp2 (a);
*/

e = a;
f = a;
g = a;
y = a;

		b = logb (5.0, e);
		c = log (f);
		d = log10 (g);
		x = log2 (y);

		printf ("a =             %25.20G\n", a);
		printf ("expb =          %25.20G\n", e);
		printf ("exp =           %25.20G\n", f);
		printf ("exp10 =         %25.20G\n", g);
		printf ("exp2 =         %25.20G\n", y);

		printf ("logb (expb) =   %25.20G\n", b);
		printf ("loge (exp) =    %25.20G\n", c);
		printf ("log10 (exp10) = %25.20G\n", d);
		printf ("log2 (exp10) = %25.20G\n", x);
	}
}
