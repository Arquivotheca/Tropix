/*
 ****************************************************************
 *								*
 *			t_rcvdis.c				*
 *								*
 *	Recebe o começo da desconexão abortiva TCP		*
 *								*
 *	Versão	2.3.0, de 30.09.91				*
 *		3.0.0, de 10.05.95				*
 *								*
 *	Módulo: t_rcvdis					*
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
 *	Recebe o começo da desconexão abortiva TCP		*
 ****************************************************************
 */
int
t_rcvdis (int fd, T_DISCON *discon)
{
#ifdef	PC
	return (itnet (fd, I_RCVDIS, discon));
#else
	return (ioctl (fd, I_RCVDIS, discon));
#endif	PC

}	/* end t_rcvdis */
