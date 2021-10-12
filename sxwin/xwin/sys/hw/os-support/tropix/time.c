/*
 ****************************************************************
 *								*
 *		hw/os-support/tropix/time.c			*
 *								*
 *	Suspende a execução por um tempo em micro-segundos	*
 *								*
 *	Versão	1.0.0, de 04.03.97				*
 *		4.2.0, de 28.09.01				*
 *								*
 *	Módulo: xc/lib/aux					*
 *		X Windows - Bibliotecas				*
 *		Baseado no software homônimo do XFree86		*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2001 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include "Xos.h"

/*
 ****************************************************************
 *	Suspende a execução do processo				*
 ****************************************************************
 */
void
xf86usleep (int delay)
{
	usleep (delay);

}	/* end xf86usleep */

/*
 ****************************************************************
 *	Obtém o tempo corrente					*
 ****************************************************************
 */
void
xf86getsecs(long * secs, long * usecs)
{
	struct timeval tv;

	gettimeofday(&tv, NULL);
	*secs = tv.tv_sec;
	*usecs= tv.tv_usec;

	return;
}
