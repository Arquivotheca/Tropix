/*
 ****************************************************************
 *								*
 *			du.c					*
 *								*
 *	Gerenciador de arquivos					*
 *								*
 *	Versão	4.5.0, de 17.10.03				*
 *		4.5.0, de 21.11.03				*
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
#include <sys/ustat.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <stat.h>

#include <X11/Intrinsic.h>

#include "../h/xfm.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
#define	NODEV		-1

/*
 ******	Estado da Remoção ***************************************
 */
entry NODE	*du_next_src_np,
		*du_src_dir_np, *du_src_sub_dir_np;

entry int	du_cnt;

/*
 ******	Item de "link" ******************************************
 */
typedef struct ditem	DITEM;

struct ditem
{
	dev_t	l_dev;		/* Dispositivo */
	ino_t	l_ino;		/* No. do Inode */
	DITEM	*l_next;	/* o Proximo */
};

#define	NODITEM	(DITEM *)NULL

entry DITEM	*dlinklist;	/* Comeco da Lista */

/*
 ******	Protótipos de funções ***********************************
 */
void		du_tree_1 (void);
int		proc_link (STAT *);
void		free_dlink_list (void);
int		round_file_size_to_BL (const STAT *sp);

/*
 ****************************************************************
 *	Verifica se é permitida um "du"				*
 ****************************************************************
 */
int
permission_du (const NODE *np)
{
	if (S_ISDIR (np->n_ls.st_mode))
		return (1);

	return (-1);

}	/* end permission_du */

/*
 ****************************************************************
 *	Remove um ou mais arquivos				*
 ****************************************************************
 */
void
du_0 (void)
{
	const MENUDATA	*mp = &menudata[present_menu];
	NODE		*np;

	/*
	 *	Prólogo
	 */
	free_dlink_list ();

	/*
	 *	Verifica se foi dado um diretório
	 */
	if ((np = mp->menu_node) != NONODE || (np = select_dir_node) != NONODE)
	{
		if (S_ISDIR (np->n_ls.st_mode))
		{
			du_src_dir_np	  = np;
			du_src_sub_dir_np = du_src_dir_np;

			if (np->n_children == NONODE)
				create_child_dir_list (np);

			du_next_src_np = np->n_children;

			du_cnt = 0;

			du_tree_1 ();
		}
	}

}	/* end du_0 */

/*
 ****************************************************************
 *	Computa o número de blocos ocupados por uma árvore	*
 ****************************************************************
 */
void
du_tree_1 (void)
{
	const WINDATA	*menu_wp = &tree_menu;
	WINDATA		*wp = &du_data;
	NODE		*src_np;
	long		blsize = 0;
	XWindowChanges	value;
	char		aux_area[16];

	/*
	 *	Obtém o tamanho de uma árvore
	 */
    again:
	for (src_np = du_next_src_np; src_np != NONODE; du_next_src_np = src_np = src_np->n_forw)
	{
		/*
		 *	Pula "." e ".."
		 */
		if (src_np->n_nm[0] == '.')
		{
			if (src_np->n_nm[1] == '\0')
				continue;

			if (src_np->n_nm[1] == '.' && src_np->n_nm[2] == '\0')
				continue;
		}

		/*
		 *	Verifica se achou um subdiretório
		 */
		if   (S_ISDIR (src_np->n_ls.st_mode))
		{
			blsize += round_file_size_to_BL (&src_np->n_ls);

                        du_src_sub_dir_np = src_np;

			if (src_np->n_children == NONODE)
				create_child_dir_list (src_np);

			du_next_src_np = src_np->n_children->n_forw;

			goto again;
		}

		/*
		 *	Guarda as informações do elo físico
		 */
		if (src_np->n_ls.st_nlink > 1 && proc_link (&src_np->n_ls))
			continue;

		/*
		 *	Computa o arquivo
		 */
		blsize += round_file_size_to_BL (&src_np->n_ls);

	}	/* for (lista de arquivos fonte) */

	/*
	 *	Verifica se deve subir de nível
	 */
	if (du_src_sub_dir_np != du_src_dir_np)
	{
		du_next_src_np	  = du_src_sub_dir_np->n_forw;

		du_src_sub_dir_np = du_src_sub_dir_np->n_parent;

		goto again;
	}

	/*
	 *	Computa a raiz da árvore
	 */
	blsize += round_file_size_to_BL (&du_src_dir_np->n_ls);

	free_dlink_list ();

	/*
	 *	Prepara o texto
	 */
	edit_len = snprintf
	(	edit_area, sizeof (edit_area), "%s",
		edit_sz_value (blsize, aux_area)
	);

	/*
	 *	Prepara o tamanho da janela
	 */
	value.width  = wp->width  = 2 * (MENU_BORDER + SHADOW) + edit_len * wp->font_width;
	value.height = wp->height = 2 * (MENU_BORDER + SHADOW) + wp->font_height;

	value.x = menu_wp->x + menu_wp->width;
	value.y = menu_wp->y + 7 * (wp->font_height + 2 * SHADOW);

	XConfigureWindow (display, wp->win, CWWidth|CWHeight|CWX|CWY, &value);

	XMapWindow (display, wp->win);

}	/* end du_1 */

/*
 ****************************************************************
 *	Desenha a janela do DU					*
 ****************************************************************
 */
void
draw_du_data_window (void)
{
	WINDATA		*wp = &du_data;

	XDrawImageString
	(	
		display, wp->win,
		wp->text_gc,
		MENU_BORDER + SHADOW,
		MENU_BORDER + 1 * (wp->font_height + 2 * SHADOW) - (wp->font_descent + SHADOW),
		edit_area, edit_len
	);

	draw_shadow
	(	wp->win,
		wp->triple.top_gc, wp->triple.bottom_gc,
		0, 0,
		wp->width, wp->height
	);

}	/* end draw_du_data_window */

/*
 ****************************************************************
 *	Processa LINKs						*
 ****************************************************************
 */
int
proc_link (STAT *sp)
{
	DITEM		*lp;

	for (lp = dlinklist; lp != NODITEM; lp = lp->l_next)
	{
		if (lp->l_dev == sp->st_dev && lp->l_ino == sp->st_ino)
			return (1);
	}

	if ((lp = malloc (sizeof (DITEM))) == NODITEM)
	{
		error (NOSTR);
	}
	else
	{
		lp->l_dev = sp->st_dev;
		lp->l_ino = sp->st_ino;

		lp->l_next = dlinklist;
		dlinklist = lp;
	}

	return (0);

}	/* end proc_link */

/*
 ****************************************************************
 *	Libera a lista de elos físicos				*
 ****************************************************************
 */
void
free_dlink_list (void)
{
	DITEM		*lp, *next;

	for (lp = dlinklist; lp != NODITEM; lp = next)
		{ next = lp->l_next; free (lp); }

	dlinklist = NODITEM;

}	/* end free_dlink_list */

/*
 ****************************************************************
 *	Função para obter o tamanho do bloco			*
 ****************************************************************
 */
int
round_file_size_to_BL (const STAT *sp)
{
	static dev_t	cache_dev = NODEV;
	static int	cache_size;
	static int	cache_symlink_sz;
	int		size, mask;

	/*
	 *	Em primeiro lugar, obtém o tamanho do bloco
	 */
	if (sp->st_dev != cache_dev)
	{
		USTAT		*up = alloca (sizeof (USTAT));

		if (ustat (sp->st_dev, up) < 0)
		{
			draw_error_msg_text
			(	"*Não consegui obter o estado do sistema de arquivos %d/%d",
				MAJOR (sp->st_dev), MINOR (sp->st_dev)
			);

			return (0);
		}

		cache_dev		= sp->st_dev;
		cache_size		= up->f_bsize;
		cache_symlink_sz	= up->f_symlink_sz;
	}

	/*
	 *	Verifica o tipo de arquivo
	 */
	size = sp->st_size;

	switch (sp->st_mode & S_IFMT)
	{
	    case S_IFREG:
	    case S_IFDIR:
		break;

	    case S_IFLNK:
		if (size < cache_symlink_sz)
			return (0);

		break;

	    default:
		return (0);
	}

	/*
	 *	Agora arredonda
	 */
	mask = cache_size - 1;

	return (((size + mask) & ~mask) >> BLSHIFT);

}	/* end round_file_size_to_BL */
