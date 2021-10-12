#include <stdlib.h>

main ()
{
	printf ("%d, %d\n", -123, abs (-123));
	printf ("%d, %d\n", 0, abs (0));
	printf ("%d, %d\n", 123, abs (123));

	printf ("%d, %d\n", -123, labs (-123));
	printf ("%d, %d\n", 0, labs (0));
	printf ("%d, %d\n", 123, labs (123));
}
