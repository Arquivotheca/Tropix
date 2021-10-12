/*
 ****************************************************************
 *								*
 *			t_connect.c				*
 *								*
 *	Estabelece uma conexão 					*
 *								*
 *	Versão	2.3.0, de 16.08.91				*
 *		3.0.0, de 10.05.95				*
 *								*
 *	Módulo: t_connect					*
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
 *	Estabelece uma conexão 					*
 ****************************************************************
 */
int
t_connect (int fd, const T_CALL *sndcall, T_CALL *rcvcall)
{
#ifdef	PC
	return (itnet (fd, I_CONNECT, sndcall, rcvcall));
#else
	return (ioctl (fd, I_CONNECT, &sndcall));
#endif	PC

}	/* end t_connect */
