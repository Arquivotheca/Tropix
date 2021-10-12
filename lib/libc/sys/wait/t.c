#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>

main ()
{
	int	status, id;

	if (fork () > 0)
	{
		/*
		 *	Pai
		 */
		id = wait (&status);

		printf
		(	"id = %d, status = %X, sinal = %s, pc = %P, fa = %P\n",
			id, status, sigtostr (status & ~0x80), _fpc, _faddr
		);

		exit (0);
	}
	else
	{
		/*
		 *	Filho
		 */
		printf ("Filho\n");

		id = *(int *)0x11;

		exit (2);
	}
}

