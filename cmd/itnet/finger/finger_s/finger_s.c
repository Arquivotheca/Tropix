/*
 ****************************************************************
 *								*
 *			finger.c				*
 *								*
 *	Obtém informações sobre usuários remotos (servidor)	*
 *								*
 *	Versão	3.0.0, de 25.04.93				*
 *		4.6.0, de 17.07.04				*
 *								*
 *	Módulo: FINGER						*
 *		Utilitários de comunicações			*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2004 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include <stat.h>
#include <fcntl.h>
#include <pwd.h>
#include <init.h>
#include <time.h>
#include <utmp.h>
#include <errno.h>
#include <xti.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] = "V4.6.0, de 17.07.04";

#define	EVER	;;
#define	elif	else if

#define	DEF_SH	"/bin/sh"	/* Para entradas de "passwd" sem "sh" */

#define	PLAN_NM	".plan"		/* Para planos do usuário */

#define	MAIL_NM	"/usr/mail"	/* Para o "mail" do usuário */

const char	tcp_dev_nm[] = "/dev/itntcp";

const char	llfile[]     = "/usr/adm/lastlog"; /* Arquivo LASTLOG */

entry int	vflag = 0;	/* Verbose */

entry long	local_addr,	/* Endereços INTERNET */
		remote_addr;

entry int	tcp_fd;		/* Conexão de controle */

entry FILE	*tcp_out;	/* Conexão de controle (saída) */

/*
 ****** Nome dos arquivos globais *******************************
 */
const char	utmp_nm[]   = "/var/adm/utmp";

/*
 ****** Protótipos de funções ***********************************
 */
const char	*read_init_line (FILE *);
void		write_who_line (const UTMP *, const char *);
void		write_passwd_line (const char *);
void		write_last_login (const PASSWD *, int);
void		write_mail_date (const PASSWD *);
void		write_plan_file (const PASSWD *);
void		help (void);

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
	int		opt;
	const char	*str;
	INADDR		client_addr;
	INADDR		serv_addr[2];
	T_BIND		bind;
	T_CALL		call;
	int		listen_port, listen_fd, sequence;
	FILE		*utmp_fp;
	char		*cp;
	const char	*local;
	UTMP		utmp;
	INITAB		*initp;
	int		flag;
	char		user_nm[80];

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

	if ((tcp_out = fdopen (tcp_fd, "w")) == NOFILE)
		error ("$*Não consegui obter um FILE para \"%s\"", tcp_dev_nm);

	t_node_to_addr (tcp_fd, argv[2], &local_addr);

	remote_addr = client_addr.a_addr;

#ifdef	DEBUG
	error ("local_addr = %s, ", t_addr_to_str (local_addr));
	error ("remote_addr = %s\n", t_addr_to_str (remote_addr));
#endif	DEBUG

	/*
	 *	Vou realizar o "bind" (usa o mesmo port do FINGER)
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
	 *	Lê a linha do cliente
	 */
	user_nm[0] = '\0';

	if (t_rcv (tcp_fd, user_nm, sizeof (user_nm), &flag) < 0)
		error (NOSTR);

	if ((cp = strchr (user_nm, '\r')) == NOSTR)
		cp = strchr (user_nm, '\n');

	if (cp != NOSTR)
		*cp = '\0';

	fprintf (tcp_out, "Servidor \"finger\" %s\r\n\r\n", pgversion);

	/*
	 *	Abre o arquivo do formato UTMP
	 */
	if ((utmp_fp = fopen (utmp_nm, "r")) == NOFILE)
		error ("$*Não consegui abrir \"%s\"", utmp_nm);

	/*
	 *	Processa o arquivo do formato UTMP
	 */
	while (fread (&utmp, sizeof (UTMP), 1, utmp_fp) == 1)
	{
		if ((initp = getinitnam ((char *)utmp.ut_line)) != NOINITAB)
			local = initp->it_local;
		else
			local = "???";

		if (utmp.ut_name[0] == 0)
			continue;

		if (user_nm[0] != '\0' && !streq ((char *)utmp.ut_name, user_nm))
			continue;

		write_who_line (&utmp, local);
	}

	/*
	 *	Processa o arquivo do formato PASSWD
	 */
	if (user_nm[0] != '\0')
		write_passwd_line (user_nm);

	/*
	 *	Terminou o relatório
	 */
	fflush (tcp_out);

	if (t_sndrel (tcp_fd) < 0)
		error ("$*Erro no SNDREL");

	if (t_rcvrel (tcp_fd) < 0)
		error ("$*Erro no RCVREL");

	return (0);

}	/* end finger_s */

/*
 ****************************************************************
 *	Imprime Uma linha					*
 ****************************************************************
 */
void
write_who_line (const UTMP *up, const char *local)
{
	const char	*cp;
	char		line_nm[32];
	STAT		s;
	static int	first_time;

	/*
	 *	Põe o cabeçalho
	 */
	if (first_time == 0)
	{
		first_time++;

		fprintf
		(	tcp_out,
			"-NOME de LOGIN-  - TTY -  ----- DATA ----- "
			" ------ LOCAL (NÓ) ------\r\n"
		);
	}

	/*
	 *	Põe o usuário e a linha
	 */
	fprintf (tcp_out, "%-16.16s %-8.8s", up->ut_name, up->ut_line);

	/*
	 *	Põe o tempo de "login"
	 */
	cp = btime (&up->ut_time);

	fprintf (tcp_out, " %-5.5s %-10.10s", cp, cp + 9);

	/*
	 *	Tenta obter o estado da linha
	 */
	strcpy (line_nm, "/dev/");
	strcat (line_nm, (char *)up->ut_line);

	if (stat (line_nm, &s) < 0)
		s.st_mode = 0;

#if (0)	/*************************************/
	/*
	 *	Estado (aceita mensagens?)
	 */
	if   (s.st_mode == 0)
		fprintf (tcp_out, "  ?");
	elif ((s.st_mode & (S_IWGRP|S_IWOTH)) == (S_IWGRP|S_IWOTH))
		fprintf (tcp_out, "  +");
	else
		fprintf (tcp_out, "  -");

	time_t	idle_time;

	/*
	 *	Põe o "idle time", se for o caso
	 */
	if (s.st_mode)
		idle_time = (time ((time_t *)NULL) - s.st_atime) / 60;
	else
		idle_time = -1;

	if   (idle_time > 24 * 60)
		fprintf (tcp_out, "   OLD ");
	elif (idle_time > 0)
		fprintf (tcp_out, "  %2d:%02d", idle_time / 60,  idle_time % 60); 
	elif (idle_time == 0)
		fprintf (tcp_out, "    .  ");
	else	/*  < 0  */
		fprintf (tcp_out, "    ?  ");
#endif	/*************************************/

	/*
	 *	Local do terminal
	 */
	if (up->ut_node[0] != 0)
		fprintf (tcp_out, "  (%s)\r\n", up->ut_node);
	else
		fprintf (tcp_out, "  %s\r\n", local);

}	/* end write_who_line */

/*
 ****************************************************************
 *	Imprime dados do arquivo "passwd" de um usuário		*
 ****************************************************************
 */
void
write_passwd_line (const char *user_nm) 
{
	const PASSWD	*pwp;
	const char	*cp;
	int		pw_slot;

	if ((pwp = getpwnam (user_nm)) == NOPASSWD)
	{
		fprintf
		(	tcp_out,
			"Usuário \"%s\" não encontrado no "
			"arquivo de contas/senhas\r\n",
			user_nm
		);

		return;
	}

	pw_slot = getpwslot ();

	fprintf (tcp_out, "\r\n");
	fprintf (tcp_out, "Nome de \"login\": %s\t", pwp->pw_name);
	fprintf (tcp_out, "Na vida real: %s\r\n", pwp->pw_comment);
	fprintf (tcp_out, "Diretório: \"%s\"\t\t", pwp->pw_dir);

	if ((cp = pwp->pw_shell) == NOSTR || cp[0] == '\0')
		cp = DEF_SH;

	fprintf (tcp_out, "Interpretador de comandos: \"%s\"\r\n", cp);

	write_last_login (pwp, pw_slot);

	write_mail_date (pwp);

	write_plan_file (pwp);

	endpwent ();

}	/* end write_passwd_line */

/*
 ****************************************************************
 *	Informa o último "login" do usuário			*
 ****************************************************************
 */
void
write_last_login (const PASSWD *pwp, int slot) 
{
	int		fd, n;
	const char	*cp;
	LASTLOG		lastlog;

	if ((fd = open (llfile, O_RDONLY)) < 0)
	{
		fprintf
		(	tcp_out,
			"Não consegui obter o último \"login\" "
			"(arquivo \"%s\" não disponível)\r\n",
			llfile
		);
		return;
	}

	lseek (fd, slot * sizeof (LASTLOG), SEEK_SET);

	if ((n = read (fd, &lastlog, sizeof (LASTLOG))) < 0)
	{
		fprintf
		(	tcp_out,
			"Não consegui obter o último \"login\" "
			"(erro de leitura do arquivo \"%s\")\r\n",
			llfile
		);

		close (fd); 	return;
	}

	if (n < sizeof (LASTLOG) || lastlog.ll_time == 0)
	{
		fprintf
		(	tcp_out,
			"Ainda não houve nenhum \"login\"\r\n"
		);

		close (fd); 	return;
	}

	cp = btime (&lastlog.ll_time);

	fprintf
	(	tcp_out,
		"Último \"login\" em %-5.5s %-15.15s em \"%s\"",
		cp, cp + 9, lastlog.ll_line
	);

	if (lastlog.ll_node[0])
		fprintf (tcp_out, " (%s)", lastlog.ll_node);

	fprintf (tcp_out, "\r\n");

	close (fd);

}	/* end write_last_login */

/*
 ****************************************************************
 *	Imprime data de leitura do "mail"			*
 ****************************************************************
 */
void
write_mail_date (const PASSWD *pwp) 
{
	const char	*cp;
	STAT		s;

	if (chdir (MAIL_NM) < 0 || stat (pwp->pw_name, &s) < 0)
	{
		fprintf (tcp_out, "O usuário \"%s\" não possui caixa postal\r\n", pwp->pw_name);
		return;
	}

	cp = btime (&s.st_atime);

	fprintf
	(	tcp_out,
		"Última leitura da correspondência em %-5.5s %-15.15s\r\n",
		cp, cp + 9

	);

}	/* end write_mail_date */

/*
 ****************************************************************
 *	Imprime dados do arquivo ".plan" de um usuário		*
 ****************************************************************
 */
void
write_plan_file (const PASSWD *pwp) 
{
	FILE		*plan_fp;
	int		c;

	if ((chdir (pwp->pw_dir)) < 0 || (plan_fp = fopen (PLAN_NM, "r")) == NOFILE)
	{
		fprintf (tcp_out, "O usuário \"%s\" não tem plano\r\n", pwp->pw_name);
		return;
	}

	fprintf (tcp_out, "\r\nPlano:\r\n");

	while ((c = getc (plan_fp)) >= 0)	/* != EOF */
		putc (c, tcp_out);

	fclose (plan_fp);

}	/* end write_plan_file */

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
