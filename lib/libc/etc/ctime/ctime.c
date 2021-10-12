/*
 ****************************************************************
 *								*
 *			ctime.c 				*
 *								*
 *	Funções de manuseio de tempos do sistema		*
 *								*
 *	Versão	1.0.0, de 29.08.86				*
 *		3.1.0, de 27.10.98				*
 *								*
 *	Módulo: ctime						*
 *		libc/etc					*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <stdio.h>
#include <time.h>

/*
 ****************************************************************
 *	Variaveis e funções globais				*
 ****************************************************************
 */
static const char	diadasem[][4] =
{
	"DOM", "SEG", "TER", "QUA", "QUI", "SEX", "SAB"
};

static const char	weekday[][4] =
{
	"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};

static const char	nomedomes[][4] =
{
	"jan", "fev", "mar", "abr", "mai", "jun",
 	"jul", "ago", "set", "out", "nov", "dez"
};

static const char	monthname[][4] =
{
	"Jan", "Feb", "Mar", "Apr", "May", "Jun",
 	"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

static char		datahora[28];

/*
 ****************************************************************
 *	Função que dá o tempo em ingles				*
 ****************************************************************
 */
char *
ctime (const time_t *timer)
{
	return (asctime (localtime (timer)));

}	/* end ctime */

/*
 ****************************************************************
 *	Função que dá o tempo em portugues			*
 ****************************************************************
 */
char *
btime (const time_t *timer)
{
	return (brasctime (localtime (timer)));

}	/* end btime */

/*
 ****************************************************************
 *	Função que dá o tempo em ingles				*
 ****************************************************************
 */
char *
asctime (const TM *tp)
{
	sprintf
	(	datahora,
		"%s %s %2d %2d:%02d:%02d %4d\n",
		weekday[tp->tm_wday],
		monthname[tp->tm_mon],
		tp->tm_mday,
		tp->tm_hour,
		tp->tm_min,
		tp->tm_sec,
		tp->tm_year + 1900
	);

	return (datahora);

}	/* end asctime */

/*
 ****************************************************************
 *	Função que dá o tempo em portugues			*
 ****************************************************************
 */
char *
brasctime (const TM *tp)
{
	sprintf
	(	datahora,
		"%2d:%02d:%02d %s %2d %s %4d\n",
		tp->tm_hour,
		tp->tm_min,
		tp->tm_sec,
		diadasem[tp->tm_wday],
		tp->tm_mday,
		nomedomes[tp->tm_mon],
		tp->tm_year + 1900
	);

	return (datahora);

}	/* end brasctime */
