#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xti.h>
#include <fcntl.h>
#include <errno.h>

#define	NOSTR	(char *)NULL

entry const char *pgname;	/* Nome do programa */

entry int	vflag;		/* Verbose */

const char	text[] = "*** Texto de teste ***";

entry int	text_len;

main (int argc, register const char *argv[])
{
	register int	opt, fd, n;
	T_BIND		bind;
	INADDR		addr;
	T_CALL		call;
	const char	*str;
	int		serv_port_no;
	char		area[80];
	int		flag, event;

	pgname = argv[0];
	text_len = strlen (text) + 1;

	/*
	 *	CLIENTE
	 *
	 *	Modo de usar:	t [-v] <serv_port_no>
	 */
	while ((opt = getopt (argc, argv, "vH")) != EOF)
	{
		switch (opt)
		{
		    case 'v':			/* Verbose */
			vflag++;
			break;

		    default:			/* Erro */
			putc ('\n', stderr);

		    case 'H':			/* Help */
			printf ("Modo de usar: c [-v] <serv_port_no>\n");
			exit (1);

		}	/* end switch */

	}	/* end while */

	argv += optind;
	argc -= optind;

	if (argc != 1)
	{
		printf ("Modo de usar: c [-v] <serv_port_no>\n");
		exit (1);
	}

	/*
	 *	Abre o "endpoint"
	 */
	str = "/dev/itntcp";

	if ((fd = t_open (str, O_RDWR, (T_INFO *)NULL)) < 0)
	{
		fprintf (stderr, "Não consegui abrir \"%s\"\n", str);
		perror (pgname);
		exit (1);
	}

	/*
	 *	Retira o valor de <serv_port_no>
	 */
	if ((serv_port_no = strtol (argv[0], &str, 0)) < 0 || *str != '\0')
	{
		fprintf (stderr, "c: Port inválido: \"%s\"\n", argv[0]);
		exit (1);
	}

	if (vflag)
		printf ("c: serv_port_no dado:   %d\n", serv_port_no);

	addr.a_port	 = 0;

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

	if (vflag)
		printf ("c: port_no do cliente obtido: %d\n", ((INADDR *)bind.addr.buf)->a_port);

	/*
	 *	Dá o CONNECT
	 */
	addr.a_port	 = serv_port_no;
	addr.a_addr	 = t_node_to_addr (fd, NOSTR, (long *)NULL); 

	call.addr.len	 = sizeof (INADDR);
	call.addr.maxlen = sizeof (INADDR);
	call.addr.buf	 = &addr;

	call.opt.len	 = 0;
	call.udata.len	 = 0;

	if (t_connect (fd, &call, &call) < 0)
	{
		fprintf (stderr, "Erro no CONNECT\n");
		perror (pgname);
		exit (1);
	}

	if (vflag)
		printf ("c: port_no obtido: %d\n", ((INADDR *)call.addr.buf)->a_port);

	/*
	 *	Manda um texto e confere o eco
	 */
	if (t_snd (fd, text, text_len, T_PUSH) < 0)
	{
		fprintf (stderr, "Erro no SND\n");
		perror (pgname);
		exit (1);
	}

	printf ("c: Texto enviado:  \"%s\"\n", text);

	do
	{
		if ((n = t_nread (fd)) < 0)
		{
			fprintf (stderr, "Erro no NREAD\n");
			perror (pgname);
			exit (1);
		}

		printf ("c: NREAD: %d\n", n);

		sleep (1);
	}
	while (n == 0);

	if ((n = t_rcv (fd, area, sizeof (area), &flag)) < 0)
	{
		fprintf (stderr, "Erro no RCV\n");
		perror (pgname);
		exit (1);
	}

	printf ("c: Texto recebido: \"%s\" (n = %d)\n", area, n);

	if (n != text_len || streq (area, text) == 0)
	{
		fprintf (stderr, "c: Erro no texto recebido: \"%s\"\n", area);
		exit (1);
	}

	do
	{
		if ((n = t_nread (fd)) < 0)
		{
			if (errno != TLOOK)
			{
				fprintf (stderr, "Erro no NREAD\n");
				perror (pgname);
				exit (1);
			}

			event = t_look (fd);
			fprintf (stderr, "c: NREAD: %d (%s)\n", n, t_strevent (fd, event));
	
		}
		else
		{
			printf ("c: NREAD: %d\n", n);
			sleep (1);
		}
	}
	while (n == 0);

	/*
	 *	Termina a conexão
	 */
	if (t_sndrel (fd) < 0)
	{
		fprintf (stderr, "Erro no SNDREL\n");
		perror (pgname);
		exit (1);
	}

	if (t_rcvrel (fd) < 0)
	{
		fprintf (stderr, "Erro no RCVREL\n");
		perror (pgname);
		exit (1);
	}

	exit (0);

}	/* end t */
