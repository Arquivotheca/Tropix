/*
 ****************************************************************
 *								*
 *			mkdir.c					*
 *								*
 *	Gerenciador de arquivos					*
 *								*
 *	Versão	4.5.0, de 17.10.03				*
 *		4.5.0, de 17.10.03				*
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
void		mkdir_1 (void);

/*
 ****************************************************************
 *	Verifica se é permitida a criação de um diretório	*
 ****************************************************************
 */
int
permission_mkdir (const NODE *np)
{
	if (np == NONODE)
		return (-1);

	if (!S_ISDIR (np->n_s.st_mode))
		return (-1);

	return (1);

}	/* end permission_mkdir */

/*
 ****************************************************************
 *	Cria um novo diretório					*
 ****************************************************************
 */
void
mkdir_0 (void)
{
	const WINDATA	*menu_wp;
	WINDATA		*wp = &nm_edit;
	KEYDATA		*kp = &keydata;
	int		len = 50;
	XWindowChanges	value;

	/*
	 *	Prepara o tamanho da janela
	 */
	switch (present_menu)
	{
	    case TREE_MENU:
		menu_wp = &tree_menu;
		break;

	    case DIR_MENU:
		menu_wp = &dir_menu;
		break;

	    default:
		return;
	}

	value.width  = wp->width  = 2 * (MENU_BORDER + SHADOW) + len * wp->font_width;
	value.height = wp->height = 2 * (MENU_BORDER + SHADOW) + wp->font_height;

	value.x = menu_wp->x + menu_wp->width;
	value.y = menu_wp->y + menu_wp->height - 2 * (wp->font_height + 2 * SHADOW);

	XConfigureWindow (display, wp->win, CWWidth|CWHeight|CWX|CWY, &value);

	/*
	 *	Prepara a área de leitura do nome do diretório
	 */
	kp->k_code	 = 'M';
	kp->k_changed	 = 0;
	kp->k_display	 = display;
	kp->k_win	 = wp->win;
	kp->k_gc	 = wp->text_gc;
	kp->k_rev_gc	 = reverse_text_gc;
	kp->k_font_width = wp->font_width;
	kp->k_area	 = edit_area;
	kp->k_count	 = 0;
	kp->k_offset	 = 0;
	kp->k_size	 = sizeof (edit_area);
	kp->k_x		 = MENU_BORDER + SHADOW;
	kp->k_y		 = MENU_BORDER + 1 * (wp->font_height + 2 * SHADOW) - (wp->font_descent + SHADOW);
	kp->k_function	 = mkdir_1;

	kp->k_area[0] 	 = '\0';

	XMapWindow (display, wp->win);

}	/* end mkdir_0 */

/*
 ****************************************************************
 *	Cria o diretório, após ter teclado o nome		*
 ****************************************************************
 */
void
mkdir_1 (void)
{
	const MENUDATA	*mp = &menudata[present_menu];
	const WINDATA	*wp = &nm_edit;
	NODE		*np, *pp = mp->menu_node;

	/*
	 *	Prólogo
	 */
	XUnmapWindow (display, wp->win);
	XUnmapWindow (display, mp->windata->win);

	/*
	 *	Cria o novo nó
	 */
	np = create_new_node (pp, edit_area, strlen (edit_area));

	/*
	 *	Tenta criar o diretório
	 */
	if (mkdir (np->n_path, 0755) < 0)
	{
	    bad:
		draw_error_msg_text ("*Não consegui criar o diretório \"%s\"", np->n_path);
		free (np); return;
	}

	/*
	 *	Obtém os estados
	 */
	if (lstat (np->n_path, &np->n_ls) < 0)
		{ rmdir (np->n_path); goto bad; }

	memmove (&np->n_s, &np->n_ls, sizeof (STAT));
	
	np->n_sb_flags = get_mount_flags (np->n_ls.st_dev);

	/*
	 *	Insere na lista
	 */
	insert_node_lexicografically (pp, np);	/* Já incrementa "n_line_size" */

	pp->n_ls.st_nlink += 1;		/* O diretório pai tem mais um filho */
   	pp->n_s.st_nlink  += 1;

	/*
	 *	Verifica se redesenha a árvore
	 */
	if (pp->n_ls.st_nlink == 3 || pp->n_expanded)
	{
		XClearWindow (display, tree.win);
		draw_tree_win ();
	}

	/*
	 *	Verifica se redesenha o diretório
	 */
	if (pp == dir_win_node)
	{
		XClearWindow (display, dir.win);
		draw_dir_win ();
	}

	/*
	 *	Mensagem final
	 */
	draw_msg_text ("Criado o Diretório \"%s\"", np->n_path);

}	/* end mkdir_1 */
