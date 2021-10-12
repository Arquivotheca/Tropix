/*
 ****************************************************************
 *								*
 *			rm.c					*
 *								*
 *	Gerenciador de arquivos					*
 *								*
 *	Versão	4.5.0, de 15.10.03				*
 *		4.5.0, de 18.12.03				*
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
#include <stat.h>

#include <X11/Intrinsic.h>

#include "../h/xfm.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */

/*
 ******	Estado da Remoção ***************************************
 */
entry NODE	*rm_next_src_np,
		*rm_src_dir_np, *rm_src_sub_dir_np;

entry int	rm_file_cnt, rm_dir_cnt;

/*
 ******	Protótipos de funções ***********************************
 */
void		rm_file_dir_1 (int code);
int		rm_file_dir_2 (NODE *np);
void		rm_tree_1 (int code);

/*
 ****************************************************************
 *	Verifica se é permitida a remoção			*
 ****************************************************************
 */
int
permission_rm (const NODE *np)
{
	return (1);

}	/* end permission_rm */

/*
 ****************************************************************
 *	Remove um ou mais arquivos				*
 ****************************************************************
 */
void
rm_file_dir_0 (void)
{
	const MENUDATA	*mp = &menudata[present_menu];
	NODE		*np;
	int		count;

	/*
	 *	Prólogo
	 */
	XUnmapWindow (display, mp->windata->win);

	/*
	 *	Verifica se foram dado um diretório ou arquivo(s)
	 */
	if ((np = mp->menu_node) != NONODE || (np = select_dir_node) != NONODE)
	{
		if (S_ISDIR (np->n_ls.st_mode))
		{
			if (np == root_node)
				return;

			rm_src_dir_np	  = np;
			rm_src_sub_dir_np = rm_src_dir_np;

			if (np->n_children == NONODE)
				create_child_dir_list (np);

			rm_next_src_np = np->n_children;

			rm_file_cnt = rm_dir_cnt = 0;

			msg_func = rm_tree_1;

			draw_question_msg_text (0, "", "Remove a árvore \"%s\"?", np->n_path);
		}
		else
		{
			msg_func = rm_file_dir_1;

			draw_question_msg_text (0, "", "Remove o arquivo \"%s\"?", np->n_path);
		}
	}
	elif ((count = selected_file_cnt) > 0)
	{
		msg_func = rm_file_dir_1;

		if (count == 1)
			draw_question_msg_text (0, "", "Remove o arquivo \"%s\"?", select_file_node->n_path);
		else
			draw_question_msg_text (0, "", "Remove os %d arquivos?", count);
	}

}	/* end rm_file_dir_0 */

/*
 ****************************************************************
 *	Remove uma árvore					*
 ****************************************************************
 */
void
rm_tree_1 (int code)
{
	NODE		*src_np, *pp;

	/*
	 *	Verifica se foi chamado pela função de erro para pular
	 *
	 *	status:		-1: chamada por "question" com NÃO
	 *			+1: chamada por "question" com SIM
	 *			 0: chamada por "rm_file_0"
	 *
	 */
	if (code <= 0)
		return;

	/*
	 *	Remove uma árvore
	 */
    again:
	for (src_np = rm_next_src_np; src_np != NONODE; src_np = rm_next_src_np)
	{
		/*
		 *	Pula "." e ".."
		 */
		if (src_np->n_nm[0] == '.')
		{
			if (src_np->n_nm[1] == '\0')
				{ rm_next_src_np = src_np->n_forw; continue; }

			if (src_np->n_nm[1] == '.' && src_np->n_nm[2] == '\0')
				{ rm_next_src_np = src_np->n_forw; continue; }
		}

		/*
		 *	Verifica se achou um subdiretório
		 */
		if (S_ISDIR (src_np->n_ls.st_mode))
		{
                        rm_src_sub_dir_np = src_np;

			if (src_np->n_children == NONODE)
				create_child_dir_list (src_np);

			rm_next_src_np = src_np->n_children->n_forw;

			goto again;
		}

		/*
		 *	Remove o arquivo
		 */
		rm_next_src_np = src_np->n_forw;

		if (rm_file_dir_2 (src_np) < 0)
			return;

		rm_file_cnt += 1;

	}	/* for (lista de arquivos fonte) */

	/*
	 *	Verifica se deve subir de nível
	 */
	if (rm_src_sub_dir_np != rm_src_dir_np)
	{
		src_np		  = rm_src_sub_dir_np;

		rm_next_src_np	  = src_np->n_forw;

		rm_src_sub_dir_np = src_np->n_parent;

		if (rm_file_dir_2 (src_np) < 0)
			return;

		rm_dir_cnt += 1;

		goto again;
	}

	/*
	 *	Remove a raiz da árvore
	 */
	if (rm_file_dir_2 (rm_src_dir_np) < 0)
		return;

	rm_dir_cnt += 1;

	/*
	 *	Imprime o resultado
	 */
	XClearWindow (display, dir.win);
   /***	dir_line_first = 0; ***/

	dir_scroll.button_no	= 0;
	dir_scroll.last_offset	= -1;

	draw_dir_win ();

	if   (rm_file_cnt == 0 && rm_dir_cnt == 1)
		draw_msg_text ("Removido o diretório \"%s\"", rm_src_dir_np->n_path);
	elif (rm_file_cnt == 1)
		draw_msg_text ("Removida a árvore \"%s\" (1 arquivo)", rm_src_dir_np->n_path);
	else
		draw_msg_text ("Removida a árvore \"%s\" (%d arquivos)", rm_src_dir_np->n_path, rm_file_cnt);

	/*
	 *	Verifica se redesenha a árvore
	 */
	pp = rm_src_dir_np->n_parent;

	if (pp->n_ls.st_nlink == 2 || pp->n_expanded)
		{ XClearWindow (display, tree.win); draw_tree_win (); }

}	/* end rm_tree_1 */

/*
 ****************************************************************
 *	Remove um ou mais arquivos				*
 ****************************************************************
 */
void
rm_file_dir_1 (int code)
{
	const MENUDATA	*mp = &menudata[present_menu];
	NODE		*np, *pp;
	int		isdir = 0, count = 0;
	const char	*last_path = NOSTR;

	if (code <= 0)
		return;

	if ((pp = dir_win_node) == NONODE)
		return;

	if ((np = mp->menu_node) == NONODE && selected_file_cnt + selected_dir_cnt == 0)
		return;

	if (np != NONODE)
	{
		if (rm_file_dir_2 (np) < 0)
			return;

		count++;

		last_path = np->n_path; isdir = S_ISDIR (np->n_ls.st_mode);
	}
	else
	{
		for (np = dir_win_first_son; np != NONODE; np = np->n_forw)
		{
			if (np->n_selected)
			{
				if (rm_file_dir_2 (np) < 0)
					return;

				count++;

				last_path = np->n_path; isdir = S_ISDIR (np->n_ls.st_mode);
			}
		}
	}

	if (count <= 0)
		return;

	/*
	 *	Epílogo
	 */
	XClearWindow (display, dir.win);
   /***	dir_line_first = 0; ***/

	dir_scroll.button_no	= 0;
	dir_scroll.last_offset	= -1;

	draw_dir_win ();

	if (count == 1)
		draw_msg_text ("Removido o %s \"%s\"", isdir ? "diretório" : "arquivo", last_path);
	else
		draw_msg_text ("Removidos %d arquivos/diretórios", count);

	/*
	 *	Verifica se redesenha a árvore
	 */
	if (pp->n_ls.st_nlink == 2 || pp->n_expanded)
		{ XClearWindow (display, tree.win); draw_tree_win (); }

}	/* end rm_file_dir_1 */

/*
 ****************************************************************
 *	Remove um arquivo					*
 ****************************************************************
 */
int
rm_file_dir_2 (NODE *np)
{
	NODE		*pp = np->n_parent;
	int		isdir;

	/*
	 *	Prólogo
	 */
	if (isdir = S_ISDIR (np->n_ls.st_mode))
	{
		if (rmdir (np->n_path) < 0)
		    { draw_error_msg_text ("*Não consegui remover o diretório \"%s\"", np->n_path); return (-1); }

		if (np->n_selected)
			selected_dir_cnt--;
	}
	else	/* NÃO diretório */
	{
		if (unlink (np->n_path) < 0)
		    { draw_error_msg_text ("*Não consegui remover o arquivo \"%s\"", np->n_path); return (-1); }

		if (np->n_selected)
			selected_file_cnt--;
	}

	if (np->n_back == NONODE)
		dir_win_first_son = pp->n_children = np->n_forw;
	else
		np->n_back->n_forw = np->n_forw;

	if (np->n_forw != NONODE)
		np->n_forw->n_back = np->n_back;

	free (np);

	pp->n_child_cnt--;

	/*
	 *	Se removeu um diretório, ...
	 */
	if (!isdir)
		return (0);

	pp->n_ls.st_nlink -= 1;		/* O diretório pai tem menos um filho */
   	pp->n_s.st_nlink  -= 1;

	for (/* acima */; pp != NONODE; pp = pp->n_parent)
	{
		pp->n_line_size -= 1;

		if (!pp->n_expanded)
			break;
	}

	return (0);

}	/* end rm_file_dir_2 */
