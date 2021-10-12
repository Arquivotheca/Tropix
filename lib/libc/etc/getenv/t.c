#include <stdio.h>

#define	NOSTR	(char *)0

char	*getenv ();

main ()
{
	char		buf[30];
	register char	*cp;

	for (;;)
	{
		if (gets (buf) == NOSTR)
			exit (0);

		if ((cp = getenv (buf)) != NOSTR)
			printf ("%s = %s\n", buf, cp);
		else
			printf ("%s NOT FOUND\n", buf);
	}

}	/* end t */
