/*
 ****************************************************************
 *								*
 *			modecolor.c				*
 *								*
 *	Prepara um vetor de cores para os tipos de arquivos	*
 *								*
 *	Versão	3.0.0, de 11.02.99				*
 *		4.6.0, de 26.08.04				*
 *								*
 *	Módulo: modecolor					*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2004 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/stat.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */

/*
 ******	Tabela de cores *****************************************
 */
typedef struct
{
	char		nm[5];
	char		index;

}	MODETB;

static const MODETB	mode_tb[] =
{
	"fifo",	S_IFIFO >> 12,	/* FIFO */
	"chr",	S_IFCHR >> 12,	/* CHR  */
	"dir",	S_IFDIR >> 12,	/* DIR  */
	"blk",	S_IFBLK >> 12,	/* BLK  */
	"reg",	S_IFREG >> 12,	/* REG  */
	"lnk",	S_IFLNK >> 12,	/* LNK  */
	"warn",	MC_WARN,	/* WARN */
	"err",	MC_ERR,		/* ERR  */
	""
};

static const char	* const color_nm[] =
{
	"preto",	"black",	/*  0 */
	"azul",		"blue",		/*  1 */
	"verde",	"green",	/*  2 */
	"ciano",	"cyan",		/*  3 */
	"vermelho",	"red",		/*  4 */
	"magenta",	"magenta",	/*  5 */
	"marrom",	"brown",	/*  6 */
	"branco",	"white",	/*  7 */
	"preto",	"black",	/*  8 */
	"azulclaro",	"lightblue",	/*  9 */
	"verdeclaro",	"lightgreen",	/* 10 */
	"cianoclaro",	"lightcyan",	/* 11 */
	"vermelhoclaro","lightred",	/* 12 */
	"magentaclaro",	"lightmagenta",	/* 13 */
	"marromclaro",	"lightbrown",	/* 14 */
	"brancoclaro",	"lightwhite",	/* 15 */
	NOSTR
};

/*
 ******	Vetores (que serão devolvidos) **************************
 */
static char		value_vector[19] =
{
	255, 255, 255, 255, 255, 255, 255, 255,		/* cores 0 a 7 */
	255, 255, 255, 255, 255, 255, 255, 255,		/* cores 8 a 15 */
	255,						/* Para restaurar a cor original */
	255, 255					/* Advertência & erro */
};

static const char	*string_vector[19] =
{
	"", "", "", "", "", "", "", "",			/* cores 0 a 7 */
	"", "", "", "", "", "", "", "",			/* cores 8 a 15 */
	"",						/* Para restaurar a cor original */
	"", ""						/* Advertência & erro */
};

/*
 ****************************************************************
 *	Prepara um vetor de cores para os tipos de arquivos	*
 ****************************************************************
 */
int
modecolor (int active, char **value_vector_ptr, const char ***string_vector_ptr)
{
	const char	*original_field;
	char		*field;
	int		index, c, code = 0;
	const MODETB	*mp;
	const char	* const *color;
	char		*cp, *string;
	const char	*str;

	/*
	 *	Inicialmente, devolve os endereços dos vetores
	 */
	if (value_vector_ptr)
		*value_vector_ptr = value_vector;

	if (string_vector_ptr)
		*string_vector_ptr = string_vector;

	if (!active)
		return (0);

#if (0)	/*******************************************************/
	/*
	 *	Insere a cadeia para restaurar a cor original
	 */
	string_vector[MC_DEF] = "\e[0m";

	/*
	 *	Processa "warn" & "err" default
	 */
	value_vector[MC_WARN] = 1; string_vector[MC_WARN] = "\e[31m";	/* Azul */

	value_vector[MC_ERR]  = 4; string_vector[MC_ERR]  = "\e[34m";	/* Vermelho */
#endif	/*******************************************************/

	/*
	 *	Procura e copia a cadeia do ambiente
	 */
	if ((original_field = getenv ("MODECOLOR")) == NOSTR)
		return (0);

	field = alloca (strlen (original_field) + 1); strcpy (field, original_field);

	/*
	 *	Processa "MODECOLOR"
	 */
	for (field = strfield (field, ','); field != NOSTR; field = strfield (NOSTR, ','))
	{
		if ((cp = strchr (field, '=')) == NOSTR)
			continue;

		*cp++ = '\0';

		for (mp = mode_tb; /* abaixo */; mp++)
		{
			if (mp->nm[0] == '\0')
				goto bad;

			if (streq (field, mp->nm))
				{ index = mp->index; break; }
		}

		if (isdigit (*cp))
		{
			c = strtol (cp, &str, 0);

			if ((unsigned)c > 15 || *str != '\0')
				goto bad;
		}
		else for (c = 0, color = color_nm; *color != NOSTR; c++, color++)
		{
			if (*color == NOSTR)
				goto bad;

			if (streq (cp, *color))
				{ c >>= 1; break; }
		}

		value_vector[index] = c;

		if ((string = malloc (12)) != NOSTR)
		{
			if (c < 8)
				sprintf (string, "\e[3%1dm", c);
			else
				sprintf (string, "\e[1m\e[3%1dm", c - 8);

			string_vector[index] = string;

			continue;
		}

		/* Em caso de erro, ... */

	    bad:
		code = -1;

	}	/* for (fields) */

	/*
	 *	Insere a cadeia para restaurar a cor original
	 */
	string_vector[MC_DEF] = "\e[0m";

	return (code);

}	/* end modecolor */
