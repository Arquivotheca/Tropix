/*
 ****************************************************************
 *								*
 *			gif.c					*
 *								*
 *	Leitura de imagens em formato GIF			*
 *								*
 *	Versão	4.5.0, de 10.01.04				*
 *		4.6.0, de 08.04.04				*
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

#include "../h/xview.h"

/*
 ****************************************************************
 *	Constantes e Macros					*
 ****************************************************************
 */
#undef	CODE_DEBUG
#ifdef	CODE_DEBUG
entry FILE	*code_fp;
#endif	CODE_DEBUG

#undef	INDEX_DEBUG
#ifdef	INDEX_DEBUG
entry FILE	*index_fp;

entry int	index_count;
#endif	INDEX_DEBUG

#undef	CLEAR_DEBUG
#ifdef	CLEAR_DEBUG
entry int	clear_count;
#endif	CLEAR_DEBUG

/*
 ******	Image Descriptor ****************************************
 */
entry int	image_width;
entry int	image_height;
entry int	interlace_flag;

/*
 ******	O mapa de cores *****************************************
 */
typedef struct
{
	ulong	c_pixel[256];

}	GIFCMAP;

entry  GIFCMAP	*global_color_map;

#if (0)	/*******************************************************/
entry  GIFCMAP	*local_color_map;
#endif	/*******************************************************/

entry XImage	*ximage;

entry int	xpos, ypos;

entry int	gif_pass;

/*
 ******	Parâmetros do LZW ***************************************
 */
#define BIT_SZ_TO_CODE(sz) ((1 << sz) - 1) /* Calcula o código máximo */

enum { MAX_BIT_SZ = 12 };		/* LZW para GIF somente até 12 bits */

entry int	EOF_CODE;		/* Final da Imagem */

entry int	MAX_CODE;		/* Nunca pode usar este código */

entry int	CODE_SOURCE;		/* Fonte de novos códigos */

entry int	bit_sz;			/* Tamanho atual */

entry int	max_code;		/* Último código do tamanho atual */

entry int		input_byte_count;
entry int		input_bit_count;
entry unsigned int	input_bit_area;

/*
 ****** Estrutura da tabela CODE ********************************
 */
typedef struct
{
	ushort	h_omega;	/* Cadeia prefixo, que adicionada com h_K, */
	char	h_K;		/* gera o código de acesso a esta entrada */

}	CODETB;

#define NOCODETB (CODETB *)NULL

/*
 ******	Protótipos de funções ***********************************
 */
void		analyse_table_based_image (const char *file_nm, FILE *fp);
void		analyse_extension (FILE *fp);
GIFCMAP		*gif_read_color_table (FILE *fp, int size, const XStandardColormap *stdmap);
int		getW (FILE *fp);
void		unreduce (int, FILE *fp);
int		GETCODE (FILE *fp);
void		PUTCHAR (int value);

/*
 ****************************************************************
 *	Verifica se o arquivo é GIF				*
 ****************************************************************
 */
int
is_a_GIF_file (FILE *fp)
{
	char		area[6];

   /***	fseek (fp, 0, SEEK_SET); ***/

	if (fread (area, 6, 1, fp) != 1)
		return (0);

	if (strncmp (area, "GIF87a", 6) == 0)
		return (1);

	if (strncmp (area, "GIF89a", 6) == 0)
		return (1);

	return (0);

}	/* end is_a_GIF_file */

/*
 ****************************************************************
 *	Lê um arquivo GIF, gerando uma "XImage"			*
 ****************************************************************
 */
int
load_GIF_file (const char *file_nm, FILE *fp)
{
	int		value, c;
	/*
	 ******	Logical Screen Descriptor *******************************
	 */
	int	logical_screen_width;
	int	logical_screen_height;

	int	global_color_table_flag;
	int	color_resolution;
	int	sort_flag;
	int	size_of_global_color_table_log;
	int	size_of_global_color_table;

	int	background_color_index;
	int	pixel_aspect_ratio;

	/*
	 *	Abre os arquivos de DEBUG
	 */
#ifdef	CODE_DEBUG
	code_fp = fopen ("/tmp/n.code", "w");
#endif	CODE_DEBUG

#ifdef	INDEX_DEBUG
	index_fp = fopen ("/tmp/n.index", "w");
#endif	INDEX_DEBUG

	/*
	 *	Em primeiro lugar, examina o "Logical Screen Descriptor"
	 */
	fseek (fp, 6, SEEK_SET);

	logical_screen_width		= getW (fp);
	logical_screen_height		= getW (fp);

	value = getc (fp);

	global_color_table_flag		= (value  >> 7);
	color_resolution		= ((value >> 4) & 7) + 1;
	sort_flag 			= (value  >> 3) & 1; 
	size_of_global_color_table_log	= (value	& 7) + 1; 
	size_of_global_color_table	= (1 << size_of_global_color_table_log);

	background_color_index		= getc (fp);
	pixel_aspect_ratio		= getc (fp);

#undef	DEBUG
#ifdef	DEBUG
	printf
	(	"logical_screen_width = %d, logical_screen_height = %d\n"
		"global_color_table_flag = %d, color_resolution = %d\n"
		"sort_flag = %d, size_of_global_color_table = %d\n"
		"background_color_index = %d,  pixel_aspect_ratio = %d\n",
		logical_screen_width, logical_screen_height,
		global_color_table_flag, color_resolution, sort_flag, size_of_global_color_table,
		background_color_index,  pixel_aspect_ratio
	);
#endif	DEBUG

#ifdef	INDEX_DEBUG
	fprintf (index_fp, "size_of_global_color_table = %d\n\n", size_of_global_color_table);
#endif	INDEX_DEBUG

	/*
	 *	Lê a tabela de cores global
	 */
	if (global_color_map != NULL)
		free (global_color_map);

	if  (global_color_table_flag)
	{
		global_color_map = gif_read_color_table (fp, size_of_global_color_table, StandardColormap);
	}

	/*
	 *	Procura Imagens ou extensões
	 */
    again:
	switch (c = getc (fp))
	{
	    case ',':
		analyse_table_based_image (file_nm, fp);
		goto again;

	    case '!':
		analyse_extension (fp);
		goto again;

	    case ';':		/* Trailer */
	    case EOF:		/* Final do arquivo */
		return (0);

	    default:
		goto again;

	}	/* end switch */

}	/* end load_GIF_file */

/*
 ****************************************************************
 *	Analisa o bloco da imagem				*
 ****************************************************************
 */
void
analyse_table_based_image (const char *file_nm, FILE *fp)
{
	int		c, value;
	int		LZW_minimum_code_size;

	/*
	 ******	Image Descriptor ****************************************
	 */
	int	image_left_position;
	int	image_top_position;
	int	local_color_table_flag;
	int	sort_flag;
	int	size_of_local_color_table_log;
	int	size_of_local_color_table;

	/*
	 *	Lê o cabeçalho
	 */
	image_left_position		= getW (fp);
	image_top_position		= getW (fp);
	image_width			= getW (fp);
	image_height			= getW (fp);

	value = getc (fp);

	local_color_table_flag		= (value  >> 7);
	interlace_flag			= ((value >> 6) & 1);
	sort_flag			= ((value >> 5) & 1);
	size_of_local_color_table_log	= ( value	& 7) + 1; 
	size_of_local_color_table	= (1 << size_of_local_color_table_log);


#ifdef	DEBUG
	printf
	(	"image_left_position = %d, image_top_position = %d\n"
		"image_width = %d, image_height = %d\n"
		"local_color_table_flag = %d, interlace_flag = %d, sort_flag = %d\n"
		"size_of_local_color_table = %d\n",
		image_left_position, image_top_position,
		image_width, image_height,
		local_color_table_flag, interlace_flag, sort_flag,
		size_of_local_color_table
	);
#endif	DEBUG

	/*
	 *	Aloca a imagem de trabalho
	 */
	if (ximage != NULL)
		XDestroyImage (ximage);

	if ((ximage = alloc_image (image_width, image_height)) == NULL)
                { draw_error_msg_text ("Erro na criação da imagem"); return; }

	/*
	 *	Aqui começa o LZW
	 */
	LZW_minimum_code_size = getc (fp);

	draw_msg_text ("Imagem \"%s\", %dx%d", file_nm, image_width, image_height);

#if (0)	/*******************************************************/
draw_msg_text ("Imagem \"%s\", %dx%d (%d bits)", file_nm, image_width, image_height, LZW_minimum_code_size);
#endif	/*******************************************************/

#ifdef	DEBUG
	printf ("LZW_minimum_code_size = %d\n", LZW_minimum_code_size);
#endif	DEBUG

	xpos = 0; ypos = 0;

	unreduce (LZW_minimum_code_size, fp);

	if ((c = getc (fp)) != 0)
		ungetc (c, fp);

	/*
	 *	Ajusta o tamanho
	 */
	adjust_win_size (image_width, image_height);

	/*
	 *	Desenha a janela
	 */
	draw_mandel_win ((XExposeEvent *)NULL);

	if (local_color_table_flag)
		draw_error_msg_text ("Contém tabela de cores local");

	/*
	 *	Verifica a memória
	 */
	if (chkalloc () < 0)
		draw_error_msg_text ("Inconsistência na lista da memória alocada");

}	/* end analyse_table_based_image */

/*
 ****************************************************************
 *	Analisa o bloco de extensão				*
 ****************************************************************
 */
void
analyse_extension (FILE *fp)
{
	int		c;
	char		area[256];

	/*
	 *	x
	 */
	switch (c = getc (fp))
	{
	    case 0x01:		/* Plain Text Extension */
#undef	EXT_DEBUG
#ifdef	EXT_DEBUG
		printf ("Plain Text Extension\n");
#endif	EXT_DEBUG
		break;

	    case 0xF9:		/* Grafic Control Extension */
#ifdef	EXT_DEBUG
		printf ("Grafic Control Extension\n");
#endif	EXT_DEBUG
		break;

	    case 0xFE:		/* Comment Extension */
#ifdef	EXT_DEBUG
		printf ("Comment Extension\n");
#endif	EXT_DEBUG
		memset (area, 0, sizeof (area));

		c = getc (fp);

#ifdef	EXT_DEBUG
		printf ("Bloco de comentários de %d bytes\n", c);
#endif	EXT_DEBUG

		if (fread (area, 1, c, fp) == c)
		{
			draw_com_text (area);
		}
		break;

	    case 0xFF:		/* Application Extension */
#ifdef	EXT_DEBUG
		printf ("Application Extension\n");
#endif	EXT_DEBUG
		break;

	    default:
		draw_error_msg_text ("Extensão desconhecida (%02X)", c);
		break;

	}	/* end switch */

	/*
	 *	Pula os blocos
	 */
	for (EVER)
	{
		c = getc (fp);		/* Lê o tamanho */

		if (c == 0)
			return;

		if (c < 0)
			{ draw_error_msg_text ("Final inesperado do arquivo"); break; }

		for (/* acima */; c > 0; c--)
			getc (fp);
	}

}	/* end analyse_extension */

/*
 ****************************************************************
 *	Lê um mapa de cores					*
 ****************************************************************
 */
GIFCMAP *
gif_read_color_table (FILE *fp, int size, const XStandardColormap *stdcmap)
{
	GIFCMAP		*map;
	ulong		*pp;
	const ulong	*end_pp;
	int		red, green, blue;
	XColor		xcol;

	/*
	 *	Aloca a área e converte
	 */
	if ((map = malloc (size * sizeof (GIFCMAP))) == NULL)
		{ draw_error_msg_text ("Memória esgotada"); return (NULL); }

	for (pp = map->c_pixel, end_pp = pp + size; pp < end_pp; pp++)
	{
		red = getc (fp); green = getc (fp); blue = getc (fp);

		if   (depth == 24)
		{
			*pp = (red << 16) | (green << 8) | blue;
		}
		elif (stdcmap != NULL)
		{
			*pp = stdcmap->base_pixel +
				(((red   * stdcmap->red_max)   >> 8) * stdcmap->red_mult) +
				(((green * stdcmap->green_max) >> 8) * stdcmap->green_mult) +
				(((blue  * stdcmap->blue_max)  >> 8) * stdcmap->blue_mult);
		}
		else
		{
			xcol.red   = red   << 8;
			xcol.green = green << 8;
			xcol.blue  = blue  << 8;

			xcol.flags = DoRed | DoGreen | DoBlue;

			XAllocColor (display, DefaultColormap (display, DefaultScreen (display)), &xcol);

			*pp = xcol.pixel;
		}
	}

#ifdef	DEBUG
	pp = map->c_pixel;
	
	printf ("pp[0] = %P, pp[1] = %P, pp[%d] = %P\n", pp[0], pp[1], size - 1, pp[size - 1]);
#endif	DEBUG

	return (map);

}	/* end gif_read_color_table */

/*
 ****************************************************************
 *	Lê dois bytes 						*
 ****************************************************************
 */
int
getW (FILE *fp)
{
	int		value;

	value = getc (fp); value |= (getc (fp) << 8);

	return (value);

}	/* end getW */

/*
 ****************************************************************
 *	Decodificação do LZW					*
 ****************************************************************
 */
void
unreduce (int LZW_minimum_code_size, FILE *fp)
{
	int		K, new_omega_K, old_omega_K, push_omega_K;
	static CODETB	*codetb;
	CODETB		*hp;
	const CODETB	*end_codetb;
	char		*sp;
	int		CODESZ;
	int		MIN_BIT_SZ = LZW_minimum_code_size + 1;
	int		CLEAR_CODE = (1 << LZW_minimum_code_size);
	int		FIRST_CODE = CLEAR_CODE + 2;
	char		STACK[7000];

	/*
	 *	Inicializa os parâmetros da expansão
	 */
	EOF_CODE	= CLEAR_CODE + 1;			/* Final da Imagem */

	CODE_SOURCE = FIRST_CODE;

	MAX_CODE = BIT_SZ_TO_CODE (MAX_BIT_SZ);

	CODESZ = (1 << MAX_BIT_SZ); 

	bit_sz = MIN_BIT_SZ;

	max_code = BIT_SZ_TO_CODE (MIN_BIT_SZ);

#ifdef	CLEAR_DEBUG
	clear_count = 0;
#endif	CLEAR_DEBUG

	input_byte_count = 0;
	input_bit_count = 0;
	input_bit_area = 0;

	gif_pass = 0;

	/*
	 *	Aloca memória para a tabela CODE 
	 */
	if (codetb == NOCODETB && (codetb = malloc (CODESZ * sizeof (CODETB))) == NOCODETB)
	{
		fprintf
		(	stderr,
			"%s: Não consegui alocar memória para a tabela CODE\n",
			pgname
		);
		exit (1);
	}

	for (hp = codetb, end_codetb = hp + CODESZ; hp < end_codetb; hp++)
		hp->h_omega = 0xFFFF;

	/*
	 *	Realiza a expansão
	 */
	while ((K = old_omega_K = GETCODE (fp)) == CLEAR_CODE)
		/* vazio */;

	if (old_omega_K == EOF_CODE)
		return;

	PUTCHAR (K);

	while ((new_omega_K = GETCODE (fp)) != EOF_CODE)
	{
		/*
		 *	Verifica se é o código de controle
		 */
		if (new_omega_K == CLEAR_CODE)
		{
			bit_sz = MIN_BIT_SZ; max_code = BIT_SZ_TO_CODE (MIN_BIT_SZ);
#undef	DEBUG
#ifdef	DEBUG
			printf ("CLEAR, Modificando o código para %d bits\n", bit_sz);
#endif	DEBUG

#ifdef	CLEAR_DEBUG
			printf ("Recebi um CLEAR: %d\n", clear_count);
			clear_count = 0;
#endif	CLEAR_DEBUG
			CODE_SOURCE = FIRST_CODE;

			for (hp = codetb; hp < end_codetb; hp++)
				hp->h_omega = 0xFFFF;

			while ((K = old_omega_K = GETCODE (fp)) == CLEAR_CODE)
				/* vazio */;

			if (old_omega_K == EOF_CODE)
				return;

			PUTCHAR (K);

			continue;
		}
#ifdef	CLEAR_DEBUG
		else
		{
			clear_count++;
		}
#endif	CLEAR_DEBUG


		/*
		 *	Caso especial (KwKwK)
		 */
		sp = STACK;

		if (new_omega_K >= CODE_SOURCE)		/* Caso especial (KwKwK) */
		{
			if (new_omega_K > CODE_SOURCE)		/* Caso especial (KwKwK) */
				draw_error_msg_text ("Caso KwKwK: %d :: %d", new_omega_K, CODE_SOURCE);

			*sp++ = K; push_omega_K = old_omega_K;
		}
		else					/* Caso normal */
		{
			push_omega_K = new_omega_K;
		}

		/*
		 *	Decodifica e empilha os caracteres
		 */
		while (push_omega_K >= CLEAR_CODE) 
		{
			hp = &codetb[push_omega_K];

			if (hp < codetb || hp >= end_codetb)
			{
				draw_error_msg_text
				(	"Acessando além do final de CODETB, push_omega_K = %d",
					push_omega_K
				);
			}

			if (hp->h_omega == 0xFFFF)
			{
				draw_error_msg_text ("Entrada da tabela de cadeias vazia");
				push_omega_K = 0;
				break;
			}

			if (sp >= &STACK[sizeof (STACK) - 1])
			{
				draw_error_msg_text ("Acessando além do final da PILHA");
				break;
			}

			*sp++ = hp->h_K; push_omega_K = hp->h_omega;
		}

		*sp++ = K = push_omega_K;

		/*
		 *	Emite os caracteres da pilha
		 */
		while (sp > STACK)
			PUTCHAR (*--sp);

		/*
		 *	Insere a cadeia omega_K nova na tabela
		 */
		if (CODE_SOURCE <= MAX_CODE)
		{
			hp = &codetb[CODE_SOURCE++];

			hp->h_K = K; hp->h_omega = old_omega_K;

			if (CODE_SOURCE > max_code  &&  bit_sz < MAX_BIT_SZ)
			{
				bit_sz++; max_code = BIT_SZ_TO_CODE (bit_sz);
#ifdef	DEBUG
				printf ("Modificando o código para %d bits\n", bit_sz);
#endif	DEBUG
			}
		}

		old_omega_K = new_omega_K;

	}	/* end loop GETCODE */

#ifdef	INDEX_DEBUG
	fprintf (index_fp, "\n\ncount = %d\n", index_count); fclose (index_fp);
#endif	INDEX_DEBUG

}	/* end unreduce */

/*
 ****************************************************************
 *	Obtém um código da entrada				*
 ****************************************************************
 */
int
GETCODE (FILE *fp)
{
	int			code;
	static const int	code_mask[] = { 0, 1, 3, 7, 15, 31, 63, 127, 255, 511, 1023, 2047, 4095 };

	/*
	 *	Enche a área
	 */
	while (input_bit_count < bit_sz)
	{
		if (input_byte_count <= 0)
		{
			if ((input_byte_count = getc (fp)) <= 0)
				return (EOF_CODE);
		}

		input_bit_area |= getc (fp) << input_bit_count;

		input_byte_count -= 1;
		input_bit_count  += 8;
	}

	code = input_bit_area & code_mask[bit_sz];

	input_bit_area  >>= bit_sz;
	input_bit_count  -= bit_sz;

#ifdef	CODE_DEBUG
	fprintf (code_fp, "sz = %d, code = %d\n", bit_sz, code);
#endif	CODE_DEBUG

	return (code);

}	/* end GETCODE */

/*
 ****************************************************************
 *	Armazena os índices RGB					*
 ****************************************************************
 */
void
PUTCHAR (int index)
{
	static int	gif_yinc[] = { 8, 8, 4, 2 };

#ifdef	INDEX_DEBUG
	static int	index_in_this_line;

	fprintf (index_fp, " %3d,", index);

	if (++index_in_this_line >= 16)
		{ fprintf (index_fp, "\n"); index_in_this_line = 0; }

	index_count++;

	if (xpos >= image_width - 1)
		fprintf (index_fp, "\n");
#endif	INDEX_DEBUG

	/*
	 *	Às vezes, o arquivo tem píxeis demais
	 */
	if (ypos >= image_height)
		return;

	/*
	 *	Armazena o pixel
	 */
	XPutPixel (ximage, xpos, ypos, global_color_map->c_pixel[index]);

	/*
	 *	Posiciona o novo píxel
	 */
	if (++xpos >= image_width)
	{
		xpos = 0;

		if (interlace_flag)
		{
			ypos += gif_yinc[gif_pass];

			if (ypos >= image_height)
			{
				if (++gif_pass > 3)
					return;

				ypos = gif_yinc[gif_pass] >> 1;
			}
		}
		else
		{
			ypos++;
		}
	}

}	/* end PUTCHAR */
