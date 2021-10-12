/*
 ****************************************************************
 *								*
 *			serv_daemon.c				*
 *								*
 *	Um esqueleto de servidor				*
 *								*
 *	Versão	2.3.0, de 08.08.91				*
 *		4.9.0, de 23.08.06				*
 *								*
 *	Módulo: Internet					*
 *		Utilitários especiais				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 * 		Copyright © 2006 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/common.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <signal.h>
#include <errno.h>
#include <xti.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
#if (0)	/*************************************/
const char	pgversion[] =  "Versão: 3.0.6, de 22.08.97";
#endif	/*************************************/

#define	NOVOID	(void *)NULL
#define	elif	else if
#define	EVER	;;
#define STR(x)	# x
#define XSTR(x)	STR (x)

/*
 ****** Variáveis diversas **************************************
 */
entry int	vflag;		/* Verbose */

entry int	exit_code;	/* Código de retorno */

entry long	udp_pid;	/* No. do processo, se for UDP */

const char	tcp_dev_nm[] = "/dev/itntcp";

entry int	global_listen_fd; /* Para "error" */

entry char	error_msg_to_log; /* Não nulo se mensagens para "itnetlog" */

/*
 ****** Protótipos de funções ***********************************
 */
void		udp_serv (int, const char *[], const char **);
void		tcp_serv (int, const char *[], const char **, int max_conn_no);
void		on_SIGTERM (int, ...);
void		error (const char *, ...);

/*
 ****************************************************************
 *	Um esqueleto de servidor				*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	int		opt;
	int		well_known_port_no;
	int		proto = 0;
	const char	*str, **cmdvp;
	const char	*max_conn_str = NOSTR;
	int		max_conn_no = 0;
	enum		{ N_CMDARG = 32 };
	const char	*cmdv[N_CMDARG];

	error_msg_to_log++;

	/*
	 *	Verifica se é SUPERUSUÁRIO
	 */
	if (geteuid () != 0)
		error ("$O usuário efetivo não é SUPERUSUÁRIO");

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "n:v")) != EOF)
	{
		switch (opt)
		{
		    case 'n':			/* No. máximo de conexões */
			max_conn_str = optarg;
			break;

		    case 'v':			/* Verbose */
			vflag++;
			break;

		}	/* end switch */

	}	/* end while */

	argv += optind;
	argc -= optind;

	/*
	 *		arg[i]:	 	     0		     1		     2
	 *
	 *				    "tcp"
	 *	serv_daemon [-n <max_conn>]	  <well_known_port_no_nm> <serv_cmd>
	 *				    "udp"
	 */
	if (vflag || argc != 3)
	{
		if (!vflag)
			error ("Número inválido de argumentos: %d", argc);

		error
		(	"%s <%s> <%s> <%s>",
			vflag ? "-v" : "", argv[0], argv[1], argv[2]
		);
	}

	if (max_conn_str != NOSTR)
	{
		if ((max_conn_no = strtol (max_conn_str, &str, 0)) < 0 || *str != '\0')
		{
			error ("No. máximo de conexões inválido: \"%s\"", max_conn_str);
			max_conn_no = 0;
		}
	}

	if   (streq (argv[0], "udp"))
	{
		proto = 'u';
		signal (SIGTERM, on_SIGTERM);
	}
	elif (streq (argv[0], "tcp"))
	{
		proto = 't';
	}
	else
	{
		error ("$Protocolo inválido: \"%s\"", argv[0]);
	}

	if ((well_known_port_no = atoi (argv[1])) <= 0)
		error ("$No. do \"port\" inválido: %s", argv[1]);

	/*
	 *	Desmembra os argumentos do servidor
	 */
	str = strtok ((char *)argv[2], "\t ");

	for (cmdvp = cmdv; str != NOSTR; cmdvp++, str = strtok (NOSTR, "\t "))
	{
		if (cmdvp >= &cmdv[N_CMDARG - 6])	/* Reserva 6 ponteiros */
			break;

		*cmdvp = str;
	}

	/*
	 *	Executa o servidor
	 */
	if (access (cmdv[0], X_OK) < 0)
		error ("$*O servidor \"%s\" não é executável", cmdv[0]);

	if (proto == 'u')
		udp_serv (well_known_port_no, cmdv, cmdvp);
	else
		tcp_serv (well_known_port_no, cmdv, cmdvp, max_conn_no);

	return (exit_code);

}	/* end serv_daemon */

/*
 ****************************************************************
 *	Servidor UDP						*
 ****************************************************************
 */
void
udp_serv (int well_known_port_no, const char *cmdv[], const char **cmdvp)
{
	long		i;
	char		port_nm[6];
	const char	*last_id, *serv_nm;

	/*
	 *	Fecha os arquivos não mais utilizados
	 */
	for (i = 3; i < NUFILE; i++)
		close (i);

	/*
	 *	Executa o servidor
	 */
	for (EVER)
	{
		if ((udp_pid = fork ()) < 0)
			error ("$*Não consegui dar \"fork\"");

		if (udp_pid == 0)
			break;

		for (EVER)
		{
			if ((i = wait ((int *)NULL)) < 0)
				error ("$*Erro no \"wait\"");

			if (i == udp_pid)
				break;
		}

	}	/* for (EVER) */

	/*
	 *	Prepara os conteúdos de "argv"
	 */
	if ((last_id = strrchr (cmdv[0], '/')) == NOSTR)
		last_id = cmdv[0];
	else
		last_id++;

	/*
	 *	Prepara os "argv"
	 */
	serv_nm = cmdv[0]; cmdv[0] = last_id;

	sprintf (port_nm, "%d", well_known_port_no);

	*cmdvp++ = "udp";
	*cmdvp++ = port_nm;
	*cmdvp++ = NOSTR;

	/*
	 *	Passa o controle para o servidor de verdade
	 *
	 *	<last_id> [-opções] "udp" <well_known_port>
	 */
	execv (serv_nm, cmdv);

	error ("$*Não consegui dar \"exec\" para \"%s\"", serv_nm);

}	/* end udp_serv */

/*
 ****************************************************************
 *	Servidor TCP						*
 ****************************************************************
 */
void
tcp_serv (int well_known_port_no, const char *cmdv[], const char **cmdvp, int max_conn_no)
{
	long		pid, i;
	int		listen_fd;
	INADDR		serv_addr, client_addr;
	T_BIND		bind;
	T_CALL		call;
	char		listen_port_nm[6], listen_fd_nm[6];
	char		addr_nm[16], port_nm[6], seq_nm[6];
	const char	*last_id, *serv_nm;
	T_OPTMGMT	req_optmgmt, ret_optmgmt;
	TCP_OPTIONS	options;

	/*
	 *	Abre o dispositivo
	 */
	if ((listen_fd = t_open (tcp_dev_nm, O_RDWR, (T_INFO *)NULL)) < 0)
		error ("$*Não consegui abrir o dispositivo \"%s\"", tcp_dev_nm);

	global_listen_fd = listen_fd;

	serv_addr.a_port = well_known_port_no;
	serv_addr.a_addr = 0;

	bind.addr.maxlen = sizeof (INADDR);
	bind.addr.len	 = sizeof (INADDR);
	bind.addr.buf	 = &serv_addr;

	bind.qlen	 = 10;

	if (t_bind (listen_fd, &bind, &bind) < 0)
		error ("$*Não consegui atribuir endereço");

	if (vflag)
		error ("QLEN negociado = %d", bind.qlen);

	/*
	 *	Negocia o tamanho da janela e o número máximo de conexões por cliente
	 */
	memset (&options, 0, sizeof (TCP_OPTIONS));

	req_optmgmt.flags      = T_CHECK;

	req_optmgmt.opt.buf    = &options;
	req_optmgmt.opt.len    = sizeof (TCP_OPTIONS);

	ret_optmgmt.opt.buf    = &options;
	ret_optmgmt.opt.maxlen = sizeof (TCP_OPTIONS);

	if (t_optmgmt (listen_fd, &req_optmgmt, &ret_optmgmt) < 0)
		error ("$*Não consegui obter as opções TCP");

	req_optmgmt.flags    = T_NEGOTIATE;

   /***	options.max_wait     = 0;	/* Sem limite */
   /***	options.max_silence  = 0;	/* Sem limite */
   	options.window_size  = 1024;

	if (max_conn_no != 0)
		options.max_client_conn = max_conn_no;

   /***	req_optmgmt.opt.buf    = &options; ***/
   /***	req_optmgmt.opt.len    = sizeof (TCP_OPTIONS); ***/

	ret_optmgmt.opt.buf    = &options;
	ret_optmgmt.opt.maxlen = sizeof (TCP_OPTIONS);

	if (t_optmgmt (listen_fd, &req_optmgmt, &ret_optmgmt) < 0)
		error ("$*Não consegui negociar as opções TCP");

	/*
	 *	Fecha os arquivos não mais utilizados
	 */
	for (i = 3; i < NUFILE; i++)
	{
		if (i != listen_fd)
			close (i);
	}

	/*
	 *	Um "esqueleto", esperando um pedido de conexão
	 */
	call.addr.maxlen = sizeof (INADDR);
	call.addr.buf = &client_addr;

   /***	call.opt.len = 0; ***/
   /***	call.udata.len = 0; ***/

	/*
	 *	Espera clientes pedirem conexão
	 */
	for (EVER)
	{
		if (t_listen (listen_fd, &call) < 0)
			error ("$*Erro no \"t_listen\"");
	
		/*
		 *	Quando chegar um pedido de conexão, o filho
		 *	atende e o pai continua esperando novas conexões
		 */
		if ((pid = fork ()) < 0)
			error ("$*Não consegui dar \"fork\"");

		if (pid == 0)
			break;

	}	/* for (EVER) */

	if (vflag) error
	(	"Recebi um pedido de conexão de (%s, %d), seq = %d",
		t_addr_to_str (client_addr.a_addr), client_addr.a_port,
		call.sequence
	);

	/*
	 *	O pai passa a ser o INIT
	 */
	if (setppid (0) < 0)
		error ("$*Não consegui dar \"setppid\"");

	/*
	 *	Prepara os conteúdos de "argv"
	 */
	if ((last_id = strrchr (cmdv[0], '/')) == NOSTR)
		last_id = cmdv[0];
	else
		last_id++;

	sprintf (listen_port_nm, "%d", well_known_port_no);
	sprintf (listen_fd_nm, "%d", listen_fd);
	sprintf (addr_nm, "%s", t_addr_to_str (client_addr.a_addr));
	sprintf (port_nm, "%d", client_addr.a_port);
	sprintf (seq_nm, "%d", call.sequence);

	/*
	 *	Prepara os "argv"
	 */
	serv_nm = cmdv[0]; cmdv[0] = last_id;

	*cmdvp++ = listen_port_nm;
	*cmdvp++ = listen_fd_nm;
	*cmdvp++ = addr_nm;
	*cmdvp++ = port_nm;
	*cmdvp++ = seq_nm;
	*cmdvp++ = NOSTR;

	/*
	 *	Passa o controle para o servidor de verdade
	 *
	 *   <last_id> [-opções] <listen_port> <listen_fd> <addr> <port> <sequence>
	 */
	execv (serv_nm, cmdv);

	error ("*Não consegui dar \"exec\" para \"%s\"", serv_nm);

	if (t_snddis (listen_fd, &call) < 0)
		error ("*Erro no \"t_snddis\"");

	exit (1);

}	/* end tcp_serv */

/*
 ****************************************************************
 *	Recebe o sinal SIGTERM					*
 ****************************************************************
 */
void
on_SIGTERM (int signo, ...)
{
	if (udp_pid)
		kill (udp_pid, SIGTERM);

	exit (1);

}	/* end on_SIGTERM */

/*
 ****************************************************************
 *	Imprime uma mensagem de erro (servidor)			*
 ****************************************************************
 */
void
error (const char *format, ...)
{
	va_list		 args;
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
			event = t_look (global_listen_fd);
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
				fprintf (log_fp, " (%s)", t_strevent (global_listen_fd, event));
			elif (save_errno != 0)
				fprintf (log_fp, " (%s)", strerror (save_errno));
		}

		putc ('\n', log_fp); fflush (log_fp);
	}

	if (abrt == '$' || event ==  T_DISCONNECT)
		exit (1);

	va_end (args);

}	/* end error */
