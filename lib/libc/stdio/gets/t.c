#include <stdio.h>

main ()
{
	char	buf[80];

	while (gets (buf) != (char *)NULL)
		puts (buf);
}
