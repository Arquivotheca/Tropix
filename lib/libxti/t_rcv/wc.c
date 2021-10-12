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
#define	BLSZ	512
#define	elif	else if

entry const char *pgname;	/* Nome do programa */

entry int	vflag;		/* Verbose */

const char	text[] = "*** Texto de teste ***";

entry int	text_len;

entry int	global_tcp_fd;

void		error (const char *format, ...);

main (int argc, register const char *argv[])
{
	register int	opt, fd, n;
	T_BIND		bind;
	INADDR		addr;
	T_CALL		call;
	const char	*str;
	int		serv_port_no;
	char		*area;
	int		flag;
	int		i, fd_vec[1];
	int		area_count = 10;
	long		count = 0;

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
			printf ("Modo de usar: wc [-v] <serv_port_no>\n");
			exit (1);

		}	/* end switch */

	}	/* end while */

	argv += optind;
	argc -= optind;

	if (argc < 2)
	{
		printf ("Modo de usar: wc [-v] <serv_port_no> [<no. de blocos>]\n");
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
		error ("$Port inválido: \"%s\"", argv[0]);

	if (argv[1] && ((area_count = strtol (argv[1], &str, 0)) <= 0 || *str != '\0'))
		error ("$Area_count inválido: \"%s\"", argv[1]);

	if ((area = malloc (area_count * BLSZ)) == NOSTR)
		error ("$Memória esgotada");

	if (vflag)
		printf ("wc: serv_port_no dado:   %d\n", serv_port_no);

	addr.a_port	 = 0;

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
		printf ("wc: port_no do cliente obtido: %d\n", ((INADDR *)bind.addr.buf)->a_port);

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
		printf ("wc: port_no obtido: %d\n", ((INADDR *)call.addr.buf)->a_port);

	/*
	 *	Manda "area_count * BLSZ" bytes
	 */
	if (t_snd (fd, area, area_count * BLSZ, T_PUSH) < 0)
		error ("$*Erro no SND");

	printf ("wc: Enviei %d bytes\n", area_count * BLSZ);

	/*
	 *	Termina a conexão
	 */
	if (t_sndrel (fd) < 0)
		error ("$*Erro no SNDREL");

	if (t_rcvrel (fd) < 0)
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
