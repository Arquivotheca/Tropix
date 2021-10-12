/*
 ****************************************************************
 *								*
 *			screen_saver.c				*
 *								*
 *	Pequeno protetor de video do PC				*
 *								*
 *	Versão	3.0.0, de 06.09.96				*
 *		4.9.0, de 13.08.06				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2006 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include "../h/common.h"
#include "../h/sync.h"
#include "../h/scb.h"

#include "../h/tty.h"
#include "../h/video.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Variáveis e Definições Globais				*
 ****************************************************************
 */
#define	PAT_LETTER	6
#define	PAT_LINE	8
#define	PAT_SPACE	2
#define	PAT_COL		(8 + PAT_SPACE)
#define	PAT_COLOR	7		/* Cor inicial: Branca */
#define C		PAT_COLOR

entry int	screen_saver_line,	/* Linha de início no momento */
		screen_saver_col;	/* Coluna de início no momento */
entry int	screen_saver_down,	/* Indica o sentido vertical */
		screen_saver_right;	/* Indica o sentido horizontal */

entry int	screen_saver_color = PAT_COLOR;	 /* Cor inicial: Branca */

/*
 ******	Tabela contendo o padrão "TROPIX" ***********************
 */
entry char	screen_saver_table[PAT_LINE][PAT_LETTER * PAT_COL] =
{
/* 0 */	'*', '*', '*', '*', 	'*', '*', '*', '*', 	' ', ' ',
 	'*', '*', '*', '*', 	'*', '*', ' ', ' ', 	' ', ' ',
	' ', '*', '*', '*', 	'*', '*', ' ', ' ', 	' ', ' ',
	'*', '*', '*', '*', 	'*', '*', ' ', ' ', 	' ', ' ',
	' ', ' ', '*', '*', 	'*', '*', ' ', ' ', 	' ', ' ',
	'*', '*', ' ', ' ', 	' ', '*', '*', ' ', 	' ', ' ',

/* 1 */	'*', '*', ' ', '*',	'*', ' ', '*', '*', 	' ', ' ',
	' ', '*', '*', ' ', 	' ', '*', '*', ' ', 	' ', ' ',
	'*', '*', ' ', ' ', 	' ', '*', '*', ' ', 	' ', ' ',
	' ', '*', '*', ' ', 	' ', '*', '*', ' ', 	' ', ' ',
	' ', ' ', ' ', '*', 	'*', ' ', ' ', ' ', 	' ', ' ',
	'*', '*', ' ', ' ', 	' ', '*', '*', ' ', 	' ', ' ',

/* 2 */	'*', ' ', ' ', '*', 	'*', ' ', ' ', '*', 	' ', ' ',
	' ', '*', '*', ' ', 	' ', '*', '*', ' ', 	' ', ' ',
	'*', '*', ' ', ' ', 	' ', '*', '*', ' ', 	' ', ' ',
	' ', '*', '*', ' ', 	' ', '*', '*', ' ', 	' ', ' ',
	' ', ' ', ' ', '*', 	'*', ' ', ' ', ' ', 	' ', ' ',
	' ', '*', '*', ' ', 	'*', '*', ' ', ' ', 	' ', ' ',

/* 4 */	' ', ' ', ' ', '*', 	'*', ' ', ' ', ' ', 	' ', ' ',
	' ', '*', '*', '*', 	'*', '*', ' ', ' ', 	' ', ' ',
	'*', '*', ' ', ' ', 	' ', '*', '*', ' ', 	' ', ' ',
	' ', '*', '*', '*', 	'*', '*', ' ', ' ', 	' ', ' ',
	' ', ' ', ' ', '*', 	'*', ' ', ' ', ' ', 	' ', ' ',
	' ', ' ', '*', '*', 	'*', ' ', ' ', ' ', 	' ', ' ',

/* 5 */	' ', ' ', ' ', '*', 	'*', ' ', ' ', ' ', 	' ', ' ',
	' ', '*', '*', ' ', 	'*', '*', ' ', ' ', 	' ', ' ',
	'*', '*', ' ', ' ', 	' ', '*', '*', ' ', 	' ', ' ',
	' ', '*', '*', ' ', 	' ', ' ', ' ', ' ', 	' ', ' ',
	' ', ' ', ' ', '*', 	'*', ' ', ' ', ' ', 	' ', ' ',
	' ', ' ', '*', '*', 	'*', ' ', ' ', ' ', 	' ', ' ',

/* 7 */	' ', ' ', ' ', '*', 	'*', ' ', ' ', ' ', 	' ', ' ',
	' ', '*', '*', ' ', 	' ', '*', '*', ' ', 	' ', ' ',
	'*', '*', ' ', ' ', 	' ', '*', '*', ' ', 	' ', ' ',
	' ', '*', '*', ' ', 	' ', ' ', ' ', ' ', 	' ', ' ',
	' ', ' ', ' ', '*', 	'*', ' ', ' ', ' ', 	' ', ' ',
	' ', '*', '*', ' ', 	'*', '*', ' ', ' ', 	' ', ' ',

/* 8 */	' ', ' ', ' ', '*', 	'*', ' ', ' ', ' ', 	' ', ' ',
	' ', '*', '*', ' ', 	' ', '*', '*', ' ', 	' ', ' ',
	'*', '*', ' ', ' ', 	' ', '*', '*', ' ', 	' ', ' ',
	' ', '*', '*', ' ', 	' ', ' ', ' ', ' ', 	' ', ' ',
	' ', ' ', ' ', '*', 	'*', ' ', ' ', ' ', 	' ', ' ',
	'*', '*', ' ', ' ', 	' ', '*', '*', ' ', 	' ', ' ',

/* 9 */	' ', ' ', '*', '*', 	'*', '*', ' ', ' ', 	' ', ' ',
	'*', '*', '*', ' ', 	' ', '*', '*', ' ', 	' ', ' ',
	' ', '*', '*', '*', 	'*', '*', ' ', ' ', 	' ', ' ',
	'*', '*', '*', '*', 	' ', ' ', ' ', ' ', 	' ', ' ',
	' ', ' ', '*', '*', 	'*', '*', ' ', ' ', 	' ', ' ',
	'*', '*', ' ', ' ', 	' ', '*', '*', ' ', 	' ', ' '
};

/*
 ******	Tabela o desenho TROPIX.GIF *****************************
 */
enum { COLOR_RED = 4, COLOR_BLUE = 9, COLOR_MAGENTA = 5, COLOR_GREEN = 2, COLOR_YELLOW = 14 };

typedef unsigned int	bit;

typedef struct
{
	bit	line  : 8;	/* 0 a 199 */
	bit	col   : 10;	/* 0 a 399 */
	bit	count : 10;	/* 0 a 399 */
	bit	color : 4;	/* 0 a  15 */

}	IMAGE_LINE;

const IMAGE_LINE	screen_saver_image[] =
{
	  7,	295,	  4,	COLOR_RED,	  8,	294,	  6,	COLOR_RED,
	  9,	293,	  8,	COLOR_RED,	 10,	293,	  8,	COLOR_RED,
	 11,	293,	  8,	COLOR_RED,	 11,	389,	  4,	COLOR_RED,
	 12,	293,	  8,	COLOR_RED,	 12,	388,	  6,	COLOR_RED,
	 13,	293,	  8,	COLOR_RED,	 13,	387,	  8,	COLOR_RED,
	 14,	293,	  8,	COLOR_RED,	 14,	387,	  8,	COLOR_RED,
	 15,	293,	  8,	COLOR_RED,	 15,	387,	  8,	COLOR_RED,
	 16,	  7,	 20,	COLOR_RED,	 16,	 97,	 12,	COLOR_RED,
	 16,	293,	  8,	COLOR_RED,	 16,	387,	  8,	COLOR_RED,
	 17,	  6,	 28,	COLOR_RED,	 17,	 84,	 26,	COLOR_RED,
	 17,	293,	  8,	COLOR_RED,	 17,	387,	  8,	COLOR_RED,
	 18,	  5,	 33,	COLOR_RED,	 18,	 49,	 62,	COLOR_RED,
	 18,	293,	  8,	COLOR_RED,	 18,	387,	  8,	COLOR_RED,
	 19,	  4,	107,	COLOR_RED,	 19,	293,	  9,	COLOR_RED,
	 19,	387,	  8,	COLOR_RED,	 20,	  4,	107,	COLOR_RED,
	 20,	293,	  9,	COLOR_RED,	 20,	387,	  8,	COLOR_RED,
	 21,	  4,	107,	COLOR_RED,	 21,	294,	  9,	COLOR_RED,
	 21,	387,	  8,	COLOR_RED,	 22,	  4,	106,	COLOR_RED,
	 22,	294,	  9,	COLOR_RED,	 22,	387,	  8,	COLOR_RED,
	 23,	  5,	104,	COLOR_RED,	 23,	295,	  9,	COLOR_RED,
	 23,	387,	  8,	COLOR_RED,	 24,	  6,	  4,	COLOR_RED,
	 24,	 24,	 76,	COLOR_RED,	 24,	295,	 10,	COLOR_RED,
	 24,	386,	  9,	COLOR_RED,	 25,	 32,	 55,	COLOR_RED,
	 25,	296,	  9,	COLOR_RED,	 25,	386,	  9,	COLOR_RED,
	 26,	 36,	  9,	COLOR_RED,	 26,	 47,	  8,	COLOR_RED,
	 26,	 57,	  4,	COLOR_RED,	 26,	296,	 10,	COLOR_RED,
	 26,	386,	  9,	COLOR_RED,	 27,	 47,	  8,	COLOR_RED,
	 27,	297,	 10,	COLOR_RED,	 27,	386,	  8,	COLOR_RED,
	 28,	 47,	  8,	COLOR_RED,	 28,	298,	  9,	COLOR_RED,
	 28,	386,	  8,	COLOR_RED,	 29,	 47,	  8,	COLOR_RED,
	 29,	298,	 10,	COLOR_RED,	 29,	385,	  9,	COLOR_RED,
	 30,	 47,	  8,	COLOR_RED,	 30,	299,	  9,	COLOR_RED,
	 30,	385,	  9,	COLOR_RED,	 31,	 47,	  8,	COLOR_RED,
	 31,	300,	  8,	COLOR_RED,	 31,	385,	  9,	COLOR_RED,
	 32,	 47,	  8,	COLOR_RED,	 32,	300,	  9,	COLOR_RED,
	 32,	385,	  8,	COLOR_RED,	 33,	 47,	  8,	COLOR_RED,
	 33,	300,	  9,	COLOR_RED,	 33,	384,	  9,	COLOR_RED,
	 34,	 47,	  8,	COLOR_RED,	 34,	300,	 10,	COLOR_RED,
	 34,	384,	  9,	COLOR_RED,	 35,	 47,	  8,	COLOR_RED,
	 35,	301,	  9,	COLOR_RED,	 35,	384,	  8,	COLOR_RED,
	 36,	 47,	  8,	COLOR_RED,	 36,	302,	  9,	COLOR_RED,
	 36,	384,	  8,	COLOR_RED,	 37,	 47,	  8,	COLOR_RED,
	 37,	302,	  9,	COLOR_RED,	 37,	383,	  9,	COLOR_RED,
	 38,	 47,	  8,	COLOR_RED,	 38,	303,	  9,	COLOR_RED,
	 38,	382,	 10,	COLOR_RED,	 39,	 47,	  8,	COLOR_RED,
	 39,	303,	  9,	COLOR_RED,	 39,	382,	  9,	COLOR_RED,
	 40,	 47,	  8,	COLOR_RED,	 40,	304,	  8,	COLOR_RED,
	 40,	382,	  9,	COLOR_RED,	 41,	 47,	  8,	COLOR_RED,
	 41,	304,	  8,	COLOR_RED,	 41,	381,	  9,	COLOR_RED,
	 42,	 47,	  8,	COLOR_RED,	 42,	304,	  9,	COLOR_RED,
	 42,	380,	 10,	COLOR_RED,	 43,	 47,	  8,	COLOR_RED,
	 43,	304,	  9,	COLOR_RED,	 43,	379,	 11,	COLOR_RED,
	 44,	 47,	  8,	COLOR_RED,	 44,	305,	  9,	COLOR_RED,
	 44,	379,	 10,	COLOR_RED,	 45,	 47,	  8,	COLOR_RED,
	 45,	305,	  9,	COLOR_RED,	 45,	378,	 11,	COLOR_RED,
	 46,	 47,	  8,	COLOR_RED,	 46,	306,	  9,	COLOR_RED,
	 46,	377,	 11,	COLOR_RED,	 47,	 47,	  8,	COLOR_RED,
	 47,	306,	  9,	COLOR_RED,	 47,	376,	 11,	COLOR_RED,
	 48,	 47,	  8,	COLOR_RED,	 48,	307,	  9,	COLOR_RED,
	 48,	376,	 10,	COLOR_RED,	 49,	 47,	  8,	COLOR_RED,
	 49,	307,	  9,	COLOR_RED,	 49,	375,	 10,	COLOR_RED,
	 50,	 47,	  8,	COLOR_RED,	 50,	308,	  8,	COLOR_RED,
	 50,	375,	  9,	COLOR_RED,	 51,	 47,	  8,	COLOR_RED,
	 51,	308,	  8,	COLOR_RED,	 51,	374,	  9,	COLOR_RED,
	 52,	 46,	  9,	COLOR_RED,	 52,	308,	  9,	COLOR_RED,
	 52,	374,	  9,	COLOR_RED,	 53,	 46,	  9,	COLOR_RED,
	 53,	308,	  9,	COLOR_RED,	 53,	373,	  9,	COLOR_RED,
	 54,	 46,	  9,	COLOR_RED,	 54,	309,	  9,	COLOR_RED,
	 54,	372,	 10,	COLOR_RED,	 55,	 46,	  8,	COLOR_RED,
	 55,	309,	  9,	COLOR_RED,	 55,	371,	 10,	COLOR_RED,
	 56,	 46,	  8,	COLOR_RED,	 56,	310,	  8,	COLOR_RED,
	 56,	370,	 11,	COLOR_RED,	 57,	 46,	  8,	COLOR_RED,
	 57,	310,	  9,	COLOR_RED,	 57,	370,	 10,	COLOR_RED,
	 58,	 46,	  8,	COLOR_RED,	 58,	310,	  9,	COLOR_RED,
	 58,	369,	 10,	COLOR_RED,	 59,	 45,	  9,	COLOR_RED,
	 59,	310,	  9,	COLOR_RED,	 59,	369,	  9,	COLOR_RED,
	 60,	 45,	  9,	COLOR_RED,	 60,	311,	  9,	COLOR_RED,
	 60,	368,	  9,	COLOR_RED,	 61,	 45,	  9,	COLOR_RED,
	 61,	311,	  9,	COLOR_RED,	 61,	368,	  9,	COLOR_RED,
	 62,	 45,	  8,	COLOR_RED,	 62,	311,	 11,	COLOR_RED,
	 62,	367,	  9,	COLOR_RED,	 63,	 45,	  8,	COLOR_RED,
	 63,	312,	 11,	COLOR_RED,	 63,	366,	 10,	COLOR_RED,
	 64,	 45,	  8,	COLOR_RED,	 64,	313,	 11,	COLOR_RED,
	 64,	365,	 10,	COLOR_RED,	 65,	 45,	  8,	COLOR_RED,
	 65,	286,	  6,	COLOR_GREEN,	 65,	313,	 12,	COLOR_RED,
	 65,	364,	 11,	COLOR_RED,	 66,	 45,	  8,	COLOR_RED,
	 66,	283,	 10,	COLOR_GREEN,	 66,	314,	 11,	COLOR_RED,
	 66,	363,	 11,	COLOR_RED,	 67,	 45,	  8,	COLOR_RED,
	 67,	282,	 12,	COLOR_GREEN,	 67,	315,	 11,	COLOR_RED,
	 67,	362,	 11,	COLOR_RED,	 68,	 45,	  8,	COLOR_RED,
	 68,	281,	 14,	COLOR_GREEN,	 68,	316,	 11,	COLOR_RED,
	 68,	361,	 11,	COLOR_RED,	 69,	 45,	  8,	COLOR_RED,
	 69,	281,	 15,	COLOR_GREEN,	 69,	318,	  9,	COLOR_RED,
	 69,	361,	 10,	COLOR_RED,	 70,	 45,	  8,	COLOR_RED,
	 70,	281,	 15,	COLOR_GREEN,	 70,	318,	 10,	COLOR_RED,
	 70,	360,	 10,	COLOR_RED,	 71,	 45,	  8,	COLOR_RED,
	 71,	281,	 15,	COLOR_GREEN,	 71,	319,	  9,	COLOR_RED,
	 71,	360,	  9,	COLOR_RED,	 72,	 45,	  8,	COLOR_RED,
	 72,	281,	 15,	COLOR_GREEN,	 72,	320,	  9,	COLOR_RED,
	 72,	359,	  9,	COLOR_RED,	 73,	 45,	  8,	COLOR_RED,
	 73,	281,	 15,	COLOR_GREEN,	 73,	320,	 10,	COLOR_RED,
	 73,	358,	 10,	COLOR_RED,	 74,	 45,	  8,	COLOR_RED,
	 74,	281,	 15,	COLOR_GREEN,	 74,	321,	 10,	COLOR_RED,
	 74,	357,	 10,	COLOR_RED,	 75,	 45,	  8,	COLOR_RED,
	 75,	281,	 15,	COLOR_GREEN,	 75,	321,	 10,	COLOR_RED,
	 75,	356,	 11,	COLOR_RED,	 76,	 45,	  8,	COLOR_RED,
	 76,	282,	 14,	COLOR_GREEN,	 76,	322,	 10,	COLOR_RED,
	 76,	355,	 11,	COLOR_RED,	 77,	 45,	  8,	COLOR_RED,
	 77,	283,	 12,	COLOR_GREEN,	 77,	323,	  9,	COLOR_RED,
	 77,	354,	 11,	COLOR_RED,	 78,	 45,	  8,	COLOR_RED,
	 78,	284,	 10,	COLOR_GREEN,	 78,	324,	  9,	COLOR_RED,
	 78,	354,	 10,	COLOR_RED,	 79,	 45,	  8,	COLOR_RED,
	 79,	285,	  8,	COLOR_GREEN,	 79,	324,	 10,	COLOR_RED,
	 79,	353,	 10,	COLOR_RED,	 80,	 45,	  8,	COLOR_RED,
	 80,	325,	 10,	COLOR_RED,	 80,	352,	 10,	COLOR_RED,
	 81,	 45,	  8,	COLOR_RED,	 81,	325,	 10,	COLOR_RED,
	 81,	352,	  9,	COLOR_RED,	 82,	 45,	  8,	COLOR_RED,
	 82,	326,	  9,	COLOR_RED,	 82,	351,	 10,	COLOR_RED,
	 83,	 45,	  8,	COLOR_RED,	 83,	327,	  9,	COLOR_RED,
	 83,	351,	  9,	COLOR_RED,	 84,	 45,	  8,	COLOR_RED,
	 84,	 90,	 12,	COLOR_GREEN,	 84,	327,	  9,	COLOR_RED,
	 84,	350,	  9,	COLOR_RED,	 85,	 45,	  8,	COLOR_RED,
	 85,	 89,	 15,	COLOR_GREEN,	 85,	327,	 10,	COLOR_RED,
	 85,	349,	 10,	COLOR_RED,	 86,	 45,	  8,	COLOR_RED,
	 86,	 88,	 18,	COLOR_GREEN,	 86,	222,	  4,	COLOR_YELLOW,
	 86,	328,	  9,	COLOR_RED,	 86,	349,	  9,	COLOR_RED,
	 87,	 45,	  8,	COLOR_RED,	 87,	 88,	 20,	COLOR_GREEN,
	 87,	220,	  7,	COLOR_YELLOW,	 87,	329,	  9,	COLOR_RED,
	 87,	348,	 10,	COLOR_RED,	 88,	 45,	  8,	COLOR_RED,
	 88,	 88,	 22,	COLOR_GREEN,	 88,	166,	 20,	COLOR_BLUE,
	 88,	219,	  9,	COLOR_YELLOW,	 88,	289,	  4,	COLOR_MAGENTA,
	 88,	329,	 10,	COLOR_RED,	 88,	348,	  9,	COLOR_RED,
	 89,	 45,	  8,	COLOR_RED,	 89,	 88,	 23,	COLOR_GREEN,
	 89,	165,	 26,	COLOR_BLUE,	 89,	218,	 21,	COLOR_YELLOW,
	 89,	288,	  6,	COLOR_MAGENTA,	 89,	330,	 10,	COLOR_RED,
	 89,	347,	  9,	COLOR_RED,	 90,	 45,	  8,	COLOR_RED,
	 90,	 88,	 24,	COLOR_GREEN,	 90,	164,	 28,	COLOR_BLUE,
	 90,	218,	 25,	COLOR_YELLOW,	 90,	287,	  8,	COLOR_MAGENTA,
	 90,	330,	 10,	COLOR_RED,	 90,	347,	  9,	COLOR_RED,
	 91,	 45,	  8,	COLOR_RED,	 91,	 88,	 25,	COLOR_GREEN,
	 91,	164,	 29,	COLOR_BLUE,	 91,	218,	 31,	COLOR_YELLOW,
	 91,	287,	  9,	COLOR_MAGENTA,	 91,	331,	 10,	COLOR_RED,
	 91,	346,	  9,	COLOR_RED,	 92,	 45,	  9,	COLOR_RED,
	 92,	 88,	  8,	COLOR_GREEN,	 92,	100,	 14,	COLOR_GREEN,
	 92,	159,	 37,	COLOR_BLUE,	 92,	218,	 36,	COLOR_YELLOW,
	 92,	287,	  9,	COLOR_MAGENTA,	 92,	332,	  9,	COLOR_RED,
	 92,	345,	 10,	COLOR_RED,	 93,	 45,	  9,	COLOR_RED,
	 93,	 88,	  8,	COLOR_GREEN,	 93,	102,	 14,	COLOR_GREEN,
	 93,	157,	 40,	COLOR_BLUE,	 93,	219,	 38,	COLOR_YELLOW,
	 93,	287,	 10,	COLOR_MAGENTA,	 93,	333,	  9,	COLOR_RED,
	 93,	344,	 10,	COLOR_RED,	 94,	 45,	  9,	COLOR_RED,
	 94,	 88,	  8,	COLOR_GREEN,	 94,	104,	 13,	COLOR_GREEN,
	 94,	154,	 44,	COLOR_BLUE,	 94,	219,	 42,	COLOR_YELLOW,
	 94,	288,	  9,	COLOR_MAGENTA,	 94,	333,	  9,	COLOR_RED,
	 94,	343,	 11,	COLOR_RED,	 95,	 46,	  8,	COLOR_RED,
	 95,	 88,	  8,	COLOR_GREEN,	 95,	106,	 12,	COLOR_GREEN,
	 95,	153,	 46,	COLOR_BLUE,	 95,	219,	 53,	COLOR_YELLOW,
	 95,	289,	  9,	COLOR_MAGENTA,	 95,	334,	 19,	COLOR_RED,
	 96,	 46,	  8,	COLOR_RED,	 96,	 88,	  8,	COLOR_GREEN,
	 96,	107,	 12,	COLOR_GREEN,	 96,	152,	 27,	COLOR_BLUE,
	 96,	183,	 18,	COLOR_BLUE,	 96,	219,	 56,	COLOR_YELLOW,
	 96,	289,	  9,	COLOR_MAGENTA,	 96,	334,	 18,	COLOR_RED,
	 97,	 46,	  9,	COLOR_RED,	 97,	 88,	  8,	COLOR_GREEN,
	 97,	108,	 12,	COLOR_GREEN,	 97,	152,	 27,	COLOR_BLUE,
	 97,	188,	 14,	COLOR_BLUE,	 97,	219,	  8,	COLOR_YELLOW,
	 97,	237,	 39,	COLOR_YELLOW,	 97,	290,	  9,	COLOR_MAGENTA,
	 97,	335,	 16,	COLOR_RED,	 98,	 46,	  9,	COLOR_RED,
	 98,	 88,	  8,	COLOR_GREEN,	 98,	109,	 12,	COLOR_GREEN,
	 98,	151,	 27,	COLOR_BLUE,	 98,	189,	 14,	COLOR_BLUE,
	 98,	219,	  8,	COLOR_YELLOW,	 98,	241,	 36,	COLOR_YELLOW,
	 98,	290,	  9,	COLOR_MAGENTA,	 98,	335,	 15,	COLOR_RED,
	 99,	 46,	  9,	COLOR_RED,	 99,	 88,	  8,	COLOR_GREEN,
	 99,	110,	 11,	COLOR_GREEN,	 99,	151,	 26,	COLOR_BLUE,
	 99,	190,	 14,	COLOR_BLUE,	 99,	219,	  8,	COLOR_YELLOW,
	 99,	246,	 32,	COLOR_YELLOW,	 99,	291,	  8,	COLOR_MAGENTA,
	 99,	336,	 13,	COLOR_RED,	100,	 47,	  8,	COLOR_RED,
	100,	 88,	  8,	COLOR_GREEN,	100,	112,	 10,	COLOR_GREEN,
	100,	150,	 11,	COLOR_BLUE,	100,	193,	 12,	COLOR_BLUE,
	100,	219,	  8,	COLOR_YELLOW,	100,	251,	 28,	COLOR_YELLOW,
	100,	291,	  9,	COLOR_MAGENTA,	100,	336,	 13,	COLOR_RED,
	101,	 47,	  8,	COLOR_RED,	101,	 88,	  8,	COLOR_GREEN,
	101,	113,	  9,	COLOR_GREEN,	101,	150,	 10,	COLOR_BLUE,
	101,	194,	 12,	COLOR_BLUE,	101,	219,	  8,	COLOR_YELLOW,
	101,	255,	 25,	COLOR_YELLOW,	101,	291,	  9,	COLOR_MAGENTA,
	101,	336,	 12,	COLOR_RED,	102,	 47,	  8,	COLOR_RED,
	102,	 88,	  8,	COLOR_GREEN,	102,	114,	  9,	COLOR_GREEN,
	102,	150,	  8,	COLOR_BLUE,	102,	195,	 11,	COLOR_BLUE,
	102,	218,	  9,	COLOR_YELLOW,	102,	258,	 23,	COLOR_YELLOW,
	102,	291,	  9,	COLOR_MAGENTA,	102,	336,	 12,	COLOR_RED,
	103,	 47,	  8,	COLOR_RED,	103,	 88,	  8,	COLOR_GREEN,
	103,	114,	  9,	COLOR_GREEN,	103,	150,	  8,	COLOR_BLUE,
	103,	197,	 10,	COLOR_BLUE,	103,	218,	  9,	COLOR_YELLOW,
	103,	269,	 12,	COLOR_YELLOW,	103,	292,	  8,	COLOR_MAGENTA,
	103,	337,	 10,	COLOR_RED,	104,	 47,	  8,	COLOR_RED,
	104,	 88,	  8,	COLOR_GREEN,	104,	115,	  9,	COLOR_GREEN,
	104,	150,	  8,	COLOR_BLUE,	104,	198,	  9,	COLOR_BLUE,
	104,	218,	  8,	COLOR_YELLOW,	104,	272,	  9,	COLOR_YELLOW,
	104,	292,	  9,	COLOR_MAGENTA,	104,	338,	  9,	COLOR_RED,
	105,	 47,	  8,	COLOR_RED,	105,	 88,	  8,	COLOR_GREEN,
	105,	115,	  9,	COLOR_GREEN,	105,	150,	  8,	COLOR_BLUE,
	105,	199,	  9,	COLOR_BLUE,	105,	218,	  8,	COLOR_YELLOW,
	105,	273,	  8,	COLOR_YELLOW,	105,	292,	  9,	COLOR_MAGENTA,
	105,	338,	  9,	COLOR_RED,	106,	 47,	  8,	COLOR_RED,
	106,	 88,	  8,	COLOR_GREEN,	106,	116,	  8,	COLOR_GREEN,
	106,	150,	  8,	COLOR_BLUE,	106,	199,	  9,	COLOR_BLUE,
	106,	218,	  8,	COLOR_YELLOW,	106,	273,	  8,	COLOR_YELLOW,
	106,	293,	  8,	COLOR_MAGENTA,	106,	338,	  9,	COLOR_RED,
	107,	 47,	  8,	COLOR_RED,	107,	 88,	  8,	COLOR_GREEN,
	107,	116,	  9,	COLOR_GREEN,	107,	150,	  8,	COLOR_BLUE,
	107,	200,	  9,	COLOR_BLUE,	107,	218,	  8,	COLOR_YELLOW,
	107,	273,	  8,	COLOR_YELLOW,	107,	293,	  8,	COLOR_MAGENTA,
	107,	337,	 11,	COLOR_RED,	108,	 47,	  8,	COLOR_RED,
	108,	 88,	  8,	COLOR_GREEN,	108,	116,	  9,	COLOR_GREEN,
	108,	150,	  8,	COLOR_BLUE,	108,	200,	 10,	COLOR_BLUE,
	108,	218,	  8,	COLOR_YELLOW,	108,	273,	  9,	COLOR_YELLOW,
	108,	293,	  9,	COLOR_MAGENTA,	108,	337,	 11,	COLOR_RED,
	109,	 47,	  8,	COLOR_RED,	109,	 88,	  8,	COLOR_GREEN,
	109,	116,	  9,	COLOR_GREEN,	109,	149,	  9,	COLOR_BLUE,
	109,	201,	 10,	COLOR_BLUE,	109,	218,	  8,	COLOR_YELLOW,
	109,	273,	  9,	COLOR_YELLOW,	109,	293,	  9,	COLOR_MAGENTA,
	109,	336,	 12,	COLOR_RED,	110,	 47,	  8,	COLOR_RED,
	110,	 88,	  8,	COLOR_GREEN,	110,	117,	  8,	COLOR_GREEN,
	110,	149,	  9,	COLOR_BLUE,	110,	201,	 10,	COLOR_BLUE,
	110,	218,	  8,	COLOR_YELLOW,	110,	274,	  8,	COLOR_YELLOW,
	110,	294,	  8,	COLOR_MAGENTA,	110,	336,	 13,	COLOR_RED,
	111,	 47,	  8,	COLOR_RED,	111,	 88,	  8,	COLOR_GREEN,
	111,	117,	  8,	COLOR_GREEN,	111,	149,	  9,	COLOR_BLUE,
	111,	202,	  9,	COLOR_BLUE,	111,	218,	  8,	COLOR_YELLOW,
	111,	274,	  8,	COLOR_YELLOW,	111,	294,	  8,	COLOR_MAGENTA,
	111,	335,	 15,	COLOR_RED,	112,	 47,	  8,	COLOR_RED,
	112,	 88,	  8,	COLOR_GREEN,	112,	117,	  8,	COLOR_GREEN,
	112,	149,	  8,	COLOR_BLUE,	112,	203,	  8,	COLOR_BLUE,
	112,	218,	  8,	COLOR_YELLOW,	112,	274,	  8,	COLOR_YELLOW,
	112,	294,	  8,	COLOR_MAGENTA,	112,	334,	 16,	COLOR_RED,
	113,	 47,	  8,	COLOR_RED,	113,	 88,	  8,	COLOR_GREEN,
	113,	117,	  8,	COLOR_GREEN,	113,	149,	  8,	COLOR_BLUE,
	113,	203,	  9,	COLOR_BLUE,	113,	217,	  9,	COLOR_YELLOW,
	113,	274,	  8,	COLOR_YELLOW,	113,	294,	  8,	COLOR_MAGENTA,
	113,	334,	 17,	COLOR_RED,	114,	 47,	  8,	COLOR_RED,
	114,	 88,	  8,	COLOR_GREEN,	114,	117,	  8,	COLOR_GREEN,
	114,	149,	  8,	COLOR_BLUE,	114,	203,	 10,	COLOR_BLUE,
	114,	217,	  9,	COLOR_YELLOW,	114,	274,	  8,	COLOR_YELLOW,
	114,	294,	  8,	COLOR_MAGENTA,	114,	334,	 18,	COLOR_RED,
	115,	 47,	  8,	COLOR_RED,	115,	 88,	  8,	COLOR_GREEN,
	115,	117,	  8,	COLOR_GREEN,	115,	149,	  8,	COLOR_BLUE,
	115,	204,	  9,	COLOR_BLUE,	115,	217,	  9,	COLOR_YELLOW,
	115,	274,	  8,	COLOR_YELLOW,	115,	294,	  8,	COLOR_MAGENTA,
	115,	334,	  8,	COLOR_RED,	115,	343,	  9,	COLOR_RED,
	116,	 47,	  8,	COLOR_RED,	116,	 88,	  8,	COLOR_GREEN,
	116,	117,	  8,	COLOR_GREEN,	116,	149,	  8,	COLOR_BLUE,
	116,	204,	  9,	COLOR_BLUE,	116,	217,	  8,	COLOR_YELLOW,
	116,	274,	  8,	COLOR_YELLOW,	116,	294,	  8,	COLOR_MAGENTA,
	116,	333,	  9,	COLOR_RED,	116,	343,	 10,	COLOR_RED,
	117,	 47,	  8,	COLOR_RED,	117,	 88,	  8,	COLOR_GREEN,
	117,	117,	  8,	COLOR_GREEN,	117,	149,	  8,	COLOR_BLUE,
	117,	205,	  8,	COLOR_BLUE,	117,	217,	  8,	COLOR_YELLOW,
	117,	274,	  8,	COLOR_YELLOW,	117,	294,	  8,	COLOR_MAGENTA,
	117,	333,	  9,	COLOR_RED,	117,	344,	 10,	COLOR_RED,
	118,	 47,	  8,	COLOR_RED,	118,	 88,	  8,	COLOR_GREEN,
	118,	117,	  8,	COLOR_GREEN,	118,	149,	  8,	COLOR_BLUE,
	118,	205,	  8,	COLOR_BLUE,	118,	217,	  8,	COLOR_YELLOW,
	118,	274,	  8,	COLOR_YELLOW,	118,	294,	  8,	COLOR_MAGENTA,
	118,	332,	  9,	COLOR_RED,	118,	345,	  9,	COLOR_RED,
	119,	 47,	  8,	COLOR_RED,	119,	 88,	  8,	COLOR_GREEN,
	119,	116,	  9,	COLOR_GREEN,	119,	149,	  8,	COLOR_BLUE,
	119,	205,	  8,	COLOR_BLUE,	119,	217,	  8,	COLOR_YELLOW,
	119,	273,	  9,	COLOR_YELLOW,	119,	294,	  8,	COLOR_MAGENTA,
	119,	332,	  9,	COLOR_RED,	119,	345,	 10,	COLOR_RED,
	120,	 47,	  8,	COLOR_RED,	120,	 87,	  9,	COLOR_GREEN,
	120,	114,	 11,	COLOR_GREEN,	120,	149,	  8,	COLOR_BLUE,
	120,	205,	  8,	COLOR_BLUE,	120,	217,	  8,	COLOR_YELLOW,
	120,	272,	 10,	COLOR_YELLOW,	120,	294,	  8,	COLOR_MAGENTA,
	120,	331,	  9,	COLOR_RED,	120,	346,	  9,	COLOR_RED,
	121,	 47,	  8,	COLOR_RED,	121,	 87,	  9,	COLOR_GREEN,
	121,	111,	 13,	COLOR_GREEN,	121,	148,	  9,	COLOR_BLUE,
	121,	205,	  8,	COLOR_BLUE,	121,	217,	  8,	COLOR_YELLOW,
	121,	270,	 12,	COLOR_YELLOW,	121,	294,	  8,	COLOR_MAGENTA,
	121,	330,	 10,	COLOR_RED,	121,	347,	  9,	COLOR_RED,
	122,	 47,	  8,	COLOR_RED,	122,	 87,	  8,	COLOR_GREEN,
	122,	108,	 16,	COLOR_GREEN,	122,	148,	  9,	COLOR_BLUE,
	122,	205,	  8,	COLOR_BLUE,	122,	217,	  8,	COLOR_YELLOW,
	122,	268,	 13,	COLOR_YELLOW,	122,	294,	  8,	COLOR_MAGENTA,
	122,	330,	  9,	COLOR_RED,	122,	347,	 10,	COLOR_RED,
	123,	 47,	  8,	COLOR_RED,	123,	 87,	  8,	COLOR_GREEN,
	123,	 99,	 24,	COLOR_GREEN,	123,	148,	  8,	COLOR_BLUE,
	123,	205,	  8,	COLOR_BLUE,	123,	217,	  8,	COLOR_YELLOW,
	123,	265,	 15,	COLOR_YELLOW,	123,	294,	  8,	COLOR_MAGENTA,
	123,	330,	  9,	COLOR_RED,	123,	348,	 10,	COLOR_RED,
	124,	 47,	  8,	COLOR_RED,	124,	 87,	  8,	COLOR_GREEN,
	124,	 96,	 26,	COLOR_GREEN,	124,	148,	  8,	COLOR_BLUE,
	124,	205,	  8,	COLOR_BLUE,	124,	217,	  8,	COLOR_YELLOW,
	124,	250,	 29,	COLOR_YELLOW,	124,	294,	  8,	COLOR_MAGENTA,
	124,	330,	  8,	COLOR_RED,	124,	348,	 10,	COLOR_RED,
	125,	 47,	  8,	COLOR_RED,	125,	 87,	 35,	COLOR_GREEN,
	125,	148,	  8,	COLOR_BLUE,	125,	205,	  8,	COLOR_BLUE,
	125,	217,	  8,	COLOR_YELLOW,	125,	244,	 34,	COLOR_YELLOW,
	125,	294,	  8,	COLOR_MAGENTA,	125,	329,	  9,	COLOR_RED,
	125,	349,	  9,	COLOR_RED,	126,	 47,	  8,	COLOR_RED,
	126,	 87,	 34,	COLOR_GREEN,	126,	148,	  8,	COLOR_BLUE,
	126,	205,	  8,	COLOR_BLUE,	126,	217,	  8,	COLOR_YELLOW,
	126,	239,	 38,	COLOR_YELLOW,	126,	294,	  8,	COLOR_MAGENTA,
	126,	329,	  9,	COLOR_RED,	126,	350,	  9,	COLOR_RED,
	127,	 47,	  8,	COLOR_RED,	127,	 87,	 33,	COLOR_GREEN,
	127,	148,	  8,	COLOR_BLUE,	127,	205,	  8,	COLOR_BLUE,
	127,	217,	  8,	COLOR_YELLOW,	127,	234,	 42,	COLOR_YELLOW,
	127,	294,	  8,	COLOR_MAGENTA,	127,	328,	  9,	COLOR_RED,
	127,	350,	  9,	COLOR_RED,	128,	 47,	  8,	COLOR_RED,
	128,	 87,	 30,	COLOR_GREEN,	128,	148,	  8,	COLOR_BLUE,
	128,	205,	  8,	COLOR_BLUE,	128,	217,	  8,	COLOR_YELLOW,
	128,	229,	 45,	COLOR_YELLOW,	128,	293,	  9,	COLOR_MAGENTA,
	128,	327,	 10,	COLOR_RED,	128,	350,	 10,	COLOR_RED,
	129,	 47,	  8,	COLOR_RED,	129,	 87,	 27,	COLOR_GREEN,
	129,	148,	  8,	COLOR_BLUE,	129,	205,	  8,	COLOR_BLUE,
	129,	217,	  8,	COLOR_YELLOW,	129,	228,	 44,	COLOR_YELLOW,
	129,	293,	  9,	COLOR_MAGENTA,	129,	326,	 10,	COLOR_RED,
	129,	351,	  9,	COLOR_RED,	130,	 47,	  8,	COLOR_RED,
	130,	 87,	 24,	COLOR_GREEN,	130,	148,	  8,	COLOR_BLUE,
	130,	205,	  8,	COLOR_BLUE,	130,	217,	  8,	COLOR_YELLOW,
	130,	227,	 44,	COLOR_YELLOW,	130,	293,	  8,	COLOR_MAGENTA,
	130,	325,	 11,	COLOR_RED,	130,	352,	  8,	COLOR_RED,
	131,	 47,	  8,	COLOR_RED,	131,	 87,	 15,	COLOR_GREEN,
	131,	148,	  8,	COLOR_BLUE,	131,	204,	  9,	COLOR_BLUE,
	131,	217,	  8,	COLOR_YELLOW,	131,	226,	 42,	COLOR_YELLOW,
	131,	292,	  9,	COLOR_MAGENTA,	131,	325,	 10,	COLOR_RED,
	131,	352,	  9,	COLOR_RED,	132,	 47,	  8,	COLOR_RED,
	132,	 87,	 11,	COLOR_GREEN,	132,	148,	  8,	COLOR_BLUE,
	132,	204,	  9,	COLOR_BLUE,	132,	217,	 35,	COLOR_YELLOW,
	132,	292,	  9,	COLOR_MAGENTA,	132,	325,	  9,	COLOR_RED,
	132,	352,	  9,	COLOR_RED,	133,	 47,	  8,	COLOR_RED,
	133,	 87,	 11,	COLOR_GREEN,	133,	148,	  8,	COLOR_BLUE,
	133,	204,	  9,	COLOR_BLUE,	133,	217,	 30,	COLOR_YELLOW,
	133,	292,	  9,	COLOR_MAGENTA,	133,	325,	  8,	COLOR_RED,
	133,	352,	 10,	COLOR_RED,	134,	 47,	  8,	COLOR_RED,
	134,	 86,	 13,	COLOR_GREEN,	134,	148,	  8,	COLOR_BLUE,
	134,	204,	  8,	COLOR_BLUE,	134,	217,	 25,	COLOR_YELLOW,
	134,	291,	  9,	COLOR_MAGENTA,	134,	324,	  9,	COLOR_RED,
	134,	353,	  9,	COLOR_RED,	135,	 47,	  8,	COLOR_RED,
	135,	 86,	 14,	COLOR_GREEN,	135,	148,	  8,	COLOR_BLUE,
	135,	204,	  8,	COLOR_BLUE,	135,	217,	 20,	COLOR_YELLOW,
	135,	291,	  9,	COLOR_MAGENTA,	135,	324,	  9,	COLOR_RED,
	135,	354,	  8,	COLOR_RED,	136,	 47,	  8,	COLOR_RED,
	136,	 86,	 14,	COLOR_GREEN,	136,	148,	  8,	COLOR_BLUE,
	136,	204,	  8,	COLOR_BLUE,	136,	217,	  8,	COLOR_YELLOW,
	136,	226,	  6,	COLOR_YELLOW,	136,	291,	  9,	COLOR_MAGENTA,
	136,	324,	  8,	COLOR_RED,	136,	354,	  9,	COLOR_RED,
	137,	 47,	  8,	COLOR_RED,	137,	 86,	 15,	COLOR_GREEN,
	137,	148,	  8,	COLOR_BLUE,	137,	204,	  8,	COLOR_BLUE,
	137,	217,	  8,	COLOR_YELLOW,	137,	227,	  4,	COLOR_YELLOW,
	137,	290,	  9,	COLOR_MAGENTA,	137,	324,	  8,	COLOR_RED,
	137,	354,	  9,	COLOR_RED,	138,	 47,	  8,	COLOR_RED,
	138,	 86,	 17,	COLOR_GREEN,	138,	148,	  8,	COLOR_BLUE,
	138,	204,	  8,	COLOR_BLUE,	138,	217,	  9,	COLOR_YELLOW,
	138,	289,	 10,	COLOR_MAGENTA,	138,	324,	  8,	COLOR_RED,
	138,	354,	  9,	COLOR_RED,	139,	 47,	  8,	COLOR_RED,
	139,	 86,	 18,	COLOR_GREEN,	139,	148,	  8,	COLOR_BLUE,
	139,	204,	  8,	COLOR_BLUE,	139,	217,	  9,	COLOR_YELLOW,
	139,	289,	 10,	COLOR_MAGENTA,	139,	323,	  9,	COLOR_RED,
	139,	355,	  9,	COLOR_RED,	140,	 47,	  8,	COLOR_RED,
	140,	 86,	 19,	COLOR_GREEN,	140,	148,	  8,	COLOR_BLUE,
	140,	204,	  8,	COLOR_BLUE,	140,	217,	  9,	COLOR_YELLOW,
	140,	289,	 10,	COLOR_MAGENTA,	140,	323,	  9,	COLOR_RED,
	140,	355,	  9,	COLOR_RED,	141,	 47,	  8,	COLOR_RED,
	141,	 86,	 20,	COLOR_GREEN,	141,	148,	  8,	COLOR_BLUE,
	141,	203,	  9,	COLOR_BLUE,	141,	218,	  8,	COLOR_YELLOW,
	141,	289,	 10,	COLOR_MAGENTA,	141,	323,	  9,	COLOR_RED,
	141,	355,	 10,	COLOR_RED,	142,	 47,	  8,	COLOR_RED,
	142,	 86,	  8,	COLOR_GREEN,	142,	 95,	 12,	COLOR_GREEN,
	142,	148,	  8,	COLOR_BLUE,	142,	203,	  9,	COLOR_BLUE,
	142,	218,	  8,	COLOR_YELLOW,	142,	289,	  9,	COLOR_MAGENTA,
	142,	322,	  9,	COLOR_RED,	142,	356,	  9,	COLOR_RED,
	143,	 47,	  8,	COLOR_RED,	143,	 86,	  8,	COLOR_GREEN,
	143,	 96,	 13,	COLOR_GREEN,	143,	148,	  8,	COLOR_BLUE,
	143,	203,	  8,	COLOR_BLUE,	143,	218,	  8,	COLOR_YELLOW,
	143,	289,	  9,	COLOR_MAGENTA,	143,	322,	  9,	COLOR_RED,
	143,	357,	  9,	COLOR_RED,	144,	 47,	  8,	COLOR_RED,
	144,	 86,	  8,	COLOR_GREEN,	144,	 97,	 13,	COLOR_GREEN,
	144,	148,	  8,	COLOR_BLUE,	144,	203,	  8,	COLOR_BLUE,
	144,	218,	  8,	COLOR_YELLOW,	144,	289,	  9,	COLOR_MAGENTA,
	144,	322,	  9,	COLOR_RED,	144,	357,	  9,	COLOR_RED,
	145,	 47,	  8,	COLOR_RED,	145,	 86,	  8,	COLOR_GREEN,
	145,	 99,	 12,	COLOR_GREEN,	145,	148,	  8,	COLOR_BLUE,
	145,	203,	  8,	COLOR_BLUE,	145,	218,	  8,	COLOR_YELLOW,
	145,	288,	  9,	COLOR_MAGENTA,	145,	321,	  9,	COLOR_RED,
	145,	358,	  9,	COLOR_RED,	146,	 47,	  8,	COLOR_RED,
	146,	 86,	  8,	COLOR_GREEN,	146,	100,	 12,	COLOR_GREEN,
	146,	148,	  8,	COLOR_BLUE,	146,	203,	  8,	COLOR_BLUE,
	146,	218,	  8,	COLOR_YELLOW,	146,	288,	  9,	COLOR_MAGENTA,
	146,	321,	  9,	COLOR_RED,	146,	358,	  9,	COLOR_RED,
	147,	 47,	  8,	COLOR_RED,	147,	 86,	  8,	COLOR_GREEN,
	147,	101,	 13,	COLOR_GREEN,	147,	148,	  8,	COLOR_BLUE,
	147,	203,	  8,	COLOR_BLUE,	147,	218,	  8,	COLOR_YELLOW,
	147,	288,	  9,	COLOR_MAGENTA,	147,	321,	  9,	COLOR_RED,
	147,	359,	  8,	COLOR_RED,	148,	 47,	  8,	COLOR_RED,
	148,	 86,	  8,	COLOR_GREEN,	148,	102,	 14,	COLOR_GREEN,
	148,	148,	  9,	COLOR_BLUE,	148,	202,	  9,	COLOR_BLUE,
	148,	218,	  9,	COLOR_YELLOW,	148,	288,	  9,	COLOR_MAGENTA,
	148,	321,	  8,	COLOR_RED,	148,	359,	  9,	COLOR_RED,
	149,	 47,	  8,	COLOR_RED,	149,	 86,	  8,	COLOR_GREEN,
	149,	103,	 15,	COLOR_GREEN,	149,	148,	  9,	COLOR_BLUE,
	149,	202,	  9,	COLOR_BLUE,	149,	218,	  9,	COLOR_YELLOW,
	149,	288,	  8,	COLOR_MAGENTA,	149,	321,	  8,	COLOR_RED,
	149,	359,	  9,	COLOR_RED,	150,	 47,	  8,	COLOR_RED,
	150,	 86,	  8,	COLOR_GREEN,	150,	105,	 14,	COLOR_GREEN,
	150,	148,	  9,	COLOR_BLUE,	150,	202,	  9,	COLOR_BLUE,
	150,	219,	  8,	COLOR_YELLOW,	150,	287,	  9,	COLOR_MAGENTA,
	150,	320,	  9,	COLOR_RED,	150,	359,	 10,	COLOR_RED,
	151,	 47,	  8,	COLOR_RED,	151,	 86,	  8,	COLOR_GREEN,
	151,	106,	 14,	COLOR_GREEN,	151,	149,	  8,	COLOR_BLUE,
	151,	201,	  9,	COLOR_BLUE,	151,	219,	  8,	COLOR_YELLOW,
	151,	287,	  9,	COLOR_MAGENTA,	151,	320,	  9,	COLOR_RED,
	151,	360,	  9,	COLOR_RED,	152,	 47,	  8,	COLOR_RED,
	152,	 86,	  8,	COLOR_GREEN,	152,	107,	 14,	COLOR_GREEN,
	152,	149,	  8,	COLOR_BLUE,	152,	201,	  9,	COLOR_BLUE,
	152,	219,	  8,	COLOR_YELLOW,	152,	287,	  9,	COLOR_MAGENTA,
	152,	320,	  9,	COLOR_RED,	152,	361,	  9,	COLOR_RED,
	153,	 47,	  8,	COLOR_RED,	153,	 86,	  8,	COLOR_GREEN,
	153,	108,	 13,	COLOR_GREEN,	153,	149,	  8,	COLOR_BLUE,
	153,	200,	 10,	COLOR_BLUE,	153,	219,	  9,	COLOR_YELLOW,
	153,	287,	  9,	COLOR_MAGENTA,	153,	320,	  8,	COLOR_RED,
	153,	361,	  9,	COLOR_RED,	154,	 47,	  8,	COLOR_RED,
	154,	 86,	  8,	COLOR_GREEN,	154,	110,	 13,	COLOR_GREEN,
	154,	149,	  9,	COLOR_BLUE,	154,	200,	  9,	COLOR_BLUE,
	154,	219,	  9,	COLOR_YELLOW,	154,	287,	  9,	COLOR_MAGENTA,
	154,	319,	  9,	COLOR_RED,	154,	362,	  9,	COLOR_RED,
	155,	 47,	  8,	COLOR_RED,	155,	 86,	  8,	COLOR_GREEN,
	155,	112,	 12,	COLOR_GREEN,	155,	149,	  9,	COLOR_BLUE,
	155,	199,	  9,	COLOR_BLUE,	155,	219,	  9,	COLOR_YELLOW,
	155,	287,	  9,	COLOR_MAGENTA,	155,	318,	 10,	COLOR_RED,
	155,	362,	  9,	COLOR_RED,	156,	 47,	  8,	COLOR_RED,
	156,	 86,	  8,	COLOR_GREEN,	156,	114,	 11,	COLOR_GREEN,
	156,	150,	  8,	COLOR_BLUE,	156,	199,	  9,	COLOR_BLUE,
	156,	220,	  8,	COLOR_YELLOW,	156,	286,	 10,	COLOR_MAGENTA,
	156,	318,	  9,	COLOR_RED,	156,	363,	  9,	COLOR_RED,
	157,	 47,	  8,	COLOR_RED,	157,	 86,	  8,	COLOR_GREEN,
	157,	114,	 13,	COLOR_GREEN,	157,	150,	  8,	COLOR_BLUE,
	157,	198,	  9,	COLOR_BLUE,	157,	220,	  8,	COLOR_YELLOW,
	157,	286,	  9,	COLOR_MAGENTA,	157,	318,	  9,	COLOR_RED,
	157,	363,	 10,	COLOR_RED,	158,	 47,	  8,	COLOR_RED,
	158,	 86,	  8,	COLOR_GREEN,	158,	115,	 13,	COLOR_GREEN,
	158,	150,	  9,	COLOR_BLUE,	158,	197,	 10,	COLOR_BLUE,
	158,	220,	  8,	COLOR_YELLOW,	158,	286,	  9,	COLOR_MAGENTA,
	158,	318,	  8,	COLOR_RED,	158,	364,	  9,	COLOR_RED,
	159,	 47,	  8,	COLOR_RED,	159,	 86,	  8,	COLOR_GREEN,
	159,	116,	 13,	COLOR_GREEN,	159,	150,	  9,	COLOR_BLUE,
	159,	197,	  9,	COLOR_BLUE,	159,	220,	  8,	COLOR_YELLOW,
	159,	286,	  9,	COLOR_MAGENTA,	159,	317,	  9,	COLOR_RED,
	159,	364,	  9,	COLOR_RED,	160,	 47,	  8,	COLOR_RED,
	160,	 86,	  8,	COLOR_GREEN,	160,	117,	 14,	COLOR_GREEN,
	160,	151,	  8,	COLOR_BLUE,	160,	196,	 10,	COLOR_BLUE,
	160,	220,	  8,	COLOR_YELLOW,	160,	286,	  9,	COLOR_MAGENTA,
	160,	317,	  9,	COLOR_RED,	160,	365,	  9,	COLOR_RED,
	161,	 47,	  8,	COLOR_RED,	161,	 86,	  8,	COLOR_GREEN,
	161,	119,	 14,	COLOR_GREEN,	161,	151,	  9,	COLOR_BLUE,
	161,	196,	  9,	COLOR_BLUE,	161,	220,	  8,	COLOR_YELLOW,
	161,	286,	  9,	COLOR_MAGENTA,	161,	316,	  9,	COLOR_RED,
	161,	365,	  9,	COLOR_RED,	162,	 47,	  8,	COLOR_RED,
	162,	 86,	  8,	COLOR_GREEN,	162,	120,	 14,	COLOR_GREEN,
	162,	151,	  9,	COLOR_BLUE,	162,	195,	  9,	COLOR_BLUE,
	162,	220,	  8,	COLOR_YELLOW,	162,	286,	  9,	COLOR_MAGENTA,
	162,	316,	  9,	COLOR_RED,	162,	365,	 10,	COLOR_RED,
	163,	 47,	  8,	COLOR_RED,	163,	 86,	  8,	COLOR_GREEN,
	163,	121,	 14,	COLOR_GREEN,	163,	151,	  9,	COLOR_BLUE,
	163,	195,	  9,	COLOR_BLUE,	163,	220,	  9,	COLOR_YELLOW,
	163,	286,	  9,	COLOR_MAGENTA,	163,	316,	  8,	COLOR_RED,
	163,	366,	  9,	COLOR_RED,	164,	 47,	  8,	COLOR_RED,
	164,	 86,	  8,	COLOR_GREEN,	164,	123,	 13,	COLOR_GREEN,
	164,	152,	  9,	COLOR_BLUE,	164,	194,	  9,	COLOR_BLUE,
	164,	220,	  9,	COLOR_YELLOW,	164,	286,	  9,	COLOR_MAGENTA,
	164,	316,	  8,	COLOR_RED,	164,	367,	  9,	COLOR_RED,
	165,	 47,	  8,	COLOR_RED,	165,	 86,	  8,	COLOR_GREEN,
	165,	124,	 13,	COLOR_GREEN,	165,	152,	  9,	COLOR_BLUE,
	165,	193,	 10,	COLOR_BLUE,	165,	221,	  8,	COLOR_YELLOW,
	165,	286,	  8,	COLOR_MAGENTA,	165,	315,	  9,	COLOR_RED,
	165,	367,	  9,	COLOR_RED,	166,	 47,	  8,	COLOR_RED,
	166,	 86,	  8,	COLOR_GREEN,	166,	125,	 12,	COLOR_GREEN,
	166,	152,	 10,	COLOR_BLUE,	166,	193,	  9,	COLOR_BLUE,
	166,	221,	  8,	COLOR_YELLOW,	166,	286,	  8,	COLOR_MAGENTA,
	166,	315,	  9,	COLOR_RED,	166,	368,	  9,	COLOR_RED,
	167,	 47,	  8,	COLOR_RED,	167,	 86,	  8,	COLOR_GREEN,
	167,	127,	 11,	COLOR_GREEN,	167,	153,	  9,	COLOR_BLUE,
	167,	192,	 10,	COLOR_BLUE,	167,	221,	  9,	COLOR_YELLOW,
	167,	285,	  9,	COLOR_MAGENTA,	167,	315,	  8,	COLOR_RED,
	167,	368,	 10,	COLOR_RED,	168,	 47,	  8,	COLOR_RED,
	168,	 86,	  8,	COLOR_GREEN,	168,	129,	 10,	COLOR_GREEN,
	168,	154,	  9,	COLOR_BLUE,	168,	192,	  9,	COLOR_BLUE,
	168,	221,	  9,	COLOR_YELLOW,	168,	285,	  9,	COLOR_MAGENTA,
	168,	314,	  9,	COLOR_RED,	168,	369,	  9,	COLOR_RED,
	169,	 47,	  8,	COLOR_RED,	169,	 86,	  8,	COLOR_GREEN,
	169,	130,	 10,	COLOR_GREEN,	169,	154,	  9,	COLOR_BLUE,
	169,	192,	  8,	COLOR_BLUE,	169,	222,	  8,	COLOR_YELLOW,
	169,	285,	  9,	COLOR_MAGENTA,	169,	314,	  9,	COLOR_RED,
	169,	369,	 10,	COLOR_RED,	170,	 47,	  8,	COLOR_RED,
	170,	 86,	  8,	COLOR_GREEN,	170,	130,	 11,	COLOR_GREEN,
	170,	155,	  9,	COLOR_BLUE,	170,	191,	  9,	COLOR_BLUE,
	170,	222,	  8,	COLOR_YELLOW,	170,	285,	  9,	COLOR_MAGENTA,
	170,	314,	  9,	COLOR_RED,	170,	370,	 10,	COLOR_RED,
	171,	 47,	  8,	COLOR_RED,	171,	 86,	  8,	COLOR_GREEN,
	171,	131,	 10,	COLOR_GREEN,	171,	155,	 10,	COLOR_BLUE,
	171,	190,	 10,	COLOR_BLUE,	171,	222,	  8,	COLOR_YELLOW,
	171,	285,	  9,	COLOR_MAGENTA,	171,	313,	  9,	COLOR_RED,
	171,	371,	  9,	COLOR_RED,	172,	 47,	  8,	COLOR_RED,
	172,	 86,	  8,	COLOR_GREEN,	172,	132,	  9,	COLOR_GREEN,
	172,	156,	  9,	COLOR_BLUE,	172,	190,	 10,	COLOR_BLUE,
	172,	222,	  8,	COLOR_YELLOW,	172,	285,	  9,	COLOR_MAGENTA,
	172,	313,	  9,	COLOR_RED,	172,	371,	 10,	COLOR_RED,
	173,	 47,	  8,	COLOR_RED,	173,	 86,	  8,	COLOR_GREEN,
	173,	133,	  9,	COLOR_GREEN,	173,	156,	 10,	COLOR_BLUE,
	173,	189,	 10,	COLOR_BLUE,	173,	222,	  8,	COLOR_YELLOW,
	173,	285,	  9,	COLOR_MAGENTA,	173,	313,	  9,	COLOR_RED,
	173,	372,	  9,	COLOR_RED,	174,	 47,	  9,	COLOR_RED,
	174,	 86,	  8,	COLOR_GREEN,	174,	133,	  9,	COLOR_GREEN,
	174,	157,	  9,	COLOR_BLUE,	174,	188,	 10,	COLOR_BLUE,
	174,	222,	  9,	COLOR_YELLOW,	174,	285,	  9,	COLOR_MAGENTA,
	174,	313,	  8,	COLOR_RED,	174,	373,	  9,	COLOR_RED,
	175,	 47,	  9,	COLOR_RED,	175,	 86,	  8,	COLOR_GREEN,
	175,	133,	  9,	COLOR_GREEN,	175,	158,	  9,	COLOR_BLUE,
	175,	187,	 10,	COLOR_BLUE,	175,	222,	  9,	COLOR_YELLOW,
	175,	285,	  9,	COLOR_MAGENTA,	175,	312,	  9,	COLOR_RED,
	175,	373,	 10,	COLOR_RED,	176,	 47,	  9,	COLOR_RED,
	176,	 86,	  8,	COLOR_GREEN,	176,	134,	  9,	COLOR_GREEN,
	176,	158,	 10,	COLOR_BLUE,	176,	186,	 11,	COLOR_BLUE,
	176,	222,	  9,	COLOR_YELLOW,	176,	285,	  9,	COLOR_MAGENTA,
	176,	312,	  9,	COLOR_RED,	176,	374,	 10,	COLOR_RED,
	177,	 48,	  8,	COLOR_RED,	177,	 86,	  9,	COLOR_GREEN,
	177,	134,	  9,	COLOR_GREEN,	177,	159,	 10,	COLOR_BLUE,
	177,	185,	 11,	COLOR_BLUE,	177,	223,	  8,	COLOR_YELLOW,
	177,	285,	  9,	COLOR_MAGENTA,	177,	311,	  9,	COLOR_RED,
	177,	374,	 11,	COLOR_RED,	178,	 48,	  8,	COLOR_RED,
	178,	 86,	  9,	COLOR_GREEN,	178,	134,	  9,	COLOR_GREEN,
	178,	159,	 13,	COLOR_BLUE,	178,	184,	 12,	COLOR_BLUE,
	178,	223,	  8,	COLOR_YELLOW,	178,	285,	  8,	COLOR_MAGENTA,
	178,	310,	 10,	COLOR_RED,	178,	375,	 10,	COLOR_RED,
	179,	 48,	  8,	COLOR_RED,	179,	 86,	  9,	COLOR_GREEN,
	179,	135,	  8,	COLOR_GREEN,	179,	160,	 13,	COLOR_BLUE,
	179,	183,	 12,	COLOR_BLUE,	179,	223,	  9,	COLOR_YELLOW,
	179,	285,	  8,	COLOR_MAGENTA,	179,	309,	 10,	COLOR_RED,
	179,	376,	  9,	COLOR_RED,	180,	 48,	  8,	COLOR_RED,
	180,	 87,	  8,	COLOR_GREEN,	180,	136,	  6,	COLOR_GREEN,
	180,	161,	 33,	COLOR_BLUE,	180,	223,	  9,	COLOR_YELLOW,
	180,	285,	  8,	COLOR_MAGENTA,	180,	309,	 10,	COLOR_RED,
	180,	377,	 10,	COLOR_RED,	181,	 48,	  8,	COLOR_RED,
	181,	 87,	  8,	COLOR_GREEN,	181,	137,	  4,	COLOR_GREEN,
	181,	162,	 30,	COLOR_BLUE,	181,	223,	  9,	COLOR_YELLOW,
	181,	285,	  8,	COLOR_MAGENTA,	181,	308,	 10,	COLOR_RED,
	181,	377,	 11,	COLOR_RED,	182,	 49,	  6,	COLOR_RED,
	182,	 87,	  8,	COLOR_GREEN,	182,	163,	 28,	COLOR_BLUE,
	182,	224,	  8,	COLOR_YELLOW,	182,	285,	  8,	COLOR_MAGENTA,
	182,	307,	 10,	COLOR_RED,	182,	377,	 12,	COLOR_RED,
	183,	 50,	  4,	COLOR_RED,	183,	 88,	  6,	COLOR_GREEN,
	183,	164,	 26,	COLOR_BLUE,	183,	224,	  8,	COLOR_YELLOW,
	183,	285,	  8,	COLOR_MAGENTA,	183,	306,	 11,	COLOR_RED,
	183,	378,	 11,	COLOR_RED,	184,	 89,	  4,	COLOR_GREEN,
	184,	165,	 24,	COLOR_BLUE,	184,	224,	  8,	COLOR_YELLOW,
	184,	285,	  8,	COLOR_MAGENTA,	184,	305,	 12,	COLOR_RED,
	184,	379,	 10,	COLOR_RED,	185,	166,	 22,	COLOR_BLUE,
	185,	225,	  6,	COLOR_YELLOW,	185,	285,	  8,	COLOR_MAGENTA,
	185,	305,	 11,	COLOR_RED,	185,	379,	 10,	COLOR_RED,
	186,	169,	 18,	COLOR_BLUE,	186,	226,	  4,	COLOR_YELLOW,
	186,	285,	  8,	COLOR_MAGENTA,	186,	305,	 10,	COLOR_RED,
	186,	380,	  9,	COLOR_RED,	187,	171,	 14,	COLOR_BLUE,
	187,	285,	  8,	COLOR_MAGENTA,	187,	305,	  8,	COLOR_RED,
	187,	381,	  8,	COLOR_RED,	188,	285,	  8,	COLOR_MAGENTA,
	188,	306,	  6,	COLOR_RED,	188,	382,	  6,	COLOR_RED,
	189,	285,	  8,	COLOR_MAGENTA,	189,	307,	  4,	COLOR_RED,
	189,	383,	  4,	COLOR_RED,	190,	285,	  8,	COLOR_MAGENTA,
	191,	286,	  6,	COLOR_MAGENTA,	192,	287,	  4,	COLOR_MAGENTA,
	  0,	  0,	  0,	0
};

/*
 ******	Protótipos de funções ***********************************
 */
void		screen_saver_draw_image (int line, int col);

/*
 ****************************************************************
 *	Liga o "screen_saver"					*
 ****************************************************************
 */
void
screen_saver_on (void)
{
	int		i, j;

	if   (video_data->video_mode == 1)
	{
		screen_saver_draw_image (0, 0);
	}
	elif (video_data->video_mode == 0)
	{
		for (i = 0; i < PAT_LINE; i++)
		{
			for (j = 0; j < PAT_LETTER * PAT_COL; j++)
				put_video_char (screen_saver_table[i][j], i, j, screen_saver_color, 0);
		}
	}

	screen_saver_line = 0; 	screen_saver_col   = 0;
	screen_saver_down = 1; 	screen_saver_right = 1;

}	/* end screen_saver_on */

/*
 ****************************************************************
 *	Avança o "screen_saver"					*
 ****************************************************************
 */
void
screen_saver_inc (void)
{
	int		i, j;

	if   (video_data->video_mode == 1)
	{
		/* Apaga o desenho anterior */
	
		screen_saver_clr ();
	
		/* Avança a posição */
	
		if (screen_saver_down)
		{
			if (screen_saver_line + 200 >= video_data->video_lines )
				{ screen_saver_down = 0; screen_saver_line -= 5; }
			else
				screen_saver_line += 5;
		}
		else	/* screen_saver_up */
		{
			if (screen_saver_line <= 0)
				{ screen_saver_down = 1; screen_saver_line += 5; }
			else
				screen_saver_line -= 5;
		}
	
		if (screen_saver_right)
		{
			if (screen_saver_col + 400 >= video_data->video_cols)
				{ screen_saver_right = 0; screen_saver_col -= 5; }
			else
				screen_saver_col += 5;
		}
		else	/* screen_saver_left */
		{
			if (screen_saver_col <= 0)
				{ screen_saver_right = 1; screen_saver_col += 5; }
			else
				screen_saver_col -= 5;
		}

		/* Desenha na nova locação com a nova cor */

		screen_saver_draw_image (screen_saver_line, screen_saver_col);
	}
	elif (video_data->video_mode == 0)
	{
		/* Apaga o desenho anterior */
	
		screen_saver_clr ();
	
		/* Troca a cor (1 a 7) */
	
		if (++screen_saver_color > 7)
			screen_saver_color  = 1;
	
		/* Avança a posição */
	
		if (screen_saver_down)
		{
			if (screen_saver_line + PAT_LINE >= video_data->video_LINE )
				{ screen_saver_down = 0; screen_saver_line--; }
			else
				screen_saver_line++;
		}
		else	/* screen_saver_up */
		{
			if (screen_saver_line <= 0)
				{ screen_saver_down = 1; screen_saver_line++; }
			else
				screen_saver_line--;
		}
	
		if (screen_saver_right)
		{
			if (screen_saver_col + PAT_LETTER * PAT_COL - PAT_SPACE >= video_data->video_COL)
				{ screen_saver_right = 0; screen_saver_col--; }
			else
				screen_saver_col++;
		}
		else	/* screen_saver_left */
		{
			if (screen_saver_col <= 0)
				{ screen_saver_right = 1; screen_saver_col++; }
			else
				screen_saver_col--;
		}
	
		/* Desenha na nova locação com a nova cor */
	
		for (i = 0; i < PAT_LINE; i++)
		{
			for (j = 0; j < PAT_LETTER * PAT_COL; j++)
			{
				put_video_char
				(	screen_saver_table[i][j],
					screen_saver_line + i,
					screen_saver_col + j,
					screen_saver_color,
					0
				);
			}
		}
	}

}	/* end screen_saver_inc */

/*
 ****************************************************************
 *	Apaga o "screen_saver"					*
 ****************************************************************
 */
void
screen_saver_clr (void)
{
	int		i, j;

	if   (video_data->video_mode == 1)
	{
		const VIDEODATA		*vp = video_data;
		char			*video_ptr;

		for (i = 0; i < 200; i++)
		{
			video_ptr = vp->video_linear_area + (screen_saver_line + i) * vp->video_cols;

			video_ptr += screen_saver_col;

			for (j = 0; j < 400; j++)
				*video_ptr++ = 0;
		}
	}
	elif (video_data->video_mode == 0)
	{
		for (i = 0; i < PAT_LINE; i++)
		{
			for (j = 0; j < PAT_LETTER * PAT_COL; j++)
				put_video_char (' ', screen_saver_line + i, screen_saver_col + j, 0, 0);
		}
	}

}	/* end screen_saver_clr */

/*
 ****************************************************************
 *	Desenha a imagem TROPIX.GIF				*
 ****************************************************************
 */
void
screen_saver_draw_image (int line, int col)
{
	const IMAGE_LINE	*ip;
	const VIDEODATA		*vp = video_data;
	char			*video_ptr;
	int			n;

	/*
	 *	x
	 */
	for (ip = screen_saver_image; ip->count != 0; ip++)
	{
		video_ptr = vp->video_linear_area + line * vp->video_cols + col; 

		video_ptr += ip->line * vp->video_cols + ip->col; 

#if (0)	/*******************************************************/
	int			n, color;
		if ((color = ip->color) == 6)
			color = 14;
#endif	/*******************************************************/

		for (n = ip->count; n > 0; n--)
			*video_ptr++ = ip->color;
	}

}	/* end screen_saver_draw_image */
