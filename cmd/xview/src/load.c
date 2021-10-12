/*
 ****************************************************************
 *								*
 *			load.c					*
 *								*
 *	Gerenciador de arquivos					*
 *								*
 *	Versão	4.5.0, de 24.01.04				*
 *		4.5.0, de 24.01.04				*
 *								*
 *	Módulo: xview						*
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
#include <dirent.h>

#include <X11/Intrinsic.h>

#include "../h/xview.h"

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
#define	OK_TEXT		" OK "
#define	NEW_DIR_TEXT	" Dir"

enum {	DIR_WIDTH = 40, DIR_HEIGHT = 16 };

enum {	LOAD_LINE = 8, LOAD_WIDTH = 58, LOAD_HEIGHT = DIR_HEIGHT + 12 };

enum {	FILE_WIDTH = 40 };

enum {	DIR_WIN_LINE = 4 };

enum {	MAX_DIR_SZ = 256, DIR_LINE_SZ = 256 };

#define	DIR_TEXT	"Diretório:  "
enum {	DIR_LINE = 2 };

#define	FILE_TEXT	"Nome:       "
enum {	FILE_LINE = DIR_HEIGHT + 10 };

/*
 ****** Estrutura de um nó da árvore ****************************
 */
typedef struct node	NODE;

struct node
{
	char		*n_path;	/* Nome do caminho completo */
	int		n_path_len;	/* Comprimento do caminho completo */
	char		*n_nm;		/* Ponteiro para o último nome */
	int		n_nm_len;	/* Comprimento do último nome */
	STAT		n_ls;		/* Estado (obtido por "lstat") */
	STAT		n_s;		/* Estado (real) */
	NODE		*n_forw;	/* PrÓximo (Irmão) */
	NODE		*n_back;	/* Anterior (Irmão) */
	NODE		*n_parent;	/* Ponteiro para o pai */
	NODE		*n_children;	/* Lista de filhos */
	int		n_child_cnt;	/* No. de filhos (dir. ou não) */
	int		n_line_size;	/* No. de linhas ocupadas (com descendentes) */
	int		n_line_first;	/* Primeiro filho visível do DIRETÓRIO */

	signed char	n_good_nm;	/* Indica se é um nome conhecido */
	char		n_exec;		/* < 0: Não executável; 0: Não sabe; > 0: Executável */
	char		n_selected;	/* Indica se está selecionado para mover/copiar */
	char		n_mnt_point;	/* Indica se este diretório está em "fstab" */
};

#define	NONODE	(NODE *)NULL

/*
 ******	Outras variáveis ****************************************
 */
entry NODE		*dir_win_node;		/* Diretório da tela do DIRETÓRIO */
entry NODE		*dir_win_first_son;	/* Primeiro filho visível do DIRETÓRIO */

#define			dir_line_vis DIR_HEIGHT	/* No. de linhas visíveis na janela do diretório */
entry int		dir_line_first;		/* No. da primeira linha visível do diretório */

entry const NODE	*dir_node_selected;

entry const char	*file_nm = "";

entry int		orig_width, orig_height;

/*
 ******	Protótipos de funções ***********************************
 */
void		read_directory (const char *path);
void		create_child_dir_list (NODE *pp);
NODE		*create_new_node (NODE *pp, const char *dname, int d_namlen);
void		insert_node_lexicografically (NODE *pp, NODE *np);
void		free_node (NODE *pp);
void		scroll_dir_window (int diff);
void		draw_dir_line (NODE *np, int line_y);
void		draw_scroll_button (const SCROLLDATA *sp, int button_no, int down_button_no);

/*
 ****************************************************************
 *	Calcula os tamanhos					*
 ****************************************************************
 */
void
set_load_win_size (void)
{
	WINDATA		*wp;
	SCROLLDATA	*sp;

	/*
	 *	Janela do diretório
	 */
	wp = &dir_win;

	wp->width	= DIR_WIDTH    * wp->font_width  + 2 * SHADOW + 2 * MENU_BORDER;
	wp->height	= DIR_HEIGHT   * wp->font_height + 2 * SHADOW + 2 * MENU_BORDER;
	wp->x		= 2  * SHADOW + 2 * MENU_BORDER;
	wp->y		= DIR_WIN_LINE * wp->font_height + 2 * SHADOW + 2 * MENU_BORDER;

	/*
	 *	Janela de rolamento
	 */
	sp = &dir_scroll;

	sp->width	= TOTAL_SCROLL_WIDTH;
	sp->height	= wp->height;
	sp->x		= wp->x + wp->width;
	sp->y		= wp->y;

   /***	sp->button_no	= 0;					***/
	sp->begin	= 2 * SHADOW + SCROLL_WIDTH;
	sp->end		= sp->height - (2 * SCROLL_WIDTH + 2 * SHADOW);
   /***	sp->bottom	= 0;					***/
	sp->top		= 100000;
	sp->button_offset = -1;

	/*
	 *	Janela do OK
	 */
	wp = &ok_win;

	wp->width	= (sizeof (OK_TEXT) - 1) * wp->font_width + 2 * MENU_BORDER + 2 * BORDER;
	wp->height	= wp->font_height + 2 * MENU_BORDER + 2 * BORDER;
	wp->x		= sp->x + sp->width + MENU_BORDER + SHADOW + BORDER;
	wp->y		= sp->y + BORDER;

	/*
	 *	Janela do Novo diretório
	 */
	wp = &new_dir_win;

	wp->width	= (sizeof (NEW_DIR_TEXT) - 1) * wp->font_width + 2 * MENU_BORDER + 2 * BORDER;
	wp->height	= wp->font_height + 2 * MENU_BORDER + 2 * BORDER;
	wp->x		= sp->x + sp->width + MENU_BORDER + SHADOW + BORDER;
	wp->y		= ok_win.y + ok_win.height + 2 * SHADOW + BORDER;

	/*
	 *	Janela do nome do diretório de carga
	 */
	wp = &dir_nm_win;

	wp->width	= DIR_WIDTH * wp->font_width + 2 * (MENU_BORDER + SHADOW);
	wp->height	= wp->font_height + 2 * (MENU_BORDER + SHADOW);
	wp->x		= SHADOW + MENU_BORDER + sizeof (DIR_TEXT) * wp->font_width;
	wp->y		= SHADOW + MENU_BORDER + DIR_LINE * wp->font_height - (wp->height >> 1);

	/*
	 *	Janela do nome do arquivo de carga
	 */
	wp = &file_nm_win;

	wp->width	= FILE_WIDTH * wp->font_width + 2 * (MENU_BORDER + SHADOW);
	wp->height	= wp->font_height + 2 * (MENU_BORDER + SHADOW);
	wp->x		= SHADOW + MENU_BORDER + (sizeof (FILE_TEXT) - 1) * wp->font_width;
	wp->y		= SHADOW + MENU_BORDER + FILE_LINE * wp->font_height - (wp->height >> 1);

	/*
	 *	Janela da carga
	 */
	wp = &load_win.windata;

	wp->width	= ok_win.x + ok_win.width + 1 * wp->font_width + 2 * SHADOW + 2 * MENU_BORDER + 2 * BORDER;
	wp->height	= LOAD_HEIGHT * wp->font_height + 2 * SHADOW + 2 * MENU_BORDER + 2 * BORDER;
	wp->x		= BORDER + file_menu.width;
	wp->y		= LOAD_LINE * wp->font_height + 2 * MENU_BORDER + 2 * BORDER;

}	/* end set_load_win_size */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
draw_load_win (void)
{
	const WINDATA	*wp = &load_win.windata;
	char		*cwd, *last_nm;
	const char	*str;
	NODE		*pp;
	int		len, overflow, width_inc, height_inc;

	/*
	 *	Verifica se a janela principal tem espaço
	 */
	width_inc  = wp->x + wp->width  + 6 * wp->font_width  - present_root_width;
	height_inc = wp->x + wp->height + 6 * wp->font_height - present_root_height;

#if (0)	/*******************************************************/
printf ("comparando %d com %d (%d)\n", present_root_width, wp->x + wp->width + 6 * wp->font_width, width_inc);
#endif	/*******************************************************/

#if (0)	/*******************************************************/
	if (orig_width == 0 && orig_height == 0 && (width_inc > 0 || height_inc > 0))
#endif	/*******************************************************/
	if (width_inc > 0 || height_inc > 0)
	{
		orig_width = present_root_width; orig_height = present_root_height;

		if (width_inc < 0)
			width_inc = 0;

		if (height_inc < 0)
			height_inc = 0;

		set_window_sizes (present_root_width + width_inc, present_root_height + height_inc);
	}

	/*
	 *	Cabeçalho do Diretório
	 */
	XDrawString
	(	display, wp->win,
		wp->text_gc,
		2 * (MENU_BORDER + SHADOW),
		dir_nm_win.y + MENU_BORDER + SHADOW + wp->font_ascent,
		DIR_TEXT, sizeof (DIR_TEXT) - 1
	);

	if (dir_win_node == NONODE)
	{
		cwd = alloca (MAX_DIR_SZ); pp = alloca (sizeof (NODE));

		memsetl (pp, 0, sizeof (NODE) / sizeof (long));

		if ((pp->n_path = getcwd (cwd, MAX_DIR_SZ)) == NOSTR)
			error (NOSTR);

		if ((last_nm = strrchr (pp->n_path, '/')) == NOSTR)
			error ("$Erro em \"getcwd\"\n");

		*last_nm++ = '\0'; len = strlen (last_nm);

		pp->n_path_len = strlen (pp->n_path);

#if (0)	/*******************************************************/
printf ("Começo: pp->n_path = \"%s\", pp->n_path_len = %d\n", pp->n_path, pp->n_path_len);
#endif	/*******************************************************/

		dir_win_node = pp = create_new_node (pp, last_nm, len);

		pp->n_parent = NONODE;		/* O pai era de "mentirinha" */

		if (stat (pp->n_path, &pp->n_s) < 0)
			error ("$*NÃO consegui obter o estado de \"%s\"\n", pp->n_path);

		create_child_dir_list (pp);

		dir_win_node	  = pp;
		dir_win_first_son = pp->n_children;

		dir_node_selected = NONODE;
	}

	/*
	 *	A Sombra da janela toda
	 */
	Shadow_up (wp);

	/*
	 *	Nome do Diretório
	 */
	pp = dir_win_node; str = pp->n_path; len = pp->n_path_len;

	if ((overflow = len - DIR_WIDTH) > 0)
		{ str += overflow; len -= overflow; }

	wp = &dir_nm_win;

	XClearWindow (display, wp->win);

	XDrawString
	(	display, wp->win,
		mode_gc_vector[S_IFDIR >> 12],
		MENU_BORDER + SHADOW,
		MENU_BORDER + SHADOW + wp->font_ascent,
		str, len
	);

	Shadow_up (wp);

	/*
	 *	Cabeçalho do Arquivo
	 */
	wp = &load_win.windata;

	XDrawString
	(	display, wp->win,
		wp->text_gc,
		2 * (MENU_BORDER + SHADOW),
		file_nm_win.y + MENU_BORDER + SHADOW + wp->font_ascent,
		FILE_TEXT, sizeof (FILE_TEXT) - 1
	);

	/*
	 *	Nome do Arquivo
	 */
	wp = &file_nm_win;

	XClearWindow (display, wp->win);

	if (dir_node_selected) XDrawString
	(	display, wp->win,
		mode_gc_vector[(dir_node_selected->n_s.st_mode & S_IFMT) >> 12],
		MENU_BORDER + SHADOW,
		MENU_BORDER + SHADOW + wp->font_ascent,
		file_nm, strlen (file_nm)
	);

	Shadow_up (wp);

}	/* end draw_load_win */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
map_load_window (int value)
{
	XMapRaised (display, load_win.windata.win);

}	/* end map_load_window */

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
		sp->back_triple.top_gc, sp->back_triple.bottom_gc,
		0, 0,
		sp->width, sp->height
	);

}	/* end draw_scroll_win */

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
	NODE		n;

	/*
	 *	Abre o diretório dado
	 */
	if ((dir_fd = inopendir (pp->n_s.st_dev, pp->n_s.st_ino)) == NODIR)
		{ draw_error_msg_text ("*Não consegui ler o diretório \"%s\"", pp->n_path); return; }

	n.n_children = NONODE; n.n_child_cnt = 0;

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
		
		/* Insere na lista */

		if (S_ISDIR (np->n_ls.st_mode))
			insert_node_lexicografically (pp, np);
		else
			insert_node_lexicografically (&n, np);

#if (0)	/*******************************************************/
		if (S_ISDIR (np->n_ls.st_mode))
			pp->n_line_size += 1;
#endif	/*******************************************************/

	}	/* end lendo o diretório */

	closedir (dir_fd);

	/*
	 *	Coloca os arquivos regulares ao final
	 */
	if (n.n_children != NONODE)
	{
		pp->n_child_cnt += n.n_child_cnt;

		if (pp->n_children == NONODE)
		{
			pp->n_children = n.n_children;
		}
		else
		{
			for (np = pp->n_children; np->n_forw != NONODE; np = np->n_forw)
				/* vazio */;

			np->n_forw = n.n_children; n.n_children->n_back = np;
		}
	}

	pp->n_line_size -= 1;		/* Desconta o ".." */

#if (0)	/*******************************************************/
	dir_win_first_son = pp->n_children;

	dir_node_selected = NONODE;
#endif	/*******************************************************/

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
   /***	np->line_first	= 0;		***/

   /***	np->n_good_nm	= 0;		***/
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
	 *	Verifica se é um nome conhecido
	 */
	np->n_good_nm = good_file_nm (np->n_path);

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
	NODE		*lp, *bp;

	/*
	 *	Insere na lista
	 */
	if   ((lp = pp->n_children) == NONODE)
	{
		pp->n_children = np;
	}
	elif (strttcmp (np->n_nm, lp->n_nm, cmpfoldtb, cmpafoldtb) <= 0)
	{
		np->n_forw = lp; lp->n_back = np;
		pp->n_children = np;
	}
	else
	{
		for (bp = lp, lp = lp->n_forw; lp != NONODE; bp = lp, lp = lp->n_forw)
		{
			if (strttcmp (np->n_nm, lp->n_nm, cmpfoldtb, cmpafoldtb) <= 0)
				break;
		}

		np->n_forw = lp; if (lp) lp->n_back = np;
		np->n_back = bp; bp->n_forw = np;
	}

	/*
	 *	Atualiza as contagens do diretório pai
	 */
	pp->n_child_cnt += 1;

}	/* end insert_node_lexicografically */

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

/*
 ****************************************************************
 *	Desenha um diretório na janela de DIRETÓRIOS		*
 ****************************************************************
 */
void
draw_dir_win (void)
{
	const WINDATA	*wp = &dir_win;
	NODE		*np;
	int		y, n;

	/*
	 *	Coloca as linhas dos arquivos
	 */
	if (!dir_win_node)
		return;

#ifdef	DEBUG
	printf ("draw_dir_win (%s)\n", dir_win_node->n_nm);
#endif	DEBUG

	if (dir_win_first_son == NONODE)
		dir_win_first_son = dir_win_node->n_children;

	y = SHADOW + MENU_BORDER;

	for (n = dir_line_vis, np = dir_win_first_son; np != NONODE && n > 0; np = np->n_forw, n--)
	{
		draw_dir_line (np, y);

		y += wp->font_height;
	}

	/*
	 *	Põe a sombra ao redor da janela
	 */
	Shadow_up (wp);

	/*
	 *	Coloca a barra de rolagem, se necessário
	 */
	update_dir_scroll_bar (0 /* button */, 0);

}	/* draw_dir_win */

/*
 ****************************************************************
 *	Desenha uma linha do diretório na janela do DIRETÓRIO	*
 ****************************************************************
 */
void
draw_dir_line (NODE *np, int y)
{
	const WINDATA	*wp = &dir_win;

	/*
	 *	Em primeiro lugar, limpa a linha
	 */
	XClearArea
	(	display, wp->win,
		SHADOW + MENU_BORDER, y,
		wp->width - 2 * (SHADOW + MENU_BORDER), wp->font_height,
		False
	);

	/*
	 *	Põe o nome do arquivo
	 */
	XDrawString
	(	display, wp->win,
		mode_gc_vector[(np->n_s.st_mode & S_IFMT) >> 12],
		SHADOW + MENU_BORDER, y + wp->font_ascent,
		np->n_nm, np->n_nm_len
	);

}	/* end draw_dir_line */

/*
 ****************************************************************
 *	Foi apertada a tecla 1 na janela do DIRETÓRIO 		*
 ****************************************************************
 */
void
button_1_press_dir_window (int y)
{
	const WINDATA	*wp = &dir_win;
	NODE		*np, *pp, *sonp;
	char		*last_nm;
	int		line, n, line_first, len;
	char		area[FILE_WIDTH];

	/*
	 *	Identifica a linha
	 */
	if   ((line = (y - SHADOW - MENU_BORDER) / wp->font_height) < 0)
		line = 0;
	elif (line >= DIR_HEIGHT)
		line = DIR_HEIGHT - 1;

	if ((np = dir_win_first_son) == NONODE)
		return;

	for (n = line; n > 0; np = np->n_forw, n--)
	{
		if (np == NONODE)
			return;
	}

	dir_node_selected = np;

	file_nm = np->n_nm;

	/*
	 *	Verifica se é um diretório (ou não)
	 */
	wp = &load_win.windata;

	if (S_ISDIR (np->n_s.st_mode))
	{
		/* É um diretório */

		dir_win_node->n_line_first = dir_line_first;
#if (0)	/*******************************************************/
printf ("salvei %d (%s)\n", dir_line_first, dir_win_node->n_path);
#endif	/*******************************************************/

		if (streq (np->n_nm, "..") && (np = dir_win_node->n_parent) == NONODE)
		{
			/* Processa o diretório pai */
#if (0)	/*******************************************************/
printf ("Procurando o pai de \"%s\"\n", dir_win_node->n_path);
#endif	/*******************************************************/

			np = dir_win_node;

			pp = alloca (sizeof (NODE)); memsetl (pp, 0, sizeof (NODE) / sizeof (long));

			pp->n_path = alloca (np->n_path_len + 1); strcpy (pp->n_path, np->n_path);

			if ((last_nm = strrchr (pp->n_path, '/')) == NOSTR)
				error ("$NÃO achei \"/\"\n");

			*last_nm = '\0';

			if ((last_nm = strrchr (pp->n_path, '/')) == NOSTR)
			{
				strcpy (pp->n_path, "/"); last_nm = "."; len = 1;
			}
			else
			{
				*last_nm++ = '\0'; len = strlen (last_nm);
			}

			pp->n_path_len = strlen (pp->n_path);

#if (0)	/*******************************************************/
printf ("Pai: pp->n_path = \"%s\", pp->n_path_len = %d\n", pp->n_path, pp->n_path_len);
#endif	/*******************************************************/

			dir_win_node = pp = create_new_node (pp, last_nm, len);

			pp->n_parent = NONODE;		/* O pai era de "mentirinha" */

			if (streq (pp->n_path, "/."))	/* Ajeita a RAIZ */
			{
				pp->n_path[1] = '\0'; pp->n_path_len = 1; pp->n_parent = pp;
			}

			if (stat (pp->n_path, &pp->n_s) < 0)
				error ("$*NÃO consegui obter o estado de \"%s\"\n", pp->n_path);

			create_child_dir_list (pp);

			dir_win_node	  = pp;
			dir_win_first_son = pp->n_children;

#if (0)	/*******************************************************/
			dir_node_selected = NONODE;
#endif	/*******************************************************/

			np = pp;
		}

		dir_node_selected = NONODE;

		if (np->n_children == NONODE)
			create_child_dir_list (np);

		dir_win_node	  = np;

		file_nm = "";

		XClearWindow (display, wp->win);

		draw_load_win ();

		XClearWindow (display, dir_win.win);

#if (0)	/*******************************************************/
printf ("restaurando %d (%s)\n", np->n_line_first, np->n_path);
#endif	/*******************************************************/
		for (sonp = np->n_children, line_first = 0; line_first < np->n_line_first; line_first++)
		{
			if (sonp->n_forw == NONODE)
				break;

			sonp = sonp->n_forw;
		}

		dir_line_first    = line_first;
		dir_win_first_son = sonp;

		draw_dir_win ();
	}
	else
	{
		/* NÃO é um diretório */

		memset (area, ' ', sizeof (area));

		strcpy (area, np->n_nm); area[np->n_nm_len] = ' ';

		wp = &file_nm_win;

		XDrawImageString
		(	display, wp->win,
			mode_gc_vector[(np->n_s.st_mode & S_IFMT) >> 12],
			MENU_BORDER + SHADOW,
			MENU_BORDER + SHADOW + wp->font_ascent,
			area, FILE_WIDTH
		);
	}

}	/* end button_1_press_dir_window */

/*
 ****************************************************************
 *	Desenha a Janela do OK					*
 ****************************************************************
 */
void
draw_ok_win (void)
{
	const WINDATA	*wp = &ok_win;

	XDrawString
	(	display, wp->win,
		wp->text_gc,
		MENU_BORDER,
		MENU_BORDER + wp->font_height - wp->font_descent,
		OK_TEXT, sizeof (OK_TEXT) - 1
	);

	Shadow_even_with_border (wp);

}	/* end draw_ok_win */

/*
 ****************************************************************
 *	Desenha a Janela do Novo Diretório			*
 ****************************************************************
 */
void
draw_new_dir_win (void)
{
	const WINDATA	*wp = &new_dir_win;

	XDrawString
	(	display, wp->win,
		wp->text_gc,
		MENU_BORDER,
		MENU_BORDER + wp->font_height - wp->font_descent,
		NEW_DIR_TEXT, sizeof (NEW_DIR_TEXT) - 1
	);

	Shadow_even_with_border (wp);

}	/* end draw_new_dir_win */

/*
 ****************************************************************
 *	Apertou a tecla OK					*
 ****************************************************************
 */
void
button_1_press_ok_window (void)
{
	const char	**ptr;
	const NODE	*np, *pp;

#if (0)	/*******************************************************/
printf ("dir_node_selected = \"%s\"\n", dir_node_selected ? dir_node_selected->n_path : "-");
#endif	/*******************************************************/

	if   ((np = dir_node_selected) != NONODE)
	{
		if (first_file_nm_argv != given_argv)
			free (first_file_nm_argv);

		ptr = malloc ((1 + 1) * sizeof (char *));

		first_file_nm_argv	= ptr;
		file_nm_argv		= ptr - 1;

		*ptr++ = np->n_path;
		*ptr   = NOSTR;

		XUnmapWindow (display, load_win.windata.win);

		draw_image (0);
	}
	elif ((pp = dir_win_node) != NONODE)
	{
		if (first_file_nm_argv != given_argv)
			free (first_file_nm_argv);

		ptr = malloc ((pp->n_child_cnt + 1) * sizeof (char *));

		first_file_nm_argv	= ptr;
		file_nm_argv		= ptr - 1;

		for (np = pp->n_children; np != NONODE; np = np->n_forw)
		{
			if (streq (np->n_nm, ".."))
				continue;

			if (np->n_good_nm < 0)
				continue;

			*ptr++ = np->n_path;
		}

		*ptr = NOSTR;

		XUnmapWindow (display, load_win.windata.win);

		draw_image (0);
	}

}	/* end button_1_press_ok_window */

/*
 ****************************************************************
 *	Avança ou retrocede a janela do DIRETÓRIO		*
 ****************************************************************
 */
void
scroll_dir_window (int diff)
{
	NODE		*np;
	const WINDATA	*wp = &dir_win;
	int		n, y;

	if ((np = dir_win_first_son) == NONODE)
		return;

	if   (abs (diff) >= dir_line_vis)
	{
		return;
	}
	elif (diff > 0)
	{
		XCopyArea
		(	display, wp->win, wp->win, wp->text_gc,
			SHADOW + MENU_BORDER, SHADOW + MENU_BORDER + diff * wp->font_height,
			wp->width - 2 * (SHADOW + MENU_BORDER), (dir_line_vis - diff) * wp->font_height,
			SHADOW + MENU_BORDER, SHADOW + MENU_BORDER
		);

		for (n = diff; n > 0; np = np->n_forw, n--)
		{
			if (np == NONODE)
				return;
		}

		dir_line_first	 += diff;
		dir_win_first_son = np;

		for (n = dir_line_vis - diff; n > 0; np = np->n_forw, n--)
		{
			if (np == NONODE)
				return;
		}

		y = SHADOW + MENU_BORDER + (dir_line_vis - diff) * wp->font_height;

		for (n = diff; n > 0; n--, np = np->n_forw, y += wp->font_height)
			draw_dir_line (np, y);
	}
	elif (diff < 0)
	{
		diff = -diff;

		XCopyArea
		(	display, wp->win, wp->win, wp->text_gc,
			SHADOW + MENU_BORDER, SHADOW + MENU_BORDER,
			wp->width - 2 * (SHADOW + MENU_BORDER), (dir_line_vis - diff) * wp->font_height,
			SHADOW + MENU_BORDER, SHADOW + MENU_BORDER + diff * wp->font_height
		);

		for (n = diff; n > 0; np = np->n_back, n--)
		{
			if (np == NONODE)
				return;
		}

		dir_line_first	 -= diff;
		dir_win_first_son = np;

		y = MENU_BORDER + SHADOW;

		for (n = diff; n > 0; n--, np = np->n_forw, y += wp->font_height)
			draw_dir_line (np, y);
	}

}	/* end scroll_dir_window */

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
	int		down_button_no = 0;
	int		scroll_height, scroll_size;
	int		last_top    = sp->top;
	int		last_bottom = sp->bottom;
	int		inc = 0, offset_inc, index_inc;

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

	scroll_height = sp->end - sp->begin;
	scroll_size   = scroll_height * dir_line_vis / np->n_child_cnt;

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
#if (0)	/*******************************************************/
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
#endif	/*******************************************************/
	{
		if (motion <= 0)
			return;

		offset_inc = (y - sp->top) - sp->button_offset;

		index_inc = offset_inc * (np->n_child_cnt - dir_line_vis) / (scroll_height - scroll_size);

		new_dir_line_first = dir_line_first + index_inc;

		if   (index_inc == 0)
		{
			return;
		}
		elif (index_inc > 0)
		{
			if (new_dir_line_first > np->n_child_cnt - dir_line_vis)
				new_dir_line_first = np->n_child_cnt - dir_line_vis;
		}
		else
		{
			if (new_dir_line_first <= 0)
				new_dir_line_first = 0;
		}
	}

	/*
	 *	Calcula o comprimento da barra de rolagem
	 */
	if   (scroll_size > sp->end - sp->begin)
		scroll_size = sp->end - sp->begin;
	elif (scroll_size < 2 * SCROLL_WIDTH)
		scroll_size = 2 * SCROLL_WIDTH;

	sp->top    = sp->begin + ((scroll_height - scroll_size) * new_dir_line_first) /
								(np->n_child_cnt - dir_line_vis);
	sp->bottom = sp->top + scroll_size;

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
		sp->fore_triple.main_gc,
		2 * SHADOW, sp->top,
		SCROLL_WIDTH, (sp->bottom - sp->top)
	);

	draw_shadow
	(	sp->win,
		sp->fore_triple.top_gc, sp->fore_triple.bottom_gc,
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
	 *	Atualiza a janela do diretório se for necessário
	 */
	if ((index_inc = new_dir_line_first - dir_line_first) != 0)
		scroll_dir_window (index_inc);

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
		sp->fore_triple.main_gc,
		x, y,
		SCROLL_WIDTH, SCROLL_WIDTH
	);

	if (down_button_no == button_no)
	{
		draw_shadow
		(	sp->win,
			sp->fore_triple.bottom_gc, sp->fore_triple.top_gc,
			x, y,
			SCROLL_WIDTH, SCROLL_WIDTH
		);
	}
	else
	{
		draw_shadow
		(	sp->win,
			sp->fore_triple.top_gc, sp->fore_triple.bottom_gc,
			x, y,
			SCROLL_WIDTH, SCROLL_WIDTH
		);
	}

}	/* end draw_scroll_button */
