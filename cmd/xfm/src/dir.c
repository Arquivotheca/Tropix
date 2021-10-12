/*
 ****************************************************************
 *								*
 *			dir.c					*
 *								*
 *	Gerenciador de arquivos					*
 *								*
 *	Versão	4.5.0, de 24.06.03				*
 *		4.5.0, de 17.11.03				*
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
#include <pwd.h>
#include <grp.h>
#include <time.h>

#include <X11/Intrinsic.h>

#include "../h/xfm.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
#define	DIR_LINE_SZ	256

static char	global_area[32];	/* Para várias edições */

/*
 ******	Outras variáveis ****************************************
 */
entry NODE	*dir_win_node;		/* Diretório da tela do DIRETÓRIO */
entry NODE	*dir_win_first_son;	/* Primeiro filho visível do DIRETÓRIO */

entry int	dir_line_vis;		/* No. de linhas visíveis na janela do diretório */
entry int	dir_line_first;		/* No. da primeira linha visível do diretório */

entry int	dir_max_line_len;	/* Comprimento máximo da linha */

entry int	max_nlink_len	= -1;	/* No. de colunas para edição */
entry int	max_uid_len	= -1;
entry int	max_gid_len	= -1;
entry int	max_size_len	= -1;

/*
 ******	Quasi-constantes do tempo *******************************
 */
entry time_t	HOJE;		/* Início do dia de hoje (GMT) */
entry time_t	AMANHA;		/* Início do dia de amanhã (GMT) */
entry time_t	SEMANA;		/* Início da semana (GMT) */

/*
 ****** Protótipos de funções ***********************************
 */
void		draw_dir_line (NODE *np, int line_y);
const char	*edit_uid (int uid);
const char	*edit_gid (int gid);
const char	*edit_decimal_number (int number);
const char	*edit_dev_number (int rdev);
int		put_time (char *line, int len, time_t timeval);

/*
 ****************************************************************
 *	Desenha a janela do nome do diretório			*
 ****************************************************************
 */
void
draw_name_win (void)
{
	const WINDATA	*wp = &name;
	const NODE	*np;
	int		len, offset;
	char		line[DIR_LINE_SZ];

	XClearWindow (display, wp->win);

	draw_shadow
	(	wp->win,
		wp->triple.top_gc, wp->triple.bottom_gc,
		0, 0,
		wp->width, wp->height
	);

	if ((np = dir_win_node) == NONODE || np->n_path == NOSTR)
		return;

	/*
	 *	Coloca o nome do diretório
	 */
	len = snprintf (line, DIR_LINE_SZ, "Diretório \"%s\"", np->n_path);

	if ((offset = (wp->width - len * wp->font_width) >> 1) < MENU_BORDER + SHADOW)
		offset = MENU_BORDER + SHADOW;

	XDrawString
	(	display, wp->win,
		wp->text_gc,
		offset,
		MENU_BORDER + wp->font_ascent,
		line, len
	);

}	/* end draw_name_win */

/*
 ****************************************************************
 *	Desenha um diretório na janela de DIRETÓRIOS		*
 ****************************************************************
 */
void
draw_dir_win (void)
{
	const WINDATA	*wp = &dir;
	NODE		*np;
	int		line_y, n;

	line_y = SHADOW;

	/*
	 *	Coloca os Ícones Grandes
	 */
#undef	BIG
#ifdef	BIG
	XImage		*ximage;

	XPutImage (display, wp->win, wp->text_gc, ximage, 0, 0, 10, 10, ximage->width, ximage->height);
	ximage = big_dir_ximage;
	XPutImage (display, wp->win, wp->text_gc, ximage, 0, 0, 50, 10, ximage->width, ximage->height);
	ximage = big_disk_ximage;
	XPutImage (display, wp->win, wp->text_gc, ximage, 0, 0, 90, 10, ximage->width, ximage->height);
	ximage = big_executable_ximage;
	XPutImage (display, wp->win, wp->text_gc, ximage, 0, 0, 130, 10, ximage->width, ximage->height);
	ximage = big_text_ximage;
	XPutImage (display, wp->win, wp->text_gc, ximage, 0, 0, 170, 10, ximage->width, ximage->height);
	ximage = big_script_ximage;
	XPutImage (display, wp->win, wp->text_gc, ximage, 0, 0, 210, 10, ximage->width, ximage->height);

	line_y += 36;
#endif	BIG

	/*
	 *	Coloca o nome do diretório
	 */
	draw_name_win ();

	/*
	 *	Verifica se mudou a inclusão do modo
	 */
	if (last_mode_in_use != mode_in_use)
	{
		XClearWindow (display, wp->win);
		last_mode_in_use  = mode_in_use;
	}
#if (0)	/*******************************************************/
	 *	Verifica se mudou a inclusão do grupo ou o modo
	if (last_group_in_use != group_in_use || last_mode_in_use != mode_in_use)
	{
		XClearWindow (display, wp->win);
		last_group_in_use = group_in_use;
		last_mode_in_use  = mode_in_use;
	}
#endif	/*******************************************************/

	/*
	 *	Coloca as linhas dos arquivos
	 */
	dir_max_line_len = 0;

	for (n = dir_line_vis, np = dir_win_first_son; np != NONODE && n > 0; np = np->n_forw, n--)
	{
		draw_dir_line (np, line_y);

		line_y += LINE_HEIGHT;
	}

	/*
	 *	Verifica se o tamanho deve aumentar
	 */
	if (extension_in_use == WIN_EXTENSION_ON && (n = dir_max_line_len * dir.font_width - dir.width) > 0)
	{
		set_window_sizes (present_root_width + n, present_root_height, 0);
		return;
	}

	/*
	 *	Põe a sombra ao redor da janela
	 */
	draw_shadow
	(	wp->win,
		wp->triple.top_gc, wp->triple.bottom_gc,
		0, 0,
		wp->width, wp->height
	);

	/*
	 *	Coloca a barra de rolagem, se necessário
	 */
	update_dir_scroll_bar (0 /* button */, 0);

}	/* draw_dir_win */

/*
 ****************************************************************
 *	Avança ou retrocede a janela do DIRETÓRIO		*
 ****************************************************************
 */
void
scroll_dir_window (int diff)
{
	NODE		*np;
	const WINDATA	*wp = &dir;
	int		n, line_y;

	line_y = SHADOW;

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
			SHADOW, diff * LINE_HEIGHT + SHADOW,
			wp->width - 2 * SHADOW, (dir_line_vis - diff) * LINE_HEIGHT,
			SHADOW, SHADOW
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

		line_y += (dir_line_vis - diff) * LINE_HEIGHT;

		for (n = diff; n > 0; n--, np = np->n_forw, line_y += LINE_HEIGHT)
		{
			XClearArea
			(	display, wp->win,
				SHADOW, line_y,
				wp->width - 2 * SHADOW,	LINE_HEIGHT,
				False
			);

			draw_dir_line (np, line_y);
		}
	}
	elif (diff < 0)
	{
		diff = -diff;

		XCopyArea
		(	display, wp->win, wp->win, wp->text_gc,
			SHADOW, SHADOW,
			wp->width - 2 * SHADOW, (dir_line_vis - diff) * LINE_HEIGHT,
			SHADOW, diff * LINE_HEIGHT + SHADOW
		);

		for (n = diff; n > 0; np = np->n_back, n--)
		{
			if (np == NONODE)
				return;
		}

		dir_line_first	 -= diff;
		dir_win_first_son = np;

		line_y = SHADOW;

		for (n = diff; n > 0; n--, np = np->n_forw, line_y += LINE_HEIGHT)
		{
			XClearArea
			(	display, wp->win,
				SHADOW, line_y,
				wp->width - 2 * SHADOW,	LINE_HEIGHT,
				False
			);

			draw_dir_line (np, line_y);
		}
	}

	/*
	 *	Verifica se o tamanho deve aumentar
	 */
	if (extension_in_use == WIN_EXTENSION_ON && (n = dir_max_line_len * dir.font_width - dir.width) > 0)
	{
		set_window_sizes (present_root_width + n, present_root_height, 0);
		return;
	}

	/*
	 *	Reescreve a sombra do lado direito
	 */
	draw_only_right_shadow
	(	wp->win,
		wp->triple.bottom_gc,
		0, 0,
		wp->width, wp->height
	);

}	/* end scroll_dir_window */

/*
 ****************************************************************
 *	Desenha uma linha do diretório na janela do DIRETÓRIO	*
 ****************************************************************
 */
void
draw_dir_line (NODE *np, int line_y)
{
	const WINDATA	*wp = &dir;
	const TRIPLEGC  *tp;
	int		len, offset = 0;
	const char	*modestr, *nm = np->n_nm;
	char		line[DIR_LINE_SZ];

	/*
	 *	Verifica se é o pai
	 */
	if (streq (nm, ".."))
	{
		if (np->n_parent != NONODE)
			np = np->n_parent;

		if (np->n_parent != NONODE)
			np = np->n_parent;
	}

	/*
	 *	Põe o ícone
	 */
	get_file_icon (np);

	if (np->n_selected)
		tp = &full_dir_triple;
	else
		tp = &back_triple;

	draw_button
	(	wp, tp, 
		MENU_BORDER + SHADOW,
		line_y + BUTTON_OFFSET,
		BUTTON_WIDTH, BUTTON_HEIGHT
	);

	XPutImage
	(	display, wp->win,
		wp->text_gc, np->n_ximage,
		0, 0,
		MENU_BORDER + SHADOW + NAME_OFFSET,
		line_y + ICON_OFFSET,
		np->n_ximage->width, np->n_ximage->height
	);

	modestr = modetostr (np->n_ls.st_mode);

	len = snprintf
	(	line, DIR_LINE_SZ, "%c %s %*d", modestr[0],
		mode_in_use == MODE_9 ? modestr + 14 : modestr + 9,
		max_nlink_len, np->n_ls.st_nlink
	);

	len += snprintf (line + len, DIR_LINE_SZ - len, " %-*s", max_uid_len, edit_uid (np->n_ls.st_uid));
	len += snprintf (line + len, DIR_LINE_SZ - len, " %-*s", max_gid_len, edit_gid (np->n_ls.st_gid));

#if (0)	/*******************************************************/
	const char	*modestr, *cp, *nm = np->n_nm;
	{
		if ((cp = pwcache (np->n_ls.st_uid)) == NOSTR)
			len += snprintf (line + len, DIR_LINE_SZ - len, " %8d", np->n_ls.st_uid);
		else
			len += snprintf (line + len, DIR_LINE_SZ - len, " %-8.8s", cp);
	}

	if (group_in_use == GROUP_ON)
	{
		if ((cp = grcache (np->n_ls.st_gid)) == NOSTR)
			len += snprintf (line + len, DIR_LINE_SZ - len, " %8d", np->n_ls.st_gid);
		else
			len += snprintf (line + len, DIR_LINE_SZ - len, " %-8.8s", cp);
	}
#endif	/*******************************************************/

	if (S_ISBLK (np->n_ls.st_mode) || S_ISCHR (np->n_ls.st_mode))
	{
		len += snprintf
		(
			line + len, DIR_LINE_SZ - len,
			" %*s", max_size_len, edit_dev_number (np->n_ls.st_rdev)
		);
	}
	else
	{
		len += snprintf
		(
			line + len, DIR_LINE_SZ - len,
			" %*d", max_size_len, np->n_ls.st_size
		);
	}


#if (0)	/*******************************************************/
	if (S_ISBLK (np->n_ls.st_mode) || S_ISCHR (np->n_ls.st_mode))
	{
		char		area[16];

		sprintf
		(	area, "%d/%d",		/* Ou "%3d,%3d" */
			MAJOR (np->n_ls.st_rdev),
			MINOR (np->n_ls.st_rdev)
		);

		len += snprintf
		(       line + len, DIR_LINE_SZ - len,
			" %9.9s", area
		);
	}
	else
	{
		len += snprintf
		(	line + len, DIR_LINE_SZ - len,
			" %9u", np->n_ls.st_size
		);

	}
#endif	/*******************************************************/

	len = put_time (line, len, np->n_ls.st_mtime);

	XDrawString
	(	display, wp->win,
		wp->text_gc,
		MENU_BORDER + SHADOW + 2 * NAME_OFFSET /* + offset * wp->font_width */,
		line_y + TEXT_OFFSET,
		line, len
	);

	offset += len;

	/*
	 *	Põe o nome do arquivo
	 */
	len = snprintf (line, DIR_LINE_SZ, "  %s", nm);

	XDrawString
	(	display, wp->win,
		mode_gc_vector[(np->n_ls.st_mode & S_IFMT) >> 12],
		MENU_BORDER + SHADOW + 2 * NAME_OFFSET + offset * wp->font_width,
		line_y + TEXT_OFFSET,
		line, len
	);

	offset += len;

	/*
	 *	O Elo simbólico
	 */
	if (S_ISLNK (np->n_ls.st_mode))
	{
		char		*link_nm;

		link_nm = alloca (np->n_ls.st_size + 1);

		len = snprintf (line, DIR_LINE_SZ, " => ");

		if (readlink (np->n_path, link_nm, np->n_ls.st_size + 1) >= 0)
			len += snprintf (line + len, DIR_LINE_SZ - len, "%s", link_nm);

		XDrawString
		(	display, wp->win,
			wp->text_gc,
			MENU_BORDER + SHADOW + 2 * NAME_OFFSET + offset * wp->font_width,
			line_y + TEXT_OFFSET,
			line, len
		);

		offset += len;

		if (np->n_s.st_mode == 0)
		{
			offset += 1;

			XPutImage
			(	display, wp->win,
				wp->text_gc, small_exclam_ximage,
				0, 0,
				MENU_BORDER + SHADOW + 2 * NAME_OFFSET + offset * wp->font_width,
				line_y + ICON_OFFSET,
				np->n_ximage->width, np->n_ximage->height
			);

			offset += 3;
		}
	}

	/*
	 *	Atualiza o tamanho máximo
	 */
	offset += 9;	/* O tamanho do botão mais o ícone */

	if (dir_max_line_len < offset)
		dir_max_line_len = offset;

}	/* end draw_dir_line */

/*
 ****************************************************************
 *	Acha os máximos das colunas				*
 ****************************************************************
 */
void
get_max_colsz (void)
{
	const NODE	*np, *forw_np;
	int		nlink_len, uid_len, gid_len, size_len;

	/*
	 *	Prólogo
	 */
	max_nlink_len = max_uid_len = max_gid_len = max_size_len = -1;

	if (dir_win_node == NONODE)
		return;

	/*
	 *	Agora acha os valores máximos
	 */
	for (np = dir_win_node->n_children; np != NONODE; np = forw_np)
	{
		forw_np = np->n_forw;

		if (streq (np->n_nm, ".."))
		{
			if (np->n_parent != NONODE)
				np = np->n_parent;

			if (np->n_parent != NONODE)
				np = np->n_parent;
		}

		if ((nlink_len = strlen (edit_decimal_number (np->n_ls.st_nlink))) > max_nlink_len)
			max_nlink_len = nlink_len;

		if ((uid_len = strlen (edit_uid (np->n_ls.st_uid))) > max_uid_len)
			max_uid_len = uid_len;

		if ((gid_len = strlen (edit_gid (np->n_ls.st_gid))) > max_gid_len)
			max_gid_len = gid_len;

		if (S_ISBLK (np->n_ls.st_mode) || S_ISCHR (np->n_ls.st_mode))
			size_len = strlen (edit_dev_number (np->n_ls.st_rdev));
		else
			size_len = strlen (edit_decimal_number (np->n_ls.st_size));

		if (size_len > max_size_len)
			max_size_len = size_len;
	}

}	/* end get_max_colsz */

/*
 ****************************************************************
 *	Edita um UID						*
 ****************************************************************
 */
const char *
edit_uid (int uid)
{
	const char	*str;

	if ((str = pwcache (uid)) != NOSTR)
		snprintf (global_area, sizeof (global_area), "<%s>", str);
	else
		snprintf (global_area, sizeof (global_area), "<%d>", uid);

	return (global_area);

}	/* end edit_uid */

/*
 ****************************************************************
 *	Edita um GID						*
 ****************************************************************
 */
const char *
edit_gid (int gid)
{
	const char	*str;

	if ((str = grcache (gid)) != NOSTR)
		snprintf (global_area, sizeof (global_area), "<%s>", str);
	else
		snprintf (global_area, sizeof (global_area), "<%d>", gid);

	return (global_area);

}	/* end edit_gid */

/*
 ****************************************************************
 *	Edita um número decimal					*
 ****************************************************************
 */
const char *
edit_decimal_number (int number)
{
	snprintf (global_area, sizeof (global_area), "%d", number);

	return (global_area);

}	/* end edit_decimal_number */

/*
 ****************************************************************
 *	Edita um dispositivo					*
 ****************************************************************
 */
const char *
edit_dev_number (int rdev)
{
	sprintf
	(	global_area, "%d/%d",		/* Ou "%3d/%-3d" */
		MAJOR (rdev),
		MINOR (rdev)
	);

	return (global_area);

}	/* end edit_dev_number */

/*
 ****************************************************************
 *	Imprime uma DATA					*
 ****************************************************************
 */
int
put_time (char *line, int len, time_t timeval)
{
	char		*tp;

	tp = btime (&timeval);

	len += snprintf				/* hora */
        (	line + len, DIR_LINE_SZ - len,
		"  %-5.5s ", tp
	);

	if (timeval < SEMANA || timeval >= AMANHA)
	{
		len += snprintf
	        (	line + len, DIR_LINE_SZ - len,
			"%4.4s %-6.6s", tp+20, tp+13
		);
	}
	else if (timeval >= HOJE)
	{
		len += snprintf
	        (	line + len, DIR_LINE_SZ - len,
			"HOJE %-6.6s", tp+13
		);
	}
	else
	{
		len += snprintf
	        (	line + len, DIR_LINE_SZ - len,
			" %-10.10s", tp+9
		);
	}

	return (len);

}	/* end put_time */

/*
 ****************************************************************
 *	Prepara as datas para o "ls"				*
 ****************************************************************
 */
void
calculate_today_time (void)
{
	time_t         	 	difuso;

	/*
	 *	Prepara as datas para o cálculo de HOJE e dias da SEMANA
	 */
	time (&HOJE);				/* Agora GMT */

	difuso = gettzmin () * 60;

	HOJE += difuso;				/* Agora local */

	HOJE =  HOJE / (3600 * 24) * (3600 * 24);

	HOJE -= difuso;				/* Zero hora GMT ajustada */

	AMANHA = HOJE + (3600 * 24);		/* Zero hora de amanhã */

	SEMANA = HOJE - (3600 * 24 * 7);	/* Zero hora há uma semana */

#undef	DEBUG
#ifdef	DEBUG
	printf ("HOJE =		%s", btime (&HOJE));
	printf ("AMANHÃ =	%s", btime (&AMANHA));
	printf ("SEMANA =	%s", btime (&SEMANA));
#endif	DEBUG

}	/* end calculate_today_time */
