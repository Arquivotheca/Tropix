#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xti.h>
#include <fcntl.h>

#define	EVER	;;

entry const char *pgname;	/* Nome do programa */

entry int	vflag;		/* Verbose */

const char	dev[] = "/dev/itntcp";

char		*edit_itnet_flag (int);

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
			execl ("uc", "uc",  "-v", serv_port_nm, NOSTR);
		else
			execl ("uc",  "uc", serv_port_nm, NOSTR);

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

	/*
	 *	x
	 */
	for (EVER)
	{
		memset (area, 0, sizeof (area));

		if ((n = t_rcv (connfd, area, sizeof (area), &flag)) < 0)
		{
			fprintf (stderr, "Erro no RCV\n");
			perror (pgname);
			exit (1);
		}

		if (n == 0)
			break;

		printf
		(	"Recebi %d bytes: \"%s\" (flag = %s)\n",
			n, area, edit_itnet_flag (flag)
		);
	}

#if (0)	/*******************************************************/
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
#endif	/*******************************************************/

	/*
	 *	Termina a conexão
	 */
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

/*
 ****************************************************************
 *	Tabela de indicadores					*
 ****************************************************************
 */
typedef struct itnet_flag_tb
{
	const char	ft_nm[12]; 
	const int	ft_flag; 

}	ITNET_FLAG_TB;

const ITNET_FLAG_TB itnet_flag_tb[] =
{
	"MORE",		0x0001,	/* A mensagem continua */
	"EXPEDITED",	0x0002,	/* Mensagem expedita */
	"NEGOTIATE",	0x0004,	/* Especificar opções */
	"CHECK",	0x0008,	/* Verificar opções */
	"DEFAULT",	0x0010,	/* Utilizar os defaults nas opções */
	"SUCCESS",	0x0020,	/* Sucesso */
	"FAILURE",	0x0040,	/* Erro */
	"PUSH",		0x0100,	/* PUSH do TCP */
	"URGENT",	0x0200,	/* Segmento urgente do TCP */
	"NO_CHECKSUM",	0x0400,	/* Datagrama UDP recebido sem "checksum" */
	"",		0
};

/*
 ****************************************************************
 *	Obtém o nome do indicador				*
 ****************************************************************
 */
char *
edit_itnet_flag (int flag)
{
	register const ITNET_FLAG_TB	*fp;
	char				not_first;
	static char			line[80];

	not_first = 0; line[0] = '\0';

	for (fp = itnet_flag_tb; fp->ft_flag != 0; fp++)
	{
		if (fp->ft_flag & flag)
		{
			if (not_first++)
				strcat (line, "|");

			strcat (line, fp->ft_nm);
		}
	}

	return (line);

}	/* end edit_itnet_flag */
