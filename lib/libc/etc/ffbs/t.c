#include <stdio.h>
#include <stdlib.h>

int
main ()
{
	const char	*str;
	int		n;
	char		area[64];

	while (gets (area) == area)
	{
		n = strtol (area, &str, 0);

		printf ("n = %d (%P), ffbs (n) = %d\n", n, n, ffbs (n));
	}

	printf ("EOF\n");

	return (0);
}
