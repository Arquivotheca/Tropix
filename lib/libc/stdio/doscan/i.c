#include <stdio.h>
#include <stdlib.h>

main ()
{
	int		n, r, a, b, c, d;
	short		s;

	r = scanf ("%d%d%d%d%n", &a, &b, &c, &d, &n);
	printf ("r = %d, nscan = %d, (%d, %d, %d, %d)\n", r, n, a, b, c, d);

	r = scanf ("%2d%3d%4d%n", &a, &b, &c, &n);
	printf ("r = %d, nscan = %d, (%d, %d, %d)\n", r, n, a, b, c);
	scanf ("%*[^\n]");

	r = scanf ("%2d%3d%4d%n", &a, &b, &c, &n);
	printf ("r = %d, nscan = %d, (%d, %d, %d)\n", r, n, a, b, c);
	scanf ("%*[^\n]");

	r = scanf ("%i%i%i%i%n", &a, &b, &c, &d, &n);
	printf ("r = %d, nscan = %d, (%d, %d, %d, %d)\n", r, n, a, b, c, d);

	r = scanf ("%i%i%i%i%n", &a, &b, &c, &d, &n);
	printf ("r = %d, nscan = %d, (%d, %d, %d, %d)\n", r, n, a, b, c, d);

	r = scanf ("%i%*i%i%i%i%n", &a, &b, &c, &d, &n);
	printf ("r = %d, nscan = %d, (%d, %d, %d, %d)\n", r, n, a, b, c, d);

	r = scanf ("%p%p%p%p%n", &a, &b, &c, &d, &n);
	printf ("r = %d, nscan = %d, (%P, %P, %P, %P)\n", r, n, a, b, c, d);

	r = scanf ("%hi%n", &s, &n);
	printf ("r = %d, nscan = %d, (%X)\n", r, n, s);
}
