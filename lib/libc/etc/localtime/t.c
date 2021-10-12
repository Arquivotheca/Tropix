#include <sys/syscall.h>

#include <stdio.h>
#include <time.h>
#include <string.h>

/*
 ******	Protótipos de funções ***********************************
 */
TM		*old_localtime (const time_t *timer);
TM		*old_gmtime (const time_t *timer);

/*
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

int
main ()
{
	TM		*tp;
	time_t		t;

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
	printf ("\n\n");

	/*
	 *	Percorre as datas boas
	 */
	for (tp = date_vec; tp->tm_year != 0; tp++)
	{
		const TM	*old_tp, *new_tp;
		const char	*old_str, *new_str;

		t = mktime (tp);

		old_tp = old_gmtime (&t);
		new_tp = gmtime (&t);

		old_str = strdup (brasctime (old_tp));
		new_str = strdup (brasctime (new_tp));

		printf ("%24.24s :: %24.24s", new_str, old_str);

		if (memeq (old_tp, new_tp, sizeof (TM)))
		{
			printf ("\t TM OK");
		}
		else
		{
			if (old_tp->tm_sec != new_tp->tm_sec)
				printf ("\tSEC");

			if (old_tp->tm_min != new_tp->tm_min)
				printf ("\tMIN");

			if (old_tp->tm_hour != new_tp->tm_hour)
				printf ("\tHOUR");

			if (old_tp->tm_mday != new_tp->tm_mday)
				printf ("\tMDAY");

			if (old_tp->tm_mon != new_tp->tm_mon)
				printf ("\tMON (%d, %d)", old_tp->tm_mon, new_tp->tm_mon);

			if (old_tp->tm_year != new_tp->tm_year)
				printf ("\tYEAR");

			if (old_tp->tm_wday != new_tp->tm_wday)
				printf ("\tWDAY");

			if (old_tp->tm_yday != new_tp->tm_yday)
				printf ("\tYDAY");

			if (old_tp->tm_isdst != new_tp->tm_isdst)
				printf ("\tISDST");

			printf ("\t***** ERRO em TM");
		}

		if (streq (old_str, new_str))
			printf ("\t\tSTR OK\n");
		else
			printf ("\t\t***** ERRO em STR\n");
	}

	return (0);
}

/*
 ****************************************************************
 *	Variáveis e funções globais				*
 ****************************************************************
 */
static TM		tm;		/* Estrutura devolvida */

static const int	ndiasmes[] =
{
	31, 28, 31, 30, 31, 30,
	31, 31, 30, 31, 30, 31
};

/*
 ****************************************************************
 *	Conversão "time_t" em TM LOCAL				*
 ****************************************************************
 */
TM *
old_localtime (const time_t *timer)
{
	time_t		tempo;
	
	tempo = *timer + gettzmin () * 60;

	return (old_gmtime (&tempo));

}	/* end old_localtime */

/*
 ****************************************************************
 *	Conversão "time_t" em TM GMT				*
 ****************************************************************
 */
TM *
old_gmtime (const time_t *timer)
{
	TM		*tp;
	int		numanos, numdias, offset_mes;
	time_t		tempo;
	int		offset_ano, offset_hora, offset_min;
	int		offset_dia, bissexto, nbissextos, n29fev, mes; 

	tp = &tm;

	tp->tm_isdst = 0;

	tempo = *timer;

	numdias = tempo / 86400;
	
	tp->tm_wday = (numdias + 4) % 7;

	offset_dia = tempo % 86400;

	tp->tm_hour = offset_dia / 3600;

	offset_hora = offset_dia % 3600;

	tp->tm_min = offset_hora / 60;

	offset_min = offset_hora % 60;

	tp->tm_sec = offset_min % 60;

	n29fev = (numdias + 671) / 1461;

	numanos = (numdias - n29fev) / 365;

	tp->tm_year = numanos + 70;

	nbissextos = (numanos + 1) >> 2;

	offset_ano = numdias - (numanos * 365 + nbissextos);

	tp->tm_yday = offset_ano;

	if (((numanos + 2) & 3) == 0)
		bissexto = 1;
	else
		bissexto = 0;

	offset_mes = offset_ano + 1;

	mes = 0;

	while (offset_mes > ndiasmes[mes])
	{
		offset_mes -= ndiasmes[mes];

		mes++;

		if (mes == 1  &&  bissexto == 1)
		{
			if (offset_mes == 29)
				break;

			if (offset_mes > 29)
				offset_mes -= 1;
		}
	}

	tp->tm_mon = mes;
	tp->tm_mday = offset_mes;

	return (tp);

}	/* end old_gmtime */
