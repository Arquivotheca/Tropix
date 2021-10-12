/*
 ****************************************************************
 *								*
 *			df.c					*
 *								*
 *	Gerenciador de arquivos					*
 *								*
 *	Versão	4.5.0, de 09.10.03				*
 *		4.5.0, de 09.10.03				*
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

#include <stdio.h>
#include <stdlib.h>
#include <stat.h>
#include <ustat.h>

#include <X11/Intrinsic.h>

#include "../h/xfm.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	*fs_names[] = FS_NAMES;

/*
 ******	Conversões para KB e MB *********************************
 */
#define	BLTOKB_INT(x)	((unsigned)(x) >> (KBSHIFT-BLSHIFT))
#define	BLTOKB_DEC(x)	(((x) & (KBSZ/BLSZ-1)) * (100 / (KBSZ/BLSZ)))


#define	BLTOMB_INT(x)	((unsigned)(x) >> (MBSHIFT-BLSHIFT))
#define	BLTOMB_DEC(x)	((100 * ((unsigned)(x) & (MBSZ/BLSZ - 1))) >> (MBSHIFT-BLSHIFT))

/*
 ****************************************************************
 *	Imprime a linha						*
 ****************************************************************
 */
void
print_df_entry (void)
{
	const MENUDATA	*mp = &menudata[present_menu];
	const WINDATA	*menu_wp = &tree_menu;
	WINDATA		*wp = &df_data;
	const NODE	*dir_np;
	long		total, used, tfree;
	const FSTAB	*tp;
	int		ratio;
	XWindowChanges	value;
	char		total_area[16], used_area[16], free_area[16];
	STAT		dev_s;
	USTAT		u;

	/*
	 *	Prólogo
	 */
	if ((dir_np = mp->menu_node) == NONODE)
		return;

	if ((tp = search_dir_in_fstab (dir_np->n_path)) == NOFSTAB)
		return;

	if (stat (tp->fs_dev_nm, &dev_s) < 0)
		{ draw_error_msg_text ("*Não consegui obter o estado de \"%s\"", tp->fs_dev_nm); return; }

	if (ustat (dev_s.st_rdev, &u) < 0)
		{ draw_error_msg_text ("*Não consegui obter o estado de \"%s\"", tp->fs_dev_nm); return; }

	/*
	 *	Calculas os valores
	 */
	ratio = u.f_bsize >> BLSHIFT;

	total = u.f_fsize * ratio;
	tfree = u.f_tfree * ratio;

	used  = total - tfree;

	/*
	 *	Prepara o texto
	 */
	edit_len = snprintf
	(	edit_area, sizeof (edit_area), "%-6.6s%-16.15s%11s %11s %11s %8.2f",
		fs_names[u.f_type], tp->fs_dev_nm, edit_sz_value (total, total_area),
		edit_sz_value (used, used_area), edit_sz_value (tfree, free_area),
		100.0 * (double)used/(double)total + 0.005
	);

	/*
	 *	Prepara o tamanho da janela
	 */
	value.width  = wp->width  = 2 * (MENU_BORDER + SHADOW) + edit_len * wp->font_width;
	value.height = wp->height = 2 * (MENU_BORDER + SHADOW) + 2 * wp->font_height + 2 * SHADOW;

	value.x = menu_wp->x + menu_wp->width;
	value.y = menu_wp->y + 6 * (wp->font_height + 2 * SHADOW);

	XConfigureWindow (display, wp->win, CWWidth|CWHeight|CWX|CWY, &value);

	XMapWindow (display, wp->win);

}	/* end proc_entry */

/*
 ****************************************************************
 *	Prepara o texto com a unidade pedida			*
 ****************************************************************
 */
const char *
edit_sz_value (unsigned long value, char *area)
{
	if   (value < (10 * MBSZ / BLSZ))		/* Menor do que 10 MB */

		sprintf (area, "%d.%02d KB", BLTOKB_INT (value), BLTOKB_DEC (value));
	else
		sprintf (area, "%d.%02d MB", BLTOMB_INT (value), BLTOMB_DEC (value));

	return (area);

}	/* end edit_sz_value */

/*
 ****************************************************************
 *	Desenha a janela do DF					*
 ****************************************************************
 */
void
draw_df_data_window (void)
{
	WINDATA		*wp = &df_data;
	const char	*str = "TIPO  DISPOSITIVO        TOTAL      OCUPADO      LIVRE         %";

	XDrawImageString
	(	
		display, wp->win,
		wp->text_gc,
		MENU_BORDER + SHADOW,
		MENU_BORDER + 1 * (wp->font_height + 2 * SHADOW) - (wp->font_descent + SHADOW),
		str, strlen (str)
	);

	XDrawImageString
	(	
		display, wp->win,
		wp->text_gc,
		MENU_BORDER + SHADOW,
		MENU_BORDER + 2 * (wp->font_height + 2 * SHADOW) - (wp->font_descent + SHADOW),
		edit_area, edit_len
	);

	draw_shadow
	(	wp->win,
		wp->triple.top_gc, wp->triple.bottom_gc,
		0, 0,
		wp->width, wp->height
	);

}	/* end draw_df_data_window */
