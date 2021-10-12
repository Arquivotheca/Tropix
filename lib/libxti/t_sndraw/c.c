#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xti.h>
#include <fcntl.h>
#include <errno.h>

#define	NOSTR	(char *)NULL
#define	EVER	;;

entry char	*pgname;	/* Nome do programa */

entry int	wflag;		/* Use read/write */
entry int	vflag;		/* Verbose */

const char	text[] = "*** Datagrama RAW de teste ***";

entry int	text_len;

main (int argc, register char *argv[])
{
	register int	opt, fd;
	T_BIND		bind;
	INADDR		addr, serv_addr;
	T_UNITDATA	unitdata;
	char		*str;
	int		proto_no;

	pgname = argv[0];
	text_len = strlen (text) + 1;

	/*
	 *	CLIENTE
	 *
	 *	Modo de usar:	t [-wv] <proto_no>
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
			printf ("Modo de usar: c [-wv] <proto_no>\n");
			exit (1);

		}	/* end switch */

	}	/* end while */

	argv += optind;
	argc -= optind;

	if (argc != 1)
	{
		printf ("Modo de usar: c [-v] <proto_no>\n");
		exit (1);
	}

	/*
	 *	Abre o "endpoint"
	 */
	str = "/dev/itnraw";

	if ((fd = t_open (str, O_RDWR, (T_INFO *)NULL)) < 0)
	{
		fprintf (stderr, "c: Não consegui abrir \"%s\"\n", str);
		perror (pgname);
		exit (1);
	}

	/*
	 *	Retira o valor de <proto_no>
	 */
	if ((proto_no = strtol (argv[0], &str, 0)) < 0 || *str != '\0')
	{
		fprintf (stderr, "c: Proto inválido: \"%s\"\n", argv[0]);
		exit (1);
	}

	if (vflag)
		printf ("c: proto_no dado:   %d\n", proto_no);

	addr.a_addr	= t_node_to_addr (fd, NOSTR, (long *)NULL); 
	addr.a_proto	= proto_no;

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
	 *	Manda o datagrama RAW
	 */
	unitdata.addr.len	= 0;
	unitdata.addr.buf	= &serv_addr;

	unitdata.udata.len	= text_len;
	unitdata.udata.buf	= text;

	if (t_sndraw (fd, &unitdata) < 0)
	{
		fprintf (stderr, "c: Erro no SNDRAW\n");
		perror (pgname);
		exit (1);
	}

	exit (0);

}	/* end c */
