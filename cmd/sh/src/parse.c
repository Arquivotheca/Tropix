/*
 ****************************************************************
 *								*
 *			parse.c					*
 *								*
 *	Analisador Sintático					*
 *								*
 *	Versão	3.2.3, de 19.12.99				*
 *		4.5.0, de 21.10.03				*
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
#include	<sys/scb.h>

#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<setjmp.h>

#include	"../h/common.h"
#include	"../h/sym.h"
#include	"../h/scan.h"
#include	"../h/command.h"
#include	"../h/error.h"

#include	"../h/extern.h"
#include	"../h/proto.h"

/*
 ****************************************************************
 *	Variáveis Exportadas					*
 ****************************************************************
 */
entry	char	**argv;			/* Vetor de argumentos */
entry	int	argvsz;			/* Tamanho corrente */
entry	int	argc;			/* Número de argumentos */

entry	int	in_command;		/* Analisando comando */
entry	int	nesting;		/* Nível de aninhamento */

/*
 ****************************************************************
 *	Protótipos de Funções					*
 ****************************************************************
 */
CMD	*command_list (int);
CMD	*command_list1 (int);
CMD	*pipe_command (int);
CMD	*simple_command (int);
CMD	*for_command (int);
CMD	*if_command (int);
CMD	*case_command (int);
CMD	*while_until_command (CMDTYPE, int);
CMD	*command_block (int, int);
CMD	*cmd_alloc (CMDTYPE, int);

int	redir_list (CMD *);
int	get_argument_list (MAJOR);
int	get_pattern_list (void);

#ifdef	DEBUG
void	print_tree (CMD *, CMD *);
void	print_redir (CMD *);
#endif

/*
 ****************************************************************
 *	Obtém um comando					*
 ****************************************************************
 */
CMD *
get_command (int flags)
{
	CMD	*cmd;

	for (EVER)
	{
		current_prompt = ps1_sp;

		sfree ();

		if (get_token () == EOT)
			return (ENDCMD);

		/* Analisa o comando */

		argc = 0;

		if ((cmd = command_list (flags)) == NOCMD)
			continue;

		if (cmd == ERRCMD)
		{
			skip_line ();
			error ("Ignorando comando com erro");
			continue;
		}
		elif (token.t_major != EOL)
		{
			skip_line ();
			sherror (LONGMARK, "Ignorando comando com erro");
			continue;
		}
#ifdef	DEBUG
		if (vflag == 2)
		{
			fprintf (stderr, "ÁRVORE => ");
			print_tree (cmd, NOCMD);
			putc ('\n', stderr);
		}
#endif
		return (cmd);

	}	/* end for (lendo comandos) */

}	/* end get_command */

/*
 ****************************************************************
 *	Analisa sintaticamente uma lista de comandos		*
 ****************************************************************
 */
CMD *
command_list (int flags)
{
	CMD		*cmd, *prox_cmd, *list_cmd;
	MAJOR		major;
	
	/*
	 *	Obtém o primeiro comando da lista.
	 */
	if ((cmd = command_list1 (flags)) == ERRCMD)
		return (ERRCMD);

	/*
	 *	Encadeia os comandos, de acordo com o operador.
	 */
	while ((major = token.t_major) == AND || major == OR)
	{
		/* Obtém o próximo comando */

		get_token ();

		if ((prox_cmd = command_list1 (flags)) == ERRCMD)
			return (ERRCMD);

		if (prox_cmd == NOCMD)
		{
			sherror (SHORTMARK, "Esperava um comando");
			return (ERRCMD);
		}

		if (cmd == NOCMD)
		{
			cmd = prox_cmd;
		}
		else
		{
			/* Aloca o nó da árvore correspondente ao operador: && ou || */

			list_cmd = cmd_alloc (major == AND ? T_AND : T_OR, flags);

			if (list_cmd == NOCMD)
				return (ERRCMD);

			list_cmd->c_leftcmd  = cmd;
			list_cmd->c_rightcmd = prox_cmd;

			cmd->c_parent = prox_cmd->c_parent = list_cmd;

			cmd = list_cmd;
		}
	}

	return (cmd);

}	/* end command_list */

/*
 ****************************************************************
 *	Analisa sintaticamente uma lista de comandos		*
 ****************************************************************
 */
CMD *
command_list1 (int flags)
{
	CMD		*cmd, *prev_cmd, *list_cmd;
	MAJOR		major;

	/*
	 *	Obtém o primeiro comando da lista.
	 */
	if ((cmd = prev_cmd = pipe_command (flags)) == ERRCMD)
		return (ERRCMD);

	/*
	 *	Encadeia os comandos, de acordo com o operador.
	 */
	while ((major = token.t_major) == BG || major == SM)
	{
		if (major == BG)	/* & */
		{
			if (prev_cmd->c_type == T_PIPE)
				prev_cmd->c_rightcmd->c_flags |= M_BACK;
			else
				prev_cmd->c_flags |= M_BACK;
		}

		/* Obtém o próximo comando */

		get_token ();

		if ((prev_cmd = pipe_command (flags)) == ERRCMD)
			return (ERRCMD);

		if (prev_cmd == NOCMD)
			break;

		if (cmd == NOCMD)
		{
			cmd = prev_cmd;
		}
		else
		{
			/* Aloca o nó da árvore */

			if ((list_cmd = cmd_alloc (T_LIST, flags)) == NOCMD)
				return (ERRCMD);

			list_cmd->c_leftcmd  = cmd;
			list_cmd->c_rightcmd = prev_cmd;

			cmd->c_parent = prev_cmd->c_parent = list_cmd;

			cmd = list_cmd;
		}
	}

	return (cmd);

}	/* end command_list1 */

/*
 ****************************************************************
 *	Analisa sintaticamente um PIPE				*
 ****************************************************************
 */
CMD *
pipe_command (int flags)
{
	CMD		*cmd, *prev_cmd, *list_cmd;
	
	/*
	 *	Obtém o primeiro comando da lista.
	 */
	if ((cmd = prev_cmd = simple_command (flags)) == ERRCMD)
		return (ERRCMD);

	/*
	 *	Encadeia os comandos, de acordo com o operador.
	 */
	while (token.t_major == PIPE)
	{
		/* O operador é |: exige o comando anterior */

		if (prev_cmd == NOCMD)
		{
			sherror (SHORTMARK, "Comando anterior esperado");
			return (ERRCMD);
		}

		if (prev_cmd->c_type != T_SIMPLE)
		{
			sherror (SHORTMARK, "O comando anterior deveria ser simples");
			return (ERRCMD);
		}

		/* A saída padrão do comando anterior NÃO pode ter sido redirecionada */

		if (prev_cmd->c_fdvector & 2)
		{
			sherror (SHORTMARK, "Saída padrão já foi redirecionada");
			return (ERRCMD);
		}

		/* A saída padrão é um PIPE */

		prev_cmd->c_fdvector |= 2;
		prev_cmd->c_flags    |= M_PIPEOUT;

		/* Obtém o próximo comando */

		get_token ();

		if ((prev_cmd = simple_command (flags | M_PIPEIN)) == ERRCMD)
			return (ERRCMD);

		if (prev_cmd == NOCMD)
		{
			sherror (SHORTMARK, "Esperava um comando");
			return (ERRCMD);
		}

		if (cmd == NOCMD)
		{
			cmd = prev_cmd;
		}
		else
		{
			/* Aloca o nó da árvore */

			if ((list_cmd = cmd_alloc (T_PIPE, flags)) == NOCMD)
				return (ERRCMD);

			list_cmd->c_leftcmd  = cmd;
			list_cmd->c_rightcmd = prev_cmd;

			cmd->c_parent = prev_cmd->c_parent = list_cmd;

			cmd = list_cmd;
		}
	}

	return (cmd);

}	/* end pipe_command */

/*
 ****************************************************************
 *	Analisa sintaticamente um comando simples		*
 ****************************************************************
 */
CMD *
simple_command (int flags)
{
	CMD		*cmd = NOCMD;
	int		sz, save_nesting;

	switch (token.t_major)
	{
		/*
		 *	( lista )
		 */
	    case LP:
		save_nesting = nesting; nesting = 0;

		get_token ();

		if ((cmd = command_list (flags)) != ERRCMD)
		{
			if (token.t_major != RP)
			{
				sherror (LONGMARK, "Esperava ')'");
				cmd = ERRCMD;
			}
			elif (cmd != NOCMD)
			{
				cmd->c_flags |= M_SUBSHELL;
				get_token ();
			}
		}

		nesting = save_nesting;
		break;

		/*
		 *	{ lista }
		 */
	    case LC:
		save_nesting = nesting; nesting = 0;

		get_token ();

		if ((cmd = command_list (flags)) != ERRCMD)
		{
			if (token.t_major != RC)
			{
				sherror (LONGMARK, "Esperava '}'");
				cmd = ERRCMD;
			}
			elif (cmd != NOCMD)
			{
				get_token ();
			}
		}

		nesting = save_nesting;
		break;

		/*
		 *	Atribuição de variáveis.
		 */
	    case DEFVAR:
		if (get_argument_list (DEFVAR) < 0)
			return (ERRCMD);

		if ((cmd = cmd_alloc (T_ASSIGN, flags)) == NOCMD)
			return (ERRCMD);

		sz = (argc + 1) * sizeof (char *);

		if ((cmd->c_assign = salloc (sz)) == NOSSTR)
			return (ERRCMD);

		memmove (cmd->c_assign, argv, sz);
		argc = 0;

		if (token.t_major != STRING)
			return (cmd);

		/* continua no caso abaixo */

		/*
		 *	Cadeia de caracteres.
		 */
	    case STRING:
		if (get_argument_list (STRING) < 0)
			return (ERRCMD);

		if (cmd == NOCMD)
		{
			if ((cmd = cmd_alloc (T_SIMPLE, flags)) == NOCMD)
				return (ERRCMD);
		}
		else
		{
			cmd->c_type = T_SIMPLE;
		}

		sz = (argc + 1) * sizeof (char *);

		if ((cmd->c_argv = salloc (sz)) == NOSSTR)
			return (ERRCMD);

		memmove (cmd->c_argv, argv, sz);

		cmd->c_argc = argc;
		cmd->c_fid = check_func (argv[0]);

		argc = 0;
		break;

		/*
		 *	Comando de programação.
		 */
	    case PROG:
		in_command++;

		switch (token.t_progid)
		{
			/*
			 *	Comando BREAK.
			 */
		    case P_BREAK:
			if ((cmd = cmd_alloc (T_BREAK, flags)) == NOCMD)
				cmd = ERRCMD;
			else
				get_token ();
			break;

			/*
			 *	Comando CASE.
			 */
		    case P_CASE:
			cmd = case_command (flags);
			break;

			/*
			 *	Comando CONTINUE.
			 */
		    case P_CONTINUE:
			if ((cmd = cmd_alloc (T_CONTINUE, flags)) == NOCMD)
				cmd = ERRCMD;
			else
				get_token ();
			break;

			/*
			 *	Comando FOR.
			 */
		    case P_FOR:
			cmd = for_command (flags);
			break;

			/*
			 *	Comando IF.
			 */
		    case P_IF:
			cmd = if_command (flags);
			break;

			/*
			 *	Comando TIME.
			 */
		    case P_TIME:
			get_token ();	cmd = simple_command (flags);

			if (cmd != NOCMD && cmd != ERRCMD)
				cmd->c_flags |= M_TIME;

			break;

			/*
			 *	Comando UNTIL.
			 */
		    case P_UNTIL:
			cmd = while_until_command (T_UNTIL, flags);
			break;

			/*
			 *	Comando WHILE.
			 */
		    case P_WHILE:
			cmd = while_until_command (T_WHILE, flags);
			break;

		    default:
			if (in_command == 1)
			{
				sherror (LONGMARK, "Palavra reservada inesperada");
				cmd = ERRCMD;
			}

		}	/* end switch (token.t_progid) */

		in_command--;
		break;

	    default:
		break;

	}	/* end switch (token.t_major) */

	/*
	 *	Analisa os redirecionamentos.
	 */
	if (cmd != ERRCMD && token.t_major == REDIREC)
	{
		if (redir_list (cmd) < 0)
			return (ERRCMD);
	}

	return (cmd);

}	/* end simple_command */

/*
 ****************************************************************
 *	Analisa sintaticamente o comando CASE			*
 ****************************************************************
 */
CMD *
case_command (int flags)
{
	int		sz;
	CMD		*cmd, *prev_cmd, *block, case_cmd;

	current_prompt = ps2_sp;

	/*
	 *	Obtém a cadeia do "case", a ser comparada com os padrões.
	 */
	if (get_token () != STRING)
	{
		sherror (LONGMARK, "Esperava uma cadeia");
		return (ERRCMD);
	}

	argc = 0;

	if (argv == NOSSTR)
	{
		argvsz = ALLOC_INC;

		if ((argv = malloc (argvsz * sizeof (char *))) == NOSSTR)
			{ error (NOSTR); return (ERRCMD); }
	}

	argv[argc++] = token.t_sval;

	/*
	 *	Em seguida, deve vir a palavra reservada IN.
	 */
	if (get_token () != PROG || token.t_progid != P_IN)
	{
		sherror (LONGMARK, "Esperava palavra reservada \"in\"");
		return (ERRCMD);
	}

	get_token ();

	/*
	 *	Permite uma eventual quebra de linha.
	 */
	if (token.t_major == EOL || token.t_major == SM)
		get_token ();

	prev_cmd = &case_cmd;

	do
	{	/*
		 *	Obtém e guarda os padrões.
		 */
		if (get_pattern_list () < 0)
			return (ERRCMD);

		if (token.t_major != RP)
		{
			sherror (LONGMARK, "Esperava ')'");
			return (ERRCMD);
		}

		sz = (argc + 1) * sizeof (char *);

		if ((cmd = cmd_alloc (T_CASE, flags)) == NOCMD)
			return (ERRCMD);

		if ((cmd->c_argv = salloc (sz)) == NOSSTR)
			return (ERRCMD);

		memmove (cmd->c_argv, argv, sz);
		cmd->c_argc = argc;

		argc = 0;

		/*
		 *	Analisa o bloco de comandos do corpo do "case".
		 */
		get_token ();

		if ((block = command_block (flags, 1 << P_ENDOPT)) == ERRCMD)
			return (ERRCMD);

		if ((cmd->c_leftcmd = block) != NOCMD)
			block->c_parent = cmd;

		prev_cmd->c_rightcmd = cmd;
		prev_cmd             = cmd;

		/*
		 *	O comando termina com "esac", eventualmente em
		 *	outra linha.
		 */
		get_token ();

		if (token.t_major == EOL || token.t_major == SM)
			get_token ();

	}	while (token.t_major != PROG || token.t_progid != P_ESAC);

	cmd->c_rightcmd = NOCMD;

	return (case_cmd.c_rightcmd);

}	/* end case_command */

/*
 ****************************************************************
 *	Analisa sintaticamente o comando FOR			*
 ****************************************************************
 */
CMD *
for_command (int flags)
{
	int		sz;
	CMD		*cmd, *block;

	current_prompt = ps2_sp;

	/*
	 *	Analisa o nome da variável de controle do FOR.
	 */
	if (get_token () != STRING)
	{
		sherror (LONGMARK, "Esperava nome de variável");
		return (ERRCMD);
	}

	if (token.t_progid != P_NOPROG)
	{
		sherror (LONGMARK, "\"%s\" é palavra reservada", token.t_sval);
		return (ERRCMD);
	}

	argc = 0;

	if (argv == NOSSTR)
	{
		argvsz = ALLOC_INC;

		if ((argv = malloc (argvsz * sizeof (char *))) == NOSSTR)
			{ error (NOSTR); return (ERRCMD); }
	}

	argv[argc++] = token.t_sval;

	/*
	 *	Em seguida, deve vir a palavra reservada IN.
	 */
	if (get_token () != PROG || token.t_progid != P_IN)
	{
		sherror (LONGMARK, "Esperava palavra reservada \"in\"");
		return (ERRCMD);
	}

	/*
	 *	Obtém e guarda os padrões.
	 */
	get_token ();

	if (get_argument_list (STRING) < 0)
		return (ERRCMD);

	if ((cmd = cmd_alloc (T_FOR, flags)) == NOCMD)
		return (ERRCMD);

	sz = (argc + 1) * sizeof (char *);

	if ((cmd->c_argv = salloc (sz)) == NOSSTR)
		return (ERRCMD);

	memmove (cmd->c_argv, argv, sz);
	cmd->c_argc = argc;

	argc = 0;

	/*
	 *	Em seguida, deve vir a palavra reservada DO.
	 */
	if (token.t_major == EOL || token.t_major == SM)
		get_token ();

	if (token.t_major != PROG || token.t_progid != P_DO)
	{
		sherror (LONGMARK, "Esperava \"do\"");
		return (ERRCMD);
	}

	if (get_token () == PROG && token.t_progid == P_DONE)
	{
		get_token (); 		/* FOR sem corpo */
		return (NOCMD);
	}

	/*
	 *	Analisa o bloco de comandos do corpo do "for".
	 */
	if ((block = command_block (flags, 1 << P_DONE)) == ERRCMD)
		return (ERRCMD);

	if ((cmd->c_leftcmd = block) != NOCMD)
		block->c_parent = cmd;

	get_token ();

	return (cmd);

}	/* end for_command */

/*
 ****************************************************************
 *	Analisa sintaticamente o comando IF			*
 ****************************************************************
 */
CMD *
if_command (int flags)
{
	CMD		if_cmd, *prev_cmd, *cmd, *block, *test;

	current_prompt = ps2_sp;

	prev_cmd = &if_cmd;	/* nó cabeça */

	do
	{	/*
		 *	Novo bloco: liga ao anterior.
		 */
		if ((cmd = cmd_alloc (T_IF, flags)) == NOCMD)
			return (ERRCMD);

		prev_cmd->c_rightcmd = cmd;
		cmd->c_parent	     = prev_cmd;
		prev_cmd	     = cmd;

		if ((cmd = cmd_alloc (T_IF, flags)) == NOCMD)
			return (ERRCMD);

		prev_cmd->c_leftcmd  = cmd;
		cmd->c_parent	     = prev_cmd;

		/*
		 *	Analisa o comando de teste do IF ou ELIF.
		 */
		get_token ();

		if ((test = command_list (flags)) == ERRCMD)
			return (ERRCMD);

		if ((cmd->c_leftcmd = test) != NOCMD)
			test->c_parent = cmd;

		/*
		 *	Em seguida, deve vir a palavra reservada THEN.
		 */
		if (token.t_major == EOL || token.t_major == SM)
			get_token ();

		if (token.t_major != PROG || token.t_progid != P_THEN)
		{
			sherror (LONGMARK, "Esperava \"then\"");
			return (ERRCMD);
		}

		/*
		 *	Analisa o bloco de comandos seguintes ao THEN.
		 *	Os terminadores são: ELIF, ELSE ou FI.
		 */
		get_token ();

		block =	command_block
			(	flags,
				(1 << P_ELIF) |	(1 << P_ELSE) |	(1 << P_FI)
			);

		if (block == ERRCMD)
			return (ERRCMD);

		if ((cmd->c_rightcmd = block) != NOCMD)
			block->c_parent = cmd;

	}
	while (token.t_major == PROG && token.t_progid == P_ELIF);

	if (token.t_major == PROG && token.t_progid == P_ELSE)
	{
		get_token ();
		block = command_block (flags, 1 << P_FI);

		if (block == ERRCMD)
			return (ERRCMD);

		if ((prev_cmd->c_rightcmd = block) != NOCMD)
			block->c_parent = prev_cmd;
	}

	if (token.t_major == EOL || token.t_major == SM)
		get_token ();

	if (token.t_major != PROG || token.t_progid != P_FI)
	{
		sherror (LONGMARK, "Esperava \"fi\"");
		return (ERRCMD);
	}

	get_token ();

	return (if_cmd.c_rightcmd);

}	/* end if_command */

/*
 ****************************************************************
 *	Analisa sintaticamente o comando WHILE			*
 ****************************************************************
 */
CMD *
while_until_command (CMDTYPE type, int flags)
{
	CMD		*cmd, *test_cmd, *block;

	current_prompt = ps2_sp;

	/*
	 *	Obtém o comando de teste do WHILE/UNTIL.
	 */
	get_token ();

	if ((test_cmd = command_list (flags)) == ERRCMD)
		return (ERRCMD);

	/*
	 *	Em seguida, deve vir a palavra reservada DO.
	 */
	if (token.t_major == EOL || token.t_major == SM)
		get_token ();

	if (token.t_major != PROG || token.t_progid != P_DO)
	{
		sherror (LONGMARK, "Esperava \"do\"");
		return (ERRCMD);
	}

	if (get_token () == PROG && token.t_progid == P_DONE)
	{	/*
		 *	WHILE/UNTIL sem corpo.
		 */
		get_token ();
		return (NOCMD);
	}

	/*
	 *	Analisa o bloco de comandos do corpo do "while".
	 */
	block = command_block (flags, 1 << P_DONE);

	get_token ();

	if (block == ERRCMD)
		return (ERRCMD);

	/*
	 *	Aloca o nó correspondente ao WHILE/UNTIL.
	 */
	if ((cmd = cmd_alloc (type, flags)) == NOCMD)
		return (ERRCMD);

	cmd->c_leftcmd  = test_cmd;
	cmd->c_rightcmd = block;

	test_cmd->c_parent = block->c_parent = cmd;

	return (cmd);

}	/* end while_until_command */

/*
 ****************************************************************
 *	Analisa um bloco de comandos				*
 ****************************************************************
 */
CMD *
command_block (int flags, int mask)
{
	CMD		*list_cmd, *cmd, *prev_cmd;

	/*
	 *	Analisa os comandos do corpo.
	 */
	nesting++;

	if ((cmd = prev_cmd = command_list (flags)) == ERRCMD)
		{ nesting--; return (ERRCMD); }

	while (token.t_major == EOL)
	{
		/*
		 *	Quando vier a palavra reservada de terminação,
		 *	chegamos o fim do corpo do comando.
		 */
		if (get_token () == PROG && (mask & (1 << token.t_progid)) != 0)
			break;

		/*
		 *	Obtém o próximo comando.
		 */
		if ((prev_cmd = command_list (flags)) == ERRCMD)
			{ cmd = ERRCMD; break; }

		if (prev_cmd == NOCMD)
			continue;

		if (cmd == NOCMD)
		{
			cmd = prev_cmd;
		}
		else
		{
			/* Aloca o nó da árvore */

			if ((list_cmd = cmd_alloc (T_LIST, flags)) == NOCMD)
				{ cmd = ERRCMD; break; }

			if ((list_cmd->c_leftcmd  = cmd) != NOCMD)
				cmd->c_parent = list_cmd;

			list_cmd->c_rightcmd = prev_cmd;
			prev_cmd->c_parent   = list_cmd;

			cmd = list_cmd;
		}
	}

	nesting--;

	return (cmd);

}	/* end command_block */

/*
 ****************************************************************
 *	Aloca um nó da árvore					*
 ****************************************************************
 */
CMD *
cmd_alloc (CMDTYPE type, int flags)
{
	CMD		*cmd;

	if ((cmd = salloc (sizeof (CMD))) == NOCMD)
		return (NOCMD);

	memsetl (cmd, 0, sizeof (CMD) / sizeof (long));

	cmd->c_type	= type;
	cmd->c_flags	= flags;

/***	cmd->c_fdvector = 0;		***/
/***	cmd->c_rdrc	= NORDRC;	***/
/***	cmd->c_argc	= 0;		***/
/***	cmd->c_argv	= NOSSTR;	***/
/***	cmd->c_assign	= NOSSTR;	***/
/***	cmd->c_leftcmd  = NOCMD;	***/
/***	cmd->c_rightcmd = NOCMD;	***/
/***	cmd->c_parent	= NOCMD;	***/

	if (flags & M_PIPEIN)
		cmd->c_fdvector = 1;

	return (cmd);

}	/* end cmd_alloc */

/*
 ****************************************************************
 *	Analisa sintaticamente uma lista de redirecionamentos	*
 ****************************************************************
 */
int
redir_list (CMD *cmd)
{
	RDRC		*rp;
	int		n, fdbit, ret = 0;

	if (cmd == NOCMD)
	{
		sherror (LONGMARK, "Esperava um comando");

		cmd = alloca (sizeof (CMD));
		memsetl (cmd, 0, sizeof (CMD) / sizeof (long));
		ret = -1;
	}

	/*
	 *	Analisa os redirecionamentos.
	 */
	while (token.t_major == REDIREC)
	{
		if (rdrc.r_type == R_ERROR)
			{ get_token (); continue; }

		/*
		 *	Verifica se o "fd" já foi utilizado.
		 */
		fdbit = 1 << rdrc.r_fd;

		if (cmd->c_fdvector & fdbit)
		{
			sherror (LONGMARK, "O descritor \"%d\" já foi redirecionado", rdrc.r_fd);
			get_token (); continue;
		}

		/*
		 *	Acrescenta um nó à lista de redirecionamentos.
		 */
		if ((rp = salloc (sizeof (RDRC))) == NORDRC)
			{ get_token (); continue; }

		cmd->c_fdvector |= fdbit;

		if (cmd->c_rdrc == NORDRC)
		{
			cmd->c_rdrc = rp;
			rp->r_next  = rp;
		}
		else
		{
			rp->r_next	    = cmd->c_rdrc->r_next;
			cmd->c_rdrc->r_next = rp;
			cmd->c_rdrc	    = rp;
		}

		rp->r_type = rdrc.r_type;
		rp->r_fd   = rdrc.r_fd;

		if (rp->r_type < R_IN)
		{
			/* NÃO há arquivos envolvidos */

			rp->r.r_dupfd = rdrc.r.r_dupfd;
			get_token (); continue;
		}

		/*
		 *	Redirecionamento de/para um arquivo
		 */
		if (get_token () != STRING)
		{
			sherror (LONGMARK, "Esperava nome de arquivo");
			ret = -1; continue;
		}

		rp->r.r_name = token.t_sval;	/* Guarda o nome do arquivo */

		/* Se foram dados vários nomes, ignora os demais */

		for (n = 0; get_token () == STRING; n++)
			/* vazio */;

		if (n > 0)
		{
			sherror (LONGMARK, "Mais de um nome de arquivo em redirecionamento");
			ret = -1; continue;
		}

		/*
		 *	Caso especial: [n] << x
		 */
		if (rp->r_type == R_SIN)
		{
			char	*tmp_path;

			if ((tmp_path = mktemp ("/tmp/shXXXXXX")) == NOSTR)
			{
				error ("*Não consegui gerar um nome temporário");
				ret = -1; tmp_path = "/dev/null";
			}

			if (copyupto (rp->r.r_name, tmp_path) < 0)
				{ ret = -1; continue; }

			if ((rp->r.r_name = salloc (strlen (tmp_path) + 1)) != NOSTR)
				strcpy (rp->r.r_name, tmp_path);
		}

	}	/* end while (houver redirecionamentos) */

	return (ret);

}	/* end redir_list */

/*
 ****************************************************************
 *	Analisa uma lista de argumentos				*
 ****************************************************************
 */
int
get_argument_list (MAJOR alt_major)
{
	MAJOR		major;
	char		**new_argv;
	int		new_argvsz;

	do
	{
		if (argc >= argvsz - 1)
		{
			/* É preciso estender a área auxiliar */

			new_argvsz = argvsz + ALLOC_INC;
			new_argv   = realloc (argv, new_argvsz * sizeof (char *));

			if (new_argv == NOSSTR)
				{ error (NOSTR); return (-1); }

			argv = new_argv; argvsz = new_argvsz;
		}

		argv[argc++] = token.t_sval;
	}
	while ((major = get_token ()) == DEFVAR || major == alt_major);

	argv[argc] = NOSTR;	/* Garantidamente não estoura */

	return (0);

}	/* end get_argument_list */

/*
 ****************************************************************
 *	Analisa uma lista de padrões (para o comando CASE)	*
 ****************************************************************
 */
int
get_pattern_list (void)
{
	char		**new_argv;
	int		new_argvsz;

	while (token.t_major == STRING || token.t_major == DEFVAR)
	{
		if (argc >= argvsz - 1)
		{
			/* É preciso estender a área auxiliar */

			new_argvsz = argvsz + ALLOC_INC;
			new_argv   = realloc (argv, new_argvsz * sizeof (char *));

			if (new_argv == NOSSTR)
				{ error (NOSTR); return (-1); }

			argv = new_argv; argvsz = new_argvsz;
		}

		hasmeta (token.t_sval);

		argv[argc++] = token.t_sval;

		if (get_token () != PIPE)
			break;

		get_token ();
	}

	argv[argc] = NOSTR;	/* Garantidamente não estoura */

	return (0);

}	/* end get_pattern_list */

#ifdef	DEBUG
/*
 ****************************************************************
 *	Imprime a árvore gerada					*
 ****************************************************************
 */
void
print_tree (CMD *cmd, CMD *parent)
{
	char		**cp;
	int		i;

	if (cmd == NOCMD || cmd == ERRCMD)
	{
		fprintf (stderr, "{NULO} ");
		return;
	}

	if (cmd->c_parent != parent)
	{
		fprintf (stderr, "Árvore inconsistente\n");
		exit (1);
	}

	fprintf (stderr, "{ ");

	switch (cmd->c_type)
	{
	    case T_ASSIGN:
		if (cmd->c_assign)
		{
			for (cp = cmd->c_assign; *cp != NOSTR; cp++)
				fprintf (stderr, "[%s] ", *cp);
		}

		goto redir;

	    case T_SIMPLE:	/* c_argv < c_input > c_output */
		if (cmd->c_assign)
		{
			for (cp = cmd->c_assign; *cp != NOSTR; cp++)
				fprintf (stderr, "[%s] ", *cp);
		}

		for (cp = cmd->c_argv; *cp != NOSTR; cp++)
			fprintf (stderr, "\"%s\" ", *cp);

	    redir:
		print_redir (cmd);

		if (cmd->c_flags & M_BACK)
			fprintf (stderr, "& ");

		if (cmd->c_flags & M_PIPEOUT)
			fprintf (stderr, "[M_PIPEOUT] ");

		if (cmd->c_flags & M_PIPEIN)
			fprintf (stderr, "[M_PIPEIN] ");

		if (cmd->c_flags & M_SUBSHELL)
			fprintf (stderr, "[M_SUBSHELL] ");

		if (cmd->c_fdvector)
			fprintf (stderr, "<%d> ", cmd->c_fdvector);

		break;

	    case T_LIST:		/* c_leftcmd ; c_rightcmd */
		print_tree (cmd->c_leftcmd, cmd);
		fprintf (stderr, "; ");
		print_tree (cmd->c_rightcmd, cmd);
		break;

	    case T_PIPE:		/* c_leftcmd | c_rightcmd */
		print_tree (cmd->c_leftcmd, cmd);
		fprintf (stderr, "| ");
		print_tree (cmd->c_rightcmd, cmd);
		break;

	    case T_OR:		/* c_leftcmd || c_rightcmd */
		print_tree (cmd->c_leftcmd, cmd);
		fprintf (stderr, "|| ");
		print_tree (cmd->c_rightcmd, cmd);
		break;

	    case T_AND:		/* c_leftcmd && c_rightcmd */
		print_tree (cmd->c_leftcmd, cmd);
		fprintf (stderr, "&& ");
		print_tree (cmd->c_rightcmd, cmd);
		break;

	    case T_BREAK:
		fprintf (stderr, "break ");
		break;

	    case T_CASE:	/* comando CASE */
		fprintf (stderr, "CASE %s in ", cmd->c_argv[0]);

		i = 1;

		do
		{
			for (cp = &cmd->c_argv[i]; *cp != NOSTR; cp++)
				fprintf (stderr, "%s ", *cp);

			fprintf (stderr, ") ");

			print_tree (cmd->c_leftcmd, cmd);

			fprintf (stderr, ";; ");

			i = 0;

		}	while ((cmd = cmd->c_rightcmd) != NOCMD);

		fprintf (stderr, "ESAC");

		break;

	    case T_CONTINUE:
		fprintf (stderr, "continue ");
		break;

	    case T_FOR:		/* comando FOR */
		if (cmd->c_assign)
		{
			for (cp = cmd->c_assign; *cp != NOSTR; cp++)
				fprintf (stderr, "[%s] ", *cp);
		}

		cp = cmd->c_argv;
		fprintf (stderr, "FOR \"%s\" IN ", *cp);

		for (++cp; *cp != NOSTR; cp++)
			fprintf (stderr, "\"%s\" ", *cp);

		fprintf (stderr, "DO ");
		print_tree (cmd->c_leftcmd, cmd);

		break;

	    case T_IF:		/* comando IF */
		fprintf (stderr, "IF (");
		print_tree (cmd->c_leftcmd->c_leftcmd, cmd->c_leftcmd);
		fprintf (stderr, ") THEN");
		print_tree (cmd->c_leftcmd->c_rightcmd, cmd->c_leftcmd);

		while
		(	cmd->c_rightcmd != NOCMD &&
			cmd->c_rightcmd->c_type == T_IF
		)
		{
			cmd = cmd->c_rightcmd;

			fprintf (stderr, "ELIF (");
			print_tree (cmd->c_leftcmd->c_leftcmd, cmd->c_leftcmd);
			fprintf (stderr, ") THEN");
			print_tree (cmd->c_leftcmd->c_rightcmd, cmd->c_leftcmd);
		}

		if (cmd->c_rightcmd != NOCMD)
		{
			fprintf (stderr, "ELSE ");
			print_tree (cmd->c_rightcmd, cmd);
			fprintf (stderr, "FI");
		}

		break;

	    case T_UNTIL:	/* comando UNTIL */
		fprintf (stderr, "UNTIL ");
		print_tree (cmd->c_leftcmd, cmd);
		fprintf (stderr, "DO ");
		print_tree (cmd->c_rightcmd, cmd);
		break;

	    case T_WHILE:	/* comando WHILE */
		fprintf (stderr, "WHILE ");
		print_tree (cmd->c_leftcmd, cmd);
		fprintf (stderr, "DO ");
		print_tree (cmd->c_rightcmd, cmd);
		break;
	}

	fprintf (stderr, "} ");

}	/* end print_tree */

void
print_redir (CMD *cmd)
{
	RDRC		*rp, *beginp;

	if ((rp = cmd->c_rdrc) == NORDRC)
		return;

	rp = beginp = rp->r_next;

	fprintf (stderr, "[ ");

	do
	{
		switch (rp->r_type)
		{
		    case R_ERROR:
			putc ('?', stderr);
			break;

		    case R_DEL:		/* [digito]{<>}&- */
			fprintf (stderr, "%d>&- ", rp->r_fd);
			break;

		    case R_FROM:	/* [digito]<&digito */
			fprintf (stderr, "%d<&%d ", rp->r_fd, rp->r.r_dupfd);
			break;

		    case R_TO:		/* [digito]>&digito */
			fprintf (stderr, "%d>&%d ", rp->r_fd, rp->r.r_dupfd);
			break;

		    case R_IN:		/* [digito]< nome */
			fprintf (stderr, "%d< %s ", rp->r_fd, rp->r.r_name);
			break;

		    case R_OUT:		/* [digito]> nome} */
			fprintf (stderr, "%d> %s ", rp->r_fd, rp->r.r_name);
			break;

		    case R_APP:		/* [digito]>> nome} */
			fprintf (stderr, "%d>> %s ", rp->r_fd, rp->r.r_name);
			break;

		    case R_SIN:		/* [digito]<< nome} */
			fprintf (stderr, "%d<< %s ", rp->r_fd, rp->r.r_name);
			break;
		}
	}
	while ((rp = rp->r_next) != beginp);

	fprintf (stderr, "] ");

}	/* end print_redir */
#endif
