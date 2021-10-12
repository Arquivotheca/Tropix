/*
 ****************************************************************
 *								*
 *			cmos.clock.c				*
 *								*
 *	Inicializa o tempo a partir do relógio do PC		*
 *								*
 *	Versão	3.0.0, de 08.04.95				*
 *		3.0.5, de 05.01.97				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 1999 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include "../h/common.h"
#include "../h/sync.h"
#include "../h/scb.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ******	Protótipos de funções ***********************************
 */
int		bcd_to_int  (unsigned int);
unsigned long 	years_to_unix_seconds (unsigned int);
unsigned long 	months_to_unix_seconds (unsigned int, int);

/*
 ****************************************************************
 *	Registros do relógio no CMOS				*
 ****************************************************************
 */
#define RTC_STATUSA	0x0A	/* Status register A */
#define RTCSA_TUP	0x80	/* Time update, don't look now */

#define RTC_SEC		0x00	/* Segundos:	0..59 */
#define RTC_MIN		0x02	/* Minutos:	0..59 */
#define RTC_HRS		0x04	/* Horas:	0..23 */

#define RTC_DAY		0x07	/* Dia do mes:	1..31 */
#define RTC_MONTH	0x08	/* Mes do ano:	1..12 */
#define RTC_YEAR	0x09	/* Ano:		0..99 */

/*
 ****************************************************************
 *	Inicializa a noção de tempo do sistema			*
 ****************************************************************
 */
void
init_time_from_cmos (void)
{
	unsigned long	sec;
	int		year, leap, status, i;

	/*
	 *	Inicializa o relógio
	 */
	write_port (0x0A, 0x70);
	write_port (0x26, 0x71);
	write_port (0x0B, 0x70);
	write_port (0x02, 0x71);

	write_port (0x0E, 0x70);

	if (status = read_port (0x71))
		printf ("init_time_from_cmos: Relógio de tempo real com erro: %X\n", status);

	write_port (0x0E, 0x70);
	write_port (0x00, 0x71);

	/*
	 *	Verifica se o relógio está presente
	 */
	if ((status = read_cmos (RTC_STATUSA)) == 0xFF || status == 0)
	{
		printf ("init_time_from_cmos: Relógio NÃO presente\n");
		return;
	}

	/*
	 *	Espera para ficar pronta para leitura
	 */
	for (i = 100000000; /* abaixo */; i--)
	{
		if (i < 0)
		{
			printf ("init_time_from_cmos: TIMEOUT no Relógio\n");
			break;
		}

		if ((status & RTCSA_TUP) != RTCSA_TUP)
			break;

		status = read_cmos (RTC_STATUSA);
	}

	/*
	 *	Efetua a conversão de formatos
	 */
	year = bcd_to_int (read_cmos (RTC_YEAR)); leap = (year & 0x03) == 0;

	if (year < 70)
		year += 100;

	sec  = years_to_unix_seconds (year);
	sec += months_to_unix_seconds (bcd_to_int (read_cmos (RTC_MONTH)), leap);
	sec += (bcd_to_int (read_cmos (RTC_DAY)) - 1) * 24 * 60 * 60;
	sec += bcd_to_int (read_cmos (RTC_HRS)) * 60 * 60;			/* hour    */
	sec += bcd_to_int (read_cmos (RTC_MIN)) * 60;
	sec += bcd_to_int (read_cmos (RTC_SEC));				/* seconds */

	if (!scb.y_ut_clock)
		sec -= (scb.y_tzmin * 60);	/* Hora Local */

	time = sec;

}	/* end init_time_from_cmos */

/*
 ****************************************************************
 *	Converte um número BCD de dois dígitos			*
 ****************************************************************
 */
int
bcd_to_int  (unsigned int i)
{
	return ((i >> 4) * 10 + (i & 0x0F));

}	/* bcd_to_int */

/*
 ****************************************************************
 *	Converte anos para segundos (desde 1970) 		*
 ****************************************************************
 */
unsigned long
years_to_unix_seconds (unsigned int year)
{
	unsigned int	i;
	unsigned long	sec = 0;

	for (i = 70; i < year; i++)
	{
		if (i % 4)
			sec += 365 * 24 * 60 * 60;
		else
			sec += 366 * 24 * 60 * 60;
	}

	return (sec);

}	/* end years_to_unix_seconds */

/*
 ****************************************************************
 *	Converte meses em segundos				*
 ****************************************************************
 */
unsigned long
months_to_unix_seconds (unsigned int months, int leap)
{
	int		i;
	unsigned long	ret = 0;

	for (i = 1; i < months; i++)
	{
		switch (i)
		{
		    case 1:
		    case 3:
		    case 5:
		    case 7:
		    case 8:
		    case 10:
		    case 12:
			ret += 31 * 24 * 60 * 60;
			break;

		    case 4:
		    case 6:
		    case 9:
		    case 11:
			ret += 30 * 24 * 60 * 60;
			break;

		    case 2:
			if (leap)
				ret += 29 * 24 * 60 * 60;
			else
				ret += 28 * 24 * 60 * 60;
			break;

		}	/* end switch */

	}	/* end for */

	return (ret);

}	/* end months_to_unix_seconds */
