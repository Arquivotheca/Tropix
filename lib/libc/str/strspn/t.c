#include <stdio.h>
#include <string.h>

char	lista[] = ",.;:";

main ()
{
	char 	s[256];
	char	*ps;
	int	n;

	for (;;)
	{
		if (gets (s) != s)
			exit (0);

		printf
		(	"strspn = %d, strcspn = %d\n",
			strspn (s, lista),
			strcspn (s, lista)
		);
	}
}
