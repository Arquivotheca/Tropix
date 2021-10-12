#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

main ()
{
	register int	fd;
	STAT		s;
	char		area[80];

	if (stat ("../t.c", &s) < 0)
	{
		fprintf
		(	stderr,
			"Não consegui obter o estado de \"%s\" (%s)\n",
			"../t.c", strerror (errno)
		);
		exit (1);
	}

	if ((fd = inopen (s.st_dev, s.st_ino)) < 0)
	{
		fprintf
		(	stderr,
			"Não consegui abrir \"%s\" (%s)\n",
			"../t.c", strerror (errno)
		);
		exit (1);
	}

	read (fd, area, 80);
	write (1, area, 80);
}
