/*
 ****************************************************************
 *								*
 *			safemalloc.c				*
 *								*
 *	"malloc" sofisticada					*
 *								*
 *	Versão	1.0.0, de 27.08.97				*
 *		4.4.0, de 20.03.03				*
 *								*
 *	Módulo: cmd/fvwm/libs					*
 *		X Windows - Clientes				*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright (c) 2003 TROPIX Technologies Ltd.	*
 * 								*
 ****************************************************************
 */

#include <stdlib.h>

char *
safemalloc (int length)
{
	void	*ptr;

	if (length <= 0)
		length = 1;

	if ((ptr = malloc (length)) == NOVOID)
		msg ("$Erro ao tentar alocar %d bytes", length);

	return (ptr);

}	/* end safemalloc */
