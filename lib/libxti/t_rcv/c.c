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

const char	text[] = "*** Texto de teste ***";

entry int	text_len;

void	error (const char *, ...);

main (int argc, register const char *argv[])
{
	register int	opt, fd, n;
	T_BIND		bind;
	INADDR		addr;
	T_CALL		call;
	const char	*str;
	int		serv_port_no;
	char		area[80];
	int		flag;
	int		i, fd_vec[1];

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
		error ("$*Não consegui abrir \"%s\"", str);

	global_tcp_fd = fd;

	/*
	 *	Retira o valor de <serv_port_no>
	 */
	if ((serv_port_no = strtol (argv[0], &str, 0)) < 0 || *str != '\0')
		error ("$Porta inválida: \"%s\"", argv[0]);

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
		error ("$*Erro no BIND");

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
		error ("$*Erro no CONNECT");

	if (vflag)
		printf ("c: port_no obtido: %d\n", ((INADDR *)call.addr.buf)->a_port);

	/*
	 *	Manda um texto e confere o eco
	 */
	if (t_snd (fd, text, text_len, T_PUSH) < 0)
		error ("$*Erro no SND");

	printf ("c: Texto enviado:  \"%s\"\n", text);

#undef	ATTENTION
#ifdef	ATTENTION
	/*
	 *	Attention
	 */
	printf ("c: event (rcv): %s\n", t_strevent (fd, t_look (fd)));

	fd_vec[0] = fd;		i = -1;

	if ((i = attention (1, fd_vec, i, 0)) < 0)
		error ("$*Erro no ATTENTION");

	printf ("attention: i = %d\n", i);

	printf ("c: event (rcv): %s\n", t_strevent (fd, t_look (fd)));
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
		if ((n = t_rcv (fd, area, sizeof (area), &flag)) >= 0)
			break;

		error ("*Erro no RCV");

		if (t_errno != TNODATA)
			exit (1);
	}

	if (fcntl (fd, F_SETFL, 0 /* desliga O_NONBLOCK */) < 0)
		error ($*Erro no FCNTL");
#else
	if ((n = t_rcv (fd, area, sizeof (area), &flag)) < 0)
		error ("$*Erro no RCV");
#endif	NONBLOCK

	printf ("c: event (após rcv): %s\n", t_strevent (fd, t_look (fd)));

	printf ("c: Texto recebido: \"%s\" (n = %d)\n", area, n);

	if (n != text_len || streq (area, text) == 0)
		error ("$Erro no texto recebido: \"%s\"", area);
	else
		printf ("c: texto confere\n");

	/*
	 *	Termina a conexão
	 */
error ("Vou dar o SNDREL");

	if (t_sndrel (fd) < 0)
		error ("$*Erro no SNDREL");

error ("Retornando do SNDREL");

	if (t_rcvrel (fd) < 0)
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
