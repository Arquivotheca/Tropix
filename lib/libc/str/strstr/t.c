#include <stdio.h>
#include <string.h>


char	cadeia[] =	"Uma cadeia cheia de palavras "
			"que podem ser procuradas por quem?";

main ()
{
	register char 	*cp;
	char		buf[128];

	printf ("Cadeia = \"%s\"\n", cadeia);

	for (;;)
	{
		if (gets (buf) != buf)
		{
			cp = strstr (cadeia, "");
			printf ("\"%s\" => \"%s\"\n", "", cp);
			return (0);
		}

		if (cp = strstr (cadeia, buf))
			printf ("\"%s\" => \"%s\"\n", buf, cp);
		else
			printf ("\"%s\" => \"%s\"\n", buf, "NOSTR");
	}
}
