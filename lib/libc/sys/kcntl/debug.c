#include <sys/kcntl.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

main (int argc, const char *argv[])
{
	int		flag, r;

	if (argv[1] == NOSTR)
		flag = -1;
	else
		flag = atoi (argv[1]);

	if ((r = kcntl (K_DEBUG, flag)) < 0)
		{ printf ("Erro: %s\n", strerror (errno )); return (-1); }

	printf ("%d => %d\n", flag, r);

}
