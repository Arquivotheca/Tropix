#include <stdio.h>
#include <stdlib.h>

main ()
{
	long		a, b, c, d;
	div_t		qer;
	char		area[16];

	for (;;)
	{
		printf ("Numerador: ");
		if (gets (area) != area)
			exit (0);
		a = atoi (area);

		printf ("Denominador: ");
		if (gets (area) != area)
			exit (0);
		b = atoi (area);

#if (0)	/*******************************************************/
		if (scanf("%d %d", &a, &b) <= 0)
			exit (0);
#endif	/*******************************************************/

		c = a / b;
		d = a % b;
	
		qer = div (a, b);
	
		printf ("rot.quot = %d, rot.rem = %d\n", c, d);
		printf ("div.quot = %d, div.rem = %d\n", qer.quot, qer.rem);
		printf ("Produto  = %d\n\n", b * qer.quot + qer.rem);
	}
}
