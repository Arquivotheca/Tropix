/*
 ****************************************************************
 *								*
 *			make.c					*
 *								*
 *	Programa principal					*
 *								*
 *	Versão	3.0.0, de 29.03.96				*
 *		4.1.0, de 28.08.01				*
 *								*
 *	Módulo: Make: Construtor de Programas			*
 *		Interpretador de Comandos			*
 *		Categoria D					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

#include "../h/common.h"
#include "../h/error.h"
#include "../h/sym.h"
#include "../h/scan.h"
#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Versão do programa					*
 ****************************************************************
 */
entry	const char	pgversion[]	= "Versão 4.1.0, de 28.08.01";
entry	const char	env_opt_nm[]	= "_MAKE_@_OPTIONS";

/*
 ****************************************************************
 *	Opções do usuário (na chamada do MAKE)			*
 ****************************************************************
 */
entry	char	*makefile_name;	/* Nome do arquivo de descrição */
entry	char	*shell;		/* Nome da "shell" a ser utilizada */

entry	int	touch,		/* Atualiza os módulos, sem executar comandos */
		nostop,		/* Prossegue, mesmo em presença de erros */
		execcmd = 1,	/* Executa os comandos */
		why,		/* Explica o motivo das atualizações */
		listonly,	/* Apenas lista os alvos */
		build;		/* Refaz tudo, independentemente das datas */

entry	void	(*sigint) (int, ...),	/* Tratamento de sinais */
		(*sigquit) (int, ...),
		(*sigterm) (int, ...);

/*
 ****************************************************************
 *	Protótipos de funções locais				*
 ****************************************************************
 */
void	sigcatch (int, ...);
void	help (int);
void	define_environment_variables (void);
int	define_variable (const char *, int);
void	get_options_from_environment (void);
void	put_options_in_environment (void);

/*
 ****************************************************************
 *	Programa principal					*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	int		opt, exitval;
	char		*makefile = NOSTR;
	SYM		*target;

	/*
	 *	Obtém as opções herdadas através do ambiente.
	 */
	get_options_from_environment ();

	/*
	 *	Analisa as opções de execução, que prevalecem sobre
	 *	as opções herdadas.
	 */
	while ((opt = getopt (argc, argv, "D:HMabf:intv")) != EOF)
	{
		switch (opt)
		{
		    case 'D':
			if (define_variable (optarg, NIL) < 0)
			{
				msg
				(	"Argumento inválido para a"
					" opção -D: %s",
					optarg
				);

				help (2);
			}
			break;

		    case 'H':
			help (0);
			break;

		    case 'M':
			return (0);

		    case 'a':
			listonly++;
			execcmd = 0;
			break;

		    case 'b':
			build++;
			break;

		    case 'f':
			makefile = (char *)optarg;
			break;

		    case 'i':
			nostop++;
			break;

		    case 'n':
			execcmd = 0;
			break;

		    case 't':
			touch++;
			break;

		    case 'v':
			why++;
			break;

		    default:
			putc ('\n', stderr);
			help (2);
		}
	}

	if (build && touch)
	{
		msg ("As opções '-b' e '-t' são mutuamente exclusivas");
		return (1);
	}

	argv += optind;

	/*
	 *	Tenta ler o Makefile, considerando se foi dado o nome
	 *	na opção -f.
	 */
	if (makefile == NOSTR)
	{
		if
		(	read_makefile ("Makefile") < 0 &&
			read_makefile ("makefile") < 0
		)
		{
			msg ("Arquivo de descrição não encontrado");
			return (1);
		}
	}
	else
	{
		if (read_makefile (makefile) < 0)
		{
			msg ("Arquivo \"%s\" não encontrado", makefile);
			return (1);
		}
	}

	/*
	 *	Define as variáveis do ambiente como macros.
	 */
	define_environment_variables ();

	/*
	 *	Redefine as opções do ambiente.
	 */
	put_options_in_environment ();

	/*
	 *	Trata respeitosamente os sinais.
	 */
	if ((sigint = signal (SIGINT, SIG_IGN)) != SIG_IGN)
		signal (SIGINT, sigcatch);

	if ((sigterm = signal (SIGTERM, SIG_IGN)) != SIG_IGN)
		signal (SIGTERM, sigcatch);

	if ((sigquit = signal (SIGQUIT, SIG_IGN)) != SIG_IGN)
		signal (SIGQUIT, sigcatch);

	/*
	 *	Analisa o Makefile.
	 */
	target = parse_makefile ();

	if (nerrors != 0)
	{
		msg
		(	"O arquivo de descrição \"%s\" contém erros",
			makefile_name
		);
		return (5);
	}

	if (listonly)
	{	/*
		 *	Apenas imprime a lista de alvos
		 */
		print_target_list ();
		return (0);
	}

	/*
	 *	Atualiza os módulos indicados.
	 */
	if (*argv == NOSTR)
	{	/*
		 *	Não há módulos na linha de argumentos.
		 *	Considera o primeiro alvo mencionado no Makefile.
		 */
		if (target == NOSYM)
		{
			msg ("Não foi dito o que atualizar");
			return (1);
		}

		exitval = make (target);
	}
	else
	{	/*
		 *	Há módulos na linha de argumentos.
		 *	Atualiza cada um deles.
		 */
		for (exitval = 0; *argv != NOSTR; argv++)
		{
			target = (hash ((char *)*argv))->h_sym;

			if
			(	target == NOSYM ||
				target->s_type != S_NAME ||
				target->s_node.d_sibling == NOSYM
			)
			{
				msg
				(	"Módulo \"%s\" não figura em \"%s\"",
					*argv, makefile_name
				);
			}
			else
			{
				exitval += make (target);
			}
		}
	}

	return (exitval);

}	/* end main */

/*
 ****************************************************************
 *	Resumo de Utilizacao do Programa			*
 ****************************************************************
 */
void
help (int exitcode)
{
	fprintf
	(	stderr,
		"%s - gerenciador de manutenção de programas\n\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-abintv] [-f <desc>] "
		"[-D <macro>=<valor>] ... <módulo> ...\n",
		pgname, pgversion, pgname
	);

	fprintf
	(	stderr,
		"\nOpções:"
		"\t-a: apenas lista os alvos (exceto \"*.o\")\n"
		"\t    (se duplicado, lista todos os alvos)\n"
		"\t-b: reconstrói todos os módulos\n"
		"\t-i: ignora erros em todos os comandos\n"
		"\t-n: apenas lista os comandos, sem os executar\n"
		"\t-t: atualiza os módulos, sem executar os comandos\n"
		"\t-v: explica o motivo das atualizações\n"
		"\t-f: usa <desc> como arquivo de descrição\n"
		"\t-D: define a macro <macro> com o texto <valor>\n"
	);

	exit (exitcode);

}	/* end help */

/*
 ****************************************************************
 *	Captura de sinais					*
 ****************************************************************
 */
void
sigcatch (int signo, ...)
{
	msg ("(\"%s\")", sigtostr (signo));
	fflush (stderr);
	exit (100);

}	/* end sigcatch */

/*
 ****************************************************************
 *	Define as variáveis do ambiente				*
 ****************************************************************
 */
void
define_environment_variables (void)
{
	const char	**ep, *cp;
	HASH		*hp;

	for (ep = environ; (cp = *ep) != NOSTR; ep++)
	{
		if (define_variable (cp, NIL) < 0)
			msg ("Cadeia esdrúxula no ambiente: \"%s\"", cp);
	}

	/*
	 *	Define o "Shell" a ser utilizado
	 */
	hp = hash ("SHELL");
	if (hp->h_sym != NOSYM)
		shell = hp->h_sym->s_macro.m_begin;

}	/* end define_environment_variables */

/*
 ****************************************************************
 *	Define variáveis					*
 ****************************************************************
 */
int
define_variable (const char *name, int nline)
{
	HASH	*hp;
	SYM	*sp;
	MACRO	*mp;
	char	*defp, *valuep;
	int	total_len, name_len;

	if ((valuep = strchr (name, '=')) == NOSTR)
		return (NIL);

	name_len  = valuep - name;
	total_len = strlen (name) + 1;

	defp = emalloc (total_len);
	(void)memmove (defp, name, total_len);

	valuep = defp + name_len;
	*valuep++ = NOCHR;

	hp = hash (defp);
	if (hp->h_sym == NOSYM)
	{
		sp = emalloc (SYM_FIXED_SZ + total_len);

		hp->h_sym  = sp;
		sp->s_type = S_NAME;
		sp->s_namelen = name_len;

		(void)memset (&sp->s_node, 0, sizeof (DEPEN));

	/***	sp->s_node.d_sibling = NOSYM;	***/
	/***	sp->s_node.d_flags   = 0;	***/
	/***	sp->s_node.d_nline   = 0;	***/

		(void)memmove (sp->s_name, defp, total_len);
		valuep = &sp->s_name[name_len + 1];

		mp = &sp->s_macro;
		mp->m_begin = valuep;
		mp->m_line  = valuep;
		mp->m_nline = nline;
	}

	free (defp);

	return (0);

}	/* end define_variable */

/*
 ****************************************************************
 *	Obtém as opções do ambiente				*
 ****************************************************************
 */
void
get_options_from_environment (void)
{
	char		opt;
	const char	*env;

	if ((env = getenv (env_opt_nm)) == NOSTR)
		return;

	while ((opt = *env++) != NOCHR)
	{
		switch (opt)
		{
		    case 'a':
			listonly++;
			execcmd = 0;
			break;

		    case 'b':
			build++;
			break;

		    case 'i':
			nostop++;
			break;

		    case 'n':
			execcmd = 0;
			break;

		    case 't':
			touch++;
			break;

		    case 'v':
			why++;
			break;

		    default:
			msg ("Opção inválida no ambiente: \"-%c\"", opt);
			help (2);

		}	/* end switch (opt) */
	}

	/*
	 *	Apaga a variável do ambiente.
	 */
	if (delenv (env_opt_nm) < 0)
		msg ("Não consegui remover \"%s\" do ambiente", env_opt_nm);

}	/* end get_options_from_environment */

/*
 ****************************************************************
 *	Insere as opções no ambiente				*
 ****************************************************************
 */
void
put_options_in_environment (void)
{
	char	*cp, *env;

	if ((env = malloc (sizeof (env_opt_nm) + 10)) == NOSTR)
	{
		msg ("Memória insuficiente");
		return;
	}

	cp = strend (strcpy (env, env_opt_nm));

	*cp++ = '=';

	if (build)
		*cp++ = 'b';

	if (touch)
		*cp++ = 't';

	if (why)
		*cp++ = 'v';

	if (!execcmd)
		*cp++ = 'n';

	if (listonly)
		*cp++ = 'a';

	if (nostop)
		*cp++ = 'i';

	*cp = NOCHR;

	if (putenv (env) < 0)
	{
		msg ("Não consegui incluir as opções no ambiente");
		free (env);
	}

}	/* end put_options_in_environment */
