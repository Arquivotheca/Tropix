#include <stdio.h>
#include <string.h>

char	buf[125];

main ()
{
	for (;;)
	{
		if (gets (buf) != buf)
			return (0);
		printf ("\"%s\" :: ", buf );
		printf ("\"%s\"\n", strcat (buf, "_O_ECHO") );
	}
}
