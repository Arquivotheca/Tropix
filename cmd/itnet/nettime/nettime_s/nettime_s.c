/*
 ****************************************************************
 *								*
 *			time_s.c				*
 *								*
 *	Time protocol (servidor)				*
 *								*
 *	Versão	3.0.0, de 25.11.92				*
 *		3.0.1, de 23.08.97				*
 *								*
 *	Módulo: TIME_S						*
 *		Utilitários de comunicações			*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>
#include <xti.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
#if (0)	/*************************************/
const char	pgversion[] =  "Versão: 3.0.1, de 23.08.97";
#endif	/*************************************/

#define	TIME_OFFSET	2208988800	/* No. de segundos 1970 - 1900 */

#define	elif	else if

const char	tcp_dev_nm[] = "/dev/itntcp";

entry int	vflag;		/* Verbose */

entry int	tcp_fd;		/* Descritor da conexão */

entry int	snd_dis;		/* Desconecta */

/*
 ******	Protótipos de funções ***********************************
 */
void		error (const char *, ...);
void		tcp_abort (int);

/*
 ****************************************************************
 *	Programa Principal					*
 ****************************************************************
 */
int
main (int argc, register const char *argv[])
{
	register int	opt;
	const char	*str;
	INADDR		client_addr;
	INADDR		serv_addr[2];
	T_BIND		bind;
	T_CALL		call;
	int		listen_port, listen_fd, sequence;
	time_t		present_time;

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
	 *  time_s [-v] <listen_port> <listen_fd>
	 *				<client_addr> <client_port> <sequence>
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

	/*
	 *	Vou realizar o "bind"
	 */
	serv_addr[0].a_port = listen_port;	/* Usa o TIME_PORT */
	serv_addr[1].a_addr = client_addr.a_addr;
	serv_addr[1].a_port = client_addr.a_port;

	bind.addr.maxlen = 0;
	bind.addr.len	 = 2 * sizeof (INADDR);
	bind.addr.buf	 = serv_addr;

	bind.qlen	 = 0;

	if (t_bind (tcp_fd, &bind, (T_BIND *)NULL) < 0)
		error ("$*Não consegui atribuir endereço");

	snd_dis++;

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

	t_close (listen_fd);

	/*
	 *	Calcula e envia o tempo
	 */
#ifdef	LITTLE_ENDIAN
	present_time = long_endian_cv (time ((time_t *)NULL) + TIME_OFFSET);
#else
	present_time = time ((time_t *)NULL) + TIME_OFFSET;
#endif	LITTLE_ENDIAN

	if (t_snd (tcp_fd, &present_time, sizeof (present_time), T_PUSH) < 0)
		error (NOSTR);

	if (t_sndrel (tcp_fd) < 0)
		error ("$*Erro no SNDREL");

	if (t_rcvrel (tcp_fd) < 0)
		error ("$*Erro no RCVREL");

	return (0);

}	/* end time_s */

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
			if (event != 0)
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
	t_snddis (tcp_fd, (T_CALL *)NULL);

	exit (n);

}	/* end tcp_abort */
