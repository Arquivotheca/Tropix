/*
 ****************************************************************
 *								*
 *			t_unbind.c				*
 *								*
 *	Libera um "endpoint" de  um "port"			*
 *								*
 *	Versão	2.3.0, de 08.08.91				*
 *		3.0.0, de 10.05.95				*
 *								*
 *	Módulo: t_unbind					*
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

#include <xti.h>

/*
 ****************************************************************
 *	Libera um "endpoint" de  um "port"			*
 ****************************************************************
 */
int
t_unbind (int fd)
{
#ifdef	PC
	return (itnet (fd, I_UNBIND));
#else
	return (ioctl (fd, I_UNBIND));
#endif	PC

}	/* end t_unbind */
