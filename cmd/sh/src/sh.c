/*
 ****************************************************************
 *								*
 *			sh.c					*
 *								*
 *	Módulo Principal					*
 *								*
 *	Versão	3.2.3, de 19.01.00				*
 *		4.5.0, de 12.11.03				*
 *								*
 *	Módulo: SHELL						*
 *		Interpretador de Comandos			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2003 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/common.h>
#include <sys/scb.h>
#include <sys/syscall.h>
#include <sys/stat.h>

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <setjmp.h>
#include <errno.h>

#include "../h/common.h"
#include "../h/sym.h"
#include "../h/scan.h"
#include "../h/command.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
#define	HOME		"/home"
#define	PATH		".:/bin:/usr/bin"
#define	SPATH		".:/bin:/usr/bin:/etc"
#define	UPS1		"'$ '"
#define	SPS1		"'# '"
#define	PS2		"'> '"
#define	PROFILE		".profile"
#define	ETCPROFILE	"/etc/.profile"
#define	LOGOUT		".logout"

/*
 ******	Variáveis Profundamente Globais *************************
 */
entry	const char	pgversion[]	= "Versão: 4.5.0, de 12.11.03";

entry	int		dflag,		/* Sai com Control-D */
			quick_exit,	/* Termina houver der erro em comando */
			rewriting = 1,	/* Verifica a reescrita */
			vflag;		/* Verboso */

entry	int		script,		/* Executando arquivo de comandos */
			interact;	/* Interativa */

entry	int		my_pid,		/* PID da Shell */
			user_id;	/* UID */

entry	int		max_fd;		/* Número máximo de FD's */

entry	SCB		scb;		/* System Control Block */

entry	int		npospar;	/* Nr. de Parâmetros Posicionais */
entry	const char	*pospar[NPAR+2];/* Parâmetros Posicionais */

entry	int		ignore_signals;		/* Ignora INT, QUIT e TERM */
entry	const char	*trap[NSIG];		/* Comandos associados ao sinais */
entry	void		(*sig[NSIG]) (int, ...);/* Estados antigos dos sinais */

entry	jmp_buf		env;

/*
 ****** Protótipos de funções ***********************************
 */
void		initialize (void);
void		define_variables_from_environment (void);
void		help (int);

/*
 ****************************************************************
 *	Ponto de Entrada					*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	int		opt, ret;
	const char	*command = NOSTR;
	CMD		*cmd;
	static int	loginsh;

	pospar[0] = argv[0];
	npospar   = 1;

	/*
	 *	Verifica se é "-sh".
	 */
	if (*pgname == '-')
		{ loginsh++; pgname++; };

	/*
	 *	Analisa as opções.
	 */
	while ((opt = getopt (argc, argv, "c:dervH")) != EOF)
	{
		switch (opt)
		{
		    case 'c':		/* Argumento é linha de comando */
			command = optarg;
			break;

		    case 'd':		/* Aceita <^D> */
			dflag++;
			break;

		    case 'e':		/* Termina se houver erro em comando */
			quick_exit++;
			break;

		    case 'r':		/* Verifica a reescrita */
			rewriting = 0;
			break;

		    case 'v':		/* Verbose */
			vflag++;
			break;

		    case 'H':		/* Help */
			help (0);

		    default:		/* Erro */
			putc ('\n', stderr);
			help (2);

		}	/* end switch (opção) */

	}	/* end while (analisando opções) */

	argv += optind;
	argc -= optind;

	initialize ();

	define_variables_from_environment ();

	restart_scanner ();

	/*
	 *	Se foi dada a opção "-c", executa somente a linha de
	 *	comando.
	 */
	if (command != NOSTR)
	{
		insert_line (command);
		return (exec (get_command (M_NOFLAGS), 0, 0, 0));
	}

	/*
	 *	Prepara a tabela HASH de executáveis.
	 */
	exec_hash_create (0 /* silencioso */);

	/*
	 *	Se foi dado o nome de um arquivo de comandos, executa-o.
	 */
	if (*argv != NOSTR)
	{
		define_params (argc, 0, argv);
		return (exec_command_file (*argv, 1 /* must exist */, 1));
	}

	/*
	 *	Provavelmente: sh < x.
	 */
	if (!interact)
	{
		dflag = 1;
		return (exec_command_file (NOSTR, 0, 0));
	}

	/*
	 *	Intercepta os sinais de terminação e o SIGWINCH.
	 */
	signal (SIGINT,	  handle_signals);
	signal (SIGQUIT,  handle_signals);
	signal (SIGTERM,  handle_signals);
	signal (SIGWINCH, handle_sigwinch);

	/*
	 *	Se argv[0] = "-sh", executa os "profiles".
	 */
	if (loginsh)
	{
		if (exec_command_file (PROFILE, 0, 1) < 0)
			exec_command_file (ETCPROFILE, 0, 1);
	}

	/*
	 *	Os sinais interceptados causam desvio para este ponto.
	 */
	setjmp (env);

	/*
	 *	Malha de leitura/execução de comandos.
	 */
	ret = 0;

	while ((cmd = get_command (M_NOFLAGS)) != ENDCMD)
	{
		if (cmd->c_fid < F_EDIT_HIST || cmd->c_fid > F_SHOW_HIST)
			put_history (linebuf);

		ret = exec (cmd, 0, 0, 0);
	}

	/*
	 *	Executa o comando de finalização, se for o caso.
	 */
	if (trap[0] != NOSTR && *trap[0] != NOCHR)
	{
		insert_line (trap[0]);
		ret = exec (get_command (M_NOFLAGS), 0, 0, 0);
	}

	/*
	 *	Executa o ".logout", se for o caso.
	 */
	if (loginsh)
	{
		chdir (home_sp->s_value);
		exec_command_file (LOGOUT, 0, 1);
	}

	return (ret);

}	/* end main */

/*
 ****************************************************************
 *	Inicializa diversas variáveis				*
 ****************************************************************
 */
void
initialize (void)
{
	const char	*s;
	int		histmax;

	/*
	 *	Obtém diversos valores.
	 */
	my_pid  = getpid ();	/* Guarda o PID */

	user_id = geteuid ();	/* Guarda o UID */

	max_fd  = NUFILE - 4;	/* Guarda o nr. máximo de fd's */

	getscb (&scb);		/* Obtém o "System Control Block" */

	error_default_text = "Memória esgotada";

	/*
	 *	Verifica se é uma shell interativa.
	 */
	interact = isatty (fileno (stdin)) && isatty (fileno (stdout));

	/*
	 *	Define os dois "prompts" (PS1, PS2)
	 */
	if ((s = getenv ("PS1")) == NOSTR)
		s = user_id ? UPS1 : SPS1;

	ps1_sp = define_variable ("PS1", s,  V_PERMANENT);

	if ((s = getenv ("PS2")) == NOSTR)
		s = PS2;

	ps2_sp = define_variable ("PS2", s, V_PERMANENT);

	/*
	 *	Define PATH.
	 */
	if ((s = getenv ("PATH")) == NOSTR)
		s = user_id ? PATH : SPATH;

	path_sp = define_variable ("PATH", s, V_EXPORT|V_PERMANENT);

	/*
	 *	Define HOME.
	 */
	if ((s = getenv ("HOME")) == NOSTR)
		s = HOME;

	home_sp = define_variable ("HOME", s, V_EXPORT|V_PERMANENT);

	/*
	 *	Define o Diretório Corrente inicial
	 */
	update_cwd ();

	/*
	 *	Define tamanho do histórico.
	 */
	if ((s = getenv ("HIST")) == NOSTR)
	{
		histmax = HISTSZ;
	}
	else
	{
		if ((histmax = atol (s)) <= 0)
			histmax = HISTSZ;
	}

	set_history_size (histmax);

	hist_sp = define_variable ("HIST", HISTMAX, V_PERMANENT);

	save_fds ();

}	/* end initialize */

/*
 ****************************************************************
 *	Define os parâmetros posicionais			*
 ****************************************************************
 */
int
define_params (int argc, int whence, const char *argv[])
{
	int		opt;

	/*
	 *	Libera os parâmetros antigos, mantendo apenas o $0.
	 */
	if (npospar)
	{
		for (opt = 1; opt < npospar; opt++)
		{
			if (pospar[opt] != NOSTR)
				free ((char *)pospar[opt]);
		}
	}

	/*
	 *	Copia os novos parâmetros.
	 */
	for (opt = 0; opt < argc; opt++)
	{
		if (whence < NPAR + 1)
			pospar[whence++] = strdup (argv[opt]);
	}

	pospar[whence] = NOSTR;

	npospar = whence;		/* $# */

	return (0);

}	/* end define_params */

/*
 ****************************************************************
 *	Instala as variáveis do ambiente			*
 ****************************************************************
 */
void
define_variables_from_environment (void)
{
	char		**envp, *str;
	SYM		*sp;

	for (envp = (char **)environ; (str = *envp) != NOSTR; envp++)
	{
		sp = define_variable_from_string (str);

		if (sp == NOSYMP)
			error ("Não consegui definir a variável %s", str);
		else
			sp->s_flags |= V_EXPORT;
	}

}	/* end define_variables_from_environment */

/*
 ****************************************************************
 *	Resumo de utilização do programa			*
 ****************************************************************
 */
void
help (int code)
{
	fprintf
	(	stderr,
		"%s - interpretador de comandos\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-c <comando>] [-derv] [<arg> ...]\n",
		pgname, pgversion, pgname
	);

	fprintf
	(	stderr,
		"\nOpções:"
		"\t-c: Executa o <comando> e encerra\n"
		"\t-d: Aceita também <^D> para sair de sessão\n"
		"\t-e: Termina a execução se um comando"
				 " retornar não-zero\n"
		"\t-r: Verifica a reescrita de arquivos\n"
		"\t-v: Lista os comandos do arquivo de entrada\n"
	);

	exit (code);

}	/* end help */

/*
 ****************************************************************
 *	Intercepta os sinais					*
 ****************************************************************
 */
void
handle_signals (int signo, ...)
{
	CMD		*cmd;

	if (ignore_signals)
	{
		signal (signo, handle_signals);
		return;
	}

	if (!interact || my_pid != getpid ())
		exit (1);

	signal (signo, SIG_IGN);

	restart_scanner ();

	script = 0;

	/*
	 *	Executa o comando associado ao sinal, se houver.
	 */
	if (trap[signo] != NOSTR)
	{
		insert_line (trap[signo]);

		if ((cmd = get_command (M_NOFLAGS)) != ENDCMD)
		{
			if (cmd->c_fid < F_EDIT_HIST || cmd->c_fid > F_SHOW_HIST)
				put_history (linebuf);

			exec (cmd, 0, 0, 0);
		}
	}

	signal (signo, handle_signals);

	longjmp (env, 1);

}	/* end handle_signals */
