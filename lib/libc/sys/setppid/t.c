#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

main ()
{
	int	r;

	if (fork () > 0)
	{
		sleep (2);
		exit (0);
	}

	printf ("pid = %d, ppid = %d\n", getpid (),  getppid () );

	if ((r = setppid (0)) < 0)
	{
		printf ("Errno no setppid: %d\n", r);
		perror ("t");
		exit (1);
	}

	printf ("pid = %d, ppid = %d\n", getpid (),  getppid () );

	if ((r = setppid (0)) < 0)
	{
		printf ("Errno no setppid: %d\n", r);
		perror ("t");
		exit (1);
	}

	printf ("pid = %d, ppid = %d\n", getpid (),  getppid () );

	if ((r = setppid (5)) < 0)
	{
		printf ("Errno no setppid (5): %d\n", r);
		perror ("t");
		exit (1);
	}
}
