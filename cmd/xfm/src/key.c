/*
 ****************************************************************
 *								*
 *			filemenu.c				*
 *								*
 *	Gerenciador de arquivos					*
 *								*
 *	Versão	4.5.0, de 24.06.03				*
 *		4.5.0, de 22.08.03				*
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
#include <fcntl.h>
#include <errno.h>

#include <X11/Intrinsic.h>
#include <X11/keysym.h>

#include "../h/xfm.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
entry KEYDATA		keydata;

/*
 ******	Protótipos de funções ***********************************
 */
void			xlineedit_cursor_on (void);
void			xlineedit_cursor_off (void);

/*
 ****************************************************************
 *	Desenha a janela de EDIÇÃO de NOMES			*
 ****************************************************************
 */
void
draw_nm_edit_win (void)
{
	const WINDATA	*wp = &nm_edit;
	const KEYDATA	*kp = &keydata;

	if (kp->k_count) XDrawImageString
	(	
		kp->k_display, kp->k_win,
		kp->k_gc,
		kp->k_x,
		kp->k_y,
		kp->k_area, kp->k_count
	);

	xlineedit_cursor_on ();

	draw_shadow
	(	wp->win,
		wp->triple.top_gc, wp->triple.bottom_gc,
		0, 0,
		wp->width, wp->height
	);

}	/* end draw_nm_edit_win */

/*
 ****************************************************************
 *	Edita uma linha no estilo do "vi"			*
 ****************************************************************
 */
void
do_key_press (XEvent event)
{
	KEYDATA		*kp = &keydata;
	int		len;
	KeySym		key;
	char		text[8];

	/*
	 *	Obtém o caractere
	 */
	len = XLookupString (&event.xkey, text, sizeof (text), &key, 0);

	/*
	 *	Os diversos caracteres de controle
	 */
	if   (key == XK_Return || key == XK_KP_Enter || key == XK_Linefeed)
	{
		kp->k_function (); return;
	}
	elif (key == XK_Delete)
	{
		if ((len = kp->k_count - kp->k_offset) <= 0)
			return;

		memmove (&kp->k_area[kp->k_offset], &kp->k_area[kp->k_offset + 1], len);

		kp->k_count--;

		XDrawImageString
		(	kp->k_display, kp->k_win,
			kp->k_gc,
			kp->k_x + kp->k_offset * kp->k_font_width,
			kp->k_y,
			&kp->k_area[kp->k_offset], len
		);

		xlineedit_cursor_on ();

		kp->k_changed = 1;

		return;
	}
	elif (key == XK_BackSpace)
	{
		if (kp->k_offset <= 0)
			return;

		len = kp->k_count - kp->k_offset + 1;

		kp->k_offset--; kp->k_count--;

		memmove (&kp->k_area[kp->k_offset], &kp->k_area[kp->k_offset + 1], len);

		XDrawImageString
		(	kp->k_display, kp->k_win,
			kp->k_gc,
			kp->k_x + kp->k_offset * kp->k_font_width,
			kp->k_y,
			&kp->k_area[kp->k_offset], len + 1
		);

		xlineedit_cursor_on ();

		kp->k_changed = 1;

		return;
	}
	elif (key == XK_Left)
	{
		if (kp->k_offset <= 0)
			return;

		xlineedit_cursor_off ();

		kp->k_offset--;

		xlineedit_cursor_on ();

		return;
	}
	elif (key == XK_Right)
	{
		if (kp->k_offset >= kp->k_count)
			return;

		xlineedit_cursor_off ();

		kp->k_offset++;

		xlineedit_cursor_on ();

		return;
	}
	elif (key == XK_Home)
	{
		if (kp->k_offset <= 0)
			return;

		xlineedit_cursor_off ();

		kp->k_offset = 0;

		xlineedit_cursor_on ();

		return;
	}
	elif (key == XK_End)
	{
		if (kp->k_offset >= kp->k_count)
			return;

		xlineedit_cursor_off ();

		kp->k_offset = kp->k_count;

		xlineedit_cursor_on ();

		return;
	}

	/*
	 *	Um texto; verifica se há espaço
	 */
	if (len == 0 || kp->k_count >= kp->k_size - 1)
		return;

	/*
	 *	Insere o texto
	 */
	len = kp->k_count - kp->k_offset + 1;

	memmove (&kp->k_area[kp->k_offset + 1], &kp->k_area[kp->k_offset], len);

	kp->k_area[kp->k_offset] = text[0];

	XDrawImageString
	(	kp->k_display, kp->k_win,
		kp->k_gc,
		kp->k_x + kp->k_offset * kp->k_font_width,
		kp->k_y,
		&kp->k_area[kp->k_offset], len + 1
	);

	kp->k_count++; kp->k_offset++;

	xlineedit_cursor_on ();

	kp->k_changed = 1;

	return;

}	/* end xlineedit */

/*
 ****************************************************************
 *	Coloca o cursor						*
 ****************************************************************
 */
void
xlineedit_cursor_on (void)
{
	const KEYDATA	*kp = &keydata;
	char		c;

	if ((c = kp->k_area[kp->k_offset]) == '\0')
		c = ' ';

	XDrawImageString
	(	kp->k_display, kp->k_win,
		kp->k_rev_gc,
		kp->k_x + kp->k_offset * kp->k_font_width,
		kp->k_y,
		&c, 1
	);

}	/* end xlineedit_cursor_on */

/*
 ****************************************************************
 *	Tira o cursor						*
 ****************************************************************
 */
void
xlineedit_cursor_off (void)
{
	const KEYDATA	*kp = &keydata;
	char		c;

	if ((c = kp->k_area[kp->k_offset]) == '\0')
		c = ' ';

	XDrawImageString
	(	kp->k_display, kp->k_win,
		kp->k_gc,
		kp->k_x + kp->k_offset * kp->k_font_width,
		kp->k_y,
		&c, 1
	);

}	/* end xlineedit_cursor_off */
