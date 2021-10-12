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

const char	text[] = "*** Texto de teste ***";

entry int	text_len;

main (int argc, register char *argv[])
{
	register int	opt, fd, n;
	T_BIND		bind;
	INADDR		addr[2], serv_addr;
	char		*str;
	int		serv_port_no;
	char		area[80];

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

	addr[1].a_addr	 = t_node_to_addr (fd, NOSTR, (long *)NULL); 
	addr[1].a_port	 = serv_port_no;

	bind.addr.len	= 2 * sizeof (INADDR);

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
	
	n = write (fd, text, text_len);

	if (vflag)
		printf ("c: Usei WRITE (n = %d)\n", n);

	/*
	 *	Confere o eco
	 */
	if ((n = read (fd, area, sizeof (area))) < 0)
	{
		fprintf (stderr, "c: Erro no read\n");
		perror (pgname);
		exit (1);
	}

	if (vflag)
		printf ("c: Usei READ (n = %d)\n", n);

	if (vflag)
		printf ("c: Texto recebido: \"%s\"\n", area);

	printf ("c: Texto recebido: \"%s\" (n = %d)\n", area, n);

	if (n != text_len || streq (area, text) == 0)
	{
		fprintf (stderr, "c: Erro no texto recebido: \"%s\"\n", area);
		exit (1);
	}

	exit (0);

}	/* end t */
