/*
 ****************************************************************
 *								*
 *			t_rcvconnect.c				*
 *								*
 *	Recebe a confirmação de um pedido de conexão		*
 *								*
 *	Versão	2.3.0, de 04.11.91				*
 *		3.0.0, de 10.05.95				*
 *								*
 *	Módulo: t_rcvconnect					*
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
 *	Recebe a confirmação de um pedido de conexão		*
 ****************************************************************
 */
int
t_rcvconnect (int fd, T_CALL *call)
{
#ifdef	PC
	return (itnet (fd, I_RCVCONNECT, call));
#else
	return (ioctl (fd, I_RCVCONNECT, call));
#endif	PC

}	/* end t_rcvconnect */
