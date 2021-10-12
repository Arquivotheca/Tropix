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
entry int	wflag;		/* Window flag */

const char	dev[] = "/dev/itntcp";

entry int	global_tcp_fd;

void		error (const char *format, ...);

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
	char		area[512];
	int		flag;
	int		i, fd_vec[1];
	long		count = 0;
	const char	*count_str = "10";

	pgname = argv[0];

	/*
	 *	Modo de usar:	t [-v]  [<no. de blocos>]
	 *
	 *		<serv_port_no> == 0	=>  O sistema escolhe
	 */
	while ((opt = getopt (argc, argv, "vwH")) != EOF)
	{
		switch (opt)
		{
		    case 'v':			/* Verbose */
			vflag++;
			break;

		    case 'w':			/* Window */
			wflag++;
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
	 *	Não retira o valor de <serv_port_no>
	 */
	serv_port_no = 9999;

	/*
	 *	x
	 */
	if (argc == 1)
		count_str = argv[0];

	if (vflag)
	{
		printf ("ws: argc:		 %d\n", argc);
		printf ("ws: serv_port_no dado:   %d\n", serv_port_no);
		printf ("ws: count_str dado:      %s\n", count_str);
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
		printf ("ws: port_no do servidor obtido: %d\n", ((INADDR *)bind.addr.buf)->a_port);

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
			execl ("wc", "wc",  "-v", serv_port_nm, count_str, NOSTR);
		else
			execl ("wc",  "wc", serv_port_nm, count_str, NOSTR);

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

	if (t_listen (fd, &call) < 0)
		error ("$*Erro no LISTEN");

	if (vflag)
	{
		printf ("ws: event (após listen): %s\n", t_strevent (fd, t_look (fd)));
		printf ("ws: port_no obtido: %d\n", ((INADDR *)call.addr.buf)->a_port);
		printf ("ws: sequence:	 %d\n", call.sequence);
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
		printf ("ws: port_no do servidor obtido: %d\n", ((INADDR *)bind.addr.buf)->a_port);

#endif	SAME_ENDPOINT

	/*
	 *	Dá o ACCEPT
	 */
	if (t_accept (fd, connfd, &call) < 0)
		error ("$*Erro no ACCEPT");

	/*
	 *	Lê o texto
	 */
	while ((n = t_rcv (connfd, area, sizeof (area), &flag)) > 0)
	{
		count += n;

		if (vflag)
			printf ("ws: recebi %d bytes\n", n);

		if (wflag)
			sleep (20);
#if (0)	/*******************************************************/
		sleep (1);
		sleep (1); printf ("Saindo do SLEEP\n");
#endif	/*******************************************************/
	}

	if (n < 0)
		error ("$*Erro no RCV");

	printf ("ws: Recebi %d bytes\n", count);

	/*
	 *	Termina a conexão
	 */
	if (t_sndrel (connfd) < 0)
		error ("$*Erro no SNDREL");

	if (t_rcvrel (connfd) < 0)
		error ("$*Erro no RCVREL");

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
