/*
 ****************************************************************
 *								*
 *			xinit.c					*
 *								*
 *	Executa o servidor X e um cliente			*
 *								*
 *	Versão	3.0.0, de 27.12.97				*
 *		4.9.0, de 14.06.06				*
 *								*
 *	Módulo: cmd/xinit					*
 *		X Windows - Clientes				*
 *		Baseado no software homônimo do XFree86		*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright (c) 2006 TROPIX Technologies Ltd.	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <signal.h>
#include <xti.h>
#include <errno.h>
#include <termio.h>

#include <X11/Xlib.h>

/*
 ****************************************************************
 *	Nome e versão do programa				*
 ****************************************************************
 */
entry	const char	pgversion[]	= "Versão 4.9.0, de 14.06.06";

/*
 ****************************************************************
 *	Definições de Macros					*
 ****************************************************************
 */
#define	TIMEOUT		16	/* tempo limite para o servidor instalar-se */

/*
 ****************************************************************
 *	Variáveis globais					*
 ****************************************************************
 */
entry	const char	xinitrc[]	  = "/.xinitrc";
entry	const char	log_file[]	  = "/var/log/xwin/xclients.log";
entry	const char	grave_dev[]	  = "/dev/grave";
entry	const char	console[]	  = "/dev/console";
entry	const char	video[]		  = "/dev/video";

entry	const char	default_display[] = ":0";

entry	char		*server_argv[50]  = { "xserver", NOSTR };
entry	char		*client_argv[50]  = { "sh", "fvwm", NOSTR };
entry	char		**server	  = server_argv;
entry	char		**client	  = client_argv + 1;

entry	int		server_pid = -1;
entry	int		client_pid = -1;
entry	int		server_installed;
entry	int		grave_fd = -1;

entry	Display		*dpy;

/*
 ****************************************************************
 *	Protótipos de Funções					*
 ****************************************************************
 */
void	check_tty (void);
void	create_log_file (void);
void	shutdown (void);
void	help (int);
void	sig_catch (int, ...);
int	start_server (const char *[]);
int	start_client (const char *[]);
void	kill_server_group (void);
void	kill_client_group (void);
void	wait_for_something (void);
int	analyze_death (void);
void	XDisconnectDisplay (Display *);

extern void	msg (const char *, ...);

/*
 ****************************************************************
 *	Ponto de Entrada					*
 ****************************************************************
 */
void
main (int argc, const char *argv[])
{
	char		**cp;
	int		fd;


	/*
	 *	Infelizmente, não é possível usar "getopt".
	 */
	argv++;

	if (*argv != NOSTR && streq (*argv, "-H"))
		help (0);

	/*
	 *	Verifica se foi executado em modo texto.
	 */
	check_tty ();

	create_log_file ();

	/*
	 *	Obtém os argumentos para o cliente.
	 */
	if (*argv != NOSTR)
	{
		cp = server_argv;

		while (*argv != NOSTR && !streq (*argv, "--"))
			*cp++ = (char *)*argv++;

		*cp = NOSTR;

		if (cp > server_argv)
			client--;

		if (*argv != NOSTR)
			argv++;		/* Pula o "--" */
	}

	/*
	 *	Obtém os argumentos para o servidor.
	 */
	if (*argv != NOSTR)
	{
		for (cp = server; *argv != NOSTR; argv++)
			*cp++ = (char *)*argv;

		*cp = NOSTR;
	}

	if (client != client_argv)
	{
		/*
		 *	O cliente não foi mencionado na linha de comando.
		 *	Verifica se o arquivo $HOME/.xinitrc existe.
		 */
		char		*buf;
		const char	*home;

		if ((home = getenv ("HOME")) != NOSTR)
		{
			buf = alloca (strlen (home) + sizeof (xinitrc));

			strcpy (buf, home);
			strcat (buf, xinitrc);

			if (access (buf, F_OK) == 0)
				*client-- = buf;	/* inclui a shell */
		}
	}

	/*
	 *	Programa alguns sinais.
	 */
	signal (SIGQUIT, sig_catch);
	signal (SIGINT,  sig_catch);
	signal (SIGHUP,  sig_catch);
	signal (SIGALRM, sig_catch);
	signal (SIGUSR1, sig_catch);

	/*
	 *	Abre a tumba.
	 */
	if ((grave_fd = open (grave_dev, O_RDWR)) < 0)
		msg ("$*não consegui abrir o dispositivo \"%s\"", grave_dev);

	/*
	 *	Dispara o servidor.
	 */
	printf
	(	"*** Servidor: \"%s\", Cliente: \"%s\"\n",
		server[0], client[0]
	);

	fclose (stdin);		/* Não vai precisar mesmo... */
	fclose (stdout);

	settgrp (0);		/* Para não aparecer no "ps" */

	if (start_server ((const char **)server) < 0)
		exit (1);

	/*
	 *	Dispara o cliente.
	 */
	setgid (getgid ());	/* Volta à identificação real */
	setuid (getuid ());

	if (start_client ((const char **)client) < 0)
	{
		kill_server_group ();
		exit (1);
	}

	/*
	 *	Servidor e cliente no ar.
	 */
	msg
	(	"PID do cliente = %d, PID do servidor = %d",
		client_pid, server_pid
	);

	wait_for_something ();

	if ((fd = open ("/dev/console", O_RDONLY)) < 0)
	{
		msg ("*Erro ao abrir \"/dev/console\"");
	}
	else
	{
		if (ioctl (fd, TC_RESTORE_VGA) < 0)
			msg ("*Erro no TC_RESTORE_VGA");
	}

	msg ("Término OK");

	exit (0);

}	/* end main */

/*
 ****************************************************************
 *	Verifica se o terminal é a console			*
 ****************************************************************
 */
void
check_tty (void)
{
	char	*tty_name;

	tty_name = ttyname (2);

	if (streq (tty_name, console) || strncmp (tty_name, video, 10) == 0)
		return;

	fprintf
	(	stderr,
		"Não é possível executar \"%s\" a partir de \"%s\"\n",
		pgname, tty_name
	);

	exit (1);

}	/* end check_tty */

/*
 ****************************************************************
 *	Redireciona o "stderr" para o arquivo de log		*
 ****************************************************************
 */
void
create_log_file (void)
{
	int	fd;

	fflush (stderr);

	if ((fd = open (log_file, O_CREAT|O_TRUNC|O_WRONLY, 0644)) >= 0)
	{
		chown (log_file, getuid (), getgid ());
		dup2 (fd, fileno (stderr));
		close (fd);
	}

}	/* end create_log_file */

/*
 ****************************************************************
 *	Verifica a conexão com o servidor			*
 ****************************************************************
 */
void
wait_for_something (void)
{
	int	nfd, vet[2], pid;
	int	client_dead, server_dead, disconnected;

	vet[0]	= grave_fd;
	vet[1]	= ConnectionNumber (dpy);
	nfd	= 2;

	disconnected = client_dead = server_dead = 0;

	while (!client_dead || !server_dead)
	{
		switch (attention (nfd, vet, -1, 0))
		{
		    case 0:
			/*
			 *	Analisa a tumba, para saber quem morreu.
			 */
			pid = analyze_death ();

			if (pid == client_pid)
			{
				client_dead++;

				if (!disconnected)
				{
					XCloseDisplay (dpy);
					disconnected++;
					nfd = 1;
				}
			}
			else if (pid == server_pid)
			{
				server_dead++;

				if (!client_dead)
					kill_client_group ();

				nfd = 1;
			}
			break;

		    case 1:
			/*
			 *	"fd" da conexão com o X.
			 */
			msg
			(	"O servidor \"%s\" desconectou",
				DisplayString (dpy)
			);

			XDisconnectDisplay (dpy);
			disconnected++;
			nfd = 1;
			break;

		    case -1:
			if (errno == EINTR)
			{
				msg ("\"attention\" interrompida");
			}
			else
			{
				msg
				(	"Erro no \"attention\" = %s, "
					"estado do fd = %d",
					strerror (errno), t_getstate (vet[1])
				);
			}
			break;
		}
	}

}	/* end wait_for_something */

/*
 ****************************************************************
 *	Rotina de Necrópsia					*
 ****************************************************************
 */
int
analyze_death (void)
{
	int	code = 0, status, pid;
	char	*who;

	pid = wait (&status);

	if (pid == client_pid)
		who = "CLIENTE";
	else if (pid == server_pid)
		who = "SERVIDOR";
	else
		who = "DESCONHECIDO";

	msg ("Morreu o processo %d (%s)", pid, who);

	if (status)
	{
		if ((code = status & 0x7F) != 0)
		{
			/* Término em virtude de um sinal */

			fprintf (stderr, "*** %s", sigtostr (code));
			fprintf (stderr, " PC: %P", _fpc);

			if (code == SIGBUS || code == SIGADR || code == SIGSEGV)
			{
				fprintf (stderr, " Addr: %P", _faddr);
			}
			else if (code == SIGILL)
			{
				fprintf
				(	stderr,
					" INST: %04X",
					(int)_faddr & 0xFFFF
				);
			}
		}
		else
		{
			/* "exit" com valor não-nulo */

			fprintf
			(	stderr,
				"*** código de retorno = %d",
				status >> 8
			);
		}

		putc ('\n', stderr);
		fflush (stderr);
	}

	return (pid);

}	/* end analyze_death */

/*
 ****************************************************************
 *	Ensina a usar o programa				*
 ****************************************************************
 */
void
help (int n)
{
	fprintf
	(	stderr,
		"%s - executa localmente o servidor X e um cliente\n\n"
		"%s\n\n"
		"Sintaxe:\n"
		"\t%s [<argumentos do cliente>] "
		"[-- <argumentos do servidor>]\n\n",
		pgname, pgversion, pgname
	);

	exit (n);

}	/* end help */

/*
 ****************************************************************
 *	Captura sinais						*
 ****************************************************************
 */
void
sig_catch (int sig, ...)
{
	signal (sig, sig_catch);

	if (sig == SIGUSR1)
		server_installed++;
	else
		msg ("Recebi o sinal %s", sigtostr (sig));

}	/* end sig_catch */

/*
 ****************************************************************
 *	Dispara o servidor					*
 ****************************************************************
 */
int
start_server (const char *xserver[])
{
	switch (server_pid = fork ())
	{
	    case 0:
		/*
		 *	O servidor é líder de seu grupo.
		 *	Ao notar SIGUSR1 programado com SIG_IGN,
		 *	o servidor X enviará o mesmo sinal ao pai
		 *	(o xinit), revelando-se apto a aceitar conexões.
		 */
		(void)setpgrp ();
		signal (SIGUSR1, SIG_IGN);

		execvp (xserver[0], xserver);
		msg ("servidor \"%s\" não encontrado", xserver[0]);

		/*
		 *	Não achou o servidor: envia logo o "timeout"
		 *	para o "xinit".
		 */
		kill (getppid (), SIGALRM);
		exit (1);

	    case -1:
		msg ("*Não consegui dar \"fork\" para executar o servidor");
		break;

	    default:
		/*
		 *	Deixa o servidor instalar-se calmamente,
		 *	até TIMEOUT segundos.
		 */
		alarm (TIMEOUT);

		if (!server_installed)
			pause ();

		if (!server_installed)
		{
			msg
			(	"o servidor \"%s\" não dá sinal de vida",
				server[0]
			);
			return (-1);
		}

		alarm (0);

		if ((dpy = XOpenDisplay (default_display)) == NULL)
		{
			msg
			(	"conexão impossível com servidor \"%s\"",
				XDisplayName (default_display)
			);
			return (-1);
		}

		break;
	}

	return (server_pid);

}	/* end start_server */

/*
 ****************************************************************
 *	Dispara o cliente					*
 ****************************************************************
 */
int
start_client (const char *xclient[])
{
	if ((client_pid = fork ()) == 0)
	{
		/* O cliente é líder de um grupo de processos */

		(void)setpgrp ();
		execvp (xclient[0], xclient);
		msg ("$cliente \"%s\" não encontrado", xclient[0]);
	}

	return (client_pid);

}	/* end start_client */

/*
 ****************************************************************
 *	Envia o SIGTERM ao grupo do servidor			*
 ****************************************************************
 */
void
kill_server_group (void)
{
	if (kill (-server_pid, SIGTERM) < 0)
	{
		msg
		(	"não consegui enviar o SIGTERM ao servidor (PID = %d)",
			server_pid
		);
	}
	else
	{
		msg
		(	"enviei o SIGTERM ao servidor (PID = %d)",
			server_pid
		);
	}

}	/* end kill_server_group */

/*
 ****************************************************************
 *	Envia o SIGKILL ao grupo do cliente			*
 ****************************************************************
 */
void
kill_client_group (void)
{
	if (kill (-client_pid, SIGKILL) < 0)
	{
		msg
		(	"não consegui enviar o SIGKILL ao grupo %d",
			client_pid
		);
	}
	else
	{
		msg
		(	"enviei o SIGKILL ao grupo %d",
			client_pid
		);
	}

}	/* end kill_client_group */
