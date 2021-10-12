/*
 ****************************************************************
 *								*
 *			jpeg.c					*
 *								*
 *	Leitura de imagens em formato JPEG			*
 *								*
 *	Versão	4.5.0, de 27.03.04				*
 *		4.5.0, de 28.03.04				*
 *								*
 *	Módulo: xpaint						*
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

#include <X11/Intrinsic.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/jpeglib.h>

#include "../h/xview.h"

/*
 ****************************************************************
 *	Constantes e Macros					*
 ****************************************************************
 */

/*
 ******	Protótipos de funções ***********************************
 */

/*
 ****************************************************************
 *	Verifica se o arquivo é JPEG				*
 ****************************************************************
 */
int
is_a_JPEG_file (FILE *fp)
{
	fseek (fp, 0, SEEK_SET);

	return (getc (fp) == 0xFF && getc (fp) == 0xD8);

}	/* end is_a_JPEG_file */

/*
 ****************************************************************
 *	Lê um arquivo JPEG, gerando uma "XImage"		*
 ****************************************************************
 */
int
load_JPEG_file (const char *file_nm, FILE *fp, const XStandardColormap *stdcmap)
{
        struct jpeg_decompress_struct	cinfo;
        struct jpeg_error_mgr		jerr;
	JSAMPARRAY			area;
	JSAMPROW			area_ptr;
	int				column;
	XColor				xcol;

	fseek (fp, 0, SEEK_SET);

	/*
	 *	x
	 */
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
                { draw_error_msg_text ("Memória esgotada"); return (-1); }

	/*
	 *	Aloca a imagem de trabalho
	 */
	if (ximage != NULL)
		XDestroyImage (ximage);

	if ((ximage = alloc_image (cinfo.output_width, cinfo.output_height)) == NULL)
                { draw_error_msg_text ("Erro na criação da imagem"); return (-1); }

	draw_msg_text ("Imagem \"%s\", %dx%d", file_nm, cinfo.output_width, cinfo.output_height);

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

			if   (depth == 24)
			{
				xcol.pixel = (xcol.red << 16) | (xcol.green << 8) | xcol.blue;
			}
			elif (stdcmap != NULL)
			{
				xcol.pixel = stdcmap->base_pixel +
					(((xcol.red   * stdcmap->red_max)   >> 8) * stdcmap->red_mult) +
					(((xcol.green * stdcmap->green_max) >> 8) * stdcmap->green_mult) +
					(((xcol.blue  * stdcmap->blue_max)  >> 8) * stdcmap->blue_mult);
			}
			else
			{
				xcol.red   <<= 8;
				xcol.green <<= 8;
				xcol.blue  <<= 8;

				xcol.flags = DoRed | DoGreen | DoBlue;

				XAllocColor (display, DefaultColormap (display, DefaultScreen (display)), &xcol);
			}

			/* Acrescenta o pixel à imagem sendo preenchida */

			XPutPixel (ximage, column, cinfo.output_scanline - 1, xcol.pixel);

		}	/* end for (varrendo colunas) */

	} 	/* end while (varrendo linhas) */

	/*
	 *	x
	 */
	jpeg_finish_decompress (&cinfo);

	jpeg_destroy_decompress (&cinfo);

	/*
	 *	Ajusta o tamanho
	 */
	adjust_win_size (cinfo.output_width, cinfo.output_height);

	/*
	 *	x
	 */
	draw_mandel_win ((XExposeEvent *)NULL);

	return (0);


}	/* end load_JPG_file */
