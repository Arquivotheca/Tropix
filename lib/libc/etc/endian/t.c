#include <stdio.h>
#include <stdlib.h>

entry unsigned long	l = 0x12345678;
entry unsigned short	s = 0x1234;

main ()
{

	printf ("%P => %P\n", l, long_endian_cv (l));
	printf ("%P => %P\n", s, short_endian_cv (s));
}
