#include <sys/syscall.h>

#include <stdio.h>

main ()
{
	if (link ("../t.c", "/etc/t.c") < 0)
		perror ("t");
	else
		printf ("OK 1\n");

	if (link ("../t.c", "n.c") < 0)
		perror ("t");
	else
		printf ("OK 2\n");

	if (unlink ("n.c") < 0)
		perror ("t");
	else
		printf ("OK 3\n");
}
