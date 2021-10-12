/*
 ****************************************************************
 *								*
 *			compute.c				*
 *								*
 *	Calcula os pontos da imagem				*
 *								*
 *	Versão	4.5.0, de 27.12.03				*
 *		4.5.0, de 27.12.03				*
 *								*
 *	Módulo: xmandel						*
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
#include <math.h>
#include <stat.h>

#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Intrinsic.h>
#include <X11/keysym.h>
#include <X11/cursorfont.h>
#if (0)	/*******************************************************/
#endif	/*******************************************************/

#include "../h/xmandel.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
entry int	MANDEL_WIDTH, MANDEL_HEIGHT;

entry IMAGE_PARAM	image_param[N_IMAGE_PARAM] =
{
	-0.75, 0.0, 4.0		/* Figura inicial */
};

entry IMAGE_PARAM	*image_param_ptr = &image_param[0];

entry int	*count_matrix;

entry ulong	image_col_vector[256];

entry ulong	gif_col_vector[256];

entry char	col_count_vector[256];

entry char	*image;

entry XImage	*ximage;

entry GC	gc, rubber_band_gc;

entry int	zoom_x, zoom_y;

#if (0)	/*******************************************************/
entry int	last_motion_x, last_motion_y;
#endif	/*******************************************************/

entry int	last_width = -1, last_height;
entry int	last_x, last_y;

entry int	zoom_button_state;

entry int	rectangle_on;

/*
 ****************************************************************
 *	Cria o vetor de cores					*
 ****************************************************************
 */
void
create_color_vector (int changed)
{
	static int		first_time = 1;
	int			i, r, g, b;
	ulong			pixel = 0;
	const XStandardColormap	*cmap;
	int			brightness_1 = (color_brightness >> 8);
	int			brightness_2 = (color_brightness & 0xFF);

	/*
	 *	Cria o vetor de cores
	 */
	if (!first_time && !changed)
		return;

	first_time = 0;

	gif_col_vector[0] = 0;	/* Especial para o preto */

	for (i = 0; i < 1 * 85; i++)
	{
		r = brightness_1;

		g = ((brightness_2 * i) / 85);

		b = brightness_2 - g;

#if (0)	/*******************************************************/
		r = 255;

		g = ((255 * i) / 85);

		b = 255 - g;
#endif	/*******************************************************/

#if (0)	/*******************************************************/
		r = r * color_brightness / 255;
		g = g * color_brightness / 255;
		b = b * color_brightness / 255;
#endif	/*******************************************************/

		switch (color_order)
		{
		    case RGB:
			pixel = (r << 16) | (g << 8) | b;
			break;

		    case GBR:
			pixel = (g << 16) | (b << 8) | r;
			break;

		    case BRG:
			pixel = (b << 16) | (r << 8) | g;
			break;

		    case BGR:
			pixel = (b << 16) | (g << 8) | r;
			break;

		    case GRB:
			pixel = (g << 16) | (r << 8) | b;
			break;

		    case RBG:
			pixel = (r << 16) | (b << 8) | g;
			break;

		}	/* end switch */

		gif_col_vector[i + 1] = pixel;
	}

	for (/* acima */; i <  2 * 85; i++)
	{
		g = brightness_1;

		b = ((brightness_2 * (i - 85)) / 85);

		r = brightness_2 - b;

#if (0)	/*******************************************************/
		g = 255;

		b = (255 * (i - 85)) / 85;

		r = 255 - b;
#endif	/*******************************************************/

#if (0)	/*******************************************************/
		r = r * color_brightness / 255;
		g = g * color_brightness / 255;
		b = b * color_brightness / 255;
#endif	/*******************************************************/

		switch (color_order)
		{
		    case RGB:
			pixel = (r << 16) | (g << 8) | b;
			break;

		    case GBR:
			pixel = (g << 16) | (b << 8) | r;
			break;

		    case BRG:
			pixel = (b << 16) | (r << 8) | g;
			break;

		    case BGR:
			pixel = (b << 16) | (g << 8) | r;
			break;

		    case GRB:
			pixel = (g << 16) | (r << 8) | b;
			break;

		    case RBG:
			pixel = (r << 16) | (b << 8) | g;
			break;

		}	/* end switch */

		gif_col_vector[i + 1] = pixel;
	}

	for (/* acima */;  i <  3 * 85; i++)
	{
		b = brightness_1;

		r = ((brightness_2 * (i - 170)) / 85);

		g = brightness_2 - r;

#if (0)	/*******************************************************/
		b = 255;

		r = (255 * (i - 170)) / 85;

		g = 255 - r;
#endif	/*******************************************************/

#if (0)	/*******************************************************/
		r = r * color_brightness / 255;
		g = g * color_brightness / 255;
		b = b * color_brightness / 255;
#endif	/*******************************************************/

		switch (color_order)
		{
		    case RGB:
			pixel = (r << 16) | (g << 8) | b;
			break;

		    case GBR:
			pixel = (g << 16) | (b << 8) | r;
			break;

		    case BRG:
			pixel = (b << 16) | (r << 8) | g;
			break;

		    case BGR:
			pixel = (b << 16) | (g << 8) | r;
			break;

		    case GRB:
			pixel = (g << 16) | (r << 8) | b;
			break;

		    case RBG:
			pixel = (r << 16) | (b << 8) | g;
			break;

		}	/* end switch */

		gif_col_vector[i + 1] = pixel;
	}

	/*
	 *	Converte, de acordo com a profundidade
	 */
	if (depth == 24)
	{
		memmove (image_col_vector, gif_col_vector, sizeof (image_col_vector));
	}
	elif ((cmap = StandardColormap) == NULL)
	{
		int	defcolormap = DefaultColormap (display, DefaultScreen (display));

		for (i = 0; i < 256; i++)
		{
			XColor		xcol;

			pixel = gif_col_vector[i];

			xcol.red   = (pixel >> 8) & 0xFF00;
			xcol.green =  pixel	  & 0xFF00;
			xcol.blue  = (pixel << 8);

			xcol.flags   = DoRed | DoGreen | DoBlue;

			XAllocColor (display, defcolormap, &xcol);

			image_col_vector[i] = xcol.pixel;
		}
	}
	else
	{
		for (i = 0; i < 256; i++)
		{
			pixel = gif_col_vector[i];

			pixel = cmap->base_pixel +
				((((pixel >> 16)         * cmap->red_max)   >> 8) * cmap->red_mult) +
				(((((pixel >> 8) & 0xFF) * cmap->green_max) >> 8) * cmap->green_mult) +
				((((pixel & 0xFF)        * cmap->blue_max)  >> 8) * cmap->blue_mult);

			image_col_vector[i] = pixel;
		}
	}

#if (0)	/*******************************************************/
printf ("%P, %P, %P, %P\n", col_vector[1], col_vector[2], col_vector[254], col_vector[255]);
#endif	/*******************************************************/

}	/* end create_color_vector */

/*
 ****************************************************************
 *	Prelúdio						*
 ****************************************************************
 */
void
prelude (void)
{
	XGCValues	gcv;
	int		screen, imagesz;

	/*
	 *	x
	 */
	screen	= DefaultScreen (display);

#if (0)	/*******************************************************/
printf ("depth = %d\n", depth);
#endif	/*******************************************************/

	imagesz = MANDEL_WIDTH * MANDEL_HEIGHT;

	if   (depth <= 8)		/* NÃO pode ser um "switch" */
		/* vazio */;
	elif (depth <= 16)
		imagesz <<= 1;
	elif (depth <= 32)
		imagesz <<= 2;
	else
		{ error ("Profundidade inválida: %d\n", depth); return; }

	/*
	 *	Desaloca as antigas
	 */
        if (ximage != NULL)
	        XDestroyImage (ximage);

   /***	if (image != NULL)	***/
   /***		free (image);	***/

        if (count_matrix != NULL)
	        free (count_matrix);

	/*
	 *	Aloca
	 */
        if ((image = malloc (imagesz)) == NULL)
                { error ("Não há memória para a imagem"); return; }

        if ((count_matrix = malloc (MANDEL_WIDTH * MANDEL_HEIGHT * sizeof (int))) == NULL)
                { error ("Não há memória para a imagem"); return; }

        ximage = XCreateImage
                 (	display, DefaultVisual (display, screen),
			depth, ZPixmap, 0, image,
                        MANDEL_WIDTH, MANDEL_HEIGHT, 8, 0
                 );

	/*
	 *	Prepara os GCs
	 */
	if (gc == NULL)
	{
		gcv.foreground = WhitePixel (display, screen);
		gcv.background = BlackPixel (display, screen);

		gc = XCreateGC (display, mandel.win, GCForeground|GCBackground, &gcv);

		/*
		 ****** Prepara a cor para mover a janela do INTERVALO **
		 */
		gcv.foreground		= WhitePixel (display, screen);
		gcv.subwindow_mode	= IncludeInferiors;
		gcv.function		= GXxor;

		rubber_band_gc = XCreateGC (display, mandel.win, GCForeground|GCSubwindowMode|GCFunction, &gcv);

	}

}	/* end prelude */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
compute (int value)
{
	int			x, y;
	double			c_x, c_y;
	double			z_x, z_y;
	double			new_z_x, new_z_y;
	double			orig_x, orig_y;
	ulong			pixel;
	int			count, index;
	double			SIZE_Y;
	IMAGE_PARAM		*ip = image_param_ptr;

	/*
	 *	Cria o vetor de cores, e outras coisas ...
	 */
	MANDEL_WIDTH  = WINDOW_WIDTH;
	MANDEL_HEIGHT = WINDOW_HEIGHT;

	create_color_vector (0);

	prelude ();

	rectangle_on = 0;

	memsetl (col_count_vector, 0, sizeof (col_count_vector) / sizeof (long));

	/*
	 *	x
	 */
	if (ip < &image_param[N_IMAGE_PARAM - 1] && (ip + 1)->i_SIZE_X != 0.0)
		{ ip++; image_param_ptr++; }

	/*
	 *	Calcula as origens
	 */
	SIZE_Y	= ip->i_SIZE_X * (double)MANDEL_HEIGHT / (double)MANDEL_WIDTH;

	orig_x  = ip->i_CENTER_X - (ip->i_SIZE_X / 2.0);
	orig_y	= ip->i_CENTER_Y - (SIZE_Y / 2.0);

	draw_msg_text
	(	"[%G, %G, %G], %dx%d",
		ip->i_CENTER_X, ip->i_CENTER_Y, ip->i_SIZE_X,
		MANDEL_WIDTH, MANDEL_HEIGHT
	);

	/*
	 *	Aqui está o cálculo propriamente dito
	 */
	for (x = 0; x < MANDEL_WIDTH; x++)
	{
		c_x = orig_x + ((double)x * ip->i_SIZE_X / (double)MANDEL_WIDTH);

		for (y = 0; y < MANDEL_HEIGHT; y++)
		{
			c_y = orig_y + ((double)(MANDEL_HEIGHT - y) * SIZE_Y / (double)MANDEL_HEIGHT);

			for (z_x = 0.0, z_y = 0.0, count = 0; /* abaixo */; count++)
			{
				new_z_x = z_x * z_x - z_y * z_y + c_x;
				new_z_y = 2 * z_x * z_y + c_y;

				if (new_z_x * new_z_x + new_z_y * new_z_y >= 4.0)
					break;

				if (count >= 1024)
					break;

				z_x = new_z_x;
				z_y = new_z_y;
			}

			count_matrix[y * MANDEL_WIDTH + x] = count;

			if (count == 0 || count >= 1024)
				index = 0;
			else
				index = (count << color_index_shift) % 255 + 1;

			pixel = image_col_vector[index];

			col_count_vector[index] = 1;

			XPutPixel (ximage, x, y, pixel);
	
		}	/* Final de uma coluna */

		if
		(	XPutImage
			(	display, mandel.win, gc, ximage,
				x, 0,					/* fonte */
				SHADOW + x, SHADOW,			/* destino */
				1, ximage->height			/* Tamanhos */
			)
		)
			error ("O servidor X rejeitou a imagem");
	}

	/*
	 *	Conta o número de cores
	 */
	for (count = 0, index = 0; index < 256; index++)
	{
		if (col_count_vector[index])
			count++;
	}

	draw_msg_text
	(	"[%G, %G, %G], %dx%d, %d cores",
		ip->i_CENTER_X, ip->i_CENTER_Y, ip->i_SIZE_X,
		MANDEL_WIDTH, MANDEL_HEIGHT, count
	);

}	/* end compute */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
draw_count_mandel_window (void)
{
	int		x, y;
	int		count, index;
	ulong		pixel;
	IMAGE_PARAM	*ip = image_param_ptr;

	/*
	 *	Prólogo
	 */
	memsetl (col_count_vector, 0, sizeof (col_count_vector) / sizeof (long));

	/*
	 *	x
	 */
	for (x = 0; x < MANDEL_WIDTH; x++)
	{
		for (y = 0; y < MANDEL_HEIGHT; y++)
		{
			count = count_matrix[y * MANDEL_WIDTH + x];

			if (count == 0 || count >= 1024)
				index = 0;
			else
				index = (count << color_index_shift) % 255 + 1;

			pixel = image_col_vector[index];

			col_count_vector[index] = 1;

			XPutPixel (ximage, x, y, pixel);
		}
	}

	if (XPutImage (display, mandel.win, gc, ximage, 0, 0, SHADOW, SHADOW, ximage->width, ximage->height))
		error ("O servidor X rejeitou a imagem");

	/*
	 *	Conta o número de cores
	 */
	for (count = 0, index = 0; index < 256; index++)
	{
		if (col_count_vector[index])
			count++;
	}

	draw_msg_text
	(	"[%G, %G, %G], %dx%d, %d cores",
		ip->i_CENTER_X, ip->i_CENTER_Y, ip->i_SIZE_X,
		MANDEL_WIDTH, MANDEL_HEIGHT, count
	);

}	/* end draw_count_mandel_window */

/*
 ****************************************************************
 *	Mostra o mapa de cores					*
 ****************************************************************
 */
void
draw_color_map (int value)
{
	int		x, y;
	ulong		pixel;

	/*
	 *	x
	 */
	create_color_vector (0);

	prelude ();

	for (x = 0; x < MANDEL_WIDTH; x++)
	{
		pixel = image_col_vector[x * 256 / MANDEL_WIDTH];

		for (y = 0; y < 50; y++)
		{
			XPutPixel (ximage, x, y, pixel);
		}
	}

	if (XPutImage (display, mandel.win, gc, ximage, 0, 0, SHADOW, SHADOW, ximage->width, 50))
		error ("O servidor X rejeitou a imagem");

}	/* end draw_color_map */

/*
 ****************************************************************
 *	Desenha a janela da imagem, ...				*
 ****************************************************************
 */
void
draw_mandel_win (void)
{
	WINDATA		*wp = &mandel;

	/*
	 *	Envia para o servidor o conteúdo do Pixmap (a imagem).
	 */
	if (ximage != NULL)
	{
		if
		(	XPutImage
			(	display, mandel.win, gc, ximage,
				0, 0, SHADOW, SHADOW, ximage->width, ximage->height
			)
		)
			error ("O servidor X rejeitou a imagem");
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
 *	x			*
 ****************************************************************
 */
void
return_last_image (int value)
{
	if (image_param_ptr <= &image_param[0])
		return;

	image_param_ptr->i_SIZE_X = 0.0;

	image_param_ptr--;

	compute (0);

}	/* end return_last_image */

/*
 ****************************************************************
 *	Foi apertado um botão na janela do MANDELBROT		*
 ****************************************************************
 */
void
do_mandel_button_press (int button, int x, int y)
{
	/*
	 *	x
	 */
	if ((x -= SHADOW) < 0 || x >= MANDEL_WIDTH)
		return;

	if ((y -= SHADOW) < 0 || y >= MANDEL_HEIGHT)
		return;

	/*
	 *	Examina qual é o botão ...
	 */
	zoom_button_state = button;

	if (image_param_ptr >= &image_param[N_IMAGE_PARAM - 1])
		return;

	/*
	 *	Apaga o último retângulo, se for o caso ...
	 */
	if (rectangle_on)
	{
		XDrawRectangle
		(	display, mandel.win, rubber_band_gc,
			last_x + SHADOW, last_y + SHADOW, last_width, last_height
		);

		rectangle_on = 0;
	}

	if (button == Button3)
	{
		last_x = x - last_width;
		last_y = y;

		XDrawRectangle
		(	display, mandel.win, rubber_band_gc,
			last_x + SHADOW, last_y + SHADOW, last_width, last_height
		);

		rectangle_on = 1;

		return;
	}

	/*
	 *	Guarda o ponto inicial do retângulo
	 */
	zoom_x = x; zoom_y = y;

}	/* end do_mandel_button_press */

/*
 ****************************************************************
 *	Foi movido o cursor na janela do MANDELBROT		*
 ****************************************************************
 */
void
motion_mandel_window (int motion_x, int motion_y)
{
	Window		win = mandel.win;
	IMAGE_PARAM	*ip = image_param_ptr;
	int		delta_x, delta_y;
	double		point_x, point_y, SIZE_Y;

	/*
	 *	x
	 */
	if ((motion_x -= SHADOW) < 0 || motion_x >= MANDEL_WIDTH)
		return;

	if ((motion_y -= SHADOW) < 0 || motion_y >= MANDEL_HEIGHT)
		return;

	/*
	 *	Verifica se o botão está apertado
	 */
	if (zoom_button_state == 0)
	{
		const WINDATA	*wp = &msg;
		char		area[32];
		int		len;

		SIZE_Y	= ip->i_SIZE_X * (double)MANDEL_HEIGHT / (double)MANDEL_WIDTH;

		delta_x = motion_x - (MANDEL_WIDTH  >> 1);
		delta_y = motion_y - (MANDEL_HEIGHT >> 1);

		point_x = ip->i_CENTER_X + ((double)delta_x * ip->i_SIZE_X / (double)MANDEL_WIDTH);
		point_y = ip->i_CENTER_Y + ((double)-delta_y * SIZE_Y       / (double)MANDEL_HEIGHT);

		len = snprintf (area, sizeof (area), "%G, %G           ", point_x, point_y);

		XDrawImageString
		(	display, wp->win,
			wp->text_gc,
			MENU_BORDER + SHADOW, 
			MENU_BORDER + 2 * (wp->font_height + 2 * SHADOW) - (wp->font_descent + SHADOW),
			area, len
		);

#if (0)	/*******************************************************/
		draw_msg_text
		(	"[%G, %G, %G], %dx%d, (%G, %G)",
			ip->i_CENTER_X, ip->i_CENTER_Y, ip->i_SIZE_X,
			MANDEL_WIDTH, MANDEL_HEIGHT,
			point_x, point_y
		);
#endif	/*******************************************************/

		return;
	}

	/*
	 *	Apaga o último retângulo, se for o caso ...
	 */
	if (rectangle_on)
	{
		XDrawRectangle
		(	display, win, rubber_band_gc,
			last_x + SHADOW, last_y + SHADOW, last_width, last_height
		);

		rectangle_on = 0;
	}

	/*
	 *	x
	 */
	if (zoom_button_state == Button3)
	{
		last_x = motion_x - last_width;
		last_y = motion_y;

		XDrawRectangle
		(	display, win, rubber_band_gc,
			last_x + SHADOW, last_y + SHADOW, last_width, last_height
		);

		rectangle_on = 1;

		return;
	}

	/*
	 *	Calcula os parâmetros do retângulo
	 */
	if (motion_x > zoom_x)
		{ last_x = zoom_x;   last_width = motion_x - zoom_x; }
	else
		{ last_x = motion_x; last_width = zoom_x - motion_x; }

#if (0)	/*******************************************************/
	if (motion_y > zoom_y)
		{ last_y = zoom_y;   /*** last_height = motion_y - zoom_y; ***/ }
	else
#endif	/*******************************************************/
		{ last_y = motion_y; /*** last_height = zoom_y - motion_y; ***/ }

	last_height = last_width * MANDEL_HEIGHT / MANDEL_WIDTH;

	XDrawRectangle
	(	display, win, rubber_band_gc,
		last_x + SHADOW, last_y + SHADOW, last_width, last_height
	);

	rectangle_on = 1;

}	/* end motion_zoom_window */

/*
 ****************************************************************
 *	Saiu da janela da imagem				*
 ****************************************************************
 */
void
leave_mandel_window (void)
{
	const WINDATA	*wp = &msg;
	char		area[32];

	memset (area, ' ', sizeof (area));

	XDrawImageString
	(	display, wp->win,
		wp->text_gc,
		MENU_BORDER + SHADOW, 
		MENU_BORDER + 2 * (wp->font_height + 2 * SHADOW) - (wp->font_descent + SHADOW),
		area, sizeof (area)
	);

}	/* end leave_mandel_window */

/*
 ****************************************************************
 *	Foi desapertado um botão na janela do MANDELBROT	*
 ****************************************************************
 */
void
do_mandel_button_release (int button, int motion_x, int motion_y)
{
	int		center_x, center_y;
	int		size_x;
	double		orig_x, orig_y;
	double		SIZE_Y;
	IMAGE_PARAM	*ip = image_param_ptr;

	/*
	 *	Examina qual é o botão ...
	 */
#if (0)	/*******************************************************/
	if (button != Button1)
		return;
#endif	/*******************************************************/

	if (zoom_button_state == 0)
		return;

	zoom_button_state = 0;

	if (last_width <= 0)
		return;

	/*
	 *	Faz as contas, ...
	 */
	SIZE_Y	= ip->i_SIZE_X * (double)MANDEL_HEIGHT / (double)MANDEL_WIDTH;

	orig_x  = ip->i_CENTER_X - (ip->i_SIZE_X / 2.0);
	orig_y	= ip->i_CENTER_Y - (SIZE_Y / 2.0);

	center_x = last_x + last_width / 2;
	center_y = last_y + last_height / 2;

	size_x = last_width;

	ip[1].i_CENTER_X = orig_x + ((double)center_x 		        * ip->i_SIZE_X / (double)MANDEL_WIDTH);
	ip[1].i_CENTER_Y = orig_y + ((double)(MANDEL_HEIGHT - center_y) * SIZE_Y / (double)MANDEL_HEIGHT);

	ip[1].i_SIZE_X	 = (double)size_x * ip->i_SIZE_X / (double)MANDEL_WIDTH;

#if (0)	/*******************************************************/
printf ("X = %G, Y = %G, size = %G\n", ip[1].i_CENTER_X, ip[1].i_CENTER_Y , ip[1].i_SIZE_X);
#endif	/*******************************************************/

}	/* end do_mandel_button_release */
