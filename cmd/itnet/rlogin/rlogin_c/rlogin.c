/*
 ****************************************************************
 *								*
 *			rlogin.c				*
 *								*
 *	Comunicação com estação remota (cliente)		*
 *								*
 *	Versão	2.3.0, de 06.10.92				*
 *		3.0.2, de 08.08.97				*
 *								*
 *	Módulo: ITNET						*
 *		Utilitários de comunicação			*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Multiprocessado		*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <fcntl.h>
#include <pwd.h>
#include <termio.h>
#include <signal.h>
#include <errno.h>
#include <xti.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão:	3.0.2, de 08.08.97";

#define RLOGIN_PORT	513

#define MIN_PORT	513
#define MAX_PORT	1023

#define	EVER	;;
#define	elif	else if

entry int	vflag;		/* Verbose */
entry int	iflag;		/* Não converte o código dos caracteres */

/*
 ****** Variáveis da linha/tela *********************************
 */
int		global_tcp_fd;	/* Para o TCP */

entry TERMIO	ovideo, video;	/* modos do terminal */

/*
 ****** Protótipos de funções ***********************************
 */
void		read_write_loop (int);
void		send_window_information (int);
void		error (const char *, ...);
void		on_SIGTERM (int, ...);
void		tcp_abort (void);
void		quit (int);
void		help (void);

/*
 ****************************************************************
 *	Comunicação com estação remota				*
 ****************************************************************
 */
int
main (int argc, register const char *argv[])
{
	register int	opt;
	register char	*cp;
	register PASSWD	*pw;
	const char	*client_user_name = NOSTR;
	const char	*server_user_name = NOSTR;
	const char	*term = NOSTR;
	const char	*node_nm;
	int		tcp_fd;
	T_BIND		bind;
	INADDR		addr;
	T_CALL		call;
	const char	*str;
	int		flag;
	long		serv_addr;
	char		c;

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "l:ivH")) != EOF)
	{
		switch (opt)
		{
		    case 'l':			/* login name */
			server_user_name = optarg;
			break;

		    case 'i':			/* Sem conversão de código */
			iflag++;
			break;

		    case 'v':			/* Verbose */
			vflag++;
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

	if (argc != 1)
		help ();

	/*
	 *	Examina a entrada padrão
	 */
	if (ioctl (0, TCGETS, &ovideo) < 0  ||  isatty (1) == 0)
		error ("$O <stdin> e/ou <stdout> não é um terminal");

	/*
	 *	Obtém o nome do usuário local (cliente)
	 */
	client_user_name = getlogin ();

	if (client_user_name == NOSTR)
		client_user_name = getenv ("USER");

	if (client_user_name == NOSTR && (pw = getpwuid (getuid ())) != NOPASSWD)
		client_user_name = pw->pw_name;

	if (client_user_name == NOSTR)
		error ("$Não consegui obter o nome do próprio usuário local");

	/*
	 *	Obtém os nome do <nó_remoto> e do usuário remoto (servidor)
	 */
	if ((cp = strchr (argv[0], '@')) == NOSTR)
	{
		node_nm = argv[0];
	}
	else
	{
		cp[0] = '\0'; node_nm = cp + 1;

		if (server_user_name != NOSTR)
			help ();

		server_user_name = argv[0];
	}

	if (server_user_name == NOSTR)
		server_user_name = client_user_name;

	if (vflag)
	{
		printf ("client_user_name  = \"%s\"\n", client_user_name);
		printf ("server_user_name = \"%s\"\n", server_user_name);
		printf ("node_name = \"%s\"\n", node_nm);
	}

	/*
	 *	Obtém o terminal local
	 */
	if ((term = getenv ("TERM")) == NOSTR)
		term = "network";

	if (vflag)
		printf ("term:   %s\n", term);

	/*
	 *	Abre o "endpoint"
	 */
	str = "/dev/itntcp";

	if ((tcp_fd = t_open (str, O_RDWR, (T_INFO *)NULL)) < 0)
		error ("$*Não consegui abrir \"%s\"", str);

	global_tcp_fd = tcp_fd;

	/*
	 *	Obtém o endereço do nó remoto
	 */
	if ((serv_addr = t_node_to_addr (tcp_fd, node_nm, (long *)NULL)) == -1)
		error ("$*Não consegui obter o endereço de \"%s\"", node_nm);

	if (vflag)
		printf ("serv_addr dado: %08X (%s)\n", serv_addr, node_nm);

	/*
	 *	Associa um endereço local
	 */
	bind.addr.len	 = sizeof (INADDR);
	bind.addr.maxlen = sizeof (INADDR);
	bind.addr.buf	 = &addr;

	bind.qlen	 = 0;

	for (addr.a_port = MAX_PORT; /* teste abaixo */; addr.a_port--)
	{
		if (addr.a_port <= MIN_PORT)
			error ("$Todos os \"ports\" estão ocupados");

		if (t_bind (tcp_fd, &bind, &bind) >= 0)
			break;

		if (errno != TADDRBUSY)
			error ("$Não consegui associar um endereço");
	}

	if (vflag)
		printf ("Port_no do cliente obtido: %d\n", ((INADDR *)bind.addr.buf)->a_port);

	/*
	 *	Dá o CONNECT
	 */
	addr.a_port	 = RLOGIN_PORT;
	addr.a_addr	 = serv_addr;

	call.addr.len	 = sizeof (INADDR);
	call.addr.maxlen = sizeof (INADDR);
	call.addr.buf	 = &addr;

	if (t_connect (tcp_fd, &call, &call) < 0)
	{
		if (errno == TLOOK && t_look (tcp_fd) == T_DISCONNECT)
		{
			error ("A conexão foi recusada por \"%s\"", node_nm);
			quit (1);
		}

		error ("$*Não consegui estabelecer conexão");
	}

	if (vflag)
		printf ("port_no do servidor obtido: %d\n", ((INADDR *)call.addr.buf)->a_port);

	/*
	 *	Manda o byte NULO indicando que não tem o outro canal (sinais)
	 */
	if (t_snd (tcp_fd, "",  1, 0) < 0)
		error (NOSTR);

	/*
	 *	Manda o usuário local, remoto e terminal
	 */
#undef	TWICE_REMOTE
#ifdef	TWICE_REMOTE
	t_snd (tcp_fd, server_user_name, strlen (server_user_name) + 1, 0);
#else
	t_snd (tcp_fd, client_user_name,  strlen (client_user_name) + 1, 0);
#endif	TWICE_REMOTE

	t_snd (tcp_fd, server_user_name, strlen (server_user_name) + 1, 0);
	t_snd (tcp_fd, term,		strlen (term) + 1, T_PUSH);

	/*
	 *	Agora, espera a resposta (OK ou Erro)
	 */
	if (t_rcv (tcp_fd, &c, 1, &flag) != 1)
		error (NOSTR);

	if (c == '\0')
	{
		if (vflag)
			fprintf (stderr, "Recebi OK (flag = %X)\n", flag);
	}
	else
	{
		while (t_rcv (tcp_fd, &c, 1, &flag) == 1)
		{
			write (2, &c, 1);

			if (c == '\n' || c == '\0')
				break;
		}

		tcp_abort ();
	}

	/*
	 *	Poe o Terminal em RAW
	 */
	video = ovideo;

	video.c_oflag &= ~OPOST;
	video.c_lflag &= ~(ISIG|ICANON|ECHO);
	video.c_cc[VMIN] = 1;

	if (iflag)
	{
		video.c_lflag &= ~CODE;
	   /***	video.c_lflag |= ISO; ***/
	}

	ioctl (0, TCSETNR, &video);

	/*
	 *	Realiza o serviço regular
	 */
	signal (SIGTERM, on_SIGTERM);

	read_write_loop (tcp_fd);

	/*
	 *	Termina a conexão
	 */
	if (t_sndrel (tcp_fd) < 0)
		error ("$*Erro no SNDREL");

	if (t_rcvrel (tcp_fd) < 0)
		error ("$*Erro no RCVREL");

	ioctl (0, TCSETNR, &ovideo);

	printf ("Conexão \"rlogin\" fechada\n");

	return (0);

}	/* end rlogin */

/*
 ****************************************************************
 *	Serviço de entrada/saída regular			*
 ****************************************************************
 */
void
read_write_loop (int tcp_fd)
{
	register int	n;
	int		fd_vec[2];
	char		area[BLSZ];
	int		flag, index = -1;

	/*
	 *	Prepara a ATTENTION
	 */
	fd_vec[0] = 0;		/* TECLADO */
	fd_vec[1] = tcp_fd;	/* TCP */

	/*
	 *	Espera entrada do TCP ou do TECLADO
	 */
	for (EVER)
	{
		if ((index = attention (2, fd_vec, index, 0)) < 0)
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
		    case 0:		/* Teclado */
			if   ((n = read (0, area, BLSZ)) < 0)
				error ("*Erro na leitura do teclado");
			elif (n == 0)
				error ("EOF na leitura do teclado");
			elif (t_snd (tcp_fd, area, n, T_PUSH) < 0)
				error (NOSTR);
			break;

		    case 1:		/* TCP */
			if ((n = t_nread (tcp_fd)) < 0)
			{
				if (errno == TLOOK && t_look (tcp_fd) == T_ORDREL)
					return;		/* EOF */

				error (NOSTR);
			}

			if (n == 0)	/* Alarme Falso! */
				break;

			if (n > BLSZ)
				n = BLSZ;

			if ((n = t_rcv (tcp_fd, area, n, &flag)) < 0)
				error (NOSTR);

			if (flag & T_URGENT)
			{
				int		i;
#ifdef	DEBUG
				printf ("%s: Recebi %d bytes urgentes:", pgname, n);

				for (i = 0; i < n; i++)
					printf (" %02X", area[i]);

				printf ("\r\n");
#endif	DEBUG
				for (i = 0; i < n; i++)
				{
					if (area[i] == 0x80)
						send_window_information (tcp_fd);
				}

				break;
			}

			if (n == 0)	/* EOF */
				return;

			write (1, area, n);
			break;

		    default:
			error ("$A chamada ao sistema \"attention\" retornou o índice %d inválido", index);

		}	/* end switch */

	}	/* end for ever */

}	/* end read_write_loop */

/*
 ****************************************************************
 *	Envia a informação de tamanho de janela			*
 ****************************************************************
 */
void
send_window_information (int tcp_fd)
{
	char		window_info[12];

	memset (window_info, 0, sizeof (window_info));

	window_info[0] = 0xFF;
	window_info[1] = 0xFF;
	window_info[2] = 's';
	window_info[3] = 's';

	window_info[5] = video.t_nline;	/* BIG endian */
	window_info[7] = video.t_ncol;	/* BIG endian */

	if (t_snd (tcp_fd, window_info, sizeof (window_info), T_PUSH) < 0)
		error (NOSTR);
#ifdef	DEBUG
	error ("Mandei informação da janela");
#endif	DEBUG
 
}	/* end send_window_information */

/*
 ****************************************************************
 *	Imprime uma mensagem de erro (cliente)			*
 ****************************************************************
 */
void
error (const char *format, ...)
{
	register va_list	args;
	register const char	*fmt = format;
	char			err, abrt;
	int			event, save_errno = errno;

	va_start (args, format);

	if (fmt == NOSTR)
		fmt = "$*Erro de entrada/saída de TCP";

	if ((abrt = *fmt) == '$')
		fmt++;

	if ((err = *fmt) == '*')
		fmt++;

	fprintf  (stderr, "%s: ", pgname);
	vfprintf (stderr, fmt, args);

	if (err == '*')
	{
		if (save_errno == TLOOK)
		{
			event = t_look (global_tcp_fd);
	
			fprintf (stderr, " (%s)", t_strevent (global_tcp_fd, event));
	
			if (event ==  T_DISCONNECT)
				{ fprintf (stderr, "\r\n"); quit (1); }
		}
		elif (save_errno != 0)
		{
			fprintf (stderr, " (%s)", strerror (save_errno));
		}
	}

	fprintf (stderr, "\r\n");

	if (abrt == '$')
		tcp_abort ();

	va_end (args);

}	/* end error */

/*
 ****************************************************************
 *	Se receber um sinal, ...				*
 ****************************************************************
 */
void
on_SIGTERM (int signo, ...)
{
	error ("$Recebi um SIGTERM, terminando ...");

	/* Não retorna */

}	/* end on_SIGTERM */

/*
 ****************************************************************
 *	Encerra as atividades (desconectando o TCP)		*
 ****************************************************************
 */
void
tcp_abort (void)
{
	t_snddis (global_tcp_fd, (T_CALL *)NULL);

	quit (1);

}	/* end tcp_abort */
/*
 ****************************************************************
 *	Restaura o estado do terminal e termina			*
 ****************************************************************
 */
void
quit (int exit_code)
{
	ioctl (0, TCSETNR, &ovideo);

	exit (exit_code);

}	/* end on_SIGTERM */

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
		"%s - entra em sessão em um nó remoto\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-iv] [-l <usuário_remoto>] <nó_remoto>\n\n"
		"\t%s [-iv] [<usuário_remoto>@]<nó_remoto>\n",
		pgname, pgversion, pgname, pgname
	);
	fprintf
	(	stderr,
		"\nOpções:"
		"\t-i: Não converte o código na recepção\n"
		"\t-v: Verboso\n"
		"\t-l: Define o nome do usuário remoto "
			"(pode ser dado como \"...@\")\n"
		"\t    (se não dado, é utilizado o nome do usuário local)\n"
	);

	exit (2);

}	/* end help */
