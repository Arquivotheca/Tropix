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

entry char	*pgname;	/* Nome do programa */

entry int	vflag;		/* Verbose */
entry int	wflag;		/* Usa read/write */

const char	dev[] = "/dev/itnraw";

main (int argc, register char *argv[])
{
	register int	opt, fd;
	long		pid;
	T_BIND		bind;
	INADDR		addr, client_addr;
	T_UNITDATA	unitdata;
	INADDR		addrvec[4];
	char		*str;
	int		proto_no;
	char		proto_nm[16];
	char		area[80];
	int		flag;

	pgname = argv[0];

	/*
	 *	Modo de usar:	t [-v] [<proto_no>]
	 *
	 *		<proto_no> não dado => 13
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
			printf ("Modo de usar: t [-v] [<proto_no>]\n");
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
	 *	Retira o valor de <proto_no>
	 */
	proto_no = 13;

	if (argc == 1 && ((proto_no = strtol (argv[0], &str, 0)) < 0 || *str != '\0'))
	{
		fprintf (stderr, "s: Proto inválido: \"%s\"\n", argv[0]);
		exit (1);
	}

	if (vflag)
	{
		printf ("s: argc:		 %d\n", argc);
		printf ("s: proto_no dado:   %d\n", proto_no);
	}

	addr.a_addr	 = 0;
	addr.a_proto	 = proto_no;

	bind.addr.len	 = sizeof (INADDR);
	bind.addr.maxlen = sizeof (INADDR);
	bind.addr.buf	 = &addr;

	/*
	 *	Associa um endereço
	 */
	if (t_bind (fd, &bind, &bind) < 0)
	{
		fprintf (stderr, "Não consegui associar um endereço\n");
		perror (pgname);
		exit (1);
	}

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
		sprintf (proto_nm, "%d", proto_no);

		if   (wflag)
			execl ("c", "c",  "-vw", proto_nm, NOSTR);
		elif (vflag)
			execl ("c", "c",  "-v", proto_nm, NOSTR);
		else
			execl ("c",  "c", proto_nm, NOSTR);

		fprintf (stderr, "s: Erro no EXECL\n");
		perror (pgname);
		exit (1);
	}

	/*
	 *	Recebe o datagrama RAW
	 */
	unitdata.addr.maxlen	= sizeof (client_addr);
	unitdata.addr.buf	= &client_addr;

	unitdata.udata.maxlen	= sizeof (area);
	unitdata.udata.buf	= area;

	if (t_rcvraw (fd, &unitdata, &flag) < 0)
	{
		fprintf (stderr, "s: Erro no RCVRAW\n");
		perror (pgname);
		exit (1);
	}

	printf
	(	"s: Endereço = \"%s\", protocolo = %d\n",
		t_addr_to_str (client_addr.a_addr), client_addr.a_proto
	);
	printf ("s: Datagrama recebido: \"%s\"\n", area);

	if (t_getaddr (fd, addrvec) < 0)
	{
		fprintf (stderr, "Não consegui obter os endereços\n");
		perror (pgname);
		exit (1);
	}

	printf ("Após o RCVRAW:");
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

	exit (0);

}	/* end t */
