#include <stdio.h>
#include <fcntl.h>
#include <xti.h>

#define	NOSTR	(char *)NULL

main (int argc, register char *argv[])
{
	register int	fd;
	char		*pgname;
	char		*id;
	long		addr;

	pgname = argv[0];

	if ((fd = t_open ("/dev/itnudp", O_RDWR, (T_INFO *)NULL)) < 0)
	{
		fprintf (stderr, "Não consegui abrir a rede\n");
		perror (pgname);
		exit (1);
	}

	if ((addr = t_str_to_addr (argv[1])) == -1)
	{
		fprintf (stderr, "Endereço inválido: \"%s\"\n", argv[1]);
		exit (1);
	}

	if ((id = t_addr_to_node (fd, addr)) == NOSTR)
	{
		fprintf (stderr, "Não consegui obter o nome de \"%s\"\n", argv[1]);
		perror (pgname);
		exit (1);
	}

	printf ("%s: %s\n", argv[1], id);

	exit (0);
}
