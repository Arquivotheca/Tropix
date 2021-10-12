#include <stdio.h>
#include <time.h>

/*
 ******	Protótipos de funções ***********************************
 ******	Algumas datas boas para teste ***************************
 */
TM	date_vec[] =
{
	{ 15,	45,	15,	12,	0,	70,	0,	0,	0 	},
	{ 15,	45,	15,	12,	6,	70,	0,	0,	0 	},
	{ 15,	45,	15,	12,	0,	71,	0,	0,	0 	},
	{ 15,	45,	15,	12,	6,	71,	0,	0,	0 	},
	{ 15,	45,	15,	12,	0,	72,	0,	0,	0 	},
	{ 15,	45,	15,	27,	1,	72,	0,	0,	0 	},
	{ 15,	45,	15,	28,	1,	72,	0,	0,	0 	},
	{ 15,	45,	15,	29,	1,	72,	0,	0,	0 	},
	{ 15,	45,	15,	01,	2,	72,	0,	0,	0 	},
	{ 15,	45,	15,	12,	6,	72,	0,	0,	0 	},
	{ 15,	45,	15,	12,	0,	73,	0,	0,	0 	},
	{ 15,	45,	15,	12,	6,	73,	0,	0,	0 	},
	{ 15,	45,	15,	12,	0,	74,	0,	0,	0 	},
	{ 15,	45,	15,	12,	6,	74,	0,	0,	0 	},
	{ 15,	45,	15,	12,	0,	75,	0,	0,	0 	},
	{ 15,	45,	15,	12,	6,	75,	0,	0,	0 	},
	{ 15,	45,	15,	12,	0,	76,	0,	0,	0 	},
	{ 15,	45,	15,	12,	6,	76,	0,	0,	0 	},
	{ 15,	45,	15,	12,	0,	77,	0,	0,	0 	},
	{ 15,	45,	15,	12,	6,	77,	0,	0,	0 	},
	{ 15,	45,	15,	12,	0,	79,	0,	0,	0 	},
	{ 15,	45,	15,	12,	6,	79,	0,	0,	0 	},
	{ 15,	45,	15,	12,	0,	80,	0,	0,	0 	},
	{ 15,	45,	15,	12,	6,	80,	0,	0,	0 	},
	{ 15,	45,	15,	12,	0,	81,	0,	0,	0 	},
	{ 15,	45,	15,	12,	6,	81,	0,	0,	0 	},
	{ 15,	45,	15,	12,	0,	103,	0,	0,	0 	},
	{ 15,	45,	15,	12,	6,	103,	0,	0,	0 	},
	{ 15,	45,	15,	12,	0,	104,	0,	0,	0 	},
	{ 15,	45,	15,	12,	6,	104,	0,	0,	0 	},
	{ 15,	45,	15,	12,	0,	105,	0,	0,	0 	},
	{ 15,	45,	15,	27,	1,	105,	0,	0,	0 	},
	{ 15,	45,	15,	28,	1,	105,	0,	0,	0 	},
	{ 15,	45,	15,	01,	2,	105,	0,	0,	0 	},
	{ 15,	45,	15,	12,	6,	105,	0,	0,	0 	},
#if (0)	/*******************************************************/
	{ 15,	45,	15,	12,	0,	139,	0,	0,	0 	},
#endif	/*******************************************************/
	{ 0	}
};

/*
 ******	Dias acumulativos para anos regulares e bissextos *******
 */
static const int	regular_days_vec[] =
{
	0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365, 1000000
};

static const int	leap_days_vec[] =
{
	0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366, 1000000
};


int
main (int argc, const char *argv[])
{
	TM		*tp;
	time_t		t;
	const int	*days_vec;
	int		day_part, second_part;
	int		feb_29_cnt, year_from_1970, year;
	int		day, leap_cnt, month;
	int		mod, hour, minute, second;
	int		dia, total;

	/*
	 *	Testa o limite
	 */
	t = 0x7FFFFFFF; printf ("%s", brasctime (gmtime (&t)));
	t = 0x80000000; printf ("%s", brasctime (gmtime (&t)));

	/*
	 *	Percorre as datas
	 */
	for (tp = date_vec; tp->tm_year != 0; tp++)
	{
		t = mktime (tp);
		printf ("%s", brasctime (gmtime (&t)));

		/*
		 *	Parte nova
		 */
		day_part	= t / (24 * 60 * 60);
		second_part	= t % (24 * 60 * 60);
	
		/* Constantes:	(4 * 365 + 1)		=> Dias em 4 anos		*/
		/*	 	(365 * 2 - 31 - 28)	=> Dias de 28.2.72 até 1.1.74	*/
	
		feb_29_cnt = (day_part + 365 * 2 - 31 - 28) / (4 * 365 + 1);
	
		year_from_1970 = (day_part - feb_29_cnt) / 365; year = year_from_1970 + 1970;
	
		leap_cnt = (year_from_1970 + 1) >> 2;

#if (0)	/*******************************************************/
if (feb_29_cnt != leap_cnt)
printf ("feb_29_cnt = %d, leap_cnt = %d\n", feb_29_cnt, leap_cnt);
#endif	/*******************************************************/
	
		day = day_part - (year_from_1970 * 365 + leap_cnt);
	
		days_vec = year & 3 ? regular_days_vec : leap_days_vec;
	
		for (month = 1; day >= days_vec[month]; month++)
			/* vazio */;
	
		if (month > 12)
			printf ("Erro: mes maior do que 12\n");
	
		day -= days_vec[month - 1]; 
	
#if (0)	/*******************************************************/
		printf ("ano = %d, mes = %d, dia = %d\n", year, month, day + 1);
#endif	/*******************************************************/
	
		/*
		 *	Calcula horas, minutos e segundos
		 */
		hour	= second_part / (60 * 60);
		mod	= second_part % (60 * 60);
	
		minute	= mod / 60;
		second	= mod % 60;
	
#if (0)	/*******************************************************/
		printf ("hora= %d, minuto = %d, segundo = %d\n", hour, minute, second);
#endif	/*******************************************************/
	
		/*
		 ******	Agora, ao contrário *************************************
		 */
		dia  = year_from_1970 * 365;
	
		dia += (year_from_1970 + 1) >> 2;
	
		days_vec = year & 3 ? regular_days_vec : leap_days_vec;
	
		dia += days_vec[month - 1];
	
		dia += day;
	
		total = ((dia * 24 + hour) * 60 + minute) * 60 + second;
	
#if (0)	/*******************************************************/
		printf ("Original = %d, calculado = %d\n", t, total);
#endif	/*******************************************************/

		if (t != total)
			printf ("****** ERRO: Original = %d, calculado = %d\n", t, total);
	}

	return (0);
}

#if (0)	/*******************************************************/
	/*
	 *	Parte convencional
	 */
	time (&t);

	tp = localtime (&t);

	printf ("tm_sec   = %d\n",  	tp->tm_sec);
	printf ("tm_min   = %d\n",  	tp->tm_min);
	printf ("tm_hour  = %d\n",  	tp->tm_hour);
	printf ("tm_mday  = %d\n",  	tp->tm_mday);
	printf ("tm_mon   = %d\n",  	tp->tm_mon);
	printf ("tm_year  = %d\n",  	tp->tm_year);
	printf ("tm_wday  = %d\n",  	tp->tm_wday);
	printf ("tm_yday  = %d\n",  	tp->tm_yday);
	printf ("tm_isdst = %d\n\n",  	tp->tm_isdst);

	printf ("LOCAL = %s", asctime (localtime (&t)));
	printf ("GMT   = %s", asctime (gmtime (&t)));

	printf ("LOCAL = %s", brasctime (localtime (&t)));
	printf ("GMT   = %s", brasctime (gmtime (&t)));
#endif	/*******************************************************/

