/*
 ****************************************************************
 *								*
 *			expand.c				*
 *								*
 *	Expansão de argumentos					*
 *								*
 *	Versão	3.2.3, de 05.01.00				*
 *		4.4.0, de 05.12.02				*
 *								*
 *	Módulo: SHELL						*
 *		Interpretador de Comandos			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#include "../h/common.h"
#include "../h/sym.h"
#include "../h/command.h"
#include "../h/scan.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Definições locais					*
 ****************************************************************
 */
#ifdef	STATIC_BUF
#define	MAXBUF		4096		/* Caso o buffer seja estático */
#endif

/*
 ******	Estrutura da lista de nomes *****************************
 */
#define	NOITEM		(ITEM *)NULL

typedef struct	item	ITEM;

struct	item
{
	ITEM	*i_next;		/* Próximo nome */
	int	i_len;			/* Tamanho do nome */
	char	i_name[1];		/* Início do nome */
};

/*
 ******	Variáveis locais da expansão ***************************
 */
static	char	**exp_argv;		/* Começo */
static	int	exp_argv_sz;		/* Número de entradas */
static	int	exp_argc;		/* Número de entradas ocupadas */

static	STAT	root_st;		/* Para dar "stat" em "/" */

#ifdef	STATIC_BUF
static	char	path[MAXBUF];
static	int	max_path_sz = MAXBUF;
#else
static	char	*path;			/* Guarda os nomes sendo formados */
static	int	max_path_sz;
#endif

/*
 ******	Protótipos de funções ***********************************
 */
int	expand (char **);
int	do_expand (int, char *, STAT *);
char	*substitute_variable (char *, char *);
char	**expand_command (char *, char *);
int	add_new_arg (char *);
void	free_args (char **);

/*
 ****************************************************************
 *	Prepara a expansão					*
 ****************************************************************
 *
 *	Retorna os novos "argv" e "argc", resultantes da expansão.
 */
char **
expand_args (char **argv, int *new_argc)
{
	/*
	 *	Há o que expandir ?
	 */
	if (argv == NOSSTR)
		return (NOSSTR);

#ifndef	STATIC_BUF
	/*
	 *	Aloca a área para conter os nomes sendo formados.
	 *	Esta área é reaproveitada em diversas chamadas.
	 */
	if (path == NOSTR)
	{
		if ((path = malloc (max_path_sz = ALLOC_INC)) == NOSTR)
			return (NOSSTR);
	}
#endif

	/*
	 *	Inicializa as variáveis de modo que a primeira chamada a
	 *	"add_new_arg" funcione corretamente.
	 */
	exp_argv    = NOSSTR;
	exp_argv_sz = exp_argc = 0;

	/*
	 *	Realiza a expansão.
	 */
	if (expand (argv) < 0)
		goto bad;

	/*
	 *	Coloca o ponteiro NULO no final.
	 */
	if (add_new_arg (NOSTR) < 0)
		goto bad;

	if (new_argc != NULL)
		*new_argc = exp_argc - 1;

	return (exp_argv);

	/*
	 *	Libera e retorna erro.
	 */
    bad:
	if (exp_argv != NOSSTR)
	{
		char	**ap;

		for (ap = exp_argv + exp_argc - 1; ap >= exp_argv; ap--)
		{
			if (*ap != NOSTR)
				free (*ap);
		}

		free (exp_argv);
	}

	return (NOSSTR);

}	/* end expand_args */

/*
 ****************************************************************
 *	Expande uma lista de argumentos				*
 ****************************************************************
 */
int
expand (char **argv)
{
	char		*arg = NOSTR, *new_arg, *cp;
	int		not_found = 0, len, old_exp_argc;
	STAT		dot_st, *sp = &root_st;

	/*
	 *	Percorre os diversos argumentos.
	 */
	for (/* vazio */; *argv != NOSTR; argv++)
	{
		/*
		 *	Salva o argumento.
		 */
		if ((arg = malloc (len = strlen (*argv) + 1)) == NOSTR)
			return (-1);

		memmove (arg, *argv, len);
 
		/*
		 *	Trata expansão de comandos.
		 */
		if ((cp = strchr (arg, CMD_SYMBOL)) != NOSTR)
		{
			char	**cmd_argv;

			/*
			 *	Executa o comando.
			 */
			ignore_signals++;
			cmd_argv = expand_command (arg, cp);
			ignore_signals--;

			if (cmd_argv == NOSSTR)
				goto bad;

			/*
			 *	Expande o resultado e libera os argumentos.
			 */
			if (expand (cmd_argv) < 0)
			{
				free_args (cmd_argv);
				goto bad;
			}

			free_args (cmd_argv);
			free (arg);

			continue;
		}

		/*
		 *	Substitui, primeiramente, as variáveis.
		 */
		while ((cp = strchr (arg, VAR_SYMBOL)) != NOSTR)
		{
			if ((new_arg = substitute_variable (arg, cp)) == NOSTR)
				goto bad;

			free (arg);
			arg = new_arg;
		}

		/*
		 *	Verifica se é necessário expandir.
		 */
		if (!hasmeta (arg))
		{
			/* NÃO é necessário, pois NÃO há metacaracteres */

			if (add_new_arg (arg) < 0)
				goto bad;

			/* Não libera "arg" */
			continue;
		}

		/*
		 *	Haverá expansão.
		 */
		new_arg = arg;
		len     = 0;

		if (*new_arg == '/')
		{
			/* Referência absoluta a partir de "/" */

			sp = &root_st;

			if (root_st.st_mode == 0 && stat ("/", sp) < 0)
			{
				error ("*Não consegui dar \"stat\" em \"/\"");
				goto bad;
			}

			path[len++] = '/';

			while (*++new_arg == '/')
				/* vazio */;
		}
		else
		{
			/* Referência relativa ao diretório corrente */

			sp = &dot_st;

			if (stat (".", sp) < 0)
			{
				error ("*Não consegui dar \"stat\" em \".\"");
				return (-1);
			}
		}

		/*
		 *	Expande e verifica se o padrão foi satisfeito.
		 */
		old_exp_argc = exp_argc;

		if (do_expand (len, new_arg, sp) < 0)
			goto bad;

		if (old_exp_argc == exp_argc)
		{
			error ("Padrão \"%s\" não encontrado", arg);
			not_found++;
		}

		free (arg);
	}

	if (not_found && (input_file != stdin || !ask_yes_no ("Continua")))
		return (-1);

	return (0);

    bad:
	if (arg != NOSTR)
		free (arg);

	return (-1);

}	/* end expand */

/*
 ****************************************************************
 *	Substitui uma variável em uma cadeia			*
 ****************************************************************
 */
char *
substitute_variable (char *arg, char *cp)
{
	char		*new_arg, *tmp, *value, *save_value;
	int		old_len, val_len, new_len, len, inquote;

	/*
	 *	O analisador léxico codifica o nome da variável a
	 *	substituir da seguinte maneira:
	 *		VAR_SYMBOL x <caracteres do nome>
	 *	onde:
	 *		VAR_SYMBOL: 24 (um caractere inválido)
	 *		x = '0':    a expansão é fora de ""
	 *		x = '1':    a expansão é dentro de "".
	 */
	tmp     = cp + 1;
	len     = cp - arg;		/* prefixo */
	inquote = *tmp++ - '0';

	/*
	 *	Obtém o valor da variável.
	 */
	if ((value = get_variable_value (&tmp, 1)) == INVSTR)
		return (NOSTR);

	if (value == NOSTR)
		value = "";

	val_len = strlen (value);

	if (!inquote)
	{
		/* Copia o valor da variável e substitui os metacaracteres */

		if ((save_value = alloca (val_len + 1)) == NOSTR)
			return (arg);

		memmove (save_value, value, val_len + 1);
		putmeta (save_value);

		value = save_value;
	}

	/*
	 *	Aloca a nova cadeia.
	 */
	old_len = strlen (arg);
	new_len = old_len - (tmp - cp) + val_len + 1;

	if ((new_arg = malloc (new_len)) == NOSTR)
		return (NOSTR);

	/*
	 *	Substitui a ocorrência da variável por seu valor.
	 */
	if (len > 0)
		memmove (new_arg, arg, len);	/* prefixo */

	memmove (new_arg + len, value, val_len);

	memmove (new_arg + len + val_len, tmp, old_len - (tmp - arg) + 1);

	return (new_arg);

}	/* end substitute_variable */

/*
 ****************************************************************
 *	Realiza a substituição de um comando			*
 ****************************************************************
 */
char **
expand_command (char *arg, char *sp)
{
	char		*cmd, *end_cmd, *cp, **argv, buf[512];
	int		tube[2], pid;
	int		len, argc, argv_sz, c, inquote;
	FILE		*fp;

	/*
	 *	Monta a linha de comando.
	 */
	inquote = *++sp - '0';
	end_cmd = strchr (++sp, CMD_SYMBOL);
	len     = end_cmd - sp;

	if (len == 0 || (cmd = alloca (len + 1)) == NOSTR)
		return (NOSSTR);

	memmove (cmd, sp--, len);

	cmd[len] = NOCHR;

	/*
	 *	Cria um "pipe" e um processo filho.
	 */
	if (pipe (tube) < 0)
	{
		error ("*Erro ao criar um pipe");
		return (NOSSTR);
	}

	if ((pid = fork ()) < 0)
	{
		error ("*Não consegui criar um processo filho");
		return (NOSSTR);
	}

	if (pid == 0)
	{
		/*
		 *	O processo filho executa o comando dado, com o
		 *	"stdout" redirecionado para o PIPE.
		 */
		close (tube[0]);
		dup2  (tube[1], 1);
		close (tube[1]);

		insert_line (cmd);

		exit (exec (get_command (M_NOFLAGS), 0, 0, 0));
	}

	/*
	 *	O pai lê os resultados do comando.
	 */
	close (tube[1]);

	if ((fp = fdopen (tube[0], "r")) == NOFILE)
	{
		error ("*Não consegui reabrir o PIPE");

		close (tube[0]);
		wait (NULL);
		return (NOSSTR);
	}

	argv = NOSSTR;
	argc = argv_sz = 0;

	/*
	 *	Copia o prefixo para o buffer.
	 */
	if ((len = (--sp) - arg) > 0)
		memmove (buf, arg, len);

	cp = &buf[len];

	/*
	 *	Lê os resultados do comando.
	 */
	while ((c = getc (fp)) != EOF)
	{
		if (c == ' ' || c == '\t' || c == '\n')
		{
			/* Veio um separador */

			if (inquote)
			{
				if (cp > buf && cp[-1] != ' ')
					*cp++ = ' ';

				continue;
			}

			if (cp > buf)
			{
				*cp = NOCHR;

				if (argc >= argv_sz - 1)
				{
					/* Aumenta o argv */

					char	**new_argv;

					argv_sz += ALLOC_INC;

					new_argv = realloc (argv, argv_sz * sizeof (char *));

					if (new_argv == NOSSTR)
					{
						error (NOSTR);
						goto bad;
					}

					argv = new_argv;
				}

				if ((argv[argc] = strdup (buf)) == NOSTR)
				{
					error (NOSTR);
					goto bad;
				}

				argc++;
				cp = buf;
			}
		}
		else
		{
			/* Não separador: vai juntando no buffer */

			if (cp < &buf[sizeof (buf) - 1])
				*cp++ = c;
		}
	}

	if (inquote)
	{
		while (--cp >= buf && *cp == ' ')
			/* vazio */;

		*++cp = NOCHR;

		if ((argv = malloc (2 * sizeof (char *))) == NOSSTR)
			goto bad;

		if ((argv[0] = strdup (buf)) == NOSTR)
			goto bad;

		argc++;
	}

	/*
	 *	Concatena o sufixo com o último argumento obtido.
	 */
	if (*++end_cmd != NOCHR)
	{
		char	**ap = &argv[argc - 1];
		int	new_len = strend (end_cmd) - end_cmd;

		len = strlen (*ap);

		if ((cp = realloc (*ap, new_len + len + 1)) == NOSTR)
			goto bad;

		*ap = cp;

		memmove (*ap + len, end_cmd, new_len + 1);
	}

	if (argv != NOSSTR)
		argv[argc] = NOSTR;	/* Arremata o "argv" */

	fclose (fp);

	wait (NULL);

	return (argv);

	/*
	 *	Libera os argumentos e retorna.
	 */
    bad:
	if (argv != NOSSTR)
	{
		for (c = 0; c < argc; c++)
		{
			if (argv[c] != NOSTR)
				free (argv[c]);
		}

		free (argv);
	}

	fclose (fp);

	wait (NULL);

	return (NOSSTR);

}	/* end expand_command */

/*
 ****************************************************************
 *	Função de expansão recursiva				*
 ****************************************************************
 */
int
do_expand (int prefix_len, char *pattern, STAT *sp)
{
	ITEM		*ip, *lp, *np, file_list;
	int		len, new_prefix_len, max_dir_len;
	char		*nextp, *end_pat, begin_pat, nextc;
	DIR		*dir_fp;
	const DIRENT	*dp;

	/*
	 ******	Obtém o padrão **********************************
	 */
	nextp = pattern;
	nextc = begin_pat = *nextp;		/* salva o caractere inicial */

	while (nextc != NOCHR && nextc != '/')
		nextc = *++nextp;

	if ((end_pat = nextp) == pattern)	/* nada a fazer */
		return (0);

	/*
	 *	"nextp" fica apontando para o próximo padrão.
	 *	"end_pat" fica apontando para o fim do padrão corrente.
	 *	"nextc" guarda o caractere seguinte ao padrão ('/' ou NOCHR).
	 */
	if (nextc == '/')
	{
		/*
		 *	Troca por um NOCHR para marcar o fim do padrão.
		 *	Pula '/'s supérfluas.
		 */
		 for (*nextp++ = NOCHR; *nextp == '/'; nextp++)
			/* vazio */;
	}

	/*
	 ******	Processa o diretório ****************************
	 */
	if ((dir_fp = inopendir (sp->st_dev, sp->st_ino)) == NODIR)
	{
		error ("*Não consegui abrir o diretório \"%s\"", path);
		goto ok;
	}

	/*
	 *	Os componentes são guardados em uma lista para:
	 *	. serem processados em ordem alfabética;
	 *	. fechar o diretório antes de chamar recursivamente
	 *	  a função (economia de descritores).
	 */
	file_list.i_next = NOITEM;
	max_dir_len      = 0;

	while ((dp = readdir (dir_fp)) != NODIRENT)
	{
		/*
		 *	Descarta entradas inconvenientes, a saber:
		 *	. as começadas por '.' (caso o padrão
		 *	  não comece por '.');
		 *	. as que não casam com o padrão.
		 */
		if
		(	dp->d_name[0] == '.' && begin_pat != '.' ||
			!patmatch (dp->d_name, pattern)
		)
			continue;

		/*
		 *	Atualiza o tamanho da maior componente.
		 */
		if ((len = dp->d_namlen) > max_dir_len)
			max_dir_len = len;

		/*
		 *	Insere na lista, na ordem correta.
		 *	"alloca" é adequada, neste caso, pois a lista
		 *	é destruída automaticamente no retorno da rotina.
		 */
		for (lp = &file_list; (np = lp->i_next) != NOITEM; lp = np)
		{
#define	FOLD
#ifdef	FOLD
			if (strttcmp (dp->d_name, np->i_name, cmpfoldtb, cmpafoldtb) <= 0)
#else
			if (strcmp (dp->d_name, np->i_name) <= 0)
#endif	FOLD
				break;
		}

		ip = alloca (sizeof (ITEM) + len);	/* ITEM + nome */

		ip->i_len  = len;
		ip->i_next = np;

		memmove (ip->i_name, dp->d_name, len + 1);

		lp->i_next = ip;
	}

	closedir (dir_fp);

	/*
	 ******	Processa a expansão *****************************
	 */
	/*
	 *	Verifica se o tamanho do buffer é suficiente.
	 */
	new_prefix_len = prefix_len + max_dir_len + 1;	/* +1 para o NOCHR */

	if (new_prefix_len > max_path_sz)
	{
#ifdef	STATIC_BUF
		goto bad;
#else
		char	*new_path;
		int	new_sz;

		new_sz = (new_prefix_len + ALLOC_MASK) & ~ALLOC_MASK;

		if ((new_path = realloc (path, new_sz)) == NOSTR)
			goto bad;

		max_path_sz = new_sz;
		path	    = new_path;
#endif
	}

	/*
	 *	Percorre a lista, expandindo.
	 */
	for (ip = file_list.i_next; ip != NOITEM; ip = ip->i_next)
	{
		STAT	s;

		len		= ip->i_len;
		new_prefix_len	= prefix_len + len;

		/*
		 *	Concatena a componente, incluindo o NOCHR.
		 */
		memmove (path + prefix_len, ip->i_name, len + 1);

		if (nextc == NOCHR)
		{
			/* NÃO há mais padrões: acrescenta o nome formado */

			if (add_new_arg (strdup (path)) < 0)
				goto bad;
		}
		elif (stat (path, &s) >= 0 && S_ISDIR (s.st_mode))
		{
			/* Ainda há padrões: chama recursivamente */

			path[new_prefix_len++] = '/';

			if (do_expand (new_prefix_len, nextp, &s) < 0)
				goto bad;
		}
	}

    ok:
	*end_pat = nextc;	/* restaura o caractere original */
	return (0);

    bad:
	*end_pat = nextc;	/* restaura o caractere original */
	return (-1);

}	/* end do_expand */

/*
 ****************************************************************
 *	Adiciona um argumento à lista de argumentos		*
 ****************************************************************
 */
int
add_new_arg (char *arg)
{
	char		**new_exp_argv;

	if (exp_argc >= exp_argv_sz)
	{
		/* É preciso aumentar o tamanho do vetor */

		exp_argv_sz += ALLOC_INC;

		if ((new_exp_argv = realloc (exp_argv, exp_argv_sz * sizeof (char *))) == NOSSTR)
		{
			exp_argv_sz -= ALLOC_INC;
			return (-1);
		}

		exp_argv = new_exp_argv;
	}

	/*
	 *	Acrescenta a nova cadeia.
	 */
#if (0)	/*******************************************************/
	if (arg != NOSTR && (arg = strdup (arg)) == NOSTR)
		return (-1);
#endif	/*******************************************************/

	exp_argv[exp_argc++] = arg;

	return (0);

}	/* end add_new_arg */

/*
 ****************************************************************
 *	Libera a área ocupada pelos argumentos expandidos	*
 ****************************************************************
 */
void
free_args (char **argv)
{
	char		**cp;

	if (argv == NOSSTR)
		return;

	for (cp = argv; *cp != NOSTR; cp++)
	{
		if (*cp != NOSTR)
			free (*cp);
		else
			error ("\"free_args\" liberando argumento NULO");
	}

	free (argv);

}	/* end free_args */



#ifdef DEBUG
main (int argc, char *argv[])
{
	char	**arg, **newargv;
	int	new_argc;

	if ((newargv = exp_argv (&argv[1], &new_argc)) != NOSSTR)
	{
		for (arg = newargv; *arg; arg++)
			fprintf (stderr, "%s ", *arg);

		putc ('\n', stderr);
	}
}
#endif
