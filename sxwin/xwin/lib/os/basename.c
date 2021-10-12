/*
 ****************************************************************
 *								*
 *			basename.c				*
 *								*
 *	Obtém a última componente de um caminho			*
 *								*
 *	Versão	1.0.0, de 18.06.97				*
 *								*
 *	Módulo: xc/lib/aux					*
 *		X Windows - Bibliotecas				*
 *		Baseado no software homônimo do XFree86		*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright (c) 1997 TROPIX Technologies Ltd.	*
 * 								*
 ****************************************************************
 */

#include <string.h>

const char *
basename (const char *path)
{
	const char	*cp;

	return ((cp = (const char *)strrchr (path, '/')) ? cp + 1 : path);

}	/* end basename */
