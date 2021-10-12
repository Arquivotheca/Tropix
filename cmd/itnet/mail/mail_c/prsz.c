#include <stdio.h>
#include <time.h>

#include "h/mail.h"

int
main ()
{
	printf ("sizeof (MDIREC) = %d\n", sizeof (MDIREC));
	printf ("sizeof (PART) = %d\n", sizeof (PART));
	printf ("sizeof (LINE) = %d\n", sizeof (LINE));

	return (0);
}
