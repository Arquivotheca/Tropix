#include <sys/syscall.h>

#include <stdio.h>

main ()
{
	register pid;

	if ((pid = fork ()) == 0)
	{
		/*
		 *	Filho
		 */
		printf ("Filho: %d\n", getpid ());
		exit (0);
	}
	else
	{
		/*
		 *	Pai
		 */
		printf ("Pai: %d, retornou %d\n", getpid (), pid);
		exit (0);
	}
}
