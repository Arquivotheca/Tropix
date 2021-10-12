/*
 ****************************************************************
 *								*
 *			bmp.c					*
 *								*
 *	Leitura do formato BMP (Windows)			*
 *								*
 *	Versão	3.0.0, de 05.04.98				*
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
#include <errno.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "../h/xpaint.h"

/*
 ****************************************************************
 *	Cabeçalho do arquivo .bmp				*
 ****************************************************************
 */
#define	elif		else if

#define	DSIZE		40

typedef	struct
{
	long	totalsize,		/* Tamanho total do arquivo */
		filler1,
		imagebegin,		/* Início da imagem */
		descsize,
		width,			/* Largura */
		height;			/* Altura */

	ushort	planes,
		bitsperpixel;		/* Nr. de bits por pixel */

	long	compression,
		imagesize,
		xpixpermeter,
		ypixpermeter,
		ncolors,
		importantcolors;

}	BMPHEADER;

/*
 ****************************************************************
 *	Protótipos de Funções					*
 ****************************************************************
 */
int		get_pixel (FILE *, int);
int		read_BMP_color_table (FILE *, COLORMAP *);

/*
 ****************************************************************
 *	Confere se o arquivo é BMP				*
 ****************************************************************
 */
int
is_a_BMP_file (FILE *fp)
{
	STAT		st;
	BMPHEADER	h;

	return
	(	fstat (fileno (fp), &st) >= 0 &&
		getc (fp) == 'B' && getc (fp) == 'M' &&
		fread (&h, sizeof (h), 1, fp) == 1 &&
		h.descsize == DSIZE && st.st_size == h.totalsize &&
		h.imagebegin >= sizeof (h) + 2 &&
		(h.bitsperpixel <= 8 || h.bitsperpixel == 24)
	);

}	/* end is_a_BMP_file */

/*
 ****************************************************************
 *	Obtém um pixel, desmembrando um byte do arquivo		*
 ****************************************************************
 */
int
get_pixel (FILE *fp, int shift)		/* shift = 1, 2, 4 ou 8 */
{
	static int	indbit = 0, x = 0;

	if (indbit == 0)
		{ x = getc (fp); indbit = 8; }

	indbit -= shift;

	return ((x >> indbit) & ((1 << shift) - 1));

}	/* end get_pixel */

/*
 ****************************************************************
 *	Lê um arquivo BMP, gerando uma "XImage"			*
 ****************************************************************
 */
XImage *
load_BMP_file (FILE *fp, int image_number, Display *dpy,
		XStandardColormap *cmap, IMAGE *ip)
{
	BMPHEADER	h;
	XImage		*ximage;
	int		x, y;
	int		pixels_per_line;
	int		width, height;

	COLORMAP	*clp = NULL;
	int		used_colors_count = 0;
	int		rgb_index;
	XColor		xcol;

	if (image_number != 1)
		{ msg ("Arquivos BMP \"%s\" contêm apenas uma imagem", ip->i_name); return (NULL); }

	getc (fp); getc (fp);	/* Pula o 'B' e o 'M' */

	if (fread (&h, sizeof (h), 1, fp) != 1)
		{ msg ("Erro na leitura do cabeçalho BMP"); return (NULL); }

	pixels_per_line	= ((h.totalsize - h.imagebegin) / h.height) * (8 / h.bitsperpixel);

	/*
	 *	Lê a tabela de cores, se existir.
	 */
	if (h.imagebegin > sizeof (h) + 2)
	{
		clp = alloca (sizeof (COLORMAP));

		clp->c_size = 1 << h.bitsperpixel;

		if (read_BMP_color_table (fp, clp))
			{ msg ("Erro na leitura do mapa de cores"); return (NULL); }
	}

	/*
	 *	Aloca a imagem de trabalho.
	 */
	ximage = alloc_image (dpy, ip, h.width, h.height, &width, &height);

        if (ximage == NULL)
                { msg ("$Não foi possível criar a imagem"); return (NULL); }

	/*
	 *	Gera a imagem. No arquivo ".bmp", ela aparece invertida!
	 */
	for (y = h.height - 1; y >= 0; y--)
	{
		/* Lê uma linha do arquivo ".bmp" */

		for (x = 0; x < h.width; x++)
		{
			if (h.bitsperpixel <= 8)
			{
				rgb_index  = get_pixel (fp, h.bitsperpixel) & (CMAPSZ - 1);

				/* Obtém a tripla (red, green, blue) */

				xcol.red   = clp->c_red   [rgb_index];
				xcol.green = clp->c_green [rgb_index];
				xcol.blue  = clp->c_blue  [rgb_index];

				/* Calcula o pixel correspondente */

				if ((xcol.pixel = clp->c_pixel[rgb_index]) == UNDEF)
				{
					if (cmap == NULL)
					{
						xcol.flags   = DoRed | DoGreen | DoBlue;
						xcol.red   <<= 8;
						xcol.green <<= 8;
						xcol.blue  <<= 8;

						XAllocColor (dpy, DefaultColormap (dpy, DefaultScreen (dpy)), &xcol);
					}
					else
					{
						xcol.pixel = cmap->base_pixel +
							(((xcol.red   * cmap->red_max)   >> 8) * cmap->red_mult) +
							(((xcol.green * cmap->green_max) >> 8) * cmap->green_mult) +
							(((xcol.blue  * cmap->blue_max)  >> 8) * cmap->blue_mult);
					}

					clp->c_pixel[rgb_index] = xcol.pixel;
				}
			}
			elif (h.bitsperpixel == 24)
			{
				xcol.blue  = getc (fp);
				xcol.green = getc (fp);
				xcol.red   = getc (fp);

				if (cmap == NULL)
				{
					xcol.flags   = DoRed | DoGreen | DoBlue;
					xcol.red   <<= 8;
					xcol.green <<= 8;
					xcol.blue  <<= 8;

					XAllocColor (dpy, DefaultColormap (dpy, DefaultScreen (dpy)), &xcol);
				}
				else
				{
					xcol.pixel = cmap->base_pixel +
						(((xcol.red   * cmap->red_max)   >> 8) * cmap->red_mult) +
						(((xcol.green * cmap->green_max) >> 8) * cmap->green_mult) +
						(((xcol.blue  * cmap->blue_max)  >> 8) * cmap->blue_mult);
				}
			}

			/* Acrescenta o pixel à imagem sendo preenchida */

			used_colors_count += put_pixel (ip, ximage, x, y, xcol.pixel);
		}

		for (/* continuação da malha */; x < pixels_per_line; x++)
			get_pixel (fp, h.bitsperpixel);
	}

	/*
	 *	Epílogo.
	 */
	if (ip != NULL)
	{
		ip->i_width	= width;
		ip->i_height	= height;
		ip->i_ncolors	= used_colors_count;
	}

	return (ximage);

}	/* end load_BMP_file */

/*
 ****************************************************************
 *	Lê a Tabela de Cores do BMP				*
 ****************************************************************
 */
int
read_BMP_color_table (FILE *fp, COLORMAP *clp)
{
	int	index;
	uchar	*rgb;

	rgb = alloca (clp->c_size << 2);

	if (fread (rgb, 4, clp->c_size, fp) != clp->c_size)
		{ msg ("Erro na leitura do mapa de cores"); return (1); }

	for (index = 0; index < clp->c_size; index++)
	{
		clp->c_blue [index]  = *rgb++;
		clp->c_green[index]  = *rgb++;
		clp->c_red  [index]  = *rgb++;
		clp->c_pixel[index]  = UNDEF;

		rgb++;
	}

	clp->c_grayscale = 0;

	return (0);

}	/* end read_BMP_color_table */
