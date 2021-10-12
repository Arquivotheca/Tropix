/*
 ****************************************************************
 *								*
 *			tree.c					*
 *								*
 *	Gerenciador de arquivos					*
 *								*
 *	Versão	4.5.0, de 24.06.03				*
 *		4.8.0, de 16.07.05				*
 *								*
 *	Módulo: xfm						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2005 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/common.h>
#include <sys/scb.h>
#include <sys/sync.h>
#include <sys/syscall.h>
#include <sys/mntent.h>
#include <sys/sb.h>

#include <stdio.h>
#include <stdlib.h>
#include <stat.h>
#include <dirent.h>

#include <X11/Intrinsic.h>

#include "../h/xfm.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
#define	CHILD_DISTANCE	(3 * wp->font_width)

/*
 ******	Outras variáveis ****************************************
 */
entry NODE	*root_node;		/* Nó da RAIZ da árvore */

entry int	tree_total_lines;	/* Total de linhas da janela da árvore */

entry int	tree_first_line;	/* No. da primeira linha da janela da árvore */

entry int	tree_line_vis;		/* No. de linhas visíveis na janela da árvore */

entry int	tree_max_line_offset;	/* Comprimento máximo da linha */

/*
 ****** Protótipos de funções ***********************************
 */
void		draw_child_directory (NODE *np, int offset, int *line_y, int *skip_lines, int, int);
void		draw_tree_line (NODE *np, int offset, int *line_y);

/*
 ****************************************************************
 *	Desenha a janela de ÁRVORE				*
 ****************************************************************
 */
void
draw_tree_win (void)
{
	const WINDATA	*wp = &tree;
	int		line_y,	line_no, n;

	/*
	 *	Desenha os filhos da RAIZ
	 */
	line_y = BORDER + SHADOW + TREE_OFFSET;

	tree_max_line_offset = 0;

	line_no		 = 0;
	tree_total_lines = 0;

	draw_child_directory
	(	root_node, 0, &line_y, &line_no,
		tree_first_line, tree_first_line + tree_line_vis
	);

	/*
	 *	Verifica se o tamanho deve aumentar
	 */
	if (extension_in_use == WIN_EXTENSION_ON && (n = tree_max_line_offset - tree.width) > 0)
	{
		set_window_sizes (0, 0, n);
		return;
	}

	/*
	 *	Põe a sombra ao redor da janela
	 */
	draw_shadow
	(	wp->win,
		wp->triple.top_gc, wp->triple.bottom_gc,
		0, 0,
		wp->width, wp->height
	);

	/*
	 *	Coloca a barra de rolagem, se necessário
	 */
	update_tree_scroll_bar (0, 0);

}	/* end draw_tree_win */

/*
 ****************************************************************
 *	Avança ou retrocede a janela da ÁRVORE			*
 ****************************************************************
 */
void
scroll_tree_window (int diff)
{
	const WINDATA	*wp = &tree;
	int		line_y, line_no, n;

	tree_total_lines = 0;
	line_y	= BORDER + SHADOW + TREE_OFFSET;
	line_no = 0;

	if   (abs (diff) >= tree_line_vis)
	{
		return;
	}
	elif (diff > 0)
	{
		XCopyArea
		(	display, wp->win, wp->win, wp->text_gc,
			SHADOW, line_y + diff * LINE_HEIGHT,
			wp->width - 2 * SHADOW, (tree_line_vis - diff) * LINE_HEIGHT,
			SHADOW, line_y
		);

		tree_first_line += diff;

		line_y += (tree_line_vis - diff) * LINE_HEIGHT;

		XClearArea
		(	display, wp->win,
			SHADOW, line_y,
			wp->width - 2 * SHADOW, diff * LINE_HEIGHT,
			False
		);

		draw_child_directory
		(	root_node, 0, &line_y, &line_no,
			tree_first_line + tree_line_vis - diff, tree_first_line + tree_line_vis
		);
	}
	elif (diff < 0)
	{
		diff = -diff;

		XCopyArea
		(	display, wp->win, wp->win, wp->text_gc,
			SHADOW, line_y,
			wp->width - 2 * SHADOW, (tree_line_vis - diff) * LINE_HEIGHT,
			SHADOW, line_y + diff * LINE_HEIGHT
		);

		tree_first_line -= diff;

		XClearArea
		(	display, wp->win,
			SHADOW, line_y,
			wp->width - 2 * SHADOW, diff * LINE_HEIGHT,
			False
		);

		draw_child_directory
		(	root_node, 0, &line_y, &line_no,
			tree_first_line, tree_first_line + diff
		);
	}

	/*
	 *	Verifica se o tamanho deve aumentar
	 */
	if (extension_in_use == WIN_EXTENSION_ON && (n = tree_max_line_offset - tree.width) > 0)
	{
		set_window_sizes (0, 0, n);
		return;
	}

	/*
	 *	Reescreve a sombra do lado direito
	 */
	draw_only_right_shadow
	(	wp->win,
		wp->triple.bottom_gc,
		0, 0,
		wp->width, wp->height
	);

}	/* end scroll_tree_window */

/*
 ****************************************************************
 *	Desenha uma subárvore					*
 ****************************************************************
 */
void
draw_child_directory (NODE *np, int offset, int *line_y, int *line_no, int first_line, int last_line)
{
	NODE		*next_np;
	const WINDATA	*wp = &tree;

	/*
	 *	Desenha os diretórios restantes
	 */
	for (/* acima */; np != NONODE; np = next_np)
	{
		next_np = np->n_forw;

		if (!S_ISDIR (np->n_ls.st_mode) || streq (np->n_nm, ".."))
			continue;

		while (next_np && !S_ISDIR (next_np->n_ls.st_mode))
			next_np = next_np->n_forw;

		/* Desenha uma linha */

		if   (*line_no >= first_line && *line_no < last_line)
		{
			draw_tree_line (np, offset, line_y);

			(*line_y) += LINE_HEIGHT;
		}

		/* Passa para a linha seguinte */

		(*line_no)++; tree_total_lines++;

		if (np->n_children != NONODE && np->n_expanded)
		{
			draw_child_directory
			(	np->n_children, offset + CHILD_DISTANCE, line_y, line_no,
				first_line, last_line
			);
		}

	}	/* end for (...) */

}	/* end draw_child_directory */

/*
 ****************************************************************
 *	Desenha uma linha da janela da ÁRVORE			*
 ****************************************************************
 */
void
draw_tree_line (NODE *np, int offset, int *line_y)
{
	const WINDATA	*wp = &tree;
	const TRIPLEGC  *tp;
	const SB	*sp;
	const NODE   	*pp;
	int		len;
	char		fname[sizeof (sp->sb_fname) + 3];

	get_file_icon (np);

	if (np->n_ls.st_nlink > 2)
		tp = &full_dir_triple;
	else
		tp = &empty_dir_triple;

	draw_button
	(	wp, tp, 
		MENU_BORDER + SHADOW + offset,
		*line_y + BUTTON_OFFSET,
		BUTTON_WIDTH, BUTTON_HEIGHT
	);

	XPutImage
	(	display, wp->win,
		wp->text_gc, np->n_ximage,
		0, 0,
		MENU_BORDER + SHADOW + NAME_OFFSET + offset,
		*line_y + ICON_OFFSET,
		np->n_ximage->width, np->n_ximage->height
	);

	offset += MENU_BORDER + SHADOW + 2 * NAME_OFFSET;

	XDrawString
	(	display, wp->win,
		wp->text_gc,
		offset,
		*line_y + TEXT_OFFSET,
		np->n_nm, np->n_nm_len
	);

	offset += (np->n_nm_len + 1) * wp->font_width;

	/*
	 *	Verifica se está montado
	 */
	if
	(	(volume_in_use == VOLUME_ON) && (pp = np->n_parent) &&
		pp->n_ls.st_dev != np->n_ls.st_dev && (sp = get_super_block (np->n_ls.st_dev))
	)
	{
		fname[0] = '['; strcpy (&fname[1], sp->sb_fname); strcat (fname, "]");

		len = strlen (fname);

		XDrawString
		(	display, wp->win,
			wp->text_gc,
			offset,
			*line_y + TEXT_OFFSET,
			fname, len
		);

		offset += (len + 1) * wp->font_width;
	}

	offset += wp->font_width;

	if (tree_max_line_offset < offset)
		tree_max_line_offset = offset;

}	/* end draw_tree_line */

/*
 ****************************************************************
 *	Foi apertada uma tecla na janela da ÁRVORE		*
 ****************************************************************
 */
void
button_press_tree_window (int button, int orig_x, int orig_y)
{
	WINDATA		*wp = &tree;
	NODE		*np, *pp;
	int		present_line, level = 0, clicked_level, desired_line;
	int		line_size, inc;
	int		x = orig_x, y = orig_y;

	/*
	 *	Examina a roda
	 */
	switch (button)
	{
	    case Button4:
		update_tree_scroll_bar (-1, 1);
		return;

	    case Button5:
		update_tree_scroll_bar (-1, 100000);
		return;

	    default:
		break;

	}	/* end switch */

	/*
	 *	Examina o "x"
	 */
	x -= MENU_BORDER + SHADOW;

	clicked_level	= x / CHILD_DISTANCE;
	x		= x % CHILD_DISTANCE;

	if (x < 0 || x > BUTTON_WIDTH)
		return;

	/*
	 *	Examina o "y"
	 */
	y -= BORDER + SHADOW + TREE_OFFSET;

	desired_line = y / LINE_HEIGHT + tree_first_line;

	/*
	 *	Procura o nó
	 */
	np = root_node; present_line = 0;
    next_level:
	for (/* acima ou abaixo */; np != NONODE; np = np->n_forw)
	{
		if (!S_ISDIR (np->n_ls.st_mode) || streq (np->n_nm, ".."))
			continue;

		if (np->n_expanded)
			line_size = np->n_line_size;
		else
			line_size = 1;

		/*
		 *	Verifica se achou a linha correta
		 */
		if (present_line == desired_line)
		{
#undef	DEBUG
#ifdef	DEBUG
printf
(	"Achei a linha \"%s\": (present = %d, sz = %d, desired = %d), root = %d\n",
	np->n_nm, present_line, line_size, desired_line, root_node->n_line_size
);
#endif	DEBUG
			/*
			 ******	Se clicou no ícone, ... *********************************
			 */
			if (level + 1 == clicked_level)
			{
				if (button == Button3)
					{ button_3_press_tree_window (orig_x, orig_y, np); return; }

				if (button != Button1)
					return;

				if (np == dir_win_node)
					return;

				if (np->n_children == NONODE)
					create_child_dir_list (np);

				disselect_all (0);

				dir_win_node	  = np;
				dir_win_first_son = np->n_children;
				np->n_line_first  = 0;

				get_max_colsz ();

				XClearWindow (display, dir.win);
				dir_line_first = 0;

				dir_scroll.button_no	= 0;
				dir_scroll.last_offset	= -1;

				draw_dir_win ();
				return;
			}

			if (clicked_level != level)
				return;

			/*
			 ******	Clicou no botão de expansão/contração *******************
			 */
			if (np->n_ls.st_nlink <= 2)
				return;

			if (np->n_children == NONODE)
				create_child_dir_list (np);

			/* Atualiza os tamanhos dos pais */

			np->n_expanded ^= 1;	/* Inverte a expansão/contração */

			if (np->n_expanded)		/* Expansão */
				inc = np->n_line_size - 1;
			else				/* Contração */
				inc = 1 - np->n_line_size;

			for (pp = np->n_parent; pp != NONODE; pp = pp->n_parent)
			{
				pp->n_line_size += inc;

				if (!pp->n_expanded)
					break;
			}

			XClearWindow (display, tree.win);

			draw_tree_win ();

			return;
		}
#ifdef	DEBUG
printf
(	"\"%s\": (present = %d, sz = %d, desired = %d)\n",
	np->n_nm, present_line, line_size, desired_line
);
#endif	DEBUG
		/*
		 *	Verifica se não pertence a um subdiretório
		 */
		if (present_line + line_size > desired_line)
		{
#ifdef	DEBUG
printf
(       "passando de \"%s\" para \"%s\" (present = %d, sz = %d, desired = %d)\n",
        np->n_nm, np->n_children->n_forw->n_nm, present_line, line_size, desired_line
);
#endif	DEBUG
			np = np->n_children;

			present_line += 1;

			level++;

			goto next_level;
		}

		present_line += line_size;

	}	/* end for (....) */

#ifdef	DEBUG
printf ("Acabou a árvore\n");
#endif	DEBUG
 
}	/* end button_press_tree_window */
