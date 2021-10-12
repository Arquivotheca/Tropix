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

	if ((addr = t_str_to_addr (argv[1])) == -1)
	{
		fprintf
		(	stderr,
			"Erro no formato do endereço \"%s\"\n",
			argv[1]
		);
		exit (1);
	}

	printf ("%s: %s\n", argv[1], t_addr_to_str (addr));

	exit (0);
}
