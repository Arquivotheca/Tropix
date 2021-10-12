/*
 ****************************************************************
 *								*
 *			localtime.c 				*
 *								*
 *	Conversão de "time_t" em "TM"				*
 *								*
 *	Versão	1.0.0, de 29.08.86				*
 *		4.8.0, de 07.09.05				*
 *								*
 *	Módulo: localtime					*
 *		libc/etc					*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2005 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/syscall.h>

#include <stdlib.h>
#include <time.h>

/*
 ****************************************************************
 *	Variáveis e funções globais				*
 ****************************************************************
 */

/*
 ******	Dias acumulativos para anos regulares e bissextos *******
 */
entry const int	_ctime_regular_days_vec[] =
{
	0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365, 1000000
};

entry const int	_ctime_leap_days_vec[] =
{
	0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366, 1000000
};

/*
 ****************************************************************
 *	Conversão "time_t" em TM LOCAL				*
 ****************************************************************
 */
TM *
localtime (const time_t *timer)
{
	time_t		tempo;
	
	tempo = *timer + gettzmin () * 60;

	return (gmtime (&tempo));

}	/* end localtime */

/*
 ****************************************************************
 *	Conversão de "time_t" para a estrutura TM em GMT	*
 ****************************************************************
 */
TM *
gmtime (const time_t *time_ptr)
{
	const int	*days_vec;
	int		day_part, second_part;
	int		feb_29_cnt, year_from_1970;
	int		day_in_year, day, leap_cnt, month;
	int		module, hour;
	TM		*tp;
	static TM	tm;

	/*
	 *	Começa com o ano, mes e dia
	 */
	day_part	= *time_ptr / (24 * 60 * 60);
	second_part	= *time_ptr % (24 * 60 * 60);

	/* Constantes:	(4 * 365 + 1)		=> Dias em 4 anos		*/
	/*		(365 * 2 - 31 - 28)	=> Dias de 28.2.72 até 1.1.74	*/

	feb_29_cnt = (day_part + 365 * 2 - 31 - 28) / (4 * 365 + 1);

	year_from_1970 = (day_part - feb_29_cnt) / 365;

	leap_cnt = (year_from_1970 + 1) >> 2;

	day_in_year = day_part - (year_from_1970 * 365 + leap_cnt);

	days_vec = (year_from_1970 + 2) & 3 ? _ctime_regular_days_vec : _ctime_leap_days_vec;

	for (month = 1; day_in_year >= days_vec[month]; month++)
		/* vazio */;

   /***	if (month > 12)		/* NÃO deve acontecer!	***/
   /***		month = 12;				***/

	day = day_in_year - days_vec[month - 1];

	/*
	 *	Calcula horas, minutos e segundos
	 */
	hour	= second_part / (60 * 60);
	module	= second_part % (60 * 60);

	/*
	 *	Preenche a estrutura TM
	 */
	tp = &tm;

	tp->tm_sec   = module % 60;		/* Segundos */
	tp->tm_min   = module / 60;		/* Minutos */
	tp->tm_hour  = hour;			/* Horas */
	tp->tm_mday  = day + 1;			/* Dia do mes (1 - 31) */
	tp->tm_mon   = month - 1;		/* Mes (0 - 11, jan = 0) */
	tp->tm_year  = year_from_1970 + 70;	/* Ano - 1900 */
	tp->tm_wday  = (day_part + 4) % 7;	/* Dia da semana (0 - 6, dom = 0) */
	tp->tm_yday  = day_in_year;		/* Dia no ano */
	tp->tm_isdst = 0;			/* Horário de verão = 1 */

	return (tp);

}	/* end gmtime */
