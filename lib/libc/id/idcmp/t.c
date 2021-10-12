#include <stdio.h>
#include <id.h>

#define NOSTR (char *)NULL

main (int argc, register const char *argv[])
{
	id_t		a, b, c, d;
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
	(	"idcan: a = \"%-14.14s\", b = \"%-14.14s\"\n",
		idcan (a, str1),
		idcan (b, str2)
	);

	printf
	(	"idcpy e idclr: c = \"%-14.14s\"\n",
		idcpy (idclr (c), idcan (a, str1))
	);

	printf
	(	"idcmp: %d\n",
		idcmp  (idcpy (c, idcan (a, str1)),
			idcpy (d, idcan (b, str2))
	));

	printf
	(	"ideq: %d\n",
		ideq   (idcpy (c, idcan (a, str1)),
			idcpy (d, idcan (b, str2))
	));
}
