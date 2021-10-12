/*
 ****************************************************************
 *								*
 *			jpeg.c					*
 *								*
 *	Leitura do formato JPEG					*
 *								*
 *	Versão  3.2.3, de 14.05.00				*
 *		4.4.0, de 08.04.03				*
 *								*
 *	Módulo: cmd/xpaint					*
 *		X Windows - Clientes				*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2003 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <X11/jpeglib.h>

#include "../h/xpaint.h"

/*
 ****************************************************************
 *	Confere se o arquivo é JPEG				*
 ****************************************************************
 */
int
is_a_JPG_file (FILE *fp)
{
	return (getc (fp) == 0xFF && getc (fp) == 0xD8);

}	/* end is_a_JPG_file */

/*
 ****************************************************************
 *	Lê um arquivo JPEG, gerando uma "XImage"		*
 ****************************************************************
 */
XImage *
load_JPG_file (FILE *fp, int image_number, Display *dpy,
		XStandardColormap *cmap, IMAGE *ip)
{
        struct jpeg_decompress_struct	cinfo;
        struct jpeg_error_mgr		jerr;
	JSAMPARRAY			area;
	JSAMPROW			area_ptr;

	XImage				*ximage = NULL;
	int				width, height, column;

	int				used_colors_count = 0;
	XColor				xcol;

	if (image_number != 1)
		{ msg ("Arquivos JPEG \"%s\" têm apenas uma imagem", ip->i_name); return (NULL); }

        cinfo.err = jpeg_std_error (&jerr);

        jpeg_create_decompress (&cinfo);

	jpeg_stdio_src (&cinfo, fp);

	jpeg_read_header (&cinfo, TRUE);

	jpeg_start_decompress (&cinfo);

	area  = (*cinfo.mem->alloc_sarray)
		(	(j_common_ptr) &cinfo, JPOOL_IMAGE,
			cinfo.output_width * cinfo.output_components, 1
		);

	if (area == NULL)
		{ msg ("Memória insuficiente"); goto out; }

	/*
	 *	Aloca a imagem de trabalho.
	 */
	ximage = alloc_image (dpy, ip, cinfo.output_width, cinfo.output_height, &width, &height);

	if (ximage == NULL)
		{ msg ("Não foi possível criar a imagem"); goto out; }

	/*
	 *	Gera a imagem.
	 */
	while (cinfo.output_scanline < cinfo.output_height)
	{
		jpeg_read_scanlines (&cinfo, area, 1);

		area_ptr = *area;

		for (column = 0; column < cinfo.output_width; column++)
		{
			/* Obtém a tripla (red, green, blue) */

			xcol.red = *area_ptr;

			if (cinfo.output_components > 1)
				area_ptr++;

			xcol.green = *area_ptr;

			if (cinfo.output_components > 2)
				area_ptr++;

			xcol.blue = *area_ptr;

			if (cinfo.output_components > 3)
				area_ptr++;

			area_ptr++;

			/* Calcula o pixel correspondente */

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

			/* Acrescenta o pixel à imagem sendo preenchida */

			used_colors_count += put_pixel (ip, ximage, column, cinfo.output_scanline - 1, xcol.pixel);

		}	/* end for (varrendo colunas) */

	} 	/* end while (varrendo linhas) */

    out:
	jpeg_finish_decompress (&cinfo);

	jpeg_destroy_decompress (&cinfo);

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

}	/* end load_JPG_file */
