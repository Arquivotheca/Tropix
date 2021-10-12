/*
 ****************************************************************
 *								*
 *			pop3_s.c				*
 *								*
 *	Post Office Protocol - Version 3 (servidor)		*
 *								*
 *	Versão	3.0.0, de 15.08.98				*
 *		4.0.0, de 11.08.01				*
 *								*
 *	Módulo: POP3_S						*
 *		Utilitários de comunicações			*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/syscall.h>
#include <sys/utsname.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <pwd.h>
#include <fcntl.h>
#include <ctype.h>
#include <time.h>
#include <errno.h>
#include <xti.h>

#include "../h/pop3_s.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char		pgversion[] =  "Versão:	4.0.0, de 11.08.01";

entry int		vflag;			/* Verbose */

entry int		global_log_fd;		/* Para "error" */
entry FILE		*global_log_fp;

entry char		tcp_dev_nm[] = "/dev/itntcp"; /* Nome do dispositivo TCP */

entry int		tcp_fd;		/* Conexão de controle */
entry FILE		*tcp_fp;		/* Conexão de controle */

entry char		snd_dis;		/* Previsão para em erro grave enviar DIS */

entry STATE		server_state = S_RESET;	/* Estado atual do servidor */

entry const char	*client_node_nm;	/* Nome do nó do cliente */

/*
 ****************************************************************
 *	Programa Principal					*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	int		opt, n;
	const char	*str;
	UTSNAME		uts;
	INADDR		client_addr;
	INADDR		serv_addr[2];
	T_BIND		bind;
	T_CALL		call;
	int		listen_port, listen_fd, sequence;

	/*
	 *	Prepara variáveis de "error" ainda como superusuário
	 */
	if ((global_log_fd = open ("/var/adm/itnetlog", O_WRONLY|O_APPEND)) >= 0)
	{
		if ((global_log_fp = fdopen (global_log_fd, "w")) == NOFILE)
			global_log_fd = -1;
	}

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "v")) != EOF)
	{
	   	switch (opt)
		{
		    case 'v':			/* Verbose */
			 vflag++;
			 break;

		}	/* end switch */

	}	/* end while */

	argv += optind;
	argc -= optind;

	/*
	 *	Obtém o UTS do sistema
	 */
	if (uname (&uts) < 0)
		error ("$*Não consegui obter o UTS\n");

	/*
	 *  tpop3_s [-v] <listen_port> <listen_fd>
	 *		 		<client_addr> <client_port> <sequence>
	 */
#ifdef	DEBUG
	if (Bflag) error
	(	"%s: %s <%s> <%s> <%s> <%s> <%s>",
		pgname, vflag ? "-v" : "",
		argv[0], argv[1], argv[2], argv[3], argv[4]
	);
#endif	DEBUG

	if (argc != 5) error
	(	"$%s: Número inválido de argumentos: %d",
		pgname, argc
	);

	listen_port	   = strtol (argv[0], &str, 0);
	listen_fd	   = strtol (argv[1], &str, 0);
	client_addr.a_addr = t_str_to_addr (argv[2]);
	client_addr.a_port = strtol (argv[3], &str, 0);
	sequence	   = strtol (argv[4], &str, 0);

	if ((tcp_fd = t_open (tcp_dev_nm, O_RDWR, (T_INFO *)NULL)) < 0)
		error ("$*Não consegui abrir \"%s\"", tcp_dev_nm);

	if ((tcp_fp = fdopen (tcp_fd, "w")) == NOFILE)
		error ("$*Não consegui obter um FILE para \"%s\"", tcp_dev_nm);

	/*
	 *	Vou realizar o "bind"
	 */
	serv_addr[0].a_port = listen_port;
	serv_addr[1].a_addr = client_addr.a_addr;
	serv_addr[1].a_port = client_addr.a_port;

	bind.addr.maxlen = 0;
	bind.addr.len	 = 2 * sizeof (INADDR);
	bind.addr.buf	 = serv_addr;

	bind.qlen	 = 0;

	if (t_bind (tcp_fd, &bind, (T_BIND *)NULL) < 0)
		error ("$*Não consegui atribuir endereço");

	/*
	 *	Vou realizar o "accept"
	 */
   /***	call.opt.len = 0; ***/
   /***	call.udata.len = 0; ***/
	call.sequence = sequence;

	call.addr.len	 = sizeof (INADDR);
	call.addr.maxlen = sizeof (INADDR);
	call.addr.buf	 = &client_addr;

#ifdef	DEBUG
	if (Bflag) error
	(	"Vou dar accept para (%s, %d)",
		t_addr_to_str (client_addr.a_addr), client_addr.a_port
	);
#endif	DEBUG

	if (t_accept (listen_fd, tcp_fd, &call) < 0)
		error ("$*Erro no t_accept");

	/*
	 *	Agora não precisa mais do "listen_fd"
	 */
	t_close (listen_fd);

	snd_dis++;

	if ((str = t_addr_to_node (tcp_fd, client_addr.a_addr)) == NOSTR)
		str = t_addr_to_str (client_addr.a_addr);

	if ((client_node_nm = strdup (str)) == NOSTR)
		error ("$*Memória esgotada");

	reply
	(	"+*%s (%s, %s) Servidor POP3 (%s)",
		uts.uts_sysname, uts.uts_machine, uts.uts_nodename, pgversion
	);

	/*
	 *	Lê os comandos do cliente e processa
	 */
	for (EVER)
	{
		char	comando[BLSZ];

		if ((n = t_rcv (tcp_fd, comando, sizeof (comando), (int *)NULL)) < 0)
			error (NOSTR);
#ifdef	DEBUG
		{
			char		line[BLSZ];
			const char	*ip = comando;
			char		*op = line;
			char		c;
			int		i;

			for (i = n; i > 0; i--)
			{
				*op++ = c = *ip++;

				if (c == '\r' || c == '\n' || c == '\0')
					{ op--; break; }
			}

			*op = '\0';

			error ("Recebi \"%s\"", line);
		}
#endif	DEBUG
		if (n <= 0)
			strcpy (comando, "HELP\r\n");
		else
			comando[n] = '\0';	/* Para marcar o final da linha */

		parse (comando);

	}	/* end for (EVER) */

}	/* end main */

/*
 ****************************************************************
 *	Analisador de  Comandos					*
 ****************************************************************
 */
void
parse (const char *comando)
{
	const CMDTB		*tp;
	const char		*cp;
	int			argc;
	const char		*argv[10];

	/*
	 *	Procura o comando na tabela 
	 */
	cp = strtoupper (strtok ((char *)comando, " \t\r\n"));

	if (cp == NOSTR || cp[0] == '\0' || cp[0] == 4 /* EOT */)
		cp = "QUIT";

	for (tp = cmdtab; /* teste abaixo */; tp++)
	{
		if (cp == NOSTR || tp->c_nome == NOSTR)
		{
			reply ("-*Comando \"%s\" desconhecido", cp);
			return;
		}

		if (streq (cp, tp->c_nome))
			break;
	}

	/*
	 *	Retira os argumentos
	 */
	for (argc = 0; (cp = strtok (NOSTR, " \t\r\n")) != NOSTR; argc++)
		argv[argc] = cp;

	argv[argc] = NOSTR;

#ifdef	DEBUG
	{
		int i;

		printf ("cmd_parse: (%d) \"%s", argc, tp->c_nome);

		for (i = 0; i < argc; i++)
			printf (" %s", argv[i]);

		printf ("\"\n");
	}
#endif	DEBUG

	/*
	 *	Executa a função pedida
	 */
	(*tp->c_funcao) (argc, argv);

}	/* end parse */

/*
 ****************************************************************
 *	Verifica se está no estado READY			*
 ****************************************************************
 */
int
check_ready_state (void)
{
	if (server_state == S_READY)
		return (0);

	reply ("-*Este comando só pode ser executado com um usuário em sessão");

	return (-1);

}	/* end check_ready_state */

/*
 ****************************************************************
 *	Converte uma cadeia para maiúsculas			*
 ****************************************************************
 */
char *
strtoupper (char *str)
{
	char *cp = str;

	if (cp == NOSTR)
		return (NOSTR);

	for (/* vazio */; *cp != '\0'; cp++)
		*cp = toupper (*cp);

	return (str);

}	/* end strtoupper */

/*
 ****************************************************************
 *	Envia uma mensagem normal para o cliente		*
 ****************************************************************
 */
void
reply (const char *format, ...)
{
	va_list		args;
	char		push = 0;

	va_start (args, format);

	if   (*format == '+')
		{ fprintf  (tcp_fp, "+OK ");  format++; }
	elif (*format == '-')
		{ fprintf  (tcp_fp, "-ERR "); format++; }

	if   (*format == '*')
		{ push++; format++; }

	vfprintf (tcp_fp, format, args); fprintf (tcp_fp, "\r\n");

	if (push)
		{ fflush (tcp_fp); t_push (tcp_fd); }

	va_end (args);

}	/* end reply */

/*
 ****************************************************************
 *	Imprime uma mensagem de erro (servidor)			*
 ****************************************************************
 */
void
error (const char *format, ...)
{
	register va_list	 args;
	register const char	*fmt = format;
	register FILE		*log_fp;
	char			err, abrt;
	int			event = 0, save_errno = errno;
	time_t			time_loc;
	static int		static_log_fd = 0;
	static FILE		*static_log_fp = NOFILE;

	va_start (args, format);

	/*
	 *	Procura o arquivo de "log"
	 */
	if (static_log_fd == 0)
	{
		if ((static_log_fd = open ("/var/adm/itnetlog", O_WRONLY|O_APPEND)) >= 0)
		{
			if ((static_log_fp = fdopen (static_log_fd, "w")) == NOFILE)
				static_log_fd = -1;
		}
	}

	log_fp = static_log_fp;

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
			event = t_look (tcp_fd);
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
			if   (event != 0)
				fprintf (log_fp, " (%s)", t_strevent (tcp_fd, event));
			elif (save_errno != 0)
				fprintf (log_fp, " (%s)", strerror (save_errno));
		}

		putc ('\n', log_fp); fflush (log_fp);
	}

	if (abrt == '$' || event ==  T_DISCONNECT)
		tcp_abort (1);

	va_end (args);

}	/* end error */

/*
 ****************************************************************
 *	Encerra as atividades (desconectando o TCP)		*
 ****************************************************************
 */
void
tcp_abort (int n)
{
	if (snd_dis)
		t_snddis (tcp_fd, (T_CALL *)NULL);

	exit (n);

}	/* end tcp_abort */
