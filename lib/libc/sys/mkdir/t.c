#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>


#define	NOSTR	(char *)NULL


main (int argc, register char *argv[])
{
	if (argv[1] == NOSTR)
		exit (0);

	if (mkdir (argv[1], 0xFFFFFFFF) < 0)
		printf ("Não consegui criar \"%s\" (%s)\n", argv[1], strerror (errno));
	else
		printf ("Criei \"%s\"\n", argv[1]);
}
