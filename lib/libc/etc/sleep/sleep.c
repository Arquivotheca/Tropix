/*
 ****************************************************************
 *								*
 *			sleep.c					*
 *								*
 *	Suspende a execução de um processo durante um periodo	*
 *								*
 *	Versão	1.0.0, de 11.11.86				*
 *		3.0.0, de 26.06.95				*
 *								*
 *	Módulo: sleep						*
 *		libc/etc					*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/syscall.h>

#include <stdlib.h>
#include <time.h>
#include <signal.h>

/*
 ****************************************************************
 *	Variáveis e definições globais				*
 ****************************************************************
 */
#define	elif	else if

static	void	(*falarm) (int, ...);

/*
 ******	Protótipos de funções ***********************************
 */
static	void	wakened (int, ...);

/*
 ****************************************************************
 *	Rotina Auxiliar						*
 ****************************************************************
 */
static void
wakened (int signo, ...)
{
	signal (SIGALRM, falarm);

}	/* end wakened */

/*
 ****************************************************************
 *	Suspende a Execução					*
 ****************************************************************
 */
int
sleep (int seconds)
{
	register int	a, b;
	time_t		before, after;

	if (seconds == 0)
		return (0);

	time (&before);

	a = alarm (0); falarm = signal (SIGALRM, wakened);

	if   (a == 0)
	{
		b = 0;
		a = seconds;
	}
	elif (seconds < a)
	{
		b = a - seconds;
		a = seconds;
	}
	else  /* seconds >= a */
	{
		b = 1;
	}

	alarm (a); pause ();

	signal (SIGALRM, falarm); alarm (b);

	time (&after);

	if ((after = seconds - (after - before)) > 0)
		return (after);
	else
		return (0);

}	/* end sleep */
