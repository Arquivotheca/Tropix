#include <stdio.h>

char	entrada[] = "      456         cadeia\000678";

main ()
{
	int		i, n1 = -1, n2 = -1, r;
	char		buf[32];
	static char	eof[32] = "EOF";

	r = sscanf (entrada, " %d%s%n%s%n", &i, buf, &n1, eof, &n2);

	printf
	(	"r = %d, buf = \"%s\", i = %d, eof = \"%s\",  n1 = %d, n2 = %d\n",
		r, buf, i, eof, n1, n2
	);
}
