#include <sys/types.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stat.h>

main (int argc, register char *argv[])
{
	int		dev, r;
	STAT		ups;

	if (argc == 2)
	{
		dev = atol (argv[1]);

		if ((r = upstat (dev, &ups)) < 0)
		{
			printf ("Não consegui dar \"upstat\" (%s)\n", strerror (errno));
			exit (1);
		}

		printf ("dev = %d, ino = %d, r = %d\n", ups.st_dev, ups.st_ino, r);

		exit (0);
	}

	exit (0);
}
