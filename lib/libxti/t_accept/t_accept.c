/*
 ****************************************************************
 *								*
 *			t_accept.c				*
 *								*
 *	Aceita um pedido de conexão				*
 *								*
 *	Versão	2.3.0, de 16.08.91				*
 *		3.0.0, de 10.05.95				*
 *								*
 *	Módulo: t_accept					*
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
 *	Aceita um pedido de conexão				*
 ****************************************************************
 */
int
t_accept (int listen_fd, int conn_fd, const T_CALL *sndcall)
{
#ifdef	PC
	return (itnet (listen_fd, I_ACCEPT, conn_fd, sndcall));
#else
	return (ioctl (listen_fd, I_ACCEPT, &conn_fd));
#endif	PC

}	/* end t_accept */
