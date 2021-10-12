/*
 ****************************************************************
 *								*
 *			image.c					*
 *								*
 *	Leitura de imagens					*
 *								*
 *	Versão	4.5.0, de 20.01.04				*
 *		4.5.0, de 27.03.04				*
 *								*
 *	Módulo: xview						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2004 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <X11/Xatom.h>
#include <X11/Intrinsic.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "../h/xview.h"

/*
 ****************************************************************
 *	Constantes e Macros					*
 ****************************************************************
 */
entry FILE	*file_fp;

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
 ****************************************************************
 *	Verifica se o nome do arquivo é de um tipo conhecido	*
 ****************************************************************
 */
int
good_file_nm (const char *file_nm)
{
	char		*str;
	char		suffix[4];

	if ((str = strrchr (file_nm, '.')) == NOSTR)
		return (-1);

	if (strlen (++str) != 3)
		return (-1);

	strcpy (suffix, str);

	for (str = &suffix[0]; str < &suffix[3]; str++)
		*str = tolower (*str);

	if (streq (suffix, "gif"))
		return (1);

	if (streq (suffix, "jpg"))
		return (1);

	return (-1);

}	/* end good_file_nm */

/*
 ****************************************************************
 *	Carrega uma imagem					*
 ****************************************************************
 */
void
draw_image (int value)
{
	const char	*file_nm;
	FILE		*fp;

	if (file_fp != NOFILE)
		fclose (file_fp);

	if (value >= 0)
	{
		if (file_nm_argv[1] == NOSTR)
			return;

		file_nm = *++file_nm_argv;
	}
	else
	{
		if (file_nm_argv <= first_file_nm_argv)
			return;

		file_nm = *--file_nm_argv;
	}

	if ((file_fp = fp = fopen (file_nm, "r")) == NOFILE)
		{ draw_error_msg_text ("*Não consegui abrir \"%s\"", file_nm); return; }

	clear_msg_window ();

	if   (is_a_GIF_file (fp))
		load_GIF_file (file_nm, fp);
	elif (is_a_JPEG_file (fp))
		load_JPEG_file (file_nm, fp, StandardColormap);
	else
		{ draw_error_msg_text ("O arquivo \"%s\" NÃO contém uma imagem GIF/JPEG", file_nm); goto bad; }

	/*
	 *	Epílogo
	 */
    bad:
	fclose (fp);

}	/* end draw_image */

/*
 ****************************************************************
 *	Aloca a imagem de trabalho				*
 ****************************************************************
 */
XImage *
alloc_image (int width, int height)
{
	int	screen, imagesz;
	char	*image;
	XImage	*value_ximage;

	screen  = DefaultScreen (display);
	depth   = DefaultDepth (display, screen);
	imagesz = width * height;

	if   (depth <= 8)		/* NÃO pode ser um "switch" */
		/* vazio */;
	elif (depth <= 16)
		imagesz <<= 1;
	elif (depth <= 32)
		imagesz <<= 2;
	else
		{ draw_error_msg_text ("Profundidade inválida: %d\n", depth); return (NULL); }

        if ((image = malloc (imagesz)) == NULL)
                { draw_error_msg_text ("Não há memória para a imagem"); return (NULL); }

        value_ximage = XCreateImage
	(       display, DefaultVisual (display, screen),
		depth, ZPixmap, 0, image,
		width, height, 8, 0
	);

	return (value_ximage);

}	/* end alloc_image */

/*
 ****************************************************************
 *	Imprime o total de memória gasta			*
 ****************************************************************
 */
void
mem_function (int value)
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
 *	Ajusta  o tamanho da janela da imagem			*
 ****************************************************************
 */
void
adjust_win_size (int width, int height)
{
	int		adjust = 0, clear = 0;

	/*
	 *	Se for igual à anterior, nada precisa fazer
	 */
	if (width == WINDOW_WIDTH && height == WINDOW_HEIGHT)
		return;

	/*
	 *	Examina se deve mexer no tamanho
	 */
	switch (adjust_mode)
	{
	    case ADJUST_PRESENT:
		adjust = 1;
	   /***	clear = 0; ***/

		break;

	    case ADJUST_MAX:
		if (width < WINDOW_WIDTH)
			{ width = WINDOW_WIDTH; clear = 1; }
		else /*	width > WINDOW_WIDTH */
			adjust = 1;

		if (height < WINDOW_HEIGHT)
			{ height = WINDOW_HEIGHT; clear = 1; }
		else /*	height > WINDOW_HEIGHT */
			adjust = 1;

		break;

	}	/* end switch */

	/*
	 *	Mexe no tamanho
	 */
	if (adjust) set_window_sizes
	(
		present_root_width  + width  - WINDOW_WIDTH,
		present_root_height + height - WINDOW_HEIGHT
	);


	if (clear)
		XClearWindow (display, mandel.win);

}	/* end adjust_win_size */

/*
 ****************************************************************
 *	Desenha a janela da imagem, ...				*
 ****************************************************************
 */
void
draw_mandel_win (XExposeEvent *xexpose)
{
	WINDATA		*wp = &mandel;

#if (0)	/*******************************************************/
	/*
	 *	Verifica se é o fundo ...
	 */
	if (Rflag)
		{ draw_background (); exit (0); }
#endif	/*******************************************************/

	/*
	 *	Envia para o servidor o conteúdo do Pixmap (a imagem).
	 */
	if (ximage != NULL)
	{
		if (xexpose == NULL)
		{
			if
			(	XPutImage
				(	display, mandel.win, wp->triple.main_gc, ximage,
					0, 0, SHADOW, SHADOW, ximage->width, ximage->height
				)
			)
				error ("O servidor X rejeitou a imagem");
		}
		else
		{
			if (xexpose->x < SHADOW)
				xexpose->x = SHADOW;

			if (xexpose->y < SHADOW)
				xexpose->y = SHADOW;

			if
			(	XPutImage
				(	display, mandel.win, wp->triple.main_gc, ximage,
					xexpose->x - SHADOW, xexpose->y - SHADOW,
					xexpose->x, xexpose->y,
					xexpose->width + SHADOW, xexpose->height + SHADOW
				)
			)
				error ("O servidor X rejeitou a imagem");
		}
	}

	/*
	 *	x
	 */
	draw_shadow
	(	wp->win,
		wp->triple.top_gc, wp->triple.bottom_gc,
		0, 0,
		wp->width, wp->height
	);

}	/* end draw_mandel_win */

/*
 ****************************************************************
 *	A Imagem torna-se o fundo da janela			*
 ****************************************************************
 */
void
draw_background (int unused)
{
	Pixmap		pic;
	int		screen, code;
	Window		root;

	/*
	 *	Cria o Pixmap correspondente à imagem
	 */
	screen = DefaultScreen (display); root = RootWindow (display, screen);

	if ((pic = XCreatePixmap (display, root, ximage->width, ximage->height, depth)) == 0)
		error ("$O servidor X não pode criar o Mapa de Pixels");

	if (XPutImage (display, pic, mandel.triple.main_gc, ximage, 0, 0, 0, 0, ximage->width, ximage->height))
		error ("$O servidor X rejeitou a imagem");

	if ((code = XSetWindowBackgroundPixmap (display, root, pic)) == 0)
		error ("$O servidor X não aceitou o fundo, erro = %d", code);

	XClearWindow (display, root);

}	/* end draw_background */
