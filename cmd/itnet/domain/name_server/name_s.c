/*
 ****************************************************************
 *								*
 *			name_server.c				*
 *								*
 *	Servidor de nomes do "domain" de brincadeira		*
 *								*
 *	Versão	3.0.0, de 28.01.94				*
 *		3.0.0, de 09.02.94				*
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
#include <sys/itnet.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <fcntl.h>
#include <string.h>
#include <xti.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
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
entry int	vflag;		/* Verbose */

entry char	*area_ptr;	/* para "get_domain_name" */

/*
 ****** Variáveis globais ***************************************
 */
const char	udp_dev[] = "/dev/itnudp";

int		udp_fd;		/* fd do UDP */

/*
 ****** Tabela de endereços de brincadeira **********************
 */
typedef struct
{
	char	d_name[32];	/* Nome do Nó */
	char	d_addr[16];	/* Endereço do Nó */

}	ADDRTB;

const ADDRTB	addrtb[] =
{
	"marte.nce.ufrj.br",	"146.164.205.20",
	"marte1.nce.ufrj.br",	"146.164.205.20",
	"marte2.nce.ufrj.br",	"146.164.205.20",
	"marte3.nce.ufrj.br",	"146.164.205.20",
	"marte4.nce.ufrj.br",	"146.164.205.20",
	"venus.nce.ufrj.br",	"146.164.205.10",
	"venus1.nce.ufrj.br",	"146.164.205.10",
	"venus2.nce.ufrj.br",	"146.164.205.10",
	"venus3.nce.ufrj.br",	"146.164.205.10",
	"venus4.nce.ufrj.br",	"146.164.205.10",
	"jupiter.nce.ufrj.br",	"146.164.205.10",
	"mercurio.nce.ufrj.br",	"146.164.205.20",
	"saturno.nce.ufrj.br",	"146.164.205.10",
	"urano.nce.ufrj.br",	"146.164.205.20",
	"plutao.nce.ufrj.br",	"146.164.205.30",
	"netuno1.nce.ufrj.br",	"146.164.205.30",
	"netuno2.nce.ufrj.br",	"146.164.205.30",
	"netuno3.nce.ufrj.br",	"146.164.205.30",
	""
};

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

#if (0)	/*************************************/
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
#endif	/*************************************/

/*
 ****** Protótipos de funções ***********************************
 */
void		send_answer (int, T_UNITDATA *, char *, long);
void		receive_answer (void);
void		put_domain_name (char *name, char **area);
void		read_debug (void);
void		analyse (T_UNITDATA *);
void		answer_question (int, T_UNITDATA *, char *, int, int);
void		print_resource_record (char *header, register char **cp);
char		*get_type_name (register int type);
char		*get_class_name (register int class);
void		analyse_unexpected_datagrams (register DG *dp, long rcv_addr);
char		*h_op_text (int);
char		*h_rc_text (int);
char		*get_domain_name (char **cpp);
int		get_short (char **cpp);
void		put_short (int value, char **cpp);
long		get_long (char **cpp);
void		put_long (long value, char **cpp);
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
	T_BIND		bind;
	INADDR		server_addr, client_addr;
	T_UNITDATA	ud;
	char		area[512];
	int		flags;
	int		well_known_port_no;

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "vM")) != EOF)
	{
		switch (opt)
		{
		    case 'v':			/* Verbose */
			vflag++;
			break;

		    case 'M':			/* Na memória */
			exit (0);

		    default:			/* Erro */
			error ("Opção inválida: '%c'", opt);
			break;

		}	/* end switch */

	}	/* end while */

	argv += optind;
	argc -= optind;

	/*
	 *	serv "udp" <well_known_port>
	 */
	well_known_port_no = atoi (argv[1]);

	if (vflag)
		printf ("name_s %s %s\n", argv[0], argv[1]);

	/*
	 *	Abre o dispositivo UDP
	 */
	if ((udp_fd = t_open (udp_dev, O_RDWR, (T_INFO *)NULL)) < 0)
	{
		fprintf (stderr, "Não consegui abrir o dispositivo\n");
		perror (pgname);
		exit (1);
	}

	/*
	 *	Obtém a porta local
	 */
	server_addr.a_port = well_known_port_no;
	server_addr.a_addr = 0;

	bind.addr.len = sizeof (INADDR);
	bind.addr.buf = &server_addr;

	if (t_bind (udp_fd, &bind, (T_BIND *)NULL) < 0)
	{
		fprintf (stderr, "Não consegui atribuir o endereço local\n");
		perror (pgname);
		exit (1);
	}

	/*
	 *	Prepara um UNITDATA para enviar/receber mensagens
	 */
	ud.addr.len	= sizeof (INADDR);
	ud.addr.maxlen	= sizeof (INADDR);
	ud.addr.buf	= &client_addr;

	ud.opt.len	= 0;

	/*
	 *	Malha principal
	 */
	for (EVER)
	{
	   /***	ud.udata.len = ...; ***/
		ud.udata.maxlen	= sizeof (area);
		ud.udata.buf	= area;

		if (t_rcvudata (udp_fd, &ud, &flags) < 0)
			error ("$*Erro no t_rcvudata");

		analyse (&ud);
	}

#if (0)	/*************************************/
	t_close (udp_fd);
#endif	/*************************************/

}	/* end domain */

/*
 ****************************************************************
 *	Analisa a pergunta					*
 ****************************************************************
 */
void
analyse (T_UNITDATA *ud)
{
	char		*area = ud->udata.buf;
	register H	*hp = (H *)area;
	register int	i;
	char		*cp = area + sizeof (H);

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
	if (vflag)
	{
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
	}

	/*
	 *	Analisa a seção de perguntas
	 */
	for (i = 0; i < hp->h_qdcnt; i++)
	{
		char		*name;
		int		qtype, qclass;

		name =  get_domain_name (&cp);
		qtype =  get_short (&cp);
		qclass = get_short (&cp);

		if (vflag)
		{
			printf ("\nQuestion name = %s, ", name);
			printf ("qtype = %s, ", get_type_name (qtype));
			printf ("qclass = %s\n", get_class_name (qclass));
		}

		answer_question (hp->h_id, ud, name, qtype, qclass);
	}

}	/* end analyse */

/*
 ****************************************************************
 *	Responde uma pergunta					*
 ****************************************************************
 */
void
answer_question (int id, T_UNITDATA *ud, char *name, int qtype, int qclass)
{
	register const ADDRTB	*dp;

	for (dp = addrtb; dp->d_name[0] != '\0'; dp++)
	{
		if (streq (dp->d_name, name))
		{
			send_answer (id, ud, name, t_str_to_addr (dp->d_addr));
			return;
		}
	}

	send_answer (id, ud, name, -1);

} 	/* end answer_question */

/*
 ****************************************************************
 *	Envia o pedido						*
 ****************************************************************
 */
void
send_answer (int id, register T_UNITDATA *ud, char *name, long addr)
{
	struct { H h; char c[256]; }  dg;
	char		*str;
	int		sz;

	/*
	 *	Prepara o datagrama
	 */
	memset (&dg, 0, sizeof (dg));

	dg.h.h_id = ENDIAN_SHORT (id);
   	dg.h.h_qr = 1;
   /***	dg.h.h_op = 0;	***/
   	dg.h.h_aa = 1;
   /***	dg.h.h_tc = 0;	***/
   /***	dg.h.h_rd = 0;	***/
	dg.h.h_ra = 1;
   /***	dg.h.h_z  = 0;	***/

	if (addr == -1)
	{
		dg.h.h_rc = 3;	/* Name error */
	   	dg.h.h_qdcnt = ENDIAN_SHORT (1);
	   /***	dg.h.h_ancnt = ENDIAN_SHORT (0); ***/
	}
	else
	{
	   /*** dg.h.h_rc = 0;	***/
	   	dg.h.h_qdcnt = ENDIAN_SHORT (1);
	   	dg.h.h_ancnt = ENDIAN_SHORT (1);
	}

   /***	dg.h.h_nscnt = ENDIAN_SHORT (0);	***/
   /***	dg.h.h_arcnt = ENDIAN_SHORT (0);	***/

	str = dg.c;

	/*
	 *	Ecoa a pergunta
	 */
	put_domain_name (name, &str);	/* Node dado */

	put_short (1, &str);		/* Type = A */

	put_short (1, &str);		/* Class = IN */

	/*
	 *	Põe a reposta
	 */
	if (addr != -1)
	{
		put_domain_name (name, &str);	/* Node dado */

		put_short (1, &str);		/* Type = A */

		put_short (1, &str);		/* Class = IN */

		put_long (15 * 60, &str);	/* TTL = 15 minutos */

		put_short (sizeof (long), &str); /* Len = 4 */

		put_long (addr, &str);		/* Endereço desejado */
	}

	sz = str - (char *)&dg;

	if (vflag)
		printf ("dg sz = %d\n", sz);

	/*
	 *	Manda o datagrama UDP
	 */
	ud->udata.len	= sz;
	ud->udata.buf	= &dg;

	if (t_sndudata (udp_fd, ud) < 0)
	{
		fprintf (stderr, "Erro no SNDUDP\n");
		perror (pgname);
		exit (1);
	}

}	/* end send_answer */

/*
 ****************************************************************
 *	Compõe um nome no formato do "domain"			*
 ****************************************************************
 */
void
put_domain_name (char *name, char **area)
{
	register char	*np = name;
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


#if (0)	/*************************************/
/*
 ****************************************************************
 *	Imprime um RR						*
 ****************************************************************
 */
void
print_resource_record (char *header, register char **cp)
{
	register int	type, class, len;

	printf ("\n%s = %s, ", header, get_domain_name (cp));
	printf ("rtype = %s, ", get_type_name (type = get_short (cp)));
	printf ("rclass = %s\n", get_class_name (class = get_short (cp)));
	printf ("ttl = %d, ", get_long (cp));
	printf ("len = %d, ", len = get_short (cp));

	if   (type == TYPE_A && class == CLASS_IN)
		printf ("Addr = %s\n", t_addr_to_str (get_long (cp)));
	elif (type == TYPE_NS && class == CLASS_IN)
		printf ("Name = %s\n", get_domain_name (cp));
	else
		{ printf ("len = %d\n", len); *cp += len; }

}	/* end print_resource_record */
#endif	/*************************************/

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
get_domain_name (register char **cpp)
{
	register char	*cp = *cpp;
	register int	i;
	static char	area[256];
	register char	*ap = area;
	int		ptr = 0;

	/*
	 *	Ponteiro
	 */
	if (*cp == 0xC0)
	{
		ptr++;
		cp = area_ptr + cp[1];
	}

	/*
	 *	Nome normal
	 */
	while (*cp != 0)
	{
		for (i = *cp++; i > 0; i--)
			*ap++ = *cp++;

		if (*cp != 0)
			*ap++ = '.';
	}

	*ap++ = '\0';

	if (ptr)
		(*cpp) += 2;
	else
		*cpp = cp + 1;

	return (area);

}	/* end get_domain_name */

/*
 ****************************************************************
 *	Obtém um "short" desalinhado				*
 ****************************************************************
 */
int
get_short (register char **cpp)
{
	register int	n;
	register char	*cp = *cpp;

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

#if (0)	/*************************************/
/*
 ****************************************************************
 *	Obtém um "long" desalinhado				*
 ****************************************************************
 */
long
get_long (register char **cpp)
{
	register long	n;
	register char	*cp = *cpp;

	n  = (*cp++ << 24);
	n |= (*cp++ << 16);
	n |= (*cp++ << 8);
	n |= *cp++;

	*cpp = cp;

	return (n);

}	/* end get_long */
#endif	/*************************************/

/*
 ****************************************************************
 *	Coloca um "long" desalinhado				*
 ****************************************************************
 */
void
put_long (register long value, char **cpp)
{
	register char	*cp = *cpp;

	*cp++ = value >> 24;
	*cp++ = value >> 16;
	*cp++ = value >> 8;
	*cp++ = value;

	*cpp = cp;

}	/* end put_long */
