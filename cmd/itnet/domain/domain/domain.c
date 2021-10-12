/*
 ****************************************************************
 *								*
 *			domain.c				*
 *								*
 *	Teste de obtenção de um endereço de um servidor		*
 *								*
 *	Versão	3.0.0, de 10.08.93				*
 *		3.0.0, de 10.08.93				*
 *								*
 *	Módulo: ITNET						*
 *		Utilitários de comunicação			*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/common.h>
#include <sys/sync.h>
#include <sys/syscall.h>
#include <sys/itnet.h>

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#if (0)	/*************************************/
#include <sys/types.h>
#include <sys/times.h>
#include <sys/itnet.h>

#endif	/*************************************/

#include <xti.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão:	3.0.0, de 10.08.93";

#define	DOMAIN_PORT	53

#define	EVER	;;
#define	NOSTR   (char *)NULL
#define	elif	else if
#define STR(x)	# x
#define XSTR(x)	STR (x)

typedef unsigned int	bit;

/*
 ******	Verifica se as macro variáveis estão definidas **********
 */
#if	!defined (BIG_ENDIAN) && !defined (LITTLE_ENDIAN)
#error	"Uma das macrovariáveis \"BIG/LITTLE_ENDIAN\" deve estar definida"
#endif	BIG_ENDIAN/LITTLE_ENDIAN

/*
 ******	Definições relativas à ordem de bytes da CPU ************
 */
#ifdef	LITTLE_ENDIAN
#define	ENDIAN_LONG(x)	long_endian_cv (x)
#define	ENDIAN_SHORT(x)	short_endian_cv (x)
#else
#define	ENDIAN_LONG(x)	(x)
#define	ENDIAN_SHORT(x)	(x)
#endif	LITTLE_ENDIAN

/*
 ****** Opções e parâmetros *************************************
 */
entry int	wflag;		/* Grave a resposta obtida */
entry int	mflag;		/* Mail */
entry int	rflag;		/* Reverso */
entry int	vflag;		/* Verbose */
entry int	dflag;		/* Debug */

entry const char *name_server_node_nm; /* Nome do nó remoto  */

entry long	name_server_addr;	/* Endereço do nó remoto  */

entry const char *desired_name;

entry char	*area_ptr;	/* para "get_domain_name" */

/*
 ****** Variáveis globais ***************************************
 */
const char	udp_dev[] = "/dev/itnudp";

int		udp_fd;		/* fd do UDP */

const char	raw_dev[] = "/dev/itnraw";

int		raw_fd;		/* fd do RAW */

const char	answer_file_nm[] = "answer";

/*
 ****** Header Section Format ***********************************
 */
typedef struct
{
	ushort	h_id;		/* Identificação */

#ifdef	LITTLE_ENDIAN
	bit	h_rd : 1;	/* Recursion desired */
	bit	h_tc : 1;	/* Truncation */
	bit	h_aa : 1;	/* Authorative Answer */
	bit	h_op : 4;	/* Kind of Query */
	bit	h_qr : 1;	/* Query or response */

	bit	h_rc : 4;	/* Response code */
	bit	h_z  : 3;	/* (reserved) */
	bit	h_ra : 1;	/* Recursion available */
#else
	bit	h_qr : 1;	/* Query or response */
	bit	h_op : 4;	/* Kind of Query */
	bit	h_aa : 1;	/* Authorative Answer */
	bit	h_tc : 1;	/* Truncation */
	bit	h_rd : 1;	/* Recursion desired */

	bit	h_ra : 1;	/* Recursion available */
	bit	h_z  : 3;	/* (reserved) */
	bit	h_rc : 4;	/* Response code */
#endif	LITTLE_ENDIAN

	ushort	h_qdcnt;	/* No. de entradas */
	ushort	h_ancnt;	/* No. de RRs de resposta */
	ushort	h_nscnt;	/* No. de RRs de autoridade */
	ushort	h_arcnt;	/* No. de RRs adicionais */

}	H;

#if (0)	/*************************************/
/*
 ****** Resource record Format **********************************
 */
typedef struct
{
	ushort	r_type;		/* Tipo */
	ushort	r_class;	/* Classe */
	long	r_ttl;		/* Tempo útil */
	ushort	r_len;		/* Tamanho do dado */
	ushort	r_data;		/* Dado (começo) */

}	RR;
#endif	/*************************************/

/*
 ****** Tabela com os nomes dos tipos do DOMAIN *****************
 */
typedef struct
{
	char	t_code[8];	/* Código do tipo */
	char	*t_name;	/* Descrição extensa do tipo */

}	TYPE_DESC;

const TYPE_DESC	type_desc[] =
{
	/*  0 */   "",		"Type ZERO",
	/*  1 */   "A",		"Host address",
	/*  2 */   "NS",	"Authoritative name server",
	/*  3 */   "ND",	"Mail destination (obsolete)",
	/*  4 */   "MF",	"Mail forwarder (obsolete)",
	/*  5 */   "CNAME",	"Canonical name for an alias",
	/*  6 */   "SOA",	"Start of a zone of authority",
	/*  7 */   "MB",	"Mailbox domain name (experimental)",
	/*  8 */   "MG",	"Mail group member (experimental)",
	/*  9 */   "MR",	"Mail rename domain name (experimental)",
	/* 10 */   "NULL",	"NULL RR (experimental)",
	/* 11 */   "WKS",	"Well known service description",
	/* 12 */   "PTR",	"Domain name pointer",
	/* 13 */   "HINFO",	"Host information",
	/* 14 */   "MINFO",	"Mailbox or mail list information",
	/* 15 */   "MX",	"Mail exchange",
	/* 16 */   "TXT",	"Text strings",
	/*    */   "",		NOSTR
};

enum TYPE_CODE
{
	/*  1 */   TYPE_A = 1,	/* Host address */
	/*  2 */   TYPE_NS,	/* Authoritative name server */
	/*  3 */   TYPE_ND,	/* Mail destination (obsolete) */
	/*  4 */   TYPE_MF,	/* Mail forwarder (obsolete) */
	/*  5 */   TYPE_CNAME,	/* Canonical name for an alias */
	/*  6 */   TYPE_SOA,	/* Start of a zone of authority */
	/*  7 */   TYPE_MB,	/* Mailbox domain name (experimental) */
	/*  8 */   TYPE_MG,	/* Mail group member (experimental) */
	/*  9 */   TYPE_MR,	/* Mail rename domain name (experimental) */
	/* 10 */   TYPE_NULL,	/* NULL RR (experimental) */
	/* 11 */   TYPE_WKS,	/* Well known service description */
	/* 12 */   TYPE_PTR,	/* Domain name pointer */
	/* 13 */   TYPE_HINFO,	/* Host information */
	/* 14 */   TYPE_MINFO,	/* Mailbox or mail list information */
	/* 15 */   TYPE_MX,	/* Mail exchange */
	/* 16 */   TYPE_TXT	/* Text strings */
};

/*
 ****** Tabela com os nomes das classes do DOMAIN ***************
 */
typedef struct
{
	char	c_code[8];	/* Código da classe */
	char	*c_name;	/* Descrição extensa da classe */

}	CLASS_DESC;

const CLASS_DESC class_desc[] =
{
	/*  0 */   "",		"Class ZERO",
	/*  1 */   "IN",	"Internet",
	/*  2 */   "CS",	"CSNET class (obsolete)",
	/*  3 */   "CH",	"CHAOS class",
	/*  4 */   "HS",	"HESIOD class",
	/*    */   "",		NOSTR
};

enum CLASS_CODE
{
	/*  1 */   CLASS_IN = 1,/* Internet */
	/*  2 */   CLASS_CS,	/* CSNET class (obsolete) */
	/*  3 */   CLASS_CH,	/* CHAOS class */
	/*  4 */   CLASS_HS	/* HESIOD class */
};

/*
 ****** Estrutura de datagrama RAW ******************************
 */
typedef struct
{
	ICMP_H	h;	/* Cabeçalho ICMP */

}	DG;

/*
 ****** Tabela com os nomes das mensagem ICMP *******************
 */
const char	*msg_type[] =
{
	/*  0 */	"Echo reply",
	/*  1 */	NOSTR,
	/*  2 */	NOSTR,
	/*  3 */	"Destination unreachable",
	/*  4 */	"Source quence",
	/*  5 */	"Redirect",
	/*  6 */	NOSTR,
	/*  7 */	NOSTR,
	/*  8 */	"Echo",
	/*  9 */	NOSTR,
	/* 10 */	NOSTR,
	/* 11 */	"Time exceeded",
	/* 12 */	"Parameter problem",
	/* 13 */	"Timestamp",
	/* 14 */	"Timestamp reply",
	/* 15 */	"Information request",
	/* 16 */	"Information reply"
};

/*
 ****** Tabela com os nomes dos códigos ICMP ********************
 */
const char	*msg_DST_UNREACH[] =
{
	/*  0 */	"Net unreachable",
	/*  1 */	"Host unreachable",
	/*  2 */	"Protocol unreachable",
	/*  3 */	"Port unreachable",
	/*  4 */	"Fragmentation needed and DF set",
	/*  5 */	"Source route failed"
};

const char	*msg_REDIRECT[] =
{
	/*  0 */	"Redirect datagrams for the network",
	/*  1 */	"Redirect datagrams for the host",
	/*  2 */	"Redirect datagrams for the type of service and network",
	/*  3 */	"Redirect datagrams for the type of service and host"
};

const char	*msg_TIME_EXCEEDED[] =
{
	/*  0 */	"Time to live exceeded in transit",
	/*  1 */	"Fragment reassembly time exceeded"
};

/*
 ****** Protótipos de funções ***********************************
 */
void		send_req (void);
void		receive_answer (void);
void		put_domain_name (const char *name, char **area);
void		read_debug (void);
void		read_answer (const char *);
void		write_answer (const void *, int);
void		analyse (char *);
void		print_resource_record (const char *header, register const char **cp);
char		*get_type_name (register int type);
char		*get_class_name (register int class);
void		analyse_unexpected_datagrams (register const DG *dp, long rcv_addr);
char		*h_op_text (int);
char		*h_rc_text (int);
char		*get_domain_name (const char **cpp);
int		get_short (const char **cpp);
void		put_short (int value, char **cpp);
long		get_long (const char **cpp);
void		help (void);

/*
 ****************************************************************
 *	Teste de obtenção de um endereço de um servidor		*
 ****************************************************************
 */
int
main (int argc, register const char *argv[])
{
	register int	opt;
	INADDR		addr;
	T_BIND		bind;
	const char	*file_nm = NOSTR;

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "wmrf:n:vH")) != EOF)
	{
		switch (opt)
		{
		    case 'w':			/* Grave a resposta */
			wflag++;
			break;

		    case 'm':			/* Mail */
			mflag++;
			break;

		    case 'r':			/* Reverso */
			rflag++;
			break;

		    case 'f':			/* De um arquivo */
			file_nm = optarg;
			break;

#if (0)	/*************************************/
		    case 'n':			/* No. de datagramas */
			n_str = optarg;
			break;
#endif	/*************************************/

		    case 'v':			/* Verbose */
			vflag++;
			break;

		    case 'd':			/* Debug */
			dflag++;
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
	 *	Verifica se é para ler um arquivo
	 */
	if (file_nm != NOSTR)
		{ read_answer (file_nm); exit (0); }

	/*
	 *	x
	 */
	if (argc == 0)
		{ read_debug (); exit (0); }

	if (argc > 2)
		help ();

	desired_name = argv[0];
	name_server_node_nm = "146.164.10.2";		/* PROVISÓRIO */
#if (0)	/*******************************************************/
	name_server_node_nm = "200.17.63.121";		/* PROVISÓRIO */
#endif	/*******************************************************/

	if (argc == 2)
		name_server_node_nm = argv[1];

	if (vflag)
	{
		printf
		(	"desired_name = \"%s\", name_server_node_nm = \"%s\"\n",
			desired_name, name_server_node_nm
		);
	}



#if (0)	/*************************************/
#define	DEBUG
#endif	/*************************************/
#ifdef	DEBUG
	{
		char		area[256];
		char		*str = area;

		put_domain_name (desired_name, &str);

		str = area;

		printf
		(	"Nome = \"%s\"\n",
			get_domain_name (&str)
		);
		exit (0);
	}
#endif	DEBUG


#if (0)	/*************************************/
	/*
	 *	Examina o número de datagramas dado
	 */
	if (n_str != NOSTR && ((n_dg = strtol (n_str, &str, 0)) < 1 || *str != '\0'))
	{
		fprintf (stderr, "%s: No. de datagramas inválido: \"%s\"\n", pgname, n_str);
		exit (1);
	}

	if (n_dg > N_DG_MAX)
	{
		fprintf
		(	stderr,
			"%s: No. de datagramas máximo: " XSTR (N_DG_MAX) "\n",
			pgname
		);
		n_dg = N_DG_MAX;
	}
#endif	/*************************************/


	/*
	 *	Abre os "endpoint"s
	 */
	if ((udp_fd = t_open (udp_dev, O_RDWR, (T_INFO *)NULL)) < 0)
	{
		fprintf (stderr, "Não consegui abrir \"%s\"\n", udp_dev);
		perror (pgname);
		exit (1);
	}

	if ((raw_fd = t_open (raw_dev, O_RDWR, (T_INFO *)NULL)) < 0)
	{
		fprintf (stderr, "Não consegui abrir \"%s\"\n", raw_dev);
		perror (pgname);
		exit (1);
	}

	/*
	 *	Obtém o endereço do nó remoto
	 */
	if ((name_server_addr = t_node_to_addr (udp_fd, name_server_node_nm, (long *)NULL)) == -1)
	{
		fprintf
		(	stderr,
			"%s: Não consegui obter o endereço de \"%s\" (%s)\n",
			pgname, name_server_node_nm, strerror (errno)
		);
		exit (1);
	}

	if (vflag)
		printf ("name_server_addr dado: %08X (%s)\n", name_server_addr, name_server_node_nm);

	/*
	 *	Associa um endereço UDP
	 */
	bind.addr.len	 = 0;
	bind.addr.maxlen = sizeof (INADDR);
	bind.addr.buf	 = &addr;

	if (t_bind (udp_fd, &bind, &bind) < 0)
	{
		fprintf (stderr, "Erro no T_BIND UDP\n");
		perror (pgname);
		exit (1);
	}

	if (vflag)
		fprintf (stderr, "Port local UDP = %d\n", addr.a_port);
	
	/*
	 *	Associa um endereço RAW
	 */
	addr.a_addr  = 0;	/* A consistência do endereço é feita abaixo */
	addr.a_proto = ICMP_PROTO;

	bind.addr.len	 = sizeof (INADDR);
	bind.addr.maxlen = sizeof (INADDR);
	bind.addr.buf	 = &addr;

	if (t_bind (raw_fd, &bind, &bind) < 0)
	{
		fprintf (stderr, "Erro no T_BIND RAW\n");
		perror (pgname);
		exit (1);
	}

	/*
	 *	Envia o pedido
	 */
	send_req ();

	/*
	 *	Espera a resposta
	 */
	receive_answer ();

	return (0);

}	/* end domain */

/*
 ****************************************************************
 *	Envia o pedido						*
 ****************************************************************
 */
void
send_req (void)
{
	T_UNITDATA	unitdata;
	INADDR		addr;
	struct { H h; char c[256]; }  dg;
	char		area[128];
	char		*str;
	int		sz;
	static int	id;
	long		a;
	char		*cp = (char *)&a;

	/*
	 *	Prepara o datagrama
	 */
	memset (&dg, 0, sizeof (dg));

	dg.h.h_id = ENDIAN_SHORT (++id);
   /***	dg.h.h_qr = 0;	***/
   /***	dg.h.h_op = 0;	***/
   /***	dg.h.h_aa = 0;	***/
   /***	dg.h.h_tc = 0;	***/
	dg.h.h_rd = 1;
   /***	dg.h.h_ra = 0;	***/
   /***	dg.h.h_z  = 0;	***/
   /***	dg.h.h_rc = 0;	***/
	dg.h.h_qdcnt = ENDIAN_SHORT (1);
   /***	dg.h.h_ancnt = 0;	***/
   /***	dg.h.h_nscnt = 0;	***/
   /***	dg.h.h_arcnt = 0;	***/

	str = dg.c;

	/*
	 *	Compõe o nome reverso, se for o caso
	 */
	if (rflag)
	{
		if ((a = t_str_to_addr (desired_name)) == -1)
		{
			fprintf (stderr, "Endereço inválido\n");
			exit (1);
		}

#ifdef	LITTLE_ENDIAN
		sprintf (area, "%u.%u.%u.%u.IN-ADDR.ARPA", cp[0], cp[1], cp[2], cp[3]);
#else
		sprintf (area, "%u.%u.%u.%u.IN-ADDR.ARPA", cp[3], cp[2], cp[1], cp[0]);
#endif	LITTLE_ENDIAN

		desired_name = area;
	}

	put_domain_name (desired_name, &str);

	if   (rflag)
		put_short (12, &str);	/* Type = PTR */
	elif (mflag)
		put_short (15, &str);	/* Type = MX */
	else
		put_short (1, &str);	/* Type = A */

	put_short (1, &str);	/* Class = IN */

	sz = str - (char *)&dg;

	if (vflag)
		printf ("dg sz = %d\n", sz);

	/*
	 *	Manda o datagrama UDP
	 */
	addr.a_addr = name_server_addr;
	addr.a_port = DOMAIN_PORT;

	unitdata.addr.len	= sizeof (addr);
   	unitdata.addr.buf	= &addr;

	unitdata.udata.len	= sz;
	unitdata.udata.buf	= &dg;

	if (t_sndudata (udp_fd, &unitdata) < 0)
	{
		fprintf (stderr, "Erro no SNDUDP\n");
		perror (pgname);
		exit (1);
	}

}	/* end send_req */

/*
 ****************************************************************
 *	Pai: recebe os pedidos de ECHO				*
 ****************************************************************
 */
void
receive_answer (void)
{
	T_UNITDATA	unitdata;
	char 		area[512 + 128];
	int		flag;
	INADDR		rcv_addr;
	int		fd_vec[2], i = -1;

	/*
	 *	Espera um datagrama UDP ou RAW
	 */
    again:
	fd_vec[0] = udp_fd;	 fd_vec[1] = raw_fd;

	if ((i = attention (2, fd_vec, i, 0)) < 0)
	{
		fprintf (stderr, "Erro no ATTENTION\n");
		perror (pgname);
		exit (1);
	}

	/*
	 *	Recebe os datagramas UDP ou RAW
	 */
	switch (i)
	{
		/*
		 *	UDP
		 */
	    case 0:	
		unitdata.addr.maxlen	= sizeof (rcv_addr);
		unitdata.addr.buf	= &rcv_addr;

		unitdata.udata.maxlen	= sizeof (area);
		unitdata.udata.buf	= area;

		if (t_rcvudata (udp_fd, &unitdata, &flag) < 0)
		{
			fprintf (stderr, "Erro no RCVUDP\n");
			perror (pgname);
			exit (1);
		}

		printf
		(	"Recebi %d bytes de resposta (%s checksum)\n",
			unitdata.udata.len,
			flag & T_NO_CHECKSUM ? "sem" : "com"
		);

		if (wflag)
			write_answer (area, unitdata.udata.len);

		analyse (area);

		break;

		/*
		 *	RAW
		 */
	    case 1:	
		unitdata.addr.maxlen	= sizeof (rcv_addr);
		unitdata.addr.buf	= &rcv_addr;

		unitdata.udata.maxlen	= sizeof (area);
		unitdata.udata.buf	= area;

		if (t_rcvraw (raw_fd, &unitdata, &flag) < 0)
		{
			fprintf (stderr, "Erro no RCVRAW\n");
			perror (pgname);
			exit (1);
		}

		if (rcv_addr.a_addr != name_server_addr)
		{
			fprintf (stderr, "Recebi datagrama ICMP de endereço errado\n");
			goto again;
		}

		analyse_unexpected_datagrams ((DG *)area, rcv_addr.a_addr);

		break;

		/*
		 *	Valor inválido
		 */
	    default:
		fprintf (stderr, "Valor %d inválido do attention\n", i);
		exit (1);

	}	/* end switch */

}	/* end receive_answer */

/*
 ****************************************************************
 *	Compõe um nome no formato do "domain"			*
 ****************************************************************
 */
void
put_domain_name (const char *name, char **area)
{
	register const  char	*np = name;
	register char	c;
	register int	n = 0;
	register char	*ap = *area + 1;
	register char	*bp = *area;

	for (EVER)
	{
		*ap++ = c = *np++;

		if (c == '.' || c == '\0')
		{
			*bp = n;
			n = 0;
			bp = ap - 1;

			if (c == '\0')
			{
				*area = ap; return;
			}
		}
		else
		{
			n++;
		}
	}

}	/* end put_domain_name */

/*
 ****************************************************************
 *	Lê o arquivo de DEBUG					*
 ****************************************************************
 */
void
read_debug (void)
{
	register int	fd, sz;
	char		area[512+128];

	/*
	 *	Abre o "debug"
	 */
	if ((fd = open ("itnet.debug", O_RDONLY)) < 0)
	{
		fprintf (stderr, "Não consegui abrir \"itnet.debug\"\n");
		perror (pgname);
		exit (1);
	}

	/*
	 *	Lê o "debug"	
	 */
	if ((sz = read (fd, area, sizeof (area))) < 0)
	{
		fprintf (stderr, "Não consegui ler \"itnet.debug\"\n");
		perror (pgname);
		exit (1);
	}

	if (vflag)
		printf ("Li %d bytes\n", sz);

	analyse (area + 4 + 20 + 8);

}	/* end read_debug */

/*
 ****************************************************************
 *	Le uma resposta de um arquivo				*
 ****************************************************************
 */
void
read_answer (const char *file_nm)
{
	register int	fd, n;
	char		area[BLSZ];

	if ((fd = open (file_nm, O_RDONLY)) < 0)
	{
		fprintf
		(	stderr,
			"%s: Não consegui abrir \"%s\" (%s)\n",
			pgname, file_nm, strerror (errno)
		);
		return;
	}

	if ((n = read (fd, area, BLSZ)) <= 0)
	{
		fprintf
		(	stderr,
			"%s: Não consegui ler \"%s\" (%s)\n",
			pgname, file_nm, strerror (errno)
		);
		return;
	}

	analyse (area);

}	/* end read_answer */

/*
 ****************************************************************
 *	Grava a resposta					*
 ****************************************************************
 */
void
write_answer (const void *area, int count)
{
	register int	fd;

	if (access (answer_file_nm, F_OK) >= 0)
	{
		fprintf (stderr, "O arquivo \"%s\" já existe. Reescreve? (n): ", answer_file_nm);

		if (askyesno () <= 0)
			return;
	}

	if ((fd = creat (answer_file_nm, 0666)) < 0)
	{
		fprintf
		(	stderr,
			"%s: Não consegui criar \"%s\" (%s)\n",
			pgname, answer_file_nm, strerror (errno)
		);
		return;
	}

	if (write (fd, area, count) != count)
	{
		fprintf
		(	stderr,
			"%s: Não consegui escrever \"%s\" (%s)\n",
			pgname, answer_file_nm, strerror (errno)
		);
		return;
	}

}	/* end write_answer */

/*
 ****************************************************************
 *	Analisa a resposta					*
 ****************************************************************
 */
void
analyse (char *area)
{
	register H		*hp = (H *)area;
	register int		i;
	const char		*cp = area + sizeof (H);

	/*
	 *	Prepara o ponteiro para os nomes
	 */
	area_ptr = area;

	/*
	 *	Converte o ENDIAN, se for o caso
	 */
#ifdef	LITTLE_ENDIAN
	hp->h_id    = ENDIAN_SHORT (hp->h_id);
	hp->h_qdcnt = ENDIAN_SHORT (hp->h_qdcnt);
	hp->h_ancnt = ENDIAN_SHORT (hp->h_ancnt);
	hp->h_nscnt = ENDIAN_SHORT (hp->h_nscnt);
	hp->h_arcnt = ENDIAN_SHORT (hp->h_arcnt);
#endif	LITTLE_ENDIAN

	/*
	 *	Analisa o "header"
	 */
	printf ("HEADER (id = %d):\n\n", hp->h_id);

	printf (hp->h_qr ? "Response, " : "Query, ");

	printf ("%s, ", h_op_text (hp->h_op));	/* Kind of Query */

	printf (hp->h_aa ? "authoritative, " : "NOT authoritative, ");
	printf (hp->h_tc ? "truncated\n" : "NOT truncated\n");

	printf (hp->h_rd ? "recursion desired, " : "NO recursion desired, ");
	printf (hp->h_ra ? "recursion available, " : "NO recursion available, ");

	printf ("%s\n", h_rc_text (hp->h_rc));	/* Response code */

	printf ("Entries: ");
	printf ("question = %d, ", hp->h_qdcnt);
	printf ("answer = %d, ", hp->h_ancnt);
	printf ("authority = %d, ", hp->h_nscnt);
	printf ("additional = %d\n", hp->h_arcnt);

	/*
	 *	Verifica se os números são razoáveis ...
	 */
	if
	(	hp->h_qdcnt > 20 || hp->h_ancnt > 20 ||
		hp->h_nscnt > 20 || hp->h_arcnt > 20
	)
	{
		fprintf 
		(
			stderr,
			"\n%s: Formato inválido (?)\n",
			pgname
		);
		exit (1);
	}

	/*
	 *	Analisa a seção de perguntas
	 */
	for (i = 0; i < hp->h_qdcnt; i++)
	{
		printf ("\nQuestion name = %s, ", get_domain_name (&cp));
		printf ("qtype = %s, ", get_type_name (get_short (&cp)));
		printf ("qclass = %s\n", get_class_name (get_short (&cp)));
	}

	/*
	 *	Analisa a seção de respostas
	 */
	for (i = 0; i < hp->h_ancnt; i++)
	{
		print_resource_record ("Answer name", &cp);
	}

	/*
	 *	Analisa a seção autoritativa
	 */
	for (i = 0; i < hp->h_nscnt; i++)
	{
		print_resource_record ("Authority", &cp);
	}

	/*
	 *	Analisa a seção adcional
	 */
	for (i = 0; i < hp->h_arcnt; i++)
	{
		print_resource_record ("Additional", &cp);
	}

}	/* end analyse */

/*
 ****************************************************************
 *	Imprime um RR						*
 ****************************************************************
 */
void
print_resource_record (const char *header, const register char **cp)
{
	register int	type, class, len;
	const char	*old_cp;

	printf ("\n%s = %s, ", header, get_domain_name (cp));
	printf ("rtype = %s, ", get_type_name (type = get_short (cp)));
	printf ("rclass = %s\n", get_class_name (class = get_short (cp)));
	printf ("ttl = %d, ", get_long (cp));
	printf ("len = %d, ", len = get_short (cp));

	old_cp = *cp;

	if   (type == TYPE_A && class == CLASS_IN)
	{
		printf ("Addr = %s\n", t_addr_to_str (get_long (cp)));
	}
	elif (type == TYPE_NS && class == CLASS_IN)
	{
		printf ("Name = %s\n", get_domain_name (cp));
	}
	elif (type == TYPE_CNAME && class == CLASS_IN)
	{
		printf ("Canonical name = %s\n", get_domain_name (cp));
	}
	elif (type == TYPE_MX && class == CLASS_IN)
	{
		printf ("preference = %d, ", get_short (cp));
		printf ("Name = %s\n", get_domain_name (cp));
	}
	elif (type == TYPE_PTR && class == CLASS_IN)
	{
		printf ("Name = %s\n", get_domain_name (cp));
	}

	*cp = old_cp + len;

#if (0)	/*******************************************************/
	else
		{ printf ("len = %d\n", len); *cp += len; }
#endif	/*******************************************************/

}	/* end print_resource_record */

/*
 ****************************************************************
 *	Obtém a descrição do TIPO				*
 ****************************************************************
 */
char *
get_type_name (register int type)
{
	static char	area[24];

	if ((unsigned)type >= sizeof (type_desc) / sizeof (TYPE_DESC) - 1)
	{
		sprintf (area, "Unknown type: %d", type);
		return (area);
	}

	return (type_desc[type].t_name);

}	/* end get_type_name */

/*
 ****************************************************************
 *	Obtém a descrição da CLASSE				*
 ****************************************************************
 */
char *
get_class_name (register int class)
{
	static char	area[24];

	if ((unsigned)class >= sizeof (class_desc) / sizeof (TYPE_DESC) - 1)
	{
		sprintf (area, "Unknown class: %d", class);
		return (area);
	}

	return (class_desc[class].c_name);

}	/* end get_type_name */

/*
 ****************************************************************
 *	Foi recebido um datagrama de tipo inesperado		*
 ****************************************************************
 */
void
analyse_unexpected_datagrams (register const DG *dp, long rcv_addr)
{
	register const char	*cp;

	/*
	 *	Imprime o tipo
	 */
	if (dp->h.ih_type < (sizeof (msg_type) / sizeof (char *)))
		cp = msg_type[dp->h.ih_type];
	else
		cp = NOSTR;

	if (cp == NOSTR)
	{
		fprintf
		(	stderr,
			"%s: Recebi um datagrama ICMP (de %s) de tipo desconhecido (%d)\n",
			pgname, t_addr_to_str (rcv_addr), dp->h.ih_type
		);
		return;
	}

	fprintf
	(	stderr,
		"%s: Recebi um datagrama ICMP (de %s): \"%s\"",
		pgname, t_addr_to_str (rcv_addr), cp
	);

	/*
	 *	Imprime o código
	 */
	cp = NOSTR;

	switch (dp->h.ih_type)
	{
	    case ICMP_DST_UNREACH:
		if (dp->h.ih_code < (sizeof (msg_DST_UNREACH) / sizeof (char *)))
			cp = msg_DST_UNREACH[dp->h.ih_code];
		else
			cp = "*";
		break;

	    case ICMP_REDIRECT:
		if (dp->h.ih_code < (sizeof (msg_REDIRECT) / sizeof (char *)))
			cp = msg_REDIRECT[dp->h.ih_code];
		else
			cp = "*";
		break;

	    case ICMP_TIME_EXCEEDED:
		if (dp->h.ih_code < (sizeof (msg_TIME_EXCEEDED) / sizeof (char *)))
			cp = msg_TIME_EXCEEDED[dp->h.ih_code];
		else
			cp = "*";
		break;

	}	/* end switch */

	if   (cp == NOSTR)
		fprintf (stderr, "\n");
	elif (*cp == '*')
		fprintf (stderr, " (código %d desconhecido)\n", dp->h.ih_code);
	else
		fprintf (stderr, " (%s)\n", cp);

} 	/* end analyse_unexpected_datagrams */

/*
 ****************************************************************
 *	Decodifica a operação do cabeçalho			*
 ****************************************************************
 */
char *
h_op_text (int op)
{
	switch (op)
	{
	    case 0:
		return ("standard query");

	    case 1:
		return ("inverse query");

	    case 2:
		return ("server status request");
	}

	return ("reserved");

}	/* end h_op_text */

/*
 ****************************************************************
 *	Decodifica o código de retorno				*
 ****************************************************************
 */
char *
h_rc_text (int op)
{
	switch (op)
	{
	    case 0:
		return ("NO error");

	    case 1:
		return ("format error");

	    case 2:
		return ("server failure");

	    case 3:
		return ("name error");

	    case 4:
		return ("NOT implemented");

	    case 5:
		return ("refused");
	}

	return ("unknown error");

}	/* end h_rc_text */

/*
 ****************************************************************
 *	Obtém um nome de domínio				*
 ****************************************************************
 */
char *
get_domain_name (const char **cpp)
{
	register const char	*cp = *cpp;
	register int		i;
	static char		area[256];
	register char		*ap = area;
	char			updated_ptr = 0;

	/*
	 *	Retira o nome
	 */
	while (*cp != 0)
	{
		if ((*cp & 0xC0) == 0xC0)
		{
			if (!updated_ptr)
				{ *cpp = cp + 2; updated_ptr = 1; }

			cp = area_ptr + (((cp[0] & ~0xC0) << 8) | cp[1]);
		}

		for (i = *cp++; i > 0; i--)
			*ap++ = *cp++;

		if (*cp != 0)
			*ap++ = '.';
	}

	*ap++ = '\0';

	if (!updated_ptr)
		*cpp = cp + 1;

	return (area);

}	/* end get_domain_name */

/*
 ****************************************************************
 *	Obtém um "short" desalinhado				*
 ****************************************************************
 */
int
get_short (register const char **cpp)
{
	register int		n;
	register const char	*cp = *cpp;

	n  = (*cp++ << 8);
	n |= *cp++;

	*cpp = cp;

	return (n);

}	/* end get_short */

/*
 ****************************************************************
 *	Coloca um "short" desalinhado				*
 ****************************************************************
 */
void
put_short (register int value, char **cpp)
{
	register char	*cp = *cpp;

	*cp++ = value >> 8;
	*cp++ = value;

	*cpp = cp;

}	/* end put_short */

/*
 ****************************************************************
 *	Obtém um "long" desalinhado				*
 ****************************************************************
 */
long
get_long (register char const **cpp)
{
	register long		n;
	register const char	*cp = *cpp;

	n  = (*cp++ << 24);
	n |= (*cp++ << 16);
	n |= (*cp++ << 8);
	n |= *cp++;

	*cpp = cp;

	return (n);

}	/* end get_long */

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
		"%s - teste de obtenção de um endereço de um servidor de nomes\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-mrwv] [-f <arquivo>] <nome> [<nó remoto do servidor>]\n",
		pgname, pgversion, pgname
	);
	fprintf
	(	stderr,
		"\nOpções:"
		"\t-m: Mail\n"
		"\t-r: Reverso\n"
		"\t-w: Grave o datagrama no arquivo \"answer\"\n"
		"\t-v: Verboso\n"
		"\t-f: Interprete o datagrama do <arquivo>\n"
		"\t-d: Debug\n"
	);

	exit (2);

}	/* end help */

#if (0)	/*************************************/


	register int	i;
	T_UNITDATA	unitdata;
	union { DG dg; char big_area[128]; } u;
	int		flag;
	INADDR		rcv_addr;
	MUTM		end_time;


	{
		i++;
		continue;
	}
	elif (u.dg.h.ih_type != ICMP_ECHO_REPLY)
	{
		analyse_unexpected_datagrams (&u.dg, rcv_addr.a_addr);
		i++;
		continue;
	}

#endif	/*************************************/
