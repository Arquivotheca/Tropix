/*
 ****************************************************************
 *								*
 *			t_rcvuderr.c				*
 *								*
 *	Recebe uma indicação de erro UDP			*
 *								*
 *	Versão	2.3.0, de 04.11.91				*
 *		3.0.0, de 10.05.95				*
 *								*
 *	Módulo: t_rcvuderr					*
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
 *	Recebe uma indicação de erro UDP			*
 ****************************************************************
 */
int
t_rcvuderr (int fd, T_UDERROR *uderr)
{
#ifdef	PC
	return (itnet (fd, I_RCVUDERR, uderr));
#else
	return (ioctl (fd, I_RCVUDERR, uderr));
#endif	PC

}	/* end t_rcvuderr */
