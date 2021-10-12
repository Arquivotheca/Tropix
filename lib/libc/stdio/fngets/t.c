#include <stdio.h>

main ()
{
	char	buf[80];

	while (fngets (buf, 80, stdin) != (char *)NULL)
		fnputs (buf, stdout);
}
