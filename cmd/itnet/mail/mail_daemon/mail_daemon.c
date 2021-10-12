/*
 ****************************************************************
 *								*
 *			mail_daemon.c				*
 *								*
 *	Providencia o reenvio de uma carta			*
 *								*
 *	Versão	4.9.0, de 26.09.07				*
 *		4.9.0, de 26.09.07				*
 *								*
 *	Módulo: MAIL						*
 *		Utilitários de comunicações			*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2007 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <pwd.h>
#include <time.h>
#include <locale.h>
#include <signal.h>
#include <xti.h>
#include <errno.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
#define	NOCSTR	(const char **)NULL
#define	NOVOID	(void *)NULL
#define	EVER	;;
#define	elif	else if
#define STR(x)	# x
#define XSTR(x)	STR (x)

enum {	MAX_VV		= 1024 };	/* Número máximo de argumentos para "send_mail" */

enum {	TIME_INTERVAL	= 20 };	/* Intervalo de tempo entre reenvios */

#if (0)	/*******************************************************/
enum {	TIME_INTERVAL	= 5 * 60 };	/* Intervalo de tempo entre reenvios */
#endif	/*******************************************************/

const char		mail_daemon_file_nm[] = "/var/tmp/mail_daemon_file";

/*
 ******	Variáveis ***********************************************
 */
const char		pgversion[] =  "Versão: 4.9.0, de 26.09.07";


const char		send_mail_nm[] = "/usr/lib/send_mail";
const char		* const send_mail_arg0 = send_mail_nm + 9;

entry int		vflag = 0;		/* Verbose */

/*
 ******	Protótipos **********************************************
 */
void		read_and_process_file (void);
void		examine_line (char *line);
void		error (const char *, ...);

/*
 ****************************************************************
 *	Retransmite as cartas periodicamente			*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	int			opt, pid;

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
	 *	Gera um filho, para não depender mais do pai
	 */
	if ((pid = fork ()) < 0)
		{ error ("$*Não consegui dar \"fork\""); return (-1); }

	if (pid > 0)
		return (0);

	/*
	 *	Malha sem fim, examinando o arquivo de reenvios
	 */
	for (EVER)
	{
		read_and_process_file ();

		sleep (TIME_INTERVAL);
	}

}	/* end mail_daemon */

/*
 ****************************************************************
 *	Lê e processa o arquivo					*
 ****************************************************************
 */
void
read_and_process_file (void)
{
	int		file_fd;
	STAT		file_stat;
	char		*file_area, *line;

	/*
	 *	Abre o arquivo, copia todas as linhas para a memória, e apaga o arquivo
	 */
	if ((file_fd = open (mail_daemon_file_nm, O_RDONLY)) < 0)
	{
#ifdef	DEBUG
		error ("*Não consegui abrir \"%s\"", mail_daemon_file_nm);
#endif	DEBUG
		return;
	}

	if (fstat (file_fd, &file_stat) < 0)
		{ error ("*Não consegui obter o estado de \"%s\"", mail_daemon_file_nm); goto bad; }

	file_area = alloca (file_stat.st_size + 1);

	if (read (file_fd, file_area, file_stat.st_size) != file_stat.st_size)
		{ error ("*Não consegui ler o arquivo \"%s\"", mail_daemon_file_nm); goto bad; }

	close (file_fd);

	if (unlink (mail_daemon_file_nm) < 0)
		error ("*Não consegui remover \"%s\"", mail_daemon_file_nm);

	file_area[file_stat.st_size] = '\0';

	/*
	 *	Examina agora, cada linha
	 */
	for (line = strfield (file_area, '\n'); line != NOSTR; line = strfield (NOSTR, '\n'))
	{
		if (line[0] != '\0')
			examine_line (line);
	}

	return;

	/*
	 *	Em caso de erro, ...
	 */
    bad:
	close (file_fd);

}	/* end read_and_process_file */

/*
 ****************************************************************
 *	Examina e processa uma linha				*
 ****************************************************************
 */
void
examine_line (char *line)
{
	char		*token;
	int		pid, already_sent_count;
	char		already_sent_count_str[16];
	const char	**end_vv, **vp;
	const char	*vv[MAX_VV];

	/*
	 *	Formato da linha;
	 *
	 *		23	[-vf]	<send_mail_file_nm>   <snd_nm>	<dst_nm> <dst_nm> ...
	 *
	 *		|    
	 *		|=====> No. de tentativas (de envio) já efetuadas
	 *
	 */
	if ((token = strtok (line, " \t")) == NOSTR)
		return;

	if ((already_sent_count = atoi (token)) <= 0)
		already_sent_count = 1;

	sprintf (already_sent_count_str, "%d", already_sent_count + 1);

	/*
	 *	Põe os argumentos fixos
	 */
	vp = &vv[0]; end_vv = &vv[MAX_VV - 1];

	*vp++ = send_mail_arg0;		/* 0: send_mail */

	if (vflag)
		*vp++ = "-v";		/* 1: -v */

	*vp++ = "-n";			/* 2: -n */

	*vp++ = already_sent_count_str;	/* 3: <already_sent_count> */

	/*
	 *	Põe o <remetente> e os <destinatários>
	 */
	while ((token = strtok (NOSTR, " \t")) != NOSTR)
	{
		if (vp >= end_vv)
			{ error ("Argumentos demais"); break; }

		*vp++ = token;
	}

	*vp = NOSTR;

#undef	MAIL_DAEMON_DEBUG
#ifdef	MAIL_DAEMON_DEBUG
	fprintf (stderr, "MAIL_DAEMON: ");

	for (vp = vv; *vp != NOSTR; vp++)
		fprintf (stderr, "%s ", *vp);

	fprintf (stderr, "\n");
#else
	if   ((pid = fork ()) < 0)
		{ error ("*Não consegui criar um processo"); return; }
	elif (pid != 0)
		return;

	signal (SIGINT,  SIG_IGN);
	signal (SIGQUIT, SIG_IGN);

	execv (send_mail_nm, vv);
	error ("$*Não consegui executar \"%s\"", send_mail_nm);
#endif	MAIL_DAEMON_DEBUG

}	/* end examine_line */

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
	int		save_errno = errno;
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
		fmt = "*Erro NÃO dado";

	if ((abrt = *fmt) == '$')
		fmt++;

	if ((err = *fmt) == '*')
		fmt++;

	/*
	 *	Escreve as mensagens
	 */
	if (log_fp != NOFILE)
	{
		/* Escreve a mensagem com o tempo */

		fprintf  (log_fp, pgname);

		time (&time_loc); fprintf (log_fp, " (%.24s): ", btime (&time_loc));

		vfprintf (log_fp, fmt, args);

		/* Escreve o tipo do erro */

		if (err == '*')
			fprintf (log_fp, " (%s)", strerror (save_errno));

		putc ('\n', log_fp); fflush (log_fp);
	}

	va_end (args);

}	/* end error */
