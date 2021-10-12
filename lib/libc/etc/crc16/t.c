#include <stdio.h>
#include <stdlib.h>
#include <string.h>

main ()
{
	char	buf[256];
	int	k;

	k = 0;

	while (gets (buf) == buf)
	{
		k++;

		printf ("%d:  %04X, %04X\n", k,
			crc16 (buf, strlen (buf)),
			crc16s (buf, strlen (buf)) );

		printf ("%d:  %04X, %04X\n", k,
			crc16 (buf, strlen (buf) + 2),
			crc16s (buf, strlen (buf) + 2) );

		printf ("%d:  %04X, %04X\n", k,
			crc1641 (buf, strlen (buf)),
			crc1641s (buf, strlen (buf)) );

		memset (buf, '\0', sizeof (buf));
	}
}
