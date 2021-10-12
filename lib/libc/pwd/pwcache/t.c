#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <pwd.h>

main ()
{
	char		buf[45];
	register char	*id;

	for (;;)
	{
		if (gets (buf) != buf)
			exit (0);

		if ((id = pwcache (atoi (buf))) != NOSTR)
			printf ("%s\n", id);
		else
			printf ("?\n");
	}

}	/* end t */
