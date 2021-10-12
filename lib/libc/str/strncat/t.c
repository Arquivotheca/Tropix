#include <stdio.h>
#include <string.h>

char	buf[125];
char	cbuf[125];

main (int argc, register char *argv[])
{
	int		size = argv[1] ? strtol (argv[1], 0, 0) : 3;

	printf ("size = %d\n", size);

	for (;;)
	{
		memset (buf, 0, sizeof buf);
		if (gets (buf) != buf)
			return (0);
		if (gets (cbuf) != cbuf)
			return (0);
		printf ("\"%s\" :: ", buf );
		printf ("\"%s\"\n", strncat (buf, cbuf, size) );
	}
}
