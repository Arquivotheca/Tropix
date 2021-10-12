#include <sys/types.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <xti.h>
#include <errno.h>

#define	EVER	;;

main (int argc, register char *argv[])
{
	register int	fd;
	char		*pgname;
	const char	*node_nm;
	int		preference = 0;

	pgname = argv[0];

	if ((fd = t_open ("/dev/itnudp", O_RDWR, (T_INFO *)NULL)) < 0)
	{
		fprintf (stderr, "Não consegui abrir a rede\n");
		perror (pgname);
		exit (1);
	}

	if (argv[1] == NOSTR)
		exit (0);

	for (EVER)
	{
		if ((node_nm = t_mail_to_node (fd, argv[1], preference, &preference)) == NOSTR)
		{
			if (errno == ENOENT)
				exit (0);

			fprintf
			(	stderr,
				"Não consegui obter o servidor de \"%s\" (%s)\n",
				argv[1], strerror (errno)
			);
			exit (1);
		}

		printf ("%s: \"%s\" (%d)\n", argv[1], node_nm, preference);
		preference++;
	}
}
