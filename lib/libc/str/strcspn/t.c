#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char	lista[] = ",.;:";

main ()
{
	char 	s[256];

	for (;;)
	{
		if (gets (s) != s)
			exit (0);

		printf
		(	"strcspn = %d\n",
			strcspn (s, lista)
		);
#if (0)	/*******************************************************/
		printf
		(	"strspn = %d, strcspn = %d\n",
			strspn (s, lista),
			strcspn (s, lista)
		);
#endif	/*******************************************************/
	}
}
