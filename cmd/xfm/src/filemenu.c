/*
 ****************************************************************
 *								*
 *			filemenu.c				*
 *								*
 *	Gerenciador de arquivos					*
 *								*
 *	Versão	4.5.0, de 24.06.03				*
 *		4.5.0, de 24.12.03				*
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
#include <a.out.h>
#include <ctype.h>

#include <X11/Intrinsic.h>

#include "../h/xfm.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
entry char		edit_area[256];		/* Nome para editar */

entry int		edit_len;

entry int		present_menu = -1;	/* TREE_MENU ou DIR_MENU, onde clicou */

entry const NODE	*select_begin_node;	/* O primeiro arquivo a ser selecionado */

entry NODE		*select_dir_node;	/* O diretório selecionado */

entry NODE		*select_file_node;	/* O arquivo selecionado (se for apenas 1) */

/*
 ******	Contadores de arquivos selecionados *********************
 */
entry int		selected_file_cnt,
			selected_dir_cnt;

/*
 ******	Tabela de Comandos a executar com o arquivo *************
 */
#define NOFUNC0	(void (*) (void))NULL

const FILE_MENU_VEC	tree_menu_vec[] =
{
	"ziptool",	NOSTR,					zip_0,		permission_zip,
	"mount",	NOSTR,					mount_0,	permission_mount,
	"umount",	NOSTR,					umount_0,	permission_mount,
	"eject",	NOSTR,					eject_0,	permission_mount,
	"df",		NOSTR,					print_df_entry,	permission_mount,
	"du",		NOSTR,					du_0,		permission_du,
	"rm",		NOSTR,					rm_file_dir_0,	permission_rm,
	"cp",		NOSTR,					copy_file_0,	permission_copy,
	"cmp",		NOSTR,					cmp_file_0,	permission_copy,
	"mv",		NOSTR,					mv_file_0,	permission_mv,
	"sh",		"xterm -h '%s' -g 112x40",		NOFUNC0,	permission_sh,
	"mkdir",	NOSTR,					mkdir_0,	permission_mkdir
};

#define	tree_menu_longest_len	2
#define	tree_menu_text_n	(sizeof (tree_menu_vec) / sizeof (FILE_MENU_VEC))


const FILE_MENU_VEC	dir_menu_vec[] =
{
	"rm",		NOSTR,					rm_file_dir_0,	permission_rm,
	"rename",	NOSTR,					rename_0,	permission_rename,
	"sh",		"xterm -h '%s' -g 112x40",		NOFUNC0,	permission_sh,
	"show",		"xterm -g 112x40 -e show '%s'",		NOFUNC0,	permission_edit,
	"vi",		"xterm -g 112x40 -e vi '%s'",		NOFUNC0,	permission_edit,
	"xedit",	"xedit '%s'", 				NOFUNC0,	permission_edit,
	"xview",	"xview '%s'", 				NOFUNC0,	permission_paint,
#if (0)	/*******************************************************/
	"xpaint",	"xpaint '%s'", 				NOFUNC0,	permission_paint,
#endif	/*******************************************************/
	"mkdir",	NOSTR,					mkdir_0,	permission_mkdir
};

#define	dir_menu_longest_len	6
#define	dir_menu_text_n		(sizeof (dir_menu_vec) / sizeof (FILE_MENU_VEC))

/*
 ******	Dados dos dois MENUs ************************************
 */
MENUDATA	menudata[2] =
{
	&tree_menu,
	NONODE,
	tree_menu_vec,
	tree_menu_text_n,
	-1,

	&dir_menu,
	NONODE,
	dir_menu_vec,
	dir_menu_text_n,
	-1,
};

/*
 ******	Protótipos de funções ***********************************
 */
void		select_node (NODE *np, int line);
void		button_1_press_dir_window (int x, int state, int line, NODE *np);
void		update_mounted_or_umounted_dir (NODE *dir_np);

/*
 ****************************************************************
 *	Clicou no botão da direita na janela da ÁRVORE		*
 ****************************************************************
 */
void
button_3_press_tree_window (int x, int y, NODE *np)
{
	MENUDATA	*mp;
	WINDATA		*wp = &tree_menu;
	XWindowChanges	value;
	int		len;

	/*
	 *	Identifica a linha
	 */
	XUnmapWindow (display, wp->win);

	get_file_icon (np);

	mp = &menudata[TREE_MENU];

	mp->menu_node		= np;
	mp->menu_highlight_line	= -1;

	/*
	 *	Calcula o tamanho e posição da janela
	 */
#define	PATH
#ifdef	PATH
	if ((len = strlen (np->n_path) + 1) < tree_menu_longest_len)
		len = tree_menu_longest_len;
#else
	if ((len = strlen (np->n_nm) + 1) < tree_menu_longest_len)
		len = tree_menu_longest_len;
#endif	PATH

	value.width  = wp->width  = len * wp->font_width + 2 * SHADOW + 2 * MENU_BORDER + NAME_OFFSET;
	value.height = wp->height = (tree_menu_text_n + 2) * (wp->font_height + 2 * SHADOW) + 2 * MENU_BORDER;

	x -= SHADOW;

	if (y + wp->height >= tree.height)
		y -= (wp->height - SHADOW);
	else
		y -= SHADOW;

	value.x = wp->x = tree.x + x;
	value.y = wp->y = tree.y + y;

	XConfigureWindow (display, wp->win, CWWidth|CWHeight|CWX|CWY, &value);

	XMapRaised (display, wp->win);

	present_menu = TREE_MENU;

}	/* end button_3_press_tree_window */

/*
 ****************************************************************
 *	Clicou no botão na janela do diretório			*
 ****************************************************************
 */
void
button_press_dir_window (int button, int state, int x, int y)
{
	WINDATA		*wp = &dir_menu;
	MENUDATA	*mp;
	NODE		*np;
	XWindowChanges	value;
	int		line, n, len;

	/*
	 *	Identifica a linha
	 */
	XUnmapWindow (display, wp->win);

	line = (y - SHADOW) / LINE_HEIGHT;

	if ((np = dir_win_first_son) == NONODE)
		return;

	for (n = line; n > 0; np = np->n_forw, n--)
	{
		if (np == NONODE)
			return;
	}

	switch (button)
	{
	    case Button1:
		button_1_press_dir_window (x, state, line, np);
		return;

	    case Button3:
		break;

	    case Button4:
		update_dir_scroll_bar (-1, 1);
		return;

	    case Button5:
		update_dir_scroll_bar (-1, 100000);
		return;

	    default:
		return;

	}	/* end switch */

	/*
	 ******	Clicou no botão da direita na janela do DIRETÓRIO ***********
	 *
	 *	Verifica se foi na região dos botões
	 */
	if   (x <= MENU_BORDER + SHADOW + BUTTON_WIDTH)
	{
		if (selected_file_cnt + selected_dir_cnt == 0)
			return;

		np = NONODE;

		len = sizeof ("xx arquivos:");
	}
	else
	{
		if (np->n_exec == EXEC_UNKNOWN || np->n_ximage == NULL)
			return;

		len = strlen (np->n_nm) + 1;
	}

	mp = &menudata[DIR_MENU];

	mp->menu_node		= np;
	mp->menu_highlight_line	= -1;

	/*
	 *	Calcula o tamanho e posição da janela
	 */
	if (len < dir_menu_longest_len)
		len = dir_menu_longest_len;

	wp->width  = len * wp->font_width + 2 * SHADOW + 2 * MENU_BORDER;
	wp->height = (dir_menu_text_n + 2) * (wp->font_height + 2 * SHADOW) + 2 * MENU_BORDER;

	if (np != NONODE)
		wp->width += NAME_OFFSET;

	value.width  = wp->width;
	value.height = wp->height;

	if (x + wp->width >= dir.width)
		x -= (wp->width - SHADOW);
	else
		x -= SHADOW;

	if (y + wp->height >= dir.height)
		y -= (wp->height - SHADOW);
	else
		y -= SHADOW;

	value.x	= wp->x = dir.x + x;
	value.y	= wp->y = dir.y + y;

	XConfigureWindow (display, wp->win, CWWidth|CWHeight|CWX|CWY, &value);

	XMapWindow (display, wp->win);

	present_menu = DIR_MENU;

}	/* end button_press_dir_window */

/*
 ****************************************************************
 *	Clicou no botão da esquerda na janela do DIRETÓRIO	*
 ****************************************************************
 */
void
button_1_press_dir_window (int x, int state, int line, NODE *button_np)
{
	const WINDATA	*wp = &dir;
	NODE		*sonp, *np;
	int		line_first;

	/*
	 *	Processa o clique no botão de marcação
	 */
	if (x >= MENU_BORDER + SHADOW && x <= MENU_BORDER + SHADOW + BUTTON_WIDTH)
	{
		select_begin_node = button_np;
		select_dir_node   = NONODE;

		select_node (button_np, line);

		if (!button_np->n_selected)
			return;

		if (S_ISDIR (button_np->n_ls.st_mode))
		{
			select_dir_node = button_np;

			for (line = 0, np = dir_win_first_son; np != NONODE; np = np->n_forw, line++)
			{
				if (np == button_np)
					continue;

				if (np->n_selected)
					select_node (np, line);
			}
		}
		else	/* NÃO Diretório */
		{
			select_file_node = button_np;

			for (line = 0, np = dir_win_first_son; np != NONODE; np = np->n_forw, line++)
			{
				if (np == button_np)
					continue;

				if (np->n_selected && S_ISDIR (np->n_ls.st_mode))
					select_node (np, line);
			}
		}

		return;
	}

	if (x < MENU_BORDER + SHADOW + NAME_OFFSET)
		return;

	/*
	 *	Clicou no ícone (ou resto da linha)
	 */
	if (!S_ISDIR (button_np->n_ls.st_mode))
		return;

	dir_win_node->n_line_first = dir_line_first;

	if (streq (button_np->n_nm, ".."))
	{
		if (dir_win_node == root_node)
			return;

		button_np = dir_win_node->n_parent;
	}

	if (button_np->n_children == NONODE)
		create_child_dir_list (button_np);

	disselect_all (0);

	dir_win_node = button_np;

	get_max_colsz ();

	for (sonp = button_np->n_children, line_first = 0; line_first < button_np->n_line_first; line_first++)
	{
		if (sonp->n_forw == NONODE)
			break;

		sonp = sonp->n_forw;
	}

	dir_line_first	  = line_first;
	dir_win_first_son = sonp;

	XClearWindow (display, dir.win);

	dir_scroll.button_no	= 0;
	dir_scroll.last_offset	= -1;

	draw_dir_win ();

}	/* end button_1_press_dir_window */

/*
 ****************************************************************
 *	Soltou o botão janela do diretório			*
 ****************************************************************
 */
void
button_release_dir_window (int y)
{
#if (0)	/*******************************************************/
	const WINDATA	*wp = &dir;
#endif	/*******************************************************/
	NODE		*button_np, *np;
	int		line, end_line, in = 0;

	/*
	 *	Termina a marcação dos arquivos/diretórios
	 */
	if (select_begin_node == NONODE)
		return;

	end_line = (y - SHADOW) / LINE_HEIGHT;

	if ((button_np = dir_win_first_son) == NONODE)
		return;

	for (line = 0; button_np != NONODE; button_np = button_np->n_forw, line++)
	{
		if (in)
			select_node (button_np, line);

		if (line == end_line)
			break;

		if (button_np == select_begin_node)
			in = 1;
	}

	for (line = 0, np = dir_win_first_son; np != NONODE; np = np->n_forw, line++)
	{
		if (np == button_np)
			continue;

		if (np->n_selected && S_ISDIR (np->n_ls.st_mode))
		{
			if (np == select_dir_node)
				select_dir_node = NONODE;

			select_node (np, line);
		}
	}

	select_begin_node = NONODE;

}	/* end button_release_dir_window */

/*
 ****************************************************************
 *	Seleciona um arquivo					*
 ****************************************************************
 */
void
select_node (NODE *np, int line)
{
	const WINDATA	*wp = &dir;
	const TRIPLEGC  *tp;
	int		line_y;

	line_y = SHADOW + wp->font_height + line * LINE_HEIGHT;

	np->n_selected ^= 1;

	if (np->n_selected)
	{
		if (S_ISDIR (np->n_ls.st_mode))
			selected_dir_cnt++;
		else
			selected_file_cnt++;

		tp = &full_dir_triple;
	}
	else
	{
		if (S_ISDIR (np->n_ls.st_mode))
			selected_dir_cnt--;
		else
			selected_file_cnt--;

		tp = &back_triple;
	}

	draw_button
	(	wp, tp, 
		MENU_BORDER + SHADOW,
		line_y - wp->font_height + 4,
		BUTTON_WIDTH, wp->font_height - 2
	);

}	/* end select_node */

/*
 ****************************************************************
 *	Seleciona todos os NÓs de um diretório			*
 ****************************************************************
 */
void
select_all (int from_menu)
{
	NODE		*np;

	selected_file_cnt = 0;
	selected_dir_cnt  = 0;

	if (dir_win_node == NONODE)
		return;

	for (np = dir_win_node->n_children; np != NONODE; np = np->n_forw)
	{
		if (S_ISDIR (np->n_s.st_mode))
			continue;

		np->n_selected = 1; selected_file_cnt++;
	}

	if (from_menu)
		draw_dir_win ();

}	/* end select_all */

/*
 ****************************************************************
 *	Deseleciona todos os NÓs de um diretório		*
 ****************************************************************
 */
void
disselect_all (int from_menu)
{
	NODE		*np;

	selected_file_cnt = 0;
	selected_dir_cnt  = 0;

	if (dir_win_node == NONODE)
		return;

	for (np = dir_win_node->n_children; np != NONODE; np = np->n_forw)
		np->n_selected = 0;

	if (from_menu)
		draw_dir_win ();

}	/* end disselect_all */

/*
 ****************************************************************
 *	Desenha a janela do MENU da ÁRVORE ou DIRETÓRIO		*
 ****************************************************************
 */
void
draw_tree_or_dir_menu_win (int MENU)
{
	const MENUDATA		*mp;
	const WINDATA		*wp;
	const NODE		*np;
	const FILE_MENU_VEC	*vp;
	GC			gc;
	int			y, line, len, n;
	const char		*nm;
	char			text[64];

	/*
	 *	Decide tree/dir
	 */
	mp = &menudata[MENU]; wp = mp->windata;

	if ((np = mp->menu_node) == NONODE && selected_file_cnt + selected_dir_cnt == 0)
		return;

	y = MENU_BORDER + (wp->font_height + 2 * SHADOW) - (wp->font_descent + SHADOW);

	/*
	 *	Põe o nome do arquivo ou o número de arquivos selecionados
	 */
	if (np != NONODE)
	{
		XPutImage
		(	display, wp->win,
			wp->text_gc, np->n_ximage,
			0, 0,
			MENU_BORDER + SHADOW,
			y + wp->font_descent - wp->font_height,
			np->n_ximage->width, np->n_ximage->height
		);

		/* Põe o nome do arquivo */
#ifdef	PATH
		if (MENU == TREE_MENU)
			{ nm = np->n_path; len = np->n_path_len; }
		else
			{ nm = np->n_nm; len = np->n_nm_len; }
#else
		nm = np->n_nm; len = np->n_nm_len;
#endif	PATH
		XDrawString
		(	display, wp->win,
			mode_gc_vector[(np->n_ls.st_mode & S_IFMT) >> 12],
			MENU_BORDER + SHADOW + NAME_OFFSET,
			y,
			nm, len
		);

		XDrawString
		(	display, wp->win,
			wp->text_gc,
			MENU_BORDER + SHADOW + NAME_OFFSET + len * wp->font_width,
			y,
			":", 1
		);
	}
	else
	{
		n = selected_file_cnt + selected_dir_cnt;

		len = snprintf (text, sizeof (text), "%2d arquivo%s:", n, n > 1 ? "s" : "");

		XDrawString
		(	display, wp->win,
			wp->text_gc,
			MENU_BORDER + SHADOW,
			y,
			text, len
		);
	}

	for (line = 0, vp = mp->menu_vec; line < mp->menu_text_n; line++, vp++)
	{
		if (vp->e_permission (np) >= 0)
			gc = wp->text_gc;
		else
			gc = absent_text_gc;

		XDrawString
		(	display, wp->win,
			gc,
			MENU_BORDER + SHADOW,
			MENU_BORDER + (line + 3) * (wp->font_height + 2 * SHADOW) - (wp->font_descent + SHADOW),
			vp->e_cmd_nm, strlen (vp->e_cmd_nm)
		);
	}

	draw_shadow
	(	wp->win,
		wp->triple.top_gc, wp->triple.bottom_gc,
		0, 0,
		wp->width, wp->height
	);

}	/* end draw_tree_or_dir_menu_win */

/*
 ****************************************************************
 *	Entrei na janela do MENU da ÁRVORE ou DIRETÓRIO		*
 ****************************************************************
 */
void
motion_tree_or_dir_menu_window (int MENU, int y)
{
	MENUDATA		*mp;
	const WINDATA		*wp;
	const NODE		*np;
	const FILE_MENU_VEC	*vp;
	int			line;

	/*
	 *	Decide tree/dir e obtém a linha
	 */
	mp = &menudata[MENU]; wp = mp->windata;

	line = (y - MENU_BORDER) / (wp->font_height + 2 * SHADOW);

	line -= 2;	/* Pula as 2 linhas iniciais */

	if ((unsigned)line >= mp->menu_text_n)
		return;

	/*
	 *	Desmarca a linha, se for o caso
	 */
	if (line == mp->menu_highlight_line)
		return;

	if (mp->menu_highlight_line >= 0)
	{
		draw_shadow
		(	wp->win,
			wp->triple.main_gc, wp->triple.main_gc,
			MENU_BORDER,
			MENU_BORDER + (mp->menu_highlight_line + 2) * (wp->font_height + 2 * SHADOW),
			wp->width - 2 * MENU_BORDER, (wp->font_height + 2 * SHADOW)
		);

		XUnmapWindow (display, df_data.win);
		XUnmapWindow (display, du_data.win);

		mp->menu_highlight_line = -1;
	}

	/*
	 *	Marca a nova linha
	 */
	np = mp->menu_node;

	vp = &mp->menu_vec[line];

	if (vp->e_permission (np) < 0)
		return;

	draw_shadow
	(	wp->win,
		wp->triple.top_gc, wp->triple.bottom_gc,
		MENU_BORDER, MENU_BORDER + (line + 2) * (wp->font_height + 2 * SHADOW),
		wp->width - 2 * MENU_BORDER, (wp->font_height + 2 * SHADOW)
	);

	mp->menu_highlight_line = line;

}	/* end motion_tree_or_dir_menu_window */

/*
 ****************************************************************
 *	Saí da janela do MENU da ÁRVORE ou DIRETÓRIO		*
 ****************************************************************
 */
void
leave_tree_or_dir_menu_window (int MENU)
{
	MENUDATA	*mp;

	mp = &menudata[MENU];

	XUnmapWindow (display, mp->windata->win);
#if (0)	/*******************************************************/
	XUnmapWindow (display, df_data.win);
#endif	/*******************************************************/

   /***	mp->menu_highlight_line = -1;	***/

}	/* end leave_tree_or_dir_menu_window */

/*
 ****************************************************************
 *	Apertou a tecla da janela do MENU da ÁRVORE		*
 ****************************************************************
 */
void
button_press_tree_or_dir_menu_window (int MENU)
{
	MENUDATA		*mp;
	int			line;
	const FILE_MENU_VEC	*vp;
	const NODE		*np;
	char			*cmd;
	int			size;

	mp = &menudata[MENU];

	if ((np = mp->menu_node) == NONODE && selected_file_cnt + selected_dir_cnt == 0)
		return;

	if ((line = mp->menu_highlight_line) < 0)
		return;

	vp = &mp->menu_vec[line];

	if (vp->e_permission (np) < 0)
		return;

   /***	XUnmapWindow (display, mp->windata->win); ***/

	/*
	 *	Verifica se há uma função ou programa
	 */
	if (vp->e_func != NOFUNC0)
	{
		vp->e_func ();
	}
	else
	{
		XUnmapWindow (display, mp->windata->win);

		seteuid (0); setruid (real_uid);

		if (access (np->n_path, R_OK) < 0)
			{ draw_error_msg_text ("*O arquivo \"%s\" NÃO é legível", np->n_path); return; }

		setruid (0); seteuid (real_uid);

		size = strlen (np->n_path) + 32;

		cmd = alloca (size);

		snprintf (cmd, size, vp->e_exec_text, np->n_path);

		if (fork () == 0)
		{
			setruid (real_uid);

			execl ("/bin/sh", "sh", "-c", cmd, 0);
		}
	}

}	/* end do_tree_or_dir_menu_button_press */

/*
 ****************************************************************
 *	Obtém o ícone de um arquivo				*
 ****************************************************************
 */
void
get_file_icon (NODE *np)
{
	XImage		*ximage;

	check_if_file_is_executable (np);

	if (np->n_ximage != NULL)
		return;

	switch (np->n_ls.st_mode & S_IFMT)
	{
	    case S_IFREG:
		if (np->n_exec == EXEC_IS_EXEC)
			ximage = small_executable_ximage;
		else
			ximage = small_text_ximage;
		break;

	    case S_IFDIR:
		if (np->n_mnt_point)
			ximage = small_mount_ximage;
		else
			ximage = small_dir_ximage;
		break;

	    case S_IFBLK:
	    case S_IFCHR:
		ximage = small_disk_ximage;
		break;

	    case S_IFLNK:
		ximage = small_symlink_ximage;
		break;

	    default:
		ximage = small_text_ximage;
		break;
	}

	np->n_ximage = ximage;

}	/* end get_file_icon */

/*
 ****************************************************************
 *	Verifica se o arquivo é executável			*
 ****************************************************************
 */
void
check_if_file_is_executable (NODE *np)
{
	int		fd, n;
	HEADER		h;

	if (np->n_exec != EXEC_UNKNOWN)
		return;

	if (!S_ISREG (np->n_s.st_mode))
		{ np->n_exec = EXEC_NO_EXEC; return; }

	if ((fd = inopen (np->n_s.st_dev, np->n_s.st_ino)) < 0)
		{ np->n_exec = EXEC_ERROR; return; }

	if ((n = read (fd, &h, sizeof (HEADER))) < 0)
		{ close (fd); np->n_exec = EXEC_ERROR; return; }

	close (fd);

	if   (n < sizeof (HEADER))
		np->n_exec = EXEC_NO_EXEC;
	elif (h.h_magic == FMAGIC || h.h_magic == NMAGIC || h.h_magic == SMAGIC)
		np->n_exec = EXEC_IS_EXEC;
	else
		np->n_exec = EXEC_NO_EXEC;

}	/* end check_if_file_is_executable */

#if (0)	/*******************************************************/
/*
 ****************************************************************
 *	Esta operação é sempre permitida			*
 ****************************************************************
 */
int
permission_always (const NODE *np)
{
	return (1);

}	/* end permission_always */
#endif	/*******************************************************/

/*
 ****************************************************************
 *	Verifica se é permitida a execução de um "sh"		*
 ****************************************************************
 */
int
permission_sh (const NODE *np)
{
	if (np == NONODE)
		return (-1);

	if (!S_ISDIR (np->n_s.st_mode))
		return (-1);

	return (1);

}	/* end permission_sh */

/*
 ****************************************************************
 *	Verifica se é permitida a edição			*
 ****************************************************************
 */
int
permission_edit (const NODE *np)
{
	if (np == NONODE)
		return (-1);

	if (!S_ISREG (np->n_s.st_mode))
		return (-1);

	if (np->n_exec == EXEC_IS_EXEC)
		return (-1);

	return (1);

}	/* end permission_edit */

/*
 ****************************************************************
 *	Verifica se é o arquivo de uma imagem			*
 ****************************************************************
 */
int
permission_paint (const NODE *np)
{
	const char	*str;
	char		code[4];

	if (np == NONODE)
		return (-1);

	if (!S_ISREG (np->n_s.st_mode))
		return (-1);

	if (np->n_exec == EXEC_IS_EXEC)
		return (-1);

	str = np->n_nm + np->n_nm_len - 3;

	if (str[-1] != '.')
		return (-1);

	/* Converte "GIF" em "gif" */

	*code	= _tolower (str[0]);
	code[1] = _tolower (str[1]);
	code[2] = _tolower (str[2]);
	code[3] = '\0';

	if (streq (code, "gif"))
		return (1);

	if (streq (code, "jpg"))
		return (1);

	if (streq (code, "bmp"))
		return (1);

	if (streq (code, "xpm"))
		return (1);

	return (-1);

}	/* end permission_paint */
