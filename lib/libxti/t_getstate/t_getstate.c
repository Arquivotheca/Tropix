/*
 ****************************************************************
 *								*
 *			t_getstate.c				*
 *								*
 *	Obtém o estado corrente					*
 *								*
 *	Versão	2.3.0, de 04.11.91				*
 *		3.0.0, de 10.05.95				*
 *								*
 *	Módulo: t_getstate					*
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
 *	Obtém o estado corrente					*
 ****************************************************************
 */
int
t_getstate (int fd)
{
#ifdef	PC
	return (itnet (fd, I_GETSTATE));
#else
	return (ioctl (fd, I_GETSTATE));
#endif	PC

}	/* end t_getstate */
