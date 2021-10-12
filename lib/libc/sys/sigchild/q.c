#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

void	inter (int, void *, void *);

main ()
{
	register long	pid;
	register int	n;

	if (pid = fork ())
	{
		/*
		 *	Pai	
		 */
		signal (SIGCHLD, inter);
		n = sigchild (pid);
		printf ("sigchild (%d) retornou = %d\n", pid, n);
		printf ("Pai aguardando o sinal do filho\n");
		pause ();
		printf ("Voltando de pause\n");
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
		exit (5);
	}
}

void
inter (int signo, void *pc, void *fa)
{
	int	status, pid;

	printf ("Início de inter\n");

	pid = wait (&status);

	printf ("wait: pid = %d, status = %X\n", pid, status);
}
