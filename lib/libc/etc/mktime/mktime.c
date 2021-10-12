/*
 ****************************************************************
 *								*
 *			mktime.c				*
 *								*
 *	Conversão de tempo: "TM" em "time_t"			*
 *								*
 *	Versão	2.3.0, de 08.03.90				*
 *		4.8.0, de 07.09.05				*
 *								*
 *	Módulo: mktime						*
 *		libc/etc					*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2005 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <stdlib.h>
#include <time.h>

/*
 ****************************************************************
 *	Conversão de tempo: "TM" em "time_t"			*
 ****************************************************************
 */
time_t
mktime (TM  *tp)
{
	const int		*days_vec;
	int			year_from_1970, day;
	time_t			value;
	extern const int	_ctime_regular_days_vec[];
	extern const int	_ctime_leap_days_vec[];

	year_from_1970 = tp->tm_year - 70;

	day = year_from_1970 * 365 + ((year_from_1970 + 1) >> 2);	/* Começa com o ano */

	tp->tm_yday = day;
	
	days_vec = (year_from_1970 + 2) & 3 ? _ctime_regular_days_vec : _ctime_leap_days_vec;

	day += days_vec[tp->tm_mon];					/* Acrescenta o mes */

	day += tp->tm_mday - 1;						/* Acrescenta o dia */

	tp->tm_yday = day - tp->tm_yday;				/* Dia no ano */

	tp->tm_wday = (day + 4) % 7;					/* Dia da semana (0 - 6, dom = 0) */

	value = day   * 24 + tp->tm_hour;				/* Acrescenta as horas */

	value = value * 60 + tp->tm_min;				/* Acrescenta os minutos */

	value = value * 60 + tp->tm_sec;				/* Acrescenta os segundos */

	return (value);

}	/* end mktime */
