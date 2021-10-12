#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xti.h>
#include <fcntl.h>
#include <errno.h>

#define	NOSTR	(char *)NULL
#define	EVER	;;
#define	elif	else if

entry int	vflag;		/* Verbose */
entry int	wflag;		/* Usa read/write */

const char	dev[] = "/dev/itnudp";

main (int argc, register char *argv[])
{
	register int	opt, fd, n;
	long		pid;
	T_BIND		bind;
	INADDR		addr, client_addr;
	INADDR		addrvec[4];
	T_CALL		call;
	T_UNITDATA	unitdata;
	char		*str;
	int		serv_port_no;
	char		serv_port_nm[16];
	char		area[80];
	int		flag;
	int		i, fd_vec[1];

	pgname = argv[0];

	/*
	 *	Modo de usar:	t [-v]  [<serv_port_no>]
	 *
	 *		<serv_port_no> == 0	=>  O sistema escolhe
	 */
	while ((opt = getopt (argc, argv, "wvH")) != EOF)
	{
		switch (opt)
		{
		    case 'w':			/* Usa read/write */
			wflag++;
			break;

		    case 'v':			/* Verbose */
			vflag++;
			break;

		    default:			/* Erro */
			putc ('\n', stderr);

		    case 'H':			/* Help */
			printf ("Modo de usar: t [-v] [<serv_port_no>]\n");
			exit (1);

		}	/* end switch */

	}	/* end while */

	argv += optind;
	argc -= optind;

	/*
	 *	Abre o "endpoint"
	 */
	if ((fd = t_open (dev, O_RDWR, (T_INFO *)NULL)) < 0)
	{
		fprintf (stderr, "Não consegui abrir \"%s\"\n", dev);
		perror (pgname);
		exit (1);
	}

	/*
	 *	Retira o valor de <serv_port_no>
	 */
	serv_port_no = 9999;

	if (argc == 1 && ((serv_port_no = strtol (argv[0], &str, 0)) < 0 || *str != '\0'))
	{
		fprintf (stderr, "s: Port inválido: \"%s\"\n", argv[0]);
		exit (1);
	}

	if (vflag)
	{
		printf ("s: argc:		 %d\n", argc);
		printf ("s: serv_port_no dado:   %d\n", serv_port_no);
	}

	addr.a_port	 = serv_port_no;

	bind.addr.len	 = sizeof (INADDR);
	bind.addr.maxlen = sizeof (INADDR);
	bind.addr.buf	 = &addr;

	bind.qlen = 0;

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
		printf ("s: port_no do servidor obtido: %d\n", ((INADDR *)bind.addr.buf)->a_port);

	/*
	 *	Executa o CLIENTE
	 */
	if ((pid = fork ()) < 0)
	{
		fprintf (stderr, "Erro no FORK\n");
		perror (pgname);
		exit (1);
	}

	if (pid == 0)
	{
		sprintf (serv_port_nm, "%d", serv_port_no);

#if (0)	/*************************************/
		sleep (2);
#endif	/*************************************/

		if   (wflag)
			execl ("c", "c",  "-vw", serv_port_nm, NOSTR);
		elif (vflag)
			execl ("c", "c",  "-v", serv_port_nm, NOSTR);
		else
			execl ("c",  "c", serv_port_nm, NOSTR);

		fprintf (stderr, "s: Erro no EXECL\n");
		perror (pgname);
		exit (1);
	}

#if (0)	/*************************************/
#if (0)	/*************************************/
	sleep (2);
#endif	/*************************************/

#undef	ATTENTION
#ifdef	ATTENTION
	/*
	 *	Attention
	 */
	fd_vec[0] = fd;		i = -1;

	printf ("s: event (listen): %s\n", t_strevent (t_look (fd)));

	if ((i = attention (1, fd_vec, i, 0)) < 0)
	{
		fprintf (stderr, "Erro no ATTENTION\n");
		perror (pgname);
		exit (1);
	}

	printf ("s: attention (listen): i = %d\n", i);

	printf ("s: event (listen): %s\n", t_strevent (t_look (fd)));
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
		if (t_listen (fd, &call) >= 0)
			break;

		fprintf (stderr, "s: Erro no LISTEN (%s)\n", strerror (t_errno));
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
	if (t_listen (fd, &call) < 0)
	{
		fprintf (stderr, "Erro no LISTEN\n");
		perror (pgname);
		exit (1);
	}
#endif	NONBLOCK

	printf ("s: event (após listen): %s\n", t_strevent (t_look (fd)));

	if (vflag)
	{
		printf ("s: port_no obtido: %d\n", ((INADDR *)call.addr.buf)->a_port);
		printf ("s: sequence:	 %d\n", call.sequence);
	}

#undef	SAME_ENDPOINT

#ifdef	SAME_ENDPOINT

#define	connfd	fd

#else
	/*
	 *	Obtém novo "endpoint" para a conexão
	 */
	if ((connfd = t_open (dev, O_RDWR, (T_INFO *)NULL)) < 0)
	{
		fprintf (stderr, "Não consegui abrir \"%s\"\n", dev);
		perror (pgname);
		exit (1);
	}

	addr.a_port	 = 0;

   /***	bind.addr.len	 = sizeof (INADDR); ***/
   /***	bind.addr.maxlen = sizeof (INADDR); ***/
   /***	bind.addr.buf	 = &addr; ***/

   /***	bind.qlen = 9; ***/

	/*
	 *	Associa um endereço
	 */
	if (t_bind (connfd, &bind, &bind) < 0)
	{
		fprintf (stderr, "Não consegui associar um endereço\n");
		perror (pgname);
		exit (1);
	}

	if (vflag)
		printf ("s: port_no do servidor obtido: %d\n", ((INADDR *)bind.addr.buf)->a_port);

#endif	SAME_ENDPOINT

	/*
	 *	Dá o ACCEPT
	 */
	if (t_accept (fd, connfd, &call) < 0)
	{
		fprintf (stderr, "Erro no ACCEPT\n");
		perror (pgname);
		exit (1);
	}
#endif	/*************************************/

	/*
	 *	Ecoa o texto
	 */
	unitdata.addr.maxlen	= sizeof (client_addr);
	unitdata.addr.buf	= &client_addr;

	unitdata.udata.maxlen	= sizeof (area);
	unitdata.udata.buf	= area;

	if (t_rcvudata (fd, &unitdata, &flag) < 0)
	{
		fprintf (stderr, "s: Erro no RCVUDATA\n");
		perror (pgname);
		exit (1);
	}

	if (t_getaddr (fd, addrvec) < 0)
	{
		fprintf (stderr, "Não consegui obter os endereços\n");
		perror (pgname);
		exit (1);
	}

	printf ("Após o RCVUDATA:");
	printf
	(	" (%s, %d)",
		t_addr_to_str (addrvec[0].a_addr),
		addrvec[0].a_port
	);
	printf
	(	" (%s, %d)",
		t_addr_to_str (addrvec[1].a_addr),
		addrvec[1].a_port
	);
	printf
	(	" (%s, %d)",
		t_addr_to_str (addrvec[2].a_addr),
		addrvec[2].a_port
	);
	printf
	(	" (%s, %d)\n",
		t_addr_to_str (addrvec[3].a_addr),
		addrvec[3].a_port
	);

	if (vflag)
		printf ("s: Texto recebido: \"%s\"\n", area);

#undef	ATTENTION
#ifdef	ATTENTION
	sleep (2);
#endif	ATTENTION

	unitdata.addr.len	= sizeof (client_addr);

   /***	unitdata.udata.len	= ...; ***/

	if (t_sndudata (fd, &unitdata) < 0)
	{
		fprintf (stderr, "s: Erro no SNDUDATA\n");
		perror (pgname);
		exit (1);
	}

	if (t_getaddr (fd, addrvec) < 0)
	{
		fprintf (stderr, "Não consegui obter os endereços\n");
		perror (pgname);
		exit (1);
	}

	printf ("Após o SNDUDATA:");
	printf
	(	" (%s, %d)",
		t_addr_to_str (addrvec[0].a_addr),
		addrvec[0].a_port
	);
	printf
	(	" (%s, %d)",
		t_addr_to_str (addrvec[1].a_addr),
		addrvec[1].a_port
	);
	printf
	(	" (%s, %d)",
		t_addr_to_str (addrvec[2].a_addr),
		addrvec[2].a_port
	);
	printf
	(	" (%s, %d)\n",
		t_addr_to_str (addrvec[3].a_addr),
		addrvec[3].a_port
	);

	/*
	 *	Recebe o datagrama vazio
	 */
	unitdata.udata.maxlen	= 0;

	if (t_rcvudata (fd, &unitdata, &flag) < 0)
	{
		fprintf (stderr, "s: Erro no RCVUDATA de vazio\n");
		perror (pgname);
		exit (1);
	}

	printf ("Tamanho do datagrama vazio = %d\n", unitdata.udata.len);

	exit (0);

}	/* end t */
