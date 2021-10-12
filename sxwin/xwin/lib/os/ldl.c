/*
 ****************************************************************
 *								*
 *			aux/ldl.c				*
 *								*
 *	Simulação de "ldl_u"					*
 *								*
 *	Versão	1.0.0, de 27.07.97				*
 *		4.2.0, de 26.09.01				*
 *								*
 *	Módulo: programs/Xserver				*
 *		X Windows - Servidor				*
 *		Baseado no software homônimo do XFree86		*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright (c) 2001 TROPIX Technologies Ltd.	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>

ulong
ldl_u (unsigned int *p)
{
	ulong	ret;

	memmove (&ret, p, sizeof (*p));

	return (ret);
}
