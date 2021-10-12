#include <stdio.h>
#include <stdlib.h>

main ()
{
	int	i, j, k;

	for (;;)
	{
		if (fscanf (stdin, "%d%d%d", &i, &j, &k) < 0)
			exit (0);

		printf ("i = %d, j = %d, k = %d\n", i, j, k);
	}
}
