#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xti.h>
#include <fcntl.h>

#define	NOSTR	(char *)NULL
#define	EVER	;;

entry char	*pgname;	/* Nome do programa */

entry int	vflag;		/* Verbose */

const char	dev[] = "/dev/itntcp";

main (int argc, register char *argv[])
{
	register int	opt, fd, connfd, n;
	long		pid;
	T_BIND		bind;
	INADDR		addr, client_addr;
	T_CALL		call;
	char		*str;
	int		serv_port_no;
	char		serv_port_nm[16];
	char		area[80];
	int		flag, event;

	pgname = argv[0];

	/*
	 *	Modo de usar:	t [-v]  [<serv_port_no>]
	 *
	 *		<serv_port_no> == 0	=>  O sistema escolhe
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

		if (vflag)
			execl ("c", "c",  "-v", serv_port_nm, NOSTR);
		else
			execl ("c",  "c", serv_port_nm, NOSTR);

		fprintf (stderr, "Erro no EXECL\n");
		perror (pgname);
		exit (1);
	}

	/*
	 *	Dá o LISTEN
	 */
	call.addr.maxlen = sizeof (INADDR);
	call.addr.len	 = sizeof (INADDR);
	call.addr.buf	 = &client_addr;

	call.opt.len	 = 0;
	call.udata.len	 = 0;

#if (0)	/*************************************/
	for (EVER)
	{
		puts (t_strevent (event = t_look (fd)));

		if (event == T_LISTEN)
			break;
	}
#endif	/*************************************/

	if (t_listen (fd, &call) < 0)
	{
		fprintf (stderr, "Erro no LISTEN\n");
		perror (pgname);
		exit (1);
	}

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

   	bind.qlen = 0;

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

	t_close (fd);

#if (0)	/*************************************/
	for (EVER)
	{
		puts (t_strevent (event = t_look (connfd)));

		if (event == T_DATA)
			break;
	}
#endif	/*************************************/

	/*
	 *	Ecoa o texto
	 */
	if ((n = t_rcv (connfd, area, sizeof (area), &flag)) < 0)
	{
		fprintf (stderr, "Erro no RCV\n");
		perror (pgname);
		exit (1);
	}

	if (vflag)
		printf ("s: Texto recebido: \"%s\"\n", area);

	if (t_snd (connfd, area, n, T_PUSH) < 0)
	{
		fprintf (stderr, "Erro no SND\n");
		perror (pgname);
		exit (1);
	}

	/*
	 *	Termina a conexão
	 */
#if (0)	/*************************************/
	for (EVER)
	{
		puts (t_strevent (event = t_look (connfd)));

		if (event == T_ORDREL)
			break;
	}
#endif	/*************************************/

	if (t_rcvrel (connfd) < 0)
	{
		fprintf (stderr, "Erro no RCVREL\n");
		perror (pgname);
		exit (1);
	}

	if (t_sndrel (connfd) < 0)
	{
		fprintf (stderr, "Erro no SNDREL\n");
		perror (pgname);
		exit (1);
	}

	exit (0);

}	/* end t */
