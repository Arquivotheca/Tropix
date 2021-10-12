#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>

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
		printf ("Pai aguardando o sinal do filho\n");
		pause ();
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
