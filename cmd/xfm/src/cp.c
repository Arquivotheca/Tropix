/*
 ****************************************************************
 *								*
 *			cp.c					*
 *								*
 *	Gerenciador de arquivos					*
 *								*
 *	Versão	4.5.0, de 29.09.03				*
 *		4.8.0, de 17.04.05				*
 *								*
 *	Módulo: xfm						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2005 NCE/UFRJ - tecle "man licença"	*
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
enum {	BIG_SIZE = 16 * BL4SZ };

/*
 ******	Estado da Cópia *****************************************
 */
entry NODE		*copy_next_src_np,
			*copy_src_dir_np, *copy_src_sub_dir_np,
			*copy_dst_dir_np, *copy_dst_sub_dir_np;

entry const char	*copy_last_path;

entry int		copy_copied_cnt;

/*
 ******	Estado da Comparação ************************************
 */
entry NODE		*cmp_next_src_np,
			*cmp_src_dir_np, *cmp_src_sub_dir_np,
			*cmp_dst_dir_np, *cmp_dst_sub_dir_np;

entry const char	*cmp_last_path;

entry int		cmp_compared_cnt;

/*
 ******	Estrutura dos "elos físicos" ****************************
 */
typedef struct litem	LITEM;

struct litem
{
	dev_t		l_src_dev;	/* Dispositivo fonte */
	ino_t		l_src_ino;	/* No. do Inode fonte */
	dev_t		l_dst_dev;	/* Dispositivo fonte */
	ino_t		l_dst_ino;	/* No. do Inode fonte */
	const char	*l_dst_path;	/* Caminho destino */
	LITEM		*l_next;	/* o Proximo */
};

entry LITEM	*linklist;		/* Comeco da Lista */

#define	NOLITEM	(LITEM *)NULL

/*
 ******	Os padrões **********************************************
 */
entry const char	*inc_pat[] =
{
	"*.[cshryv]",
	"*[Mm]akefile",
	NOSTR
};

/*
 ******	Dados da janela da PERCENTAGEM **************************
 */
entry int		percent_old_percent;	/* de 0 a 1000 */
entry int		percent_old_width;
entry int		percent_win_open;

/*
 ******	Protótipos de funções ***********************************
 */
void		copy_file_1 (int);
void		copy_tree_1 (int);
int		copy_file_2 (const NODE *src_file_np, NODE *dst_np);
void		cmp_file_1 (int);
void		cmp_tree_1 (int);
int		cmp_file_2 (const NODE *src_np, const NODE *dst_np);
void		adjust_obj_mod (void *src_area, void *dst_area);
LITEM		*search_link (const STAT *src_sp);
LITEM		*search_and_insert_link (const STAT *src_sp, const STAT *dst_sp, const char *dst_path);
void		free_link_list (void);
int		pattern_accept (const char *file_nm);
NODE		*create_subdir (NODE *pp, const NODE *son_np);
void		open_percent_win (void);
void		close_percent_win (void);
void		draw_percent_frame (int present);
void		draw_percent_percent (int done, int total);

/*
 ****************************************************************
 *	Verifica se é permitida a cópia				*
 ****************************************************************
 */
int
permission_copy (const NODE *dir_np)
{
	if (dir_win_node == NONODE)
		return (-1);

	if (selected_file_cnt + selected_dir_cnt > 0)
		return (1);

	return (-1);

}	/* end permission_copy */

/*
 ****************************************************************
 *	Prepara a comparação de arquivos ou de um  diretório	*
 ****************************************************************
 */
void
copy_file_0 (void)
{
	const MENUDATA	*mp = &menudata[present_menu];
	NODE		*src_np, *np;

	/*
	 *	Prólogo
	 */
	XUnmapWindow (display, tree_menu.win);

	draw_tree_win (); draw_dir_win (); draw_scroll_win (&tree_scroll);

	free_link_list ();

	if (dir_win_node == NONODE)
		return;

	if ((copy_next_src_np = dir_win_node->n_children) == NONODE)
		return;

	if ((copy_dst_dir_np = mp->menu_node) == NONODE)
		return;

	copy_copied_cnt = 0;

	/*
	 *	Verifica se foi dado um diretório fonte
	 */
	if ((src_np = select_dir_node) != NONODE)
	{
		if (src_np == copy_dst_dir_np)
		{
			draw_error_msg_text ("O diretórios fonte é o mesmo do destino");
			return;
		}

		/* Verifica se o subdiretório destino já existe */

		if ((np = find_node_in_children_list (copy_dst_dir_np, src_np->n_nm)) == NONODE)
		{
			if ((np = create_subdir (copy_dst_dir_np, src_np)) == NONODE)
				return;

			if (copy_dst_dir_np->n_expanded)
			{
				XClearWindow (display, tree.win);
				draw_tree_win ();
			}
		}

		copy_src_dir_np	    = src_np;
		copy_dst_sub_dir_np = np;
		copy_src_sub_dir_np = copy_src_dir_np;

		if (src_np->n_children == NONODE)
			create_child_dir_list (src_np);

		copy_next_src_np = src_np->n_children;

		msg_func = copy_tree_1;

		copy_tree_1 (0);
	}
	else		/* Arquivos */
	{
		msg_func = copy_file_1;

		copy_file_1 (0);
	}

}	/* end copy_file_0 */

/*
 ****************************************************************
 *	Ciclo de cópias, chamado também por "draw_question..."	*
 ****************************************************************
 */
void
copy_file_1 (int status)
{
	NODE		*src_np, *dst_np;
	char		created;
	int		code;

	/*
	 *	Verifica se foi chamado pela função de erro para pular
	 *
	 *	status:		-1: chamada por "question" com NÃO
	 *			+1: chamada por "question" com SIM
	 *			 0: chamada por "copy_file_0"
	 *
	 */
	if (copy_dst_dir_np == NONODE)
		return;

	/*
	 *	Copia os arquivos selecionados
	 */
	for (src_np = copy_next_src_np; src_np != NONODE; copy_next_src_np = src_np = src_np->n_forw)
	{
		if (cancel_event_pending ())
			goto cancel;

		if (!src_np->n_selected)
			continue;

		if (S_ISDIR (src_np->n_ls.st_mode))
			continue;

		/*
		 *	Obtém o nó destino
		 */
		if ((dst_np = find_node_in_children_list (copy_dst_dir_np, src_np->n_nm)) == NONODE)
		{
			dst_np = create_new_node (copy_dst_dir_np, src_np->n_nm, src_np->n_nm_len);

			created = 1;
		}
		else
		{
			created = 0;
		}

		/*
		 *	Guarda as informações do elo físico
		 */
		if (src_np->n_ls.st_nlink > 1)
			search_and_insert_link (&src_np->n_ls, &dst_np->n_ls, dst_np->n_path);

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
		 *	Copia o arquivo
		 */
		if ((code = copy_file_2 (src_np, dst_np)) < 0)
		{
			if (created)
				free (dst_np);

			if (code == -2)
				goto cancel;

			continue;
		}

		/* A cópia foi realizada com sucesso */

		if (created)
			insert_node_lexicografically (copy_dst_dir_np, dst_np);

		copy_last_path = src_np->n_path; copy_copied_cnt += 1;

	}	/* end for (nós do diretório) */

	/*
	 *	Imprime o resultado
	 */
	if   (copy_copied_cnt == 1)
	{
		draw_msg_text
		(	"Foi copiado o arquivo \"%s\" para o diretório \"%s\"",
			copy_last_path, copy_dst_dir_np->n_path
		);
	}
	elif (copy_copied_cnt > 1)
	{
		draw_msg_text
		(	"Foram copiados %d arquivos para o diretório \"%s\"",
			copy_copied_cnt, copy_dst_dir_np->n_path
		);
	}

    cancel:
	copy_dst_dir_np = NONODE;

	free_link_list ();

}	/* end copy_file_1 */

/*
 ****************************************************************
 *	Cópia de uma árvore					*
 ****************************************************************
 */
void
copy_tree_1 (int status)
{
	NODE		*src_np, *dst_np, *np;
	char		created;
	int		code;

	/*
	 *	Verifica se foi chamado pela função de erro para pular
	 *
	 *	status:		-1: chamada por "question" com NÃO
	 *			+1: chamada por "question" com SIM
	 *			 0: chamada por "copy_file_0"
	 *
	 */
	if (copy_dst_dir_np == NONODE)
		return;

	/*
	 *	Copia uma árvore
	 */
    again:
	for (src_np = copy_next_src_np; src_np != NONODE; copy_next_src_np = src_np = src_np->n_forw)
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

			if (hidden_in_use == DOT_FLAG_OFF)
				continue;
		}

		/*
		 *	Verifica se achou um subdiretório
		 */
		if (S_ISDIR (src_np->n_ls.st_mode))
		{
			/* Verifica se o subdiretório já existe */

			if ((np = find_node_in_children_list (copy_dst_sub_dir_np, src_np->n_nm)) == NONODE)
			{
				if ((np = create_subdir (copy_dst_sub_dir_np, src_np)) == NONODE)
					return;
			}

			copy_dst_sub_dir_np = np;
			copy_src_sub_dir_np = src_np;

			if (src_np->n_children == NONODE)
				create_child_dir_list (src_np);

			copy_next_src_np = src_np->n_children->n_forw;

			goto again;
		}

		/*
		 *	Analisa os padrões
		 */
		if (pattern_in_use == PATTERN_SOURCE)
		{
			if (!pattern_accept (src_np->n_nm))
				continue;
		}

		/*
		 *	Obtém o nó destino
		 */
		if ((dst_np = find_node_in_children_list (copy_dst_sub_dir_np, src_np->n_nm)) == NONODE)
		{
			dst_np = create_new_node (copy_dst_sub_dir_np, src_np->n_nm, src_np->n_nm_len);

			created = 1;
		}
		else
		{
			created = 0;
		}

		/*
		 *	Guarda as informações do elo físico
		 */
		if (src_np->n_ls.st_nlink > 1)
			search_and_insert_link (&src_np->n_ls, &dst_np->n_ls, dst_np->n_path);

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
		 *	Copia o arquivo
		 */
		if ((code = copy_file_2 (src_np, dst_np)) < 0)
		{
			if (created)
				free (dst_np);

			if (code == -2)
				goto cancel;

			continue;
		}

		/* A cópia foi realizada com sucesso */

		if (created)
			insert_node_lexicografically (copy_dst_sub_dir_np, dst_np);

		copy_copied_cnt += 1;

	}	/* for (lista de arquivos fonte) */

	/*
	 *	Verifica se deve subir de nível
	 */
	if (copy_src_sub_dir_np != copy_src_dir_np)
	{
		copy_next_src_np = copy_src_sub_dir_np->n_forw;

		copy_src_sub_dir_np = copy_src_sub_dir_np->n_parent;
		copy_dst_sub_dir_np = copy_dst_sub_dir_np->n_parent;

		goto again;
	}

	/*
	 *	Imprime o resultado
	 */
	if   (copy_copied_cnt == 1)
	{
		draw_msg_text
		(	"Foi copiado um arquivo para a árvore \"%s\"",
			copy_dst_dir_np->n_path
		);
	}
	elif (copy_copied_cnt > 1)
	{
		draw_msg_text
		(	"Foram copiados %d arquivos para a árvore \"%s\"",
			copy_copied_cnt, copy_dst_dir_np->n_path
		);
	}

    cancel:
	copy_dst_dir_np = NONODE;

	free_link_list ();

}	/* end copy_tree_1 */

/*
 ****************************************************************
 *	Copia um arquivo					*
 ****************************************************************
 */
int
copy_file_2 (const NODE *src_np, NODE *dst_np)
{
	const NODE	*error_np = dst_np;
	const char	*text = "???";
	LITEM		*lp = NOLITEM;
	int		n, fd_in = -1, fd_out = -1, code = -1;
	long		src_mode, dst_mode, offset = 0;
	char		big_file = 0;
	char		area[BL4SZ];

	/*
	 *	Imprime a mensagem
	 */
	draw_msg_text ("Copiando \"%s\"", src_np->n_path);
	XFlush (display);

#ifdef	DEBUG
	printf ("copy_file_2: Copiando \"%s\" em \"%s\"\n",  src_np->n_path, dst_np->n_path);
#endif	DEBUG

	/*
	 *	Prólogo
	 */
	src_mode = src_np->n_ls.st_mode & S_IFMT;
	dst_mode = dst_np->n_ls.st_mode & S_IFMT;

	/*
	 *	Processa o elo físico
	 */
	if
	(	src_np->n_ls.st_nlink > 1 && (lp = search_link (&src_np->n_ls)) != NOLITEM &&
		!streq (lp->l_dst_path, dst_np->n_path)
	)
	{
#ifdef	DEBUG
		printf ("copy_file_2: Encontrei \"%s\"\n",  lp->l_dst_path);
#endif	DEBUG
		if   (dst_mode == S_IFDIR)
		{
			if (rmdir (dst_np->n_path) < 0)
				{ text = "*NÃO consegui remover o diretório \"%s\""; goto bad; }
		}
		elif (dst_mode != 0)
		{
			if (unlink (dst_np->n_path) < 0)
				{ text = "*NÃO consegui remover o arquivo \"%s\""; goto bad; }
		}
#ifdef	DEBUG
		printf ("copy_file_2: LINK de \"%s\" para \"%s\"\n",  lp->l_dst_path, dst_np->n_path);
#endif	DEBUG

		if (link (lp->l_dst_path, dst_np->n_path) < 0)
			{ text = "*NÃO criar o elo físico para \"%s\""; goto bad; }

		goto good;
	}

	/*
	 *	Remove o arquivo destino anterior, se for o caso ...
	 */
	if (dst_mode != 0)
	{
		if (src_mode != S_IFREG || dst_mode != S_IFREG)
		{
			if   (dst_mode == S_IFDIR)
			{
				if (rmdir (dst_np->n_path) < 0)
					{ text = "*NÃO consegui remover o diretório \"%s\""; goto bad; }
			}
			else
			{
				if (unlink (dst_np->n_path) < 0)
					{ text = "*NÃO consegui remover o arquivo \"%s\""; goto bad; }
			}
		}
	}

	/*
	 *	Copia o arquivo
	 */
	switch (src_mode)
	{
	    case S_IFREG:
		if ((fd_in = inopen (src_np->n_ls.st_dev, src_np->n_ls.st_ino)) < 0)
			{ error_np = src_np; text = "*NÃO consegui abrir o arquivo \"%s\""; goto bad; }

		if ((fd_out = creat (dst_np->n_path, 0644)) < 0)
			{ text = "*NÃO consegui criar o arquivo \"%s\""; goto bad; }

		if (src_np->n_ls.st_size >= BIG_SIZE)
			{ big_file = 1; open_percent_win (); }

		while ((n = read (fd_in, area, BL4SZ)) > 0)
		{
			if (big_file)
			{
				draw_percent_percent (offset, src_np->n_ls.st_size);

				if (cancel_event_pending ())
				{
					close_percent_win (); unlink (dst_np->n_path);
					text = "Cópia cancelada"; code = -2; goto bad;
				}
			} 

			offset += BL4SZ;

			if (write (fd_out, area, n) != n)
				{ text = "*Erro de escrita em \"%s\""; goto bad; }
		}

		if (n < 0)
			{ error_np = src_np; text = "*Erro de leitura de \"%s\""; goto bad; }

		close (fd_in); close (fd_out);

		if (big_file)
			close_percent_win ();

		break;

	    case S_IFBLK:
	    case S_IFCHR:
	    case S_IFIFO:
		if (mknod (dst_np->n_path, src_np->n_ls.st_mode, src_np->n_ls.st_rdev) < 0)
			{ text = "*NÃO consegui criar o arquivo \"%s\""; goto bad; }

		break;

	    case S_IFLNK:
	    {
		char		*symlink_nm;

		symlink_nm = alloca (src_np->n_ls.st_size + 1);

		if (readlink (src_np->n_path, symlink_nm, src_np->n_ls.st_size + 1) < 0)
		{
			error_np = src_np;
			text = "*Não consegui obter o conteúdo do elo simbólico de \"%s\"";
			goto bad;
		}

		if (symlink (symlink_nm, dst_np->n_path) < 0)
			{ text = "*Não consegui criar o elo simbólico \"%s\""; goto bad; }

		break;
	    }

	}	/* end switch (mode) */

	/*
	 *	A cópia foi realizada com sucesso
	 */
    good:
	if (lstat (dst_np->n_path, &dst_np->n_ls) < 0)
		draw_msg_text ("*Não consegui obter o estado de \"%s\"", dst_np->n_path);

	memmove (&dst_np->n_s, &dst_np->n_ls, sizeof (STAT));

	dst_np->n_sb_flags = get_mount_flags (dst_np->n_ls.st_dev);

	/*
	 *	Atualiza o lista de elos físicos
	 */
	if (lp != NOLITEM)
	{
		lp->l_dst_dev  = dst_np->n_ls.st_dev;
		lp->l_dst_ino  = dst_np->n_ls.st_ino;
	}

	return (0);

	/*
	 *	Em caso de erro, ...
	 */
    bad:
	if (fd_in >= 0)
		close (fd_in);

	if (fd_out >= 0)
		close (fd_out);

	draw_error_msg_text (text, error_np->n_path);

	return (code);

}	/* end copy_file_2 */

/*
 ****************************************************************
 *	Prepara a comparação de arquivos ou de um  diretório	*
 ****************************************************************
 */
void
cmp_file_0 (void)
{
	const MENUDATA	*mp = &menudata[present_menu];
	NODE		*src_np;

	/*
	 *	Prólogo
	 */
	XUnmapWindow (display, tree_menu.win);

	draw_tree_win (); draw_dir_win (); draw_scroll_win (&tree_scroll);

	free_link_list ();

	if (dir_win_node == NONODE)
		return;

	if ((cmp_next_src_np = dir_win_node->n_children) == NONODE)
		return;

	if ((cmp_dst_dir_np = mp->menu_node) == NONODE)
		return;

	cmp_compared_cnt = 0;

	/*
	 *	Verifica se foi dado um diretório fonte
	 */
	if ((src_np = select_dir_node) != NONODE)
	{
		if (src_np == cmp_dst_dir_np)
		{
			draw_error_msg_text ("A árvore fonte e destino são as mesmas");
			return;
		}

		cmp_src_dir_np	   = src_np;
		cmp_dst_sub_dir_np = cmp_dst_dir_np;
		cmp_src_sub_dir_np = cmp_src_dir_np;

		if (src_np->n_children == NONODE)
			create_child_dir_list (src_np);

		cmp_next_src_np = src_np->n_children;

		msg_func = cmp_tree_1;

		cmp_tree_1 (0);
	}
	else		/* Arquivos */
	{
		msg_func = cmp_file_1;

		cmp_file_1 (0);
	}

}	/* end cmp_file_0 */

/*
 ****************************************************************
 *	Ciclo de comparações de arquivos			*
 ****************************************************************
 */
void
cmp_file_1 (int status)
{
	NODE		*src_np, *dst_np;
	char		created, canceled = 0;
	int		code;

	/*
	 *	Verifica se foi chamado pela função de erro para pular
	 *
	 *	status:		-1: chamada por "question" com NÃO
	 *			+1: chamada por "question" com SIM
	 *			 0: chamada por "cmp_file_0"
	 *
	 */
	if (cmp_dst_dir_np == NONODE)
		return;

	/*
	 *	Compara os arquivos selecionados
	 */
	for (src_np = cmp_next_src_np; src_np != NONODE; cmp_next_src_np = src_np = src_np->n_forw)
	{
		if (cancel_event_pending ())
			goto cancel;

		if (!src_np->n_selected)
			continue;

		if (S_ISDIR (src_np->n_ls.st_mode))
			continue;

		/*
		 *	Obtém o nó destino
		 */
		if ((dst_np = find_node_in_children_list (cmp_dst_dir_np, src_np->n_nm)) == NONODE)
		{
			dst_np = create_new_node (cmp_dst_dir_np, src_np->n_nm, src_np->n_nm_len);

			created = 1;
		}
		else
		{
			created = 0;
		}

		/*
		 *	Compara os arquivos
		 */
		if   (status == 0)
		{
			/* NÃO foi chamada por "draw_question_msg_text" */

			cmp_last_path = src_np->n_path; cmp_compared_cnt += 1;

			switch (cmp_file_2 (src_np, dst_np))
			{
			    case 0:
				continue;

			    case -1:
				goto cancel;
			}

			if (created)
				free (dst_np);

			return;
		}
		elif (status < 0)
		{
			if (created)
				free (dst_np);

			status = 0; continue;
		}
		else 	/* status > 0 */
		{
			status = 0;
		}

		/*
		 *	Copia o arquivo
		 */
		if ((code = copy_file_2 (src_np, dst_np)) < 0)
		{
			if (created)
				free (dst_np);

			if (code == -2)
				goto cancel;

			continue;
		}

		/* A cópia foi realizada com sucesso */

		if (created)
			insert_node_lexicografically (cmp_dst_dir_np, dst_np);

	}	/* end for (nós do diretório) */

	/*
	 *	Imprime o resultado
	 */
	if   (canceled)
	{
	    cancel:
		draw_error_msg_text
		(	"Comparação cancelada"
		);
	}
	elif (cmp_compared_cnt == 1)
	{
		draw_msg_text
		(	"Foi comparado o arquivo \"%s\" com o diretório \"%s\"",
			cmp_last_path, cmp_dst_dir_np->n_path
		);
	}
	elif (cmp_compared_cnt > 1)
	{
		draw_msg_text
		(	"Foram comparados %d arquivos com o diretório \"%s\"",
			cmp_compared_cnt, cmp_dst_dir_np->n_path
		);
	}

	cmp_dst_dir_np = NONODE;

	free_link_list ();

}	/* end cmp_file_1 */

/*
 ****************************************************************
 *	Comparações de uma árvore				*
 ****************************************************************
 */
void
cmp_tree_1 (int status)
{
	NODE		*src_np, *dst_np, *np;
	char		created, canceled = 0;
	int		code;

	/*
	 *	Verifica se foi chamado pela função de erro para pular
	 *
	 *	status:		-1: chamada por "question" com NÃO
	 *			+1: chamada por "question" com SIM
	 *			 0: chamada por "cmp_file_0"
	 *
	 */
	if (cmp_dst_dir_np == NONODE)
		return;

	/*
	 *	Compara a árvore
	 */
    again:
	for (src_np = cmp_next_src_np; src_np != NONODE; cmp_next_src_np = src_np = src_np->n_forw)
	{
		if (cancel_event_pending ())
			goto cancel;

		/*
		 *	Pula "." e ".."
		 */
		if (src_np->n_nm[0] == '.')
		{
			if (src_np->n_nm[1] == '\0')
				continue;

			if (src_np->n_nm[1] == '.' && src_np->n_nm[2] == '\0')
				continue;

			if (hidden_in_use == DOT_FLAG_OFF)
				continue;
		}

		/*
		 *	Verifica se achou um subdiretório
		 */
		if   (S_ISDIR (src_np->n_ls.st_mode))
		{
			/* Verifica se o subdiretório já existe */

			if ((np = find_node_in_children_list (cmp_dst_sub_dir_np, src_np->n_nm)) == NONODE)
			{
				if ((np = create_subdir (cmp_dst_sub_dir_np, src_np)) == NONODE)
					return;
			}

			cmp_dst_sub_dir_np = np;
			cmp_src_sub_dir_np = src_np;

			if (src_np->n_children == NONODE)
				create_child_dir_list (src_np);

			cmp_next_src_np = src_np->n_children->n_forw;

			goto again;
		}

		/*
		 *	Analisa os padrões
		 */
		if (pattern_in_use == PATTERN_SOURCE)
		{
			if (!pattern_accept (src_np->n_nm))
				continue;
		}

		/*
		 *	Obtém o nó destino
		 */
		if ((dst_np = find_node_in_children_list (cmp_dst_sub_dir_np, src_np->n_nm)) == NONODE)
		{
			dst_np = create_new_node (cmp_dst_sub_dir_np, src_np->n_nm, src_np->n_nm_len);

			created = 1;
		}
		else
		{
			created = 0;
		}

		/*
		 *	Compara os arquivos
		 */
		if   (status == 0)
		{
			/* NÃO foi chamada por "draw_question_msg_text" */

			cmp_compared_cnt += 1;

			switch (cmp_file_2 (src_np, dst_np))
			{
			    case 0:
				continue;

			    case -1:
				goto cancel;
			}

			if (created)
				free (dst_np);

			return;
		}
		elif (status < 0)
		{
			if (created)
				free (dst_np);

			status = 0; continue;
		}
		else 	/* status > 0 */
		{
			status = 0;
		}

		/*
		 *	Copia o arquivo
		 */
		if ((code = copy_file_2 (src_np, dst_np)) < 0)
		{
			if (created)
				free (dst_np);

			if (code == -2)
				goto cancel;

			continue;
		}

		/* A cópia foi realizada com sucesso */

		if (created)
			insert_node_lexicografically (cmp_dst_sub_dir_np, dst_np);

	}	/* for (lista de arquivos fonte) */

	/*
	 *	Verifica se deve subir de nível
	 */
	if (cmp_src_sub_dir_np != cmp_src_dir_np)
	{
		cmp_next_src_np = cmp_src_sub_dir_np->n_forw;

		cmp_src_sub_dir_np = cmp_src_sub_dir_np->n_parent;
		cmp_dst_sub_dir_np = cmp_dst_sub_dir_np->n_parent;

		goto again;
	}

	/*
	 *	Imprime o resultado
	 */
	if   (canceled)
	{
	    cancel:
		draw_error_msg_text
		(	"Comparação cancelada"
		);
	}
	elif   (cmp_compared_cnt == 1)
	{
		draw_msg_text
		(	"Foi comparado um arquivo com a árvore \"%s\"",
			cmp_dst_dir_np->n_path
		);
	}
	elif (cmp_compared_cnt > 1)
	{
		draw_msg_text
		(	"Foram comparados %d arquivos com a árvore \"%s\"",
			cmp_compared_cnt, cmp_dst_dir_np->n_path
		);
	}

	cmp_dst_dir_np = NONODE;

	free_link_list ();

}	/* end cmp_tree_1 */

/*
 ****************************************************************
 *	Compara dois arquivo					*
 ****************************************************************
 */
int
cmp_file_2 (const NODE *src_np, const NODE *dst_np)
{
	const char	*error_path = dst_np->n_path, *text = "???";
	const LITEM	*lp = NOLITEM;
	int		src_n, dst_n, src_fd = -1, dst_fd = -1, i = 0, rm = 0;
	long		src_mode, offset = 0;
	char		big_file = 0;
	char		src_area[BL4SZ], dst_area[BL4SZ];

	/*
	 *	Imprime a mensagem
	 */
	draw_msg_text ("Comparando \"%s\"", src_np->n_path);
	XFlush (display);

#ifdef	DEBUG
	printf ("Comparando \"%s\" com \"%s\"\n", src_np->n_path, dst_np->n_path);
#endif	DEBUG

	/*
	 *	Guarda as informações do elo físico
	 */
	if (src_np->n_ls.st_nlink > 1)
		lp = search_and_insert_link (&src_np->n_ls, &dst_np->n_ls, dst_np->n_path);

	/*
	 *	Verifica a existência do arquivo destino
	 */
	if (dst_np->n_ls.st_mode == 0)
		{ text = "O arquivo \"%s\" NÃO existe"; rm = 1; msg_rm_node = src_np; goto bad; }

	/*
	 *	Verifica se é o próprio arquivo da árvore fonte
	 */
	if (dst_np->n_ls.st_dev == src_np->n_ls.st_dev && dst_np->n_ls.st_ino == src_np->n_ls.st_ino)
		return (0);

	/*
	 *	Verifica se é um elo físico
	 */
	if (lp != NOLITEM)
	{
		if (lp->l_dst_dev == dst_np->n_ls.st_dev && lp->l_dst_ino == dst_np->n_ls.st_ino)
			return (0);

		text = "O elo físico para \"%s\" NÃO confere"; goto bad;
	}

	/*
	 *	Compara o modo do arquivo
	 */
	if ((src_mode = src_np->n_ls.st_mode & S_IFMT) != (dst_np->n_ls.st_mode & S_IFMT))
		{ text = "O arquivo \"%s\" é de tipo diferente"; goto bad; }

	/*
	 *	Compara o arquivo
	 */
	switch (src_mode)
	{
	    case S_IFREG:
		if ((src_fd = inopen (src_np->n_ls.st_dev, src_np->n_ls.st_ino)) < 0)
			{ error_path = src_np->n_path; text = "*NÃO consegui abrir o arquivo \"%s\""; goto bad; }

		if ((dst_fd = inopen (dst_np->n_ls.st_dev, dst_np->n_ls.st_ino)) < 0)
			{ text = "*NÃO consegui abrir o arquivo \"%s\""; goto bad; }

		if (src_np->n_ls.st_size >= BIG_SIZE) 
			{ big_file = 1; open_percent_win (); }

		while ((src_n = read (src_fd, src_area, BL4SZ)) > 0)
		{
#if (0)	/*******************************************************/
			if (offset != 0 && (offset & (BIG_SIZE - 1)) == 0)
#endif	/*******************************************************/
			if (big_file)
			{
				draw_percent_percent (offset, src_np->n_ls.st_size);

				if (cancel_event_pending ())
					{ close_percent_win (); return (-1); }
			}

			offset += BL4SZ;

			if ((dst_n = read (dst_fd, dst_area, src_n)) < 0)
				{ text = "*Erro de leitura de \"%s\""; goto bad; }

			if (dst_n != src_n)
				{ text = "O Arquivo destino \"%s\" é menor"; goto bad; }

			if (i++ == 0)
				adjust_obj_mod (src_area, dst_area);

			if (!memeq (src_area, dst_area, src_n))
				{ text = "O Arquivo destino \"%s\" é diferente"; goto bad; }
		}

#if (0)	/*******************************************************/
		if (big_file)
			draw_percent_percent (1000, 1000);
#endif	/*******************************************************/

		if (src_n < 0)
			{ error_path = src_np->n_path; text = "*Erro de leitura de \"%s\""; goto bad; }

		if ((dst_n = read (dst_fd, dst_area, BL4SZ)) < 0)
			{ text = "*Erro de leitura de \"%s\""; goto bad; }

		if (dst_n > 0)
			{ text = "O Arquivo destino \"%s\" é maior"; goto bad; }

		close (src_fd); close (dst_fd);

		if (big_file)
			close_percent_win ();

		return (0);

	    case S_IFBLK:
	    case S_IFCHR:
		if (src_np->n_ls.st_rdev == dst_np->n_ls.st_rdev)
			return (0);

		text = "O Arquivo destino \"%s\" têm (MAJOR+MINOR) diferentes";
		break;

	    case S_IFIFO:
		return (0);

	    case S_IFLNK:
	    {
		char		*src_symlink_nm, *dst_symlink_nm;

		if (src_np->n_ls.st_size != dst_np->n_ls.st_size)
			{ text = "O elo simbólico de \"%s\" tem conteúdo diferente"; break; }

		src_symlink_nm = alloca (src_np->n_ls.st_size + 1);

		if (readlink (src_np->n_path, src_symlink_nm, src_np->n_ls.st_size + 1) < 0)
		{
			error_path = src_np->n_path;
			text = "*Não consegui ler o conteúdo de \"%s\"";
			break;
		}

		dst_symlink_nm = alloca (dst_np->n_ls.st_size + 1);

		if (readlink (dst_np->n_path, dst_symlink_nm, dst_np->n_ls.st_size + 1) < 0)
			{ text = "*Não consegui ler o conteúdo de \"%s\""; break; }

		if (streq (src_symlink_nm, dst_symlink_nm))
			return (0);
	    }

	}	/* end switch (src_mode) */

	/*
	 *	Em caso de erro, ...
	 */
    bad:
	if (src_fd >= 0)
		close (src_fd);

	if (dst_fd >= 0)
		close (dst_fd);

	if (big_file)
		close_percent_win ();

	draw_question_msg_text (rm, ". Atualiza?", text, error_path);

	return (1);

}	/* end cmp_file_2 */

/*
 ****************************************************************
 *	Mascara os campos não relevantes			*
 ****************************************************************
 */
void
adjust_obj_mod (void *src_area, void *dst_area)
{
	HEADER	*src_hp = src_area, *dst_hp = dst_area;

	if (src_hp->h_magic != FMAGIC && src_hp->h_magic != NMAGIC && src_hp->h_magic != SMAGIC)
		return;

	if (src_hp->h_magic != dst_hp->h_magic)
		return;

	src_hp->h_time = 0; dst_hp->h_time = 0;

}	/* end adjust_obj_mod */

/*
 ****************************************************************
 *	Procura um elo físico					*
 ****************************************************************
 */
LITEM *
search_link (const STAT *src_sp)
{
	LITEM		*lp;

	/*
	 *	Procura na lista
	 */
	for (lp = linklist; lp != NOLITEM; lp = lp->l_next)
	{
		if (lp->l_src_dev == src_sp->st_dev && lp->l_src_ino == src_sp->st_ino)
		{
#ifdef	DEBUG
			printf ("search_link: Achei \"%s\"\n", lp->l_dst_path);
#endif	DEBUG
			return (lp);
		}
	}

	return (NOLITEM);

}	/* end search_and_insert_link */

/*
 ****************************************************************
 *	Processa elos físicos					*
 ****************************************************************
 */
LITEM *
search_and_insert_link (const STAT *src_sp, const STAT *dst_sp, const char *dst_path)
{
	LITEM		*lp;

	/*
	 *	Procura na lista
	 */
	for (lp = linklist; lp != NOLITEM; lp = lp->l_next)
	{
		if (lp->l_src_dev == src_sp->st_dev && lp->l_src_ino == src_sp->st_ino)
		{
#ifdef	DEBUG
			printf ("search_and_insert_link: Achei \"%s\"\n", lp->l_dst_path);
#endif	DEBUG
			return (lp);
		}
	}

	/*
	 *	Insere na lista
	 */
	if ((lp = malloc (sizeof (LITEM) + strlen (dst_path) + 1)) == NOLITEM)
		error (NOSTR);

	lp->l_src_dev  = src_sp->st_dev;
	lp->l_src_ino  = src_sp->st_ino;
	lp->l_dst_dev  = dst_sp->st_dev;
	lp->l_dst_ino  = dst_sp->st_ino;
	lp->l_dst_path = (char *)lp + sizeof (LITEM);

	strcpy ((char *)lp->l_dst_path, dst_path);

	lp->l_next = linklist; linklist = lp;

#ifdef	DEBUG
	printf ("search_and_insert_link: Armazenei \"%s\"\n", dst_path);
#endif	DEBUG

	return (NOLITEM);

}	/* end search_and_insert_link */

/*
 ****************************************************************
 *	Libera a lista de elos físicos				*
 ****************************************************************
 */
void
free_link_list (void)
{
	LITEM		*lp, *next;

	for (lp = linklist; lp != NOLITEM; lp = next)
		{ next = lp->l_next; free (lp); }

	linklist = NOLITEM;

}	/* end free_link_list */

/*
 ****************************************************************
 *	Verifica se o nome do arquivo satisfaz um dos padrões	*
 ****************************************************************
 */
int
pattern_accept (const char *file_nm)
{
	const char	**pp;

	/*
	 *	Agora testa os padrões de inclusão
	 */
	if ((pp = inc_pat)[0] != NOSTR)
	{
		for (/* vazio */; /* vazio */; pp++)
		{
			if (*pp == NOSTR)
				return (0);

			if (patmatch (file_nm, *pp))
				break;
		}
	}

	return (1);

}	/* end pattern_accept */

/*
 ****************************************************************
 *	Cria um subdiretório					*
 ****************************************************************
 */
NODE *
create_subdir (NODE *pp, const NODE *son_np)
{
	NODE		*np;

	np = create_new_node (pp, son_np->n_nm, son_np->n_nm_len);

	if (mkdir (np->n_path, 0755) < 0)
	{
	    bad:
		draw_error_msg_text
		(	"*Não consegui criar o diretório \"%s\"",
			np->n_path
		);
		free (np); return (NONODE);
	}

	if (lstat (np->n_path, &np->n_ls) < 0)
		{ rmdir (np->n_path); goto bad; }

	memmove (&np->n_s, &np->n_ls, sizeof (STAT));

	np->n_sb_flags = get_mount_flags (np->n_ls.st_dev);

	insert_node_lexicografically (pp, np);

	pp->n_ls.st_nlink += 1;
	pp->n_s.st_nlink  += 1;

	return (np);

}	/* end create_subdir */

/*
 ****************************************************************
 *	Prepara a janela de percentagem				*
 ****************************************************************
 */
void
open_percent_win (void)
{
	percent_win_open = 1; percent_old_percent = 0;

	draw_percent_win ();

}	/* end open_percent_win */

/*
 ****************************************************************
 *	Desprepara a janela de percentagem			*
 ****************************************************************
 */
void
close_percent_win (void)
{
	percent_win_open = 0;

	XClearWindow (display, percent.win);

	draw_percent_win ();

}	/* end close_percent_win */

/*
 ****************************************************************
 *	Desenha a janela do PROGRESSO da alocação		*
 ****************************************************************
 */
void
draw_percent_win (void)
{
	draw_shadow
	(	percent.win,
		percent.triple.top_gc, percent.triple.bottom_gc,
		0, 0,
		percent.width, percent.height
	);

	if (!percent_win_open)
		return;

	XFillRectangle
	(	display, percent.win, black_triple.main_gc,
		PERCENT_BORDER + SHADOW, PERCENT_BORDER + SHADOW,
		percent.width  - 2 * (PERCENT_BORDER + SHADOW),
		percent.height - 2 * (PERCENT_BORDER + SHADOW)
	);

	draw_shadow
	(	percent.win,
		percent.triple.top_gc, percent.triple.bottom_gc,
		PERCENT_BORDER, PERCENT_BORDER,
		percent.width  - 2 * PERCENT_BORDER,
		percent.height - 2 * PERCENT_BORDER
	);

	percent_old_width = 0; draw_percent_percent (percent_old_percent, 1000);

}	/* end draw_percent_win */

/*
 ****************************************************************
 *	(Re)Desenha a janela das percentagem			*
 ****************************************************************
 */
void
draw_percent_percent (int done, int total)
{
	int		percent_new_width, percent_new_percent;

	if (total <= 0)
		return;

	percent_new_percent = (long long)done * 1000 / total;

	percent_new_width = percent_new_percent *
				(percent.width  - 2 * (PERCENT_BORDER + SHADOW)) / 1000;

	/*
	 *	Atualiza a barra de percentagem
	 */
	if   (percent_new_width == percent_old_width)
	{
		return;
	}
	elif (percent_new_width < percent_old_width)
	{
		XFillRectangle
		(	display, percent.win, black_triple.main_gc,
			PERCENT_BORDER + SHADOW + percent_new_width,
			PERCENT_BORDER + SHADOW,
			percent_old_width - percent_new_width,
			percent.height - 2 * (PERCENT_BORDER + SHADOW)
		);
	}
	else	/* percent_new_width > percent_old_width */
	{
		XFillRectangle
		(	display, percent.win, percent_gc,
			PERCENT_BORDER + SHADOW + percent_old_width,
			PERCENT_BORDER + SHADOW, 
			percent_new_width - percent_old_width,
			percent.height - 2 * (PERCENT_BORDER + SHADOW)
		);
	}

	percent_old_width = percent_new_width; percent_old_percent = percent_new_percent;

}	/* end draw_percent_percent */
