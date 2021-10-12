#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <grp.h>

main ()
{
	char		buf[45];
	register char	*cp;

	for (;;)
	{
		if (gets (buf) != buf)
			exit (0);

		if ((cp = grcache (atoi (buf))) != NOSTR)
			printf ("%s\n", cp);
		else
			printf ("?\n");
	}

}	/* end t */
