/*
 ****************************************************************
 *								*
 *			echomon_gen.c				*
 *								*
 *	Teste de acesso a vários nós remotos			*
 *								*
 *	Versão	3.0.0, de 13.03.96				*
 *		3.0.0, de 03.05.96				*
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
#include <sys/times.h>
#include <sys/itnet.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <xti.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
#undef	DEBUG

#define	TIME_INTERVAL	5

const char	pgversion[] =  "Versão:	3.0.0, de 03.05.96";

#define	EVER	;;
#define	NOSTR   (char *)NULL
#define	elif	else if
#define STR(x)	# x
#define XSTR(x)	STR (x)

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
entry const char *pgname;	/* Nome do programa */

entry int	vflag;		/* Verbose */
entry int	wflag;		/* WAIT */

entry int	n_node;		/* No. de nós */

entry long	sender_pid,	/* No. da ID do emissário (usado no DG) */
		receiver_pid;	/* No. da ID do receptor */

/*
 ****** Variáveis globais ***************************************
 */
const char	*echomonfile = "echomonfile";

entry char	echomonlog[] = "/usr/pub/etc/echomonlogXXXX";

#define echomonlog_index 23

#if (0)	/*******************************************************/
const char	*echomonlog = "/usr/pub/etc/echomonlog";
#endif	/*******************************************************/

entry FILE	*log_fp;

entry int	time_interval = TIME_INTERVAL;

const char	raw_dev[] = "/dev/itnraw";

int		raw_fd;		/* fd do RAW */

/*
 ******	Tabela dos diversos nós *********************************
 */
#define	NNODE	100

typedef struct
{
	char	*n_name;	/* Nome do nó */
	char	*n_city;	/* Local do nó */
	int	n_index;	/* Número de ordem */
	IPADDR	n_addr;		/* Endereço do nó */
	int	n_snd;		/* No. de datagramas enviados */
	int	n_rcv;		/* No. de datagramas recebidos */
	long	n_time;		/* Último tempo */

}	NODE;

entry NODE	node_status[NNODE + 1];

/*
 ****** Estrutura do datagrama enviado **************************
 */
typedef struct
{
	ICMP_H	h;	/* Cabeçalho ICMP */
	MUTM	t;	/* Tempo de envio */

}	DG;

/*
 ****** Protótipos de funções ***********************************
 */
void		receiver (void);
void		sender (void);
void		log (void);
long		compute_time_interval (MUTM *, MUTM *);
void		sig_catch (int, void *, void *);
void		help (void);

extern int	compute_checksum (const void *, int);

/*
 ****************************************************************
 *	Teste de acesso a um nó remoto				*
 ****************************************************************
 */
main (int argc, register const char *argv[])
{
	register int	opt;
	register FILE	*fp;
	register NODE	*np;
	const char	*time_str = NOSTR, *str;
	char		*cp;
	INADDR		addr;
	T_BIND		bind;
	int		i;
#if (0)	/*******************************************************/
	int		error_code = 0;
#endif	/*******************************************************/
	char		area[120];

	/*
	 *	Pequena inicialização
	 */
	pgname = argv[0];

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "vwt:f:H")) != EOF)
	{
		switch (opt)
		{
		    case 'v':			/* Verbose */
			vflag++;
			break;

		    case 'w':			/* Wait */
			wflag++;
			break;

		    case 'f':			/* Pingfile */
			echomonfile = optarg;
			break;

		    case 't':			/* Interval time */
			time_str = optarg;
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

	if (argc != 0)
		help ();

	/*
	 *	Retira o tempo dado
	 */
	if (time_str != NOSTR && ((time_interval = strtol (time_str, &str, 0)) < 1 || *str != '\0'))
	{
		fprintf
		(	stderr,
			"%s: Intervalo de tempo inválido: \"%s\"\n",
			pgname, time_str
		);
		exit (1);
	}

	/*
	 *	Verifica se deve esperar
	 */
	if (wflag)
		sleep (5 * 60);

	/*
	 *	Abre o "endpoint"
	 */
	if ((raw_fd = t_open (raw_dev, O_RDWR, (T_INFO *)NULL)) < 0)
	{
		fprintf
		(	stderr,
			"%s: Não consegui abrir \"%s\" (%s)\n",
			pgname, raw_dev, strerror (errno)
		);
		exit (1);
	}

	/*
	 *	Abre o "echomonfile"
	 */
	if ((fp = fopen (echomonfile, "r")) == NOFILE)
	{
		fprintf
		(	stderr,
			"%s: Não consegui abrir \"%s\" (%s)\n",
			pgname, echomonfile, strerror (errno)
		);
		exit (1);
	}

	/*
	 *	Obtém os endereços do nós remotos
	 */
	for (np = node_status; fngets (area, sizeof (area), fp) == area; /* abaixo*/)
	{
		if (area[0] == '\0' || area[0] == '#')
			continue;

		if (np >= &node_status[NNODE])
		{
			fprintf
			(	stderr,
				"%s: O arquivo \"%s\" contém nós demais\n",
				pgname, echomonfile
			);
			exit (1);
		}

		if ((np->n_name = strdup (strtok (area, "\t"))) == NOSTR)
		{
		    no_mem:
			fprintf
			(	stderr,
				"%s: Memória esgotada\n",
				pgname
			);
			exit (1);
		}

		np->n_city = "";

		if ((cp = strtok (NOSTR, "\t")) != NOSTR)
		{
			if ((np->n_city = strdup (cp)) == NOSTR)
				goto no_mem;
		}

		if ((np->n_addr = t_node_to_addr (raw_fd, np->n_name, (long *)NULL)) == -1)
		{
			fprintf
			(	stderr,
				"%s: Não consegui obter o endereço de \"%s\" (%s)\n",
				pgname, np->n_name, strerror (errno)
			);
		   	np->n_name = NOSTR;	/* Não avança o ponteiro */
			continue;
#if (0)	/*******************************************************/
			error_code++; continue;
#endif	/*******************************************************/
		}

	   	np->n_index = n_node;
	   /***	np->n_addr = ... /* acima ***/
	   /***	np->n_snd  = 0; ***/
	   /***	np->n_rcv  = 0; ***/

#ifdef	DEBUG
		printf ("\"%s\" => %s\n", np->n_name, t_addr_to_str (np->n_addr));
#endif	DEBUG
		np++; n_node++;		/* Obteve um nó */

	}	/* end (lendo o arquivo "echomonfile") */

#if (0)	/*******************************************************/
	if (error_code != 0)
		exit (error_code);
#endif	/*******************************************************/

   /***	np->n_name = NOSTR; ***/	/* Marca o final da tabela */

	/*
	 *	Verifica se já existe o arquivo de registro
	 */
	if (access (echomonlog, F_OK) == 0)
	{
		fprintf
		(	stderr,
			"%s: O arquivo \"%s\" já existe. Reescreve? (n): ",
			pgname, echomonlog
		);

		if (askyesno (stdin, 0) <= 0)
			exit (0);
	}

	/*
	 *	Acha um Nome para "log"
	 */
	for (i = 0; /* abaixo */; i++)
	{
		sprintf (echomonlog + echomonlog_index, "%04d", i);

		if (access (echomonlog, F_OK) < 0)
			break;
	}

#if (0)	/*******************************************************/
entry char	echomonlog[] = "/usr/pub/etc/echomonlogXXXX";
#define echomonlog_index 23
#endif	/*******************************************************/

	/*
	 *	Cria o arquivo de registro
	 */
	if ((log_fp = fopen (echomonlog, "w")) == NOFILE)
	{
		fprintf
		(	stderr,
			"%s: Não consegui criar o arquivo de registro (%s)\n",
			pgname, strerror (errno)
		);
		exit (1);
	}

	/*
	 *	Escreve o nome do próprio nó no arquivo de registro
	 */
	fnputs (t_addr_to_node (raw_fd, 0), log_fp);
	
	/*
	 *	Escreve os nomes dos nós no arquivo de registro
	 */
	putw (n_node, log_fp);

	for (np = node_status; np < &node_status[NNODE]; np++)
	{
		if (np->n_name == NOSTR)
			break;

		fnputs (np->n_name, log_fp);
		fnputs (np->n_city, log_fp);
		putw   (np->n_addr, log_fp);
	}

	fflush (log_fp);

	/*
	 *	Associa um endereço
	 */
	addr.a_addr  = 0;	/* A consistência do endereço é feita abaixo */
	addr.a_proto = ICMP_PROTO;

	bind.addr.len	 = sizeof (INADDR);
	bind.addr.maxlen = sizeof (INADDR);
	bind.addr.buf	 = &addr;

	if (t_bind (raw_fd, &bind, &bind) < 0)
	{
		fprintf
		(	stderr,
			"%s: Erro no T_BIND (%s)\n",
			pgname, strerror (errno)
		);
		exit (1);
	}

	/*
	 *	Cria um filho para liberar o "sh"
	 */
	if ((sender_pid = thread ()) < 0)
	{
		fprintf
		(	stderr,
			"%s: Não consegui dar o thread (%s)\n",
			pgname, strerror (errno)
		);
		exit (1);
	}

	if (sender_pid > 0)
		_exit (0);

	/*
	 *	Espera até o início do minuto ou hora, ...
	 */
	if (time_interval <= 60)
		sleep ((1 * 60) - time ((time_t *)NULL) % (1 * 60));
	else
		sleep ((5 * 60) - time ((time_t *)NULL) % (5 * 60));

	/*
	 *	Prepara o término do programa
	 */
	signal (SIGINT,  SIG_IGN);
	signal (SIGTERM, sig_catch);

	/*
	 *	Cria um filho para receber os datagramas de ECHO
	 */
	if ((receiver_pid = thread ()) < 0)
	{
		fprintf
		(	stderr,
			"%s: Não consegui dar o thread (%s)\n",
			pgname, strerror (errno)
		);
		exit (1);
	}

	if (receiver_pid == 0)
		receiver ();

	/*
	 *	O processo pai original enviar os datagramas
	 */
	sender ();

	exit (0);

}	/* end echomon_gen */

/*
 ****************************************************************
 *	Processo para enviar os pedidos de ECHO			*
 ****************************************************************
 */
void
sender (void)
{
	register NODE		*np;
	T_UNITDATA		unitdata;
	DG			dg;
	INADDR			addr;
	int			seq = 0;

	/*
	 *	Repare no uso cuidado das variáveis globais (thread)
	 *
	 *	Prepara o datagrama ICMP de pedido de echo
	 */
	dg.h.ih_type	 = ICMP_ECHO_REQ;
	dg.h.ih_code	 = 0;
   /***	dg.h.ih_checksum = 0; ***/
	dg.h.u.s.ih_id	 = ENDIAN_SHORT (sender_pid);
   /***	dg.h.u.s.ih_seq  = ...; /* abaixo ***/

	/*
	 *	Manda o datagrama RAW
	 */
   /***	addr.a_addr  = ...; /* abaixo ***/
	addr.a_proto = ICMP_PROTO;

	unitdata.addr.len	= sizeof (addr);
   	unitdata.addr.buf	= &addr;

	unitdata.udata.len	= sizeof (dg);
	unitdata.udata.buf	= &dg;

	for (EVER)
	{
	   /***	mutime (&dg.t); ***/
		dg.h.u.s.ih_seq  = ENDIAN_SHORT (seq++);
		dg.h.ih_checksum = 0;
	   /***	dg.h.ih_checksum = compute_checksum (&dg, sizeof (dg)); ***/

#if (0)	/*******************************************************/
	   	mutime (&dg.t);
		dg.h.u.s.ih_seq  = ENDIAN_SHORT (seq++);
		dg.h.ih_checksum = 0;
	   	dg.h.ih_checksum = compute_checksum (&dg, sizeof (dg));
#endif	/*******************************************************/

		for (np = node_status; np < &node_status[NNODE]; np++)
		{
			if (np->n_name == NOSTR)
				break;

			mutime (&dg.t);
			dg.h.ih_checksum = 0;
			dg.h.ih_checksum = compute_checksum (&dg, sizeof (dg));
#if (0)	/*******************************************************/
#endif	/*******************************************************/

		   	addr.a_addr = np->n_addr;

			if (t_sndraw (raw_fd, &unitdata) < 0)
			{
				fprintf
				(	stderr,
					"%s: Erro no SNDRAW (%s)\n",
					pgname, strerror (errno)
				);
				kill (receiver_pid, SIGTERM);
				_exit (1);
			}

		   	np->n_snd++;

		}	/* end for (os nós) */

		sleep (time_interval);

		log ();

	}	/* end for (EVER) */

}	/* end sender */

/*
 ****************************************************************
 *	Processo para receber as respostas dos pedidos de ECHO	*
 ****************************************************************
 */
void
receiver (void)
{
	register NODE	*np;
	T_UNITDATA	unitdata;
	union { DG dg; char big_area[128]; } u;
	int		flag;
	INADDR		rcv_addr;
	MUTM		end_time;

	/*
	 *	Recebe os datagramas RAW
	 */
	unitdata.addr.maxlen	= sizeof (rcv_addr);
	unitdata.addr.buf	= &rcv_addr;

	unitdata.udata.maxlen	= sizeof (u);
	unitdata.udata.buf	= &u;

	/*
	 *	Recebe os datagramas
	 */
	for (EVER)
	{
		if (t_rcvraw (raw_fd, &unitdata, &flag) < 0)
		{
			fprintf
			(	stdout,
				"%s: Erro no RCVRAW (%s)\n",
				pgname, strerror (errno)
			);
			kill (sender_pid,   SIGTERM);
			_exit (1);
		}

		mutime (&end_time);

		if (u.dg.h.ih_type == ICMP_ECHO_REQ)	/* Ping para 0.0.0.0 */
			continue;

		if (u.dg.h.ih_type != ICMP_ECHO_REPLY)
			continue;

		/*
		 *	Calcula o checksum e outras consistências
		 */
		if (unitdata.udata.len != sizeof (u.dg))
			continue;

		if (compute_checksum (&u.dg, sizeof (u.dg)) != 0)
			continue;

		if (ENDIAN_SHORT (u.dg.h.u.s.ih_id) != sender_pid)
			continue;

		/*
		 *	Verifica de qual nó vem o ECHO REPLY
		 */
		for (np = node_status; np < &node_status[NNODE]; np++)
		{
			if (np->n_name == NOSTR)
				break;

			if (np->n_addr == rcv_addr.a_addr)
				goto found;
		}

		continue;

		/*
		 *	Computa os dados deste nó
		 */
	    found:
	   	np->n_rcv++;
	   	np->n_time = compute_time_interval (&u.dg.t, &end_time);

	}	/* end for (EVER) */

}	/* end receiver */

/*
 ****************************************************************
 *	Processo para gerar o arquivo de registro		*
 ****************************************************************
 */
void
log (void)
{
	register FILE		*fp = log_fp;
	register NODE		*np;

	/*
	 *	Escreve o tempo
	 */
	putw (time ((time_t *)NULL), fp);

	/*
	 *	Escreve a tabela
	 */
	for (np = node_status; np < &node_status[NNODE]; np++)
	{
		if (np->n_name == NOSTR)
			break;

		if (np->n_rcv == 0)
			putw (-1, fp);
		else
			putw (np->n_time, fp);

		np->n_rcv = 0;
	}

	fflush (fp);

}	/* end log */

/*
 ****************************************************************
 *	Calcula o tempo de transferência			*
 ****************************************************************
 */
long
compute_time_interval (register MUTM *begin_time, register MUTM *end_time)
{
	register time_t	TIME, UTIME;

	/*
	 *	Faz a diferença de tempos - cuidado com o "carry"
	 */
	TIME = end_time->mu_time - begin_time->mu_time;

	if ((UTIME = end_time->mu_utime - begin_time->mu_utime) < 0)
	{
		TIME  -= 1;		/* Tira um segundo daqui */
		UTIME += 1000000;	/* E compensa aqui */
	}

	return (TIME * 1000 + (UTIME + 500) / 1000);

}	/* end compute_and_print_time_interval */

/*
 ****************************************************************
 *	Intercepta alguns sinais				*
 ****************************************************************
 */
void
sig_catch (int signo, void *pc, void *fa)
{
	kill (sender_pid, SIGTERM);
	kill (receiver_pid, SIGTERM);

	_exit (0);

}	/* end sig_catch */

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
		"%s - teste de acesso a vários nós remotos\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-vw] [-t <intervalo>] [-f <file>]\n",
		pgname, pgversion, pgname
	);
	fprintf
	(	stderr,
		"\nOpções:"
		"\t-v: Verboso\n"
		"\t-w: Só inicia o programa após 5 minutos\n"
		"\t-t: Envia datagramas a cada <intervalo> seg., ao invés "
			"de " XSTR (TIME_INTERVAL) " seg.\n"
		"\t-f: Le os nomes dos nós do arquivo <file> ao invés "
			"de \"echomonfile\"\n"
	);

	exit (2);

}	/* end help */
