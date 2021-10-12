/*
 ****************************************************************
 *								*
 *			list.c					*
 *								*
 *	Manipulações com listas					*
 *								*
 *	Versão	2.0.0, de 17.08.88				*
 *		3.0.1, de 30.04.97				*
 *								*
 *	Módulo:	EX/VI						*
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
#include <string.h>

#include "../h/global.h"
#include "../h/text.h"
#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Definições globais					*
 ****************************************************************
 */
#define	abs(n)	((n > 0) ? (n) : (-(n)))

/*
 ****************************************************************
 *	Compara a ordem de duas bases na lista			*
 ****************************************************************
 */
int
txt_inorder (BASE *base1, BASE *base2)
{
	/*
	 *	Retorna:	 0 :	base1 e base2 são iguais
	 *			-1 :	base1 é anterior a base2
	 *			 1 :	base1 é posterior a base2
	 */
	BASE		*b;

	if (base1 == base2)
		return (0);

	for (b = base2; b != base1; b = b->ant)
	{
		if (ISFIRST (b))
			return (1);
	}

	return (-1);

}	/* end txt_inorder */

/*
 ****************************************************************
 *	Informa a ordem de uma BASE na lista			*
 ****************************************************************
 */
int	
txt_order (BASE *base)
{
	int		order;

	for (order = 1; !ISFIRST (base); base = base->ant, order++)
		/* vazio */;

	return (order);

}	/* end txt_order */

/*
 ****************************************************************
 *	Informa base na ordem "n" na lista			*
 ****************************************************************
 */
BASE *
txt_base (int n)
{
	BASE		*base;

	base = FIRST ();

	while (--n > 0)
	{
		if (ISLAST (base))
			return (NOBASE);

		base = base->pos;
	}

	return (base);

}	/* end txt_base */

/*
 ****************************************************************
 *	Avanca N vezes  na lista a partir de Base		*
 *	Retorna base avancada ou NOBASE se passar por Root	*
 ****************************************************************
 */
BASE *
txt_advance (BASE *base, int	n)
{
	while (n > 0 && !ISLAST (base))
	{
		base = base->pos;
		n--;
	}

	while (n < 0 && !ISFIRST (base))
	{
		base = base->ant;
		n++;
	}

	return (n == 0 ? base : NOBASE);

}	/* txt_advance */

/*
 ****************************************************************
 *	Avanca N vezes na lista a partir de "base"		*
 ****************************************************************
 */
BASE *
txt_walk (BASE *base, int n)
{
	while (n > 0 && !ISLAST (base))
	{
		base = base->pos;
		n--;
	}

	while (n < 0 && !ISFIRST (base))
	{
		base = base->ant;
		n++;
	}

	return (base);

}	/* txt_walk */

/*
 ****************************************************************
 *	Tamanho de "first" até "last"				*
 ****************************************************************
 */
void	
txt_size (BASE *first, BASE *last, SIZE *size)
{
	BASE		*b;
	int		nlines;
	int		nch;

	nlines = 0; nch = 0;

	for (b = first; b != last; b = b->pos)
	{
		nch += strlen (b->ch);
		nlines++;
	}

	nch += strlen (b->ch);
	nlines++;

	size->s_nc = nch + nlines;
	size->s_nl = nlines;

}	/* end txt_size */

/*
 ****************************************************************
 *	Libera a lista						*
 ****************************************************************
 */
void	
txt_free (BASE *first, BASE *last)
{
	BASE		*base, *next;

	if (Debug)
	{
		fprintf
		(	stderr,
			"txt_free: liberando de \"%s\" até \"%s\"\n",
			first->ch, last->ch
		);
	}

	for (base = first; /* teste abaixo */; base = next)
	{
		next = base->pos;
		free (base->ch);
		free ((char *)base);

		/*
		 *	Teste de terminação
		 */
		if (base == last)
			break;
	}

}	/* end txt_free */

/*
 ****************************************************************
 *	Movimento de bases					*
 ****************************************************************
 */
void
txt_move (BASE *first, BASE *last, BASE *anterior)
{
	/*
	 *	Desliga bases "first" até "last" da lista
	 */
	UNLINK (first, last);

	/*
	 *	Liga bases "first" até "last" após anterior
	 */
	LINK (anterior, first, last);
	
}	/* end txt_move */

/*
 ****************************************************************
 *	Cópia de bases						*
 ****************************************************************
 */
bool
txt_copy (BASE *first, BASE *last, BASE *base)
{
	BASE		*from, *to;
	BASE		*list, *tail = NOBASE;	/* CUIDADO */
	bool		again;

	/*
	 *	Copia bases "first" até "last" em list
	 */
	list = NOBASE;

	for (from = first, again = true; again; from = from->pos)
	{
		again = from != last;

		if ((to = malloc (sizeof (BASE))) == NOBASE)
			goto No_mem;

		if (list == NOBASE)
			tail = list = to;

		if ((to->ch = malloc (strlen (from->ch) +1)) == NOSTR)
			goto No_mem;

		strcpy (to->ch, from->ch);

		FLINK (tail, to);;

		tail = to;

	}	/* for from = first up to last */

	/*
	 *	Insere list após base
	 */
	LINK (base, list, tail);

	return (true);

	/*
	 *	Se faltar memória libera list e retorna false
	 */
    No_mem:
	if (to != NOBASE)
		free (to);

	if (list != NOBASE)
		txt_free (list, tail);

	return (false);

}	/* end txt_copy */

/*
 ****************************************************************
 *	Criação de N novas linhas				*
 ****************************************************************
 */
bool	
txt_new (BASE *base, int count)
{
	BASE		*first, *last = NOBASE, *b;	/* CUIDADO */
	int		i;

	first = NOBASE;

	for (i = abs (count); i > 0; i--)
	{
		if ((b = malloc(sizeof (BASE))) == NOBASE)
			goto  No_mem;

		if (first == NOBASE)
			last = first = b;

		if ((b->ch = malloc(1)) == NOSTR)
			goto No_mem;

		b->ch[0] = '\0';

		FLINK (last, b);

		last = last->pos;

	}	/* end (for i > 0) */

	if (count > 0)
	{
		LINK (base, first, last);
	}
	else	/* count < 0 */
	{
		LINK (base->ant, first, last);
	}

	return (true);


    No_mem:
	if (b != NOBASE)
		free (b);

	if (last != NOBASE)
		txt_free (first, last);

	return (false);

}	/* end txt_new */

/*
 ****************************************************************
 *	Inserção de uma linha na lista				*
 ****************************************************************
 */
bool
txt_insert (BASE *anterior, char *line)
{
	BASE		*base;

	/*
	 *	Aloca memória para base e base->ch
	 */
	if (line == NOSTR)
		line = "";

	if ((base = malloc (sizeof (BASE))) == NOBASE)
		return (false);

	if ((base->ch = malloc (strlen (line) + 1)) == NOSTR)
		{ free (base); return (false); }

	/*
	 *	Copia line para base->ch
	 */
	strcpy (base->ch, line);

	/*
	 *	Liga base na lista após anterior
	 */
	LINK (anterior, base, base);

	return (true);

}	/* end txt_insert */

/*
 ****************************************************************
 *	Junta bases "first" até "last" em last			*
 ****************************************************************
 */
int
txt_join (BASE *first, BASE *last, int space)
{
	BASE		*base, *next;
	int		length;
	bool		again;
	char		*line;
	int		i;
	int		ret;

	/*
	 *	Valor de retorno
	 */
	ret = strlen (first->ch);

	/*
	 *	Calcula tamanho da nova linha
	 */
	length = 0;

	for (base = first, again = true; again; base = base->pos)
	{
		again = (base != last);
		length += strlen (base->ch) + space;
	}

	/*
	 *	Aloca área para nova linha
	 */
	if ((line = malloc (length + 1)) == NOSTR)
		return (-1);

	/*
	 *	Copia linhas antigas para a nova
	 */
	line[0] = '\0';

	for (base = first, again = true; again; base = base->pos)
	{
		again = (base != last);

		strcat (line, base->ch);

		if ((space > 0) && again)
		{
			for (i = 0; i < space; i++)
				strcat (line, " ");
		}
	}

	/*
	 *	Substitui "last->ch" por "line"
	 */
	free (last->ch);
	last->ch = line;

	/*
	 *	Desliga "first" até "last->ant" da lista
	 */
	UNLINK (first, last->ant);

	/*
	 *	Libera first até (antigo) last->ant
	 */
	for (base = first; base != last; base = next)
	{
		next = base->pos;
		free (base->ch);
		free (base);
	}

	return (ret);

}	/* end txt_join */

/*
 ****************************************************************
 *	Remoção de bases da lista				*
 ****************************************************************
 */
void	
txt_remove (BASE *first, BASE *last)
{
	/*
	 *	Retira as linhas da Lista
	 */
	if (ISFIRST (first) && ISLAST (last))
	{
		txt_init ();
	}
	else
	{
		UNLINK (first, last);
	}

	/*
	 *	Libera área de memória
	 */
	txt_free (first, last);

}	/* end txt_remove */


/*
 ****************************************************************
 *	Inicia Lista						*
 ****************************************************************
 */
void	
txt_init (void)
{
	BASE		*base;

	base = malloc (sizeof (BASE));
	base->ch = malloc (1);
	base->ch[0] = '\0';

	NEWFIRST (base);
	NEWLAST (base);

}	/* end txt_init */
