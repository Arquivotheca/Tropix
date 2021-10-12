/*
 ****************************************************************
 *								*
 *			scroll.c				*
 *								*
 *	Gerenciador de arquivos					*
 *								*
 *	Versão	4.5.0, de 24.06.03				*
 *		4.5.0, de 17.11.03				*
 *								*
 *	Módulo: xfm						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2003 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <stat.h>

#include <X11/Intrinsic.h>

#include "../h/xfm.h"

/*
 ****************************************************************
 *	Desenha a janela da BARRA de ROLAGEM			*
 ****************************************************************
 */
void
draw_scroll_win (const SCROLLDATA *sp)
{
	draw_shadow
	(	sp->win,
		sp->triple.top_gc, sp->triple.bottom_gc,
		0, 0,
		sp->width, sp->height
	);

}	/* end draw_scroll_win */

/*
 ****************************************************************
 *	Atualiza a janela da BARRA de ROLAGEM da ÁRVORE		*
 ****************************************************************
 */
void
update_tree_scroll_bar (int motion, int y)
{
	SCROLLDATA	*sp = &tree_scroll;
	int		new_tree_first_line;
	int		down_button_no = 0, offset, last_offset;
	int		size, height;
	int		last_top    = sp->top;
	int		last_bottom = sp->bottom;
	int		inc = 0;
	int		diff;

	/*
	 *	Significado de "motion":
	 *
	 *		-1: Roda do "mouse"
	 *		 0: Clicado em um dos 3 botões da barra de rolagem
	 *		+1: A barra de rolagem foi movimentada
	 */

	/*
	 *	Verifica se há linhas suficientes
	 */
	if (tree_first_line == 0 && tree_total_lines <= tree_line_vis)
	{
		XClearWindow (display, sp->win);
		draw_scroll_win (sp);
		return;
	}

	/*
	 *	Analisa o pedido
	 */
	if   (y == 0)
	{
		new_tree_first_line = tree_first_line;
	}
	elif (y < sp->begin)			/* Sobe uma linha */
	{
		if (motion > 0)
			return;

		if (tree_first_line <= 0)
			return;

		new_tree_first_line = tree_first_line - 1;

		if (motion == 0)
			down_button_no = 1;
	}
	elif (y < last_top)			/* Sobe meia tela */
	{
		if (motion > 0)
			return;

		if (tree_first_line <= 0)
			return;

		if ((new_tree_first_line = tree_first_line - (tree_line_vis >> 1)) < 0)
			new_tree_first_line = 0;
	}
	elif (y > sp->end + SCROLL_WIDTH)	/* Desce uma linha */
	{
		if (motion > 0)
			return;

		if (tree_first_line >= tree_total_lines - 1)
			return;

		new_tree_first_line = tree_first_line + 1;

		if (motion == 0)
			down_button_no = 3;
	}
	elif (y > sp->end)			/* Sobe uma linha */
	{
		if (motion > 0)
			return;

		if (tree_first_line <= 0)
			return;

		new_tree_first_line = tree_first_line - 1;

		if (motion == 0)
			down_button_no = 2;
	}
	elif (y > last_bottom)			/* Desce meia tela */
	{
		if (motion > 0)
			return;

		if (tree_first_line >= tree_total_lines - 1)
			return;

		new_tree_first_line = tree_first_line + (tree_line_vis >> 1);

		if (new_tree_first_line >= tree_total_lines - 1)
			new_tree_first_line = tree_total_lines - 1;
	}
	else					/* Em cima da barra */
	{
		if (motion <= 0)
			return;

		offset = y - sp->top;

		if ((last_offset = sp->last_offset) < 0)
			{ sp->last_offset = offset; return; }

		size = sp->bottom - sp->top;

		if (abs (offset - last_offset) < (size / tree_line_vis))
			return;

		if (offset > last_offset)
		{
			if (tree_first_line >= tree_total_lines - 1)
				return;

			new_tree_first_line = tree_first_line + 1;
		}
		else
		{
			if (tree_first_line <= 0)
				return;

			new_tree_first_line = tree_first_line - 1;
		}
	}

	/*
	 *	Calcula o comprimento da barra de rolagem
	 */
	height = sp->end - sp->begin;

	size = height * tree_line_vis / (tree_total_lines + tree_line_vis - 1);

	if   (size > sp->end - sp->begin)
		size = sp->end - sp->begin;
	elif (size < SCROLL_WIDTH)
		size = SCROLL_WIDTH;

	sp->top    = sp->begin + ((height - size) * new_tree_first_line) / (tree_total_lines - 1);
	sp->bottom = sp->top + size;

	/*
	 *	Desenha a barra de rolagem
	 */
	if ((inc = sp->top - last_top) > 0)
	{
		XClearArea
		(	display, sp->win,
			2 * SHADOW, last_top,
			SCROLL_WIDTH, inc,
			False
		);
	}

	if ((inc = last_bottom - sp->bottom) > 0)
	{
		XClearArea
		(	display, sp->win,
			2 * SHADOW, sp->bottom,
			SCROLL_WIDTH, inc,
			False
		);
	}

	XFillRectangle
	(	display, sp->win,
		sp->triple.main_gc,
		2 * SHADOW, sp->top,
		SCROLL_WIDTH, (sp->bottom - sp->top)
	);

	draw_shadow
	(	sp->win,
		sp->triple.top_gc, sp->triple.bottom_gc,
		2 * SHADOW, sp->top,
		SCROLL_WIDTH, (sp->bottom - sp->top)
	);

	/*
	 *	Redesenha os botões
	 */
	draw_scroll_button (sp, 1, down_button_no);

	draw_scroll_button (sp, 2, down_button_no);

	draw_scroll_button (sp, 3, down_button_no);

	sp->button_no = down_button_no;

	/*
	 *	Reescreve a janela da árvore se for necessário
	 */
	if ((diff = new_tree_first_line - tree_first_line) != 0)
		scroll_tree_window (diff);

}	/* end update_tree_scroll_bar */

/*
 ****************************************************************
 *	Atualiza a janela da BARRA de ROLAGEM do DIRETÓRIO	*
 ****************************************************************
 */
void
update_dir_scroll_bar (int motion, int y)
{
	SCROLLDATA	*sp = &dir_scroll;
	const NODE	*np;
	int		new_dir_line_first;
	int		down_button_no = 0, offset, last_offset;
	int		size, height;
	int		last_top    = sp->top;
	int		last_bottom = sp->bottom;
	int		inc = 0;
	int		diff;

	/*
	 *	Significado de "motion":
	 *
	 *		-1: Roda do "mouse"
	 *		 0: Clicado em um dos 3 botões da barra de rolagem
	 *		+1: A barra de rolagem foi movimentada
	 */

	/*
	 *	Verifica se há linhas suficientes
	 */
	if ((np = dir_win_node) == NONODE || np->n_child_cnt <= dir_line_vis)
	{
		XClearWindow (display, sp->win);
		draw_scroll_win (sp);
		return;
	}

	/*
	 *	Analisa o pedido
	 */
	if   (y == 0)
	{
		new_dir_line_first = dir_line_first;
	}
	elif (y < sp->begin)			/* Sobe uma linha */
	{
		if (motion > 0)
			return;

		if (dir_line_first <= 0)
			return;

		new_dir_line_first = dir_line_first - 1;

		if (motion == 0)
			down_button_no = 1;
	}
	elif (y < last_top)			/* Sobe meia tela */
	{
		if (motion > 0)
			return;

		if (dir_line_first <= 0)
			return;

		if ((new_dir_line_first = dir_line_first - (dir_line_vis >> 1)) < 0)
			new_dir_line_first = 0;
	}
	elif (y > sp->end + SCROLL_WIDTH)	/* Desce uma linha */
	{
		if (motion > 0)
			return;

		if (dir_line_first >= np->n_child_cnt - dir_line_vis)
			return;

		new_dir_line_first = dir_line_first + 1;

		if (motion == 0)
			down_button_no = 3;
	}
	elif (y > sp->end)			/* Sobe uma linha */
	{
		if (motion > 0)
			return;

		if (dir_line_first <= 0)
			return;

		new_dir_line_first = dir_line_first - 1;

		if (motion == 0)
			down_button_no = 2;
	}
	elif (y > last_bottom)			/* Desce meia tela */
	{
		if (motion > 0)
			return;

		if (dir_line_first >= np->n_child_cnt - dir_line_vis)
			return;

		new_dir_line_first = dir_line_first + (dir_line_vis >> 1);

		if (new_dir_line_first > np->n_child_cnt - dir_line_vis)
			new_dir_line_first = np->n_child_cnt - dir_line_vis;
	}
	else					/* Em cima da barra */
	{
		if (motion <= 0)
			return;

		offset = y - sp->top;

		if ((last_offset = sp->last_offset) < 0)
			{ sp->last_offset = offset; return; }

		size = sp->bottom - sp->top;

		if (abs (offset - last_offset) < (size / dir_line_vis))
			return;

		if (offset > last_offset)
		{
			if (dir_line_first >= np->n_child_cnt - dir_line_vis)
				return;

			new_dir_line_first = dir_line_first + 1;
		}
		else
		{
			if (dir_line_first <= 0)
				return;

			new_dir_line_first = dir_line_first - 1;
		}
	}

	/*
	 *	Calcula o comprimento da barra de rolagem
	 */
	height = sp->end - sp->begin;

	size = height * dir_line_vis / np->n_child_cnt;

	if   (size > sp->end - sp->begin)
		size = sp->end - sp->begin;
	elif (size < SCROLL_WIDTH)
		size = SCROLL_WIDTH;

	sp->top    = sp->begin + ((height - size) * new_dir_line_first) /
								(np->n_child_cnt - dir_line_vis);
	sp->bottom = sp->top + size;

	/*
	 *	Desenha a barra de rolagem
	 */
	if ((inc = sp->top - last_top) > 0)
	{
		XClearArea
		(	display, sp->win,
			2 * SHADOW, last_top,
			SCROLL_WIDTH, inc,
			False
		);
	}

	if ((inc = last_bottom - sp->bottom) > 0)
	{
		XClearArea
		(	display, sp->win,
			2 * SHADOW, sp->bottom,
			SCROLL_WIDTH, inc,
			False
		);
	}

	XFillRectangle
	(	display, sp->win,
		sp->triple.main_gc,
		2 * SHADOW, sp->top,
		SCROLL_WIDTH, (sp->bottom - sp->top)
	);

	draw_shadow
	(	sp->win,
		sp->triple.top_gc, sp->triple.bottom_gc,
		2 * SHADOW, sp->top,
		SCROLL_WIDTH, (sp->bottom - sp->top)
	);

	/*
	 *	Redesenha os botões
	 */
	draw_scroll_button (sp, 1, down_button_no);

	draw_scroll_button (sp, 2, down_button_no);

	draw_scroll_button (sp, 3, down_button_no);

	sp->button_no = down_button_no;

	/*
	 *	Atualiza a janela das cartas se for necessário
	 */
	if ((diff = new_dir_line_first - dir_line_first) != 0)
		scroll_dir_window (diff);

}	/* end update_dir_scroll_bar */

/*
 ****************************************************************
 *	Levanta o botão da barra de rolagem			*
 ****************************************************************
 */
void
button_release_scroll_button (SCROLLDATA *sp)
{
	if (sp->button_no != 0)
		draw_scroll_button (sp, sp->button_no, 0);

	sp->button_no   = 0;
	sp->last_offset	= -1;

}	/* end button_release_scroll_button */

/*
 ****************************************************************
 *	Desenha o Retangulo da barra de rolagem			*
 ****************************************************************
 */
void
draw_scroll_button (const SCROLLDATA *sp, int button_no, int down_button_no)
{
	int		x = 2 * SHADOW, y;

	switch (button_no)
	{
	    case 1:
		y = 2 * SHADOW;
		break;

	    case 2:
		y = sp->end;
		break;

	    case 3:
		y = sp->end + SCROLL_WIDTH;
		break;

	    default:
		return;
	}

	XFillRectangle
	(	display, sp->win,
		sp->triple.main_gc,
		x, y,
		SCROLL_WIDTH, SCROLL_WIDTH
	);

	if (down_button_no == button_no)
	{
		draw_shadow
		(	sp->win,
			sp->triple.bottom_gc, sp->triple.top_gc,
			x, y,
			SCROLL_WIDTH, SCROLL_WIDTH
		);
	}
	else
	{
		draw_shadow
		(	sp->win,
			sp->triple.top_gc, sp->triple.bottom_gc,
			x, y,
			SCROLL_WIDTH, SCROLL_WIDTH
		);
	}

}	/* end draw_scroll_button */
