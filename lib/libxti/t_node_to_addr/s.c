#include <sys/types.h>

#include <stdio.h>
#include <fcntl.h>
#include <xti.h>

#define	NOSTR	(char *)NULL

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

	puts (t_addr_to_node (fd, t_node_to_addr (fd, NOSTR, (long *)NULL)));
}
