#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <nlist.h>

int
main (int argc, const char *argv[])
{
	int		n;
	NLIST		nl[2];

	if (argc != 3)
	{
		printf ("Args ?\n");
		exit (1);
	}

	nl[0].n_name = argv[2];
	nl[1].n_name = NOSTR;

	n = nlist (argv[1], nl);

	printf ("Codigo de Retorno = %d\n", n);

	printf ("%s %P %02X\n", nl[0].n_name, nl[0].n_value, nl[0].n_type);

	return (0);

}	/* end t */

