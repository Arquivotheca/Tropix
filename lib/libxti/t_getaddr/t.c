#include <stdio.h>
#include <stdlib.h>
#include <xti.h>
#include <fcntl.h>

#define	NOSTR	(char *)NULL

entry char	*pgname;	/* Nome do programa */

main (int argc, register char *argv[])
{
	register int	fd;
	T_INFO		info;

	pgname = argv[0];

	/*
	 *	Modo de usar: t
	 *
	 *	T_COTS		1	connection mode
	 *	T_COTS_ORD	2	connection mode com desconexão ordenada
	 *	T_CLTS		3	connectionless mode
	 */
	argv += 1;
	argc -= 1;

	/*
	 *	Abre o "endpoint" UDP
	 */
	if ((fd = t_open ("/dev/itnudp", O_RDWR, (T_INFO *)NULL)) < 0)
	{
		fprintf (stderr, "Não consegui abrir \"%s\"\n", "/dev/itnudp");
		perror (pgname);
		exit (1);
	}

	/*
	 *	Obtém as informações UDP
	 */
	if (t_getinfo (fd, &info) < 0)
	{
		fprintf (stderr, "Não consegui obter as informações\n");
		perror (pgname);
		exit (1);
	}

	/*
	 *	Imprime as informações UDP
	 */
	printf
	(	"\nUDP:\n\n"
		"info.addr = %d\n"
		"info.options = %d\n"
		"info.tsdu = %d\n"
		"info.etsdu = %d\n"
		"info.connect = %d\n"
		"info.discon = %d\n"
		"info.servtype = %d\n",

	 	info.addr,
		info.options,
		info.tsdu,
		info.etsdu,
		info.connect,
		info.discon,
		info.servtype
	);

	t_close (fd);

	/*
	 *	Abre o "endpoint" TCP
	 */
	if ((fd = t_open ("/dev/itntcp", O_RDWR, (T_INFO *)NULL)) < 0)
	{
		fprintf (stderr, "Não consegui abrir \"%s\"\n", "/dev/itntcp");
		perror (pgname);
		exit (1);
	}

	/*
	 *	Obtém as informações TCP
	 */
	if (t_getinfo (fd, &info) < 0)
	{
		fprintf (stderr, "Não consegui obter as informações\n");
		perror (pgname);
		exit (1);
	}

	/*
	 *	Imprime as informações TCP
	 */
	printf
	(	"\nTCP:\n\n"
		"info.addr = %d\n"
		"info.options = %d\n"
		"info.tsdu = %d\n"
		"info.etsdu = %d\n"
		"info.connect = %d\n"
		"info.discon = %d\n"
		"info.servtype = %d\n",

	 	info.addr,
		info.options,
		info.tsdu,
		info.etsdu,
		info.connect,
		info.discon,
		info.servtype
	);

	t_close (fd);

	exit (0);

}	/* end t */
