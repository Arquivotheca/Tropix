/*
 ****************************************************************
 *								*
 *			cmp.c					*
 *								*
 *	Gerenciador de arquivos					*
 *								*
 *	Versão	4.5.0, de 24.06.03				*
 *		4.5.0, de 05.09.03				*
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
entry int	redraw_tree;	/* A janela da ÁRVORE deve ser redesenhada */

entry int	redraw_dir;	/* A janela do DIRETÓRIO deve ser redesenhada */

/*
 ******	Dados de "malloc" ***************************************
 */
#define	PGSZ		4096		/* Incremento para "sbrk" */
#define	PGMASK		(PGSZ-1)	/* Máscara para "sbrk" */

#define	BUSY		1		/* O endereço é sempre par */

#define	IS_BUSY(p)	((p) &  BUSY)

#define	WITH_BUSY(p)	((STORE *)((int)(p) |  BUSY))
#define	WITHOUT_BUSY(p)	((STORE *)((int)(p) & ~BUSY))

#define	SET_BUSY(p)	((p) |=  BUSY)
#define	CLR_BUSY(p)	((p) &= ~BUSY)

typedef	union store	STORE;

union store
{
	int	m_status;		/* O bloco está ou não em uso */
	STORE	*m_ptr;			/* Aponta para o próximo bloco */
};

#define	STORE_SHIFT	2		/* LOG (2) (sizeof (STORE)) */

#define	NOSTORE		(STORE *)NULL

extern	STORE	*_malloc_base;		/* Primeiro elemento da Arena */
extern	STORE	*_malloc_top;		/* Último elemento da Arena */

extern	STORE	*_malloc_search;	/* Ptr para busca */

/*
 ******	Protótipos de funções ***********************************
 */
void		compare_tree (int expanded, NODE *old_pp, NODE *new_pp);
void		remove_node_from_list (NODE *pp, NODE *np);
void		insert_new_node_in_list (NODE *pp, NODE *new_np, NODE *old_np);

/*
 ****************************************************************
 *	Compara e atualiza a árvore				*
 ****************************************************************
 */
void
compare_root_tree (int value)
{
	const NODE	*np;
	NODE		*new_root;
	int		found = 0, total;

	if ((new_root = malloc (sizeof (NODE) + sizeof ("/"))) == NONODE)
		error (NOSTR);

	memmove (new_root, root_node, sizeof (NODE));

	new_root->n_path = (char *)new_root + sizeof (NODE);

	strcpy (new_root->n_path, "/"); new_root->n_nm = new_root->n_path;

	if (lstat ("/", &new_root->n_ls) < 0)
		draw_error_msg_text ("*Não consegui obter o estado de \"%s\"", new_root->n_path);

	memmove (&new_root->n_s, &new_root->n_ls, sizeof (STAT));

	new_root->n_sb_flags = get_mount_flags (new_root->n_ls.st_dev);

	new_root->n_children = NONODE;

	redraw_tree = 0; redraw_dir = 0;

	compare_tree (1 /* expanded */, root_node, new_root);

	free_node (new_root);

	/*
	 *	Verifica se deve redesenhar a árvore
	 */
#if (0)	/*******************************************************/
printf ("redraw_tree = %d\n", redraw_tree);
#endif	/*******************************************************/

	if (redraw_tree)
		{ XClearWindow (display, tree.win); draw_tree_win (); }

	/*
	 *	Verifica se deve redesenhar o diretório
	 */
#if (0)	/*******************************************************/
printf ("redraw_dir = %d\n", redraw_dir);
#endif	/*******************************************************/

	if (redraw_dir && dir_win_node != NONODE)
	{
		for (np = dir_win_node->n_children; np != NONODE; np = np->n_forw)
		{
			if (np == dir_win_first_son)
				{ found++; break; }
		}

		if (!found)
			dir_win_first_son = dir_win_node->n_children;

		XClearWindow (display, dir.win);
		draw_dir_win ();
	}

	/*
	 *	A mensagem, ...
	 */
	if   ((total = redraw_tree + redraw_dir) == 1)
		draw_msg_text ("Foi atualizado um arquivo");
	elif (total > 1)
		draw_msg_text ("Foram atualizados %d arquivos", total);

}	/* end compare_root_tree */

/*
 ****************************************************************
 *	Imprime o total de memória gasta			*
 ****************************************************************
 */
void
mem_function (void)
{
	STORE		*p;
	int		total = 0;

	if (chkalloc () < 0)
		{ draw_error_msg_text ("Inconsistência na lista da memória alocada"); return ; }

	for (p = _malloc_base; WITHOUT_BUSY (p->m_ptr) > p; p = WITHOUT_BUSY (p->m_ptr))
	{
		if (IS_BUSY (p->m_status))
			total += WITHOUT_BUSY (p->m_ptr) - p - sizeof (STORE);
	}

	draw_msg_text ("Memória alocada dinâmicamente = %d bytes", total);

}	/* end mem_function */

/*
 ****************************************************************
 *	Compara e atualiza a lista de um diretório		*
 ****************************************************************
 */
void
compare_tree (int expanded, NODE *old_pp, NODE *new_pp)
{
	NODE		*old_np, *new_np, *forw, *last_old_np;
	int		n;

	/*
	 *	Cria e ordena a nova lista de filhos
	 */
	if (new_pp->n_children == NONODE)
		create_child_dir_list (new_pp);

	/*
	 *	Compara os pares de filhos (pula logo o "..")
	 */
	last_old_np = old_pp->n_children; old_np = last_old_np->n_forw;

	new_np = new_pp->n_children->n_forw;

	for (EVER)
	{
		/*
		 *	Em primeiro lugar, verifica se uma das listas acabou
		 */
		if   (old_np == NONODE)
		{
			for (/* acima */; new_np != NONODE; new_np = new_np->n_forw)
			{
				if (old_pp == dir_win_node)
					redraw_dir++;

				insert_new_node_in_list (old_pp, new_np, last_old_np);

				if (S_ISDIR (new_np->n_ls.st_mode) && expanded)
					redraw_tree++;

				last_old_np = last_old_np->n_forw;
			}

			break;
		}
		elif (new_np == NONODE)
		{
			for (/* acima */; old_np != NONODE; old_np = forw)
			{
				if (old_pp == dir_win_node)
					redraw_dir++;

				if (S_ISDIR (old_np->n_ls.st_mode) && expanded)
					redraw_tree++;

				forw = old_np->n_forw;

				remove_node_from_list (old_pp, old_np);
			}

			break;
		}

		/*
		 *	Compara os NÓs
		 */
		if   ((n = strttcmp (old_np->n_nm, new_np->n_nm, cmpfoldtb, cmpafoldtb)) == 0)
		{
			/* O Nó continua presente */

			/* Lembrar o caso (improvável, mas possível) da troca de tipo de arquivo */

			if (!memeq (&old_np->n_ls, &new_np->n_ls, sizeof (STAT)))
			{
				if (old_pp == dir_win_node)
					redraw_dir++;

				if (old_np->n_ls.st_dev != new_np->n_ls.st_dev)
				{
					/* O diretório foi montado/desmontado */

					if (old_np->n_children != NONODE)
					{
						NODE		*np, *next_np;

						for (np = old_np->n_children; np != NONODE; np = next_np)
							{ next_np = np->n_forw; free_node (np); }

						old_np->n_children = NONODE;
					}

					redraw_tree++;
				}

				memmove (&old_np->n_ls, &new_np->n_ls, sizeof (STAT));

				old_np->n_ximage = NULL;
			}

			if
			(	S_ISLNK (new_np->n_ls.st_mode) &&
				!memeq (&old_np->n_s, &new_np->n_s, sizeof (STAT))
			)
			{
				if (old_pp == dir_win_node)
					redraw_dir++;

				memmove (&old_np->n_s,  &new_np->n_s,  sizeof (STAT));
			}

			if (old_np->n_children != NONODE)
			{
				int	expand = expanded ? old_np->n_expanded : 0;

				compare_tree (expand, old_np, new_np);
			}

			last_old_np = old_np; old_np = old_np->n_forw; new_np = new_np->n_forw;
		}
		elif (n < 0)						/* O Nó foi removido */
		{
			if (old_pp == dir_win_node)
				redraw_dir++;

			if (S_ISDIR (old_np->n_ls.st_mode) && expanded)
				redraw_tree++;

			forw = old_np->n_forw;

			remove_node_from_list (old_pp, old_np);

			old_np = forw;
		}
		else /* n > 0 */					/* Um novo NÓ foi inserido */
		{
			if (old_pp == dir_win_node)
				redraw_dir++;

			insert_new_node_in_list (old_pp, new_np, last_old_np);

			if (S_ISDIR (new_np->n_ls.st_mode) && expanded)
				redraw_tree++;

			last_old_np = last_old_np->n_forw;

			new_np = new_np->n_forw;
		}
		
	} 	/* for (EVER) */

}	/* end compare_tree */

/*
 ****************************************************************
 *	Remove um Nó da lista					*
 ****************************************************************
 */
void
remove_node_from_list (NODE *pp, NODE *np)
{

	if (np->n_back == NONODE)
		pp->n_children = np->n_forw;
	else
		np->n_back->n_forw = np->n_forw;

	if (np->n_forw != NONODE)
		np->n_forw->n_back = np->n_back;

	pp->n_child_cnt--;

	if (S_ISDIR (np->n_ls.st_mode))
	{
		for (/* acima */; pp != NONODE; pp = pp->n_parent)
		{
			pp->n_line_size -= 1;

			if (!pp->n_expanded)
				break;
		}
	}

	free_node (np);

}	/* end remove_node_from_list */

/*
 ****************************************************************
 *	Insere um novo Nó na lista				*
 ****************************************************************
 */
void
insert_new_node_in_list (NODE *pp, NODE *new_np, NODE *old_np)
{
	NODE		*np;

	/*
	 *	Aloca o novo nó
	 */
	np = create_new_node (pp, new_np->n_nm, new_np->n_nm_len);

	memmove (&np->n_ls, &new_np->n_ls, sizeof (STAT));
	memmove (&np->n_s,  &new_np->n_s,  sizeof (STAT));

	/*
	 *	Insere na lista
	 */
	np->n_forw = old_np->n_forw;

	if (old_np->n_forw != NONODE)
		old_np->n_forw->n_back = np;

	old_np->n_forw = np; np->n_back = old_np;

	pp->n_child_cnt++;

	if (S_ISDIR (np->n_ls.st_mode))
	{
		for (/* acima */; pp != NONODE; pp = pp->n_parent)
		{
			pp->n_line_size += 1;

			if (!pp->n_expanded)
				break;
		}
	}

}	/* end insert_new_node_in_list */
