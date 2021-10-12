
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

main (argc, argv)
char	**argv;
{
	errno = atoi (argv[1]);

	printf ("%s\n", strerror (errno));
	perror ("t");
}
