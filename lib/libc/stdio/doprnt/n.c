#include <stdio.h>
#include <stdlib.h>

main ()
{
	int		n, i, j, k;
	short		w;

	printf ("===>%r<===\n");

	n = printf ("%E %E %d %i\n", 1.2, 2.3, 4, 5);
	printf ("Retorna %d\n", n);

	n = printf ("%G %G %d %i\n", 1.2, 2.3, 4, 5);
	printf ("Retorna %d\n", n);

	n = printf ("%E%n %E%n %d%n %i%hn\n", 1.2, &i, 2.3, &j, 4, &k, 5, &w);
	printf ("Valores de %%n: %d, %d, %d, %d\n", i, j, k, w);
	printf ("Retorna %d\n", n);

	printf ("%15.3d\n", 1234);
	printf ("%*.3d\n", 15, 1234);
	printf ("%15.*d\n", 3, 1234);
	printf ("%*.*d\n", 15, 3, 1234);
}
