#include <stdio.h>
#include <stdlib.h>
#include <xti.h>
#include <fcntl.h>

#define	NOSTR	(char *)NULL

entry char	*pgname;	/* Nome do programa */

main (int argc, register char *argv[])
{
	register int	fd;
	T_BIND		bind;
	INADDR		addr;
	char		*str;
	int		port_no;

	pgname = argv[0];

	/*
	 *	Modo de usar:	t "u" <port_no>
	 *			t "t" <port_no>
	 *
	 *		<port_no> == 0	=>  O sistema escolhe
	 */
	argv += 1;
	argc -= 1;

	if (argc != 2)
		{ printf ("Modo de usar: t 'u' <port_no>\n"); exit (1); }

	/*
	 *	Abre o "endpoint"
	 */
	if (argv[0][0] == 'u')
		str = "/dev/itnudp";
	else
		str = "/dev/itntcp";

	if ((fd = t_open (str, O_RDWR, (T_INFO *)NULL)) < 0)
	{
		fprintf (stderr, "Não consegui abrir \"%s\"\n", str);
		perror (pgname);
		exit (1);
	}

	printf ("Estado:	 %d\n", t_getstate (fd));

	/*
	 *	Retira o valor de <port_no>
	 */
	if ((port_no = strtol (argv[1], &str, 0)) < 0 || *str != '\0')
	{
		fprintf (stderr, "Valor inválido: \"%s\"\n", argv[1]);
		exit (1);
	}

	printf ("port_no dado:   %d\n", port_no);

	addr.a_port = port_no;

	bind.addr.len	 = sizeof (INADDR);
	bind.addr.maxlen = sizeof (INADDR);
	bind.addr.buf	 = &addr;

	bind.qlen = 9;

	/*
	 *	Associa um endereço
	 */
	if (t_bind (fd, &bind, &bind) < 0)
	{
		fprintf (stderr, "Não consegui associar um endereço\n");
		perror (pgname);
		exit (1);
	}

	printf ("Estado:	 %d\n", t_getstate (fd));
	printf ("port_no obtido: %d\n", ((INADDR *)bind.addr.buf)->a_port);
	printf ("qlen:		 %d\n", bind.qlen);

	/*
	 *	Dessocia o endereço
	 */
	if (t_unbind (fd) < 0)
	{
		fprintf (stderr, "Não consegui desassociar o endereço\n");
		perror (pgname);
		exit (1);
	}

	printf ("Estado:	 %d\n", t_getstate (fd));

	/*
	 *	Associa novamente um endereço
	 */
	bind.qlen = 10;

	if (t_bind (fd, &bind, &bind) < 0)
	{
		fprintf (stderr, "Não consegui associar um novo endereço\n");
		perror (pgname);
		exit (1);
	}

	printf ("Estado:	 %d\n", t_getstate (fd));
	printf ("port_no obtido: %d\n", ((INADDR *)bind.addr.buf)->a_port);
	printf ("qlen:		 %d\n", bind.qlen);

	exit (0);

}	/* end t */
