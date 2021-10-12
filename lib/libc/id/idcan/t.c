#include <stdio.h>
#include <stdlib.h>
#include <id.h>

main ()
{
	id_t		id;
	char		area[32];

	while (gets (area) == area)
	{
		printf ("\"%s\" => \"%s\"\n", area, idcan (id, area));
	}

	exit (0);
}
