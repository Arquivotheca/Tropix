#include <sys/types.h>

#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <xti.h>

main (int argc, register char *argv[])
{
	register int	fd;
	char		*pgname;
	long		addr, my_addr;

	pgname = argv[0];

	if ((fd = t_open ("/dev/itnudp", O_RDWR, (T_INFO *)NULL)) < 0)
	{
		fprintf (stderr, "Não consegui abrir a rede\n");
		perror (pgname);
		exit (1);
	}


	if ((addr = t_node_to_addr (fd, argv[1], &my_addr)) == -1)
	{
		fprintf
		(	stderr,
			"Não consegui obter o endereço de \"%s\" (%s)\n",
			argv[1], strerror (errno)
		);
		exit (1);
	}

	printf ("%s: %s (%s), ", argv[1], t_addr_to_node (fd, addr), t_addr_to_str (addr));

	printf ("%s (%s)\n", t_addr_to_node (fd, my_addr), t_addr_to_str (my_addr));

	exit (0);
}
