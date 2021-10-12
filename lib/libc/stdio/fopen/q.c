#include <stdio.h>
#include <errno.h>

main (argc, argv)
char	**argv;
{
	FILE		*fp;
	int		i;

	for (i = 0; i < 100; i++)
	{
		if ((fp = fopen ("/", "r")) == NOFILE)
			break;

		printf (" %d", fileno (fp));
	}

	printf ("\nTotal = %d\n", i);

}	/* end t */
