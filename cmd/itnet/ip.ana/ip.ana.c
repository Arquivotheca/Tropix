/*
 ****************************************************************
 *								*
 *			ip.ana.c				*
 *								*
 *	Analisa o arquivo contendo datagramas IP		*
 *								*
 *	Versão	2.3.0, de 16.09.91				*
 *		3.0.0, de 01.05.93				*
 *								*
 *	Módulo: ip.ana						*
 *		Utilitários de depuração			*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/common.h>
#include <sys/sync.h>
#include <sys/itnet.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <xti.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão:	3.0.0, de 01.05.93";

#define	LINESZ	16

#define	NOSTR	(char *)NULL
#define	NOVOID	(void *)NULL
#define	elif	else if
#define	EVER	;;
#define STR(x)	# x
#define XSTR(x)	STR (x)

entry int	vflag;		/* Verbose */
entry int	Nflag;		/* Nomes dos pgms no <stdin> */

/*
 ****** Tabela dos nomes dos protocolos *************************
 */
typedef struct
{
	int	t_proto;	/* O no. do protocolo */
	char	t_proto_nm[8];	/* O nome. do protocolo */

}	PROTO_TB;

const PROTO_TB	proto_tb[] =
{
	ICMP_PROTO,	"ICMP",
	TCP_PROTO,	"TCP",
	UDP_PROTO,	"UDP",
	0
};

/*
 ****** Tabela dos nomes dos controles TCP **********************
 */
typedef struct
{
	int	t_ctl;		/* O bit de controle */
	char	t_ctl_nm[8];	/* O nome do bit de controle */

}	CTL_TB;

const CTL_TB	ctl_tb[] =
{
	C_URG, 	"URG",
	C_ACK, 	"ACK",
	C_PSH, 	"PSH",
	C_RST, 	"RST",
	C_SYN, 	"SYN",
	C_FIN, 	"FIN",
	C_DATA,	"DATA",
	0
};

/*
 ****** Nomes das filas *****************************************
 */
typedef struct
{
	char	*q_queue_nm;
	int	q_ioctl_code;

}	QUEUE_TB;

const QUEUE_TB	queue_tb[] =
{
	"sort",		I_GET_TCP_SORTQ,
	"in",		I_GET_TCP_INQ,
	"out",		I_GET_TCP_OUTQ,
	"incom",	I_GET_TCP_INCOM,
	"listen",	I_GET_TCP_LISQ,
	"raw",		I_GET_RAW_INQ,
	"udp",		I_GET_UDP_INQ,
	NOSTR
};

/*
 ****** Protótipos de funções ***********************************
 */
void		help (void);
void		pr_datagram (IP_H *, int);
void		print_queue_datagrams (const QUEUE_TB *, int);
void		print_ip_header (IP_H *, int);
void		print_tcp_header (void *, int, int);
void		print_data_dump (void *, int);
const char	*get_proto_name (int);

extern int	getw (FILE *);

/*
 ****************************************************************
 *	Analisa o arquivo contendo datagramas IP		*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	int		opt, sz;
	FILE		*fp;
	char		*cp, *queue_nm = NOSTR;
	const QUEUE_TB	*qp;
	IP_H		*iph;
	const char	*file_nm;
	int		number = 0;
	const char	*str;
	int		port;
	char		buf[1024];

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "q:vNH")) != EOF)
	{
		switch (opt)
		{
		    case 'q':			/* Fila do endpoint */
			queue_nm = (char *)optarg;
			break;

		    case 'v':			/* Verbose */
			vflag++;
			break;

		    case 'N':			/* Nomes do <stdin> */
			Nflag++;
			break;

		    case 'H':			/* Help */
			help ();

		    default:			/* Erro */
			putc ('\n', stderr);
			help ();

		}	/* end switch */

	}	/* end while */

	argv += optind;
	argc -= optind;

	/*
	 *	Verifica a fonte dos datagramas
	 */
	if (queue_nm != NOSTR)
	{
		if (argc > 0)
			help ();

		if ((cp = strchr (queue_nm, ',')) == NOSTR)
		{
			fprintf
			(	stderr,
				"Não achei a ',' em \"%s\"\n",
				queue_nm
			);
			exit (1);
		}

		*cp++ = '\0';

		if ((port = strtol (cp, &str, 0)) <= 0 || *str)
		{
			fprintf
			(	stderr,
				"Port inválido: \"%s\"\n",
				cp
			);
			exit (1);
		}

		for (qp = queue_tb; /* abaixo */; qp++)
		{
			if (qp->q_queue_nm == NOSTR)
			{
				fprintf
				(	stderr,
					"Fila inválida: \"%s\"\n",
					queue_nm
				);
				exit (1);
			}

			if (streq (qp->q_queue_nm, queue_nm))
				break;

		}	/* for (tabela de filas) */

#define	DEBUG
#ifdef	DEBUG
		printf
		(	"Fila \"%s\", código de fila = ('%c', %d), port = %d\n",
			qp->q_queue_nm, qp->q_ioctl_code >> 8,
			qp->q_ioctl_code & 0x00FF, port
		);
#endif	DEBUG
		print_queue_datagrams (qp, port);

		exit (0);

	}	/* if (fila dada) */




	/*
	 *	A fonte de datagramas é um arquivo
	 */
	if ((file_nm = argv[0]) == NOSTR)
		file_nm = "itnet.debug";

	if ((fp = fopen (file_nm, "r")) == NOFILE)
	{
		fprintf (stderr, "Não consegui abrir \"%s\"\n", file_nm);
		perror (pgname);
		exit (1);
	}

	iph = (IP_H *)buf;

	for (EVER)
	{
		sz = getw (fp);

		if (feof (fp))
			exit (0);

		printf
		(	"---------------------- DATAGRAMA %3d, "
			"tamanho total = %3d bytes --------------\n",
			number++, sz
		);

		if (fread (buf, sz, 1, fp) != 1)
			{ printf ("EOF prematuro\n"); exit (1); }

		if (sz >= sizeof (IP_H) - sizeof (iph->ip_extension))
			pr_datagram (iph, 0 /* Não tem PDH */);
	}

}	/* end ip.ana */

/*
 ****************************************************************
 *	Imprime datagramas de uma fila de "endpoint"		*
 ****************************************************************
 */
void
print_queue_datagrams (const QUEUE_TB *qp, int port)
{
	int		fd, n;
	ITBLOCK		*bp;
	struct { ITBLOCK *q_q; long q_port; }	q_bl;
	ITBLOCK		itblock[100];
	int		number = 0;

	if ((fd = t_open ("/dev/itndaemon", O_RDWR, (T_INFO *)NULL)) < 0)
	{
		fprintf (stderr, "Não consegui inicializar a rede\n");
		perror (pgname);
		exit (1);
	}

	q_bl.q_q    = itblock;
	q_bl.q_port = port;

	if ((n = ioctl (fd, qp->q_ioctl_code, &q_bl)) < 0)
	{
		fprintf (stderr, "Erro no acesso à fila \"%s\"\n", qp->q_queue_nm);
		perror (pgname);
		exit (1);
	}

	printf ("%d\n", n); 

	for (bp = itblock; n > 0; bp++, n--)
	{
		printf
		(	"---------------------- DATAGRAMA %3d "
			"--------------\n",
			number++
		);

print_data_dump (bp, 48); printf ("\n");

		pr_datagram ((IP_H *)bp, 1 /* Tem PDH */);
	}

}	/* end print_queue_datagrams */

/*
 ****************************************************************
 *	Imprime dados sobre um datagrama			*
 ****************************************************************
 */
void
pr_datagram (IP_H *iph, int PDH)
{
	int		ip_h_size, count;
	void		*area;

	print_ip_header (iph, PDH);

	ip_h_size = iph->ip_h_size << 2;	/* Tamanho longs => bytes */

	area = (char *)iph + ip_h_size;
	count = iph->ip_size - ip_h_size;

	if (PDH)
	{
		PSD_H	*ph;

		ph = (PSD_H *)(area - sizeof (PSD_H));

		if (ph->ph_proto == TCP_PROTO)
			print_tcp_header (area, count, PDH);

	}
	else
	{
		if (iph->ip_proto == TCP_PROTO)
			print_tcp_header (area, count, PDH);
	}

	printf ("\n");

}	/* end pr_datagram */

/*
 ****************************************************************
 *	Imprime alguns dados sobre o cabeçalho IP		*
 ****************************************************************
 */
void
print_ip_header (IP_H *iph, int PDH)
{
	printf
	(	"h_size = %2d, id = %d, time_to_live = %d, proto = %d (%s)\n",
		iph->ip_h_size << 2,  iph->ip_id,  iph->ip_time_to_live, 
		iph->ip_proto, get_proto_name (iph->ip_proto)
	);

	if (PDH == 0)
	{
		printf ("src_addr = %s, ", t_addr_to_str (iph->ip_src_addr));
		printf ("dst_addr = %s\n", t_addr_to_str (iph->ip_dst_addr));
	}

}	/* end print_ip_header */

/*
 ****************************************************************
 *	Imprime alguns dados sobre o cabeçalho TCP		*
 ****************************************************************
 */
void
print_tcp_header (void *area, int count, int PDH)
{
	TT_H		*thp;
	const CTL_TB	*cp;
	int		tcp_h_size, data_cnt, flag, ctl;

	thp = (TT_H *)(area - sizeof (PSD_H));

	printf ("\n");

	if (PDH)
	{
		printf ("src_addr = %s, ", t_addr_to_str (thp->ph.ph_src_addr));
		printf ("dst_addr = %s\n", t_addr_to_str (thp->ph.ph_dst_addr));
	}

	printf ("src_port = %d, ", thp->th.th_src_port);
	printf ("dst_port = %d\n", thp->th.th_dst_port);

	tcp_h_size = thp->th.th_h_size << 2;

	data_cnt = count - tcp_h_size;

	ctl = thp->th.th_ctl;

	if (data_cnt > 0)
		ctl |= C_DATA;

	printf ("seq = %u", thp->th.th_seq_no);

	printf ("  ack = %u", thp->th.th_ack_no);

	printf ("  wnd = %u", thp->th.th_window);

	printf ("  (");

	for (cp = ctl_tb, flag = 0; cp->t_ctl != 0; cp++)
	{
		if (ctl & cp->t_ctl)
		{
			if (flag)
				printf ("|");

			printf (cp->t_ctl_nm); 	flag++;
		}
	}

	if (data_cnt > 0)
		printf ("=%d", data_cnt);

	printf (")\n");

	if (data_cnt > 0)
		print_data_dump (area + tcp_h_size, data_cnt);

}	/* end print_tcp_header */

/*
 ****************************************************************
 *	Obtém o nome do protocolo				*
 ****************************************************************
 */
const char *
get_proto_name (int proto)
{
	const PROTO_TB	*pp;

	for (pp = proto_tb; pp->t_proto != 0; pp++)
	{
		if (pp->t_proto == proto)
			return (pp->t_proto_nm);
	}

	return ("???");

}	/* end get_proto_name */

/*
 ****************************************************************
 *	Imprime uma ára de dados (no formato de "xd")		*
 ****************************************************************
 */
void
print_data_dump (void *area, int count)
{
	int		n, i, start = 0;
	char		*dump = area, *cp;

	printf ("\n");

	for (EVER)
	{
		n = (count >= LINESZ ? LINESZ : count);

		count -= LINESZ;

		if (n <= 0)
			break;

		printf ("%P:   ", start);

		for (i = 0; i < LINESZ; i++)
		{
			if ((i & 1) == 0)
				printf (" ");

			if (i == LINESZ/2)
				printf (" ");

			if (i < n)
				printf ("%02X", dump[i]);
			else
				printf ("  ");
		}

		printf ("     ");

		/*
		 *	Escreve caracteres em ascii
		 */
		for
		(	cp = dump, i = 0;
			i < LINESZ && i < n;
			cp++, i++
		)
		{
			int	c;

			c = *cp;
			if (c < 0x20)
				c = '.';
			putchar (c);
		}

		putchar ('\n');

		/*
		 *	Se o endereço é multiplo de 128, pula uma linha
		 */
		start += n;

		dump += n;

		if ((start & 127) == 0)
			putchar ('\n');

	}	/* end for (EVER) */

}	/* end print_data_dump */

/*
 ****************************************************************
 *	Resumo de utilização do programa			*
 ****************************************************************
 */
void
help (void)
{
	fprintf
	(	stderr,
		"%s - analisa datagramas UDP/TCP/IP\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-v] [-q <fila>,<port>] [<arquivo>]\n",
		pgname, pgversion, pgname
	);
	fprintf
	(	stderr,
		"\nOpções:"
		"\t--: Analisa datagramas do <arquivo> (ou de \"itnet.debug\" se não dado)\n"
		"\t-q: Analisa datagramas da <fila> do <port> dado\n"
		"\t-v: Verboso\n"
	);

	exit (2);

}	/* end help */
