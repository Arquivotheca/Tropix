/*
 ****************************************************************
 *								*
 *			txt/list.c				*
 *								*
 *	Manipulações com Listas					*
 *								*
 *	Versão	1.0.0, de 22.10.86				*
 *		3.0.0, de 27.05.97				*
 *								*
 *	Modulo:	ED						*
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

#include "../h/ed.h"
#include "../h/proto.h"
#include "../h/extern.h"

/*
 ****************************************************************
 *	Compara ordem de duas bases na lista.			*
 ****************************************************************
 */
int	
txt_inorder (BASE *base1, BASE *base2)
{
	BASE		*b;

	if (base1 == base2)
		return (0);

	if (base1 == Root || base2 == Root->ant)
		return (-1);

	for (b = base2->ant; b != base1 && b != Root; b = b->ant)
		/* vazio */;

	return ((b == base1) ? -1 : 1);

}	/* end txt_inorder */

/*
 ****************************************************************
 *	Informa a ordem de uma BASE na lista. 			*
 ****************************************************************
 */
int	
txt_order (BASE *base)
{
	int		order;

	for (order = 1; base != Root; base = base->ant, order++)
		/* vazio */;

	return (order);

}	/* end txt_order */

/*
 ****************************************************************
 *	Informa base na ordem n na lista.			*
 ****************************************************************
 */
BASE *
txt_base (int n)
{
	BASE		*base; 

	base = Root;

	while (--n > 0)
	{
		base = base->pos;

		if (base == Root)
			return (NOBASE);
	}

	return (base);

}	/* end txt_base */

/*
 ****************************************************************
 *	Avanca N vezes  na lista a partir de Base.		*
 *	Retorna base avancada ou NOBASE se passar por Root.	*
 ****************************************************************
 */
BASE *
txt_advance (BASE *base, int n)
{
	while (n > 0  && base->pos != Root)
	{
		base = base->pos;
		n--;
	}

	while (n < 0  &&  base != Root)
	{
		base = base->ant;
		n++;
	}

	return (n == 0 ? base : NOBASE);

}	/* txt_advance */

/*
 ****************************************************************
 *	Tamanho de first até last.				*
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
 *	Libera a Lista.						*
 ****************************************************************
 */
void	
txt_free (BASE *list)
{
	BASE		*b;

	if (list == NOBASE)
		return;

	b = list;

	while (b->pos != list)
	{
		free (b->ch);
		b = b->pos;
		free (b->ant);
	}

	free (b->ch);
	free (b);

}	/* end txt_free */

/*
 ****************************************************************
 *	Movimento de bases.					*
 ****************************************************************
 */
void
txt_move (BASE *first, BASE *last, BASE *anterior)
{
	if (first == Root)
		Root = last->pos;

	/*
	 *	Desliga bases first até last da lista
	 */
	first->ant->pos = last->pos;
	last->pos->ant = first->ant;

	/*
	 *	Liga bases first até last após anterior
	 */
	first->ant = anterior;
	last->pos = anterior->pos;
	first->ant->pos = first;
	last->pos->ant = last;
	
}	/* end txt_move */

/*
 ****************************************************************
 *	Copia de bases.						*
 ****************************************************************
 */
bool
txt_copy (BASE *first, BASE *last, BASE *base)
{
	BASE		*from, *to;
	BASE		*list, *tail = NOBASE;
	bool		again;

	/*
	 *	Copia bases first até last em list
	 */
	list = NOBASE;

	for (from = first, again = true; again; from = from->pos)
	{
		again = from != last;

		if ((to = (BASE *)malloc (sizeof (BASE))) == NOBASE)
			goto No_mem;

		if (list == NOBASE)
			tail = list = to;

		if ((to->ch = malloc (strlen (from->ch) +1)) == NULL)
			goto No_mem;

		strcpy (to->ch, from->ch);

		tail->pos = to; to->ant = tail; tail = to;

	}	/* for from = first up to last */

	/*
	 *	Insere list após base
	 */
	if (base == NOBASE)
	{
		tail->pos = Root;
		list->ant = Root->ant;
		Root = list;
	}
	else
	{
		tail->pos = base->pos;
		list->ant = base;
	}

	base->pos->ant = tail;
	base->pos = list;

	return (true);

	/*
	 *	Se faltar memória libera list e retorna false
	 */
    No_mem:
	if (to != NOBASE)
		free (to);

	if (list != NOBASE)
	{
		tail->pos = list;
		txt_free (list);
	}

	return (false);

}	/* end txt_copy */

#ifdef	TXT_NEW
/*
 ****************************************************************
 *	Criação de N novas linhas após Base			*
 ****************************************************************
 */
bool	
txt_new (int n, BASE *base)
{
	BASE		*first, *last, *b;

	first = NOBASE;

	while (n-- > 0)
	{
		if ((b = (BASE *)malloc (sizeof (BASE))) == NOBASE)
			goto  No_mem;

		if (first == NOBASE)
			last = first = b;

		if ((b->ch = malloc(1)) == NULL)
			goto No_mem;

		b->ch[0] = '\0';

		last->pos = b;
		b->ant = last;

		last = last->pos;

	}	/*  while  n-- > 0  */

	first->ant = base; last->pos = base->pos;

	base->pos = first; last->pos->ant = last;

	return (true);

	/*
	 *	Se não conseguir memória, ...
	 */
   No_mem:
	if (b != NOBASE)
		free (b);

	if (last != NOBASE)
	{
		last->pos = first;
		txt_free (first);
	}

	return (false);

}	/* end txt_new */
#endif	TXT_NEW

/*
 ****************************************************************
 *	Inserção de uma linha na lista.				*
 ****************************************************************
 */
bool
txt_insert (BASE *anterior, const char *line)
{
	BASE		*base;

	/*
	 *	Aloca memória para base e base->ch
	 */
	if ((base = (BASE *)malloc (sizeof (BASE))) == NOBASE)
		return (false);

	if ((base->ch = malloc (strlen (line) + 1)) == NULL)
	{
		free (base);
		return (false);
	}

	/*
	 *	Copia line para base->ch
	 */
	strcpy (base->ch, line);

	/*
	 *	Liga base na lista após anterior
	 */
	if (anterior == NOBASE)
	{
		base->ant = Root->ant;
		base->pos = Root;
		Root = base;
	}
	else
	{
		base->ant = anterior;
		base->pos = anterior->pos;
	}

	base->pos->ant = base;
	base->ant->pos = base;

	return (true);

}	/* end txt_insert */

/*
 ****************************************************************
 *	Junta bases first até last em last			*
 ****************************************************************
 */
bool
txt_join (BASE *first, BASE *last)
{
	BASE		*base;
	int		length;
	bool		again;
	char		*line;

	/*
	 *	Calcula o tamanho da nova linha
	 */
	length = 0;

	for (base = first, again = true; again; base = base->pos)
	{
		again = base != last;
		length += strlen (base->ch);
	}

	/*
	 *	Aloca área para nova linha
	 */
	if ((line = malloc (length + 1)) == NULL)
		return (false);

	/*
	 *	Copia linhas antigas para a nova
	 */
	line[0] = '\0';

	for (base = first, again = true; again; base = base->pos)
	{
		again = base != last;
		strcat (line, base->ch);
	}

	/*
	 *	Substitui last->ch por line
	 */
	free (last->ch);
	last->ch = line;

	/*
	 *	Desliga first até last->ant da lista
	 */
	if (first == Root)
		Root = last;

	last->ant = first->ant;
	last->ant->pos = last;

	/*
	 *	Libera first até last->ant
	 */
	for (base = first; base != last; /* vazio */)
	{
		free (base->ch);
		base = base->pos;
		free (base->ant);
	}

	return (true);

}	/* end txt_join */

/*
 ****************************************************************
 *	Remoção de bases da lista				*
 ****************************************************************
 */
void	
txt_remove (BASE *first, BASE *last)
{
	BASE		*anterior, *posterior;
	int		new_Root;

	new_Root = false;

	/*
	 *	Determina bases anterior e posterior a remoção
	 */
	if (first == Root && last == Root->ant)
	{
		/*
		 *	Remoção de toda a lista
		 */
		anterior = NOBASE;
		posterior = NOBASE;
	}
	else
	{
		new_Root = first == Root;

		anterior = first->ant;
		posterior = last->pos;
	}

	/*
	 *	Libera área antes ocupada pelas linhas removidas
	 */
	last->pos = first;
	txt_free (first);

	/*
	 *  Religa a lista
	 */
	if (anterior == NOBASE && posterior == NOBASE)
	{
		txt_init ();
	}
	else
	{
		anterior->pos = posterior;
		posterior->ant = anterior;

		if (new_Root)
			Root = posterior;
	}

}	/* end txt_remove */

/*
 ****************************************************************
 *	Inicia Lista						*
 ****************************************************************
 */
void	
txt_init (void)
{
	Root = (BASE *)malloc (sizeof (BASE));
	Root->ant = Root;
	Root->pos = Root;
	Root->ch = malloc (1);
	Root->ch[0] = '\0';

	Text.t_size.s_nl++;
	Text.t_size.s_nc++;

}	/* end txt_init */
