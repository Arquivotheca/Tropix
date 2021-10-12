#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>

/*
 *	Pai esquece do filho
 */
main ()
{
	register long	pid;
	register int	n;

	if (pid = fork ())
	{
		/*
		 *	Pai	
		 */
		n = sigchild (pid);
		printf ("sigchild (%d) retornou = %d\n", pid, n);
		printf ("Pai vai simplesmente esquecer do filho\n");
		exit (0);
	}
	else
	{
		/*
		 *	filho	
		 */
		printf ("Filho começando\n");
		sleep (5);
		printf ("Filho terminando\n");
		exit (0);
	}
}
