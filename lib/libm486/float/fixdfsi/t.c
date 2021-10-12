#include <stdio.h>
#include <stdlib.h>

main ()
{
	int		i;
	double		d;
	char		area[64];

	for (;;)
	{
		if (gets (area) != area)
			exit (0);
		d = strtod (area, 0);

		printf ("d = %e, fixsdfsi = %d\n", d, __fixdfsi (d));
	}
}
