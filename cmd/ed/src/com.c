/*
 ****************************************************************
 *								*
 *			src/com.c				*
 *								*
 *	Modo Comandos						*
 *								*
 *	Versão	1.0.0, de 24.11.86				*
 *		3.0.0, de 27.05.97				*
 *								*
 *	Modulo: ED						*
 *		Editores de Texto				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Multiprocessado		*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "../h/ed.h"
#include "../h/scan.h"
#include "../h/txtio.h"
#include "../h/proto.h"
#include "../h/extern.h"

/*
 ****************************************************************
 *	Definições locais					*
 ****************************************************************
 */
#define	PGLINES	20

typedef	enum
{
	EX_NORMAL,	/* Normal */
	EX_FGLOBAL,	/* Primeiro Global */
	EX_RGLOBAL,	/* Repetição Global */
	EX_ERROR	/* Houve Erro: não implementado */

}	STAT;

static AUTO	Subst;		/* Expressão Regular a ser Substituida */
static REPL	Replacement;	/* Cadeia p/ Substituição */
static char	System[LSIZE];	/* Ultimo comando para o Shell */

static BASE	*add1;
static BASE	*add2;

static SIZE	size,
		newsize;

static STAT	State = EX_NORMAL;

/*
 ****************************************************************
 *	Interpretação do Comando				*
 ****************************************************************
 */
bool
command (void)
{
	char		com;		/* Comando */
	static char	*args;		/* Argumento cadeia */
	static int	argi;		/* Argumento inteiro */
	BASE		*add3 = NOBASE;
	int		nadds;
	bool		new_global = false;
	char		delim;
	static bool	print;		/* Sufixo "p" */
	static bool	variant;	/* Forma variante */

	if (Interruption)
		return (false);

	/*
	 *	Pega endereços
	 */
	if ((add1 = get_address (Current)) != NOBASE)
	{
		if (*nextp == ',' || *nextp == ';')
		{
			if (*nextp == ';')
				Current = add1;

			nextp++;

			if ((add2 = get_address (Current)) != NOBASE)
			{
				nadds = 2;
			}
			elif (error_msg != NULL)
			{
				error (error_msg, LONGMARK);
				return (false);
			}
			else
			{
				error ("Espero segundo endereço", SHORTMARK);
				return (false);
			}
		}
		else
		{
			nadds = 1;
		}
	}
	elif (error_msg != NULL)
	{
		error (error_msg, LONGMARK);
		return (false);
	}
	else
	{
		nadds = 0;
	}

	/*
	 *	Leitura do comando
	 */
	while (isspace (*nextp) || *nextp == ':')
		nextp++;

	if (*nextp)
		com = *nextp++;
	else
		com = ' ';

	if (*nextp == '!')
	{
		variant = true;
		nextp++;
	}
	else
	{
		variant = false;
	}

	/*
	 *	Análise do Endereçamento
	 */
	if (nadds == 2 && txt_inorder (add1, add2) > 0)
	{
		error ("Segundo endereço menor que o primeiro", NOMARK);
		return (false);
	}

	switch (com)
	{
	    case 'a':
	    case 'i':
	    case 'k':
	    case 'r':
	    case '=':
		if (nadds == 0)
		{
			if (com == 'r' || com == '=')
				add1 = Root->ant;
			else
				add1 = Current;
		}
		elif (nadds == 2)
		{
			error ("Endereçamento duplo", NOMARK);
			return (false);
		}

		break;

	    case 'e':	case 'E':
	    case 'f':
	    case 'q':	case 'Q':
	    case 'x':
		if (nadds > 0)
		{
			error ("Endereçamento não permitido", NOMARK);
			return (false);
		}

		break;

	    case ' ':
		if (nadds == 0)
		{
			if ((State == EX_FGLOBAL))
			{
				add1 = add2 = Current;
			}
			elif (Current == Root->ant)
			{
				puts ("Fim do arquivo");
				return (false);
			}
			else
			{
				add1 = add2 = Current->pos;
			}
		}
		elif (nadds == 1)
		{
			add2 = add1;
		}

		break;

	    case 'c':
	    case 'd':
	    case 'l':
	    case 'm':
	    case 'n':
	    case 'p':
	    case 's':
	    case 't':
		if (nadds == 0)
			add1 = add2 = Current;
		elif (nadds == 1)
			add2 = add1;

		break;

	    case 'g':
	    case 'v':
	    case 'w':	case 'W':
		if (nadds == 0)
		{
			add1 = Root;
			add2 = Root->ant;
		}
		elif (nadds == 1)
		{
			add2 = add1;
		}

		break;

	    case 'j':
		if (nadds == 0)
		{
			add1 = Current;
			add2 = Current->pos;
		}
		elif (nadds == 1)
		{
			add2 = add1->pos;
		}

		if (add1 == add2)
		{
			error ("Endereços iguais", NOMARK);
			return (false);
		}

		break;

	    case 'F':
		switch (nadds)
		{
		    case 0:
			add1 = Current;
			add2 = txt_advance (Current, PGLINES);
			break;

		    case 1:
			add2 = txt_advance (add1, PGLINES);
			break;
		}

		goto case_F_B_P;

	    case 'B':
		switch (nadds)
		{
		    case 0:
			add1 = txt_advance (Current, -PGLINES);
			add2 = Current;
			break;

		    case 1:
			add2 = add1;
			add1 = txt_advance (add1, -PGLINES);
			break;
		}

		goto case_F_B_P;

	    case 'P':
		switch (nadds)
		{
		    case 0:
			add1 = txt_advance (Current, -PGLINES/2);
			add2 = txt_advance (add1 == NOBASE ? Root : add1, PGLINES);
			break;

		    case 1:
			add2 = txt_advance (add1, -PGLINES/2);
			add1 = txt_advance (add1,  PGLINES/2);
			break;
		}
		/* continua */

	    case_F_B_P:
		if (add1 == NOBASE)
			add1 = Root;
		if (add2 == NOBASE)
			add2 = Root->ant;
		break;

	}	/* switch (com) { análise do endereçamento } */

	/*
	 *	Análise dos Argumentos
	 */
	if (State == EX_RGLOBAL && com !='m' && com != 't')
		goto execute;

	while (isspace (*nextp))
		nextp++;

	switch (com)
	{
	    case 's':
		if (isdigit (*nextp))
		{
			argi = *nextp - '0';
			nextp++;
		}
		else
		{
			argi = 0;
		}

		while (isspace (*nextp))
			nextp++;

		if (ispunct (*nextp))
		{
			delim = *nextp++;

			if ((args = get_string (delim)) == NULL)
			{
				error (error_msg, LONGMARK);
				return (false);
			}

			if (*args)
			{
				if(!re_compile (args, Subst))
				{
					error (error_msg, LONGMARK);
					return (false);
				}
			}
			elif (Subst[0] == '\0' || (State == EX_FGLOBAL))
			{
				strcpy (Subst, Search);
			}

			if ((args = get_string (delim)) == NULL)
			{
				error (error_msg, LONGMARK);
				return (false);
			}

			sub_compile (args, &Replacement);
		}

		if (Subst[0] == '\0')
		{
			error ("Expressão regular para substituição indefinida", NOMARK);
			return (false);
		}

		print = !(State == EX_FGLOBAL);

		if (*nextp == 'p')
		{
			print = true;
		}
		elif (*nextp && *nextp != 'g')
		{
			error ("Sufixo inexistente", SHORTMARK);
			return (false);
		}

		break;

	    case 'd':
		print = *nextp == 'p';
		break;

	    case 'e':	case 'E':
	    case 'f':
	    case 'r':
	    case 'w':	case 'W':
		if ((args = get_name ()) == NULL)
		{
			if (Text.t_name[0] != '\0')
			{
				args = Text.t_name;
			}
			else
			{
				puts ("Nome do arquivo em edição ainda indefinido");
				return (false);
			}
		}
		elif (Text.t_name[0] == '\0')
		{
			strncpy (Text.t_name, args, NSIZE);
		}
		break;

	    case 'm':
	    case 't':
		if ((add3 = get_address (Current)) == NOBASE)
		{
			if (error_msg)
				error (error_msg, LONGMARK);
			else
				error ("Endereço esperado", SHORTMARK);
			return (false);

		}

		if (txt_inorder (add3, add1) >= 0 && txt_inorder (add3, add2) <= 0)
		{
			error ("Endereço dentro do endereçamento anterior", LONGMARK);
			return (false);
		}

		break;

	    case 'g':
	    case 'v':
		if (new_global = ispunct (*nextp))
		{
			delim = *nextp++;

			if ((args = get_string (delim)) == NULL)
			{
				error (error_msg, LONGMARK);
				return (false);
			}

			if (*args)
			{
				if(!re_compile (args, Search))
				{
					error (error_msg, LONGMARK);
					return (false);
				}
			}
		}

		if (Search[0] == '\0')
		{
			error ("Expressão regular para busca indefinida", NOMARK);
			return (false);
		}

		break;

	    case 'j':
		print = !(State == EX_FGLOBAL);

		for (/* vazio */; *nextp; nextp++)
			print = print || *nextp == 'p';

		break;

	    case 'k':
		if ((argi = get_lower ()) == '\0')
		{
			error (error_msg, LONGMARK);
			return (false);
		}

		break;

	}	/* switch (com) { análise dos argumentos } */

	/*
	 *	Execução do Comando
	 */
    execute:
	switch (com)
	{
	    case 's':
		if (!c_substitute (argi, print))
			return (false);

		break;

	    case 'd':
		c_delete (add1, add2, print);
		break;

	    case 'c':
		add3 = (add1 != Root) ? add1->ant : NOBASE;
		c_delete (add1, add2, false);

		if (add3 != NOBASE)
			c_append (add3);
		else
			c_insert (Root);

		break;

	    case 'a':
		if ((State == EX_FGLOBAL))
		{
			error ("Entrada de texto global ainda não implementada", NOMARK);
			return (false);
		}

		if (!c_append (add1))
			return (false);

		break;

	    case 'i':
		if ((State == EX_FGLOBAL))
		{
			error ("Entrada de texto global ainda não implementada", NOMARK);
			return (false);
		}

		if (!c_insert (add1))
			return (false);
		break;

	    case 'j':
		if (!c_join (print))
			return (false);
		break;

	    case 'f':
		strncpy (Text.t_name, args, NSIZE);
		printf (M_size, Text.t_name, Text.t_size.s_nl,Text.t_size.s_nc);
		putchar ('\n');
		break;

	    case 'g':
	    case 'v':
		if (!c_global (new_global, com == 'g'))
			return (false);
		break;

	    case 'k':
		mrk_define (argi, add1);
		break;

	    case 'm':
		Current = (add3->pos != Root) ? add3->pos : NOBASE;
		txt_move (add1, add2, add3);
		Current = (Current != NOBASE) ? Current : Root->ant;
		Text.t_flag |= T_CHANGED;
		break;

	    case 't':
		Current = (add3->pos != Root) ? add3->pos : NOBASE;
		txt_size (add1, add2, &size);

		if (!txt_copy (add1, add2, add3))
			return (false);

		Current = (Current != NOBASE) ? Current : Root->ant;
		Text.t_size.s_nl += size.s_nl;
		Text.t_size.s_nc += size.s_nc;
		Text.t_flag |= T_CHANGED;
		break;

	    case 'r':
		add2 = add1->pos;

		if (txt_read (args, add1, &size) == IO_OK)
		{
			Text.t_size.s_nl += size.s_nl;
			Text.t_size.s_nc += size.s_nc;
			Text.t_flag |= T_CHANGED;
			printf (M_size, args, size.s_nl, size.s_nc);
		}
		else
		{
			printf (M_txtio, args);
		}

		putchar ('\n');
		Current = add2;
		break;

	    case 'w':
		if (!variant && streq (args, Text.t_name) && (Text.t_flag & T_CHANGED) == 0)
		{
			printf (M_saved, args);
		}
		elif (txt_write (args, add1, add2, false, !variant) == IO_OK)
		{
			if (add1 == Root && add2 == Root->ant)
			{
				Text.t_flag &= ~T_CHANGED;
				memmove (&size, &Text.t_size, sizeof (SIZE));
			}
			else
			{
				txt_size (add1, add2, &size);
			}
			printf (M_size, args, size.s_nl, size.s_nc);
		}
		else
		{
			printf (M_txtio, args);
		}

		putchar ('\n');
		break;

	    case 'W':
		if (txt_write (args, add1, add2, true, false) == IO_OK)
		{
			if (add1 == Root && add2 == Root->ant)
				memmove (&size, &Text.t_size, sizeof (SIZE));
			else
				txt_size (add1, add2, &size);

			printf (M_size, args, size.s_nl, size.s_nc);
		}
		else
		{
			printf (M_txtio, args);
		}

		putchar ('\n');
		break;

	    case 'l':
		c_list (add1, add2);
		Current = add2;
		break;

	    case ' ':
	    case 'p':
	    case 'F':
		c_print (add1, add2);
		Current = add2;
		break;

	    case 'P':
		c_print (add1, add2);
		break;

	    case 'B':
		c_print (add1, add2);
		Current = add1;
		break;

	    case 'n':
		for (Current = add1; Current != add2; Current = Current->pos)
			printf ("%d:\t%s\n", txt_order (Current), Current->ch);

		printf ("%d:\t%s\n", txt_order (Current), Current->ch);
		break;

	    case 'e':
		if (!variant && Text.t_flag & T_CHANGED)
		{
			printf ("\"%s\" ainda não foi atualizado\n", Text.t_name);
			break;
		}
		/* continua */

	    case 'E':
		c_edit (args);
		Current = Root;
		break;

	    case 'q':
		if (!variant && Text.t_flag & T_CHANGED)
		{
			printf ("\"%s\" ainda não foi atualizado\n", Text.t_name);
			break;
		}
		/* continua */

	    case 'Q':
		c_quit ();
		break;

	    case '!':
		if (!variant)
			strcpy (System, nextp);

		system (System);
		break;

	    case '=':
		printf ("%d\n", txt_order (add1));
		break;

	    case 'u':
		puts ("Comando ainda NÃO implementado");
		break;

	    default:
		puts ("Comando inexistente");
		break;

	}	/* end switch (com) { execução do comando } */

	return (true);

}	/* end command */

/*
 ****************************************************************
 *	Comando Global						*
 ****************************************************************
 */
bool
c_global (bool new_list, int normal)
{
	/*
	 *	normal: lista consiste de linhas onde ocorre a exp. reg.
	 *
	 *	Valores para new_list:
	 *		-1  -> liberar lista
	 *		 0  -> reutilizar lista atual
	 *		 1  -> utilizar nova lista
	 */
	BASE		*next, *add;
	bool		again;
	char		*lp;
	bool		found;

	/*
	 *	Pilha com linhas onde ocorre a Exp. Reg
	 */
#	define	N_GLOBAL 200

	static	BASE	*Stack[N_GLOBAL];
	static	BASE	**stack = Stack;
	static	BASE	**first = &Stack[0];

	BASE	**top;
	BASE		**limit;
	int		Size;
	int		area;

	/*
	 *	Examina o estado
	 */
	if ((State == EX_FGLOBAL))
	{
		error ("Não são permitidos comandos globais simultaneos", NOMARK);
		return (false);
	}

	/*
	 *	Guarda "nextp" atual
	 */
	lp = nextp;

	if (new_list == 0)
		goto execute;

	/*
	 *	Empilha todas as linhas com Search
	 */
	if (stack != (BASE **) 0 && stack != Stack)
		free (stack);

	stack = Stack;
	top = &stack[0];
	first = &stack[0];
	Size = N_GLOBAL;
	limit = stack + Size;

	if (new_list == -1)
		return (true);

	for (add = add2, again = true; again; add = add->ant)
	{
		again = add != add1;

		found = re_execute (Search, add->ch, add->ch) != NULL;

		if ((!found && normal) || (found && !normal))
			continue;

		if (top >= limit)
		{
			area = (Size + N_GLOBAL)* sizeof (BASE *);

			if (stack == Stack)
				stack = (BASE **) 0;

			if ((stack = (BASE **)realloc (stack, area)) != (BASE **) 0)
			{
				if (Size == N_GLOBAL)
					memmove (stack, Stack, N_GLOBAL * sizeof(BASE *));

				top = stack + Size;
				Size += N_GLOBAL;
				limit = stack + Size;
			}
			else
			{
				error ("Falta memória para global", NOMARK);
				return (false);
			}
		}

		*top++ = add;
	}

	first = top;

	/*
	 *	Executa command() para cada uma das linhas empilhadas
	 *	ainda existente
	 */
    execute:
	top = first;

	if (top == stack)
		return (true);
	else
		State = EX_FGLOBAL;

	Current = add1;

	while (top > stack)
	{
		next = *--top;

		if (Current == next)
		{
			add = Current;
		}
		elif (next != NOBASE)
		{
			for (add = Current->pos; add != next; add = add->pos)
			{
				if (add == Current)
					break;
			}

			if (add != next)
				{ *top = NOBASE; continue; }
		}
		else
		{
			continue;
		}

		nextp = lp; Current = add;

		if (!command ())
		{
			State = EX_NORMAL;
			return (false);
		}

		State = EX_RGLOBAL;
	}

	State = EX_NORMAL;

	return (true);

}	/* end c_global */

/*
 ****************************************************************
 *	Comando Substitute					*
 ****************************************************************
 */
bool
c_substitute (int order, bool print)
{
	BASE		*add;
	char		*start;
	const char	*next;
	int		count;
	bool		again;
	bool		advance;

	txt_size (add1, add2, &size);

	for (add = add1, again = true; again; add = add->pos)
	{
		again = add != add2;

		next = add->ch; count = 0;

		while ((start = (char *)re_execute (Subst, add->ch, next)) != NULL)
		{
			advance = _end_match == next && *next; count++;

			if (order == 0 || count == order)
			{
				if ((next = sub_execute (add, start, _end_match, &Replacement)) == NULL)
				{
					error ("Falta memória para substituição", NOMARK);
					return (false);
				}
			}
			else
			{
				if (count > order)
					break;

				next = _end_match;
			}

			if (advance)
				next++;

			/*
			 *	Substitui ao final da linha apenas uma vez
			 */
			if (*next == '\0')
				break;
		}

		if (print && count > 0 && count >= order)
			ln_print (add->ch);
	}

	/*
	 *	Atualiza o tamanho do Texto
	 */
	txt_size (add1, add2, &newsize);
	Text.t_size.s_nl += newsize.s_nl - size.s_nl;
	Text.t_size.s_nc += newsize.s_nc - size.s_nc;
	Text.t_flag |= T_CHANGED;

	Current = add2;

	return (true);

}	/* end c_substitute */

/*
 ****************************************************************
 *	Comando Edit						*
 ****************************************************************
 */
void
c_edit (const char *name)
{
	/*
	 *	Libera o Texto atual
	 */
	if (Root != NOBASE)
		{ txt_free (Root); Root = NOBASE; }

	/*
	 *	Atualiza o Nome corrente
	 */
	strncpy (Text.t_name, name, NSIZE);

	/*
	 *	Le o Texto novo
	 */
	if (name[0] != '\0')
	{
		switch (txt_read (Text.t_name, Root, &Text.t_size))
		{
		    case IO_OK:
			printf (M_size, Text.t_name, Text.t_size.s_nl, Text.t_size.s_nc);
			break;

		    case IO_NFOUND:
			txt_init ();
			Text.t_size.s_nl = 1;
			Text.t_size.s_nc = 1;
			printf (M_newfile, Text.t_name);
			break;

		    case IO_EMPTY:
			txt_init ();
			Text.t_size.s_nl = 1;
			Text.t_size.s_nc = 1;
			printf (M_txtio, Text.t_name);
			break;

		    default:
			txt_init ();
			Text.t_size.s_nl = 1;
			Text.t_size.s_nc = 1;
			printf (M_txtio, Text.t_name);
			break;

		}  /* switch txt_read */

		putchar ('\n');

	}
	else	/* name[0] == '\0' */
	{
		txt_init ();
	}

	/*
	 *	Libera a lista Global
	 */
	c_global (-1, true);

}	/* end c_edit */

/*
 ****************************************************************
 *	Comando Join						*
 ****************************************************************
 */
bool
c_join (bool print)
{
	txt_size (add1, add2, &size);

	if (!txt_join (add1, add2))
	{
		error ("Falta memória para join", NOMARK);
		return (false);
	}

	/*
	 *	Atualiza tamanho do Texto
	 */
	txt_size (add2, add2, &newsize);
	Text.t_size.s_nl += newsize.s_nl - size.s_nl;
	Text.t_size.s_nc += newsize.s_nc - size.s_nc;
	Text.t_flag |= T_CHANGED;

	Current = add2;

	if (print)
		ln_print (Current->ch);

	return (true);

}	/* end c_join */

/*
 ****************************************************************
 *	Comando Delete: remoção de texto entre add1 e add2.	*
 ****************************************************************
 */
void
c_delete (BASE *Add1, BASE *Add2, bool print)
{
	Current = (Add2 != Root->ant) ? Add2->pos : NOBASE;

	txt_size (Add1, Add2, &size);
	txt_remove (Add1, Add2);
	Text.t_size.s_nl -= size.s_nl;
	Text.t_size.s_nc -= size.s_nc;

	Current = (Current != NOBASE) ? Current : Root->ant;

	if (print)
		ln_print (Current->ch);

	Text.t_flag |= T_CHANGED;

}	/* end c_delete */

/*
 ****************************************************************
 *	Comando Append: inserção de texto após add.		*
 ****************************************************************
 */
bool
c_append (BASE *add)
{
	BASE		*posterior;

	posterior = (add != Root->ant) ? add->pos : NOBASE;

	if (!append (add))
	{
		error ("Falta memória para a entrada de texto\n", NOMARK);
		return (false);
	}

	Current = (posterior != NOBASE) ? posterior : Root->ant;
	posterior = (posterior != NOBASE) ? posterior : Root;

	/*
	 *	Se algo foi acrescentado, atualiza o tamanho do Texto
	 */
	if (posterior->ant != add)
	{
		txt_size (add->pos, posterior->ant, &size);
		Text.t_size.s_nl += size.s_nl;
		Text.t_size.s_nc += size.s_nc;
		Text.t_flag |= T_CHANGED;
	}

	return (true);

}	/* end c_append */

/*
 ****************************************************************
 *	Comando Insert: inserção de texto antes de add.		*
 ****************************************************************
 */
bool
c_insert (BASE *add)
{
	BASE		*anterior;

	anterior = (add != Root) ? add->ant : NOBASE;

	if (!append (anterior))
	{
		error ("Falta memória para a entrada de texto\n", NOMARK);
		return (false);
	}

	anterior = (anterior != NOBASE) ? anterior : Root->ant;

	/*
	 *	Se algo foi acrescentado, atualiza o tamanho do Texto
	 */
	if (anterior->pos != add)
	{
		txt_size (anterior->pos, add->ant, &size);
		Text.t_size.s_nl += size.s_nl;
		Text.t_size.s_nc += size.s_nc;
		Text.t_flag |= T_CHANGED;
	}

	Current = add;
	return (true);

}	/* end c_insert */

/*
 ****************************************************************
 *	Comando Print: mostra linhas.				*
 ****************************************************************
 */
void
c_print (BASE *first, BASE *last)
{
	while (first != last)
	{
		if (Interruption)
			return;

		ln_print (first->ch);
		first = first->pos;
	}

	ln_print (last->ch);

}	/* end c_print */

/*
 ****************************************************************
 *	Comando List: mostra linhas.				*
 ****************************************************************
 */
void
c_list (BASE *first, BASE *last)
{
	while (first != last)
	{
		if (Interruption)
			return;

		ln_list (first->ch);
		first = first->pos;
	}

	ln_list (last->ch);

}	/* end c_list */

/*
 ****************************************************************
 *	Comando Quit: encerra a edição.				*
 ****************************************************************
 */
void	
c_quit (void)
{
	fflush (stdout);
	exit (0);

}	/* end c_quit */
