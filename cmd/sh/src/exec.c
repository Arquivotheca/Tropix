/*
 ****************************************************************
 *								*
 *			exec.c					*
 *								*
 *	Execução de Comandos					*
 *								*
 *	Versão	3.2.3, de 06.01.00				*
 *		4.5.0, de 12.06.03				*
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
#include	<dirent.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<signal.h>
#include	<errno.h>

#include	"../h/common.h"
#include	"../h/sym.h"
#include	"../h/scan.h"
#include	"../h/command.h"

#include	"../h/extern.h"
#include	"../h/proto.h"

/*
 ****************************************************************
 *	Constantes e Macros					*
 ****************************************************************
 */
#define	BREAK		(-10000)
#define	CONTINUE	(-20000)

#define	EXEC_FILE	0x01080486		/* (FMAGIC << 16) | 0x486 */

/*
 ****************************************************************
 *	A Tabela HASH de Executáveis				*
 ****************************************************************
 */
#define	EXEC_HASH_SZ	677

typedef struct
{
	char		*eh_name;		/* Nome curto */
	int		eh_len;			/* strlen (eh_name) */
	char		*eh_file_path;		/* Caminho completo */

}	EXEC_HASH;

entry	EXEC_HASH	*exec_hash_tb;		/* Início da tabela HASH */
entry	int		exec_hash_col;		/* Número de colisões */

/*
 ****************************************************************
 *	Variáveis Exportadas					*
 ****************************************************************
 */
entry	int	sonsignaled;	/* Filho esperado recebeu um sinal */

entry	int	firstpid,	/* pid do primeiro cmd de um pipe */
		lastpid;	/* PID do último comando executado */

entry	int	last_bg_pid,	/* PID do último comando em & */
		last_cmd_code;	/* Status do último comando */

entry	int	fdchanged = 0;	/* Houve redirecionamento */

/*
 ****************************************************************
 *	Protótipos de Funções					*
 ****************************************************************
 */
int		do_exec (CMD *, int, int, int);
int		exec_internal_func (CMD *, int, int, int);
int		exec_simple_command (CMD *, int, int, int);
int		exec_redir (CMD *, int, int);
char		**expand_file_name (RDRC *);

void		exec_hash_create (int);
int		exec_hash_dir (const char *, int);
const char	*exec_hash_search (const char *, int, const char *);

void		printmu (char *, MUTM *, MUTM *);

/*
 ****************************************************************
 *	Ponto de Entrada para Execução de um Comando		*
 ****************************************************************
 */
int
exec (CMD *cmd, int fdin, int fdout, int extra)
{
	int		ret;

	fdchanged = 0;

	ret = do_exec (cmd, fdin, fdout, extra);

	if (fdchanged)
		restore_fds ();

	return (ret);

}	/* end exec */

/*
 ****************************************************************
 *	Executa um comando					*
 ****************************************************************
 */
int
do_exec (CMD *cmd, int fdin, int fdout, int extra)
{
	char		**cp, **argv, *str[2];
	int		subshell;
	int		first, pid, ret, tube[2], timed;
	MUTMS		tbefore, tafter;
	MUTM		hbefore, hafter;

	if (cmd == NOCMD || cmd == ENDCMD)
		return (0);

	if (cmd == ERRCMD)
		return (1);

	ret = subshell = timed = 0;

	/*
	 *	Verifica se o comando deve ser executado em uma subshell.
	 */
	if (cmd->c_flags & M_SUBSHELL)
	{
		if ((pid = fork ()) < 0)
		{
			error ("*Não consegui criar um processo");
			return (1);
		}

		if (pid > 0)
		{
			/* Shell MÃE: pode esperar ou não pela subshell */

			if (cmd->c_flags & M_BACK)
				return (0);

			return (await (pid, "subshell"));
		}

		subshell++;	/* A subshell começa aqui */
	}

	if (cmd->c_flags & M_TIME)
		{ timed++; mutime  (&hbefore); mutimes (&tbefore); }

	/*
	 *	Se for um nó intermediário da árvore, verifica se há
	 *	redirecionamentos a serem feitos.
	 */
	if (cmd->c_type > T_SIMPLE && cmd->c_rdrc != NORDRC)
	{
		if (exec_redir (cmd, fdin, fdout) < 0)
			{ ret = 2; goto out; }
	}

	/*
	 *	Agora, analisa cada nó da árvore de comandos.
	 */
	switch (cmd->c_type)
	{
	/*
	 ******	Declaração de variáveis *************************
	 */
	    case T_ASSIGN:
		/*
		 *	Expande os argumentos.
		 */
		if ((argv = expand_args (cmd->c_assign , NULL)) == NOSSTR)
			{ ret = 1; break; }

		/*
		 *	Realiza as atribuições.
		 */
		for (cp = argv; *cp != NOSTR; cp++)
		{
			if (define_variable_from_string (*cp) == NOSYMP)
				{ error ("Atribuição impossível: \"%s\"", *cp); ret = 1; }
		}

		free_args (argv);
		break;

	/*
	 ****** Comando simples *********************************
	 */
	    case T_SIMPLE:
		if (cmd->c_fid == F_NOFUNC)
			ret = exec_simple_command (cmd, fdin, fdout, extra);
		else
			ret = exec_internal_func (cmd, fdin, fdout, extra);

		break;

	/*
	 ****** Lista de comandos *******************************
	 */
	    case T_LIST:
		if (cmd->c_leftcmd)
			ret = do_exec (cmd->c_leftcmd, fdin, fdout, -1);

		if (ret == BREAK || ret == CONTINUE)
			goto out;

		if (cmd->c_rightcmd)
			ret = do_exec (cmd->c_rightcmd, fdin, fdout, -1);

		break;

	/*
	 ****** PIPE ********************************************
	 */
	    case T_PIPE:
		if (pipe (tube) < 0)
			{ error ("*Erro na criação do \"pipe\""); ret = 3; break; }

		ret = do_exec (cmd->c_leftcmd,  fdin,    tube[1], tube[0]);
		ret = do_exec (cmd->c_rightcmd, tube[0], fdout,   tube[1]);

		close (tube[0]); close (tube[1]);

		if ((cmd->c_rightcmd->c_flags & (M_BACK | M_PIPEOUT)) == 0)
			ret = await (lastpid, cmd->c_rightcmd->c_argv[0]);

		break;

	/*
	 ****** OR de comandos **********************************
	 */
	    case T_OR:
		if (cmd->c_leftcmd)
			ret = do_exec (cmd->c_leftcmd, fdin, fdout, -1);

		if (ret == BREAK || ret == CONTINUE)
			goto out;

		if (ret != 0 && cmd->c_rightcmd)
			ret = do_exec (cmd->c_rightcmd, fdin, fdout, -1);

		break;

	/*
	 ****** AND de comandos *********************************
	 */
	    case T_AND:
		if (cmd->c_leftcmd)
			ret = do_exec (cmd->c_leftcmd, fdin, fdout, -1);

		if (ret == BREAK || ret == CONTINUE)
			goto out;

		if (ret == 0 && cmd->c_rightcmd)
			ret = do_exec (cmd->c_rightcmd, fdin, fdout, -1);

		break;

	/*
	 ****** Comando BREAK ***********************************
	 */
	    case T_BREAK:
		ret = BREAK;
		break;

	/*
	 ****** Comando CASE ************************************
	 */
	    case T_CASE:
		str[0] = cmd->c_argv[0]; str[1] = NOSTR;

		if ((argv = expand_args (str, NULL)) == NOSSTR)
			{ ret = 1; break; }

		first = 1;

		do
		{
			for (cp = &cmd->c_argv[first]; *cp != NOSTR; cp++)
			{
				if (patmatch (argv[0], *cp))
				{
					ret = do_exec (cmd->c_leftcmd, fdin, fdout, extra);
					goto end_case;
				}
			}

			first = 0;
		}
		while ((cmd = cmd->c_rightcmd) != NOCMD);

	    end_case:
		free_args (argv);
		break;

	/*
	 ****** Comando CONTINUE ********************************
	 */
	    case T_CONTINUE:
		ret = CONTINUE;
		break;

	/*
	 ****** Comando FOR *************************************
	 */
	    case T_FOR:
		/*
		 *	Expande os argumentos.
		 */
		if ((argv = expand_args (&cmd->c_argv[1], NULL)) == NOSSTR)
			{ error ("erro na execução do comando \"for\""); ret = 2; break; }

		/*
		 *	Executa a malha do "for".
		 */
		for (cp = argv; *cp != NOSTR; cp++)
		{
			/* Faz a atribuição à variável de controle */

			if (define_variable (cmd->c_argv[0], *cp, -1) == NOSYMP)
				{ error ("erro na execução do comando \"for\""); ret = 2; break; }

			/* Executa o corpo do "for" */

			ret = do_exec (cmd->c_leftcmd, fdin, fdout, -1);

			if (sonsignaled || ret == BREAK)
				{ ret = 0; break; }
		}

		free_args (argv);
		break;

	/*
	 ****** Comando IF **************************************
	 */
	    case T_IF:
		do
		{
			/* Realiza o teste */

			ret = do_exec (cmd->c_leftcmd->c_leftcmd, fdin, fdout, -1);

			if (ret == 0)
			{
				/* É verdadeiro: executa o comando */

				ret = do_exec (cmd->c_leftcmd->c_rightcmd, fdin, fdout, -1);
				goto out;
			}

			/* Processa o "elif" (se existir) */

			cmd = cmd->c_rightcmd;
		}
		while (cmd != NOCMD && cmd->c_type == T_IF);

		/* Se houver "else", executa o comando correspondente */

		ret = (cmd == NOCMD) ? 1 : do_exec (cmd, fdin, fdout, -1);

		break;

	/*
	 ****** Comando UNTIL  ***********************************
	 */
	    case T_UNTIL:
		while ((ret = do_exec (cmd->c_leftcmd, fdin, fdout, -1)) != 0)
		{
			/* O teste é falso: executa o corpo */

			ret = do_exec (cmd->c_rightcmd, fdin, fdout, -1);

			if (sonsignaled || ret == BREAK)
				{ ret = 0; break; }
		}

		break;

	/*
	 ****** Comando WHILE ***********************************
	 */
	    case T_WHILE:
		while ((ret = do_exec (cmd->c_leftcmd, fdin, fdout, -1)) == 0)
		{
			/* O teste é verdadeiro: executa o corpo */

			ret = do_exec (cmd->c_rightcmd, fdin, fdout, -1);

			if (sonsignaled || ret == BREAK)
				{ ret = 0; break; }
		}

		break;
	}

    out:
	if (timed)
	{
		mutime  (&hafter); mutimes (&tafter);

		printmu ("REAL", &hbefore, &hafter);
		printmu ("USER", (MUTM *)&tbefore.mu_cutime, (MUTM *)&tafter.mu_cutime);
		printmu ("SYS ", (MUTM *)&tbefore.mu_cstime, (MUTM *)&tafter.mu_cstime);

		putc ('\n', stderr); fflush (stderr);
	}

	/*
	 *	Se era uma "subshell", a tarefa já foi cumprida.
	 */
	if (subshell)
		exit (ret);

	return (ret);		/* Só deve haver este "return" na função */

}	/* end do_exec */

/*
 ****************************************************************
 *	Executa uma função interna				*
 ****************************************************************
 */
int
exec_internal_func (CMD *cmd, int fdin, int fdout, int extra)
{
	char		**argv, **save_argv;
	int		ret, argc, save_argc, subshell = 0;

	/*
	 *	Expande os argumentos.
	 */
	if ((argv = expand_args (cmd->c_argv, &argc)) == NOSSTR)
		return (1);

	/*
	 *	Funções internas conectadas por PIPE devem ser
	 *	executadas em uma subshell.
	 */
	if (cmd->c_flags & (M_PIPEIN | M_PIPEOUT))
	{
		int	pid;

		if ((pid = fork ()) < 0)
		{
			error ("*Não consegui criar um processo");
			return (1);
		}

		lastpid = pid;

		if (pid > 0)		/* A shell MÃE retorna */
			return (0);

		close (extra);

		if (cmd->c_flags & M_PIPEIN)
		{
			dup2 (fdin, 0);
			close (fdin);
		}

		if (cmd->c_flags & M_PIPEOUT)
		{
			dup2 (fdout, 1);
			close (fdout);
		}

		subshell++;
	}

	/*
	 *	Processa os redirecionamentos.
	 */
	if (exec_redir (cmd, fdin, fdout) < 0)
	{
		if (subshell)
			exit (2);

		return (2);
	}

	/*
	 *	Substitui os argumentos pelos expandidos.
	 */
	save_argc = cmd->c_argc;
	save_argv = cmd->c_argv;

	cmd->c_argc = argc;
	cmd->c_argv = argv;

	/*
	 *	Executa a função
	 */
	ret = exec_func (cmd, fdin, fdout, extra);

	/*
	 *	Recupera os argumentos
	 */
	cmd->c_argc = save_argc;
	cmd->c_argv = save_argv;

	if (subshell)
		exit (ret);

	/*
	 *	Libera os argumentos expandidos.
	 */
	free_args (argv);

	return (ret);

}	/* end exec_internal_func */

/*
 ****************************************************************
 *	Executa um comando simples				*
 ****************************************************************
 */
int
exec_simple_command (CMD *cmd, int fdin, int fdout, int extra)
{
	const char	*file_path;
	char		**argv, **envp;
	int		pid, ret = 0;

	/*
	 *	Expande os argumentos.
	 */
	if ((argv = expand_args (cmd->c_argv, (int *)NULL)) == NOSSTR)
		return (1);

	/*
	 *	Cria um filho para executar o comando (thread ou subprocesso).
	 */
	if (cmd->c_assign == NOSSTR && cmd->c_flags == M_NOFLAGS && cmd->c_rdrc == NORDRC)
		pid = thread ();
	else
		pid = fork ();

	if (pid < 0)
	{
		error ("*Não consegui criar um processo para executar \"%s\"", argv[0]);
		free_args (argv);
		return (1);
	}

	if (pid > 0)
	{
		/*
		 *	A SHELL executa este trecho.
		 */
		lastpid = pid;

		if (cmd->c_flags & M_BACK)
		{
			fprintf (stderr, "%d\n", pid);
			firstpid = 0; last_bg_pid = pid;
		}
		elif ((cmd->c_flags & (M_PIPEIN | M_PIPEOUT)) == 0)
		{
			ret = await (pid, argv[0]);
		}
		elif ((cmd->c_flags & M_PIPEIN) == 0)
		{
			firstpid = pid;
		}
	}
	else
	{
		/*
		 *	O FILHO executa este trecho.
		 */
		if (cmd->c_flags & (M_PIPEOUT | M_PIPEIN))
		{
			/* Trata o PIPE */

			close (extra);

			if (cmd->c_flags & M_PIPEIN)
				{ dup2 (fdin,  0); close (fdin); }

			if (cmd->c_flags & M_PIPEOUT)
				{ dup2 (fdout, 1); close (fdout); }
		}

		/*
		 *	Trata os redirecionamentos.
		 */
		if (exec_redir (cmd, fdin, fdout) < 0)
			exit (1);

		/*
		 *	Se a execução for em "background", ignora
		 *	os sinais SIGINT e SIGQUIT.
		 */
		if (cmd->c_flags & M_BACK)
		{
			signal (SIGINT,  SIG_IGN);
			signal (SIGQUIT, SIG_IGN);
		}

		/*
		 *	Coloca as variáveis no ambiente.
		 */
		envp = expand_args (cmd->c_assign, NULL);

		if (envp != NOSSTR)
		{
			while (*envp != NOSTR)
				putenv (*envp++);
		}

		/*
		 *	Procura na tabela HASH de executáveis.
		 */
		file_path = exec_hash_search (argv[0], strlen (argv[0]), NOSTR);

		if (file_path != NOSTR)
		{
			if (vflag == 2)
				fprintf (stderr, "exec_hash_search: \"%s\" => \"%s\"\n", argv[0], file_path);

			execv (file_path, (const char **)argv);
		}

		execvp ((const char *)argv[0], (const char **)argv);

		error ("*Não foi possível executar \"%s\"", argv[0]);

		_exit (1);	/* Não podemos usar "exit" */
	}

	/*
	 *	Libera os argumentos expandidos.
	 */
	free_args (argv);

	return (ret);

}	/* end exec_simple_command */

/*
 ****************************************************************
 *	Espera um filho terminar a execução			*
 ****************************************************************
 */
int
await (int pid, const char *name)
{
	int	code, status, retpid, received = 0;

	if (!script)
		sonsignaled = 0;

	do
	{
		/*
		 *	Evita o "longjmp" para o programa principal.
		 */
		if (!script)
			ignore_signals++;

		/*
		 *	Espera por um filho.
		 */
		do
		{
			retpid = wait (&status);
		}
		while (retpid < 0 && errno == EINTR);

		ignore_signals = 0;

		if (retpid == firstpid)
			firstpid = 0;

		/*
		 *	É o filho esperado!!!
		 */
		if (retpid == pid)
			received++;

		if (retpid < 0)
			goto nochild;

		/*
		 *	Verifica se a terminação foi normal
		 */
		if ((code = status & 0x7F) != 0)
		{
			if (retpid == pid)
			{
				sonsignaled = 1;

				fprintf
				(	stderr, "%s: %s (\"%s\")",
					pgname,	sigtostr (code), name
				);
			}
			else
			{
				fprintf
				(	stderr, "%s: %s (processo %d)",
					pgname, sigtostr (code), retpid
				);
			}

			fprintf (stderr, ", pc = %P", _fpc);

			if   (code == SIGBUS || code == SIGADR || code == SIGSEGV)
				fprintf (stderr, ", fa = %P", _faddr);
			elif (code == SIGILL)
				fprintf (stderr, ", inst = %04X", (int)_faddr & 0xFFFF);

			putc ('\n', stderr);
#undef CORE
#ifdef CORE
			if (status & 0x80)
			{
				if (retpid == pid)
				{
					fprintf
					(	stderr,
						"%s: \"%s\": core dumped\n",
						pgname,
						name
					);
				}
				else
				{
					fprintf
					(	stderr,
						"%s: Proc %d: core dumped\n",
						pgname,
						retpid
					);
				}
			}
#endif
			if (quick_exit)
			{
				if ((status >> 8) & 0xFF)
					exit ((status >> 8) & 0xFF);
			}
		}
	}
	while (!received || firstpid != 0);

    nochild:
	last_cmd_code = (status >> 8) & 0xFF;

	return (last_cmd_code);

}	/* end await */

/*
 ****************************************************************
 *	Trata o PIPE e os redirecionamentos			*
 ****************************************************************
 */
int
exec_redir (CMD *cmd, int fdin, int fdout)
{
	int		fd, stdinused = 0;
	RDRC		*rp, *beginp;
	STAT		statb;
	char		**names = NOSSTR;

	if (cmd->c_flags & M_PIPEIN)
		stdinused++;

	/*
	 *	Redirecionamento
	 */
	if ((rp = cmd->c_rdrc) != NORDRC)
	{
		rp = beginp = rp->r_next;

		do
		{
			if (rp->r_fd == 0)
				stdinused++;

			if (rp->r_type >= R_IN)
			{
				/*
				 *	Pode ser necessária a expansão em
				 *	redirecionamentos. Ex: cat < *
				 */
				if ((names = expand_file_name (rp)) == NOSSTR)
				{
					hasmeta (rp->r.r_name);
					error ("Erro em redirecionamento");
					return (-1);
				}
			}

			switch (rp->r_type)
			{
			    case R_ERROR:
				break;

			    case R_DEL:		/* [digito]{<>}&- */
				close (rp->r_fd);
				break;

			    case R_TO:		/* [digito]>&digito */
			    case R_FROM:	/* [digito]<&digito */
				dup2 (rp->r.r_dupfd, rp->r_fd);
				fdchanged++;
				break;

			    case R_IN:		/* [digito]< nome */
				if ((fd = open (rp->r.r_name, O_RDONLY)) < 0)
				{
					error ("*Não consegui abrir %s", rp->r.r_name);
					return (-1);
				}

				dup2 (fd, rp->r_fd); close (fd);
				fdchanged++;
				break;

			    case R_OUT:		/* [digito]> nome} */
				if
				(	rewriting ||
					stat (rp->r.r_name, &statb) < 0 ||
					statb.st_mode & (S_IFBLK | S_IFCHR)
				)
					goto create;

				if (!interact || (cmd->c_flags & M_BACK))
				{
					error
					(	"O arquivo \"%s\" já existe: "
						"\"%s\" não será executado",
						rp->r.r_name, cmd->c_argv[0]
					);

					return (-1);
				}
		
				fprintf	(stderr, "O arquivo \"%s\" já existe, ", rp->r.r_name);
				fflush (stderr);

				if (!ask_yes_no ("reescreve"))
					return (-1);

			    create:
				if ((fd = creat (rp->r.r_name, 0666)) < 0)
				{
					error ("*Não consegui abrir \"%s\"", rp->r.r_name);
					return (-1);
				}

				dup2 (fd, rp->r_fd); close (fd);
				fdchanged++;
				break;

			    case R_APP:		/* [digito]>> nome} */
				if ((fd = open (rp->r.r_name, O_WRONLY)) < 0)
					goto create;

				dup2 (fd, rp->r_fd); close (fd);
				lseek (rp->r_fd, 0, L_END);
				fdchanged++;
				break;

			    case R_SIN:		/* [digito]<< nome} */
				if ((fd = open (rp->r.r_name, O_RDWR)) < 0)
				{
					error ("*Não consegui abrir %s", rp->r.r_name);
					return (-1);
				}

				dup2 (fd, rp->r_fd); close (fd);
				unlink (rp->r.r_name);
				fdchanged++;
				break;
			}

			if (rp->r_type >= R_IN)
				free_args (names);
		}
		while ((rp = rp->r_next) != beginp);

	}	/* end if (há redirecionamentos) */

	/*
	 *	A entrada "default" para processos em "background" é o "/dev/null".
	 */
	if ((cmd->c_flags & M_BACK) && !stdinused)
	{
		if ((fd = open ("/dev/null", O_RDONLY)) < 0)
			{ error ("*Não consegui abrir \"/dev/null\""); return (-1); }

		dup2 (fd, 0); close (fd); fdchanged++;
	}

	return (0);

}	/* end exec_redir */

/*
 ****************************************************************
 *	Expande os nomes dos arquivos no redirecionamento	*
 ****************************************************************
 */
char **
expand_file_name (RDRC *rp)
{
	char		*exp[2], **names;

	exp[0] = rp->r.r_name;
	exp[1] = NOSTR;

	names = expand_args (exp, NULL);

	if (names == NOSSTR || names[0] == NOSTR)
		{ free_args (names); return (NOSSTR); }

	if (names[1] != NOSTR)
	{
		error ("Mais de um nome de arquivo em redirecionamento");
		free_args (names); return (NOSSTR);
	}

	rp->r.r_name = names[0];

	return (names);

}	/* end expand_file_name */

/*
 ****************************************************************
 *	Salva os descritores do "stdin", "stdout" e "stderr"	*
 ****************************************************************
 */
void
save_fds (void)
{
	dup2 (0, max_fd + 0);
	fcntl (max_fd + 0, F_SETFD, 1);		/* Liga o EXCLOSE */

	dup2 (1, max_fd + 1);
	fcntl (max_fd + 1, F_SETFD, 1);		/* Liga o EXCLOSE */

	dup2 (2, max_fd + 2);
	fcntl (max_fd + 2, F_SETFD, 1);		/* Liga o EXCLOSE */

}	/* end save_fds */

/*
 ****************************************************************
 *	Restaura os descritores de arquivos da shell		*
 ****************************************************************
 */
void
restore_fds (void)
{
	int		fd;

	fflush (stdout);
	fflush (stderr);

	for (fd = 3; fd < max_fd; fd++)
		close (fd);

	dup2 (max_fd + 0, 0);
	dup2 (max_fd + 1, 1);
	dup2 (max_fd + 2, 2);

}	/* end restore_fds */

/*
 ****************************************************************
 *	Executa um arquivo de comandos				*
 ****************************************************************
 */
int
exec_command_file (const char *name, int must_exist, int dot_script)
{
	FILE		*fp;
	CMD		*cmd;
	int		fd, nfd, ret = 0;
	ulong		header;
	STAT		st;

	if (name != NOSTR)
	{
		/* Arquivo de comandos para executar */

		if ((fd = open (name, O_RDONLY)) < 0)
		{
			if (must_exist)
				error ("*Erro na abertura de \"%s\"", name);

			return (-1);
		}

		/* Verifica se é um executável válido */

		if (fstat (fd, &st) < 0 || !S_ISREG (st.st_mode))
			{ error ("\"%s\" não é um arquivo regular", name); close (fd); return (-1); }

		if (read (fd, &header, sizeof (header)) != sizeof (header))
			{ error ("*Erro de leitura em \"%s\"", name); close (fd); return (-1); }

		if (header == EXEC_FILE)
			{ error ("O arquivo \"%s\" não contém comandos", name); close (fd); return (-1); }

		lseek (fd, 0, L_SET);

		dup2 (fd, nfd = max_fd + 3); close (fd);

		if ((fp = fdopen (nfd, "r")) == NOFILE)
			{ error ("*Erro na reabertura de \"%s\"", name); close (nfd); return (-1); }

		fcntl (nfd, F_SETFD, 1);	/* close on exec */

		if (vflag == 2)
			error ("Executando o arquivo de comandos \"%s\"", name);

		input_file = fp;
	}

	script += dot_script;

	while ((cmd = get_command (M_NOFLAGS)) != ENDCMD && !sonsignaled)
	{
		if (cmd->c_fid < F_EDIT_HIST || cmd->c_fid > F_SHOW_HIST)
			put_history (linebuf);

		ret = exec (cmd, 0, 0, 0);
	}

	script -= dot_script;

	sonsignaled = 0;

	if (name != NOSTR)
		{ fclose (input_file); input_file = stdin; }

	return (ret);

}	/* end exec_command_file */

/*
 ****************************************************************
 *	Constrói a Tabela HASH de Executáveis			*
 ****************************************************************
 */
void
exec_hash_create (int verbose)
{
	EXEC_HASH	*hp;
	int		len, total_files;
	char		*path_value, *dir_path, *cp;

	/*
	 *	Da primeira vez, a tabela é alocada e zerada.
	 *	Nas demais vezes, as entradas antigas são liberadas.
	 */
	if (exec_hash_tb == (EXEC_HASH *)NULL)
	{
		exec_hash_tb = calloc (EXEC_HASH_SZ, sizeof (EXEC_HASH));

		if (exec_hash_tb == (EXEC_HASH *)NULL)
			{ error ("Não consegui alocar a tabela HASH de executáveis"); return; }
	}
	else
	{
		for (hp = exec_hash_tb; hp < exec_hash_tb + EXEC_HASH_SZ; hp++)
		{
			if (hp->eh_len == 0)
				continue;

			free (hp->eh_name);
			free (hp->eh_file_path);

			hp->eh_len  = 0;
			hp->eh_name = hp->eh_file_path = NOSTR;
		}
	}

	/*
	 *	Obtém o valor da variável PATH
	 */
	if (path_sp == NOSYMP || path_sp->s_type != S_VAR)
		return;

	path_value = alloca (len = strlen (path_sp->s_value) + 1);

	memmove (path_value, path_sp->s_value, len);

	/*
	 *	Processa os diversos diretórios do PATH
	 */
	total_files = exec_hash_col = 0;

	if (verbose)
		fprintf (stderr, "ARQUIVOS COLISÕES   ------ DIR ------\n");

	for (cp = dir_path = path_value; /* abaixo */; cp++)
	{
		if (*cp == '\0')
		{
			total_files += exec_hash_dir (dir_path, verbose);
			break;
		}

		if (*cp == ':')
		{
			*cp = '\0';

			total_files += exec_hash_dir (dir_path, verbose);
			dir_path = cp + 1;
		}
	}

	if (verbose)
		fprintf (stderr, "%6d   %6d     TOTAL\n", total_files, exec_hash_col);

}	/* end exec_hash_create */

/*
 ****************************************************************
 *	Instala os executáveis de um diretório			*
 ****************************************************************
 */
int
exec_hash_dir (const char *dir_path, int verbose)
{
	DIR		*dir_fp;
	const DIRENT	*dp;
	char		*file_path, *end_cp;
	int		len, nfiles, ncolision;
	STAT		st;

	if (streq (dir_path, ".") || stat (dir_path, &st) < 0)
		return (0);

	len = strlen (dir_path);

	file_path = alloca (len + MAXNAMLEN + 2);

	memmove (file_path, dir_path, len);

	end_cp = file_path + len; *end_cp++ = '/';

	/*
	 *	Abre o diretório
	 */
	if ((dir_fp = opendir (dir_path)) == NODIR)
		return (0);

	ncolision = exec_hash_col;	/* salva o valor antigo */

	/*
	 *	Varre o diretório, procurando arquivos executáveis
	 */
	for (nfiles = 0; (dp = readdir (dir_fp)) != NODIRENT; /* abaixo */)
	{
		int	fd;
		ulong	header;

		if (dp->d_name[0] == '.')
			continue;

		strcpy (end_cp, dp->d_name);

		if ((fd = inopen (st.st_dev, dp->d_ino)) < 0)
			continue;

		/* Verifica se é um executável */

		if (read (fd, &header, sizeof (header)) == sizeof (header) && header == EXEC_FILE)
		{
			exec_hash_search (dp->d_name, dp->d_namlen, file_path);
			nfiles++;
		}

		close (fd);
	}

	closedir (dir_fp);

	if (verbose)
		fprintf (stderr, "%6d   %6d     %s\n", nfiles, exec_hash_col - ncolision, dir_path);

	return (nfiles);

}	/* end exec_hash_dir */

/*
 ****************************************************************
 *	Busca/Inserção na Tabela HASH de Executáveis		*
 ****************************************************************
 *
 *	Se "file_path" == NOSTR, trata-se de uma busca apenas.
 */
const char *
exec_hash_search (const char *name, int len, const char *file_path)
{
	EXEC_HASH	*hp;
	int		hash_len;

	if (exec_hash_tb == (EXEC_HASH *)NULL)
		return (NOSTR);

	hp	 = exec_hash_tb + strhash (name, len, EXEC_HASH_SZ);
	hash_len = EXEC_HASH_SZ >> 1;

	for (EVER)
	{
		if (hp->eh_len == 0)
		{
			/* Entrada VAGA */

			if (file_path == NOSTR)		/* Apenas busca */
				return (NOSTR);

			/* Instala o novo nome */

			if ((hp->eh_name = strdup (name)) == NOSTR)
				{ error (NOSTR); return (NOSTR); }

			if ((hp->eh_file_path = strdup (file_path)) == NOSTR)
				{ error (NOSTR); free (hp->eh_name); return (NOSTR); }

			hp->eh_len = len;

			return (hp->eh_file_path);
		}

		if (hp->eh_len == len && streq (hp->eh_name, name))
		{
			/* O nome já pertence à tabela */

			if (file_path == NOSTR)		/* Apenas busca */
				return (hp->eh_file_path);

			if (vflag == 2)
			{
				error
				(	"Executável duplicado: (\"%s\" :: \"%s\")",
					hp->eh_file_path, file_path
				);

				error ("\"%s\" será ignorado", file_path);
			}

			return (NOSTR);
		}

		/* Colisão: próxima tentativa */

		if (hash_len <= 0)
			{ error ("Tabela HASH de executáveis cheia"); return (NOSTR); }

		if ((hp += hash_len) >= &exec_hash_tb[EXEC_HASH_SZ])
			hp -= EXEC_HASH_SZ;

		exec_hash_col++; hash_len -= 2;
	}

}	/* end exec_hash_search */

/*
 ****************************************************************
 *	Converte e imprime o tempo				*
 ****************************************************************
 */
void
printmu (char *s, MUTM *before, MUTM *after)
{
	int	min, seg, ms, us; 

	if (after->mu_utime < before->mu_utime)
	{
		after->mu_time--;
		after->mu_utime += 1000000;
	}

	seg = after->mu_time  - before->mu_time;
	us  = after->mu_utime - before->mu_utime;

	min = seg / 60;
	seg = seg % 60;

	ms  = us / 1000;
	us  = us % 1000;

	fprintf (stderr, "%s %3d:%02d.%03d          ", s, min, seg, ms);

}	/* end printmu */
