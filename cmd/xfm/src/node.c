/*
 ****************************************************************
 *								*
 *			node.c					*
 *								*
 *	Gerenciador de arquivos					*
 *								*
 *	Versão	4.5.0, de 24.06.03				*
 *		4.5.0, de 18.11.03				*
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

#include <sys/common.h>
#include <sys/sync.h>
#include <sys/mntent.h>
#include <sys/sb.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <stat.h>
#include <dirent.h>

#include <X11/Intrinsic.h>

#include "../h/xfm.h"

/*
 ****************************************************************
 *	Cria a primeira ÁRVORE da RAIZ				*
 ****************************************************************
 */
void
create_root_tree (void)
{
	NODE		*np;

	if ((np = malloc (sizeof (NODE) + sizeof ("/"))) == NONODE)
		error (NOSTR);

	memsetl (np, 0, sizeof (NODE) / sizeof (long));

	np->n_path	= (char *)np + sizeof (NODE);
	np->n_path_len	= 1;
	np->n_nm	= np->n_path;
	np->n_nm_len	= 1;
   /***	np->n_ls	= 0;		***/
   /***	np->n_s		= 0;		***/
   /***	np->n_forw	= NONODE;	***/
   /***	np->n_back	= NONODE;	***/
   /***	np->n_parent	= NONODE;	***/
   /***	np->n_children	= NONODE;	***/
   /***	np->n_child_cnt	= 0;		***/
	np->n_line_size	= 1;
   /***	np->n_ximage	= NULL;		***/
   /***	np->n_line_first = 0;		***/
   /***	np->n_sb_flags	= 0;		***/

	np->n_expanded	= 1;
	np->n_exec	= EXEC_NO_EXEC;
   /***	np->n_selected	= 0;		***/
   /***	np->n_mnt_point	= 0;		***/

	/*
	 *	Completa o caminho e nome
	 */
	strcpy (np->n_path, "/");

	if (lstat ("/", &np->n_ls) < 0)
		error ("$*Não consegui obter o estado de \"/\"");

	memmove (&np->n_s, &np->n_ls, sizeof (STAT));

	np->n_sb_flags = get_mount_flags (np->n_ls.st_dev);

	create_child_dir_list (np);

	root_node = np;

}	/* end create_root_tree */

/*
 ****************************************************************
 *	Cria novamente a ÁRVORE da RAIZ				*
 ****************************************************************
 */
void
recreate_root_tree (int value)
{
	XClearWindow (display, tree.win);
	XClearWindow (display, dir.win);

	free_node (root_node);

	tree_total_lines = 0;
   /***	tree_first_line = 0; ***/

	create_root_tree ();

	dir_win_first_son = NONODE;
	dir_win_node	  = NONODE;

	draw_tree_win ();
	draw_dir_win ();

}	/* end recreate_root_tree */

/*
 ****************************************************************
 *	Prepara a lista de um subdiretório			*
 ****************************************************************
 */
void
create_child_dir_list (NODE *pp)
{
	DIR		*dir_fd;
	const DIRENT	*dp;
	NODE		*np;

	/*
	 *	Abre o diretório dado
	 */
	if ((dir_fd = inopendir (pp->n_s.st_dev, pp->n_s.st_ino)) == NODIR)
		{ draw_error_msg_text ("*Não consegui ler o diretório \"%s\"", pp->n_path); return; }

	/*
	 *	Le o diretório
	 */
	while ((dp = readdir (dir_fd)) != NODIRENT)
	{
		if (dp->d_name[0] == '.' && dp->d_name[1] == '\0')
			continue;

		np = create_new_node (pp, dp->d_name, dp->d_namlen);

		/* Obtém os estados */

		if   (instat (pp->n_s.st_dev, dp->d_ino, &np->n_ls) < 0)
		{
			np->n_ls.st_mode = np->n_s.st_mode  = 0;
		}
		elif (!S_ISLNK (np->n_ls.st_mode))
		{
			memmove (&np->n_s, &np->n_ls, sizeof (STAT));
		}
		elif (stat (np->n_path, &np->n_s) < 0)
		{
			np->n_s.st_mode = 0;
		}

		np->n_sb_flags = get_mount_flags (np->n_ls.st_dev);
		
		/* Insere na lista */

		insert_node_lexicografically (pp, np);

	}	/* end lendo o diretório */

	closedir (dir_fd);

}	/* end create_child_dir_list */

/*
 ****************************************************************
 *	Cria um novo NÓ						*
 ****************************************************************
 */
NODE *
create_new_node (NODE *pp, const char *dname, int d_namlen)
{
	NODE		*np;
	int		pp_path_len;

	/*
	 *	Cria o novo nó
	 */
	if ((pp_path_len = pp->n_path_len) == 1)
		pp_path_len = 0;

	if ((np = malloc (sizeof (NODE) + pp_path_len + d_namlen + 2)) == NONODE)
		error (NOSTR);

	memsetl (np, 0, sizeof (NODE) / sizeof (long));

	np->n_path	= (char *)np + sizeof (NODE);
	np->n_path_len	= pp_path_len + d_namlen + 1;
	np->n_nm	= np->n_path + pp_path_len + 1;
	np->n_nm_len 	= d_namlen;
   /***	np->n_ls	= 0;		***/
   /***	np->n_s		= 0;		***/
   /***	np->n_forw	= NONODE;	***/
   /***	np->n_back	= NONODE;	***/
	np->n_parent	= pp;
   /***	np->n_children	= NONODE;	***/
   /***	np->n_child_cnt = 0;		***/
	np->n_line_size = 1;
   /***	np->n_ximage	= NULL;		***/
   /***	np->n_line_first = 0;		***/
   /***	np->n_sb_flags	= 0;		***/

   /***	np->n_expanded	= 0;		***/
	np->n_exec	= EXEC_UNKNOWN;
   /***	np->n_selected	= 0;		***/
   /***	np->n_mnt_point	= 0;		***/

	/*
	 *	Completa o caminho e nome
	 */
	if (pp_path_len)
		strcpy (np->n_path, pp->n_path);

	np->n_path[pp_path_len] = '/';
	strcpy (np->n_path + pp_path_len + 1, dname);

	/*
	 *	Completa o ponto de montagem
	 */
	if (search_dir_in_fstab (np->n_path) != NOFSTAB)
		np->n_mnt_point	= 1;

	return (np);

}	/* end create_new_node */

/*
 ****************************************************************
 *	Insere no local correto da lista de NÓs			*
 ****************************************************************
 */
void
insert_node_lexicografically (NODE *pp, NODE *np)
{
	NODE		*lp, *ap;

	/*
	 *	Insere na lista
	 */
	if   ((lp = pp->n_children) == NONODE)
	{
		pp->n_children = np;
	}
	elif (strttcmp (np->n_nm, lp->n_nm, cmpfoldtb, cmpafoldtb) <= 0 && !streq (lp->n_nm, ".."))
	{
		np->n_forw = lp; lp->n_back = np;
		pp->n_children = np;
	}
	else
	{
		for (ap = lp, lp = lp->n_forw; lp != NONODE; ap = lp, lp = lp->n_forw)
		{
			if (strttcmp (np->n_nm, lp->n_nm, cmpfoldtb, cmpafoldtb) <= 0)
				break;
		}

		np->n_forw = lp; if (lp) lp->n_back = np;
		np->n_back = ap; ap->n_forw = np;
	}

	/*
	 *	Atualiza as contagens do diretório pai
	 */
	pp->n_child_cnt += 1;

	if (S_ISDIR (np->n_ls.st_mode) && !streq (np->n_nm, ".."))
	{
		for (/* acima */; pp != NONODE; pp = pp->n_parent)
		{
			pp->n_line_size += 1;

			if (!pp->n_expanded)
				break;
		}
	}

}	/* end insert_node_lexicografically */

/*
 ****************************************************************
 *	Verifica se o nó já existe				*
 ****************************************************************
 */
NODE *
find_node_in_children_list (NODE *pp, const char *nm)
{
	NODE		*np;

	if (pp->n_children == NONODE)
		create_child_dir_list (pp);

	if (pp->n_sb_flags & SB_CASE)
	{
#undef	DEBUG
#ifdef	DEBUG
		printf ("Diretório \"%s\", procurando por \"%s\", COM caixa\n", pp->n_nm, nm);
#endif	DEBUG
		for (np = pp->n_children; np != NONODE; np = np->n_forw)
		{
			if (streq (np->n_nm, nm))
				return (np);
		}
	}
	else
	{
#ifdef	DEBUG
		printf ("Diretório \"%s\", procurando por \"%s\", SEM caixa\n", pp->n_nm, nm);
#endif	DEBUG
		for (np = pp->n_children; np != NONODE; np = np->n_forw)
		{
			if (!strttcmp (np->n_nm, nm, cmpfoldtb, NOSTR))
				return (np);
		}
	}

	return (NONODE);

}	/* end find_node_in_children_list */

/*
 ****************************************************************
 *	Remove um diretório e seus filhos			*
 ****************************************************************
 */
void
free_node (NODE *pp)
{
	NODE		*np, *next_np;

	for (np = pp->n_children; np != NONODE; np = next_np)
	{
		if (np == dir_win_node)
			dir_win_node = NONODE;

		if (np == dir_win_first_son)
			dir_win_first_son = NONODE;

		next_np = np->n_forw; free_node (np);
	}

	free (pp);

}	/* end free_node */
