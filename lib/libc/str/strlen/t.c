#include <stdio.h>
#include <string.h>

char	buf[125];

main ()
{
	for (;;)
	{
		if (gets (buf) != buf)
			return (0);
		printf ("%s: %d\n", buf, strlen (buf) );
	}
}
