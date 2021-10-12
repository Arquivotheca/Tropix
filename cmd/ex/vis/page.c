/*
 ****************************************************************
 *								*
 *			page.c					*
 *								*
 *	Manipulação da página					*
 *								*
 *	Versão	2.0.0, de 17.08.88				*
 *		3.0.1, de 30.04.97				*
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

#include "../h/global.h"
#include "../h/text.h"
#include "../h/visual.h"
#include "../h/extern.h"
#include "../h/proto.h"

/*
 ******	Protótipos de funções ***********************************
 */
static int	disp (BASE *);
static void	newmap (void);
static void	down (int, BASE *, int, int);
static void	up (int, BASE *, int, int);
static void	fill (void);

/*
 ****************************************************************
 *	Verifica a presença de uma linha na janela		*
 ****************************************************************
 */
int
pg_present (BASE *base)
{
	/*
	 *	Valores retornados:
	 *		>= 0	presente
	 *		-1	imediatamente anterior
	 *		-2	imediatamente posterior
	 *		-3	ausente
	 */
	int		i;
	BASE		**map;

#if (0)	/*************************************/
	if (Debug)
		fprintf (stderr, "present: v_size: %d\n", View->v_size);
#endif	/*************************************/

	/*
	 *	Verifica se a página já existe
	 */
	if (View->v_map == NOBASEP)
		return (-3);

	map = View->v_map;

	for (i = View->v_size - 1; i >= 0; i--)
	{
		if (map[i] == base)
			return (i);
	}

	if (base == View->v_ant)
		return (-1);

	if (base == View->v_pos)
		return (-2);

	return (-3);

}	/* end pg_present */

/*
 ****************************************************************
 *	Mostra uma página					*
 ****************************************************************
 */
void
pg_display (BASE *base)
{
	int		n;
	BASE		*first, *last;
	WINDOW		*win;

	if (Debug)
		fprintf (stderr, "pg_display: \"%s\"\n", base->ch);

	win = View->v_win;

	first = txt_walk (base, -View->v_lines/2 + 1);
	last = txt_walk (first, View->v_lines - 1);

	View->v_first = first;

	n = 0;

	for (EVER)
	{
		wmove (win, n, 0);
		waddstr (win, first->ch);

		if (first == base)
			View->v_curl = n;

		n++;

		if (first != last)
			first = first->pos;
		else
			break;
	}

	View->v_size = n;
	View->v_last = last;

	View->v_base = base;
	View->v_curc = str_nonwhite (base->ch);

	newmap (); fill ();

	wmove (win, View->v_curl, View->v_curc);

}	/* end pg_display */

/*
 ****************************************************************
 *	Rolamento da página					*
 ****************************************************************
 */
void
pg_scroll (int n)
{
	BASE		*first, *last;
	BASE		*vfirst, *vlast;

	vfirst = View->v_first;
	vlast =  View->v_last;

	first = txt_walk (View->v_first, n);
	last = txt_walk (first, View->v_lines - 1);

	if (n > 0)
	{
		while (vlast != last)
		{
			vlast = vlast->pos;
			vfirst = vfirst->pos;
			wscroll (View->v_win, 1);
			waddstr (View->v_win, vlast->ch);
		}

		while (vfirst != first)
		{
			vfirst = vfirst->pos;
			wscroll (View->v_win, 1);
			View->v_size--;
		}
	}
	else
	{
		while (vfirst != first)
		{
			vfirst = vfirst->ant;
			wscroll (View->v_win, -1);
			waddstr (View->v_win, vfirst->ch);
			View->v_size++;
		}

		if (View->v_size > View->v_lines)
			View->v_size = View->v_lines;
	}

	View->v_first = first;
	View->v_last  = last;

	newmap ();

	if ((View->v_curl = pg_present (View->v_base)) < 0)
	{
		if (n > 0)
		{
			View->v_base = first;
			View->v_curc = str_nonwhite (first->ch);
			View->v_curl = 0;
		}
		else
		{
			View->v_base = last;
			View->v_curc = str_nonwhite (last->ch);
			View->v_curl = View->v_size - 1;
		}
	}

	fill ();

	wmove (View->v_win, View->v_curl, View->v_curc);

}	/* end pg_scroll */

/*
 ****************************************************************
 *	Move							*
 ****************************************************************
 */
void
pg_move (BASE *base, int order, bool checking)
{
	int		d, l;

	if (Debug)
		fprintf (stderr, "pg_move: \"%s\"\n", base->ch);

	if ((l = pg_present (base)) < 0)
	{
		if (d = disp (base))
			pg_scroll (d);
		else
			pg_display (base);

		View->v_curl = pg_present (base);
	}
	else
	{
		View->v_curl = l;
	}

	View->v_base = base;

	if (checking)
	{
		if (base->ch[order] == '\0' && order > 0)
			order -= 1;
	}

	View->v_curc = order;

	wmove (View->v_win, View->v_curl, View->v_curc);

}	/* end pg_move */

/*
 ****************************************************************
 *	Atualização						*
 ****************************************************************
 */
void
pg_update (void)
{
	int		l;
	BASE		*anterior, *posterior;
	int		presize, possize;

	/*
	 *	Verifica se a página já existe
	 */
	if (View->v_map == NOBASEP)
		return;

	/*
	 *	Atualiza cada alteração
	 */
	while (txt_change (&anterior, &posterior, &presize, &possize))
	{
		/*
		 *	Tenta referência a anterior
		 */
		l = pg_present (anterior);

		if (Debug)
			fprintf (stderr, "anterior: \"%s\", l = %d\n", anterior->ch, l);

		if (l >= -1 && !ISLAST (anterior))
		{
			down (l + 1, anterior->pos, presize, possize);
			continue;
		}

		if (l >= 0) 	/* ISLAST (anterior) !	*/
		{
			View->v_size = l + 1;
			View->v_last = anterior;
			View->v_pos = anterior->pos;
			continue;
		}

		/*
		 *	Tenta referência a posterior
		 */
		l = pg_present (posterior);

		if (Debug)
			fprintf (stderr, "posterior: \"%s\", l = %d\n", posterior->ch, l);

		if (l >= 0 && !ISFIRST (posterior))
		{
			up (l - 1, posterior->ant, presize, possize);
			continue;
		}

		/*
		 *	Redesenha a página
		 */
		if (ISLAST (anterior))
			pg_display (anterior);
		else
			pg_display (anterior->pos);
	}

	fill ();

}	/* end pg_update */

/*
 ****************************************************************
 *	Remap							*
 ****************************************************************
 */
void
pg_remap (BASE *base)
{
	BASE		**map;
	BASE		*b;
	int		i;
	int		l;

	map = View->v_map;
	wgetyx (View->v_win, l, i);

	/*
	 *	Remapeia linhas superiores à atual
	 */
	for (b = base, i = l; i >= 0; i--)
		{ map[i] = b; b = b->ant; }

	/*
	 *	Remapeia linhas inferiores à atual
	 */
	b = base; i = l + 1;

	while (!ISLAST (b) && i < View->v_lines)
		{ b = b->pos; map[i] = b; i++; }

	View->v_size = i;

	/*
	 *	Complementa as informações sobre a página
	 */
	View->v_first = map[0];
	View->v_last = map[View->v_size - 1];

	View->v_ant = View->v_first->ant;
	View->v_pos = View->v_last->pos;

}	/* end pg_remap */

/*
 ****************************************************************
 *	Deslocamento						*
 ****************************************************************
 */
static int
disp (BASE *base)
{
	BASE		*b;
	int		d;
	int		halfpage;

	halfpage = View->v_lines / 2;

	b = View->v_last;

	for (d = 0; d < halfpage; d++)
	{
		if (b == base)
			return (d);

		elif (ISLAST (b))
			break;
		else
			b = b->pos;
	}

	b = View->v_first;

	for (d = 0; d < halfpage; d++)
	{
		if (b == base)
			return (-d);
		elif (ISFIRST (b))
			break;
		else
			b = b->ant;
	}

	return (0);

}	/* end disp */

/*
 ****************************************************************
 *	Contrói Mapa de BASEs na página				*
 ****************************************************************
 */
static void
newmap (void)
{
	/*
	 *	Hipótese: View->v_first e View->v_last estão corretos
	 */
	BASE		**map;
	BASE		*base;
	int		i;

	/*
	 *	Verifica se a página já existe
	 */
	if (View->v_map == NOBASEP)
		View->v_map = malloc (View->v_lines * sizeof (BASE *));

	map = View->v_map;

	View->v_ant = View->v_first->ant;
	View->v_pos = View->v_last->pos;

	base = View->v_last;

	for (i = View->v_size - 1; i >= 0; i--)
		{ map[i] = base; base = base->ant; }

}	/* end newmap */

/*
 ****************************************************************
 *	Atualiza para baixo					*
 ****************************************************************
 */
static void
down (int line, BASE *base, int presize, int possize)
{
	int		i;
	int		size;
	WINDOW		*win;
	BASE		**map;

	win = View->v_win;
	map = View->v_map;

	if (Debug)
	{
		fprintf
		(	stderr,
			"down: a partir de \"%s\" (%d->%d)\n",
			base->ch, presize, possize
		);
	}

	/*
	 *	Ajusta os tamanhos
	 */
	if (presize > View->v_lines - line)
		presize = View->v_lines - line;

	if (possize > View->v_lines - line)
		possize = View->v_lines - line;

	if (Debug)
		fprintf (stderr, "down: size: %d, line:%d\n", possize - presize, line);

	/*
	 *	Abre ou fecha espaço na janela
	 */
	if ((size = possize - presize) > 0)
	{
		wmove (win, line + presize, 0);		/* CUIDADO */

		for (i = size; i > 0; i--)
			winsertln (win);

		if (View->v_size + size > View->v_lines)
			View->v_size = View->v_lines;
		else
			View->v_size += size;

		for (i = View->v_lines - 1 - size; i >= line; i--) 
			map[i + size] = map[i];
	}
	elif (size < 0)
	{
		size = -size;

		wmove (win, line + possize, 0);		/* CUIDADO */

		for (i = size; i > 0; i--)
			wdeleteln (win);

		View->v_size -= size;

		for (i = line + size; i < View->v_lines; i++)
			map[i - size] = map[i];
	}

	/*
	 *	Escreve linhas diferentes
	 */
	for (i = line; i < line + possize; i++)
	{
		if (Debug)
			fprintf (stderr, "down: \"%s\" (#%d)\n", base->ch, i);

		wmove (win, i, 0);
		waddstr (win, base->ch);

		map[i] = base;

		if (ISLAST (base))
			break;
		else
			base = base->pos;
	}

	/*
	 *	Completa o restante da página
	 */
	if ((i = View->v_size) > 0)
		base = map[i - 1];
	else
		base = View->v_ant;

	while (i < View->v_lines && !ISLAST (base))
	{
		base = base->pos;

		wmove (win, i, 0);
		waddstr (win, base->ch);

		map[i] = base;
		i++;
	}

	View->v_size = i;

	/*
	 *	Acerta informações sobre a página
	 */
	View->v_first = map[0];
	View->v_last = map[View->v_size - 1];
	View->v_ant = View->v_first->ant;
	View->v_pos = View->v_last->pos;

}	/* end down */

/*
 ****************************************************************
 *	Atualiza para cima					*
 ****************************************************************
 */
static void
up (int line, BASE *base, int presize, int possize)
{
	int		i;

	if (Debug)
		fprintf (stderr, "up: \"%s\" (#%d)\n", base->ch, line);

	/*
	 *	Ajusta os tamanhos
	 */
	if (presize > line + 1)
		presize = line + 1;

	if (possize > line + 1)
		possize = line + 1;

	/*
	 *	Pega a primeira base
	 */
	for (i = possize - 1; i > 0; i--)
		base = base->ant;

	/*
	 *	Atualiza para baixo
	 */
	down (line - presize + 1, base, presize, possize);

}	/* end up */

/*
 ****************************************************************
 *	Completa a página com tils				*
 ****************************************************************
 */
static void
fill (void)
{
	int		l;
	WINDOW		*win;

	win = View->v_win;

	for (l = View->v_size; l < View->v_lines; l++)
	{
		wmove (win, l, 0);
		waddstr (win, "~");
	}

}	/* end fill */
