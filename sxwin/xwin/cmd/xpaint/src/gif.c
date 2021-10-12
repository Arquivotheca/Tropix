/*
 ****************************************************************
 *								*
 *			gif.c					*
 *								*
 *	Leitura de imagens em formato GIF			*
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "../h/xpaint.h"

/*
 ****************************************************************
 *	Constantes e Macros					*
 ****************************************************************
 */
#define	elif			else if

#define	MAX_LWZ_BITS		12

#define	TRUE			1
#define	FALSE			0

#define INTERLACE		0x40
#define COLORMAP_PRESENT	0x80

/*
 ****************************************************************
 *	Definições de tipos					*
 ****************************************************************
 */
/*
 ****** Cabeçalho global ****************************************
 */
typedef struct
{
	char	g_magic[6];		/* Cadeia mágica (sem o '\0') */

	ushort	g_width,
		g_height;

	uchar	g_info,
		g_background,
		g_aspectratio;

}	GIFHEADER;

/*
 ****** Cabeçalho local *****************************************
 */
typedef struct
{
	ushort	l_left,
		l_top,
		l_width,
		l_height;

	uchar	l_info;

}	IMHEADER;

/*
 ****** Sei lá o que é isso  ************************************
 */
static struct
{
	int	transparent;
	int	delayTime;
	int	inputFlag;
	int	disposal;

}	Gif89 = { -1, -1, -1, 0 };

/*
 ******	Para a conversão para XPM *******************************
 */
#undef	CV_TO_XPM
#ifdef	CV_TO_XPM
entry int	xpm_width, xpm_height;

entry ulong	*xpm_map;
#endif	CV_TO_XPM

/*
 ****************************************************************
 *	Protótipos de Funções					*
 ****************************************************************
 */
int		read_color_table (FILE *, COLORMAP *);
int		do_extension (FILE *, int);
int		get_data_block (FILE *, uchar *);
int		get_code (FILE *, int, int);
int		get_LZW_byte (FILE *, int, int);
XImage		*read_GIF_image (FILE *, int, int, COLORMAP *, int, int,
				Display *, XStandardColormap *, IMAGE *);
void		cv_to_xpm (int xpos, int ypos, ulong pixel);
void		write_xpm_file (void);

/*
 ****************************************************************
 *	Confere se o arquivo é GIF				*
 ****************************************************************
 */
int
is_a_GIF_file (FILE *fp)
{
	GIFHEADER	header;

	if (fread (&header, 13, 1, fp) != 1)
		return (0);

	return
	(	strncmp (header.g_magic, "GIF87a", 6) == 0 ||
		strncmp (header.g_magic, "GIF89a", 6) == 0
	);

}	/* end is_a_GIF_file */

/*
 ****************************************************************
 *	Lê um arquivo GIF, gerando uma "XImage"			*
 ****************************************************************
 */
XImage *
load_GIF_file (FILE *fp, int imagem, Display *dpy,
		XStandardColormap *cmap, IMAGE *ip)
{
	GIFHEADER	header;
	XImage		*ximage = NULL;
	uchar		c;
	int		image_count = 0, image_number = 1;
	COLORMAP	global_cmap, local_cmap, *cmp;
	IMHEADER	lheader;

	if (fread (&header, 13, 1, fp) != 1)
		{ msg ("Erro na leitura do cabeçalho GIF de \"%s\"", ip->i_name); return (NULL); }

	if (imagem != 1)
		{ msg ("Só é possível mostrar a primeira imagem"); return (NULL); }

	/*
	 *	Obtém o número de cores utilizadas (máximo).
	 */
	global_cmap.c_size = 2 << (header.g_info & 0x07);

	if (global_cmap.c_size > CMAPSZ)
	{
		msg
		(	"Tamanho inválido para a tabela de cores global: %d (> %d)",
			global_cmap.c_size, CMAPSZ
		); 

		return (NULL);
	}

	/*
	 *	Lê a tabela de cores global, se houver.
	 */
	if (header.g_info & COLORMAP_PRESENT)
	{
		if (read_color_table (fp, &global_cmap))
		{
			msg ("Erro na leitura da tabela de cores global");
			return (NULL);
		}
	}

	/*
	 *	Processa as imagens.
	 */
	do
	{
		c = getc (fp);

		if (c == ';')		/* Terminador do GIF */
		{
			if (image_count < image_number)
			{
				msg
				(	"\"%s\" contém apenas %d image%s",
					ip->i_name, image_count, image_count > 1 ? "ns" : "m"
				);

				break;
			}
		}

		if (c == '!')		/* Extensões */
		{
			c = getc (fp);
			do_extension (fp, c);
			continue;
		}

		if (c != ',')		/* Inválido */
			continue;

		/*
		 *	Lê o cabeçalho da próxima imagem.
		 */
		++image_count;

		if (fread (&lheader, 9, 1, fp) != 1)
		{
			msg ("Erro na leitura do cabeçalho da imagem %d", image_count);
			break;
		}

		if (lheader.l_info & COLORMAP_PRESENT)
		{
			/* A imagem tem sua própria tabela de cores */

			local_cmap.c_size = 1 << ((lheader.l_info & 0x07) + 1);

			if (local_cmap.c_size > CMAPSZ)
			{
				msg
				(	"Tamanho inválido para a tabela de cores "
					"local: %d (> %d)",
					local_cmap.c_size, CMAPSZ
				); 
		
				break;
			}

			if (read_color_table (fp, &local_cmap))
			{
				msg ("Erro na leitura da tabela de cores local");
				break;
			}

			cmp = &local_cmap;
		}
		else
		{
			cmp = &global_cmap;
		}

		ximage =	read_GIF_image
				(	fp,
					lheader.l_width, lheader.l_height,
					cmp,
					lheader.l_info & INTERLACE,
					image_count != image_number,
					dpy, cmap,
					ip
				);
	}
	while (ximage == NULL);

	return (ximage);

}	/* end load_GIF_file */

/*
 ****************************************************************
 *	Lê uma Tabela de Cores (Global ou Local)		*
 ****************************************************************
 */
int
read_color_table (FILE *fp, COLORMAP *clp)
{
	int		index, nbytes;
	uchar		*rgb;

	rgb = alloca (nbytes = 3 * clp->c_size);

	if (fread (rgb, clp->c_size, 3, fp) != 3)
		{ msg ("Erro na leitura da tabela de cores"); return (1); }

	for (index = 0; index < clp->c_size; index++)
	{
		clp->c_red  [index] = *rgb++;
		clp->c_green[index] = *rgb++;
		clp->c_blue [index] = *rgb++;
		clp->c_pixel[index] = UNDEF;
	}

	clp->c_grayscale = 0;

	return (0);

}	/* end read_color_table */

/*
 ****************************************************************
 *	Processa as extensões					*
 ****************************************************************
 */
int
do_extension (FILE *fp, int label)
{
	char		*str, buf[CMAPSZ];

	switch (label)
	{
	    case 0x01:			/* Plain Text Extension */
		str = "Plain Text Extension";
		break;

	    case 0xFF:			/* Application Extension */
		str = "Application Extension";
		break;

	    case 0xFE:			/* Comment Extension */
		str = "Comment Extension";

		while (get_data_block (fp, (uchar *)buf) != 0)
			/* vazio */;

		return (FALSE);

	    case 0xF9:			/* Graphic Control Extension */
		str = "Graphic Control Extension";
		(void)get_data_block (fp, (uchar *)buf);

		Gif89.disposal  = (buf[0] >> 2) & 0x7;
		Gif89.inputFlag = (buf[0] >> 1) & 0x1;
		Gif89.delayTime = (buf[2] << 8) | buf[1]; /* LM_to_uint(buf[1], buf[2]); */

		if ((buf[0] & 0x1) != 0)
			Gif89.transparent = buf[3];

		while (get_data_block (fp, (uchar *)buf) != 0)
			/* vazio */;

		return (FALSE);

	    default:
		str = buf;
		sprintf (buf, "UNKNOWN (0x%02x)", label);
		break;
	}

	while (get_data_block (fp, (uchar *)buf) != 0)
		/* vazio */;

	return (FALSE);

}	/* end do_extension */

/*
 ****************************************************************
 *	Lê um bloco de dados, precedido de seu tamanho		*
 ****************************************************************
 */
static int ZeroDataBlock;

int
get_data_block (FILE *fp, uchar *buf)
{
	uchar	count;

	count = getc (fp);

	ZeroDataBlock = (count == 0);	/* Cuidado */

	if (count && fread (buf, count, 1, fp) != 1)
		return (-1);

	return (count);

}	/* end get_data_block */

/*
 ****************************************************************
 *	Ininteligível						*
 ****************************************************************
 */
int
get_code (FILE *fp, int code_size, int flag)
{
	static int	curbit, lastbit, done, last_byte;
	static uchar	buf[280];
	int		i, j, ret;
	uchar		count;

	if (flag)
	{
		/* Inicialização */

		return (done = curbit = lastbit = 0);
	}

	if ((curbit + code_size) >= lastbit)
	{
		if (done)
		{
			if (curbit >= lastbit)
				msg ("ran off the end of my bits");

			return (-1);
		}

		buf[0] = buf[last_byte - 2];
		buf[1] = buf[last_byte - 1];

		if ((count = get_data_block (fp, &buf[2])) == 0)
			done = TRUE;

		last_byte	= 2 + count;
		curbit		= (curbit - lastbit) + 16;
		lastbit		= (2 + count) << 3;
	}

	for (i = curbit, ret = j = 0; j < code_size; ++i, ++j)
		ret |= ((buf[i >> 3] & (1 << (i & 7))) != 0) << j;

	curbit += code_size;

	return (ret);

}	/* end get_code */

/*
 ****************************************************************
 *	Obtém o próximo byte da imagem sendo descomprimida	*
 ****************************************************************
 */
int
get_LZW_byte (FILE *fp, int flag, int input_code_size)
{
	static int	fresh = FALSE;
	static int	code_size, set_code_size;
	static int	max_code, max_code_size;
	static int	firstcode, oldcode;
	static int	clear_code, end_code;
	static int	*table[2], *stack, *sp;
	int		i, code, incode;

	if (flag)
	{
		/* Inicialização */

		if (table[0] == NULL)
		{
			table[0] = malloc ((1 << MAX_LWZ_BITS) * sizeof (int));
			table[1] = malloc ((1 << MAX_LWZ_BITS) * sizeof (int));
			stack    = malloc ((1 << MAX_LWZ_BITS) * sizeof (int) * 2);

			if (table[0] == NULL || table[1] == NULL || stack == NULL)
				msg ("$Memória insuficiente para o LZW");
		}

		set_code_size	= input_code_size;
		code_size	= set_code_size + 1;
		clear_code	= 1 << set_code_size;
		end_code	= clear_code + 1;
		max_code_size	= 2 * clear_code;
		max_code	= clear_code + 2;

		get_code (fp, 0, TRUE /* inicialização */);

		fresh = TRUE;

		for (i = 0; i < clear_code; ++i)
			{ table[0][i] = 0; table[1][i] = i; }

		for (/* vazio */; i < (1 << MAX_LWZ_BITS); ++i)
			table[0][i] = table[1][0] = 0;

		sp = stack;

		return (0);
	}

	if (fresh)
	{
		fresh = FALSE;

		do
		{
			firstcode = oldcode = get_code (fp, code_size, FALSE);

		}	while (firstcode == clear_code);

		return (firstcode);
	}

	if (sp > stack)
		return (*--sp);

	while ((code = get_code (fp, code_size, FALSE)) >= 0)
	{
		if (code == clear_code)
		{
			for (i = 0; i < clear_code; ++i)
			{
				table[0][i] = 0;
				table[1][i] = i;
			}

			for (/* vazio */; i < (1 << MAX_LWZ_BITS); ++i)
				table[0][i] = table[1][i] = 0;

			code_size	= set_code_size + 1;
			max_code_size	= 2 * clear_code;
			max_code	= clear_code + 2;
			sp		= stack;
			firstcode	= oldcode = get_code (fp, code_size, FALSE);

			return (firstcode);
		}

		if (code == end_code)
		{
			int	count;
			uchar	buf[260];

			if (ZeroDataBlock)
				return (-2);

			while ((count = get_data_block (fp, buf)) > 0)
				/* vazio */;

#if (0)	/****************************************************/
			if (count != 0)
			{
		/*
		 * pm_message("missing EOD in data stream (common occurence)");
		 */
			}
#endif	/****************************************************/

			return (-2);
		}

		incode = code;

		if (code >= max_code)
		{
			*sp++ = firstcode;
			code = oldcode;
		}

		while (code >= clear_code)
		{
			*sp++ = table[1][code];

			if (code == table[0][code])
				msg ("circular table entry BIG ERROR");

			code = table[0][code];
		}

		*sp++ = firstcode = table[1][code];

		if ((code = max_code) < (1 << MAX_LWZ_BITS))
		{
			table[0][code] = oldcode;
			table[1][code] = firstcode;

			++max_code;

			if
			(	(max_code >= max_code_size) &&
				(max_code_size < (1 << MAX_LWZ_BITS))
			)
			{
				max_code_size *= 2;
				++code_size;
			}
		}

		oldcode = incode;

		if (sp > stack)
			return (*--sp);
	}

	return (code);

}	/* end get_LZW_byte */

/*
 ****************************************************************
 *	Lê uma imagem, guardando na memória			*
 ****************************************************************
 */
XImage *
read_GIF_image (FILE *fp, int real_width, int real_height, COLORMAP *clp,
		int interlace, int ignore, Display *dpy,
		XStandardColormap *cmap, IMAGE *ip)
{
	XImage		*ximage;
	char		c;
	int		xpos, ypos;
	int		width, height;

	int		used_colors_count = 0;
	XColor		xcol;
	int		rgb_index;

	ulong		pass;
	static int	yinc[] = { 8, 8, 4, 2 };

#ifdef	CV_TO_XPM
	/*
	 *	Aloca memória para a conversão para XPM
	 */
	if (xpm_map == NULL && (xpm_map = calloc (real_width * real_width, sizeof (ulong))) == NULL)
		{ msg ("Memória esgotada"); return (NULL); }

	xpm_width = real_width; xpm_height = real_height;
#endif	CV_TO_XPM

	/*
	 *	Inicializa o descompressor.
	 */
	c = getc (fp);

	if (get_LZW_byte (fp, TRUE, c) < 0)
		{ msg ("Erro na leitura da imagem"); return (NULL); }

	/*
	 *	Verifica se a imagem deve simplesmente ser ignorada.
	 */
	if (ignore)
	{
		while (get_LZW_byte (fp, FALSE, c) >= 0)
			/* vazio */;

		return (NULL);
	}

	/*
	 *	Aloca a imagem de trabalho.
	 */
	ximage = alloc_image (dpy, ip, real_width, real_height, &width, &height);

        if (ximage == NULL)
                { msg ("Erro na criação da imagem"); return (NULL); }

	xpos = ypos = pass = 0;

	/*
	 *	Preenche a imagem.
	 */
	while ((rgb_index = get_LZW_byte (fp, FALSE, c)) >= 0)
	{
		rgb_index &= (CMAPSZ - 1);

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

		}	/* end if (pixel indefinido) */

		/* Acrescenta o pixel à imagem sendo preenchida */

		used_colors_count += put_pixel (ip, ximage, xpos, ypos, xcol.pixel);

		/*
		 *	x
		 */
#ifdef	CV_TO_XPM
		cv_to_xpm (xpos, ypos, xcol.pixel);
#if (0)	/*******************************************************/
		fprintf (stderr, "(%d %d): %P\n", xpos, ypos, xcol.pixel);
#endif	/*******************************************************/
#endif	CV_TO_XPM

		/* Incrementa as coordenadas */

		if (++xpos == real_width)
		{
			xpos = 0;

			if (interlace)
			{
				ypos += yinc[pass];

				if (ypos >= real_height)
				{
					if (++pass > 3)
						break;

					ypos = yinc[pass] >> 1;
				}
			}
			else
			{
				++ypos;
			}
		}

		if (ypos >= real_height)
			break;

	} 	/* end while (descomprimindo o arquivo) */

	/*
	 *	Epílogo.
	 */
	if (ip != NULL)
	{
		ip->i_width	= width;
		ip->i_height	= height;
		ip->i_ncolors	= used_colors_count;
	}

#ifdef	CV_TO_XPM
	write_xpm_file ();
#endif	CV_TO_XPM

	return (ximage);

}	/* end read_GIF_image */

#ifdef	CV_TO_XPM
/*
 ****************************************************************
 *	Converte para o formato XPM				*
 ****************************************************************
 */
void
cv_to_xpm (int xpos, int ypos, ulong pixel)
{
	xpm_map[ypos * xpm_width + xpos] = pixel;

}	/* end cv_to_xpm */

/*
 ****************************************************************
 *	Escreve o arquivo XPM					*
 ****************************************************************
 */
void
write_xpm_file (void)
{
	int		ncolors = 0, i, x, y;
	ulong		*color_list, color;
	const ulong	*up, *end_up;
	FILE		*fp;

	/*
	 *	Conta o número de cores
	 */
	color_list = alloca (256 * sizeof (ulong));

	memsetl (color_list, 0, 256);

	up = xpm_map; end_up = up + xpm_width * xpm_height;

	for (/* acima */; up < end_up; up++)
	{
		color = *up;

		for (i = 0; /* abaixo */; i++)
		{
			if (i >= ncolors)
				{ color_list[i] = color; ncolors++; break; }

			if (color_list[i] == color)
				break;
		}
	}

fprintf (stderr, "Cores = %d\n", ncolors);

	/*
	 *	Gera o arquivo
	 */
	if ((fp = fopen ("x.xpm", "w")) == NOFILE)
		{ msg ("NÃO consegui abrir o arquivo \"%s\"", "x.xpm"); return; }

	fprintf (fp, "/* XPM */\n");
	fprintf (fp, "static char	*unknown_xpm[] = {\n");
	fprintf (fp, "\"%d %d %d %d\",\n", xpm_width, xpm_height, ncolors, 1);

	for (i = 0; i < ncolors; i++)
	{
		color = color_list[i];

		if (color == 0)
			fprintf (fp, "\".	c Black\",\n");
		else
			fprintf (fp, "\"%c	c #%06X\",\n", 'A' + i, color);
	}

	up = xpm_map;

	for (y = 0; y < xpm_height; y++)
	{
		fputc ('\"', fp);
	
		for (x = 0; x < xpm_width; x++)
		{
			color = *up++;

			if (color == 0)
			{
				fputc ('.', fp);
			}
			else
			{
				for (i = 0; i < ncolors; i++)
				{
					if (color == color_list[i])
						{ fputc ('A' + i, fp); break; }
				}
			}
		}

		if (y < xpm_height - 1)
			fprintf (fp, "\",\n");
		else
			fprintf (fp, "\"};\n");
	}

	fclose (fp);

}	/* end cv_to_xpm */
#endif	CV_TO_XPM
