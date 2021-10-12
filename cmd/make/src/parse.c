/*
 ****************************************************************
 *								*
 *			parse.c					*
 *								*
 *	Analisador Sintático					*
 *								*
 *	Versão	3.0.0, de 29.03.96				*
 *		3.2.3, de 10.01.00				*
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
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../h/common.h"
#include "../h/error.h"
#include "../h/sym.h"
#include "../h/scan.h"
#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Estrutura de uma Lista de Alvos				*
 ****************************************************************
 */
#define	NOTLIST		(TLIST *)NULL

typedef	struct tlist	TLIST;

struct	tlist
{
	SYM	*t_sym;
	TLIST	*t_next;
};

/*
 ****************************************************************
 *	Variáveis locais a este módulo				*
 ****************************************************************
 */
static	TLIST	*virtual_list;		/* Lista de alvos virtuais */
static	TLIST	*defined_list;		/* Lista de alvos definidos */
static	SYM	*first_target;		/* Primeiro alvo definido */

/*
 ****************************************************************
 *	Protótipos de funções locais				*
 ****************************************************************
 */
void	define_macro (NLIST *);
void	dependency_list (NLIST *);
void	add_macros_to_environment (void);
void	declare_virtual_targets (void);
int	check_virtual_targets (void);
NLIST	*name_list (void);
CLIST	*command_list (void);
void	graph_init (SYM *);
void	drop_name_list (NLIST *);
void	defined_list_insert (SYM *);
int	forbidden (char *);

/*
 ****************************************************************
 *	Analisa sintaticamente o Makefile			*
 ****************************************************************
 */
entry SYM *
parse_makefile (void)
{
	SYM	*target;
	NLIST	*target_list;

	target = NOSYM;

	while (get_token () != EOT)
	{
		switch (token.t_major)
		{
		    case NAME:
			switch (token.t_sym->s_type)
			{
			    case S_PUTENV:
				add_macros_to_environment ();
				break;

			    case S_VIRTUAL:
				declare_virtual_targets ();
				break;

			    default:
				target_list = name_list ();

				if (token.t_major == CL)
				{
					if (target == NOSYM)
						target = target_list->n_sym;

					dependency_list (target_list);
				}
				elif (token.t_major == EQ)
				{
					define_macro (target_list);
				}
				else
				{
					err_msg
					(	ERROR, TOKMARK,
						"Esperava \":\" ou \"=\""
					);

					if (token.t_major == EOL || token.t_major == BOC)
						break;

					skip_line ();
					get_token ();
					break;
				}

				drop_name_list (target_list);
				break;
			}
			break;

		    case CL:
			err_msg (ERROR, TOKMARK, "Esperava um nome");
			dependency_list (NONLIST);
			break;

		    case EQ:
			err_msg (ERROR, TOKMARK, "Esperava um nome");
			define_macro (NONLIST);
			break;

		    case BOC:
		    case EOL:
		    case EOT:	/* para fazer o "lint" feliz */
			break;
		}

		if (token.t_major == EOT)
			break;

		if (token.t_major != EOL && token.t_major != BOC)
		{
			err_msg (ERROR, TOKMARK, "Esperava fim de linha");
			skip_line ();

			if (get_token () == EOT)
				break;
		}

	}	/* end while (get_token ()) */

	return (check_virtual_targets () ? NOSYM : (first_target = target));

}	/* end parse */

/*
 ****************************************************************
 *	Processa a definição de uma macro			*
 ****************************************************************
 */
void
define_macro (NLIST *target_list)
{
	SYM		*sp;
	NLIST		*np;
	MACRO		*mp;
	RANGE		*rp;
	static RANGE	macro_text;

	/*
	 *	Obtém o texto da macro.
	 */
	get_string (rp = &macro_text);

	/*
	 *	Processa as definições.
	 */
	for (np = target_list; np != NONLIST; np = np->n_next)
	{
		sp = np->n_sym;

		/*
		 *	Se "m_line" < 0, a macro foi definida na opção -D e
		 *	a nova definição deve ser ignorada.
		 */
		if ((mp = &sp->s_macro)->m_nline >= 0)
		{
			if (!sp->s_isid)
			{
				set_mark (&np->n_range);
				err_msg
				(	ERROR, GIVENMARK,
					"\"%s\" não é um identificador",
					sp->s_name
				);
			}

			mp->m_begin = rp->r_begin;
			mp->m_line  = rp->r_line;
			mp->m_nline = rp->r_nline;
		}
	}

	get_token ();		/* Obtém o <nl> */

#ifdef	DEBUG
	for (np = target_list; np != NONLIST; np = np->n_next)
	{
		sp = np->n_sym;
		if (sp->s_macro.m_begin != NOSTR)
		{
			fprintf
			(	stderr,
				"Macro: <%s> = <%s>\n",
				sp->s_name, sp->s_macro.m_begin
			);
		}
	}
#endif	DEBUG

}	/* end define_macro */

/*
 ****************************************************************
 *	Processa uma lista de dependências e comandos		*
 ****************************************************************
 */
void
dependency_list (NLIST *target_list)
{
	NLIST	*depend_list, *np;
	int	flags;
	SYM	*sp, *lastsp;
	DEPEN	*dp;
	CLIST	*cp;

	flags = D_DEFINED;
	depend_list = NONLIST;

	if (get_token () == NAME)
	{
		if (token.t_sym->s_type == S_EXIST)
		{	/*
			 *	A lista de pre-requisitos contém apenas a
			 *	palavra reservada ".exist".
			 */
			flags |= D_EXIST;
			get_token ();
		}
		else
		{	/*
			 *	Obtém a lista de pre-requisitos.
			 */
			depend_list = name_list ();

			for (np = depend_list; np != NONLIST; np = np->n_next)
				graph_init (np->n_sym);
		}
	}

#ifdef	DEBUG
	putc ('\n', stderr);
	for (np = target_list; np != NONLIST; np = np->n_next)
		fprintf (stderr, "<%s> ", np->n_sym->s_name);

	fprintf
	(	stderr,
		"depende%s de: \n",
		target_list->n_next == NONLIST ? "" : "m"
	);

	for (np = depend_list; np != NONLIST; np = np->n_next)
		fprintf (stderr, "   <%s>\n", np->n_sym->s_name);

	fflush (stderr);
#endif	DEBUG

	/*
	 *	Obtém a lista de comandos.
	 */
	cp = command_list ();

	/*
	 *	Coloca as arestas do grafo de dependências.
	 */
	lastsp = NOSYM;
	for (np = target_list; np != NONLIST; np = np->n_next)
	{
		graph_init (sp = np->n_sym);

		dp = &sp->s_node;
		if (dp->d_flags & D_DEFINED)
		{	/*
			 *	Erro: alvo redefinido.
			 */
			set_mark (&np->n_range);
			err_msg
			(	ERROR, GIVENMARK,
				"\"%s\" já apareceu antes como alvo, "
				"na linha %d",
				sp->s_name, dp->d_nline
			);
		}
		else
		{	/*
			 *	Preenche as informações sobre o alvo.
			 */
			dp->d_flags    |= flags;
			dp->d_depen	= depend_list;
			dp->d_command	= cp;
			dp->d_nline	= np->n_range.r_nline;

			/*
			 *	Inclui na lista de alvos definidos.
			 */
			if (listonly)
				defined_list_insert (sp);

			/*
			 *	Lista circular de alvos irmãos.
			 */
			dp->d_sibling = lastsp;
			lastsp = sp;
		}
	}

	/*
	 *	"Fecha" a lista circular de alvos irmãos.
	 */
	if (lastsp != NOSYM)
		target_list->n_sym->s_node.d_sibling = lastsp;

}	/* end dependency_list */

/*
 ****************************************************************
 *	Processa o ".putenv"					*
 ****************************************************************
 */
void
add_macros_to_environment (void)
{
	const SYM	*sp;
	char		*str;
	const char	*envstr, *cp;

	while (get_token () == NAME)
	{
		sp = token.t_sym;
		if (sp->s_type != S_NAME || sp->s_macro.m_begin == NOSTR)
		{
			err_msg
			(	ERROR, TOKMARK,
				"\"%s\" não é um nome de macro",
				sp->s_name
			);

			continue;
		}

		if (!execcmd)
			continue;

		if ((cp = getenv (sp->s_name)) != NOSTR)
		{
			str = emalloc (strlen (sp->s_name) + 2);
			sprintf (str, "$%s", sp->s_name);

			envstr = expand (str, NOINT);

			if (!streq (cp, envstr))
			{
				err_msg
				(	WARNING, NOMARK,
					"\"%s\" já está no ambiente com outro valor (\"%s\" :: \"%s\")",
					sp->s_name, envstr, cp
				);
			}

			free (str);

		}
		else
		{
			str = emalloc (2 * strlen (sp->s_name) + 3);
			sprintf (str, "%s=$%s", sp->s_name, sp->s_name);

			if ((envstr = strdup (expand (str, NOINT))) == NOSTR)
			{
				msg ("Memória insuficiente");
				exit (2);
			}

			if (putenv (envstr) < 0)
			{
				err_msg
				(	ERROR, NOMARK,
					"Não há mais lugar no ambiente"
				);

				free ((char *)envstr);
			}
			else
			{
				fprintf (stderr, "=> putenv %s\n", envstr);
			}

			free (str);
		}
	}

}	/* end add_macros_to_environment */

/*
 ****************************************************************
 *	Declara alvos virtuais					*
 ****************************************************************
 */
void
declare_virtual_targets (void)
{
	SYM	*sp;
	DEPEN	*dp;
	TLIST	*tp;

	while (get_token () == NAME)
	{
		if ((sp = token.t_sym)->s_type != S_NAME)
		{
			err_msg
			(	ERROR, TOKMARK,
				"\"%s\" é palavra reservada", sp->s_name
			);

			continue;
		}

		dp = &sp->s_node;

		if (dp->d_flags & D_VIRTUAL)
		{
			err_msg
			(	ERROR, TOKMARK,
				"Alvo virtual repetido"
			);
		}
		elif (dp->d_flags & D_DEFINED)
		{
			err_msg
			(	ERROR, TOKMARK,
				"Declaração tardia: o alvo já foi definido "
				"na linha %d\n",
				dp->d_nline
			);
		}
		else
		{
			dp->d_flags |= D_VIRTUAL;
			dp->d_nline  = token.t_range.r_nline;

			tp = emalloc (sizeof (TLIST));
			tp->t_sym    = sp;
			tp->t_next   = virtual_list;
			virtual_list = tp;
		}
	}

}	/* end declare_virtual_targets */

/*
 ****************************************************************
 *	Verifica se existem os alvos declarados como virtuais	*
 ****************************************************************
 */
int
check_virtual_targets (void)
{
	SYM	*sp;
	TLIST	*tp;
	int	superfluous = 0;

	for (tp = virtual_list; tp != NOTLIST; tp = tp->t_next)
	{
		sp = tp->t_sym;
		if (sp->s_node.d_sibling == NOSYM)
		{
			superfluous++;
			err_msg
			(	ERROR,
				NOMARK,
				"Alvo virtual \"%s\" supérfluo, na linha %d",
				sp->s_name,
				sp->s_node.d_nline
			);
		}
	}

	return (superfluous);

}	/* end check_virtual_targets */

/*
 ****************************************************************
 *	Obtém uma lista de nomes				*
 ****************************************************************
 */
NLIST *
name_list (void)
{
	NLIST		*last, *np;
	SYM		*sp;
	static NLIST	blank_node;

	np = last = &blank_node;	/* começa com um nó em branco */

	do
	{
		if ((sp = token.t_sym)->s_type != S_NAME)
		{
			err_msg
			(	ERROR, TOKMARK,
				"\"%s\" é palavra reservada", sp->s_name
			);

			continue;
		}

		np = emalloc (sizeof (NLIST));

		np->n_sym   = sp;
		np->n_range = token.t_range;

		last->n_next = np;
		last = np;

	}	while (get_token () == NAME);

	np->n_next = NONLIST;

	return (blank_node.n_next);	/* despreza o nó em branco */

}	/* end name_list */

/*
 ****************************************************************
 *	Obtém uma lista de comandos				*
 ****************************************************************
 */
CLIST *
command_list (void)
{
	CLIST		*last, *cp;
	RANGE		*rp;
	static CLIST	blank_node;
	static RANGE	command;

	last = cp = &blank_node;	/* começa com um nó em branco */

	rp = &command;

	while (token.t_major == BOC)
	{
		cp = emalloc (sizeof (CLIST));

		get_string (rp);

		cp->c_command	= rp->r_begin;
		cp->c_line	= rp->r_line;
		cp->c_nline	= rp->r_nline;
		cp->c_metachar	= 0;

		last->c_next = cp;
		last = cp;

		(void)get_token ();

#ifdef	DEBUG
		{
			char *x;
			int  meta;

			fprintf (stderr, "=> <%s>\n", cp->c_command);
			x = expand (cp->c_command, &meta);
			fprintf (stderr, "=> (meta = %d) <%s>\n", meta, x);

			fflush (stderr);
		}
#endif	DEBUG
	}

	cp->c_next = NOCLIST;

	return (blank_node.c_next);	/* despreza o nó em branco */

}	/* end command_list */

/*
 ****************************************************************
 *	Inicializa uma entrada no grafo de dependências		*
 ****************************************************************
 */
void
graph_init (SYM *sp)
{
	DEPEN	*dp;
	STAT	st;

	if ((dp = &sp->s_node)->d_sibling == NOSYM)
	{	/*
		 *	Primeira ocorrência do nome.
		 */
	/***	dp->d_depen   = NONLIST;	***/
	/***	dp->d_command = NOCLIST;	***/

		dp->d_sibling = sp;

		if (stat (sp->s_name, &st) < 0)
			/*** dp->d_time = 0 ***/;
		else
			dp->d_time = st.st_mtime;
	}

}	/* end graph_init */

/*
 ****************************************************************
 *	Libera uma lista de nomes				*
 ****************************************************************
 */
void
drop_name_list (NLIST *np)
{
	NLIST	*nextp;

	while (np != NONLIST)
	{
		nextp = np->n_next;
		free (np);
		np = nextp;
	}

}	/* end drop_name_list */

/*
 ****************************************************************
 *	Insere um alvo na lista de definidos, ordenadamente	*
 ****************************************************************
 */
void
defined_list_insert (SYM *sp)
{
	TLIST	*tlistp, *btp, *tp;

	tlistp = emalloc (sizeof (TLIST));

	/*
	 *	Procura o lugar correto da inserção
	 */
	for
	(	btp = NOTLIST, tp = defined_list;
		tp != NOTLIST;
		btp = tp, tp = tp->t_next
	)
	{
		if (strcmp (sp->s_name, tp->t_sym->s_name) <= 0)
			break;
	}

	tlistp->t_sym  = sp;
	tlistp->t_next = tp;

	if (btp == NOTLIST)
		defined_list = tlistp;
	else
		btp->t_next = tlistp;

}	/* end defined_list_insert */

/*
 ****************************************************************
 *	Verifica se o alvo é listável				*
 ****************************************************************
 */
int
forbidden (char *name)
{
	static char	*forbidden_patterns[] = { "*.o", NOSTR };
	char		**pp;

	for (pp = forbidden_patterns; *pp != NOSTR; pp++)
	{
		if (patmatch (name, *pp))
			return (1);
	}

	return (0);

}	/* end forbidden */

/*
 ****************************************************************
 *	Imprime a lista de alvos definidos			*
 ****************************************************************
 */
void
print_target_list (void)
{
	TLIST	*tp;
	SYM	*sp;
	DEPEN	*dp;

	if ((tp = defined_list) == NOTLIST)
	{
		printf ("Não há alvos definidos em \"%s\"\n", makefile_name);
	}
	else
	{
		printf ("VIR  LINHA  ------ ALVO -------\n");

		for (/* tp definido acima */; tp != NOTLIST; tp = tp->t_next)
		{
			sp = tp->t_sym;
			dp = &sp->s_node;

			if (listonly == 1 && forbidden (sp->s_name))
				continue;

			printf
			(	" %c   %5d  %s %s\n",
				dp->d_flags & D_VIRTUAL ? '*' : ' ',
				dp->d_nline,
				sp == first_target ? "=>" : "  ",
				sp->s_name
			);
		}
	}

}	/* end print_target_list */
