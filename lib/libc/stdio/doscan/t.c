#include <stdio.h>
#include <stdlib.h>

main ()
{
	int		i, j, k, r, n;
	double		d;
	char		area[32];
	char		c;
	char		line[128];

	for (;;)
	{
#if (0)	/*******************************************************/
		if ((r = scanf ("%p", &i)) < 0)
		if ((r = scanf ("%i", &i)) < 0)
		if ((r = scanf ("%lf", &d)) < 0)
		if ((r = scanf ("%s", area)) < 0)
		if ((r = scanf ("%c", &c)) < 0)
		if ((r = scanf (" %[<a-c#x-z>] %i", area, &i)) < 0)
		if ((r = scanf ("%[^<a-c#x-z>0-9]%i", area, &i)) < 0)
		if ((r = scanf ("%[-]a-c] %i ", area, &i)) < 0)
		if ((r = scanf (" %[a-c]%i", area, &i)) < 0)
		if ((r = scanf (" %[]a-c---]%i", area, &i)) < 0)
		if ((r = scanf ("%%%i%n", &i, &n)) < 0)
		if ((r = scanf ("%d%n", &i, &n)) < 0)
		if ((r = scanf ("%i%n", &i, &n)) < 0)
#endif	/*******************************************************/
		if (gets (line) != line)
			exit (0);
#if (0)	/*******************************************************/
		if ((r = sscanf (line, "%i%n%i%n%i%n", &i, &n, &j, &n, &k, &n)) < 0)
#endif	/*******************************************************/
		if ((r = sscanf (line, "%d%d%d%n", &i, &j, &k, &n)) < 0)
			{ printf ("EOF\n"); exit (0); }
		
		if (r == 0)
			{ printf ("Matching failure\n"); exit (0); }
		
		printf ("Retornou %d, nscan = %d, (%d, %d, %d)\n", r, n, i, j, k);
#if (0)	/*******************************************************/
		printf ("Retornou %d; area = \"%s\", i = %d\n", r, area, i);
		printf ("Retornou %d; c = \"%c\"\n", r, c);
		printf ("Retornou %d; d = %E\n", r, d);
		printf ("Retornou %d; i = %d, 0x%X, 0%o\n", r, i, i, i);
#endif	/*******************************************************/
	}
}
