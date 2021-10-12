/*
 ****************************************************************
 *								*
 *			cp.c					*
 *								*
 *	Gerenciador de arquivos					*
 *								*
 *	Versão	4.5.0, de 20.10.03				*
 *		4.5.0, de 22.11.03				*
 *								*
 *	Módulo: xfm						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2003 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include <sys/common.h>
#include <sys/sync.h>
#include <sys/mntent.h>
#include <sys/sb.h>
#include <sys/ioctl.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <stat.h>
#include <a.out.h>
#include <fcntl.h>
#include <errno.h>

#include <X11/Intrinsic.h>

#include "../h/xfm.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */

/*
 ******	Estado da Mudança ***************************************
 */
entry NODE		*mv_next_src_np,
			*mv_dst_dir_np;

entry const char	*mv_last_path;
entry int		mv_last_isdir;

entry int		mv_copied_cnt;

/*
 ******	Protótipos de funções ***********************************
 */

void		mv_file_1 (int);
void		mv_tree_1 (int);
int		mv_file_2 (NODE *src_np, NODE *dst_np);

/*
 ****************************************************************
 *	Verifica se é permitida a mudança			*
 ****************************************************************
 */
int
permission_mv (const NODE *dir_np)
{
	if (dir_win_node == NONODE)
		return (-1);

	if (selected_file_cnt + selected_dir_cnt > 0)
		return (1);

	return (-1);

}	/* end permission_mv */

/*
 ****************************************************************
 *	Prepara a mudança de arquivos ou de um  diretório	*
 ****************************************************************
 */
void
mv_file_0 (void)
{
	const MENUDATA	*mp = &menudata[present_menu];

	/*
	 *	Prólogo
	 */
	XUnmapWindow (display, tree_menu.win);

	if (dir_win_node == NONODE)
		return;

	if ((mv_next_src_np = dir_win_node->n_children) == NONODE)
		return;

	if ((mv_dst_dir_np = mp->menu_node) == NONODE)
		return;

	if (mv_dst_dir_np == dir_win_node)
		{ draw_error_msg_text ("Os diretórios fonte e destino são idênticos"); return; }

	mv_copied_cnt = 0;

	msg_func = mv_file_1;

	mv_file_1 (0);

}	/* end mv_file_0 */

/*
 ****************************************************************
 *	Ciclo de cópias, chamado também por "draw_question..."	*
 ****************************************************************
 */
void
mv_file_1 (int status)
{
	NODE		*src_np, *dst_np, *pp;
	char		created;

	/*
	 *	Verifica se foi chamado pela função de erro para pular
	 *
	 *	status:		-1: chamada por "question" com NÃO
	 *			+1: chamada por "question" com SIM
	 *			 0: chamada por "mv_file_0"
	 *
	 */

	/*
	 *	Move os arquivos selecionados
	 */
	for (src_np = mv_next_src_np; src_np != NONODE; mv_next_src_np = src_np = src_np->n_forw)
	{
		if (!src_np->n_selected)
			continue;

		/*
		 *	Obtém o nó destino
		 */
		if ((dst_np = find_node_in_children_list (mv_dst_dir_np, src_np->n_nm)) == NONODE)
		{
			dst_np = create_new_node (mv_dst_dir_np, src_np->n_nm, src_np->n_nm_len);

			created = 1;
		}
		else
		{
			created = 0;
		}

		/*
		 *	Verifica se foi chamada por "draw_question_msg_text"
		 */
		if   (status == 0)
		{
			if (dst_np->n_ls.st_mode != 0)
			{
				draw_question_msg_text
				(	0, ". Reescreve?", "O arquivo \"%s\" já existe",
					dst_np->n_path
				);

				return;
			}
		}
		elif (status < 0)
		{
			if (created)
				free (dst_np);

			status = 0; continue;
		}
		else	/* status > 0 */
		{
			status = 0;
		}

		/*
		 *	Move o arquivo
		 */
		if (mv_file_2 (src_np, dst_np) < 0)
		{
			if (created)
				free (dst_np);

			continue;
		}

		/* A mudança foi realizada com sucesso */

		if (created)
			insert_node_lexicografically (mv_dst_dir_np, dst_np);

		if (S_ISDIR (src_np->n_ls.st_mode))
		{
			mv_dst_dir_np->n_ls.st_nlink += 1;	
		   	mv_dst_dir_np->n_s.st_nlink  += 1;

			for (pp = mv_dst_dir_np; pp != NONODE; pp = pp->n_parent)
			{
				pp->n_line_size += 1;

				if (!pp->n_expanded)
					break;
			}
		}

		mv_last_path = src_np->n_path; mv_last_isdir = S_ISDIR (src_np->n_ls.st_mode);
		mv_copied_cnt += 1;

	}	/* end for (nós do diretório) */

	/*
	 *	Imprime o resultado
	 */
	if   (mv_copied_cnt == 1)
	{
		draw_msg_text
		(	"Foi movido o %s \"%s\" para o diretório \"%s\"",
			mv_last_isdir ? "diretório" : "arquivo",
			mv_last_path, mv_dst_dir_np->n_path
		);
	}
	elif (mv_copied_cnt > 1)
	{
		draw_msg_text
		(	"Foram movidos %d arquivos para o diretório \"%s\"",
			mv_copied_cnt, mv_dst_dir_np->n_path
		);
	}

	/*
	 *	Redesenha as janelas, se necessário
	 */
	XClearWindow (display, dir.win);
   /***	dir_line_first = 0; ***/

	dir_scroll.button_no	= 0;
	dir_scroll.last_offset	= -1;

	draw_dir_win ();

	pp = mv_dst_dir_np->n_parent;

	if (pp->n_ls.st_nlink == 2 || pp->n_expanded)
	{
		XClearWindow (display, tree.win);
		draw_tree_win ();
	}

	mv_dst_dir_np = NONODE;

}	/* end mv_file_1 */

/*
 ****************************************************************
 *	Move um arquivo						*
 ****************************************************************
 */
int
mv_file_2 (NODE *src_np, NODE *dst_np)
{
	const NODE	*error_np;
	const char	*text;
	NODE		*pp = src_np->n_parent;

#ifdef	DEBUG
	printf ("mv_file_2: Move \"%s\" em \"%s\"\n",  src_np->n_path, dst_np->n_path);
#endif	DEBUG

	/*
	 *	Tenta move o arquivo
	 */
	if (rename (src_np->n_path, dst_np->n_path) < 0)
		{ error_np = src_np; text = "*NÃO consegui mover o arquivo \"%s\""; goto bad; }

	if (lstat (dst_np->n_path, &dst_np->n_ls) < 0)
		draw_msg_text ("*Não consegui obter o estado de \"%s\"", dst_np->n_path);

	memmove (&dst_np->n_s, &dst_np->n_ls, sizeof (STAT));

	dst_np->n_sb_flags = get_mount_flags (dst_np->n_ls.st_dev);

	/* Remove o nó */

	if (src_np->n_back == NONODE)
		dir_win_first_son = pp->n_children = src_np->n_forw;
	else
		src_np->n_back->n_forw = src_np->n_forw;

	if (src_np->n_forw != NONODE)
		src_np->n_forw->n_back = src_np->n_back;

	pp->n_child_cnt--;

	/*
	 *	Se removeu um diretório, ...
	 */
	if (S_ISDIR (src_np->n_ls.st_mode))
	{
		pp->n_ls.st_nlink -= 1;		/* O diretório pai tem menos um filho */
	   	pp->n_s.st_nlink  -= 1;

		for (/* acima */; pp != NONODE; pp = pp->n_parent)
		{
			pp->n_line_size -= 1;

			if (!pp->n_expanded)
				break;
		}
	}

	free (src_np);

	return (0);

	/*
	 *	Em caso de erro, ...
	 */
    bad:
	draw_error_msg_text (text, error_np->n_path);

	return (-1);

}	/* end mv_file_2 */
