#include <stdio.h>
#include <string.h>
#include <time.h>

int
main ()
{
	register char	*ocp, *cp;
	register	i;
	time_t		time0, time1;
	char		line[80];

	time (&time0);
	clockstamp ();

	for (i = 0; i < 1000000; i++)
		cp = (char *)0;

	i = clock ();
	printf ("clock = %d (%d s + %d us)\n", i, i / CLK_TCK, i % CLK_TCK);

	time (&time1);
	printf ("difftime = %g\n", difftime (time1, time0));

	ocp = cp = clocktostr (line);

	printf ("ocp = %P\n", ocp);

/*************
	for (i = 0; i < 80; i++, cp++)
	{
		if (*cp)
			putchar (*cp);
		else
			putchar ('.');
	}

	putchar ('\n');
****************/

	printf ("%s\n%d\n", ocp, strlen (ocp) );

	return (0);

}	/* end main */
