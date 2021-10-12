#include <sys/syscall.h>

#include <stdio.h>
#include <errno.h>

main ()
{
	int		sn;

	if ((sn = getsn ()) == -1)
	{
		fprintf
		(	stderr,
			"Não consegui obter o número de série (%s)\n",
			strerror (errno)
		);
		exit (1);
	}

	printf ("Número de série = %d\n", sn);
}
