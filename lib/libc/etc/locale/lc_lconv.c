/*
 ****************************************************************
 *								*
 *			lc_lconv.c				*
 *								*
 *	Gerência de localização da "libc"			*
 *								*
 *	Versão	3.0.0, de 29.01.98				*
 *		3.0.0, de 29.01.98				*
 *								*
 *	Módulo: lc_lconv					*
 *		libc/etc					*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <locale.h>
#include <limits.h>

/*
 ****************************************************************
 *	Vetor de dados numéricos				*
 ****************************************************************
 */
entry LCONV	_lc_lconv =
{
	".",		/* char	*decimal_point */
	"",		/* char	*thousands_sep */
	"",		/* char	*grouping */
	"",		/* char	*int_curr_symbol */
	"",		/* char	*currency_symbol */
	"",		/* char	*mon_decimal_point */
	"",		/* char	*mon_thousands_sep */
	"",		/* char	*mon_grouping */
	"",		/* char	*positive_sign */
	"",		/* char	*negative_sign */
	CHAR_MAX,	/* char	int_frac_digits */
	CHAR_MAX,	/* char	frac_digits */
	CHAR_MAX,	/* char	p_cs_precedes */
	CHAR_MAX,	/* char	p_sep_by_space */
	CHAR_MAX,	/* char	n_cs_precedes */
	CHAR_MAX,	/* char	n_sep_by_space */
	CHAR_MAX,	/* char	p_sign_posn */
	CHAR_MAX	/* char	n_sign_posn */
};
