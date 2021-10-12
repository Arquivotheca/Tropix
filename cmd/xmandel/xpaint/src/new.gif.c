/*
 ****************************************************************
 *								*
 *			gif.c					*
 *								*
 *	Leitura de imagens em formato GIF			*
 *								*
 *	Versão	4.5.0, de 10.01.04				*
 *		4.5.0, de 10.01.04				*
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

#if (0)	/*******************************************************/
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
#endif	/*******************************************************/

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
 ******	Logical Screen Descriptor *******************************
 */
entry int	logical_screen_width;
entry int	logical_screen_height;

entry int	global_color_table_flag;
entry int	color_resolution;
entry int	sort_flag;
entry int	size_of_global_color_table_log;
entry int	size_of_global_color_table;

entry int	background_color_index;
entry int	pixel_aspect_ratio;

/*
 ******	Image Descriptor ****************************************
 */
entry int	image_left_position;
entry int	image_top_position;
entry int	image_width;
entry int	image_height;

entry int	local_color_table_flag;
entry int	interlace_flag;
entry int	sort_flag;
entry int	size_of_local_color_table_log;
entry int	size_of_local_color_table;

/*
 ******	O mapa de cores *****************************************
 */
typedef struct
{
	ulong	c_pixel[256];

	int	c_size;

}	GIFCMAP;

entry  GIFCMAP	*global_color_map;

entry  GIFCMAP	*local_color_map;

entry XImage	*ximage;

entry IMAGE	*global_ip;

int		xpos, ypos;

/*
 ******	Parâmetros do LZW ***************************************
 */
#define BIT_SZ_TO_CODE(sz) ((1 << sz) - 1) /* Calcula o código máximo */

enum
{
	MIN_BIT_SZ =	9,		/* O algoritmo começa com 9 bits */ 
	MAX_BIT_SZ =	12		/* LZW para GIF somente até 12 bits */
};

enum
{
	CLEAR_CODE =	256,		/* Reinicializa a compressão */
	EOF_CODE   =	257,		/* Final da Imagem */
	FIRST_CODE =	258		/* Código inicial */
};

entry int	LZW_minimum_code_size;

entry int	MAX_CODE;		/* Nunca pode usar este código */

entry int	CODE_SOURCE;		/* Fonte de novos códigos */

entry int	bit_sz;			/* Tamanho atual */

entry int	max_code;		/* Último código do tamanho atual */

#if (0)	/*******************************************************/
entry int	TEST_POINT;		/* Próximo teste de performance */

entry int	clear_flg;

entry long	in_cnt, out_cnt;	/* Total de caracteres de entrada/saída */

entry char	lzw_area[256 + 2];

entry int	bit_count;

entry int	bit_offset;
#endif	/*******************************************************/

entry int	used_colors_count;

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
GIFCMAP		*gif_read_color_table (FILE *fp, int size, const XStandardColormap *stdmap);
int		getW (FILE *fp);
void		unreduce (FILE *fp);
int		GETCODE (FILE *fp);
void		PUTCHAR (int value);

/*
 ****************************************************************
 *	Verifica se o arquivo é GIF				*
 ****************************************************************
 */
int
is_a_new_GIF_file (FILE *fp)
{
	char		area[6];

	if (fread (area, 6, 1, fp) != 1)
		return (0);

	if (strncmp (area, "GIF87a", 6) == 0)
		return (1);

	if (strncmp (area, "GIF89a", 6) == 0)
		return (1);

	return (0);

}	/* end is_a_new_GIF_file */

/*
 ****************************************************************
 *	Lê um arquivo GIF, gerando uma "XImage"			*
 ****************************************************************
 */
XImage *
load_new_GIF_file (FILE *fp, int imagem, Display *dpy, XStandardColormap *stdcmap, IMAGE *ip)
{
	int		value;
	int		width, height;

	/*
	 *	x
	 */
#ifdef	CODE_DEBUG
	code_fp = fopen ("/tmp/n.code", "w");
#endif	CODE_DEBUG

#ifdef	INDEX_DEBUG
	index_fp = fopen ("/tmp/n.index", "w");
#endif	INDEX_DEBUG

	/*
	 *	x
	 */
	used_colors_count = 0;
	global_ip = ip;

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

	/*
	 *	Lê a tabela de cores global
	 */
	if  (global_color_table_flag)
		global_color_map = gif_read_color_table (fp, size_of_global_color_table, stdcmap);

	/*
	 *	Lê a descrição uma imagem
	 */
	if (getc (fp) != ',')
		{ msg ("Espera a descrição de uma imagem"); return (NULL); }

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
	if ((ximage = alloc_image (dpy, ip, image_width, image_height, &width, &height)) == NULL)
                { msg ("Erro na criação da imagem"); return (NULL); }

	/*
	 *	Aqui começa o LZW
	 */
	LZW_minimum_code_size = getc (fp);

#ifdef	DEBUG
	printf ("LZW_minimum_code_size = %d\n", LZW_minimum_code_size);
#endif	DEBUG

	xpos = 0; ypos = 0;

	unreduce (fp);

	/*
	 *	Verifica se vem depois o bloco de tamanho 0
	 */
	if (getc (fp) == 0)
		/* vazio */;
#if (0)	/*******************************************************/
		msg ("Li o bloco de tamanho 0\n");
#endif	/*******************************************************/

	if (getc (fp) == '!' && getc (fp) == 0xFE)
	{
		char		buf[128];

		memset (buf, 0, sizeof (buf));

		if (get_data_block (fp, (uchar *)buf) != 0)
			ip->i_comment = strdup (buf);
	}

	/*
	 *	Epílogo
	 */
	if (ip != NULL)
	{
		ip->i_width	= image_width;
		ip->i_height	= image_height;
		ip->i_ncolors	= used_colors_count;
	}

	return (ximage);

}	/* end load_new_GIF_file */

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
		{ msg ("Memória esgotada"); return (NULL); }

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
unreduce (FILE *fp)
{
	int		K, new_omega_K, old_omega_K, push_omega_K;
	static CODETB	*codetb;
	CODETB		*hp;
	const CODETB	*end_codetb;
	char		*sp;
	int		CODESZ;
	char		STACK[7000];
char *origin;

	/*
	 *	Inicializa os parâmetros da expansão
	 */
	CODE_SOURCE = FIRST_CODE;

	MAX_CODE = BIT_SZ_TO_CODE (MAX_BIT_SZ);

	CODESZ = (1 << MAX_BIT_SZ); 

	bit_sz = MIN_BIT_SZ;

	max_code = BIT_SZ_TO_CODE (MIN_BIT_SZ);

#ifdef	CLEAR_DEBUG
	clear_count = 0;
#endif	CLEAR_DEBUG

#if (0)	/*******************************************************/
	bit_count = 0;
#endif	/*******************************************************/

	input_byte_count = 0;
	input_bit_count = 0;
	input_bit_area = 0;

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
#if (0)	/*******************************************************/
#endif	/*******************************************************/

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
{
msg ("KwKwK: new_omega_K = %d, CODE_SOURCE = %d", new_omega_K, CODE_SOURCE);
}

			if (new_omega_K > MAX_CODE)
				msg ("Cuidado: código inválido (%d, %d)", new_omega_K, MAX_CODE);

			*sp++ = K; push_omega_K = old_omega_K;

origin = "KwKwK";
		}
		else					/* Caso normal */
		{
			push_omega_K = new_omega_K;
origin = "OK";
		}

		/*
		 *	Decodifica e empilha os caracteres
		 */
		while (push_omega_K >= CLEAR_CODE) 
		{
			hp = &codetb[push_omega_K];

			if (hp->h_omega == 0xFFFF)
			{
				msg
				(	"Cuidado: entrada \"codetb\" vazia (%s), CODE_SOURCE = %d, "
					"push_omega_K = %d",
					origin, CODE_SOURCE, push_omega_K
				);
				push_omega_K = 0;
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

	while (input_bit_count <= bit_sz)
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

#if (0)	/*******************************************************/
/*
 ****************************************************************
 *	Obtém um código da entrada				*
 ****************************************************************
 */
int
GETCODE (FILE *fp)
{
	char		*bp;
	int		byte_offset, byte_count, bit_shift;
	int		needed_bits, code;

	/*
	 *	Verifica se a área tem bits suficientes
	 */
	if (bit_count < bit_sz)
	{
		if (bit_count > 0)
		{
			byte_offset = (bit_offset >> 3);

			lzw_area[0] = lzw_area[byte_offset]; lzw_area[1] = lzw_area[byte_offset + 1];

			bit_offset -= (byte_offset << 3);
			byte_offset = (bit_count + 7) >> 3;
		}
		else
		{
			bit_offset  = 0;
			byte_offset = 0;
		}

		if ((byte_count = getc (fp)) == 0)
			return (EOF_CODE);

		if (byte_count < 0)
			{ msg ("Li um tamanho de bloco de %d bytes", byte_count); return (EOF_CODE); }

		if (fread (lzw_area + byte_offset, byte_count, 1, fp) != 1)
			{ msg ("Erro na leitura do arquivo LZW (%d bytes)", byte_count); return (EOF_CODE); }

		bit_count += (byte_count << 3);
	}

	/*
	 *	Retira o código
	 */
	needed_bits = bit_sz;

	bp = lzw_area + (bit_offset >> 3); bit_shift = (bit_offset & 7);

	code = (*bp++ >> bit_shift); needed_bits -= (8 - bit_shift);

	if (needed_bits >= 8)
		{ code |= (*bp++ << (bit_sz - needed_bits)); needed_bits -= 8; }

	if (needed_bits > 0)
		{ code |= (((*bp << (8 - needed_bits)) & 0xFF) << (bit_sz - 8)); }

	bit_count  -= bit_sz;
	bit_offset += bit_sz;

#if (0)	/*******************************************************/
	if (code == EOF_CODE)
		msg ("Li o código EOF_CODE");
#endif	/*******************************************************/

#ifdef	CODE_DEBUG
	fprintf (code_fp, "sz = %d, code = %d\n", bit_sz, code);
#endif	CODE_DEBUG

	return (code);

}	/* end GETCODE */
#endif	/*******************************************************/

/*
 ****************************************************************
 *	Armazena os índices RGB					*
 ****************************************************************
 */
void
PUTCHAR (int index)
{
#ifdef	INDEX_DEBUG
		fprintf (index_fp, "index = %d\n", index);

		index_count++;
#endif	INDEX_DEBUG

	used_colors_count += put_pixel (global_ip, ximage, xpos, ypos, global_color_map->c_pixel[index]);

	if (++xpos >= image_width)
		{ xpos = 0; ypos++; }

}	/* end PUTCHAR */
