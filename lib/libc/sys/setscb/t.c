#include <sys/common.h>
#include <sys/scb.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

main (int argc, const char *argv[])
{
	register const SCB	*sp;
	SCB			scb;

	if ((int)(sp = getscb (&scb)) == -1)
	{
		fprintf
		(	stderr,
			"Não consegui obter o SCB (%s)\n",
			strerror (errno)
		);
		exit (1);
	}

	printf ("GETSCB retornou %P\n", sp);
	printf ("TZMIN = %d\n", scb.y_tzmin);

	if (argc <= 1)
		exit (0);

	scb.y_tzmin = atol (argv[1]);

	if (setscb (&scb) < 0)
	{
		fprintf
		(	stderr,
			"Não consegui alterar o SCB (%s)\n",
			strerror (errno)
		);
		exit (1);
	}

	printf ("Novo tzmin = %d\n", gettzmin ());
	exit (0);
}
