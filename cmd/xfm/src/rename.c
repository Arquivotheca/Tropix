/*
 ****************************************************************
 *								*
 *			rename.c				*
 *								*
 *	Gerenciador de arquivos					*
 *								*
 *	Versão	4.5.0, de 17.10.03				*
 *		4.5.0, de 22.11.03				*
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
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stat.h>

#include <X11/Intrinsic.h>

#include "../h/xfm.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */

/*
 ******	Protótipos de funções ***********************************
 */
void		rename_1 (void);

/*
 ****************************************************************
 *	Verifica se é permitida trocar de nome			*
 ****************************************************************
 */
int
permission_rename (const NODE *np)
{
	if (np == NONODE)
		return (-1);

	return (1);

}	/* end permission_rename */

/*
 ****************************************************************
 *	Troca o nome do arquivo (0)				*
 ****************************************************************
 */
void
rename_0 (void)
{
	const MENUDATA  *mp = &menudata[present_menu];
	WINDATA		*wp = &nm_edit;
	const NODE	*np;
	KEYDATA		*kp = &keydata;
	int		len = 50;
	XWindowChanges	value;

	/*
	 *	Prepara o tamanho da janela
	 */
	if ((np = mp->menu_node) == NONODE)
		return;

	value.width  = wp->width  = 2 * (MENU_BORDER + SHADOW) + len * wp->font_width;
	value.height = wp->height = 2 * (MENU_BORDER + SHADOW) + wp->font_height;

	value.x = mp->windata->x + mp->windata->width;
	value.y = mp->windata->y + mp->windata->height - 7 * (wp->font_height + 2 * SHADOW);

	XConfigureWindow (display, wp->win, CWWidth|CWHeight|CWX|CWY, &value);

	/*
	 *	Prepara a área de leitura do nome do diretório
	 */
	strncpy (edit_area, np->n_nm, sizeof (edit_area));

	kp->k_code	 = 'M';
	kp->k_changed	 = 0;
	kp->k_display	 = display;
	kp->k_win	 = wp->win;
	kp->k_gc	 = wp->text_gc;
	kp->k_rev_gc	 = reverse_text_gc;
	kp->k_font_width = wp->font_width;
	kp->k_area	 = edit_area;
	kp->k_count	 = np->n_nm_len;
	kp->k_offset	 = 0;
	kp->k_size	 = sizeof (edit_area);
	kp->k_x		 = MENU_BORDER + SHADOW;
	kp->k_y		 = MENU_BORDER + 1 * (wp->font_height + 2 * SHADOW) - (wp->font_descent + SHADOW);
	kp->k_function	 = rename_1;

	XMapWindow (display, wp->win);

}	/* end rename_0 */

/*
 ****************************************************************
 *	Troca o nome do arquivo (1)				*
 ****************************************************************
 */
void
rename_1 (void)
{
	const MENUDATA	*mp = &menudata[present_menu];
	const WINDATA	*wp = &nm_edit;
	NODE		*np = mp->menu_node, *pp = np->n_parent;
	NODE		*new_np, *child_np, *next_np;
	char		*new_path;
	int		pp_path_len, new_nm_len = strlen (edit_area), new_path_len;

	/*
	 *	Prólogo
	 */
	XUnmapWindow (display, wp->win);
	XUnmapWindow (display, mp->windata->win);

	/*
	 *	compõe o novo nome completo
	 */
	if ((pp_path_len = pp->n_path_len) == 1)
		pp_path_len = 0;

	new_path_len = pp_path_len + new_nm_len + 1;

	if ((new_path = alloca (new_path_len + 1)) == NOSTR)
		error (NOSTR);

	if (pp_path_len)
		strcpy (new_path, pp->n_path);

	new_path[pp_path_len] = '/';
	strcpy (new_path + pp_path_len + 1, edit_area);

	/*
	 *	Troca o nome
	 */
	if (rename (np->n_path, new_path) < 0)
	{
		draw_error_msg_text
		(	"*Não consegui trocar o nome do arquivo \"%s\" para \"%s\"",
			np->n_path, new_path
		);

		return;
	}

	/*
	 *	Aloca um (talvez) novo nó
	 */
	if ((new_np = realloc (np, sizeof (NODE) + new_path_len + 1)) == NONODE)
		error (NOSTR);

	if (new_np != np)
	{
		if (new_np->n_back != NONODE)
			new_np->n_back->n_forw = new_np;

		if (new_np->n_forw != NONODE)
			new_np->n_forw->n_back = new_np;

		new_np->n_path = (char *)new_np + sizeof (NODE);

		np = new_np;
	}

	/*
	 *	Altera os nomes do nó
	 */
	strcpy (np->n_path, new_path);			np->n_path_len = new_path_len;
	np->n_nm = np->n_path + pp_path_len + 1;	np->n_nm_len   = new_nm_len;

	/*
	 *	O diretório foi alterado (os caminhos estão errados)
	 */
	for (child_np = np->n_children; child_np != NONODE; child_np = next_np)
		{ next_np = child_np->n_forw; free_node (child_np); }

	np->n_children = NONODE;

	/*
	 *	Verifica se redesenha a árvore
	 */
	if (pp->n_expanded)
		{ XClearWindow (display, tree.win); draw_tree_win (); }

	/*
	 *	Verifica se redesenha o diretório
	 */
	if (pp == dir_win_node)
		{ XClearWindow (display, dir.win); draw_dir_win (); }

}	/* end rename_1 */
