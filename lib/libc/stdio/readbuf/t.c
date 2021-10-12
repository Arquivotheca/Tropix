#include <stdio.h>

main ()
{
	register int	c;

	while ((c = getchar ()) >= 0)
		putchar (c);
}
