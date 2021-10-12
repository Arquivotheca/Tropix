#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char	esp[] = ":;.,";

main ()
{
	char 	str[256];
	char	*ps;

	for (;;)
	{
		if (gets (str) != str)
			exit (0);

		ps = strtok (str, esp);

		while (ps != NOSTR)
		{
			printf ("%s\n", ps);
			ps = strtok (0, esp);
		}
	}
}
