#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
main (int argc, const char *argv[])
{
	int	inf, sup, i;

	if (argc != 4 || !streq (argv[2], "to"))
		exit (1);

	inf = atol (argv[1]);
	sup = atol (argv[3]);

	for (i = inf; i <= sup; i++)
		printf ("%d ", i);

	putchar ('\n');

	return (0);
}
