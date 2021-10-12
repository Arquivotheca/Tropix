#include <stdio.h>
#include <stdlib.h>

main ()
{
	int		i;
	double		d;
	char		area[64];
	extern double	ldexp (double, int);

	for (;;)
	{
		if (gets (area) != area)
			exit (0);
		d = strtod (area, 0);

		printf ("d = %e, fixunssdfsi = %u\n", d, __fixunsdfsi (d));
	}
}
