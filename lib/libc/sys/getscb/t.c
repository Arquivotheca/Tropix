#include <sys/common.h>
#include <sys/scb.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

main ()
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
	printf ("CPU type = %d\n", scb.y_cputype);
}
