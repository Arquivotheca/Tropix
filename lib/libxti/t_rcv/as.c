#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <xti.h>
#include <fcntl.h>
#include <errno.h>

#define	NOSTR	(char *)NULL
#define	EVER	;;
#define	elif	else if

entry const char *pgname;	/* Nome do programa */

entry int	vflag;		/* Verbose */

entry int	global_tcp_fd;

const char	dev[] = "/dev/itntcp";

void	error (const char *, ...);

main (int argc, register const char *argv[])
{
	register int	opt, fd, connfd, n;
	long		pid;
	T_BIND		bind;
	INADDR		addr, client_addr;
	T_CALL		call;
	const char	*str;
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
		error ("$*Não consegui abrir \"%s\"", dev);

	global_tcp_fd = fd;

	/*
	 *	Retira o valor de <serv_port_no>
	 */
	serv_port_no = 9999;

	if (argc == 1 && ((serv_port_no = strtol (argv[0], &str, 0)) < 0 || *str != '\0'))
		error ("$Porta inválida: \"%s\"", argv[0]);

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
		error ("$*Não consegui associar um endereço");

	if (vflag)
		printf ("s: port_no do servidor obtido: %d\n", ((INADDR *)bind.addr.buf)->a_port);

	/*
	 *	Executa o CLIENTE
	 */
	if ((pid = fork ()) < 0)
		error ("$*Erro no FORK");

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

		error ("$*Erro no EXECL");
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
	sleep (2);
#endif	/*************************************/

#define	ATTENTION
#ifdef	ATTENTION
	/*
	 *	Attention
	 */
	fd_vec[0] = fd;		i = -1;

	printf ("s: event (listen): %s\n", t_strevent (fd, t_look (fd)));

	if ((i = attention (1, fd_vec, i, 0)) < 0)
		error ("$*Erro no ATTENTION");

	printf ("s: attention (listen): i = %d\n", i);

	printf ("s: event (listen): %s\n", t_strevent (fd, t_look (fd)));
#endif	ATTENTION

#undef	NONBLOCK
#ifdef	NONBLOCK
	/*
	 *	Não espera
	 */
	if (fcntl (fd, F_SETFL, O_NONBLOCK) < 0)
		error ("$*Erro no FCNTL");

	for (EVER)
	{
		if (t_listen (fd, &call) >= 0)
			break;

		error ("*Erro no LISTEN");

		if (t_errno != TNODATA)
			exit (1);
	}

	if (fcntl (fd, F_SETFL, 0 /* desliga O_NONBLOCK */) < 0)
		error ("$*Erro no FCNTL");
#else
	if (t_listen (fd, &call) < 0)
		error ("$*Erro no LISTEN");
#endif	NONBLOCK

	printf ("s: event (após listen): %s\n", t_strevent (fd, t_look (fd)));

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
		error ("$*Não consegui abrir \"%s\"", dev);

	addr.a_port	 = 0;

   /***	bind.addr.len	 = sizeof (INADDR); ***/
   /***	bind.addr.maxlen = sizeof (INADDR); ***/
   /***	bind.addr.buf	 = &addr; ***/

   /***	bind.qlen = 9; ***/

	/*
	 *	Associa um endereço
	 */
	if (t_bind (connfd, &bind, &bind) < 0)
		error ("$*Não consegui associar um endereço");

	if (vflag)
		printf ("s: port_no do servidor obtido: %d\n", ((INADDR *)bind.addr.buf)->a_port);

#endif	SAME_ENDPOINT

	/*
	 *	Dá o ACCEPT
	 */
	if (t_accept (fd, connfd, &call) < 0)
		error ("$*Erro no ACCEPT");

	/*
	 *	Attention
	 */
	fd_vec[0] = connfd; i = -1;

	if ((i = attention (1, fd_vec, i, 0)) < 0)
		error ("$*Erro no ATTENTION");

	printf ("s: attention (rcv): i = %d\n", i);

	/*
	 *	Ecoa o texto
	 */
	if ((n = t_rcv (connfd, area, sizeof (area), &flag)) < 0)
		error ("$*Erro no RCV");

	if (vflag)
		printf ("s: Texto recebido: \"%s\"\n", area);

#undef	ATTENTION
#ifdef	ATTENTION
	sleep (2);
#endif	ATTENTION

	if (t_snd (connfd, area, n, T_PUSH) < 0)
		error ("$*Erro no SND");

	/*
	 *	Termina a conexão
	 */
error ("Vou dar o SNDREL");

	if (t_sndrel (connfd) < 0)
		error ("$*Erro no SNDREL");

error ("Retornando do SNDREL");

	/*
	 *	Attention
	 */
	fd_vec[0] = connfd; i = -1;

	if ((i = attention (1, fd_vec, i, 0)) < 0)
		error ("$*Erro no ATTENTION");

	printf ("s: attention (rcvrel): i = %d\n", i);

	if (t_rcvrel (connfd) < 0)
		error ("$*Erro no RCVREL");

error ("Retornando do RCVREL");

	exit (0);

}	/* end t */

/*
 ****************************************************************
 *	Imprime uma mensagem de erro 				*
 ****************************************************************
 */
void
error (const char *format, ...)
{
	register va_list	args;
	register const char	*fmt = format;
	char			err, abrt;
	int			event;

	va_start (args, format);

	if (fmt == NOSTR)
		fmt = "$*Erro de entrada/saída de TCP";

	if ((abrt = *fmt) == '$')
		fmt++;

	if ((err = *fmt) == '*')
		fmt++;

	fprintf  (stderr, "%s: ", pgname);
	vfprintf (stderr, fmt, args);

	if (err == '*')
	{
		if (errno == TLOOK)
		{
			event = t_look (global_tcp_fd);
	
			fprintf (stderr, " (%s)", t_strevent (global_tcp_fd, event));
	
			if (event ==  T_DISCONNECT)
				{ fprintf (stderr, "\r\n"); exit (1); }
		}
		elif (errno != 0)
		{
			fprintf (stderr, " (%s)", strerror (errno));
		}
	}

	fprintf (stderr, "\r\n");

	if (abrt == '$')
		exit (1);

	va_end (args);

}	/* end error */
