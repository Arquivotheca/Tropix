#include <stdio.h>
#include <string.h>

char	buf[125];

main ()
{
	register char	*cp;

	for (;;)
	{
		if (gets (buf) != buf)
			return (0);

		printf ("\"%s\" :: ", buf );

		cp = strend (buf);

		cp[0] = '*';	cp[1] = '\0';

		printf (" => \"%s\"\n", buf);
	}
}
