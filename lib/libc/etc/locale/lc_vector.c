/*
 ****************************************************************
 *								*
 *			lc_vector.c				*
 *								*
 *	Gerência de localização da "libc"			*
 *								*
 *	Versão	3.0.0, de 29.01.98				*
 *		3.0.0, de 29.01.98				*
 *								*
 *	Módulo: lc_vector					*
 *		libc/etc					*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <locale.h>

/*
 ****************************************************************
 *	Vetor das linguagens definidas				*
 ****************************************************************
 */
#define	LC_COL	4

entry char	_lc_vector[LC_SIZE][LC_COL] =
{
	{ 'P' },	/* LC_ALL 	Todo o "locale" */
	{ 'P' },	/* LC_COLLATE,	Funções "strcoll" e "strxfrm" */
	{ 'P' },	/* LC_CTYPE,	Funções de análise de caracteres */
	{ 'P' },	/* LC_MONETARY,	Informação monetária */
	{ 'P' },	/* LC_NUMERIC,	Ponto decimal (e outros) */
	{ 'P' }		/* LC_TIME	Função "strftime" */
};
