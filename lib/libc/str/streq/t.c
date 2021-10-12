#include <stdio.h>
#include <string.h>

main ()
{
	char s[256], t[256];

	for (;;)
	{
		if (gets (s) != s)
			return (0);
		if (gets (t) != t)
			return (0);

		printf ("streq = %d\n", streq (s, t));
	}
}
