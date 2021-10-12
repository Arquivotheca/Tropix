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
		if (gets (cbuf) != cbuf)
			return (0);

		printf ("ASCII:     \"%s\" :: ", buf );
		printf ("\"%s\" => %d\n", cbuf, strcmp (buf, cbuf) );

		printf ("ISO/ASCII: \"%s\" :: ", buf );
		printf ("\"%s\" => %d\n", cbuf, strtcmp (buf, cbuf, cmpisotb) );

		printf ("ISO/FOLD:  \"%s\" :: ", buf );
		printf ("\"%s\" => %d\n", cbuf, strttcmp (buf, cbuf, cmpisotb, cmpfoldtb) );

		printf ("ISO:       \"%s\" :: ", buf );
		printf ("\"%s\" => %d\n", cbuf, strttcmp (buf, cbuf, cmpisotb, 0) );
	}
}
