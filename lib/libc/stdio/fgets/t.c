#include <stdio.h>

main ()
{
	char	buf[80];

	while (fgets (buf, 80, stdin) != (char *)NULL)
		fputs (buf, stdout);
}
