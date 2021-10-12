#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

main (int argc, register const char *argv[])
{
	if (argv[1] == NOSTR)
		exit (0);

	if (rmdir (argv[1]) < 0)
		printf ("Não consegui remover \"%s\" (%s)\n", argv[1], strerror (errno));
	else
		printf ("Removi \"%s\"\n", argv[1]);
}
