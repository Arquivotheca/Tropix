#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xti.h>
#include <fcntl.h>
#include <errno.h>

#define	NOSTR	(char *)NULL
#define	EVER	;;

entry int	wflag;		/* Use read/write */
entry int	vflag;		/* Verbose */

const char	text[] = "*** Texto de teste ***";

entry int	text_len;

main (int argc, const char *argv[])
{
	int		opt, fd, n;
	T_BIND		bind;
	INADDR		addr[2], serv_addr;
	T_UNITDATA	unitdata;
	char		*str;
	int		serv_port_no;
	char		area[80];
	int		flag;
	int		i, fd_vec[1];

	pgname = argv[0];
	text_len = strlen (text) + 1;

	/*
	 *	CLIENTE
	 *
	 *	Modo de usar:	t [-wv] <serv_port_no>
	 */
	while ((opt = getopt (argc, argv, "wvH")) != EOF)
	{
		switch (opt)
		{
		    case 'w':			/* Read/write */
			wflag++;
			break;

		    case 'v':			/* Verbose */
			vflag++;
			break;

		    default:			/* Erro */
			putc ('\n', stderr);

		    case 'H':			/* Help */
			printf ("Modo de usar: c [-wv] <serv_port_no>\n");
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
	str = "/dev/itnudp";

	if ((fd = t_open (str, O_RDWR, (T_INFO *)NULL)) < 0)
	{
		fprintf (stderr, "c: Não consegui abrir \"%s\"\n", str);
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

	addr[0].a_port	 = 0;

	if (wflag)
	{
		addr[1].a_addr	 = t_node_to_addr (fd, NOSTR, (long *)NULL); 
		addr[1].a_port	 = serv_port_no;

		bind.addr.len	= 2 * sizeof (INADDR);
	}
	else
	{
		bind.addr.len	 = sizeof (INADDR);
	}

	bind.addr.maxlen = 2 * sizeof (INADDR);
	bind.addr.buf	 = addr;

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
	 *	Manda um texto
	 */
	serv_addr.a_addr	= t_node_to_addr (fd, NOSTR, (long *)NULL); 
	serv_addr.a_port	= serv_port_no;
	
	if (wflag)
	{
		n = write (fd, text, text_len);

		if (vflag)
			printf ("c: Usei WRITE (n = %d)\n", n);
	}
	else
	{
		unitdata.addr.len	= sizeof (serv_addr);
		unitdata.addr.buf	= &serv_addr;
	
		unitdata.udata.len	= text_len;
		unitdata.udata.buf	= text;
	
		if (t_sndudata (fd, &unitdata) < 0)
		{
			fprintf (stderr, "c: Erro no SNDUDATA\n");
			perror (pgname);
			exit (1);
		}

		if (vflag)
			printf ("c: Usei SNDUDATA\n");
	}

	/*
	 *	Confere o eco
	 */
	if (wflag)
	{
		if ((n = read (fd, area, sizeof (area))) < 0)
		{
			fprintf (stderr, "c: Erro no read\n");
			perror (pgname);
			exit (1);
		}

		if (vflag)
			printf ("c: Usei READ (n = %d)\n", n);
	}
	else
	{
		serv_addr.a_addr	= 0;
		serv_addr.a_port	= 0;

		unitdata.addr.maxlen	= sizeof (serv_addr);
		unitdata.addr.buf	= &serv_addr;
	
		unitdata.udata.maxlen	= sizeof (area);
		unitdata.udata.buf	= area;
	
		if (t_rcvudata (fd, &unitdata, &flag) < 0)
		{
			fprintf (stderr, "c: Erro no RCVUDATA\n");
			perror (pgname);
			exit (1);
		}

		n = unitdata.udata.len;

		if (vflag)
		{
			printf ("c: Usei RCVUDATA (n = %d, flag = %P) ", n, flag);
			printf
			(	" (%s, %d)\n",
				t_addr_to_str (serv_addr.a_addr),
				serv_addr.a_port
			);
		}
	}

	if (vflag)
		printf ("c: Texto recebido: \"%s\"\n", area);

	printf ("c: Texto recebido: \"%s\" (n = %d)\n", area, n);

	if (n != text_len || streq (area, text) == 0)
	{
		fprintf (stderr, "c: Erro no texto recebido: \"%s\"\n", area);
		exit (1);
	}

	/*
	 *	Envia um datagrama vazio
	 */
	unitdata.addr.len	= sizeof (serv_addr);
	unitdata.addr.buf	= &serv_addr;

	unitdata.udata.len	= 0;
	unitdata.udata.buf	= NOSTR;

	if (t_sndudata (fd, &unitdata) < 0)
	{
		fprintf (stderr, "c: Erro no SNDUDATA\n");
		perror (pgname);
		exit (1);
	}

	exit (0);

#if (0)	/*************************************/
#undef	ATTENTION
#ifdef	ATTENTION
	sleep (2);
#endif	ATTENTION

	if (t_snd (fd, text, text_len, T_PUSH) < 0)
	{
		fprintf (stderr, "Erro no SND\n");
		perror (pgname);
		exit (1);
	}

	printf ("c: Texto enviado:  \"%s\"\n", text);

#define	ATTENTION
#ifdef	ATTENTION
	/*
	 *	Attention
	 */
	printf ("c: event (rcv): %s\n", t_strevent (t_look (fd)));

	fd_vec[0] = fd;		i = -1;

	if ((i = attention (1, fd_vec, i, 0)) < 0)
	{
		fprintf (stderr, "Erro no ATTENTION\n");
		perror (pgname);
		exit (1);
	}

	printf ("attention: i = %d\n", i);

	printf ("c: event (rcv): %s\n", t_strevent (t_look (fd)));
#endif	ATTENTION

#undef	NONBLOCK
#ifdef	NONBLOCK
	/*
	 *	Não espera
	 */
	if (fcntl (fd, F_SETFL, O_NONBLOCK) < 0)
	{
		fprintf (stderr, "Erro no FCNTL\n");
		perror (pgname);
		exit (1);
	}

	for (EVER)
	{
		if ((n = t_rcv (fd, area, sizeof (area), &flag)) >= 0)
			break;

		fprintf (stderr, "c: Erro no RCV (%s)\n", strerror (t_errno));
		if (t_errno != TNODATA)
			exit (1);
	}

	if (fcntl (fd, F_SETFL, 0 /* desliga O_NONBLOCK */) < 0)
	{
		fprintf (stderr, "Erro no FCNTL\n");
		perror (pgname);
		exit (1);
	}
#else
	if ((n = t_rcv (fd, area, sizeof (area), &flag)) < 0)
	{
		fprintf (stderr, "Erro no RCV\n");
		perror (pgname);
		exit (1);
	}
#endif	NONBLOCK

	printf ("c: event (após rcv): %s\n", t_strevent (t_look (fd)));
#endif	/*************************************/

}	/* end t */
