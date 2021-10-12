/*
 ****************************************************************
 *								*
 *			login.c					*
 *								*
 *	Controle de sessão - Contas/senhas			*
 *								*
 *	Versão	1.0.0, de 09.02.87				*
 *		4.6.0, de 12.10.04				*
 *								*
 *	Modulo:	login						*
 *		Utilitários básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2004 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/syscall.h>
#include <sys/stat.h>
#include <sys/utsname.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <utmp.h>
#include <signal.h>
#include <pwd.h>
#include <errno.h>
#include <time.h>
#include <termio.h>

/*
 ****************************************************************
 *	Variáveis Globais e Definições				*
 ****************************************************************
 */
#define	EVER	;;
#define	elif	else if

#define	NSPD	86400		/* Nr. de segundos por dia */
#define	PWSZ	13		/* No. de caracteres da senha */

const char	pgversion[] =  "Versão:	4.6.0, de 12.10.04";

/*
 ******	Nomes em geral ******************************************
 */
entry char	maildir[48]  = "/var/mail/";	/* Diretorio do MAIL */
const char	status_nm[]  = "/status";	/* Arquivo de estado do MAIL */
const char	utmpf[]      = "/var/adm/utmp";	/* Arquivo UTMP */
const char	wtmpf[]      = "/var/adm/wtmp";	/* Arquivo WTMP */
const char	llfile[]     = "/var/adm/lastlog"; /* Arquivo LASTLOG */
const char	stdsh[]      = "/bin/sh";	/* Sh padrão */
const char	motdf[]      = "/etc/motd";	/* Mensagem do Dia */
entry char	full_tty_nm[20] = "/dev/";	/* Nome completo do tty */
const char	nopwd[]	     = "-------------"; /* Senha nula */

entry PASSWD	usuarioinv =	/* Para Conta errada */
{
	"",			/* Usuário */
	"?????????????",	/* Senha */
	"",			/* Comentário */
	0, 0,			/* UID, GID */
	0, 0,			/* Quota, Expir */
	"",			/* Lock */
	"",			/* Dir */
	""			/* Shell */
};

entry char	minusnam[16] = "-";	/* Para o Arg 0 do sh */

entry int	caught;			/* Foi dado um sinal */

/*
 ******	Ambiente ************************************************
 */
entry char	homedir[64]	= "HOME=";
entry char	shell[64]	= "SHELL=";
entry char	user[24]	= "USER=";
entry char	term[24]	= "TERM=";

entry const char *envinit[16]	=
{
	homedir,
	shell,
	user,
	term,
	"PATH=.:/bin:/usr/bin:/etc"
};

entry const char **envnext = &envinit[5];	/* proximo a inserir */

/*
 ******	Tabela de Velocidades ***********************************
 */
typedef	struct
{
	char	speedname[6];
	short	speed;

}	SPEED;

const SPEED	speedtb[] =
{
	"110",		B110,
	"134",		B134,
	"150",		B150,
	"200",		B200,
	"300",		B300,
	"600",		B600,
	"1200",		B1200,
	"1.2",		B1200,
	"1800",		B1800,
	"1.8",		B1800,
	"2400",		B2400,
	"2.4",		B2400,
	"4800",		B4800,
	"4.8",		B4800,
	"9600",		B9600,
	"9.6",		B9600,
	"19200",	B19200,
	"19.2",		B19200,
	"38400",	B38400,
	"38.4",		B38400,
	"57600",	B57600,
	"57.6",		B57600,
	"115200",	B115200,
	"115.2",	B115200,
	""

}	/* end speed */;

/*
 ******	Protótipos **********************************************
 */
void		proc_options (const char *, int, char *);
void		catch (int, ...);
void		showmotd (void);

/*
 ****************************************************************
 *	Controle de sessão - Contas/senhas			*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	int		opt;
	char		*cp;
	PASSWD		*pwd;
	const char	*tty_nm;
	int		slot, fd, c;
	int		hoje;
	const char	*TERM;
	char		*STTY;
	int		fflag = 0;
	char		user_nm[16];
	const char	*node_nm;
	UTMP		utmp;
	LASTLOG		lastlog;
	UTSNAME		uts;

	/*
	 *	Forma de Chamar:
	 *
	 *		   0	     1	        2	     3		4
	 *
	 *  login [-f] <ttyslot> <tty_nm> <TERM>,<STTY> [<node_nm> [<user_nm>]]
	 */

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "fH")) != EOF)
	{
		switch (opt)
		{
		    case 'f':			/* Senha já conferida */
			fflag++;
			break;

		    default:			/* Erro */
			putchar ('\n');
		    case 'H':			/* Help */
			error ("$%s", pgversion);

		}	/* end switch */

	}	/* end while */

	argv += optind;
	argc -= optind;

#undef	DEBUG
#ifdef	DEBUG
	error ("Começando com \"%s\", \"%s\" e \"%s\"", argv[0], argv[1], argv[2]); 
#endif	DEBUG

	/*  login [-f] <ttyslot> <tty_nm> <TERM>,<STTY> [<node_nm> [<user_nm>]] */

	if (*argv != NOSTR)
		slot = atoi (*argv++);
	else
		slot = 0;

	if (*argv != NOSTR)
		tty_nm = *argv++;
	else
		tty_nm = "";

	strcat (full_tty_nm, tty_nm);

	if (*argv != NOSTR)
		TERM = *argv++;
	else
		TERM = "";

	if (*argv != NOSTR)
		node_nm = *argv++;
	else
		node_nm = "";

	if (*argv != NOSTR)
		strscpy (user_nm, *argv++, sizeof (user_nm));
	else
		user_nm[0] = '\0';

	/*
	 *	Verifica o usuário efetivo
	 */
	if (geteuid () != 0)
		error ("CUIDADO: NÃO sou superusuário");

	setpgrp ();

	/*
	 *	Zera a Entrada do UTMP (Final de Sessão)
	 */
	signal (SIGQUIT, SIG_IGN);
	signal (SIGINT,  SIG_IGN);

	memset (&utmp, 0, sizeof (UTMP));

	if ((fd = open (utmpf, O_WRONLY)) >= 0)
	{
		lseek (fd, slot * sizeof (UTMP), SEEK_SET);
		write (fd, &utmp, sizeof (UTMP));
		close (fd);
	}

	/*
	 *	Processa o Arquivo "/var/adm/wtmp" (Final de Sessão)
	 */
   /***	clr     (utmp.ut_name); ***/
	strscpy (utmp.ut_line, tty_nm, sizeof (utmp.ut_line));
   /***	clr     (utmp.ut_node); ***/
	time    (&utmp.ut_time);

	if ((fd = open (wtmpf, O_WRONLY)) >= 0)
	{
		lseek (fd, 0, SEEK_END);
		write (fd, &utmp, sizeof (UTMP));
		close (fd);
	}

	/*
	 *	Desmembra o TERM,STTY
	 */
	if ((STTY = strchr (TERM, ',')) != NOSTR)
	{
		*STTY++ = '\0';		/* Pula a vírgula */

		proc_options (full_tty_nm, 2 /* stderr */, STTY);
	}

	/*
	 *	Imprime o cabeçalho (somente se veio do "init")
	 */
	if (node_nm[0] == '\0')
	{
		uname (&uts);

		if (uts.uts_machine[0])
		{
			printf
			(	"\n\a%s (%s, %s)  &  %s %s (%s)\n\n",
				uts.uts_machine, uts.uts_nodename, tty_nm,
				uts.uts_sysname, uts.uts_version, uts.uts_date
			);
		}
		else
		{
			printf ("\n\aTROPIX\n\n");
		}
	}

	/*
	 *	Leitura do nome do usuário
	 */
	if (user_nm[0] == '\0')
	{
	    loop:
		user_nm[0] = '\0';

		while (user_nm[0] == '\0')
		{
			printf ("\aLOGIN: ");

			cp = user_nm;

			while ((c = getchar ()) != '\n' && c != '\r')
			{
				if   (c == EOF)
					return (0);
				elif ('A' <= c && c <= 'Z')
					c += 'a' - 'A';

				if (cp < user_nm + sizeof (user_nm) - 1)
					*cp++ = c;
			}

			*cp++ = '\0';
		}

	}	/* end if (não tem nome) */

	/*
	 *	Prepara para ler a Senha
	 */
	if ((pwd = getpwnam (user_nm)) == NOPASSWD)
		pwd = &usuarioinv;

	/*
	 *	Le a senha
	 */
	if (strlen (pwd->pw_passwd) != PWSZ)
	{
		printf ("Senha encriptada inválida: \"%s\"\n", pwd->pw_passwd);
		goto loop;
	}

	/*
	 *	Confere a primeira senha
	 */
	if (!fflag && !streq (pwd->pw_passwd, nopwd))
	{
		if ((cp = getpass ("Senha: ")) == NOSTR)
			{ printf ("Erro em \"getpass\"\n"); goto loop; }

		if (!streq (crypt (cp, pwd->pw_passwd), pwd->pw_passwd))
			{ printf ("Senha incorreta\n"); goto loop; }
	}

	/*
	 *	Vai para o diretório
	 */
	if (chdir (pwd->pw_dir) < 0)
		error ("*Diretório inválido: %s", pwd->pw_dir);

	/*
	 *	Verifica se a conta está trancada e analisa a data de expiração
	 */
	if (pwd->pw_uid != 0)
	{
		if (pwd->pw_lock[0] == 'U')
			{ printf ("Sua conta está com a cota excedida\n"); goto loop; }

		hoje = time ((time_t *)NULL) / NSPD;

		if (pwd->pw_expir > 0 && pwd->pw_expir < hoje)
			{ printf ("Sua conta está expirada\n"); goto loop; }
	}

	/*
	 *	Processa o Arquivo "/var/adm/utmp" (início de sessão)
	 */
	strscpy (utmp.ut_name, user_nm, sizeof (utmp.ut_name));
   /***	strscpy (utmp.ut_line, tty_nm,  sizeof (utmp.ut_line)); ***/
	strscpy (utmp.ut_node, node_nm, sizeof (utmp.ut_node));
	time    (&utmp.ut_time);

	if ((fd = open (utmpf, O_WRONLY)) >= 0)
	{
		lseek (fd, slot * sizeof (UTMP), SEEK_SET);
		write (fd, &utmp, sizeof (UTMP));
		close (fd);
	}

	/*
	 *	Processa o Arquivo "/var/adm/wtmp" (início de sessão)
	 */
	if ((fd = open (wtmpf, O_WRONLY)) >= 0)
	{
		lseek (fd, 0, SEEK_END);
		write (fd, &utmp, sizeof (UTMP));
		close (fd);
	}

	/*
	 *	Processa o Arquivo LASTLOG
	 */
	if ((c = getpwslot ()) >= 0  &&  (fd = open (llfile, O_WRONLY)) >= 0)
	{
		strcpy (lastlog.ll_line, utmp.ut_line);
		strcpy (lastlog.ll_node, utmp.ut_node);
		lastlog.ll_time = utmp.ut_time;

		lseek (fd, c * sizeof (LASTLOG), SEEK_SET);
		write (fd, &lastlog, sizeof (LASTLOG));

		close (fd);
	}

	endpwent ();

	/*
	 *	Atualiza o dono/grupo
	 */
	if (chown (full_tty_nm, pwd->pw_uid, pwd->pw_gid) < 0)
		error ("*Erro no \"chown\" em \"%s\"", full_tty_nm);

	if (setgid (pwd->pw_gid) < 0)
		error ("*Erro no \"setgid\"");

	if (setuid (pwd->pw_uid) < 0)
		error ("*Erro no \"setuid\"");

	/*
	 *	Prepara o SHELL
	 */
	if (pwd->pw_shell == NOSTR || pwd->pw_shell[0] == '\0')
		pwd->pw_shell = (char *)stdsh;

	/*
	 *	Prepara o Ambiente (Environment)
	 */
	strcat (shell, pwd->pw_shell);
	strcat (homedir, pwd->pw_dir);
	strcat (user, pwd->pw_name);
	strcat (term, TERM);

	/*
	 *	Copia o Environment Herdado
	 */
	if (environ != (const char **)NULL)
	{
		while (*environ != (const char *)NULL)
			*envnext++ = *environ++; 
	}

	environ = envinit;

	/*
	 *	Processa o "arg0" do Shell
	 */
	if ((cp = strrchr (pwd->pw_shell, '/')) == (char *)NULL)
		cp = pwd->pw_shell;
	else
		cp++;

	strcat (minusnam, cp);
	argv[0] = minusnam;
	argv[1] = (char *)NULL;

	/*
	 *	Mostra a mensagem do dia
	 */
	showmotd ();

	/*
	 *	Verifica se tem cartas
	 */
	strcat (maildir, pwd->pw_name);
	strcat (maildir, status_nm);

	if ((fd = open (maildir, O_RDONLY)) >= 0)
	{
		char		status_char;

		if (read (fd, &status_char, 1) == 1 && status_char == 'N')
			printf ("Há correspondência nova para você\n");

		close (fd);
	}

	/*
	 *	Passa o Controle para o Shell
	 */
	signal (SIGQUIT, SIG_DFL);
	signal (SIGINT,  SIG_DFL);
	execv (pwd->pw_shell, argv);

	error ("*Não consegui executar", pwd->pw_shell);

	return (0);

}	/* end login */

/*
 ****************************************************************
 *	Processa o campo de opções				*
 ****************************************************************
 */
void
proc_options (const char *dev_nm, int fd, char *options)
{
	const SPEED	*sp;
	char		*cp;
	TERMIO		mode;
	char		hardware = 0;
	int		c;
	char		area[80];
	char		is_a_modem = 0;

	/*
	 *	Obtém a TERMIO
	 */
	ioctl (fd, TCLOCK);

	if (ioctl (fd, TCGETS, &mode) < 0)
		{ error ("*Não consegui dar ioctl na linha \"%s\"", dev_nm); return; }

	for (options = strfield (options, ','); options != NOSTR; options = strfield (NOSTR, ','))
	{
		if   (streq (options, "iso"))
		{
			mode.c_lflag &= ~CODE; 	mode.c_lflag |= ISO;
		}
		elif (streq (options, "ascii"))
		{
			mode.c_lflag &= ~CODE; 	mode.c_lflag |= ASCII;
		}
		elif (streq (options, "user1"))
		{
			mode.c_lflag &= ~CODE; 	mode.c_lflag |= USER1;
		}
		elif (streq (options, "user2"))
		{
			mode.c_lflag &= ~CODE; 	mode.c_lflag |= USER2;
		}
		elif (streq (options, "clocal"))
		{
			mode.c_cflag |= CLOCAL; hardware++;
		}
		elif (streq (options, "-clocal"))
		{
			mode.c_cflag &= ~CLOCAL; hardware++;
		}
		elif (streq (options, "lowcase"))
		{
			mode.c_iflag &= ~IUCLC;
			mode.c_oflag &= ~OLCUC;
		}
		elif (streq (options, "-lowcase"))
		{
			mode.c_iflag |= IUCLC;
			mode.c_oflag |= OLCUC;
		}
		elif (streq (options, "tabs"))
		{
			mode.c_oflag &= ~TABDLY;
			mode.c_oflag |= TAB0;
		}
		elif (streq (options, "-tabs"))
		{
			mode.c_oflag &= ~TABDLY;
			mode.c_oflag |= TAB3;
		}
		elif (streq (options, "modem"))
		{
			is_a_modem = 1;

			mode.c_oflag &= ~OPOST;

			mode.c_lflag &= ~(ICANON|ECHO);
			mode.c_lflag |=  ICOMM;

			mode.c_cc[VMIN] =  1;
		}
		else for (sp = speedtb; /* teste abaixo */; sp++)
		{
			if (sp->speedname[0] == '\0')
				{ error ("Opção inválida: \"%s\"", options); break; }

			if (streq (options, sp->speedname))
			{
				mode.c_cflag &= ~CBAUD;
				mode.c_cflag |= sp->speed;
				hardware++;
				break;
			}

		}	/* end percorrendo velocidade */

	}	/* end processando as opções */

	/*
	 *	Põe o novo modo
	 */
	if (ioctl (fd, hardware ? TCSETAW : TCSETNR, &mode) < 0)
		error ("*Não consegui dar ioctl na linha \"%s\"", dev_nm);

	ioctl (fd, TCFREE);

	/*
	 *	Verifica se deve processar o nodem
	 */
	if (is_a_modem)
	{
		/*
		 *	Desliga o MODEM
		 */
		fprintf (stderr, "+++");
		fflush (stderr);
		sleep (4);

		while (ioctl (fd, TCNREAD) > 0)
			getchar ();

		fprintf (stderr, "ATH0\r");
		fflush (stderr);
		sleep (3);
		while (ioctl (fd, TCNREAD) > 0)
			getchar ();

		/*
		 *	Reset do MODEM
		 */
		fprintf (stderr, "ATZ\r");
		fflush (stderr);
		sleep (4);
		while (ioctl (fd, TCNREAD) > 0)
			getchar ();

		/*
		 *	Cadeia para o MODEM atender
		 */
		fprintf (stderr, "ATS0=1\r");
		fflush (stderr);

		cp = area;

		for (EVER)
		{
			c = getchar ();

			if   (c == '\r')
			{
				continue;
			}
			elif (c == '\n')
			{
				*cp++ = 0;

				if (!strncmp (area, "CONNECT", 7))
					break;

				cp = area;
				continue;
			}

			if (cp < area + 80)
				*cp++ = c;
		}

		ioctl (fd, TCLOCK);

		ioctl (fd, TCGETS, &mode);

		mode.c_oflag |= OPOST;

		mode.c_lflag |= (ICANON|ECHO);
		mode.c_lflag &=  ~ICOMM;

		if (ioctl (fd, TCSETNR, &mode) < 0)
			error ("*Não consegui dar ioctl na linha \"%s\"", dev_nm);

		ioctl (fd, TCFREE);
	}

}	/* end proc_options */

/*
 ****************************************************************
 *	Para receber o Sinal					*
 ****************************************************************
 */
void
catch (int signo, ...)
{
	signal (SIGINT, SIG_IGN); caught++;

}	/* end catch */

/*
 ****************************************************************
 *	Imprime a Mensagem do Dia				*
 ****************************************************************
 */
void
showmotd (void)
{
	FILE	*fp;
	int	c;

	signal (SIGINT, catch);

	if ((fp = fopen (motdf, "r")) != NOFILE)
	{
		while ((c = getc (fp)) != EOF && !caught)
			putchar (c);

		if (caught)
			putchar ('\n');

		fclose (fp);
	}

	signal (SIGINT, SIG_IGN);

}	/* end showmotd */
