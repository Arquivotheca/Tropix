#include <time.h>
#include <stdio.h>
#include <string.h>


int
main ()
{
	time_t		data, mdata;
	TM		tm, *new_tp;
	char		buf[32];

	time (&data); memmove (&tm, gmtime (&data), sizeof (TM));

	printf ("data = %d, mktime = %d\n", data, mktime (&tm));

	for (data = 0; data < 0x7F000000; data += 050000111)
	{
		memmove (&tm, gmtime (&data), sizeof (TM));
		strftime (buf, 32, "%c", &tm);

		tm.tm_wday = tm.tm_yday = -1;

		mdata = mktime (&tm);

		printf ("data = %10d, \"%s\", %10d   ", data, buf, mdata);

		if (data == mdata)
			printf ("OK");
		else
			printf ("******* ERRO");

		new_tp = gmtime (&mdata);

		if (new_tp->tm_wday != tm.tm_wday)
			printf ("\t\tErro no WDAY");
		else
			printf ("\t\tWDAY OK");

		if (new_tp->tm_yday != tm.tm_yday)
			printf ("\t\tErro no YDAY");
		else
			printf ("\t\tYDAY OK");

		printf ("\n");
	}

	return (0);
}


#if (0)	/*******************************************************/
/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
static const int	ndiasmes[] =
{
	31, 28, 31, 30, 31, 30,	31, 31, 30, 31, 30, 31
};

/*
 ****************************************************************
 *	Conversão de tempo: "TM" em "time_t"			*
 ****************************************************************
 */
time_t
mktime (TM  *tp)
{
	int		numanos, i;
	int		bissexto, n29fev;
	time_t		tempo;

	numanos = tp->tm_year - 70;

	if (((numanos + 2) % 4) == 0)
		bissexto = 1;
	else
		bissexto = 0;

	if (tp->tm_mday > ndiasmes[tp->tm_mon])
	{
		if (tp->tm_mon != 1 || tp->tm_mday != 29 || bissexto != 1)
			return (-1);
	}

	n29fev = (numanos + 1) / 4;

	tempo = (numanos * 365 + n29fev);

	tp->tm_yday = tempo;

	for (i = 0; i < tp->tm_mon; i++)
	{
		tempo += ndiasmes[i];

		if (i == 1)
			tempo += bissexto;
	}

	tempo += tp->tm_mday - 1;

	tp->tm_yday = tempo - tp->tm_yday;

	tp->tm_wday = (tempo + 4) % 7;

	tempo = ((tempo * 24) + tp->tm_hour) * 60;
	tempo = (tempo + tp->tm_min) * 60 + tp->tm_sec;

	return (tempo);

}	/* end mktime */
#endif	/*******************************************************/
