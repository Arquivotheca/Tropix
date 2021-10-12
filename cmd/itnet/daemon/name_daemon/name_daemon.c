/*
 ****************************************************************
 *								*
 *			name_daemon.c				*
 *								*
 *	Domain name resolver					*
 *								*
 *	Versão	3.0.0, de 30.01.94				*
 *		4.9.0, de 31.08.06				*
 *								*
 *	Módulo: ITNET						*
 *		Utilitários de comunicação			*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2006 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include <sys/common.h>
#include <sys/sync.h>
#include <sys/itnet.h>
#include <sys/times.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <signal.h>
#include <xti.h>
#include <errno.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão:	4.9.0, de 31.08.06";

#define	DOMAIN_PORT	53
#define	TIMEOUT		10	/* Segundos */
#define	MILHAO		1000000

#define	EVER	;;
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
 ****** Variáveis globais ***************************************
 */
entry int		vflag = 0;	/* Verbose */

entry const char	*area_ptr;	/* para "get_domain_name" */

entry char		error_msg_to_log; /* Não nulo se mensagens para "itnetlog" */

const char		udp_dev[] = "/dev/itnudp";

entry int		udp_fd;		/* fd do UDP */

entry long		pidfilho;	/* Para enviar o sinal */

/*
 ****** Tabela de servidores de nomes ***************************
 */
typedef enum
{ 
	/* Repare os códigos dos servidores crescentes em qualidade */

	SERVER_NO_SERVER_AT_ALL = 0,	/* Não tenho nenhum servidor disponível */
	SERVER_NO_SERVER_ON_ADDR,	/* Neste endereço não tem servidor */
	SERVER_TIMEOUT,			/* Não respondeu */
	SERVER_BAD_ANSWER,		/* O servidor deu uma resposta inútil */
	SERVER_OK,			/* Obteve o endereço desejado */
	SERVER_NULL			/* Não usado ainda */

}	SERVER_ENUM;

typedef struct
{
	IPADDR	s_addr;		/* Endereço do Servidor */

	char	s_index;	/* No. do servidor */
	char	s_code;		/* Código de serviço: veja acima */
	bit	s_checksum : 1;	/* A resposta tem "checksum" */

	long	s_time;		/* Tempo de resposta em micro-segundos */
	MUTM	s_mutm;		/* Data de envio da consulta */

}	SERVTB;

entry SERVTB	servtb[DNS_SERVER_CNT + 1];	/* A tabela de servidores */	

entry IPADDR	nm_serv_tb[DNS_SERVER_CNT + 1];

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

/*
 ****** Tabela com os tipos de erros do DOMAIN ******************
 */
enum DOMAIN_ERROR_CODE
{
	/*  0 */   DOMAIN_NO_ERROR = 0,		/* Nenhum erro */
	/*  1 */   DOMAIN_FORMAT_ERROR,		/* O servidor não entendeu */
	/*  2 */   DOMAIN_SERVER_FAILURE,	/* Problemas com o servidor */
	/*  3 */   DOMAIN_NAME_ERROR,		/* Nome não existe */
	/*  4 */   DOMAIN_NOT_IMPLEMENTED,	/* Serviço inexistente */
	/*  5 */   DOMAIN_REFUSED		/* O servidor não faz este serviço */

	/*  6 a 15 Reservado para uso futuro */
};

/*
 ****** Tabela com os nomes dos tipos do DOMAIN *****************
 */
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

/*
 ****** Tabela com os nomes das classes do DOMAIN ***************
 */
enum CLASS_CODE
{
	/*  1 */   CLASS_IN = 1,/* Internet */
	/*  2 */   CLASS_CS,	/* CSNET class (obsolete) */
	/*  3 */   CLASS_CH,	/* CHAOS class */
	/*  4 */   CLASS_HS	/* HESIOD class */
};

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

/*
 ****** Protótipos de funções ***********************************
 */
void		filho (void);
int		send_req (SERVTB *, DNS *);
void		pai (void);
void		analyse_udp_datagrams (SERVTB *, char *);
char		*get_domain_name (const char **cpp);
void		put_domain_name (char *name, char **area);
int		get_short (const char **cpp);
void		put_short (int value, char **cpp);
long		get_long (const char **cpp);
IPADDR		t_arpa_str_to_addr (const char *);
const char	*get_type_name (int);
const char	*get_class_name (int);
char		*h_op_text (int);
char		*h_rc_text (enum DOMAIN_ERROR_CODE);
char		*edit_kernel_code (enum DNS_ERROR_CODE);
void		on_signal (int, ...);
void		error (const char *fmt, ...);
void		help (void);

/*
 ****************************************************************
 *	Domain name resolver					*
 ****************************************************************
 */
void
main (int argc, const char *argv[])
{
	int		opt;
	SERVTB		*sp;
	int		index;
	IPADDR		*ip;
	INADDR		addr;
	T_BIND		bind;

	/*
	 *	Pequena inicialização
	 */
	error_msg_to_log++;

	/*
	 *	Verifica se é SUPERUSUÁRIO
	 */
	if (geteuid () != 0)
		error ("$O usuário efetivo não é SUPERUSUÁRIO");

	/*
	 *	Analisa as opções
	 *
	 *	Sintaxe:
	 *		name_daemon [-v]
	 */
	while ((opt = getopt (argc, argv, "v")) != EOF)
	{
		switch (opt)
		{
		    case 'v':			/* Verbose */
			vflag++;
			break;

		    default:			/* Erro */
			putc ('\n', stderr);
			help ();

		}	/* end switch */

	}	/* end while */

	argv += optind;
	argc -= optind;

	/*
	 *	Abre os "endpoint"s
	 */
	if ((udp_fd = t_open (udp_dev, O_RDWR, (T_INFO *)NULL)) < 0)
		error ("$*Não consegui abrir \"%s\"", udp_dev);

	/*
	 *	Prepara a tabela de servidores 
	 */
	if (ioctl (udp_fd, I_GET_DNS_SERVER, nm_serv_tb) < 0)
		error ("$*Não consegui receber a tabela de servidores DNS");

	ip = nm_serv_tb;

	if (*ip == 0)
		error ("$Recebi a tabela de servidores DNS vazia");

	for (index = 1, sp = servtb; *ip != 0; sp++, index++, ip++)
	{
		sp->s_addr	= *ip;

		sp->s_index	= index;
		sp->s_code	= SERVER_NULL;
	   /***	sp->s_checksum	= 0; ***/

	   /***	sp->s_time	= ...; ***/
	   /***	sp->s_mutm	= ...; ***/
	}

	if (vflag) for (sp = servtb; sp->s_addr != 0; sp++)
		error ("Servidor DNS \"%s\"", t_addr_to_str (sp->s_addr));

	/*
	 *	Associa um endereço UDP
	 */
	bind.addr.len	 = 0;
	bind.addr.maxlen = sizeof (INADDR);
	bind.addr.buf	 = &addr;

	if (t_bind (udp_fd, &bind, &bind) < 0)
		error ("$*Não consegui dar \"t_bind\" UDP");

	/*
	 *	Cria um filho para enviar os pedidos de DNS.
	 *	O pai le os datagramas de resposta.
	 */
	if ((pidfilho = thread ()) < 0)
		error ("$*Não consegui criar um novo processo");

	if (pidfilho > 0)
		pai ();
	else
		filho ();

	/* Não retorna */

}	/* end domain */

/*
 ****************************************************************
 *	Filho: Espera receber novos pedidos			*
 ****************************************************************
 */
void
filho (void)
{
	SERVTB		*sp;
	DNS		dns;

	for (EVER)
	{
		if (ioctl (udp_fd, I_DNS_WAIT_REQ, &dns) < 0)
			error ("*Erro no \"ioctl\" I_DNS_WAIT_REQ");

		if (vflag) error
		(	"Recebendo pedido do endereço de \"%s\"",
			dns.d_host_nm
		);

		for (sp = servtb; sp->s_addr != 0; sp++)
		{
			if (vflag) error
			(	"Consultando a \"%s\"",
				t_addr_to_str (sp->s_addr)
			);

			send_req (sp, &dns);
		}

	}	/* end for (EVER) */

}	/* end filho */

/*
 ****************************************************************
 *	Envia o pedido						*
 ****************************************************************
 */
int
send_req (SERVTB *sp, DNS *np)
{
	T_UNITDATA			unitdata;
	INADDR				addr;
	struct { H h; char c[256]; }	dg;
	char				*str;
	int				sz;
	char				area[128];
	static int			id;

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
	if (np->d_entry_code == 'R')
	{
		const char	*cp = (char *)&np->d_host_addr;
#ifdef	LITTLE_ENDIAN
		sprintf (area, "%u.%u.%u.%u.IN-ADDR.ARPA", cp[0], cp[1], cp[2], cp[3]);
#else
		sprintf (area, "%u.%u.%u.%u.IN-ADDR.ARPA", cp[3], cp[2], cp[1], cp[0]);
#endif	LITTLE_ENDIAN

		put_domain_name (area, &str);
	}
	else
	{
		put_domain_name (np->d_host_nm, &str);
	}

	if   (np->d_entry_code == 'R')
		put_short (TYPE_PTR, &str);	/* Type = PTR */
	elif (np->d_entry_code == 'M')
		put_short (TYPE_MX, &str);	/* Type = MX */
	elif (np->d_entry_code == 'C')
		put_short (TYPE_A, &str);	/* Type = A */
	else
		return (0);

	put_short (CLASS_IN, &str);	/* Class = IN */

	sz = str - (char *)&dg;

	/*
	 *	Manda o datagrama UDP
	 */
	addr.a_addr = sp->s_addr;
	addr.a_port = DOMAIN_PORT;

	unitdata.addr.len	= sizeof (addr);
	unitdata.addr.buf	= &addr;

	unitdata.udata.len	= sz;
	unitdata.udata.buf	= &dg;

	mutime (&sp->s_mutm);

	if (t_sndudata (udp_fd, &unitdata) >= 0)
	{
		if (vflag) error
		(
			"Enviando UDP (id %d) para %s", 
			id, t_addr_to_str (sp->s_addr)
		);

		return (0);
	}

	/*
	 *	Não conseguiu enviar
	 */
   /***	strcpy (np->d_host_nm, ...); ***/
	np->d_host_addr = 0;

   /***	np->d_entry_code = ...; ***/
   /***	np->d_resolver_done = ...; ***/
	np->d_error_code = DNS_SERVER_ERROR;
	np->d_server_index = sp->s_index;

	np->d_expir_time = 60;
   /***	np->d_last_use_time = ...; ***/

	sp->s_code = SERVER_NO_SERVER_ON_ADDR;

	if (ioctl (udp_fd, I_DNS_PUT_INFO, np) < 0)
		error ("*Erro no \"ioctl\" I_DNS_PUT_INFO");

	error ("*Erro no SNDUDP");

	return (-1);

}	/* end send_req */

/*
 ****************************************************************
 *	Espera uma resposta					*
 ****************************************************************
 */
void
pai (void)
{
	SERVTB		*sp;
	T_UNITDATA	unitdata;
	char		area[512 + 128];
	int		flag;
	INADDR		rcv_addr;
	MUTM		end_time;

	/*
	 *	Para terminar elegantemente
	 */
	signal (SIGINT,  on_signal);
	signal (SIGQUIT, on_signal);
	signal (SIGTERM, on_signal);
	signal (SIGHUP,  on_signal);

	/*
	 *	Espera datagramas UDP
	 */
	for (EVER)
	{
	    again:
		unitdata.addr.maxlen	= sizeof (rcv_addr);
		unitdata.addr.buf	= &rcv_addr;

		unitdata.udata.maxlen	= sizeof (area);
		unitdata.udata.buf	= area;

		if (t_rcvudata (udp_fd, &unitdata, &flag) < 0)
			error ("$*Erro no RCVUDP");

		mutime (&end_time);

		for (sp = servtb; /* abaixo */; sp++)
		{
			if (sp->s_addr == 0)
			{
				error ("Recebendo datagrama UDP de endereço errado");
				goto again;
			}

			if (rcv_addr.a_addr == sp->s_addr)
				break;
		}

		if (vflag) error
		(	"Recebi UDP %d bytes (%s checksum) de \"%s\"",
			unitdata.udata.len, flag & T_NO_CHECKSUM ? "sem" : "com",
			t_addr_to_str (sp->s_addr)
		);

		analyse_udp_datagrams (sp, area);

	}	/* end for (EVER) */

}	/* end pai */

/*
 ****************************************************************
 *	Analisa a resposta					*
 ****************************************************************
 */
void
analyse_udp_datagrams (SERVTB *sp, char *area)
{
	H		*hp = (H *)area;
	int		i;
	int		len;
	char		*str;
	const char	*cp = area + sizeof (H), *old_cp;
	char		*qname, *aname;
	int		qtype, qclass;
	int		atype, aclass, ttl;
	DNS		dns[2];
#	define	NMAIL	8
	int		n_mail = 0;
	DNS		mail_server[NMAIL];
	DNS		canon;

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
	 *	Pequenas inicializações
	 */
	area_ptr = area;
	memsetl (dns,    0, sizeof (dns)   / sizeof (long));
	memsetl (&canon, 0, sizeof (canon) / sizeof (long));

	/*
	 *	Analisa o "header"
	 */
#ifdef	DEBUG
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
#endif	DEBUG

	/*
	 *	Só deve haver uma pergunta
	 */
	if (hp->h_qdcnt != 1)
	{
		error ("Recebi um datagrama com %d perguntas", hp->h_qdcnt);
		return;
	}

	/*
	 *	Retira a pergunta (ela só é relevante em caso de erro)
	 */
	str    = get_domain_name (&cp); len = strlen (str);
	qname  = alloca (len + 1); strcpy (qname, str);
	qtype  = get_short (&cp);
	qclass = get_short (&cp);

	/*
	 *	Examina o tipo da pergunta
	 */
	if (qclass != CLASS_IN)
	{
		error ("Recebi um datagrama com classe %s", get_class_name (qclass));
		return;
	}

	if (qtype != TYPE_A && qtype != TYPE_MX && qtype != TYPE_PTR)
	{
		error ("Recebi um datagrama com pergunta de tipo %s", get_type_name (qtype));
		return;
	}

	/*
	 *	Se não há respostas, é como se fôsse um erro
	 */
	if (hp->h_ancnt == 0 && hp->h_rc == DOMAIN_NO_ERROR)
		hp->h_rc = DOMAIN_NAME_ERROR;

	/*
	 *	Verifica se houve erro
	 */
	if (hp->h_rc != DOMAIN_NO_ERROR)
	{
		int		error_code = DNS_NAME_ERROR;

		sp->s_code = SERVER_OK;

		if (hp->h_rc != DOMAIN_NAME_ERROR)
		{
			error
			(	"Código \"%s\" inesperado do servidor DNS \"%s\"",
				h_rc_text (hp->h_rc), t_addr_to_str (sp->s_addr)
			);
			error_code = DNS_SERVER_ERROR;
			sp->s_code = SERVER_BAD_ANSWER;
		}

		if (qtype == TYPE_PTR)
		{
			dns[0].d_host_nm[0] = '\0';
			dns[0].d_host_addr = t_arpa_str_to_addr (qname);
			dns[0].d_entry_code = 'R';
		}
		else
		{
			strcpy (dns[0].d_host_nm, qname);
			dns[0].d_host_addr = 0;
			dns[0].d_entry_code = (qtype == TYPE_MX) ? 'M' : 'C';
		}

		dns[0].d_error_code = error_code;
		dns[0].d_server_index = sp->s_index;

		dns[0].d_expir_time = 60;
	   /***	dns[0].d_preference = ...; ***/

		if (ioctl (udp_fd, I_DNS_PUT_INFO, dns) < 0)
			error ("*Erro no \"ioctl\" I_DNS_PUT_INFO");

		return;
	}

	/*
	 *	Processa a seção "answer"
	 */
	for (i = 0; i < hp->h_ancnt; i++, cp = old_cp + len)
	{
		str	= get_domain_name (&cp); len = strlen (str);
		aname	= alloca (len + 1); strcpy (aname, str);
		atype	= get_short (&cp);
		aclass	= get_short (&cp);
		ttl	= get_long (&cp);
		len	= get_short (&cp);

		if (ttl == 0)
			ttl = 8 * 60;

		old_cp = cp;

		/*
		 *	Examina a classe
		 */
		if (aclass != CLASS_IN)
		{
			error ("Recebi uma reposta com classe %s", get_class_name (aclass));
			continue;
		}

		/*
		 *	Examina o tipo da resposta
		 */
		if   (atype == TYPE_PTR)	/* O nome, dado o endereço */
		{
			IPADDR		addr;

			if ((addr = t_arpa_str_to_addr (aname)) == -1)
			{
				error
				(	"Endereço IN.ADDR-ARPA inválido (%s)",
					aname
				);
			}

			strcpy (dns[0].d_host_nm, get_domain_name (&cp));
			dns[0].d_host_addr = addr;

			dns[0].d_entry_code = 'R';
			dns[0].d_error_code = DNS_NO_ERROR;
			dns[0].d_server_index = sp->s_index;

			dns[0].d_expir_time = ttl;
		   /***	dns[0].d_preference = ...; ***/

			if (ioctl (udp_fd, I_DNS_PUT_INFO, dns) < 0)
				error ("*Erro no \"ioctl\" I_DNS_PUT_INFO");

			continue;
		}
		elif (atype == TYPE_MX)		/* O nome de um servidor de correio */
		{
			const DNS	*dp;
			DNS		*new_dp;

			if (!streq (aname, qname))
			{
				error
				(	"Recebi um servidor de correio espúrio (%s, %s)",
					aname, qname
				);
				continue;
			}

			if (n_mail >= NMAIL)
				continue;

			new_dp = &mail_server[n_mail];

			new_dp->d_preference = get_short (&cp);
			strcpy (new_dp->d_host_nm, get_domain_name (&cp));

			/* Processa a preferência */

		    again:	
			for (dp = mail_server; dp < new_dp; dp++)
			{
				if (dp->d_preference == new_dp->d_preference)
					{ new_dp->d_preference++; goto again; }
			}

			n_mail++;

			/* Envia o servidor de correio (sem endereço IP) */

			strcpy (dns[0].d_host_nm, aname);
			dns[0].d_host_addr = 0;

			dns[0].d_entry_code = 'M';
			dns[0].d_error_code = DNS_NO_ERROR;
			dns[0].d_server_index = sp->s_index;

			dns[0].d_expir_time = ttl;
		   /***	dns[0].d_preference = ...; ***/


			strcpy (dns[1].d_host_nm, new_dp->d_host_nm);
			dns[1].d_host_addr = 0;

			dns[1].d_entry_code = 'C';
			dns[1].d_error_code = DNS_NO_ERROR;
			dns[1].d_server_index = sp->s_index;

			dns[1].d_expir_time = ttl;
			dns[1].d_preference = new_dp->d_preference;

			if (ioctl (udp_fd, I_DNS_PUT_INFO, dns) < 0)
				error ("*Erro no \"ioctl\" I_DNS_PUT_INFO");

			continue;
		}
		elif (atype == TYPE_CNAME)	/* O nome canônico de um alias */
		{
			if (!streq (aname, qname))
			{
				error
				(	"Recebi um alias espúrio (%s, %s)",
					aname, qname
				);
				continue;
			}

			strcpy (canon.d_host_nm, get_domain_name (&cp));

			continue;
		}
		elif (atype != TYPE_A)
		{
			error
			(	"Recebi um resposta de tipo %s",
				get_type_name (atype)
			);

			continue;
		}

		/*
		 *	É um endereço IP
		 */
		if (streq (aname, canon.d_host_nm))	/* Alias */
		{
			strcpy (dns[0].d_host_nm, qname);
			dns[0].d_host_addr = get_long (&cp);

			dns[0].d_entry_code = 'A';
			dns[0].d_error_code = DNS_NO_ERROR;
			dns[0].d_server_index = sp->s_index;

			dns[0].d_expir_time = ttl;
		   /***	dns[0].d_preference = ...; ***/


			strcpy (dns[1].d_host_nm, aname);
			dns[1].d_host_addr = dns[0].d_host_addr;

			dns[1].d_entry_code = 'C';
			dns[1].d_error_code = DNS_NO_ERROR;
			dns[1].d_server_index = sp->s_index;

			dns[1].d_expir_time = ttl;
		   /***	dns[1].d_preference = ...; ***/

			if (ioctl (udp_fd, I_DNS_PUT_INFO, dns) < 0)
				error ("*Erro no \"ioctl\" I_DNS_PUT_INFO");
		}
		else					/* Nome normal */
		{
			strcpy (dns[0].d_host_nm, aname);
			dns[0].d_host_addr = get_long (&cp);

			dns[0].d_entry_code = 'C';
			dns[0].d_error_code = DNS_NO_ERROR;
			dns[0].d_server_index = sp->s_index;

			dns[0].d_expir_time = ttl;
		   /***	dns[0].d_preference = ...; ***/

			if (ioctl (udp_fd, I_DNS_PUT_INFO, dns) < 0)
				error ("*Erro no \"ioctl\" I_DNS_PUT_INFO");
		}

	}	/* Examinando as respostas */

	/*
	 *	Pula a seção "authority"
	 */
	for (i = 0; i < hp->h_nscnt; i++, cp = old_cp + len)
	{
		(void)get_domain_name (&cp);
		(void)get_short (&cp);
		(void)get_short (&cp);
		(void)get_long (&cp);
		len = get_short (&cp);

		old_cp = cp;

	}	/* Pulando as "autoridades" */

	/*
	 *	Processa a seção "additional"
	 */
	for (i = 0; i < hp->h_arcnt; i++, cp = old_cp + len)
	{
		str	= get_domain_name (&cp); len = strlen (str);
		aname	= alloca (len + 1); strcpy (aname, str);
		atype	= get_short (&cp);
		aclass	= get_short (&cp);
		ttl	= get_long (&cp);
		len	= get_short (&cp);

		if (ttl == 0)
			ttl = 8 * 60;

		old_cp = cp;

		/*
		 *	Examina a classe
		 */
		if (aclass != CLASS_IN)
		{
			error ("Recebi um adicional com classe %s", get_class_name (aclass));
			continue;
		}

		/*
		 *	Só considera endereços
		 */
		if (atype != TYPE_A)
			continue;

		/*
		 *	Processa o endereço
		 */
		strcpy (dns[0].d_host_nm, aname);
		dns[0].d_host_addr = get_long (&cp);

		dns[0].d_entry_code = 'C';
		dns[0].d_error_code = DNS_NO_ERROR;
		dns[0].d_server_index = sp->s_index;

		dns[0].d_expir_time = ttl;
	   /***	dns[0].d_preference = ...; ***/

		if (ioctl (udp_fd, I_DNS_PUT_INFO, dns) < 0)
			error ("*Erro no \"ioctl\" I_DNS_PUT_INFO");

	}	/* Examinando as respostas */

	/*
	 *	Coloca o final dos servidores de correio
	 */
	if (qtype == TYPE_MX)
	{
		strcpy (dns[0].d_host_nm, qname);
		dns[0].d_host_addr = 0;

		dns[0].d_entry_code = 'M';
		dns[0].d_error_code = DNS_NO_ERROR;
		dns[0].d_server_index = sp->s_index;

		dns[0].d_expir_time = 0;
	   /***	dns[0].d_preference = ...; ***/


		dns[1].d_host_nm[0] = '\0';
		dns[1].d_host_addr = 0;

		dns[1].d_entry_code = 'E';
		dns[1].d_error_code = DNS_NO_ERROR;
		dns[1].d_server_index = sp->s_index;

		dns[1].d_expir_time = 0;
		dns[1].d_preference = 0;

		if (ioctl (udp_fd, I_DNS_PUT_INFO, dns) < 0)
			error ("*Erro no \"ioctl\" I_DNS_PUT_INFO");
	}

}	/* end analyse_udp_datagrams */

/*
 ****************************************************************
 *	Obtém um nome de domínio				*
 ****************************************************************
 */
char *
get_domain_name (const char **cpp)
{
	const char	*cp = *cpp;
	int		i;
	static char	area[256];
	char		*ap = area;
	char		updated_ptr = 0;

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
 *	Compõe um nome no formato do "domain"			*
 ****************************************************************
 */
void
put_domain_name (char *name, char **area)
{
	char		*np = name;
	char		c;
	int		n = 0;
	char		*ap = *area + 1;
	char		*bp = *area;

	for (EVER)
	{
		*ap++ = c = *np++;

		if (c == '.' || c == '\0')
		{
			*bp = n; n = 0; bp = ap - 1;

			if (c == '\0')
				{ *area = ap; return; }
		}
		else
		{
			n++;
		}

	}	/* end for (EVER) */

}	/* end put_domain_name */

/*
 ****************************************************************
 *	Obtém um "short" desalinhado				*
 ****************************************************************
 */
int
get_short (const char **cpp)
{
	int		n;
	const char	*cp = *cpp;

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
put_short (int value, char **cpp)
{
	char		*cp = *cpp;

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
get_long (const char **cpp)
{
	long		n;
	const char	*cp = *cpp;

	n  = (*cp++ << 24);
	n |= (*cp++ << 16);
	n |= (*cp++ << 8);
	n |= *cp++;

	*cpp = cp;

	return (n);

}	/* end get_long */

/*
 ****************************************************************
 *	Decodifica um nome reverso  ("d.d.d.d.IN-ADDR.ARPA")	*
 ****************************************************************
 */
IPADDR
t_arpa_str_to_addr (const char *node_str)
{
	register long		i, n;
	register const char	*cp = node_str;
	const char		*str;
	char			node[4];
	register char		*np;

#ifdef	LITTLE_ENDIAN
	np = &node[0];
#else
	np = &node[4];
#endif	LITTLE_ENDIAN

	for (i = 0; i < 4; i++)
	{
		if ((n = strtol (cp, &str, 0)) & ~0xFF || *str != '.' || str == cp)
			return (-1);

		cp = str + 1;

#ifdef	LITTLE_ENDIAN
		*np++ = n;
#else
		*--np = n;
#endif	LITTLE_ENDIAN
	}

	return (*(IPADDR *)node);

}	/* end t_arpa_str_to_addr */
#ifdef	DEBUG
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
#endif	DEBUG

/*
 ****************************************************************
 *	Obtém a descrição do TIPO				*
 ****************************************************************
 */
const char *
get_type_name (int type)
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
const char *
get_class_name (int class)
{
	static char	area[24];

	if ((unsigned)class >= sizeof (class_desc) / sizeof (TYPE_DESC) - 1)
	{
		sprintf (area, "Unknown class: %d", class);
		return (area);
	}

	return (class_desc[class].c_name);

}	/* end get_class_name */

/*
 ****************************************************************
 *	Decodifica o código de retorno do servidor		*
 ****************************************************************
 */
char *
h_rc_text (enum DOMAIN_ERROR_CODE code)
{
	switch (code)
	{
	    case DOMAIN_NO_ERROR:
		return ("DOMAIN_NO_ERROR");

	    case DOMAIN_FORMAT_ERROR:
		return ("DOMAIN_FORMAT_ERROR");

	    case DOMAIN_SERVER_FAILURE:
		return ("DOMAIN_SERVER_FAILURE");

	    case DOMAIN_NAME_ERROR:
		return ("DOMAIN_NAME_ERROR");

	    case DOMAIN_NOT_IMPLEMENTED:
		return ("DOMAIN_NOT_IMPLEMENTED");

	    case DOMAIN_REFUSED:
		return ("DOMAIN_REFUSED");
	}

	return ("DOMAIN_???");

}	/* end h_rc_text */

#ifdef	DEBUG
/*
 ****************************************************************
 *	Decodifica o código de retorno para o KERNEL		*
 ****************************************************************
 */
char *
edit_kernel_code (enum DNS_ERROR_CODE code)
{
	switch (code)
	{
	    case DNS_NO_ERROR:
		return ("DNS_NO_ERROR");

	    case DNS_NAME_ERROR:
		return ("DNS_NAME_ERROR");

	    case DNS_SERVER_ERROR:
		return ("DNS_SERVER_ERROR");
	}

	return ("DNS_???");

}	/* end edit_kernel_code */
#endif	DEBUG

/*
 ****************************************************************
 *	Em caso de sinal, ...					*
 ****************************************************************
 */
void
on_signal (int signo, ...)
{
	kill (pidfilho, SIGHUP);

	exit (0);

}	/* on_signal */

/*
 ****************************************************************
 *	Imprime uma mensagem de erro (servidor)			*
 ****************************************************************
 */
void
error (const char *format, ...)
{
	va_list		args;
	const char	*fmt = format;
	FILE		*log_fp = NOFILE;
	char		err, abrt;
	int		event = 0, save_errno = errno;
	time_t		time_loc;
	static int	static_log_fd = 0;
	static FILE	*static_log_fp = NOFILE;

	va_start (args, format);

	/*
	 *	Procura o arquivo de "log"
	 */
	if   (error_msg_to_log == 0)
	{
		log_fp = stderr;
	}
	elif (static_log_fd == 0)
	{
		if ((static_log_fd = open ("/var/adm/itnetlog", O_WRONLY|O_APPEND)) >= 0)
		{
			if ((static_log_fp = log_fp = fdopen (static_log_fd, "w")) == NOFILE)
				static_log_fd = -1;
		}
	}
	else
	{
		log_fp = static_log_fp;
	}

	/*
	 *	Processa os argumentos da cadeia
	 */
	if (fmt == NOSTR)
		fmt = "$*Erro de entrada/saída de TCP";

	if ((abrt = *fmt) == '$')
		fmt++;

	if ((err = *fmt) == '*')
	{
		fmt++;

		if (save_errno == TLOOK)
			event = t_look (udp_fd);
	}

	/*
	 *	Escreve as mensagens
	 */
	if (log_fp != NOFILE)
	{
		/* Escreve a mensagem com o tempo */

		fprintf  (log_fp, pgname);

		time (&time_loc);
		fprintf (log_fp, " (%.24s)", btime (&time_loc));

		fprintf  (log_fp, ": ", pgname);
		vfprintf (log_fp, fmt, args);

		/* Escreve o tipo do erro */

		if (err == '*')
		{
			if (event != 0)
				fprintf (log_fp, " (%s)", t_strevent (udp_fd, event));
			elif (save_errno != 0)
				fprintf (log_fp, " (%s)", strerror (save_errno));
		}

		putc ('\n', log_fp); fflush (log_fp);
	}

	if (abrt == '$' || event ==  T_DISCONNECT)
		exit (1);

	va_end (args);

}	/* end error */

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
		"%s - consulta um servidor de DNS para obter um endereço\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-v]\n",
		pgname, pgversion, pgname
	);
	fprintf
	(	stderr,
		"\nOpções:"
		"\t-v: Verboso\n"
	);

	exit (2);

}	/* end help */
