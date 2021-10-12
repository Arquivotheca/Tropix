#include <stdio.h>

main ()
{
	char		area[64];
	int		n;

	n = snprintf (area, sizeof (area), "O valor é %d\n", 1234);
	printf (area);
	printf ("Valor retornado = %d\n\n", n);

	n = snprintf (area, 12, "O valor é %d\n", 1234);
	printf (area);
	printf ("\n");
	printf ("Valor retornado = %d\n", n);
}
