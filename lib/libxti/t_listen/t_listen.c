/*
 ****************************************************************
 *								*
 *			t_listen.c				*
 *								*
 *	Aguarda um pedido de conexão				*
 *								*
 *	Versão	2.3.0, de 16.08.91				*
 *		3.0.0, de 10.05.95				*
 *								*
 *	Módulo: t_listen					*
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
 *	Aguarda um pedido de conexão				*
 ****************************************************************
 */
int
t_listen (int fd, T_CALL *rcvcall)
{
#ifdef	PC
	return (itnet (fd, I_LISTEN, rcvcall));
#else
	return (ioctl (fd, I_LISTEN, rcvcall));
#endif	PC

}	/* end t_listen */
