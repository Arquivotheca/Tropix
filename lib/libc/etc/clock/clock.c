/*
 ****************************************************************
 *								*
 *			clock.c					*
 *								*
 *	Medição de Tempo 					*
 *								*
 *	Versão	1.0.0, de 04.11.86				*
 *		3.2.3, de 05.01.00				*
 *								*
 *	Módulo:	clock	 					*
 *		libc/etc					*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/times.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*
 ****************************************************************
 *	Definições globais					*
 ****************************************************************
 */
#define	MIL	1000
#define	MILHAO	1000000

#define	NOMUTM	(MUTM *)NULL
#define	NOMUTMS	(MUTMS *)NULL

static	MUTM	*bmutm;		/* Estampa de Tempo Real */ 
static	MUTMS	*bmutms;	/* Estampa de Tempo de CPU */ 

/*
 ******	Protótipos de funções ***********************************
 */
static void	printmu (char *, MUTM *, MUTM *);

/*
 ****************************************************************
 *	Obtém o tempo desde o início da execução		*
 ****************************************************************
 */
clock_t
clock (void)
{
	MUTMS		m;

	/*
	 *	Código simplificado, supondo que CLK_TCK == MILHAO
	 */
#	if CLK_TCK != MILHAO
#		error 	ATENÇÃO: CKL_TCK != MILHAO !!!
#	endif

	mutimes (&m);

	return
	(	(m.mu_utime + m.mu_stime + m.mu_cutime + m.mu_cstime) * MILHAO +
		m.mu_uutime + m.mu_sutime + m.mu_cuutime + m.mu_csutime
	);

}	/* end clock */

/*
 ****************************************************************
 *	Marca o Tempo Inicial					*
 ****************************************************************
 */
int
clockstamp (void)
{
	if (bmutms == NOMUTMS)
	{
		if ((bmutm  =  malloc (sizeof (MUTM))) == NOMUTM)
			return (-1);

		if ((bmutms =  malloc (sizeof (MUTMS))) == NOMUTMS)
			return (-1);
	}

	mutime  (bmutm);
	mutimes (bmutms);

	return (0);

}	/* end clockstamp */

/*
 ****************************************************************
 *	Obtem o Tempo de CPU					*
 ****************************************************************
 */
char *
clocktostr (char *line)
{
	MUTM		amutm;
	MUTMS		amutms;

	mutime  (&amutm);
	mutimes (&amutms);

	sprintf (line,    "%s", "REAL ");
	printmu (line+5,  bmutm, &amutm);
	sprintf (line+24, "%s", "USER ");
	printmu (line+29, (MUTM *)&bmutms->mu_utime, (MUTM *)&amutms.mu_utime);
	sprintf (line+48, "%s", "SYS  ");
	printmu (line+53, (MUTM *)&bmutms->mu_stime, (MUTM *)&amutms.mu_stime);
	line[65] = '\0';

	return (line);

}	/* end clocktostr */

/*
 ****************************************************************
 *	Calcula a diferença de tempos				*
 ****************************************************************
 */
double
difftime (time_t time1, time_t time0) 
{
	return ((double)(time1 - time0));

}	/* end difftime */

/*
 ****************************************************************
 *	Converte e imprime o tempo				*
 ****************************************************************
 */
static void
printmu (char *buf, register MUTM *ap, register MUTM *dp)
{
	register int	hor, min, seg, ms, us; 

	if (dp->mu_utime < ap->mu_utime)
	{
		dp->mu_time--;
		dp->mu_utime += MILHAO;
	}

	seg = dp->mu_time  - ap->mu_time;
	us  = dp->mu_utime - ap->mu_utime;

	min = seg / 60;
	seg = seg % 60;

	hor = min / 60;
	min = min % 60;

	ms  = us / MIL;
	us  = us % MIL;

	sprintf (buf, "%2d:%02d:%02d.%03d       ", hor, min, seg, ms);

}	/* end printmu */
