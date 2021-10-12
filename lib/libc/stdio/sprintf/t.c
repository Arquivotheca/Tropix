#include <stdio.h>

main ()
{
	char		area[64];
	int		n;

	n = sprintf (area, "O valor é %d\n", 1234);
	printf (area);
	printf ("Valor retornado = %d (%d)\n", n, strlen (area));
}
