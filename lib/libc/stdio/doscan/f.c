#include <stdio.h>
#include <stdlib.h>

main ()
{
	int		n, r;
	double		d, e, f, g;
	float		l;

	r = scanf ("%le%lf%lg%le%n", &d, &e, &f, &g, &n);
	printf ("r = %d, nscan = %d, (%g, %g, %g, %g)\n", r, n, d, e, f, g);

	r = scanf ("%6e%10lf%11lg%11le%n", &l, &e, &f, &g, &n);
	printf ("r = %d, nscan = %d, (%g, %g, %g, %g)\n", r, n, l, e, f, g);
}
