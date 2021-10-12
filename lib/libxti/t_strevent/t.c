#include <xti.h>

char	number[20];

main (int argc, register char **argv)
{
	int		eventnum;
	extern char	*gets ();
	int		fd;

	if ((fd = open ("/dev/itntcp", 2)) < 0)
	{
		printf ("ERRO no OPEN\n");
		exit (1);
	}

	if (argc == 2)
	{
		eventnum = strtol (argv[1], (char **)0, 0);
		printf ("%d: %s\n", eventnum,  t_strevent (fd, eventnum));
		exit (0);
	}

	for (;;)
	{
		printf ("Número do evento: ");
		if (gets (number) != number)
			exit (0);
		eventnum = strtol (number, (char **)0, 0);
		printf ("%d: \"%s\"\n", eventnum,  t_strevent (fd, eventnum));
	}

}
