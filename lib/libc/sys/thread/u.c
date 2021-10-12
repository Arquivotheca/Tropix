#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define	NTHREADS	50

int	th[3];

main ()
{
	int	i, thid;

	for (i = 0; i < NTHREADS; i++)
	{
		if ((th[i] = thread ()) == 0)
		{
			_exit (0);
		}

		if (th[i] < 0)
			printf ("Não consegui criar thread %d\n", i);

/*
		if (th[i] > 0)
			printf ("Consegui criar thread no. %d\n", th[i]);
*/
	}

/*
	printf ("\n");
*/
	for (i = 0; i < NTHREADS; i++)
	{
		thid = wait (0);
/*
		printf ("Terminou thread no. %d\n", thid);
*/

	}
}
