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

const char	dev[] = "/dev/itnudp";

main (int argc, register char *argv[])
{
	register int	opt, fd, n;
	long		pid;
	T_BIND		bind;
	INADDR		addr[2];
	INADDR		addrvec[3];
	T_UNITDATA	unitdata;
	char		*str;
	int		serv_port_no;
	char		serv_port_nm[16];
	char		area[80];

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

	addr[0].a_port	 = serv_port_no;

	bind.addr.len	 = sizeof (INADDR);
	bind.addr.maxlen = sizeof (INADDR);
	bind.addr.buf	 = addr;

	bind.qlen = 0;

	/*
	 *	Associa um endereço
	 */
	if (t_bind (fd, &bind, &bind) < 0)
	{
		fprintf (stderr, "s: Não consegui associar um endereço\n");
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
		fprintf (stderr, "s: Erro no FORK\n");
		perror (pgname);
		exit (1);
	}

	if (pid == 0)
	{
		sprintf (serv_port_nm, "%d", serv_port_no);

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

	/*
	 *	Ecoa o texto
	 */
	if ((n = read (fd, area, sizeof (area))) < 0)
	{
		fprintf (stderr, "s: Erro no read\n");
		perror (pgname);
		exit (1);
	}

	if (t_getaddr (fd, addrvec) < 0)
	{
		fprintf (stderr, "s: Não consegui obter os endereços\n");
		perror (pgname);
		exit (1);
	}

	printf ("s: Após o read:");
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
	(	" (%s, %d)\n",
		t_addr_to_str (addrvec[2].a_addr),
		addrvec[2].a_port
	);

	if (vflag)
		printf ("s: Texto recebido: \"%s\"\n", area);

	unitdata.addr.len	= sizeof (INADDR);
	unitdata.addr.buf	= &addrvec[2];

	unitdata.udata.len	= n;
	unitdata.udata.buf	= area;

	if (t_sndudata (fd, &unitdata) < 0)
	{
		fprintf (stderr, "c: Erro no SNDUDATA\n");
		perror (pgname);
		exit (1);
	}

#if (0)	/*************************************/
	if (write (fd, area, n) < 0)
	{
		fprintf (stderr, "s: Erro no WRITE\n");
		perror (pgname);
		exit (1);
	}
#endif	/*************************************/

	if (t_getaddr (fd, addrvec) < 0)
	{
		fprintf (stderr, "s: Não consegui obter os endereços\n");
		perror (pgname);
		exit (1);
	}

	printf ("s: Após o write:");
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
	(	" (%s, %d)\n",
		t_addr_to_str (addrvec[2].a_addr),
		addrvec[2].a_port
	);

	exit (0);

}	/* end t */
