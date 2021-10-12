/*
 ****************************************************************
 *								*
 *			usleep.c				*
 *								*
 *	Suspende a execução por um tempo em micro-segundos	*
 *								*
 *	Versão	1.0.0, de 04.03.97				*
 *		4.0.0, de 06.07.00				*
 *								*
 *	Módulo: xc/lib/aux					*
 *		X Windows - Bibliotecas				*
 *		Baseado no software homônimo do XFree86		*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/common.h>
#include <sys/scb.h>
#include <sys/syscall.h>

#include <stdlib.h>


/*
 ******	Variáveis globais ***************************************
 */
entry long	DELAY_value;	/* Constante para 1 micro segundo */
extern void	DELAY (long);	/* Função para esperar */

/*
 ****************************************************************
 *	Suspende a execução do processo				*
 ****************************************************************
 */
void
usleep (int delay)
{
	if (DELAY_value == 0)
	{
		SCB	*sp;

		sp = alloca (sizeof (SCB));

		getscb (sp);

		DELAY_value = sp->y_DELAY_value;
	}

	DELAY (delay);

}	/* end usleep */
