
#include <stdlib.h>

#define	NOSTRP	(const char **)0

main ()
{
	register long	l;

	l = strtol ("3000000000", NOSTRP, 0);
	printf ("%d, %u\n", l, l);

	l = strtol ("+21", NOSTRP, 0);
	printf ("%d, %u\n", l, l);

	l = strtol ("+2147483647", NOSTRP, 0);
	printf ("%d, %u\n", l, l);

	l = strtol ("+2147483648", NOSTRP, 0);
	printf ("%d, %u\n", l, l);

	l = strtol ("-2147483648", NOSTRP, 0);
	printf ("%d, %u\n", l, l);

	l = strtol ("-2147483649", NOSTRP, 0);
	printf ("%d, %u\n", l, l);

	l = strtol ("4294967295", NOSTRP, 0);
	printf ("%d, %u\n", l, l);

	l = strtol ("4294967296", NOSTRP, 0);
	printf ("%d, %u\n", l, l);

	printf ("--------------\n");

	l = strtoul ("3000000000", NOSTRP, 0);
	printf ("%d, %u\n", l, l);

	l = strtoul ("+21", NOSTRP, 0);
	printf ("%d, %u\n", l, l);

	l = strtoul ("+2147483647", NOSTRP, 0);
	printf ("%d, %u\n", l, l);

	l = strtoul ("+2147483648", NOSTRP, 0);
	printf ("%d, %u\n", l, l);

	l = strtoul ("-2147483648", NOSTRP, 0);
	printf ("%d, %u\n", l, l);

	l = strtoul ("-2147483649", NOSTRP, 0);
	printf ("%d, %u\n", l, l);

	l = strtoul ("4294967295", NOSTRP, 0);
	printf ("%d, %u\n", l, l);

	l = strtoul ("4294967296", NOSTRP, 0);
	printf ("%d, %u\n", l, l);

}
