/*
 ****************************************************************
 *								*
 *			t_alloc.c				*
 *								*
 *	Aloca memória para estruturas (NÃO IMPLEMENTADA)	*
 *								*
 *	Versão	2.3.0, de 12.11.91				*
 *		3.0.0, de 10.05.95				*
 *								*
 *	Módulo: t_alloc						*
 *		libxti 						*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 * 		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/common.h>
#include <sys/sync.h>
#include <sys/itnet.h>
#include <sys/syscall.h>

#include <errno.h>
#include <xti.h>

/*
 ****************************************************************
 *	Aloca memória para estruturas (NÃO IMPLEMENTADA)	*
 ****************************************************************
 */
void *
t_alloc (int fd, T_ALLOC_ENUM struct_type, int fields)
{
	errno = TNOTSUPPORT;

	return ((void *)NULL);

}	/* end t_alloc */
