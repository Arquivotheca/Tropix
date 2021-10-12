/*
 ****************************************************************
 *								*
 *			com/com.c				*
 *								*
 *	Versão	2.0.0, de 17.08.88				*
 *		3.0.0, de 23.03.95				*
 *								*
 *	Módulo: EX/VI						*
 *		Editores de texto				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <string.h>
#include <ctype.h>

#include "../h/global.h"
#include "../h/text.h"
#include "../h/commands.h"
#include "../h/scan.h"
#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Variáveis globais					*
 ****************************************************************
 */
entry WINDOW	*Win;		/* Janela onde ocorre a edição */

/*
 ****************************************************************
 *	Constantes locais					*
 ****************************************************************
 */
#define	PGLINES		20
#define	N_GLOBAL	200

#define	DEL		0xFF
#define	DELCH		'?'

/*
 ****************************************************************
 *	Tipos locais						*
 ****************************************************************
 */
typedef	enum
{
	EX_NORMAL,	/* Normal */
	EX_FGLOBAL,	/* Primeiro Global */
	EX_RGLOBAL,	/* Repetição Global */
	EX_ERROR	/* Houve Erro: não implementado */

}	STAT;

/*
 ****************************************************************
 *	Variáveis locais					*
 ****************************************************************
 */
static bool	Active;		/* Modo Comandos em Atividade */
static bool	Quit;		/* */
static BASE	*Current;	/* Posição Corrente */

static BASE	*add1;
static BASE	*add2;

static STAT	State = EX_NORMAL;

/*
 ****************************************************************
 *	Mensagens						*
 ****************************************************************
 */
static const char	Size[] = "\"%s\": %d linhas, %d caracteres (%dx%d)";
static const char	Changed[] = "\"%s\" ainda não foi atualizado";

/*
 ******	Protótipos de funções ***********************************
 */
static bool	command (bool);
static bool	c_global (int, bool);
static bool	c_substitute (int, bool);
static void	c_edit (const char *);
static bool	c_join (bool);
static void	c_delete (BASE *, BASE *, bool);
static bool	c_append (BASE *);
static bool	c_insert (BASE *);
static void	c_print (BASE *, BASE *);
static void	c_list (BASE *, BASE *);
static bool	append (BASE *);
static void	list (char *);

/*
 ****************************************************************
 *	Modo "comandos"						*
 ****************************************************************
 */
int
Commands (WINDOW *winp, bool stay)
{
	Win = winp;

	/*
	 *	(Re)inicia a edição
	 */
	if (FIRST () == NOBASE)
	{
		c_edit (Text->t_name);
		Comm->c_cur = FIRST ();

		if (!stay)
			return (COM_NORMAL);
	}

	/*
	 *	Torna a janela visível
	 */
	if (!stay)
		werase (Win);

	touchwin (Win);

	/*
	 *	Reprograma modos de E/S da Janela
	 */
	waddset (Win, A_REAL|A_WRAP|A_SCROLL|A_PAGE);
	wgeton  (Win, G_ECHO|G_EDIT|G_TONL|G_IGXONXOFF|G_SIGRET);
	wgetoff (Win, G_INSERT|G_KEYPAD|G_NODELAY);
	wrefset (Win, R_CURSOR|R_WATCH|R_HOLD);

	/*
	 *	Edição
	 */
	Current = Comm->c_cur;

	Active = true;
	Quit = false;

	do
	{
		/*
		 *	Tratamento de sinais
		 */
		if (Interruption)
		{
			waddstr (Win, "\nInterrupção");
			Interruption = false;
		}

		/*
		 *	Escreve Prompt
		 */
		if (stay)
			waddch (Win, '\n');

		waddch (Win, ':');
		waddch (Win, ' ');
		wrefresh (Win);

		/*
		 *	Lê e executa comando
		 */
		if (scn_command ())
		{
			txt_usave ();
			command (stay);
		}

		/*
		 *	Atualiza a janela do modo de comandos
		 */
		wrefresh (Win);
	}
	while (Active && stay && !Quit);

	/*
	 *	Guarda a posição corrente
	 */
	Comm->c_cur = Current;

	/*
	 *	Tratamento de sinais
	 */
	if (Interruption)
	{
		waddstr (Win, "\nInterrupção");
		Interruption = false;
	}

	wrefoff (Win, R_CURSOR);

	if (Quit)
		return (COM_QUIT);
	else
		return (COM_NORMAL);

}	/* end Commands */

/*
 ****************************************************************
 *	Interpretação do Comando				*
 ****************************************************************
 */
static bool
command (bool stay)
{
	char		com;	/* Comando */
	char		*sargs = "";	/* Argumento cadeia */	/* CUIDADO */
	static char	*args;	/* Argumento cadeia */
	static int	argi;	/* Argumento inteiro */
	BASE		*add3 = NOBASE;				/* CUIDADO */
	int		nadds;
	bool		new_global = false;			/* CUIDADO */
	static bool	print;		/* Sufixo 'p' */
	static bool	variant;	/* Forma variante */
	char		delim;
	SIZE		size;

	if (Interruption)
		return (false);

	/*
	 *	Pega os endereços
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
			elif (error_msg != NOSTR)
			{
				scn_error (error_msg, LONGMARK);
				return (false);
			}
			else
			{
				scn_error
				(
					"Espero segundo endereço",
					SHORTMARK
				);
				return (false);
			}
		}
		else
		{
			nadds = 1;
		}
	}
	elif (error_msg != NOSTR)
	{
		scn_error (error_msg, LONGMARK);
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
		scn_error
		(
			"O segundo endereço se refere a uma linha "
			"anterior ao primeiro endereço",
			NOMARK
		);
		return (false);
	}

	switch (com)
	{
		/*
		 *	Comandos sem endereçamento
		 */
	    case 'e':	case 'E':
	    case 'f':
	    case 'q':
	    case 'x':
		if (nadds > 0)
		{
			scn_error
			(
				"Este comando não admite endereçamento",
				NOMARK
			);
			return (false);
		}
		break;

	    case 'a':
	    case 'i':
	    case 'k':
	    case 'r':
	    case '=':
		if (nadds == 0)
		{
			if (com == '=')
				add1 = LAST ();
			else
				add1 = Current;
		}
		elif (nadds == 2)
		{
			scn_error
			(
				"Este comando não admite endereçamento duplo",
				NOMARK
			);
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
			elif (ISLAST (Current))
			{
				waddstr (Win, "Fim do arquivo");
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
	    case 'w':	case 'W':
		if (nadds == 0)
		{
			add1 = FIRST ();
			add2 = LAST ();
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
			scn_error
			(
			    "Ambos endereços se referem a mesma linha do texto",
			    NOMARK
			);
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
			add2 = txt_advance (add1, PGLINES);
			break;

		    case 1:
			add2 = txt_advance (add1, -PGLINES/2);
			add1 = txt_advance (add1,  PGLINES/2);
			break;
		}

	    case_F_B_P:
		if (add1 == NOBASE)
			add1 = FIRST ();
		if (add2 == NOBASE)
			add2 = LAST ();
		break;

	}	/* switch (com) { analise do endereçamento } */

	/*
	 *	Análise dos Argumentos
	 */
	if (State == EX_RGLOBAL && com !='m' && com != 't')
		goto execute;

	while (isspace (*nextp))
		nextp++;

	switch (com)
	{
		/*
		 *	Dígito
		 */
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

		/*
		 *	Expressões para a substituição
		 */
		while (isspace (*nextp))
			nextp++;

		if (ispunct(*nextp))
		{
			delim = *nextp;
			nextp++;

			if ((sargs = scn_string (delim)) == NOSTR)
			{
				scn_error (error_msg, LONGMARK);
				return (false);
			}

			if (sargs[0])
			{
				if(!re_compile (sargs, &Comm->c_subst))
				{
					scn_error (error_msg, LONGMARK);
					return (false);
				}
			}
			elif (Comm->c_subst == NOSTR)
			{
				Comm->c_subst = strdup (Search);
			}
			elif (State == EX_FGLOBAL)
			{
				free (Comm->c_subst);
				Comm->c_subst = strdup (Search);
			}

			if ((sargs = scn_string (delim)) == NOSTR)
			{
				scn_error (error_msg, LONGMARK);
				return (false);
			}

			sub_compile (sargs, Comm->c_repl);
		}

		if (Comm->c_subst == NOSTR)
		{
			scn_error
			(
			    "Expressão regular para substituição indefinida",
			    NOMARK
			);
			return (false);
		}

		/*
		 *	Sufixos
		 */
		print = (!(State == EX_FGLOBAL)) && stay;

		if (*nextp == 'p')
		{
			print = true;
		}
		elif (*nextp && *nextp != 'g')
		{
			scn_error
			(
				"Este sufixo não é válido para este comando",
				SHORTMARK
			);
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
		if ((sargs = scn_name ()) == NOSTR)
		{
			if (Text->t_name[0])
			{
				sargs = Text->t_name;
			}
			else
			{
				scn_error
				(
					"Nome do arquivo em edição ainda indefinido",
					NOMARK
				);

				return (false);
			}
		}
		elif (Text->t_name[0] == '\0')
		{
			free (Text->t_name);
			Text->t_name = strdup (sargs);
		}
		break;

		/*
		 *	Pega endereço de destino
		 */
	    case 'm':
	    case 't':
		if ((add3 = get_address (Current)) == NOBASE)
		{
			if (error_msg)
			{
				scn_error (error_msg, LONGMARK);
			}
			else
			{
				scn_error
				(
					"Endereço de destino esperado",
					SHORTMARK
				);
			}

			return (false);
		}

		/*
		 *	Verifica o endereço de destino
		 */
		if (txt_inorder (add3, add1) >= 0 && txt_inorder (add3, add2) <= 0)
		{
			scn_error
			(
				"O endereço de destino "
				"está dentro do endereçamento de partida",
				LONGMARK
			);
			return (false);
		}

		break;

		/*
		 *	Pega expressão para busca
		 */
	    case 'g':
		if (new_global = ispunct (*nextp))
		{
			delim = *nextp;
			nextp++;

			if ((sargs = scn_string (delim)) == NOSTR)
			{
				scn_error (error_msg, LONGMARK);
				return (false);
			}

			if (*sargs)
			{
				if (!re_compile (sargs, &Search))
				{
					scn_error (error_msg, LONGMARK);
					return (false);
				}
			}
		}

		if (Search == NOSTR)
		{
			scn_error
			(
				"Expressão regular para busca indefinida",
				NOMARK
			);
			return (false);
		}
		break;

	    case 'j':
		print = !(State == EX_FGLOBAL);

		for (/* vazio */; *nextp; nextp++)
			print = print || *nextp == 'p';

		break;

	    case 'k':
		if ((argi = scn_lower ()) == '\0')
		{
			scn_error (error_msg, LONGMARK);
			return (false);
		}

		break;

	}	/* switch (com) { análise dos argumentos } */

	/*
	 *	Armazena arg. cadeia em uma área alocada
	 */
	if (args != NOSTR)
		free (args);

	args = strdup (sargs);

	/*
	 *	Execução do Comando
	 */
    execute:
	switch (com)
	{
	    case 'u':
		if ((add3 = txt_restore ()) != NOBASE)
		{
			Current = add3;
			c_print (Current, Current);
		}
		else
		{
			scn_error ("O texto ainda não foi alterado", NOMARK);
		}
		break;

	    case 'v':
		Active = false;
		break;

	    case 's':
		if (!c_substitute (argi, print))
			return (false);
		break;

	    case 'd':
		c_delete (add1, add2, print);
		break;

	    case 'c':
		add3 = add1->ant;
		c_delete (add1, add2, false);
		c_append (add3);
		break;

	    case 'a':
		if ((State == EX_FGLOBAL))
		{
			scn_error
			(
			    "Entrada de texto global ainda não implementada",
			    NOMARK
			);
			return (false);
		}

		if (!c_append (add1))
			return (false);
		break;

	    case 'i':
		if ((State == EX_FGLOBAL))
		{
			scn_error
			(
			    "Entrada de texto global ainda não implementada",
			    NOMARK
			);
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
		if (!streq (Text->t_name, args))
		{
			free (Text->t_name);
			Text->t_name = strdup (args);
		}

		txt_size (FIRST (), LAST (), &size);
		wprintw (Win, Size, Text->t_name, size.s_nl, size.s_nc, COLS, LINES);

		if (check (Text->t_flags, T_RDONLY))
			waddstr (Win, " (só LEITURA)");

		if (dos_mode)
			waddstr (Win, " (estilo DOS)");

		if (check (Text->t_flags, T_CHANGED))
			waddstr (Win, " (não atualizado)");
		break;

	    case 'g':
		if (!c_global (new_global, !variant))
			return (false);
		break;

	    case 'k':
		mrk_define ((char)argi, add1, 0);
		break;

	    case 'm':
		txt_lsave (add1->ant, add2->pos);
		txt_fsave (add3);
		Current = add3->pos;
		txt_move (add1, add2, add3);
		break;

	    case 't':
		txt_fsave (add3);
		Current = add3->pos;

		if (!txt_copy (add1, add2, add3))
			return (false);
		break;

	    case 'r':
		txt_fsave (add1);
		Current = add1;

		if (txt_read (args, add1, &size) == IO_OK)
			wprintw (Win, Size, args, size.s_nl, size.s_nc, COLS, LINES);
		else
			wprintw (Win, M_txtio, args);
		break;

	    case 'w':
		if (!variant && streq (args, Text->t_name) && !check (Text->t_flags, T_CHANGED))
		{
			wprintw (Win, "\"%s\" já está atualizado", args);
		}
		elif (txt_write (args, add1, add2, false, !variant) == IO_OK)
		{
			if (ISFIRST (add1) && ISLAST (add2))
				reset (Text->t_flags, T_CHANGED);

			txt_size (add1, add2, &size);
			wprintw (Win, Size, args, size.s_nl, size.s_nc, COLS, LINES);
		}
		else
		{
			wprintw (Win, M_txtio, args);
		}
		break;

	    case 'W':
		if (txt_write (args, add1, add2, true, false) == IO_OK)
		{
			txt_size (add1, add2, &size);
			wprintw (Win, Size, args, size.s_nl, size.s_nc, COLS, LINES);
		}
		else
		{
			wprintw (Win, M_txtio, args);
		}
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
		for (Current = add1; /* abaixo */; Current = Current->pos)
		{
			wprintw
			(
				Win,
				"%d:\t%s",
				txt_order (Current),
				Current->ch
			);

			if (Current != add2)
				waddch (Win, '\n');
			else
				break;
		}
		break;

	    case 'e':
		if (!variant && check (Text->t_flags, T_CHANGED))
		{
			wprintw (Win, Changed, Text->t_name);
			break;
		}
	    case 'E':
		c_edit (args);
		Current = FIRST ();
		break;

	    case 'q':
		if (!variant && check (Text->t_flags, T_CHANGED))
		{
			wprintw (Win, Changed, Text->t_name);
			break;
		}

		Quit = true;
		break;

	    case '!':
		if (!variant)
		{
			System = (char *)realloc (System, strlen (nextp) + 1);
			strcpy (System, nextp);
		}

		outcurses ();
		system (System);
		incurses (NOSTR);
		touchwin (Win);
		break;

	    case '=':
		wprintw (Win, "%d", txt_order (add1));
		break;

	    default:
		waddstr (Win, "Comando inexistente");
		break;

	}	/* end switch (com) { execução do comando } */

	return (true);

}	/* end command */

/*
 ****************************************************************
 *	Comando Global						*
 ****************************************************************
 */
static bool
c_global (int new_list, bool normal)
{
	/*
	 *	normal == true:	A lista consiste de linhas onde ocorre a exp. reg
	 *
	 *	Valores para new_list:
	 *		-1  -> liberar lista
	 *		 0  -> reutilizar lista atual
	 *		 1  -> utilizar nova lista
	 */

	BASE		*next, *add;
	bool		again;
	bool		found;
	char		*lp;

	/*
	 *	Pilha com linhas onde ocorre a Exp. Reg
	 */
	static	BASE	**stack;
	static	BASE	**first;

	BASE	**top,
			**limit;
	int		size;
	int		area;

	if ((State == EX_FGLOBAL))
	{
		scn_error
		(
			"Não são permitidos comandos globais simultâneos",
			NOMARK
		);
		return (false);
	}

	/*
	 *	Guarda "nextp" atual
	 */
	lp = nextp;

	if (new_list == 0)
	{
		goto execute;
	}
	elif (new_list == -1)
	{
		if (stack != (BASE **)0)
			free ((char *) stack);

		stack = (BASE **) 0;

		return (true);
	}

	/*
	 *	Aloca uma nova pilha
	 */
	if ((stack = realloc (stack, N_GLOBAL * sizeof (BASE *))) == (BASE **)0)
	{
		scn_error ("Falta memória para global", NOMARK);
		stack = (BASE **)0;
		return (false);
	}
	else
	{
		top = &stack[0];
		first = &stack[0];
		size = N_GLOBAL;
		limit = stack + size;
	}

	/*
	 *	Empilha todas as linhas com "search"
	 */
	for (add = add2, again = true; again; add = add->ant)
	{
		again = add != add1;

		found = (re_execute (Search, add->ch, add->ch) >= 0);

		if ((!found && normal) || (found && !normal))
			continue;

		if (top >= limit)
		{
			area = (size + N_GLOBAL) * sizeof (BASE *);

			if ((stack = realloc (stack, area)) != (BASE **) 0)
			{
				top = stack + size;
				size += N_GLOBAL;
				limit = stack + size;
			}
			else
			{
				scn_error ("Falta memória para global", NOMARK);
				return (false);
			}
		}

		*top++ = add;
	}

	first = top;

	/*
	 *	Executa command () para cada uma das linhas empilhadas
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
			add = Current;

		elif (next != NOBASE)
		{
			for (add = Current->pos; add != next; add = add->pos)
			{
				if (add == Current)
					break;
			}

			if (add != next)
			{
				*top = NOBASE;
				continue;
			}
		}
		else
		{
			continue;
		}

		nextp = lp;
		Current = add;

		if (!command (false))
		{
			State = EX_NORMAL;
			return (false);
		}

		State = EX_RGLOBAL;

		waddch (Win, '\n');
	}

	State = EX_NORMAL;

	return (true);

}	/* end c_global */

/*
 ****************************************************************
 *	Comando "substitute"					*
 ****************************************************************
 */
static bool
c_substitute (int order, bool print)
{
	BASE		*add;
	int		start;
	char		*next;
	int		count;
	bool		again;
	bool		advance;
	bool		saved;

	for (add = add1, again = true; again; add = add->pos)
	{
		again = add != add2;

		next = add->ch;
		count = 0;
		saved = false;

		while ((start = re_execute (Comm->c_subst, add->ch, next)) >= 0)
		{
			advance = _end_match == next && *next;

			count++;

			if (order == 0 || count == order)
			{
				/*
				 *	Guarda a linha original
				 */
				if (!saved)
				{
					txt_lsave (add->ant, add->pos);
					saved = true;
				}

				/*
				 *	Realiza substituição
				 */
				if ((next = sub_execute (add, &add->ch[start], _end_match, Comm->c_repl)) == NOSTR)
				{
					scn_error
					(
					    "Falta memória para a substituição",
					    NOMARK
					);

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
		{
			waddstr (Win, add->ch);
			waddch (Win, '\n');
		}
	}

	Current = add2;

	return (true);

}	/* end c_substitute */

/*
 ****************************************************************
 *	Comando "edit"						*
 ****************************************************************
 */
static void
c_edit (const char *name)
{
	SIZE		size;

	if (Debug)
		fprintf (stderr, "Editando \"%s\"\n", name);

	/*
	 *	Libera Texto atual
	 */
	if (FIRST () != NOBASE)
		txt_free (FIRST (), LAST());

	/*
	 *	Atualiza Nome corrente
	 */
	free (Text->t_name);
	Text->t_name = strdup (name);

	/*
	 *	Leitura  do novo Texto
	 */
	if (name[0] != '\0')
	{
		switch (txt_read (Text->t_name, NOBASE, &size))
		{
		    case IO_OK:
			wprintw (Win, Size, Text->t_name, size.s_nl, size.s_nc, COLS, LINES);
			break;

		    case IO_NFOUND:
			txt_init ();
			wprintw (Win, "\"%s\": arquivo novo", Text->t_name);
			break;

		    case IO_EMPTY:
			txt_init ();
			wprintw (Win, M_txtio, Text->t_name);
			break;

		    default:
			txt_init ();
			wprintw (Win, M_txtio, Text->t_name);
			break;

		}  /* switch txt_read */

	}
	else 	/* name[0] == '\0' */
	{
		txt_init ();
	}

	/*
	 *	Libera a lista global
	 */
	c_global (-1, true);

	/*
	 *	Libera Alterações
	 */
	txt_usave ();

	reset (Text->t_flags, T_CHANGED|T_EDITION);
	set (Text->t_flags, T_NEW);

}	/* end c_edit */

/*
 ****************************************************************
 *	Comando "join"						*
 ****************************************************************
 */
static bool
c_join (bool print)
{
	txt_lsave (add1->ant, add2->pos);

	if (txt_join (add1, add2, 0) < 0)
	{
		scn_error ("Falta memória para join", NOMARK);
		return (false);
	}

	Current = add2;

	if (print)
		waddstr (Win, Current->ch);

	return (true);

}	/* end c_join */

/*
 ****************************************************************
 *	Comando "delete": remoção de texto entre add1 e add2	*
 ****************************************************************
 */
static void
c_delete (BASE *Add1, BASE *Add2, bool print)
{
	bool		last;

	if (!(last = ISLAST (Add2)))
		Current = Add2->pos;

	txt_lsave (Add1->ant, Add2->pos);
	txt_remove (Add1, Add2);

	if (last)
		Current = LAST ();

	if (print)
		waddstr (Win, Current->ch);

}	/* end c_delete */

/*
 ****************************************************************
 *	Comando "append": inserção de texto após add		*
 ****************************************************************
 */
static bool
c_append (BASE *add)
{
	BASE		*posterior;

	txt_fsave (add);

	posterior = add->pos;

	if (!append (add))
	{
		scn_error ("Falta memória para entrada de texto", NOMARK);
		return (false);
	}

	Current = posterior;

	return (true);

}	/* end c_append */

/*
 ****************************************************************
 *	Comando "Insert": inserção de texto antes de add	*
 ****************************************************************
 */
static bool
c_insert (BASE *add)
{
	BASE		*anterior;

	txt_bsave (add);

	anterior = add->ant;

	if (!append (anterior))
	{
		scn_error ("Falta memória para entrada de texto", NOMARK);
		return (false);
	}

	Current = add;
	return (true);

}	/* end c_insert */

/*
 ****************************************************************
 *	Comando "print": mostra linhas				*
 ****************************************************************
 */
static void
c_print (BASE *first, BASE *last)
{
	while (first != last)
	{
		if (Interruption)
			return;

		waddstr (Win, first->ch);
		waddch (Win, '\n');

		first = first->pos;
	}

	waddstr (Win, last->ch);

}	/* end c_print */

/*
 ****************************************************************
 *	Comando "list": mostra linhas				*
 ****************************************************************
 */
static void
c_list (BASE *first, BASE *last)
{
	while (first != last)
	{
		if (Interruption)
			return;

		list (first->ch);
		first = first->pos;
	}

	list (last->ch);

}	/* end c_list */

/*
 ****************************************************************
 *	Comand "append"						*
 ****************************************************************
 */
static bool
append (BASE *anterior)
{
	BASE		*base;
	char		line[512];

	base = anterior;

	for (EVER)
	{
		wgetstr (Win, line);

		if (streq (line, ".") || Interruption)
			break;

		if (!txt_insert (base, line))
			return (false);

		base = base->pos;
	}

	return (true);

}	/* end append */

/*
 ****************************************************************
 *	Lista uma linha						*
 ****************************************************************
 */
static void
list (char *line)
{
	unsigned char	c;
	int		col;

	col = 1;

	while ((c = *line++))
	{
		if (isgraph (c) || c == ' ')
		{
			waddch (Win, c);
		}
		elif (c == '\t')
		{
			waddch (Win, '>');
		}
		elif (c == DEL)
		{
			waddch (Win, '^');
			waddch (Win, DELCH);
			col++;
		}
		else
		{
			waddch (Win, '^');
			waddch (Win, c | 0100);
			col++;
		}

		col++;

		if (col >= 72)
		{
			waddch (Win, '\\');
			waddch (Win, '\n');
			col = 0;
		}
	}

	waddch (Win, '$');
	waddch (Win, '\n');

}	/* end list */
