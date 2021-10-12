#include <stdio.h>

#define	NOSTR	(char *)NULL

main ()
{
	register	i;
	register char	*cp;
	char		buf[L_tmpnam];

	for (i = 0; i < 30; i++)
	{
		if ((cp = tmpnam (buf)) != NOSTR)
		{
			creat (cp, 0644);
			printf ("Consegui: \"%s\"\n", cp);
		}
		else
		{
			printf ("Nao consegui: \"%s\"\n", cp);
		}
	}
}
