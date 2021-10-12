/*
 ****************************************************************
 *								*
 *			rlogin_s.c				*
 *								*
 *	Login remoto (servidor)					*
 *								*
 *	Versão	3.0.0, de 14.11.92				*
 *		4.6.0, de 16.07.04				*
 *								*
 *	Módulo: RLOGIN						*
 *		Utilitários de comunicações			*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2004 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/common.h>
#include <sys/utsname.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include <fcntl.h>
#include <init.h>
#include <time.h>
#include <termio.h>
#include <utmp.h>
#include <errno.h>
#include <signal.h>
#include <xti.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "V4.6.0, de 16.07.04";

#define	EVER	;;
#define	elif	else if
#define	PWSZ	13

#define	WIN_SZ	12		/* Tamanho da informação de janela */

entry const char *pgname;	/* Nome do programa */

const char	tcp_dev_nm[] = "/dev/itntcp";

const char	utmpf[]      = "/var/adm/utmp";	/* Arquivo UTMP */
const char	wtmpf[]      = "/usr/adm/wtmp";	/* Arquivo WTMP */

entry UTMP	utmp;		/* Para atualizar "/var/adm/utmp" */

entry int	vflag;		/* Verbose */

entry long	local_addr,	/* Endereços INTERNET */
		remote_addr;

entry int	global_tcp_fd;	/* Conexão de controle */

entry FILE	*tcp_in_fp,	/* Conexão de controle */
		*tcp_out_fp;

entry int	slot = -1;	/* No. da entrada na "initab" */

PTYIO		ptyio;		/* Dados do pseudo terminal */

entry long	global_login_pid; /* Valor global */

/*
 ******	Variáveis globais ***************************************
 */
entry char	server_user_nm[16];	/* Usuário no servidor */

entry char	client_user_nm[16];	/* Usuário no cliente */

entry char	term_nm[16];		/* Nome do terminal remoto */

const char	globalenv[] =	"/etc/globalenv";
const char	login[]	 =	"/bin/login";

entry UTSNAME	uts;		/* Para tirar uma série de informações */

#define	ENV_SZ	10

entry const char *envp[ENV_SZ + 1];	/* Para Criar o Environment */

/*
 ******	Protótipos **********************************************
 */
void		on_sigchild (int, void *, void *);
void		on_sigterm_sighup (int, ...);
int		get_avail_pty (void);
void		read_globalenv (void);
void		login_proc (const char *);
void		pai (void);
void		filho (void);
void		read_write_loop (int);
void		receive_window_information (const char *);
const char	*ftgets (char *, int, FILE *);
void		clr_utmp (void);
void		error (const char *, ...);
void		tcp_abort (int);

/*
 ****************************************************************
 *	Programa Principal					*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	int		opt, c;
	const char	*str;
	INADDR		client_addr;
	INADDR		serv_addr[2];
	T_BIND		bind;
	T_CALL		call;
	int		listen_port, listen_fd, sequence;
	long		login_pid;
	int		tcp_fd, code;

	pgname = argv[0];

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

		    case 'M':			/* Residente */
			 exit (0);

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
	 *  tcmpto_s [-v] <listen_port> <listen_fd>
	 *				<client_addr> <client_port> <sequence>
	 */
#undef	DEBUG
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

	global_tcp_fd = tcp_fd;

	if ((tcp_in_fp = fdopen (tcp_fd, "r")) == NOFILE)
		error ("$*Não consegui obter um FILE para \"%s\"", tcp_dev_nm);

	if ((tcp_out_fp = fdopen (tcp_fd, "w")) == NOFILE)
		error ("$*Não consegui obter um FILE para \"%s\"", tcp_dev_nm);

	t_node_to_addr (tcp_fd, argv[2], &local_addr);

	remote_addr = client_addr.a_addr;

#ifdef	DEBUG
	error ("local_addr = %s, ", t_addr_to_str (local_addr));
	error ("remote_addr = %s\n", t_addr_to_str (remote_addr));
#endif	DEBUG

	/*
	 *	Vou realizar o "bind"
	 */
	serv_addr[0].a_port = listen_port;	/* Usa o RLOGIN_PORT */
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

	/*
	 ******	Prepara o ambiente ******************************
	 */
	environ = envp;	read_globalenv ();

	/*
	 ******	Lê o prólogo do protocolo ***********************
	 */
	if ((c = getc (tcp_in_fp)) < 0)
		error (NOSTR);

	if (c != 0)
		error ("$O primeiro caractere recebido não é NULO");

	if (ftgets (client_user_nm, sizeof (client_user_nm), tcp_in_fp) == NOSTR)
		error (NOSTR);

	if (ftgets (server_user_nm, sizeof (server_user_nm), tcp_in_fp) == NOSTR)
		error (NOSTR);

	if (ftgets (term_nm, sizeof (term_nm), tcp_in_fp) == NOSTR)
		error (NOSTR);

#ifdef	DEBUG
	error
	(	"client_user_nm = \"%s\", server_user_nm = \"%s\", term_nm = \"%s\"",
		client_user_nm, server_user_nm, term_nm
	);
#endif	DEBUG

	/*
	 ******	Obtém um pseudo terminal ************************
	 */
	if ((code = get_avail_pty ()) < 0)
	{
		fprintf
		(	tcp_out_fp,
			"1%s (%s): Não consegui obter um pseudo terminal (%d)\r\n",
			pgname, t_addr_to_node (tcp_fd, local_addr), code
		);

		fflush (tcp_out_fp); t_push (tcp_fd);

		sleep (2);
		error ("$Não consegui obter um pseudo terminal");
	}

	/*
	 *	Manda o caractere NULO para indicar sucesso
	 */
	if (t_snd (tcp_fd, "", 1, T_PUSH) != 1)
		error (NOSTR);

#ifdef	DEBUG
	error ("Pseudo terminal = \"%s\"\n", ptys_nm);
#endif	DEBUG

	/*
	 *	Indica que aceita o tamanho da janela
	 */
	if (t_snd (tcp_fd, "\x80", 1, T_PUSH|T_URGENT) != 1)
		error (NOSTR);

	/*
	 ****** Cria o processo de "login" **********************
	 */
	if ((login_pid = fork ()) < 0)
		error ("$*Não consegui dar FORK");

	if (login_pid == 0)
		login_proc (ptyio.t_nm_client + sizeof ("/dev/") - 1);

	global_login_pid = login_pid;

	/*
	 ****** Executa a comunicação entre o PTY e a internet **
	 */
	signal (SIGHUP,  on_sigterm_sighup);
	signal (SIGINT,  SIG_IGN);
	signal (SIGQUIT, SIG_IGN);
	signal (SIGTERM, on_sigterm_sighup);

	read_write_loop (tcp_fd);

	/*
	 ****** Término *****************************************
	 */
	memset (&utmp, 0, sizeof (UTMP));

	clr_utmp ();

	if (t_sndrel (tcp_fd) < 0)
		error ("$*Erro no SNDREL");

	if (t_rcvrel (tcp_fd) < 0)
		error ("$*Erro no RCVREL");

	kill (-global_login_pid, SIGHUP);

	return (0);

}	/* end rlogin_s */

/*
 ****************************************************************
 *	Obtém um pseudo terminal 				*
 ****************************************************************
 */
int
get_avail_pty (void)
{
	const INITAB	*initp;

	settgrp (0);

	if (getpty (&ptyio) < 0)
		{ error ("*Erro em GETPTY"); return (-1); }

	/*
	 *	Acha o "slot"
	 */
	if ((initp = getinitnam (ptyio.t_nm_client + sizeof ("/dev/") - 1)) == NOINITAB)
		return (-3);

	slot = initp->it_slot;

	endinitent ();

	return (0);

}	/* end get_avail_pty */

/*
 ****************************************************************
 *	Le o arquivo "globalenv"				*
 ****************************************************************
 */
void
read_globalenv (void)
{
	int		i;
	FILE		*pp;
	char		area[BLSZ];
	static char	sysname[24];

	/*
	 *	A primeira variável do "ambiente" é o SISTEMA OPERACIONAL
	 */
	strcpy (sysname, "SYS=");
	strcat (sysname, (char *)uts.uts_sysname);
	envp[0] = sysname;

	/*
	 *	Agora, processa o "globalenv"
	 */
	if ((pp = fopen (globalenv, "r")) == NOFILE)
		{ error ("*Não consegui ler \"%s\"", globalenv); return; }
	
	for
	(	i = 1;
		i < ENV_SZ && fngets (area, BLSZ, pp) == area;
		/* vazio */
	)
	{
		if (strchr (area, '=') == NOSTR)
		{
			error
			(	"Linha inválida em \"%s\": \"%s\"",
				globalenv, area
			);
			continue;
		}

		if ((envp[i] = strdup (area)) == NOSTR)
			break;

		i++;
	}

	fclose (pp);

}	/* end read_globalenv */

/*
 ****************************************************************
 *	Cria o processo para o "login"				*
 ****************************************************************
 */
void
login_proc (const char *line_nm)
{
	int		i;
	char		*cp;
	char		slot_nm[10];
	int		fflag = 0;
	const char	*argv[8], **ap = argv;
	const char	*client_node_nm = t_addr_to_node (global_tcp_fd, remote_addr);

	/*
	 *	Filho
	 */
	for (i = NUFILE - 1; i >= 0; i--)
		close (i);

	settgrp (0);

	ptyio.t_fd_client = open (ptyio.t_nm_client, O_RDWR);	/* 0 */
	dup (0);						/* 1 */
	dup (0);						/* 2 */

#ifdef	EASY_PWD
	/*
	 *	Teste PROVISÓRIO da dispensa da senha
	 */
	if (((local_addr ^ remote_addr) & 0xFFFF0000) == 0)
	{
		if (streq (server_user_nm, client_user_nm))
			fflag++;
	}
#endif	EASY_PWD

	if (!fflag)
	{
		printf
		(	"\aServidor \"rlogin\" (%s): %s (%s, %s) & %s %s (%s)\n",
			pgversion, uts.uts_machine, uts.uts_nodename, line_nm,
			uts.uts_sysname, uts.uts_version, uts.uts_date
		);
	}

	sprintf (slot_nm, "%d", slot);

	if ((cp = strchr (term_nm, '/')) != NOSTR)
		cp[0] = '\0';

	/*
	 *	Prepara os argumentos
	 */
	*ap++ = "login";

	if (fflag)
		*ap++ = "-f";

	*ap++ = slot_nm;
	*ap++ = line_nm;
	*ap++ = term_nm;
	*ap++ = (client_node_nm == NOSTR ? "???" : client_node_nm);
	*ap++ = server_user_nm;
	*ap++ = NOSTR;

	/*
	 *	Passa o controle para "login"
	 */
	execv (login, argv);

	error ("*Não consegui executar LOGIN");

	for (EVER)
		pause ();

}	/* end login_proc */

/*
 ****************************************************************
 *	Serviço de entrada/saída regular			*
 ****************************************************************
 */
void
read_write_loop (int tcp_fd)
{
	int		count, total, n;
	const char	*cp, *end_area;
	const char	*area_ptr;
	int		fd_vec[3];
	char		area[BLSZ];
	int		flag, index = -1;
	int		grave_fd, pid, status;
	char		window_info[WIN_SZ];
	int		window_count = 0;

	/*
	 *	Prepara o GRAVE
	 */
	if ((grave_fd = open ("/dev/grave", O_RDONLY)) < 0)
		error ("$*Não consegui abrir \"/dev/grave\"");

	/*
	 *	Prepara a ATTENTION
	 */
	fd_vec[0] = ptyio.t_fd_server;	/* Pseudo-terminal */
	fd_vec[1] = tcp_fd;		/* TCP */
	fd_vec[2] = grave_fd;		/* GRAVE */

	/*
	 *	Espera entrada do TCP ou do TECLADO
	 */
	for (EVER)
	{
		if ((index = attention (3, fd_vec, index, 0)) < 0)
		{
			if ((index = _attention_index) < 0)
			{
				if (errno == EINTR)
					{ index = -1; continue; }

				error ("$*Erro na chamada ao sistema \"attention\"");
			}
		}

		switch (index)
		{
		    case 0:		/* Pseudo-terminal */
			total = read (ptyio.t_fd_server, area, BLSZ);

			if   (total == 0)
				{ error ("EOF de leitura do PTYS"); continue; }
			elif (total < 0)
				error ("$*Erro de leitura do PTYS");
#ifdef	JOIN_LINES
			while (ioctl (ptyio.t_fd_server, TCNREAD) > 0 && total < BLSZ) 
			{
				if ((count = read (ptyio.t_fd_server, area + total, BLSZ - total)) < 0)
					error ("$*Erro de leitura do PTYS");

				total += count;
			}
#endif	JOIN_LINES
			if (t_snd (tcp_fd, area, total, T_PUSH) != total)
				error (NOSTR);

			break;

		    case 1:		/* TCP */
			if ((count = t_nread (tcp_fd)) < 0)	/* Erros e EOF */
				error (NOSTR);

			if (count == 0)	/* Alarme Falso! */
				break;

			if (count > BLSZ)
				count = BLSZ;

			if ((count = t_rcv (tcp_fd, area, count, &flag)) <= 0)
				error (NOSTR);
#ifdef	DEBUG
			if (flag & T_URGENT)
				error ("Recebi %d bytes urgentes", count);
#endif	DEBUG
			area_ptr = area; end_area = area + count;

			for (EVER)
			{
				if (count <= 0)
					break;

				if (window_count > 0)
				{
					n = WIN_SZ - window_count;

					if (n > count)
						n = count;

					memmove (&window_info[window_count], area_ptr, n);

					if ((window_count += n) >= WIN_SZ)
					{
						receive_window_information (window_info);
						window_count = 0;
					}

					area_ptr += n; count -= n; continue;
				}

				if ((cp = memchr (area_ptr, 0xFF, count)) == NOSTR)
				{
					if (write (ptyio.t_fd_server, area_ptr, count) != count)
						error ("$*Erro de escrita no pseudo-terminal");
					break;
				}

				if ((n = cp - area_ptr) > 0) 
				{
					if (write (ptyio.t_fd_server, area_ptr, n) != n)
						error ("$*Erro de escrita no pseudo-terminal");

					area_ptr += n; count -= n;
				}

				/* Temos no mínimo o "0xFF" */

				window_info[0] = 0xFF; window_count = 1;
				area_ptr += 1; count -= 1;

			}	/* end for (EVER) */

			break;

#if (0)	/*******************************************************/
				if (count >= WIN_SZ)
				{
					n = receive_window_information (area_ptr, count);
					area_ptr += n; count -= n;
					continue;
				}

				memmove (window_info, area_ptr, count);
				window_count = count;
				break;
#endif	/*******************************************************/

#if (0)	/*******************************************************/
				if (cp[1] == 0xFF)
				{
					n = receive_window_information (cp, end_area - cp);
					area_ptr += n; count -= n;
				}
				else
				{
					if (write (ptyio.t_fd_server, area_ptr, 1) != 1)
						error ("$*Erro de escrita no pseudo-terminal");

					area_ptr += 1; count -= 1;
				}
#endif	/*******************************************************/

		    case 2:		/* GRAVE */
			if   ((pid = wait (&status)) < 0)
				error ("*Erro na chamada \"wait\"");
			elif (pid != global_login_pid)
				error ("*PID da chamada \"wait\" incorreto");

			return;

		    default:
			error ("$A chamada ao sistema \"attention\" retornou o índice %d inválido", index);

		}	/* end switch */

	}	/* end for ever */

}	/* end read_write_loop */

/*
 ****************************************************************
 *	Recebe a informação da janela				*
 ****************************************************************
 */
void
receive_window_information (const char *cp)
{
	TERMIO		video;

	/*
	 *	Não deve retornar <= 0!
	 */
	if (cp[1] != 0xFF || cp[2] != 's' || cp[3] != 's')
		return;

#ifdef	DEBUG
	error ("Recebi %d linhas, %d colunas", cp[5], cp[7]);
#endif	DEBUG

	ioctl (ptyio.t_fd_server, TCLOCK);

	if (ioctl (ptyio.t_fd_server, TCGETS, &video) < 0)
		error ("*Não consegui obter as características do pseudo-terminal");

	if (cp[5] != 0)
		video.t_nline = cp[5];	/* BIG endian */

	if (cp[7] != 0)
		video.t_ncol  = cp[7];	/* BIG endian */

	if (video.t_page != 0)
		video.t_page = video.t_nline - 1;

	if (ioctl (ptyio.t_fd_server, TCSETNR, &video) < 0)
		error ("*Não consegui atribuir as características do pseudo-terminal");

	ioctl (ptyio.t_fd_server, TCFREE);

}	/* end receive_window_information */

/*
 ****************************************************************
 *	Lê uma linha terminada em '\0' (sem <nl>)		*
 ****************************************************************
 */
const char *
ftgets (char *str, int size, FILE *fp)
{
	const char	*str_end = str + size - 1;
	char		*cp = str;
	int		c;

	while ((c = getc (fp)) > 0)
	{
		if (cp < str_end)
			*cp++ = c;
	}

	if (c < 0)
		return (NOSTR);

	*cp = '\0';

	return (str);

}	/* end ftgets */

/*
 ****************************************************************
 *	Zera a entrada da UTMP					*
 ****************************************************************
 */
void
clr_utmp (void)
{
	int		fd;

	if (slot < 0)
		return;

	memset (&utmp, 0, sizeof (UTMP));

	if ((fd = open (utmpf, O_WRONLY)) >= 0)
	{
		lseek (fd, slot * sizeof (UTMP), 0);
		write (fd, &utmp, sizeof (UTMP));
		close (fd);
	}

	time   (&utmp.ut_time);
	strcpy (utmp.ut_line, ptyio.t_nm_client + sizeof ("/dev/") - 1);

	if ((fd = open (wtmpf, O_WRONLY)) >= 0)
	{
		lseek (fd, 0, 2);
		write (fd, &utmp, sizeof (UTMP));
		close (fd);
	}

}	/* end clr_utmp */

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
	FILE		*log_fp;
	char		err, abrt;
	int		event = 0, save_errno = errno;
	time_t		time_loc;
	static int	static_log_fd = 0;
	static FILE	*static_log_fp = NOFILE;

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
			event = t_look (global_tcp_fd);
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
				fprintf (log_fp, " (%s)", t_strevent (global_tcp_fd, event));
			elif (save_errno != 0)
				fprintf (log_fp, " (%s)", strerror (save_errno));
		}

		putc ('\n', log_fp); fflush (log_fp);
	}

	if (abrt == '$' || event == T_DISCONNECT)
		tcp_abort (1);

	va_end (args);

}	/* end error */

/*
 ****************************************************************
 *	Intercepta o sinal SIGTERM e SIGHUP			*
 ****************************************************************
 */
void
on_sigterm_sighup (int signo, ...)
{
	tcp_abort (1);

}	/* end on_sigterm_sighup */

/*
 ****************************************************************
 *	Encerra as atividades (desconectando o TCP)		*
 ****************************************************************
 */
void
tcp_abort (int n)
{
	kill (-global_login_pid, SIGHUP);

	error ("Enviei SIGHUP para grupo %d", global_login_pid);

	clr_utmp ();

	t_snddis (global_tcp_fd, (T_CALL *)NULL);

	exit (n);

}	/* end tcp_abort */
