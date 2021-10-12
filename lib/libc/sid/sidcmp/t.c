#include <stdio.h>
#include <id.h>

#define NOSTR (char *)NULL

main (int argc, register const char *argv[])
{
	sid_t		a, b, c, d;
	const char	*str1 = argv[1], *str2 = argv[2];

	if (str1 == NOSTR)
	{
		str1 = "alpha";
		str2 = "alpha";
	}
	else if (str2 == NOSTR)
	{
		str2 = "alpha";
	}

	printf
	(	"sidcan: a = \"%-7.7s\", b = \"%-7.7s\"\n",
		sidcan (a, str1),
		sidcan (b, str2)
	);

	printf
	(	"sidcpy e sidclr: c = \"%-7.7s\"\n",
		sidcpy (sidclr (c), sidcan (a, str1))
	);

	printf
	(	"sidcmp: %d\n",
		sidcmp  (sidcpy (c, sidcan (a, str1)),
			sidcpy (d, sidcan (b, str2))
	));

	printf
	(	"sideq: %d\n",
		sideq   (sidcpy (c, sidcan (a, str1)),
			sidcpy (d, sidcan (b, str2))
	));
}
