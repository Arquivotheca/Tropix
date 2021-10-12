/*
 ****************************************************************
 *								*
 *			<locale.h>				*
 *								*
 *	Localization						*
 *								*
 *	Versão	2.3.0, de 02.03.90				*
 *		3.0.0, de 27.01.98				*
 *								*
 *	Módulo: locale.h					*
 *		/usr/include					*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Multiprocessado		*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

/*
 ****************************************************************
 *	Definição de tipos					*
 ****************************************************************
 */
typedef enum
{
	LC_ALL,		/* Todo o "locale" */
	LC_COLLATE,	/* Funções "strcoll" e "strxfrm" */
	LC_CTYPE,	/* Funções de análise de caracteres */
	LC_MONETARY,	/* Informação monetária */
	LC_NUMERIC,	/* Ponto decimal (e outros) */
	LC_TIME		/* Função "strftime" */

} 	LC_ENUM;

#define	LC_SIZE	(LC_TIME + 1)

typedef struct lconv
{
	char	*decimal_point;	
	char	*thousands_sep;
	char	*grouping;
	char	*int_curr_symbol;
	char	*currency_symbol;
	char	*mon_decimal_point;
	char	*mon_thousands_sep;
	char	*mon_grouping;
	char	*positive_sign;
	char	*negative_sign;
	char	int_frac_digits;
	char	frac_digits;
	char	p_cs_precedes;
	char	p_sep_by_space;
	char	n_cs_precedes;
	char	n_sep_by_space;
	char	p_sign_posn;
	char	n_sign_posn;

}	LCONV;

/*
 ****************************************************************
 *	Definição de macros					*
 ****************************************************************
 */
#define	NULL	0

/*
 ****************************************************************
 *	Variáveis externas					*
 ****************************************************************
 */
#define	LC_COL	4

extern char	_lc_vector[LC_SIZE][LC_COL];
extern LCONV	_lc_lconv;

/*
 ****************************************************************
 *	Protótipos						*
 ****************************************************************
 */
extern const char	*setlocale (int, const char *);
extern const LCONV	*localeconv (void);
