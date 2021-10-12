#include <sys/syscall.h>

#include <unistd.h>
#include <stdio.h>
#include <errno.h>

main (int argc, register const char *argv[])
{
	register const char	*f;

	setuid (1);

	f = argv[1];

	if (access (f, F_OK) <  0)
		printf ("F erro: %s\n", strerror (errno) );
	else
		printf ("F ok\n");

	if (access (f, R_OK) <  0)
		printf ("R erro: %s\n", strerror (errno) );
	else
		printf ("R ok\n");

	if (access (f, W_OK) <  0)
		printf ("W erro: %s\n", strerror (errno) );
	else
		printf ("W ok\n");

	if (access (f, X_OK) <  0)
		printf ("X erro: %s\n", strerror (errno) );
	else
		printf ("X ok\n");

}
