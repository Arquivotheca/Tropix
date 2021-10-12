#include <stdio.h>
#include <stdlib.h>
#include <locale.h>

void		print_locale (void);

main (int argc, register const char *argv[])
{
	register const LCONV	*lp;

	lp = localeconv ();

	printf
	(	"decimal_point\t\t= \"%s\"\n"
		"thousands_sep\t\t= \"%s\"\n"
		"grouping\t\t= \"%s\"\n"
		"int_curr_symbol\t\t= \"%s\"\n"
		"currency_symbol\t\t= \"%s\"\n"
		"mon_decimal_point\t= \"%s\"\n"
		"mon_thousands_sep\t= \"%s\"\n"
		"mon_grouping\t\t= \"%s\"\n"
		"positive_sign\t\t= \"%s\"\n"
		"negative_sign\t\t= \"%s\"\n"
		"int_frac_digits\t\t= \"%d\"\n"
		"frac_digits\t\t= \"%d\"\n"
		"p_cs_precedes\t\t= \"%d\"\n"
		"p_sep_by_space\t\t= \"%d\"\n"
		"n_cs_precedes\t\t= \"%d\"\n"
		"n_sep_by_space\t\t= \"%d\"\n"
		"p_sign_posn\t\t= \"%d\"\n"
		"n_sign_posn\t\t= \"%d\"\n\n",

		lp->decimal_point,	 	lp->thousands_sep,
		lp->grouping, 			lp->int_curr_symbol,
		lp->currency_symbol, 		lp->mon_decimal_point,
		lp->mon_thousands_sep, 		lp->mon_grouping,
		lp->positive_sign, 		lp->negative_sign,
		lp->int_frac_digits, 		lp->frac_digits,
		lp->p_cs_precedes, 		lp->p_sep_by_space,
		lp->n_cs_precedes, 		lp->n_sep_by_space,
		lp->p_sign_posn, 		lp->n_sign_posn
	);

	print_locale ();
	printf ("setlocale LC_COLLATE (C) = \"%s\"\n\n", setlocale (LC_COLLATE, "C"));
	print_locale ();
	printf ("setlocale LC_COLLATE (E) = \"%s\"\n\n", setlocale (LC_COLLATE, "E"));
	print_locale ();
	printf ("setlocale LC_COLLATE (P) = \"%s\"\n\n", setlocale (LC_COLLATE, "P"));
	print_locale ();
	printf ("setlocale LC_ALL (E) = \"%s\"\n\n", setlocale (LC_ALL, "E"));
	print_locale ();
	printf ("setlocale LC_COLLATE (F) = \"%s\"\n\n", setlocale (LC_COLLATE, "F"));
	print_locale ();
	printf ("setlocale LC_COLLATE (P) = \"%s\"\n\n", setlocale (LC_COLLATE, "P"));
	print_locale ();
	printf ("setlocale LC_COLLATE (E) = \"%s\"\n\n", setlocale (LC_COLLATE, "E"));
	print_locale ();
}

/*
 ****************************************************************
 *	Imprime os valores					*
 ****************************************************************
 */
void
print_locale (void)
{
	printf ("LC_ALL		= \"%s\"\n", setlocale (LC_ALL, NOSTR));
	printf ("LC_COLLATE	= \"%s\"\n", setlocale (LC_COLLATE, NOSTR));
	printf ("LC_CTYPE	= \"%s\"\n", setlocale (LC_CTYPE, NOSTR));
	printf ("LC_MONETARY	= \"%s\"\n", setlocale (LC_MONETARY, NOSTR));
	printf ("LC_NUMERIC	= \"%s\"\n", setlocale (LC_NUMERIC, NOSTR));
	printf ("LC_TIME   	= \"%s\"\n", setlocale (LC_TIME, NOSTR));
	printf ("\n");

}	/* end print_locale */

#if (0)	/*******************************************************/
/*
 ****************************************************************
 *	Definição de tipos					*
 ****************************************************************
 */
typedef enum
{
	LC_ALL = 1,	/* Todo o "locale" */
	LC_COLLATE,	/* Funções "strcoll" e "strxfrm" */
	LC_CTYPE,	/* Funções de análise de caracteres */
	LC_MONETARY,	/* Informação monetária */
	LC_NUMERIC,	/* Ponto decimal (e outros) */
	LC_TIME		/* Função "strftime" */

} 	LC_ENUM;

#define	LC_SIZE	LC_TIME

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
 *	Protótipos						*
 ****************************************************************
 */
extern const char	*setlocale (int, const char *);
extern const LCONV	*localeconv (void);
#endif	/*******************************************************/
