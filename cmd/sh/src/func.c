/*
 ****************************************************************
 *								*
 *			func.c					*
 *								*
 *	Execução de Funções Internas				*
 *								*
 *	Versão	3.2.3, de 03.01.00				*
 *		4.5.0, de 10.10.03				*
 *								*
 *	Módulo: SHELL						*
 *		Interpretador de Comandos			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2003 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include	<sys/common.h>
#include	<sys/syscall.h>
#include	<sys/times.h>
#include	<sys/stat.h>
#include	<sys/scb.h>

#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<signal.h>

#include	"../h/common.h"
#include	"../h/sym.h"
#include	"../h/scan.h"
#include	"../h/command.h"

#include	"../h/extern.h"
#include	"../h/proto.h"

/*
 ****************************************************************
 *	Variáveis globais					*
 ****************************************************************
 */
#define	CWD_SZ		512

entry char	*current_wd;	/* Nome do diretório corrente */

/*
 ****************************************************************
 *	Protótipos de Funções					*
 ****************************************************************
 */
int	do_cd (CMD *);
int	do_edit_hist (CMD *, int, int, int);
int	do_graphic_hist (CMD *, int, int, int);
int	do_eval (CMD *, int, int, int);
int	do_exec_hist (CMD *, int, int, int);
int	do_read (CMD *);
int	do_set (CMD *);
int	do_set_flags (CMD *, int);
int	do_shift (CMD *);
int	do_trap (CMD *);
int	do_umask (CMD *);
int	do_unset (CMD *);
void	wrtimes (time_t, char);

/*
 ****************************************************************
 *	Verifica se é uma função interna			*
 ****************************************************************
 */
FUNCID
check_func (const char *name)
{
	SYM		*sp;

	if ((sp = search_sym (name)) == NOSYMP)
		return (F_NOFUNC);

	return (sp->s_type == S_FUNC ? sp->s_funcid : F_NOFUNC);

}	/* end check_func */

/*
 ****************************************************************
 *	Executa uma função interna				*
 ****************************************************************
 */
int
exec_func (CMD *cmd, int fdin, int fdout, int extra)
{
	MUTMS		t;
	int		ret;
	char		*tty;

	switch (cmd->c_fid)
	{
		/*
		 *	Comando "h".
		 */
	    case F_SHOW_HIST:
		return (show_history (stdout), -2);

		/*
		 *	Comando "eh".
		 */
	    case F_EDIT_HIST:
		if (!interact)
			return (1);

		return (do_edit_hist (cmd, fdin, fdout, extra));

		/*
		 *	Comando "gh".
		 */
	    case F_GRAPHIC_HIST:
		if (!interact)
			return (1);

		return (do_graphic_hist (cmd, fdin, fdout, extra));

		/*
		 *	Comando "xh".
		 */
	    case F_EXEC_HIST:
		if (!interact)
			return (1);

		return (do_exec_hist (cmd, fdin, fdout, extra));

		/*
		 *	Comando "eval".
		 */
	    case F_EVAL:
		return (do_eval (cmd, fdin, fdout, extra));

		/*
		 *	Comando "exec".
		 */
	    case F_EXEC:
		if (cmd->c_argc < 2)
		{
			error ("Esperava um comando");
			return (1);
		}

		execvp
		(	(const char *)cmd->c_argv[1],
			(const char **)cmd->c_argv + 1
		);

		error ("*Não foi possível executar \"%s\"", cmd->c_argv[1]);
		return (1);

	    case F_DOT:
		if (cmd->c_argc != 2)
		{
			error ("Esperava, como argumento, apenas o nome de um arquivo de comandos");
			return (-1);
		}

		return (exec_command_file (cmd->c_argv[1], 1 /* must exist */, 1));

		/*
		 *	Comando "cd".
		 */
	    case F_CD:
		return (do_cd (cmd));

		/*
		 *	Comando "shid".
		 */
	    case F_SHID:
		fprintf (stderr, "%s\n", pgversion);
		break;

		/*
		 *	Comandos "login" e "logout".
		 */
	    case F_LOGIN:
	    case F_LOGOUT:
		exit (1);

		/*
		 *	Comandos ":", "true" e "return".
		 */
	    case F_COLON:
	    case F_TRUE:
	    case F_RETURN:
		return (0);

		/*
		 *	Comando "exit".
		 */
	    case F_EXIT:
		if (cmd->c_argc > 2)
			{ error ("Esperava no máximo um argumento numérico"); return (1); }

		if (cmd->c_argv[1] != NOSTR)
			ret = atol (cmd->c_argv[1]);
		else
			ret = 0;

		if (!script)
			exit (ret);

		sonsignaled++;
		break;

		/*
		 *	Comando "false".
		 */
	    case F_FALSE:
		return (1);

		/*
		 *	Comando "umask".
		 */
	    case F_UMASK:
		return (do_umask (cmd));

		/*
		 *	Comando "export".
		 */
	    case F_EXPORT:
		return (do_set_flags (cmd, V_EXPORT));

		/*
		 *	Comando "read".
		 */
	    case F_READ:
		return (do_read (cmd));

		/*
		 *	Comando "readonly".
		 */
	    case F_READONLY:
		return (do_set_flags (cmd, V_READONLY));

		/*
		 *	Comando "rehash".
		 */
	    case F_REHASH:
		exec_hash_create (1 /* verbose */);
		break;

		/*
		 *	Comando "unset".
		 */
	    case F_UNSET:
		return (do_unset (cmd));

		/*
		 *	Comando "set".
		 */
	    case F_SET:
		return (do_set (cmd));

		/*
		 *	Comando "shift".
		 */
	    case F_SHIFT:
		return (do_shift (cmd));

	    case F_SPY:
		if
		(	isatty (fdin) &&
			(streq (tty = ttyname (fdin), "/dev/console") ||
			strncmp (tty, "/dev/video", 10) == 0)
		)
			return (spy ());

		error ("Não é possível executar \"spy\" nesta tela");
		return (1);

	    case F_TIMES:
		mutimes (&t);
		wrtimes (t.mu_cutime, ' ');
		wrtimes (t.mu_cstime, '\n');
		break;

	    case F_TRAP:
		return (do_trap (cmd));

	    case F_WAIT:
		if (cmd->c_argc > 2)
			{ error ("Esperava, no máximo, um argumento numérico"); return (1); }

		if (cmd->c_argv[1] == NOSTR)
			ret = 0;
		else
			ret = atol (cmd->c_argv[1]);

		return (await (ret, NOSTR));

	    default:
		break;
	}

	return (0);

}	/* end exec_func */

/*
 ****************************************************************
 *	Trata o comando "cd"					*
 ****************************************************************
 */
int
do_cd (CMD *cmd)
{
	const char	*dir;
	char		buf[6];
	int		argc, i;

	if ((dir = cmd->c_argv[1]) == NOSTR)
	{
		if (home_sp == NOSYMP || (dir = home_sp->s_value) == NOSTR)
		{
			error ("A variável \"HOME\" está indefinida!");
			return (-1);
		}
	}
	elif (cmd->c_argv[2] != NOSTR)
	{
		do			/* Vários diretórios */
		{
			argc = cmd->c_argc - 1;

			fprintf (stderr, "\nForam dados %d arquivos:\n\n", argc);

			for (i = 1; i <= argc; i++)
				fprintf (stderr, "%3d: %s\n", i, cmd->c_argv[i]);

			fprintf (stderr, "\nIndique qual o desejado (1 a %d): ", argc);

			fflush (stderr);

			i = atol (fgets (buf, sizeof (buf) - 1, stdin));

			if (i == 0)
				return (1);
		}
		while (i < 0 || i > argc);

		dir = cmd->c_argv[i];
	}

	/*
	 *	Migra para o diretório especificado.
	 */
	if (chdir (dir) < 0)
		{ error ("*Não consegui migrar para o diretório \"%s\"", dir); return (1); }

	update_cwd ();

	return (0);

}	/* end do_cd */

/*
 ****************************************************************
 *	Obtém/atualiza o diretório corrente			*
 ****************************************************************
 */
void
update_cwd (void)
{
	static char	current_wd_area[CWD_SZ];

	/*
	 *	Obtém o diretório corrente
	 */
	if ((current_wd = getcwd (current_wd_area, CWD_SZ)) == NOSTR)
		error ("*NÃO consegui obter o diretório corrente");

}	/* end update_cwd */

/*
 ****************************************************************
 *	Trata o comando "eh"					*
 ****************************************************************
 */
int
do_edit_hist (CMD *cmd, int fdin, int fdout, int extra)
{
	char		*new_cmd, *edit_cmd, **cp, buf[6];
	int		len, ncmd;
	const char	*cmdp;

	snprintf (buf, sizeof (buf), "%d: ", last_cmd + 1);

	ncmd = last_cmd;

	if (cmd->c_argc == 1)
	{
		/* Apenas um argumento */

		if ((cmdp = get_history (ncmd)) == NOSTR)
			return (-2);

		if ((new_cmd = alloca (len = strlen (cmdp) + 1)) == NOSTR)
			return (-2);

		memmove (new_cmd, cmdp, len);
	}
	else
	{
		/* Vários argumentos: concatena as linhas do histórico */

		len = 0;

		for (cp = &cmd->c_argv[1]; *cp != NOSTR; cp++)
		{
			ncmd = atol (*cp);
			cmdp = get_history (ncmd);

			if (cmdp == NOSTR)
			{
				error ("O comando %d não está presente no histórico\n", ncmd);
				return (-2);
			}

			len += strlen (cmdp) + 3;
		}

		if ((new_cmd = alloca (len)) == NOSTR)
			return (-2);

		cmdp = get_history (atol (cmd->c_argv[1]));
		strcpy (new_cmd, cmdp);

		for (cp = &cmd->c_argv[2]; *cp != NOSTR; cp++)
		{
			ncmd = atol (*cp);
			cmdp = get_history (ncmd);

			strcat (new_cmd, " ; ");
			strcat (new_cmd, cmdp);
		}
	}

	/*
	 *	Deixa o usuário modificar a linha antes de executar.
	 */
	if ((edit_cmd = alloca (len << 1)) == NOSTR)
		return (-2);

	if (edit (buf, new_cmd, edit_cmd) < 0)
		{ error ("Memória esgotada ou erro na edição"); return (-2); }

	if (!ask_yes_no ("Deseja executar o comando"))
		return (-2);

	insert_line (edit_cmd);

	fprintf (stderr, "==> %s\n", edit_cmd);

	put_history (edit_cmd);

	return (exec (get_command (M_NOFLAGS), fdin, fdout, extra));

}	/* end do_edit_hist */

/*
 ****************************************************************
 *	Trata o comando "gh"					*
 ****************************************************************
 */
int
do_graphic_hist (CMD *cmd, int fdin, int fdout, int extra)
{
	char		*new_cmd, *edit_cmd, buf[6];
	int		len, ncmd, sel;
	const char	*cmdp;

	snprintf (buf, sizeof (buf), "%d: ", last_cmd + 1);

	ncmd = last_cmd;

	if (cmd->c_argc != 1)
		{ error ("Não esperava argumentos"); return (-2); }

	if ((cmdp = select_command_from_history (&sel)) == NOSTR || sel == 'q')
		return (-2);

	if ((new_cmd = alloca (len = strlen (cmdp) + 1)) == NOSTR)
		return (-2);

	memmove (new_cmd, cmdp, len);

	if (sel == 'e')
	{
		/* Deixa o usuário modificar a linha antes de executar */

		if ((edit_cmd = alloca (len << 1)) == NOSTR)
			return (-2);

		if (edit (buf, new_cmd, edit_cmd) < 0)
			{ error ("Memória esgotada ou erro na edição"); return (-2); }

		if (!ask_yes_no ("Deseja executar o comando"))
			return (-2);
	}
	else
	{
		edit_cmd = new_cmd;

		if (sel == '\r')
		{
			fprintf (stderr, "%s%s\n", buf, edit_cmd);

			if (!ask_yes_no ("Deseja executar o comando"))
				return (-2);
		}
	}

	insert_line (edit_cmd);

	fprintf (stderr, "==> %s\n", edit_cmd);

	put_history (edit_cmd);

	return (exec (get_command (M_NOFLAGS), fdin, fdout, extra));

}	/* end do_graphic_hist */

/*
 ****************************************************************
 *	Trata o comando "eval"					*
 ****************************************************************
 */
int
do_eval (CMD *cmd, int fdin, int fdout, int extra)
{
	char		**cp, *s, *str;
	int		len;

	for (len = 0, cp = &cmd->c_argv[1]; *cp != NOSTR; cp++)
		len += strlen (*cp) + 1;

	if (len <= 1)
		return (-1);

	if ((str = s = alloca (len + 1)) == NOSTR)
		return (-1);

	for (cp = &cmd->c_argv[1]; *cp != NOSTR; cp++)
	{
		memmove (s, *cp, len = strlen (*cp));
		s += len; *s++ = ' ';
	}

	*--s = NOCHR;

	insert_line (str);

	return (exec (get_command (cmd->c_flags), fdin, fdout, extra));

}	/* end do_eval */

/*
 ****************************************************************
 *	Trata o comando "xh"					*
 ****************************************************************
 */
int
do_exec_hist (CMD *cmd, int fdin, int fdout, int extra)
{
	int		ncmd;
	const char	*cmdp;

	if (cmd->c_argc > 2)
		{ error ("Esperava, no máximo, o número de um comando"); return (-2); }

	if (cmd->c_argc == 1)
		ncmd = last_cmd;
	else
		ncmd = atol (cmd->c_argv[1]);

	if ((cmdp = get_history (ncmd)) == NOSTR)
		{ error ("O comando %d não está presente no histórico", ncmd); return (-2); }

	insert_line (cmdp);

	fprintf (stderr, "==> %s\n", cmdp);

	put_history (linebuf);

	return (exec (get_command (M_NOFLAGS), fdin, fdout, extra));

}	/* end do_exec_hist */

/*
 ****************************************************************
 *	Trata o comando "read"					*
 ****************************************************************
 */
int
do_read (CMD *cmd)
{
	int		c = NOCHR;
	char		buf[512], *cp, **namep;

	if (cmd->c_argc == 1)
		{ error ("Esperava nomes de variáveis"); return (-1); }

	cp    = buf;
	namep = &cmd->c_argv[1];

	while (*namep != NOSTR && (c = getchar ()) != EOF)
	{
		if (c == ' ' || c == '\t' || c == '\n')
		{
			/* Veio um separador: define a próxima variável */

			if (cp > buf)
			{
				*cp = NOCHR;

				if (define_variable (*namep, buf, -1) == NOSYMP)
				{
					error
					(	"Erro na definição da variável \"%s\"",
						*namep
					);
				}

				cp = buf;
				namep++;
			}
		}
		else
		{
			/* Não separador: vai juntando no buffer */

			if (cp < &buf[sizeof (buf) - 1])
				*cp++ = c;
		}
	}

	/*
	 *	Define as variáveis que porventura faltarem.
	 */
	buf[0] = NOCHR;

	while (*namep != NOSTR)
		define_variable (*namep++, buf, -1);

	while (c != '\n' && c != EOF)
		c = getchar ();

	return (0);

}	/* end do_read */

/*
 ****************************************************************
 *	Trata o comando "set"					*
 ****************************************************************
 */
int
do_set (CMD *cmd)
{
	char		**cp, *arg;
	int		minus, argc;

	if ((argc = cmd->c_argc) == 1)
	{
		/* Não foram dados argumentos: apenas mostra as variáveis */

		show_variables (stdout, V_ALL);
		return (0);
	}

	/*
	 *	Analisa, primeiramente, os argumentos precedidos de
	 *	'-' e '+', alterando as opções de funcionamento.
	 */
	for (cp = &cmd->c_argv[1]; (arg = *cp) != NOSTR; cp++, argc--)
	{
		if (*arg == '-')
			{ arg++; minus = 1; }
		elif (*arg == '+')
			{ arg++; minus = 0; }
		else
			break;

		for (/* acima */; *arg != NOCHR; arg++)
		{
			switch (*arg)
			{
			    case 'd':
				dflag = minus;
				break;

			    case 'e':
				quick_exit = minus;
				break;

			    case 'r':
				rewriting = !minus;
				break;

			    case 'v':
				vflag = minus;
				break;

			    case 'V':
				vflag = minus + minus;
				break;

			    default:
				error ("Opção inválida: '%c'", *arg);
				return (1);
			}
		}
	}

	/*
	 *	Os demais argumentos substituem os parâmetros
	 *	posicionais.
	 */
	if (argc > 0)
		define_params (argc - 1, 1, (const char **)cp);

	return (0);

}	/* end do_set */

/*
 ****************************************************************
 *	Trata os comandos "export" e "readonly"			*
 ****************************************************************
 */
int
do_set_flags (CMD *cmd, int flag)
{
	char		**cp;
	int		ret;

	if (cmd->c_argc == 1)
	{
		/* Não foram dados argumentos: apenas mostra as variáveis */

		show_variables (stdout, flag);
		return (0);
	}

	for (ret = 0, cp = &cmd->c_argv[1]; *cp != NOSTR; cp++)
	{
		if (set_variable_flag (*cp, flag) < 0)
			ret++;
	}

	return (ret);

}	/* end do_set_flags */

/*
 ****************************************************************
 *	Trata o comando "shift"					*
 ****************************************************************
 */
int
do_shift (CMD *cmd)
{
	int		nshift, ind;

	if (cmd->c_argc != 2)
		{ error ("Esperava apenas um argumento numérico"); return (1); }

	nshift = atol (cmd->c_argv[1]);

	if (nshift < 1 || nshift >= npospar)
		{ error ("Argumento inválido: %d", nshift); return (1); }

	for (ind = 1, nshift++; nshift < npospar; ind++, nshift++)
		pospar[ind] = pospar[nshift];

	npospar = ind;

	return (0);

}	 /* end do_shift */

/*
 ****************************************************************
 *	Trata o comando "trap"					*
 ****************************************************************
 */
int
do_trap (CMD *cmd)
{
	int		signo;
	char		c, **cp;
	const char	*tmp, *trap_cmd;

	/*
	 *	Se NÃO foram dados argumentos, apenas mostra os
	 *	comandos a serem executados na recepção de cada sinal.
	 */
	if (cmd->c_argc == 1)
	{
		for (signo = 0; signo < NSIG; signo++)
		{
			if (trap[signo] != NOSTR)
			{
				fprintf
				(	stderr, "%2d\t%.9s\t%s\n",
					signo, sigtostr (signo), trap[signo]
				);
			}
		}

		return (0);
	}

	/*
	 *	Verifica se foi dado o comando.
	 */
	cp = &cmd->c_argv[1];

	if ((c = **cp) < '0' || c > '9')
		trap_cmd = *cp++;
	else
		trap_cmd = NOSTR;

	/*
	 *	Obtém os números dos sinais.
	 */
	for (/* cp inicializado acima */; *cp != NOSTR; cp++)
	{
		signo = strtol (*cp, &tmp, 0);

		if
		(	*tmp != NOCHR ||
			signo > NSIG || signo < 0 || signo == SIGSEGV
		)
		{
			error ("Espera o número de um sinal");
			return (-1);
		}

		if (trap[signo] != NOSTR)
		{
			/* O sinal já estava programado */

			free ((char *)trap[signo]);
		}
		else
		{
			/* O sinal nunca havia sido programado: obtém seu estado */

			sig[signo] = signal (signo, SIG_IGN);
		}

		if (trap_cmd == NOSTR)
		{
			/* Não foi dado comando: retorna o sinal a seu estado original */

			signal (signo, sig[signo]);
			trap[signo] = NOSTR;
		}
		else
		{
			/* Associa o comando à ocorrência do sinal */

			if ((trap[signo] = strdup (trap_cmd)) == NOSTR)
				continue;

			if (*trap[signo] != NOCHR)
				signal (signo, handle_signals);
			else
				signal (signo, SIG_IGN);
		}
	}

	return (0);

}	/* end do_trap */

/*
 ****************************************************************
 *	Trata o comando "umask"					*
 ****************************************************************
 */
int
do_umask (CMD *cmd)
{
	int		mask;
	const char	*tmp;

	if (cmd->c_argv[1] == NOSTR)
	{
		umask (mask = umask (0));
		fprintf (stderr, "%o\n", mask);
	}
	else
	{
		mask = strtol (cmd->c_argv[1], &tmp, 8);

		if (*tmp != NOCHR)
			{ error ("Esperava um número octal como argumento"); return (1); }

		if (mask > 0777 || mask < 0)
			{ error ("Máscara \"%o\" inválida", mask); return (1); }

		umask (mask);
	}

	return (0);

}	/* end do_umask */

/*
 ****************************************************************
 *	Trata o comando "unset"					*
 ****************************************************************
 */
int
do_unset (CMD *cmd)
{
	char		**cp;
	int		ret;

	if (cmd->c_argc == 1)
		{ error ("Esperava nomes de variáveis"); return (1); }

	for (ret = 0, cp = &cmd->c_argv[1]; *cp != NOSTR; cp++)
	{
		if (delete_variable (*cp) < 0)
			ret++;
	}

	return (ret);

}	/* end do_unset */

/*
 ****************************************************************
 *	Imprime o tempo						*
 ****************************************************************
 */
void
wrtimes (time_t t, char c)
{
	int		hr, min, sec;

	min = t / 60; 	sec = t % 60;
	hr  = min / 60;	min = min % 60;

	fprintf (stderr, "%2d:%02d:%02d%c", hr, min, sec, c);

}	/* end wrtimes */

/*
 ****************************************************************
 *	Faz uma pergunta ao usuário				*
 ****************************************************************
 */
int
ask_yes_no (const char *question)
{
	char		buf[8];
	int		tty_fd;

	if ((tty_fd = open ("/dev/tty", O_RDWR)) < 0)
		{ error ("Não consegui abrir \"/dev/tty\""); return (0); }

	write (tty_fd, question, strlen (question));
	write (tty_fd, "? (n) ", 6);

	read (tty_fd, buf, sizeof (buf));

	return (close (tty_fd), buf[0] == 'S' || buf[0] == 's');

}	/* end ask_yes_no */
