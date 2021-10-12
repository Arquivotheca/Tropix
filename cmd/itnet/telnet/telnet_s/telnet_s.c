/*
 ****************************************************************
 *								*
 *			telnet.c				*
 *								*
 *	Login remoto (servidor)					*
 *								*
 *	Versão	3.0.0, de 04.04.93				*
 *		4.6.0, de 16.07.04				*
 *								*
 *	Módulo: TELNET						*
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
#include <utmp.h>
#include <termio.h>
#include <time.h>
#include <signal.h>
#include <errno.h>
#include <xti.h>

#include "../telnet.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char      pgversion[] =  "V4.6.0, de 16.07.04";

#define	EVER	;;
#define	elif	else if

const char	tcp_dev_nm[] = "/dev/itntcp";

const char	utmpf[]      = "/var/adm/utmp";	/* Arquivo UTMP */
const char	wtmpf[]      = "/usr/adm/wtmp";	/* Arquivo WTMP */

entry UTMP	utmp;		/* Para atualizar "/var/adm/utmp" */

entry int	vflag = 0;	/* Verbose */

entry long	local_addr,	/* Endereços INTERNET */
		remote_addr;

entry char	client_node_nm[80];	/* Nome do cliente */

entry int	global_tcp_fd;	/* Conexão de controle */

entry FILE	*tcp_in,	/* Conexão de controle */
		*tcp_out;

entry int	slot = -1;	/* No. da entrada na "initab" */

PTYIO		ptyio;		/* Dados do pseudo terminal */

entry FILE	*ptys_out;	/* FPs do pseudo terminal (servidor) */

TERMIO		ovideo, video;	/* modos do terminal */

entry long	global_login_pid; /* Valores globais */

entry int	static_log_fd;	/* Para "error" */
entry FILE	*static_log_fp;

/*
 ******	Variáveis do estado *************************************
 */
#define	OPTSZ	256

/* Todos os estados começam em "0" (STATE_NO) */

entry char	my_state[OPTSZ];	/* Estado daqui */

entry char	his_state[OPTSZ];	/* Estado dali */

entry char	his_answer[OPTSZ];	/* Aceitou ou recusou */

/*
 ******	Variáveis globais ***************************************
 */
entry char	term_nm[80] = "UNKNOWN"; /* Para o tipo recebido do cliente */

const char	globalenv[] =	"/etc/globalenv";
const char	login[]	 =	"/bin/login";

entry UTSNAME	uts;		/* Para tirar uma série de informações */

#define	ENV_SZ	10

entry const char *envp[ENV_SZ + 1];	/* Para Criar o Environment */

/*
 ******	Protótipos **********************************************
 */
int		get_avail_pty (void);
void		read_globalenv (void);
void		login_proc (const char *);
void		read_write_loop (int);
int		read_tcp (int, int);
int		process_telnet_option (const char *, int);
void		receive_WILL (int);
void		receive_WONT (int);
void		send_DO (int);
void		send_DONT (int);
void		receive_DO (int);
void		receive_DONT (int);
void		send_WILL (int);
void		send_WONT (int);
int		he_should (int);
int		we_should (int);
void		enable (int);
void		disable (int);
int		receive_SB (const char *, int);
void		send_request (int);
void		request_terminal_type (void);
int		receive_terminal_type (const char *, int);
int		receive_window_size (const char *, int);
void		send_option (int, int);
const char	*edit_telnet_code (int);
const char	*edit_telnet_option (int);
void		clr_utmp (void);
void		error (const char *, ...);
void		on_sigterm_sighup (int, ...);
void		tcp_abort (int);

/*
 ****************************************************************
 *	Login remoto (servidor)					*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	int		opt;
	const char	*str;
	INADDR		client_addr;
	INADDR		serv_addr[2];
	T_BIND		bind;
	T_CALL		call;
	int		tcp_fd, listen_port, listen_fd, sequence, code;
	long		login_pid;

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
	(	"%s <%s> <%s> <%s> <%s> <%s>",
		vflag ? "-v" : "",
		argv[0], argv[1], argv[2], argv[3], argv[4]
	);
#endif	DEBUG

	if (argc != 5) error
	(	"$Número inválido de argumentos: %d",
		argc
	);

	listen_port	   = strtol (argv[0], &str, 0);
	listen_fd	   = strtol (argv[1], &str, 0);
	client_addr.a_addr = t_str_to_addr (argv[2]);
	client_addr.a_port = strtol (argv[3], &str, 0);
	sequence	   = strtol (argv[4], &str, 0);

	if ((tcp_fd = t_open (tcp_dev_nm, O_RDWR, (T_INFO *)NULL)) < 0)
		error ("$*Não consegui abrir \"%s\"", tcp_dev_nm);

	global_tcp_fd = tcp_fd;

	if ((tcp_in  = fdopen (tcp_fd, "r")) == NOFILE)
		error ("$*Não consegui obter um FILE para \"%s\"", tcp_dev_nm);

	if ((tcp_out = fdopen (tcp_fd, "w")) == NOFILE)
		error ("$*Não consegui obter um FILE para \"%s\"", tcp_dev_nm);

	t_node_to_addr (tcp_fd, argv[2], &local_addr);

	remote_addr = client_addr.a_addr;

	if ((str = t_addr_to_node (tcp_fd, remote_addr)) == NOSTR)
		str = t_addr_to_str (remote_addr);

	strcpy (client_node_nm, str);

#ifdef	DEBUG
	error ("local_addr = %s, ", t_addr_to_str (local_addr));
	error ("remote_addr = %s\n", t_addr_to_str (remote_addr));
#endif	DEBUG

	/*
	 *	Vou realizar o "bind" (usa o mesmo port do TELNET)
	 */
	serv_addr[0].a_port = listen_port;
	serv_addr[1].a_addr = client_addr.a_addr;
	serv_addr[1].a_port = client_addr.a_port;

	bind.addr.maxlen = sizeof (INADDR);
	bind.addr.len	 = 2 * sizeof (INADDR);
	bind.addr.buf	 = serv_addr;

	bind.qlen	 = 0;

	if (t_bind (tcp_fd, &bind, &bind) < 0)
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
	 ******	Obtém um pseudo terminal ************************
	 */
	if ((code = get_avail_pty ()) < 0)
	{
		fprintf
		(	tcp_out,
			"%s (%s): Não consegui obter um pseudo terminal (%d)\r\n",
			pgname, t_addr_to_node (tcp_fd, local_addr), code
		);

		fflush (tcp_out); t_push (tcp_fd);

		sleep (2);
		error ("$Não consegui obter um pseudo terminal");
	}

#ifdef	DEBUG
	error ("Pseudo terminal = \"%s\"\n", ptyio.t_nm_server);
#endif	DEBUG

	/*
	 *	Prepara o modo do pseudo-terminal (default sem echo)
	 */
	if (ioctl (ptyio.t_fd_client, TCGETS, &ovideo) < 0)
		error ("$*Não consegui obter o modo do pseudo-terminal");

	video = ovideo;

	if (video.t_nline == 0)
		video.t_nline = 24;

	if (video.t_ncol == 0)
		video.t_ncol = 80;

	video.c_lflag &= ~ECHO;
	ioctl (ptyio.t_fd_client, TCSETNR, &video);

	/*
	 *	Imprime o cabeçalho
	 */
	fprintf
	(	tcp_out,
		"\aServidor \"telnet\" (%s): %s (%s, %s) & %s %s (%s)\r\n\r\n",
		pgversion, uts.uts_machine, uts.uts_nodename, ptyio.t_nm_client + sizeof ("/dev/") - 1,
		uts.uts_sysname, uts.uts_version, uts.uts_date
	);

	fflush (tcp_out); t_push (tcp_fd);

	error ("Iniciando para \"%s\"", client_node_nm);

	/*
	 *	Envia as opções de preferência
	 */
	send_WILL (NVT_ECHO);
	send_DO   (NVT_TERMINAL_TYPE);
	send_DO   (NVT_WINDOW_SIZE);

	for (EVER)
	{
		read_tcp (tcp_fd, BLSZ);

		/* Examina para ver se recebemos o que esperamos */

		if
		(	his_answer[NVT_TERMINAL_TYPE] == STATE_RECUSE ||
			his_answer[NVT_TERMINAL_TYPE] == STATE_INFO
		)
			break;

	}	/* end for (EVER) */

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
	error ("Terminando para \"%s\"", client_node_nm);

	ioctl (ptyio.t_fd_client, TCSETNR, &ovideo);

	clr_utmp ();

	if (t_sndrel (tcp_fd) < 0)
		error ("$*Erro no SNDREL");

	if (t_rcvrel (tcp_fd) < 0)
		error ("$*Erro no RCVREL");

	kill (-global_login_pid, SIGHUP);

	return (0);

}	/* end telnet_s */

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

	if ((ptys_out = fdopen (ptyio.t_fd_server, "w")) == NOFILE)
		return (-2);

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

	/*
	 *	Filho
	 */
	fclose (static_log_fp);
	static_log_fd = 0;

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
	*ap++ = client_node_nm;
   /***	*ap++ = server_user_nm; ***/
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
	int		count;
	int		fd_vec[3];
	char		area[BLSZ];
	int		index = -1;
	int		grave_fd, pid, status;

	/*
	 *	Prepara "grave"
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
	 *	Espera entrada do TCP, do TECLADO ou o término do FILHO
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
			count = read (ptyio.t_fd_server, area, BLSZ);

			if   (count == 0)
				{ error ("EOF de leitura do PTYS"); continue; }
			elif (count < 0)
				error ("$*Erro de leitura do PTYS");

			if (t_snd (tcp_fd, area, count, T_PUSH) != count)
				error (NOSTR);

			break;

#if (0)	/*******************************************************/
	int		c;
	const char	*cp, *end_area;

			end_area = area + count;

			for (cp = area; cp < end_area; cp++)
			{
				switch (c = *cp)
				{
				    case '\r':
					putc (c, tcp_out);
					c = '\0';
					break;

				    case '\n':
					putc ('\r', tcp_out);
					break;

				    default:
					break;

				}	/*  end switch */

				putc (c, tcp_out);

			}	/* end for (EVER) */

			fflush (tcp_out); t_push (tcp_fd);
			break;
#endif	/*******************************************************/

		    case 1:		/* TCP */
			if ((count = t_nread (tcp_fd)) < 0)
			{
				if (errno == TLOOK && t_look (tcp_fd) == T_ORDREL)
					return;		/* EOF */

				error (NOSTR);
			}

			if (count == 0)	/* Alarme Falso! */
				break;

			if (count > BLSZ)
				count = BLSZ;

			if (read_tcp (tcp_fd, count) < 0)
				return;
			break;

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
 *	Processa um pacote chegando do TCP			*
 ****************************************************************
 */
int
read_tcp (int tcp_fd, int count)
{
	int		n;
	int		c, next;
	const char	*cp, *end_area;
	char		area[BLSZ];
	int		flag;

	if (count > 0)
		count = t_rcv (tcp_fd, area, count, &flag);

	if (count < 0)
		error (NOSTR);

	if (count == 0)
	{
		kill (-global_login_pid, SIGHUP);
		error ("EOF do TCP: enviando SIGHUP para %d", global_login_pid);
		return (-1);
	}

	end_area = area + count;

	for (cp = area; cp < end_area; /* abaixo */)
	{
		switch (c = *cp++)
		{
		    case '\r':
			if   (cp >= end_area)
				break;
			elif ((next = *cp++) == '\0')
				break;
			elif (next == '\n')
				{ c = '\n'; break; }
			putc (c, ptys_out);
			c = next;
			break;

		    case '\0':
			continue;

		    case IAC:
			n = end_area - cp + 1;

			if   (n < 2)
				break;
			elif (*cp == IAC)
				{ cp++; break; }

			if (n < 3)
				break;

			cp += process_telnet_option (--cp, n);

			continue;

		    default:
			break;

		}	/*  end switch */

		putc (c, ptys_out);

	}	/* end for (EVER) */

	fflush (ptys_out);

	return (0);

}	/* end read_tcp */

/*
 ****************************************************************
 *	Processa uma opção TELNET				*
 ****************************************************************
 */
int
process_telnet_option (const char *cp, int count)
{
	int		n = 3, code = cp[1], option = cp[2];

	/*
	 *	Não pode retornar <= 0
	 */
	if (vflag) error
	( 	"Recebi: %s %s",
		edit_telnet_code (code),
		edit_telnet_option (option)
	);

	switch (code)
	{
	    case WILL:
		receive_WILL (option);
		break;

	    case WONT:
		receive_WONT (option);
		break;

	    case DO:
		receive_DO (option);
		break;

	    case DONT:
		receive_DONT (option);
		break;

	    case SUB:
		n = receive_SB (cp, count);
		break;

	    default:
		error ("process_telnet_option: código %02X inválido\n", code);
		break;

	}	/* end switch */

	if (n < 3)
		n = 3;

	return (n);

}	/* end process_telnet_option */

/*
 ****************************************************************
 *	Recebi um WILL						*
 ****************************************************************
 */
void
receive_WILL (int option)
{
	char		*state_ptr = &his_state[option];

	his_answer[option] = STATE_ACCEPT;	/* Aceitou */

	switch (*state_ptr)
	{
	    case STATE_NO:
		if (he_should (option))
			{ *state_ptr = STATE_YES; send_option (DO, option); }
		else
			send_option (DONT, option);
		break;
			
	    case STATE_YES:
		/* Ignore */
		break;

	    case STATE_WANTNO:
		error ("DONT respondido com WILL (opção %s)", edit_telnet_option (option));
		*state_ptr = STATE_NO;
		break;

	    case STATE_WANTYES:
		*state_ptr = STATE_YES;
		send_request (option);
		break;

	    default:
		error ("receive_WILL: Estado %d inválido (opção %s)", *state_ptr, edit_telnet_option (option));
		break;

	}	/* end switch */

}	/* end receive_WILL */

/*
 ****************************************************************
 *	Recebi um WONT						*
 ****************************************************************
 */
void
receive_WONT (int option)
{
	char		*state_ptr = &his_state[option];

	his_answer[option] = STATE_RECUSE;	/* Recusou */

	switch (*state_ptr)
	{
	    case STATE_NO:
		/* Ignore */
		break;

	    case STATE_YES:
		*state_ptr = STATE_NO;
		send_option (DONT, option);
		break;
			
	    case STATE_WANTNO:
	    case STATE_WANTYES:
		*state_ptr = STATE_NO;
		break;

	    default:
		error ("receive_WONT: Estado %d inválido (opção %s)", *state_ptr, edit_telnet_option (option));
		break;

	}	/* end switch */

}	/* end receive_WONT */

/*
 ****************************************************************
 *	Pede para ligar						*
 ****************************************************************
 */
void
send_DO (int option)
{
	char		*state_ptr = &his_state[option];

	switch (*state_ptr)
	{
	    case STATE_NO:
		*state_ptr = STATE_WANTYES;
		send_option (DO, option);
		break;

	    case STATE_YES:
		error ("Opção %s já ligada", edit_telnet_option (option));
		break;
			
	    case STATE_WANTNO:
	    case STATE_WANTYES:
		error ("Opção %s em processamento", edit_telnet_option (option));
		break;

	    default:
		error ("send_DO: Estado %d inválido (opção %s)", *state_ptr, edit_telnet_option (option));
		break;

	}	/* end switch */

}	/* end send_DO */

#ifdef	WHEN_NEEDED
/*
 ****************************************************************
 *	Pede para desligar					*
 ****************************************************************
 */
void
send_DONT (int option)
{
	char		*state_ptr = &his_state[option];

	switch (*state_ptr)
	{
	    case STATE_NO:
		error ("Opção %s já desligada", edit_telnet_option (option));
		break;

	    case STATE_YES:
		*state_ptr = STATE_WANTNO;
		send_option (DONT, option);
		break;
			
	    case STATE_WANTNO:
	    case STATE_WANTYES:
		error ("Opção %s em processamento", edit_telnet_option (option));
		break;

	    default:
		error ("send_DONT: Estado %d inválido (opção %s)", *state_ptr, edit_telnet_option (option));
		break;

	}	/* end switch */

}	/* end send_DONT */
#endif	WHEN_NEEDED

/*
 ****************************************************************
 *	Recebi um DO						*
 ****************************************************************
 */
void
receive_DO (int option)
{
	char		*state_ptr = &my_state[option];

	switch (*state_ptr)
	{
	    case STATE_NO:
		if (we_should (option))
		{
			*state_ptr = STATE_YES;
			send_option (WILL, option);
			enable (option);
		}
		else
		{
			send_option (WONT, option);
		}
		break;
			
	    case STATE_YES:
		/* Ignore */
		break;

	    case STATE_WANTNO:
		error ("WONT respondido com DO (opção %s)", edit_telnet_option (option));
		*state_ptr = STATE_NO;
		break;

	    case STATE_WANTYES:
		*state_ptr = STATE_YES;
		enable (option);
		break;

	    default:
		error ("receive_DO: Estado %d inválido (opção %s)", *state_ptr, edit_telnet_option (option));
		break;

	}	/* end switch */

}	/* end receive_DO */

/*
 ****************************************************************
 *	Recebi um DONT						*
 ****************************************************************
 */
void
receive_DONT (int option)
{
	char		*state_ptr = &my_state[option];

	switch (*state_ptr)
	{
	    case STATE_NO:
		/* Ignore */
		break;

	    case STATE_YES:
		disable (option);
		*state_ptr = STATE_NO;
		send_option (WONT, option);
		break;
			
	    case STATE_WANTNO:
	    case STATE_WANTYES:
		*state_ptr = STATE_NO;
		break;

	    default:
		error ("receive_DONT: Estado %d inválido (opção %s)", *state_ptr, edit_telnet_option (option));
		break;

	}	/* end switch */

}	/* end receive_DONT */

/*
 ****************************************************************
 *	Concorda em ligar					*
 ****************************************************************
 */
void
send_WILL (int option)
{
	char		*state_ptr = &my_state[option];

	switch (*state_ptr)
	{
	    case STATE_NO:
		*state_ptr = STATE_WANTYES;
		send_option (WILL, option);
		break;

	    case STATE_YES:
		error ("Opção %s já ligada", edit_telnet_option (option));
		break;
			
	    case STATE_WANTNO:
	    case STATE_WANTYES:
		error ("Opção %s em processamento", edit_telnet_option (option));
		break;

	    default:
		error ("send_WILL: Estado %d inválido (opção %s)", *state_ptr, edit_telnet_option (option));
		break;

	}	/* end switch */

}	/* end send_WILL */

#ifdef	WHEN_NEEDED
/*
 ****************************************************************
 *	Não concorda em ligar					*
 ****************************************************************
 */
void
send_WONT (int option)
{
	char		*state_ptr = &my_state[option];

	switch (*state_ptr)
	{
	    case STATE_NO:
		error ("Opção %s já desligada", edit_telnet_option (option));
		break;

	    case STATE_YES:
		*state_ptr = STATE_WANTNO;
		send_option (WONT, option);
		break;
			
	    case STATE_WANTNO:
	    case STATE_WANTYES:
		error ("Opção %s em processamento", edit_telnet_option (option));
		break;

	    default:
		error ("send_WONT: Estado %d inválido (opção %s)", *state_ptr, edit_telnet_option (option));
		break;

	}	/* end switch */

}	/* end send_WONT */
#endif	WHEN_NEEDED

/*
 ****************************************************************
 *	Concordamos com a sua opção?				*
 ****************************************************************
 */
int
he_should (int option)
{
	switch (option)
	{
	    case NVT_ECHO:
		return (1);

	    case NVT_SUPRESS_GO_AHEAD:
		return (1);

	    case NVT_TERMINAL_TYPE:
		return (1);

	    case NVT_WINDOW_SIZE:
		return (1);

	    default:
		error ("he_should: Opção %d desconhecida", option);
		break;

	}	/* end switch */

	return (0);

}	/* end he_should */

/*
 ****************************************************************
 *	Concordamos com a sua opção?				*
 ****************************************************************
 */
int
we_should (int option)
{
	switch (option)
	{
	    case NVT_ECHO:
		return (1);

	    case NVT_SUPRESS_GO_AHEAD:
		return (1);

	    case NVT_TERMINAL_TYPE:
		return (0);

	    case NVT_WINDOW_SIZE:
		return (0);

	    default:
		error ("we_should: Opção %d desconhecida", option);
		break;

	}	/* end switch */

	return (0);

}	/* end we_should */

/*
 ****************************************************************
 *	Liga a opção						*
 ****************************************************************
 */
void
enable (int option)
{
	switch (option)
	{
	    case NVT_ECHO:
		video.c_lflag |= ECHO;
		ioctl (ptyio.t_fd_client, TCSETNR, &video);
		break;

	    case NVT_SUPRESS_GO_AHEAD:
		break;

	    case NVT_TERMINAL_TYPE:
		break;

	    case NVT_WINDOW_SIZE:
		break;

	    default:
		error ("enable: Opção %d desconhecida", option);
		break;

	}	/* end switch */

}	/* end enable */

/*
 ****************************************************************
 *	Desliga a opção						*
 ****************************************************************
 */
void
disable (int option)
{
	switch (option)
	{
	    case NVT_ECHO:
		video.c_lflag &= ~ECHO;
		ioctl (ptyio.t_fd_client, TCSETNR, &video);
		break;

	    case NVT_SUPRESS_GO_AHEAD:
		break;

	    case NVT_TERMINAL_TYPE:
		break;

	    case NVT_WINDOW_SIZE:
		break;

	    default:
		error ("disable: Opção %d desconhecida", option);
		break;

	}	/* end switch */

}	/* end disable */

/*
 ****************************************************************
 *	Recebe um comando SB					*
 ****************************************************************
 */
int
receive_SB (const char *cp, int count)
{
	int		option = cp[2];

	switch (option)
	{
	    case NVT_TERMINAL_TYPE:
		return (receive_terminal_type (cp, count));

	    case NVT_WINDOW_SIZE:
		return (receive_window_size (cp, count));

	    default:
		error ("receive_SB: Opção %d desconhecida", option);
		return (3);

	}	/* end switch */

}	/* end he_should */

/*
 ****************************************************************
 *	Envia o pedido adicional de uma opção			*
 ****************************************************************
 */
void
send_request (int option)
{
	switch (option)
	{
	    case NVT_TERMINAL_TYPE:
		request_terminal_type ();
		break;

#if (0)	/*******************************************************/
	    default:
		error ("send_request: Opção %d desconhecida", option);
#endif	/*******************************************************/

	}	/* end switch */

}	/* end send_request */

/*
 ****************************************************************
 *	Pede para enviar o tipo do terminal			*
 ****************************************************************
 */
void
request_terminal_type (void)
{
	char		area[6];

	area[0] = IAC;
	area[1] = SUB;
	area[2] = NVT_TERMINAL_TYPE;
	area[3] = 1 /* == SEND */;
	area[4] = IAC;
	area[5] = SE;

	if (t_snd (global_tcp_fd, area, sizeof (area), T_PUSH) != sizeof (area))
		error (NOSTR);

	if (vflag)
		error ("Enviei: SEND TERM_TYPE");

}	/* end request_terminal_type */

/*
 ****************************************************************
 *	Recebe o nome do terminal				*
 ****************************************************************
 */
int
receive_terminal_type (const char *cp, int count)
{
	char		*tp = term_nm;
	int		c;
	const char	*cp_begin = cp;

	if (count < 7)
		{ error ("receive_terminal_type: Tamanho insuficiente"); goto bad; }

	if ((c = cp[3]) != 0 /* ==  IS */)
		{ error ("receive_terminal_type: Esperava um IS ao invés de %d", c); goto bad; }

	cp += 4; count -= 4;	/* Pula "IAC SB TERMINAL-TYPE IS" */

	for (EVER)
	{
		if (count-- <= 0)
			{ error ("receive_terminal_type: Tipo de terminal inválido"); break; }

		if ((c = *cp++) == IAC)
			break;

		if (tp < &term_nm[sizeof (term_nm) - 1])
			*tp++ = c;
	}
		
	*tp = '\0';

	if (count <= 0 || *cp++ != SE)
		error ("receive_terminal_type: Esperava um SE");

	if (vflag) error
	( 	"Recebi: TERM_TYPE = \"%s\"",
		term_nm
	);

	his_answer[NVT_TERMINAL_TYPE] = STATE_INFO;	/* Enviou a informação */

	return (cp - cp_begin);

	/*
	 *	A resposta teve sintaxe errada
	 */
    bad:
	his_answer[NVT_TERMINAL_TYPE] = STATE_RECUSE;	/* Recusou */
	return (3);

}	/* end receive_terminal_type */

/*
 ****************************************************************
 *	Recebe o tamanho da janela				*
 ****************************************************************
 */
int
receive_window_size (const char *cp, int count)
{
	if (count < 9)
		{ error ("receive_window_size: Tamanho insuficiente"); goto bad; }

	if (cp[4] == 0 || cp[6] == 0)
		{ error ("receive_window_size: Valores inválidos: (%d,%d)", cp[4], cp[6]); goto bad; }

	video.t_ncol  = cp[4];
	video.t_nline = cp[6];

	if (video.t_page != 0)
		video.t_page = video.t_nline - 1;

	ioctl (ptyio.t_fd_client, TCSETNR, &video);

	if (cp[7] != IAC || cp[8] != SE)
		error ("receive_window_size: Esperava IAC/SE");

	if (vflag) error
	( 	"Recebi: WINDOW_SIZE = (%d,%d)",
		cp[4], cp[6]
	);

	his_answer[NVT_WINDOW_SIZE] = STATE_INFO;	/* Enviou a informação */

	return (9);

	/*
	 *	A resposta teve sintaxe errada
	 */
    bad:
	his_answer[NVT_WINDOW_SIZE] = STATE_RECUSE;	/* Recusou */
	return (3);

}	/* end receive_window_size */

/*
 ****************************************************************
 *	Envia uma opção						*
 ****************************************************************
 */
void
send_option (int code, int option)
{
	char		area[3];

	area[0] = IAC;
	area[1] = code;
	area[2] = option;

	if (t_snd (global_tcp_fd, area, 3, T_PUSH) != 3)
		error (NOSTR);

	if (vflag) error
	( 	"Enviei: %s %s",
		edit_telnet_code (code),
		edit_telnet_option (option)
	);

}	/* end send_option */

/*
 ****************************************************************
 *	Edita um código TELNET					*
 ****************************************************************
 */
typedef struct
{
	int	c_code;		/* Código numérico */
	char	c_name[8];	/* Código em ISO */

}	CODE_TABLE;

const CODE_TABLE	code_table[] =
{
	WILL,	"WILL",
	WONT,	"WON'T",
	DO,	"DO",
	DONT,	"DON'T",
	SUB,	"SB",
	0
};

const char *
edit_telnet_code (int code)
{
	const CODE_TABLE	*cp;

	for (cp = code_table; cp->c_code != 0; cp++)
	{
		if (cp->c_code == code)
			return (cp->c_name);
	}

	return ("???");

}	/* end edit_telnet_code */

/*
 ****************************************************************
 *	Edita uma opção TELNET					*
 ****************************************************************
 */
typedef struct
{
	int	o_option;	/* Código numérico da opção */
	char	o_name[20];	/* Opção em ISO */

}	OPTION_TABLE;

const OPTION_TABLE	option_table[] =
{
	NVT_ECHO,		"ECHO",
	NVT_SUPRESS_GO_AHEAD,	"SUPRESS_GO_AHEAD",	
	NVT_NAOL,		"TERMINAL_LINE",
	NVT_NAOP,		"TERMINAL_PAGE",
	NVT_TERMINAL_TYPE,	"TERMINAL_TYPE",
	NVT_WINDOW_SIZE,	"WINDOW_SIZE",
	0
};

const char *
edit_telnet_option (int option)
{
	const OPTION_TABLE	*cp;

	for (cp = option_table; cp->o_option != 0; cp++)
	{
		if (cp->o_option == option)
			return (cp->o_name);
	}

	return ("???");

}	/* end edit_telnet_option */

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
	va_list	 args;
	const char	*fmt = format;
	FILE		*log_fp;
	char			err, abrt;
	int			event = 0, save_errno = errno;
	time_t			time_loc;

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

	if (abrt == '$' || event ==  T_DISCONNECT)
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
	clr_utmp ();

	kill (-global_login_pid, SIGHUP);

	error ("ABORT do TCP: enviando SIGHUP para %d", global_login_pid);

	t_snddis (global_tcp_fd, (T_CALL *)NULL);

	exit (n);

}	/* end tcp_abort */
