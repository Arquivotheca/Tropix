#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

main (int argc, register char *argv[])
{
	int		dev, ino;
	STAT		s, ins;

	if (argc == 3)
	{
		dev = atol (argv[1]);	ino = atol (argv[2]);

		if (instat (dev, ino, &ins) < 0)
		{
			printf ("Não consegui dar \"instat\" (%s)\n", strerror (errno));
			exit (1);
		}

		printf ("dev = %d, ino = %d, size = %d\n", ins.st_dev, ins.st_ino, ins.st_size);

		exit (0);
	}

	if (stat (".", &s) < 0)
	{
		printf ("Não consegui dar \"stat\" (%s)\n", strerror (errno));
		exit (1);
	}

	printf ("nm = \"%s\", ", argv[1]);

	if (instat (s.st_dev, s.st_ino, &ins) < 0)
	{
		printf ("Não consegui dar \"instat\" (%s)\n", strerror (errno));
		exit (1);
	}

	printf ("dev = %d, ino = %d, ", ins.st_dev, ins.st_ino);
	printf ("size = %d\n", ins.st_size);

	exit (0);
}
