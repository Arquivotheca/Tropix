/*
 ****************************************************************
 *								*
 *			fulltime.c				*
 *								*
 *	Retorna uma cadeia contendo o tempo atual em us		*
 *								*
 *	Versão	1.0.0, de 16.07.97				*
 *		1.0.1, de 27.03.98				*
 *								*
 *	Módulo: xc/lib/aux					*
 *		X Windows - Bibliotecas				*
 *		Baseado no software homônimo do XFree		*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright (c) 1998 TROPIX Technologies Ltd.	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/times.h>

#include <stdio.h>
#include <time.h>
#include <errno.h>

/*
 ****************************************************************
 *	Retorna uma cadeia contendo o tempo atual em us		*
 ****************************************************************
 */
char *
fulltime (void)
{
	static char	str[16];
	MUTM		mu;
	TM		*tp;

	mutime (&mu);
	tp = localtime (&mu.mu_time);

	sprintf
	(	str, "%02d:%02d:%02d.%06d",
		tp->tm_hour, tp->tm_min,
		tp->tm_sec, mu.mu_utime
	);

	return (str);

}	/* end fulltime */
