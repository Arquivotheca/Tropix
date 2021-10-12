#include <stdio.h>
#include <stdlib.h>
#include <string.h>

main ()
{
	int		n, r;
	char		a[128], b[128], c[128], d[128]; 

	memset (a, '\0', 128); memset (b, '\0', 128);
	memset (c, '\0', 128); memset (d, '\0', 128);
	r = scanf ("%s%s%s%s%n", a, b, c, d, &n);
	printf ("r = %d, nscan = %d, (\"%s\", \"%s\", \"%s\", \"%s\")\n", r, n, a, b, c, d);

	memset (a, '\0', 128); memset (b, '\0', 128);
	memset (c, '\0', 128); memset (d, '\0', 128);
	r = scanf ("%5s%4s%5s%5s%n", a, b, c, d, &n);
	printf ("r = %d, nscan = %d, (\"%s\", \"%s\", \"%s\", \"%s\")\n", r, n, a, b, c, d);

	memset (a, '\0', 128); memset (b, '\0', 128);
	memset (c, '\0', 128); memset (d, '\0', 128);
	r = scanf ("%5s%*4s%5s%5s%n", a, c, d, &n);
	printf ("r = %d, nscan = %d, (\"%s\", \"%s\", \"%s\", \"%s\")\n", r, n, a, b, c, d);

	scanf ("%*[^\n]"); scanf (" ");
	memset (a, '\0', 128); memset (b, '\0', 128);
	memset (c, '\0', 128); memset (d, '\0', 128);
	r = scanf ("%c%c%c%c%n", a, b, c, d, &n);
	printf ("r = %d, nscan = %d, (\"%c\", \"%c\", \"%c\", \"%c\")\n", r, n, a[0], b[0], c[0], d[0]);

	scanf ("%*[^\n]"); scanf ("\n");
	memset (a, '\0', 128); memset (b, '\0', 128);
	memset (c, '\0', 128); memset (d, '\0', 128);
	r = scanf ("%2c%2c%2c%2c%n", a, b, c, d, &n);
	printf ("r = %d, nscan = %d, (\"%s\", \"%s\", \"%s\", \"%s\")\n", r, n, a, b, c, d);

	scanf ("%*[^\n]"); scanf (" ");
	memset (a, '\0', 128); memset (b, '\0', 128);
	memset (c, '\0', 128); memset (d, '\0', 128);

	r = scanf (" %[<a-c#x-z>]%[0-9]%[^0-9]%[]-1]%n", a, b, c, d, &n);
	printf ("r = %d, nscan = %d, (\"%s\", \"%s\", \"%s\", \"%s\")\n", r, n, a, b, c, d);
	printf ("Caractere seguinte = \"%c\"\n", getchar ());
}
