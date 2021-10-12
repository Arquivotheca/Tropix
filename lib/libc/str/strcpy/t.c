#include <stdio.h>
#include <string.h>

char	buf[125];
char	cbuf[125];

main ()
{
	for (;;)
	{
		if (gets (buf) != buf)
			return (0);
		printf ("\"%s\" :: \"%s\"\n", buf, strcpy (cbuf, buf) );
	}
}
