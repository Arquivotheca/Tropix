/*
 ****************************************************************
 *								*
 *			colors.c				*
 *								*
 *	Contagem de cores efetivas				*
 *								*
 *	Versão	3.0.0, de 15.03.98				*
 *		4.4.0, de 08.04.03				*
 *								*
 *	Módulo: cmd/xpaint					*
 *		X Windows - Clientes				*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright (c) 2003 TROPIX Technologies Ltd.	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "../h/xpaint.h"

#define	MAXDEPTH	24
#define	HASHSHIFT	19
#define	HASHSZ		(1 << HASHSHIFT)

/*
 ****************************************************************
 *	Acrescenta um pixel à tabela				*
 ****************************************************************
 */
int
add_pixel (ulong pixel)
{
	int		index, incr;
	static ulong	*hashtb;

	if (hashtb == NULL)
	{
		/* Inicialização */

		if ((hashtb = malloc (HASHSZ * sizeof (ulong))) == NULL)
			return (-1);
	}

	if (pixel == UNDEF)
	{
		memsetl (hashtb, UNDEF, HASHSZ);
		return (0);
	}

	index = (pixel >> (MAXDEPTH - HASHSHIFT)) & (HASHSZ - 1);
 	incr  = HASHSZ - 1;

	do
	{
		if (hashtb[index] == pixel)
			return (0);				/* cor velha */

		if (hashtb[index] == UNDEF)
			{ hashtb[index] = pixel; return (1); }	/* cor nova */

		if ((index += incr) >= HASHSZ)			/* colisão */
			index -= HASHSZ;

	}	while ((incr -= 2) > 0);

	msg ("Estouro da tabela HASH para contagem de cores");

	return (0);

}	/* end add_pixel */

/*
 ****************************************************************
 *	Aloca a imagem de trabalho				*
 ****************************************************************
 */
XImage *
alloc_image (Display *dpy, IMAGE *ip, int real_width, int real_height,
		int *width_ptr, int *height_ptr)
{
	int	width, height, factor;
	int	screen, depth, imagesz;
	char	*image;
	XImage	*ximage;

	if (ip->i_factor < 0)
	{
		factor	= -ip->i_factor + 1;
		width	= real_width  / factor + ((real_width  % factor) != 0);
		height	= real_height / factor + ((real_height % factor) != 0);
	}
	elif (ip->i_factor > 0)
	{
		factor	= ip->i_factor + 1;
		width	= real_width  * factor;
		height	= real_height * factor;
	}
	else
	{
		factor	= 1;
		width	= real_width;
		height	= real_height;
	}

	if (width <= 0 || height <= 0)
		return (NULL);

	screen  = DefaultScreen (dpy);
	depth   = DefaultDepth (dpy, screen);
	imagesz = width * height;

	if   (depth <= 8)		/* NÃO pode ser um "switch" */
		/* vazio */;
	elif (depth <= 16)
		imagesz <<= 1;
	elif (depth <= 32)
		imagesz <<= 2;
	else
		{ msg ("Profundidade inválida: %d\n", depth); return (NULL); }

        if ((image = malloc (imagesz)) == NULL)
                { msg ("Não há memória para a imagem"); return (NULL); }

        ximage =	XCreateImage
                        (       dpy, DefaultVisual (dpy, screen),
				depth, ZPixmap, 0, image,
                                width, height, 8, 0
                        );

	*width_ptr	= width;
	*height_ptr	= height;

	add_pixel (UNDEF);	/* Inicializa a contagem de cores */

	return (ximage);

}	/* end alloc_image */

/*
 ****************************************************************
 *	Acrescenta um pixel à imagem				*
 ****************************************************************
 */
int
put_pixel (IMAGE *ip, XImage *ximage, int xpos, int ypos, ulong pixel)
{
	int	x, y, factor;

	if (ip->i_factor == 0)
	{
		/* Tamanho natural */

		XPutPixel (ximage, xpos, ypos, pixel);
	}
	elif (ip->i_factor > 0)
	{
		/* Ampliação */

		factor = ip->i_factor + 1;

		for (x = 0; x < factor; x++)
		{
			for (y = 0; y < factor; y++)
			{
				XPutPixel (ximage, xpos * factor + x, ypos * factor + y, pixel);
			}
		}
	}
	else
	{
		/* Redução */

		factor = -ip->i_factor + 1;

		XPutPixel (ximage, xpos / factor, ypos / factor, pixel);
	}

	return (add_pixel (pixel));

}	/* end put_pixel */
